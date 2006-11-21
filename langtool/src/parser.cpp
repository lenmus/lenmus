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

//-------------------------------------------------------------------------------------
// eBooks structure
// ----------------
//  eBooks are structured as books. They are writen in XML using a subset of DocBook.
//  Each eBook must be fully containes in a folder.
//  Each displayed subject must be a chapter. And must be contained in a different
//  xml file. A chapter can not be splitted into several files and a file can not
//  contain more than a chapter.
//
// Coversion to htb format
// -----------------------
// The hhc, hhk and hhp files will be named after the eBook directory.
// The htm files will be named by changing the xml extension.
// 
//-------------------------------------------------------------------------------------------

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
#include "wx/dir.h"             // to search directories
#include "wx/filename.h"

#include "parser.h"


lmXmlParser::lmXmlParser()
{
}

lmXmlParser::~lmXmlParser()
{
}

wxXmlNode* lmXmlParser::GetNextSibling(wxXmlNode* pNode)
{
    // Return next sibling element or NULL if no more
    pNode = pNode->GetNext();
    while (pNode && pNode->GetType() != wxXML_ELEMENT_NODE)
        pNode = pNode->GetNext();
    return pNode;

}
wxXmlNode* lmXmlParser::GetFirstChild(wxXmlNode* pNode)
{
    // Return first child element or NULL if no more
    pNode = pNode->GetChildren();
    while (pNode && pNode->GetType() != wxXML_ELEMENT_NODE)
        pNode = pNode->GetNext();
    return pNode;
}

wxString lmXmlParser::GetAttribute(wxXmlNode* pNode, wxString sName, wxString sDefault)
{
    wxXmlProperty* pAttrib = pNode->GetProperties();
    while(pAttrib) {
        if (pAttrib->GetName() == sName)
            return pAttrib->GetValue();
        pAttrib = pAttrib->GetNext();
    }

    if (sDefault == _T(""))
        ParseError(
            _T("Attribute \"%s\" not found in tag <%s>."),
            sName, pNode->GetName() );

    return sDefault;
}

bool lmXmlParser::GetYesNoAttribute(wxXmlNode* pNode, wxString sName, bool fDefault)
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
                    _T("Yes-no attribute \"%s\" has an invalid value \"%s\"."),
                    sName, sValue );
                return fDefault;
            }
        }
        pAttrib = pAttrib->GetNext();
    }
    return fDefault;
}

wxString lmXmlParser::GetText(wxXmlNode* pElement)
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

void lmXmlParser::DumpXMLTree(wxXmlNode *pRoot)
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

void lmXmlParser::ParseError(const wxChar* szFormat, ...)
{
    m_nErrors++;
    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg = wxString::FormatV(szFormat, argptr);
    wxLogMessage(sMsg);
    va_end(argptr);
}

void lmXmlParser::TagError(const wxString sElement, const wxString sTagName, wxXmlNode* pElement)
{
    m_nErrors++;
    wxString sMsg = wxString::Format(
        _T("Parsing <%s>: tag <%s> not supported."),
        sElement, sTagName);
    wxLogMessage(sMsg);

    if (pElement) {
        //! @todo Log source code or reference to source line. But how?
        //wxLogMessage(pElement->));
    }
}


//---------------------------------------------------------------------------------------

void lmXmlParser::ParseBook(const wxString& sPath) 
{

    // scan all files in selected directory
    wxLogMessage(_T("Scanning path <%s>"), sPath);
    wxDir dir(sPath);
    if ( !dir.IsOpened() ) {
        // TODO: deal with the error here - wxDir would already log an error message
        // explaining the exact reason of the failure
        wxMessageBox(_T("Error when trying to move to folder %s"), sPath );
        return;
    }

    wxLogMessage(_T("Enumerating .xml files in directory: %s"), sPath);
    wxString sFilename;
    bool fFound = dir.GetFirst(&sFilename, _T("*.xml"), wxDIR_FILES);
    while (fFound) {
        wxLogMessage(_T("Found %s"), sFilename);
        wxFileName oFilename(sPath, sFilename, wxPATH_NATIVE);
        //process file
        ParseFile(oFilename.GetFullPath());
        fFound = dir.GetNext(&sFilename);
    }

    //create files
    wxFileName oFN(sPath);
    wxString sBookName = oFN.GetName();
    WriteHHC(sBookName);
    WriteHHK(sBookName);
    WriteHHP(sBookName);

}

