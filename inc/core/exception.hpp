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

#ifndef __AXOLOTL_CORE_EXCEPTION_H__
#define __AXOLOTL_CORE_EXCEPTION_H__

#include "core/forward.hpp"
#include "core/object.hpp"
#include "core/class.hpp"
#include "core/some.hpp"
#include "vm/forward.hpp"

#include <vector>
#include <string>
#include <exception>

namespace core
{
    class Exception
    {
    public:
        Exception() = default;
        virtual ~Exception();

        virtual std::string what() const = 0;
    };

    class InternalError : public Exception
    {
    public:
        InternalError(std::string const& desc);
        virtual ~InternalError();

        virtual std::string what() const;

    private:
        std::string m_desc;
    };

    class NoMemberError : public Exception
    {
    public:
        NoMemberError(Object self, std::string const& name);
        virtual ~NoMemberError();

        virtual std::string what() const;

    private:
        Object m_self;
        std::string m_name;
    };

    class ClassError : public Exception
    {
    public:
        ClassError(Object self, Class const& expected);
        virtual ~ClassError();

        virtual std::string what() const;

    private:
        Object m_self;
        Class m_expected;
    };

    class SignatureError : public Exception
    {
    public:
        SignatureError(Object self, std::string const& name, std::vector<Object> const& argv);
        virtual ~SignatureError();

        virtual std::string what() const;

    private:
        Object m_self;
        std::string m_name;
        std::vector<Object> m_argv;
    };

    class NoFileError : public Exception
    {
    public:
        NoFileError(std::string const& filename);
        virtual ~NoFileError();

        virtual std::string what() const;

    private:
        std::string m_filename;
    };

    class NoGlobalError : public Exception
    {
    public:
        NoGlobalError(vm::Module const& module, std::string const& name);
        virtual ~NoGlobalError();

        virtual std::string what() const;

    private:
        std::string m_module_name;
        std::string m_name;
    };
}

#endif // __AXOLOTL_CORE_EXCEPTION_H__
