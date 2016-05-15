#include "lang/parser.hpp"
#include "util/ansi.hpp"

#include <algorithm>
#include <sstream>

using namespace lang;

Parser::Parser(std::istream& in, std::size_t lookahead, std::size_t lex_lookahead)
    : m_lexer(in, lex_lookahead)
    , m_lookahead_depth(lookahead)
{ }

Parser::~Parser()
{}

void Parser::rewind()
{
    m_lexer.rewind();
    M_initLookahead();
}

std::string Parser::message(Token const& token, std::string const& msg)
{
    std::ostringstream ss;
    ss << util::ansi::bold << token.where().filename << ":" << token.where().line << ":" << token.where().col << ": ";
    ss << util::ansi::clear << msg << std::endl;

    std::size_t pos;
    std::string line = m_lexer.snippet(token, pos);

    if (pos)
        --pos;

    if (line.size())
    {
        std::string fmt = util::ansi::colors::RebeccaPurple;
        line.insert(pos, fmt);
        line.insert(pos + fmt.size() + token.lexeme().size(), util::ansi::clear);
    }

    pos += 4;
    ss << "    " << line << std::endl;

    for (int i = 0; i < ((int) pos); ++i)
        ss << " ";
    ss << util::ansi::colors::RebeccaPurple << "^";

    for (int i = 0; i < ((int) token.lexeme().size()) - 1; ++i)
        ss << "~";
    ss << util::ansi::clear << std::endl;

    return ss.str();
}

void Parser::error(Token const& token, std::string const& msg)
{
    std::ostringstream ss;
    ss << util::ansi::colors::Tomato << util::ansi::bold;
    ss << "error: " << util::ansi::clear;
    ss << msg;
    throw std::runtime_error(message(token, ss.str()));
}

void Parser::warning(Token const& token, std::string const& msg)
{
    std::ostringstream ss;
    ss << util::ansi::colors::Orange << util::ansi::bold;
    ss << "warning: " << util::ansi::clear;
    ss << msg;

    std::clog << message(token, ss.str());
}

void Parser::M_build()
{
    M_setupTokens();
    M_setupLexer();
    m_lexer.build();

    rewind();
}

void Parser::M_define(std::string const& name, std::string const& definition, core::Object const& build_token)
{ m_lexer.define(name, definition, build_token); }

void Parser::M_setTokenName(int token, std::string const& name)
{ m_token_names.insert(std::make_pair(token, name)); }

std::string Parser::M_tokenName(int token)
{
    auto it = m_token_names.find(token);
    if (it == m_token_names.end())
        return "???";

    return it->second;
}

void Parser::M_initLookahead()
{
    m_lookahead.resize(m_lookahead_depth);
    for (int i = 0; i < (int) m_lookahead_depth; ++i)
        M_get();
}

Token Parser::M_get()
{
    // Save top element
    Token top = m_lookahead.front();

    // Shift left
    std::rotate(m_lookahead.begin(), m_lookahead.begin() + 1, m_lookahead.end());

    // Get a new token
    m_lookahead.back() = m_lexer.getToken();

    return top;
}

Token Parser::M_eat(int which)
{
    if (M_peek().which() != which)
    {
        std::ostringstream ss;
        ss << "expecting " << M_tokenName(which) << ", got " << M_tokenName(M_peek().which());
        error(M_peek(), ss.str());
    }

    return M_get();
}

Token const& Parser::M_peek(std::size_t depth)
{
    if (depth >= m_lookahead_depth)
        throw std::runtime_error("lang::Parser::M_peek: peek depth larger than lookahead");

    return m_lookahead[depth];
}

void Parser::M_unexpected(Token const& token)
{ error(token, "unexpected token " + M_tokenName(token.which())); }
