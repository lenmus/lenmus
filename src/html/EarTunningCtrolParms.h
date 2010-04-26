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

#ifndef __LM_EARTUNNINGCTROLPARMS_H__        //to avoid nested includes
#define __LM_EARTUNNINGCTROLPARMS_H__

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "ObjectParams.h"
#include "../ldp_parser/AuxString.h"
#include "ParamsParser.h"


// This class pack all parameters to set up a Ear Tunning exercise,
// The settings must be read/setup by the lmEarTunningCtrol object.

class lmEarTunningCtrolParms : public lmExerciseParams
{
public:
    lmEarTunningCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmEarTunningCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:

        // Member variables:

    // html object window attributes
    long                m_nWindowStyle;
    lmExerciseOptions*  m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmEarTunningCtrolParms)
};



lmEarTunningCtrolParms::lmEarTunningCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmExerciseParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // construct constraints object
    m_pConstrains = new lmExerciseOptions(_T("EarTunning"));
    m_pOptions = m_pConstrains;

    // initializations
    m_sParamErrors = _T("");    //no errors

}


lmEarTunningCtrolParms::~lmEarTunningCtrolParms()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != _T("")) {
        if (m_pConstrains) delete m_pConstrains;
    }

}

void lmEarTunningCtrolParms::AddParam(const wxHtmlTag& tag)
{
    /* Params for lmEarTunningCtrol

        control_settings    Value="[key for storing the settings]"
                            By coding this param it is forced the inclusion of
                            the 'settings' link. Its value will be used
                            as the key for saving the user settings.

    */

    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.MakeUpper();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute

    //// control_settings
    //if ( sName == _T("CONTROL_SETTINGS") ) {
    //    m_pConstrains->SetSettingsLink(true);
    //    m_pConstrains->SetSection( tag.GetParam(_T("VALUE") ));
    //}

    // Unknown param
    else
        lmExerciseParams::AddParam(tag);


}

void lmEarTunningCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    //inform about param errors or create the control
    wxWindow* pWnd;
    if (m_sParamErrors != _T("")) {
        // there are errors: display a text box with the error message
        pWnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1, m_sParamErrors,
            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
    }
    else {
        // create the EarTunningCtrol
        pWnd = new lmEarTunningCtrol((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1,
            m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    }
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(pWnd, m_nPercent));

}

#endif  // __LM_EARTUNNINGCTROLPARMS_H__
