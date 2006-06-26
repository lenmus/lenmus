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
/*! @file XMLParser.cpp
    @brief Implementation file for class lmXMLParser
    @ingroup xml_parser
*/
#ifdef __GNUG__
// #pragma implementation
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/textfile.h"
#include "wx/log.h"

#include "../score/Score.h"
#include "XMLParser.h"
#include "../ldp_parser/AuxString.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;


lmXMLParser::lmXMLParser()
{
    m_pTupletBracket = (lmTupletBracket*)NULL;        //no tuplet being created
    m_nCurrentDivisions = 0;                        //not set yet

}

lmXMLParser::~lmXMLParser()
{
    wxASSERT(!m_pTupletBracket);
}

lmScore* lmXMLParser::ParseMusicXMLFile(const wxString& filename, bool fNewLog, bool fShowLog) 
{
    if (fNewLog) g_pLogger->FlushDataErrorLog();
    g_pLogger->LogDataMessage(_("Importing XML file %s\n\n"), filename);

    // load the XML file as tree of nodes
    wxXmlDocument xdoc;
    if (!xdoc.Load(filename))
    {
        g_pLogger->ReportProblem(_("Error parsing XML file ") + filename);
        return (lmScore*) NULL;
    }

    //Verify type of score. Must be <score-partwise>
    wxXmlNode *pRoot = xdoc.GetRoot();
    if (pRoot->GetName() != _T("score-partwise")) {
        g_pLogger->ReportProblem(
            _("Error. <%s> files are not supported"),
            pRoot->GetName() );
        return (lmScore*) NULL;
    }
    
    // build the score
    lmScore* pScore = new lmScore();
    m_nErrors = 0;
    ParseScorePartwise(pRoot, pScore);

    // report errors
    g_pLogger->LogDataMessage(_("\nMusicXML file imported OK. There are %d warnings."), m_nErrors);
    if (fShowLog && m_nErrors != 0) {
        g_pLogger->ShowDataErrors(_("Warnings while importing a MusicXML score."));
    }

    // Dump doc tree
    //wxLogMessage(_T("*** DUMP OF XML TREE ***:"));
    //wxXmlNode *pRoot = xdoc.GetRoot();
    //wxLogMessage(_T("Root element: [%s]"), pRoot->GetName());
    //DumpXMLTree(pRoot);
    //wxLogMessage(_T("*** END OF DUMP ***"));

    return pScore;

}

void lmXMLParser::DumpXMLTree(wxXmlNode *pRoot)
{
    if (pRoot == NULL) return;
    if (pRoot->GetType() != wxXML_ELEMENT_NODE) return;

    wxXmlNode *pNode = pRoot->GetChildren();
    while (pNode)
    {
        // if final node dump content
        if ((pNode->GetType() == wxXML_TEXT_NODE || 
             pNode->GetType() == wxXML_CDATA_SECTION_NODE))
        {
            wxLogMessage(_T("Node: [%s] = \"%s\""), pNode->GetName(), pNode->GetContent() );
        }

        // dump subnodes:
        if (pNode->GetType() == wxXML_ELEMENT_NODE)
        {
            wxLogMessage(_T("Element: [%s]"), pNode->GetName());
            DumpXMLTree(pNode);
        }

        pNode = pNode->GetNext();
    }
}

void lmXMLParser::ParseError(const wxChar* szFormat, ...)
{
    m_nErrors++;
    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg = wxString::FormatV(szFormat, argptr);
    g_pLogger->LogDataError(sMsg);
    va_end(argptr);
}

void lmXMLParser::TagError(const wxString sElement, const wxString sTagName, wxXmlNode* pElement)
{
    m_nErrors++;
    wxString sMsg = wxString::Format(
        _("Parsing <%s>: tag <%s> not supported."),
        sElement, sTagName);
    g_pLogger->LogDataError(sMsg);

    if (pElement) {
        //! @todo Log source code or reference to source line. But how?
        //g_pLogger->LogDataError(pElement->));
    }
}

