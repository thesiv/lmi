// Basic tables.
//
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019 Gregory W. Chicares.
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

#include "pchfile.hpp"

#include "basic_tables.hpp"

#include "actuarial_table.hpp"
#include "data_directory.hpp"
#include "database.hpp"
#include "dbnames.hpp"                  // e_database_key
#include "oecumenic_enumerations.hpp"
#include "product_data.hpp"

#include <string>

namespace
{
std::vector<double> basic_table_rates
    (product_data     const& product
    ,product_database const& database
    ,std::string      const& table_name
    ,e_database_key          table_id
    ,int                     issue_age
    ,int                     years_to_maturity
    )
{
    return actuarial_table_rates
        (AddDataDir(product.datum(table_name))
        ,database.query<int>(table_id)
        ,issue_age
        ,years_to_maturity
        );
}
} // Unnamed namespace

std::vector<double> LMI_SO irc_7702_q
    (product_data     const& product
    ,product_database const& database
    ,int                     issue_age
    ,int                     years_to_maturity
    )
{
    switch(database.query<oenum_7702_q_whence>(DB_Irc7702QWhence))
        {
        case oe_7702_q_builtin:
            {
            throw "Not yet implemented.";
            }
        case oe_7702_q_external_table:
            {
            return basic_table_rates
                (product
                ,database
                ,"Irc7702QFilename"
                ,DB_Irc7702QTable
                ,issue_age
                ,years_to_maturity
                );
            }
        }
    throw "Unreachable--silences a compiler diagnostic.";
}

std::vector<double> LMI_SO irc_7702A_7pp
    (product_data     const& product
    ,product_database const& database
    ,int                     issue_age
    ,int                     years_to_maturity
    )
{
    switch(database.query<oenum_7702_7pp_whence>(DB_SevenPayWhence))
        {
        case oe_7702_7pp_first_principles:
            {
            throw "Not yet implemented (and don't forget rounding).";
            }
        case oe_7702_7pp_from_table:
            {
            return basic_table_rates
                (product
                ,database
                ,"SevenPayFilename"
                ,DB_SevenPayTable
                ,issue_age
                ,years_to_maturity
                );
            }
        }
    throw "Unreachable--silences a compiler diagnostic.";
}

std::vector<double> LMI_SO irc_7702_nsp
    (product_data     const& product
    ,product_database const& database
    ,int                     issue_age
    ,int                     years_to_maturity
    )
{
    switch(database.query<oenum_7702_nsp_whence>(DB_Irc7702NspWhence))
        {
        case oe_7702_nsp_first_principles:
            {
            throw "Not yet implemented (and don't forget rounding).";
            }
        case oe_7702_nsp_from_table:
            {
            return basic_table_rates
                (product
                ,database
                ,"Irc7702NspFilename"
                ,DB_Irc7702NspTable
                ,issue_age
                ,years_to_maturity
                );
            }
        case oe_7702_nsp_reciprocal_cvat_corridor:
            {
            throw "Not yet implemented (and don't forget rounding).";
            }
        }
    throw "Unreachable--silences a compiler diagnostic.";
}

std::vector<double> LMI_SO cvat_corridor_factors
    (product_data     const& product
    ,product_database const& database
    ,int                     issue_age
    ,int                     years_to_maturity
    )
{
    switch(database.query<oenum_7702_corr_whence>(DB_CorridorWhence))
        {
        case oe_7702_corr_first_principles:
            {
            throw "Not yet implemented (and don't forget rounding).";
            }
        case oe_7702_corr_from_table:
            {
            return basic_table_rates
                (product
                ,database
                ,"CvatCorridorFilename"
                ,DB_CorridorTable
                ,issue_age
                ,years_to_maturity
                );
            }
        }
    throw "Unreachable--silences a compiler diagnostic.";
}

std::vector<double> LMI_SO target_premium_rates
    (product_data     const& product
    ,product_database const& database
    ,int                     issue_age
    ,int                     years_to_maturity
    )
{
    switch(database.query<oenum_modal_prem_type>(DB_TgtPremType))
        {
        case oe_monthly_deduction:
            {
            throw "Not yet implemented (and don't forget rounding).";
            }
        case oe_modal_nonmec:
            {
            return irc_7702A_7pp
                (product
                ,database
                ,issue_age
                ,years_to_maturity
                );
            }
        case oe_modal_table:
            {
            return basic_table_rates
                (product
                ,database
                ,"TgtPremFilename"
                ,DB_TgtPremTable
                ,issue_age
                ,years_to_maturity
                );
            }
        }
    throw "Unreachable--silences a compiler diagnostic.";
}