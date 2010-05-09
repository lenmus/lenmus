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
    lmID_PLAY_ALL_NOTES = 3720,
    lmID_PLAY_A4,
    lmID_CONTINUE,
    lmID_BUTTON,
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
{
    m_pConstrains = pConstrains;
    m_pConstrains->SetTheoryMode(false);
    CreateControls();
    MoveToInitialState();
}

lmIdfyNotesCtrol::~lmIdfyNotesCtrol()
{
}

void lmIdfyNotesCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{
    //Change 'Play' label to 'Play again'
    m_pPlayButton->ChangeNormalLabel(_("Play again"));

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
    m_pButtonLabel[0] = _("C");
    m_pButtonLabel[1] = _("C#/Db");
    m_pButtonLabel[2] = _("D");
    m_pButtonLabel[3] = _("D#/Eb");
    m_pButtonLabel[4] = _("E");
    m_pButtonLabel[5] = _("F");
    m_pButtonLabel[6] = _("F#/Gb");
    m_pButtonLabel[7] = _("G");
    m_pButtonLabel[8] = _("G#/Ab");
    m_pButtonLabel[9] = _("A");
    m_pButtonLabel[10] = _("A#/Bb");
    m_pButtonLabel[11] = _("B");
}

void lmIdfyNotesCtrol::OnSettingsChanged()
{
    // The settings have been changed. Set button labels as needed

    if (m_pConstrains->SelectNotesFromKeySignature())
    {
        int nAcc[7];
        lmComputeAccidentals(m_pConstrains->GetKeySignature(), nAcc);

        wxButton* pNaturalButton[7];
        pNaturalButton[0] = m_pAnswerButton[0];
        pNaturalButton[1] = m_pAnswerButton[2];
        pNaturalButton[2] = m_pAnswerButton[4];
        pNaturalButton[3] = m_pAnswerButton[5];
        pNaturalButton[4] = m_pAnswerButton[7];
        pNaturalButton[5] = m_pAnswerButton[9];
        pNaturalButton[6] = m_pAnswerButton[11];

        static const wxString sSteps = _T("CDEFGAB");
        for (int iB=0; iB < 7; iB++)
        {
            wxString sLabel = sSteps.Mid(iB, 1);
            if (nAcc[iB] == 1)
                sLabel += _T(" #");
            else if (nAcc[iB] == -1)
                sLabel += _T(" b");
            pNaturalButton[iB]->SetLabel( wxGetTranslation(sLabel) );
        }
    }
    else
    {
        for (int iB=0; iB < m_NUM_BUTTONS; iB++)
        {
            m_pAnswerButton[iB]->SetLabel( m_pButtonLabel[iB] );
        }
    }

    EnableButtons(true);
}

void lmIdfyNotesCtrol::EnableButtons(bool fEnable)
{
    //enable/disable valid buttons

    if (m_pConstrains->SelectNotesFromKeySignature())
    {
        m_pAnswerButton[0]->Enable(fEnable);
        m_pAnswerButton[2]->Enable(fEnable);
        m_pAnswerButton[4]->Enable(fEnable);
        m_pAnswerButton[5]->Enable(fEnable);
        m_pAnswerButton[7]->Enable(fEnable);
        m_pAnswerButton[9]->Enable(fEnable);
        m_pAnswerButton[11]->Enable(fEnable);

        m_pAnswerButton[1]->Enable(false);
        m_pAnswerButton[3]->Enable(false);
        m_pAnswerButton[6]->Enable(false);
        m_pAnswerButton[8]->Enable(false);
        m_pAnswerButton[10]->Enable(false);

        m_pAnswerButton[1]->Show(false);
        m_pAnswerButton[3]->Show(false);
        m_pAnswerButton[6]->Show(false);
        m_pAnswerButton[8]->Show(false);
        m_pAnswerButton[10]->Show(false);
    }
    else
    {
        for (int iB=0; iB < m_NUM_BUTTONS; iB++)
        {
            m_pAnswerButton[iB]->Enable( fEnable && m_pConstrains->IsValidNote(iB) );
            m_pAnswerButton[iB]->Show(true);
        }
    }
    m_pMainSizer->Layout();
}

