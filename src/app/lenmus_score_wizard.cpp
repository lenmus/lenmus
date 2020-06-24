//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
//---------------------------------------------------------------------------------------

#include "lenmus_score_wizard.h"
#include "lenmus_standard_header.h"

#include "lenmus_images_creator.h"
#include "lenmus_string.h"
#include "lenmus_paths.h"
#include "lenmus_utilities.h"
#include "lenmus_msg_box.h"

//lomse
#include <lomse_score_utilities.h>
#include <lomse_ldp_exporter.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/ffile.h>

//other
#include <vector>
using namespace std;


namespace lenmus
{


//=======================================================================================
// static variables to save data about the score to create
//=======================================================================================

struct ScoreData
{
    //initial key signature
    bool        fAddKey;
    bool        fMajor;
    EKeySignature key;

    //initial time signature
    bool        fAddTime;
    int         nBeats;
    int         nBeatType;
};

static ScoreData m_ScoreData;


//---------------------------------------------------------------------------------------
//info about templates
class TemplateData
{
public:

    TemplateData() {}
    TemplateData(wxString name, wxString tpl) : sName(name), sTemplate(tpl) {}

    wxString    sName;          //name to display
    wxString    sTemplate;      //associated template file
};

#define lmNUM_TEMPLATES 11
static TemplateData m_Templates[lmNUM_TEMPLATES];
static wxString m_language = "??";


//template to use
static const int m_nEmptyTemplate = 0;          //index to empty template (manuscript paper)
static int m_nSelTemplate = m_nEmptyTemplate;   //"Empty (manuscript paper)"


////=======================================================================================
//// Classes related to processing templates
////TODO: Move in future to a separate file in Lomse library
////=======================================================================================
//
//typedef std::map<std::string, std::string> Replacements
//
////---------------------------------------------------------------------------------------
//// Template: helper class for processing templates
//class template
//{
//protected:
//    wxString m_src;
//
//public:
//    Template() {}
//
//    void load_template(const wxString fullpath)
//    {
//        wxFFile ifile(fullpath);
//        ifile.ReadAll(&m_src);
//    }
//
////    wxString generate_document(Replacements& data)
////    {
////        //do replacements
////        m_src.Replace("(t_Var key)", key, true /*replace all*/);
////        m_src.Replace("(t_Var time)", time, true /*replace all*/);
////    }
//
//};


//=======================================================================================
// ScoreWizard implementation
//=======================================================================================

// control identifiers
enum
{
    //ScoreWizard
    lmID_SCORE_WIZARD = 10000,

    //ScoreWizardLayout page
    lmID_LIST_ENSEMBLE,

    //ScoreWizardKeyPage page
	lmID_RADIO_KEY,
	lmID_COMBO_KEY,