void lmXmlParser::ParseFile(const wxString& filename) 
{
    wxLogMessage(_T("Processing DocBook file %s\n\n"), filename);

    // load the XML file as tree of nodes
    wxXmlDocument xdoc;
    if (!xdoc.Load(filename))
    {
        wxLogMessage(_T("Error parsing DocBook file ") + filename);
        return;
    }

    //Verify type of document. Must be <book>
    wxXmlNode *pRoot = xdoc.GetRoot();
    if (pRoot->GetName() != _T("book")) {
        wxLogMessage(
            _T("Error. First tag is not <book> but <%s>"),
            pRoot->GetName() );
        return;
    }
    
    m_nErrors = 0;
    
    wxFileName oFN( filename );
    oFN.SetExt(_T("htm"));
    CreateBookIndex(pRoot, oFN.GetFullName());

    // report errors
    wxLogMessage(_T("\nHTML file processed OK. There are %d warnings."), m_nErrors);

    // Dump doc tree
    //wxLogMessage(_T("*** DUMP OF XML TREE ***:"));
    //wxXmlNode *pRoot = xdoc.GetRoot();
    //wxLogMessage(_T("Root element: [%s]"), pRoot->GetName());
    //DumpXMLTree(pRoot);
    //wxLogMessage(_T("*** END OF DUMP ***"));

}

