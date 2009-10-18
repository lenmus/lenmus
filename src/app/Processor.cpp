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
#include "../score/AuxObj.h"        //lmScoreLine
#include "../graphic/ShapeNote.h"
#include "ScoreCommand.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;
#include "../auxmusic/Chord.h"
#include "../auxmusic/HarmonyExercisesData.h"

//-------------------------------------------------------------------------------------------
// Implementation of class lmScoreProcessor
//-------------------------------------------------------------------------------------------

//IDs for controls
const int lmID_DO_PROCESS = wxNewId();
const int lmID_UNDO_PROCESS = wxNewId();

IMPLEMENT_ABSTRACT_CLASS(lmScoreProcessor, wxEvtHandler)

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
    lmDocument* pDoc = pMainFrame->GetActiveDoc();
    if (!pDoc) return;
    lmScore* pScore = pDoc->GetScore();
    if (!pScore) return;

    //create and issue the command
    wxCommandProcessor* pCP = pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdScoreProcessor(lmCMD_NORMAL, pDoc, this) );
}

bool lmScoreProcessor::CreateToolsPanel(wxString sTitle, wxString sDoLink)
{
    lmMainFrame* pMainFrame = GetMainFrame();
    lmToolBox* pToolbox = pMainFrame->GetActiveToolBox();
    if (!pToolbox) return false;        //false -> failure

    m_pToolsPanel = new wxPanel(pToolbox, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

	m_pMainSizer = new wxBoxSizer( wxVERTICAL );

    AddDoLink(m_pMainSizer, sDoLink);

    return true;
}
void lmScoreProcessor::AddDoLink(wxBoxSizer* pSizer, wxString sDoLink)
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
}


//-------------------------------------------------------------------------------------------
// lmProcessorMngr implementation
//-------------------------------------------------------------------------------------------

lmProcessorMngr* lmProcessorMngr::m_pInstance = (lmProcessorMngr*)NULL;

lmProcessorMngr::lmProcessorMngr()
{
}

lmProcessorMngr::~lmProcessorMngr()
{
    //delete all alive score processors

    std::map<lmScoreProcessor*, long>::iterator it;
    for (it = m_ActiveProcs.begin(); it != m_ActiveProcs.end(); ++it)
    {
        delete it->first;
    }
    m_ActiveProcs.clear();
}

lmProcessorMngr* lmProcessorMngr::GetInstance()
{
    if (!m_pInstance)
        m_pInstance = new lmProcessorMngr();
    return m_pInstance;
}

void lmProcessorMngr::DeleteInstance()
{
    if (m_pInstance)
        delete m_pInstance;
    m_pInstance = (lmProcessorMngr*)NULL;
}

lmScoreProcessor* lmProcessorMngr::CreateScoreProcessor(wxClassInfo* pScoreProcInfo)
{
    lmScoreProcessor* pProc = (lmScoreProcessor*)NULL;
    if (pScoreProcInfo)
    {
        //create the score processor
        pProc = (lmScoreProcessor*)pScoreProcInfo->CreateObject();
        m_ActiveProcs.insert(std::make_pair(pProc, 1L));
    }
    return pProc;
}

void lmProcessorMngr::IncrementReference(lmScoreProcessor* pProc)
{
    std::map<lmScoreProcessor*, long>::iterator it = m_ActiveProcs.find(pProc);
    wxASSERT(it != m_ActiveProcs.end());

    ++(it->second);
}

void lmProcessorMngr::DeleteScoreProcessor(lmScoreProcessor* pProc)
{
    std::map<lmScoreProcessor*, long>::iterator it = m_ActiveProcs.find(pProc);
    wxASSERT(it != m_ActiveProcs.end());

    if (--(it->second) == 0)
    {
        delete it->first;
        m_ActiveProcs.erase(it);
    }
}


//-------------------------------------------------------------------------------------------
// Implementation of class lmTestProcessor
//-------------------------------------------------------------------------------------------
#ifdef __WXDEBUG__