    //ScoreWizardTimePage page
	lmID_RADIO_TIME,
	lmID_TIME_TOP_NUMBER,
	lmID_TIME_BOTTOM_NUMBER,
};


//---------------------------------------------------------------------------------------
//size for preview bitmap (pixels)
const int k_preview_width = 315;
const int k_preview_height = 445;

//max size for wizard. Real size depends on controls, mainly preview bitmap
const wxSize k_wizard_size(837, 600);

//---------------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE( ScoreWizard, Wizard )
    EVT_WIZARD_CANCEL( lmID_SCORE_WIZARD, ScoreWizard::OnWizardCancel )
    EVT_WIZARD_FINISHED( lmID_SCORE_WIZARD, ScoreWizard::OnWizardFinished )
    EVT_WIZARD_PAGE_CHANGED(wxID_ANY, ScoreWizard::OnPageChanged )
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
ScoreWizard::ScoreWizard(wxWindow* parent, ApplicationScope& appScope)
    : Wizard(parent, lmID_SCORE_WIZARD, _("Score configuration wizard"),
               wxDefaultPosition, k_wizard_size)
    , m_appScope(appScope)
{
    SetExtraStyle(GetExtraStyle() | wxWIZARD_EX_HELPBUTTON);

    m_score = "(score (vers 2.0) (instrument (musicData (clef G2) )))";

    //load language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (m_language != ApplicationScope::get_language())
    {
        //AWARE: When addign more templates, update lmNUM_TEMPLATES;
        //                                  Displayed name                  Template file
        m_Templates[0] =  TemplateData( _("Empty (manuscript paper)"),    "" );
		m_Templates[1] = TemplateData(_("Cello and piano"),				  "cello_piano.lms");
		m_Templates[2] =  TemplateData( _("Flute"),                       "flute.lms" );
        m_Templates[3] =  TemplateData( _("Guitar"),                      "guitar.lms" );
        m_Templates[4] =  TemplateData( _("Piano"),                       "piano.lms" );
		m_Templates[5] = TemplateData(_("Trio sonata"),				      "trio_sonata.lms");
		m_Templates[6] = TemplateData(_("Tuba"),						  "tuba.lms");
		m_Templates[7] = TemplateData(_("Viola and piano"),			      "viola_piano.lms");
		m_Templates[8] = TemplateData(_("Violin"),						  "violin.lms");
		m_Templates[9] = TemplateData( _("Violin and piano"),             "violin_piano.lms" );
        m_Templates[10] = TemplateData( _("Voice and keyboard"),          "voice_keyboard.lms" );
        //m_Templates[13] = TemplateData( _("String quartet"),              "string_quartet.lms" );
        //m_Templates[16] = TemplateData( _("Woodwind trio"),               "woodwind_trio.lms" );
        //m_Templates[17] = TemplateData( _("Woodwind quintet"),            "woodwind_quintet.lms" );
        //m_Templates[1] =  TemplateData( _("Brass quintet"),               "brass_quintet.lms" );
        //m_Templates[2] =  TemplateData( _("Brass trio"),                  "brass_trio.lms" );
        //m_Templates[3] =  TemplateData( _("Choir 4 voices (SATB)"),       "choir_SATB.lms" );
        //m_Templates[4] =  TemplateData( _("Choir SATB + piano"),          "choir_SATB_piano.lms" );
        //m_Templates[5] =  TemplateData( _("Choir 3 voices (SSA)"),        "choir_SSA.lms" );
        //m_Templates[6] =  TemplateData( _("Choir SSA + piano"),           "choir_SSA_piano.lms" );
        //AWARE: When addign more templates, update lmNUM_TEMPLATES;
        m_language = ApplicationScope::get_language();
    }


    //initialize default score configuration

    //key signature
    m_ScoreData.fAddKey = false;
    m_ScoreData.fMajor = true;
    m_ScoreData.key = k_key_C;

    //time signature
    m_ScoreData.fAddTime = false;
    m_ScoreData.nBeats = 4;         //initial time signature:  4/4
    m_ScoreData.nBeatType = 4;

    //create the pages in the order of presentation
    ScoreWizardLayout* pPageLayout = new ScoreWizardLayout((wxWizard*)this);
    AddPage(pPageLayout, true);     //true -> Optional

    ScoreWizardKeyPage* pPageKey = new ScoreWizardKeyPage((wxWizard*)this, &m_appScope);
    AddPage(pPageKey, true);     //true -> Optional

    ScoreWizardTimePage* pPageTime = new ScoreWizardTimePage((wxWizard*)this);
    AddPage(pPageTime, true);     //true -> Optional
}

//---------------------------------------------------------------------------------------
ScoreWizard::~ScoreWizard()
{
}

//---------------------------------------------------------------------------------------
void ScoreWizard::OnWizardFinished(wxWizardEvent& WXUNUSED(event))
{
    PrepareScore();
}

//---------------------------------------------------------------------------------------
void ScoreWizard::OnWizardCancel(wxWizardEvent& WXUNUSED(event))
{
    m_score.clear();
}

//---------------------------------------------------------------------------------------
void ScoreWizard::OnPageChanged( wxWizardEvent& event )
{
    //The page has changed. The event page points to the page to be displayed now.
    //Inform the page

    ((WizardPage*)event.GetPage())->OnEnterPage();
}

//---------------------------------------------------------------------------------------
void ScoreWizard::PrepareScore()
{
    //delete existing score
    m_score.clear();

    wxString sFile = m_Templates[m_nSelTemplate].sTemplate;

    //load score from LDP template
    if (m_nSelTemplate != m_nEmptyTemplate && sFile != "")
    {
        Paths* pPaths = m_appScope.get_paths();
        wxString sPath = pPaths->GetTemplatesPath();
        wxFileName oFileName(sPath, sFile, wxPATH_NATIVE);
        wxFFile ifile(oFileName.GetFullPath());
        if (!ifile.ReadAll(&m_score))
        {
            //template load failure: inform user
            wxString sMsg = wxString::Format(_("Error: Template '%s' not found."), sFile.wx_str());
            sMsg += "\n\n";
            sMsg += _("Possible causes:");
            sMsg += "\n";
            sMsg += _("- An error during lenmus installation.");
            sMsg += "\n";
            sMsg += _("- An accidental deletion of the required template.");

            ErrorBox oEB(sMsg, _("An empty score will be created."));
            oEB.ShowModal();
        }
    }

    //Create an empty score if no template or load failure
    if (m_nSelTemplate == m_nEmptyTemplate || m_score.empty())
    {
        m_score = "(score (vers 2.0)(instrument (musicData )))";
    }

    //Prepare replacements
    wxString key = "/*no key*/";
    if (m_ScoreData.fAddKey)
    {
        key = "(key ";
        wxString name = to_wx_string( LdpExporter::key_type_to_ldp(m_ScoreData.key) );
        key += name;
        key += ")";
    }

    wxString time = "/*no time*/";
    if (m_ScoreData.fAddTime)
    {
        time = wxString::Format("(time %d %d)",
                                m_ScoreData.nBeats, m_ScoreData.nBeatType);
    }

    //do replaces
    m_score.Replace("(t_Var key)", key, true /*replace all*/);
    m_score.Replace("(t_Var time)", time, true /*replace all*/);
}

//---------------------------------------------------------------------------------------
void ScoreWizard::UpdatePreview(wxStaticBitmap* pBmpPreview)
{
    PrepareScore();

    //render score in a bitmap
    LomseDoorway& lomselib = m_appScope.get_lomse();
    ImagesCreator creator(lomselib);
    wxImage image(k_preview_width, k_preview_height);

    //AWARE: scale is computed to fit paper width into image width. Computation is:
    //
    //                    image_width [px] x 25,4
    // scale = ---------------------------------------------
    //           paper_width [mm] x screen_resolution [ppi]
    //
    // In this case, paper is A4: 210x297 mm

    double ppi = lomselib.get_screen_ppi();
    double scale = (k_preview_width * 25.4) / (210.0 * ppi);
    creator.create_image(to_std_string(m_score), Document::k_format_ldp, &image, scale);

    //image.SaveFile("score-wizard-preview.jpg", wxBITMAP_TYPE_JPEG);

    wxBitmap bmp(image);
    pBmpPreview->SetBitmap(bmp);
}

//=======================================================================================
// ScoreWizardLayout implementation
//=======================================================================================

wxIMPLEMENT_DYNAMIC_CLASS( ScoreWizardLayout, WizardPage );

wxBEGIN_EVENT_TABLE( ScoreWizardLayout, WizardPage )
    EVT_LISTBOX(lmID_LIST_ENSEMBLE, ScoreWizardLayout::OnEnsembleSelected)

wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
ScoreWizardLayout::ScoreWizardLayout()
    : WizardPage()
{
}

//---------------------------------------------------------------------------------------
ScoreWizardLayout::ScoreWizardLayout(wxWizard* parent)
    : WizardPage(parent)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
bool ScoreWizardLayout::Create(wxWizard* parent)
{
    // page creation
    CreateControls();
    GetSizer()->Fit(this);

        // populate controls

    //available layouts
    for (int i=0; i < lmNUM_TEMPLATES; i++)
        m_pLstEnsemble->Append( m_Templates[i].sName );
    m_pLstEnsemble->SetSelection(m_nSelTemplate);

    //bitmap preview
    ((ScoreWizard*)parent)->UpdatePreview(m_pBmpPreview);

    return true;
}

//---------------------------------------------------------------------------------------
void ScoreWizardLayout::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* pLeftColumnSizer;
	pLeftColumnSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* pLayoutSizer;
	pLayoutSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Select instruments and style") ), wxVERTICAL );

