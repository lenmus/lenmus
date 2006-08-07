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
/*! @file Tags.cpp
    @brief Hooks to handle LenMus defined HTML directives.
    @ingroup html_management
*/
//-------------------------------------------------------------------------------------
/*! @page embedded_controls    Controls embedded in HTML pages

    Two types of embedded controls: main (Ctrol) and auxiliary (AuxCtrol).

    A Ctrol can be included directly in html by using an \<object\> directive. Therefore
    there must exist a correspondence between Ctrol objects and CtrolParams objects.
    An AuxCtrol can not be included directly in html by using an \<object\> directive. It
    only can be included programatically, either inside a Ctrol or by direct usage during
    the construction of an HtmlCell.

    Main controls enclose all the functionality. Are implemented as windows with all its
    controls on it. So commnads for main controls are processed as events on that windows
    (see TheoIntervalCtrol.cpp as an example) and there is no need to respond to
    external commands.

    Auxiliary controls implement sub-controls in main controls. They generate events to
    be processed in the parent window.

    @verbatim

    Ctrols                                          AuxCtrols
    ---------------------------------------         -------------------------------------
    lmScoreCtrol : public wxWindow                  lmScoreAuxCtrol : public wxWindow
    lmTheoIntervalsCtrol : public wxWindow          lmUrlAuxCtrol : public wxStaticText
    lmTheoScalesCtrol : public wxWindow             lmCountersCtrol : public wxWindow
    lmTheoKeySignCtrol : public wxWindow
    lmEarIntervalsCtrol : public wxWindow
    lmEarCompareIntvCtrol : public wxWindow
    lmEarChordCtrol : public wxWindow
    lmTheoMusicReadingCtrol : public wxWindow


    <object> directives and param classes
    -------------------------------------------------------------------------------------
                                                    lmObjectParams
    type="Application/LenMusScore"                  lmScoreCtrolParams : public lmObjectParams
    type="Application/LenMusTheoIntervals"          lmTheoIntervalsCtrolParms : public lmObjectParams
    type="Application/LenMusTheoScales"             lmTheoScalesCtrolParms : public lmObjectParams
    type="Application/LenMusTheoKeySignatures"      lmTheoKeySignCtrolParms : public lmObjectParams
    type="Application/LenMusEarIntervals"           lmEarIntervalsCtrolParms : public lmObjectParams
    type="Application/LenMusEarCompareIntervals"    lmEarCompareIntvCtrolParms : public lmObjectParams
    type="Application/LenMusEarChord"               lmEarChordCtrolParms : public lmObjectParams

    @endverbatim

*/


#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


// to define html tags handlers
#include "wx/html/forcelnk.h"
#include "wx/html/m_templ.h"

// for displaying a score
#include "../exercises/ScoreCtrol.h"
#include "../score/score.h"
#include "../xml_parser/XMLParser.h"
#include "../ldp_parser/LDPParser.h"

// for displaying exercises
#include "../exercises/Constrains.h"

#include "../exercises/TheoIntervalsCtrol.h"
#include "../exercises/TheoScalesCtrol.h"
#include "../exercises/TheoKeySignConstrains.h"
#include "../exercises/TheoKeySignCtrol.h"
#include "../exercises/TheoMusicReadingCtrol.h"

#include "../exercises/EarIntervalsCtrol.h"
#include "../exercises/EarCompareIntvCtrol.h"
#include "../exercises/EarIntvalConstrains.h"
#include "../exercises/EarChordCtrol.h"

#include "../app/MainFrame.h"
extern lmMainFrame* g_pMainFrame;

/*
\<object\>
    width            pixels | percent        default: 100%
    height            pixels
    border            0 | 1                    default: 1 (with border)

*/

#include "ObjectParams.h"
#include "TheoMusicReadingCtrolParms.h"
#include "ScoreCtrolParams.h"
#include "EarChordCtrolParms.h"


