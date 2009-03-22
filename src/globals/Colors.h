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

#ifndef __LM_COLORS_H__
#define __LM_COLORS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Colors.cpp"
#endif


#include <wx/config.h>

class lmColors
{
public:
    lmColors();
    ~lmColors();

    void SaveUserPreferences();

    //Access to colors
    wxColour& Success() { return m_oSuccess; }
    wxColour& Failure() { return m_oFailure; }
    wxColour& Normal() { return m_oNormal; }
    wxColour& ButtonHighlight() { return m_oButtonHighlight; }

    // colors for scores
    wxColour& ScoreHighlight() { return m_oScoreHighlight; }
    wxColour& ScoreNormal() { return m_oScoreNormal; }
    wxColour& ScoreSelected() { return m_oScoreSelected; }
    wxColour& GhostObject() { return m_oGhostObject; }
    wxColour& CursorColor() { return m_oCursorColor; }

    //HTML controls
    wxColour& HtmlLinks() { return m_oHtmlLinks; }

private:
    void LoadUserPreferences();
    wxString PackColor(wxColor* pColor);
    void UnpackColor(wxString sColor, wxColour* pColor);

        //
        // member variables
        //

    //colors for exercises ctrols: buttons for answers
    wxColour m_oSuccess;
    wxColour m_oFailure;
    wxColour m_oNormal;
    wxColour m_oButtonHighlight;

    //colors for scores
    wxColour m_oScoreHighlight;
    wxColour m_oScoreNormal;
    wxColour m_oScoreSelected;
    wxColour m_oCursorColor;
    wxColour m_oGhostObject;

    //HTML controls
    wxColour m_oHtmlLinks;


    
};

#endif    // __LM_COLORS_H__
