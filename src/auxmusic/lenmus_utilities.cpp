//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

//lenmus
#include "lenmus_utilities.h"

#include "lenmus_string.h"
#include "lenmus_images_creator.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/bmpcbox.h>

//lomse
#include <lomse_doorway.h>
#include <lomse_document.h>
#include <lomse_graphic_view.h>
#include <lomse_interactor.h>
#include <lomse_presenter.h>
#include <lomse_im_note.h>
using namespace lomse;



namespace lenmus
{

//---------------------------------------------------------------------------------------
bool ldp_pattern_is_rest(const wxString& sElement)
{
    // returns true if received element is a rest.
    // sElement must be normalized (lower case, no extra spaces)

    return (sElement.substr(1, 1) == "r" );
}

//---------------------------------------------------------------------------------------
int split_ldp_pattern(const wxString& sSource)
{
    // Receives an string formed by concatenated elements, for example:
    // "(n * q)(n * s g+)(n * s)(n * e g-)"
    // sSource must be normalized (lower case, no extra spaces)
    // Return the index to the end (closing parenthesis) of first element

    int i;                  //index to character being explored
    int iMax;               //sSource length
    int nAPar;              //open parenthesis counter

    iMax = sSource.length();
    wxASSERT(iMax > 0);                         //sSource must not be empty
    wxASSERT(sSource.substr(0, 1) == "(" );    //must start with parenthesis

    nAPar = 1;       //let//s count first parenthesis
    //look for the matching closing parenthesis
    bool fFound = false;
    for (i=1; i < iMax; i++) {
        if (sSource.substr(i, 1) == "(" ) {
            nAPar++;
        } else if (sSource.substr(i, 1) == ")" ) {
            nAPar--;
            if (nAPar == 0) {
                //matching parenthesis found. Exit loop
                fFound = true;
                break;
            }
        }
    }
    if (fFound)
        return i;

    wxASSERT(fFound);
    return i;
}

//---------------------------------------------------------------------------------------
void load_combobox_with_note_names(wxComboBox* pCboBox, DiatonicPitch nSelNote)
{
    pCboBox->Clear();
    for (int i=1; i < 60; i++)
    {
        DiatonicPitch dp(i);
        pCboBox->Append( to_wx_string(dp.get_english_note_name()) );
    }
    DiatonicPitch dp(nSelNote);
    pCboBox->SetValue( to_wx_string(dp.get_english_note_name()) );
}

//---------------------------------------------------------------------------------------
void load_combobox_with_note_names(wxComboBox* pCboBox, wxString sNoteName)
{
    pCboBox->Clear();
    for (int i=1; i < 60; i++)
    {
        DiatonicPitch dp(i);
        pCboBox->Append( to_wx_string(dp.get_english_note_name()) );
    }
    pCboBox->SetValue( sNoteName );
}

////---------------------------------------------------------------------------------------
//void lmLoadChoiceWithNoteNames(wxChoice* pChoice, wxString sNoteName)
//{
//    int i;
//    pChoice->Clear();
//    for (i=1; i < 60; i++) {
//        pChoice->Append( GetNoteNamePhysicists((DiatonicPitch) i) );
//    }
//    pChoice->SetStringSelection(sNoteName);
//}
//
////---------------------------------------------------------------------------------------
//wxString LineStyleToLDP(lmELineStyle nStyle)
//{
//    switch(nStyle)
//    {
//        case lm_eLine_None:
//            return "none";
//        case lm_eLine_Solid:
//            return "solid";
//        case lm_eLine_LongDash:
//            return "longDash";
//        case lm_eLine_ShortDash:
//            return "shortDash";
//        case lm_eLine_Dot:
//            return "dot";
//        case lm_eLine_DotDash:
//            return "dotDash";
//        default:
//            wxLogMessage("[LineStyleToLDP] Error: invalid line style %d", nStyle);
//            wxASSERT(false);
//            return "";      //compiler happy
//    }
//};
//
////---------------------------------------------------------------------------------------
//wxString LineCapToLDP(lmELineCap nLineCap)
//{
//    switch(nLineCap)
//    {
//        case lm_eLineCap_None:
//            return "none";
//        case lm_eLineCap_Arrowhead:
//            return "arrowhead";
//        case lm_eLineCap_Arrowtail:
//            return "arrowtail";
//        case lm_eLineCap_Circle:
//            return "circle";
//        case lm_eLineCap_Square:
//            return "square";
//        case lm_eLineCap_Diamond:
//            return "diamond";
//        default:
//            wxLogMessage("[LineCapToLDP] Error: invalid line cap %d", nLineCap);
//            wxASSERT(false);
//            return "";      //compiler happy
//    }
//};
//
////---------------------------------------------------------------------------------------
//wxString lmTPointToLDP(lmTPoint& tPoint, const wxString& sName,
//                       bool fEmptyIfZero)
//{
//    //if sName != empty generates "(<sName> dx:<x> dy:<y>)"
//    //else generates "dx:<x> dy:<y>".
//    //if fEmptyIfZero==true values 0.0 are not generated. For instance:
//    //  tPoint=(0.0, 3.7) will generate "dy:3.7"
//    //if both are 0 will return empty string
//
//	wxString sSource = "";
//
//    if (fEmptyIfZero && tPoint.x == 0.0f && tPoint.y == 0.0f)
//        return sSource;
//
//    //element name
//    if (sName != wxEmptyString)
//        sSource = "(" + sName;
//
//    //dx & dy values
//    if (!(fEmptyIfZero && tPoint.x == 0.0f))
//    {
//		sSource += " dx:";
//        sSource += DoubleToStr((double)tPoint.x, 4);
//    }
//    if (!(fEmptyIfZero && tPoint.y == 0.0f))
//    {
//		sSource += " dy:";
//        sSource += DoubleToStr((double)tPoint.y, 4);
//    }
//
//    //close element
//    if (sName != wxEmptyString)
//        sSource += ")";
//
//    return sSource;
//}
//
////---------------------------------------------------------------------------------------
//wxString lmColorToLDP(const wxColour& nColor, bool fEmptyIfEqual,
//                      const wxColour& nRefColor)
//{
//    //Generates "(color #rrggbb)"
//    //if fEmptyIfEqual==true and nRefColor==nColor returns empty string
//
//    if (fEmptyIfEqual && nRefColor==nColor)
//        return wxEmptyString;
//
//    //element name
//	wxString sSource = "(color ";
//    sSource += nColor.GetAsString(wxC2S_HTML_SYNTAX);
//    sSource += ")";
//
//    return sSource;
//}
//
////---------------------------------------------------------------------------------------
//wxString lmFloatToLDP(float rValue, const wxString& sName,
//                      bool fEmptyIfEqual, float rRefValue)
//{
//    //Generates "(<sName> <value>)"
//    //if fEmptyIfEqual==true and rRefValue==rValue returns empty string
//
//    if (fEmptyIfEqual && rRefValue==rValue)
//        return wxEmptyString;
//
//    //element name
//    wxString sSource = wxString::Format("(%s ", sName.wx_str());
//	sSource += DoubleToStr((double)rValue, 4);
//    sSource += ")";
//
//    return sSource;
//}
//




//=======================================================================================
// global functions related to barlines
//=======================================================================================

void load_barlines_bitmap_combobox(ApplicationScope& appScope,
                                   wxBitmapComboBox* pCtrol,
                                   BarlinesDBEntry tBarlines[])
{
    pCtrol->Clear();
    int i=0;
	while (tBarlines[i].nBarlineType != k_barline_unknown)
    {
        pCtrol->Append(wxEmptyString,
                       generate_bitmap_for_barline_ctrol(appScope,
                                                         tBarlines[i].sBarlineName,
                                                         tBarlines[i].nBarlineType),
					   (void*)(&tBarlines[i]) );
		i++;
    }
	if (i > 0)
		pCtrol->SetSelection(0);
}

//---------------------------------------------------------------------------------------
void select_barline_in_bitmap_combobox(wxBitmapComboBox* pCtrol, EBarline nType)
{
	//select received barline type in the barlines combo box
	int nMax = pCtrol->GetCount();
	for (int iB=0; iB < nMax; iB++)
	{
		if (nType == ((BarlinesDBEntry*)pCtrol->GetClientData(iB))->nBarlineType)
		{
			//wxLogMessage("[select_barline_in_bitmap_combobox] nType=%d, iB=%d, nMax=%d", nType, iB, nMax);
			pCtrol->SetSelection(iB);
			return;
		}
	}
}

//---------------------------------------------------------------------------------------
const wxString get_barline_name(int barlineType)
{
    static wxString m_name[k_max_barline];
    static wxString m_language = "??";

    if (m_language != ApplicationScope::get_language())
    {
        //language dependent strings. Can not be statically initialized because
        //then they do not get translated
        m_name[k_barline_simple] =                  _("Simple barline");
        m_name[k_barline_double] =                  _("Double barline");
        m_name[k_barline_end] =                     _("Final barline");
        m_name[k_barline_start_repetition] =        _("Start repetition");
        m_name[k_barline_end_repetition] =          _("End repetition");
        m_name[k_barline_start] =                   _("Start barline");
        m_name[k_barline_double_repetition] =       _("Double repetition");
        m_name[k_barline_double_repetition_alt] =   _("Double repetition alt.");

        m_language = ApplicationScope::get_language();
    }

    if (barlineType <= 0 || barlineType >= k_max_barline)
        return "Invalid barline";
    else
        return m_name[barlineType];
}

//---------------------------------------------------------------------------------------
const wxString get_stem_name(int stemType)
{
    static wxString m_name[5];
    static wxString m_language = "??";

    if (m_language != ApplicationScope::get_language())
    {
        //language dependent strings. Can not be statically initialized because
        //then they do not get translated
        m_name[k_stem_default] =    _("Default stem");
        m_name[k_stem_up] =         _("Stem up");
        m_name[k_stem_down] =       _("Stem down");
        m_name[k_stem_double] =     _("Stem double");
        m_name[k_stem_none] =       _("Stem none");

        m_language = ApplicationScope::get_language();
    }

    if (stemType < 0 || stemType > 4)
        return "Invalid stem";
    else
        return m_name[stemType];
}

//=======================================================================================
// global functions related to clefs
//=======================================================================================



//=======================================================================================
// global functions related to key signatures
//=======================================================================================

const wxString& get_key_signature_name(EKeySignature type)
{
    static wxString m_name[k_num_keys];
    static wxString m_language = "??";

    if (m_language != ApplicationScope::get_language())
    {
        //language dependent strings. Can not be statically initiallized because
        //then they do not get translated
        m_name[k_key_C] = _("C Major");
        m_name[k_key_G] = _("G Major");
        m_name[k_key_D] = _("D Major");
        m_name[k_key_A] = _("A Major");
        m_name[k_key_E] = _("E Major");
        m_name[k_key_B] = _("B Major");
        m_name[k_key_Fs] = _("F # Major");
        m_name[k_key_Cs] = _("C # Major");
        m_name[k_key_Cf] = _("C b Major");
        m_name[k_key_Gf] = _("G b Major");
        m_name[k_key_Df] = _("D b Major");
        m_name[k_key_Af] = _("A b Major");
        m_name[k_key_Ef] = _("E b Major");
        m_name[k_key_Bf] = _("B b Major");
        m_name[k_key_F] = _("F Major");

        m_name[k_key_a] = _("A minor");
        m_name[k_key_e] = _("E minor");
        m_name[k_key_b] = _("B minor");
        m_name[k_key_fs] = _("F # minor");
        m_name[k_key_cs] = _("C # minor");
        m_name[k_key_gs] = _("G # minor");
        m_name[k_key_ds] = _("D # minor");
        m_name[k_key_as] = _("A # minor");
        m_name[k_key_af] = _("A b minor");
        m_name[k_key_ef] = _("E b minor");
        m_name[k_key_bf] = _("B b minor");
        m_name[k_key_f] = _("F minor");
        m_name[k_key_c] = _("C minor");
        m_name[k_key_g] = _("G minor");
        m_name[k_key_d] = _("D minor");

        m_language = ApplicationScope::get_language();
    }
    return m_name[type];
}


//=======================================================================================
// global functions related to rendering scores
//=======================================================================================

wxBitmap generate_bitmap_for_key_ctrol(ApplicationScope& appScope,
                                       wxString& sName, EKeySignature type)
{
    //create a document
    LomseDoorway& lomse = appScope.get_lomse();
    ImagesCreator creator(lomse);
    Document* pDoc = creator.get_empty_document();

    //remove page margins
    ImoPageInfo* pPageInfo = pDoc->get_page_info();
    pPageInfo->set_top_margin(0);
    pPageInfo->set_left_margin(0);
    pPageInfo->set_right_margin(0);

    //create the score
    ImoScore* pScore = pDoc->add_score();
	pScore->set_bool_option("Staff.DrawLeftBarline", false);
    ImoInstrument* pInstr = pScore->add_instrument();
    ImoSystemInfo* pInfo = pScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(15) );     //1.5 lines
    pPageInfo = pScore->get_page_info();
    pPageInfo->set_top_margin( 0 );
    pPageInfo->set_left_margin( 0 );
    pPageInfo->set_right_margin( 0 );