bool lmXmlParser::CreateBookIndex(wxXmlNode* pNode, const wxString& sFilename)
{
    // receives a <book> node.
    // return true if error

    wxString sTag = _T("book");
    wxString sElement = pNode->GetName();
    wxLogMessage(_T("XML Parser: Entering parser for %s"), sElement);
    wxASSERT(sTag == sElement);

    //find first child. Should be <bookinfo>
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    sElement = pElement->GetName();
    sTag = _T("bookinfo");
    if (sElement != sTag ) {
        wxLogMessage(_T("Error: Expected <%s> but found <%s>"), sTag, sElement);
        return true;
    }

    //get book title
    m_sBookTitle = GetTitle(pElement);

    //get first sibling. Should be <chapter>
    pNode = GetNextSibling(pNode);
    pElement = pNode;
    sTag = _T("chapter");
    
    while (pElement) {
        sElement = pElement->GetName();
        sTag = _T("chapter");
        if (sElement != sTag) {
            wxLogMessage(_T("Error: Expected <%s> but found <%s>"), sTag, sElement);
            return true;
        }
        //get chapter title
        m_cChapterTitle.Add( GetTitle(pElement) );
        m_cFileName.Add( sFilename );

        // Find next sibling. Should be <chapter> 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    }

    return false;   //no error

}

wxString lmXmlParser::GetTitle(wxXmlNode* pNode)
{
    wxString sElement = pNode->GetName();
    wxLogMessage(_T("XML Parser: Entering GetTitle() for %s"), sElement);

    //locate <title> child
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    wxString sTag = _T("title");
    while (pElement)
    {
        sElement = pElement->GetName();
        if (sElement == sTag) {
            return GetText(pElement);
        }

        // Find next sibling
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    }

    return _T("");

}


void lmXmlParser::WriteHHC(const wxString& sBookname, const wxString& sExt)
{
    wxString sPath = sBookname + sExt;
    wxFile* pFile = new wxFile(sPath, wxFile::write);
    if (!pFile->IsOpened()) {
        wxLogMessage(_T("Error: File %s can not be created"), sPath);
        return;
    }

    wxString sNil = _T("");
    wxString sCharset = _T("utf-8");
    wxString sHHC = sNil +
        _T("<html>\n<head>\n")
        _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") + sCharset +
        _T("\">\n")
        _T("</head>\n<body>\n\n<ul>\n");
    pFile->Write(sHHC);

    for (int i=0; i < (int)m_cChapterTitle.GetCount(); i++) {
        sHHC =
            _T("  <li><object type=\"text/sitemap\">\n")
            _T("      <param name=\"Name\" value=\"") +           //this is the name for links
            m_cChapterTitle.Item(i)  + _T("\">\n")
            _T("      <param name=\"Local\" value=\"") +
            m_cFileName.Item(i) +
            _T("\"></object></li>\n");
        pFile->Write(sHHC);
    }
    pFile->Write( _T("</ul>\n\n</body>\n</html>\n") );

    pFile->Close();
    delete pFile;

}

void lmXmlParser::WriteHHK(const wxString& sBookname)
{
    WriteHHC(sBookname, _T(".hhk"));
}

void lmXmlParser::WriteHHP(const wxString& sBookname)
{
    wxString sPath = sBookname + _T(".hhp");
    wxFile* pFile = new wxFile(sPath, wxFile::write);
    if (!pFile->IsOpened()) {
        wxLogMessage(_T("Error: File %s can not be created"), sPath);
        return;
    }

    wxString sNil = _T("");
    wxString sCharset = _T("utf-8");
    wxString sHHP = sNil +
        _T("[OPTIONS]\n")
        _T("Contents file=") + sBookname +_T(".hhc\n")
        _T("Charset=") + sCharset + _T("\n")
        _T("Index file=") + sBookname +_T(".hhk\n")
        _T("Title=") + m_sBookTitle + _T("\n")
        _T("Default topic=") + m_cFileName.Item(0) + _T("\n");
    
    pFile->Write(sHHP);

    pFile->Close();
    delete pFile;
}

/*
void lmXmlParser::ParseScorePartwise(wxXmlNode* pNode, lmScore* pScore)
{
    wxString sElement = _T("score-partwise");
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("--> XML Parser: Entering parser for %s"), sElement);
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
            g_pLogger->LogTrace(_T("lmXmlParser"), _T("movement-number not yet implemented"), _T(""));
        }
        else if (pElement->GetName() == _T("movement-title")) {
            //! @todo
            g_pLogger->LogTrace(_T("lmXmlParser"), _T("movement-title not yet implemented"), _T(""));
        } else {
            TagError(sElement, pElement->GetName(), pElement);
        }

        // Find next sibling 
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    
    }

}

void lmXmlParser::ParsePart(wxXmlNode* pNode, lmScore* pScore)
{
    wxString sElement = _T("part");
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    //Get part id
    wxString sId = GetAttribute(pNode, _T("id"));
    if (sId.IsEmpty()) return;
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Procesing part id=%s"), sId);

    // At this point all score instruments have been created (in score-part).
    // Find the instrument that corresponds to this part
    lmInstrument* pInstr = pScore->XML_FindInstrument(sId);
    if (!pInstr) {
        ParseError(_T("Part id = %s not defined in <part-list>"), sId);
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

void lmXmlParser::ParseMeasure(wxXmlNode* pNode,     lmVStaff* pVStaff)
{
    wxString sElement = _T("measure");
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Entering parser for %s"), sElement);
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

float lmXmlParser::ParseMusicDataBackupForward(wxXmlNode* pNode, lmVStaff* pVStaff)
{
    wxString sElement = pNode->GetName();
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Entering parser for %s"), sElement);
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
    g_pLogger->LogTrace(_T("lmXmlParser"), 
        _T("Parsing <%s>: duration=%d, timeShift=%f"), sElement, nDuration, rShift );
    return rShift;
      
}

bool lmXmlParser::ParseMusicDataAttributes(wxXmlNode* pNode, lmVStaff* pVStaff)
{
    wxString sElement = _T("attributes");
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    bool fError = false;

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
            g_pLogger->LogTrace(_T("lmXmlParser"), 
                _T("Parsing <clef>: staff_number=%s, clef=%s, line=%s"),
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
            g_pLogger->LogTrace(_T("lmXmlParser"), 
                _T("Parsing <staves>: num_staves=%s"), sNumStaves );

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
            g_pLogger->LogTrace(_T("lmXmlParser"), 
                _T("Parsing <key>: fifths=%s, mode=%s"), sFifths, sMode );

            //! @todo Change this for dealing with non-traditional key signatures
            fMajor = (sMode == _T("major"));
            nFifths = 0;
            bool fError = !sFifths.ToLong(&nFifths);

            if (fError) {
                wxLogMessage(
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
            g_pLogger->LogTrace(_T("lmXmlParser"), 
                _T("Parsing <time>: beats=%s, beat-type=%s"), sBeats, sBeatType );

            //! @todo Change this for a more general treatment
            bool fError = !sBeats.ToLong(&nBeats) || !sBeatType.ToLong(&nBeatType);

            if (fError) {
                wxLogMessage(
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

bool lmXmlParser::ParseMusicDataBarline(wxXmlNode* pNode, lmVStaff* pVStaff)
{
    wxString sElement = _T("barline");
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Entering parser for %s"), sElement);
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
            g_pLogger->LogTrace(_T("lmXmlParser"), 
                _T("Parsing <bar-style>: bar_style=%s"), sBarStyle );
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

bool lmXmlParser::ParseMusicDataDirection(wxXmlNode* pNode, lmVStaff* pVStaff)
{
    wxString sElement = _T("direction");
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Entering parser for %s"), sElement);
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
    lmFontInfo oFontData = tBasicTextDefaultFont;            
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
            wxXmlNode* pChild = GetFirstChild(pNode);
            wxXmlNode* pElmnt = pChild;
            while (pElmnt) {
                wxString sChildName = pElmnt->GetName();
                if (sChildName == _T("words")) {
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
            wxString sOffset = GetText(pElement);

            //Log results
            g_pLogger->LogTrace(_T("lmXmlParser"), 
                _T("Parsing <offset>: offset=%s"), sOffset );

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

bool lmXmlParser::ParseMusicDataNote(wxXmlNode* pNode, lmVStaff* pVStaff)
{

    wxString sElement = _T("note");
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Entering parser for %s"), sElement);
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
                nNoteType = eBreve;
            else if (sType == _T("long"))
                nNoteType = eLonga;
            else
                ParseError(
                    _T("Parsing <%s>: unknown note type %s"),
                    sElement + _T(">:<pitch"), sType );
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("beam")) {
            wxString sValue = GetText(pElement);
            wxString sLevel = GetAttribute(pElement, _T("number"));

            //Log results
            g_pLogger->LogTrace(_T("lmXmlParser"), 
                _T("Parsing <beam>: value=%s, beam-level=%s"), sValue, sLevel );

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
                    _T("Parsing <note.<beam>: unknown beam type %s"),
                    sValue );
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("stem")) {
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
                    _T("Parsing <note>.<stem>: unknown type %s"),
                    sValue );

            //Log results
            g_pLogger->LogTrace(_T("lmXmlParser"), 
                _T("Parsing <stem>: stem type=%s, enum=%d"), sValue, nStem );

        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("staff")) {
            wxString sNumStaff = GetText(pElement);

            //Log results
            g_pLogger->LogTrace(_T("lmXmlParser"), 
                _T("Parsing <staff>: num.Staff=%s"), sNumStaff );

            if (!sNumStaff.IsEmpty()) {
                fError = !sNumStaff.ToLong(&nNumStaff);
                //! @todo control error and range
                wxASSERT(!fError);
                wxASSERT(nNumStaff > 0);
            }
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("chord")) {
            fInChord = true;
            g_pLogger->LogTrace(_T("lmXmlParser"), _T("Parsing <chord>"), _T(""));
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("rest")) {
            fIsRest = true;
            g_pLogger->LogTrace(_T("lmXmlParser"), _T("Parsing <rest>"), _T(""));
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("grace")) {
            fGraceNote = true;
            g_pLogger->LogTrace(_T("lmXmlParser"), _T("Parsing <grace>"), _T(""));
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("cue")) {
            fCueNote = true;
            g_pLogger->LogTrace(_T("lmXmlParser"), _T("Parsing <cue>"), _T(""));
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("tie")) {
            wxString sTieType = GetAttribute(pElement, _T("type"));

            fTie = (sTieType == _T("start"));
            g_pLogger->LogTrace(_T("lmXmlParser"), _T("Parsing <tie>. Type=%s"), sTieType );
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("duration")) {
            nDuration = ParseDuration(pElement);
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("notations")) {
            g_pLogger->LogTrace(_T("lmXmlParser"), _T("Parsing <notations>"), _T(""));

            wxXmlNode* pChild = GetFirstChild(pNode);
            wxXmlNode* pElmnt = pChild;
            while (pElmnt) {
                wxString sChildName = pElmnt->GetName();

                //.........................................................................
                if (sChildName == _T("fermata")) {
                    wxString sFermataType = GetAttribute(pElmnt, _T("type"));
                    fFermata = true;
                    fFermataOverNote = (sFermataType == _T("upright"));
                    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Parsing <fermata>"), _T(""));
                }

                //.........................................................................
                else if (sChildName == _T("tuplet")) {
                    wxString sTupletType = GetAttribute(pElmnt, _T("type"));
                    bool fTupletBracket = GetYesNoAttribute(pElmnt, _T("bracket"), true);
                    bool fShowNumber = true;        //! @todo
                    int nTupletNumber = 3;            //! @todo
                    bool fTupletAbove = ParsePlacement(pElmnt);
                    
                    g_pLogger->LogTrace(_T("lmXmlParser"), 
                        _T("Parsing <tuplet>. Type=%s, bracket=%s, above=%s, showNumber=%s, number=%d"),
                        sTupletType,
                        (fTupletBracket ? _T("yes") : _T("no")),
                        (fTupletAbove ? _T("yes") : _T("no")),
                        (fShowNumber ? _T("yes") : _T("no")),
                        nTupletNumber );

                    //Create the tuplet or mark it for termination
                    if (sTupletType == _T("start")) {
                        wxASSERT(!m_pTupletBracket);
                        m_pTupletBracket = new lmTupletBracket(fShowNumber, nTupletNumber, 
                                fTupletBracket, fTupletAbove, nTupletNumber, nTupletNumber);
                        //! @todo Get nActualNotes and nNormalNotes
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
                            _T("Parsing <note>.<lyric>: unknown syllabic value %s"),
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

            g_pLogger->LogTrace(_T("lmXmlParser"), _T("Parsing <lyric>"), _T(""));
        }

        //----------------------------------------------------------------------------
        else if (sTag == _T("accidental")) {
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
                    _T("Parsing <note>.<accidental>: unknown type %s"),
                    sValue );

            g_pLogger->LogTrace(_T("lmXmlParser"), _T("Parsing <accidental>. Type=%s"), sValue );
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
    if (g_pLogger->IsAllowedTraceMask(_T("lmXmlParser")) ) {
        wxString sDump = wxString::Format(
            _T("AddNote fBeamed=%s"), (fBeamed ? _T("Y") : _T("N")) );
        if (fBeamed) {
            for (int i=0; i < 6; i++) {
                sDump += wxString::Format(_T(", BeamType[%d]=%d"), i, BeamInfo[i].Type);
            }
        }
        sDump += _T("\n");
        g_pLogger->LogTrace(_T("lmXmlParser"), sDump, _T(""));
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

void lmXmlParser::ParsePartList(wxXmlNode* pNode, lmScore* pScore)
{
    wxString sElement = _T("part-list");
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Entering parser for %s"), sElement);
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

void lmXmlParser::ParseWork(wxXmlNode* pNode, lmScore* pScore)
{
    wxString sElement = _T("work");
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Entering parser for %s"), sElement);
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
        sTitle += _T(", ");
        sTitle += sNum;
        pScore->AddTitle(sTitle, lmALIGN_CENTER, tPos, _T("Times New Roman"), 14, lmTEXT_BOLD);
    }

}

void lmXmlParser::ParseIdentification(wxXmlNode* pNode, lmScore* pScore)
{
    wxString sElement = _T("identification");
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Entering parser for %s"), sElement);
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

void lmXmlParser::ParseScorePart(wxXmlNode* pNode, lmScore* pScore)
{
    wxString sElement = _T("score-part");
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Entering parser for %s"), sElement);
    wxASSERT(pNode->GetName() == sElement);

    //Get part id
    wxString sId = GetAttribute(pNode, _T("id"));
    if (sId.IsEmpty()) return;

    // create one instrument with empty VStaves
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Procesing score-part id = "), sId);
    long nVStaves=1;
    int nMIDIChannel=0, nMIDIInstr=0;        //dbg
    lmInstrument* pInstr = pScore->AddInstrument(nVStaves, nMIDIChannel, nMIDIInstr, _T(""));
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
void lmXmlParser::ParsePosition(wxXmlNode* pElement, lmLocation* pPos)
{
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

void lmXmlParser::ParseFont(wxXmlNode* pElement, lmFontInfo* pFontData)
{
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


int lmXmlParser::ParseDuration(wxXmlNode* pElement)
{
    wxString sDuration = GetText(pElement);
    long nDuration = 0;
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Parsing <duration>: duration=%s"), sDuration );
    bool fError = !sDuration.ToLong(&nDuration);
    wxASSERT(!fError);
    //! @todo display error message
    return (int)nDuration;

}

int lmXmlParser::ParseDivisions(wxXmlNode* pElement)
{
    wxString sDivisions = GetText(pElement);
    long nDivisions = 0;
    g_pLogger->LogTrace(_T("lmXmlParser"), _T("Parsing <divisions>: divisions=%s"), sDivisions );
    bool fError = !sDivisions.ToLong(&nDivisions);
    wxASSERT(!fError);
    //! @todo display error message
    return (int)nDivisions;

}

bool lmXmlParser::ParsePlacement(wxXmlNode* pElement, bool fDefault)
{
    wxXmlProperty* pAttrib = pElement->GetProperties();
    while(pAttrib) {
        if (pAttrib->GetName() == _T("placement")) {
            wxString sValue = pAttrib->GetValue();
            g_pLogger->LogTrace(_T("lmXmlParser"), 
                _T("Parsing <placement>: placement='%s'>"), sValue );
            if (sValue == _T("above"))
                return true;
            else if (sValue == _T("below"))
                return false;
            else {
                ParseError(
                    _T("<placement> element has an invalid value \"%s\". Asumed 'above'"),
                    sValue );
                return fDefault;
            }
        }
        pAttrib = pAttrib->GetNext();
    }
    return fDefault;

}

*/

