#include <iostream>
#include <iomanip>
#include <type_traits>
#include <cstring>
#include <string>
#include <map>
#include <iomanip>
#include <stack>
#include <sstream>

#include "core/core.hpp"
#include "bits/bits.hpp"
#include "vm/vm.hpp"
#include "lang/lang.hpp"

using namespace core;
using namespace bits;
using namespace vm;
using namespace lang;
using namespace nfa;

void dumpBlobStrings(Blob const& blob, std::ostream& os)
{
    os << "Strings :" << std::endl;
    blob.foreachString([&](blob_idx sidx, std::string const& str)
    {
        os << "  [" << std::setw(4) << std::hex << sidx << std::dec << "] ";
        os << str << std::endl;
    });
}

void dumpBlobSignature(Blob const& blob, blob_idx sigidx, std::ostream& os)
{
    std::vector<std::string> args;
    blob.foreachSignatureArgument(sigidx, [&](blob_off soff)
    { args.push_back(blob.string(soff)); });

    os << '(';
    for (int i = 0; i < (int) args.size(); ++i)
        os << args[i] << (i == (int) args.size() - 1 ? "" : ", ");
    os << ')';
}

void dumpBlobSymbols(Blob const& blob, std::ostream& os)
{
    os << "Symbols :" << std::endl;
    blob.foreachSymbol([&](blob_idx symidx, blob_symbol* sym)
    {
        os << "  [" << std::setw(4) << symidx << "] ";

        if (sym->s_bind == BLOB_SYMB_NULL)
            os << "NULL   ";
        else if (sym->s_bind == BLOB_SYMB_LOCAL)
            os << "LOCAL  ";
        else if (sym->s_bind == BLOB_SYMB_GLOBAL)
            os << "GLOBAL ";

        if (sym->s_type == BLOB_SYMT_NULL)
            os << "NULL     ";
        else if (sym->s_type == BLOB_SYMT_FUNCTION)
            os << "FUNCTION ";
        else if (sym->s_type == BLOB_SYMT_METHOD)
            os << "METHOD   ";

        os << std::setw(8) << std::setfill('0') << std::hex
           << sym->s_addr << std::setfill(' ') << std::dec << " ";

        os << "locals:" << std::setw(2) << sym->s_nlocals << " ";

        os << blob.string(sym->s_name);
        dumpBlobSignature(blob, sym->s_signature, os);

        os << std::endl;
    });
}

void dumpBlobTypeSpecs(Blob const& blob, std::ostream& os)
{
    os << "Type specifications :" << std::endl;
    blob.foreachTypeSpec([&](blob_idx tsidx, blob_typespec* tspec)
    {
        os << "  [" << std::setw(4) << tsidx << "] " << blob.string(tspec->ts_name) << std::endl;
        blob.foreachTypeSpecSymbol(tsidx, [&](blob_idx symidx)
        {
            blob_symbol* sym = blob.symbol(symidx);
            os << "         [" << std::setw(4) << symidx << "] " << blob.string(sym->s_name);
            dumpBlobSignature(blob, sym->s_signature, os);
            os << std::endl;
        });
    });
}

void dumpBlobConstants(Blob const& blob, std::ostream& os)
{
    os << "Constants :" << std::endl;
    blob.foreachConstant([&](blob_idx cstidx, blob_constant* cst)
    {
        os << "  [" << std::setw(4) << cstidx << "] ";
        os << std::setw(8) << std::left << blob.string(cst->c_type) << std::right << " ";
        os << '\'' << blob.string(cst->c_serialized) << '\'' << std::endl;
    });
}