    //add spacer with attached text
    if (sName != "")
    {
        string text = "(spacer 10 (text \"";
        text.append( to_std_string(sName) );
        text.append( "\" (dx -10)(dy 75)))" );
        pInstr->add_object(text);
    }
    else
        pInstr->add_spacer(10);       // 1 line

    //add key signature and some additional space
    pInstr->add_clef(k_clef_G2, 1, k_no_visible);
    pInstr->add_key_signature(type);
    pInstr->add_spacer(20);       // 2 lines
    pInstr->add_barline(k_barline_simple, k_no_visible);

    pScore->end_of_changes();

    //render the document
    wxImage image(108, 64);
    creator.create_image_for_document(&image, 0.80);

    image.SaveFile("keys.jpg", wxBITMAP_TYPE_JPEG);

    wxBitmap bmp(image);
    return bmp;
}

//---------------------------------------------------------------------------------------
wxBitmap generate_bitmap_for_clef_ctrol(ApplicationScope& appScope,
                                        wxString& sName, EClef type)
{
    //create a document
    LomseDoorway& lomse = appScope.get_lomse();
    ImagesCreator creator(lomse);
    Document* pDoc = creator.get_empty_document();

    //remove page margins
    ImoPageInfo* pPageInfo = pDoc->get_page_info();
    pPageInfo->set_top_margin(0);
    pPageInfo->set_left_margin(0);
    pPageInfo->set_right_margin(0);

    //create the score
    ImoScore* pScore = pDoc->add_score();
	pScore->set_bool_option("Staff.DrawLeftBarline", false);
    ImoInstrument* pInstr = pScore->add_instrument();
    ImoSystemInfo* pInfo = pScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(15) );     //1.5 lines
    pPageInfo = pScore->get_page_info();
    pPageInfo->set_top_margin( 0 );
    pPageInfo->set_left_margin( 0 );
    pPageInfo->set_right_margin( 0 );

    //add spacer with attached text
    if (sName != "")
    {
        string text = "(spacer 10 (text \"";
        text.append( to_std_string(sName) );
        text.append( "\" (dx -10)(dy 75)))" );
        pInstr->add_object(text);
    }
    else
        pInstr->add_spacer(10);       // 1 line

    //add clef and some additional space
    pInstr->add_clef(type);
    pInstr->add_spacer(20);       // 2 lines
    pInstr->add_barline(k_barline_simple, k_no_visible);

    pScore->end_of_changes();

    //render the document
    wxImage image(108, 64);
    creator.create_image_for_document(&image, 0.80);

    //image.SaveFile("clefs.jpg", wxBITMAP_TYPE_JPEG);

    wxBitmap bmp(image);
    return bmp;
}

