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

#include "core/signature.hpp"

using namespace core;

const std::string Signature::AnyTypeName = "*";

Signature::Signature(Signature::TypeList const& arguments, bool returns)
    : m_returns(returns)
    , m_arguments(arguments)
{}

Signature::~Signature()
{}

bool Signature::returns() const
{ return m_returns; }

Signature::TypeList const& Signature::arguments() const
{ return m_arguments; }

bool Signature::match(std::vector<Object> const& args) const
{
    if (args.size() != m_arguments.size())
        return false;

    for (int i = 0; i < (int) args.size(); ++i)
    {
        if (m_arguments[i] == AnyTypeName)
            continue;

        if (args[i].classname() != m_arguments[i])
            return false;
    }

    return true;
}
