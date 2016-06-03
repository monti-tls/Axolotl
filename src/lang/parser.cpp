#include "lang/parser.hpp"
#include "lang/std_names.hpp"
#include "lang/symtab.hpp"
#include "lib/dict.hpp"
#include "vm/import_table.hpp"

#include "lang/ast/ast.hpp"
#include "util/ansi.hpp"
#include "core/core.hpp"

#include <algorithm>
#include <sstream>

using namespace lang;
using namespace ast;
using namespace lib;

Parser::Parser(std::istream& in, vm::Module const& module)
    : ParserBase(in, 1)
    , m_module(module)
{ M_build(); }

Parser::~Parser()
{}

Node* Parser::parse()
{ return M_prog(); }

Node* Parser::M_name()
{
    Token start_token;
    std::ostringstream ss;

    // Eventual package name prefix
    if (M_peek().which() == TOK_PACKAGE)
    {
        Token package = M_eat(TOK_PACKAGE);
        M_eat(TOK_DOT);

        ss << package.what().unwrap<std::string>() << ".";
        start_token = package;
    }

    // Identifier
    Token name = M_eat(TOK_IDENTIFIER);
    if (start_token.which() < 0)
        start_token = name;

    ss << name.what().unwrap<std::string>();

    NameNode* node = new NameNode(start_token);
    node->value = ss.str();

    return node;
}

Node* Parser::M_expr_0()
{    
    switch (M_peek().which())
    {
        // Expression group
        case TOK_LPAR:
        {
            M_eat(TOK_LPAR);
            Node* node = M_check(M_expr());
            M_eat(TOK_RPAR);

            return node;
        }

        // Name
        case TOK_PACKAGE:
        case TOK_IDENTIFIER:
        {
            return M_check(M_name());
        }

        // Literals
        case TOK_LIT_CHAR:
        case TOK_LIT_STRING:
        case TOK_LIT_INTEGER:
        case TOK_LIT_FLOATING:
        {
            Token start_token = M_get();

            ConstNode* node = new ConstNode(start_token);
            node->value = start_token.what();
            return node;
        }

        default:
            M_unexpected(M_peek());
            break;
    }

    return nullptr;
}

Node* Parser::M_expr_1()
{
    Node* node = M_check(M_expr_0());

    while (M_peek().which() == TOK_LPAR ||
           M_peek().which() == TOK_DOT)
    {
        switch (M_peek().which())
        {
            // Object invocation
            case TOK_LPAR:
            {
                Token start_token;
                Node* args = nullptr;

                start_token = M_eat(TOK_LPAR);
                if (M_peek().which() != TOK_RPAR)
                    args = M_check(M_expr_list());
                M_eat(TOK_RPAR);

                InvokeNode* new_node = new InvokeNode(start_token);
                new_node->addSibling(node);
                new_node->addSibling(args);

                node = new_node;
                break;
            }

            // Member access and method call
            case TOK_DOT:
            {
                Token start_token;
                Token name;

                start_token = M_eat(TOK_DOT);
                name = M_eat(TOK_IDENTIFIER);

                // Method call
                if (M_peek().which() == TOK_LPAR)
                {
                    Node* args = nullptr;

                    M_eat(TOK_LPAR);
                    if (M_peek().which() != TOK_RPAR)
                        args = M_check(M_expr_list());
                    M_eat(TOK_RPAR);

                    MethodNode* new_node = new MethodNode(start_token);
                    new_node->addSibling(node);
                    new_node->addSibling(args);
                    new_node->name = name.what().unwrap<std::string>();

                    node = new_node;
                }
                // Simple member access
                else
                {
                    MemberNode* new_node = new MemberNode(start_token);
                    new_node->addSibling(node);
                    new_node->name = name.what().unwrap<std::string>();

                    node = new_node;
                }
                break;
            }
        }
    }

    return node;
}

