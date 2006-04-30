// RCS-ID: $Id: ScoreAuxCtrol.cpp,v 1.3 2006/02/23 19:19:53 cecilios Exp $
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
/*! @file ScoreAuxCtrol.cpp
    @brief Implementation file for class lmScoreAuxCtrol
    @ingroup html_controls
*/
#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "../score/score.h"
#include "ScoreAuxCtrol.h"
#include "../app/DlgDebug.h"

/*! @class lmScoreAuxCtrol
    @ingroup html_controls
    @brief A indow on which a music score is rendered.

    ScoreControl is a window on which a music score is drawn. No direct interaction
    with the score is allowed, that is, it is is just a display control.

    It combines the behaviour of classes lmScoreCanvas, lmScoreView and Doc, so that scores
    can be displayed/printed without having to use the doc/view model.
*/


BEGIN_EVENT_TABLE(lmScoreAuxCtrol, wxWindow)
    EVT_PAINT(lmScoreAuxCtrol::OnPaint)
    EVT_SIZE(lmScoreAuxCtrol::OnSize)
    LM_EVT_SCORE_HIGHLIGHT(lmScoreAuxCtrol::OnVisualHighlight)
END_EVENT_TABLE()


lmScoreAuxCtrol::lmScoreAuxCtrol(wxWindow* parent, wxWindowID id, lmScore* pScore,
                           const wxPoint& pos, const wxSize& size, EScoreStyles style) :
    wxWindow(parent, id, pos, size, style)
{
    m_pScore = pScore;
    m_sMsg = wxEmptyString;
    m_fDisplayMessage = false;

    SetMargins(lmToLogicalUnits(10, lmMILLIMETERS),
               lmToLogicalUnits(10, lmMILLIMETERS),
               lmToLogicalUnits(10, lmMILLIMETERS));    //right=1cm, left=1cm, top=1cm
    SetScale(1.0);

    m_fHidden = false;

}

lmScoreAuxCtrol::~lmScoreAuxCtrol()
{
    if (m_pScore) delete m_pScore;
}

void lmScoreAuxCtrol::SetMargins(lmLUnits nLeft, lmLUnits nRight, lmLUnits nTop)
{
    /*
    Margings are absolute, that is, independent of the scale
    */
    m_nTopMargin = nTop;
    m_nLeftMargin = nLeft;
    m_nRightMargin = nRight;

}

void lmScoreAuxCtrol::SetScale(float rScale)
{
    //Scale is the zooming factor. It does not affect to margins
    m_rZoom = rScale;
    m_rScale = rScale * lmSCALE;
    //wxLogMessage(_T("[lmScoreAuxCtrol::SetScale]rScale=%f, lmSCALE=%f"), rScale, lmSCALE);
    ResizePaper();
}

void lmScoreAuxCtrol::ResizePaper()
{
    //adjust paper size to match window size
    wxClientDC dc(this);
    wxCoord xPixels, yPixels;
    dc.GetSize(&xPixels, &yPixels);
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( m_rScale, m_rScale );
    wxCoord xMicrons = dc.DeviceToLogicalXRel(xPixels);
    wxCoord yMicrons = dc.DeviceToLogicalYRel(yPixels);
    m_Paper.SetPageSize(xMicrons, yMicrons);

    //save new DC scaling factor to be used later for message positioning
    m_yScalingFactor =(float)yPixels / (float)yMicrons;

    //adjust margins to maintain absolute margin values
    m_Paper.SetPageTopMargin(m_nTopMargin / m_rZoom);
    m_Paper.SetPageLeftMargin(m_nLeftMargin / m_rZoom);
    m_Paper.SetPageRightMargin(m_nRightMargin / m_rZoom);

    //wxLogMessage(wxString::Format(
    //    _T("[lmScoreAuxCtrol::ResizePaper]Paper size = (%d, %d), m_rScale=%f, scaling factor=%f, margins: left=%d, right=%d, top=%d"),
    //    xMicrons, yMicrons, m_rScale, m_yScalingFactor, (int)(m_nLeftMargin / m_rZoom), (int)(m_nRightMargin / m_rZoom), 
    //    (int)(m_nTopMargin / m_rZoom) ));
}

void lmScoreAuxCtrol::OnSize(wxSizeEvent& WXUNUSED(event))
{
    //The window has been resized. Adjust paper size and scale

    ////DBG ---------------------------------------------------------------------
    //int xPixels, yPixels;
    //wxWindow::GetClientSize(&xPixels, &yPixels);
    //wxLogMessage(wxString::Format(
    //    _T("[lmScoreAuxCtrol::OnSize]New window size = (%d, %d)"), xPixels, yPixels));
    ////END DBG -----------------------------------------------------------------

    ResizePaper();
    Refresh();
}


