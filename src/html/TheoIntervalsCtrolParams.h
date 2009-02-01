//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#ifndef __LM_THOEINTERVALSCTROLPARAMS_H__        //to avoid nested includes
#define __LM_THOEINTERVALSCTROLPARAMS_H__

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
#include "../exercises/TheoIntervalsConstrains.h"


// This class pack all parameters to set up a Theo Intervals Identification exercise,
// The settings must be read/setup by the TheoIntervalsCtrol object.

class lmTheoIntervalsCtrolParms : public lmExerciseParams
{
public:
    lmTheoIntervalsCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmTheoIntervalsCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:

    // html object window attributes
    long    m_nWindowStyle;
    lmTheoIntervalsConstrains* m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmTheoIntervalsCtrolParms)
};


//------------------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------------------

lmTheoIntervalsCtrolParms::lmTheoIntervalsCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmExerciseParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // create the constraints object
    m_pConstrains = new lmTheoIntervalsConstrains(_T("TheoIntervals"));
    m_pOptions = m_pConstrains;

}


lmTheoIntervalsCtrolParms::~lmTheoIntervalsCtrolParms()
{
    //Constrains will be deleted by the Ctrol. DO NOT DELETE IT HERE
    //if (m_pConstrains) delete m_pConstrains;

}

void lmTheoIntervalsCtrolParms::AddParam(const wxHtmlTag& tag)
{
    // problem_level     0 | 1 | 2 | 3                                  [2]
    // accidentals       none | simple | double                         [none]
    // problem_type      DeduceInterval | BuildInterval
    // clef*             G | F4 | F3 | C4 | C3 | C2 | C1                [G]
    // control_settings  Value="[key for storing the settings]"
    //                      By coding this param it is forced the inclusion of
    //                      the 'settings' link. Its value will be used
    //                      as the key for saving the user settings.

    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.MakeUpper();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute

    // accidentals        none | simple | double
    if ( sName == _T("ACCIDENTALS") ) {
        wxString sAccidentals = tag.GetParam(_T("VALUE"));
        sAccidentals.MakeUpper();
        if (sAccidentals == _T("NONE")) {
            m_pConstrains->SetAccidentals(false);
            m_pConstrains->SetDoubleAccidentals(false);
        }
        else if (sAccidentals == _T("SIMPLE"))
            m_pConstrains->SetAccidentals(true);
        else if (sAccidentals == _T("DOUBLE"))
            m_pConstrains->SetDoubleAccidentals(true);
        else
            LogError(wxString::Format(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values: none | simple | double"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() ));
    }

    //problem_type    DeduceInterval | BuildInterval
    else if ( sName == _T("PROBLEM_TYPE") )
    {
        wxString sProblem = tag.GetParam(_T("VALUE"));
        sProblem.MakeUpper();
        if (sProblem == _T("DEDUCEINTERVAL"))
            m_pConstrains->SetProblemType( ePT_DeduceInterval );
        else if (sProblem == _T("BUILDINTERVAL"))
            m_pConstrains->SetProblemType( ePT_BuildInterval );
        else
            LogError(wxString::Format(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values: DeduceInterval | BuildInterval"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() ));
    }

    //problem_level     0 | 1 | 2 | 3          
    else if ( sName == _T("PROBLEM_LEVEL") )
    {
        int nLevel;
        bool fOK = tag.GetParamAsInt(_T("VALUE"), &nLevel);
        if (!fOK || nLevel < 0 || nLevel > 3)
            LogError(wxString::Format(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values: 0 | 1 | 2 | 3"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() ));
        else
            m_pConstrains->SetProblemLevel( nLevel );
    }

    // clef        G | F4 | F3 | C4 | C3 | C2 | C1
    else if ( sName == _T("CLEF") ) {
        wxString sClef = tag.GetParam(_T("VALUE"));
        lmEClefType nClef = LDPNameToClef(sClef);
        if (nClef != -1)
            m_pConstrains->SetClef(nClef, true);
        else
            LogError(wxString::Format(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values: G | F4 | F3 | C4 | C3 | C2 | C1"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() ));
    }

    // Unknown param
    else
        lmExerciseParams::AddParam(tag);

}

void lmTheoIntervalsCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    // ensure that at least a Clef is selected
    bool fClefSpecified = false;
    for (int i=lmMIN_CLEF; i <= lmMAX_CLEF; i++) {
        fClefSpecified = fClefSpecified || m_pConstrains->IsValidClef((lmEClefType)i);
        if (fClefSpecified) break;
    }
    if (!fClefSpecified) {
        m_pConstrains->SetClef(lmE_Sol, true);
    }

    // create the window
    wxWindow* wnd;
    if (m_pConstrains->GetProblemType() == ePT_BuildInterval)
        wnd = new lmBuildIntervalCtrol((wxWindow*)g_pMainFrame->GetHtmlWindow(), -1,
                                       m_pConstrains, wxPoint(0,0), 
                                       wxSize(m_nWidth, m_nHeight), m_nWindowStyle);
    else
        wnd = new lmIdfyIntervalCtrol((wxWindow*)g_pMainFrame->GetHtmlWindow(), -1,
                                       m_pConstrains, wxPoint(0,0), 
                                       wxSize(m_nWidth, m_nHeight), m_nWindowStyle);
    wnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(wnd, m_nPercent));
}



#endif  // __LM_THOEINTERVALSCTROLPARAMS_H__
