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

void registerCoreTypes()
{
    ObjectFactory::registerType(Signature::AnyTypeName,
        std::function<Object(Object const&)>([](Object const& o) { return o; }));
}

void registerBuiltinTypes()
{
    ObjectFactory::registerType<bool>("bool",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        (std_and,       [](bool a, bool b) { return a && b; })
        (std_or,        [](bool a, bool b) { return a || b; })
        (std_not,       [](bool a) { return !a; })
        (std_equals,    [](bool a, bool b) { return a == b; })
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
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
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

    ObjectFactory::registerType<float>("float",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        (std_add,       [](float a, float b) { return a + b; })
        (std_sub,       [](float a, float b) { return a - b; })
        (std_mul,       [](float a, float b) { return a * b; })
        (std_div,       [](float a, float b) { return a / b; })
        (std_mod,       [](float a, float b) { return std::fmod(a, b); })
        (std_equals,    [](float a, float b) { return a == b; })
        (std_serialize, [](float a)
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
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
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
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        (std_equals,      [](std::string const& a, std::string const& b) { return a == b; })
        (std_serialize,   [](Object const& o) { return o; })
        (std_unserialize, [](Object const& o) { return o; })
        ("append",        [](std::string& s, char c) { s += c; })
        ("size",          [](std::string const& s) { return (int) s.size(); })
        ("at",            [](std::string const& s, int i) { return s[i]; }));
}

void registerOtherTypes()
{
    ObjectFactory::registerType<vm::StackFrame>("StackFrame",
        ObjectFactory::constructorList(),
        ObjectFactory::methodList()
        ("pc",           [](StackFrame const& sf) { return sf.pc; })
        ("locals_count", [](StackFrame const& sf) { return sf.locals_count; })
        ("locals_start", [](StackFrame const& sf) { return sf.locals_start; })
        ("argc",         [](StackFrame const& sf) { return sf.argc; }));

    ObjectFactory::registerType<Token>("Token",
        ObjectFactory::constructorList()
        ([](int which, Object const& what) { return Token(which, what); }),
        ObjectFactory::methodList()
        (std_equals, [](Token const& tok, Token const& other) { return tok.which() == other.which(); })
        (std_equals, [](Token const& tok, int which) { return tok.which() == which; })
        ("which",    [](Token const& tok) { return tok.which(); })
        ("what",     [](Token const& tok) { return tok.what(); }));
}

void registerAllTypes()
{
    registerCoreTypes();
    registerBuiltinTypes();
    registerOtherTypes();
}

int main()
{
    registerAllTypes();

    std::string s =
    "if_axXx_1,x|xifaif,import,45,89.85,yolo";

    std::istringstream ss;
    ss.str(s);
    
    Lexer lex(ss, 32);

    enum
    {
        TOK_LPAREN,
        TOK_RPAREN,
        TOK_LBRACE,
        TOK_RBRACE,
        TOK_STAR,
        TOK_COLON,
        TOK_SEMICOLON,
        TOK_COMMA,
        TOK_DOT,
        TOK_KW_FUN,
        TOK_KW_IMPORT,
        TOK_KW_IF,
        TOK_KW_ELIF,
        TOK_KW_ELSE,
        TOK_KW_WHILE,
        TOK_KW_DO,
        TOK_KW_BREAK,
        TOK_KW_CONTINUE,
        TOK_KW_RETURN,
        TOK_IDENTIFIER,
        TOK_NUMBER
    };

    lex.addDefinition("alpha",       "'[a-zA-Z]'");
    lex.addDefinition("digit",       "'[0-9]'");
    lex.addDefinition("hex_digit",   "'[0-9a-fA-F]'");
    lex.addDefinition("alpha_",      "alpha | '_'");
    lex.addDefinition("alnum",       "alpha | digit");
    lex.addDefinition("alnum_",      "alpha_ | digit");

    lex.addDefinition("lparen",      "'('",          Token(TOK_LPAREN));
    lex.addDefinition("rparen",      "')'",          Token(TOK_RPAREN));
    lex.addDefinition("lbrace",      "'{'",          Token(TOK_LBRACE));
    lex.addDefinition("rbrace",      "'}'",          Token(TOK_RBRACE));
    lex.addDefinition("star",        "'*'",          Token(TOK_STAR));
    lex.addDefinition("colon",       "':'",          Token(TOK_COLON));
    lex.addDefinition("semicolon",   "';'",          Token(TOK_SEMICOLON));
    lex.addDefinition("comma",       "','",          Token(TOK_COMMA));
    lex.addDefinition("dot",         "'.'",          Token(TOK_DOT));

    lex.addDefinition("kw_fun",      "\"fun\"",      Token(TOK_KW_FUN));
    lex.addDefinition("kw_import",   "\"import\"",   Token(TOK_KW_IMPORT));
    lex.addDefinition("kw_if",       "\"if\"",       Token(TOK_KW_IF));
    lex.addDefinition("kw_elif",     "\"elif\"",     Token(TOK_KW_ELIF));
    lex.addDefinition("kw_else",     "\"else\"",     Token(TOK_KW_ELSE));
    lex.addDefinition("kw_while",    "\"while\"",    Token(TOK_KW_WHILE));
    lex.addDefinition("kw_do",       "\"do\"",       Token(TOK_KW_DO));
    lex.addDefinition("kw_break",    "\"break\"",    Token(TOK_KW_BREAK));
    lex.addDefinition("kw_continue", "\"continue\"", Token(TOK_KW_CONTINUE));
    lex.addDefinition("kw_return",   "\"return\"",   Token(TOK_KW_RETURN));

    lex.addDefinition("identifier",  "alpha_ alnum_*",
    [](std::string const& lexeme)
    {
        return Token(TOK_IDENTIFIER, lexeme);
    });

    lex.addDefinition("number",  "digit+ ('.' digit+)?",
    [](std::string const& lexeme)
    {
        return Token(TOK_NUMBER, lexeme);
    });

    lex.build();

    Token tok(Token::Eof);
    do
    {
        tok = lex.M_getToken();
        if (tok.which() == Token::Invalid)
            std::cout << "<invalid>" << std::endl;
    } while (tok.which() != Token::Eof);

    return 0;
}

int main2()
{
    registerAllTypes();

    Blob blob = makeBlob();

    Disassembler dis(blob, std::cout);
    dis.dumpAll();

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
