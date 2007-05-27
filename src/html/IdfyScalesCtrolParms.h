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
/*! @file IdfyScalesCtrolParms.h
    @brief Header file for class lmIdfyScalesCtrolParms
    @ingroup html_controls
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __EARSCALESCTROLPARMS_H__        //to avoid nested includes
#define __EARSCALESCTROLPARMS_H__

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
#include "../exercises/ScalesConstrains.h"


//! This class pack all parameters to set up a Scale Identification exercise,
//! The settings must be read/setup by the IdfyScalesCtrol object.

class lmIdfyScalesCtrolParms : public lmObjectParams
{
public:
    lmIdfyScalesCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmIdfyScalesCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:

        // Member variables:

    // html object window attributes
    long                    m_nWindowStyle;
    wxString                m_sParamErrors;
    lmScalesConstrains*     m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmIdfyScalesCtrolParms)
};



lmIdfyScalesCtrolParms::lmIdfyScalesCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmObjectParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // construct constraints object
    m_pConstrains = new lmScalesConstrains(_T("IdfyScale"));

    // initializations
    m_sParamErrors = _T("");    //no errors

}


lmIdfyScalesCtrolParms::~lmIdfyScalesCtrolParms()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != _T("")) {
        if (m_pConstrains) delete m_pConstrains;
    }

}

void lmIdfyScalesCtrolParms::AddParam(const wxHtmlTag& tag)
{
    /*! @page IdfyChordCtrolParams
        @verbatim

        Params for lmIdfyChordCtrol - html object type="Application/LenMusIdfyChord"

        keys        Keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
                    Default: all

        scales      Keyword "all" or a list of allowed scales:
                        m-minor, M-major, a-augmented, d-diminished, s-suspended
                        T-triad, dom-dominant, hd-half diminished

                        triads: mT, MT, aT, dT, s4, s2
                        sevenths: m7, M7, a7, d7, mM7, aM7 dom7, hd7
                        sixths: m6, M6, a6

                    Default: "mT,MT,aT,dT,m7,M7"

        mode        'theory' | 'earTraining'  Keyword indicating type of exercise

        play_mode*   'ascending | descending | both' allowed play modes. Default: ascending

        show_key     '0 | 1' Default: 0 (do not display key signature)

        control_settings    Value="[key for storing the settings]"
                            By coding this param it is forced the inclusion of
                            the 'settings' link. Its value will be used
                            as the key for saving the user settings.

        Example:
        ------------------------------------
        <object type="Application/LenMus" class="IdfyScale" width="100%" height="300" border="0">
            <param name="mode" value="earTraining">
            <param name="scales" value="mT,MT,aT,dT,m7,M7,dom7">
            <param name="keys" value="all">
        </object>

        @endverbatim

    */

    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.UpperCase();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute

    // show Key signature
    else if ( sName == _T("SHOW_KEY") ) {
        int nValue;
        bool fOK = tag.GetParamAsInt(_T("VALUE"), &nValue);
        if (!fOK)
            m_sParamErrors += wxString::Format(
                _("Invalid param value in:\n<param %s >\nAcceptable values: 1 | 0 \n"),
                tag.GetAllParams().c_str() );
        else
            m_pConstrains->SetDisplayKey( nValue != 0 );
    }

    // play mode
    else if ( sName == _T("PLAY_MODE") ) {
        wxString sMode = tag.GetParam(_T("VALUE"));
        if (sMode == _T("ascending"))
            m_pConstrains->SetPlayMode(0);
        else if (sMode == _T("descending"))
            m_pConstrains->SetPlayMode(1);
        else if (sMode == _T("both"))
            m_pConstrains->SetPlayMode(2);
        else {
            m_sParamErrors += wxString::Format( wxGetTranslation(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values: 'ascending | descending | both'\n")),
                tag.GetAllParams().c_str(), sMode.c_str() );
        }
    }

    // chords      Keyword "all" or a list of allowed chords:
    else if ( sName == _T("SCALES") ) {
        wxString sClef = tag.GetParam(_T("VALUE"));
        m_sParamErrors += ParseChords(tag.GetParam(_T("VALUE")), tag.GetAllParams(),
                                    m_pConstrains->GetValidScales());
    }

    // mode        'theory | earTraining'  Keyword indicating type of exercise
    else if ( sName == _T("MODE") ) {
        wxString sMode = tag.GetParam(_T("VALUE"));
        if (sMode == _T("theory"))
            m_pConstrains->SetTheoryMode(true);
        else if (sMode == _T("earTraining"))
            m_pConstrains->SetTheoryMode(false);
        else {
            m_sParamErrors += wxString::Format( wxGetTranslation(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values:  'theory | earTraining'\n")),
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
            _("lmIdfyScalesCtrol. Unknown param: <param %s >\n"),
            tag.GetAllParams().c_str() );

}

void lmIdfyScalesCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    //inform about param errors or create the control
    wxWindow* pWnd;
    if (m_sParamErrors != _T("")) {
        // there are errors: display a text box with the error message
        pWnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1, m_sParamErrors,
            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
    }
    else {
        // create the IdfyScalesCtrol
        pWnd = new lmIdfyScalesCtrol((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1,
            m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    }
    pWnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(pWnd, m_nPercent));

}

#endif  // __EARSCALESCTROLPARMS_H__