void dumpBlobText(Blob const& blob, std::ostream& os)
{
    os << "Text section :" << std::endl;

    std::shared_ptr<Buffer> text = blob.text();
    if (!text)
        return;
    int count = (int) (text->size() / sizeof(uint32_t));

    auto decodeInstruction = [&](int& pc, std::vector<int>& operands)
    {
        static std::map<Opcode, int> opcodes_nargs;
        static bool opcodes_nargs_inited = false;

        if (!opcodes_nargs_inited)
        {   
            #define OPCODE(name, nargs) opcodes_nargs[name] = nargs;
            #include "bits/opcodes.inc"
            #undef OPCODE

            opcodes_nargs_inited = true;
        }

        auto fetch = [&]()
        { return *((uint32_t*) text->raw(sizeof(uint32_t) * pc++, sizeof(uint32_t))); };

        Opcode opcode = (Opcode) fetch();
        for (int i = 0; i < opcodes_nargs[opcode]; ++i)
            operands.push_back((int) fetch());

        return opcode;
    };

    // Find all jump targets and allocate label names to them
    int jmp_targets_count = 0;
    std::map<int, std::pair<std::string, std::vector<int>>> jmp_targets;
    for (int pc = 0; pc < count; )
    {
        std::vector<int> operands;
        Opcode opcode = decodeInstruction(pc, operands);

        bool is_jmp = false;
        int target = 0;
        switch (opcode)
        {
            case JMPR:
            case JMPR_IF_FALSE:
            case JMPR_IF_TRUE:
            {
                is_jmp = true;
                target = pc + operands[0];
                break;
            }

            case JMP:
            case JMP_IF_FALSE:
            case JMP_IF_TRUE:
            {
                is_jmp = true;
                target = operands[0];
                break;
            }

            default:
                break;
        }

        if (is_jmp)
        {
            if (jmp_targets.find(target) == jmp_targets.end())
            {
                std::ostringstream ss;
                ss << "t" << std::setw(3) << std::setfill('0') << jmp_targets_count++;
                jmp_targets[target].first = ss.str();
            }
            jmp_targets[target].second.push_back(pc);
        }
    }

    // Display the actual disassembly
    for (int pc = 0; pc < count; )
    {
        // Find if this instruction begins a symbol
        blob_symbol* symbol = nullptr;
        blob.foreachSymbol([&](blob_idx, blob_symbol* sym)
        {
            if ((int) sym->s_addr == pc)
                symbol = sym;
        });

        // If yes, display the symbol prototype
        if (symbol)
        {
            os << std::endl << blob.string(symbol->s_name);
            dumpBlobSignature(blob, symbol->s_signature, os);
            os << ':' << std::endl;
        }

        // If this instruction is a jump target, display its label
        bool is_jmp_target = jmp_targets.find(pc) != jmp_targets.end();
        if (is_jmp_target)
        {
            os << jmp_targets[pc].first << " > ";
        }
        else
            os << "       ";

        // Decode the isntruction
        std::vector<int> operands;
        Opcode opcode = decodeInstruction(pc, operands);

        // Display the mnemonic
        os << std::setw(20) << std::left << opcode_as_string(opcode) << std::right;

        switch (opcode)
        {
            case LOAD_CONST:
            {
                if (operands[0] < 0)
                {
                    os << "a" << (-1-operands[0]);
                }
                else
                {
                    blob_constant* cst = blob.constant(operands[0]);
                    if (!cst)
                        os << "<invalid>";
                    else
                        os << "(" << blob.string(cst->c_type) << ") '" << blob.string(cst->c_serialized) << "'";
                }
                break;
            }

            case LOAD_LOCAL:
            case STOR_LOCAL:
            {
                os << "l" << operands[0];
                break;
            }

            case LOAD_GLOBAL:
            case STOR_GLOBAL:
            {
                std::string name;
                if (!blob.string(operands[0], name))
                    name = "<invalid>";
                os << name;
                break;
            }

            case INVOKE:
                os << operands[0];
                break;

            case METHOD:
            {
                std::string name;
                if (!blob.string(operands[0], name))
                    name = "<invalid>";
                os << name << ", " << operands[1];
                break;
            }

            case JMPR:
            case JMPR_IF_FALSE:
            case JMPR_IF_TRUE:
                os << jmp_targets[pc + operands[0]].first;
                break;

            case JMP:
            case JMP_IF_FALSE:
            case JMP_IF_TRUE:
                os << jmp_targets[operands[0]].first;
                break;

            default:
                break;
        }

        os << std::endl;
    }
}

