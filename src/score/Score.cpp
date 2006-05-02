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
/*! @file Score.cpp
    @brief Implementation file for class lmScore
    @ingroup score_kernel
*/
//--------------------------------------------------------------------------------------------------
/*! @class lmScore
    @ingroup score_kernel

    A score is a collection of instruments (\<parts\> in MusicXML).
    An instrument is, normally, one VStaff, but more VStaves are posible (for what?)
    And an VStaff is a collection of StaffObjs.

*/
//--------------------------------------------------------------------------------------------------

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

#include "wx/debug.h"
#include "wx/list.h"
#include "Score.h"
#include "../app/global.h"

// global unique variables used during score building
lmNoteRest*    g_pLastNoteRest;
lmBeam*        g_pCurBeam;            // lmBeam object that is being built ot NULL if none in process. See lmNote constructor

// access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//to give a unique ID to each score
static long m_nCounterID = 0;

//---------------------------------------------------------------------------------------
// lmScore constructors and destructor
//---------------------------------------------------------------------------------------

lmScore::lmScore()
{
    //Set up an empty score, that is, without any lmInstrument.

    m_nID = ++m_nCounterID;
    
    m_pTitle = (lmText*)NULL;        //no title
    m_pSubtitle = (lmText*)NULL;    //no subtitle

    //initializations
    m_pSoundMngr = (lmSoundManager*)NULL;

    //! @todo fill this not with constants
    m_nSystemsDistance = lmToLogicalUnits(2, lmCENTIMETERS);    // 2 cm
    m_nTopSystemDistance = lmToLogicalUnits(2, lmCENTIMETERS);    // 2 cm
    m_nSystemsLeftMargin = 0;
    m_nSystemsRightMargin = 0;

}

lmScore::~lmScore()
{
    //wxLogMessage(_T("[lmScore::~lmScore] Deleting lmScore object"));
    m_cInstruments.DeleteContents(true);
    m_cInstruments.Clear();

    m_cGlobalStaffobjs.DeleteContents(true);
    m_cGlobalStaffobjs.Clear();

    if (m_pSoundMngr) {
        m_pSoundMngr->DeleteEventsTable();
        delete m_pSoundMngr;
    }

    m_cHighlighted.DeleteContents(false);    //Staffobjs must not be deleted, only the list
    m_cHighlighted.Clear();

}

//---------------------------------------------------------------------------------------
// score object methods
//---------------------------------------------------------------------------------------

void lmScore::SetTitle(wxString title)
{
    if (!m_pTitle) {
        m_pTitle = new lmText(this, title, 0, 0, false, false, _T("Garamond"), 14, true, false );
        IncludeInGlobalList(m_pTitle);
    }
    else {
        m_pTitle->SetText(title);
    }
}

void lmScore::SetSubtitle(wxString subtitle)
{
    if (!m_pSubtitle) {
        m_pSubtitle = new lmText(this, subtitle, 0, 0, false, false, _T("Garamond"), 11, false, true );
        IncludeInGlobalList(m_pSubtitle);
    }
    else {
        m_pSubtitle->SetText(subtitle);
    }
}

wxInt32 lmScore::GetNumMeasures()
{
    //! @limit it is being assumed that all instruments and staves have the same number of bars
    InstrumentsList::Node *node = m_cInstruments.GetFirst();
    lmInstrument *pInstr = node->GetData();
    lmVStaff *pStaff = pInstr->GetVStaff(1);
    return(pStaff->GetNumMeasures());
}

lmInstrument* lmScore::AddInstrument(wxInt32 nVStaves, wxInt32 nMIDIChannel, wxInt32 nMIDIInstr)
{
    //add an lmInstrument with nVStaves (1..m) empty VStaves.
    //nMIDIChannel is the MIDI channel to use for playing this instrument
    lmInstrument* pInstr = new lmInstrument(this, nVStaves, nMIDIChannel, nMIDIInstr);
    m_cInstruments.Append(pInstr);
    return pInstr;
    
}

//returns lmVStaff number nVStaff (1..n), of lmInstrument nInstr (1..m)
lmVStaff* lmScore::GetVStaff(wxInt32 nInstr, wxInt32 nVStaff)
{
    wxInt32 i;
    InstrumentsList::Node *node;
    lmInstrument *pInstr;
    //iterate over the list to locate lmInstrument nInstr
    for (i=1, node = m_cInstruments.GetFirst(); 
        node && i < nInstr; node = node->GetNext(), i++ ) {}
//    wxASSERT_MSG{pInstr != 0, _T("No existe el Instrumento num. nInstr"));
    pInstr = node->GetData();
    return(pInstr->GetVStaff(nVStaff));
}

