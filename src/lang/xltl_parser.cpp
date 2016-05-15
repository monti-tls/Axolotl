#include "lang/xltl_parser.hpp"
#include "lang/std_names.hpp"
#include "lang/ast_xltl_node.hpp"
#include "lang/xltl_symtab.hpp"
#include "lib/dict.hpp"

#include "lang/ast_xltl_node_visitor.hpp"
#include "lang/ast_xltl_node_generator.hpp"

#include "lang/ast_xltl_node_visitor_impl.hpp"
#include "lang/ast_xltl_node_generator_impl.hpp"

#include <algorithm>
#include <sstream>

using namespace lang;
using namespace ast;
using namespace lib;

XltlParser::XltlParser(std::istream& in)
    : Parser(in, 1)
{ M_build(); }

XltlParser::~XltlParser()
{}

XltlNode* XltlParser::parse()
{ return M_prog(); }

XltlNode* XltlParser::M_name()
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

    XltlNameNode* node = new XltlNameNode(start_token);
    node->value = ss.str();

    return node;
}

XltlNode* XltlParser::M_expr_0()
{    
    switch (M_peek().which())
    {
        // Expression group
        case TOK_LPAR:
        {
            M_eat(TOK_LPAR);
            XltlNode* node = M_check(M_expr());
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
        {
            Token start_token = M_get();

            XltlConstNode* node = new XltlConstNode(start_token);
            node->value = start_token.what();
            return node;
        }

        default:
            M_unexpected(M_peek());
            break;
    }

    return nullptr;
}

XltlNode* XltlParser::M_expr_1()
{
    XltlNode* node = M_check(M_expr_0());

    while (M_peek().which() == TOK_LPAR ||
           M_peek().which() == TOK_DOT)
    {
        switch (M_peek().which())
        {
            // Object invocation
            case TOK_LPAR:
            {
                Token start_token;
                XltlNode* args = nullptr;

                start_token = M_eat(TOK_LPAR);
                if (M_peek().which() != TOK_RPAR)
                    args = M_check(M_expr_list());
                M_eat(TOK_RPAR);

                XltlInvokeNode* new_node = new XltlInvokeNode(start_token);
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
                    XltlNode* args = nullptr;

                    M_eat(TOK_LPAR);
                    if (M_peek().which() != TOK_RPAR)
                        args = M_check(M_expr_list());
                    M_eat(TOK_RPAR);

                    XltlMethodNode* new_node = new XltlMethodNode(start_token);
                    new_node->addSibling(node);
                    new_node->addSibling(args);
                    new_node->name = name.what().unwrap<std::string>();

                    node = new_node;
                }
                // Simple member access
                else
                {
                    XltlMemberNode* new_node = new XltlMemberNode(start_token);
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

XltlNode* XltlParser::M_expr_2()
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
                XltlNode* node = M_check(M_expr_1());

                XltlMethodNode* new_node = new XltlMethodNode(start_token);
                new_node->addSibling(node);
                new_node->name = std_neg;

                node = new_node;
                break;
            }

            case TOK_LOGIC_NOT:
            {
                Token start_token = M_get();
                XltlNode* node = M_check(M_expr_1());

                XltlMethodNode* new_node = new XltlMethodNode(start_token);
                new_node->addSibling(node);
                new_node->name = M_operatorMethodName(start_token);

                node = new_node;
                break;
            }
        }
    }

    return M_check(M_expr_1());
}

XltlNode* XltlParser::M_expr_3()
{
    XltlNode* node = M_check(M_expr_2());

    switch (M_peek().which())
    {
        case TOK_ARITH_MUL:
        case TOK_ARITH_DIV:
        case TOK_ARITH_MOD:
        do
        {
            Token start_token = M_get();
            XltlNode* other_node = M_check(M_expr_2());

            XltlMethodNode* new_node = new XltlMethodNode(start_token);
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

XltlNode* XltlParser::M_expr_4()
{
    XltlNode* node = M_check(M_expr_3());

    switch (M_peek().which())
    {
        case TOK_ARITH_ADD:
        case TOK_ARITH_SUB:
        do
        {
            Token start_token = M_get();
            XltlNode* other_node = M_check(M_expr_3());

            XltlMethodNode* new_node = new XltlMethodNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);
            new_node->name = M_operatorMethodName(start_token);

            node = new_node;
        } while (M_peek().which() == TOK_ARITH_ADD ||
                 M_peek().which() == TOK_ARITH_SUB);
    }

    return node;
}

XltlNode* XltlParser::M_expr_5()
{
    XltlNode* node = M_check(M_expr_4());

    switch (M_peek().which())
    {
        case TOK_REL_LT:
        case TOK_REL_LTE:
        case TOK_REL_GT:
        case TOK_REL_GTE:
        do
        {
            Token start_token = M_get();
            XltlNode* other_node = M_check(M_expr_4());

            XltlMethodNode* new_node = new XltlMethodNode(start_token);
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

XltlNode* XltlParser::M_expr_6()
{
    XltlNode* node = M_check(M_expr_5());

    switch (M_peek().which())
    {
        case TOK_REL_EQ:
        case TOK_REL_NEQ:
        do
        {
            Token start_token = M_get();
            XltlNode* other_node = M_check(M_expr_5());
            
            XltlMethodNode* new_node = new XltlMethodNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);
            new_node->name = M_operatorMethodName(start_token);

            node = new_node;
        } while (M_peek().which() == TOK_REL_EQ ||
                 M_peek().which() == TOK_REL_NEQ);
    }

    return node;
}

XltlNode* XltlParser::M_expr_7()
{
    XltlNode* node = M_check(M_expr_6());

    switch (M_peek().which())
    {
        case TOK_LOGIC_AND:
        do
        {
            Token start_token = M_get();
            XltlNode* other_node = M_check(M_expr_6());

            XltlMethodNode* new_node = new XltlMethodNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);
            new_node->name = M_operatorMethodName(start_token);
            
            node = new_node;
        } while (M_peek().which() == TOK_LOGIC_AND);
    }

    return node;
}

XltlNode* XltlParser::M_expr_8()
{
    XltlNode* node = M_expr_7();

    switch (M_peek().which())
    {
        case TOK_LOGIC_OR:
        do
        {
            Token start_token = M_get();
            XltlNode* other_node = M_check(M_expr_7());

            XltlMethodNode* new_node = new XltlMethodNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);
            new_node->name = M_operatorMethodName(start_token);
            
            node = new_node;
        } while (M_peek().which() == TOK_LOGIC_OR);
    }

    return node;
}

XltlNode* XltlParser::M_expr_9()
{
    XltlNode* node = M_expr_8();

    switch (M_peek().which())
    {
        case TOK_ASSIGN:
        {
            Token start_token = M_eat(TOK_ASSIGN);
            XltlNode* other_node = M_expr();

            XltlAssignNode* new_node = new XltlAssignNode(start_token);
            new_node->addSibling(node);
            new_node->addSibling(other_node);

            return new_node;
        }
    }

    return node;
}

XltlNode* XltlParser::M_expr()
{ return M_check(M_expr_9()); }

XltlNode* XltlParser::M_expr_list()
{
    XltlNode* node = M_expr();

    if (M_peek().which() == TOK_COMMA)
    {
        M_eat(TOK_COMMA);
        node->chain(M_check(M_expr_list()));
    }

    return node;
}

XltlNode* XltlParser::M_if_stmt()
{
    Token start_token = M_eat(TOK_KW_IF);
    M_eat(TOK_LPAR);
    XltlNode* expr_node = M_check(M_expr());
    M_eat(TOK_RPAR);
    XltlNode* stmt_node = M_check(M_stmt());

    XltlIfNode* node = new XltlIfNode(start_token);
    node->addSibling(expr_node);
    node->addSibling(stmt_node);

    while (M_peek().which() == TOK_KW_ELIF)
    {
        start_token = M_eat(TOK_KW_ELIF);
        M_eat(TOK_LPAR);
        expr_node = M_check(M_expr());
        M_eat(TOK_RPAR);
        stmt_node = M_check(M_stmt());

        XltlElifNode* new_node = new XltlElifNode(start_token);
        new_node->addSibling(expr_node);
        new_node->addSibling(stmt_node);

        node->chain(new_node);
    }

    if (M_peek().which() == TOK_KW_ELSE)
    {
        start_token = M_eat(TOK_KW_ELSE);
        stmt_node = M_check(M_stmt());

        XltlElseNode* new_node = new XltlElseNode(start_token);
        new_node->addSibling(stmt_node);

        node->chain(new_node);
    }

    return node;
}

XltlNode* XltlParser::M_while_stmt()
{
    Token start_token = M_eat(TOK_KW_WHILE);
    M_eat(TOK_LPAR);
    XltlNode* expr_node = M_check(M_expr());
    M_eat(TOK_RPAR);
    XltlNode* stmt_node = M_check(M_stmt());

    XltlWhileNode* node = new XltlWhileNode(start_token);
    node->addSibling(expr_node);
    node->addSibling(stmt_node);

    return node;
}

XltlNode* XltlParser::M_return_stmt()
{
    Token start_token = M_eat(TOK_KW_RETURN);
    XltlNode* expr_node = nullptr;

    if (M_peek().which() != TOK_SEMICOLON)
        expr_node = M_check(M_expr());

    M_eat(TOK_SEMICOLON);

    XltlReturnNode* node = new XltlReturnNode(start_token);
    node->addSibling(expr_node);

    return node;
}

XltlNode* XltlParser::M_block_stmt()
{
    XltlNode* node = nullptr;
    XltlNode* last = nullptr;

    M_eat(TOK_LCURL);
    do
    {
        XltlNode* new_node = M_check(M_stmt());

        if (!node)
            node = new_node;
        else
            last->chain(new_node);
        last = new_node;
    } while (M_peek().which() != TOK_RCURL);
    M_eat(TOK_RCURL);

    return node;
}

void XltlParser::M_param_list_decl()
{
    Token name = M_eat(TOK_IDENTIFIER);
    std::string pattern = "";

    if (M_peek().which() == TOK_COLON)
    {
        M_eat(TOK_COLON);

        switch (M_peek().which())
        {
            case TOK_WILDCARD:
                M_eat(TOK_WILDCARD);
                break;

            default:
                XltlNode* name = M_name();
                pattern = static_cast<XltlNameNode*>(name)->value;
                delete name;
                break;
        }
    }

    XltlSymbol sym(XltlSymbol::Argument, name.what().unwrap<std::string>(), pattern);
    if (!M_check(M_scope())->add(sym))
        error(name, "parameter name clash");

    if (M_peek().which() == TOK_COMMA)
    {
        M_get();
        M_param_list_decl();
    }
}

XltlNode* XltlParser::M_fun_decl()
{
    Token start_token = M_eat(TOK_KW_FUN);
    Token name = M_eat(TOK_IDENTIFIER);

    // Setup a new scope so we can store the parameter
    //   list inside
    M_pushScope();

    // Get the parameter list
    M_eat(TOK_LPAR);
    if (M_peek().which() != TOK_RPAR)
        M_param_list_decl();
    M_eat(TOK_RPAR);

    // Get the function body
    XltlNode* body = M_check(M_stmt());

    // Create the declarator node
    XltlFunDeclNode* node = new XltlFunDeclNode(start_token);
    node->name = name.what().unwrap<std::string>();
    node->addSibling(body);
    node->attachSymtab(M_popScope());

    // Add the function to the current scope
    XltlSymbol symbol(XltlSymbol::Auto, node->name);
    if (!M_check(M_scope())->add(symbol))
        error(name, "redefining function");

    return node;
}

XltlNode* XltlParser::M_stmt()
{
    XltlNode* node = nullptr;

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

        case TOK_LCURL:
            node = M_check(M_block_stmt());
            break;

        case TOK_KW_FUN:
            node = M_check(M_fun_decl());
            break;

        default:
            node = M_check(M_expr());
            M_eat(TOK_SEMICOLON);
            break;
    }

    return node;
}

XltlNode* XltlParser::M_prog()
{
    XltlSymtab* symtab = M_pushScope();
    symtab->add(XltlSymbol(XltlSymbol::Auto, std_const_dict, Dict()));

    XltlNode* node = nullptr;
    XltlNode* last = nullptr;
    do
    {
        XltlNode* new_node = M_check(M_stmt());

        if (!node)
            node = new_node;
        else
            last->chain(new_node);
        
        last = new_node;
    } while (M_peek().which() != Token::Eof);

    XltlProgNode* prog = new XltlProgNode(node->startToken());
    prog->addSibling(node);
    prog->attachSymtab(M_popScope());

    return prog;
}

void XltlParser::M_setupLexer()
{
    M_define("WS",
             "'[ \t\n]'+ | (\"//\" '[^\n]'*) | (\"/*\" ('[^*]' | ('*'+ '[^*/]'))* \"*/\")",
             Token(Token::Skip));

    M_define("LPAR",         "'('",     Token(TOK_LPAR));
    M_define("RPAR",         "')'",     Token(TOK_RPAR));
    M_define("LCURL",        "'{'",     Token(TOK_LCURL));
    M_define("RCURL",        "'}'",     Token(TOK_RCURL));
    M_define("DOT",          "'.'",     Token(TOK_DOT));
    M_define("COMMA",        "','",     Token(TOK_COMMA));
    M_define("SEMICOLON",    "';'",     Token(TOK_SEMICOLON));
    M_define("COLON",        "':'",     Token(TOK_COLON));
    M_define("YIELDS",       "\"->\"",  Token(TOK_YIELDS));
    M_define("YIELDS_MAYBE", "\"->?\"", Token(TOK_YIELDS_MAYBE));

    M_define("ARITH_ADD",    "'+'",     Token(TOK_ARITH_ADD));
    M_define("ARITH_SUB",    "'-'",     Token(TOK_ARITH_SUB));
    M_define("ARITH_MUL",    "'*'",     Token(TOK_ARITH_MUL));
    M_define("ARITH_DIV",    "'/'",     Token(TOK_ARITH_DIV));
    M_define("ARITH_MOD",    "'%'",     Token(TOK_ARITH_MOD));
    M_define("LOGIC_NOT",    "'!'",     Token(TOK_LOGIC_NOT));
    M_define("LOGIC_AND",    "\"&&\"",  Token(TOK_LOGIC_AND));
    M_define("LOGIC_OR",     "\"||\"",  Token(TOK_LOGIC_OR));
    M_define("REL_LT",       "'<'",     Token(TOK_REL_LT));
    M_define("REL_LTE",      "\"<=\"",  Token(TOK_REL_LTE));
    M_define("REL_GT",       "'>'",     Token(TOK_REL_GT));
    M_define("REL_GTE",      "\">=\"",  Token(TOK_REL_GTE));
    M_define("REL_EQ",       "\"==\"",  Token(TOK_REL_EQ));
    M_define("REL_NEQ",      "\"!=\"",  Token(TOK_REL_NEQ));
    M_define("ASSIGN",       "'='",     Token(TOK_ASSIGN));

    M_define("KW_IMPORT", "\"import\"", Token(TOK_KW_IMPORT));
    M_define("KW_IF", "\"if\"",         Token(TOK_KW_IF));
    M_define("KW_ELIF", "\"elif\"",     Token(TOK_KW_ELIF));
    M_define("KW_ELSE", "\"else\"",     Token(TOK_KW_ELSE));
    M_define("KW_WHILE", "\"while\"",   Token(TOK_KW_WHILE));
    M_define("KW_FUN", "\"fun\"",       Token(TOK_KW_FUN));
    M_define("KW_RETURN", "\"return\"", Token(TOK_KW_RETURN));
    M_define("KW_CLASS", "\"class\"",   Token(TOK_KW_CLASS));

    M_define("IDENTIFIER", "'[_a-zA-Z]' '[_a-zA-Z0-9]'*",
            [](std::string const& lexeme)
            { return Token(TOK_IDENTIFIER, lexeme); });

    M_define("LIT_CHAR", "'\\'' '\\\\'? . '\\''",
            [](std::string const& lexeme)
            { return Token(TOK_LIT_CHAR, (char) lexeme[0]); });

    M_define("LIT_STRING", "'\"' ('[^\"\\\\]' | ('\\\\' .))* '\"'",
            [](std::string const& lexeme)
            { return Token(TOK_LIT_STRING, lexeme); });

    M_define("LIT_INTEGER", "('-'? '[0-9]'+) | ((\"0x\" | \"0X\") '[0-9a-fA-F]'+) | ('-'? '0' '[0-7]'+) | ((\"0b\" | \"0B\") '[0-1]'+)",
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

void XltlParser::M_setupTokens()
{
    #define DEF_TOKEN(name) M_setTokenName(TOK_ ## name, #name);
    #define DEF_ALIAS(name, alias) M_setTokenName(TOK_ ## name, #name);
    #include "lang/xltl_tokens.def"
    #undef DEF_ALIAS
    #undef DEF_TOKEN

    M_setTokenName(Token::Invalid, "Invalid");
    M_setTokenName(Token::Eof, "Eof");
    M_setTokenName(Token::Skip, "Skip");
}

XltlNode* XltlParser::M_check(XltlNode* node)
{
    if (!node)
        error(M_peek(), "internal error (null node)");

    return node;
}

XltlSymtab* XltlParser::M_check(XltlSymtab* symtab)
{
    if (!symtab)
        error(M_peek(), "internal error (no scope)");

    return symtab;
}

std::string XltlParser::M_operatorMethodName(Token const& token)
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
        { TOK_REL_NEQ, std_nequals }
    };
    
    auto it = names.find(token.which());
    if (it == names.end())
        error(token, "operator is not mapped to any standard method name");

    return it->second;
}

XltlSymtab* XltlParser::M_scope() const
{
    if (m_scope.size())
        return m_scope.top();

    return nullptr;
}

XltlSymtab* XltlParser::M_pushScope()
{
    XltlSymtab* symtab = new XltlSymtab(M_scope());
    m_scope.push(symtab);

    return symtab;
}

XltlSymtab* XltlParser::M_popScope()
{
    XltlSymtab* symtab = M_scope();
    if (symtab)
        m_scope.pop();

    return symtab;
}