Node* Parser::M_expr_2()
{
    while (M_peek().which() == TOK_ARITH_PLUS ||
           M_peek().which() == TOK_ARITH_MINUS ||
           M_peek().which() == TOK_LOGIC_NOT)
    {
        switch (M_peek().which())
        {
            case TOK_ARITH_PLUS:
            {
                M_get();
                break;
            }

            case TOK_ARITH_MINUS:
            {
                Token start_token = M_get();
                Node* node = M_check(M_expr_1());

                MethodNode* new_node = new MethodNode(start_token);
                new_node->addSibling(node);
                new_node->name = std_neg;

                return new_node;
            }

            case TOK_LOGIC_NOT:
            {
                Token start_token = M_get();
                Node* node = M_check(M_expr_1());

                MethodNode* new_node = new MethodNode(start_token);
                new_node->addSibling(node);
                new_node->name = M_operatorMethodName(start_token);

                return new_node;
            }
        }
    }

    return M_check(M_expr_1());
}

Node* Parser::M_expr_3()
{
    Node* node = M_check(M_expr_2());

    switch (M_peek().which())
    {
        case TOK_ARITH_MUL:
        case TOK_ARITH_DIV:
        case TOK_ARITH_MOD:
        do
        {
            Token start_token = M_get();
            Node* other_node = M_check(M_expr_2());

            MethodNode* new_node = new MethodNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);
            new_node->name = M_operatorMethodName(start_token);
            
            node = new_node;
        } while (M_peek().which() == TOK_ARITH_MUL ||
                 M_peek().which() == TOK_ARITH_DIV ||
                 M_peek().which() == TOK_ARITH_MOD);
    }

    return node;
}

Node* Parser::M_expr_4()
{
    Node* node = M_check(M_expr_3());

    switch (M_peek().which())
    {
        case TOK_ARITH_ADD:
        case TOK_ARITH_SUB:
        do
        {
            Token start_token = M_get();
            Node* other_node = M_check(M_expr_3());

            MethodNode* new_node = new MethodNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);
            new_node->name = M_operatorMethodName(start_token);

            node = new_node;
        } while (M_peek().which() == TOK_ARITH_ADD ||
                 M_peek().which() == TOK_ARITH_SUB);
    }

    return node;
}

Node* Parser::M_expr_5()
{
    Node* node = M_check(M_expr_4());

    switch (M_peek().which())
    {
        case TOK_REL_LT:
        case TOK_REL_LTE:
        case TOK_REL_GT:
        case TOK_REL_GTE:
        do
        {
            Token start_token = M_get();
            Node* other_node = M_check(M_expr_4());

            MethodNode* new_node = new MethodNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);
            new_node->name = M_operatorMethodName(start_token);

            node = new_node;
        } while (M_peek().which() == TOK_REL_LT ||
                 M_peek().which() == TOK_REL_LTE ||
                 M_peek().which() == TOK_REL_GT ||
                 M_peek().which() == TOK_REL_GTE);
    }

    return node;
}

Node* Parser::M_expr_6()
{
    Node* node = M_check(M_expr_5());

    switch (M_peek().which())
    {
        case TOK_REL_EQ:
        case TOK_REL_NEQ:
        do
        {
            Token start_token = M_get();
            Node* other_node = M_check(M_expr_5());
            
            MethodNode* new_node = new MethodNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);
            new_node->name = M_operatorMethodName(start_token);

            node = new_node;
        } while (M_peek().which() == TOK_REL_EQ ||
                 M_peek().which() == TOK_REL_NEQ);
    }

    return node;
}

Node* Parser::M_expr_7()
{
    Node* node = M_check(M_expr_6());

    switch (M_peek().which())
    {
        case TOK_LOGIC_AND:
        do
        {
            Token start_token = M_get();
            Node* other_node = M_check(M_expr_6());

            MethodNode* new_node = new MethodNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);
            new_node->name = M_operatorMethodName(start_token);
            
            node = new_node;
        } while (M_peek().which() == TOK_LOGIC_AND);
    }

    return node;
}

Node* Parser::M_expr_8()
{
    Node* node = M_expr_7();

    switch (M_peek().which())
    {
        case TOK_LOGIC_OR:
        do
        {
            Token start_token = M_get();
            Node* other_node = M_check(M_expr_7());

            MethodNode* new_node = new MethodNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);
            new_node->name = M_operatorMethodName(start_token);
            
            node = new_node;
        } while (M_peek().which() == TOK_LOGIC_OR);
    }

    return node;
}