void dumpBlob(Blob const& blob, std::ostream& os)
{
    dumpBlobStrings(blob, os);
    os << std::endl;
    dumpBlobSymbols(blob, os);
    os << std::endl;
    dumpBlobTypeSpecs(blob, os);
    os << std::endl;
    dumpBlobConstants(blob, os);
    os << std::endl;
    dumpBlobText(blob, os);
}

Blob makeBlob()
{
    Blob blob;

    // Strings
    blob_idx s_debug, s_main, s_factorial, s___sub__, s___mul__, s___equals__;
    blob.addString("debug", s_debug);
    blob.addString("main", s_main);
    blob.addString("factorial", s_factorial);
    blob.addString(std_sub, s___sub__);
    blob.addString(std_mul, s___mul__);
    blob.addString(std_equals, s___equals__);

    // Bytecode
    int32_t bytecode[] =
    {
        // main:
        LOAD_CONST, 1, // 5
        LOAD_GLOBAL, (int32_t) s_factorial,
        INVOKE, 1,
        LOAD_GLOBAL, (int32_t) s_debug,
        INVOKE, 1,
        LEAVE,

        // factorial(n):
        LOAD_CONST, 0,  // 1
        LOAD_CONST, -1, // n
        METHOD, (int32_t) s___equals__, 1,
        JMPR_IF_FALSE, 3,
        LOAD_CONST, 0, // 1
        RETURN,
        LOAD_CONST, 0, // 1
        LOAD_CONST, -1, // n
        METHOD, (int32_t) s___sub__, 1,
        LOAD_GLOBAL, (int32_t) s_factorial,
        INVOKE, 1,
        LOAD_CONST, -1,
        METHOD, (int32_t) s___mul__, 1,
        RETURN,

        LOAD_CONST, -1,
        STOR_LOCAL, 0,
        LOAD_CONST, 0,
        STOR_LOCAL, 1,
        LOAD_CONST, 0,
        LOAD_LOCAL, 0,
        METHOD, (int32_t) s___equals__, 1,
        JMPR_IF_TRUE, 20,
        LOAD_LOCAL, 0,
        LOAD_LOCAL, 1,
        METHOD, (int32_t) s___mul__, 1,
        STOR_LOCAL, 1,
        LOAD_CONST, 0,
        LOAD_LOCAL, 0,
        METHOD, (int32_t) s___sub__, 1,
        STOR_LOCAL, 0,
        JMPR, -29,
        LOAD_LOCAL, 1,
        RETURN
    };

    Buffer* text = new BasicBuffer((uint8_t*) bytecode, sizeof(bytecode));
    blob.setText(text);
    delete text;

    // Symbols
    blob_symbol* sym;
    blob_off sigidx;

    blob.addSignature(sigidx);
    sym = blob.addSymbol("main");
    sym->s_addr = 0;
    sym->s_bind = BLOB_SYMB_GLOBAL;
    sym->s_type = BLOB_SYMT_FUNCTION;
    sym->s_signature = sigidx;

    blob.addSignature(sigidx);
    blob.addSignatureArgument(sigidx, "int");
    sym = blob.addSymbol("factorial");
    sym->s_addr = 11;
    sym->s_bind = BLOB_SYMB_GLOBAL;
    sym->s_type = BLOB_SYMT_FUNCTION;
    sym->s_nlocals = 0;
    sym->s_signature = sigidx;

    blob.addSignature(sigidx);
    blob.addSignatureArgument(sigidx, "int");
    sym = blob.addSymbol("factorial2");
    sym->s_addr = 40;
    sym->s_bind = BLOB_SYMB_GLOBAL;
    sym->s_type = BLOB_SYMT_FUNCTION;
    sym->s_nlocals = 2;
    sym->s_signature = sigidx;

    blob.addConstant("int", "1");
    blob.addConstant("int", "7");

    return blob;
}