	m_pLstEnsemble = new wxListBox( this, lmID_LIST_ENSEMBLE, wxDefaultPosition, wxSize( -1,-1 ), 0, nullptr, 0 );
	pLayoutSizer->Add( m_pLstEnsemble, 1, wxALL|wxEXPAND, 5 );

	pLeftColumnSizer->Add( pLayoutSizer, 1, wxEXPAND|wxALL, 5 );
	pMainSizer->Add( pLeftColumnSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* pRightColumnSizer;
	pRightColumnSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Preview") ), wxVERTICAL );

	m_pBmpPreview = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition,
                            wxSize(k_preview_width, k_preview_height), wxBORDER_SUNKEN );
	sbSizer4->Add( m_pBmpPreview, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	pRightColumnSizer->Add( sbSizer4, 1, wxEXPAND, 5 );

	pMainSizer->Add( pRightColumnSizer, 1, wxEXPAND, 5 );

	this->SetSizer( pMainSizer );
}

//---------------------------------------------------------------------------------------
void ScoreWizardLayout::OnEnsembleSelected(wxCommandEvent& WXUNUSED(event))
{
    m_nSelTemplate = m_pLstEnsemble->GetSelection();

    //When selecting template 'empty' do not allow next page. Only finish
    ((Wizard*)GetParentWizard())->EnableButtonNext(m_nSelTemplate != m_nEmptyTemplate);

    //update preview
    ((ScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
}

//---------------------------------------------------------------------------------------
bool ScoreWizardLayout::TransferDataFromWindow()
{
    // when moving to another page this method is automatically invoked to
    // verify that the data entered is correct before passing to the next page,
    // and to copy entered data to the main page
    // Returns true to allow moving to next page.
    // If false is returned it should display a meesage box to the user to explain
    // the reason

    m_nSelTemplate = m_pLstEnsemble->GetSelection();
    return true;
}

//---------------------------------------------------------------------------------------
void ScoreWizardLayout::OnEnterPage()
{
    //Mark all following pages as 'untouched' so that
    //if user finishes in this page, all following settings will not apply.
    //This is necessary in case user moves backwards and finish.

    m_ScoreData.fAddKey = false;
    m_ScoreData.fAddTime = false;

    //When selecting template 'empty' do not allow next page. Only finish
    ((Wizard*)GetParentWizard())->EnableButtonNext(m_nSelTemplate != m_nEmptyTemplate);
}


//=======================================================================================
// ScoreWizardKeyPage implementation
//=======================================================================================

wxIMPLEMENT_DYNAMIC_CLASS( ScoreWizardKeyPage, WizardPage );

wxBEGIN_EVENT_TABLE( ScoreWizardKeyPage, WizardPage )
    EVT_RADIOBOX (lmID_RADIO_KEY, ScoreWizardKeyPage::OnKeyType)
    EVT_COMBOBOX (lmID_COMBO_KEY, ScoreWizardKeyPage::OnComboKey)
wxEND_EVENT_TABLE()


//keys data
struct KeysData
{
    wxString            sKeyName;
    EKeySignature       nKeyType;

};

#define k_num_minor_keys    k_max_minor_key - k_min_minor_key + 1
#define k_num_major_keys    k_max_major_key - k_min_major_key + 1

static KeysData m_tMajorKeys[k_num_major_keys];
static KeysData m_tMinorKeys[k_num_minor_keys];


//---------------------------------------------------------------------------------------
ScoreWizardKeyPage::ScoreWizardKeyPage()
    : WizardPage()
    , m_pAppScope(nullptr)
{
}

//---------------------------------------------------------------------------------------
ScoreWizardKeyPage::ScoreWizardKeyPage(wxWizard* parent, ApplicationScope* pAppScope)
    : WizardPage(parent)
    , m_pAppScope(pAppScope)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
bool ScoreWizardKeyPage::Create(wxWizard* WXUNUSED(parent))
{
    //To avoid having to translate again key signature names, we are going to load them
    //by using global function get_key_signature_name()
    for (int j=0, i = k_min_major_key; i <= k_max_major_key; i++, j++)
    {
        m_tMajorKeys[j].nKeyType = (EKeySignature)i;
        m_tMajorKeys[j].sKeyName = get_key_signature_name((EKeySignature)i);
    }

    for (int j=0, i = k_min_minor_key; i <= k_max_minor_key; i++, j++)
    {
        m_tMinorKeys[j].nKeyType = (EKeySignature)i;
        if (i == 16)
            j = 1;
        m_tMinorKeys[j].sKeyName = get_key_signature_name((EKeySignature)i);    //wxString::Format("%s (%d%s)",;
    }

    // page creation
    CreateControls();
    GetSizer()->Fit(this);

    //initial selection
    int nType = (m_ScoreData.fMajor ? 0 : 1);
    m_pKeyRadioBox->SetSelection(nType);
    LoadKeyList(nType);

    return true;
}

//---------------------------------------------------------------------------------------
void ScoreWizardKeyPage::CreateControls()
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
    m_pKeyList->Create(this, lmID_COMBO_KEY, wxEmptyString, wxDefaultPosition, wxSize(135, 68),
                       0, nullptr, wxCB_READONLY);
	pKeySizer->Add( m_pKeyList, 0, wxALL, 5 );

	pLeftColumnSizer->Add( pKeySizer, 1, wxEXPAND|wxALL, 5 );

	pMainSizer->Add( pLeftColumnSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* pRightColumnSizer;
	pRightColumnSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* pPreviewSizer;
	pPreviewSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Preview") ), wxVERTICAL );

	m_pBmpPreview = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition,
                        wxSize(k_preview_width, k_preview_height), wxSUNKEN_BORDER );
	pPreviewSizer->Add( m_pBmpPreview, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	pRightColumnSizer->Add( pPreviewSizer, 1, wxEXPAND, 5 );

	pMainSizer->Add( pRightColumnSizer, 1, wxEXPAND, 5 );

	this->SetSizer( pMainSizer );
}

//---------------------------------------------------------------------------------------
bool ScoreWizardKeyPage::TransferDataFromWindow()
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
        m_ScoreData.key = m_tMajorKeys[iK].nKeyType;
    else
        m_ScoreData.key = m_tMinorKeys[iK].nKeyType;

    return true;
}

//---------------------------------------------------------------------------------------
void ScoreWizardKeyPage::LoadKeyList(int nType)
{
    //nType: 0=major, 1=minor

    if (nType==0)
    {
        m_pKeyList->Clear();
        for (int i=0; i < k_num_major_keys; i++)
        {
            m_pKeyList->Append(wxEmptyString,
                               generate_bitmap_for_key_ctrol(*m_pAppScope,
                                                             m_tMajorKeys[i].sKeyName,
                                                             m_tMajorKeys[i].nKeyType) );
        }
    }
    else
    {
        m_pKeyList->Clear();
        for (int i=0; i < k_num_minor_keys; i++)
        {
            m_pKeyList->Append(wxEmptyString,
                               generate_bitmap_for_key_ctrol(*m_pAppScope,
                                                             m_tMinorKeys[i].sKeyName,
                                                             m_tMinorKeys[i].nKeyType) );
        }
    }
    m_pKeyList->SetSelection(0);
}

//---------------------------------------------------------------------------------------
void ScoreWizardKeyPage::OnKeyType(wxCommandEvent& event)
{
    //load list box with the appropiate keys for selected key type

    LoadKeyList(event.GetSelection());

    //update preview
    TransferDataFromWindow();
    ((ScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
}

//---------------------------------------------------------------------------------------
void ScoreWizardKeyPage::OnComboKey(wxCommandEvent& WXUNUSED(event))
{
    //update preview

    TransferDataFromWindow();
    ((ScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
}

//---------------------------------------------------------------------------------------
void ScoreWizardKeyPage::OnEnterPage()
{
    //Mark all following pages as 'untouched' so that
    //if user finishes in this page, all following settings will not apply.
    //This is necessary in case user moves backwards and finish.

    m_ScoreData.fAddTime = false;

    //bitmap preview
    TransferDataFromWindow();
    ((ScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
}



//=======================================================================================
// ScoreWizardTimePage implementation
//=======================================================================================

wxIMPLEMENT_DYNAMIC_CLASS( ScoreWizardTimePage, WizardPage );

wxBEGIN_EVENT_TABLE( ScoreWizardTimePage, WizardPage )
    EVT_RADIOBOX (lmID_RADIO_TIME, ScoreWizardTimePage::OnTimeType)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
//time signatures data
static const wxString m_sTimeRadioBoxChoices[] =
        { "2/2", "2/4", "6/8", "2/8",
          "3/2", "3/4", "9/8", "3/8",
          "4/2", "4/4", "12/8", "4/8",
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


//---------------------------------------------------------------------------------------
ScoreWizardTimePage::ScoreWizardTimePage()
    : WizardPage()
{
}

//---------------------------------------------------------------------------------------
ScoreWizardTimePage::ScoreWizardTimePage(wxWizard* parent)
    : WizardPage(parent)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
bool ScoreWizardTimePage::Create(wxWizard* WXUNUSED(parent))
{
    // page creation
    CreateControls();
    GetSizer()->Fit(this);

    //initialize controls
    m_pTimeRadioBox->SetSelection( 9 );
    EnableOtherTimeSignatures(false);

    return true;
}

//---------------------------------------------------------------------------------------
void ScoreWizardTimePage::CreateControls()
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

	m_pBmpPreview = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition,
                        wxSize(k_preview_width, k_preview_height), wxSUNKEN_BORDER );
	pPreviewSizer->Add( m_pBmpPreview, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	pRightColumnSizer->Add( pPreviewSizer, 1, wxEXPAND|wxALL, 5 );

	pMainSizer->Add( pRightColumnSizer, 1, wxEXPAND, 5 );

	this->SetSizer( pMainSizer );
}

//---------------------------------------------------------------------------------------
bool ScoreWizardTimePage::TransferDataFromWindow()
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

//---------------------------------------------------------------------------------------
void ScoreWizardTimePage::OnTimeType(wxCommandEvent& event)
{
    //if 'other' selected enable other time signatures. In any other case,
    //disable the static box

    EnableOtherTimeSignatures( m_anBeats[event.GetSelection()] == -1);

    //bitmap preview
    TransferDataFromWindow();
    ((ScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
}

//---------------------------------------------------------------------------------------
void ScoreWizardTimePage::EnableOtherTimeSignatures(bool fEnable)
{
    //enable/disable other time signatures

    wxStaticBox* pSB = m_pOtherTimeSizer->GetStaticBox();
    pSB->Enable(fEnable);
    m_pOtherTimeBoxPanel->Enable(fEnable);
}

//---------------------------------------------------------------------------------------
void ScoreWizardTimePage::OnEnterPage()
{
    //Mark all following pages as 'untouched' so that
    //if user finishes in this page, all following settings will not apply.
    //This is necessary in case user moves backwards and finish.

    //bitmap preview
    TransferDataFromWindow();
    ((ScoreWizard*)GetParentWizard())->UpdatePreview(m_pBmpPreview);
}


}   // namespace lenmus