Node* Parser::M_expr_9()
{
    Node* node = M_expr_8();

    switch (M_peek().which())
    {
        case TOK_ASSIGN:
        case TOK_ASSIGN_ADD:
        case TOK_ASSIGN_SUB:
        case TOK_ASSIGN_MUL:
        case TOK_ASSIGN_DIV:
        case TOK_ASSIGN_MOD:
        {
            Token start_token = M_get();
            Node* other_node = M_expr();

            AssignNode* new_node = new AssignNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);
            new_node->operation = "";

            if (start_token.which() != TOK_ASSIGN)
                new_node->operation = M_operatorMethodName(start_token.which());

            return new_node;
        }
    }

    return node;
}

Node* Parser::M_expr()
{ return M_check(M_expr_9()); }

Node* Parser::M_expr_list()
{
    Node* node = M_expr();

    if (M_peek().which() == TOK_COMMA)
    {
        M_eat(TOK_COMMA);
        node->chain(M_check(M_expr_list()));
    }

    return node;
}

Node* Parser::M_if_stmt()
{
    Token start_token = M_eat(TOK_KW_IF);
    M_eat(TOK_LPAR);
    Node* expr_node = M_check(M_expr());
    M_eat(TOK_RPAR);
    Node* stmt_node = M_check(M_stmt());

    IfNode* node = new IfNode(start_token);
    node->addSibling(expr_node);
    node->addSibling(stmt_node);

    while (M_peek().which() == TOK_KW_ELIF)
    {
        start_token = M_eat(TOK_KW_ELIF);
        M_eat(TOK_LPAR);
        expr_node = M_check(M_expr());
        M_eat(TOK_RPAR);
        stmt_node = M_check(M_stmt());

        ElifNode* new_node = new ElifNode(start_token);
        new_node->addSibling(expr_node);
        new_node->addSibling(stmt_node);

        node->chain(new_node);
    }

    if (M_peek().which() == TOK_KW_ELSE)
    {
        start_token = M_eat(TOK_KW_ELSE);
        stmt_node = M_check(M_stmt());

        ElseNode* new_node = new ElseNode(start_token);
        new_node->addSibling(stmt_node);

        node->chain(new_node);
    }

    return node;
}

Node* Parser::M_while_stmt()
{
    Token start_token = M_eat(TOK_KW_WHILE);
    M_eat(TOK_LPAR);
    Node* expr_node = M_check(M_expr());
    M_eat(TOK_RPAR);
    Node* stmt_node = M_check(M_stmt());

    WhileNode* node = new WhileNode(start_token);
    node->addSibling(expr_node);
    node->addSibling(stmt_node);

    return node;
}

Node* Parser::M_return_stmt()
{
    Token start_token = M_eat(TOK_KW_RETURN);
    Node* expr_node = nullptr;

    if (M_peek().which() != TOK_SEMICOLON)
        expr_node = M_check(M_expr());

    M_eat(TOK_SEMICOLON);

    ReturnNode* node = new ReturnNode(start_token);
    node->addSibling(expr_node);

    return node;
}

Node* Parser::M_break_stmt()
{
    Token start_token = M_eat(TOK_KW_BREAK);
    M_eat(TOK_SEMICOLON);

    BreakNode* node = new BreakNode(start_token);

    return node;
}

Node* Parser::M_continue_stmt()
{
    Token start_token = M_eat(TOK_KW_CONTINUE);
    M_eat(TOK_SEMICOLON);

    ContinueNode* node = new ContinueNode(start_token);

    return node;
}

Node* Parser::M_block_stmt()
{
    Node* node = nullptr;
    Node* last = nullptr;

    M_eat(TOK_LCURL);
    do
    {
        Node* new_node = M_check(M_stmt());

        if (!node)
            node = new_node;
        else
            last->chain(new_node);
        last = new_node;
    } while (M_peek().which() != TOK_RCURL);
    M_eat(TOK_RCURL);

    return node;
}