lmInstrument* lmScore::XML_FindInstrument(wxString sId)
{
    // iterate over instrument list to find the one with id == sId
    wxInstrumentsListNode *node;
    lmInstrument* pInstr = (lmInstrument*)NULL;
    for (node = m_cInstruments.GetFirst(); node; node = node->GetNext()) {
        pInstr = (lmInstrument*)node->GetData();
        if (pInstr->XML_GetId() == sId) break;
    }
    return pInstr;
}

void lmScore::WriteTitles(bool fMeasuring, lmPaper *pPaper)
{
    long nWidth, nHeight;
    lmLUnits xPos=0;

    if (fMeasuring) {
        // Measurement phase ---------------------------------------------------

        m_nHeadersHeight = 0;

        // measure Title if exits
        if (m_pTitle) {
            // center Title
            if (!m_pTitle->IsFixed()) {
                m_pTitle->Draw(DO_MEASURE, pPaper);
                nWidth = m_pTitle->GetSelRect().width;
                nHeight = m_pTitle->GetSelRect().height;
                xPos = (pPaper->GetRightMarginXPos() - pPaper->GetLeftMarginXPos() - nWidth)/2;
                pPaper->SetCursorX(pPaper->GetLeftMarginXPos() + xPos);
            }
            m_pTitle->Draw(DO_MEASURE, pPaper);
            m_pTitle->SetFixed(true);
            nWidth = m_pTitle->GetSelRect().width;
            nHeight = m_pTitle->GetSelRect().height;

            // advance paper and update headers total height
            pPaper->IncrementCursorY(nHeight);
            m_nHeadersHeight += nHeight;
        }

        // measure Subtitle if exits
        if (m_pSubtitle) {
            // center subtitle
            if (!m_pSubtitle->IsFixed()) {
                m_pSubtitle->Draw(DO_MEASURE, pPaper);
                nWidth = m_pSubtitle->GetSelRect().width;
                nHeight = m_pSubtitle->GetSelRect().height;
                xPos = (pPaper->GetRightMarginXPos() - pPaper->GetLeftMarginXPos() - nWidth)/2;
                pPaper->SetCursorX(pPaper->GetLeftMarginXPos() + xPos);
            }

            //render subtitle
            m_pSubtitle->Draw(DO_MEASURE, pPaper);
            m_pSubtitle->SetFixed(true);
            nWidth = m_pSubtitle->GetSelRect().width;
            nHeight = m_pSubtitle->GetSelRect().height;

            //update headers total height
            m_nHeadersHeight += nHeight;
        }

    }

    else {
        // Drawing phase ---------------------------------------------------
        if (m_pTitle) m_pTitle->Draw(DO_DRAW, pPaper);
        if (m_pSubtitle) m_pSubtitle->Draw(DO_DRAW, pPaper);
    }

}

void lmScore::IncludeInGlobalList(lmStaffObj* pSO)
{
    m_cGlobalStaffobjs.Append(pSO);
}

void lmScore::RemoveFromGlobalList(lmStaffObj* pSO)
{
    m_cGlobalStaffobjs.DeleteObject(pSO);
}

//=========================================================================================
// Methods for finding StaffObjs
//=========================================================================================

lmScoreObj* lmScore::FindSelectableObject(wxPoint& pt)
{
    wxInt32 iVStaff;
    lmInstrument *pInstr;
    lmVStaff *pStaff;
    InstrumentsList::Node *node;
    lmScoreObj* pScO;

    //Look up in the global StaffObjs list
    wxStaffObjsListNode* pNode;
    for(pNode = m_cGlobalStaffobjs.GetFirst(); pNode; pNode = pNode->GetNext()) {
        pScO = (lmScoreObj*)pNode->GetData();
        if (pScO->IsAtPoint(pt)) {
            return pScO;
        }
    }

    //Not found in global list. Look up in the VStaffs' lists

    //for each instrument
    for (node = m_cInstruments.GetFirst(); node; node=node->GetNext()) {
        pInstr = node->GetData();
        
        //for each lmVStaff
        for (iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++) {
            pStaff = pInstr->GetVStaff(iVStaff);
            
            //look for posible lmStaffObj and exit if any found
            pScO = pStaff->FindSelectableObject(pt);
            if (pScO) return(pScO);
        }
    }
    return (lmScoreObj *)NULL;    //none found
}

////Friend methods for lmFormatter object and for internal use
////=========================================================================================