//---------------------------------------------------------------------------------------
wxBitmap generate_bitmap_for_barline_ctrol(ApplicationScope& appScope,
                                           wxString& sName, EBarline type)
{
    //create a document
    LomseDoorway& lomse = appScope.get_lomse();
    ImagesCreator creator(lomse);
    Document* pDoc = creator.get_empty_document();

    //remove page margins
    ImoPageInfo* pPageInfo = pDoc->get_page_info();
    pPageInfo->set_top_margin(0);
    pPageInfo->set_left_margin(0);
    pPageInfo->set_right_margin(0);

    //create the score
    ImoScore* pScore = pDoc->add_score();
	pScore->set_bool_option("Staff.DrawLeftBarline", false);
    ImoInstrument* pInstr = pScore->add_instrument();
    ImoSystemInfo* pInfo = pScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(15) );     //1.5 lines
    pPageInfo = pScore->get_page_info();
    pPageInfo->set_top_margin( 0 );
    pPageInfo->set_left_margin( 0 );
    pPageInfo->set_right_margin( 0 );

    //add spacer with attached text
    if (sName != "")
    {
        string text = "(spacer 30 (text \"";
        text.append( to_std_string(sName) );
        text.append( "\" (dx -25)(dy 65)))" );
        pInstr->add_object(text);
    }
    else
        pInstr->add_spacer(20);       // 2 lines

    //add barline and some additional space
    pInstr->add_barline(type);
    pInstr->add_spacer(20);       // 2 lines
    pInstr->add_barline(k_barline_simple, k_no_visible);

    pScore->end_of_changes();

    //render the document
    wxImage image(108, 64);
    creator.create_image_for_document(&image, 0.80);

    //image.SaveFile("barlines.jpg", wxBITMAP_TYPE_JPEG);

    wxBitmap bmp(image);
    return bmp;
}



}  //namespace lenmus
