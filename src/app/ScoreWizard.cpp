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

#include "wx/paper.h"

#include "ScoreWizard.h"
#include "ArtProvider.h"
#include "../score/Score.h"
#include "../app/Page.h"
#include "../widgets/MsgBox.h"
#include "../ldp_parser/LDPParser.h"

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;

extern wxPrintPaperDatabase* wxThePrintPaperDatabase;


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


//score titles -----------------------------
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


//info for one instrument ---------------
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

//info about templates ---------------------------
typedef struct lmTemplateDataStruct
{
    wxString    sName;          //name to display
    wxString    sTemplate;      //associated template file
    bool        fPortrait;      //default paper orientation

} lmTemplateData;

static lmTemplateData m_Templates[] = {
        //    Displayed name                Template                Portrait
        { _("Empty (manuscript paper)"),    _T(""),     true },
        { _("Choir 4 voices (SATB)"),       _T("choir_SATB.lms"),   true },
        { _("Choir SATB + piano"),          _T("x"),    true },
        { _("Choir 3 voices (SSA)"),        _T("x"),    true },
        { _("Choir SSA + piano"),           _T("x"),    true },
        { _("Flute"),                       _T("x"),    false },
        { _("Guitar"),                      _T("x"),    true },
        { _("Jazz quartet"),                _T("x"),    true },
        { _("Lead sheet"),                  _T("x"),    true },
        { _("Piano"),                       _T("piano.lms"),        true },
        { _("Voice + keyboard"),            _T("x"),    true },
        { _("String quartet"),              _T("x"),    true },
        { _("String trio"),                 _T("x"),    true },
        { _("Woodwind trio"),               _T("x"),    true },
        { _("Woodwind quartet"),            _T("x"),    true },
};

//template to use
static int m_nSelTemplate = 0;      //"Empty (manuscript paper)"


//info about paper sizes ---------------------------
typedef struct lmPaperSizeDataStruct
{
    wxPaperSize nId;        //paper Id. Enum defined in wx/defs.h
    wxString    sName;      //name to display
    int         nWidth;     //width, in tenths of one millimeter
    int         nHeight;    //height, in tenths of one millimeter

} lmPaperSizeData;

