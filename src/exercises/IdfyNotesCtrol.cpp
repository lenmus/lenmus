//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
#pragma implementation "IdfyNotesCtrol.h"
#endif

// For compilers that support precompilation, includes <wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "IdfyNotesCtrol.h"

#include "../score/VStaff.h"
#include "../score/Instrument.h"
#include "../score/Pitch.h"
#include "Constrains.h"
#include "Generators.h"
#include "NotesConstrains.h"
#include "../ldp_parser/LDPParser.h"
#include "dialogs/DlgCfgIdfyNotes.h"
#include "../auxmusic/ScalesManager.h"
#include "auxctrols/UrlAuxCtrol.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"


//------------------------------------------------------------------------------------
// Implementation of lmIdfyNotesCtrol




//IDs for controls
enum {
    lmID_PLAY_ALL_NOTES = 3010,
    lmID_PLAY_A4,
    lmID_CONTINUE,
    lmID_BUTTON,
};

//exercise states
enum {
    lmE_StateStart = 0,
    lmE_PlayNote,
};


BEGIN_EVENT_TABLE(lmIdfyNotesCtrol, lmOneScoreCtrol)
    EVT_COMMAND_RANGE(lmID_BUTTON, lmID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmIdfyNotesCtrol::OnRespButton)
    LM_EVT_URL_CLICK(lmID_PLAY_A4, lmIdfyNotesCtrol::OnPlayA4)
    LM_EVT_URL_CLICK(lmID_PLAY_ALL_NOTES, lmIdfyNotesCtrol::OnPlayAllNotes)
    LM_EVT_URL_CLICK(lmID_CONTINUE, lmIdfyNotesCtrol::OnContinue)
END_EVENT_TABLE()


lmIdfyNotesCtrol::lmIdfyNotesCtrol(wxWindow* parent, wxWindowID id,
                           lmNotesConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(400,140), pos, size, style )
    , m_state(lmE_StateStart)
{
    //initializations
    m_pConstrains = pConstrains;

    ////initializatios to allow to play cadences when clicking on answer buttons
    ////TODO: Review this
    //m_nKey = earmDo;

    CreateControls();

    MoveToInitialState();
}

lmIdfyNotesCtrol::~lmIdfyNotesCtrol()
{
}

void lmIdfyNotesCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{
    //a sizer for extra links
    wxBoxSizer* pPlayRefSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(
        pPlayRefSizer,
        wxSizerFlags(0).Center().Border(wxBOTTOM, 3*nSpacing) );

    // "Play all notes to identify" link
    m_pPlayAllNotes = new lmUrlAuxCtrol(this, lmID_PLAY_ALL_NOTES, m_rScale,
                                        _("Play all notes to identify"), _T(""),
                                        _("Stop playing"), _T("") );
    pPlayRefSizer->Add(
        m_pPlayAllNotes,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 4*nSpacing) );

    // "Play A4 reference note" link
    m_pPlayA4 = new lmUrlAuxCtrol(this, lmID_PLAY_A4, m_rScale,
                                        _("Play A4 reference note"), _T(""),
                                        _("Stop playing"), _T("") );
    pPlayRefSizer->Add(
        m_pPlayA4,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 4*nSpacing) );

    // "Continue" link
    m_pContinue = new lmUrlAuxCtrol(this, lmID_CONTINUE, m_rScale,
                                        _("Continue"), _T(""));
    pPlayRefSizer->Add(
        m_pContinue,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 4*nSpacing) );

    //create answer buttons
    for (int iB=0; iB < m_NUM_BUTTONS; iB++)
    {
        m_pAnswerButton[iB] = new wxButton( this, lmID_BUTTON + iB, m_pButtonLabel[iB],
            wxDefaultPosition, wxSize(16*nSpacing, nHeight));
        m_pAnswerButton[iB]->SetFont(font);
    }

    //main sizer: a grid with two rows and one column
    m_pKeyboardSizer = new wxFlexGridSizer(2, 1, 0, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );

    //first row: sharp/flat notes
	wxBoxSizer* pRowOneSizer = new wxBoxSizer( wxHORIZONTAL );
	pRowOneSizer->Add( 0, 0, 1, wxEXPAND, nSpacing );
	pRowOneSizer->Add( m_pAnswerButton[1], 0, wxALL, nSpacing );
	pRowOneSizer->Add( m_pAnswerButton[3], 0, wxALL, nSpacing );
	pRowOneSizer->Add( 0, 0, 1, wxEXPAND, nSpacing );
	pRowOneSizer->Add( m_pAnswerButton[6], 0, wxALL, nSpacing );
	pRowOneSizer->Add( m_pAnswerButton[8], 0, wxALL, nSpacing );
	pRowOneSizer->Add( m_pAnswerButton[10], 0, wxALL, nSpacing );
	pRowOneSizer->Add( 0, 0, 1, wxEXPAND, nSpacing );
	m_pKeyboardSizer->Add( pRowOneSizer, 0, wxEXPAND, nSpacing );

    //second row: natural notes
	wxBoxSizer* pRowTwoSizer = new wxBoxSizer( wxHORIZONTAL );
    pRowTwoSizer->Add( m_pAnswerButton[0], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[2], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[4], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[5], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[7], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[9], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[11], 0, wxALL, nSpacing );
	m_pKeyboardSizer->Add( pRowTwoSizer, 0, wxEXPAND, nSpacing );

    //inform base class about the settings
    SetButtons(m_pAnswerButton, m_NUM_BUTTONS, lmID_BUTTON);
}

