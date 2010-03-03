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
#include "../score/VStaff.h"
#include "../score/Instrument.h"
#include "../widgets/MsgBox.h"
#include "../ldp_parser/LDPParser.h"
#include "../graphic/GraphicManager.h"      //to use GenerateBitmapForKeyCtrol()

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

    //initial key signature
    bool        fAddKey;
    bool        fMajor;
    int         nFifths;

    //initial time signature
    bool        fAddTime;
    int         nBeats;
    int         nBeatType;

    //titles
    bool        fAddTitles;

} lmScoreData;

static lmScoreData m_ScoreData;


//score titles -----------------------------
typedef struct lmTitleDataStruct
{
    wxString        sStyleName;
    wxString        sTitle;
    lmEHAlign       nHAlign;
    lmLocation      tPos;
    wxString        sFontName;
    int             nFontSize;
    int             nFontStyle;
    wxFontWeight    nFontWeight;

} lmTitleData;

enum {
    lmTITLE = 0,
    lmSUBTITLE,
    lmCOMPOSER,
    lmARRANGER,
    lmLYRICIST,
    //
    lmNUM_TITLES
};

static lmTitleData  m_Titles[lmNUM_TITLES];


//info about templates ---------------------------
class lmTemplateData
{
public:

    lmTemplateData() {}
    lmTemplateData(wxString name, wxString tpl, bool portrait) :
        sName(name), sTemplate(tpl), fPortrait(portrait) {}

    wxString    sName;          //name to display
    wxString    sTemplate;      //associated template file
    bool        fPortrait;      //default paper orientation
};

#define lmNUM_TEMPLATES 18
static lmTemplateData m_Templates[lmNUM_TEMPLATES];
static bool m_fStringsInitialized = false;