wxDialog* lmIdfyNotesCtrol::GetSettingsDlg()
{
    wxDialog* pDlg = new lmDlgCfgIdfyNotes(this, m_pConstrains);
    return pDlg;
}

void lmIdfyNotesCtrol::PrepareAuxScore(int nButton)
{
    if (m_pAuxScore)
        delete m_pAuxScore;
    m_pAuxScore = NULL;
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

    // generate a random note and set m_nKey, nStep, nAcc and nNoteIndex
    int nNoteIndex;
    int nStep;
    int nAcc;
    if (m_pConstrains->SelectNotesFromKeySignature())
    {
        m_nKey = m_pConstrains->GetKeySignature();

        //Generate a random root note
        wxString sRootNote = oGenerator.GenerateRandomRootNote(nClef, m_nKey, false);  //false = do not allow accidentals. Only those in the key signature
        lmScalesManager oScaleMngr(sRootNote, est_MajorNatural, m_nKey);

        //generate a random note, different from previous one
        static int iPrev = 0;
        int iN = oGenerator.RandomNumber(0, 6);
        while (iPrev == iN)
            iN = oGenerator.RandomNumber(0, 6);
        iPrev = iN;

        lmFPitch fpNote = oScaleMngr.GetNote(iN);
        nStep = FPitch_Step(fpNote);
        nAcc = FPitch_Accidentals(fpNote);
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
    }
    else
    {
        m_nKey = earmDo;
        nNoteIndex = m_pConstrains->GetRandomNoteIndex();
        static const int nNoteStep[12] = { 0,0,1,1,2,3,3,4,4,5,5,6 };
        static const int nNoteAcc[12] = { 0,1,0,1,0,0,1,0,1,0,1,0 };
        nStep = nNoteStep[nNoteIndex];
        nAcc = nNoteAcc[nNoteIndex];
    }
    sNote = FPitch_ToAbsLDPName( FPitch(nStep, nOctave, nAcc) );

    //prepare answer
    static const wxString sSteps = _T("CDEFGAB");
    wxString sAnswer = sSteps.Mid(nStep, 1);
    if (nAcc == 1)
        sAnswer += _T(" sharp");
    else if (nAcc == -1)
        sAnswer += _T(" flat");
    m_sAnswer = _("The note is: ");
    m_sAnswer += wxGetTranslation(sAnswer);
    m_sAnswer += _T("\n");
    m_sAnswer += _("Click on 'Continue' to listen a new note");

    //create the score
    if (m_pConstrains->IsTheoryMode())
        PrepareScore(nClef, sNote, &m_pProblemScore);
    else
        PrepareScore(nClef, sNote, &m_pProblemScore, &m_pSolutionScore);

	//compute the index for the button that corresponds to the right answer
    m_nRespIndex = nNoteIndex;

    return _("Identify the next note:");
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
    (*pProblemScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(50, 1) );     // 5 lines
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature( m_nKey );
    pVStaff->AddTimeSignature(2 ,4);

    // Now add the note to identify
    sPattern = _T("(n ") + sNotePitch + _T(" w)");
    pNode = parserLDP.ParseText( sPattern );
    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);

    return sNotePitch;
}

void lmIdfyNotesCtrol::OnPlayA4(wxCommandEvent& event)
{
    StopSounds();

    //delete any previous score
    if (m_pAuxScore)
        delete m_pAuxScore;

    //create an score with the A4 note
    m_pAuxScore = new_score();
    lmInstrument* pInstr = m_pAuxScore->AddInstrument(g_pMidi->DefaultVoiceChannel(),
							g_pMidi->DefaultVoiceInstr(), _T(""));
    lmVStaff* pVStaff = pInstr->GetVStaff();
    pVStaff->AddClef( lmE_Sol );
    pVStaff->AddKeySignature( earmDo );
    pVStaff->AddTimeSignature(2 ,4);
    lmLDPParser parser;
    lmLDPNode* pNode = parser.ParseText( _T("(n a4 w)") );
    parser.AnalyzeNote(pNode, pVStaff);

    m_pAuxScore->Play(lmNO_VISUAL_TRACKING, lmNO_COUNTOFF,
                      ePM_NormalInstrument, m_nPlayMM, (wxWindow*) NULL);
}

