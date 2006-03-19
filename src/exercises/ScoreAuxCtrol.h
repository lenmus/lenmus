// RCS-ID: $Id: ScoreAuxCtrol.h,v 1.4 2006/02/23 19:19:53 cecilios Exp $
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
/*! @file ScoreAuxCtrol.h
    @brief Header file for class lmScoreAuxCtrol
    @ingroup html_controls
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __SCOREAUXCTROL_H__        //to avoid nested includes
#define __SCOREAUXCTROL_H__

#include "../app/Paper.h"
#include "../app/global.h"
#include "../sound/SoundEvents.h"

// ----------------------------------------------------------------------------
// lmScoreAuxCtrol: a control which shows a music score
// ----------------------------------------------------------------------------

enum EScoreStyles {
    eNO_BORDER = 0,
    eSIMPLE_BORDER = wxSIMPLE_BORDER,
};

class lmScoreAuxCtrol : public wxWindow    
{

public:

    // constructor and destructor    
    lmScoreAuxCtrol(wxWindow* parent, wxWindowID id, lmScore* pScore,
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, EScoreStyles style = eNO_BORDER);

    ~lmScoreAuxCtrol();

    // event handlers
    void OnPaint(wxPaintEvent& WXUNUSED(event));
    void OnSize(wxSizeEvent& WXUNUSED(event));
    void OnVisualHighlight(lmScoreHighlightEvent& event);

    void DisplayMessage(wxString sMsg, lmMicrons posMsg, bool fClearScore = true);
    void DisplayScore(lmScore* pScore, bool fClearMessage = true);

    //settings
    void SetMargins(lmMicrons nLeft, lmMicrons nRight, lmMicrons nTop);
    void SetScale(float rScale);
    void SetScore(lmScore* pScore, bool fHidden = false);
    void HideScore(bool fHide);

    //Play commands
    void PlayScore(bool fVisualTracking = lmVISUAL_TRACKING,
                   bool fMarcarCompasPrevio = NO_MARCAR_COMPAS_PREVIO,
                   EPlayMode nPlayMode = ePM_NormalInstrument,
                   long nMM = 0);
    void PlayMeasure(int nMeasure,
                     bool fVisualTracking = lmNO_VISUAL_TRACKING, 
                     EPlayMode nPlayMode = ePM_NormalInstrument,
                     long nMM = 0);
    void Stop();
    void Pause();

    // Debug methods
    void Dump();
    void SourceLDP();
    void SourceXML();
    void DumpMidiEvents();


private:
    void ResizePaper();

        // member variables

    lmScore*        m_pScore;       //the score to display
    bool            m_fHidden;      //the score is hidden, that is, it must not be displayed
                                    //      but can be played back
    double          m_rScale;       //presentation scale (default 1.0)
    lmPaper         m_Paper;        //the lmPaper object to use

    //message management
    wxString        m_sMsg;                    // message to display
    bool            m_fDisplayMessage;        
    lmMicrons       m_yMsg;                    // message position (microns)
    float           m_yScalingFactor;        // pixels per micron

    //paper margins
    float           m_rZoom;            //zooming factor (default 1.0)
    lmMicrons       m_nTopMargin;
    lmMicrons       m_nLeftMargin;
    lmMicrons       m_nRightMargin;

    DECLARE_EVENT_TABLE()
};



#endif  // __SCOREAUXCTROL_H__
