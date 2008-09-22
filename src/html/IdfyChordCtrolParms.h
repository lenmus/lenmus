//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_IDFYCHORDCTROLPARMS_H__        //to avoid nested includes
#define __LM_IDFYCHORDCTROLPARMS_H__

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


//! This class pack all parameters to set up a Chord Identification exercise,
//! either for ear training or for theory.
//! The settings must be read/setup by the IdfyChordCtrol object.

class lmIdfyChordCtrolParms : public lmExerciseParams
{
public:
    lmIdfyChordCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmIdfyChordCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:

        // Member variables:

    // html object window attributes
    long                    m_nWindowStyle;
    lmChordConstrains*      m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmIdfyChordCtrolParms)
};



lmIdfyChordCtrolParms::lmIdfyChordCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmExerciseParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // construct constraints object
    m_pConstrains = new lmChordConstrains(_T("IdfyChord"));
    m_pOptions = m_pConstrains;

    // initializations
    m_sParamErrors = _T("");    //no errors

}


lmIdfyChordCtrolParms::~lmIdfyChordCtrolParms()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != _T("")) {
        if (m_pConstrains) delete m_pConstrains;
    }

}

void lmIdfyChordCtrolParms::AddParam(const wxHtmlTag& tag)
{
    /*! @page IdfyChordCtrolParams
        @verbatim

        Params for lmIdfyChordCtrol - html object type="Application/LenMusIdfyChord"

        keys        Keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
                    Default: all

        chords      Keyword "all" or a list of allowed chords:
                        m-minor, M-major, a-augmented, d-diminished, s-suspended
                        T-triad, dom-dominant, hd-half diminished

                        triads: mT, MT, aT, dT, s4, s2
                        sevenths: m7, M7, a7, d7, mM7, aM7 dom7, hd7
                        sixths: m6, M6, a6

                    Default: "mT,MT,aT,dT,m7,M7"

        mode        'theory' | 'earTraining'  Keyword indicating type of exercise

        play_mode*   'chord | ascending | descending' allowed play modes. Default: chord

        show_key     '0 | 1' Default: 0 (do not display key signature)

        inversions  '0 | 1' Default: 0 (do not allow inversions)

        control_settings    Value="[key for storing the settings]"
                            By coding this param it is forced the inclusion of
                            the 'settings' link. Its value will be used
                            as the key for saving the user settings.

        Example:
        ------------------------------------
        <object type="Application/LenMusIdfyChord" width="100%" height="300" border="0">
            <param name="mode" value="earTraining">
            <param name="chords" value="mT,MT,aT,dT,m7,M7,dom7">
            <param name="keys" value="all">
        </object>

        @endverbatim

    */

    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.MakeUpper();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute

    // allow inversions
    if ( sName == _T("INVERSIONS") ) {
        int nValue;
        bool fOK = tag.GetParamAsInt(_T("VALUE"), &nValue);
        if (!fOK)
            m_sParamErrors += wxString::Format(
                _T("Invalid param value in:\n<param %s >\nAcceptable values: 1 | 0 \n"),
                tag.GetAllParams().c_str() );
        else
            m_pConstrains->SetInversionsAllowed( nValue != 0 );
    }

    // show Key signature
    else if ( sName == _T("SHOW_KEY") ) {
        int nValue;
        bool fOK = tag.GetParamAsInt(_T("VALUE"), &nValue);
        if (!fOK)
            m_sParamErrors += wxString::Format(
                _T("Invalid param value in:\n<param %s >\nAcceptable values: 1 | 0 \n"),
                tag.GetAllParams().c_str() );
        else
            m_pConstrains->SetDisplayKey( nValue != 0 );
    }

    // play mode
    else if ( sName == _T("PLAY_MODE") ) {
        wxString sMode = tag.GetParam(_T("VALUE"));
        if (sMode == _T("chord"))
            m_pConstrains->SetModeAllowed(0, true);
        else if (sMode == _T("ascending"))
            m_pConstrains->SetModeAllowed(1, true);
        else if (sMode == _T("descending"))
            m_pConstrains->SetModeAllowed(2, true);
        else {
            m_sParamErrors += wxString::Format( wxGetTranslation(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values: 'chord | ascending | descending'\n")),
                tag.GetAllParams().c_str(), sMode.c_str() );
        }
    }

    // chords      Keyword "all" or a list of allowed chords:
    else if ( sName == _T("CHORDS") ) {
        wxString sClef = tag.GetParam(_T("VALUE"));
        m_sParamErrors += ParseChords(tag.GetParam(_T("VALUE")), tag.GetAllParams(),
                                    m_pConstrains->GetValidChords());
    }

    //keys        keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
    else if ( sName == _T("KEYS") ) {
        m_sParamErrors += ParseKeys(tag.GetParam(_T("VALUE")), tag.GetAllParams(),
                                    m_pConstrains->GetKeyConstrains());
    }

    // Unknown param
    else
        lmExerciseParams::AddParam(tag);


}

void lmIdfyChordCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    // ensure that at least a play mode is selected
    bool fModeSpecified = false;
    for (int i=0; i < 3; i++) {
        fModeSpecified = fModeSpecified || m_pConstrains->IsModeAllowed(i);
        if (fModeSpecified) break;
    }
    if (!fModeSpecified) {
        m_pConstrains->SetModeAllowed(0, true);     //harmonic
    }

    //inform about param errors or create the control
    wxWindow* pWnd;
    if (m_sParamErrors != _T("")) {
        // there are errors: display a text box with the error message
        pWnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1, m_sParamErrors,
            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
    }
    else {
        // create the IdfyChordCtrol
        pWnd = new lmIdfyChordCtrol((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1,
            m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    }
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(pWnd, m_nPercent));
    //pWnd->Show(true);

}

#endif  // __LM_IDFYCHORDCTROLPARMS_H__