void Parser::M_param_list_decl()
{
    Token name = M_eat(TOK_IDENTIFIER);
    Token pattern_token;
    std::string pattern = "";

    if (M_peek().which() == TOK_COLON)
    {
        pattern_token = M_eat(TOK_COLON);

        switch (M_peek().which())
        {
            case TOK_WILDCARD:
                M_eat(TOK_WILDCARD);
                break;

            default:
                Node* name = M_name();
                pattern = static_cast<NameNode*>(name)->value;
                delete name;
                break;
        }
    }

    core::Class::Id id = core::Class::AnyId;
    if (pattern.size())
    {
        Symtab::FindResult res;
        bool ok = false;

        Symtab* scope = M_scope();
        do
        {
            if (scope->find(pattern, &res))
            {
                if (res.symbol->binding() == Symbol::Global)
                {
                    core::Object const& c = m_module.global(pattern);
                    if (c.meta().is<core::Class>())
                    {
                        id = c.unwrap<core::Class>().classid();
                        ok = true;
                        break;
                    }
                }
            }
        } while ((scope = scope->up()));

        if (!ok)
            error(pattern_token, "invalid class pattern");
    }

    Symbol sym(Symbol::Argument, Symbol::Local, name.what().unwrap<std::string>(), id);
    if (!M_check(M_scope())->add(sym))
        error(name, "parameter name clash");

    if (M_peek().which() == TOK_COMMA)
    {
        M_get();
        M_param_list_decl();
    }
}

Node* Parser::M_fun_decl(bool in_class)
{
    Token start_token = M_eat(TOK_KW_FUN);
    Token name = M_eat(TOK_IDENTIFIER);

    // Setup a new scope so we can store the parameter
    //   list inside
    M_pushScope();

    // If this function is rather a method, implicitely define the
    //   'self' argument
    if (in_class)
    {
        Symbol sym(Symbol::Argument, Symbol::Local, std_self, core::Class::AnyId);
        if (!M_check(M_scope())->add(sym))
            error(name, "internal error: self argument name clash");
    }

    // Get the parameter list
    M_eat(TOK_LPAR);
    if (M_peek().which() != TOK_RPAR)
        M_param_list_decl();
    M_eat(TOK_RPAR);

    // Get the function body
    Node* body = M_check(M_stmt());

    // Create the declarator node
    FunDeclNode* node = new FunDeclNode(start_token);
    node->name = name.what().unwrap<std::string>();
    node->addSibling(body);
    node->attachSymtab(M_popScope());

    // Add the function to the current scope
    if (!in_class)
    {
        Symbol symbol(Symbol::Variable, Symbol::Auto, node->name);
        if (!M_check(M_scope())->add(symbol, in_class ? true : false))
            error(name, "name clash");
    }

    return node;
}

Node* Parser::M_class_decl()
{
    Token start_token = M_eat(TOK_KW_CLASS);
    Token name = M_eat(TOK_IDENTIFIER);

    std::string classname = name.what().unwrap<std::string>();

    // Add the class to the current scope
    Symbol symbol(Symbol::Variable, Symbol::Auto, classname);
    if (!M_check(M_scope())->top()->add(symbol))
        error(name, "name clash");

    // M_pushScope();

    // Setup a bare class global here to allow the use
    //   of polymorphic patterns
    m_module.global(classname) = core::Class(m_module.name(), classname);

    M_eat(TOK_LCURL);

    Node* body = nullptr;
    do
    {
        Node* method = M_check(M_fun_decl(true));

        if (body)
            body->chain(method);
        else
            body = method;
    } while (M_peek().which() == TOK_KW_FUN);

    M_eat(TOK_RCURL);
    M_eat(TOK_SEMICOLON);

    ClassDeclNode* node = new ClassDeclNode(start_token);
    node->name = name.what().unwrap<std::string>();
    node->addSibling(body);
    // node->attachSymtab(M_popScope());

    return node;
}