void lmIdfyNotesCtrol::OnPlayAllNotes(wxCommandEvent& event)
{
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->PlayScore(lmVISUAL_TRACKING, lmNO_COUNTOFF,
                            ePM_NormalInstrument, m_nPlayMM);
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

void lmIdfyNotesCtrol::PrepareAllNotesScore()
{
    //This method prepares a score with all the notes to identify and
    //stores it in m_pProblemScore

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
            wxLogMessage(_T("[lmIdfyNotesCtrol::PrepareAllNotesScore] Note=%s"),
                         sPattern.c_str());
            pNode = parserLDP.ParseText( sPattern );
            pNote = parserLDP.AnalyzeNote(pNode, pVStaff);

        }
    }
    pVStaff->AddSpacer(50);
    pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);
    if (m_pProblemScore)
        delete m_pProblemScore;
    m_pProblemScore = pScore;
}

void lmIdfyNotesCtrol::MoveToInitialState()
{
    ResetExercise();

    //display the intro
    m_fQuestionAsked = false;
    wxString sMsg = _("Click on 'New problem' to start");
    DisplayMessage(sMsg, false);
    m_pPlayButton->Enable(false);
    m_pShowSolution->Enable(false);
    m_pPlayA4->Enable(false);
    m_pPlayAllNotes->Enable(false);
    m_pContinue->Enable(false);
}

void lmIdfyNotesCtrol::OnNewProblem(wxCommandEvent& event)
{
    DisplayAllNotes();
    m_pPlayButton->Enable(false);
    m_pShowSolution->Enable(false);
}

void lmIdfyNotesCtrol::DisplayAllNotes()
{
    wxString sProblemMessage = _("You will have to identify the following notes:");
    PrepareAllNotesScore();
    wxLogMessage(_T("[lmIdfyNotesCtrol::DisplayAllNotes] Before displying score"));
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->SetScore(m_pProblemScore);
    DisplayMessage(sProblemMessage, false);
    m_pPlayA4->Enable(true);
    m_pNewProblem->Enable(false);
    m_pPlayAllNotes->Enable(true);
    m_pContinue->Enable(true);
}

void lmIdfyNotesCtrol::OnContinue(wxCommandEvent& event)
{
    ResetExercise();

    //prepare answer buttons and counters
    if (m_pCounters && m_fCountersValid)
        m_pCounters->OnNewQuestion();
    EnableButtons(true);

    //set m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex, m_nPlayMM
    wxString sProblemMessage = SetNewProblem();

    //display the problem
    m_fQuestionAsked = true;
    DisplayProblem();
    DisplayMessage(sProblemMessage, false);

    //enable/disable links
    m_pPlayButton->Enable(true);
    m_pShowSolution->Enable(true);
    m_pPlayA4->Enable(false);
    m_pNewProblem->Enable(true);
    m_pPlayAllNotes->Enable(false);
    m_pContinue->Enable(false);

    //save time
    m_tmAsked = wxDateTime::Now();
}

void lmIdfyNotesCtrol::OnRespButton(wxCommandEvent& event)
{
    //First, stop any possible score being played to avoid crashes
    StopSounds();

    //identify button pressed
    int nIndex = event.GetId() - m_nIdFirstButton;

    if (m_fQuestionAsked)
    {
        // There is a question asked. The user press the button to give the answer

        //verify if success or failure
        bool fSuccess = (nIndex == m_nRespIndex);

        //inform problem manager of the result
        OnQuestionAnswered(m_iQ, fSuccess);

        //produce feedback sound, and update statistics display
        if (m_pCounters)
        {
            m_pCounters->UpdateDisplay();
            m_pCounters->RightWrongSound(fSuccess);
        }

        //if success generate a new problem. Else, ask user to tray again
        if (fSuccess)
            NewProblem();
        else
        {
            m_fQuestionAsked = true;
            DisplayProblem();
            DisplayMessage(_("Try again!"), false);
        }
    }
    //else
        // No problem presented. Ignore click on answer button
}

void lmIdfyNotesCtrol::DisplaySolution()
{
    lmOneScoreCtrol::DisplaySolution();

    m_pContinue->Enable(true);
}
