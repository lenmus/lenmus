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

#include "lenmus_colors.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

namespace lenmus
{

//---------------------------------------------------------------------------------------
Colors::Colors(ApplicationScope& appScope)
    : m_appScope(appScope)
{
    load_user_preferences();
}

//---------------------------------------------------------------------------------------
Colors::~Colors()
{
    save_user_preferences();
}

//---------------------------------------------------------------------------------------
void Colors::load_user_preferences()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    wxString sColor;
    wxColour color;

    sColor = pPrefs->Read("/Colors/Exercises/Success", "000,255,031");    //mid-light green
    m_oSuccess = to_lomse_color(sColor);
    sColor = pPrefs->Read("/Colors/Exercises/Failure", "255,128,128" );    //Light red
    m_oFailure = to_lomse_color(sColor);
    sColor = pPrefs->Read("/Colors/Exercises/Normal", "224,224,255" );    //very light blue
    m_oNormal = to_lomse_color(sColor);
    sColor = pPrefs->Read("/Colors/Exercises/Highlight", "255,255,000" );    //yellow
    m_oHighlight = to_lomse_color(sColor);

    //colors for scores
    sColor = pPrefs->Read("/Colors/Scores/Normal", "000,000,000" );    //black
    unpack_color(sColor, m_oScoreNormal);
    sColor = pPrefs->Read("/Colors/Scores/Highlight", "255,000,000" );    //red
    unpack_color(sColor, m_oVisualTracking);
    sColor = pPrefs->Read("/Colors/Scores/Selected", "000,000,255" );    //blue
    unpack_color(sColor, m_oScoreSelected);
    sColor = pPrefs->Read("/Colors/Scores/Cursor", "000,000,255" );    //blue
    unpack_color(sColor, m_oCursorColor);
    sColor = pPrefs->Read("/Colors/Scores/GhostObject", "170,212,255" );    //faint blue
    unpack_color(sColor, m_oGhostObject);

    //HTML controls
    sColor = pPrefs->Read("/Colors/HTML/Links", "000,000,255" );    //blue
    unpack_color(sColor, m_oHtmlLinks);

    //Visual tracking colors
    sColor = pPrefs->Read("/Colors/Playback/Highlight", "255,000,000");   //solid red
    m_oHighlightColor = to_lomse_color(sColor);
    sColor = pPrefs->Read("/Colors/Playback/TempoLine", "000,000,255,128"); //transparent blue
    m_oTempoLineColor = to_lomse_color(sColor);
}

//---------------------------------------------------------------------------------------
void Colors::save_user_preferences()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    //colors for exercises
    pPrefs->Write("/Colors/Exercises/Success", pack_color(m_oSuccess) );
    pPrefs->Write("/Colors/Exercises/Failure", pack_color(m_oFailure) );
    pPrefs->Write("/Colors/Exercises/Normal", pack_color(m_oNormal) );
    pPrefs->Write("/Colors/Exercises/Highlight", pack_color(m_oHighlight) );

    //colors for scores
    pPrefs->Write("/Colors/Scores/Normal", pack_color(m_oScoreNormal) );
    pPrefs->Write("/Colors/Scores/Highlight", pack_color(m_oVisualTracking) );
    pPrefs->Write("/Colors/Scores/Selected", pack_color(m_oScoreSelected) );
    pPrefs->Write("/Colors/Scores/Cursor", pack_color(m_oCursorColor) );
    pPrefs->Write("/Colors/Scores/GhostObject", pack_color(m_oGhostObject) );

    //HTML controls
    pPrefs->Write("/Colors/HTML/Links", pack_color(m_oHtmlLinks) );

    //Visual tracking colors
    pPrefs->Write("/Colors/Playback/Highlight", pack_color(m_oHighlightColor));
    pPrefs->Write("/Colors/Playback/TempoLine", pack_color(m_oTempoLineColor));
}

//---------------------------------------------------------------------------------------
wxString Colors::pack_color(wxColor& color)
{
    int R = (int) color.Red();
    int G = (int) color.Green();
    int B = (int) color.Blue();
    int A = (int) color.Alpha();
    return wxString::Format("%03d,%03d,%03d,%03d", R, G, B, A);
}

//---------------------------------------------------------------------------------------
wxString Colors::pack_color(Color& color)
{
    return wxString::Format("%03d,%03d,%03d,%03d",
                            color.r, color.g, color.b, color.a);
}

//---------------------------------------------------------------------------------------
void Colors::unpack_color(wxString& sColor, wxColour& color)
{
    long R, G, B, A;
    (sColor.Left(3)).ToLong(&R);
    (sColor.Mid(4, 3)).ToLong(&G);
    (sColor.Mid(8, 3)).ToLong(&B);
    if (sColor.length() == 15)
    {
        (sColor.Mid(12, 3)).ToLong(&A);
        color.Set(R, G, B, A);
    }
    color.Set(R, G, B);
}

//---------------------------------------------------------------------------------------
Color Colors::to_lomse_color(wxString& sColor)
{
    long R, G, B, A;
    (sColor.Left(3)).ToLong(&R);
    (sColor.Mid(4, 3)).ToLong(&G);
    (sColor.Mid(8, 3)).ToLong(&B);
    if (sColor.length() == 15)
    {
        (sColor.Mid(12, 3)).ToLong(&A);
        return Color(R,G,B,A);
    }
    return Color(R,G,B);
}

//---------------------------------------------------------------------------------------
wxColour Colors::to_wx_color(Color& color)
{
    return wxColour(color.r, color.g, color.b, color.a);
}

//---------------------------------------------------------------------------------------
Color Colors::to_lomse_color(wxColour& color)
{
    return Color(color.Red(), color.Green(), color.Blue(), color.Alpha());
}


}   //namespace lenmus