Node* Parser::M_import_stmt()
{
    Token start_token = M_eat(TOK_KW_IMPORT);
    Token name_token;

    if (M_peek().which() == TOK_PACKAGE)
        name_token = M_get();
    else
        name_token = M_eat(TOK_IDENTIFIER);

    std::string name = name_token.what().unwrap<std::string>();
    bool masked = false;
    bool aliased = false;
    std::string mask = "";
    std::string alias = "";

    if (M_peek().which() == TOK_DOT)
    {
        masked = true;
        M_get();

        if (M_peek().which() == TOK_WILDCARD)
        {
            M_get();
            mask = std_package_wildcard;
        }
        else
        {
            mask = M_eat(TOK_IDENTIFIER).what().unwrap<std::string>();
        }
    }
    else if (M_peek().which() == TOK_KW_AS)
    {
        aliased = true;
        M_get();

        alias = M_eat(TOK_IDENTIFIER).what().unwrap<std::string>();
    }

    // Setup the symbol right now because the lookahead
    //   might need it
    Symbol symbol(Symbol::Package, Symbol::Global, aliased ? alias : name);
    M_check(M_scope())->add(symbol);

    // Load the package right now also
    try
    {
        if (masked)
            m_module.importTable()->importMask(m_module, name, mask);
        else if (aliased)
            m_module.importTable()->importAs(m_module, name, alias);
        else
            m_module.importTable()->import(m_module, name);
    }
    catch(std::exception const& exc)
    {
        std::string desc = exc.what();

        std::ostringstream ss;
        ss << note_color << util::ansi::bold;
        ss << "note: " << util::ansi::clear;
        ss << "during import of package ";
        ss << emph_color << util::ansi::bold;
        ss << name << util::ansi::clear;
        std::string top_err = message(start_token, ss.str());

        ss.str("");
        ss << desc << std::endl << top_err;

        throw std::runtime_error(ss.str());
    }

    M_eat(TOK_SEMICOLON);

    // Create the AST node
    Node* node = nullptr;
    if (masked)
    {
        ImportMaskNode* new_node = new ImportMaskNode(start_token);
        new_node->name = name;
        new_node->mask = mask;

        node = new_node;
    }
    else
    {
        ImportNode* new_node = new ImportNode(start_token);
        new_node->name = name;

        node = new_node;
    }

    return node;
}

Node* Parser::M_stmt()
{
    Node* node = nullptr;

    switch (M_peek().which())
    {
        case TOK_KW_IF:
            node = M_check(M_if_stmt());
            break;

        case TOK_KW_WHILE:
            node = M_check(M_while_stmt());
            break;

        case TOK_KW_RETURN:
            node = M_check(M_return_stmt());
            break;

        case TOK_KW_BREAK:
            node = M_check(M_break_stmt());
            break;

        case TOK_KW_CONTINUE:
            node = M_check(M_continue_stmt());
            break;

        case TOK_LCURL:
            node = M_check(M_block_stmt());
            break;

        case TOK_KW_FUN:
            node = M_check(M_fun_decl());
            break;

        case TOK_KW_CLASS:
            node = M_check(M_class_decl());
            break;

        case TOK_KW_IMPORT:
            node = M_check(M_import_stmt());
            break;

        default:
            node = M_check(M_expr());
            M_eat(TOK_SEMICOLON);
            break;
    }

    return node;
}

Node* Parser::M_prog()
{
    M_pushScope();

    m_module.importTable()->attachSymtab(M_scope());

    Node* node = nullptr;
    Node* last = nullptr;
    do
    {
        Node* new_node = M_check(M_stmt());

        if (!node)
            node = new_node;
        else
            last->chain(new_node);
        
        last = new_node;
    } while (M_peek().which() != Token::Eof);

    ProgNode* prog = new ProgNode(node->startToken());
    prog->addSibling(node);
    prog->attachSymtab(M_popScope());

    m_module.importTable()->detachSymtab();
    return prog;
}

