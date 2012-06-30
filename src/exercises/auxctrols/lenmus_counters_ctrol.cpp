//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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

//lenmus headers
#include "lenmus_counters_ctrol.h"
#include "lenmus_standard_header.h"

#include "lenmus_generators.h"
#include "lenmus_constrains.h"
#include "lenmus_exercise_ctrol.h"
#include "lenmus_art_provider.h"
#include "lenmus_injectors.h"
#include "lenmus_paths.h"

//wxWidgets
#include <wx/artprov.h>

////access to global flag
//extern bool g_fAnswerSoundsEnabled;

//lomse
#include <lomse_shapes.h>
#include <lomse_image_reader.h>
using namespace lomse;


namespace lenmus
{

//IDs for controls
enum {
    lmID_LINK_RESET_COUNTERS = 3600,
	lmID_LINK_EXPLAIN,
    lmID_CBO_MODE,
};

//=======================================================================================
// CountersCtrol implementation
// An abstract control to embed in html exercises to display statistics about
// user performance in doing the exercise
//=======================================================================================
CountersCtrol::CountersCtrol(ApplicationScope& appScope,
                             Document* pDoc, ExerciseCtrol* pOwner,
                             ExerciseOptions* pConstrains)
    : Control(pOwner, pDoc)
    , m_appScope(appScope)
    , m_pOwner(pOwner)
    , m_pConstrains(pConstrains)
    //, m_modes(0.0, 0.0, 110.0, 210.0, false),
{
    pOwner->accept_control_ownership(this);
}

//---------------------------------------------------------------------------------------
CountersCtrol::~CountersCtrol()
{
}

//---------------------------------------------------------------------------------------
void CountersCtrol::RightWrongSound(bool fSuccess)
{
    //TODO 5.0
    //if (g_fAnswerSoundsEnabled)
    //{
    //    lmWaveManager* pWave = lmWaveManager::GetInstance();
    //    if (fSuccess)
    //        pWave->RightAnswerSound();
    //    else
    //        pWave->WrongAnswerSound();
    //}
}

//---------------------------------------------------------------------------------------
void CountersCtrol::add_mode_controls(ImoContent* pWrapper)
{
	//ImoParagraph* pModePara = LENMUS_NEW ImoParagraph(wxHORIZONTAL);
 //   pMainPara->Add(pModePara);

	//ImoTextItem* pLblMode = LENMUS_NEW ImoTextItem(
 //       this, wxID_ANY, _("Mode:"), wxDefaultPosition, wxDefaultSize, 0);
	//pLblMode->Wrap( -1 );
	//pModePara->Add( pLblMode, 0, wxALL|wxALIGN_CENTER_VERTICAL, 10);

 //   //load strings for Mode combo
 //   int nNumValidModes = 0;
	//wxString sCboModeChoices[k_num_generation_modes];
 //   for (long i=0; i < k_num_generation_modes; i++)
 //   {
 //       //TODO 5.0
 //       if (m_pConstrains->IsGenerationModeSupported(i))
 //           sCboModeChoices[nNumValidModes++] = get_generation_mode_name(i);
 //   }
	//m_pCboMode = LENMUS_NEW wxChoice(this, lmID_CBO_MODE, wxDefaultPosition,
 //                             wxDefaultSize, nNumValidModes, sCboModeChoices, 0);
	//pModePara->Add( m_pCboMode, 1, wxALL|wxALIGN_CENTER_VERTICAL, 10);
}

//---------------------------------------------------------------------------------------
void CountersCtrol::on_mode_changed(wxCommandEvent& WXUNUSED(event))
{
 //   //locate LENMUS_NEW mode
 //   wxString sMode = m_pCboMode->GetStringSelection();
	//int nMode;
 //   for (nMode=0; nMode < k_num_generation_modes; nMode++)
 //   {
 //       if (sMode == get_generation_mode_name(nMode))
 //           break;
 //   }
 //   wxASSERT(nMode < k_num_generation_modes);

 //   change_generation_mode(nMode);
}

//---------------------------------------------------------------------------------------
void CountersCtrol::change_generation_mode(int mode)
{
    //if (m_pOwner)
    //    m_pOwner->change_mode(mode);
}

//---------------------------------------------------------------------------------------
void CountersCtrol::change_generation_mode_label(int mode)
{
    //if (m_pCboMode)
    //    m_pCboMode->SetStringSelection( get_generation_mode_name(mode) );
}



//=======================================================================================
// QuizCounters implementation
// A control to embed in html exercises to display the number of right and
// wrong student answers, in current session, and the total score (percentage)
//=======================================================================================
QuizCounters::QuizCounters(ApplicationScope& appScope,
                           Document* pDoc, ExerciseCtrol* pOwner,
                           ExerciseOptions* pConstrains, QuizManager* pProblemMngr,
                           int nNumTeams)
    : CountersCtrol(appScope, pDoc, pOwner, pConstrains)
    , m_size(0.0f, 0.0f)
    , m_pProblemMngr(pProblemMngr)
    , m_fTwoTeamsMode(nNumTeams == 2)
{
    wxASSERT(nNumTeams == 1 || nNumTeams == 2);

    m_pRightCounter[0] = NULL;

    ArtProvider art(m_appScope);
    m_pProblemMngr->ResetCounters();
    m_pProblemMngr->SetNumTeams(nNumTeams);
}

//---------------------------------------------------------------------------------------
QuizCounters::~QuizCounters()
{
}

//---------------------------------------------------------------------------------------
USize QuizCounters::measure()
{
    return USize(3250.0f, 2900.0f);
}

//---------------------------------------------------------------------------------------
GmoBoxControl* QuizCounters::layout(LibraryScope& libraryScope, UPoint pos)
{
    UPoint cursor(pos);

    ImoStyle* mainBoxStyle = m_pDoc->create_private_style();
    mainBoxStyle->border_width(0.0f)->padding(0.0f)->margin(0.0f);

    m_pMainBox = LENMUS_NEW GmoBoxControl(this, pos, 3250.0f, 2900.0f, mainBoxStyle);

    //load bitmaps ---------------------------
    ArtProvider art(m_appScope);
    m_imgRight = art.get_image(_T("right_answers"), wxART_TOOLBAR, wxSize(36,36));
    m_imgWrong = art.get_image(_T("wrong_answers"), wxART_TOOLBAR, wxSize(36,36));
    m_imgTotal = art.get_image(_T("total_marks"), wxART_TOOLBAR, wxSize(36,36));
    if (m_fTwoTeamsMode)
    {
        m_imgRed = art.get_image(_T("team_red"), wxART_TOOLBAR, wxSize(24,24));
        m_pImgTeam[0] = &m_imgRed;
        m_imgBlue = art.get_image(_T("team_blue"), wxART_TOOLBAR, wxSize(24,24));
        m_pImgTeam[1] = &m_imgBlue;
        m_imgGrey = art.get_image(_T("team_grey"), wxART_TOOLBAR, wxSize(24,24));
    }

 //   add_mode_controls(pMainWrapper);

    //row with icons ---------------------------

    cursor.x += 500.0f;
    cursor.y += 15.0f;  //counters ctrol border
    USize imgSize1(24.0f * 2540.0f / 96.0f, 24.0f * 2540.0f / 96.0f);
    //SpImage image1( LENMUS_NEW Image(m_imgRight.GetData(), VSize(24,24), k_pix_format_rgb24, imgSize1));

    Paths* pPaths = m_appScope.get_paths();
    string sFolder = to_std_string( pPaths->GetImagePath() );
    string sPath = sFolder + "right_answers_24.png";
    ImageReader reader;
    SpImage image1 = reader.load_image(sPath);

    GmoShapeImage* pImage1 = LENMUS_NEW GmoShapeImage(NULL, image1, cursor, imgSize1);
    m_pMainBox->add_shape(pImage1, GmoShape::k_layer_top);
 ////       m_imgRight->SetToolTip(_("Right answers counter"));

    //second icon
    cursor.x += imgSize1.width + 250.0f;
//    SpImage image2( LENMUS_NEW Image(m_imgWrong.GetData(), VSize(24,24), k_pix_format_rgb24, imgSize1));

    sPath = sFolder + "wrong_answers_24.png";
    SpImage image2 = reader.load_image(sPath);

    GmoShapeImage* pImage2 = LENMUS_NEW GmoShapeImage(NULL, image2, cursor, imgSize1);
    m_pMainBox->add_shape(pImage2, GmoShape::k_layer_top);

    //third icon
    cursor.x += imgSize1.width + 350.0f;
//    SpImage image3( LENMUS_NEW Image(m_imgTotal.GetData(), VSize(24,24), k_pix_format_rgb24, imgSize1));

    sPath = sFolder + "total_marks_24.png";
    SpImage image3 = reader.load_image(sPath);

    GmoShapeImage* pImage3 = LENMUS_NEW GmoShapeImage(NULL, image3, cursor, imgSize1);
    m_pMainBox->add_shape(pImage3, GmoShape::k_layer_top);
    //m_imgTotal->SetToolTip(_("Total: your marks"));

    // Create counters -------------------------------------
    cursor.x = pos.x + 500.0f;
    cursor.y += imgSize1.height + 200.0f;
    CreateCountersGroup(0, m_pMainBox, cursor);
    cursor.x = pos.x + 500.0f;
    cursor.y += 1050.0f;
    if (m_fTwoTeamsMode)
    {
        CreateCountersGroup(1, m_pMainBox, cursor);
        cursor.x = pos.x + 500.0f;
        cursor.y += 1050.0f;
    }


    //'reset counters' link -----------------------------
    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();
    HyperlinkCtrl* pResetLink =
        LENMUS_NEW HyperlinkCtrl(*pLibScope, get_owner(), m_pDoc,
                                 to_std_string(_("Reset counters")) );
    m_pMainBox->add_child_box( pResetLink->layout(*pLibScope, cursor) );
    pResetLink->add_event_handler(k_on_click_event, this, on_reset_counters);

    UpdateDisplay();

    return m_pMainBox;
}

//---------------------------------------------------------------------------------------
void QuizCounters::CreateCountersGroup(int nTeam, GmoBox* m_pMainBox, UPoint pos)
{
    // Create the controls for counter group nTeam (0...n)

//    ImoStyle* pRowStyle = m_pDoc->create_private_style();
//    pRowStyle->set_float_property(ImoStyle::k_font_size, 10.0f);
//    pRowStyle->set_lunits_property(ImoStyle::k_margin_bottom, 0.0f);
//    pRowStyle->set_lunits_property(ImoStyle::k_margin_left, 0.0f);
//
//    LUnits counterWidth = 1000.0f;
//    LUnits sideBorderWidth = 500.0f;
//    LUnits innerBorderWidth = 200.0f;
//    ImoInlineWrapper* pBox;
//
//    //row with counters
//    ImoParagraph* pCountersRow = pMainWrapper->add_paragraph(pRowStyle);
//
//    ImoStyle* pBoxStyle = m_pDoc->create_private_style();
//    pBoxStyle->set_lunits_property(ImoStyle::k_font_size, 12.0f);
//    pBoxStyle->set_lunits_property(ImoStyle::k_margin_bottom, 0.0f);
//    pBoxStyle->set_lunits_property(ImoStyle::k_margin_left, 0.0f);
//    pBoxStyle->set_border_width_property(15.0f);
//    pBoxStyle->set_lunits_property(ImoStyle::k_padding_top, 500.0f);
//    pBoxStyle->set_lunits_property(ImoStyle::k_padding_bottom, 500.0f);
//    pBoxStyle->set_lunits_property(ImoStyle::k_padding_left, 500.0f);
//    pBoxStyle->set_lunits_property(ImoStyle::k_padding_right, 500.0f);

    ImoStyle* pCounterStyle = m_pDoc->create_private_style();
    pCounterStyle->font_size(14.0f)->margin(0.0f);

    // Team label
    if (m_fTwoTeamsMode)
    {
//        USize imgSize(24.0f * 2540.0f / 96.0f, 24.0f * 2540.0f / 96.0f);
//        pBox = pCountersRow->add_inline_box(imgSize.width, pRowStyle);
//        //add team icon
//        if (m_pImgTeam[nTeam]->HasAlpha())
//            pBox->add_image(m_pImgTeam[nTeam]->GetData(), VSize(24,24), k_pix_format_rgba32, imgSize);
//        else
//            pBox->add_image(m_pImgTeam[nTeam]->GetData(), VSize(24,24), k_pix_format_rgb24, imgSize);
// //       m_pImgTeam[nTeam]->SetToolTip(
// //           (nTeam == 0 ? _("Counters for Team A") : _("Counters for Team B")) );
    }


    LibraryScope* pLibScope = m_appScope.get_lomse().get_library_scope();
    ImoStyle* style = m_pDoc->create_private_style();
    style->border_width(15.0f)->padding(0.0f)->margin(0.0f)->font_size(14.0f);
    style->text_align(ImoTextStyle::k_align_center);

    //display for right answers
    m_pRightCounter[nTeam] =
        LENMUS_NEW StaticTextCtrl(*pLibScope, get_owner(), m_pDoc, "5",
                                  660.0f, 760.0f, style);
    m_pRightCounter[nTeam]->set_tooltip( to_std_string(_("Right answers counter")) );
    m_pMainBox->add_child_box( m_pRightCounter[nTeam]->layout(*pLibScope, pos) );


    //display for wrong answers
    pos.x = m_pRightCounter[nTeam]->right() + 200.0f;
    m_pWrongCounter[nTeam] =
        LENMUS_NEW StaticTextCtrl(*pLibScope, get_owner(), m_pDoc, "4",
                                  660.0f, 760.0f, style);
    m_pWrongCounter[nTeam]->set_tooltip( to_std_string(_("Wrong answers counter")) );
    m_pMainBox->add_child_box( m_pWrongCounter[nTeam]->layout(*pLibScope, pos) );


    //display for total score
    pos.x = m_pWrongCounter[nTeam]->right() + 200.0f;
    m_pTotalCounter[nTeam] =
        LENMUS_NEW StaticTextCtrl(*pLibScope, get_owner(), m_pDoc, "0.0",
                                  1000.0f, 760.0f, style);
    m_pTotalCounter[nTeam]->set_tooltip( to_std_string(_("Total: your marks")) );
    m_pMainBox->add_child_box( m_pTotalCounter[nTeam]->layout(*pLibScope, pos) );
}

//---------------------------------------------------------------------------------------
void QuizCounters::on_draw(Drawer* pDrawer, RenderOptions& opt)
{
}

//---------------------------------------------------------------------------------------
void QuizCounters::handle_event(SpEventInfo pEvent)
{
//    if (pEvent->is_mouse_in_event())
//        wxLogMessage(_T("Mouse In at QuizCounters"));
//    else if (pEvent->is_mouse_out_event())
//        wxLogMessage(_T("Mouse Out at QuizCounters"));
//    else if (pEvent->is_on_click_event())
//    {
//        wxLogMessage(_T("On Click at QuizCounters"));
//    }
}

//---------------------------------------------------------------------------------------
void QuizCounters::UpdateDisplay()
{
    if (m_pRightCounter[0] == NULL) return;
    UpdateDisplays( m_pProblemMngr->GetCurrentTeam() );
}

//---------------------------------------------------------------------------------------
void QuizCounters::on_reset_counters(void* pThis, SpEventInfo pEvent)
{
    static_cast<QuizCounters*>(pThis)->reset_counters();
}

//---------------------------------------------------------------------------------------
void QuizCounters::reset_counters()
{
    m_pProblemMngr->ResetCounters();
    for (int i=0; i < m_pProblemMngr->GetNumTeams(); i++)
        UpdateDisplays(i);
}

//---------------------------------------------------------------------------------------
void QuizCounters::UpdateDisplays(int nTeam)
{
    int nRight = m_pProblemMngr->GetRight(nTeam);
    int nWrong = m_pProblemMngr->GetWrong(nTeam);

    //update display for right answers
    m_pRightCounter[nTeam]->set_text( to_std_string(wxString::Format(_T("%d"), nRight)) );

    //update display for wrong answers
    m_pWrongCounter[nTeam]->set_text( to_std_string(wxString::Format(_T("%d"), nWrong)) );

    //update display for total score
    int nTotal = nRight + nWrong;
    if (nTotal == 0)
        m_pTotalCounter[nTeam]->set_text("-");
    else
    {
        float rScore = 10.0f * float(nRight) / float(nTotal);
        m_pTotalCounter[nTeam]->set_text( to_std_string(wxString::Format(_T("%.01f"), rScore)) );
    }
}

//---------------------------------------------------------------------------------------
void QuizCounters::NextTeam()
{
    //m_pProblemMngr->NextTeam();

    ////update label
    //if (m_fTwoTeamsMode)
    //{
    //    if (m_pProblemMngr->GetCurrentTeam() == 0)
    //    {
    //        m_pImgTeam[0] = &m_imgRed;
    //        m_pImgTeam[1] = &m_imgGrey;
    //    }
    //    else
    //    {
    //        m_pImgTeam[0] = &m_imgGrey;
    //        m_pImgTeam[1] = &m_imgBlue;
    //    }
    //    //m_pDoc->set_modified(true);
    //    //m_pDoc->notify_if_document_modified();
    //}
}

//---------------------------------------------------------------------------------------
void QuizCounters::OnNewQuestion()
{
    NextTeam();
}


////=======================================================================================
//// LeitnerCounters implementation
//// A control to embed in html exercises to display statistics
//// on user performance in learning the subject. It uses the Leitner system of
//// spaced repetitions
////=======================================================================================
//
//BEGIN_EVENT_TABLE(LeitnerCounters, CountersCtrol)
//    //LM_EVT_URL_CLICK(lmID_LINK_EXPLAIN, LeitnerCounters::OnExplainProgress)
//    EVT_BUTTON(lmID_LINK_EXPLAIN, LeitnerCounters::OnExplainProgress)
//END_EVENT_TABLE()
//
//
////---------------------------------------------------------------------------------------
//LeitnerCounters::LeitnerCounters(wxWindow* parent, wxWindowID id,
//                                 ExerciseCtrol* pOwner, ExerciseOptions* pConstrains,
//                                 LeitnerManager* pProblemMngr, const wxPoint& pos)
//    : CountersCtrol(parent, id, pOwner, pConstrains, pos)
//{
//    //initializations
//    m_pProblemMngr = pProblemMngr;
//
//    // Create the controls
//    CreateControls();
//}
//
////---------------------------------------------------------------------------------------
//LeitnerCounters::~LeitnerCounters()
//{
//}
//
////---------------------------------------------------------------------------------------
//void LeitnerCounters::CreateControls()
//{
//    //AWARE: Code created with wxFormBuilder and copied here.
//    //Modifications near lin 357: replacement of m_pLinkExplain (wxStaticTxt) by
//    //an lmUrlAuxCtrol
//
//	ImoParagraph* m_pDataPara;
//	m_pDataPara = LENMUS_NEW ImoParagraph( wxVERTICAL );
//
//	ImoParagraph* m_pFirstLinePara;
//	m_pFirstLinePara = LENMUS_NEW ImoParagraph( wxHORIZONTAL );
//
//	m_pTxtNumQuestions = LENMUS_NEW ImoTextItem( this, wxID_ANY, _T("36 / 0"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
//	m_pTxtNumQuestions->Wrap( -1 );
//	m_pTxtNumQuestions->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
//
//	m_pFirstLinePara->Add( m_pTxtNumQuestions, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT|wxLEFT, 5 );
//
//
//	m_pFirstLinePara->Add( 0, 0, 1, wxEXPAND, 5 );
//
//	m_pLblEST = LENMUS_NEW ImoTextItem( this, wxID_ANY, _T("EST:"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pLblEST->Wrap( -1 );
//	m_pLblEST->SetFont( wxFont( 8, 74, 90, 92, false, wxT("Tahoma") ) );
//
//	m_pFirstLinePara->Add( m_pLblEST, 0, wxRIGHT|wxLEFT, 5 );
//
//	m_pTxtTime = LENMUS_NEW ImoTextItem( this, wxID_ANY, _T("1h:27m"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
//	m_pTxtTime->Wrap( -1 );
//	m_pTxtTime->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
//
//	m_pFirstLinePara->Add( m_pTxtTime, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
//
//
//	m_pFirstLinePara->Add( 0, 0, 1, wxEXPAND, 5 );
//
//    //FOLLOWING LINES REPLACED------------------------------
//	//m_pLinkExplain = LENMUS_NEW ImoTextItem( this, lmID_LINK_EXPLAIN, _("?"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
//	//m_pLinkExplain->Wrap( -1 );
//	//m_pLinkExplain->SetFont( wxFont( 8, 74, 90, 90, true, wxT("Tahoma") ) );
//	//m_pLinkExplain->SetForegroundColour( wxColour( 0, 0, 255 ) );
//	//
//	//m_pFirstLinePara->Add( m_pLinkExplain, 0, wxRIGHT|wxLEFT, 5 );
//
//    ////TODO 5.0
//    //m_pFirstLinePara->Add(
//    //    LENMUS_NEW lmUrlAuxCtrol(this, lmID_LINK_EXPLAIN, m_rScale, _T("?"), lmNO_BITMAP),
//    //    0, wxRIGHT|wxLEFT, 5);
//    //END OF MODIFICATION----------------------------------
//
//	m_pDataPara->Add( m_pFirstLinePara, 0, wxEXPAND, 5 );
//
//	wxFlexGridPara* m_pGridPara;
//	m_pGridPara = LENMUS_NEW wxFlexGridPara( 2, 3, 0, 0 );
//	m_pGridPara->SetFlexibleDirection( wxHORIZONTAL );
//	m_pGridPara->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
//
//	m_pLblSession = LENMUS_NEW ImoTextItem( this, wxID_ANY, _("Session:"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pLblSession->Wrap( -1 );
//	m_pLblSession->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
//
//	m_pGridPara->Add( m_pLblSession, 0, wxLEFT, 5 );
//
//	m_pTxtSession = LENMUS_NEW ImoTextItem( this, wxID_ANY, _T("0.0%"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pTxtSession->Wrap( -1 );
//	m_pTxtSession->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
//
//	m_pGridPara->Add( m_pTxtSession, 0, wxRIGHT|wxLEFT|wxALIGN_RIGHT, 5 );
//
//	m_pGaugeSession = LENMUS_NEW wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( 85,15 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
//	m_pGaugeSession->SetValue( 100 );
//	m_pGridPara->Add( m_pGaugeSession, 0, 0, 5 );
//
//	m_pLblGlobal = LENMUS_NEW ImoTextItem( this, wxID_ANY, _("Global:"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pLblGlobal->Wrap( -1 );
//	m_pLblGlobal->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
//
//	m_pGridPara->Add( m_pLblGlobal, 0, wxLEFT, 5 );
//
//	m_pTxtGlobal = LENMUS_NEW ImoTextItem( this, wxID_ANY, _T("0.0%"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pTxtGlobal->Wrap( -1 );
//	m_pTxtGlobal->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Arial") ) );
//
//	m_pGridPara->Add( m_pTxtGlobal, 0, wxRIGHT|wxLEFT|wxALIGN_RIGHT, 5 );
//
//	m_pGaugeGlobal = LENMUS_NEW wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( 85,15 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
//	m_pGaugeGlobal->SetValue( 70 );
//	m_pGridPara->Add( m_pGaugeGlobal, 0, 0, 5 );
//
//	m_pDataPara->Add( m_pGridPara, 1, wxEXPAND|wxTOP, 5 );
//    add_mode_controls(m_pDataPara);
//
//	this->SetPara( m_pDataPara );
//	this->Layout();
//}
//
////---------------------------------------------------------------------------------------
//void LeitnerCounters::UpdateDisplay()
//{
//    int nNew = m_pProblemMngr->GetNew();
//    int nExpired = m_pProblemMngr->GetExpired();
//    int nTotal = m_pProblemMngr->GetTotal();
//    float rSessionProgress = m_pProblemMngr->GetSessionProgress();
//    float rGlobalProgress = m_pProblemMngr->GetGlobalProgress();
//    wxTimeSpan tsEST = m_pProblemMngr->GetEstimatedSessionTime();
//
//    //update display
//    m_pTxtNumQuestions->SetLabel( wxString::Format(_T("%d / %d"), nNew, nExpired) );
//    m_pTxtSession->SetLabel( wxString::Format(_T("%.01f%"), rSessionProgress) );
//	m_pGaugeSession->SetValue( int(rSessionProgress + 0.5f) );
//    m_pTxtGlobal->SetLabel( wxString::Format(_T("%.01f%"), rGlobalProgress) );
//	m_pGaugeGlobal->SetValue( int(rGlobalProgress + 0.5f) );
//    m_pTxtTime->SetLabel( tsEST.Format(_T("%Hh:%Mm:%Ss")) );
//
//    Layout();
//}
//
////---------------------------------------------------------------------------------------
//void LeitnerCounters::OnExplainProgress(wxCommandEvent& WXUNUSED(event))
//{
//    ////TODO 5.0
//    //lmHtmlDlg dlg(this, _("Progress report"));
//    //dlg.SetContent( m_pProblemMngr->GetProgressReport() );
//    //dlg.ShowModal();
//}
//
//
////=======================================================================================
//// PractiseCounters implementation
//// A control to embed in html exercises to display statistics
//// on user performance in learning the subject. It uses the Leitner system of
//// spaced repetitions
////=======================================================================================
//
//BEGIN_EVENT_TABLE(PractiseCounters, CountersCtrol)
//    //LM_EVT_URL_CLICK(lmID_LINK_RESET_COUNTERS, PractiseCounters::OnResetCounters)
//    EVT_BUTTON(lmID_LINK_RESET_COUNTERS, PractiseCounters::OnResetCounters)
//END_EVENT_TABLE()
//
//
////---------------------------------------------------------------------------------------
//PractiseCounters::PractiseCounters(wxWindow* parent, wxWindowID id,
//                                   ExerciseCtrol* pOwner, ExerciseOptions* pConstrains,
//                                   LeitnerManager* pProblemMngr, const wxPoint& pos)
//    : CountersCtrol(parent, id, pOwner, pConstrains, pos)
//{
//    //initializations
//    m_pProblemMngr = pProblemMngr;
//
//    // Create the controls
//    CreateControls();
//
//    //load icons
//    wxBitmap bmp = wxArtProvider::GetBitmap(_T("button_accept"), wxART_TOOLBAR, wxSize(24,24));
//    m_imgRight->SetBitmap(bmp);
//    bmp = wxArtProvider::GetBitmap(_T("button_cancel"), wxART_TOOLBAR, wxSize(24,24));
//    m_imgWrong->SetBitmap(bmp);
//    bmp = wxArtProvider::GetBitmap(_T("diploma_cap"), wxART_TOOLBAR, wxSize(35,24));
//    m_imgTotal->SetBitmap(bmp);
//}
//
////---------------------------------------------------------------------------------------
//PractiseCounters::~PractiseCounters()
//{
//}
//
////---------------------------------------------------------------------------------------
//void PractiseCounters::CreateControls()
//{
//    //AWARE: Code created with wxFormBuilder and copied here.
//    //Modifications:
//    // - near line 483: do not load bitmap at creation time
//    // - near line 499: do not load bitmap at creation time
//    // - near line 515: do not load bitmap at creation time
//    // - near line 530: replacement of m_pLinkResetCounters (wxStaticTxt) by an lmUrlAuxCtrol
//
//	ImoParagraph* pCtrolPara;
//	pCtrolPara = LENMUS_NEW ImoParagraph( wxVERTICAL );
//
//	ImoParagraph* pCountersPara;
//	pCountersPara = LENMUS_NEW ImoParagraph( wxHORIZONTAL );
//
//	ImoParagraph* pWrongPara;
//	pWrongPara = LENMUS_NEW ImoParagraph( wxVERTICAL );
//
//    //m_imgWrong = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("../res/icons/button_cancel_24.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
//    m_imgWrong = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
//    pWrongPara->Add( m_imgWrong, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
//
//	m_pTxtWrong = LENMUS_NEW ImoTextItem( this, wxID_ANY, _T("0"), wxDefaultPosition, wxSize( 50,-1 ), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxBORDER_SIMPLE );
//	m_pTxtWrong->Wrap( -1 );
//	m_pTxtWrong->SetFont( wxFont( 12, 74, 90, 92, false, wxT("Arial") ) );
//	m_pTxtWrong->SetBackgroundColour( wxColour( 255, 255, 255 ) );
//
//	pWrongPara->Add( m_pTxtWrong, 0, wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE|wxBOTTOM, 0 );
//
//	pCountersPara->Add( pWrongPara, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT, 5 );
//
//	ImoParagraph* pRightPara;
//	pRightPara = LENMUS_NEW ImoParagraph( wxVERTICAL );
//
//	//m_imgRight = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("../res/icons/button_accept_24.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
//	m_imgRight = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
//	pRightPara->Add( m_imgRight, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
//
//	m_pTxtRight = LENMUS_NEW ImoTextItem( this, wxID_ANY, _T("0"), wxDefaultPosition, wxSize( 50,-1 ), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxBORDER_SIMPLE );
//	m_pTxtRight->Wrap( -1 );
//	m_pTxtRight->SetFont( wxFont( 12, 74, 90, 92, false, wxT("Arial") ) );
//	m_pTxtRight->SetBackgroundColour( wxColour( 255, 255, 255 ) );
//
//	pRightPara->Add( m_pTxtRight, 0, wxTOP|wxBOTTOM|wxADJUST_MINSIZE|wxALIGN_CENTER_HORIZONTAL, 0 );
//
//	pCountersPara->Add( pRightPara, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT, 5 );
//
//	ImoParagraph* pTotalPara;
//	pTotalPara = LENMUS_NEW ImoParagraph( wxVERTICAL );
//
//	//m_imgTotal = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("../res/icons/diploma_cap_24.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
//	m_imgTotal = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
//	pTotalPara->Add( m_imgTotal, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
//
//	m_pTxtTotal = LENMUS_NEW ImoTextItem( this, wxID_ANY, _T("-"), wxDefaultPosition, wxSize( 50,-1 ), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxBORDER_SIMPLE );
//	m_pTxtTotal->Wrap( -1 );
//	m_pTxtTotal->SetFont( wxFont( 12, 74, 90, 92, false, wxT("Arial") ) );
//	m_pTxtTotal->SetBackgroundColour( wxColour( 255, 255, 255 ) );
//
//	pTotalPara->Add( m_pTxtTotal, 0, wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE|wxTOP|wxBOTTOM, 0 );
//
//	pCountersPara->Add( pTotalPara, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );
//
//	pCtrolPara->Add( pCountersPara, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxRIGHT|wxLEFT, 5 );
//    add_mode_controls(pCtrolPara);
//
//    //FOLLOWING LINES REPLACED------------------------------
//	//m_pLinkResetCounters = LENMUS_NEW ImoTextItem( this, lmID_LINK_RESET_COUNTERS, _("Reset counters"), wxDefaultPosition, wxDefaultSize, 0 );
//	//m_pLinkResetCounters->Wrap( -1 );
//	//m_pLinkResetCounters->SetFont( wxFont( 8, 74, 90, 90, true, wxT("Tahoma") ) );
//	//m_pLinkResetCounters->SetForegroundColour( wxColour( 0, 0, 255 ) );
//	//
//	//pCtrolPara->Add( m_pLinkResetCounters, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5 );
//
//    ////TODO 5.0
//    //pCtrolPara->Add(
//    //    LENMUS_NEW lmUrlAuxCtrol(this, lmID_LINK_RESET_COUNTERS, m_rScale, _("Reset counters"),
//    //                      _T("link_reset")),
//    //    0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5 );
//    //END OF MODIFICATION----------------------------------
//
//	this->SetPara( pCtrolPara );
//	this->Layout();
//}
//
////---------------------------------------------------------------------------------------
//void PractiseCounters::UpdateDisplay()
//{
//    int nRight = m_pProblemMngr->GetRight();
//    int nWrong = m_pProblemMngr->GetWrong();
//
//    //update display for right answers
//    m_pTxtRight->SetLabel( wxString::Format(_T("%d"), nRight) );
//
//    //update display for wrong answers
//    m_pTxtWrong->SetLabel( wxString::Format(_T("%d"), nWrong) );
//
//    //update display for total score
//    int nTotal = nRight + nWrong;
//    if (nTotal == 0)
//        m_pTxtTotal->SetLabel( _T("-") );
//    else
//    {
//        float rScore = (float)(10 * nRight) / (float)nTotal;
//        m_pTxtTotal->SetLabel( wxString::Format(_T("%.01f"), rScore) );
//    }
//    Refresh();
//}
//
////---------------------------------------------------------------------------------------
//void PractiseCounters::OnResetCounters(wxCommandEvent& WXUNUSED(event))
//{
//    m_pProblemMngr->ResetPractiseCounters();
//    UpdateDisplay();
//}


}   // namespace lenmus
