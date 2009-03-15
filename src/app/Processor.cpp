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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Processor.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#else
    #include <wx/sizer.h>
    #include <wx/panel.h>
#endif


#include "Processor.h"

// access to main frame
#include "../app/MainFrame.h"
extern lmMainFrame* GetMainFrame();

#include "toolbox/ToolsBox.h"
#include "ScoreDoc.h"
#include "../exercises/auxctrols/UrlAuxCtrol.h"
#include "../score/Score.h"
#include "../score/VStaff.h"

//@@Carlos
//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;
#include "../auxmusic/ChordManager.h"

//-------------------------------------------------------------------------------------------
// Implementation of class lmScoreProcessor
//-------------------------------------------------------------------------------------------

//IDs for controls
const int lmID_DO_PROCESS = wxNewId();
const int lmID_UNDO_PROCESS = wxNewId();


lmScoreProcessor::lmScoreProcessor()
    : wxEvtHandler()
{
}

lmScoreProcessor::~lmScoreProcessor()
{
}

void lmScoreProcessor::DoProcess()
{
    //get the score
    lmMainFrame* pMainFrame = GetMainFrame();
    lmScoreDocument* pDoc = pMainFrame->GetActiveDoc();
    if (!pDoc) return;
    lmScore* pScore = pDoc->GetScore();
    if (!pScore) return;

    if (ProcessScore(pScore))
    {
        //if changes done in the score, update views
	    pDoc->Modify(true);
        pDoc->UpdateAllViews(true, new lmUpdateHint() );

        //enable undo link
        m_pUndoLink->Enable(true);
    }
}

void lmScoreProcessor::UndoProcess()
{
    //get the score
    lmMainFrame* pMainFrame = GetMainFrame();
    lmScoreDocument* pDoc = pMainFrame->GetActiveDoc();
    if (!pDoc) return;
    lmScore* pScore = pDoc->GetScore();
    if (!pScore) return;

    if (UndoChanges(pScore))
    {
        //if changes done in the score, update views
	    pDoc->Modify(true);
        pDoc->UpdateAllViews(true, new lmUpdateHint() );
    }

    //disable undo link
    m_pUndoLink->Enable(false);
}