void Parser::M_setupLexer()
{
    M_define("WS",
             "'[ \t\n]'+ | (\"//\" '[^\n]'*) | (\"/*\" ('[^*]' | ('*'+ '[^*/]'))* \"*/\")",
             Token(Token::Skip));

    M_define("LPAR",         "'('",         Token(TOK_LPAR));
    M_define("RPAR",         "')'",         Token(TOK_RPAR));
    M_define("LCURL",        "'{'",         Token(TOK_LCURL));
    M_define("RCURL",        "'}'",         Token(TOK_RCURL));
    M_define("DOT",          "'.'",         Token(TOK_DOT));
    M_define("COMMA",        "','",         Token(TOK_COMMA));
    M_define("SEMICOLON",    "';'",         Token(TOK_SEMICOLON));
    M_define("COLON",        "':'",         Token(TOK_COLON));

    M_define("ARITH_ADD",    "'+'",         Token(TOK_ARITH_ADD));
    M_define("ARITH_SUB",    "'-'",         Token(TOK_ARITH_SUB));
    M_define("ARITH_MUL",    "'*'",         Token(TOK_ARITH_MUL));
    M_define("ARITH_DIV",    "'/'",         Token(TOK_ARITH_DIV));
    M_define("ARITH_MOD",    "'%'",         Token(TOK_ARITH_MOD));
    M_define("LOGIC_NOT",    "'!'",         Token(TOK_LOGIC_NOT));
    M_define("LOGIC_AND",    "\"&&\"",      Token(TOK_LOGIC_AND));
    M_define("LOGIC_OR",     "\"||\"",      Token(TOK_LOGIC_OR));
    M_define("REL_LT",       "'<'",         Token(TOK_REL_LT));
    M_define("REL_LTE",      "\"<=\"",      Token(TOK_REL_LTE));
    M_define("REL_GT",       "'>'",         Token(TOK_REL_GT));
    M_define("REL_GTE",      "\">=\"",      Token(TOK_REL_GTE));
    M_define("REL_EQ",       "\"==\"",      Token(TOK_REL_EQ));
    M_define("REL_NEQ",      "\"!=\"",      Token(TOK_REL_NEQ));
    M_define("ASSIGN",       "'='",         Token(TOK_ASSIGN));
    M_define("ASSIGN_ADD",   "\"+=\"",        Token(TOK_ASSIGN_ADD));
    M_define("ASSIGN_SUB",   "\"-=\"",        Token(TOK_ASSIGN_SUB));
    M_define("ASSIGN_MUL",   "\"*=\"",        Token(TOK_ASSIGN_MUL));
    M_define("ASSIGN_DIV",   "\"/=\"",        Token(TOK_ASSIGN_DIV));
    M_define("ASSIGN_MOD",   "\"%=\"",        Token(TOK_ASSIGN_MOD));

    M_define("KW_IMPORT",   "\"import\"",   Token(TOK_KW_IMPORT));
    M_define("KW_IF",       "\"if\"",       Token(TOK_KW_IF));
    M_define("KW_ELIF",     "\"elif\"",     Token(TOK_KW_ELIF));
    M_define("KW_ELSE",     "\"else\"",     Token(TOK_KW_ELSE));
    M_define("KW_WHILE",    "\"while\"",    Token(TOK_KW_WHILE));
    M_define("KW_FUN",      "\"fun\"",      Token(TOK_KW_FUN));
    M_define("KW_RETURN",   "\"return\"",   Token(TOK_KW_RETURN));
    M_define("KW_CLASS",    "\"class\"",    Token(TOK_KW_CLASS));
    M_define("KW_AS",       "\"as\"",       Token(TOK_KW_AS));
    M_define("KW_BREAK",    "\"break\"",    Token(TOK_KW_BREAK));
    M_define("KW_CONTINUE", "\"continue\"", Token(TOK_KW_CONTINUE));

    M_define("IDENTIFIER", "'[_a-zA-Z]' '[_a-zA-Z0-9]'*",
            [&](std::string const& lexeme)
            {
                Symtab::FindResult res;
                if (M_scope() && M_scope()->find(lexeme, &res))
                {
                    if (res.symbol->which() == Symbol::Package)
                        return Token(TOK_PACKAGE, lexeme);
                }

                return Token(TOK_IDENTIFIER, lexeme);
            });

    M_define("LIT_CHAR", "'\\'' '\\\\'? . '\\''",
            [](std::string const& lexeme)
            {
                char val = lexeme[1];
                if (val == '\\')
                {
                    switch (lexeme[2])
                    {
                        case '\\':
                            break;

                        case 'n':
                            val = '\n';
                            break;

                        case 't':
                            val = '\t';
                            break;

                        case '\'':
                            val = '\'';
                            break;

                        default:
                            val = '\0';
                            break;
                    }
                }
                return Token(TOK_LIT_CHAR, val);
            });

    M_define("LIT_STRING", "'\"' ('[^\"\\\\]' | ('\\\\' .))* '\"'",
            [](std::string const& lexeme)
            { return Token(TOK_LIT_STRING, lexeme.substr(1, lexeme.size()-2)); });

    M_define("LIT_FLOATING", "'[0-9]'+ '.' '[0-9]'+",
            [](std::string const& lexeme)
            { return Token(TOK_LIT_FLOATING, std::stof(lexeme)); });

    M_define("LIT_INTEGER", "('[0-9]'+) | ((\"0x\" | \"0X\") '[0-9a-fA-F]'+) | ('-'? '0' '[0-7]'+) | ((\"0b\" | \"0B\") '[0-1]'+)",
            [](std::string const& lexeme)
            {
                const char* start = lexeme.c_str();
                int base = 10;
                int minus = 0;

                if (*start == '-')
                {
                    minus = 1;
                    ++start;
                }

                if (*start == '0')
                {
                    ++start;
                    if (*start == 'x' ||
                        *start == 'X')
                    {
                        ++start;
                        base = 16;
                    }
                    else if (*start == 'b' ||
                             *start == 'B')
                    {
                        ++start;
                        base = 2;
                    }
                    else
                        base = 8;
                }

                unsigned int number = 0;

                auto digit = [&](char c)
                {
                    if (base == 16)
                    {
                        if (c >= 'a' && c <= 'f')
                            return 10 + c - 'a';
                        else if (c >= 'A' && c <= 'F')
                            return 10 + c - 'A';
                    }

                    return c - '0';
                };

                while (*start)
                    number = number * base + digit(*start++);

                if (minus)
                    return Token(TOK_LIT_INTEGER, - (int) number);
                
                return Token(TOK_LIT_INTEGER, (int) number);
            });
}

