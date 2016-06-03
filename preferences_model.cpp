// MVC Model for user preferences.
//
// Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016 Gregory W. Chicares.
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

#include LMI_PCH_HEADER
#ifdef __BORLANDC__
#   pragma hdrstop
#endif // __BORLANDC__

#include "preferences_model.hpp"

#include "alert.hpp"
#include "configurable_settings.hpp"
#include "miscellany.hpp"               // begins_with()
#include "value_cast.hpp"

#include <cstddef>                      // std::size_t
#include <sstream>
#include <vector>

namespace
{
// Empty columns: it would be even better to reimplement this class to
// store the twelve instances of mce_report_column here the same way
// that class 'Input' stores its twelve in its xml file.

std::string const empty_column_name("[none]");

/// Does a member name nominate a calculation-summary column?

bool is_calculation_summary_column_name(std::string const& member_name)
{
    return begins_with(member_name, "CalculationSummaryColumn");
}
} // Unnamed namespace.

PreferencesModel::PreferencesModel()
{
    AscribeMembers();
    Load();
}

PreferencesModel::~PreferencesModel()
{
}

void PreferencesModel::AscribeMembers()
{
    ascribe("CalculationSummaryColumn00"    , &PreferencesModel::CalculationSummaryColumn00    );
    ascribe("CalculationSummaryColumn01"    , &PreferencesModel::CalculationSummaryColumn01    );
    ascribe("CalculationSummaryColumn02"    , &PreferencesModel::CalculationSummaryColumn02    );
    ascribe("CalculationSummaryColumn03"    , &PreferencesModel::CalculationSummaryColumn03    );
    ascribe("CalculationSummaryColumn04"    , &PreferencesModel::CalculationSummaryColumn04    );
    ascribe("CalculationSummaryColumn05"    , &PreferencesModel::CalculationSummaryColumn05    );
    ascribe("CalculationSummaryColumn06"    , &PreferencesModel::CalculationSummaryColumn06    );
    ascribe("CalculationSummaryColumn07"    , &PreferencesModel::CalculationSummaryColumn07    );
    ascribe("CalculationSummaryColumn08"    , &PreferencesModel::CalculationSummaryColumn08    );
    ascribe("CalculationSummaryColumn09"    , &PreferencesModel::CalculationSummaryColumn09    );
    ascribe("CalculationSummaryColumn10"    , &PreferencesModel::CalculationSummaryColumn10    );
    ascribe("CalculationSummaryColumn11"    , &PreferencesModel::CalculationSummaryColumn11    );
    ascribe("DefaultInputFilename"          , &PreferencesModel::DefaultInputFilename          );
    ascribe("PrintDirectory"                , &PreferencesModel::PrintDirectory                );
    ascribe("SecondsToPauseBetweenPrintouts", &PreferencesModel::SecondsToPauseBetweenPrintouts);
    ascribe("SkinFileName"                  , &PreferencesModel::SkinFileName                  );
    ascribe("SpreadsheetFileExtension"      , &PreferencesModel::SpreadsheetFileExtension      );
    ascribe("UseBuiltinCalculationSummary"  , &PreferencesModel::UseBuiltinCalculationSummary  );
}

void PreferencesModel::DoAdaptExternalities()
{
}

datum_base const* PreferencesModel::DoBaseDatumPointer
    (std::string const& name
    ) const
{
    return member_cast<datum_base>(operator[](name));
}

any_entity& PreferencesModel::DoEntity(std::string const& name)
{
    return MemberSymbolTable<PreferencesModel>::operator[](name);
}

any_entity const& PreferencesModel::DoEntity(std::string const& name) const
{
    return MemberSymbolTable<PreferencesModel>::operator[](name);
}

MvcModel::NamesType const& PreferencesModel::DoNames() const
{
    return member_names();
}

MvcModel::StateType PreferencesModel::DoState() const
{
    return member_state(*this);
}

void PreferencesModel::DoCustomizeInitialValues()
{
}

void PreferencesModel::DoEnforceCircumscription(std::string const&)
{
}

void PreferencesModel::DoEnforceProscription(std::string const&)
{
}

