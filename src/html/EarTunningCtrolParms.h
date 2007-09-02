//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifndef __LM_IDFYCADENCESCTROLPARMS_H__        //to avoid nested includes
#define __LM_IDFYCADENCESCTROLPARMS_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ObjectParams.h"
#include "../ldp_parser/AuxString.h"
#include "ParamsParser.h"
#include "../exercises/CadencesConstrains.h"


// This class pack all parameters to set up a Cadence identification exercise,
// The settings must be read/setup by the IdfyCadencesCtrol object.

class lmIdfyCadencesCtrolParms : public lmObjectParams
{
public:
    lmIdfyCadencesCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmIdfyCadencesCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:
    wxString ParseCadences(wxString sParamValue, wxString sFullParam, bool* pfValidCadences);
    lmECadenceType CadenceNameToType(wxString sCadence);
    wxString ParseAnswerButtons(wxString sParamValue, wxString sFullParam, bool* pfValidButtons);

        // Member variables:

    // html object window attributes
    long                    m_nWindowStyle;
    wxString                m_sParamErrors;
    lmCadencesConstrains*   m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmIdfyCadencesCtrolParms)
};



lmIdfyCadencesCtrolParms::lmIdfyCadencesCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmObjectParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // construct constraints object
    m_pConstrains = new lmCadencesConstrains(_T("IdfyCadence"));

    // initializations
    m_sParamErrors = _T("");    //no errors

}


lmIdfyCadencesCtrolParms::~lmIdfyCadencesCtrolParms()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != _T("")) {
        if (m_pConstrains) delete m_pConstrains;
    }

}

void lmIdfyCadencesCtrolParms::AddParam(const wxHtmlTag& tag)
{
    //    Params for lmIdfyCadenceCtrol - html object type="Application/LenMusIdfyChord"
    //
    //    keys        Keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
    //                Default: all
    //
    //    cadences    Keywords "all", "all_perfect", "all_imperfect", "all_plagal",
    //                  "all_deceptive", "all_half", "all_terminal", "all_transient"
    //                  or a list of allowed cadences:
    //                  Perfect authentic: 
    //                      V_I_Perfect, V7_I, Va5_I, Vd5_I
    //                  Plagal:
    //                      IV_I, IVm_I, IIc6_I, IImc6_I
    //                  Imperfect authentic:
	//                      V_I_Imperfect
    //                  Deceptive:
    //                      V_IV, V_IVm, V_VI, V_VIm, V_IIm, V_III, V_VII
    //                  Half cadences:
    //                      IImc6_V, IV_V, I_V, Ic64_V, IV6_V, II_V, IIdimc6_V, VdeVdim5c64_V
    //
    //                Default: "all"
    //
    //    mode        'theory' | 'earTraining'  Keyword indicating type of exercise
    //
    //    play_key     'A4 | tonic_chord' Default: 'tonic_chord'
    //
    //    cadence_buttons   list of answer buttons to display:
    //                 'terminal, transient, perfect, plagal, imperfect, deceptive, half'
    //
    //
    //    control_settings    Value="[key for storing the settings]"
    //                        By coding this param it is forced the inclusion of
    //                        the 'settings' link. Its value will be used
    //                        as the key for saving the user settings.

    //    Example:
    //    ------------------------------------
    //    <object type="Application/LenMus" class="IdfyScale" width="100%" height="300" border="0">
    //        <param name="mode" value="earTraining">
    //        <param name="scales" value="mT,MT,aT,dT,m7,M7,dom7">
    //        <param name="keys" value="all">
    //    </object>


    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.MakeUpper();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute

        // Process the parameters

    // "Go back to theory" link
    if ( sName == _T("CONTROL_GO_BACK") ) {
        m_pConstrains->SetGoBackLink( tag.GetParam(_T("VALUE") ));
    }

    // cadences      list of allowed cadences:
    else if ( sName == _T("CADENCES") ) {
        m_sParamErrors += ParseCadences(tag.GetParam(_T("VALUE")), tag.GetAllParams(),
                                    m_pConstrains->GetValidCadences());
    }

    // cadence_buttons      list of answer buttons to display
    else if ( sName == _T("CADENCE_BUTTONS") ) {
        m_sParamErrors += ParseAnswerButtons(tag.GetParam(_T("VALUE")), tag.GetAllParams(),
                                    m_pConstrains->GetValidButtons());
    }

    // mode        'theory | earTraining'  Keyword indicating type of exercise
    else if ( sName == _T("MODE") ) {
        wxString sMode = tag.GetParam(_T("VALUE"));
        if (sMode == _T("theory"))
            m_pConstrains->SetTheoryMode(true);
        else if (sMode == _T("earTraining"))
            m_pConstrains->SetTheoryMode(false);
        else {
            m_sParamErrors += wxString::Format( 
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values:  'theory | earTraining'\n"),
                tag.GetAllParams().c_str(), sMode.c_str() );
        }
    }

    //keys        keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
    else if ( sName == _T("KEYS") ) {
        m_sParamErrors += ParseKeys(tag.GetParam(_T("VALUE")), tag.GetAllParams(),
                                    m_pConstrains->GetKeyConstrains());
    }

    // control_settings
    else if ( sName == _T("CONTROL_SETTINGS") ) {
        m_pConstrains->SetSettingsLink(true);
        m_pConstrains->SetSection( tag.GetParam(_T("VALUE") ));
    }

    // Unknown param
    else
        m_sParamErrors += wxString::Format(
            _T("lmIdfyCadencesCtrol. Unknown param: <param %s >\n"),
            tag.GetAllParams().c_str() );

}

void lmIdfyCadencesCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    //inform about param errors or create the control
    wxWindow* pWnd;
    if (m_sParamErrors != _T("")) {
        // there are errors: display a text box with the error message
        pWnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1, m_sParamErrors,
            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
    }
    else {
        // create the IdfyCadencesCtrol
        pWnd = new lmIdfyCadencesCtrol((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1,
            m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    }
    pWnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(pWnd, m_nPercent));

}

wxString lmIdfyCadencesCtrolParms::ParseAnswerButtons(wxString sParamValue, wxString sFullParam,
                                                 bool* pfValidButtons)
{
    //    cadence_buttons   list of answer buttons to display:
    //                 'terminal, transient, perfect, plagal, imperfect, deceptive, half'

    bool fError = false;

    //disable all buttons
    for (long i=0; i < lm_eCadMaxButton; i++) {
        *(pfValidButtons + i) = false;
    }

    //loop to get allowed buttons
    lmECadenceButtons nButton;
    int iColon;
    wxString sButton;
    while (sParamValue != _T("")) {
        //get button
        iColon = sParamValue.Find(_T(","));
        if (iColon != -1) {
            sButton = sParamValue.Left(iColon);
            sParamValue = sParamValue.substr(iColon + 1);      //skip the colon
        }
        else {
            sButton = sParamValue;
            sParamValue = _T("");
        }

        if (sButton == _T("terminal"))
            nButton = lm_eCadButtonTerminal;
        else if (sButton == _T("transient"))
            nButton = lm_eCadButtonTransient;
        else if (sButton == _T("perfect"))
            nButton = lm_eCadButtonPerfect;
        else if (sButton == _T("plagal"))
            nButton = lm_eCadButtonPlagal;
        else if (sButton == _T("imperfect"))
            nButton = lm_eCadButtonImperfect;
        else if (sButton == _T("deceptive"))
            nButton = lm_eCadButtonDeceptive;
        else if (sButton == _T("half"))
            nButton = lm_eCadButtonHalf;
        else {
            fError = true;
            break;
        }
        *(pfValidButtons + (int)nButton) = true;
    }

    if (fError)
        return wxString::Format( 
            _T("Invalid param value in:\n<param %s >\n")
            _T("Invalid value = %s \n")
            _T("Acceptable format: a list of allowed buttons.\n"),
            sFullParam.c_str(), sParamValue.c_str() );
    else
        return wxEmptyString;

}