void lmXMLParser::ParseScorePartwise(wxXmlNode* pNode, lmScore* pScore)
{
    /*
    The score is the root element for the DTD. It includes the score-header entity,
    followed by a series of parts with measures inside (score-partwise type).

    <!ELEMENT score-partwise (%score-header;, part+)>

    The score-header entity contains basic score metadata about the work and movement,
    plus the part list. 

    <!ENTITY % score-header
        "(work?, movement-number?, movement-title?,
        identification?, part-list)">

    <!ELEMENT movement-number (#PCDATA)>
    <!ELEMENT movement-title (#PCDATA)>

    */
    wxString sElement = _T("score-partwise");
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("--> XML Parser: Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    //find first child
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    
    while (pElement) {
        if (pElement->GetName() == _T("part-list")) {
            ParsePartList(pElement, pScore);
        }
        else if (pElement->GetName() == _T("part")) {
            ParsePart(pElement, pScore);
        }
        else if (pElement->GetName() == _T("work")) {
            ParseWork(pElement, pScore);
        }
        else if (pElement->GetName() == _T("identification")) {
            ParseIdentification(pElement, pScore);
        }
        else if (pElement->GetName() == _T("movement-number")) {
            //! @todo
            g_pLogger->LogTrace(_T("lmXMLParser"), _T("movement-number not yet implemented"), _T(""));
        }
        else if (pElement->GetName() == _T("movement-title")) {
            //! @todo
            g_pLogger->LogTrace(_T("lmXMLParser"), _T("movement-title not yet implemented"), _T(""));
        } else {
            TagError(sElement, pElement->GetName(), pElement);
        }

        // Find next sibling 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    
    }

}

void lmXMLParser::ParsePart(wxXmlNode* pNode, lmScore* pScore)
{
    /*
    <part> contains all measures for one instrument
        <!ELEMENT part (measure+)>
    */
    wxString sElement = _T("part");
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    //Get part id
    wxString sId = GetAttribute(pNode, _T("id"));
    if (sId.IsEmpty()) return;
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Procesing part id=%s"), sId);

    // At this point all score instruments have been created (in score-part).
    // Find the instrument that corresponds to this part
    lmInstrument* pInstr = pScore->XML_FindInstrument(sId);
    if (!pInstr) {
        ParseError(_("Part id = %s not defined in <part-list>"), sId);
        return;
    }

    // Get VStaff
    lmVStaff* pVStaff = pInstr->GetVStaff(1);
    wxASSERT(pVStaff);

    // parse xml element

    //find first child
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
      
    while (pElement)
    {
        if (pElement->GetName() == _T("measure")) {
            ParseMeasure(pElement, pVStaff);
        }
        else {
            TagError(sElement, pElement->GetName());
        }

        // Find next sibling 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    }

}

void lmXMLParser::ParseMeasure(wxXmlNode* pNode,     lmVStaff* pVStaff)
{
    /*
    score.dtd
    Here is the basic musical data that is either associated
    with a part or a measure, depending on whether partwise
    or timewise hierarchy is used.

    <!ELEMENT measure (%music-data;)>
    <!ENTITY % music-data
        "(note | backup | forward | direction | attributes |
        harmony | figured-bass | print | sound | barline | 
        grouping | link | bookmark)*">

    */
    wxString sElement = _T("measure");
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    //find first child
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
      
    bool fSomethingAdded = false, fBarline = false;
    while (pElement)
    {
        if (pElement->GetName() == _T("attributes")) {
            fSomethingAdded |= ParseMusicDataAttributes(pElement, pVStaff);
        }
        else if (pElement->GetName() == _T("barline")) {
            fBarline |= ParseMusicDataBarline(pElement, pVStaff);
        }
        else if (pElement->GetName() == _T("direction")) {
            fSomethingAdded |= ParseMusicDataDirection(pElement, pVStaff);
        }
        else if (pElement->GetName() == _T("note")) {
            fSomethingAdded |= ParseMusicDataNote(pElement, pVStaff);
        }
        else if (pElement->GetName() == _T("backup")) {
            float rShift = ParseMusicDataBackupForward(pElement, pVStaff);
            pVStaff->ShiftTime(rShift);
        }
        else if (pElement->GetName() == _T("forward")) {
            float rShift = ParseMusicDataBackupForward(pElement, pVStaff);
            pVStaff->ShiftTime(rShift);
        }
        else {
            TagError(sElement, pElement->GetName());
        }

        // Find next sibling 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    }

    if (fSomethingAdded && !fBarline) {
        pVStaff->AddBarline(etb_SimpleBarline);    //finish the bar
    }

}

float lmXMLParser::ParseMusicDataBackupForward(wxXmlNode* pNode, lmVStaff* pVStaff)
{
    /*
    Backup and forward are required to coordinate multiple voices in one part, 
    including music on multiple staves. Forward is generally used within voices
    and staves, while backup to move between voices and staves. Thus the backup
    element does not include voice or staff elements. Duration values should 
    always be positive, and should not cross measure boundaries.

    <!ELEMENT backup (duration, %editorial;)>
    <!ELEMENT forward
        (duration, %editorial-voice;, staff?)>

    */
    wxString sElement = pNode->GetName();
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(sElement == _T("backup") || sElement == _T("forward"));

    bool fBackup = (sElement == _T("backup"));
    int nDuration = 0;

     //find first child
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    wxString sTag, sDbgTag;
    while (pElement)
    {
        sTag = pElement->GetName();
        if (sTag == _T("duration")) {
            nDuration = ParseDuration(pElement);
        }
        //! @todo elements voice, staff (only for <forward>)
        else {
            TagError(sElement, pElement->GetName());
        }

        // Find next sibling 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    }
    
    float rShift = ((float)nDuration / (float)m_nCurrentDivisions) * (fBackup ? -1 : 1) * XML_DURATION_TO_LDP ;
    g_pLogger->LogTrace(_T("lmXMLParser"), 
        _("Parsing <%s>: duration=%d, timeShift=%f"), sElement, nDuration, rShift );
    return rShift;
      
}

bool lmXMLParser::ParseMusicDataAttributes(wxXmlNode* pNode, lmVStaff* pVStaff)
{
    /*
    attributes.dtd
    The attributes element contains musical information that typically changes on
    measure boundaries. This includes key and time signatures, clefs, transpositions,
    and staving.

    <!ELEMENT attributes (%editorial;, divisions?, key?, time?,
        staves?, instruments?, clef*, staff-details*, transpose?,
        directive*, measure-style*)>

    */

    wxString sElement = _T("attributes");
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    bool fError = false;

    /*
    There is a problem with MusicXML. Clefs, time signatures and key signatures are
    treated as attributes of a measure, not as objects and, therefore, definion ordering
    is not important for MusicXML and there are examples in wich this information is
    coded in reversed orders (i.e. time signature, clef). 
    This causes problems as LenMus expects that this objects are defined in    right order.
    To solve this, object creation will be delayed until all attributes are parsed.
    To simplify, as Clefs are always the first object to define and as there can be 
    many clefs inside an <attribute> tag (i.e. for a grand staff there will be two clefs),
    Clefs creation is not going to be delayed.
    Here we define the variables to save the data for each object.
    */
        //TimeSignature data
    bool fTimeSignature = false;        //define a clef
    long nBeats=4;
    long nBeatType=4;
        //lmKeySignature data
    bool fKeySignature = false;        //define a clef
    bool fMajor = true;
    long nFifths = 0;


     //find first child
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
      
    wxString sTag, sDbgTag;
    while (pElement)
    {
        sTag = pElement->GetName();

        // <cleft> tag ------------------------------------------------------
        if (sTag == _T("clef")) {
            wxString sDbgTag = _T(">:<") + sTag;
            wxString sClef = _T("G");            //<cleft> default values
            wxString sLine = _T("4");
            wxString sStaffNumber = _T("1");

            // get staff number
            sStaffNumber = GetAttribute(pNode, _T("number"));
            if (sStaffNumber.IsEmpty()) sStaffNumber = _T("1");

            // get cleft data
            wxXmlNode* pChild = GetFirstChild(pNode);
            wxXmlNode* pElmnt = pChild;
            while (pElmnt) {
                if (pElmnt->GetName() == _T("sign"))
                    sClef = GetText(pElmnt);
                else if (pElmnt->GetName() == _T("line"))
                    sLine = GetText(pElmnt);
                else
                    TagError(sElement + sDbgTag, pElmnt->GetName()); 
                pChild = GetNextSibling(pChild);
                pElmnt = pChild;
            }

            //Log results
            g_pLogger->LogTrace(_T("lmXMLParser"), 
                _("Parsing <clef>: staff_number=%s, clef=%s, line=%s"),
                sStaffNumber, sClef, sLine );

            // Add clef to score
            //! @todo verify numeric and not greater than NumStaves
            long nStaff = 1;
            EClefType nClef = eclvSol;
            sStaffNumber.ToLong(&nStaff);
            fError = XmlDataToClef(sClef+sLine, &nClef);
            pVStaff->AddClef(nClef, nStaff);
        }

         // <staves> tag -----------------------------------------------------
        else if (sTag == _T("staves")) {
            wxString sNumStaves = GetText(pElement);

            //Log results
            g_pLogger->LogTrace(_T("lmXMLParser"), 
                _("Parsing <staves>: num_staves=%s"), sNumStaves );

            //Define how many staves the instrument has
            //! @todo verify numeric
            if (sNumStaves != _T("1")) {
                long nNumStaves, i;
                sNumStaves.ToLong(&nNumStaves);
                for(i=1; i < nNumStaves; i++) {
                    pVStaff->AddStaff(5);    //five lines, standard size
                }
            }
        }

         // <key> tag -----------------------------------------------------
        else if (sTag == _T("key")) {
            /*
            Traditional key signatures are represented by the number
            of flats and sharps, plus an optional mode for major/minor mode
            distinctions. Negative numbers are used for
            flats and positive numbers for sharps, reflecting the
            key's placement within the circle of fifths (hence the
            element name). A cancel element indicates that the old
            key signature should be cancelled before the new one
            appears. This will always happen when changing to C major
            or A minor and need not be specified then. The cancel
            value matches the fifths value of the cancelled key
            signature (e.g., a cancel of -2 will provide an explicit
            cancellation for changing from B flat major to F major).
            
            Non-traditional key signatures can be represented using
            the Humdrum/Scot concept of a list of altered tones.
            The key-step and key-alter elements are represented the
            same way as the step and alter elements are in the pitch
            element in note.dtd. The different element names indicate
            the different meaning of altering notes in a scale versus
            altering a sounding pitch.
            
            Valid mode values include major, minor, dorian, phrygian,
            lydian, mixolydian, aeolian, ionian, and locrian.

            <!ELEMENT key ((cancel?, fifths, mode?) |
                ((key-step, key-alter)*))>
            <!ELEMENT cancel (#PCDATA)>
            <!ELEMENT fifths (#PCDATA)>
            <!ELEMENT mode (#PCDATA)>
            <!ELEMENT key-step (#PCDATA)>
            <!ELEMENT key-alter (#PCDATA)>

            */

            /*! @todo Here I am dealing only with "traditional" key signatures: 
                chromatic scale in major and minor modes)
            */
             wxString sFifths = _T("0");            //<key> default value: C major
            wxString sMode = _T("major");
            // get data
            wxXmlNode* pChild = GetFirstChild(pNode);
            wxXmlNode* pElmnt = pChild;
            while (pElmnt) {
                if (pElmnt->GetName() == _T("fifths"))
                    sFifths = GetText(pElmnt);
                else if (pElmnt->GetName() == _T("mode"))
                    sMode = GetText(pElmnt);
                else
                    TagError(sElement + sDbgTag, pElmnt->GetName()); 
                pChild = GetNextSibling(pChild);
                pElmnt = pChild;
            }
            //Log results
            g_pLogger->LogTrace(_T("lmXMLParser"), 
                _("Parsing <key>: fifths=%s, mode=%s"), sFifths, sMode );

            //! @todo Change this for dealing with non-traditional key signatures
            fMajor = (sMode == _T("major"));
            nFifths = 0;
            bool fError = !sFifths.ToLong(&nFifths);

            if (fError) {
                g_pLogger->LogDataError(
                    _T("Key signature fifths=%s, mode=%s not supported. Assumed C major"),
                    sFifths, sMode );
                nFifths = 0;
                fMajor = true;
            }

            //create the key signature object
            fKeySignature = true;    //delay creation
            
        }

         // <time> tag -----------------------------------------------------
        else if (sTag == _T("time")) {
            /*
            Time signatures are represented by two elements. The
            beats element indicates the number of beats, as found in
            the numerator of a time signature. The beat-type element
            indicates the beat unit, as found in the denominator of
            a time signature. The symbol attribute is used to
            indicate another notation beyond a fraction: the common 
            and cut time symbols, as well as a single number with an
            implied denominator. Normal (a fraction) is the implied
            symbol type if none is specified. Multiple pairs of
            beat and beat-type elements are used for composite
            time signatures with multiple denominators, such as
            2/4 + 3/8. A composite such as 3+2/8 requires only one
            beat/beat-type pair. A senza-misura element explicitly
            indicates that no time signature is present.

            <!ELEMENT time ((beats, beat-type)+ | senza-misura)>
            <!ATTLIST time
                symbol (common | cut | single-number | normal) #IMPLIED
            >
            <!ELEMENT beats (#PCDATA)>
            <!ELEMENT beat-type (#PCDATA)>
            <!ELEMENT senza-misura EMPTY>

            */

            wxString sBeats = _T("4");            //<time> default value: 4/4
            wxString sBeatType = _T("4");
            // get data
            wxXmlNode* pChild = GetFirstChild(pNode);
            wxXmlNode* pElmnt = pChild;
            while (pElmnt) {
                if (pElmnt->GetName() == _T("beats"))
                    sBeats = GetText(pElmnt);
                else if (pElmnt->GetName() == _T("beat-type"))
                    sBeatType = GetText(pElmnt);
                else
                    TagError(sElement + sDbgTag, pElmnt->GetName()); 
                pChild = GetNextSibling(pChild);
                pElmnt = pChild;
            }
            //Log results
            g_pLogger->LogTrace(_T("lmXMLParser"), 
                _("Parsing <time>: beats=%s, beat-type=%s"), sBeats, sBeatType );

            //! @todo Change this for a more general treatment
            bool fError = !sBeats.ToLong(&nBeats) || !sBeatType.ToLong(&nBeatType);

            if (fError) {
                g_pLogger->LogDataError(
                    _T("Time signature %s / %s not supported. Assumed 4 / 4"),
                    sBeats, sBeatType );
                nBeats = 4;
                nBeatType = 4;
            }

            //create the time signature object
            fTimeSignature = true;        //dealy its creation
            
        }

        // <divisions> tag --------------------------------------------------
        else if (sTag == _T("divisions")) {
            int nDivisions = ParseDivisions(pNode);
            m_nCurrentDivisions = nDivisions;
            //! @todo Do something else with divisions
        }

        // Other tags within <attributes> -----------------------------------
        else {
            TagError(sElement, pElement->GetName());
        }

        // Find next sibling 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    }

    // the creation of all objects has been delayed to solve the ordering problem.
    // Now, proceed with its creation in the right order
    if (fKeySignature)  pVStaff->AddKeySignature((int)nFifths, fMajor);
    if (fTimeSignature)  pVStaff->AddTimeSignature((int)nBeats, (int)nBeatType);

    return true;

}

bool lmXMLParser::ParseMusicDataBarline(wxXmlNode* pNode, lmVStaff* pVStaff)
{
    /*
    <!ELEMENT barline (bar-style?, %editorial;, wavy-line?, segno?,
        coda?, (fermata, fermata?)?, ending?, repeat?)>
    <!ATTLIST barline  location (right | left | middle) "right" >

    Bar-style contains style information. Choices are: regular, dotted, heavy, light-light,
    light-heavy, heavy-light, heavy-heavy, and none.
    <!ELEMENT bar-style (#PCDATA)>

    */
    wxString sElement = _T("barline");
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    //default values
    bool fVisible = true;
    EBarline nBarStyle = etb_SimpleBarline;

    bool fError = false;

     //find first child
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
      
    wxString sTag, sDbgTag;
    while (pElement)
    {
        sTag = pElement->GetName();

        // <bar-style> tag ------------------------------------------------------
        if (sTag == _T("bar-style")) {
            wxString sDbgTag = _T(">:<") + sTag;
            wxString sBarStyle = GetText(pElement);

            //Log results
            g_pLogger->LogTrace(_T("lmXMLParser"), 
                _("Parsing <bar-style>: bar_style=%s"), sBarStyle );
            fError = XmlDataToBarStyle(sBarStyle, &nBarStyle);
            //! @todo verify error
        }

        // Other tags within <barline> -----------------------------------
        else {
            TagError(sElement, pElement->GetName());
        }

        // Find next sibling 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    }

    // Add the bar to the score
    pVStaff->AddBarline(nBarStyle, fVisible);
    return true;
}

bool lmXMLParser::ParseMusicDataDirection(wxXmlNode* pNode, lmVStaff* pVStaff)
{
        /* <directions> are StaffObjs associated to a measure
        A direction is a musical indication that is not attached
        to a specific note. Two or more may be combined to
        indicate starts and stops of wedges, dashes, etc.

        <!ELEMENT direction (direction-type+, offset?,
            %editorial-voice;, staff?, sound?)>
        <!ATTLIST direction
            %placement; >

        */

    wxString sElement = _T("direction");
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    enum EDirectionType {
        eNone = 0,
        eWords
    };
    EDirectionType nDirectionType = eNone;

    //common direction data
    long nOffset;
        // <direction> attributes (only one: placement)
    bool fPlacementAbove = (GetAttribute(pNode, _T("placement")) == _T("above") );

    //<words> direction data
    wxString sText;
    wxString sJustify;
    wxString sLanguage;
    lmFontInfo oFontData = goBasicTextDefaultFont;            
    lmLocation tPos;


    //default values
    bool fError = false;
    wxString sStep = _T("C");
    wxString sAlter = _T("0");
    wxString sOctave = _T("4");

     //find first child
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
      
    wxString sTag;
    while (pElement)
    {
        sTag = pElement->GetName();

        //----------------------------------------------------------------------------
        if (sTag == _T("direction-type")) {
            /*
             Textual direction types may have more than 1 component
            due to multiple font numbers.

            <!ELEMENT direction-type (rehearsal+ | segno+ | words+ |
                coda+ | wedge | dynamics+ | dashes | bracket | pedal | 
                metronome | octave-shift | damp | damp-all | 
                eyeglasses | other-direction)>

            */
            wxXmlNode* pChild = GetFirstChild(pNode);
            wxXmlNode* pElmnt = pChild;
            while (pElmnt) {
                wxString sChildName = pElmnt->GetName();
                if (sChildName == _T("words")) {
                    /*
                    <!ELEMENT words (#PCDATA)>

                    Left justification is assumed if not specified. 
                    Language is Italian ("it") by default.

                    <!ATTLIST words
                        justify (left | center | right) #IMPLIED
                        %position; 
                        %font;
                        xml:lang NMTOKEN #IMPLIED    >

                    */
                    sText = GetText(pElmnt);
                    sJustify = GetAttribute(pElmnt, _T("justify"), _T("left"));
                    sLanguage = GetAttribute(pElmnt, _T("xml:lang"), _T("it"));

                    // get font and position info
                    ParseFont(pElmnt, &oFontData);    
                    ParsePosition(pElmnt, &tPos);

                    //! @todo Verify attributes

                    nDirectionType = eWords;
                }
     //           else if (sChildName == _T("alter"))
                    //sAlter = GetText(pElmnt);
     //           else if (sChildName == _T("octave"))
                    //sOctave = GetText(pElmnt);
                else
                    TagError(sElement + _T(">:<direction-type"), sChildName); 

                pChild = GetNextSibling(pChild);        // next sibling 
                pElmnt = pChild;
            }
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("offset")) {
            /*
            direction.dtd
            An offset is represented in terms of divisions, and
            indicates where the direction will appear relative
            to the current musical location. This is for visual
            appearance only; any sound associated with the
            direction takes effect at the current location.

            <!ELEMENT offset (#PCDATA)>

            */
            wxString sOffset = GetText(pElement);

            //Log results
            g_pLogger->LogTrace(_T("lmXMLParser"), 
                _("Parsing <offset>: offset=%s"), sOffset );

            if (!sOffset.IsEmpty()) {
                fError = !sOffset.ToLong(&nOffset);
                //! @todo control error and range
                wxASSERT(!fError);
            }
        }

        //----------------------------------------------------------------------------
        else {
            TagError(sElement, sTag);
        }

        // Find next sibling 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    }

    //create the lmStaffObj
    switch (nDirectionType) {
        case eNone:
            return false;    //nothing added to lmVStaff
            break;
        case eWords:
            pVStaff->AddWordsDirection(sText, lmALIGN_LEFT, &tPos, oFontData, false);
            break;
        default:
            wxASSERT(false);
    }

    return true;

}

bool lmXMLParser::ParseMusicDataNote(wxXmlNode* pNode, lmVStaff* pVStaff)
{
    /*
    Notes = gace note | Cue note | full (normal) note
    Grace notes do not have a duration element.
    Cue notes have a duration element, as do forward elements, but no tie elements.
    <!ELEMENT note 
    ( ( (grace, %full-note;, (tie, tie?)?) |
           (cue, %full-note;, duration) |
        (%full-note;, duration, (tie, tie?)?)),
      instrument?, %editorial-voice;, type?, dot*, accidental?, time-modification?,
      stem?, notehead?, staff?, beam*, notations*, lyric*)>

    <!ENTITY % full-note "(chord?, (pitch | unpitched | rest))">

    */

    wxString sElement = _T("note");
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    //default values
    EAccidentals nAccidentals = eNoAccidentals;
    EStemType nStem = eDefaultStem;
    bool fDotted = false;
    bool fDoubleDotted = false;
    ENoteType nNoteType = eQuarter;
    bool fBeamed = false;
    lmTBeamInfo BeamInfo[6];
    for (int i=0; i < 6; i++) {
        BeamInfo[i].Repeat = false;
        BeamInfo[i].Type = eBeamNone;
    }
    long nNumStaff = 1;
    bool fInChord = false;
    bool fIsRest = false;
    bool fTie = false;
    bool fGraceNote = false;
    bool fCueNote = false;
    int nDuration = 0;

    //lyrics
    bool fLyrics = false;
    AuxObjsList cLyrics;                //list of lyrics associated to this note
    cLyrics.DeleteContents(false);        //do not delete list content when deleting the list object

    //notations
    bool fFermata = false;
    bool fFermataOverNote = true;

    //Tuplet brakets
    bool fEndTuplet = false;


    bool fError = false;
    wxString sStep = _T("C");
    wxString sAlter = _T("0");
    wxString sOctave = _T("4");

     //find first child
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
      
    wxString sTag;
    while (pElement)
    {
        sTag = pElement->GetName();

        //----------------------------------------------------------------------------
        if (sTag == _T("pitch")) {
            /*
            Pitch is represented as a combination of the step of the diatonic scale, 
            the chromatic alteration, and the octave.
            The step element uses the English letters A through G. 
            The alter element represents chromatic alteration in number of semitones
            (e.g., -1 for flat, 1 for sharp). Decimal values like 0.5 (quarter tone sharp)
            may be  used for microtones.
            The octave element is represented by the numbers 0 to 9, where 4 indicates
            the octave started by middle C.

            <!ELEMENT pitch (step, alter?, octave)>
            <!ELEMENT step (#PCDATA)>
            <!ELEMENT alter (#PCDATA)>
            <!ELEMENT octave (#PCDATA)>

            */

            wxXmlNode* pChild = GetFirstChild(pNode);
            wxXmlNode* pElmnt = pChild;
            while (pElmnt) {
                if (pElmnt->GetName() == _T("step"))    // A-G
                    sStep = GetText(pElmnt);
                else if (pElmnt->GetName() == _T("alter"))    // -1=flat 1=sharp (0.5=quarter sharp)
                    sAlter = GetText(pElmnt);
                else if (pElmnt->GetName() == _T("octave"))    // 0-9 4=middle C
                    sOctave = GetText(pElmnt);
                else
                    TagError(sElement + _T(">:<pitch"), pElmnt->GetName()); 
                pChild = GetNextSibling(pChild);        // next sibling 
                pElmnt = pChild;
            }
       }

        //----------------------------------------------------------------------------
        else if (sTag == _T("type")) {
            wxString sType = GetText(pElement);
            if (sType == _T("quarter"))
                nNoteType = eQuarter;
            else if (sType == _T("eighth"))
                nNoteType = eEighth;
            else if (sType == _T("256th"))
                nNoteType = e256th;
            else if (sType == _T("128th"))
                nNoteType = e128th;
            else if (sType == _T("64th"))
                nNoteType = e64th;
            else if (sType == _T("32nd"))
                nNoteType = e32th;
            else if (sType == _T("16th"))
                nNoteType = e16th;
            else if (sType == _T("half"))
                nNoteType = eHalf;
            else if (sType == _T("whole"))
                nNoteType = eWhole;
            else if (sType == _T("breve"))
                nNoteType = eLong;
            //else if (sType == _T("long"))
            //    nNoteType = etn???????;        //! @todo falta esta nota 
            else
                ParseError(
                    _("Parsing <%s>: unknown note type %s"),
                    sElement + _T(">:<pitch"), sType );
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("beam")) {
            /*
            lmBeam types: begin, continue, end, forward hook, and backward hook.
            Up to six concurrent beams are available to cover up to 256th notes.
            The repeater attribute, used for tremolos, needs to be specified with a "yes" 
            value for each beam  using it.
                <!ELEMENT beam (#PCDATA)>
                <!ATTLIST beam number %beam-level; "1" repeater %yes-no; #IMPLIED >
            */
            wxString sValue = GetText(pElement);
            wxString sLevel = GetAttribute(pElement, _T("number"));

            //Log results
            g_pLogger->LogTrace(_T("lmXMLParser"), 
                _("Parsing <beam>: value=%s, beam-level=%s"), sValue, sLevel );

            long nLevel = 1;
            if (!sLevel.IsEmpty()) {
                fError = !sLevel.ToLong(&nLevel);
                //! @todo control error
                wxASSERT(!fError);
                wxASSERT(nLevel > 0 && nLevel < 7);
            }
            nLevel--;

            fBeamed = true;
            if (sValue == _T("begin"))
                BeamInfo[nLevel].Type = eBeamBegin;
            else if (sValue == _T("continue"))
                BeamInfo[nLevel].Type = eBeamContinue;
            else if (sValue == _T("end"))
                BeamInfo[nLevel].Type = eBeamEnd;
            else if (sValue == _T("forward hook"))
                BeamInfo[nLevel].Type = eBeamForward;
            else if (sValue == _T("backward hook"))
                BeamInfo[nLevel].Type = eBeamBackward;
            else
                ParseError(
                    _("Parsing <note.<beam>: unknown beam type %s"),
                    sValue );
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("stem")) {
            /*
            Stems can be down, up, none, or double. For down and up
            stems, the position attributes can be used to specify
            stem length. The relative values specify the end of the
            stem relative to the program default. Default values
            specify an absolute end stem position. Negative values
            of default-y are not permitted. Negative values of
            relative-y that would flip a stem instead of shortening
            it should also be ignored.

            <!ELEMENT stem (#PCDATA)>
            <!ATTLIST stem
                %position; >
            */
            wxString sValue = GetText(pElement);

            if (sValue == _T("none") )
                nStem = eStemNone;
            else if (sValue == _T("up") )
                nStem = eStemUp;
            else if (sValue == _T("down") )
                nStem = eStemDown;
            else if (sValue == _T("double") )
                nStem = eStemDouble;
            else
                ParseError(
                    _("Parsing <note>.<stem>: unknown type %s"),
                    sValue );

            //Log results
            g_pLogger->LogTrace(_T("lmXMLParser"), 
                _("Parsing <stem>: stem type=%s, enum=%d"), sValue, nStem );

        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("staff")) {
            /*
            lmStaff assignment is only needed for music notated on
            multiple staves. Used by both notes and directions. 
            <!ELEMENT staff (#PCDATA)>
            */
            wxString sNumStaff = GetText(pElement);

            //Log results
            g_pLogger->LogTrace(_T("lmXMLParser"), 
                _("Parsing <staff>: num.Staff=%s"), sNumStaff );

            if (!sNumStaff.IsEmpty()) {
                fError = !sNumStaff.ToLong(&nNumStaff);
                //! @todo control error and range
                wxASSERT(!fError);
                wxASSERT(nNumStaff > 0);
            }
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("chord")) {
            /*
            The chord element indicates that this note is an additional chord tone 
            with the preceding note. The duration of this note can be no longer 
            than the preceding note.

            <!ELEMENT chord EMPTY>

            */

            fInChord = true;
            g_pLogger->LogTrace(_T("lmXMLParser"), _("Parsing <chord>"), _T(""));
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("rest")) {
            /*
            The rest element indicates notated rests or silences. Rest are usually empty,
            but placement on the staff can be specified using display-step and 
            display-octave elements.

            <!ELEMENT rest ((display-step, display-octave)?)>

            */

            fIsRest = true;
            g_pLogger->LogTrace(_T("lmXMLParser"), _("Parsing <rest>"), _T(""));
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("grace")) {
            /*
            The cue and grace elements indicate the presence of cue and grace notes.
            The slash attribute for a grace note is yes for slashed eighth notes. 
            The other grace note attributes are sound suggestions:
              - Steal-time-previous indicates the percentage of time to steal from the
                previous note for the grace note.
              - Steal-time-following indicates the percentage of time to steal from the
                following note for the grace note.
              - Make-time indicates to make time, not steal time; the units are in 
                real-time divisions for the grace note. 

            <!ELEMENT grace EMPTY>
            <!ATTLIST grace
                steal-time-previous CDATA #IMPLIED
                steal-time-following CDATA #IMPLIED
                make-time CDATA #IMPLIED
                slash %yes-no; #IMPLIED >

            */

            fGraceNote = true;
            g_pLogger->LogTrace(_T("lmXMLParser"), _("Parsing <grace>"), _T(""));
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("cue")) {
            fCueNote = true;
            g_pLogger->LogTrace(_T("lmXMLParser"), _("Parsing <cue>"), _T(""));
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("tie")) {
            /*
            the note element's attack and release attributes. The 
            tie element indicates that a tie begins or ends with 
            this note. The tie element indicates sound; the tied 
            element indicates notation.

            <!ELEMENT tie EMPTY>
            <!ATTLIST tie
                type %start-stop; #REQUIRED >

            */

            wxString sTieType = GetAttribute(pElement, _T("type"));

            fTie = (sTieType == _T("start"));
            g_pLogger->LogTrace(_T("lmXMLParser"), _("Parsing <tie>. Type=%s"), sTieType );
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("duration")) {
            nDuration = ParseDuration(pElement);
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("notations")) {
            /*
            Notations are musical notations, not XML notations.
            Multiple notations are allowed in order to represent
            multiple editorial levels. The set of notations will be
            refined and expanded over time, especially to handle
            more instrument-specific technical notations.

            <!ELEMENT notations
                (%editorial;, 
                (tied | slur | tuplet | glissando | slide | 
                ornaments | technical | articulations | dynamics |
                fermata | arpeggiate | non-arpeggiate | 
                accidental-mark | other-notation)*)>

            */

            g_pLogger->LogTrace(_T("lmXMLParser"), _("Parsing <notations>"), _T(""));

            wxXmlNode* pChild = GetFirstChild(pNode);
            wxXmlNode* pElmnt = pChild;
            while (pElmnt) {
                wxString sChildName = pElmnt->GetName();

                //.........................................................................
                if (sChildName == _T("fermata")) {
                    wxString sFermataType = GetAttribute(pElmnt, _T("type"));
                    fFermata = true;
                    fFermataOverNote = (sFermataType == _T("upright"));
                    g_pLogger->LogTrace(_T("lmXMLParser"), _("Parsing <fermata>"), _T(""));
                }

                //.........................................................................
                else if (sChildName == _T("tuplet")) {
                    /*
                    A tuplet element is present when a tuplet is to be displayed graphically,
                    in addition to the sound data provided by the time-modification elements.
                    The number attribute is used to distinguish nested tuplets. The
                    bracket attribute is used to indicate the presence of a bracket.
                    If unspecified, the results are implementation-dependent. 
                    
                    Whereas a time-modification element shows how the cumulative, sounding 
                    effect of tuplets compare to the written note type, the tuplet element 
                    describes how this is displayed. The tuplet-actual and tuplet-normal 
                    elements provide optional full control over tuplet specifications. 
                    Each allows the number and note type (including dots) describing a single
                    tuplet. If any of these elements are absent, their values are based on 
                    the time-modification element.
                    
                    The show-number attribute is used to display either the number of actual
                    notes, the number of both actual and normal notes, or neither. It is 
                    actual by default.
                    
                    The show-type attribute is used to display either the actual type, both 
                    the actual and normal types, or neither. It is none by default.

                    <!ELEMENT tuplet (tuplet-actual?, tuplet-normal?)>
                    <!ATTLIST tuplet
                        type %start-stop; #REQUIRED
                        number %number-level; #IMPLIED
                        bracket %yes-no; #IMPLIED
                        show-number (actual | both | none) #IMPLIED
                        show-type (actual | both | none) #IMPLIED
                        %position;
                        %placement;
                    >
                    <!ELEMENT tuplet-actual (tuplet-number?,
                        tuplet-type?, tuplet-dot*)>
                    <!ELEMENT tuplet-normal (tuplet-number?,
                        tuplet-type?, tuplet-dot*)>
                    <!ELEMENT tuplet-number (#PCDATA)>
                    <!ELEMENT tuplet-type (#PCDATA)>
                    <!ELEMENT tuplet-dot EMPTY>

                    */
                    wxString sTupletType = GetAttribute(pElmnt, _T("type"));
                    bool fTupletBracket = GetYesNoAttribute(pElmnt, _T("bracket"), true);
                    bool fShowNumber = true;        //! @todo
                    int nTupletNumber = 3;            //! @todo
                    bool fTupletAbove = ParsePlacement(pElmnt);
                    
                    g_pLogger->LogTrace(_T("lmXMLParser"), 
                        _("Parsing <tuplet>. Type=%s, bracket=%s, above=%s, showNumber=%s, number=%d"),
                        sTupletType,
                        (fTupletBracket ? _T("yes") : _T("no")),
                        (fTupletAbove ? _T("yes") : _T("no")),
                        (fShowNumber ? _T("yes") : _T("no")),
                        nTupletNumber );

                    //Create the tuplet or mark it for termination
                    if (sTupletType == _T("start")) {
                        wxASSERT(!m_pTupletBracket);
                        m_pTupletBracket = new lmTupletBracket(fShowNumber, nTupletNumber, 
                                                            fTupletBracket, fTupletAbove);
                    }
                    else if (sTupletType == _T("stop")) {
                        //signal that bracket must be ended
                        wxASSERT(m_pTupletBracket);
                        fEndTuplet = true;
                    }

                }

                //.........................................................................
                else
                    TagError(sElement + _T(">:<notations"), sChildName); 
                pChild = GetNextSibling(pChild);        // next sibling 
                pElmnt = pChild;
            }

        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("lyric")) {
            /*
            Text underlays for lyrics. The lyric number indicates multiple
            lines, though a name can be used as well. Word extensions are
            represented using the extend element.
            
            Hyphenation is indicated by the syllabic element, which can be single, 
            begin, end, or middle. These represent single-syllable words, word-beginning
            syllables, word-ending syllables, and mid-word syllables.
            
            Multiple syllables on a single note are separated by elision elements. 
            A hyphen in the text element should only be used for an actual hyphenated
            word.
            
            Humming and laughing representations are taken from Humdrum.
            
            The end-line and end-paragraph elements come from RP-017 for Standard MIDI 
            File lyric meta-events; they help facilitate lyric display for Karaoke and
            similar applications.
            
            Language names for text elements come from ISO 639, with optional country 
            subcodes from ISO 3166.

                <!ELEMENT lyric
                    ( ( ( (syllabic?, text), (elision, syllabic?, text)*, extend?) |
                        extend | laughing | humming),
                      end-line?, end-paragraph?, %editorial;)>
                <!ATTLIST lyric
                    number NMTOKEN #IMPLIED
                    name CDATA #IMPLIED
                >

                <!ELEMENT text (#PCDATA)>
                <!ATTLIST text
                    %font;
                    xml:lang NMTOKEN #IMPLIED
                >
                <!ELEMENT syllabic (#PCDATA)>
                <!ELEMENT elision EMPTY>
                <!ELEMENT extend EMPTY>
                <!ELEMENT laughing EMPTY>
                <!ELEMENT humming EMPTY>
                <!ELEMENT end-line EMPTY>
                <!ELEMENT end-paragraph EMPTY>

            */
            ESyllabicTypes nSyllabic = eSyllabicSingle;
            wxString sText = _T("");
            wxString sNumber = GetAttribute(pElement, _T("number"), _T("1"));
            long nNumber;
            bool fError = !sNumber.ToLong(&nNumber);
            wxASSERT(!fError);
            //! @todo Verify number and display error msg
            wxString sName = GetAttribute(pElement, _T("name"), _T(""));

            wxXmlNode* pChild = GetFirstChild(pNode);
            wxXmlNode* pElmnt = pChild;
            while (pElmnt) {
                wxString sChildName = pElmnt->GetName();
                if (sChildName == _T("syllabic")) {
                    wxString sSyllabic = GetText(pElmnt);
                    if (sSyllabic == _T("single"))
                        nSyllabic = eSyllabicSingle;
                    else if (sSyllabic == _T("begin"))
                        nSyllabic = eSyllabicBegin;
                    else if (sSyllabic == _T("end"))
                        nSyllabic = eSyllabicEnd;
                    else if (sSyllabic == _T("middle"))
                        nSyllabic = eSyllabicMiddle;
                    else {
                        ParseError(
                            _("Parsing <note>.<lyric>: unknown syllabic value %s"),
                            sSyllabic );
                        sSyllabic = _T("single");
                    }
                }
                else if (sChildName == _T("text")) {
                    sText = GetText(pElmnt);
                    //! @todo Get language and font
                }
     //           else if (sChildName == _T("elision"))
                    //;    //TODO    sAlter = GetText(pElmnt);
     //           else if (sChildName == _T("extend"))
                    //;    //TODO    sAlter = GetText(pElmnt);
     //           else if (sChildName == _T("laughing"))
                    //;    //TODO    sAlter = GetText(pElmnt);
     //           else if (sChildName == _T("humming"))
                    //;    //TODO    sAlter = GetText(pElmnt);
                else if (sChildName == _T("end-line"))
                    ;    //ignore. Only for Karaoke
                else if (sChildName == _T("end-paragraph"))
                    ;    //ignore. Only for Karaoke
                else
                    TagError(sElement + _T(">:<lyric"), sChildName);

                pChild = GetNextSibling(pChild);        // next sibling 
                pElmnt = pChild;
            }

            // create de lmLyric object
            lmLyric* pLyric = new lmLyric((lmNoteRest*)NULL, sText, nSyllabic, (int)nNumber);

            //Add the lmLyric to the list of lyrics
            fLyrics = true;
            cLyrics.Append(pLyric);

            g_pLogger->LogTrace(_T("lmXMLParser"), _("Parsing <lyric>"), _T(""));
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("accidental")) {
            /*
            Actual notated accidentals. Valid values include: sharp,
            natural, flat, double-sharp, sharp-sharp, flat-flat,
            natural-sharp, natural-flat, quarter-flat, quarter-sharp,
            three-quarters-flat, and three-quarters-sharp. Editorial
            and cautionary indications, are indicated by attributes.
            Values for these attributes are "no" if not present.

            <!ELEMENT accidental (#PCDATA)>
            <!ATTLIST accidental
                cautionary %yes-no; #IMPLIED
                editorial %yes-no; #IMPLIED
                size %symbol-size; #IMPLIED
                %position; >
            */

            wxString sValue = GetText(pElement);
            //wxString sEditorial = GetAttribute(pElement, _T("editorial"));

            if (sValue == _T("natural") )
                nAccidentals = eNatural;
            else if (sValue == _T("flat") )
                nAccidentals = eFlat;
            else if (sValue == _T("sharp") )
                nAccidentals = eSharp;
            else if (sValue == _T("flat-flat") )
                nAccidentals = eFlatFlat;
            else if (sValue == _T("double-sharp") )
                nAccidentals = eDoubleSharp;
            else if (sValue == _T("sharp-sharp") )
                nAccidentals = eSharpSharp;
            else if (sValue == _T("natural-flat") )
                nAccidentals = eNaturalFlat;
            else if (sValue == _T("natural-sharp") )
                nAccidentals = eNaturalSharp;
             else if (sValue == _T("quarter-flat") )
                nAccidentals = eQuarterFlat;
            else if (sValue == _T("quarter-sharp") )
                nAccidentals = eQuarterSharp;
            else if (sValue == _T("three-quarters-flat") )
                nAccidentals = eThreeQuartersFlat;
            else if (sValue == _T("three-quarters-sharp") )
                nAccidentals = eThreeQuartersSharp;
            else
                ParseError(
                    _("Parsing <note>.<accidental>: unknown type %s"),
                    sValue );

            g_pLogger->LogTrace(_T("lmXMLParser"), _("Parsing <accidental>. Type=%s"), sValue );
        }

        //----------------------------------------------------------------------------
        else {
            TagError(sElement, sTag, pElement);
        }

        // Find next sibling 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    }

    //! @todo verify beaming information
    if (g_pLogger->IsAllowedTraceMask(_T("lmXMLParser")) ) {
        wxString sDump = wxString::Format(
            _T("AddNote fBeamed=%s"), (fBeamed ? _T("Y") : _T("N")) );
        if (fBeamed) {
            for (int i=0; i < 6; i++) {
                sDump += wxString::Format(_T(", BeamType[%d]=%d"), i, BeamInfo[i].Type);
            }
        }
        sDump += _T("\n");
        g_pLogger->LogTrace(_T("lmXMLParser"), sDump, _T(""));
    }

    //! @todo Remove this and treat cue and grace notes properly
    //For now ignore grace notes and cue notes
    if (fGraceNote || fCueNote) return false;
    
    lmNoteRest* pNR;
    float rDuration = ((float)nDuration / (float)m_nCurrentDivisions) * XML_DURATION_TO_LDP;
    if (fIsRest) {
        pNR = pVStaff->AddRest(nNoteType, rDuration,
                        fDotted, fDoubleDotted,  
                        nNumStaff, fBeamed, BeamInfo);
    }
    else {
        pNR = pVStaff->AddNote(true,    //absolute pitch
                        sStep, sOctave, sAlter, nAccidentals,
                        nNoteType, rDuration,
                        fDotted, fDoubleDotted,  
                        nNumStaff, fBeamed, BeamInfo, fInChord, fTie,
                        nStem);
    }

    // Add notations
    if (fFermata) pNR->AddFermata(fFermataOverNote);

    if (m_pTupletBracket) {
        m_pTupletBracket->Include(pNR);
        pNR->SetTupletBracket(m_pTupletBracket);

        if (fEndTuplet) {
            m_pTupletBracket = (lmTupletBracket*)NULL;
        }
    }

    // Add lyrics
    if (fLyrics) {
        wxASSERT(cLyrics.GetCount() > 0);
        lmLyric* pLyric = (lmLyric*)NULL;
        wxAuxObjsListNode* pNode = cLyrics.GetFirst();
        for(; pNode; pNode = pNode->GetNext() ) {
            pLyric = (lmLyric*)pNode->GetData();
            pNR->AddLyric(pLyric);
        }
    }

    return true;
}

void lmXMLParser::ParsePartList(wxXmlNode* pNode, lmScore* pScore)
{
    /*
    The part-list identifies the different musical parts in this movement. Each part has
    an ID that is used later within the musical data. Since parts may be encoded
    separately and combined later, identification elements are present at both the score
    and score-part levels. There must be at least one score-part, combined as desired with
    part-group elements that indicate braces and brackets. Parts are ordered from top to
    bottom in a score based on the order in which they appear in the part-list.

    <!ELEMENT part-list (part-group*, score-part,
        (part-group | score-part)*)>

    */
    wxString sElement = _T("part-list");
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    //find first child
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
      
    while (pElement)
    {
        if (pElement->GetName() == _T("score-part")) {
            ParseScorePart(pElement, pScore);
        }
        else {
            TagError(sElement, pElement->GetName());
        }

        // Find next sibling 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    }

}

void lmXMLParser::ParseWork(wxXmlNode* pNode, lmScore* pScore)
{
    /*
    Works and movements are optionally identified by number and title. The work element
    also may indicate a link to the opus document that composes multiple movements
    into a collection.

    <!ELEMENT work (work-number?, work-title?, opus?)>
    <!ELEMENT work-number (#PCDATA)>
    <!ELEMENT work-title (#PCDATA)>
    <!ELEMENT opus EMPTY>
        <!ATTLIST opus
            %link-attributes; >
    */

    wxString sElement = _T("work");
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    wxString sTitle = _T("");
    wxString sNum = _T("");

    //find first child
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
      
    wxString sTag;
    while (pElement)
    {
        sTag = pElement->GetName();

        //----------------------------------------------------------------------------
        if (sTag == _T("work-title")) {
            sTitle = GetText(pElement);
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("work-number")) {
            sNum = GetText(pElement);
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("opus")) {
            ;    //! @todo what to do with the <opus> information?
        }

        //----------------------------------------------------------------------------
        else {
            TagError(sElement, sTag);
        }

        // Find next sibling 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    }

    lmLocation tPos;
    tPos.xType = lmLOCATION_DEFAULT;
    tPos.yType = lmLOCATION_DEFAULT;

    if (sTitle == _T("")) {
        if (sNum != _T(""))
            pScore->AddTitle(sNum, lmALIGN_CENTER, tPos, _T("Times New Roman"), 14, lmTEXT_BOLD);
    }
    else if (sNum != _T("")) {
        sTitle += ", ";
        sTitle += sNum;
        pScore->AddTitle(sTitle, lmALIGN_CENTER, tPos, _T("Times New Roman"), 14, lmTEXT_BOLD);
    }

}

void lmXMLParser::ParseIdentification(wxXmlNode* pNode, lmScore* pScore)
{
    /*
    <identification>
        <creator type="composer">Wolfgang Amadeus Mozart</creator>
        <rights>Copyright © 2003 Recordare LLC</rights>
        <encoding>
            <software>Finale 2003 for Windows</software>
            <encoding-date>2003-03-14</encoding-date>
        </encoding>
    </identification>
    */
    wxString sElement = _T("identification");
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

            //wxXmlNode pNode = pNode->firstChild();
            //while (!pNode->isNull()) {
            //    QDomElement e = pNode->toElement();
            //    if (pElement->isNull())
            //            continue;
            //    if (pElement->GetName() == "creator") {
            //            // type is an arbitrary label
            //            QString type = pElement->attribute(QString("type"));
            //            QString str = pElement->text();
            //            if (type == "composer")
            //                composer = str;
            //            else if (type == "poet")
            //                poet = str;
            //            else if (type == "translator")
            //                translator = str;
            //            else
            //                printf("creator type %s unknown\n", str.latin1());
            //            }
            //    else if (pElement->GetName() == "rights")
            //            cp->rights = pElement->text();
            //    else if (pElement->GetName() == "encoding")
            //            ;
            //    else if (pElement->GetName() == "source")
            //            ;
            //    else
            //            printf("Import MusicXml:scoreParwise:identification: <%s> not supported\n", pElement->GetName().latin1());
            //    pNode = pNode->nextSibling();
            //}

}

wxXmlNode* lmXMLParser::GetNextSibling(wxXmlNode* pNode)
{
    // Return next sibling element or NULL if no more
    pNode = pNode->GetNext();
    while (pNode && pNode->GetType() != wxXML_ELEMENT_NODE)
        pNode = pNode->GetNext();
    return pNode;

}
wxXmlNode* lmXMLParser::GetFirstChild(wxXmlNode* pNode)
{
    // Return first child element or NULL if no more
    pNode = pNode->GetChildren();
    while (pNode && pNode->GetType() != wxXML_ELEMENT_NODE)
        pNode = pNode->GetNext();
    return pNode;
}

wxString lmXMLParser::GetAttribute(wxXmlNode* pNode, wxString sName, wxString sDefault)
{
    wxXmlProperty* pAttrib = pNode->GetProperties();
    while(pAttrib) {
        if (pAttrib->GetName() == sName)
            return pAttrib->GetValue();
        pAttrib = pAttrib->GetNext();
    }

    if (sDefault == _T(""))
        ParseError(
            _("Attribute \"%s\" not found in tag <%s>."),
            sName, pNode->GetName() );

    return sDefault;
}

bool lmXMLParser::GetYesNoAttribute(wxXmlNode* pNode, wxString sName, bool fDefault)
{
    wxXmlProperty* pAttrib = pNode->GetProperties();
    while(pAttrib) {
        if (pAttrib->GetName() == sName) {
            wxString sValue = pAttrib->GetValue();
            if (sValue == _T("yes"))
                return true;
            else if (sValue == _T("no"))
                return false;
            else {
                ParseError(
                    _("Yes-no attribute \"%s\" has an invalid value \"%s\"."),
                    sName, sValue );
                return fDefault;
            }
        }
        pAttrib = pAttrib->GetNext();
    }
    return fDefault;
}

wxString lmXMLParser::GetText(wxXmlNode* pElement)
{
    //    Receives node of type ELEMENT and returns its text content
    wxASSERT(pElement->GetType() == wxXML_ELEMENT_NODE);

    wxXmlNode* pNode = pElement->GetChildren();
    wxString sName = pElement->GetName();
    wxString sValue = _T("");
    
    if (pNode->GetType() == wxXML_TEXT_NODE) {
        sValue = pNode->GetContent();
    }
    return sValue;
}

void lmXMLParser::ParseScorePart(wxXmlNode* pNode, lmScore* pScore)
{
    /*
    <score-part> element contains information about an instrument:

    <!ELEMENT score-part (identification?, part-name,
        part-abbreviation?, group*, score-instrument*,
        midi-device?, midi-instrument*)>
    <!ATTLIST score-part
        id ID #REQUIRED>

    */
    wxString sElement = _T("score-part");
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    //Get part id
    wxString sId = GetAttribute(pNode, _T("id"));
    if (sId.IsEmpty()) return;

    // create one instrument with empty VStaves
    g_pLogger->LogTrace(_T("lmXMLParser"), _T("Procesing score-part id = "), sId);
    long nVStaves=1;
    int nMIDIChannel=0, nMIDIInstr=0;        //dbg
    lmInstrument* pInstr = pScore->AddInstrument(nVStaves, nMIDIChannel, nMIDIInstr);
    pInstr->XML_SetId(sId);

    //while (!node.isNull()) {
    //    QDomElement e = node.toElement();
    //    if (e.isNull())
    //            continue;
    //    if (e.tagName() == "part-name")
    //            instrument->setName(e.text());
    //    else if (e.tagName() == "part-abbreviation")
    //            ;
    //    else if (e.tagName() == "score-instrument") {
    //            QDomNode n = node.firstChild();
    //            while (!n.isNull()) {
    //                QDomElement e = n.toElement();
    //                if (e.isNull())
    //                        continue;
    //                if (e.tagName() == "instrument-name")
    //                        instrument->setInstrumentName(e.text());
    //                else
    //                        printf("Import MusicXml:scorePart:score-instrument: <%s> not supported\n", e.tagName().latin1());
    //                n = n.nextSibling();
    //                }
    //            }
    //    else if (e.tagName() == "midi-instrument") {
    //            QDomNode n = node.firstChild();
    //            while (!n.isNull()) {
    //                QDomElement e = n.toElement();
    //                if (e.isNull())
    //                        continue;
    //                if (e.tagName() == "midi-channel")
    //                        instrument->setMidiChannel(e.text().toInt());
    //                else if (e.tagName() == "midi-program")
    //                        instrument->setMidiProgram(e.text().toInt());
    //                else
    //                        printf("Import MusicXml:scoreParwise:midi-instrument:: <%s> not supported\n", e.tagName().latin1());
    //                n = n.nextSibling();
    //                }
    //            }
    //    else
    //            printf("Import MusicXml:xmlScorePart: %s not supported\n", e.tagName().latin1());
    //    node = node.nextSibling();
    //    }
    //cp->appendInstrument(instrument);
}


//----------------------------------------------------------------------------------------
// common.dtd
//----------------------------------------------------------------------------------------
void lmXMLParser::ParsePosition(wxXmlNode* pElement, lmLocation* pPos)
{
    /*
    For most elements, any program will compute a default x and y position. 
    The position attribute lets this be changed two ways.
    
    The default-x and default-y attributes change the computation of the default
    position. The origin becomes the left-hand side of the note or the musical position 
    within the bar (x) and the top line of the staff (y).
    
    The relative-x and relative-y attributes change the position relative to
    the default position, either as computed by the individual program, or as overridden
    by the default-x and default-y attributes.
    
    * Positive x is right, negative x is left; positive y is up, negative y is down.
    * All units are in tenths of interline space.
    * Positions can be applied to notes, notations, directions, and stems. 
    * For stems, positive y lengthens a stem while negative y shortens it.
    * Negative values for default-y are not allowed.

    <!ENTITY % position
        "default-x     %tenths;    #IMPLIED
        default-y     %tenths;    #IMPLIED
        relative-x    %tenths;    #IMPLIED
        relative-y    %tenths;    #IMPLIED">
    */
    wxString sXDef = GetAttribute(pElement, _T("default-x"), _T("NoData"));
    wxString sYDef = GetAttribute(pElement, _T("default-y"), _T("NoData"));
    wxString sXRel = GetAttribute(pElement, _T("relative-x"), _T("NoData"));
    wxString sYRel = GetAttribute(pElement, _T("relative-y"), _T("NoData"));

    long nValue=0;
    bool fError = false;

    //initialization with defaults
    pPos->x=0;
    pPos->y = 0;
    pPos->xUnits = lmTENTHS;
    pPos->yUnits = lmTENTHS;
    pPos->xType = lmLOCATION_RELATIVE;
    pPos->yType = lmLOCATION_RELATIVE;

    // default-x (Absolute position)
    if (sXDef != _T("NoData")) {
        pPos->xType = lmLOCATION_ABSOLUTE;
        fError = !sXDef.ToLong(&nValue);
        //! @todo control error and range
        wxASSERT(!fError);
        pPos->x = (int)nValue;
    }

    // default-y  (Absolute position)
    if (sYDef != _T("NoData")) {
        pPos->yType = lmLOCATION_ABSOLUTE;
        fError = !sYDef.ToLong(&nValue);
        //! @todo control error and range
        wxASSERT(!fError);
        pPos->y = (int)nValue;
    }

    // relative-x (relative position)
    if (sXRel != _T("NoData")) {
        pPos->xType = lmLOCATION_RELATIVE;
        fError = !sXRel.ToLong(&nValue);
        //! @todo control error and range
        wxASSERT(!fError);
        pPos->x = (int)nValue;
    }

    // relative-y (relative position)
    if (sYRel != _T("NoData")) {
        pPos->yType = lmLOCATION_RELATIVE;
        fError = !sYRel.ToLong(&nValue);
        //! @todo control error and range
        wxASSERT(!fError);
        pPos->y = -(int)nValue;     // reverse sign as, for LenMus, positive y is down, negative y is up
        // as relative-y refers to the top line of the staff, so 5 lines must be 
        // substracted from yBase position 
        pPos->y -= 50;      //50 tenths = 5 lines
   }

}

void lmXMLParser::ParseFont(wxXmlNode* pElement, lmFontInfo* pFontData)
{
    /*
    The font entity gathers together attributes for determining the font within a 
    directive or direction.
    They are based on the text styles for Cascading Style Sheets.
    The font-family can be music, serif, or sans-serif.
    The font-style can be normal or italic.
    The font-size can be one of the CSS sizes (xx-small, x-small, small, medium, 
    large, x-large, xx-large) or a numeric point size.
    The font-weight can be normal or bold.
    The default is the application-dependent, but is a text font vs. a music font.

    <!ENTITY % font
        "font-family  CDATA  #IMPLIED
        font-style   CDATA  #IMPLIED
        font-size    CDATA  #IMPLIED
        font-weight  CDATA  #IMPLIED" >
    
    */

    //! @todo font-family and font-size
    wxString sValue;
    wxString sName = GetAttribute(pElement, _T("font-family"), _T("NoData"));
    wxString sSize = GetAttribute(pElement, _T("font-size"), _T("12"));
    pFontData->nFontSize = 12;
    pFontData->sFontName = _T("Arial");

    //! @todo verify values
    //weight: normal or bold
    sValue = GetAttribute(pElement, _T("font-weight"), _T("normal"));
    bool fBold = (sValue == _T("bold"));

    //style: normal or italic
    sValue = GetAttribute(pElement, _T("font-style"), _T("normal"));
    bool fItalic = (sValue == _T("italic"));

    if (fBold && fItalic)
        pFontData->nStyle = lmTEXT_ITALIC_BOLD;
    else if (fBold)
        pFontData->nStyle = lmTEXT_BOLD;
    else if (fItalic)
        pFontData->nStyle = lmTEXT_ITALIC;
    else
        pFontData->nStyle = lmTEXT_NORMAL;

}


int lmXMLParser::ParseDuration(wxXmlNode* pElement)
{
    /*
    Duration is specified in division units. This is the intended duration vs. notated
    duration (for instance, swing eighths vs. even eighths, or differences in dotted
    notes in Baroque-era music). Differences in duration specific to an interpretation
    or performance should use the note element's attack and release attributes.
    
    <!ELEMENT duration (#PCDATA)>

    */
    wxString sDuration = GetText(pElement);
    long nDuration = 0;
    g_pLogger->LogTrace(_T("lmXMLParser"), _("Parsing <duration>: duration=%s"), sDuration );
    bool fError = !sDuration.ToLong(&nDuration);
    wxASSERT(!fError);
    //! @todo display error message
    return (int)nDuration;

}

int lmXMLParser::ParseDivisions(wxXmlNode* pElement)
{
    /*
    Musical notation duration is commonly represented as fractions. The divisions 
    element indicates how many divisions per quarter note are used to indicate a note's
    duration. For example, if duration = 1 and divisions = 2, this is an eighth note 
    duration. Duration and divisions are used directly for generating sound output, so 
    they must be chosen to take tuplets into account. Using a divisions element lets 
    us use just one number to represent a duration for each note in the score, while
    retaining the full power of a fractional representation. For maximum compatibility
    with Standard MIDI Files, the divisions value should not exceed 16383.

    <!ELEMENT divisions (#PCDATA)>

    */
    wxString sDivisions = GetText(pElement);
    long nDivisions = 0;
    g_pLogger->LogTrace(_T("lmXMLParser"), _("Parsing <divisions>: divisions=%s"), sDivisions );
    bool fError = !sDivisions.ToLong(&nDivisions);
    wxASSERT(!fError);
    //! @todo display error message
    return (int)nDivisions;

}

bool lmXMLParser::ParsePlacement(wxXmlNode* pElement, bool fDefault)
{
    /*
    Returns true if placement above

    The placement attribute indicates whether something is above or below another
    element, such as a note or a notation.

    <!ENTITY % placement
        "placement (above | below) #IMPLIED">

    */
    wxXmlProperty* pAttrib = pElement->GetProperties();
    while(pAttrib) {
        if (pAttrib->GetName() == _T("placement")) {
            wxString sValue = pAttrib->GetValue();
            g_pLogger->LogTrace(_T("lmXMLParser"), 
                _("Parsing <placement>: placement='%s'>"), sValue );
            if (sValue == _T("above"))
                return true;
            else if (sValue == _T("below"))
                return false;
            else {
                ParseError(
                    _("<placement> element has an invalid value \"%s\". Asumed 'above'"),
                    sValue );
                return fDefault;
            }
        }
        pAttrib = pAttrib->GetNext();
    }
    return fDefault;

}

