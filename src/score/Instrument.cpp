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

// lmInstrument    A collection of Staves
//
// An lmInstrument is a collection of Staves. Usually only one staff or two staves
// (piano grand staff) but could be any other number.

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Instrument.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "VStaff.h"
#include "InstrGroup.h"
#include "wx/debug.h"
#include "properties/DlgProperties.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeText.h"
#include "../graphic/ShapeBracket.h"
#include "../app/Preferences.h"


//Global variables used as default initializators
lmFontInfo g_tInstrumentDefaultFont = { _T("Times New Roman"), 14, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD };




//--------------------------------------------------------------------------------------
/// Class lmMidiProperties
//--------------------------------------------------------------------------------------

#include "defs.h"
#include "properties/DlgProperties.h"

class lmController;

class lmMidiProperties : public lmPropertiesPage 
{
public:
	lmMidiProperties(wxWindow* parent, lmInstrument* pInstr);
	~lmMidiProperties();

    //implementation of pure virtual methods in base class
    void OnAcceptChanges(lmController* pController);

    //overrides
    void OnCancelChanges();

    //event handlers
    void OnComboGroup(wxCommandEvent& event);
    void OnComboInstrument(wxCommandEvent& event);
    void OnButtonTestSoundClick(wxCommandEvent& event);

protected:
    void CreateControls();
    void DoProgramChange();

    //controls
	wxStaticText*   m_pTxtChannel;
	wxChoice*       m_pVoiceChannelCombo;
	wxStaticText*   m_pTxtGroup;
	wxChoice*       m_pGroupCombo;
	wxStaticText*   m_pTxtInstrument;
	wxChoice*       m_pInstrCombo;
	wxButton*       m_pBtnTestSound;

