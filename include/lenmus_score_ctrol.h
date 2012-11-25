//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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

#ifndef __LENMUS_SCORE_CTROL_H__        //to avoid nested includes
#define __LENMUS_SCORE_CTROL_H__

//#include "lenmus_standard_header.h"

//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma interface "ScoreCtrol.cpp"
//#endif
//
////wxWidgets
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#ifndef WX_PRECOMP
//#include <wx/wx.h>
//#endif
//
//#include "Constrains.h"
//#include "../score/Score.h"
//#include "auxctrols/ScoreAuxCtrol.h"
//#include "auxctrols/UrlAuxCtrol.h"
//#include "../sound/SoundEvents.h"
//#include "ExerciseCtrol.h"
//
//
//class ScoreCtrol : public EBookCtrol
//{
//   DECLARE_DYNAMIC_CLASS(ScoreCtrol)
//
//public:
//
//    // constructor and destructor
//    ScoreCtrol(wxWindow* parent, wxWindowID id, ImoScore* pScore,
//              ScoreCtrolOptions* pOptions,
//               const wxPoint& pos = wxDefaultPosition,
//               const wxSize& size = wxDefaultSize, int style = 0);
//
//    ~ScoreCtrol();
//
//    // event handlers
//    void OnSolfa(wxCommandEvent& event);
//    void OnPlayMeasure(wxCommandEvent& event);
//
//    // event handlers related to debugging
//    void OnDebugShowSourceScore(wxCommandEvent& event);
//    void OnDebugDumpScore(wxCommandEvent& event);
//    void OnDebugShowMidiEvents(wxCommandEvent& event);
//
//    // event handlers related with playing a score
//    void OnEndOfPlay(lmEndOfPlayEvent& event);
//
//protected:
//    //implementation of virtual pure methods
//    void InitializeStrings() {}
//    wxDialog* GetSettingsDlg() { return (wxDialog*)NULL; }
//    void Play() { DoPlay(ePM_NormalInstrument, m_pPlayLink); }
//    void StopSounds();
//    void OnSettingsChanged() {}
//    void CreateControls();
//    void SetButtons(wxButton* pButton[], int nNumButtons, int nIdFirstButton) {}
//
//private:
//    void DoPlay(lmEPlayMode nPlayMode, lmUrlAuxCtrol* pLink, int nMeasure=0);
//
//        // member variables
//
//    ScoreAuxCtrol*        m_pScoreCtrol;
//    ScoreCtrolOptions*    m_pOptions;
//    ImoScore*                m_pScore;       //the score to display
//    wxSize                  m_nScoreSize;
//
//    //to control play back
//    lmUrlAuxCtrol*      m_CurPlayLink;      // link that is being played back
//    bool                m_fPlaying;         ///< the score is being played back
//
//    //controls
//    lmUrlAuxCtrol*      m_pPlayLink;
//    lmUrlAuxCtrol*      m_pSolfaLink;
//    lmUrlAuxCtrol*      m_pMeasureLink[10];
//    /*! @limit Max number of 'Play measure' links is limited to ten. */
//
//    DECLARE_EVENT_TABLE()
//};



#endif  // __LENMUS_SCORE_CTROL_H__