bool lmScoreProcessor::CreateToolsPanel(wxString sTitle, wxString sDoLink,
                                       wxString sUndoLink)
{
    lmMainFrame* pMainFrame = GetMainFrame();
    lmToolBox* pToolbox = pMainFrame->GetActiveToolBox();
    if (!pToolbox) return false;        //false -> failure

    m_pToolsPanel = new wxPanel(pToolbox, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

	m_pMainSizer = new wxBoxSizer( wxVERTICAL );

    AddStandardLinks(m_pMainSizer, sDoLink, sUndoLink);

    return true;
}
void lmScoreProcessor::AddStandardLinks(wxBoxSizer* pSizer, wxString sDoLink,
                                        wxString sUndoLink)
{
    // 'Do process' link
    if (sDoLink != wxEmptyString)
    {
        m_pDoLink = new lmUrlAuxCtrol(m_pToolsPanel, lmID_DO_PROCESS, 1.0, sDoLink);
        pSizer->Add(m_pDoLink, 0, wxALL|wxEXPAND, 5);
        m_pToolsPanel->Connect(lmID_DO_PROCESS, lmEVT_URL_CLICK,
                               wxObjectEventFunction(&lmScoreProcessor::DoProcess),
                               NULL, this);
    }

    // 'Do process' link
    if (sDoLink != wxEmptyString)
    {
        m_pUndoLink = new lmUrlAuxCtrol(m_pToolsPanel, lmID_UNDO_PROCESS, 1.0, sUndoLink);
        pSizer->Add(m_pUndoLink, 0, wxALL|wxEXPAND, 5);
        m_pToolsPanel->Connect(lmID_UNDO_PROCESS, lmEVT_URL_CLICK,
                               wxObjectEventFunction(&lmScoreProcessor::UndoProcess),
                               NULL, this);
    }
}

void lmScoreProcessor::RealizePanel()
{
    m_pToolsPanel->SetSizer( m_pMainSizer );
	m_pToolsPanel->Layout();
	m_pMainSizer->Fit( m_pToolsPanel );

    lmMainFrame* pMainFrame = GetMainFrame();
    lmToolBox* pToolbox = pMainFrame->GetActiveToolBox();
    if (pToolbox)
        pToolbox->AddSpecialTools(m_pToolsPanel, this);

    //disable undo link
    m_pUndoLink->Enable(false);
}

//-------------------------------------------------------------------------------------------
// Implementation of class lmHarmonyProcessor
//-------------------------------------------------------------------------------------------

lmHarmonyProcessor::lmHarmonyProcessor()
    : lmScoreProcessor()
{
}

lmHarmonyProcessor::~lmHarmonyProcessor()
{
    //delete all markup pairs
    std::list<lmMarkup*>::iterator it = m_markup.begin();
    while (it != m_markup.end())
    {
        lmMarkup* pError = *it;
        m_markup.erase(it++);
        delete pError;
    }
}

bool lmHarmonyProcessor::SetTools()
{
    //Create a panel with the exercise buttons, and place it on the ToolBox
    //Returns false if failure.

    if (!CreateToolsPanel(_("Harmony exercise"),
                          _("Check harmony"),
                          _("Clear errors")) )
        return false;

    //No more tools to add. Show panel in toolbox
    RealizePanel();
    return true;
}


#ifdef __WXDEBUG__
void lmHarmonyProcessor::UnitTests()
{
    /******
    int i, j;

    //lmConverter::NoteToBits and lmConverter::NoteBitsToName
    wxLogMessage(_T("[lmChordManager::UnitTests] Test of lmConverter::NoteToBits() method:"));
    wxString sNote[8] = { _T("a4"), _T("+a5"), _T("--b2"), _T("-a4"),
        _T("+e4"), _T("++f6"), _T("b1"), _T("xc4") };
    lmNoteBits tNote;
    for(i=0; i < 8; i++) {
        if (lmConverter::NoteToBits(sNote[i], &tNote))
            wxLogMessage(_T("Unexpected error in lmConverter::NoteToBits()"));
        else {
            wxLogMessage(_T("Note: '%s'. Bits: Step=%d, Octave=%d, Accidentals=%d, StepSemitones=%d --> '%s'"),
                sNote[i].c_str(), tNote.nStep, tNote.nOctave, tNote.nAccidentals, tNote.nStepSemitones,
                lmConverter::NoteBitsToName(tNote, m_nKey).c_str() );
        }
    }

    //ComputeInterval(): interval computation
    wxString sIntv[8] = { _T("M3"), _T("m3"), _T("p8"), _T("p5"),
        _T("a5"), _T("d7"), _T("M6"), _T("M2") };
    for(i=0; i < 8; i++) {
        for (j=0; j < 8; j++) {
            wxString sNewNote = ComputeInterval(sNote[i], sIntv[j], true, m_nKey);
            wxLogMessage(_T("Note='%s' + Intv='%s' --> '%s'"),
                         sNote[i].c_str(), sIntv[j].c_str(), sNewNote.c_str() );
            wxString sStartNote = ComputeInterval(sNewNote, sIntv[j], false, m_nKey);
            wxLogMessage(_T("Note='%s' - Intv='%s' --> '%s'"),
                         sNewNote.c_str(), sIntv[j].c_str(), sStartNote.c_str() );
        }
    }

    //IntervalCodeToBits and IntervalBitsToCode
    wxLogMessage(_T("[lmChordManager::UnitTests] Test of IntervalCodeToBits() method:"));
    lmIntvBits tIntv;
    for(i=0; i < 8; i++) {
        if (IntervalCodeToBits(sIntv[i], &tIntv))
            wxLogMessage(_T("Unexpected error in IntervalCodeToBits()"));
        else {
            wxLogMessage(_T("Intv: '%s'. Bits: num=%d, Semitones=%d --> '%s'"),
                sIntv[i].c_str(), tIntv.nNum,tIntv.nSemitones,
                IntervalBitsToCode(tIntv).c_str() );
        }
    }
    **/

    ////SubstractIntervals
    //wxLogMessage(_T("[lmChordManager::UnitTests] Test of SubstractIntervals() method:"));
    //wxString sIntv1[8] = { _T("p5"), _T("p5"), _T("M7"), _T("M6"), _T("m6"), _T("M7"), _T("M6"), _T("p4") };
    //wxString sIntv2[8] = { _T("M3"), _T("m3"), _T("p5"), _T("p5"), _T("a5"), _T("M3"), _T("m3"), _T("M2") };
    //for(i=0; i < 8; i++) {
    //    wxLogMessage(_T("Intv1='%s', intv2='%s' --> dif='%s'"),
    //        sIntv1[i], sIntv2[i], SubstractIntervals(sIntv1[i], sIntv2[i]) );
    //}

    ////AddIntervals
    //wxLogMessage(_T("[lmChordManager::UnitTests] Test of AddIntervals() method:"));
    //wxString sIntv1[8] = { _T("p5"), _T("p5"), _T("M6"), _T("M3"), _T("M3"), _T("M6"), _T("d4"), _T("p8") };
    //wxString sIntv2[8] = { _T("M3"), _T("m3"), _T("m2"), _T("m3"), _T("M3"), _T("M3"), _T("m7"), _T("p8") };
    //for(i=0; i < 8; i++) {
    //    wxLogMessage(_T("Intv1='%s', intv2='%s' --> sum='%s'"),
    //        sIntv1[i].c_str(), sIntv2[i].c_str(), AddIntervals(sIntv1[i], sIntv2[i]).c_str() );
    //}

}
#endif  // __WXDEBUG__




bool lmHarmonyProcessor::ProcessScore(lmScore* pScore)
{
    //This method checks the score and show errors
    //Returns true if it has done any change in the score

    //As an example, I will put red and a green lines pointing to fourth and
    //sixth notes, respectively, and add some texts
    bool fScoreModified = false;



    //define the font to use for texts
    lmFontInfo tFont = {_("Comic Sans MS"), 8, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL};
    lmTextStyle* pStyle = pScore->GetStyleName(tFont);

    //Get the instrument
    lmInstrument* pInstr = pScore->GetFirstInstrument();
    lmVStaff* pVStaff = pInstr->GetVStaff();

    //@@@Carlos
    const int MAX_CHORDS = 20;
    int numChords = 0;
    lmChordManager* pChords[MAX_CHORDS];

	lmNote* pChordNotes[lmNOTES_IN_CHORD];
	lmNote* currentNote;
    int currentNotePos, chordNotePos = -2;
    int chordNoteIndex = 0;

    // Loop to process notes/rests in first staff of first instrument
    int nNote = 0;
    lmSOIterator* pIter = pVStaff->CreateIterator();
    while(!pIter->EndOfCollection())
    {
        lmStaffObj* pSO = pIter->GetCurrent();
        if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsNote())
        {
            // It is a note. Count it
            ++nNote;

 //@@@CARLOS
			currentNote = (lmNote*) pSO;
			currentNotePos = currentNote->GetChordPosition();
            wxLogMessage(_T("[ProcessScore] note %d: pitch: %d, pos: %d")
                , nNote, currentNote->GetFPitch(), currentNotePos);
            wxLogMessage(_T("  LDP:%s")
                ,  currentNote->SourceLDP(0));

            // Notes with beat position of lmNON_CHORD_NOTE are ignored
            // New beat position means new chord

            if (  currentNotePos != lmNON_CHORD_NOTE)
            {
                // Note candidate to chord

                if (  currentNotePos != chordNotePos )
                {
                   // Chord initialization
                   wxLogMessage(_T("  New chord because: currentPos %d, prev: %d, index:%d")
                        ,  currentNotePos,  chordNotePos, chordNoteIndex );

                    // Different beat position: new chord
                    // If at least 3 notes: create chord
                    if (chordNoteIndex > 3)
                    {
                        // Create Chord
                        wxLogMessage(_T("@@Creamos chord con %d notas"), chordNoteIndex-1);
                        pChords[numChords++] = new lmChordManager(chordNoteIndex-1, pChordNotes);

/*@@ provisional: display chord info in score with a line and text */
                        for (int i = 0; i<chordNoteIndex-1; i++)
                        {
                            pChordNotes[i]->SetColour(*wxGREEN);
                        }

                        //green line pointing to the chord
                        lmStaffObj* cpSO = pChordNotes[2];
                        lmScoreLine* pLine = new lmScoreLine(40, -20, 5, 100, 2, *wxGREEN);
                        cpSO->AttachAuxObj(pLine);
                        lmMarkup* pError = new lmMarkup(cpSO, pLine);
                        m_markup.push_back(pError);

                        //text at top of the line
                        wxString sText = pChords[numChords-1]->toString();
                        lmTextItem* pText = new lmTextItem(sText, lmHALIGN_DEFAULT, pStyle);
                        cpSO->AttachAuxObj(pText);
                        pText->SetUserLocation(40.0f, -30.0f);
                        pError = new lmMarkup(cpSO, pText);
                        m_markup.push_back(pError);

                        fScoreModified = true;



                    }

                    chordNoteIndex = 0;
        			pChordNotes[0] = currentNote;
                    chordNotePos = currentNotePos;
                    chordNoteIndex++;
                    wxLogMessage(_T("[ProcessScore] First chord note pitch: %d, pos: %d")
                        , currentNote->GetFPitch(), chordNotePos);

                } //  if (  currentNotePos != chordNotePos )
                // else: in the same beat then add to chord
                //   @@ also verify same duration?
                else 
                {
        			pChordNotes[chordNoteIndex] = currentNote;
                    wxLogMessage(_T("[ProcessScore] new %d chord note, pitch: %d")
                        , chordNoteIndex, currentNote->GetFPitch());
                    chordNoteIndex++;
                }


            }
            //  else [ currentNotePos == lmNON_CHORD_NOTE]: ignore
        } 
        // else [ not IsNote]: ignore

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@	
			if (nNote == 4)
            {
                //fourth note. Change its colour to red
                pSO->SetColour(*wxRED);

                //Put a red line pointing to it
                //Remember: all 'y' positions are relative to top line (5th line of
                //first staff). 'x' positions are relative to current object position.
                lmScoreLine* pLine = new lmScoreLine(-30, -30, 5, 80, 2, *wxRED);
                pSO->AttachAuxObj(pLine);
                lmMarkup* pError = new lmMarkup(pSO, pLine);
                m_markup.push_back(pError);

                //text at top of the line
                wxString sText = _T("Bad !!!");
                lmTextItem* pText = new lmTextItem(sText, lmHALIGN_DEFAULT, pStyle);
                pSO->AttachAuxObj(pText);
                pText->SetUserLocation(-80.0f, -40.0f);
                pError = new lmMarkup(pSO, pText);
                m_markup.push_back(pError);

                fScoreModified = true;
            }
            else if (nNote == 6)
            {
                //sixth note. Change its colour to green, put a line and a text and finish loop
                pSO->SetColour(*wxGREEN);

                //green line pointing to the note
                lmScoreLine* pLine = new lmScoreLine(40, -20, 5, 100, 2, *wxGREEN);
                pSO->AttachAuxObj(pLine);
                lmMarkup* pError = new lmMarkup(pSO, pLine);
                m_markup.push_back(pError);

                //text at top of the line
                wxString sText = _T("I like this note!");
                lmTextItem* pText = new lmTextItem(sText, lmHALIGN_DEFAULT, pStyle);
                pSO->AttachAuxObj(pText);
                pText->SetUserLocation(40.0f, -30.0f);
                pError = new lmMarkup(pSO, pText);
                m_markup.push_back(pError);

                fScoreModified = true;
                break;
            }
        }
*****************************/
        pIter->MoveNext();
    } // while
    delete pIter;       //Do not forget this. We are not using smart pointers!

    // Last chord?
    if (chordNoteIndex > 2)
    {
        // Create Chord
        wxLogMessage(_T("@@Creamos chord con %d notas"), chordNoteIndex);
        pChords[numChords++] = new lmChordManager(chordNoteIndex, pChordNotes);
    }

    return fScoreModified;      //true -> score modified
}

bool lmHarmonyProcessor::UndoChanges(lmScore* pScore)
{
    //This method removes all error markup from the score

    //TODO:
    //The base class should provide an standard functionality for doing this.
    //For example, it should maintain a list of changes and it will undo all these
    //changes. This way, derived classes have nothing to implement, unless an
    //special behaviour is desired.

    //This code is just an example to undo what we did at ProcessScore()

    WXUNUSED(pScore);
    std::list<lmMarkup*>::iterator it = m_markup.begin();
    while (it != m_markup.end())
    {
        //undo this markup
        lmMarkup* pError = *it;
        lmStaffObj* pParent = pError->first;
        lmAuxObj* pAttachment = pError->second;
	    pParent->DetachAuxObj(pAttachment);
	    pParent->SetColour(*wxBLACK);
        delete pAttachment;

        //remove used element and move to next item in list
        m_markup.erase(it++);
        delete pError;
    }
    return true;
}
