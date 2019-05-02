//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

#ifndef __LENMUS_PLAYBACK_OPT_PANEL_H__
#define __LENMUS_PLAYBACK_OPT_PANEL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_options_panel.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

class wxColourPickerCtrl;


namespace lenmus
{

//---------------------------------------------------------------------------------------
class PlaybackOptPanel : public OptionsPanel
{
public:
    PlaybackOptPanel(wxWindow* parent, ApplicationScope& appScope);
    ~PlaybackOptPanel();
    bool Verify();
    void Apply();

		enum
		{
			k_id_check_highlight = 1000,
			k_id_highlight_color,
			k_id_check_tempo_line,
			k_id_tempo_line_color,
			k_id_tempo_line_width,
		};

		wxPanel* m_pHeaderPanel;
		wxStaticText* m_pTxtTitle;

		wxStaticBitmap* m_pTitleIcon;
		wxCheckBox* m_pChkHighlight;

		wxStaticText* m_staticText2;
		wxColourPickerCtrl* m_colorHighlight;
		wxCheckBox* m_pChkTempoLine;

		wxStaticText* m_staticText21;
		wxColourPickerCtrl* m_colorTempoLine;

		wxStaticText* m_staticText5;
		wxTextCtrl* m_txtLineWidth;
		wxStaticBitmap* m_pBmpLineWidthError;
		wxStaticText* m_pLblSpacer1;
		wxStaticText* m_pTxtLineWidthError;

    //to detect that it is the first time we display an error message
    bool m_fFirstTimeError;

private:
    void create_controls();


};


}   // namespace lenmus

#endif    // __LENMUS_PLAYBACK_OPT_PANEL_H__
