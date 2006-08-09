//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file IdfyChordCtrolParms.h
    @brief Header file for class lmIdfyChordCtrolParms
    @ingroup html_controls
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __IDFYCHORDCTROLPARMS_H__        //to avoid nested includes
#define __IDFYCHORDCTROLPARMS_H__

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

/*! This class pack all parameters to set up a Side Reading exercise.
    The contained lmScoreConstrains object has the constrains for the 'ByProgram'
    settings mode (default mode). For other modes ('UserSettings' and 'ReadingNotes')
    the settings must be read/setup by the IdfyChordCtrol object.
*/
class lmIdfyChordCtrolParms : public lmObjectParams
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
    wxString                m_sParamErrors;
    lmChordConstrains*      m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmIdfyChordCtrolParms)
};



lmIdfyChordCtrolParms::lmIdfyChordCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmObjectParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // construct constrains object
    m_pConstrains = new lmChordConstrains(_T("EarChord"));

    // initializations
    m_sParamErrors = _T("");    //no errors

}


lmIdfyChordCtrolParms::~lmIdfyChordCtrolParms()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != _T("")) {
        if (m_pConstrains) delete m_pConstrains;
        //if (m_pOptions) delete m_pOptions;
    }

}

void lmIdfyChordCtrolParms::AddParam(const wxHtmlTag& tag)
{
    /*! @page EarChordCtrolParams
        @verbatim    

        Params for lmIdfyChordCtrol - html object type="Application/LenMusEarChord"


        @endverbatim

    */

    /*
    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.UpperCase();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute
    
    // control_play
    if ( sName == _T("CONTROL_PLAY") ) {
        m_pOptions->SetControlPlay(true, tag.GetParam(_T("VALUE")) );
    }

    // control_solfa
    else if ( sName == _T("CONTROL_SOLFA") ) {
        m_pOptions->SetControlSolfa(true, tag.GetParam(_T("VALUE")) );
    }

    // "Go back to theory" link
    else if ( sName == _T("CONTROL_GO_BACK") ) {
        m_pOptions->SetGoBackURL( tag.GetParam(_T("VALUE") ));
    }

    // control_settings
    else if ( sName == _T("CONTROL_SETTINGS") ) {
        m_pOptions->SetControlSettings(true, tag.GetParam(_T("VALUE")) );
        m_pConstrains->SetSection( tag.GetParam(_T("VALUE") ));
    }

    //fragments   the list of fragmens to use
    //  <param name="fragment" value="68,98;(n * n)(n * c +l)(g (n * c)(n * c)(n * c))">
	//  <param name="fragment" value="68,98;(n * c)(n * n +l)(g (n * c)(n * c)(n * c))">
	//  <param name="fragment" value="68,98;(n * n)(n * c)">
	//  <param name="fragment" value="68,98;(g (n * c)(n * c)(n * c))">

    else if ( sName == _T("FRAGMENT") ) {
        wxString sFragments = tag.GetParam(_T("VALUE"));
        AnalyzeFragments(sFragments);
    }

    //clef*        one param for each allowed clef. It includes the pitch scope.
    //  <param name="clef" value="Sol;a3;a5" />
    //  <param name="clef" value="Fa4;a2;e4" />

    else if ( sName == _T("CLEF") ) {
        wxString sClef = tag.GetParam(_T("VALUE"));
        if (AnalyzeClef(sClef)) {
            m_sParamErrors += wxString::Format( wxGetTranslation(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable format: <Clef,LowerNote,UpperNote> \n")
                _T("Acceptable clef values: Sol | Fa4 | Fa3 | Do4 | Do3 | Do2 | Do1 \n")
                _T("Acceptable note pitch: c0 - c9")),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) );
        }
    }

    //time        a list of allowed time signatures, i.e.: "68,98,128"
    //    <param name="time" value="68,98,128">

    else if ( sName == _T("TIME") ) {
        wxString sTime = tag.GetParam(_T("VALUE"));
        if (AnalyzeTime(sTime)) {
            m_sParamErrors += wxString::Format( wxGetTranslation(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable format: list of time signatures \n")),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) );
        }
    }

    //key         keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
    //    <param name="key" value="all">

    else if ( sName == _T("KEY") ) {
        wxString sKeys = tag.GetParam(_T("VALUE"));
        if (AnalyzeKeys(sKeys)) {
            m_sParamErrors += wxString::Format( wxGetTranslation(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable format: list of key signatures or keyword 'all' \n")),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) );
        }
    }

    //maxGroupInterval    a number
    else if ( sName == _T("MAXINTERVAL") ) {
        wxString sMaxInterval = tag.GetParam(_T("VALUE"));
        long nMaxInterval;
        bool fOK = sMaxInterval.ToLong(&nMaxInterval);
        if (!fOK || nMaxInterval < 0 ) {
            m_sParamErrors += wxString::Format( wxGetTranslation(
                _T("Invalid param value in:/n<param %s >/n")
                _T("Invalid value = %s /n")
                _T("Acceptable values: numeric, greater than 0\n") ),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) );
        }
        else {
            m_pConstrains->SetMaxInterval((int)nMaxInterval);
        }
    }

    // Unknown param
    else
        m_sParamErrors += wxString::Format( 
            _("lmIdfyChordCtrol. Unknown param: <param %s >\n"),
            tag.GetAllParams() );

    */
}

void lmIdfyChordCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    //verify that all necessary html params has been specified
    wxWindow* pWnd;
    //m_sParamErrors += m_pConstrains->Verify();
    if (m_sParamErrors != _T("")) {
        // there are errors: display a text box with the error message
        pWnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindow(), -1, m_sParamErrors,
            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
    }
    else {
        // create the IdfyChordCtrol
        pWnd = new lmIdfyChordCtrol((wxWindow*)pHtmlParser->GetWindow(), -1, 
            m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    }
    pWnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(pWnd, m_nPercent));

}

#endif  // __IDFYCHORDCTROLPARMS_H__