void PreferencesModel::DoHarmonize()
{
    SecondsToPauseBetweenPrintouts.minimum_and_maximum(0, 60);

    bool do_not_use_builtin_defaults = "No" == UseBuiltinCalculationSummary;
    CalculationSummaryColumn00.enable(do_not_use_builtin_defaults);
    CalculationSummaryColumn01.enable(do_not_use_builtin_defaults);
    CalculationSummaryColumn02.enable(do_not_use_builtin_defaults);
    CalculationSummaryColumn03.enable(do_not_use_builtin_defaults);
    CalculationSummaryColumn04.enable(do_not_use_builtin_defaults);
    CalculationSummaryColumn05.enable(do_not_use_builtin_defaults);
    CalculationSummaryColumn06.enable(do_not_use_builtin_defaults);
    CalculationSummaryColumn07.enable(do_not_use_builtin_defaults);
    CalculationSummaryColumn08.enable(do_not_use_builtin_defaults);
    CalculationSummaryColumn09.enable(do_not_use_builtin_defaults);
    CalculationSummaryColumn10.enable(do_not_use_builtin_defaults);
    CalculationSummaryColumn11.enable(do_not_use_builtin_defaults);

    // TODO ?? EGREGIOUS_DEFECT: instead, don't offer these columns at all.
    std::vector<mcenum_report_column> weird_report_columns;
    weird_report_columns.push_back(mce_current_0_account_value_general_account    );
    weird_report_columns.push_back(mce_guaranteed_0_account_value_general_account );
    weird_report_columns.push_back(mce_current_0_account_value_separate_account   );
    weird_report_columns.push_back(mce_guaranteed_0_account_value_separate_account);
    weird_report_columns.push_back(mce_current_0_account_value                    );
    weird_report_columns.push_back(mce_guaranteed_0_account_value                 );
    weird_report_columns.push_back(mce_current_0_cash_surrender_value             );
    weird_report_columns.push_back(mce_guaranteed_0_cash_surrender_value          );

    typedef std::vector<mcenum_report_column>::const_iterator vrci;
    for(vrci i = weird_report_columns.begin(); i != weird_report_columns.end(); ++i)
        {
        CalculationSummaryColumn00.allow(*i, false);
        CalculationSummaryColumn01.allow(*i, false);
        CalculationSummaryColumn02.allow(*i, false);
        CalculationSummaryColumn03.allow(*i, false);
        CalculationSummaryColumn04.allow(*i, false);
        CalculationSummaryColumn05.allow(*i, false);
        CalculationSummaryColumn06.allow(*i, false);
        CalculationSummaryColumn07.allow(*i, false);
        CalculationSummaryColumn08.allow(*i, false);
        CalculationSummaryColumn09.allow(*i, false);
        CalculationSummaryColumn10.allow(*i, false);
        CalculationSummaryColumn11.allow(*i, false);
        }
}

void PreferencesModel::DoTransmogrify()
{
}

/// Determine whether any member has been changed.
///
/// Any parse_calculation_summary_columns() diagnostics are repeated
/// when 'unchanged' is constructed, because the ctor calls Load().
/// But Load() must be called in that case, because a copy of *(this)
/// would be identical to itself, frustrating this function's purpose.
///
/// The test that compares column selections as a single string is not
/// superfluous: it serves to detect removal of invalid substrings by
/// parse_calculation_summary_columns().
///
/// This might be renamed operator==(configurable_settings const&),
/// but that doesn't seem clearer.

bool PreferencesModel::IsModified() const
{
    PreferencesModel unchanged;
    configurable_settings const& z = configurable_settings::instance();
    std::vector<std::string>::const_iterator i;
    for(i = member_names().begin(); i != member_names().end(); ++i)
        {
        if(operator[](*i) != unchanged[*i])
            {
            return true;
            }
        }

    if(string_of_column_names() != z.calculation_summary_columns())
        {
        return true;
        }

    return false;
}

void PreferencesModel::Load()
{
    configurable_settings const& z = configurable_settings::instance();

    std::vector<std::string> columns(input_calculation_summary_columns());
    for(std::size_t i = 0; i < member_names().size(); ++i)
        {
        std::string const& name = member_names()[i];
        if(!is_calculation_summary_column_name(name))
            {
            continue;
            }
        if(columns.size() <= i)
            {
            operator[](name) = empty_column_name;
            }
        else
            {
            operator[](name) = columns[i];
            }
        }

    DefaultInputFilename           = z.default_input_filename();
    PrintDirectory                 = z.print_directory();
    SecondsToPauseBetweenPrintouts = z.seconds_to_pause_between_printouts();
    SkinFileName                   = z.skin_filename();
    SpreadsheetFileExtension       = z.spreadsheet_file_extension();
    UseBuiltinCalculationSummary   = z.use_builtin_calculation_summary() ? "Yes" : "No";
}

std::string PreferencesModel::string_of_column_names() const
{
    std::ostringstream oss;
    std::vector<std::string>::const_iterator i;
    for(i = member_names().begin(); i != member_names().end(); ++i)
        {
        if(!is_calculation_summary_column_name(*i))
            {
            continue;
            }
        std::string const column = operator[](*i).str();
        if(column != empty_column_name)
            {
            oss << column << " ";
            }
        }
    return oss.str();
}

void PreferencesModel::Save() const
{
    configurable_settings& z = configurable_settings::instance();

    z["calculation_summary_columns"       ] = string_of_column_names();
    z["default_input_filename"            ] = DefaultInputFilename    .value();
    z["print_directory"                   ] = PrintDirectory          .value();
    z["seconds_to_pause_between_printouts"] = value_cast<std::string>(SecondsToPauseBetweenPrintouts.value());
    z["skin_filename"                     ] = SkinFileName            .value();
    z["spreadsheet_file_extension"        ] = SpreadsheetFileExtension.value();
    z["use_builtin_calculation_summary"   ] = value_cast<std::string>("Yes" == UseBuiltinCalculationSummary);
}

