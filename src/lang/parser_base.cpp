#include "lang/parser_base.hpp"
#include "util/ansi.hpp"
#include "util/ios_filename.hpp"

#include <algorithm>
#include <sstream>

using namespace lang;

ParserBase::ParserBase(std::istream& in, std::size_t lookahead, std::size_t lex_lookahead)
    : m_lexer(in, lex_lookahead)
    , m_lookahead_depth(lookahead)
{ }

ParserBase::~ParserBase()
{}

void ParserBase::rewind()
{
    m_lexer.rewind();
    M_initLookahead();
}

std::string ParserBase::message(Token const& token, std::string const& msg)
{
    std::ostringstream ss;
    ss << util::ansi::bold << token.where().filename << ":" << token.where().line << ":" << token.where().col << ": ";
    ss << util::ansi::clear << msg << std::endl;

    std::size_t pos;
    std::string line = m_lexer.snippet(token, pos);

    if (line.size())
    {
        std::string fmt = emph_color;
        line.insert(pos, fmt);
        std::size_t end = std::min(line.size()-1, pos + fmt.size() + token.lexeme().size());
        line.insert(end, util::ansi::clear);
    }

    pos += 4;
    ss << "    " << line << std::endl;

    for (int i = 0; i < ((int) pos); ++i)
        ss << " ";
    ss << emph_color << "^";

    for (int i = 0; i < ((int) token.lexeme().size()) - 1; ++i)
        ss << "~";
    ss << util::ansi::clear << std::endl;

    return ss.str();
}

void ParserBase::error(Token const& token, std::string const& msg)
{
    std::ostringstream ss;
    ss << error_color << util::ansi::bold;
    ss << "error: " << util::ansi::clear;
    ss << msg;
    throw std::runtime_error(message(token, ss.str()));
}

void ParserBase::warning(Token const& token, std::string const& msg)
{
    std::ostringstream ss;
    ss << warning_color << util::ansi::bold;
    ss << "warning: " << util::ansi::clear;
    ss << msg;

    std::clog << message(token, ss.str());
}

std::string ParserBase::streamName() const
{ return m_lexer.streamName(); }

void ParserBase::M_build()
{
    M_setupTokens();
    M_setupLexer();
    m_lexer.build();

    rewind();
}

void ParserBase::M_define(std::string const& name, std::string const& definition, core::Object build_token)
{ m_lexer.define(name, definition, build_token); }

void ParserBase::M_setTokenName(int token, std::string const& name, bool prefer_lexeme)
{ m_token_names.insert(std::make_pair(token, std::make_pair(prefer_lexeme, name))); }

std::string ParserBase::M_tokenName(int token) const
{
    auto it = m_token_names.find(token);
    if (it == m_token_names.end())
        return "???";

    return it->second.second;
}

std::string ParserBase::M_tokenName(Token const& token) const
{
    auto it = m_token_names.find(token.which());
    if (it == m_token_names.end())
        return "???";

    if (it->second.first)
        return token.lexeme();

    return it->second.second;
}

void ParserBase::M_initLookahead()
{
    m_lookahead.resize(m_lookahead_depth);
    for (int i = 0; i < (int) m_lookahead_depth; ++i)
        M_get();
}

Token ParserBase::M_get()
{
    // Save top element
    Token top = m_lookahead.front();

    // Shift left
    std::rotate(m_lookahead.begin(), m_lookahead.begin() + 1, m_lookahead.end());

    // Get a new token
    m_lookahead.back() = m_lexer.getToken();

    return top;
}

Token ParserBase::M_eat(int which)
{
    if (M_peek().which() != which)
    {
        std::ostringstream ss;
        ss << "expecting ";
        ss << util::ansi::bold << emph_color;
        ss << M_tokenName(which) << util::ansi::clear;
        ss << ", got " << util::ansi::bold << emph_color;
        ss << M_tokenName(M_peek()) << util::ansi::clear;
        error(M_peek(), ss.str());
    }

    return M_get();
}

Token const& ParserBase::M_peek(std::size_t depth)
{
    if (depth >= m_lookahead_depth)
        throw std::runtime_error("lang::ParserBase::M_peek: peek depth larger than lookahead");

    return m_lookahead[depth];
}

void ParserBase::M_unexpected(Token const& token)
{
    std::ostringstream ss;
    ss << util::ansi::bold << emph_color;
    ss << M_tokenName(token) << util::ansi::clear;
    error(token, "unexpected token " + ss.str());
}
