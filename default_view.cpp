// Default MVC View for life-insurance illustrations.
//
// Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Gregory W. Chicares.
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

// $Id$

#ifdef __BORLANDC__
#   include "pchfile.hpp"
#   pragma hdrstop
#endif // __BORLANDC__

#include "default_view.hpp"

#include "configurable_settings.hpp"

DefaultView::DefaultView()
{
}

DefaultView::~DefaultView()
{
}

char const* DefaultView::DoBookControlName() const
{
    return "input_notebook";
}

char const* DefaultView::DoMainDialogName() const
{
    return "dialog_containing_notebook";
}

char const* DefaultView::DoResourceFileName() const
{
    return configurable_settings::instance().skin_filename().c_str();
}