//to avoid having to translate paper names, I will use the wxWidgets papers database, but only for
//some applicable papers (i.e excluding, envelopes, ...). Using wxPAPER_NONE means that paper data
//is in this table. Any other identifier means that paper data must be taken from wxWidgets DB
static lmPaperSizeData m_Papers[] = {
    //                              Displayed name                  Size
    { wxPAPER_NONE,                 _("DIN A3"),                    2970, 4200 },
    { wxPAPER_NONE,                 _("DIN A4"),                    2100, 2970 },
    { wxPAPER_LETTER,               _T(""),   0,0 },  //"Letter, 8 1/2 x 11 in"), 2159, 2794);
    { wxPAPER_LEGAL,                _T(""),   0,0 },  //"Legal, 8 1/2 x 14 in"), 2159, 3556);
    { wxPAPER_A4,                   _T(""),   0,0 },  //"A4 sheet, 210 x 297 mm"), 2100, 2970);
    { wxPAPER_CSHEET,               _T(""),   0,0 },  //"C sheet, 17 x 22 in"), 4318, 5588);
    { wxPAPER_DSHEET,               _T(""),   0,0 },  //"D sheet, 22 x 34 in"), 5588, 8636);
    { wxPAPER_ESHEET,               _T(""),   0,0 },  //"E sheet, 34 x 44 in"), 8636, 11176);
    { wxPAPER_LETTERSMALL,          _T(""),   0,0 },  //"Letter Small, 8 1/2 x 11 in"), 2159, 2794);
    { wxPAPER_TABLOID,              _T(""),   0,0 },  //"Tabloid, 11 x 17 in"), 2794, 4318);
    { wxPAPER_LEDGER,               _T(""),   0,0 },  //"Ledger, 17 x 11 in"), 4318, 2794);
    { wxPAPER_STATEMENT,            _T(""),   0,0 },  //"Statement, 5 1/2 x 8 1/2 in"), 1397, 2159);
    { wxPAPER_EXECUTIVE,            _T(""),   0,0 },  //"Executive, 7 1/4 x 10 1/2 in"), 1842, 2667);
    { wxPAPER_A3,                   _T(""),   0,0 },  //"A3 sheet, 297 x 420 mm"), 2970, 4200);
    { wxPAPER_A4SMALL,              _T(""),   0,0 },  //"A4 small sheet, 210 x 297 mm"), 2100, 2970);
    { wxPAPER_A5,                   _T(""),   0,0 },  //"A5 sheet, 148 x 210 mm"), 1480, 2100);
    { wxPAPER_B4,                   _T(""),   0,0 },  //"B4 sheet, 250 x 354 mm"), 2500, 3540);
    { wxPAPER_B5,                   _T(""),   0,0 },  //"B5 sheet, 182 x 257 millimeter"), 1820, 2570);
    { wxPAPER_FOLIO,                _T(""),   0,0 },  //"Folio, 8 1/2 x 13 in"), 2159, 3302);
    { wxPAPER_QUARTO,               _T(""),   0,0 },  //"Quarto, 215 x 275 mm"), 2150, 2750);
    { wxPAPER_10X14,                _T(""),   0,0 },  //"10 x 14 in"), 2540, 3556);
    { wxPAPER_11X17,                _T(""),   0,0 },  //"11 x 17 in"), 2794, 4318);
    { wxPAPER_NOTE,                 _T(""),   0,0 },  //"Note, 8 1/2 x 11 in"), 2159, 2794);
    { wxPAPER_FANFOLD_US,           _T(""),   0,0 },  //"US Std Fanfold, 14 7/8 x 11 in"), 3778, 2794);
    { wxPAPER_FANFOLD_STD_GERMAN,   _T(""),   0,0 },  //"German Std Fanfold, 8 1/2 x 12 in"), 2159, 3048);
    { wxPAPER_FANFOLD_LGL_GERMAN,   _T(""),   0,0 },  //"German Legal Fanfold, 8 1/2 x 13 in"), 2159, 3302);
    { wxPAPER_ISO_B4,               _T(""),   0,0 },  //"B4 (ISO) 250 x 353 mm"), 2500, 2530);
    { wxPAPER_JAPANESE_POSTCARD,    _T(""),   0,0 },  //"Japanese Postcard 100 x 148 mm"), 1000, 1480);
    { wxPAPER_9X11,                 _T(""),   0,0 },  //"9 x 11 in"), 2286, 2794);
    { wxPAPER_10X11,                _T(""),   0,0 },  //"10 x 11 in"), 2540, 2794);
    { wxPAPER_15X11,                _T(""),   0,0 },  //"15 x 11 in"), 3810, 2794);
    { wxPAPER_LETTER_EXTRA,         _T(""),   0,0 },  //"Letter Extra 9 1/2 x 12 in"), 2413, 3048);
    { wxPAPER_LEGAL_EXTRA,          _T(""),   0,0 },  //"Legal Extra 9 1/2 x 15 in"), 2413, 3810);
    { wxPAPER_TABLOID_EXTRA,        _T(""),   0,0 },  //"Tabloid Extra 11.69 x 18 in"), 2969, 4572);
    { wxPAPER_A4_EXTRA,             _T(""),   0,0 },  //"A4 Extra 9.27 x 12.69 in"), 2355, 3223);
    { wxPAPER_LETTER_TRANSVERSE,    _T(""),   0,0 },  //"Letter Transverse 8 1/2 x 11 in"), 2159, 2794);
    { wxPAPER_A4_TRANSVERSE,        _T(""),   0,0 },  //"A4 Transverse 210 x 297 mm"), 2100, 2970);
    { wxPAPER_LETTER_EXTRA_TRANSVERSE, _T(""),   0,0 },  //"Letter Extra Transverse 9.275 x 12 in"), 2355, 3048);
    { wxPAPER_A_PLUS,               _T(""),   0,0 },  //"SuperA/SuperA/A4 227 x 356 mm"), 2270, 3560);
    { wxPAPER_B_PLUS,               _T(""),   0,0 },  //"SuperB/SuperB/A3 305 x 487 mm"), 3050, 4870);
    { wxPAPER_LETTER_PLUS,          _T(""),   0,0 },  //"Letter Plus 8 1/2 x 12.69 in"), 2159, 3223);
    { wxPAPER_A4_PLUS,              _T(""),   0,0 },  //"A4 Plus 210 x 330 mm"), 2100, 3300);
    { wxPAPER_A5_TRANSVERSE,        _T(""),   0,0 },  //"A5 Transverse 148 x 210 mm"), 1480, 2100);
    { wxPAPER_B5_TRANSVERSE,        _T(""),   0,0 },  //"B5 (JIS) Transverse 182 x 257 mm"), 1820, 2570);
    { wxPAPER_A3_EXTRA,             _T(""),   0,0 },  //"A3 Extra 322 x 445 mm"), 3220, 4450);
    { wxPAPER_A5_EXTRA,             _T(""),   0,0 },  //"A5 Extra 174 x 235 mm"), 1740, 2350);
    { wxPAPER_B5_EXTRA,             _T(""),   0,0 },  //"B5 (ISO) Extra 201 x 276 mm"), 2010, 2760);
    { wxPAPER_A2,                   _T(""),   0,0 },  //"A2 420 x 594 mm"), 4200, 5940);
    { wxPAPER_A3_TRANSVERSE,        _T(""),   0,0 },  //"A3 Transverse 297 x 420 mm"), 2970, 4200);
    { wxPAPER_A3_EXTRA_TRANSVERSE,  _T(""),   0,0 },  //"A3 Extra Transverse 322 x 445 mm"), 3220, 4450);
    { wxPAPER_DBL_JAPANESE_POSTCARD, _T(""),   0,0 },  //"Japanese Double Postcard 200 x 148 mm"), 2000, 1480);
    { wxPAPER_A6,                  _T(""),   0,0 },  //"A6 105 x 148 mm"), 1050, 1480);
    { wxPAPER_LETTER_ROTATED,      _T(""),   0,0 },  //"Letter Rotated 11 x 8 1/2 in"), 2794, 2159);
    { wxPAPER_A3_ROTATED,          _T(""),   0,0 },  //"A3 Rotated 420 x 297 mm"), 4200, 2970);
    { wxPAPER_A4_ROTATED,          _T(""),   0,0 },  //"A4 Rotated 297 x 210 mm"), 2970, 2100);
    { wxPAPER_A5_ROTATED,          _T(""),   0,0 },  //"A5 Rotated 210 x 148 mm"), 2100, 1480);
    { wxPAPER_B4_JIS_ROTATED,      _T(""),   0,0 },  //"B4 (JIS) Rotated 364 x 257 mm"), 3640, 2570);
    { wxPAPER_B5_JIS_ROTATED,      _T(""),   0,0 },  //"B5 (JIS) Rotated 257 x 182 mm"), 2570, 1820);
    { wxPAPER_A6_ROTATED,          _T(""),   0,0 },  //"A6 Rotated 148 x 105 mm"), 1480, 1050);
    { wxPAPER_B6_JIS,              _T(""),   0,0 },  //"B6 (JIS) 128 x 182 mm"), 1280, 1820);
    { wxPAPER_B6_JIS_ROTATED,      _T(""),   0,0 },  //"B6 (JIS) Rotated 182 x 128 mm"), 1920, 1280);
    { wxPAPER_12X11,               _T(""),   0,0 },  //"12 x 11 in"), 3048, 2794);
    { wxPAPER_P16K,                _T(""),   0,0 },  //"PRC 16K 146 x 215 mm"), 1460, 2150);
    { wxPAPER_P32K,                _T(""),   0,0 },  //"PRC 32K 97 x 151 mm"), 970, 1510);
    { wxPAPER_P32KBIG,             _T(""),   0,0 },  //"PRC 32K(Big) 97 x 151 mm"), 970, 1510);
    { wxPAPER_P16K_ROTATED,        _T(""),   0,0 },  //"PRC 16K Rotated"), 2150, 1460);
    { wxPAPER_P32K_ROTATED,        _T(""),   0,0 },  //"PRC 32K Rotated"), 1510, 970);
    { wxPAPER_P32KBIG_ROTATED,     _T(""),   0,0 },  //"PRC 32K(Big) Rotated"), 1510, 970);
};