lmInstrument* lmScore::GetFirstInstrument()
{
    m_pNode = m_cInstruments.GetFirst();
    return (m_pNode ? (lmInstrument *)m_pNode->GetData() : (lmInstrument *)m_pNode);
}

lmInstrument* lmScore::GetNextInstrument()
{
    wxASSERT(m_pNode);
    m_pNode = m_pNode->GetNext();
    return (m_pNode ? (lmInstrument *)m_pNode->GetData() : (lmInstrument *)m_pNode);
}

lmInstrument* lmScore::GetLastInstrument()
{
    wxASSERT(m_pNode);
    m_pNode = m_cInstruments.GetLast();
    return (m_pNode ? (lmInstrument *)m_pNode->GetData() : (lmInstrument *)m_pNode);
}

////Returns the lmInstrument number nInstr (1..n)
//Friend Property Get lmInstrument(nInstr As Long) As CInstrumento
//    Set lmInstrument = m_cInstruments.Item(nInstr)
//    
//}
//
////Returns the number of Instruments in the Score
//Friend Property Get InstrumentsCount() As Long
//    InstrumentsCount = m_cInstruments.Count
//    
//}
//
////== End of Friend methods for lmFormatter object ============================================

wxString lmScore::Dump()
{
    wxString sDump = _T("Global objects:\n");

    //loop to dump global StaffObjs
    lmStaffObj* pSO;
    wxStaffObjsListNode* pNode;
    for(pNode = m_cGlobalStaffobjs.GetFirst(); pNode; pNode = pNode->GetNext()) {
        pSO = (lmStaffObj*)pNode->GetData();
        sDump += pSO->Dump();
    }

    //loop to dump all instruments
    sDump += _T("\nLocal objects:\n");
    lmInstrument *pInstr = GetFirstInstrument();
    for (int i=1; i<= (int)m_cInstruments.GetCount(); i++, pInstr = GetNextInstrument())
    {
        sDump += wxString::Format(_T("\nInstrument %d\n"), i );
        sDump += pInstr->Dump();
    }
    return sDump;
}

wxString lmScore::SourceLDP()
{
    wxString sSource = 
        wxString::Format(_T("Score\n   (Vers 1.3)\n   (NumInstrumentos %d)\n"),
                    m_cInstruments.GetCount() );

    //loop for each instrument
     lmInstrument *pInstr = GetFirstInstrument();
    for (int i=1; i<= (int)m_cInstruments.GetCount(); i++, pInstr = GetNextInstrument())
    {
        sSource += wxString::Format(_T("   (Instrumento %d\n"), i);
        sSource += pInstr->SourceLDP();
        sSource += _T("   )\n");
    }
    sSource += _T(")");
    return sSource;
}

wxString lmScore::SourceXML()
{
    wxString sSource = _T("TODO: lmScore XML Source code generation methods");

//    Dim i As Long, sFuente As String
//    
//    sFuente = "<?xml version=""1.0"" standalone=""no""?>" & sCrLf & _
//            "<!DOCTYPE score-partwise PUBLIC ""-//Recordare//DTD MusicXML 0.7 Partwise//EN"" " & _
//            """http://www.musicxml.org/dtds/partwise.dtd"">" & sCrLf & _
//            "<score-partwise>" & sCrLf & _
//            "  <part-list>" & sCrLf
////        <score-part id="P1">
////            <part-name>Voice</part-name>
////        </score-part>
//
////    sFuente = sFuente & "<Score" & sCrLf & "   (Vers 1.3)" & sCrLf & "   (NumInstrumentos " & _
////        m_cInstruments.Count & ")" & sCrLf
//    for (i = 1 To m_cInstruments.Count
//        sFuente = sFuente & "    <score-part id=""P" & i & """ >" & sCrLf
////        sFuente = sFuente & m_cInstruments.Item(i).FuenteXML
////        sFuente = sFuente & "   )" & sCrLf
//        sFuente = sFuente & "    </score-part>" & sCrLf
//    }    // i
//    sFuente = sFuente & "  </part-list>" & sCrLf
//    for (i = 1 To m_cInstruments.Count
//        sFuente = sFuente & "  <part id=""P" & i & """ >" & sCrLf
//        sFuente = sFuente & m_cInstruments.Item(i).FuenteXML
//        sFuente = sFuente & "  </part>" & sCrLf
//    }    // i
//    FuenteXML = sFuente & "</score-partwise>" & sCrLf
    return sSource;
        
}

////Toca el compas nCompas (1 .. n)
//void lmScore::PlayMeasure(nCompas As Long, fVisualTracking As Boolean, _
//                fRecuadrarCompas As Boolean, nPlayMode As EPlayMode)
//{
//
//    if (!fMIDIEnabled) return;
//    if (m_pSoundMngr Is Nothing) { ComputeMidiEvents
//
//    m_pSoundMngr->PlayMeasure nCompas, fVisualTracking, fRecuadrarCompas, nPlayMode
//
//}

