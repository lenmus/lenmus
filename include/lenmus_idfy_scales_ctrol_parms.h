//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

//#ifndef __LENMUS_IDFYSCALESCTROLPARMS_H__        //to avoid nested includes
//#define __LENMUS_IDFYSCALESCTROLPARMS_H__
//
//// For compilers that support precompilation, includes <wx/wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#ifndef WX_PRECOMP
//#include <wx/wx.h>
//#endif
//
//#include "ObjectParams.h"
//#include "../ldp_parser/AuxString.h"
//#include "ParamsParser.h"
//#include "../exercises/ScalesConstrains.h"
//
//
////! This class pack all parameters to set up a Scale Identification exercise,
////! The settings must be read/setup by the IdfyScalesCtrol object.
//
//class lmIdfyScalesCtrolParms : public lmExerciseParams
//{
//public:
//    lmIdfyScalesCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
//                              int nPercent, long nStyle);
//    ~lmIdfyScalesCtrolParms();
//
//    void AddParam(const wxHtmlTag& tag);
//    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);
//
//protected:
//
//        // Member variables:
//
//    // html object window attributes
//    long                    m_nWindowStyle;
//    lmScalesConstrains*     m_pConstrains;
//
//    DECLARE_NO_COPY_CLASS(lmIdfyScalesCtrolParms)
//};
//
//
//
//lmIdfyScalesCtrolParms::lmIdfyScalesCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
//                                   int nPercent, long nStyle)
//    : lmExerciseParams(tag, nWidth, nHeight, nPercent)
//{
//
//    // html object window attributes
//    m_nWindowStyle = nStyle;
//
//    // construct constraints object
//    m_pConstrains = new lmScalesConstrains(_T("IdfyScale"));
//    m_pOptions = m_pConstrains;
//
//    // initializations
//    m_sParamErrors = _T("");    //no errors
//
//}
//
//
//lmIdfyScalesCtrolParms::~lmIdfyScalesCtrolParms()
//{
//    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
//    //IF THE CONTROL HAS BEEN CREATED
//    if (m_sParamErrors != _T("")) {
//        if (m_pConstrains) delete m_pConstrains;
//    }
//
//}
//
//void lmIdfyScalesCtrolParms::AddParam(const wxHtmlTag& tag)
//{
//    /*! @page IdfyScalesCtrolParams
//        @verbatim
//
//        Params for lmIdfyChordCtrol - html object type="Application/LenMusIdfyChord"
//
//        keys        Keyword "all", "allMajor", "allMinor" or a list of allowed
//                    key signatures, i.e.: "Do,Fas". Default: all
//                    AWARE: if major and minor scales selected, the chosen key signatures
//                    will be automatically transformed into major/minor, even if it is
//                    not specified in the keys list. For example, <keys>C</keys> will
//                    transform C major into A minor for minor scales.
//
//        scales      Keyword "all" or a list of allowed scales:
//                        major: MN (natural), MH (harmonic), M3 (type III), MM (mixolydian)
//                        minor: mN (natural), mM (melodic), mD (dorian), mH (harmonic)
//                        medieval modes: Do (Dorian), Ph (Phrygian), Ly (Lydian),
//                                        Mx (Mixolydian), Ae (Aeolian), Io (Ionian),
//                                        Lo (Locrian)
//                        other: Pm (Pentatonic minor), PM (Pentatonic Major), Bl (Blues)
//                        non-tonal: WT (Whole Tones), Ch (Chromatic)
//
//
//                    Default: "MN, mN, mH, mM"
//
//        mode        'theory' | 'earTraining'  Keyword indicating type of exercise
//
//        play_mode   'ascending | descending | both' allowed play modes. Default: ascending
//
//        show_key     '0 | 1' Default: 0 (do not display key signature)
//
//        control_settings    Value="[key for storing the settings]"
//                            By coding this param it is forced the inclusion of
//                            the 'settings' link. Its value will be used
//                            as the key for saving the user settings.
//
//        Example:
//        ------------------------------------
//        <exercise type="IdfyScales" width="100%" height="300" border="0">
//            <control_go_back>th1_140_scales</control_go_back>
//            <keys>all</keys>
//            <scales>MN, mN</scales>
//            <mode>theory</mode>
//        </exercise>
//
//    */
//
//    wxString sName = wxEmptyString;
//    wxString sValue = wxEmptyString;
//
//    // scan name and value
//    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
//    sName = tag.GetParam(_T("NAME"));
//    sName.MakeUpper();        //convert to upper case
//
//    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute
//
//    // show Key signature
//    else if ( sName == _T("SHOW_KEY") ) {
//        int nValue;
//        bool fOK = tag.GetParamAsInt(_T("VALUE"), &nValue);
//        if (!fOK)
//            m_sParamErrors += wxString::Format(
//                _T("Invalid param value in:\n<param %s >\nAcceptable values: 1 | 0 \n"),
//                tag.GetAllParams().c_str() );
//        else
//            m_pConstrains->SetDisplayKey( nValue != 0 );
//    }
//
//    // play mode
//    else if ( sName == _T("PLAY_MODE") ) {
//        wxString sMode = tag.GetParam(_T("VALUE"));
//        if (sMode == _T("ascending"))
//            m_pConstrains->SetPlayMode(0);
//        else if (sMode == _T("descending"))
//            m_pConstrains->SetPlayMode(1);
//        else if (sMode == _T("both"))
//            m_pConstrains->SetPlayMode(2);
//        else {
//            m_sParamErrors += wxString::Format(
//                _T("Invalid param value in:\n<param %s >\n")
//                _T("Invalid value = %s \n")
//                _T("Acceptable values: 'ascending | descending | both'\n"),
//                tag.GetAllParams().c_str(), sMode.c_str() );
//        }
//    }
//
//    // scales      Keyword "all" or a list of allowed scales:
//    else if ( sName == _T("SCALES") ) {
//        wxString sClef = tag.GetParam(_T("VALUE"));
//        m_sParamErrors += lmParseScales(tag.GetParam(_T("VALUE")), tag.GetAllParams(),
//                                    m_pConstrains->GetValidScales());
//    }
//
//    //keys        keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
//    else if ( sName == _T("KEYS") ) {
//        m_sParamErrors += lmParseKeys(tag.GetParam(_T("VALUE")), tag.GetAllParams(),
//                                    m_pConstrains->GetKeyConstrains());
//    }
//
//    // Unknown param
//    else
//        lmExerciseParams::AddParam(tag);
//
//
//}
//
//void lmIdfyScalesCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
//{
//    //inform about param errors or create the control
//    wxWindow* pWnd;
//    if (m_sParamErrors != _T("")) {
//        // there are errors: display a text box with the error message
//        pWnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1, m_sParamErrors,
//            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
//    }
//    else {
//        // create the IdfyScalesCtrol
//        pWnd = new lmIdfyScalesCtrol((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1,
//            m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
//    }
//    pWnd->Show(true);
//    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(pWnd, m_nPercent));
//
//}
//
//#endif  // __LENMUS_IDFYSCALESCTROLPARMS_H__