void lmIdfyNotesCtrol::InitializeStrings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

        // button labels
    m_pButtonLabel[0] = _("C/B#");
    m_pButtonLabel[1] = _("C#/Db");
    m_pButtonLabel[2] = _("D");
    m_pButtonLabel[3] = _("D#/Eb");
    m_pButtonLabel[4] = _("E/Fb");
    m_pButtonLabel[5] = _("F/E#");
    m_pButtonLabel[6] = _("F#/Gb");
    m_pButtonLabel[7] = _("G");
    m_pButtonLabel[8] = _("G#/Ab");
    m_pButtonLabel[9] = _("A");
    m_pButtonLabel[10] = _("A#/Bb");
    m_pButtonLabel[11] = _("B/Cb");
}

void lmIdfyNotesCtrol::OnSettingsChanged()
{
    // The settings have been changed. Set button labels as needed

    if (m_pConstrains->SelectNotesFromKeySignature())
    {
        int nAcc[7];
        lmComputeAccidentals(m_pConstrains->GetKeySignature(), nAcc);
        m_pAnswerButton[0]->SetLabel(nAcc[0] == 0 ? _("C") : _("B #"));
        m_pAnswerButton[1]->SetLabel(nAcc[0] == 1 ? _("C #") : _("D b"));
        m_pAnswerButton[2]->SetLabel( _("D") );
        m_pAnswerButton[3]->SetLabel(nAcc[1] == 1 ? _("D #") : _("E b"));
        m_pAnswerButton[4]->SetLabel(nAcc[2] == 0 ? _("E") : _("F b"));
        m_pAnswerButton[5]->SetLabel(nAcc[3] == 0 ? _("F") : _("E #"));
        m_pAnswerButton[6]->SetLabel(nAcc[3] == 1 ? _("F #") : _("G b"));
        m_pAnswerButton[7]->SetLabel( _("G") );
        m_pAnswerButton[8]->SetLabel(nAcc[4] == 1 ? _("G #") : _("A b"));
        m_pAnswerButton[9]->SetLabel( _("A") );
        m_pAnswerButton[10]->SetLabel(nAcc[5] == 1 ? _("A #") : _("B b"));
        m_pAnswerButton[11]->SetLabel(nAcc[6] == 0 ? _("B") : _("C b"));
    }
    else
    {
        for (int iB=0; iB < m_NUM_BUTTONS; iB++)
        {
            m_pAnswerButton[iB]->SetLabel( m_pButtonLabel[iB] );
        }
    }

    //if (m_pConstrains->StartWithA4())
    //    m_pPlayReference->SetNormalLabel(_("Play A4 reference note"));
    //else
    //    m_pPlayReference->SetNormalLabel(_("Play all notes to identify"));

    EnableButtons(true);
}

void lmIdfyNotesCtrol::EnableButtons(bool fEnable)
{
    //enable/disable valid buttons
    for (int iB=0; iB < m_NUM_BUTTONS; iB++)
    {
        m_pAnswerButton[iB]->Enable( fEnable && m_pConstrains->IsValidNote(iB) );
    }
}

wxDialog* lmIdfyNotesCtrol::GetSettingsDlg()
{
    wxDialog* pDlg = new lmDlgCfgIdfyNotes(this, m_pConstrains);
    return pDlg;
}

