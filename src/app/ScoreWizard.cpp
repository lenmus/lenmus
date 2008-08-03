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
        { _("Choir 4 voices (SATB)"),       _T("choir_SATB.lms"),   false },
        { _("Choir SATB + piano"),          _T("x"),    true },
        { _("Choir 3 voices (SSA)"),        _T("x"),    true },
        { _("Choir SSA + piano"),           _T("x"),    true },
        { _("Flute"),                       _T("flute.lms"),        true },
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

// control identifiers
enum 
{
    //lmScoreWizard
    lmID_SCORE_WIZARD = 10000,

    //lmScoreWizardLayout page
    lmID_LIST_ENSEMBLE,

    //lmScoreWizardKey page
	lmID_RADIO_KEY,
	lmID_COMBO_KEY,

    //lmScoreWizardTime page
	lmID_RADIO_TIME,
	lmID_TIME_TOP_NUMBER,
	lmID_TIME_BOTTOM_NUMBER,

    //lmScoreWizardTitles page

};


BEGIN_EVENT_TABLE( lmScoreWizard, lmWizard )
    EVT_WIZARD_CANCEL( lmID_SCORE_WIZARD, lmScoreWizard::OnWizardCancel )
    EVT_WIZARD_FINISHED( lmID_SCORE_WIZARD, lmScoreWizard::OnWizardFinished )
END_EVENT_TABLE()

lmScoreWizard::lmScoreWizard(wxWindow* parent, lmScore** pPtrScore)
    : lmWizard(parent, lmID_SCORE_WIZARD, _("Score configuration wizard"), wxDefaultPosition)
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

    lmScoreWizardKey* pPageKey = new lmScoreWizardKey((wxWizard*)this);
    AddPage(pPageKey, true);     //true -> Optional

    lmScoreWizardTime* pPageTime = new lmScoreWizardTime((wxWizard*)this);
    AddPage(pPageTime, true);     //true -> Optional

    lmScoreWizardTitles* pPageTitles = new lmScoreWizardTitles((wxWizard*)this);
    AddPage(pPageTitles, true);     //true -> Optional
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

    //add key signature 
    //TODO: How?

    //add time signature 
    //TODO: How?


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
    EVT_LISTBOX(lmID_LIST_ENSEMBLE, OnEnsembleSelected)

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

	m_pLstEnsemble = new wxListBox( this, lmID_LIST_ENSEMBLE, wxDefaultPosition, wxSize( 240,-1 ), 0, NULL, 0 );
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

void lmScoreWizardLayout::OnEnsembleSelected(wxCommandEvent& event)
{
    m_nSelTemplate = m_pLstEnsemble->GetSelection();
    bool fPortrait = m_Templates[m_nSelTemplate].fPortrait;
    m_pRadOrientation->SetSelection(fPortrait ? 1 : 0);
}