IMPLEMENT_DYNAMIC_CLASS(lmTestProcessor, lmScoreProcessor)

lmTestProcessor::lmTestProcessor()
    : lmScoreProcessor()
{
}

lmTestProcessor::~lmTestProcessor()
{
}

bool lmTestProcessor::SetTools()
{
    //Create a panel with the exercise buttons, and place it on the ToolBox
    //Returns false if failure.

    return true;
}

bool lmTestProcessor::ProcessScore(lmScore* pScore, void* pOptions)
{
    //This method checks the score and show errors
    //Returns true if it has done any change in the score

	bool fScoreModified = false;

	//Get the instrument
	lmInstrument* pInstr = pScore->GetFirstInstrument();
	lmVStaff* pVStaff = pInstr->GetVStaff();

    //notes to join with arrows
	int nNote = 0;
    lmNote* pNote1;
    lmNote* pNote2;
    lmNote* pNote3;
    lmNote* pNote4;
    lmNote* pNote5;
    lmNote* pNote6;

	// Loop to process notes/rests in first staff of first instrument
	lmSOIterator* pIter = pVStaff->CreateIterator();
	while(!pIter->EndOfCollection())
	{
	  	lmStaffObj* pSO = pIter->GetCurrent();
	  	if (pSO->IsNote())
	  	{
            // It is a note. Count it
			++nNote;
            if (nNote == 2)
                pNote1 = (lmNote*)pSO;
            else if (nNote == 6)
                pNote2 = (lmNote*)pSO;
            else if (nNote == 3)
                pNote3 = (lmNote*)pSO;
            else if (nNote == 7)
                pNote4 = (lmNote*)pSO;
            else if (nNote == 5)
                pNote5 = (lmNote*)pSO;
            else if (nNote == 12)
                pNote6 = (lmNote*)pSO;

		}
		pIter->MoveNext();
	}
	delete pIter;       //Do not forget this. We are not using smart pointers!

    //Add line between pairs of notes

    if (pNote1 && pNote2)
    {
        DrawArrow(pNote1, pNote2, *wxRED);
	    fScoreModified = true;
    }
    if (pNote3 && pNote4)
    {
        DrawArrow(pNote3, pNote4, wxColour(0, 170, 0) );
	    fScoreModified = true;
    }
    if (pNote5 && pNote6)
    {
        DrawArrow(pNote5, pNote6, wxColour(0, 0, 200) );
	    fScoreModified = true;
    }

	return fScoreModified;      //true -> score modified
}

void lmTestProcessor::DrawArrow(lmNote* pNote1, lmNote* pNote2, wxColour color)
{
    //get VStaff
    lmVStaff* pVStaff = pNote1->GetVStaff();

    //get note heads positions
    lmURect uBounds1 = pNote1->GetNoteheadShape()->GetBounds();
    lmURect uBounds2 = pNote2->GetNoteheadShape()->GetBounds();

    //start point
    lmUPoint uStart( uBounds1.GetWidth(), 0);
    uStart.y = pNote1->GetShiftToNotehead();        //center of notehead

    //end point
    lmUPoint uEnd(uBounds2.GetRightTop() - uBounds1.GetRightTop());
    uEnd.y += uStart.y;

    //convert to tenths
    lmTenths xtStart = pVStaff->LogicalToTenths(uStart.x) + 8.0;
    lmTenths ytStart = pVStaff->LogicalToTenths(uStart.y);
    lmTenths xtEnd = pVStaff->LogicalToTenths(uEnd.x) - 8.0;
    lmTenths ytEnd = pVStaff->LogicalToTenths(uEnd.y);

    //create arrow
    pNote1->AttachLine(xtStart, ytStart, xtEnd, ytEnd, 2, lm_eLineCap_None,
                       lm_eLineCap_Arrowhead, lm_eLine_Solid, color);
    pNote1->SetColour(color);
    pNote2->SetColour(color);
}
#endif


