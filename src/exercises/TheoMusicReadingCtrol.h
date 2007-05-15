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

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __THOEMUSICREADINGCTROL_H__        //to avoid nested includes
#define __THOEMUSICREADINGCTROL_H__

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/combobox.h"

#include "Constrains.h"
#include "../score/Score.h"
#include "ScoreAuxCtrol.h"
#include "UrlAuxCtrol.h"
#include "../sound/SoundEvents.h"
#include "ScoreConstrains.h"


class lmTheoMusicReadingCtrol : public wxWindow    
{
   DECLARE_DYNAMIC_CLASS(lmTheoMusicReadingCtrol)

public:

    // constructor and destructor    
    lmTheoMusicReadingCtrol(wxWindow* parent, wxWindowID id,
               lmMusicReadingCtrolOptions* pOptions,
               lmScoreConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmTheoMusicReadingCtrol();

    // event handlers
    void OnClose(wxCloseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnPlay(wxCommandEvent& event);
    void OnNewProblem(wxCommandEvent& event);
    void OnSettingsButton(wxCommandEvent& event);
    void OnGoBackButton(wxCommandEvent& event);

    // event handlers related to debugging
    void OnDebugShowSourceScore(wxCommandEvent& event);
    void OnDebugDumpScore(wxCommandEvent& event);
    void OnDebugShowMidiEvents(wxCommandEvent& event);

    // event handlers related with playing a score
    void OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event));


private:
    void Play();
    void NewProblem();

        // member variables

    lmScore*                    m_pScore;            // the score to read
    lmScoreAuxCtrol*            m_pScoreCtrol;
    lmScoreConstrains*          m_pConstrains;
    lmMusicReadingCtrolOptions*  m_pOptions;

    bool        m_fProblemCreated;  //there is a problem prepared
    bool        m_fPlayEnabled;     //Play enabled

    lmUrlAuxCtrol*  m_pSettingsLink;

    lmUrlAuxCtrol*  m_pPlayLink;
    bool            m_fPlaying;         //playing
    bool            m_fClosing;         // waiting for play stopped to close the window

    DECLARE_EVENT_TABLE()
};



#endif  // __THOEMUSICREADINGCTROL_H__

