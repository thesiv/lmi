// Test files for consistency with various rules.
//
// Copyright (C) 2006, 2007 Gregory W. Chicares.
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
// email: <chicares@cox.net>
// snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

// $Id: test_coding_rules.cpp,v 1.33 2007-12-31 16:20:11 chicares Exp $

#ifdef __BORLANDC__
#   include "pchfile.hpp"
#   pragma hdrstop
#endif // __BORLANDC__

#include "assert_lmi.hpp"
#include "handle_exceptions.hpp"
#include "istream_to_string.hpp"
#include "main_common.hpp"
#include "obstruct_slicing.hpp"

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>
#include <boost/utility.hpp>

#include <ctime>
#include <ios>
#include <iostream>
#include <map>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

std::map<std::string, bool> my_taboos();

// Open predefined standard streams in binary mode.
//
// There is no portable way to do this. Of course, it doesn't matter
// on *nix anyway.
//
// SOMEDAY !! Consider moving this to 'main_common.cpp'. The issue is
// that there are two behaviors:
//   (1) open all files in binary mode
//   (2) open predefined streams in binary mode before main()
// and we want only (2) and not (1), but MinGW doesn't permit that.
// For portable correctness, we should take care to open all streams
// in binary mode; but if we fail to do so, then (1) masks the defect.
// Yet MinGW offers only
//   (a) '_fmode'     --> (1) only
//   (b) '_CRT_fmode' --> both (1) and (2)
// and not [(2) and not (1)]. It is not sufficient to override
// '_fmode' on the first line of main() because non-local objects can
// be constructed before main() is called.

#if defined __MINGW32__
#   include <fcntl.h> // _O_BINARY
    int _CRT_fmode = _O_BINARY;
#endif // defined __MINGW32__

class file
    :private boost::noncopyable
    ,virtual private obstruct_slicing<file>
{
  public:
    explicit file(std::string const& file_path);
    ~file() {}

    fs::path    const& path     () const {return path_;     }
    std::string const& full_name() const {return full_name_;}
    std::string const& leaf_name() const {return leaf_name_;}
    std::string const& extension() const {return extension_;}
    std::string const& data     () const {return data_;     }

  private:
    fs::path    path_;
    std::string full_name_;
    std::string leaf_name_;
    std::string extension_;
    std::string data_;
};

/// Read file contents into a string.
///
/// Require a '\n' at the end of every file, extending the C++98
/// [2.1/1/2] requirement to all files as an lmi standard. C++98 makes
/// an exception for empty files, but there's no reason for lmi to
/// have any.
///
/// Add a '\n' sentry at the beginning of the string for the reason
/// explained in 'regex_test.cpp'.

file::file(std::string const& file_path)
    :path_     (file_path)
    ,full_name_(file_path)
    ,leaf_name_(path_.leaf())
    ,extension_(fs::extension(path_))
{
    if(!fs::exists(path_))
        {
        throw std::runtime_error("File not found.");
        }

    if(fs::is_directory(path_))
        {
        throw std::runtime_error("Argument is a directory.");
        }

    fs::ifstream ifs(path_, std::ios_base::binary);
    istream_to_string(ifs, data_);
    if(!ifs)
        {
        throw std::runtime_error("Failure in file input stream.");
        }

    data_ = '\n' + data_;
    if(0 == data_.size() || '\n' != data_.at(data_.size() - 1))
        {
        throw std::runtime_error("File does not end in '\\n'.");
        }
}

void complain(file const& f, std::string const& complaint)
{
    std::cout << "File '" << f.full_name() << "' " << complaint << '\n';
}

void require
    (file const&        f
    ,std::string const& regex
    ,std::string const& complaint
    )
{
    if(!boost::regex_search(f.data(), boost::regex(regex)))
        {
        complain(f, complaint);
        }
}

void forbid
    (file const&        f
    ,std::string const& regex
    ,std::string const& complaint
    )
{
    if(boost::regex_search(f.data(), boost::regex(regex)))
        {
        complain(f, complaint);
        }
}

void taboo
    (file const&             f
    ,std::string const&      regex
    ,boost::regex::flag_type flags = boost::regex::ECMAScript
    )
{
    boost::regex::flag_type syntax = flags | boost::regex::ECMAScript;
    if(boost::regex_search(f.data(), boost::regex(regex, syntax)))
        {
        std::ostringstream oss;
        oss << "breaks taboo '" << regex << "'.";
        complain(f, oss.str());
        }
}

/// 'config.hpp' must be included exactly when and as required.
///
/// Except as noted below, it must be included in every header, but in
/// no other file. Where required, the include directive must take the
/// canonical form '#include "unutterable"' (its name can't be uttered
/// here because this file is not a header) on a line by itself, and
/// must precede all other include directives.
///
/// Exceptions are necessarily made for
///  - 'config*.hpp' headers;
///  - this facility's test script; and
///  - 'ChangeLog' and its kin, whose names conveniently enough
///    contain capitals by convention.

