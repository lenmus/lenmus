//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ScoreWizard.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <vector>

#include "ScoreWizard.h"
#include "ArtProvider.h"
#include "../score/Score.h"
#include "../app/Page.h"


//--------------------------------------------------------------------------------
// static variables to save data about the score to create
//--------------------------------------------------------------------------------

typedef struct lmScoreDataStruct
{
    //page layout
    wxSize      nPageSize;          //Page size in mm (default: A4)
    bool        fPortrait;          //Orientation (default: portrait)
    int         nTopMargin;         //Margins
    int         nBottomMargin;
    int         nLeftMargin;
    int         nRightMargin;
    int         nBindingMargin;

    //    - Headers (default: none)
    //    - Footers (default: none)

    //initial time signature
    lmETimeSignature    nTimeType;  //initial time signature:  4/4

} lmScoreData;

static lmScoreData m_ScoreData;


//score titles
typedef struct lmTitleDataStruct
{
    wxString        sTitle;     
    lmEAlignment    nAlign;
    lmLocation      tPos;
    wxString        sFontName;
    int             nFontSize;
    lmETextStyle    nStyle;

} lmTitleData;

static std::vector<lmTitleData*>    m_Titles;


//info for one instrument
typedef struct lmInstrDataStruct
{
    wxString    sName;          //instrument name
    wxString    sAbbrev;        //instrument abbreviation
    int         nMidiChanel;    //MIDI channel
    int         nMidiInstr;     //MIDI instr

    //staves
    int         nStaves;        //num of staves
    int         nStaffSize;     //staff size in mm (default: 7.2mm)

    //clefs
    lmEClefType nClefType;      //initial clef  (default: trebble)

    //initial key signature
    int         nFifths;        
    bool        fMajor; 

} lmInstrData;

static std::vector<lmInstrData*>    m_Instruments;

//info for one template
typedef struct lmTemplateDataStruct
{
    wxString    sName;
    bool        fPortrait;

} lmTemplateData;

//template to use
static int m_nSelTemplate = 0;
static lmTemplateData m_Templates[] = {
        { _T("Empty (manuscript paper)"), true },
        { _T("Choir 4 voices (SATB)"), true },
        { _T("Choir SATB + piano"), true },
        { _T("Choir 3 voices (SSA)"), true },
        { _T("Choir SSA + piano"), true },
        { _T("Flute"), true },
        { _T("Guitar"), true },
        { _T("Jazz quartet"), true },
        { _T("Lead sheet"), true },
        { _T("Piano"), true },
        { _T("Voice + keyboard"), true },
        { _T("String quartet"), true },
        { _T("String trio"), true },
        { _T("Woodwind trio"), true },
        { _T("Woodwind quartet"), true },
};

//--------------------------------------------------------------------------------
// lmScoreWizard implementation
//--------------------------------------------------------------------------------

// lmScoreWizard type definition
IMPLEMENT_DYNAMIC_CLASS( lmScoreWizard, wxWizard )

BEGIN_EVENT_TABLE( lmScoreWizard, wxWizard )
    EVT_WIZARD_CANCEL( lmID_SCORE_WIZARD, lmScoreWizard::OnWizardCancel )
    EVT_WIZARD_FINISHED( lmID_SCORE_WIZARD, lmScoreWizard::OnWizardFinished )
END_EVENT_TABLE()


lmScoreWizard::lmScoreWizard()
{
}

lmScoreWizard::~lmScoreWizard()
{
    //delete instruments info
    std::vector<lmInstrData*>::iterator itI;
    for (itI = m_Instruments.begin(); itI != m_Instruments.end(); ++itI)
    {
        delete *itI;
    }
    m_Instruments.clear();

    //delete titles info
    std::vector<lmTitleData*>::iterator itT;
    for (itT = m_Titles.begin(); itT != m_Titles.end(); ++itT)
    {
        delete *itT;
    }
    m_Titles.clear();
}

lmScoreWizard::lmScoreWizard(wxWindow* parent, lmScore** pPtrScore, wxWindowID id, const wxPoint& pos)
{
    Create(parent, id, pos);
    m_pPtrScore = pPtrScore;
}