//paper to use
static int m_nSelPaper = 1;     //DIN A4


//--------------------------------------------------------------------------------
// lmScoreWizard implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( lmScoreWizard, lmWizard )
    EVT_WIZARD_CANCEL( lmID_SCORE_WIZARD, lmScoreWizard::OnWizardCancel )
    EVT_WIZARD_FINISHED( lmID_SCORE_WIZARD, lmScoreWizard::OnWizardFinished )
END_EVENT_TABLE()


lmScoreWizard::lmScoreWizard(wxWindow* parent, lmScore** pPtrScore, wxWindowID id,
                             const wxPoint& pos)
    : lmWizard(parent, id, _("Score configuration wizard"), pos)
{
    SetExtraStyle(GetExtraStyle() | wxWIZARD_EX_HELPBUTTON);

    m_pPtrScore = pPtrScore;

    //initialize default score configuration

    //time signature
    m_ScoreData.nTimeType = emtr44;             //initial time signature:  4/4

    ////titles
    //lmTitleData* pTitle = new lmTitleData;
    //pTitle->sTitle = _("Score title");
    //pTitle->nAlign = lmALIGN_CENTER;
    //pTitle->tPos.xType = lmLOCATION_DEFAULT;
    //pTitle->tPos.xUnits = lmTENTHS;
    //pTitle->tPos.yType = lmLOCATION_DEFAULT;
    //pTitle->tPos.yUnits = lmTENTHS;
    //pTitle->tPos.x = 0.0f;
    //pTitle->tPos.y = 0.0f;
    //pTitle->sFontName = _T("Times New Roman");
    //pTitle->nFontSize = 14;
    //pTitle->nStyle = lmTEXT_BOLD;
    //
    //m_Titles.push_back( pTitle );

    ////one instrument
    //lmInstrData* pInstrData = new lmInstrData;
    //pInstrData->sName = _T("");         //instrument name
    //pInstrData->sAbbrev = _T("");       //instrument abbreviation
    //pInstrData->nMidiChanel = 0;        //MIDI channel
    //pInstrData->nMidiInstr = 0;         //MIDI instr
    //pInstrData->nClefType = lmE_Sol;    //initial clef
    //pInstrData->nFifths = 0;            //initial key: C major
    //pInstrData->fMajor = true;;

    //m_Instruments.push_back( pInstrData );

    //initialize paper strings and sizes
    int nNumPapers = sizeof(m_Papers) / sizeof(lmPaperSizeData);
    for (int i=0; i < nNumPapers; i++)
    {
        if ( m_Papers[i].nId != 0)
        {
            //take info from wxWidgets database
            //Note: wxThePrintPaperDatabase is defined in common/paper.cpp
            wxPrintPaperType* paper = wxThePrintPaperDatabase->FindPaperType( m_Papers[i].nId );
            m_Papers[i].sName = paper->GetName();
            m_Papers[i].nWidth = paper->GetWidth();
            m_Papers[i].nHeight = paper->GetHeight();
        }
    }

    //create the pages in the order of presentation
    lmScoreWizardLayout* pPageLayout = new lmScoreWizardLayout((wxWizard*)this);
    AddPage(pPageLayout, true);     //true -> Optional

    lmScoreWizardInstrPage* pPageInstr = new lmScoreWizardInstrPage((wxWizard*)this);
    AddPage(pPageInstr);
    ////FitToPage(pPageInstr);

    //wxWizardPageSimple::Chain(pPageLayout, pPageInstr);
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

void lmScoreWizard::OnWizardFinished( wxWizardEvent& event )
{
    lmScore* pScore = (lmScore*)NULL;
    wxString sFile = m_Templates[m_nSelTemplate].sTemplate;

    //create the basic score
    if (sFile != _T(""))
    {
        //load score from LDP template

        lmLDPParser parser;
        wxString sPath = g_pPaths->GetTemplatesPath();
        wxFileName oFileName(sPath, sFile, wxPATH_NATIVE);
        pScore = parser.ParseFile( oFileName.GetFullPath(), false );        //false -> No message if file not found
        if (!pScore)
        {
            wxString sMsg = wxString::Format(
                wxGetTranslation(
				    _T("Error: Template '%s' not found\n\n")
                    _T("Posible causes:\n")
                    _T("- An error during lenmus installation.\n")
                    _T("- An accidental deletion of the required template.") ), sFile.c_str());

            lmErrorBox oEB(sMsg, _("An empty score will be created."));
            oEB.ShowModal();
        }
        //{
        //    //add instruments
        //    std::vector<lmInstrData*>::iterator itI;
        //    for (itI = m_Instruments.begin(); itI != m_Instruments.end(); ++itI)
        //    {
        //        lmInstrument* pInstr =
        //            pScore->AddInstrument((*itI)->nMidiChanel, (*itI)->nMidiInstr, (*itI)->sName,
        //                                (*itI)->sAbbrev );
        //        lmVStaff *pVStaff = pInstr->GetVStaff();
        //        pVStaff->AddClef( (*itI)->nClefType );
        //        pVStaff->AddKeySignature( (*itI)->nFifths, (*itI)->fMajor );
        //        pVStaff->AddTimeSignature( m_ScoreData.nTimeType );
	       //     //pVStaff->AddBarline(lm_eBarlineEOS, true);
        //    }
        //}

    }

    //if no template, or template load failure, create a basic score
    if (!pScore)
    {
        // create an empty score

        pScore = new lmScore();
        pScore->AddInstrument(0,0,_T(""));			//MIDI channel 0, MIDI instr 0
        //lmInstrument* pInstr = pScore->AddInstrument(0,0,_T(""));			//MIDI channel 0, MIDI instr 0
        //lmVStaff *pVStaff = pInstr->GetVStaff();
	    //pVStaff->AddBarline(lm_eBarlineEOS, true);

        //In scores created in the score editor, we should render a full page,
        //with empty staves. To this end, we need to change some options default value
        pScore->SetOption(_T("Score.FillPageWithEmptyStaves"), true);
    }
    pScore->SetOption(_T("StaffLines.StopAtFinalBarline"), false);


    // Modify score with user options (paper size, margins, titles, etc.)

    //set paper settings
    //m_ScoreData.nPageSize = wxSize(m_Papers[m_nSelPaper].nWidth / 10,
    //                               m_Papers[m_nSelPaper].nHeight / 10 );
    m_ScoreData.nTopMargin = 20;                //20 mm
    m_ScoreData.nBottomMargin = 20;             //20 mm;
    m_ScoreData.nLeftMargin = 20;               //20 mm;
    m_ScoreData.nRightMargin = 20;              //20 mm;
    m_ScoreData.nBindingMargin = 0;             //no binding margin

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
    wxWizardPageSimple::Create(parent, NULL, NULL, wxNullBitmap);
    CreateControls();
    GetSizer()->Fit(this);

        // populate controls

    //available layouts
    int nNumTemplates = sizeof(m_Templates) / sizeof(lmTemplateData);
    for (int i=0; i < nNumTemplates; i++)
        m_pLstEnsemble->Append( m_Templates[i].sName );
    m_pLstEnsemble->SetSelection(m_nSelTemplate);

    //paper size
    int nNumPapers = sizeof(m_Papers) / sizeof(lmPaperSizeData);
    for (int i=0; i < nNumPapers; i++)
        m_pCboPaper->Append( m_Papers[i].sName );
    m_pCboPaper->SetSelection(m_nSelPaper);

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

	m_pBmpPreview = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN );
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
    m_nSelPaper = m_pCboPaper->GetSelection();

    if (m_nSelPaper != -1)
    {
        wxPrintPaperType* paper = wxThePrintPaperDatabase->Item(m_nSelPaper);
        if ( paper )
            m_ScoreData.nPageSize = wxSize(paper->GetWidth()/10, paper->GetHeight()/10 );
    }

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
    wxWizardPageSimple::Create(parent, NULL, NULL, wxNullBitmap);
    CreateControls();
    GetSizer()->Fit(this);

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
    return true;
}
