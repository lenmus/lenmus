//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
//-------------------------------------------------------------------------------------

/*! @class lmColors
    @ingroup configuration
    @brief Current Colors configuration is maintained in this class
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Colors.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Colors.h"

// the config object
extern wxConfigBase *g_pPrefs;


//-------------------------------------------------------------------------------------------
// lmColors implementation
//-------------------------------------------------------------------------------------------


lmColors::lmColors()
{
    LoadUserPreferences();
}

lmColors::~lmColors()
{
    SaveUserPreferences();
}

void lmColors::LoadUserPreferences()
{
    //load settings form user congiguration data or default values

    wxString sColor;
    sColor = g_pPrefs->Read(_T("/Colors/Exercises/Success"), _T("000,255,031"));    //mid-light green
    UnpackColor(sColor, &m_oSuccess);
    sColor = g_pPrefs->Read(_T("/Colors/Exercises/Failure"), _T("255,128,128") );    //Light red
    UnpackColor(sColor, &m_oFailure);
    sColor = g_pPrefs->Read(_T("/Colors/Exercises/Normal"), _T("224,224,255") );    //very light blue
    UnpackColor(sColor, &m_oNormal);
    sColor = g_pPrefs->Read(_T("/Colors/Exercises/Highlight"), _T("255,255,000") );    //yellow
    UnpackColor(sColor, &m_oButtonHighlight);

    //colors for scores
    sColor = g_pPrefs->Read(_T("/Colors/Scores/Normal"), _T("000,000,000") );    //black
    UnpackColor(sColor, &m_oScoreNormal);
    sColor = g_pPrefs->Read(_T("/Colors/Scores/Highlight"), _T("255,000,000") );    //red
    UnpackColor(sColor, &m_oScoreHighlight);
    sColor = g_pPrefs->Read(_T("/Colors/Scores/Selected"), _T("000,000,255") );    //blue
    UnpackColor(sColor, &m_oScoreSelected);
    sColor = g_pPrefs->Read(_T("/Colors/Scores/Cursor"), _T("000,000,255") );    //blue
    UnpackColor(sColor, &m_oCursorColor);
    sColor = g_pPrefs->Read(_T("/Colors/Scores/GhostObject"), _T("170,212,255") );    //faint blue
    UnpackColor(sColor, &m_oGhostObject);

    //HTML controls
    sColor = g_pPrefs->Read(_T("/Colors/HTML/Links"), _T("000,000,255") );    //blue
    UnpackColor(sColor, &m_oHtmlLinks);


    //Active=000,000,255
}

void lmColors::SaveUserPreferences()
{
    //save colors settings in user congiguration data

    //colors for exercises
    g_pPrefs->Write(_T("/Colors/Exercises/Success"), PackColor(&m_oSuccess) );   
    g_pPrefs->Write(_T("/Colors/Exercises/Failure"), PackColor(&m_oFailure) );   
    g_pPrefs->Write(_T("/Colors/Exercises/Normal"), PackColor(&m_oNormal) ); 
    g_pPrefs->Write(_T("/Colors/Exercises/Highlight"), PackColor(&m_oButtonHighlight) ); 

    //colors for scores
    g_pPrefs->Write(_T("/Colors/Scores/Normal"), PackColor(&m_oScoreNormal) );   
    g_pPrefs->Write(_T("/Colors/Scores/Highlight"), PackColor(&m_oScoreHighlight) );   
    g_pPrefs->Write(_T("/Colors/Scores/Selected"), PackColor(&m_oScoreSelected) ); 
    g_pPrefs->Write(_T("/Colors/Scores/Cursor"), PackColor(&m_oCursorColor) ); 
    g_pPrefs->Write(_T("/Colors/Scores/GhostObject"), PackColor(&m_oGhostObject) ); 

    //HTML controls
    g_pPrefs->Write(_T("/Colors/HTML/Links"), PackColor(&m_oHtmlLinks) ); 
}

wxString lmColors::PackColor(wxColor* pColor)
{
    int R = (int) pColor->Red();
    int G = (int) pColor->Green();
    int B = (int) pColor->Blue();
    return wxString::Format(_T("%03d,%03d,%03d"), R, G, B);
}

void lmColors::UnpackColor(wxString sColor, wxColour* pColor)
{
    long R, G, B;
    (sColor.Left(3)).ToLong(&R);
    (sColor.Mid(4, 3)).ToLong(&G);
    (sColor.Mid(8, 3)).ToLong(&B);

    //wxLogMessage(wxString::Format(
    //    _T("[lmColors.UnpackColor] sColor=%s, R=%d, G=%d, B=%d"),
    //    sColor, R, G, B) );

    pColor->Set(R, G, B);
}