bool lmScoreWizard::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos)
{
    //initialize default score configuration

    //paper size and margins
    m_ScoreData.nPageSize = wxSize(210, 297);   //Page size in mm (default A4: 21.0 x 29.7 cm)
    m_ScoreData.fPortrait = true;               //Orientation (default: portrait)
    m_ScoreData.nTopMargin = 20;                //20 mm
    m_ScoreData.nBottomMargin = 20;             //20 mm;
    m_ScoreData.nLeftMargin = 20;               //20 mm;
    m_ScoreData.nRightMargin = 20;              //20 mm;
    m_ScoreData.nBindingMargin = 0;             //no binding margin

    //time signature
    m_ScoreData.nTimeType = emtr44;             //initial time signature:  4/4

    //titles
    lmTitleData* pTitle = new lmTitleData;
    pTitle->sTitle = _("Score title");  
    pTitle->nAlign = lmALIGN_CENTER;
    pTitle->tPos.xType = lmLOCATION_DEFAULT;
    pTitle->tPos.xUnits = lmTENTHS;
    pTitle->tPos.yType = lmLOCATION_DEFAULT;
    pTitle->tPos.yUnits = lmTENTHS;
    pTitle->tPos.x = 0.0f;
    pTitle->tPos.y = 0.0f;
    pTitle->sFontName = _T("Times New Roman");
    pTitle->nFontSize = 14;
    pTitle->nStyle = lmTEXT_BOLD;

    m_Titles.push_back( pTitle );

    //one instrument
    lmInstrData* pInstrData = new lmInstrData;
    pInstrData->sName = _T("");         //instrument name
    pInstrData->sAbbrev = _T("");       //instrument abbreviation
    pInstrData->nMidiChanel = 0;        //MIDI channel
    pInstrData->nMidiInstr = 0;         //MIDI instr
    pInstrData->nClefType = lmE_Sol;    //initial clef
    pInstrData->nFifths = 0;            //initial key: C major        
    pInstrData->fMajor = true;; 

    m_Instruments.push_back( pInstrData );

    // create the wizard
    SetExtraStyle(GetExtraStyle()|wxWIZARD_EX_HELPBUTTON);
    wxBitmap wizardBitmap(wxNullBitmap);
    wxWizard::Create(parent, id, _("Score configuration wizard"), wizardBitmap, pos);
    CreateControls();

    return true;
}


void lmScoreWizard::CreateControls()
{
    // Control creation for lmScoreWizard

    //create the pages
    lmScoreWizardLayout* pPageLayout = new lmScoreWizardLayout(this);
    FitToPage(pPageLayout);

    lmScoreWizardInstrPage* pPageInstr = new lmScoreWizardInstrPage(this);
    FitToPage(pPageInstr);

    //lmScoreWizardClefPage* pPageClef = new lmScoreWizardClefPage(this);
    //FitToPage(pPageClef);

    //lmScoreWizardClefPage* pPageTime = new lmScoreWizardTimePage(this);
    //FitToPage(pPageTime);

    //chain the pages in the order of presentation
    wxWizardPageSimple::Chain(pPageLayout, pPageInstr);
    //wxWizardPageSimple::Chain(pPageInstr, pPageClef);
    //wxWizardPageSimple::Chain(pPageClef, pPageTime);
}


bool lmScoreWizard::Run()
{
    // Runs the wizard.

    wxWindowListNode* node = GetChildren().GetFirst();
    while (node)
    {
        wxWizardPage* startPage = wxDynamicCast(node->GetData(), wxWizardPage);
        if (startPage) return RunWizard(startPage);
        node = node->GetNext();
    }
    return false;
}

