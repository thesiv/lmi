// Product database entity names.
//
// Copyright (C) 1998, 1999, 2000, 2001, 2005 Gregory W. Chicares.
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
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// http://savannah.nongnu.org/projects/lmi
// email: <chicares@cox.net>
// snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

// $Id: dbnames.cpp,v 1.1 2005-01-14 19:47:44 chicares Exp $

#ifdef __BORLANDC__
#   include "pchfile.hpp"
#   pragma hdrstop
#endif // __BORLANDC__

#include "dbnames.hpp"

namespace
{
    std::vector<db_names> const& static_get_db_names()
        {
        static int const n = DB_LAST;

        static db_names const static_DBNames[n] =
            {
#           include "dbnames.txt"
            };

        static std::vector<db_names> const v(static_DBNames, static_DBNames + n);
        return v;
        }
}

std::vector<db_names> const& LMI_EXPIMP GetDBNames()
{
    return static_get_db_names();
}