//OBSOLETE:
/*
Obsolete code. Left here as reference about how to insert text into the HTML stream

    //Insert the score in the html page
    wnd = new lmScoreAuxCtrol((wxWindow*)pHtmlParser->GetWindow(), -1, m_pScore,
        wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    wnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(wnd, m_nPercent));

    //insert optional controls
    if (m_fPlayCtrol) {
        wxHtmlLinkInfo oldlnk = pHtmlParser->GetLink();
        wxColour oldclr = pHtmlParser->GetActualColor();
        int oldund = pHtmlParser->GetFontUnderlined();

        pHtmlParser->SetActualColor(pHtmlParser->GetLinkColor());
        pHtmlParser->GetContainer()->InsertCell(
            new wxHtmlColourCell(pHtmlParser->GetLinkColor()));
        pHtmlParser->SetFontUnderlined(true);
        pHtmlParser->GetContainer()->InsertCell(
            new wxHtmlFontCell(pHtmlParser->CreateCurrentFont()));
        pHtmlParser->SetLink(wxHtmlLinkInfo(_T("#LenMusCmd"), wxEmptyString));

        // temporarily change the source being parsed, to include the link label.
        // I restore the state back after parsing
        pHtmlParser->SetSourceAndSaveState(_T("<p>") + m_sPlayLabel);
        pHtmlParser->DoParsing();
        pHtmlParser->RestoreState();

        pHtmlParser->SetLink(oldlnk);
        pHtmlParser->SetFontUnderlined(oldund);
        pHtmlParser->GetContainer()->InsertCell(
            new wxHtmlFontCell(pHtmlParser->CreateCurrentFont()));
        pHtmlParser->SetActualColor(oldclr);
        pHtmlParser->GetContainer()->InsertCell(
            new wxHtmlColourCell(oldclr));
    }
*/

//===============================================================================================
class lmTheoKeySignParms : public lmObjectParams
{
public:
    lmTheoKeySignParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmTheoKeySignParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:


        // Member variables:

    // html object window attributes
    long    m_nWindowStyle;
    lmTheoKeySignConstrains* m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmTheoKeySignParms)
};



lmTheoKeySignParms::lmTheoKeySignParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmObjectParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // create constrains object (construtor initilizes it with default values for attributes)
    m_pConstrains = new lmTheoKeySignConstrains();

}


lmTheoKeySignParms::~lmTheoKeySignParms()
{
    //Constrains will be deleted by the Ctrol. DO NOT DELETE IT HERE
    //if (m_pConstrains) delete m_pConstrains;

}

