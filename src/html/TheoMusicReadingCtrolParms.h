// RCS-ID: $Id: TheoMusicReadingCtrolParms.h,v 1.14 2006/03/03 15:01:11 cecilios Exp $
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
/*! @file TheoMusicReadingCtrolParms.h
    @brief Header file for class lmTheoMusicReadingCtrolParms
    @ingroup html_controls
*/
#ifdef __GNUG__
// #pragma interface
#endif

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

#include "ObjectParams.h"
#include "../exercises/ScoreConstrains.h"
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

/*! This class pack all parameters to set up a Music Reading exercise.
    The contained lmScoreConstrains object has the constrains for the 'ByProgram'
    settings mode (default mode). For other modes ('UserSettings' and 'ReadingNotes')
    the settings must be read/setup by the TheoMusicReadingCtrol object.
*/
class lmTheoMusicReadingCtrolParms : public lmObjectParams
{
public:
    lmTheoMusicReadingCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmTheoMusicReadingCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:
    bool AnalyzeClef(wxString sLine);
    bool AnalyzeTime(wxString sLine);
    bool AnalyzeKeys(wxString sLine);
    bool AnalyzeFragments(wxString sLine);

        // Member variables:

    // html object window attributes
    long                        m_nWindowStyle;
    lmScoreConstrains*          m_pConstrains;
    lmMusicReadingCtrolOptions*  m_pOptions;
    wxString                    m_sParamErrors;

    DECLARE_NO_COPY_CLASS(lmTheoMusicReadingCtrolParms)
};



lmTheoMusicReadingCtrolParms::lmTheoMusicReadingCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmObjectParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // construct constrains object
    m_pConstrains = new lmScoreConstrains();

    // object options
    m_pOptions = new lmMusicReadingCtrolOptions();

    // initializations
    m_sParamErrors = _T("");    //no errors

}


lmTheoMusicReadingCtrolParms::~lmTheoMusicReadingCtrolParms()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != _T("")) {
        if (m_pConstrains) delete m_pConstrains;
        if (m_pOptions) delete m_pOptions;
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


        params to set up the score composer:
        ------------------------------------

        fragment*   one param for each fragment to use

        clef*       one param for each allowed clef. It includes the pitch scope.

        time        a list of allowed time signatures, i.e.: "68,98,128"

        key         keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"

        maxInterval    a number indicating the maximum allowed interval for two consecutive notes
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
            <param name="maxInterval" value="4">
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
            _("lmTheoMusicReadingCtrol. Unknown param: <param %s >\n"),
            tag.GetAllParams() );

}

void lmTheoMusicReadingCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    //The <object> tag has been read. If param 'control_settings' has been specified
    // configuration values must be loaded from the specified section key
    m_pConstrains->LoadSettings();

    //verify that all necessary html params has been specified
    wxWindow* pWnd;
    m_sParamErrors += m_pConstrains->Verify();
    if (m_sParamErrors != _T("")) {
        // there are errors: display a text box with the error message
        pWnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindow(), -1, m_sParamErrors,
            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
    }
    else {
        // create the TheoMusicReadingCtrol
        pWnd = new lmTheoMusicReadingCtrol((wxWindow*)pHtmlParser->GetWindow(), -1, 
            m_pOptions, m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
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
    EClefType nClef = LDPNameToClef(sClef);
    if (nClef == (EClefType)-1) return true;    //error

    //get lower scope
    sLine = sLine.Mid(iSemicolon+1);
    iSemicolon = sLine.Find(_T(";"));
    wxString sLowerScope = sLine.Left(iSemicolon);

    //get upper scope
    wxString sUpperScope = sLine.Mid(iSemicolon + 1);

    //Update information for this clef
    lmClefConstrain* pClefs = m_pConstrains->GetClefConstrains();
    pClefs->SetValid(nClef, true);
    pClefs->SetLowerPitch(nClef, sLowerScope);
    pClefs->SetUpperPitch(nClef, sUpperScope);

    return false;   //no error

}

/// returns true if error
bool lmTheoMusicReadingCtrolParms::AnalyzeTime(wxString sLine)
{
    //i.e.: "98" , "38,68,128" , "24,44"

    //build time signatures constrains object
    lmTimeSignConstrains* pTimeSigns = new lmTimeSignConstrains();
    if (pTimeSigns->SetConstrains(sLine)) return true;

    //Replace information about allowed time signatures
    lmTimeSignConstrains* pOldTimeSigns = m_pConstrains->GetTimeSignConstrains();
    int i;
    ETimeSignature nTime;
    for (i=lmMIN_TIME_SIGN; i <= lmMAX_TIME_SIGN; i++) {
        nTime = (ETimeSignature)i;
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
        lmKeyConstrains* pKeys = m_pConstrains->GetKeyConstrains();
        int i;
        for (i=0; i <= earmFa; i++) {
            pKeys->SetValid((EKeySignatures)i, true);
        }
    }

    else {
        //analyze and set key signatures
        lmKeyConstrains* pKeys = m_pConstrains->GetKeyConstrains();

        //loop to get all keys
        int iColon;
        wxString sKey;
        EKeySignatures nKey;
        while (sLine != _T("")) {
            //get key
            iColon = sLine.Find(_T(","));
            if (iColon != -1) {
                sKey = sLine.Left(iColon);
                sLine = sLine.Mid(iColon + 1);      //skip the colon
            }
            else {
                sKey = sLine;
                sLine = _T("");
            }
            nKey = LDPNameToKey(sKey);
            if (nKey == (EKeySignatures)-1) return true;
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
    wxString sFragment = sLine.Mid(iSemicolon + 1);   //skip the semicolon and take the rest

    //build time signatures constrains object
    lmTimeSignConstrains* pTimeSigns = new lmTimeSignConstrains();
    if (pTimeSigns->SetConstrains(sTimeSign)) {
        m_sParamErrors += wxString::Format(_("Error in fragment. Invalid time signature list '%s'\nIn fragment: '%s'\n"),
                sTimeSign, sFragment);
        return true;
     }
    // verify fragment to avoid program failures
    lmLDPParser parserLDP;
    //! @todo strong verification
    //lmLDPNode* pRoot = parserLDP.ParseText(sFragment);
    //if (! pRoot) {
    if (!parserLDP.ParenthesisMatch(sFragment)) {
        m_sParamErrors += wxString::Format(_("Invalid fragment. Unmatched parenthesis: '%s'\n"), sFragment);
        return true;
     }
    if (sFragment.Find(_T("(g (")) != -1 || sFragment.Find(_T("(g(")) != -1) {
        m_sParamErrors += wxString::Format(_("Invalid fragment. Old G syntax: '%s'\n"), sFragment);
        return true;
     }

    // build the entry
    m_pConstrains->AddFragment(pTimeSigns, sFragment);

    return false;   //no error

}

#endif  // __THEOMUSICREADINGCTROLPARAMS_H__
