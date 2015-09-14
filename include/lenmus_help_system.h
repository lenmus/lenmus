//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

#ifndef _LENMUS_HELP_SYSTEM_H__
#define _LENMUS_HELP_SYSTEM_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//wxWidgets
class wxExtHelpController;


namespace lenmus
{

//---------------------------------------------------------------------------------------
class HelpSystem
{
protected:
    ApplicationScope& m_appScope;
    wxWindow* m_pParent;
    wxExtHelpController* m_pHelp;

public:
    HelpSystem(wxWindow* pParent, ApplicationScope& appScope);
    ~HelpSystem() {}

    bool initialize();

    bool display_section(int nSect);
    bool display_theme(const wxString& theme);
    bool search_for(const wxString& key);

protected:

};


}   //namespace lenmus

#endif    // _LENMUS_HELP_SYSTEM_H__