void lmScoreWizard::OnWizardFinished( wxWizardEvent& event )
{
    // create an empty score
    lmScore* pScore = new lmScore();

    //set paper settings
    lmPageInfo* pPageInfo 
        = new lmPageInfo(m_ScoreData.nLeftMargin, m_ScoreData.nRightMargin, m_ScoreData.nTopMargin,
                         m_ScoreData.nBottomMargin, m_ScoreData.nBindingMargin,
                         m_ScoreData.nPageSize, m_ScoreData.fPortrait);  
    pScore->SetPageInfo(pPageInfo);

    //add titles
    std::vector<lmTitleData*>::iterator itT;
    for (itT = m_Titles.begin(); itT != m_Titles.end(); ++itT)
    {
        pScore->AddTitle((*itT)->sTitle, (*itT)->nAlign, (*itT)->tPos, (*itT)->sFontName,
                         (*itT)->nFontSize, (*itT)->nStyle );
    }

    //add instruments
    std::vector<lmInstrData*>::iterator itI;
    for (itI = m_Instruments.begin(); itI != m_Instruments.end(); ++itI)
    {
        lmInstrument* pInstr = 
            pScore->AddInstrument((*itI)->nMidiChanel, (*itI)->nMidiInstr, (*itI)->sName,
                                  (*itI)->sAbbrev );
        lmVStaff *pVStaff = pInstr->GetVStaff();
        pVStaff->AddClef( (*itI)->nClefType );
        pVStaff->AddKeySignature( (*itI)->nFifths, (*itI)->fMajor );
        pVStaff->AddTimeSignature( m_ScoreData.nTimeType );
	    //pVStaff->AddBarline(lm_eBarlineEOS, true);
    }

    ////In scores created in the score editor, we should render a full page, 
    ////with empty staves. To this end, we need to change some options default value
    //pScore->SetOption(_T("Score.FillPageWithEmptyStaves"), true);
    //pScore->SetOption(_T("StaffLines.StopAtFinalBarline"), false);

    //return the created score
    *m_pPtrScore = pScore;
}

void lmScoreWizard::OnWizardCancel( wxWizardEvent& event )
{
}




//--------------------------------------------------------------------------------
// lmScoreWizardLayout implementation
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( lmScoreWizardLayout, wxWizardPageSimple )

BEGIN_EVENT_TABLE( lmScoreWizardLayout, wxWizardPageSimple )
    //
END_EVENT_TABLE()


lmScoreWizardLayout::lmScoreWizardLayout()
{
}

lmScoreWizardLayout::lmScoreWizardLayout(wxWizard* parent)
{
    Create(parent);
}

bool lmScoreWizardLayout::Create(wxWizard* parent)
{
    // page creation
    wxWizardPageSimple::Create(parent, NULL, NULL, wxArtProvider::GetBitmap(_T("score_wizard")));
    CreateControls();
    GetSizer()->Fit(this);

        // populate controls

    //available layouts
    int nNumTemplates = sizeof(m_Templates) / sizeof(lmTemplateData);
    for (int i=0; i < nNumTemplates; i++)
        m_pLstEnsemble->Append( m_Templates[i].sName );
    m_pLstEnsemble->SetSelection(m_nSelTemplate);

    //paper size
	m_pCboPaper->Append( _T("A4") );
	m_pCboPaper->Append( _T("Legal") );
    m_pCboPaper->SetSelection(0);

    //paper orientation
    m_pRadOrientation->SetSelection(m_ScoreData.fPortrait ? 1 : 0);

    //bitmap preview
	//m_pBmpPreview;

    return true;
}

