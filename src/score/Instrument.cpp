//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
/// Class lmInstrProperties
//--------------------------------------------------------------------------------------

#include "defs.h"
#include "properties/DlgProperties.h"

class lmScoreObj;
class lmController;
class lmScore;

//class lmInstrProperties : public lmPropertiesPage 
//{
//public:
//	lmInstrProperties(wxWindow* parent, lmBarline* pBL);
//	~lmInstrProperties();
//
//    //implementation of pure virtual methods in base class
//    void OnAcceptChanges(lmController* pController);
//
//    // event handlers
//
//protected:
//    void CreateControls();
//
//    //controls
//	wxStaticText*		m_pTxtBarline;
//	wxBitmapComboBox*	m_pBarlinesList;
//
//    //other variables
//    lmBarline*			m_pBL;
//
//
//    DECLARE_EVENT_TABLE()
//};
//
//
////--------------------------------------------------------------------------------------
///// Implementation of lmInstrProperties
////--------------------------------------------------------------------------------------
//
//#include "../app/ScoreCanvas.h"			//lmConroller
//
//enum {
//    lmID_BARLINE = 2600,
//};
//
//lmBarlinesDBEntry g_tBarlinesDB[] = {
//    { _("Simple barline"),		lm_eBarlineSimple },
//    { _("Double barline"),		lm_eBarlineDouble },
//    { _("Final barline"),		lm_eBarlineEnd },
//    { _("Start repetition"),	lm_eBarlineStartRepetition },
//    { _("End repetition"),		lm_eBarlineEndRepetition },
//    { _("Star barline"),		lm_eBarlineStart },
//    { _("Double repetition"),	lm_eBarlineDoubleRepetition },
//	//End of table item
//	{ _T(""),					(lmEBarline)-1 }
//};
//
//BEGIN_EVENT_TABLE(lmInstrProperties, lmPropertiesPage)
//
//END_EVENT_TABLE()
//
//
////AWARE: pScore is needed as parameter in the constructor for those cases in
////wich the text is being created and is not yet included in the score. In this
////cases method GetScore() will fail, so we can not use it in the implementation
////of this class
//lmInstrProperties::lmInstrProperties(wxWindow* parent, lmBarline* pBL)
//    : lmPropertiesPage(parent)
//{
//    m_pBL = pBL;
//    CreateControls();
//	LoadBarlinesBitmapComboBox(m_pBarlinesList, g_tBarlinesDB);
//	SelectBarlineBitmapComboBox(m_pBarlinesList, m_pBL->GetBarlineType() );
//}
//
//void lmInstrProperties::CreateControls()
//{
//	wxBoxSizer* pMainSizer;
//	pMainSizer = new wxBoxSizer( wxVERTICAL );
//	
//	m_pTxtBarline = new wxStaticText( this, wxID_ANY, wxT("Barline type"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pTxtBarline->Wrap( -1 );
//	m_pTxtBarline->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Tahoma") ) );
//	
//	pMainSizer->Add( m_pTxtBarline, 0, wxALL, 5 );
//	
//	wxArrayString m_pBarlinesListChoices;
//    m_pBarlinesList = new wxBitmapComboBox();
//    m_pBarlinesList->Create(this, lmID_BARLINE, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
//							0, NULL, wxCB_READONLY);
//	pMainSizer->Add( m_pBarlinesList, 0, wxALL, 5 );
//	
//	this->SetSizer( pMainSizer );
//	this->Layout();
//}
//
//lmInstrProperties::~lmInstrProperties()
//{
//}
//
//void lmInstrProperties::OnAcceptChanges(lmController* pController)
//{
//	int iB = m_pBarlinesList->GetSelection();
//    lmEBarline nType = g_tBarlinesDB[iB].nBarlineType;
//	if (nType == m_pBL->GetBarlineType())
//		return;		//nothing to change
//
//    if (pController)
//    {
//        //Editing and existing object. Do changes by issuing edit commands
//        pController->ChangeBarline(m_pBL, nType, m_pBL->IsVisible());
//    }
//  //  else
//  //  {
//  //      //Direct creation. Modify text object directly
//  //      m_pParentText->SetText( m_pTxtCtrl->GetValue() );
//  //      m_pParentText->SetStyle(pStyle);
//		//m_pParentText->SetAlignment(m_nHAlign);
//  //  }
//}
//


//=======================================================================================
// lmInstrument implementation
//=======================================================================================

lmInstrument::lmInstrument(lmScore* pScore, int nMIDIChannel,
                           int nMIDIInstr, wxString sName, wxString sAbbrev)
    : lmScoreObj(pScore)
{
    //create objects for name and abbreviation
    lmTextItem* pName = (lmTextItem*)NULL;
    lmTextItem* pAbbreviation = (lmTextItem*)NULL;

    if (sName != _T(""))
    { 
        lmTextStyle* pStyle = GetScore()->GetStyleName(g_tInstrumentDefaultFont);
        wxASSERT(pStyle);
        pName = new lmTextItem(sName, lmHALIGN_LEFT, pStyle);
    }

    if (sAbbrev != _T(""))
    { 
        lmTextStyle* pStyle = GetScore()->GetStyleName(g_tInstrumentDefaultFont);
        wxASSERT(pStyle);
        pAbbreviation = new lmTextItem(sAbbrev, lmHALIGN_LEFT, pStyle);
    }

    //create the instrument
    Create(pScore, nMIDIChannel, nMIDIInstr, pName, pAbbreviation);
}

lmInstrument::lmInstrument(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
						   lmTextItem* pName, lmTextItem* pAbbrev)
    : lmScoreObj(pScore)
{
    Create(pScore, nMIDIChannel, nMIDIInstr, pName, pAbbrev);
}

void lmInstrument::Create(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
						  lmTextItem* pName, lmTextItem* pAbbrev)
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
	sSource += wxString::Format(_T(" (infoMIDI %d %d)\n"), m_nMidiInstr, m_nMidiChannel);

    //the music data (lmVStaff)
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

	return wxEmptyString;
}

void lmInstrument::MeasureNames(lmPaper* pPaper)
{
	// This method is invoked only from lmFormatter4::RenderJustified(), in order to
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

void lmInstrument::AddNameAbbrevShape(lmBox* pBox, lmPaper* pPaper, lmTextItem* pName)
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
        pBox->AddShape( pShape );
    }

    //add shape for the name/abbreviation
    if (pName)
    {
        lmUPoint uPos(GetScore()->GetPageLeftMargin(), uBox.y);
        lmShape* pShape = pName->CreateShape(pPaper, uPos);
        pShape->Shift(0.0f, (pBox->GetHeight() - pShape->GetHeight())/2.0f );
        pBox->AddShape( pShape );
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

lmVStaffCursor* lmInstrument::AttachCursor(lmScoreCursor* pSCursor)
{ 
    return m_pVStaff->GetVCursor()->AttachCursor(pSCursor); 
}

void lmInstrument::DetachCursor() 
{ 
    m_pVStaff->GetVCursor()->DetachCursor(); 
}

int lmInstrument::GetNumStaves() 
{ 
    return m_pVStaff->GetNumStaves(); 
}


void lmInstrument::OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName)
{
	//invoked to add specific panels to the dialog

	//pDlg->AddPanel( new lmInstrProperties(pDlg->GetNotebook(), this),
	//			_("Instrument"));
	//add pages to edit name and abbreviation
	m_pName->OnEditProperties(pDlg, _("Name"));
	//m_pAbbreviation->>OnEditProperties(pDlg, _("Abbreviation"));

	//change dialog title
	pDlg->SetTitle(_("Instrument properties"));
}
