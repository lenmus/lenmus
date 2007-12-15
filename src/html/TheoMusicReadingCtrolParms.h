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

#ifndef __THEOMUSICREADINGCTROLPARAMS_H__        //to avoid nested includes
#define __THEOMUSICREADINGCTROLPARAMS_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/html/htmlwin.h"
#include "ObjectParams.h"
#include "../exercises/ScoreConstrains.h"
#include "../exercises/MusicReadingConstrains.h"
#include "../ldp_parser/AuxString.h"

//! enum assigning name to the score generation settings source.
/*
    Four different ways for choosing the settings:

    1. By level and lesson: Useful for what? It was necessary with old non-html
        organization. Now for each lesson there will be an html page and the
        score object will have all necessary information for that lesson. So method
        3 will replace this one.

    2. Personal settings dialog: the user selects the rithmic patterns to generate.
        Valid only to generate simple repetitive patterns. Composer must have
        knowledge for completing bars with rest or single notes.

    3. Pararameters in html object: the parameters must include all necesary data.
        Score generation based only on received fragments.

    4. Reading notes exercises: the parameters are fixed for this exercise (parameters
        the html object) but certain values (clefs, notes range) would need
        user settings

    Free exercises page will be based only on method 2 in coherence with its purpose
    (free exercises, user customizable). For practising an specific level/lesson the
    user must choose the corresponding book/page. In these pages it would be allowed
    to customize settings by adding/removing fragments or changing clefs and note ranges.

*/

// This class pack all parameters to set up a Music Reading exercise.
// The contained lmScoreConstrains object has the constraints for the 'ByProgram'
// settings mode (default mode). For other modes ('UserSettings' and 'ReadingNotes')
// the settings must be read/setup by the TheoMusicReadingCtrol object.

class lmTheoMusicReadingCtrolParms : public lmExerciseParams
{
public:
    lmTheoMusicReadingCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmTheoMusicReadingCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

    // Options for Music Reading Ctrol
    void SetControlPlay(bool fValue, wxString sLabels = _T(""))
        {
            fPlayCtrol = fValue;
            if (sLabels != _T(""))
                SetLabels(sLabels, &sPlayLabel, &sStopPlayLabel);
        }
    void SetControlSolfa(bool fValue, wxString sLabels = _T(""))
        {
            fSolfaCtrol = fValue;
            if (sLabels != _T(""))
                SetLabels(sLabels, &sSolfaLabel, &sStopSolfaLabel);
        }


    bool        fPlayCtrol;             //Instert "Play" link
    wxString    sPlayLabel;             //label for "Play" link
    wxString    sStopPlayLabel;         //label for "Stop playing" link

    bool        fSolfaCtrol;            //insert a "Sol-fa" link
    wxString    sSolfaLabel;            //label for "Sol-fa" link
    wxString    sStopSolfaLabel;        //label for "Stop sol-fa" link

    bool        fBorder;


protected:
    bool AnalyzeClef(wxString sLine);
    bool AnalyzeTime(wxString sLine);
    bool AnalyzeKeys(wxString sLine);
    bool AnalyzeFragments(wxString sLine);

    void SetLabels(wxString& sLabel, wxString* pStart, wxString* pStop);

        // Member variables:

    // html object window attributes
    long                        m_nWindowStyle;
    lmScoreConstrains*          m_pScoreConstrains;
    lmMusicReadingConstrains*   m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmTheoMusicReadingCtrolParms)

};



lmTheoMusicReadingCtrolParms::lmTheoMusicReadingCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmExerciseParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // construct constraints object
    m_pScoreConstrains = new lmScoreConstrains();

    // object options
    m_pConstrains = new lmMusicReadingConstrains(_T("MusicReading"));
    m_pOptions = m_pConstrains;


    // initializations
    m_sParamErrors = _T("");    //no errors

    // control options initializations
    fPlayCtrol = false;
    fSolfaCtrol = false;
    fBorder = false;
    sPlayLabel = _("Play");
    sStopPlayLabel = _("Stop");
    sSolfaLabel = _("Read");
    sStopSolfaLabel = _("Stop");


}


lmTheoMusicReadingCtrolParms::~lmTheoMusicReadingCtrolParms()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != _T("")) {
        if (m_pScoreConstrains) delete m_pScoreConstrains;
        if (m_pConstrains) delete m_pConstrains;
    }

}