//-------------------------------------------------------------------------------------------
// Implementation of class lmHarmonyProcessor
//-------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(lmHarmonyProcessor, lmScoreProcessor)

lmHarmonyProcessor::lmHarmonyProcessor()
    : lmScoreProcessor()
{
  pBoxSize = new wxSize(400, 60);
  pErrorBoxSize = new wxSize(560, 60);  //@@@@TODO REMOVE: antes (510, 60) (550, 60)...

  tFont.sFontName = _("Comic Sans MS");
  tFont.nFontSize = 6;
  tFont.nFontStyle = wxFONTSTYLE_NORMAL;
  tFont.nFontWeight = wxFONTWEIGHT_NORMAL;

 // Text box for general information
 // -200 --> Box X position shifted to left
 //  400 --> Initial Box Y position at the bottom
 //    0 --> Line X end position: centered with chord
 //  100 --> Line Y end position: slightly shifted down
  pInfoBox = new ChordInfoBox(pBoxSize, &tFont, -200, 500, 0, 100, -50);

 // Text box for errror information
 // -200 --> Box X position shifted to left
 // -200 --> Initial Box Y position at the TOP
 //    0 --> Line X end position: centered with chord
 //  100 --> Line Y end position: slightly shifted down
 //  +50 --> Increment y position after each use --> go downwards
  pChordErrorBox = new ChordInfoBox(pErrorBoxSize, &tFont, -150, -200, 0, 100, +50);
}

lmHarmonyProcessor::~lmHarmonyProcessor()
{
    delete pBoxSize;
    delete pErrorBoxSize;
    delete pInfoBox;
    delete pChordErrorBox;

}

bool lmHarmonyProcessor::SetTools()
{
    //Create a panel with the exercise buttons, and place it on the ToolBox
    //Returns false if failure.

    if (!CreateToolsPanel(_("Harmony exercise"),
                          _("Check harmony")) )
        return false;

    //No more tools to add. Show panel in toolbox
    RealizePanel();
    return true;
}


