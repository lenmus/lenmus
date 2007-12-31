//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_STATUSBAR_H__        //to avoid nested includes
#define __LM_STATUSBAR_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "StatusBar.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

//different layouts
enum lmEStatusBarLayout
{
    lm_eStatBar_Books =0,
    lm_eStatBar_ScoreEdit,
};


//the application status bar
class lmStatusBar : public wxStatusBar
{
public:
    lmStatusBar(wxFrame* pFrame, lmEStatusBarLayout nType, wxWindowID id);
    virtual ~lmStatusBar();

    //updating fields
    void SetMsgText(const wxString& sText);

    //info
    inline lmEStatusBarLayout GetType() const { return m_nType; }


private:
    wxFrame*            m_pFrame;       //parent frame
    int                 m_nNumFields;
    lmEStatusBarLayout  m_nType;

};


#endif    // __LM_STATUSBAR_H__