    //other variables
    lmInstrument*       m_pInstr;
    int                 m_nMidiInstr;   //to save Midi data
    int                 m_nMidiChannel;


    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------------
/// Implementation of lmMidiProperties
//--------------------------------------------------------------------------------------

#include "../app/ScoreCanvas.h"			    //lmController
#include "wxMidi.h"                         // MIDI support throgh Portmidi lib
#include "../sound/MidiManager.h"           //access to Midi configuration

//event identifiers
enum
{
    lmID_COMBO_GROUP = 2600,
    lmID_COMBO_INSTRUMENT,
    lmID_COMBO_CHANNEL,
    lmID_BUTTON_TEST_SOUND,
};


BEGIN_EVENT_TABLE(lmMidiProperties, lmPropertiesPage)
    EVT_CHOICE  (lmID_COMBO_GROUP, lmMidiProperties::OnComboGroup)
    EVT_CHOICE  (lmID_COMBO_INSTRUMENT, lmMidiProperties::OnComboInstrument)
    EVT_BUTTON  (lmID_BUTTON_TEST_SOUND, lmMidiProperties::OnButtonTestSoundClick)

END_EVENT_TABLE()


lmMidiProperties::lmMidiProperties(wxWindow* parent, lmInstrument* pInstr)
    : lmPropertiesPage(parent)
{
    m_pInstr = pInstr;
    CreateControls();

    //save Midi data as it will be modified during sound testing
    m_nMidiInstr = g_pMidi->VoiceInstr();
    m_nMidiChannel = g_pMidi->VoiceChannel();

    //populate channel combo
    m_pVoiceChannelCombo->Clear();
    for(int i=1; i <= 16; i++)
    {
        m_pVoiceChannelCombo->Append(wxString::Format(_T("%d"), i));
    }
    m_pVoiceChannelCombo->SetSelection( pInstr->GetMIDIChannel() - 1 );

    //populate sections and instruments combos
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nInstr = pInstr->GetMIDIInstrument();
    int nSect = pMidiGM->PopulateWithSections((wxControlWithItems*)m_pGroupCombo, nInstr );
    pMidiGM->PopulateWithInstruments((wxControlWithItems*)m_pInstrCombo, nSect, nInstr, true);
}

void lmMidiProperties::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* pChannelSizer;
	pChannelSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_pTxtChannel = new wxStaticText( this, wxID_ANY, _("Channel:"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_pTxtChannel->Wrap( -1 );
	pChannelSizer->Add( m_pTxtChannel, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_pVoiceChannelComboChoices;
	m_pVoiceChannelCombo = new wxChoice( this, lmID_COMBO_CHANNEL, wxDefaultPosition, wxSize( 70,-1 ), m_pVoiceChannelComboChoices, 0 );
	m_pVoiceChannelCombo->SetSelection( 0 );
	pChannelSizer->Add( m_pVoiceChannelCombo, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	pMainSizer->Add( pChannelSizer, 0, wxTOP|wxLEFT, 20 );
	
	wxBoxSizer* pInstrSizer;
	pInstrSizer = new wxBoxSizer( wxVERTICAL );
	
	m_pTxtGroup = new wxStaticText( this, wxID_ANY, _("Group:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtGroup->Wrap( -1 );
	pInstrSizer->Add( m_pTxtGroup, 0, wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_pGroupComboChoices;
	m_pGroupCombo = new wxChoice( this, lmID_COMBO_GROUP, wxDefaultPosition, wxSize( 250,-1 ), m_pGroupComboChoices, 0 );
	m_pGroupCombo->SetSelection( 0 );
	pInstrSizer->Add( m_pGroupCombo, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_pTxtInstrument = new wxStaticText( this, wxID_ANY, _("Instrument:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtInstrument->Wrap( -1 );
	pInstrSizer->Add( m_pTxtInstrument, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_pInstrComboChoices;
	m_pInstrCombo = new wxChoice( this, lmID_COMBO_INSTRUMENT, wxDefaultPosition, wxSize( 250,-1 ), m_pInstrComboChoices, 0 );
	m_pInstrCombo->SetSelection( 0 );
	pInstrSizer->Add( m_pInstrCombo, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	pMainSizer->Add( pInstrSizer, 1, wxEXPAND|wxTOP|wxLEFT, 20 );
	
	wxBoxSizer* pButtonSizer;
	pButtonSizer = new wxBoxSizer( wxVERTICAL );
	
	m_pBtnTestSound = new wxButton( this, lmID_BUTTON_TEST_SOUND, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonSizer->Add( m_pBtnTestSound, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	pMainSizer->Add( pButtonSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( pMainSizer );
	this->Layout();
}

lmMidiProperties::~lmMidiProperties()
{
}

void lmMidiProperties::OnAcceptChanges(lmController* pController)
{
    int nInstr = m_pInstrCombo->GetSelection();
    int nSect = m_pGroupCombo->GetSelection();
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nMidiInstr = pMidiGM->GetInstrFromSection(nSect, nInstr);
    int nMidiChannel = m_pVoiceChannelCombo->GetSelection() + 1;

    //check if anything changed
	if (nMidiInstr == m_pInstr->GetMIDIInstrument() 
        && nMidiChannel == m_pInstr->GetMIDIChannel())
		return;		//nothing to change

    //proceed to do changes
    if (pController)
    {
        //Changing an existing object. Do changes by issuing edit commands
        pController->ChangeMidiSettings(m_pInstr, nMidiChannel, nMidiInstr);
    }
    else
    {
        //Direct creation. Modify lmInstrument object directly
        m_pInstr->SetMIDIChannel( nMidiChannel );
        m_pInstr->SetMIDIInstrument( nMidiInstr );
    }

    //restore MIDI controller settings
    g_pMidi->VoiceChange(m_nMidiChannel, m_nMidiInstr);
}

void lmMidiProperties::OnCancelChanges()
{
    //restore MIDI controller settings
    g_pMidi->VoiceChange(m_nMidiChannel, m_nMidiInstr);
}

void lmMidiProperties::DoProgramChange()
{
    //Change Midi instrument to the one selected in combo Instruments
    int nInstr = m_pInstrCombo->GetSelection();
    int nSect = m_pGroupCombo->GetSelection();
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nVoiceInstr = pMidiGM->GetInstrFromSection(nSect, nInstr);
    int nVoiceChannel = m_pVoiceChannelCombo->GetSelection();
    g_pMidi->VoiceChange(nVoiceChannel, nVoiceInstr);
}

void lmMidiProperties::OnComboGroup(wxCommandEvent& event)
{
    // A new section selected. Reload Instruments combo with the instruments in the
    //selected section

    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nSect = m_pGroupCombo->GetSelection();
    pMidiGM->PopulateWithInstruments((wxControlWithItems*)m_pInstrCombo, nSect, 0, true);
    DoProgramChange();
}

void lmMidiProperties::OnComboInstrument(wxCommandEvent& event)
{
    // A new instrument selected. Change Midi program
    DoProgramChange();
}

void lmMidiProperties::OnButtonTestSoundClick(wxCommandEvent& event)
{
    //play a scale
    g_pMidi->TestOut();
}




//=======================================================================================
// lmInstrument implementation
//=======================================================================================

lmInstrument::lmInstrument(lmScore* pScore, int nMIDIChannel,
                           int nMIDIInstr, wxString sName, wxString sAbbrev)
    : lmScoreObj(pScore)
{
    //create objects for name and abbreviation
    lmInstrNameAbbrev* pName = (lmInstrNameAbbrev*)NULL;
    lmInstrNameAbbrev* pAbbreviation = (lmInstrNameAbbrev*)NULL;

    if (sName != _T(""))
    { 
        lmTextStyle* pStyle = GetScore()->GetStyleName(g_tInstrumentDefaultFont);
        wxASSERT(pStyle);
        pName = new lmInstrNameAbbrev(sName, pStyle);
    }

    if (sAbbrev != _T(""))
    { 
        lmTextStyle* pStyle = GetScore()->GetStyleName(g_tInstrumentDefaultFont);
        wxASSERT(pStyle);
        pAbbreviation = new lmInstrNameAbbrev(sAbbrev, pStyle);
    }

    //create the instrument
    Create(pScore, nMIDIChannel, nMIDIInstr, pName, pAbbreviation);
}

lmInstrument::lmInstrument(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
						   lmInstrNameAbbrev* pName, lmInstrNameAbbrev* pAbbrev)
    : lmScoreObj(pScore)
{
    Create(pScore, nMIDIChannel, nMIDIInstr, pName, pAbbrev);
}

void lmInstrument::Create(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
						  lmInstrNameAbbrev* pName, lmInstrNameAbbrev* pAbbrev)
{
    m_pScore = pScore;
    m_nMidiInstr = nMIDIInstr;
    m_nMidiChannel = nMIDIChannel;
    m_uIndentFirst = 0;
    m_uIndentOther = 0;
    m_pVStaff = new lmVStaff(m_pScore, this);
    m_pGroup = (lmInstrGroup*)NULL;
    m_nBracket = lm_eBracketDefault;
    m_uBracketWidth = 0.0f;
    m_uBracketGap = 0.0f;

    m_pName = pName;
    if (pName)
        pName->SetOwner(this);

    m_pAbbreviation = pAbbrev;
    if (pAbbrev)
        pAbbrev->SetOwner(this);
}

void lmInstrument::AddName(wxString& sName)
{ 
    if (m_pName)
        delete m_pName;

    lmTextStyle* pStyle = GetScore()->GetStyleName(g_tInstrumentDefaultFont);
    wxASSERT(pStyle);
    m_pName = new lmInstrNameAbbrev(sName, pStyle);
    m_pName->SetOwner(this);
}

void lmInstrument::AddAbbreviation(wxString& sAbbrev)
{ 
    if (m_pAbbreviation) 
        delete m_pAbbreviation;

    lmTextStyle* pStyle = GetScore()->GetStyleName(g_tInstrumentDefaultFont);
    wxASSERT(pStyle);
    m_pAbbreviation = new lmInstrNameAbbrev(sAbbrev, pStyle);
    m_pAbbreviation->SetOwner(this);
}

lmInstrument::~lmInstrument()
{
	delete m_pVStaff;

	//delete names
	if (m_pName) delete m_pName;
    if (m_pAbbreviation) delete m_pAbbreviation;

    //remove from group
    if (m_pGroup)
    {
        m_pGroup->Remove(this);
		if (m_pGroup->NumInstruments() <= 1)
			delete m_pGroup;
	}
}

lmLUnits lmInstrument::TenthsToLogical(lmTenths nTenths)
{
    return GetVStaff()->TenthsToLogical(nTenths, 1);
}

lmTenths lmInstrument::LogicalToTenths(lmLUnits uUnits)
{
    return GetVStaff()->LogicalToTenths(uUnits, 1);
}

bool lmInstrument::IsFirstOfSystem() 
{ 
    return m_pScore->IsFirstInstrument(this);
}

void lmInstrument::SetIndent(lmLUnits* pIndent, lmLocation* pPos)
{
    if (pPos->xUnits == lmTENTHS) {
        lmVStaff *pVStaff = GetVStaff();
        *pIndent = pVStaff->TenthsToLogical(pPos->x, 1);
    }
    else {
        *pIndent = lmToLogicalUnits(pPos->x, pPos->xUnits);
    }

}

wxString lmInstrument::Dump()
{
    wxString sDump = _T("\nVStaff\n");
    sDump += m_pVStaff->Dump();
    return sDump;

}

wxString lmInstrument::SourceLDP(int nIndent)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(instrument");

    //num of staves
	sSource += wxString::Format(_T(" (staves %d)"), m_pVStaff->GetNumStaves());

	//MIDI info
	sSource += wxString::Format(_T(" (infoMIDI %d %d)"), m_nMidiInstr, m_nMidiChannel);

    //Name and abbreviation
    if (m_pName)
        sSource += m_pName->SourceLDP(_T("name"));
    if (m_pAbbreviation)
        sSource += m_pAbbreviation->SourceLDP(_T("abbrev"));

    //the music data (lmVStaff)
    sSource += _T("\n");
	nIndent++;
	sSource += m_pVStaff->SourceLDP(nIndent);
	nIndent--;

    //close instrument
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T(")\n");
    return sSource;

}

wxString lmInstrument::SourceXML(int nIndent)
{
	wxString sSource = _T("");
	sSource += m_pVStaff->SourceXML(nIndent);
	return sSource;

}

const wxString& lmInstrument::GetInstrName()
{
    if (m_pName)
		return m_pName->GetText();

	return lmEmptyString;
}

void lmInstrument::MeasureNames(lmPaper* pPaper)
{
	// This method is invoked only from lmFormatter4::LayoutScore(), in order to
	// measure the indentation for each instrument, so that a suitable common indentation
    // value for the whole system can be computed.
    //
    // When this method is invoked paper is positioned at top left corner of instrument
    // renderization point (x = left margin, y = top line of first staff)
	// To measure the names we need to create the shapes but they are deleted before
	// returning

    //Save original position to restore it later
    lmLUnits xPaper = pPaper->GetCursorX();

    //if this instrument is in a group get group names & brace widths
    if (m_pGroup)
    {
        if (m_pGroup->GetFirstInstrument() == this)
        {
            m_pGroup->MeasureNames(pPaper);
        }
        m_uIndentFirst = m_pGroup->GetIndentFirst();
        m_uIndentOther = m_pGroup->GetIndentOther();
    }
    else
    {
        m_uIndentFirst = 0;
        m_uIndentOther = 0;
    }

    //TODO: user options
    lmLUnits uSpaceAfterName = TenthsToLogical(10.0f);

    if (m_pName)
    {
        // measure text extent
        lmShape* pShape = m_pName->CreateShape(pPaper, m_uPaperPos);
        // set indent =  text extend + after text space
        m_uIndentFirst += pShape->GetWidth() + uSpaceAfterName;
		delete pShape;
    }

    if (m_pAbbreviation)
    {
        // measure text extent
        lmShape* pShape = m_pAbbreviation->CreateShape(pPaper, m_uPaperPos);
        // set indent =  text extend + after text space
        m_uIndentOther += pShape->GetWidth() + uSpaceAfterName;
		delete pShape;
    }

    if (RenderBraket())
    {
        lmPgmOptions* pPgmOpt = lmPgmOptions::GetInstance();
        float rOptValue = pPgmOpt->GetFloatValue(lm_EO_GRP_BRACKET_WIDTH);
        m_uBracketWidth = TenthsToLogical(rOptValue);
        rOptValue = pPgmOpt->GetFloatValue(lm_EO_GRP_BRACKET_GAP);
        m_uBracketGap = TenthsToLogical(rOptValue);

        m_uIndentOther += m_uBracketWidth + m_uBracketGap;
        m_uIndentFirst += m_uBracketWidth + m_uBracketGap;
    }

    //restore original paper position
    pPaper->SetCursorX( xPaper );

}

void lmInstrument::AddNameAndBracket(lmBox* pBSystem, lmBox* pBSliceInstr, lmPaper* pPaper,
                                     int nSystem)
{
    //Layout.
    // invoked when layouting first measure in system, to add instrument name and bracket/brace.
    // This method is also responsible for managing group name and bracket/brace layout
    // When reaching this point, BoxSystem and BoxSliceInstr have their bounds correctly
    // set (except xRight)

	if (nSystem == 1)
        AddNameAbbrevShape(pBSliceInstr, pPaper, m_pName);
	else
        AddNameAbbrevShape(pBSliceInstr, pPaper, m_pAbbreviation);

    // if first instrument in group, save yTop position for group
    static lmLUnits yTopGroup;
	if (IsFirstOfGroup())
		yTopGroup = pBSliceInstr->GetYTop();

    // if last instrument of a group, add group name and bracket/brace
	if (IsLastOfGroup())
        m_pGroup->AddNameAndBracket(pBSystem, pPaper, nSystem, pBSliceInstr->GetXLeft(),
                                    yTopGroup, pBSliceInstr->GetYBottom() );
}

void lmInstrument::AddNameAbbrevShape(lmBox* pBox, lmPaper* pPaper, lmInstrNameAbbrev* pName)
{
    //get box position
    lmUPoint uBox(pBox->GetXLeft(), pBox->GetYTop() );

    //add shape for the bracket, if necessary
    if (RenderBraket())
    {
        lmLUnits xLeft = uBox.x - m_uBracketWidth - m_uBracketGap;
        lmLUnits xRight = uBox.x - m_uBracketGap;
        lmLUnits yBottom = pBox->GetYBottom();
        lmEBracketSymbol nSymbol = (m_nBracket == lm_eBracketDefault ? lm_eBracket : m_nBracket);
        lmShape* pShape;
        if (nSymbol == lm_eBracket)
            pShape = new lmShapeBracket(this, xLeft, uBox.y, xRight, yBottom, *wxBLACK);
        else
        {
            lmLUnits dyHook = TenthsToLogical(6.0f);
            pShape = new lmShapeBrace(this, xLeft, uBox.y, xRight, yBottom,
                                      dyHook, *wxBLACK);
        }
        pBox->AddShape(pShape, lm_eLayerStaff);
    }

    //add shape for the name/abbreviation
    if (pName)
    {
        lmUPoint uPos(GetScore()->GetPageLeftMargin(), uBox.y);
        lmShape* pShape = pName->CreateShape(pPaper, uPos);
        pShape->Shift(0.0f, (pBox->GetHeight() - pShape->GetHeight())/2.0f );
        pBox->AddShape(pShape, lm_eLayerStaff);
    }
}

bool lmInstrument::RenderBraket()
{
    //returns true if a brace / bracket must be rendered

    return (m_nBracket == lm_eBracketDefault && m_pVStaff->GetNumStaves() > 1 ||
            m_nBracket == lm_eBracket ||
            m_nBracket == lm_eBrace );
}
void lmInstrument::OnRemovedFromGroup(lmInstrGroup* pGroup)
{
	//AWARE: this method is invoked only when the group is being deleted and
	//this deletion is not requested by this instrument. If this instrument would
    //like to delete the group it MUST invoke Remove(this) before deleting the 
	//group object

    m_pGroup = (lmInstrGroup*)NULL;
}

void lmInstrument::OnIncludedInGroup(lmInstrGroup* pGroup)
{
    m_pGroup = pGroup;
}

bool lmInstrument::IsLastOfGroup()
{
    return (m_pGroup && m_pGroup->GetLastInstrument() == this);
}

bool lmInstrument::IsFirstOfGroup()
{
    return (m_pGroup && m_pGroup->GetFirstInstrument() == this);
}

lmVStaffCursor* lmInstrument::GetVCursor() 
{ 
    return m_pVStaff->GetVCursor();
}

void lmInstrument::ResetCursor() 
{ 
    m_pVStaff->ResetCursor(); 
}

void lmInstrument::AttachCursor(lmVStaffCursor* pVCursor)
{ 
    m_pVStaff->AttachCursor(pVCursor); 
}

int lmInstrument::GetNumStaves() 
{ 
    return m_pVStaff->GetNumStaves(); 
}


void lmInstrument::OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName)
{
	//invoked to add specific panels to the dialog

	pDlg->AddPanel( new lmMidiProperties(pDlg->GetNotebook(), this),
				_("MIDI"));

	//add pages to edit name and abbreviation
    wxString sEmpty = _T("");
    if (!m_pName)
        AddName(sEmpty);
	if (!m_pAbbreviation)
        AddAbbreviation(sEmpty);

	m_pName->OnEditProperties(pDlg, _("Name"));
    m_pAbbreviation->OnEditProperties(pDlg, _("Abbreviation"));

	//change dialog title
	pDlg->SetTitle(_("Instrument properties"));
}

void lmInstrument::OnPropertiesChanged()
{
    //AWARE: This method is invoked AFTER issuing the edit commands. Therefore, cannot
    //be used to prevent issuing the commands

    if (m_pName->GetText() == _T(""))
    {
        delete m_pName;
        m_pName = (lmInstrNameAbbrev*)NULL;
    }

    if (m_pAbbreviation->GetText() == _T(""))
    {
        delete m_pAbbreviation;
        m_pAbbreviation = (lmInstrNameAbbrev*)NULL;
    }
}
