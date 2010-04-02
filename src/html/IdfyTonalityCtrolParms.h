//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_IDFYTONALITYCTROLPARMS_H__        //to avoid nested includes
#define __LM_IDFYTONALITYCTROLPARMS_H__

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
#include "../exercises/TonalityConstrains.h"


// This class pack all parameters to set up a Tonality identification exercise,
// The settings must be read/setup by the IdfyTonalityCtrol object.

class lmIdfyTonalityCtrolParms : public lmExerciseParams
{
public:
    lmIdfyTonalityCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmIdfyTonalityCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:
    wxString ParseAnswerButtons(wxString sParamValue, wxString sFullParam);

        // Member variables:

    // html object window attributes
    long                    m_nWindowStyle;
    lmTonalityConstrains*   m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmIdfyTonalityCtrolParms)
};



lmIdfyTonalityCtrolParms::lmIdfyTonalityCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmExerciseParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // construct constraints object
    m_pConstrains = new lmTonalityConstrains(_T("IdfyTonality"));
    m_pOptions = m_pConstrains;

    // initializations
    m_sParamErrors = _T("");    //no errors

}


lmIdfyTonalityCtrolParms::~lmIdfyTonalityCtrolParms()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != _T("")) {
        if (m_pConstrains) delete m_pConstrains;
    }

}

void lmIdfyTonalityCtrolParms::AddParam(const wxHtmlTag& tag)
{
    //    Params for lmIdfyTonalityCtrol - html object type="Application/LenMusIdfyTonality"
    //
    //    keys        Keyword "all", "allMajor", "allMinor" or a list of allowed
    //                key signatures, i.e.: "Do,Fas". Default: all
    //
    //    tonality_buttons    Either "majorMinor" or "keySignatures".
    //                        Default: "keySignatures"
    //
    //    control_settings    Value="[key for storing the settings]"
    //                        By coding this param it is forced the inclusion of
    //                        the 'settings' link. Its value will be used
    //                        as the key for saving the user settings.
    //
    //    Example:
    //    ------------------------------------
    //    <exercise type="IdfyTonality" width="100%" height="300" border="0">
    //        <control_settings>EarIdfyTonality<control_settings/>
    //        <keys>all</keys>
    //    </exercise>


    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.MakeUpper();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute

        // Process the parameters

    //keys        keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
    else if ( sName == _T("KEYS") ) {
        m_sParamErrors += ParseKeys(tag.GetParam(_T("VALUE")), tag.GetAllParams(),
                                    m_pConstrains->GetKeyConstrains());
    }

    //tonality_buttons
    else if ( sName == _T("TONALITY_BUTTONS")) {
        m_sParamErrors += ParseAnswerButtons(tag.GetParam(_T("VALUE")), tag.GetAllParams());
    }

    // Unknown param
    else
        lmExerciseParams::AddParam(tag);


}

void lmIdfyTonalityCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    //inform about param errors or create the control
    wxWindow* pWnd;
    if (m_sParamErrors != _T("")) {
        // there are errors: display a text box with the error message
        pWnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1, m_sParamErrors,
            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
    }
    else {
        // create the IdfyTonalityCtrol
        pWnd = new lmIdfyTonalityCtrol((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1,
            m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    }
    pWnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(pWnd, m_nPercent));

}

wxString lmIdfyTonalityCtrolParms::ParseAnswerButtons(wxString sParamValue, wxString sFullParam)
{
    // tonality_buttons    Either "majorMinor" or "keySignatures".

    if (sParamValue != _T("MAJORMINOR"))
    {
        m_pConstrains->UseMajorMinorButtons(true);
        return wxEmptyString;
    }
    else if (sParamValue != _T("KEYSIGNATURES"))
    {
        m_pConstrains->UseMajorMinorButtons(false);
        return wxEmptyString;
    }
    else
        return wxString::Format( 
            _T("Invalid param value in:\n<param %s >\n")
            _T("Invalid value = %s \n")
            _T("Acceptable format: either 'majorMinor' or 'keySignatures'.\n"),
            sFullParam.c_str(), sParamValue.c_str() );
}


#endif  // __LM_IDFYTONALITYCTROLPARMS_H__
