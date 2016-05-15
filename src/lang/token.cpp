/*  This file is part of Axolotl.
 *
 * Axolotl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Axolotl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Axolotl.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "lang/token.hpp"

using namespace lang;
using namespace core;

Token::Token(int which, Object const& what)
    : m_which(which)
    , m_what(what)
{}

int Token::which() const
{ return m_which; }

Object const& Token::what() const
{ return m_what; }

Token::Where const& Token::where() const
{ return m_where; }

void Token::setWhere(Token::Where const& where)
{ m_where = where; }

std::string const& Token::lexeme() const
{ return m_lexeme; }

void Token::setLexeme(std::string const& lexeme)
{ m_lexeme = lexeme; }
