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

#ifndef __LENMUS_LANG_OPT_PANEL_H__
#define __LENMUS_LANG_OPT_PANEL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_options_panel.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

namespace lenmus
{

//---------------------------------------------------------------------------------------
class LangOptionsPanel : public OptionsPanel
{
public:
    LangOptionsPanel(wxWindow* parent, ApplicationScope& appScope);
    ~LangOptionsPanel();
    bool Verify();
    void Apply();

protected:
    void create_controls();

    // controls
    wxPanel*        m_pTitlePannel;
    wxStaticText*   m_pTitle;
    wxStaticBitmap* m_pTitleIcon;
    wxStaticText*   m_pLang;
    wxChoice*       m_pChoice;

    // other member variables
    int             m_nNumLangs;
    wxArrayString   m_cLangCodes;
    wxArrayString   m_cLangNames;
    wxString        m_sCurLang;
};


}   // namespace lenmus

#endif    // __LENMUS_LANG_OPT_PANEL_H__