//template to use
static const int m_nEmptyTemplate = 0;          //index to empty template (manuscript paper)
static int m_nSelTemplate = m_nEmptyTemplate;   //"Empty (manuscript paper)"


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
//is in this table (i.e.- the first two commented entries). Any other identifier means that
//paper data must be taken from wxWidgets DB
static lmPaperSizeData m_Papers[] = {
    //                              Displayed name                  Size
    //{ wxPAPER_NONE,                 _("DIN A3"),                    2970, 4200 },
    //{ wxPAPER_NONE,                 _("DIN A4"),                    2100, 2970 },
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
static int m_nSelPaper = 2;     //wxPAPER_A4


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
    EVT_WIZARD_PAGE_CHANGED(wxID_ANY, lmScoreWizard::OnPageChanged )
END_EVENT_TABLE()

lmScoreWizard::lmScoreWizard(wxWindow* parent, lmScore** pPtrScore)
    : lmWizard(parent, lmID_SCORE_WIZARD, _("Score configuration wizard"),
               wxDefaultPosition, wxSize(600, 430))
{
    SetExtraStyle(GetExtraStyle() | wxWIZARD_EX_HELPBUTTON);

    m_pPtrScore = pPtrScore;

    //load language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fStringsInitialized)
    {
        //AWARE: When addign more templates, update lmNUM_TEMPLATES;
        //                                  Displayed name                  Template                    Portrait
        m_Templates[0] =  lmTemplateData( _("Empty (manuscript paper)"),    _T(""),                     true );
        m_Templates[1] =  lmTemplateData( _("Brass quintet"),               _T("brass_quintet.lms"),    true );
        m_Templates[2] =  lmTemplateData( _("Brass trio"),                  _T("brass_trio.lms"),       true );
        m_Templates[3] =  lmTemplateData( _("Choir 4 voices (SATB)"),       _T("choir_SATB.lms"),       true );
        m_Templates[4] =  lmTemplateData( _("Choir SATB + piano"),          _T("choir_SATB_piano.lms"), true );
        m_Templates[5] =  lmTemplateData( _("Choir 3 voices (SSA)"),        _T("choir_SSA.lms"),        true );
        m_Templates[6] =  lmTemplateData( _("Choir SSA + piano"),           _T("choir_SSA_piano.lms"),  true );
        m_Templates[7] =  lmTemplateData( _("Flute"),                       _T("flute.lms"),            true );
        m_Templates[8] =  lmTemplateData( _("Guitar"),                      _T("guitar.lms"),           true );
        m_Templates[9] =  lmTemplateData( _("Piano"),                       _T("piano.lms"),            true );
        m_Templates[10] = lmTemplateData( _("Violin and piano"),            _T("violin_piano.lms"),     true );
        m_Templates[11] = lmTemplateData( _("Viola and piano"),             _T("viola_piano.lms"),      true );
        m_Templates[12] = lmTemplateData( _("Cello and piano"),             _T("cello_piano.lms"),      true );
        m_Templates[13] = lmTemplateData( _("String quartet"),              _T("string_quartet.lms"),   true );
        m_Templates[14] = lmTemplateData( _("Violin"),                      _T("violin.lms"),           true );
        m_Templates[15] = lmTemplateData( _("Trio sonata"),                 _T("trio_sonata.lms"),      true );
        m_Templates[16] = lmTemplateData( _("Woodwind trio"),               _T("woodwind_trio.lms"),    true );
        m_Templates[17] = lmTemplateData( _("Woodwind quintet"),            _T("woodwind_quintet.lms"), true );
        //AWARE: When addign more templates, update lmNUM_TEMPLATES;
        m_fStringsInitialized = true;
    }


    //initialize default score configuration

    //key signature
    m_ScoreData.fAddKey = false;
    m_ScoreData.nFifths = 0;            //Do major
    m_ScoreData.fMajor = true;

    //time signature
    m_ScoreData.fAddTime = false;
    m_ScoreData.nBeats = 4;         //initial time signature:  4/4
    m_ScoreData.nBeatType = 4;

    //paper orientation and margins
    m_ScoreData.fPortrait = true;          //Orientation (default: portrait)
    m_ScoreData.nTopMargin = 20;                //20 mm
    m_ScoreData.nBottomMargin = 20;             //20 mm;
    m_ScoreData.nLeftMargin = 20;               //20 mm;
    m_ScoreData.nRightMargin = 20;              //20 mm;
    m_ScoreData.nBindingMargin = 0;             //no binding margin


    //titles
    m_ScoreData.fAddTitles = false;
    for (int i=0; i < lmNUM_TITLES; ++i)
    {
        m_Titles[i].sTitle = _T("");
        m_Titles[i].tPos.xUnits = lmTENTHS;
        m_Titles[i].tPos.yUnits = lmTENTHS;
        m_Titles[i].tPos.x = 0.0f;
        m_Titles[i].tPos.y = 0.0f;
        m_Titles[i].sFontName = _T("Times New Roman");
        m_Titles[i].nFontWeight = wxFONTWEIGHT_NORMAL;
        m_Titles[i].nFontStyle = wxFONTSTYLE_NORMAL;
    }

    //Name of the style
    m_Titles[lmTITLE].sStyleName = _("Title");
    m_Titles[lmSUBTITLE].sStyleName = _("Subtitle");
    m_Titles[lmCOMPOSER].sStyleName = _("Composer");
    m_Titles[lmARRANGER].sStyleName = _("Arranger");
    m_Titles[lmLYRICIST].sStyleName = _("Lyricist");

    //titles alignment
    m_Titles[lmTITLE].nHAlign = lmHALIGN_CENTER;
    m_Titles[lmSUBTITLE].nHAlign = lmHALIGN_CENTER;
    m_Titles[lmCOMPOSER].nHAlign = lmHALIGN_RIGHT;
    m_Titles[lmARRANGER].nHAlign = lmHALIGN_LEFT;
    m_Titles[lmLYRICIST].nHAlign = lmHALIGN_RIGHT;

    //titles font size
    m_Titles[lmTITLE].nFontSize = 21;
    m_Titles[lmSUBTITLE].nFontSize = 16;
    m_Titles[lmCOMPOSER].nFontSize = 12;
    m_Titles[lmARRANGER].nFontSize = 12;
    m_Titles[lmLYRICIST].nFontSize = 12;

    //titles font style and weight
    m_Titles[lmTITLE].nFontWeight = wxFONTWEIGHT_BOLD;
    m_Titles[lmSUBTITLE].nFontStyle = wxFONTSTYLE_ITALIC;


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
}

void lmScoreWizard::OnWizardFinished( wxWizardEvent& event )
{
    PrepareScore();
}

void lmScoreWizard::OnWizardCancel( wxWizardEvent& event )
{
    //delete existing score
    if (*m_pPtrScore)
        delete *m_pPtrScore;
    *m_pPtrScore = (lmScore*)NULL;
}

void lmScoreWizard::OnPageChanged( wxWizardEvent& event )
{
    //The page has changed. The event page points to the page to be displayed now.
    //Inform the page

    ((lmWizardPage*)event.GetPage())->OnEnterPage();
}

void lmScoreWizard::PrepareScore()
{
    //delete existing score
    if (*m_pPtrScore)
        delete *m_pPtrScore;
    *m_pPtrScore = (lmScore*)NULL;

    lmScore* pScore = (lmScore*)NULL;
    wxString sFile = m_Templates[m_nSelTemplate].sTemplate;

    //create the basic score
    if (m_nSelTemplate != m_nEmptyTemplate && sFile != _T(""))
    {
        //load score from LDP template

        lmLDPParser parser;
        wxString sPath = g_pPaths->GetTemplatesPath();
        wxFileName oFileName(sPath, sFile, wxPATH_NATIVE);
        pScore = parser.ParseFile( oFileName.GetFullPath(), false );        //false -> No message if file not found

        if (pScore && (m_ScoreData.fAddKey || m_ScoreData.fAddTime))
        {
            //add key signature and time signature
            lmInstrument* pInstr = pScore->GetFirstInstrument();
            while (pInstr)
            {
                lmVStaff* pVStaff = pInstr->GetVStaff();

                //move cursor to this VStaff
                pScore->GetCursor()->MoveToEnd(pVStaff);

                //add key and time
                if (m_ScoreData.fAddKey)
                    pVStaff->AddKeySignature( m_ScoreData.nFifths, m_ScoreData.fMajor );
                if (m_ScoreData.fAddTime)
                    pVStaff->AddTimeSignature( m_ScoreData.nBeats, m_ScoreData.nBeatType );

                pInstr = pScore->GetNextInstrument();
            }
        }
    }

    //Create an empty score if user selected that option of template load failure
    if (m_nSelTemplate == m_nEmptyTemplate || pScore == (lmScore*)NULL)
    {
        //if template load failure, inform user
        if(m_nSelTemplate != m_nEmptyTemplate)
        {
            wxString sMsg = wxString::Format(_("Error: Template '%s' not found."), sFile.c_str());
            sMsg += _T("\n\n");
            sMsg += _("Posible causes:");
            sMsg += _T("\n");
            sMsg += _("- An error during lenmus installation.");
            sMsg += _T("\n");
            sMsg += _("- An accidental deletion of the required template.");

            lmErrorBox oEB(sMsg, _("An empty score will be created."));
            oEB.ShowModal();
        }

        // create an empty score

        pScore = new lmScore();
        pScore->AddInstrument(0,0,_T(""));   //MIDI channel 0, MIDI instr 0
        pScore->SetSystemDistance( lmToLogicalUnits(12, lmMILLIMETERS) );

        //In scores created in the score editor, we should render a full page,
        //with empty staves. To this end, we need to change some options default value
        pScore->SetOption(_T("Score.FillPageWithEmptyStaves"), true);
    }
    pScore->SetOption(_T("StaffLines.StopAtFinalBarline"), false);
    pScore->SetOption(_T("Score.JustifyFinalBarline"), false);


    // Modify score with user options (paper size, margins, etc.)

    //set paper settings
    pScore->SetPageLeftMargin( lmToLogicalUnits( m_ScoreData.nLeftMargin, lmMILLIMETERS) );
    pScore->SetPageRightMargin( lmToLogicalUnits( m_ScoreData.nRightMargin, lmMILLIMETERS) );
    pScore->SetPageTopMargin( lmToLogicalUnits( m_ScoreData.nTopMargin, lmMILLIMETERS) );
    pScore->SetPageBottomMargin( lmToLogicalUnits( m_ScoreData.nBottomMargin, lmMILLIMETERS) );
    pScore->SetPageBindingMargin( lmToLogicalUnits( m_ScoreData.nBindingMargin, lmMILLIMETERS) );
    pScore->SetPageSizeMillimeters( m_ScoreData.nPageSize );
    pScore->SetPageOrientation( m_ScoreData.fPortrait );


    //add default styles
    lmTextStyle* pTitleStyle[lmNUM_TITLES];
    if (pScore)
    {
        for (int i=0; i < lmNUM_TITLES; ++i)
        {
            lmFontInfo tFont;
            tFont.nFontSize = m_Titles[i].nFontSize;
            tFont.nFontStyle = m_Titles[i].nFontStyle;
            tFont.nFontWeight = m_Titles[i].nFontWeight;
            tFont.sFontName = m_Titles[i].sFontName;
            pTitleStyle[i] =
                pScore->AddStyle(m_Titles[i].sStyleName, tFont, *wxBLACK);
        }
    }

    //add titles

    int iPrev = -1;             //index to previous added title
    bool fFirstLR = true;
    if (pScore && m_ScoreData.fAddTitles)
    {
        //prepare a DC to measure texts
        wxClientDC dc(this);
        dc.SetMapMode(lmDC_MODE);
        dc.SetUserScale(lmSCALE, lmSCALE);      //any scale is ok, so use 1.0

        for (int i=0; i < lmNUM_TITLES; ++i)
        {
            if (!m_Titles[i].sTitle.IsEmpty())
            {
               //shift down the first left/right title
                if (i > lmSUBTITLE && fFirstLR)
                {
                    fFirstLR = false;
                    m_Titles[i].tPos.yUnits = lmMILLIMETERS;
                    m_Titles[i].tPos.y = 10.0f;
                }

                //Fix y_location if a left aligned title is followed by a right
                //aligned one. This last one must be repositioned in the same line
                //than the left aligned title, unless it doesn't fit in the
                //available line space.

                if (iPrev != -1 &&
                    m_Titles[iPrev].nHAlign == lmHALIGN_LEFT &&
                    m_Titles[i].nHAlign == lmHALIGN_RIGHT )
                {
                    //Adding a title automatically increments y-cursor by the height of
                    //the title. Therefore, it is only necessary to undo this increment
                    //if necessary

                    lmLUnits uLineSpace = pScore->GetRightMarginXPos() -
                                          pScore->GetLeftMarginXPos();

                    int nWidth, nHeight;
                    wxFont font1((int)PointsToLUnits(m_Titles[iPrev].nFontSize),
                                 wxFONTFAMILY_DEFAULT, m_Titles[iPrev].nFontStyle,
                                 m_Titles[iPrev].nFontWeight, false,
                                 m_Titles[iPrev].sFontName, wxFONTENCODING_DEFAULT);
                    dc.SetFont(font1);
                    dc.GetTextExtent(m_Titles[iPrev].sTitle, &nWidth, &nHeight);
                    uLineSpace -= (lmLUnits)nWidth;

                    wxFont font2((int)PointsToLUnits(m_Titles[i].nFontSize),
                                 wxFONTFAMILY_DEFAULT, m_Titles[i].nFontStyle,
                                 m_Titles[i].nFontWeight, false,
                                 m_Titles[i].sFontName, wxFONTENCODING_DEFAULT);
                    dc.SetFont(font2);
                    dc.GetTextExtent(m_Titles[i].sTitle, &nWidth, &nHeight);
                    uLineSpace -= (lmLUnits)nWidth;

                    if (uLineSpace > 0.0f)
                    {
                        //reposition title in the same line than previous one
                        m_Titles[i].tPos.y = - pScore->LogicalToTenths((lmLUnits)nHeight);
                        m_Titles[i].tPos.yUnits = lmTENTHS;
                    }
                }


                //add the title
                //lmFontInfo tFont;
                //tFont.nFontSize = m_Titles[i].nFontSize;
                //tFont.nFontStyle = m_Titles[i].nFontStyle;
                //tFont.nFontWeight = m_Titles[i].nFontWeight;
                //tFont.sFontName = m_Titles[i].sFontName;
                //lmTextStyle* pStyle =
                //    pScore->AddStyle(m_Titles[i].sStyleName, tFont, *wxBLACK);

                lmScoreTitle* pTitle =
                    pScore->AddTitle(m_Titles[i].sTitle, m_Titles[i].nHAlign, pTitleStyle[i]);
	            pTitle->SetUserLocation(m_Titles[i].tPos);
                iPrev = i;
            }
        }
    }

    //save the created score
    *m_pPtrScore = pScore;
}

void lmScoreWizard::UpdatePreview(wxStaticBitmap* pBmpPreview)
{
    wxSize size = pBmpPreview->GetSize();
    PrepareScore();

    //render score in a bitmap
	wxBitmap oBM = lmGenerateBitmap(*m_pPtrScore, size, 0.5);
    pBmpPreview->SetBitmap(oBM);
}

//--------------------------------------------------------------------------------
// lmScoreWizardLayout implementation
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( lmScoreWizardLayout, lmWizardPage )

BEGIN_EVENT_TABLE( lmScoreWizardLayout, lmWizardPage )
    EVT_LISTBOX(lmID_LIST_ENSEMBLE, lmScoreWizardLayout::OnEnsembleSelected)

END_EVENT_TABLE()


lmScoreWizardLayout::lmScoreWizardLayout()
    : lmWizardPage()
{
}

lmScoreWizardLayout::lmScoreWizardLayout(wxWizard* parent)
    : lmWizardPage(parent)
{
    Create(parent);
}

bool lmScoreWizardLayout::Create(wxWizard* parent)
{
    // page creation
    CreateControls();
    GetSizer()->Fit(this);

        // populate controls

    //available layouts
    for (int i=0; i < lmNUM_TEMPLATES; i++)
        m_pLstEnsemble->Append( m_Templates[i].sName );
    m_pLstEnsemble->SetSelection(m_nSelTemplate);

    //paper size
    int nNumPapers = sizeof(m_Papers) / sizeof(lmPaperSizeData);
    for (int i=0; i < nNumPapers; i++)
    {
        if (m_Papers[i].nId != 0)
            m_pCboPaper->Append(m_Papers[i].sName, (void *)i );
    }
    m_pCboPaper->SetSelection(m_nSelPaper);

    //paper orientation
    m_pRadOrientation->SetSelection(m_ScoreData.fPortrait ? 1 : 0);

    //bitmap preview
    ((lmScoreWizard*)parent)->UpdatePreview(m_pBmpPreview);

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

	m_pLstEnsemble = new wxListBox( this, lmID_LIST_ENSEMBLE, wxDefaultPosition, wxSize( -1,-1 ), 0, NULL, 0 );
	pLayoutSizer->Add( m_pLstEnsemble, 1, wxALL|wxEXPAND, 5 );

	pLeftColumnSizer->Add( pLayoutSizer, 1, wxEXPAND|wxALL, 5 );

	wxStaticBoxSizer* pPaperSizer;
	pPaperSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Select paper size and orientation") ), wxVERTICAL );
	
	wxBoxSizer* pCboSizeSizer;
	pCboSizeSizer = new wxBoxSizer( wxVERTICAL );
	
	m_pLblPaper = new wxStaticText( this, wxID_ANY, _("Paper size"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblPaper->Wrap( -1 );
	pCboSizeSizer->Add( m_pLblPaper, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_pCboPaperChoices;
	m_pCboPaper = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_pCboPaperChoices, 0 );
	m_pCboPaper->SetSelection( 0 );
	pCboSizeSizer->Add( m_pCboPaper, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	pPaperSizer->Add( pCboSizeSizer, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	wxString m_pRadOrientationChoices[] = { _("Landscape"), _("Portrait") };
	int m_pRadOrientationNChoices = sizeof( m_pRadOrientationChoices ) / sizeof( wxString );
	m_pRadOrientation = new wxRadioBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_pRadOrientationNChoices, m_pRadOrientationChoices, 1, wxRA_SPECIFY_ROWS );
	m_pRadOrientation->SetSelection( 0 );
	pPaperSizer->Add( m_pRadOrientation, 0, wxRIGHT|wxLEFT, 5 );

	pLeftColumnSizer->Add( pPaperSizer, 0, wxALL|wxEXPAND, 5 );

	pMainSizer->Add( pLeftColumnSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* pRightColumnSizer;
	pRightColumnSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Preview") ), wxVERTICAL );

	m_pBmpPreview = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(240,315), wxBORDER_SUNKEN );
	sbSizer4->Add( m_pBmpPreview, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	pRightColumnSizer->Add( sbSizer4, 1, wxEXPAND, 5 );

	pMainSizer->Add( pRightColumnSizer, 1, wxEXPAND, 5 );

	this->SetSizer( pMainSizer );
}

void lmScoreWizardLayout::OnEnsembleSelected(wxCommandEvent& event)
{
    m_nSelTemplate = m_pLstEnsemble->GetSelection();
    bool fPortrait = m_Templates[m_nSelTemplate].fPortrait;
    m_pRadOrientation->SetSelection(fPortrait ? 1 : 0);

    //When selecting template 'empty' do not allow next page. Only finish
    ((lmWizard*)GetParentWizard())->EnableButtonNext(m_nSelTemplate != m_nEmptyTemplate);

    //update preview
    ((lmScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
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
        #if defined(__ia64__) || defined(__amd64__)
            //In Linux 64bits next sentence produces a compilation error: cast from 'void*' to
            //'int' loses precision. This double cast fixes the issue.
            int iP = static_cast<int>(reinterpret_cast<long long>(m_pCboPaper->GetClientData(m_nSelPaper)));
        #else
            int iP = (int)m_pCboPaper->GetClientData(m_nSelPaper);
        #endif
        wxPaperSize nPaperId = m_Papers[iP].nId;
        wxPrintPaperType* paper = wxThePrintPaperDatabase->FindPaperType(nPaperId);
        if ( paper )
            m_ScoreData.nPageSize = wxSize(paper->GetWidth()/10, paper->GetHeight()/10 );
    }

    return true;
}

void lmScoreWizardLayout::OnEnterPage()
{
    //Mark all following pages as 'untouched' so that
    //if user finishes in this page, all following settings will not apply.
    //This is necessary in case user moves backwards and finish.

    m_ScoreData.fAddKey = false;
    m_ScoreData.fAddTime = false;
    m_ScoreData.fAddTitles = false;

    //When selecting template 'empty' do not allow next page. Only finish
    ((lmWizard*)GetParentWizard())->EnableButtonNext(m_nSelTemplate != m_nEmptyTemplate);
}


//--------------------------------------------------------------------------------
// lmScoreWizardKey implementation
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( lmScoreWizardKey, lmWizardPage )

BEGIN_EVENT_TABLE( lmScoreWizardKey, lmWizardPage )
    EVT_RADIOBOX (lmID_RADIO_KEY, lmScoreWizardKey::OnKeyType)
    EVT_COMBOBOX (lmID_COMBO_KEY, lmScoreWizardKey::OnComboKey)
END_EVENT_TABLE()


//keys data
typedef struct lmKeysStruct
{
    wxString            sKeyName;
    int                 nFifths;
    lmEKeySignatures    nKeyType;

} lmKeysData;

#define lmMAX_MINOR_KEYS    lmMAX_MINOR_KEY - lmMIN_MINOR_KEY + 1
#define lmMAX_MAJOR_KEYS    lmMAX_MAJOR_KEY - lmMIN_MAJOR_KEY + 1

static lmKeysData m_tMajorKeys[lmMAX_MAJOR_KEYS];
static lmKeysData m_tMinorKeys[lmMAX_MINOR_KEYS];


lmScoreWizardKey::lmScoreWizardKey()
    : lmWizardPage()
{
}

lmScoreWizardKey::lmScoreWizardKey(wxWizard* parent)
    : lmWizardPage(parent)
{
    Create(parent);
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

    // page creation
    CreateControls();
    GetSizer()->Fit(this);

    //initial selection
    int nType = (m_ScoreData.fMajor ? 0 : 1);
    m_pKeyRadioBox->SetSelection(nType);
    LoadKeyList(nType);
    m_pKeyList->SetSelection(m_ScoreData.nFifths);

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

	m_pBmpPreview = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(240,315), wxSUNKEN_BORDER );
	pPreviewSizer->Add( m_pBmpPreview, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

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

    //Get info about selected key signature
	int iK = m_pKeyList->GetSelection();
    m_ScoreData.fAddKey = true;
    m_ScoreData.fMajor = (m_pKeyRadioBox->GetSelection() == 0);
    if (m_ScoreData.fMajor)
        m_ScoreData.nFifths = m_tMajorKeys[iK].nFifths;
    else
        m_ScoreData.nFifths = m_tMinorKeys[iK].nFifths;

    return true;
}

void lmScoreWizardKey::LoadKeyList(int nType)
{
    //nType: 0=major, 1=minor

	int iK = m_pKeyList->GetSelection();
    if (nType==0)
    {
        m_pKeyList->Clear();
        for (int i=0; i < lmMAX_MAJOR_KEYS; i++)
        {
            m_pKeyList->Append(wxEmptyString,
                               GenerateBitmapForKeyCtrol(m_tMajorKeys[i].sKeyName,
                                                         m_tMajorKeys[i].nKeyType) );
        }
    }
    else
    {
        m_pKeyList->Clear();
        for (int i=0; i < lmMAX_MINOR_KEYS; i++)
        {
            m_pKeyList->Append(wxEmptyString,
                               GenerateBitmapForKeyCtrol(m_tMinorKeys[i].sKeyName,
                                                         m_tMinorKeys[i].nKeyType) );
        }
    }
    m_pKeyList->SetSelection(iK);
}

void lmScoreWizardKey::OnKeyType(wxCommandEvent& event)
{
    //load list box with the appropiate keys for selected key type

    LoadKeyList(event.GetSelection());

    //update preview
    TransferDataFromWindow();
    ((lmScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
}

void lmScoreWizardKey::OnComboKey(wxCommandEvent& event)
{
    //update preview

    TransferDataFromWindow();
    ((lmScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
}

void lmScoreWizardKey::OnEnterPage()
{
    //Mark all following pages as 'untouched' so that
    //if user finishes in this page, all following settings will not apply.
    //This is necessary in case user moves backwards and finish.

    m_ScoreData.fAddTime = false;
    m_ScoreData.fAddTitles = false;

    //bitmap preview
    TransferDataFromWindow();
    ((lmScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
}



//--------------------------------------------------------------------------------
// lmScoreWizardTime implementation
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( lmScoreWizardTime, lmWizardPage )

BEGIN_EVENT_TABLE( lmScoreWizardTime, lmWizardPage )
    EVT_RADIOBOX (lmID_RADIO_TIME, lmScoreWizardTime::OnTimeType)
END_EVENT_TABLE()


//time signatures data
static const wxString m_sTimeRadioBoxChoices[] =
        { _T("2/2"), _T("2/4"), _T("6/8"), _T("2/8"),
          _T("3/2"), _T("3/4"), _T("9/8"), _T("3/8"),
          _T("4/2"), _T("4/4"), _T("12/8"), _T("4/8"),
          _("none") //, _("other")
        };

static const int m_anBeats[] =
        { 2, 2, 6, 2,
          3, 3, 9, 3,
          4, 4, 12, 4,
          0, -1             //0=none, -1=other
        };

static const int m_anBeatType[] =
        { 2, 4, 8, 8,
          2, 4, 8, 8,
          2, 4, 8, 8,
          0, 0
        };


lmScoreWizardTime::lmScoreWizardTime()
    : lmWizardPage()
{
}

lmScoreWizardTime::lmScoreWizardTime(wxWizard* parent)
    : lmWizardPage(parent)
{
    Create(parent);
}

bool lmScoreWizardTime::Create(wxWizard* parent)
{
    // page creation
    CreateControls();
    GetSizer()->Fit(this);

    //initialize controls
    m_pTimeRadioBox->SetSelection( 9 );
    EnableOtherTimeSignatures(false);

    return true;
}

void lmScoreWizardTime::CreateControls()
{
    // Code generated with wxFormBuilder. Lines marked as ** are modified

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* pLeftColumnSizer;
	pLeftColumnSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* pTimeSizer;
	pTimeSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Select time signature") ), wxVERTICAL );

    //** definition of m_pTimeRadioBoxChoices removed. Name changed. Global definition

	int nTimeRadioBoxNChoices = sizeof( m_sTimeRadioBoxChoices ) / sizeof( wxString );
	m_pTimeRadioBox = new wxRadioBox( this, lmID_RADIO_TIME, _("Time signature"), wxDefaultPosition, wxDefaultSize, nTimeRadioBoxNChoices, m_sTimeRadioBoxChoices, 4, wxRA_SPECIFY_COLS );
	m_pTimeRadioBox->SetSelection( 9 );
	pTimeSizer->Add( m_pTimeRadioBox, 0, wxTOP|wxBOTTOM, 5 );

    //** definition of m_pOtherTimeSizer moved to header

	m_pOtherTimeSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Other time signatures") ), wxVERTICAL );

	m_pOtherTimeBoxPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* pOtherTimeBoxSizer;
	pOtherTimeBoxSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* pTopNumSizer;
	pTopNumSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pLblTopNumber = new wxStaticText( m_pOtherTimeBoxPanel, wxID_ANY, _("Numerator"), wxDefaultPosition, wxSize( 75,-1 ), 0 );
	m_pLblTopNumber->Wrap( -1 );
	pTopNumSizer->Add( m_pLblTopNumber, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTxtTopNumber = new wxTextCtrl( m_pOtherTimeBoxPanel, lmID_TIME_TOP_NUMBER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pTopNumSizer->Add( m_pTxtTopNumber, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	pOtherTimeBoxSizer->Add( pTopNumSizer, 0, 0, 5 );

	wxBoxSizer* pBottomNumSizer;
	pBottomNumSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pLblBottomNum = new wxStaticText( m_pOtherTimeBoxPanel, wxID_ANY, _("Denominator"), wxDefaultPosition, wxSize( 75,-1 ), 0 );
	m_pLblBottomNum->Wrap( -1 );
	pBottomNumSizer->Add( m_pLblBottomNum, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_pTxtBottomNumber = new wxTextCtrl( m_pOtherTimeBoxPanel, lmID_TIME_BOTTOM_NUMBER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pBottomNumSizer->Add( m_pTxtBottomNumber, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	pOtherTimeBoxSizer->Add( pBottomNumSizer, 0, 0, 5 );

	m_pOtherTimeBoxPanel->SetSizer( pOtherTimeBoxSizer );
	m_pOtherTimeBoxPanel->Layout();
	pOtherTimeBoxSizer->Fit( m_pOtherTimeBoxPanel );
	m_pOtherTimeSizer->Add( m_pOtherTimeBoxPanel, 0, wxALL, 5 );

	pTimeSizer->Add( m_pOtherTimeSizer, 0, 0, 5 );

	pLeftColumnSizer->Add( pTimeSizer, 1, wxALL|wxEXPAND, 5 );

	pMainSizer->Add( pLeftColumnSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* pRightColumnSizer;
	pRightColumnSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* pPreviewSizer;
	pPreviewSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Preview") ), wxVERTICAL );

	m_pBmpPreview = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(240,315), wxSUNKEN_BORDER );
	pPreviewSizer->Add( m_pBmpPreview, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

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

    //get selected time signature
    int nType = m_pTimeRadioBox->GetSelection();
    m_ScoreData.fAddTime = m_anBeats[nType] > 0;
    if (m_ScoreData.fAddTime)
    {
        m_ScoreData.nBeats = m_anBeats[nType];
        m_ScoreData.nBeatType = m_anBeatType[nType];
    }

    return true;
}

void lmScoreWizardTime::OnTimeType(wxCommandEvent& event)
{
    //if 'other' selected enable other time signatures. In any other case,
    //disable the static box

    EnableOtherTimeSignatures( m_anBeats[event.GetSelection()] == -1);

    //bitmap preview
    TransferDataFromWindow();
    ((lmScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
}

void lmScoreWizardTime::EnableOtherTimeSignatures(bool fEnable)
{
    //enable/disable other time signatures

    wxStaticBox* pSB = m_pOtherTimeSizer->GetStaticBox();
    pSB->Enable(fEnable);
    m_pOtherTimeBoxPanel->Enable(fEnable);
}

void lmScoreWizardTime::OnEnterPage()
{
    //Mark all following pages as 'untouched' so that
    //if user finishes in this page, all following settings will not apply.
    //This is necessary in case user moves backwards and finish.

    m_ScoreData.fAddTitles = false;

    //bitmap preview
    TransferDataFromWindow();
    ((lmScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
}




//--------------------------------------------------------------------------------
// lmScoreWizardTitles implementation
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( lmScoreWizardTitles, lmWizardPage )

BEGIN_EVENT_TABLE( lmScoreWizardTitles, lmWizardPage )
    //
END_EVENT_TABLE()

lmScoreWizardTitles::lmScoreWizardTitles()
    : lmWizardPage()
{
}

lmScoreWizardTitles::lmScoreWizardTitles(wxWizard* parent)
    : lmWizardPage(parent)
{
    Create(parent);
}

bool lmScoreWizardTitles::Create(wxWizard* parent)
{
    // page creation
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

    m_ScoreData.fAddTitles = true;

	if (m_pTxtTitle->GetValue() != _T(""))
        m_Titles[lmTITLE].sTitle = m_pTxtTitle->GetValue();

	if (m_pTxtSubtitle->GetValue() != _T(""))
        m_Titles[lmSUBTITLE].sTitle = m_pTxtSubtitle->GetValue();

	if (m_pTxtComposer->GetValue() != _T(""))
        m_Titles[lmCOMPOSER].sTitle = m_pTxtComposer->GetValue();

	if (m_pTxtArranger->GetValue() != _T(""))
        m_Titles[lmARRANGER].sTitle = m_pTxtArranger->GetValue();

	if (m_pTxtLyricist->GetValue() != _T(""))
        m_Titles[lmLYRICIST].sTitle = m_pTxtLyricist->GetValue();

    return true;
}