void Parser::M_setupTokens()
{
    #define DEF_TOKEN(name, printable, prefer_lexeme) M_setTokenName(TOK_ ## name, printable, prefer_lexeme);
    #define DEF_ALIAS(name, alias)
    #include "lang/tokens.def"
    #undef DEF_ALIAS
    #undef DEF_TOKEN

    M_setTokenName(Token::Invalid, "invalid");
    M_setTokenName(Token::Eof, "eof");
    M_setTokenName(Token::Skip, "skip");
}

Node* Parser::M_check(Node* node)
{
    if (!node)
        error(M_peek(), "internal error (null node)");

    return node;
}

Symtab* Parser::M_check(Symtab* symtab)
{
    if (!symtab)
        error(M_peek(), "internal error (no scope)");

    return symtab;
}

std::string Parser::M_operatorMethodName(Token const& token)
{
    static const std::map<int, std::string> names =
    {
        { TOK_ARITH_ADD, std_add },
        { TOK_ARITH_SUB, std_sub },
        { TOK_ARITH_MUL, std_mul },
        { TOK_ARITH_DIV, std_div },
        { TOK_ARITH_MOD, std_mod },
        { TOK_LOGIC_AND, std_and },
        { TOK_LOGIC_OR, std_or },
        { TOK_LOGIC_NOT, std_not },
        { TOK_REL_LT, std_lt },
        { TOK_REL_LTE, std_lte },
        { TOK_REL_GT, std_gt },
        { TOK_REL_GTE, std_gte },
        { TOK_REL_EQ, std_equals },
        { TOK_REL_NEQ, std_nequals },
        { TOK_ASSIGN_ADD, std_add},
        { TOK_ASSIGN_SUB, std_sub},
        { TOK_ASSIGN_MUL, std_mul},
        { TOK_ASSIGN_DIV, std_div},
        { TOK_ASSIGN_MOD, std_mod}
    };
    
    auto it = names.find(token.which());
    if (it == names.end())
        error(token, "operator is not mapped to any standard method name");

    return it->second;
}

Symtab* Parser::M_scope() const
{
    if (m_scope.size())
        return m_scope.top();

    return nullptr;
}

Symtab* Parser::M_pushScope()
{
    Symtab* symtab = new Symtab(M_scope());
    m_scope.push(symtab);

    return symtab;
}

Symtab* Parser::M_popScope()
{
    Symtab* symtab = M_scope();
    if (symtab)
        m_scope.pop();

    return symtab;
}