void lmIdfyNotesCtrol::PrepareAuxScore(int nButton)
{
    //PrepareScore(lmE_Sol, m_nStartCadence[nButton], &m_pAuxScore);
}

wxString lmIdfyNotesCtrol::SetNewProblem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex and m_nPlayMM

    wxString sNote;
    lmRandomGenerator oGenerator;
    lmEClefType nClef = m_pConstrains->GetClef();

    //select octave
    int nSecondOctave = m_pConstrains->GetOctaves() == 2 ? oGenerator.FlipCoin() : 0;
    int nOctave;
    switch (nClef)
    {
        case lmE_Sol:   nOctave = 4 + nSecondOctave;    break;  //4,5
        case lmE_Fa4:   nOctave = 3 - nSecondOctave;    break;  //3,2
        case lmE_Fa3:   nOctave = 3 - nSecondOctave;    break;  //3,2
        case lmE_Do1:   nOctave = 4 - nSecondOctave;    break;  //4,3
        case lmE_Do2:   nOctave = 4 - nSecondOctave;    break;  //4,3
        case lmE_Do3:   nOctave = 4 - nSecondOctave;    break;  //4,3
        case lmE_Do4:   nOctave = 3 + nSecondOctave;    break;  //3,4
    }

    // generate a random note
    int nNoteIndex;
    if (m_pConstrains->SelectNotesFromKeySignature())
    {
        m_nKey = m_pConstrains->GetKeySignature();

        //Generate a random root note
        wxString sRootNote = oGenerator.GenerateRandomRootNote(nClef, m_nKey, false);  //false = do not allow accidentals. Only those in the key signature
        lmScalesManager oScaleMngr(sRootNote, est_MajorNatural, m_nKey);

        //generate a random note
        int iN = oGenerator.RandomNumber(0, 6);
        lmFPitch fpNote = oScaleMngr.GetNote(iN);
        int nStep = FPitch_Step(fpNote);
        int nAcc = FPitch_Accidentals(fpNote);
        switch(nStep)
        {
            case 0: nNoteIndex = 0; break;
            case 1: nNoteIndex = 2; break;
            case 2: nNoteIndex = 4; break;
            case 3: nNoteIndex = 5; break;
            case 4: nNoteIndex = 7; break;
            case 5: nNoteIndex = 9; break;
            case 6: nNoteIndex = 11; break;
        }
        nNoteIndex += nAcc;
        if (nNoteIndex < 0)
            nNoteIndex += 12;
        sNote = FPitch_ToAbsLDPName( FPitch(nStep, nOctave, nAcc) );
    }
    else
    {
        nNoteIndex = m_pConstrains->GetRandomNoteIndex();
        wxString sNotePitch[12] = { _T("c4"), _T("+c4"), _T("d4"), _T("+d4"), _T("e4"),
            _T("f4"), _T("+f4"), _T("g4"), _T("+g4"), _T("a4"), _T("+a4"),  _T("b4") };
        sNote = sNotePitch[nNoteIndex];
    }
    m_nKey = earmDo;

    //create the score
    if (m_pConstrains->IsTheoryMode())
        m_sAnswer = PrepareScore(nClef, sNote, &m_pProblemScore);
    else
        m_sAnswer = PrepareScore(nClef, sNote, &m_pProblemScore, &m_pSolutionScore);

	//compute the index for the button that corresponds to the right answer
    m_nRespIndex = nNoteIndex;

    //return _("Identify the next note:");
    return ShowAllNotesScore();

}