void check_config_hpp(file const& f)
{
    static std::string const loose ("# *include *[<\"]config.hpp[>\"]");
    static std::string const strict("\\n(#include \"config.hpp\")\\n");

    if(".hpp" == f.extension() || ".h" == f.extension())
        {
        if("config" == f.leaf_name().substr(0, 6))
            {
            return;
            }

        require(f, loose , "must include 'config.hpp'.");
        require(f, strict, "lacks line '#include \"config.hpp\"'.");
        boost::smatch match;
        static boost::regex const first_include("(# *include[^\\n]*)");
        boost::regex_search(f.data(), match, first_include);
        if("#include \"config.hpp\"" != match[1])
            {
            complain(f, "must include 'config.hpp' first.");
            }
        }
    else
        {
        static std::string const AtoZ("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        if
            (   std::string::npos != f.leaf_name().find_first_of(AtoZ)
            ||  "test_coding_rules_test.sh" == f.leaf_name()
            )
            {
            return;
            }

        forbid(f, loose, "must not include 'config.hpp'.");
        }
}

// SOMEDAY !! This test could be liberalized to permit copyright
// notices to span multiple lines.

// SOMEDAY !! Move the 'GNUmakefile' logic to exclude certain other
// files hither.

void check_copyright(file const& f)
{
    if(".xpm" == f.extension())
        {
        return;
        }

    std::time_t const t0 = fs::last_write_time(f.path());
    std::tm const*const t1 = std::gmtime(&t0);
    LMI_ASSERT(NULL != t1);
    std::ostringstream oss;
    oss << "Copyright \\(C\\)[^\\n]*" << 1900 + t1->tm_year;
    require(f, oss.str(), "lacks current copyright.");
}

/// Check defect markers, which contain a doubled '!' or '?'.
///
/// SOMEDAY !! Replace doubled '!' with something else. In retrospect,
/// doubled '!' was a poor choice, because '!!' is a legitimate idiom
/// for conversion to bool. Perhaps tripling both '!' and '?' would be
/// best.

void check_defect_markers(file const& f)
{
    if(".xpm" == f.extension() || "test_coding_rules_test.sh" == f.leaf_name())
        {
        return;
        }

    {
    static boost::regex const r("(\\b\\w+\\b\\W*)\\?\\?(.)");
    boost::sregex_iterator i(f.data().begin(), f.data().end(), r);
    boost::sregex_iterator omega;
    for(; i != omega; ++i)
        {
        boost::smatch const& z(*i);
        bool const error_preceding = "TODO " != z[1];
        bool const error_following = " " != z[2] && "\n" != z[2];
        if(error_preceding || error_following)
            {
            std::ostringstream oss;
            oss << "has irregular defect marker '" << z[0] << "'.";
            complain(f, oss.str());
            }
        }
    }

    {
    static boost::regex const r("(\\b\\w+\\b\\W?)!!(.)");
    boost::sregex_iterator i(f.data().begin(), f.data().end(), r);
    boost::sregex_iterator omega;
    for(; i != omega; ++i)
        {
        boost::smatch const& z(*i);
        bool const error_preceding =
                true
            &&  "APACHE "      != z[1]
            &&  "BOOST "       != z[1]
            &&  "COMPILER "    != z[1]
            &&  "CYGWIN "      != z[1]
            &&  "DATABASE "    != z[1]
            &&  "ET "          != z[1]
            &&  "EVGENIY "     != z[1]
            &&  "INELEGANT "   != z[1]
            &&  "INPUT "       != z[1]
            &&  "MPATROL "     != z[1]
            &&  "MSYS "        != z[1]
            &&  "PORT "        != z[1]
            &&  "SOMEDAY "     != z[1]
            &&  "THIRD_PARTY " != z[1]
            &&  "TRICKY "      != z[1]
            &&  "USER "        != z[1]
            &&  "WX "          != z[1]
            &&  "XMLWRAPP "    != z[1]
            ;
        bool const error_following = " " != z[2] && "\n" != z[2];
        if(error_preceding || error_following)
            {
            std::ostringstream oss;
            oss << "has irregular defect marker '" << z[0] << "'.";
            complain(f, oss.str());
            }
        }
    }
}

void check_include_guards(file const& f)
{
    if(".hpp" != f.extension())
        {
        return;
        }

    std::string guard = f.leaf_name();
    std::string::size_type position = guard.find('.');
    while(position != std::string::npos)
        {
        guard.replace(position, 1, std::string("_"));
        position = guard.find('.', 1 + position);
        }

    if
        (   std::string::npos == f.data().find("\n#ifndef "   + guard + "\n")
        ||  std::string::npos == f.data().find("\n#define "   + guard + "\n")
        ||  std::string::npos == f.data().find("\n#endif // " + guard + "\n")
        )
        {
        complain(f, "has noncanonical header guards.");
        }
}

void check_label_indentation(file const& f)
{
    // Remove this once these hopeless files have been expunged.
    if("ihs_fpios.hpp" == f.leaf_name() || "ihs_pios.hpp" == f.leaf_name())
        {
        return;
        }

    bool const is_c_source = ".c" == f.extension() || ".h" == f.extension();
    bool const is_cxx_source =
            4 == f.extension().size()
        &&  "pp" == f.extension().substr(2, 2)
        ;
    if(!(is_c_source || is_cxx_source))
        {
        return;
        }

    static boost::regex const r("\\n( *)([A-Za-z][A-Za-z0-9_]*)( *:)(?!:)");
    boost::sregex_iterator i(f.data().begin(), f.data().end(), r);
    boost::sregex_iterator omega;
    for(; i != omega; ++i)
        {
        boost::smatch const& z(*i);
        if
            (   "default" != z[2]
            &&  "  "      != z[1]
            &&  "      "  != z[1]
            )
            {
            std::ostringstream oss;
            oss << "has misindented label '" << z[1] << z[2] << z[3] << "'.";
            complain(f, oss.str());
            }
        }
}

/// Check boilerplate at the beginning of each file.
///
/// Strings that would otherwise be subject to RCS keyword
/// substitution are split across multiple lines.

void check_preamble(file const& f)
{
    if(".xpm" == f.extension())
        {
        return;
        }

    static std::string const url("http://savannah.nongnu.org/projects/lmi");
    require(f, url, "lacks lmi URL.");

    static std::string const bad_rcs_id =
        "\\$"
        "Id[^\\$\\n]*\\n"
        ;
    forbid(f, bad_rcs_id, "contains a malformed RCS Id.");
}

void check_xpm(file const& f)
{
    if(".xpm" != f.extension())
        {
        return;
        }

    if(std::string::npos == f.data().find("\nstatic char const*"))
        {
        complain(f, "lacks /^static char const\\*/.");
        }
}

void enforce_taboos(file const& f)
{
    if(std::string::npos != f.leaf_name().find("test_coding_rules"))
        {
        return;
        }

    // ASCII copyright symbol requires upper-case 'C'.
    taboo(f, "\\(c\\) *[0-9]");
    // Former addresses of the Free Software Foundation.
    taboo(f, "Cambridge");
    taboo(f, "Temple");
    // Obsolete email address.
    taboo(f, "chicares@mindspring.com");
    // Certain proprietary libraries.
    taboo(f, "\\bowl\\b", boost::regex::icase);
    taboo(f, "vtss", boost::regex::icase);
    // Suspiciously specific to msw.
    taboo(f, "Microsoft");
    taboo(f, "Visual [A-Z]");
    taboo(f, "\\bWIN\\b");
    taboo(f, "\\bBAT\\b", boost::regex::icase);
    taboo(f, "\\bExcel\\b");
    taboo(f, "xls|xl4", boost::regex::icase);
    // Insinuated by certain msw tools.
    taboo(f, "Microsoft Word");
    taboo(f, "Stylus Studio");
    taboo(f, "Sonic Software");
    taboo(f, "windows-1252");
    taboo(f, "Arial");
    // Unspeakable private taboos.
    std::map<std::string, bool> const z = my_taboos();
    typedef std::map<std::string, bool>::const_iterator mci;
    for(mci i = z.begin(); i != z.end(); ++i)
        {
        boost::regex::flag_type syntax =
            i->second
            ? boost::regex::ECMAScript | boost::regex::icase
            : boost::regex::ECMAScript
            ;
        taboo(f, i->first, syntax);
        }
}

void process_file(std::string const& file_path)
{
    file f(file_path);

    if(std::string::npos != f.data().find('\r'))
        {
        complain(f, "contains '\\r'.");
        }

    if(std::string::npos != f.data().find("\n\n\n"))
        {
        complain(f, "contains '\\n\\n\\n'.");
        }

    if(std::string::npos != f.data().find(" \n"))
        {
        complain(f, "contains ' \\n'.");
        }

    check_config_hpp        (f);
    check_copyright         (f);
    check_defect_markers    (f);
    check_include_guards    (f);
    check_label_indentation (f);
    check_preamble          (f);
    check_xpm               (f);

    enforce_taboos          (f);
}

int try_main(int argc, char* argv[])
{
    bool error_flag = false;
    for(int j = 1; j < argc; ++j)
        {
        try
            {
            process_file(argv[j]);
            }
        catch(...)
            {
            error_flag = true;
            std::cerr << "Exception--file '" << argv[j] << "': " << std::flush;
            report_exception();
            }
        }
    return error_flag ? EXIT_FAILURE : EXIT_SUCCESS;
}

