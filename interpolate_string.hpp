// Interpolate string containing embedded variable references.
//
// Copyright (C) 2017 Gregory W. Chicares.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
//
// http://savannah.nongnu.org/projects/lmi
// email: <gchicares@sbcglobal.net>
// snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

#ifndef interpolate_string_hpp
#define interpolate_string_hpp

#include "config.hpp"

#include <functional>
#include <string>

/// Interpolate string containing embedded variable references.
///
/// Return the input string after replacing all ${variable} references in it
/// with the value of the variable as returned by the provided function.
///
/// To allow embedding literal "${" fragment into the returned string, create a
/// pseudo-variable returning these characters as its expansion, there is no
/// built-in way to escape these characters.
///
/// Throw if the lookup function throws or if the string uses invalid syntax.
std::string interpolate_string
    (char const* s
    ,std::function<std::string (std::string const&)> const& lookup
    );

#endif // interpolate_string_hpp