wxString lmIdfyNotesCtrol::PrepareScore(lmEClefType nClef, wxString& sNotePitch,
                                           lmScore** pProblemScore,
                                           lmScore** pSolutionScore)
{
    //delete the previous score
    if (*pProblemScore) {
        delete *pProblemScore;
        *pProblemScore = (lmScore*)NULL;
    }
    if (pSolutionScore) {
        delete *pSolutionScore;
        *pSolutionScore = (lmScore*)NULL;
    }

    //create the score with the note
    wxString sPattern;
    lmNote* pNote;
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    *pProblemScore = new_score();
    (*pProblemScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    lmInstrument* pInstr = (*pProblemScore)->AddInstrument(g_pMidi->DefaultVoiceChannel(),
							g_pMidi->DefaultVoiceInstr(), _T(""));
    pVStaff = pInstr->GetVStaff();
    (*pProblemScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature( m_nKey );
    pVStaff->AddTimeSignature(2 ,4);

    // Now add the note to identify
    sPattern = _T("(n ") + sNotePitch + _T(" w)");
    pNode = parserLDP.ParseText( sPattern );
    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);

    ////Prepare Solution Score
    //if (pSolutionScore) {
    //    *pSolutionScore = new lmScore();
    //    (*pSolutionScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    //    lmInstrument* pInstr = (*pSolutionScore)->AddInstrument(g_pMidi->DefaultVoiceChannel(),
				//			    g_pMidi->DefaultVoiceInstr(), _T(""));
    //    pVStaff = pInstr->GetVStaff();
    //    (*pSolutionScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    //    pVStaff->AddStaff(5);                       //add second staff: five lines, standard size
    //    pVStaff->AddClef( lmE_Sol, 1 );
    //    pVStaff->AddClef( lmE_Fa4, 2 );
    //    pVStaff->AddKeySignature( m_nKey );
    //    pVStaff->AddTimeSignature(2 ,4);

    //    // Loop to add chords
    //    for (int iC=0; iC < oCad.GetNumChords(); iC++)
    //    {
    //        pVStaff->AddSpacer(15);
    //        if (iC != 0) pVStaff->AddBarline(lm_eBarlineSimple);
    //        // first and second notes on F4 clef staff
    //        sPattern = _T("(n ") + oCad.GetNotePattern(iC, 0) + _T(" w p2)");
    //    //wxLogMessage(_T("[lmIdfyNotesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
    //        pNode = parserLDP.ParseText( sPattern );
    //        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    //        sPattern = _T("(na ") + oCad.GetNotePattern(iC, 1) + _T(" w p2)");
    //    //wxLogMessage(_T("[lmIdfyNotesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
    //        pNode = parserLDP.ParseText( sPattern );
    //        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    //        // third and fourth notes on G clef staff
    //        sPattern = _T("(na ") + oCad.GetNotePattern(iC, 2) + _T(" w p1)");
    //    //wxLogMessage(_T("[lmIdfyNotesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
    //        pNode = parserLDP.ParseText( sPattern );
    //        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    //        sPattern = _T("(na ") + oCad.GetNotePattern(iC, 3) + _T(" w p1)");
    //    //wxLogMessage(_T("[lmIdfyNotesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
    //        pNode = parserLDP.ParseText( sPattern );
    //        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    //    }
    //    pVStaff->AddSpacer(20);
    //    pVStaff->AddBarline(lm_eBarlineEnd);
    //}

    ////return cadence name
    //return  oCad.GetName();

    return _("The note is: ") + sNotePitch;

}

void lmIdfyNotesCtrol::OnPlayA4(wxCommandEvent& event)
{
    wxMessageBox(_T("OnPlayA4"));
}

void lmIdfyNotesCtrol::OnPlayAllNotes(wxCommandEvent& event)
{
    wxMessageBox(_T("OnPlayAllNotes"));
}

void lmIdfyNotesCtrol::OnContinue(wxCommandEvent& event)
{
    wxMessageBox(_T("OnContinue"));
}

int lmIdfyNotesCtrol::GetFirstOctaveForClef(lmEClefType nClef)
{
    switch (nClef)
    {
        case lmE_Sol:   return 4;
        case lmE_Fa4:   return 3;
        case lmE_Fa3:   return 3;
        case lmE_Do1:   return 4;
        case lmE_Do2:   return 4;
        case lmE_Do3:   return 4;
        case lmE_Do4:   return 3;
        default:
            return 4;
    }
}

wxString lmIdfyNotesCtrol::ShowAllNotesScore()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex and m_nPlayMM

    lmEClefType nClef = m_pConstrains->GetClef();

    //select octave
    int nFirstOctave = GetFirstOctaveForClef(nClef);
    int nSecondOctave = nFirstOctave;
    bool fTwoOctaves = (m_pConstrains->GetOctaves() == 2);
    if (fTwoOctaves)
    {
        switch (nClef)
        {
            case lmE_Sol:   nSecondOctave += 1;    break;  //4,5
            case lmE_Fa4:   nSecondOctave -= 1;    break;  //3,2
            case lmE_Fa3:   nSecondOctave -= 1;    break;  //3,2
            case lmE_Do1:   nSecondOctave -= 1;    break;  //4,3
            case lmE_Do2:   nSecondOctave -= 1;    break;  //4,3
            case lmE_Do3:   nSecondOctave -= 1;    break;  //4,3
            case lmE_Do4:   nSecondOctave += 1;    break;  //3,4
        }
    }

    //create the score
    wxString sPattern;
    lmNote* pNote;
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    lmScore* pScore = new_score();
    pScore->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    lmInstrument* pInstr = pScore->AddInstrument(g_pMidi->DefaultVoiceChannel(),
							g_pMidi->DefaultVoiceInstr(), _T(""));
    pVStaff = pInstr->GetVStaff();
    pScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature( earmDo );
    //pVStaff->AddTimeSignature(2 ,4);

    //generate all valid notes
    for (int i=0; i < 12; i++)
    {
        if (m_pConstrains->IsValidNote(i))
        {
            wxString sNote;
            if (m_pConstrains->SelectNotesFromKeySignature())
            {
                int nStep;
                int nAcc[7];
                lmComputeAccidentals(m_pConstrains->GetKeySignature(), nAcc);

                switch(i)
                {
                    case 0: //C or B#
                        nStep = (nAcc[6] == 1 ? 6 : 0);
                        break;

                    case 1: //C# or Db
                        nStep = (nAcc[1] == -1 ? 1 : 0);
                        break;

                    case 2: //D
                        nStep = 1;
                        break;

                    case 3: //D# or Eb
                        nStep = (nAcc[2] == -1 ? 2 : 1);
                        break;

                    case 4: //E or Fb
                        nStep = (nAcc[3] == -1 ? 3 : 2);
                        break;

                    case 5: //E# or F
                        nStep = (nAcc[2] == 1 ? 2 : 3);
                        break;

                    case 6: //F# or Gb
                        nStep = (nAcc[3] == 1 ? 3 : 4);
                        break;

                    case 7: //G
                        nStep = 4;
                        break;

                    case 8: //G# or Ab
                        nStep = (nAcc[4] == 1 ? 4 : 5);
                        break;

                    case 9: //A
                        nStep = 5;
                        break;

                    case 10: //A# or Bb
                        nStep = (nAcc[5] == 1 ? 5 : 6);
                        break;

                    case 11: //B or Cb
                        nStep = (nAcc[0] == -1 ? 0 : 6);
                        break;
                }
                sNote = FPitch_ToAbsLDPName( FPitch(nStep, nFirstOctave, nAcc[nStep]) );
            }
            else
            {
                int nStep;
                int nAcc = 0;
                switch(i)
                {
                    case 0: //C or B#
                        nStep = 0;
                        break;

                    case 1: //C# or Db
                        nStep = 0;
                        nAcc = 1;
                        break;

                    case 2: //D
                        nStep = 1;
                        break;

                    case 3: //D# or Eb
                        nStep = 1;
                        nAcc = 1;
                        break;

                    case 4: //E or Fb
                        nStep = 2;
                        break;

                    case 5: //E# or F
                        nStep = 2;
                        break;

                    case 6: //F# or Gb
                        nStep = 3;
                        nAcc = 1;
                        break;

                    case 7: //G
                        nStep = 4;
                        break;

                    case 8: //G# or Ab
                        nStep = 4;
                        nAcc = 1;
                        break;

                    case 9: //A
                        nStep = 5;
                        break;

                    case 10: //A# or Bb
                        nStep = 5;
                        nAcc = 1;
                        break;

                    case 11: //B or Cb
                        nStep = 6;
                        break;
                }
                sNote = FPitch_ToAbsLDPName( FPitch(nStep, nFirstOctave, nAcc) );
            }

            //add note
            sPattern = _T("(n ") + sNote + _T(" w)");
            wxLogMessage(sPattern);
            pNode = parserLDP.ParseText( sPattern );
            pNote = parserLDP.AnalyzeNote(pNode, pVStaff);

        }
    }
    pVStaff->AddSpacer(50);
    pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);
    m_pProblemScore = pScore;
    return _("You will have to identify the following notes:");
}

void lmIdfyNotesCtrol::MoveToInitialState()
{
    ResetExercise();

    //display the intro
    m_fQuestionAsked = false;
    //DisplayProblem();
    wxString sMsg = _("Click on 'New problem' to start");
    DisplayMessage(sMsg, false);
    m_pPlayButton->Enable(false);
    m_pShowSolution->Enable(false);
    m_pPlayA4->Enable(false);
    m_pPlayAllNotes->Enable(false);
    m_pContinue->Enable(false);
}