void lmTheoMusicReadingCtrolParms::AddParam(const wxHtmlTag& tag)
{
    /*! @page MusicReadingCtrolParams
        @verbatim

        Params for lmScoreCtrol - html object type="Application/LenMusTheoMusicReading"


        optional params to include controls: (not yet implemented marked as [])
        --------------------------------------

        control_play    Include 'play' link. Default: do not include it.
                        Value="play label|stop playing label". i.e.: "Play|Stop" Stop label
                        is optional. Default labels: "Play|Stop"

        control_solfa   Include 'solfa' link. Default: do not include it.
                        Value="music read label|stop music reading label". i.e.:
                        "Play|Stop". Stop label is optional.
                        Default labels: "Read|Stop"

        control_settings    Value="[key for storing the settings]"
                            This param forces to include the 'settings' link. The
                            key will be used both as the key for saving the user settings
                            and as a tag to select the Setting Dialog options to allow.

        control_go_back    URL, i.e.: "v2_L2_MusicReading_203.htm"

        metronome       Set a fixed metronome rate to play this piece of music
                        Value="MM number". Default: user value in metronome control


        params to set up the score composer:
        ------------------------------------

        fragment*   one param for each fragment to use

        clef*       one param for each allowed clef. It includes the pitch scope.

        time        a list of allowed time signatures, i.e.: "68,98,128"

        key         keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"

        max_interval    a number indicating the maximum allowed interval for two consecutive notes
                        Default: 4

        @todo: update the example
        Example:
        ------------------------------------

        <object type="Application/LenMusTheoMusicReading" width="100%" height="300" border="0">
            <param name="control" value="play">
            <param name="label_play" value="Play|Stop">
            <param name="control" value="solfa">
            <param name="mode" value="PersonalSettings">
            <param name="mode" value="NotesReading">
            <param name="fragment" value="68,98;(n * n)(n * c +l)(g (n * c)(n * c)(n * c))">
	        <param name="fragment" value="68,98;(n * c)(n * n +l)(g (n * c)(n * c)(n * c))">
	        <param name="fragment" value="68,98;(n * n)(n * c)">
	        <param name="fragment" value="68,98;(g (n * c)(n * c)(n * c))">
            <param name="clef" value="Sol;a3;a5">
            <param name="clef" value="Fa4;a2;e4">
            <param name="time" value="68,98,128">
            <param name="key" value="all">
            <param name="max_interval" value="4">
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

    // control_solfa
    else if ( sName == _T("CONTROL_SOLFA") ) {
        m_pConstrains->SetControlSolfa(true, tag.GetParam(_T("VALUE")) );
    }

    // metronome
    else if ( sName == _T("METRONOME") ) {
        wxString sMM = tag.GetParam(_T("VALUE"));
        long nMM;
        bool fOK = sMM.ToLong(&nMM);
        if (!fOK || nMM < 0 ) {
            m_sParamErrors += wxString::Format(
_T("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: numeric, greater than 0\n"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() );
        }
        else {
            m_pScoreConstrains->SetMetronomeMM(nMM);
        }
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
            m_sParamErrors += wxString::Format(
_T("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable format: <Clef,LowerNote,UpperNote> \n \
Acceptable clef values: Sol | Fa4 | Fa3 | Do4 | Do3 | Do2 | Do1 \n \
Acceptable note pitch: c0 - c9"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() );
        }
    }

    //time        a list of allowed time signatures, i.e.: "68,98,128"
    //    <param name="time" value="68,98,128">

    else if ( sName == _T("TIME") ) {
        wxString sTime = tag.GetParam(_T("VALUE"));
        if (AnalyzeTime(sTime)) {
            m_sParamErrors += wxString::Format(
_T("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable format: list of time signatures \n"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() );
        }
    }

    //key         keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
    //    <param name="key" value="all">

    else if ( sName == _T("KEY") ) {
        wxString sKeys = tag.GetParam(_T("VALUE"));
        if (AnalyzeKeys(sKeys)) {
            m_sParamErrors += wxString::Format(
_T("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable format: list of key signatures or keyword 'all' \n"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() );
        }
    }

    //maxGroupInterval    a number
    else if ( sName == _T("MAX_INTERVAL") ) {
        wxString sMaxInterval = tag.GetParam(_T("VALUE"));
        long nMaxInterval;
        bool fOK = sMaxInterval.ToLong(&nMaxInterval);
        if (!fOK || nMaxInterval < 0 ) {
            m_sParamErrors += wxString::Format(
_T("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: numeric, greater than 0\n"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() );
        }
        else {
            m_pScoreConstrains->SetMaxInterval((int)nMaxInterval);
        }
    }

    // Unknown param
    else
        lmExerciseParams::AddParam(tag);

}

void lmTheoMusicReadingCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    //The <object> tag has been read. If param 'control_settings' has been specified
    // configuration values must be loaded from the specified section key
    m_pScoreConstrains->LoadSettings();

    //verify that all necessary html params has been specified
    wxWindow* pWnd;
    m_sParamErrors += m_pScoreConstrains->Verify();
    if (m_sParamErrors != _T("")) {
        // there are errors: display a text box with the error message
        pWnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1, m_sParamErrors,
            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
    }
    else {
        // create the TheoMusicReadingCtrol
        m_pConstrains->SetScoreConstrains(m_pScoreConstrains);
        pWnd = new lmTheoMusicReadingCtrol((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1,
            m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    }
    pWnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(pWnd, m_nPercent));

}

/// returns true if error
bool lmTheoMusicReadingCtrolParms::AnalyzeClef(wxString sLine)
{
    //i.e.: "Sol;c4;c5"

    //get clef
    int iSemicolon = sLine.Find(_T(";"));
    wxString sClef = sLine.Left(iSemicolon);
    lmEClefType nClef = LDPNameToClef(sClef);
    if (nClef == (lmEClefType)-1) return true;    //error

    //get lower scope
    sLine = sLine.substr(iSemicolon+1);
    iSemicolon = sLine.Find(_T(";"));
    wxString sLowerScope = sLine.Left(iSemicolon);

    //get upper scope
    wxString sUpperScope = sLine.substr(iSemicolon + 1);

    //Update information for this clef
    lmClefConstrain* pClefs = m_pScoreConstrains->GetClefConstrains();
    pClefs->SetValid(nClef, true);
    pClefs->SetLowerPitch(nClef, sLowerScope);
    pClefs->SetUpperPitch(nClef, sUpperScope);

    return false;   //no error

}

/// returns true if error
bool lmTheoMusicReadingCtrolParms::AnalyzeTime(wxString sLine)
{
    //i.e.: "98" , "38,68,128" , "24,44"

    //build time signatures constraints object
    lmTimeSignConstrains* pTimeSigns = new lmTimeSignConstrains();
    if (pTimeSigns->SetConstrains(sLine)) return true;

    //Replace information about allowed time signatures
    lmTimeSignConstrains* pOldTimeSigns = m_pScoreConstrains->GetTimeSignConstrains();
    int i;
    lmETimeSignature nTime;
    for (i=lmMIN_TIME_SIGN; i <= lmMAX_TIME_SIGN; i++) {
        nTime = (lmETimeSignature)i;
        pOldTimeSigns->SetValid(nTime, pTimeSigns->IsValid(nTime) );
    }
    delete pTimeSigns;

    return false;   //no error

}

/// returns true if error
bool lmTheoMusicReadingCtrolParms::AnalyzeKeys(wxString sLine)
{
    //i.e.: "all" , "do,sol,fa,"

    if (sLine == _T("all")) {
        // allow all key signatures
        lmKeyConstrains* pKeys = m_pScoreConstrains->GetKeyConstrains();
        int i;
        for (i=0; i <= earmFa; i++) {
            pKeys->SetValid((lmEKeySignatures)i, true);
        }
    }

    else {
        //analyze and set key signatures
        lmKeyConstrains* pKeys = m_pScoreConstrains->GetKeyConstrains();

        //loop to get all keys
        int iColon;
        wxString sKey;
        lmEKeySignatures nKey;
        while (sLine != _T("")) {
            //get key
            iColon = sLine.Find(_T(","));
            if (iColon != -1) {
                sKey = sLine.Left(iColon);
                sLine = sLine.substr(iColon + 1);      //skip the colon
            }
            else {
                sKey = sLine;
                sLine = _T("");
            }
            nKey = LDPInternalNameToKey(sKey);
            if (nKey == (lmEKeySignatures)-1) return true;
            pKeys->SetValid(nKey, true);
        }
    }

    return false;   //no error

}

/// returns true if error
bool lmTheoMusicReadingCtrolParms::AnalyzeFragments(wxString sLine)
{
    //i.e.: "24,34;(s c)(n * n)(n * s g+)(n * s )(n * c g-)(s c)"

    //get time signatures
    int iSemicolon = sLine.Find(_T(";"));
    wxString sTimeSign = sLine.Left(iSemicolon);

    //get fragment
    wxString sFragment = sLine.substr(iSemicolon + 1);   //skip the semicolon and take the rest

    //build time signatures constraints object
    lmTimeSignConstrains* pTimeSigns = new lmTimeSignConstrains();
    if (pTimeSigns->SetConstrains(sTimeSign)) {
        m_sParamErrors += wxString::Format(_T("Error in fragment. Invalid time signature list '%s'\nIn fragment: '%s'\n"),
                sTimeSign.c_str(), sFragment.c_str());
        return true;
     }
    // verify fragment to avoid program failures
    lmLDPParser parserLDP;
    //TODO strong verification
    //lmLDPNode* pRoot = parserLDP.ParseText(sFragment);
    //if (! pRoot) {
    if (!parserLDP.ParenthesisMatch(sFragment)) {
        m_sParamErrors += wxString::Format(_T("Invalid fragment. Unmatched parenthesis: '%s'\n"),
                                sFragment.c_str());
        return true;
     }
    if (sFragment.Find(_T("(g (")) != -1 || sFragment.Find(_T("(g(")) != -1) {
        m_sParamErrors += wxString::Format(_T("Invalid fragment. Old G syntax: '%s'\n"),
                                sFragment.c_str());
        return true;
     }

    // build the entry
    m_pScoreConstrains->AddFragment(pTimeSigns, sFragment);

    return false;   //no error

}

#endif  // __THEOMUSICREADINGCTROLPARAMS_H__
