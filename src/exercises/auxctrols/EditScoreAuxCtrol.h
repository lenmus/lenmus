//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 Cecilio Salmeron
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

#ifndef __LM_EDITSCOREAUXCTROL_H__        //to avoid nested includes
#define __LM_EDITSCOREAUXCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "EditScoreAuxCtrol.cpp"
#endif

#include "../../app/Paper.h"
#include "../../app/global.h"
#include "../../sound/SoundEvents.h"
#include "../../graphic/GraphicManager.h"
#include "wx/window.h"          //window styles
#include "ScoreAuxCtrol.h"

// ----------------------------------------------------------------------------
// lmEditScoreAuxCtrol: a control which shows a music score
// ----------------------------------------------------------------------------

class lmEditScoreAuxCtrol : public wxWindow    
{

public:

    // constructor and destructor    
    lmEditScoreAuxCtrol(wxWindow* parent, wxWindowID id, lmScore* pScore,
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, EScoreStyles style = eNO_BORDER);

    ~lmEditScoreAuxCtrol();

    // event handlers
    void OnPaint(wxPaintEvent& WXUNUSED(event));
    void OnSize(wxSizeEvent& WXUNUSED(event));
    void OnVisualHighlight(lmScoreHighlightEvent& event);

    void DisplayMessage(wxString sMsg, lmLUnits posMsg, bool fClearScore = true);
    void DisplayScore(lmScore* pScore, bool fClearMessage = true);

    //settings
    void SetMargins(lmLUnits nLeft, lmLUnits nRight, lmLUnits nTop);
    void SetScale(float rScale);
    const float GetScale() const { return m_rZoom; }
    void SetScore(lmScore* pScore, bool fHidden = false);
    void HideScore(bool fHide);

    //Play commands
    void PlayScore(bool fVisualTracking = lmVISUAL_TRACKING,
                   bool fCountOff = lmNO_COUNTOFF,
                   lmEPlayMode nPlayMode = ePM_NormalInstrument,
                   long nMM = 0);
    void PlayMeasure(int nMeasure,
                     bool fVisualTracking = lmNO_VISUAL_TRACKING, 
                     lmEPlayMode nPlayMode = ePM_NormalInstrument,
                     long nMM = 0);
    void Stop();
    void Pause();

    // Debug methods
    void Dump();
    void SourceLDP();
    void SourceXML(int nIndent);
    void DumpMidiEvents();

    // info
    double GetPixelsPerLU();


private:
    void ResizePaper();
    void SetBaseScale();
    void DoStopSounds();
    void ComputeScale();

        // member variables

    lmScore*        m_pScore;       //the score to display. Never owned
    bool            m_fHidden;      //the score is hidden, that is, it must not be displayed
                                        //but can be played back
    //scale management
    // m_rBaseScale     Is the scale for 1:1 presentation (real size)
    // m_rScale         Is the scale in use 
    double          m_rBaseScale;
    double          m_rScale;       //presentation scale (default 1.0)
    float           m_rZoom;                //zooming factor (default 1.0)

    //message management
    wxString        m_sMsg;                 // message to display. Line 1
    wxString        m_sMsg2;                // message to display. Line 2
    bool            m_fDisplayMessage;        
    lmLUnits        m_yMsg;                 // message position (microns)
    float           m_yScalingFactor;       // pixels per micron

    //paper
    lmPaper         m_Paper;        //the lmPaper object to use
    lmLUnits        m_nTopMargin;       //paper margins
    lmLUnits        m_nLeftMargin;
    lmLUnits        m_nRightMargin;
    lmLUnits        m_uPaperWidth;
    lmLUnits        m_uPaperHeight;

    // managers
    lmGraphicManager    m_graphMngr;        //rederization manager

    DECLARE_EVENT_TABLE()
};



#endif  // __LM_EDITSCOREAUXCTROL_H__

