// RCS-ID: $Id: Colors.h,v 1.3 2006/02/23 19:20:28 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file Colors.h
    @brief Header file for class lmColors
    @ingroup configuration
*/
#ifndef __SBCOLORS_H__
#define __SBCOLORS_H__

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

#endif    // __SBCOLORS_H__