int main()
{
    std::string s =
    "if_axxx_1xxxx|if|aif";

    std::istringstream ss;
    ss.str(s);
    
    Lexer lex(ss, 8);

    lex.addDefinition("letter",
        "'a'|'b'|'c'|'d'|'e'|'f'|'g'|'h'|'i'|'j'|'k'|'l'|'m'|'n'|'o'|'p'|'q'|'r'|'s'|'t'|'u'|'v'|'w'|'x'|'y'|'z'|"
        "'A'|'B'|'C'|'D'|'E'|'F'|'G'|'H'|'I'|'J'|'K'|'L'|'M'|'N'|'O'|'P'|'Q'|'R'|'S'|'T'|'U'|'V'|'W'|'X'|'Y'|'Z'");
    lex.addDefinition("digit", "'0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'");

    lex.addDefinition("kw_if", "\"if\"", true);
    lex.addDefinition("ident", "('_' | letter) ('_' | letter | digit)*", true);
    lex.addDefinition("pipe", "'|'", true);
    lex.build();

    lex.get();
    lex.get();
    lex.get();
    lex.get();
    lex.get();
}

int main2()
{
    ObjectFactory::registerType(Signature::AnyTypeName,
        std::function<Object(Object const&)>([](Object const& o) { return o; }));

    ObjectFactory::registerType<vm::StackFrame>("@StackFrame",
        ObjectFactory::memberList());

    ObjectFactory::registerType<bool>("bool",
        ObjectFactory::memberList()
        (std_equals,    [](bool a, bool b) { return a == b; })
        (std_and,       [](bool a, bool b) { return a && b; })
        (std_or,        [](bool a, bool b) { return a || b; })
        (std_not,       [](bool a) { return !a; })
        (std_serialize, [](bool a)
        {
            std::ostringstream ss;
            ss << std::boolalpha;
            ss << a;
            return ss.str();
        })
        (std_unserialize, [](std::string const& s)
        {
            std::istringstream ss;
            ss.str(s);
            bool a;
            ss >> std::boolalpha >> a;
            return a;
        }));

    ObjectFactory::registerType<int>("int",
        ObjectFactory::memberList()
        (std_add,       [](int a, int b) { return a + b; })
        (std_sub,       [](int a, int b) { return a - b; })
        (std_mul,       [](int a, int b) { return a * b; })
        (std_div,       [](int a, int b) { return a / b; })
        (std_mod,       [](int a, int b) { return a % b; })
        (std_equals,    [](int a, int b) { return a == b; })
        (std_serialize, [](int a)
        {
            std::ostringstream ss;
            ss << a;
            return ss.str();
        })
        (std_unserialize, [](std::string const& s)
        {
            std::istringstream ss;
            ss.str(s);
            int a;
            ss >> a;
            return a;
        }));

    ObjectFactory::registerType<char>("char",
        ObjectFactory::memberList()
        (std_equals,    [](char a, char b) { return a == b; })
        (std_serialize, [](char a)
        {
            std::ostringstream ss;
            ss << a;
            return ss.str();
        })
        (std_unserialize, [](std::string const& s)
        {
            std::istringstream ss;
            ss.str(s);
            char a;
            ss >> a;
            return a;
        }));

    ObjectFactory::registerType<std::string>("string",
        ObjectFactory::memberList()
        (std_equals,      [](std::string const& a, std::string const& b) { return a == b; })
        (std_serialize,   [](Object const& o) { return o; })
        (std_unserialize, [](Object const& o) { return o; })
        // (std_del, [](std::string const& s) { std::cout << "__del__(" << &s << " '" << s << "')" << std::endl; })
        ("append", [](std::string& s, char c) { s += c; })
        ("size", [](std::string const& s) { return (int) s.size(); })
        ("at", [](std::string const& s, int i) { return s[i]; }));

    Blob blob = makeBlob();

    // dumpBlob(blob, std::cout);

    Module module(blob);
    module.global("debug").newPolymorphic(std_call) = [](int a) {std::cout << "(int) " << a << std::endl;};
    module.global("debug").newPolymorphic(std_call) = [](std::string const& s) {std::cout << "(string) " << s << std::endl;};
    module.global("debug").newPolymorphic(std_call) = [](Object const& other) {std::cout << "(other) " << &other << std::endl;};

    Engine engine(&module);

    std::cout << module.global("factorial2")(7).unwrap<int>() << std::endl;
    std::cout << module.global("factorial")(8).unwrap<int>() << std::endl;
    module.global("main")();

    return 0;
}
