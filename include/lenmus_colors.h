//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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

#ifndef __LENMUS_COLORS_H__
#define __LENMUS_COLORS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//lomse
#include "lomse_basic.h"
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/config.h>


namespace lenmus
{


//---------------------------------------------------------------------------------------
// Current Colors configuration is maintained in this class
class Colors
{
private:
    ApplicationScope& m_appScope;

    //colors for exercises ctrols: buttons for answers
    Color m_oSuccess;
    Color m_oFailure;
    Color m_oNormal;
    Color m_oHighlight;

    //colors for scores
    wxColour m_oVisualTracking;
    wxColour m_oScoreNormal;
    wxColour m_oScoreSelected;
    wxColour m_oCursorColor;
    wxColour m_oGhostObject;

    //HTML controls
    wxColour m_oHtmlLinks;

    //Visual tracking colors
    Color m_oHighlightColor;
    Color m_oTempoLineColor;

public:
    Colors(ApplicationScope& appScope);
    ~Colors();

    void save_user_preferences();

    //colors for buttons
    Color& Success() { return m_oSuccess; }
    Color& Failure() { return m_oFailure; }
    Color& Normal() { return m_oNormal; }
    Color& Highlight() { return m_oHighlight; }

    // colors for scores
    wxColour& VisualTracking() { return m_oVisualTracking; }
    wxColour& ScoreNormal() { return m_oScoreNormal; }
    wxColour& ScoreSelected() { return m_oScoreSelected; }
    wxColour& GhostObject() { return m_oGhostObject; }
    wxColour& CursorColor() { return m_oCursorColor; }

    //HTML controls
    wxColour& HtmlLinks() { return m_oHtmlLinks; }

    //Visual tracking colors
    Color& highlight_color() { return m_oHighlightColor; }
    Color& tempo_line_color() { return m_oTempoLineColor; }
    inline void set_highlight_color(const Color& color) { m_oHighlightColor = color; }
    inline void set_tempo_line_color(const Color& color) { m_oTempoLineColor = color; }

    //helper
    static wxColour to_wx_color(Color& color);
    static Color to_lomse_color(wxColour& color);

private:
    void load_user_preferences();
    wxString pack_color(wxColor& color);
    wxString pack_color(Color& color);
    void unpack_color(wxString& sColor, wxColour& color);
    Color to_lomse_color(wxString& sColor);

};


}   // namespace lenmus

#endif    // __LENMUS_COLORS_H__
