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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ObjectParams.h"
#endif

// For compilers that support precompilation, includes <wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <wx/html/winpars.h>
#include <wx/html/htmlwin.h>

#include "../exercises/Constrains.h"
#include "ObjectParams.h"


//---------------------------------------------------------------------------------------
// Implementation of lmEBookCtrolParams
//---------------------------------------------------------------------------------------

lmEBookCtrolParams::lmEBookCtrolParams(const wxHtmlTag& tag, int nWidth, int nHeight, int nPercent)
{
    // html object window attributes
    m_nWidth = nWidth;
    m_nHeight = nHeight;
    m_nPercent = nPercent;

}

void lmEBookCtrolParams::LogError(const wxString& sMsg)
{
    //TODO do something else with the error
    wxLogMessage(sMsg);
}

void lmEBookCtrolParams::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    wxWindow *wnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1,
            _T("Default <object> window: you MUST implement this virtual method!"),
            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
    wnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(wnd, m_nPercent));

}

void lmEBookCtrolParams::AddParam(const wxHtmlTag& tag)
{
    // Parse common parameters to all controls (lmEBookCtrolOptions)
    //
    // control_play        Include 'play' link. Default: do not include it.
    //                     Value="play label|stop playing label". i.e.: "Play|Stop"
    //                     Stop label is optional. Default labels: "Play|Stop"
    //
    // control_settings    Value="[key for storing the settings]"
    //                     By coding this param it is forced the inclusion of
    //                     the 'settings' link. Its value will be used
    //                     as the key for saving the user settings.
    // control_go_back     Include a 'Go back to theory' link. Value is an URL,
    //                     i.e.: "v2_L2_MusicReading_203.htm"
    //


    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    wxString sName = tag.GetParam(_T("NAME"));
    sName.MakeUpper();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute

    // control_settings
    if ( sName == _T("CONTROL_SETTINGS") ) {
        m_pOptions->SetSettingsLink(true);
        m_pOptions->SetSection( tag.GetParam(_T("VALUE")) );
    }

    // "Go back to theory" link
    else if ( sName == _T("CONTROL_GO_BACK") ) {
        m_pOptions->SetGoBackLink( tag.GetParam(_T("VALUE") ));
    }

    // control_play
    else if ( sName == _T("CONTROL_PLAY") ) {
        m_pOptions->SetPlayLink(true);      //, tag.GetParam(_T("VALUE")) );
    }

    // Unknown param
    else
        LogError(wxString::Format(
            _T("lmEBookCtrolParams. Unknown param: <param %s >\n"),
            tag.GetAllParams().c_str() ));

}

//---------------------------------------------------------------------------------------
// Implementation of lmExerciseParams
//---------------------------------------------------------------------------------------

lmExerciseParams::lmExerciseParams(const wxHtmlTag& tag, int nWidth, int nHeight, int nPercent)
    : lmEBookCtrolParams(tag, nWidth, nHeight, nPercent)
{
}

void lmExerciseParams::AddParam(const wxHtmlTag& tag)
{
    // Parse common parameters to all lmExerciseParams
    //
    // mode                'theory' | 'earTraining'  Keyword indicating type of exercise
    //

    lmExerciseOptions* pOptions = (lmExerciseOptions*)m_pOptions;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    wxString sName = tag.GetParam(_T("NAME"));
    sName.MakeUpper();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute

    // mode        'theory | earTraining'  Keyword indicating type of exercise
    if ( sName == _T("MODE") ) {
        wxString sMode = tag.GetParam(_T("VALUE"));
        if (sMode == _T("theory"))
            pOptions->SetTheoryMode(true);
        else if (sMode == _T("earTraining"))
            pOptions->SetTheoryMode(false);
        else {
            m_sParamErrors += wxString::Format(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values:  'theory | earTraining'\n"),
                tag.GetAllParams().c_str(), sMode.c_str() );
        }
    }

    // Unknown param
    else
        lmEBookCtrolParams::AddParam(tag);

}





