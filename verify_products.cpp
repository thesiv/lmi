// Verify product files.
//
// Copyright (C) 2019 Gregory W. Chicares.
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

#include "verify_products.hpp"

#include "actuarial_table.hpp"
#include "ce_product_name.hpp"
#include "cso_tables.hpp"
#include "data_directory.hpp"           // AddDataDir()
#include "database.hpp"
#include "input.hpp"
#include "mc_enum.hpp"                  // all_strings<>()
#include "product_data.hpp"
#include "ssize_lmi.hpp"
#include "yare_input.hpp"

#include <iostream>
#include <string>
#include <vector>

/// Verify all products.

void verify_products()
{
    std::vector<std::string> const& products = ce_product_name().all_strings();
    for(auto const& z : products)
        {
        std::cout << "Testing product " << z << '\n';
        Input input;
        input["ProductName"] = z;
        for(auto const& g : all_strings<mcenum_gender>())
            {
            input["Gender"] = g;
            for(auto const& s : all_strings<mcenum_smoking>())
                {
                input["Smoking"] = s;
                yare_input yi(input);
                product_database db(yi);
                oenum_cso_era const era = db.query<oenum_cso_era>(DB_CsoEra);
                oenum_alb_or_anb const a_b = db.query<oenum_alb_or_anb>(DB_AgeLastOrNearest);
                int const t = db.query<int>(DB_Irc7702QTable);
                if(0 == t)
                    continue;
                std::vector<double> v0 = cso_table
                    (era
                    ,oe_orthodox
                    ,a_b
                    ,mce_gender (g).value()
                    ,mce_smoking(s).value()
                    );
                product_data p(z);
                std::string const f = AddDataDir(p.datum("Irc7702QFilename"));
                actuarial_table a(f, t);
                std::vector<double> v1 = a.values
                    (a.min_age()
                    ,1 + a.max_age() - a.min_age()
                    );
                if(v0 == v1)
                    {
                    std::cout << "validated: table " << t << ' ' << g << ' ' << s << std::endl;
                    }
                else
                    {
                    std::cout << "PROBLEM: " << z << ' ' << g << ' ' << s << std::endl;
                    std::cout << lmi::ssize(v0) << ' ' << lmi::ssize(v1) << '\n';
                    std::cout
                        << "\n  CSO era: " << era
                        << "\n  ALB or ANB: " << a_b
                        << "\n  table file: " << f
                        << "\n  table number: " << t
                        << "\n  min age: " << a.min_age()
                        << "\n  max age: " << a.max_age()
                        << "\n  length: " << a.max_age() - a.min_age()
                        << "\n  v0.front(): " << v0.front()
                        << "\n  v1.front(): " << v1.front()
                        << "\n  v0.back (): " << v0.back ()
                        << "\n  v1.back (): " << v1.back ()
                        << std::endl
                        ;
                    }
                }
            }
        }
    std::cout << std::endl;
}