void lmScoreWizardLayout::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* pLeftColumnSizer;
	pLeftColumnSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* pLayoutSizer;
	pLayoutSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Select instruments and style") ), wxVERTICAL );
	
	m_pLstEnsemble = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 240,-1 ), 0, NULL, 0 ); 
	pLayoutSizer->Add( m_pLstEnsemble, 1, wxALL, 5 );
	
	pLeftColumnSizer->Add( pLayoutSizer, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* pPaperSizer;
	pPaperSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Select paper size and orientation") ), wxHORIZONTAL );
	
	wxBoxSizer* pCboSizeSizer;
	pCboSizeSizer = new wxBoxSizer( wxVERTICAL );
	
	m_pLblPaper = new wxStaticText( this, wxID_ANY, _("Paper size"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblPaper->Wrap( -1 );
	pCboSizeSizer->Add( m_pLblPaper, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_pCboPaperChoices;
	m_pCboPaper = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_pCboPaperChoices, 0 );
	m_pCboPaper->SetSelection( 0 );
	pCboSizeSizer->Add( m_pCboPaper, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	pPaperSizer->Add( pCboSizeSizer, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_pRadOrientationChoices[] = { _("Landscape"), _("Portrait") };
	int m_pRadOrientationNChoices = sizeof( m_pRadOrientationChoices ) / sizeof( wxString );
	m_pRadOrientation = new wxRadioBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_pRadOrientationNChoices, m_pRadOrientationChoices, 1, wxRA_SPECIFY_COLS );
	m_pRadOrientation->SetSelection( 0 );
	pPaperSizer->Add( m_pRadOrientation, 0, wxALL, 5 );
	
	pLeftColumnSizer->Add( pPaperSizer, 0, wxALL|wxEXPAND, 5 );
	
	pMainSizer->Add( pLeftColumnSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* pRightColumnSizer;
	pRightColumnSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Preview") ), wxVERTICAL );
	
	m_pBmpPreview = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
	sbSizer4->Add( m_pBmpPreview, 1, wxALL|wxEXPAND, 5 );
	
	pRightColumnSizer->Add( sbSizer4, 1, wxEXPAND, 5 );
	
	pMainSizer->Add( pRightColumnSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( pMainSizer );
}

bool lmScoreWizardLayout::TransferDataFromWindow()
{
    // when moving to another page this methos is automatically invoked to
    // verify that the data entered is correct before passing to the next page,
    // and to copy entered data to the main page
    // Returns true to allow moving to next page.
    // If false is returned it should display a meesage box to the user to explain 
    // the reason 

    m_ScoreData.fPortrait = (m_pRadOrientation->GetSelection() == 1);
    m_nSelTemplate = m_pLstEnsemble->GetSelection();

    return true;

}



//--------------------------------------------------------------------------------
// lmScoreWizardInstrPage implementation
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( lmScoreWizardInstrPage, wxWizardPageSimple )

BEGIN_EVENT_TABLE( lmScoreWizardInstrPage, wxWizardPageSimple )
    //
END_EVENT_TABLE()

lmScoreWizardInstrPage::lmScoreWizardInstrPage()
{
}

lmScoreWizardInstrPage::lmScoreWizardInstrPage(wxWizard* parent)
{
    Create(parent);
}

bool lmScoreWizardInstrPage::Create(wxWizard* parent)
{
    //// member initialisation
    //m_pOutCombo = NULL;
    //m_pInCombo = NULL;

    // page creation
    wxWizardPageSimple::Create(parent, NULL, NULL, wxArtProvider::GetBitmap(_T("score_wizard")));
    CreateControls();
    GetSizer()->Fit(this);

    //// populate combo boxes with available Midi devices
    ////int nInput=0;
    //int nItem, nOutput=0;
    //int nNumDevices = g_pMidi->CountDevices();
    //for (int i = 0; i < nNumDevices; i++) {
    //    wxMidiOutDevice* pMidiDev = new wxMidiOutDevice(i);
    //    if (pMidiDev->IsOutputPort()) {
    //        nOutput++;
    //        nItem = m_pOutCombo->Append( pMidiDev->DeviceName() );
    //        m_pOutCombo->SetClientData(nItem, (void *)i);
    //    }
    //    delete pMidiDev;
    //}
    //if (nOutput == 0) {
    //    nItem = m_pOutCombo->Append( _("None") );
    //    m_pOutCombo->SetClientData(nItem, (void *)(-1));
    //}
    //m_pOutCombo->SetSelection(0);

    return true;
}

void lmScoreWizardInstrPage::CreateControls()
{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    pMainSizer->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( this, wxID_STATIC, _("Midi devices to use"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText5->SetFont(wxFont(14, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    pMainSizer->Add(itemBoxSizer6, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer6->Add(itemBoxSizer7, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( this, wxID_STATIC, _("To generate sounds the program needs a MIDI synthesizer device. Normally, one of these devices is included in the sound board of the PC, but your PC might have more than one."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer7->Add(itemStaticText8, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText9 = new wxStaticText( this, wxID_STATIC, _("If your PC has more than one device, choose one of them. You can test all of them and choose the one whose sound you prefer."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer7->Add(itemStaticText9, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticLine* itemStaticLine10 = new wxStaticLine( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer6->Add(itemStaticLine10, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer6->Add(itemBoxSizer11, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText12 = new wxStaticText( this, wxID_STATIC, _("Output device:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText12, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pOutComboStrings = NULL;
    m_pOutCombo = new wxComboBox( this, lmID_COMBO_OUT_DEVICES, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pOutComboStrings, wxCB_READONLY );
    itemBoxSizer11->Add(m_pOutCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

}

bool lmScoreWizardInstrPage::TransferDataFromWindow()
{
    // Save temporary data and open temporary Midi Devices

 //   //get number of Midi device to use for output
 //   int nIndex = m_pOutCombo->GetSelection();
 //   int nOutDevId = (int) m_pOutCombo->GetClientData(nIndex);
 //   g_pMidi->SetOutDevice(nOutDevId);

 //   //open input device
 //   int nInDevId = -1;
	////TODO: Un-comment when ready to use MIDI input
 //   //if (m_pInCombo->GetStringSelection() != _("None") ) {
 //   //    nIndex = m_pInCombo->GetSelection();
 //   //    nInDevId = (int) m_pInCombo->GetClientData(nIndex);
 //   //}
 //   g_pMidi->SetInDevice(nInDevId);

    return true;

}



//--------------------------------------------------------------------------------------
// lmScoreWizardClefPage implementation
//--------------------------------------------------------------------------------------

/*

IMPLEMENT_DYNAMIC_CLASS( lmScoreWizardClefPage, wxWizardPageSimple )

BEGIN_EVENT_TABLE( lmScoreWizardClefPage, wxWizardPageSimple )
    EVT_COMBOBOX( lmID_COMBO_SECTION, lmScoreWizardClefPage::OnComboSection )
    EVT_COMBOBOX( lmID_COMBO_INSTRUMENT, lmScoreWizardClefPage::OnComboInstrument )
    EVT_BUTTON( lmID_BUTTON_TEST_SOUND, lmScoreWizardClefPage::OnButtonTestSoundClick )
END_EVENT_TABLE()

lmScoreWizardClefPage::lmScoreWizardClefPage()
{
}

lmScoreWizardClefPage::lmScoreWizardClefPage(wxWizard* parent)
{
    Create(parent);
}

bool lmScoreWizardClefPage::Create(wxWizard* parent)
{
    // member initialisation
    m_pVoiceChannelCombo = NULL;
    m_pSectCombo = NULL;
    m_pInstrCombo = NULL;

    // page creation
    wxBitmap wizardBitmap(wxNullBitmap);
    wxWizardPageSimple::Create(parent, NULL, NULL, wizardBitmap );
    CreateControls();
    GetSizer()->Fit(this);

    // populate channel combo
    m_pVoiceChannelCombo->Clear();
    for(int i=1; i <= 16; i++) {
        m_pVoiceChannelCombo->Append(wxString::Format(_T("%d"), i));
    }
    //Set selection according to current user prefs
    m_pVoiceChannelCombo->SetSelection( g_pMidi->VoiceChannel() );

    //populate sections and instruments combos
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nInstr = g_pMidi->VoiceInstr();
    int nSect = pMidiGM->PopulateWithSections((wxControlWithItems*)m_pSectCombo, nInstr );
    pMidiGM->PopulateWithInstruments((wxControlWithItems*)m_pInstrCombo, nSect, nInstr);

    return true;
}

void lmScoreWizardClefPage::CreateControls()
{
    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxVERTICAL);
    SetSizer(itemBoxSizer17);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer17->Add(itemBoxSizer18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText19 = new wxStaticText( this, wxID_STATIC, _("Voice channel and instrument"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText19->SetFont(wxFont(14, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
    itemBoxSizer18->Add(itemStaticText19, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer17->Add(itemBoxSizer20, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer20->Add(itemBoxSizer21, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText22 = new wxStaticText( this, wxID_STATIC, _("Channels 10 and 16 are specialized in percussion sounds. So it is recommended to choose any other channel (it doesn't matter wich one)."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer21->Add(itemStaticText22, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText23 = new wxStaticText( this, wxID_STATIC, _("To facilitate access to the instruments they are grouped into sections. First choose a section and then choose the desired instrument."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer21->Add(itemStaticText23, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticLine* itemStaticLine24 = new wxStaticLine( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer20->Add(itemStaticLine24, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer20->Add(itemBoxSizer25, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText26 = new wxStaticText( this, wxID_STATIC, _("Channel:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText26, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pVoiceChannelComboStrings = NULL;
    m_pVoiceChannelCombo = new wxComboBox( this, lmID_COMBO_CHANNEL, _T(""), wxDefaultPosition, wxSize(70, -1), 0, m_pVoiceChannelComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pVoiceChannelCombo, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer25->Add(itemBoxSizer28, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText29 = new wxStaticText( this, wxID_STATIC, _("Section:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText29, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pSectComboStrings = NULL;
    m_pSectCombo = new wxComboBox( this, lmID_COMBO_SECTION, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pSectComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pSectCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText31 = new wxStaticText( this, wxID_STATIC, _("Instrument:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText31, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pInstrComboStrings = NULL;
    m_pInstrCombo = new wxComboBox( this, lmID_COMBO_INSTRUMENT, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pInstrComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pInstrCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxButton* itemButton33 = new wxButton( this, lmID_BUTTON_TEST_SOUND, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemButton33, 0, wxALIGN_LEFT|wxALL, 5);
}

bool lmScoreWizardClefPage::TransferDataFromWindow()
{
    //
    // Save temporary data and set temporary Midi program
    //

    DoProgramChange();
    return true;

}

void lmScoreWizardClefPage::DoProgramChange()
{
    //Change Midi instrument to the one selected in combo Instruments
    int nInstr = m_pInstrCombo->GetSelection();
    int nSect = m_pSectCombo->GetSelection();
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nVoiceInstr = pMidiGM->GetInstrFromSection(nSect, nInstr);
    int nVoiceChannel = m_pVoiceChannelCombo->GetSelection();
    g_pMidi->VoiceChange(nVoiceChannel, nVoiceInstr);

}

void lmScoreWizardClefPage::OnComboSection( wxCommandEvent& event )
{
    // A new section selected. Reload Instruments combo with the instruments in the
    //selected section

    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nSect = m_pSectCombo->GetSelection();
    pMidiGM->PopulateWithInstruments((wxControlWithItems*)m_pInstrCombo, nSect);
    DoProgramChange();

}

void lmScoreWizardClefPage::OnComboInstrument( wxCommandEvent& event )
{
    // A new instrument selected. Change Midi program
    DoProgramChange();
}

void lmScoreWizardClefPage::OnButtonTestSoundClick( wxCommandEvent& event )
{
    //play a scale
    g_pMidi->TestOut();
}


//---------------------------------------------------------------------------------------
// lmScoreWizardTimePage implementation
//---------------------------------------------------------------------------------------

// lmScoreWizardTimePage type definition
IMPLEMENT_DYNAMIC_CLASS( lmScoreWizardTimePage, wxWizardPageSimple )

// lmScoreWizardTimePage event table definition
BEGIN_EVENT_TABLE( lmScoreWizardTimePage, wxWizardPageSimple )
    EVT_COMBOBOX( lmID_COMBO_MTR_INSTR1, lmScoreWizardTimePage::OnComboMtrInstr1Selected )
    EVT_COMBOBOX( lmID_COMBO_MTR_INSTR2, lmScoreWizardTimePage::OnComboMtrInstr2Selected )
    EVT_BUTTON( lmID_BUTTON, lmScoreWizardTimePage::OnButtonClick )
END_EVENT_TABLE()


lmScoreWizardTimePage::lmScoreWizardTimePage()
{
}

lmScoreWizardTimePage::lmScoreWizardTimePage(wxWizard* parent)
{
    Create(parent);
}

bool lmScoreWizardTimePage::Create(wxWizard* parent)
{
    // member initialisation
    m_pMtrInstr1Combo = NULL;
    m_pMtrInstr2Combo = NULL;

    // Page creation
    wxBitmap wizardBitmap(wxNullBitmap);
    wxWizardPageSimple::Create(parent, NULL, NULL, wizardBitmap );
    CreateControls();
    GetSizer()->Fit(this);

    //populate metronome sounds combos
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nTone1 = g_pMidi->MtrTone1();
    int nTone2 = g_pMidi->MtrTone2();
    pMidiGM->PopulateWithPercusionInstr((wxControlWithItems*)m_pMtrInstr1Combo, nTone1);
    pMidiGM->PopulateWithPercusionInstr((wxControlWithItems*)m_pMtrInstr2Combo, nTone2);

    return true;
}

void lmScoreWizardTimePage::CreateControls()
{
    wxBoxSizer* itemBoxSizer35 = new wxBoxSizer(wxVERTICAL);
    SetSizer(itemBoxSizer35);

    wxBoxSizer* itemBoxSizer36 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer35->Add(itemBoxSizer36, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText37 = new wxStaticText( this, wxID_STATIC, _("lmMetronome channel and sounds"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText37->SetFont(wxFont(14, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
    itemBoxSizer36->Add(itemStaticText37, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer38 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer35->Add(itemBoxSizer38, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer39 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer38->Add(itemBoxSizer39, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText40 = new wxStaticText( this, wxID_STATIC, _("Channels 10 and 16 are specialized in percussion sounds. So it is recommended to choose one of these (it doesn't matter wich one)."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer39->Add(itemStaticText40, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText41 = new wxStaticText( this, wxID_STATIC, _("To better identify the first beat of each measure it is possible to assign a different sound to it. But you can also choose the same sound for both, the first beat and the others."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer39->Add(itemStaticText41, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticLine* itemStaticLine42 = new wxStaticLine( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer38->Add(itemStaticLine42, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer43 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer38->Add(itemBoxSizer43, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText44 = new wxStaticText( this, wxID_STATIC, _("Channel:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText44, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText47 = new wxStaticText( this, wxID_STATIC, _("Sound for first beat of each measure:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText47, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pMtrInstr1ComboStrings = NULL;
    m_pMtrInstr1Combo = new wxComboBox( this, lmID_COMBO_MTR_INSTR1, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pMtrInstr1ComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrInstr1Combo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText49 = new wxStaticText( this, wxID_STATIC, _("Sound for other beats of each measure:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText49, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pMtrInstr2ComboStrings = NULL;
    m_pMtrInstr2Combo = new wxComboBox( this, lmID_COMBO_MTR_INSTR2, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pMtrInstr2ComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrInstr2Combo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxButton* itemButton51 = new wxButton( this, lmID_BUTTON, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemButton51, 0, wxALIGN_LEFT|wxALL, 5);

}

void lmScoreWizardTimePage::OnComboMtrInstr1Selected( wxCommandEvent& event )
{
    //Change metronome sound, tone1, to the one selected in combo Instr1
    int nTone1 = m_pMtrInstr1Combo->GetSelection() + 35;
    g_pMidi->SetMetronomeTones(nTone1, g_pMidi->MtrTone2());

}

void lmScoreWizardTimePage::OnComboMtrInstr2Selected( wxCommandEvent& event )
{
    //Change metronome sound, tone2, to the one selected in combo Instr2
    int nTone2 = m_pMtrInstr2Combo->GetSelection() + 35;
    g_pMidi->SetMetronomeTones(g_pMidi->MtrTone1(), nTone2);
}

void lmScoreWizardTimePage::OnButtonClick( wxCommandEvent& event )
{
    if (!g_pMidiOut) return;

    //two measures, 3/4 time signature
    for (int i=0; i < 2; i++) {
        //firts beat
        g_pMidiOut->NoteOn(g_pMidi->MtrChannel(), g_pMidi->MtrTone1(), 127);
        ::wxMilliSleep(500);    // wait 500ms
        g_pMidiOut->NoteOff(g_pMidi->MtrChannel(), g_pMidi->MtrTone1(), 127);
        // two more beats
        for (int j=0; j < 2; j++) {
            g_pMidiOut->NoteOn(g_pMidi->MtrChannel(), g_pMidi->MtrTone2(), 127);
            ::wxMilliSleep(500);    // wait 500ms
            g_pMidiOut->NoteOff(g_pMidi->MtrChannel(), g_pMidi->MtrTone2(), 127);
        }
    }

}

*/