bool lmHarmonyProcessor::ProcessScore(lmScore* pScore, void* pOptions)
{
    //This method checks the score and show errors
    //Returns true if it has done any change in the score

    //As an example, I will put red and a green lines pointing to fourth and
    //sixth notes, respectively, and add some texts
    bool fScoreModified = false;

    // total number of chords (both valid and invalid chords)
    int nNumChords = 0;
    // all the information of each chord
    lmScoreChord* tChordDescriptor[lmMAX_NUM_CHORDS];
    // aux variable to know the active notes in each time step
    lmActiveNotes tActiveNotesList;
    lmNote* pPossibleChordNotes[lmNOTES_IN_CHORD];

    float rAbsTime = 0.0f;
    float rTimeAtStartOfMeasure = 0.0f;

    //Get the instrument
    lmInstrument* pInstr = pScore->GetFirstInstrument();
    lmVStaff* pVStaff = pInstr->GetVStaff();

	lmNote* pCurrentNote;
    float rCurrentNoteAbsTime = -2.0f;
    float rRelativeTime = -2.0f;
    wxString sStatusStr;

    // TODO: improve postioning of the textbox...
    pInfoBox->ResetPosition();

    int nBadChords = 0;

    /*---

      Algorithm of chord detection, keeping a list of "active notes"

       When NEW NOTE
	      if NEW TIME (i.e. higher) then
             update current list of active notes: remove notes with end-time < TIME
             analize possible chord in previous state: with current list of active notes
             set new time
          add new note to the list of active notes

    ---*/

    // loop to process notes/rests in first staff of first instrument
    int nNote = 0;
    lmSOIterator* pIter = pVStaff->CreateIterator();
    lmEKeySignatures nKey;

    while(!pIter->EndOfCollection())
    {
        // process only notes, rests and barlines
        lmStaffObj* pSO = pIter->GetCurrent();
        if (pSO->IsBarline())
        {
            // new measure starts. Update current time
            rTimeAtStartOfMeasure += pSO->GetTimePos();
            rAbsTime = rTimeAtStartOfMeasure;
        }
        else if (pSO->IsNoteRest())
        {
            // we continue in previous measure. Update current time if necessary
            rRelativeTime = pSO->GetTimePos();
            rAbsTime = rTimeAtStartOfMeasure + rRelativeTime;

            // process notes
            if (pSO->IsNote())
            {
                // It is a note. Count it
                ++nNote;
			    pCurrentNote  = (lmNote*) pSO;

                // calculate note's absolute time
                rCurrentNoteAbsTime = rTimeAtStartOfMeasure + rRelativeTime;

                // check new starting time (to analyze previous chord candidate)
                if (  IsHigherTime(rCurrentNoteAbsTime, tActiveNotesList.GetTime())  )
                {
                    /*-----
                      if NEW-TIME (i.e. higher) then
                         update current list of active notes: remove notes with end-time < CURRENT-TIME
                         analize possible chord in previous state: with current list of active notes
                         set new time
                      add new note to the list of active notes
                    ---*/

                    // Get the key
                    if (pCurrentNote->GetApplicableKeySignature())
                        nKey = pCurrentNote->GetApplicableKeySignature()->GetKeyType();
                    else
                        nKey = earmDo;

                    // Get the notes
                    int nNumActiveNotes = tActiveNotesList.GetNotes(&pPossibleChordNotes[0]);

                    // sort the notes
                    SortChordNotes(nNumActiveNotes, &pPossibleChordNotes[0]);

                    //
                    // Get the chord from the notes
                    //
                    tChordDescriptor[nNumChords] = new lmScoreChord
                        (nNumActiveNotes, &pPossibleChordNotes[0], nKey);

                    if (!tChordDescriptor[nNumChords]->IsStandardChord())
                    {
                        wxColour colour = wxColour(255,0,0,128); // R, G, B, Transparency
                        wxString sStr = tChordDescriptor[nNumChords]->ToString();
                        pInfoBox->DisplayChordInfo(pScore, tChordDescriptor[nNumChords], colour, sStr);
                    }
                    nNumChords++;

                    // set new time and recalculate list of active notes
                    tActiveNotesList.SetTime( rCurrentNoteAbsTime );

                    fScoreModified = true; // repaint
                }

                // add new note to the list of active notes
                tActiveNotesList.AddNote(pCurrentNote, rCurrentNoteAbsTime + pCurrentNote->GetDuration());

            } //  if (pSO->IsNote())

        } // [else if (!pSO->IsNoteRest()) ] ignore other staff objects

        pIter->MoveNext();

    } // while
    delete pIter;       //Do not forget this. We are not using smart pointers!

    // Analyze the remaining notes
    //
    tActiveNotesList.RecalculateActiveNotes( );

    // TODO: not sure if this is the correct way of getting the key here...
    // Get the key
    if (pCurrentNote->GetApplicableKeySignature())
        nKey = pCurrentNote->GetApplicableKeySignature()->GetKeyType();
    else
        nKey = earmDo;

    // Get the notes
    int nNumActiveNotes = tActiveNotesList.GetNotes(&pPossibleChordNotes[0]);

    // Sort the notes
    SortChordNotes(nNumActiveNotes, &pPossibleChordNotes[0]);

    //
    // Get the chord from the notes
    //
    tChordDescriptor[nNumChords] = new lmScoreChord
        (nNumActiveNotes, &pPossibleChordNotes[0], nKey);

    if (!tChordDescriptor[nNumChords]->IsStandardChord())
    {
        wxColour colour = wxColour(255,0,0,128); // R, G, B, Transparency
        wxString sStr = tChordDescriptor[nNumChords]->ToString();
        pInfoBox->DisplayChordInfo(pScore, tChordDescriptor[nNumChords], colour, sStr);
        nBadChords++;
    }
    nNumChords++;

    ChordInfoBox* pChordErrorBoxD;
    if (nHarmonyExerciseChordsToCheck && nHarmonyExcerciseType == 3)
        pChordErrorBoxD = 0; // do not display chord errors
    else
        pChordErrorBoxD = pChordErrorBox;

    int nNumErrors = AnalyzeChordsLinks(&tChordDescriptor[0], nNumChords, pChordErrorBoxD);

    wxLogMessage(_T("ANALYSIS RESULT of %d chords:  bad: %d, Num Errors:%d ")
        ,nNumChords, nBadChords, nNumErrors);

    if (nBadChords == 0 && nNumErrors == 0)
    {
       wxString sOkMsg = _T(" Harmony is OK.");
       wxLogMessage( sOkMsg );
       pInfoBox->DisplayChordInfo(pScore, tChordDescriptor[nNumChords-1], *wxGREEN, sOkMsg );
       fScoreModified = true; // repaint
    }

    // todo: where to locate the box?  restart: pChordErrorBox->SetYPosition(-200);

    ////////////////////////////
    //
    // Exercise specific checks
    //
    ////////////////////////////

    if (nHarmonyExerciseChordsToCheck)
    {
        int nExerciseErrors = nNumErrors;

        wxLogMessage(_T(" *** HARMONY EXERCISE CHECK. Type: %d, Chords:%d ***")
            , nHarmonyExcerciseType, nHarmonyExerciseChordsToCheck);

        // Check: total number of chords
        if ( nHarmonyExerciseChordsToCheck != nNumChords)
        {
            wxString sMsg = wxString::Format(
                _T("Missing chords; Expected:%d, Actual: %d")
                ,nNumChords, nHarmonyExerciseChordsToCheck);
            wxColour colour = wxColour(255,10,0,128); // R, G, B, Transparency: RED
            pChordErrorBox->DisplayChordInfo(pScore, tChordDescriptor[nNumChords-1], colour, sMsg);
            wxLogMessage(_T(" Error: %s"), sMsg.c_str() );
        }

        // For exercise 3: read the FiguredBass introduced by the user
        //
        lmSOIterator* pIter = pVStaff->CreateIterator();
        while(!pIter->EndOfCollection())
        {
            // process only notes, rests and barlines
            lmStaffObj* pSO = pIter->GetCurrent();
            if (pSO->IsFiguredBass())
            {
                pHE_UserFiguredBass[gnHE_NumUserFiguredBass++] = (lmFiguredBass*) pSO;
            }
            pIter->MoveNext();
        }
        delete pIter;       //Do not forget this. We are not using smart pointers!

        lmEChordType nChordType;
        int nNotePitch;
        int nInversions;
        // Check
        for (int nChordCount=0;
            nChordCount<nNumChords && nChordCount<nMAX_HARMONY_EXERCISE_CHORDS && nChordCount < nHarmonyExerciseChordsToCheck;
            nChordCount++)
        {
            nChordType = tChordDescriptor[nChordCount]->GetChordType();
            nInversions = tChordDescriptor[nChordCount]->GetInversion();
            wxLogMessage(_T("Chord %d [%s] Type:%d, %d inversions")
                ,nChordCount+1, tChordDescriptor[nChordCount]->ToString().c_str()
                , nChordType, nInversions, pHE_Chords[nChordCount]->GetChordType());

            // if chord is not valid, no need to say anything: the error message was already shown
            if ( nChordType != lmINVALID_CHORD_TYPE )
            {
                // Check: inversions
                //  todo: consider to allow inversions as an option
                if ( (!bInversionsAllowedInHarmonyExercises) && nInversions > 0)
                {
                    nExerciseErrors++;
                    wxString sMsg = wxString::Format(
                        _("Chord %d [%s] is not at root position: %d inversions")
                        ,nChordCount+1
                        , tChordDescriptor[nChordCount]->ToString().c_str()
                        ,nInversions);
                    wxColour colour = wxColour(255,10,0,128); // R, G, B, Transparency: RED
                    pChordErrorBox->DisplayChordInfo(pScore, tChordDescriptor[nChordCount], colour, sMsg);
                }
                //@@@@@@@@@@@@@@@@@@@@@@@@@
                // todo: the check of chord type is not necessary. Just check that the chord is valid
                //        and the bass/soprano notes
                //       Remove completely the use of chord type?
                //       We leave it in just for debug.
                if ( nChordType != pHE_Chords[nChordCount]->GetChordType())
                {
                    nExerciseErrors++;
                    wxString sMsg = wxString::Format(
                    _("Chord %d [%s]: The type is %s, but the expected was: %s")
                    ,nChordCount+1, tChordDescriptor[nChordCount]->ToString().c_str()
                    , lmChordTypeToName( nChordType ).c_str()
                    , lmChordTypeToName( pHE_Chords[nChordCount]->GetChordType() ).c_str()
                        );
                    // A different chord type with no inversions implies a different root note (todo: confirm this)
                    if (nInversions == 0)
                      sMsg += _(", Wrong root note?");

                    wxColour colour = wxColour(255,10,0,128); // R, G, B, Transparency: RED
                    //todo: remove; no necessary  pChordErrorBox->DisplayChordInfo(pScore, &tChordDescriptor[nChordCount], colour, sMsg);
                    wxLogMessage(_T(" Error: %s"), sMsg.c_str() );
                }
               /*-- todo: remove this "else"; only for debugging
                else
                {
                    wxColour colour = wxColour(10,255,0,128); // R, G, B, Transparency: GREEN
                    pInfoBox->DisplayChordInfo(pScore, &tChordDescriptor[nChordCount], colour, sMsg);
                } --*/
            }

            wxLogMessage( _T(" Chord %d has %d notes ")
                , nChordCount+1
                , tChordDescriptor[nChordCount]->GetNumNotes() );
            for (int nxc=0; nxc<tChordDescriptor[nChordCount]->GetNumNotes(); nxc++)
            {
                wxLogMessage( _T(" NOTE %d: %s")
                    , nxc, tChordDescriptor[nChordCount]->GetNote(nxc)->GetPrintName().c_str() );
            }

            /*-- TODO REMOVE; not clear...
            // CHECK BASS NOTE
            if ( nHarmonyExcerciseType == 1 )
            {
                // Check bass note: consider also the octave
                //  The bass note must match exactly the original generated by lenmus
                nNotePitch = tChordDescriptor[nChordCount]->GetNote(0)->GetFPitch();
                wxString sMsg = wxString::Format( _("Chord %d [%s]: bass note:%s, expected:%s ")
                    , nChordCount+1
                    , tChordDescriptor[nChordCount]->ToString()
                    , FPitch_ToAbsLDPName(nNotePitch).c_str()
                    , FPitch_GetEnglishNoteName(nHE_NotesFPitch[nChordCount][nBassVoiceIndex]).c_str()
                        );
                wxLogMessage( sMsg );
                // compare the notes, but just the step, not the absolute pitch
                if ( nNotePitch !=  nHE_NotesFPitch[nChordCount][nBassVoiceIndex] )
                {
                    nExerciseErrors++;
                    wxColour colour = wxColour(255,10,0,128);
                    pChordErrorBox->DisplayChordInfo(pScore, tChordDescriptor[nChordCount], colour, sMsg);
                    wxLogMessage(_T(" Error: %s"), sMsg.c_str() );
                }
            } --*/
            if ( nHarmonyExcerciseType == 2 )
            {
                // Check bass note: but IGNORE the OCTAVE
                //  (follow the traditional chord rules)
                nNotePitch = tChordDescriptor[nChordCount]->GetNote(0)->GetFPitch();
                wxString sMsg = wxString::Format( _("Chord %d [%s]: bass note:%s, expected:%s ")
                    , nChordCount+1
                    , tChordDescriptor[nChordCount]->ToString().c_str()
                    , FPitch_GetEnglishNoteName(nNotePitch).c_str()
                    , FPitch_GetEnglishNoteName(nHE_NotesFPitch[nChordCount][nBassVoiceIndex]).c_str()
                        );
                wxLogMessage( sMsg );
                // compare the notes, but just the step, not the absolute pitch
                if ( FPitch_Step(nNotePitch) !=  FPitch_Step(nHE_NotesFPitch[nChordCount][nBassVoiceIndex]) )
                {
                    nExerciseErrors++;
                    wxColour colour = wxColour(255,10,0,128);
                    pChordErrorBox->DisplayChordInfo(pScore, tChordDescriptor[nChordCount], colour, sMsg);
                    wxLogMessage(_T(" Error: %s"), sMsg.c_str() );
                }
            }
            // Exercise 2: check also the soprano note...
            if ( nHarmonyExcerciseType == 2 )
            {
                if (tChordDescriptor[nChordCount]->GetNumNotes() <= 3)
                {
                    wxString sMsg = wxString::Format( _("Chord %d [%s], Soprano voice is missing")
                        , nChordCount+1, tChordDescriptor[nChordCount]->ToString().c_str());
                }
                else
                {
                    // CHECK SOPRANO NOTE ( index: 3 )
                    nNotePitch = tChordDescriptor[nChordCount]->GetNote(3)->GetFPitch();
                    wxString sMsg = wxString::Format( _("Chord %d [%s]: soprano note:%s, expected:%s  ")
                        , nChordCount+1
                        , tChordDescriptor[nChordCount]->ToString().c_str()
                        , FPitch_ToAbsLDPName(nNotePitch).c_str()
                        , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nSopranoVoiceIndex]).c_str()
                            );
                    wxLogMessage( sMsg );
                    //  The soprano note must match exactly the original generated by lenmus
                    if ( nNotePitch != nHE_NotesFPitch[nChordCount][nSopranoVoiceIndex] )
                    {
                        nExerciseErrors++;
                        wxColour colour = wxColour(255,10,0,128);
                        pChordErrorBox->DisplayChordInfo(pScore, tChordDescriptor[nChordCount], colour, sMsg);
                        wxLogMessage(_T(" Error: %s"), sMsg.c_str() );
                    }
                }
           }


        }

        // EXERCISE 3
        if ( nHarmonyExcerciseType == 3 )
        {
            wxString sMsg = wxString::Format(
                _("Exercise 3: Number of figured bass introduced by user:%d, expected:%d")
                , gnHE_NumUserFiguredBass,  nNumChords);
            wxLogMessage(sMsg.c_str());

            // Check chord count
            if ( gnHE_NumUserFiguredBass != nNumChords )
            {
                nExerciseErrors++;
                wxColour colour = wxColour(255,10,0,128);
                pChordErrorBox->DisplayChordInfo(pScore, tChordDescriptor[nNumChords-1], colour, sMsg);
                wxLogMessage( _("DISPLAYED ERROR :%s"), sMsg.c_str());
            }

            // check each Figured bass
            for (int nFB=0; nFB<gnHE_NumUserFiguredBass && nFB<nNumChords; nFB++)
            {
                sMsg = wxString::Format(
                    _("Chord %d [%s] Figured bass: user: {%s}, expected:{%s}")
                    , nFB+1
                    , tChordDescriptor[nFB]->ToString().c_str()
                    , pHE_UserFiguredBass[nFB]->GetFiguredBassString().c_str()
                    , pHE_FiguredBass[nFB]->GetFiguredBassString().c_str()
                    );
                wxLogMessage(sMsg.c_str());
                if (    pHE_UserFiguredBass[nFB]->GetFiguredBassString()
                        !=  pHE_FiguredBass[nFB]->GetFiguredBassString()
                    )
                {
                    nExerciseErrors++;
                    wxColour colour = wxColour(255,10,0,128);
                    pChordErrorBox->DisplayChordInfo(pScore, tChordDescriptor[nFB], colour, sMsg);
                }
            }

        }

        wxLogMessage( _T(" Exercise errors: %d"), nExerciseErrors);

        if (nExerciseErrors > 0)
        {
         // TODO: remove; not important
         //   wxString sOkMsg = wxString::Format( _(" Exercise errors: %d"), nExerciseErrors);
         //   pInfoBox->DisplayChordInfo(pScore, &tChordDescriptor[nNumChords-1], *wxRED, sOkMsg );
        }
        else
        {
           wxString sOkMsg = _T(" Exercise is OK.");
           pInfoBox->DisplayChordInfo(pScore, tChordDescriptor[nNumChords-1], *wxGREEN, sOkMsg );
        }

        // TODO @@@@@@@@@@@@@@ DELETE
        // pHE_Chords pHE_UserFiguredBass  pHE_Notes pgHE_FiguredBass
        for (int i = 0; i <nHarmonyExerciseChordsToCheck; i++)
        {
            if (pHE_Chords[i] != NULL)
            {
               wxLogMessage( _T(" DELETED pHE_Chords[%d]"), i);
               delete pHE_Chords[i];
            }
            if (pHE_FiguredBass[i] != NULL)
            {
               wxLogMessage( _T(" DELETED pHE_FiguredBass[%d]"), i);
               delete pHE_FiguredBass[i];
            }
            // pHE_UserFiguredBass: is a copy; do not delete
            // pHE_Notes: is a copy; do not delete
        }

        nHarmonyExerciseChordsToCheck = 0;

    }

    //clean