void lmTheoKeySignParms::AddParam(const wxHtmlTag& tag)
{
    /*! @page KeySignParms
        @verbatim    

        Params for lmTheoKeySignParms - html object type="Application/LenMusTheoKeySignatures"

        param name          value                                       default value
        -----------------------------------------------------------------------------
        max_accidentals     num (0..7)                                  [5]
        problem_type        DeduceKey | WriteKey | Both                 [Both]
        clef*               Sol | Fa4 | Fa3 | Do4 | Do3 | Do2 | Do1     [Sol]
        mode                Major | Minor | Both                        [Both]

        Example

        <object type="Application/LenMusTheoKeySignatures" width="100%" height="300" border="0">
            <param  name="max_accidentals" value="7" />
            <param  name="problem_type" value="both" />
            <param  name="clef" value="sol" />
            <param  name="mode" value="both" />
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
    
    // max_accidentals        num (0..7)
    if ( sName == _T("MAX_ACCIDENTALS") ) {
        wxString sAccidentals = tag.GetParam(_T("VALUE"));
        long nAccidentals;
        bool fOK = sAccidentals.ToLong(&nAccidentals);
        if (!fOK || nAccidentals < 0 || nAccidentals > 7) {
            LogError( wxString::Format(
                _("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: numeric, 0..7"),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
        }
        else {
            m_pConstrains->SetMaxAccidentals((int)nAccidentals);
        }
    }

    // problem_type        DeduceKey | WriteKey | Both                 [Both]
    else if ( sName == _T("PROBLEM_TYPE") ) {
        wxString sProblem = tag.GetParam(_T("VALUE"));
        sProblem.UpperCase();
        if (sProblem == _T("DEDUCEKEY"))
            m_pConstrains->SetProblemType( eIdentifyKeySignature );
        else if (sProblem == _T("WRITEKEY"))
            m_pConstrains->SetProblemType( eWriteKeySignature );
        else if (sProblem == _T("BOTH"))
            m_pConstrains->SetProblemType( eBothKeySignProblems );
        else
            LogError(wxString::Format( 
_("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: DeduceKey | WriteKey | Both"),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    // clef        Sol | Fa4 | Fa3 | Do4 | Do3 | Do2 | Do1 
    else if ( sName == _T("CLEF") ) {
        wxString sClef = tag.GetParam(_T("VALUE"));
        sClef.UpperCase();
        if (sClef == _T("SOL"))
            m_pConstrains->SetClef(eclvSol, true);
        else if (sClef == _T("FA4"))
            m_pConstrains->SetClef(eclvFa4, true);
        else if (sClef == _T("FA3"))
            m_pConstrains->SetClef(eclvFa3, true);
        else if (sClef == _T("DO4"))
            m_pConstrains->SetClef(eclvDo4, true);
        else if (sClef == _T("DO3"))
            m_pConstrains->SetClef(eclvDo3, true);
        else if (sClef == _T("DO2"))
            m_pConstrains->SetClef(eclvDo2, true);
        else if (sClef == _T("DO1"))
            m_pConstrains->SetClef(eclvDo1, true);
        else
            LogError(wxString::Format(
_("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: Sol | Fa4 | Fa3 | Do4 | Do3 | Do2 | Do1"),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    // mode         Major | Minor | Both                        [Both]
    else if ( sName == _T("MODE") ) {
        wxString sProblem = tag.GetParam(_T("VALUE"));
        sProblem.UpperCase();
        if (sProblem == _T("MAJOR"))
            m_pConstrains->SetScaleMode( eMajorMode );
        else if (sProblem == _T("MINOR"))
            m_pConstrains->SetScaleMode( eMinorMode );
        else if (sProblem == _T("BOTH"))
            m_pConstrains->SetScaleMode( eMayorAndMinorModes );
        else
            LogError(wxString::Format( 
_("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: Major | Minor | Both"),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    // Unknown param
    else
        LogError(wxString::Format( 
            _("lmTheoIntervalsCtrol. Unknown param: <param %s >\n"),
            tag.GetAllParams() ));

}

void lmTheoKeySignParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    // ensure that at least a Clef is selected
    bool fClefSpecified = false;
    for (int i=lmMIN_CLEF; i <= lmMAX_CLEF; i++) {
        fClefSpecified = fClefSpecified || m_pConstrains->IsValidClef((EClefType)i);
        if (fClefSpecified) break;
    }
    if (!fClefSpecified) {
        m_pConstrains->SetClef(eclvSol, true);
    }

    // create the window
    wxWindow* wnd = new lmTheoKeySignCtrol((wxWindow*)g_pMainFrame->GetHtmlWindow(), -1, 
        m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    wnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(wnd, m_nPercent));

}



//===============================================================================================
class lmTheoIntervalsCtrolParms : public lmObjectParams
{
public:
    lmTheoIntervalsCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmTheoIntervalsCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:


        // Member variables:

    // html object window attributes
    long    m_nWindowStyle;
    lmTheoIntervalsConstrains* m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmTheoIntervalsCtrolParms)
};



lmTheoIntervalsCtrolParms::lmTheoIntervalsCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmObjectParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // create the constrains object
    m_pConstrains = new lmTheoIntervalsConstrains();

}


lmTheoIntervalsCtrolParms::~lmTheoIntervalsCtrolParms()
{
    //Constrains will be deleted by the Ctrol. DO NOT DELETE IT HERE
    //if (m_pConstrains) delete m_pConstrains;

}

void lmTheoIntervalsCtrolParms::AddParam(const wxHtmlTag& tag)
{
    /*
        accidentals        none | simple | double                        [none]
        problem_type    DeduceInterval | BuildInterval | Both        [both]
        clef*            Sol | Fa4 | Fa3 | Do4 | Do3 | Do2 | Do1        [Sol]

    */

    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.UpperCase();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute
    
    // accidentals        none | simple | double
    if ( sName == _T("ACCIDENTALS") ) {
        wxString sAccidentals = tag.GetParam(_T("VALUE"));
        sAccidentals.UpperCase();
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
_("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: none | simple | double"),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    //problem_type    DeduceInterval | BuildInterval | Both
    else if ( sName == _T("PROBLEM_TYPE") ) {
        wxString sProblem = tag.GetParam(_T("VALUE"));
        sProblem.UpperCase();
        if (sProblem == _T("DEDUCEINTERVAL"))
            m_pConstrains->SetProblemType( ePT_DeduceInterval );
        else if (sProblem == _T("BUILDINTERVAL"))
            m_pConstrains->SetProblemType( ePT_BuildInterval );
        else if (sProblem == _T("BOTH"))
            m_pConstrains->SetProblemType( ePT_Both );
        else
            LogError(wxString::Format(
_("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: DeduceInterval | BuildInterval | Both"),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    // clef        Sol | Fa4 | Fa3 | Do4 | Do3 | Do2 | Do1 
    else if ( sName == _T("CLEF") ) {
        wxString sClef = tag.GetParam(_T("VALUE"));
        sClef.UpperCase();
        if (sClef == _T("SOL"))
            m_pConstrains->SetClef(eclvSol, true);
        else if (sClef == _T("FA4"))
            m_pConstrains->SetClef(eclvFa4, true);
        else if (sClef == _T("FA3"))
            m_pConstrains->SetClef(eclvFa3, true);
        else if (sClef == _T("DO4"))
            m_pConstrains->SetClef(eclvDo4, true);
        else if (sClef == _T("DO3"))
            m_pConstrains->SetClef(eclvDo3, true);
        else if (sClef == _T("DO2"))
            m_pConstrains->SetClef(eclvDo2, true);
        else if (sClef == _T("DO1"))
            m_pConstrains->SetClef(eclvDo1, true);
        else
            LogError(wxString::Format( 
_("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: Sol | Fa4 | Fa3 | Do4 | Do3 | Do2 | Do1"),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    // Unknown param
    else
        LogError(wxString::Format( 
            _("lmTheoIntervalsCtrol. Unknown param: <param %s >\n"),
            tag.GetAllParams() ));

}

void lmTheoIntervalsCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    // ensure that at least a Clef is selected
    bool fClefSpecified = false;
    for (int i=lmMIN_CLEF; i <= lmMAX_CLEF; i++) {
        fClefSpecified = fClefSpecified || m_pConstrains->IsValidClef((EClefType)i);
        if (fClefSpecified) break;
    }
    if (!fClefSpecified) {
        m_pConstrains->SetClef(eclvSol, true);
    }

    // create the window
    wxWindow* wnd = new lmTheoIntervalsCtrol((wxWindow*)g_pMainFrame->GetHtmlWindow(), -1, 
        m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    wnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(wnd, m_nPercent));

}


//===============================================================================================
class lmEarIntervalsCtrolParms : public lmObjectParams
{
public:
    lmEarIntervalsCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmEarIntervalsCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:


        // Member variables:

    // html object window attributes
    long    m_nWindowStyle;
    lmEarIntervalsConstrains* m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmEarIntervalsCtrolParms)
};



lmEarIntervalsCtrolParms::lmEarIntervalsCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmObjectParams(tag, nWidth, nHeight, nPercent)
{
    m_nWindowStyle = nStyle;
    m_pConstrains = new lmEarIntervalsConstrains(_T("EarIntervals"));
}


lmEarIntervalsCtrolParms::~lmEarIntervalsCtrolParms()
{
    //Constrains will be deleted by the Ctrol. DO NOT DELETE IT HERE
    //if (m_pConstrains) delete m_pConstrains;

}

void lmEarIntervalsCtrolParms::AddParam(const wxHtmlTag& tag)
{
    /*
        max_interval    num         default: 8
    */

    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.UpperCase();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute
    
    // max_interval    num         default: 8
    if ( sName == _T("MAX_INTERVAL") ) {
        //wxString sAccidentals = tag.GetParam(_T("VALUE"));
        //sAccidentals.UpperCase();
        //if (sAccidentals == _T("NONE")) {
        //    m_pConstrains->SetAccidentals(false);
        //    m_pConstrains->SetDoubleAccidentals(false);
        //}
        //else if (sAccidentals == _T("SIMPLE"))
        //    m_pConstrains->SetAccidentals(true);
        //else if (sAccidentals == _T("DOUBLE"))
        //    m_pConstrains->SetDoubleAccidentals(true);
        //else
        //    LogError(wxString::Format( wxGetTranslation(
        //        _T("Invalid param value in:\n<param %s >\n")
        //        _T("Invalid value = %s \n")
        //        _T("Acceptable values: none | simple | double") ),
        //        tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    // Unknown param
    else
        LogError(wxString::Format( 
            _("lmEarIntervalsCtrol. Unknown param: <param %s >\n"),
            tag.GetAllParams() ));

}

void lmEarIntervalsCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    // ensure that at least an interval is selected
    bool fIntervalSpecified = false;
    for (int i=0; i < 25; i++) {
        fIntervalSpecified = fIntervalSpecified || m_pConstrains->IsIntervalAllowed(i);
        if (fIntervalSpecified) break;
    }
    if (!fIntervalSpecified) {
        m_pConstrains->SetIntervalAllowed(0, true);
    }

    // create the window
    wxWindow* wnd = new lmEarIntervalsCtrol((wxWindow*)g_pMainFrame->GetHtmlWindow(), -1, 
        m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    wnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(wnd, m_nPercent));

}


//===============================================================================================
class lmEarCompareIntvCtrolParms : public lmObjectParams
{
public:
    lmEarCompareIntvCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmEarCompareIntvCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:


        // Member variables:

    // html object window attributes
    long    m_nWindowStyle;
    lmEarIntervalsConstrains* m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmEarCompareIntvCtrolParms)
};



lmEarCompareIntvCtrolParms::lmEarCompareIntvCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmObjectParams(tag, nWidth, nHeight, nPercent)
{
    m_nWindowStyle = nStyle;
    m_pConstrains = new lmEarIntervalsConstrains(_T("EarCompare"));
}


lmEarCompareIntvCtrolParms::~lmEarCompareIntvCtrolParms()
{
    //Constrains will be deleted by the Ctrol. DO NOT DELETE IT HERE
    //if (m_pConstrains) delete m_pConstrains;

}

void lmEarCompareIntvCtrolParms::AddParam(const wxHtmlTag& tag)
{
    /*
        max_interval    num         default: 8
    */

    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.UpperCase();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute
    
    // max_interval    num         default: 8
    if ( sName == _T("MAX_INTERVAL") ) {
        //wxString sAccidentals = tag.GetParam(_T("VALUE"));
        //sAccidentals.UpperCase();
        //if (sAccidentals == _T("NONE")) {
        //    m_pConstrains->SetAccidentals(false);
        //    m_pConstrains->SetDoubleAccidentals(false);
        //}
        //else if (sAccidentals == _T("SIMPLE"))
        //    m_pConstrains->SetAccidentals(true);
        //else if (sAccidentals == _T("DOUBLE"))
        //    m_pConstrains->SetDoubleAccidentals(true);
        //else
        //    LogError(wxString::Format( wxGetTranslation(
        //        _T("Invalid param value in:\n<param %s >\n")
        //        _T("Invalid value = %s \n")
        //        _T("Acceptable values: none | simple | double") ),
        //        tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    // Unknown param
    else
        LogError(wxString::Format( 
            _("lmEarIntervalsCtrol. Unknown param: <param %s >\n"),
            tag.GetAllParams() ));

}

void lmEarCompareIntvCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    // ensure that at least an interval is selected
    bool fIntervalSpecified = false;
    for (int i=0; i < 25; i++) {
        fIntervalSpecified = fIntervalSpecified || m_pConstrains->IsIntervalAllowed(i);
        if (fIntervalSpecified) break;
    }
    if (!fIntervalSpecified) {
        m_pConstrains->SetIntervalAllowed(0, true);
    }

    // create the window
    wxWindow* wnd = new lmEarCompareIntvCtrol((wxWindow*)g_pMainFrame->GetHtmlWindow(), -1, 
        m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    wnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(wnd, m_nPercent));

}


//===============================================================================================
class lmTheoScalesCtrolParms : public lmObjectParams
{
public:
    lmTheoScalesCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                              int nPercent, long nStyle);
    ~lmTheoScalesCtrolParms();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:


        // Member variables:

    // html object window attributes
    long                    m_nWindowStyle;
    lmTheoScalesConstrains*    m_pConstrains;

    DECLARE_NO_COPY_CLASS(lmTheoScalesCtrolParms)
};



lmTheoScalesCtrolParms::lmTheoScalesCtrolParms(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, long nStyle)
    : lmObjectParams(tag, nWidth, nHeight, nPercent)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // construct constrains object
    m_pConstrains = new lmTheoScalesConstrains();

}


lmTheoScalesCtrolParms::~lmTheoScalesCtrolParms()
{
    //Constrains will be deleted by the Ctrol. DO NOT DELETE IT HERE
    //if (m_pConstrains) delete m_pConstrains;

}

void lmTheoScalesCtrolParms::AddParam(const wxHtmlTag& tag)
{
    /*
        scale_type*        major | minor                                [both]
        problem_type    DeduceScale | BuildScale | Both                [both]
        clef*            Sol | Fa4 | Fa3 | Do4 | Do3 | Do2 | Do1        [Sol]
        control            chkKeySignature                                [none]
    */

    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.UpperCase();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute
    
    // scale_type*        major | minor
    if ( sName == _T("SCALE_TYPE") ) {
        wxString sScaleType = tag.GetParam(_T("VALUE"));
        sScaleType.UpperCase();
        if (sScaleType == _T("MAJOR")) {
            m_pConstrains->SetMajorType(true);
        }
        else if (sScaleType == _T("MINOR"))
            m_pConstrains->SetMinorType(true);
        else
            LogError(wxString::Format( 
_("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: major | minor"),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    //problem_type        DeduceScale | BuildScale | Both
    else if ( sName == _T("PROBLEM_TYPE") ) {
        wxString sProblem = tag.GetParam(_T("VALUE"));
        sProblem.UpperCase();
        if (sProblem == _T("DEDUCESCALE"))
            m_pConstrains->SetProblemType( ePTS_DeduceScale );
        else if (sProblem == _T("BUILDSCALE"))
            m_pConstrains->SetProblemType( ePTS_BuildScale );
        else if (sProblem == _T("BOTH"))
            m_pConstrains->SetProblemType( ePTS_Both );
        else
            LogError(wxString::Format( 
_("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: DeduceScale | BuildScale | Both"),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    // clef*        Sol | Fa4 | Fa3 | Do4 | Do3 | Do2 | Do1 
    else if ( sName == _T("CLEF") ) {
        wxString sClef = tag.GetParam(_T("VALUE"));
        sClef.UpperCase();
        if (sClef == _T("SOL"))
            m_pConstrains->SetClef(eclvSol, true);
        else if (sClef == _T("FA4"))
            m_pConstrains->SetClef(eclvFa4, true);
        else if (sClef == _T("FA3"))
            m_pConstrains->SetClef(eclvFa3, true);
        else if (sClef == _T("DO4"))
            m_pConstrains->SetClef(eclvDo4, true);
        else if (sClef == _T("DO3"))
            m_pConstrains->SetClef(eclvDo3, true);
        else if (sClef == _T("DO2"))
            m_pConstrains->SetClef(eclvDo2, true);
        else if (sClef == _T("DO1"))
            m_pConstrains->SetClef(eclvDo1, true);
        else
            LogError(wxString::Format( 
_("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: Sol | Fa4 | Fa3 | Do4 | Do3 | Do2 | Do1"),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }
    // control            chkKeySignature
    else if ( sName == _T("CONTROL") ) {
        wxString sProblem = tag.GetParam(_T("VALUE"));
        sProblem.UpperCase();
        if (sProblem == _T("CHKKEYSIGNATURE"))
            m_pConstrains->SetCtrolKeySignature(true);
        else
            LogError(wxString::Format( 
_("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: chkKeySignature"),
                tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    // Unknown param
    else
        LogError(wxString::Format( 
            _("lmTheoScalesCtrol. Unknown param: <param %s >\n"),
            tag.GetAllParams() ));

}

void lmTheoScalesCtrolParms::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    // ensure that at least a Clef is selected
    bool fClefSpecified = false;
    for (int i=lmMIN_CLEF; i <= lmMAX_CLEF; i++) {
        fClefSpecified = fClefSpecified || m_pConstrains->IsValidClef((EClefType)i);
        if (fClefSpecified) break;
    }
    if (!fClefSpecified) {
        m_pConstrains->SetClef(eclvSol, true);
    }

    // if no scale type selected, select major and minor
    bool fScaleTypeSpecified = false;
    fScaleTypeSpecified = m_pConstrains->MajorType() || m_pConstrains->MinorType();
    if (!fScaleTypeSpecified) {
        m_pConstrains->SetMajorType(true);
        m_pConstrains->SetMinorType(true);
    }

    // create the window
    wxWindow* wnd = new lmTheoScalesCtrol((wxWindow*)g_pMainFrame->GetHtmlWindow(), -1, 
        m_pConstrains, wxPoint(0,0), wxSize(m_nWidth, m_nHeight), m_nWindowStyle );
    wnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(wnd, m_nPercent));

}


//===============================================================================================
//    Tag handlers for html extensions
//===============================================================================================

enum EHtmlObjectTypes {
    eHO_Unknown = 0,
    eHO_MusicScore,
    eHO_Exercise_TheoIntervals,
    eHO_Exercise_TheoScales,
    eHO_Exercise_TheoKeySignatures,
    eHO_Exercise_EarIntervals,
    eHO_Exercise_EarCompareIntervals,
    eHO_Exercise_TheoMusicReading,
    eHO_Exercise_EarChord,
    eHO_Control
};


FORCE_LINK_ME(Tags)


TAG_HANDLER_BEGIN(OBJECT, "OBJECT,PARAM")

TAG_HANDLER_VARS
    lmObjectParams* m_pObjectParams;

TAG_HANDLER_CONSTR(OBJECT)
{
    m_pObjectParams = (lmObjectParams*) NULL;
}



TAG_HANDLER_PROC(tag)
{
    /*
    This handler deals with the \<object\> and the <param> tags.
    object tags types:
        type="image/LenMusScore"            - music score
        type="application/LenMusControl"    - a control (i.e. exercises)

    No nested \<object\> tags are allowed

   */

    if (tag.GetName() == wxT("OBJECT"))
    {
        //verify type -----------------------------------------------------
        EHtmlObjectTypes nType = eHO_Unknown;
        if (tag.HasParam(wxT("TYPE")) ) {
            wxString sType = tag.GetParam(wxT("TYPE"));
            if (sType.Upper() == _T("APPLICATION/LENMUSSCORE"))
                nType = eHO_MusicScore;
            else if (sType.Upper() == _T("APPLICATION/LENMUSTHEOINTERVALS"))
                nType = eHO_Exercise_TheoIntervals;
            else if (sType.Upper() == _T("APPLICATION/LENMUSTHEOSCALES"))
                nType = eHO_Exercise_TheoScales;
            else if (sType.Upper() == _T("APPLICATION/LENMUSTHEOKEYSIGNATURES"))
                nType = eHO_Exercise_TheoKeySignatures;
            else if (sType.Upper() == _T("APPLICATION/LENMUSCONTROL"))
                nType = eHO_Control;
            else if (sType.Upper() == _T("APPLICATION/LENMUSEARINTERVALS"))
                nType = eHO_Exercise_EarIntervals;
            else if (sType.Upper() == _T("APPLICATION/LENMUSEARCOMPAREINTERVALS"))
                nType = eHO_Exercise_EarCompareIntervals;
            else if (sType.Upper() == _T("APPLICATION/LENMUSTHEOMUSICREADING"))
                nType = eHO_Exercise_TheoMusicReading;
            else if (sType.Upper() == _T("APPLICATION/LENMUSEARCHORD"))
                nType = eHO_Exercise_EarChord;
        }
        if (nType == eHO_Unknown) return true;        // type non processable by LenMus

        // parse common attributes for all object types
        // attributes "width" and "height": height is always in pixels
        // width can be pixels or pencentage
        bool fOK;
        int nWidth=0, nHeight=0, nPercent=100;        //window size
        if (tag.HasParam(_T("WIDTH"))) {
            wxString sWidth = tag.GetParam(_T("WIDTH"));
            int i = sWidth.Find(_T("%"));
            if (i > 0) {
                long nPercentage = 0;
                sWidth = sWidth.Left(i);    // take out the '%' symbol
                fOK = sWidth.ToLong(&nPercentage);
                nPercent = (int)nPercentage;
            }
            else {
                fOK = tag.GetParamAsInt(_T("WIDTH"), &nWidth);
                nPercent = 0;
            }
            wxASSERT(fOK);
        }
        tag.ScanParam(_T("HEIGHT"), _T("%i"), &nHeight);

        //Get attibute "border"
        int nBorder=1;        //default value: with border
        if (tag.HasParam(wxT("BORDER")) ) {
            fOK = tag.GetParamAsInt(wxT("BORDER"), &nBorder);
            wxASSERT(fOK);
        }
        EScoreStyles nStyle = (nBorder == 0 ? eNO_BORDER : eSIMPLE_BORDER);
    
        // create object to store param data, depending of object type
        switch (nType) {
            case eHO_Control:
                //! @todo no specific param container object
                m_pObjectParams = new lmScoreCtrolParams(tag, nWidth, nHeight, nPercent, nStyle);
                break;

            case eHO_MusicScore:
                m_pObjectParams = new lmScoreCtrolParams(tag, nWidth, nHeight, nPercent, nStyle);
                break;

            case eHO_Exercise_TheoIntervals:
                m_pObjectParams = new lmTheoIntervalsCtrolParms(tag, nWidth, nHeight, 
                    nPercent, nStyle);
                break;

            case eHO_Exercise_TheoScales:
                m_pObjectParams = new lmTheoScalesCtrolParms(tag, nWidth, nHeight, 
                    nPercent, nStyle);
                break;

            case eHO_Exercise_TheoKeySignatures:
                m_pObjectParams = new lmTheoKeySignParms(tag, nWidth, nHeight, 
                    nPercent, nStyle);
                break;

            case eHO_Exercise_EarIntervals:
                m_pObjectParams = new lmEarIntervalsCtrolParms(tag, nWidth, nHeight, 
                    nPercent, nStyle);
                break;

            case eHO_Exercise_EarCompareIntervals:
                m_pObjectParams = new lmEarCompareIntvCtrolParms(tag, nWidth, nHeight, 
                    nPercent, nStyle);
                break;

            case eHO_Exercise_EarChord:
                m_pObjectParams = new lmEarChordCtrolParms(tag, nWidth, nHeight, 
                    nPercent, nStyle);
                break;

            case eHO_Exercise_TheoMusicReading:
                m_pObjectParams = new lmTheoMusicReadingCtrolParms(tag, nWidth, nHeight, 
                    nPercent, nStyle);
                break;

            default:
                wxASSERT(false);
        }

        // parse <param> tags
        ParseInner(tag);        

        // finally, create the html object
        m_pObjectParams->CreateHtmlCell(m_WParser);
        delete m_pObjectParams;        //no longer needed

        return false;

    }

    // <param> tag ------------------------------------------------------------
    else if (tag.GetName() == wxT("PARAM"))
    {
        if (m_pObjectParams) {
            m_pObjectParams->AddParam(tag);
            return false;
        }
        return true;    // error: tag <param> not inside \<object\> tag
    }

    // unknown tag
    else
        return true;

}

TAG_HANDLER_END(OBJECT)


TAGS_MODULE_BEGIN(Object)

    TAGS_MODULE_ADD(OBJECT)

TAGS_MODULE_END(Object)


