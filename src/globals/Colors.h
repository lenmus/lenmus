//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LM_COLORS_H__
#define __LM_COLORS_H__

#if defined(__GNUG__) && !defined(__APPLE__)
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

    //HTML controls
    wxColour m_oHtmlLinks;


    
};

#endif    // __LM_COLORS_H__