// Repainting behaviour
void lmScoreAuxCtrol::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    bool fClear = true;

    // In a paint event handler, the application must always create a wxPaintDC object, 
    // even if it is not used. Otherwise, under MS Windows, refreshing for this and 
    // other windows will go wrong.
    wxPaintDC dc(this);

    if (m_pScore && !m_fHidden) {
        // Get size of window
        int dxBitmap, dyBitmap;
        dc.GetSize(&dxBitmap, &dyBitmap);        // size of the DC in pixels

        //wxLogMessage(wxString::Format(
        //    _T("[lmScoreAuxCtrol::OnPaint]dxyBitmap = (%d, %d)"), dxBitmap, dyBitmap));

        // inform the paper that we are going to use it
        m_Paper.Prepare(m_pScore, dxBitmap, dyBitmap, m_rScale);

        // allocate a DC in memory for using the offscreen bitmaps
        wxMemoryDC memoryDC;

        // ask paper for the offscreen bitmap for first page
        int nPage = 1;
        wxBitmap* pPageBitmap = m_Paper.GetOffscreenBitmap(nPage-1);
        wxASSERT(pPageBitmap && pPageBitmap->Ok());
        memoryDC.SelectObject(*pPageBitmap);

        // Copy the page onto the device DC
        dc.Blit(0, 0, dxBitmap, dyBitmap, &memoryDC, 0, 0);

        // deselect the las bitmap
        memoryDC.SelectObject(wxNullBitmap);
        fClear = false;
    }

    if (m_fDisplayMessage) {
        fClear = false;
        dc.DrawText(m_sMsg,
                    lmToLogicalUnits(5, lmMILLIMETERS) * m_yScalingFactor,
                    m_yMsg * m_yScalingFactor);
    }

    if (fClear) {
        dc.Clear();
    }

}

void lmScoreAuxCtrol::DisplayMessage(wxString sMsg, lmLUnits posMsg, bool fClearScore)
{
    if (m_pScore && !m_fHidden && fClearScore) {
        delete m_pScore;
        m_pScore = (lmScore*)NULL;
    }
    m_sMsg = sMsg;
    m_yMsg = posMsg;
    m_fDisplayMessage = true;
    Refresh();
}

void lmScoreAuxCtrol::DisplayScore(lmScore* pScore, bool fClearMessage)
{
    SetScore(pScore, false);        //false: not hidden
    if (fClearMessage) m_fDisplayMessage = false;
    Refresh();
}

void lmScoreAuxCtrol::SetScore(lmScore* pScore, bool fHidden)
{
    if (m_pScore) {
        delete m_pScore;
        m_pScore = (lmScore*)NULL;
    }
    m_fHidden = fHidden;
    m_pScore = pScore;
}

void lmScoreAuxCtrol::HideScore(bool fHide)
{
    m_fHidden = fHide;
    Refresh();
}

void lmScoreAuxCtrol::PlayScore(bool fVisualTracking, bool fMarcarCompasPrevio,
        EPlayMode nPlayMode, long nMM)
{
    if (m_pScore) {
        m_pScore->Play(fVisualTracking && !m_fHidden, fMarcarCompasPrevio, nPlayMode, nMM, this);
    }
}

void lmScoreAuxCtrol::PlayMeasure(int nMeasure, bool fVisualTracking, EPlayMode nPlayMode,
        long nMM)
{
    if (m_pScore) {
        m_pScore->PlayMeasure(nMeasure, fVisualTracking && !m_fHidden, nPlayMode, nMM, this);
    }
}

void lmScoreAuxCtrol::Stop()
{
    if (m_pScore) {
        m_pScore->Stop();
    }
}

void lmScoreAuxCtrol::Pause()
{
    if (m_pScore) {
        m_pScore->Pause();
    }
}

void lmScoreAuxCtrol::OnVisualHighlight(lmScoreHighlightEvent& event)
{
    if (!m_pScore) return;

    //prepare paper DC
    wxClientDC dc(this);
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( m_rScale, m_rScale );
    m_Paper.SetDC(&dc);

    /*! @todo
        Position DC origing according to current scrolling and page position
        For now it is assumed that all score is displayed
    */

    //do the highlight / unhighlight
    lmStaffObj* pSO = event.GetStaffObj();
    EHighlightType nHighlightType = event.GetHighlightType();
    m_pScore->ScoreHighlight(pSO, &m_Paper, nHighlightType);

}


    //
    // Debug methods
    //

void lmScoreAuxCtrol::Dump()
{
    if (!m_pScore) return;
    lmDlgDebug dlg(this, _T("lmStaff objects dump"), m_pScore->Dump());
    dlg.ShowModal();

}

void lmScoreAuxCtrol::SourceLDP()
{
    if (!m_pScore) return;
    lmDlgDebug dlg(this, _T("Generated source code"), m_pScore->SourceLDP());
    dlg.ShowModal();

}

void lmScoreAuxCtrol::SourceXML()
{
    if (!m_pScore) return;
    lmDlgDebug dlg(this, _T("Generated MusicXML code"), m_pScore->SourceXML());
    dlg.ShowModal();

}

void lmScoreAuxCtrol::DumpMidiEvents()
{
    if (!m_pScore) return;
    lmDlgDebug dlg(this, _T("MIDI events table"), m_pScore->DumpMidiEvents() );
    dlg.ShowModal();

}
