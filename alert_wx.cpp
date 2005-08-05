// Alert messages for wx interface.
//
// Copyright (C) 2004, 2005 Gregory W. Chicares.
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

// $Id: alert_wx.cpp,v 1.3 2005-08-05 17:02:30 chicares Exp $

// TODO ?? Once this facility is tested, consider using it widely,
// perhaps instead of using wx's logging classes directly at all.

#ifdef __BORLANDC__
#   include "pchfile.hpp"
#   pragma hdrstop
#endif // __BORLANDC__

#include "alert.hpp"

#include "configurable_settings.hpp"

#include <wx/app.h> // wxTheApp
#include <wx/frame.h>
#include <wx/log.h>
#include <wx/msgdlg.h>

#include <stdexcept>

namespace
{
    bool ensure_setup = set_alert_functions
        (status_alert
        ,warning_alert
        ,hobsons_choice_alert
        ,fatal_error_alert
        );

#if 0
    // Alert messages could be lost if they're flushed before wx has
    // initialized its logging facility. Here is an untested idea for
    // a function that could be called prior to each use of that
    // facility in this file.
    void show_safely_if_log_uninitialized(std::string const& s)
        {
        if(0 == wxLog::GetActiveTarget())
            {
            wxSafeShowMessage("Error before GUI initialized", s.c_str());
            }
        }
#endif // 0
} // Unnamed namespace.

void status_alert(std::string const& s)
{
    wxWindow* w = wxTheApp->GetTopWindow();
    wxLogStatus(dynamic_cast<wxFrame*>(w), s.c_str());
    // TODO ?? If no frame with statusbar, consider writing to
    // diagnostics static control on dialog if that exists.
}

// By design, wx buffers warning messages, and even discards them if
// a (more severe) error message occurs later. This is an interesting
// concept, and it prevents flurries of popup messageboxes. But until
// these 'alert' classes are tested with legacy code that assumes a
// different behavior, warnings are explicitly flushed here as soon as
// they occur.
//
// TODO ?? Test and decide whether to adopt that wx behavior.
//
void warning_alert(std::string const& s)
{
    wxLogWarning(s.c_str());
    wxLog::FlushActive();
}

void hobsons_choice_alert(std::string const& s)
{
    if(configurable_settings::instance().offer_hobsons_choice())
        {
        int rc = wxMessageBox
            (s.c_str()
            ,"Stop the current operation and attempt to resume safely?"
            ,wxYES_NO | wxICON_QUESTION
            ,wxTheApp->GetTopWindow()
            );
        if(wxYES == rc)
            {
            throw std::runtime_error(s);
            }
        else
            {
            wxMessageBox
                (s.c_str()
                ,"Warning: the illustration may be invalid."
                ,wxYES_NO | wxICON_QUESTION
                ,wxTheApp->GetTopWindow()
                );
            }
        }
    else
        {
// TODO ?? expunge?        wxLogError(s.c_str());
        throw std::runtime_error(s);
        }
}

void fatal_error_alert(std::string const& s)
{
// TODO ?? expunge?    wxLogError(s.c_str());
    throw std::runtime_error(s);
}