wxString lmIdfyCadencesCtrolParms::ParseCadences(wxString sParamValue, wxString sFullParam,
                                                 bool* pfValidCadences)
{
    //    cadences    Keywords "all", "all_perfect", "all_imperfect", "all_plagal",
    //                  "all_deceptive", "all_half"
    //                  or a list of allowed cadences:
    //                  Perfect authentic: 
    //                      V_I_Perfect, V7_I, Va5_I, Vd5_I
    //                  Plagal:
    //                      IV_I, IVm_I, IIc6_I, IImc6_I
    //                  Imperfect authentic:
	//                      V_I_Imperfect
    //                  Deceptive:
    //                      V_IV, V_IVm, V_VI, V_VIm, V_IIm, V_III, V_VII
    //                  Half cadences:
    //                      IImc6_V, IV_V, I_V, Ic64_V, IV6_V, II_V, IIdimc6_V, VdeVdim5c64_V
    //
    //                Default: "all"

    bool fError = false;

    // disable all cadences
    for (int i=0; i < lm_eCadMaxCadence; i++) {
        *(pfValidCadences+i) = false;
    }

    //loop to get allowed cadences
    int iColon;
    wxString sCadence;
    lmECadenceType nType;
    while (sParamValue != _T("")) {
        //get cadence
        iColon = sParamValue.Find(_T(","));
        if (iColon != -1) {
            sCadence = sParamValue.Left(iColon);
            sParamValue = sParamValue.substr(iColon + 1);      //skip the colon
        }
        else {
            sCadence = sParamValue;
            sParamValue = _T("");
        }

        //determine cadence
        if (sCadence == _T("all")) {
            // allow all cadences
            for (int i=0; i < lm_eCadMaxCadence; i++) {
                *(pfValidCadences+i) = true;
            }
        }
        else if (sCadence == _T("all_perfect")) {
            // allow all Perfect cadences
            for (int i=lm_eCadPerfect; i < lm_eCadLastPerfect; i++) {
                *(pfValidCadences+i) = true;
            }
        }
        else if (sCadence == _T("all_plagal")) {
            // allow all Plagal cadences
            for (int i=lm_eCadPlagal; i < lm_eCadLastPlagal; i++) {
                *(pfValidCadences+i) = true;
            }
        }
        else if (sCadence == _T("all_deceptive")) {
            // allow all Deceptive cadences
            for (int i=lm_eCadDeceptive; i < lm_eCadLastDeceptive; i++) {
                *(pfValidCadences+i) = true;
            }
        }
        else if (sCadence == _T("all_half")) {
            // allow all Half cadences
            for (int i=lm_eCadHalf; i < lm_eCadLastHalf; i++) {
                *(pfValidCadences+i) = true;
            }
        }
        else if (sCadence == _T("all_imperfect")) {
            // allow all Imperfect cadences
            for (int i=lm_eCadImperfect; i < lm_eCadLastImperfect; i++) {
                *(pfValidCadences+i) = true;
            }
        }
        else {
            nType = CadenceNameToType(sCadence);
            if (nType == (lmECadenceType)-1) {
                fError = true;
                break;
            }
            *(pfValidCadences + (int)nType) = true;
        }
    }

    if (fError)
        return wxString::Format( 
            _T("Invalid param value in:\n<param %s >\n")
            _T("Invalid value = %s \n")
            _T("Acceptable format: Keywords 'all', 'all_xxxx' or a list of allowed cadences.\n"),
            sFullParam.c_str(), sCadence.c_str() );
    else
        return wxEmptyString;

}

lmECadenceType lmIdfyCadencesCtrolParms::CadenceNameToType(wxString sCadence)
{
    //AWARE: biyective to lmECadenceType
    static const wxString sNames[] = {
        // Perfect authentic: 
        _T("V_I_Perfect"), _T("V7_I"), _T("Va5_I"), _T("Vd5_I"),
        // Plagal:
        _T("IV_I"), _T("IVm_I"), _T("IIc6_I"), _T("IImc6_I"),
        // Imperfect authentic:
	    _T("V_I_Imperfect"),
        // Deceptive:
        _T("V_IV"), _T("V_IVm"), _T("V_VI"), _T("V_VIm"), _T("V_IIm"), 
        _T("V_III"), _T("V_VII"),
        // Half cadences:
        _T("IImc6_V"), _T("IV_V"), _T("I_V"), _T("Ic64_V"), _T("IV6_V"), 
        _T("II_V"), _T("IIdimc6_V"), _T("VdeVdim5c64_V"),
    };

    for (int i=0; i < lm_eCadMaxCadence; i++) {
        if (sCadence == sNames[i]) return (lmECadenceType)i;
    }
    return (lmECadenceType)-1;

}

#endif  // __LM_IDFYCADENCESCTROLPARMS_H__