bool lmScoreWizardLayout::TransferDataFromWindow()
{
    // when moving to another page this method is automatically invoked to
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
// lmScoreWizardKey implementation
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( lmScoreWizardKey, wxWizardPageSimple )

BEGIN_EVENT_TABLE( lmScoreWizardKey, wxWizardPageSimple )
    EVT_RADIOBOX (lmID_RADIO_KEY, lmScoreWizardKey::OnKeyType)
END_EVENT_TABLE()

#define lmMAX_MINOR_KEYS    lmMAX_MINOR_KEY - lmMIN_MINOR_KEY + 1
#define lmMAX_MAJOR_KEYS    lmMAX_MAJOR_KEY - lmMIN_MAJOR_KEY + 1

static lmScoreWizardKey::lmKeysData m_tMajorKeys[lmMAX_MAJOR_KEYS];
static lmScoreWizardKey::lmKeysData m_tMinorKeys[lmMAX_MINOR_KEYS];


lmScoreWizardKey::lmScoreWizardKey()
{
}

lmScoreWizardKey::lmScoreWizardKey(wxWizard* parent)
{
    Create(parent);

    //load initial data
    m_pKeyRadioBox->SetSelection(0);
    LoadKeyList(0);
}

bool lmScoreWizardKey::Create(wxWizard* parent)
{
    //To avoid having to translate again key signature names, we are going to load them
    //by using global function GetKeySignatureName()
    for (int j=0, i = lmMIN_MAJOR_KEY; i <= lmMAX_MAJOR_KEY; i++, j++)
    {
        m_tMajorKeys[j].nKeyType = (lmEKeySignatures)i;
        m_tMajorKeys[j].sKeyName = GetKeySignatureName((lmEKeySignatures)i);    //wxString::Format(_T("%s (%d%s)"),;
        m_tMajorKeys[j].nFifths = KeySignatureToNumFifths((lmEKeySignatures)i);
    }

    for (int j=0, i = lmMIN_MINOR_KEY; i <= lmMAX_MINOR_KEY; i++, j++)
    {
        m_tMinorKeys[j].nKeyType = (lmEKeySignatures)i;
        m_tMinorKeys[j].sKeyName = GetKeySignatureName((lmEKeySignatures)i);    //wxString::Format(_T("%s (%d%s)"),;
        m_tMinorKeys[j].nFifths = KeySignatureToNumFifths((lmEKeySignatures)i);
    }

    //// member initialisation
    //m_pOutCombo = NULL;
    //m_pInCombo = NULL;

    // page creation
    wxWizardPageSimple::Create(parent, NULL, NULL, wxNullBitmap);
    CreateControls();
    GetSizer()->Fit(this);

    return true;
}

void lmScoreWizardKey::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* pLeftColumnSizer;
	pLeftColumnSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* pKeySizer;
	pKeySizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Select a key signature") ), wxVERTICAL );
	
	wxString m_pKeyRadioBoxChoices[] = { _("Major"), _("minor") };
	int m_pKeyRadioBoxNChoices = sizeof( m_pKeyRadioBoxChoices ) / sizeof( wxString );
	m_pKeyRadioBox = new wxRadioBox( this, lmID_RADIO_KEY, _("Key type"), wxDefaultPosition, wxDefaultSize, m_pKeyRadioBoxNChoices, m_pKeyRadioBoxChoices, 1, wxRA_SPECIFY_ROWS );
	m_pKeyRadioBox->SetSelection( 0 );
	pKeySizer->Add( m_pKeyRadioBox, 0, wxALL, 5 );
	
    m_pKeyList = new wxBitmapComboBox();
    m_pKeyList->Create(this, lmID_COMBO_KEY, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
                       0, NULL, wxCB_READONLY);
	pKeySizer->Add( m_pKeyList, 0, wxALL, 5 );
	
	pLeftColumnSizer->Add( pKeySizer, 1, wxEXPAND|wxALL, 5 );
	
	pMainSizer->Add( pLeftColumnSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* pRightColumnSizer;
	pRightColumnSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* pPreviewSizer;
	pPreviewSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Preview") ), wxVERTICAL );
	
	m_pBmpPreview = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
	pPreviewSizer->Add( m_pBmpPreview, 1, wxALL|wxEXPAND, 5 );
	
	pRightColumnSizer->Add( pPreviewSizer, 1, wxEXPAND, 5 );
	
	pMainSizer->Add( pRightColumnSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( pMainSizer );
}

bool lmScoreWizardKey::TransferDataFromWindow()
{
    // when moving to another page this method is automatically invoked to
    // verify that the data entered is correct before passing to the next page,
    // and to copy entered data to the main page
    // Returns true to allow moving to next page.
    // If false is returned it should display a meesage box to the user to explain
    // the reason

    //m_ScoreData.fPortrait = (m_pRadOrientation->GetSelection() == 1);
    //m_nSelTemplate = m_pLstEnsemble->GetSelection();
    //m_nSelPaper = m_pCboPaper->GetSelection();

	int iK = m_pKeyList->GetSelection();
    bool fMajor = (m_pKeyRadioBox->GetSelection() == 0);
    int nFifths = 0;
    if (fMajor)
        nFifths = m_tMajorKeys[iK].nFifths;
    else
        nFifths = m_tMinorKeys[iK].nFifths;

    return true;
}

void lmScoreWizardKey::LoadKeyList(int nType)
{
    //nType: 0=major, 1=minor

    if (nType==0)
    {
        m_pKeyList->Clear();
        for (int i=0; i < lmMAX_MAJOR_KEYS; i++)
        {
            wxString sKeyName = m_tMajorKeys[i].sKeyName;
            m_pKeyList->Append(wxEmptyString, GenerateBitmap(sKeyName));
        }
    }
    else
    {
        m_pKeyList->Clear();
        for (int i=0; i < lmMAX_MINOR_KEYS; i++)
        {
            wxString sKeyName = m_tMinorKeys[i].sKeyName;
            m_pKeyList->Append(wxEmptyString, GenerateBitmap(sKeyName));
        }
    }
    m_pKeyList->SetSelection(0);
}

void lmScoreWizardKey::OnKeyType(wxCommandEvent& event)
{
    //load list box with the appropiate keys for selected key type

    LoadKeyList(event.GetSelection());
}

wxBitmap lmScoreWizardKey::GenerateBitmap(wxString sKeyName)
{
    wxMemoryDC dc;
    wxSize size(108, 64);
	wxBitmap bmp(size.x, size.y);

    //fill bitmap in white
    dc.SelectObject(bmp);
    dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

    //draw rectangle and two red diagonals
    dc.SetPen(*wxBLACK);
    dc.DrawRectangle(0, 0, size.x, size.y);
    dc.SetPen(*wxRED);
    dc.DrawLine(0, 0, size.x, size.y);
    dc.DrawLine(0, size.y, size.x, 0);

    //write key signature name in black
    int h, w;
    dc.SetPen(*wxBLACK);
    dc.SetFont(*wxNORMAL_FONT);
    dc.GetTextExtent(sKeyName, &w, &h);
    dc.DrawText(sKeyName, (size.x-w)/2, (size.y-h)/2);

    //clean up and return new bitmap
    dc.SelectObject(wxNullBitmap);

    return bmp;
}



//--------------------------------------------------------------------------------
// lmScoreWizardTime implementation
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( lmScoreWizardTime, wxWizardPageSimple )

BEGIN_EVENT_TABLE( lmScoreWizardTime, wxWizardPageSimple )
    //
END_EVENT_TABLE()

lmScoreWizardTime::lmScoreWizardTime()
{
}

lmScoreWizardTime::lmScoreWizardTime(wxWizard* parent)
{
    Create(parent);
}

bool lmScoreWizardTime::Create(wxWizard* parent)
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

void lmScoreWizardTime::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* pLeftColumnSizer;
	pLeftColumnSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* pTimeSizer;
	pTimeSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Select time signature") ), wxVERTICAL );
	
	wxString m_pTimeRadioBoxChoices[] = { _("2/2"), _("2/4"), _("6/8"), _("2/8"), _("3/2"), _("3/4"), _("9/8"), _("3/8"), _("4/2"), _("4/4"), _("12/8"), _("4/8"), _("other"), _("none") };
	int m_pTimeRadioBoxNChoices = sizeof( m_pTimeRadioBoxChoices ) / sizeof( wxString );
	m_pTimeRadioBox = new wxRadioBox( this, lmID_RADIO_TIME, _("Time signature"), wxDefaultPosition, wxDefaultSize, m_pTimeRadioBoxNChoices, m_pTimeRadioBoxChoices, 4, wxRA_SPECIFY_COLS );
	m_pTimeRadioBox->SetSelection( 9 );
	pTimeSizer->Add( m_pTimeRadioBox, 0, wxALL, 5 );
	
	wxStaticBoxSizer* pOtherTimeSizer;
	pOtherTimeSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Other time signatures") ), wxVERTICAL );
	
	wxBoxSizer* pTopNumSizer;
	pTopNumSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_pLblTopNumber = new wxStaticText( this, wxID_ANY, _("Numerator"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblTopNumber->Wrap( -1 );
	pTopNumSizer->Add( m_pLblTopNumber, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_pTxtTopNumber = new wxTextCtrl( this, lmID_TIME_TOP_NUMBER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pTopNumSizer->Add( m_pTxtTopNumber, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	pOtherTimeSizer->Add( pTopNumSizer, 0, 0, 5 );
	
	wxBoxSizer* pBottomNumSizer;
	pBottomNumSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_pLblBottomNum = new wxStaticText( this, wxID_ANY, _("Denominator"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblBottomNum->Wrap( -1 );
	pBottomNumSizer->Add( m_pLblBottomNum, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pTxtBottomNumber = new wxTextCtrl( this, lmID_TIME_BOTTOM_NUMBER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pBottomNumSizer->Add( m_pTxtBottomNumber, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	pOtherTimeSizer->Add( pBottomNumSizer, 0, 0, 5 );
	
	pTimeSizer->Add( pOtherTimeSizer, 0, wxEXPAND, 5 );
	
	pLeftColumnSizer->Add( pTimeSizer, 1, wxALL|wxEXPAND, 5 );
	
	pMainSizer->Add( pLeftColumnSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* pRightColumnSizer;
	pRightColumnSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* pPreviewSizer;
	pPreviewSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Preview") ), wxVERTICAL );
	
	m_pBmpPreview = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
	pPreviewSizer->Add( m_pBmpPreview, 1, wxALL|wxEXPAND, 5 );
	
	pRightColumnSizer->Add( pPreviewSizer, 1, wxEXPAND|wxALL, 5 );
	
	pMainSizer->Add( pRightColumnSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( pMainSizer );
}

bool lmScoreWizardTime::TransferDataFromWindow()
{
    // when moving to another page this method is automatically invoked to
    // verify that the data entered is correct before passing to the next page,
    // and to copy entered data to the main page
    // Returns true to allow moving to next page.
    // If false is returned it should display a meesage box to the user to explain
    // the reason

    //m_ScoreData.fPortrait = (m_pRadOrientation->GetSelection() == 1);
    //m_nSelTemplate = m_pLstEnsemble->GetSelection();
    //m_nSelPaper = m_pCboPaper->GetSelection();

    //if (m_nSelPaper != -1)
    //{
    //    wxPrintPaperType* paper = wxThePrintPaperDatabase->Item(m_nSelPaper);
    //    if ( paper )
    //        m_ScoreData.nPageSize = wxSize(paper->GetWidth()/10, paper->GetHeight()/10 );
    //}

    return true;
}




//--------------------------------------------------------------------------------
// lmScoreWizardTitles implementation
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( lmScoreWizardTitles, wxWizardPageSimple )

BEGIN_EVENT_TABLE( lmScoreWizardTitles, wxWizardPageSimple )
    //
END_EVENT_TABLE()

lmScoreWizardTitles::lmScoreWizardTitles()
{
}

lmScoreWizardTitles::lmScoreWizardTitles(wxWizard* parent)
{
    Create(parent);
}

bool lmScoreWizardTitles::Create(wxWizard* parent)
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

void lmScoreWizardTitles::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* pLeftColumnSizer;
	pLeftColumnSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Add titles") ), wxVERTICAL );
	
	m_pLblTitle = new wxStaticText( this, wxID_ANY, _("Title:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblTitle->Wrap( -1 );
	pLeftColumnSizer->Add( m_pLblTitle, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_pTxtTitle = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pLeftColumnSizer->Add( m_pTxtTitle, 0, wxEXPAND|wxALL, 5 );
	
	m_pLblSubtitle = new wxStaticText( this, wxID_ANY, _("Subtitle:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblSubtitle->Wrap( -1 );
	pLeftColumnSizer->Add( m_pLblSubtitle, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_pTxtSubtitle = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pLeftColumnSizer->Add( m_pTxtSubtitle, 0, wxEXPAND|wxALL, 5 );
	
	m_pLblComposer = new wxStaticText( this, wxID_ANY, _("Composer:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblComposer->Wrap( -1 );
	pLeftColumnSizer->Add( m_pLblComposer, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_pTxtComposer = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pLeftColumnSizer->Add( m_pTxtComposer, 0, wxALL|wxEXPAND, 5 );
	
	m_pLblArranger = new wxStaticText( this, wxID_ANY, _("Arranger:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblArranger->Wrap( -1 );
	pLeftColumnSizer->Add( m_pLblArranger, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_pTxtArranger = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pLeftColumnSizer->Add( m_pTxtArranger, 0, wxALL|wxEXPAND, 5 );
	
	m_pLblLyricist = new wxStaticText( this, wxID_ANY, _("Lyricist:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblLyricist->Wrap( -1 );
	pLeftColumnSizer->Add( m_pLblLyricist, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_pTxtLyricist = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pLeftColumnSizer->Add( m_pTxtLyricist, 0, wxALL|wxEXPAND, 5 );
	
	pMainSizer->Add( pLeftColumnSizer, 1, wxEXPAND|wxALL, 10 );
	
	this->SetSizer( pMainSizer );
}

bool lmScoreWizardTitles::TransferDataFromWindow()
{
    // when moving to another page this method is automatically invoked to
    // verify that the data entered is correct before passing to the next page,
    // and to copy entered data to the main page
    // Returns true to allow moving to next page.
    // If false is returned it should display a meesage box to the user to explain
    // the reason

    //m_ScoreData.fPortrait = (m_pRadOrientation->GetSelection() == 1);
    //m_nSelTemplate = m_pLstEnsemble->GetSelection();
    //m_nSelPaper = m_pCboPaper->GetSelection();

    //if (m_nSelPaper != -1)
    //{
    //    wxPrintPaperType* paper = wxThePrintPaperDatabase->Item(m_nSelPaper);
    //    if ( paper )
    //        m_ScoreData.nPageSize = wxSize(paper->GetWidth()/10, paper->GetHeight()/10 );
    //}

    return true;
}
