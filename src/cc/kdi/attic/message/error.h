//---------------------------------------------------------- -*- Mode: C++ -*-
// Copyright (C) 2007 Josh Taylor (Kosmix Corporation)
// Created 2007-11-21
// 
// This file is part of KDI.
// 
// KDI is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or any later version.
// 
// KDI is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//----------------------------------------------------------------------------

#ifndef KDI_MESSAGE_ERROR_H
#define KDI_MESSAGE_ERROR_H

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace kdi {
namespace message {

    /// Base class for a rethrowable exception 
    class Error : private boost::noncopyable
    {
    public:
        virtual ~Error() {}

        /// Rethrow the exception
        virtual void rethrow() const = 0;

        /// Get the message associated with the exception
        virtual char const * what() const = 0;
    };

    /// Class for rethrowing specific exception types
    template <class T>
    class TypedError : public Error
    {
        T ex;
    public:
        explicit TypedError(T const & ex) : ex(ex) {}
        virtual void rethrow() const { throw ex; }
        virtual char const * what() const { return ex.what(); }
    };

    /// Shared pointer to an Error
    typedef boost::shared_ptr<Error const> ErrorPtr;

    /// Wrap an exception in an ErrorPtr for later use
    template <class E>
    ErrorPtr wrapError(E const & ex)
    {
        ErrorPtr p(new TypedError<E>(ex));
        return p;
    }

} // namespace message
} // namespace kdi

#endif // KDI_MESSAGE_ERROR_H