/* TODO: REMOVE
    assert(nNumChords<lmMAX_NUM_CHORDS);
    wxLogMessage(_T(" Deleting %d chords"), nNumChords );
    for (int i = 0; i <nNumChords; i++)
        delete tChordDescriptor[i].pChord;
--*/




    return fScoreModified;      //true -> score modified

}

/*- TODO REVOME (moved to Harmony)
// return: total number of errors
int lmHarmonyProcessor::AnalyzeChordsLinks(lmScoreChord** pChordDescriptor, int nNCH, bool fDisplay)
{
    ChordInfoBox* pChordErrorBoxD = (fDisplay? pChordErrorBox : 0); // show errors?

    wxLogMessage(_T("AnalyzeChordsLinks N:%d "), nNCH);

    int nNumChordError[lmMAX_NUM_CHORDS]; // number of errors in each chord

    lmRuleList tRules(pChordDescriptor, nNCH);

    wxString sStr;
    sStr.clear();

    int nNumErros = 0; // TODO: decide: total num of errors or num of chords with error
    lmRule* pRule;
    // TODO: create a method of the list to evaluate ALL the rules ?
    for (int nR = lmCVR_FirstChordValidationRule; nR<lmCVR_LastChordValidationRule; nR++)
    {
        pRule = tRules.GetRule(nR);
        if ( pRule == NULL)
        {
//todo: remove this message?            wxLogMessage(_T(" Rule %d is NULL !!!"), nR);
        }
        else if (pRule->IsEnabled())
        {
            wxLogMessage(_T("Evaluating rule %d, description: %s")
                , pRule->GetRuleId(), pRule->GetDescription().c_str());
            nNumErros += pRule->Evaluate(sStr, &nNumChordError[0], pChordErrorBoxD);
            wxLogMessage(_T("   Total error count after rule %d: %d errors"), pRule->GetRuleId(), nNumErros   );
            if (nNumErros > 0)
            {
              // TODO: anything else here?
            }
        }
    }

    return nNumErros;
} --*/

