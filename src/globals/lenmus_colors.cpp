//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

    sColor = pPrefs->Read(_T("/Colors/Exercises/Success"), _T("000,255,031"));    //mid-light green
    m_oSuccess = to_lomse_color(sColor);
    sColor = pPrefs->Read(_T("/Colors/Exercises/Failure"), _T("255,128,128") );    //Light red
    m_oFailure = to_lomse_color(sColor);
    sColor = pPrefs->Read(_T("/Colors/Exercises/Normal"), _T("224,224,255") );    //very light blue
    m_oNormal = to_lomse_color(sColor);
    sColor = pPrefs->Read(_T("/Colors/Exercises/Highlight"), _T("255,255,000") );    //yellow
    m_oHighlight = to_lomse_color(sColor);

    //colors for scores
    sColor = pPrefs->Read(_T("/Colors/Scores/Normal"), _T("000,000,000") );    //black
    unpack_color(sColor, m_oScoreNormal);
    sColor = pPrefs->Read(_T("/Colors/Scores/Highlight"), _T("255,000,000") );    //red
    unpack_color(sColor, m_oScoreHighlight);
    sColor = pPrefs->Read(_T("/Colors/Scores/Selected"), _T("000,000,255") );    //blue
    unpack_color(sColor, m_oScoreSelected);
    sColor = pPrefs->Read(_T("/Colors/Scores/Cursor"), _T("000,000,255") );    //blue
    unpack_color(sColor, m_oCursorColor);
    sColor = pPrefs->Read(_T("/Colors/Scores/GhostObject"), _T("170,212,255") );    //faint blue
    unpack_color(sColor, m_oGhostObject);

    //HTML controls
    sColor = pPrefs->Read(_T("/Colors/HTML/Links"), _T("000,000,255") );    //blue
    unpack_color(sColor, m_oHtmlLinks);
}

//---------------------------------------------------------------------------------------
void Colors::save_user_preferences()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    //colors for exercises
    pPrefs->Write(_T("/Colors/Exercises/Success"), pack_color(m_oSuccess) );
    pPrefs->Write(_T("/Colors/Exercises/Failure"), pack_color(m_oFailure) );
    pPrefs->Write(_T("/Colors/Exercises/Normal"), pack_color(m_oNormal) );
    pPrefs->Write(_T("/Colors/Exercises/Highlight"), pack_color(m_oHighlight) );

    //colors for scores
    pPrefs->Write(_T("/Colors/Scores/Normal"), pack_color(m_oScoreNormal) );
    pPrefs->Write(_T("/Colors/Scores/Highlight"), pack_color(m_oScoreHighlight) );
    pPrefs->Write(_T("/Colors/Scores/Selected"), pack_color(m_oScoreSelected) );
    pPrefs->Write(_T("/Colors/Scores/Cursor"), pack_color(m_oCursorColor) );
    pPrefs->Write(_T("/Colors/Scores/GhostObject"), pack_color(m_oGhostObject) );

    //HTML controls
    pPrefs->Write(_T("/Colors/HTML/Links"), pack_color(m_oHtmlLinks) );
}

//---------------------------------------------------------------------------------------
wxString Colors::pack_color(wxColor& color)
{
    int R = (int) color.Red();
    int G = (int) color.Green();
    int B = (int) color.Blue();
    return wxString::Format(_T("%03d,%03d,%03d"), R, G, B);
}

//---------------------------------------------------------------------------------------
wxString Colors::pack_color(Color& color)
{
    return wxString::Format(_T("%03d,%03d,%03d"), color.r, color.g, color.b);
}

//---------------------------------------------------------------------------------------
void Colors::unpack_color(wxString& sColor, wxColour& color)
{
    long R, G, B;
    (sColor.Left(3)).ToLong(&R);
    (sColor.Mid(4, 3)).ToLong(&G);
    (sColor.Mid(8, 3)).ToLong(&B);
    color.Set(R, G, B);
}

//---------------------------------------------------------------------------------------
Color Colors::to_lomse_color(wxString& sColor)
{
    long R, G, B;
    (sColor.Left(3)).ToLong(&R);
    (sColor.Mid(4, 3)).ToLong(&G);
    (sColor.Mid(8, 3)).ToLong(&B);
    return Color(R,G,B);
}


}   //namespace lenmus