void lmScore::Play(bool fVisualTracking, bool fMarcarCompasPrevio, EPlayMode nPlayMode, 
                 long nMM, wxWindow* pWindow)
{
    if (!m_pSoundMngr) {
        m_pSoundMngr = new lmSoundManager();
        ComputeMidiEvents();
    }

    m_pSoundMngr->Play(fVisualTracking, fMarcarCompasPrevio, nPlayMode, nMM, pWindow);

}

void lmScore::PlayMeasure(int nMeasure, bool fVisualTracking, EPlayMode nPlayMode,
                          long nMM, wxWindow* pWindow)
{
    if (!m_pSoundMngr) {
        m_pSoundMngr = new lmSoundManager();
        ComputeMidiEvents();
    }

    m_pSoundMngr->PlayMeasure(nMeasure, fVisualTracking, nPlayMode, nMM, pWindow);
}

void lmScore::Pause()
{
    if (!m_pSoundMngr) return;
    m_pSoundMngr->Pause();

}

void lmScore::Stop()
{
    if (!m_pSoundMngr) return;
    m_pSoundMngr->Stop();

}

void lmScore::WaitForTermination()
{
    if (!m_pSoundMngr) return;
    m_pSoundMngr->WaitForTermination();

}

void lmScore::ScoreHighlight(lmStaffObj* pSO, lmPaper* pPaper, EHighlightType nHighlightType)
{
    switch (nHighlightType) {
        case eVisualOn:
            m_cHighlighted.Append(pSO);
            pSO->Draw(DO_DRAW, pPaper, g_pColors->ScoreHighlight() );
            break;

        case eVisualOff:
            m_cHighlighted.DeleteObject(pSO);
            RemoveHighlight(pSO, pPaper);
            break;

        case eRemoveAllHighlight:
            //remove highlight from all staffobjs in m_cHighlighted list
            wxStaffObjsListNode* pNode;
            for(pNode = m_cHighlighted.GetFirst(); pNode; pNode = pNode->GetNext()) {
                RemoveHighlight( (lmStaffObj*)pNode->GetData(), pPaper );
            }
            //clear the list
            m_cHighlighted.DeleteContents(false);    //Staffobjs must not be deleted, only the list
            m_cHighlighted.Clear();
            break;

        default:
            wxASSERT(false);
    }

}

void lmScore::RemoveHighlight(lmStaffObj* pSO, lmPaper* pPaper)
{
    /*! @todo
        If we paint in black it remains a red aureole around
        the note. By painting it first in white the size of the aureole
        is smaller but still visible. A posible better solution is to
        modify Draw() method to accept an additional parameter: a flag
        to signal that XOR draw mode in RED followed by a normal
        draw in BLACK must be done.
    */
    pSO->Draw(DO_DRAW, pPaper, *wxWHITE);
    pSO->Draw(DO_DRAW, pPaper, g_pColors->ScoreNormal() );
}

void lmScore::ComputeMidiEvents()
{
    int nChannel, nInstr;        //MIDI info. for instrument in process
    lmSoundManager* pSM;            
    
    if (m_pSoundMngr)
        m_pSoundMngr->DeleteEventsTable();
    else
        m_pSoundMngr = new lmSoundManager();

    //Loop to generate Midi events for each instrument
    lmVStaff* pVStaff;
     lmInstrument *pInstr = GetFirstInstrument();
    for (int i=1; i<= (int)m_cInstruments.GetCount(); i++, pInstr = GetNextInstrument())
    {       
        nChannel = pInstr->GetMIDIChannel();
        nInstr = pInstr->GetMIDIInstrument();

       //for each lmVStaff
        for (int iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++) {
            pVStaff = pInstr->GetVStaff(iVStaff);
            pSM = pVStaff->ComputeMidiEvents(nChannel);
            m_pSoundMngr->Append(pSM);
            delete pSM;
        }

        //Add an event to program sound for this instrument
        m_pSoundMngr->StoreEvent(0, eSET_ProgInstr, nChannel, nInstr, 0, (lmStaffObj*)NULL, 0);
        
    }
    
    //End up Midi events table and sort it by time
    m_pSoundMngr->CloseTable();
    
}

wxString lmScore::DumpMidiEvents()
{
    if (!m_pSoundMngr) ComputeMidiEvents();
    return m_pSoundMngr->DumpMidiEvents();
}

