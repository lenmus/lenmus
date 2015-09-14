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

#ifndef __LENMUS_OTHER_OPT_PANEL_H__
#define __LENMUS_OTHER_OPT_PANEL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_options_panel.h"


namespace lenmus
{

//---------------------------------------------------------------------------------------
class OtherOptionsPanel : public OptionsPanel
{
public:
    OtherOptionsPanel(wxWindow* parent, ApplicationScope& appScope);
    ~OtherOptionsPanel();
    bool Verify();
    void Apply();

private:
    void CreateControls();

    // controls
	wxPanel*        m_pHeaderPanel;
	wxStaticText*   m_pTxtTitle;
	wxStaticBitmap* m_pBmpIconTitle;
	wxCheckBox*     m_pChkAnswerSounds;
	wxCheckBox*     m_pChkAutoNewProblem;
	wxCheckBox*     m_pChkExperimental;

	//other
	bool    m_fExperimentalEnabled;

};


}   // namespace lenmus

#endif    // __LENMUS_OTHER_OPT_PANEL_H__
