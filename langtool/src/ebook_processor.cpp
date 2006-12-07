//--------------------------------------------------------------------------------------
//    LenMus project: free software for music theory and language
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
#ifdef __GNUG__
#pragma implementation ebook_processor.h
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/txtstrm.h"
#include "wx/zipstrm.h"

#include "ebook_processor.h"
#include "wx/xml2.h"


#define ltNO_INDENT false

enum {
    eTOC = 1,
    eHTML = 2,
    eIDX = 4,
    eTRANSLATE = 8
};

lmEbookProcessor::lmEbookProcessor()
{
    m_pTocFile = (wxFile*) NULL;
    m_pHtmlFile = (wxFile*) NULL;
    m_pPoFile = (wxFile*)NULL;
    m_pLmbFile = (wxTextOutputStream*)NULL;
    m_pZipFile = (wxZipOutputStream*)NULL;

    //options. TODO: dialog to change options
    m_fGenerateLmb = true;

}

lmEbookProcessor::~lmEbookProcessor()
{
    if (m_pTocFile) delete m_pTocFile;
    if (m_pHtmlFile) delete m_pHtmlFile;
    if (m_pPoFile) delete m_pPoFile;
    if (m_pLmbFile) delete m_pLmbFile;
    if (m_pZipFile) delete m_pZipFile;

}

bool lmEbookProcessor::GenerateLMB(wxString sFilename, int nOptions)
{
    // returns false if error

    // Prepare for a new XML file processing
    m_fProcessingBookinfo = false;
    m_fOnlyPoFile = nOptions & lmPO_FILE;

    // load the XML file as tree of nodes
    wxXml2Document oDoc;
    wxString sError;
    if (!oDoc.Load(sFilename, &sError)) {
        wxLogMessage(_T("Error parsing file %s\nError:%s"), sFilename, sError);
        wxFile *  pFile = new wxFile(_T("ErrorXML.txt"), wxFile::write);
        wxString str(sError, *wxConvCurrent);
        pFile->Write(sError);
        pFile->Close();
        delete pFile;

        return false;
    }
    m_sFilename = sFilename;

    //Verify type of document. Must be <book>
    wxXml2Node oRoot = oDoc.GetRoot();
    if (oRoot.GetName() != _T("book")) {
        wxLogMessage(
            _T("Error. First tag is not <book> but <%s>"),
            oRoot.GetName() );
        oRoot.DestroyIfUnlinked();
        oDoc.DestroyIfUnlinked();
        return false;
    }

    //If you want to do it "by hand" I think the following _may_ work:
    //1) after loading your doc, you need wxXml2Document::GetDTD
    //2) inside your wxXml2DTD you'll find the wxXml2EntityDecl nodes
    //3) for each of those nodes you need to use the GetContent() function and if a
    //valid filename is returned load it using wxXml2Document
    //4) parse your original tree and replace the wxXML2_ENTITY_REF_NODE with the root
    //of the wxXml2Document loaded at step #3

    //DumpXMLTree(oRoot);   //DBG
    CreateLinksTable(oRoot);

    // Prepare PO file
    if (m_fOnlyPoFile) {
        if (!StartPoFile( sFilename )) {
            wxLogMessage(_T("Error: PO file can not be created"));
            oRoot.DestroyIfUnlinked();
            oDoc.DestroyIfUnlinked();
            return false;        //error
        }
    }

    // Prepare the TOC file
    if (!StartTocFile( sFilename )) {
        wxLogMessage(_T("Error: toc file can not be created"));
        oRoot.DestroyIfUnlinked();
        oDoc.DestroyIfUnlinked();
        return false;        //error
    }

    // prepare de LMB file
    if (m_fGenerateLmb) {
        if (!StartLmbFile( sFilename )) {
            wxLogMessage(_T("Error: LMB file can not be created"));
            oRoot.DestroyIfUnlinked();
            oDoc.DestroyIfUnlinked();
            return false;        //error
        }
    }

    bool fError = BookTag(oRoot);
    if (fError)
        wxMessageBox(_T("There are errors in conversion to HTML"));

    // Close files
    TerminateTocFile();
    TerminateLmbFile();

    oRoot.DestroyIfUnlinked();
    oDoc.DestroyIfUnlinked();

    return !fError;

}

void lmEbookProcessor::CreateLinksTable(wxXml2Node& oRoot)
{
    // explores the tree and creates the cross-reference table relating
    // themes' ids with theme number

    m_PagesIds.clear();
    m_nNumHtmlPage = 0;

    wxXml2Node oCurr(oRoot);
    do {
        FindThemeNode(oCurr);
        oCurr = oCurr.GetNext();
    } while (oCurr != wxXml2EmptyNode);
}

void lmEbookProcessor::FindThemeNode(const wxXml2Node& oNode)
{
    if (oNode == wxXml2EmptyNode) return;

    if (oNode.GetName() == _T("theme")) {
        // Add to list
        wxString sId = oNode.GetPropVal(_T("id"), _T(""));
        if (sId == _T("")) {
            wxLogMessage(_T("Node <theme> has no id property"));
        }
        m_PagesIds[sId] = m_nNumHtmlPage;
        wxLogMessage(_T("Cross-refs table: id='%s', page=%d"), sId, m_nNumHtmlPage);
        m_nNumHtmlPage++;
    }
        
    // process its children recursively
    wxXml2Node oChild(oNode.GetFirstChild());
    while (oChild != wxXml2EmptyNode) {
        FindThemeNode(oChild);
        oChild = oChild.GetNext();
    }

}


bool lmEbookProcessor::ProcessChildAndSiblings(const wxXml2Node& oNode, int nWriteOptions)
{
    wxXml2Node oCurr(oNode.GetFirstChild());
    //m_xx += 5;
    //wxString spaces(wxT(' '), m_xx);

    bool fError = false;
    while (oCurr != wxXml2EmptyNode) {
        //if (oCurr.GetName() == _T("text")) {
        //    wxLogMessage(spaces + wxT("text [%s]"), oCurr.GetContent() );
        //}
        //else
        //    wxLogMessage(spaces + wxT("parsing [%s]"), oCurr.GetName() );
        fError |= ProcessChildren(oCurr, nWriteOptions);
        oCurr = oCurr.GetNext();
    }
    //m_xx -= 5;
    return fError;
}

bool lmEbookProcessor::ProcessChildren(const wxXml2Node& oNode, int nWriteOptions)
{
    bool fError = false;

    if (oNode == wxXml2EmptyNode) return false;
        
    if (oNode.GetType() == wxXML_TEXT_NODE)
    {
        // it is a text node: write its contents to output
        wxString sContent = oNode.GetContent();
        if (sContent.Last() == wxT('\n')) sContent.RemoveLast();
        if (sContent.GetChar(0) == wxT('\n')) sContent.Remove(0, 1);

        // libxml2 creates text nodes associated to all elements, even 
        // when no content is present in the xml file. In these cases
        // the text node contains just a simple '\n'. Next code lines
        // are for filtering ou these lines
        wxString tmp = sContent.Trim();
        if (!tmp.IsEmpty()) {
            if (nWriteOptions & eTOC) WriteToToc(sContent, ltNO_INDENT);        //text not indented
            if (nWriteOptions & eHTML) WriteToHtml(sContent);
            if (m_fOnlyPoFile && (nWriteOptions & eTRANSLATE)) WriteToPo(sContent);
            //if (fIdx) WriteToIdx(sContent);
        }

    }
    else if (oNode.GetType() == wxXML_ELEMENT_NODE)
    {
        // it is an element. Process it (recursive, as ProcessTags call ProcessChildAndSiblings)
        fError |= ProcessTag(oNode);
    }
    else {
        // ignore it
    }
    
    return fError;

}

bool lmEbookProcessor::ProcessTag(const wxXml2Node& oNode)
{
    if (oNode == wxXml2EmptyNode) return false;
    if (oNode.GetType() != wxXML_ELEMENT_NODE) return false;

    wxString sElement = oNode.GetName();
    //wxString spaces(wxT(' '), m_xx);
    //wxLogMessage(spaces + _T("[ProcessTag] - element [%s]"), sElement.c_str());

    if (sElement == _T("bookinfo")) {
        return BookinfoTag(oNode);
    }
    else if (sElement == _T("chapter")) {
        return ChapterTag(oNode);
    }
    else if (sElement == _T("emphasis")) {
        return EmphasisTag(oNode);
    }
    else if (sElement == _T("exercise")) {
        return ExerciseTag(oNode);
    }
    else if (sElement == _T("itemizedlist")) {
        return ItemizedlistTag(oNode);
    }
    else if (sElement == _T("link")) {
        return LinkTag(oNode);
    }
    else if (sElement == _T("listitem")) {
        return ListitemTag(oNode);
    }
    else if (sElement == _T("score")) {
        return SectionTag(oNode);
    }
    else if (sElement == _T("section")) {
        return SectionTag(oNode);
    }
    else if (sElement == _T("para")) {
        return ParaTag(oNode);
    }
    else if (sElement == _T("part")) {
        return PartTag(oNode);
    }
    else if (sElement == _T("theme")) {
        return ThemeTag(oNode);
    }
    else if (sElement == _T("title")) {
        return TitleTag(oNode);
    }
    else {
        //check for exercises related param tags

        // a) Non-translatable params
        if (   (sElement == _T("accidentals"))
            || (sElement == _T("chords"))
            || (sElement == _T("clef"))
            || (sElement == _T("control_go_back"))
            || (sElement == _T("control_settings"))
            || (sElement == _T("fragment"))
            || (sElement == _T("inversions"))
            || (sElement == _T("key"))          //keys
            || (sElement == _T("keys"))
            || (sElement == _T("max_accidentals"))
            || (sElement == _T("max_interval"))
            || (sElement == _T("maxInterval"))  //max_interval
            || (sElement == _T("mode"))
            || (sElement == _T("music"))
            || (sElement == _T("music_border"))
            || (sElement == _T("problem_type"))
            || (sElement == _T("playMode"))     //play_mode
            || (sElement == _T("scales"))
            || (sElement == _T("showKey"))      //show_key
            || (sElement == _T("time"))
            || (sElement == _T("type"))         //score_type
           )
        {
            return ExerciseParamTag(oNode, false);
        }
        // b) translatable params
        else if ((sElement == _T("control_measures"))
              || (sElement == _T("control_play"))
              || (sElement == _T("control_solfa"))
           )
        {
            return ExerciseParamTag(oNode, true);
        }
        else {
            wxLogMessage(_T("Error: Found tag <%s>. No treatment defined."), sElement);
            return true;
        }
    }

}

wxString lmEbookProcessor::GetLibxml2Version() 
{
    return wxXml2::GetLibxml2Version(); 
}



//------------------------------------------------------------------------------------
// Tags' processors
//------------------------------------------------------------------------------------

bool lmEbookProcessor::BookTag(const wxXml2Node& oNode)
{
    // receives and processes a <book> node and its children.
    // return true if error
    //<book id="xxxx"> -->
    //  TOC:
    //  <html><head> | </html>

    //// convert tag: output to open html tags
    //wxString sNil = _T("");
    //wxString sCharset = _T("utf-8");
    //wxString sHtml = sNil +
    //    _T("<html>\n<head>\n")
    //    _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") + sCharset +
    //    _T("\">\n");
    //    _T("</head>\n<body>\n\n<ul>\n");
    //WriteToHtml(sHtml);

    //// tag processing implications
    //m_nHeaderLevel = -1;        //not initialized

    //process tag's children
    bool fError = ProcessChildAndSiblings(oNode);

    ////convert tag: output to close html tags
    //WriteToHtml(_T("</html>\n"));

    return fError;

}

bool lmEbookProcessor::BookinfoTag(const wxXml2Node& oNode)
{
    // receives and processes a <bookinfo> node and its children.
    // return true if error
    //<bookinfo> = nil / nLevel = 0

    // convert tag: output to open html tags
    // -- no output implied by this tag --

    // tag processing implications
    m_nHeaderLevel = 0;
    m_fProcessingBookinfo = true;
    m_fTitleToToc = true;

    //process tag's children
    bool fError = ProcessChildAndSiblings(oNode);

    //end tag: processing implications
    m_fProcessingBookinfo = false;

    return fError;
}

bool lmEbookProcessor::ChapterTag(const wxXml2Node& oNode)
{
    // convert tag: output to open html tags
    WriteToToc(_T("<entry id=\"xxxx\">\n"));
    m_nTocIndentLevel++;
 
    // tag processing implications
    //m_nHeaderLevel = 1;
    m_fTitleToToc = true;

    //process tag's children
    bool fError = ProcessChildAndSiblings(oNode);

    //convert tag: output to close html tags
    m_nTocIndentLevel--;
    WriteToToc(_T("</entry>\n"));

    return fError;
}

bool lmEbookProcessor::EmphasisTag(const wxXml2Node& oNode)
{
    // openning tag
    WriteToHtml(_T("<b>"));

    //process tag's children and write note content to html
    bool fError = ProcessChildAndSiblings(oNode, eHTML | eTRANSLATE);
    
    //convert tag
    WriteToHtml(_T("</b>"));

    return fError;
}

bool lmEbookProcessor::ExerciseTag(const wxXml2Node& oNode)
{
    // get attributes
    wxString sType = oNode.GetPropVal(_T("type"), _T(""));
    wxString sWidth = oNode.GetPropVal(_T("width"), _T(""));
    wxString sHeight = oNode.GetPropVal(_T("height"), _T(""));
    wxString sBorder = oNode.GetPropVal(_T("border"), _T(""));

    // convert tag: output to open html tags
    WriteToHtml(_T("<object type=\"Application/LenMus\" classid=\"") + sType +
        _T("\" width=\"") + sWidth +
        _T("\" height=\"") + sHeight +
        _T("\" border=\"") + sBorder +
        _T("\">\n") );
 
    //process tag's children
    bool fError = ProcessChildAndSiblings(oNode, eHTML);

    //convert tag: output to close html tags
    WriteToHtml(_T("</object>\n"));

    return fError;
}

bool lmEbookProcessor::ExerciseParamTag(const wxXml2Node& oNode, bool fTranslate)
{
    // convert tag
    WriteToHtml(_T("<param name=\"") + oNode.GetName() + _T("\" value=\"") );

    //params have no more xml content, just the value. Get it
    bool fError = ProcessChildAndSiblings(oNode, eHTML | (fTranslate ? eTRANSLATE : 0));

    WriteToHtml(_T("\">\n"));

    return fError;
}

bool lmEbookProcessor::ItemizedlistTag(const wxXml2Node& oNode)
{
    // openning tag
    WriteToHtml(_T("<ul>\n"));

    // tag processing implications
    // no

    //process tag's children
    bool fError = ProcessChildAndSiblings(oNode);

    // closing tag
    WriteToHtml(_T("</ul>\n"));

    return fError;
}

bool lmEbookProcessor::LinkTag(const wxXml2Node& oNode)
{
    // openning tag

    // get its 'linkend' property and find the associated page
    wxString sId = oNode.GetPropVal(_T("linkend"), _T(""));
    if (sId != _T("")) {
        if (m_PagesIds.find(sId) != m_PagesIds.end()) {
            wxFileName oFN( m_sFilename );
            wxString sName = oFN.GetName();
            int nFile = m_PagesIds[sId];
            sName += wxString::Format(_T("_%d"), nFile);
            oFN.SetName(sName);
            oFN.SetExt(_T("htm"));
            wxString sLink = _T("<a href=\"#LenMusPage/") + oFN.GetFullName() + _T("\">");
            WriteToHtml( sLink );
        }
        else {
            WriteToHtml( _T("<a href=\"#\">") );
        }
    }

    //process tag's children and write note content to html
    bool fError = ProcessChildAndSiblings(oNode, eHTML | eTRANSLATE);

    // closing tag
    if (sId != _T("")) WriteToHtml(_T("</a>"));

    return fError;
}

bool lmEbookProcessor::ListitemTag(const wxXml2Node& oNode)
{
    // openning tag
    WriteToHtml(_T("<li>"));

    // tag processing implications

    //process tag's children and write note content to html
    bool fError = ProcessChildAndSiblings(oNode, eHTML | eTRANSLATE);

    // closing tag
    WriteToHtml(_T("</li>\n"));

    return fError;
}

bool lmEbookProcessor::ParaTag(const wxXml2Node& oNode)
{
    // openning tag
    WriteToHtml(_T("<p>"));

    //process tag's children and write note content to html
    bool fError = ProcessChildAndSiblings(oNode, eHTML | eTRANSLATE);
    
    //convert tag: output to close html tags
    WriteToHtml(_T("</p>\n"));

    return fError;
}

bool lmEbookProcessor::PartTag(const wxXml2Node& oNode)
{
    // openning tag
    // HTML:
    WriteToHtml(_T("<div id=\"Cxxxx\">\n"));
    // TOC:
    // no toc output
 
    // tag processing implications
    m_fTitleToToc = false;
    m_nHeaderLevel++;

    //process tag's children
    bool fError = ProcessChildAndSiblings(oNode);

    //closing tag:
    // HTML:
    WriteToHtml(_T("</div>\n"));
    // TOC:
    // no toc content
    // processing implications:
    m_nHeaderLevel--;

    return fError;
}

bool lmEbookProcessor::ScoreTag(const wxXml2Node& oNode)
{
    // get attributes
    wxString sWidth = oNode.GetPropVal(_T("width"), _T(""));
    wxString sHeight = oNode.GetPropVal(_T("height"), _T(""));
    wxString sBorder = oNode.GetPropVal(_T("border"), _T(""));

    // convert tag: output to open html tags
    WriteToHtml(_T("<object type=\"Application/LenMus\" classid=\"Score")
        _T("\" width=\"") + sWidth +
        _T("\" height=\"") + sHeight +
        _T("\" border=\"") + sBorder +
        _T("\">\n") );
 
    //process tag's children
    bool fError = ProcessChildAndSiblings(oNode, eHTML);

    //convert tag: output to close html tags
    WriteToHtml(_T("</object>\n"));

    return fError;
}

bool lmEbookProcessor::SectionTag(const wxXml2Node& oNode)
{
    // openning tag
    WriteToHtml(_T("<div id=\"xxxx\">\n"));
 
    // tag processing implications
    m_fTitleToToc = true;
    //m_nHeaderLevel = nLevel+1;

    //process tag's children
    bool fError = ProcessChildAndSiblings(oNode);

    //convert tag: output to close html tags
    WriteToHtml(_T("</div>\n"));

    return fError;
}

bool lmEbookProcessor::ThemeTag(const wxXml2Node& oNode)
{
    // get its 'id' property
    wxString sId = oNode.GetPropVal(_T("id"), _T(""));

    // openning tag
    // HTML:
    StartHtmlFile(m_sFilename, sId);
    // TOC
    WriteToToc(_T("<entry id=\"xxxx\">\n"));
    m_nTocIndentLevel++;
    WriteToToc(_T("<page>") + m_sHtmlPagename + _T("</page>\n"));
 
    // tag processing implications
    m_nHeaderLevel = 1;
    m_fTitleToToc = true;

    //process tag's children
    bool fError = ProcessChildAndSiblings(oNode);

    //closing tag:
    // HTML:
    TerminateHtmlFile();    // Close previous html page
    // TOC:
    m_nTocIndentLevel--;
    WriteToToc(_T("</entry>\n"));

    return fError;
}

bool lmEbookProcessor::TitleTag(const wxXml2Node& oNode)
{
    // openning tag
    //TOC
    if (m_fProcessingBookinfo || m_fTitleToToc) WriteToToc(_T("<title>"));
    //HTML
    WriteToHtml( wxString::Format(_T("<h%d>"), m_nHeaderLevel));

    //process tag's children and write note content to html and toc
    int nOptions = eHTML | eTRANSLATE |
                   ((m_fProcessingBookinfo || m_fTitleToToc) ? eTOC : 0);
    bool fError = ProcessChildAndSiblings(oNode, nOptions);

    // End of tag processing implications
    //TOC:
    if (m_fProcessingBookinfo || m_fTitleToToc) WriteToToc(_T("</title>\n"), ltNO_INDENT );
    //HTML:
    WriteToHtml( wxString::Format(_T("</h%d>\n"), m_nHeaderLevel));

    return fError;
}


//------------------------------------------------------------------------------------
// File managing
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Toc file
//------------------------------------------------------------------------------------

bool lmEbookProcessor::StartTocFile(wxString sFilename)
{
    // returns false if error

    wxFileName oTOC( sFilename );
    oTOC.SetExt(_T("toc"));
    m_sTocFilename = oTOC.GetFullName();
    m_pTocFile = new wxFile(m_sTocFilename, wxFile::write);
    if (!m_pTocFile->IsOpened()) {
        wxLogMessage(_T("Error: File %s can not be created"), oTOC.GetFullName());
        return false;        //error
    }

    //initializations
    m_nTocIndentLevel = 0;

    //Generate header
    wxString sNil = _T("");
    wxString sHeader = sNil +
        _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
        _T("<lmBookTOC>\n");

    WriteToToc(sHeader);
    m_nTocIndentLevel++;

    return true;

}

void lmEbookProcessor::TerminateTocFile()
{
    if (!m_pTocFile) return;

    wxString sNil = _T("");
    wxString sHeader = sNil +
        _T("</lmBookTOC>\n");

    WriteToToc(sHeader);
    m_pTocFile->Close();

    delete m_pTocFile;
    m_pTocFile = (wxFile*) NULL;

}

void lmEbookProcessor::WriteToToc(wxString sText, bool fIndent)
{
    if (!m_pTocFile) return;

    if (fIndent) {
        wxString sIndent;
        sIndent.Append(_T(' '), 3 * m_nTocIndentLevel);
        m_pTocFile->Write(sIndent + sText);
    }
    else {
        m_pTocFile->Write(sText);
    }

}

//------------------------------------------------------------------------------------
// Html file managing
//------------------------------------------------------------------------------------

bool lmEbookProcessor::StartHtmlFile(wxString sFilename, wxString sId)
{
    // returns false if error

    wxFileName oHTM( sFilename );
    wxString sName = oHTM.GetName();
    if (sId == _T("")) return false;

    int nFile = m_PagesIds[sId];
    sName += wxString::Format(_T("_%d"), nFile);

    oHTM.SetName(sName);
    oHTM.SetExt(_T("htm"));
    m_sHtmlPagename = oHTM.GetFullName();

    if (m_fGenerateLmb) {
        m_pZipFile->PutNextEntry( m_sHtmlPagename );
    }
    else {
        m_pHtmlFile = new wxFile(oHTM.GetFullName(), wxFile::write);
        if (!m_pHtmlFile->IsOpened()) {
            wxLogMessage(_T("Error: File %s can not be created"), oHTM.GetFullName());
            return false;        //error
        }
    }

    //Generate header
    wxString sNil = _T("");
    wxString sCharset = _T("utf-8");
    wxString sHtml = sNil +
        _T("<html>\n<head>\n")
        _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") + sCharset +
        _T("\">\n")
        _T("</head>\n<body>\n\n");

    WriteToHtml(sHtml);

    //initializations
    m_nHtmlIndentLevel = 1;

    return true;

}

void lmEbookProcessor::TerminateHtmlFile()
{
    if (!((m_fGenerateLmb && m_pLmbFile) || m_pHtmlFile)) return;

    WriteToHtml(_T("\n</body>\n"));

    if (m_pHtmlFile) {
        m_pHtmlFile->Close();
        delete m_pHtmlFile;
        m_pHtmlFile = (wxFile*) NULL;
    }
    else {
        m_pZipFile->CloseEntry();
    }

}

void lmEbookProcessor::WriteToHtml(wxString sText)
{
    if (!((m_fGenerateLmb && m_pLmbFile) || m_pHtmlFile)) return;

    //wxString sIndent;
    //sIndent.Append(_T(' '), 3 * m_nHtmlIndentLevel);
    //m_pHtmlFile->Write(sIndent + sText);

    if (m_fGenerateLmb) {
        m_pLmbFile->WriteString( sText );
    }
    else {
        m_pHtmlFile->Write(sText);
    }
}

//------------------------------------------------------------------------------------
// Lmb file management
//------------------------------------------------------------------------------------


bool lmEbookProcessor::StartLmbFile(wxString sFilename)
{
    // returns true if success

    wxFileName oFNP( sFilename );
    oFNP.SetExt(_T("lmb"));
    m_pZipOutFile = new wxFFileOutputStream( oFNP.GetFullName() );
    //wxFFileOutputStream out( oFNP.GetFullName() );
    //if (!m_pPoFile->IsOpened()) {
    //    wxLogMessage(_T("Error: File %s can not be created"), oFNP.GetFullName());
    //    m_pPoFile = (wxFile*)NULL;
    //    return false;        //error
    //}
    m_pZipFile = new wxZipOutputStream(*m_pZipOutFile);
    m_pLmbFile = new wxTextOutputStream(*m_pZipFile);
    return true;

}

void lmEbookProcessor::TerminateLmbFile()
{
    if (!(m_fGenerateLmb && m_pLmbFile)) return;

    CopyToLmb( m_sTocFilename );

    // delete temporal toc file
    if (!::wxRemoveFile(m_sTocFilename)) {
        wxLogMessage(_T("Error: File %s could not be deleted"), m_sTocFilename);
    }

    delete m_pZipFile;
    m_pLmbFile = (wxTextOutputStream*)NULL;
    m_pZipFile = (wxZipOutputStream*)NULL;

}

void lmEbookProcessor::CopyToLmb(wxString sFilename)
{
    wxFFileInputStream inFile( sFilename, _T("r") );
    if (!inFile.IsOk()) {
        wxLogMessage(_T("Error: File %s can not be merged into LMB"), sFilename);
        return;
    }
    m_pZipFile->PutNextEntry( sFilename ); 
    m_pZipFile->Write( inFile );
    m_pZipFile->CloseEntry(); 

}



//------------------------------------------------------------------------------------
// PO file management
//------------------------------------------------------------------------------------


bool lmEbookProcessor::StartPoFile(wxString sFilename)
{
    // returns true if success

    wxFileName oFNP( sFilename );
    oFNP.SetExt(_T("po"));
    m_pPoFile = new wxFile(oFNP.GetFullName(), wxFile::write);
    if (!m_pPoFile->IsOpened()) {
        wxLogMessage(_T("Error: File %s can not be created"), oFNP.GetFullName());
        m_pPoFile = (wxFile*)NULL;
        return false;        //error
    }

    //Generate Po header
    wxString sNil = _T("");
    wxString sHeader = sNil +
        _T("msgid \"\"\n") 
        _T("msgstr \"\"\n")
        _T("\"Project-Id-Version: LenMus 3.3\\n\"\n")
        _T("\"POT-Creation-Date: \\n\"\n")
        _T("\"PO-Revision-Date: 2006-08-25 12:19+0100\\n\"\n")
        _T("\"Last-Translator: \\n\"\n")
        _T("\"Language-Team:  <cecilios@gmail.com>\\n\"\n")
        _T("\"MIME-Version: 1.0\\n\"\n")
        _T("\"Content-Type: text/plain; charset=utf-8\\n\"\n")
        _T("\"Content-Transfer-Encoding: 8bit\\n\"\n")
        _T("\"X-Poedit-Language: English\\n\"\n")
        _T("\"X-Poedit-SourceCharset: iso-8859-1\\n\"\n\n");

    m_pPoFile->Write(sHeader);
    return true;

}

void lmEbookProcessor::WriteToPo(wxString& sText)
{
    //add text to PO file

    if (!m_pPoFile || sText == _T("")) return;

    //change /n by //n
    wxString sContent = sText;
    sContent.Replace(_T("\n"), _T("\\n"));
    m_pPoFile->Write(_T("msgid \""));
    m_pPoFile->Write(sContent + _T("\"\n"));
    m_pPoFile->Write(_T("msgstr \"\"\n\n"));

}


//------------------------------------------------------------------------------------
// Debug methods
//------------------------------------------------------------------------------------


void lmEbookProcessor::DumpXMLTree(const wxXml2Node& oNode)
{
    wxString sTree;
    sTree.Alloc(1024);
    int nIndent = 3;

    // get a string with the tree structure...
    DumpNodeAndSiblings(oNode, sTree, nIndent);
    wxLogMessage(sTree);

}

void lmEbookProcessor::DumpNodeAndSiblings(const wxXml2Node& oNode, wxString& sTree, int n)
{
    wxXml2Node oCurr(oNode);

    do {
        DumpNode(oCurr, sTree, n);
        oCurr = oCurr.GetNext();
    } while (oCurr != wxXml2EmptyNode);
}

void lmEbookProcessor::DumpNode(const wxXml2Node& oNode, wxString& sTree, int n)
{
#define STEP            4

    if (oNode == wxXml2EmptyNode) return;
    wxString toadd, spaces(wxT(' '), n);

    // concatenate the name of this node
    toadd = oNode.GetName();
        
    // if this is a text node, then add also the contents...
    if (oNode.GetType() == wxXML_TEXT_NODE ||
        oNode.GetType() == wxXML_COMMENT_NODE || 
        oNode.GetType() == wxXML_CDATA_SECTION_NODE) {

        wxString content = oNode.GetContent();
        if (content.Last() == wxT('\n')) content.RemoveLast();
        if (content.GetChar(0) == wxT('\n')) content.Remove(0, 1);

        // a little exception: libxml2 when loading a document creates a
        // lot of text nodes containing just a simple \n;
        // in this cases, just show "[null]"
        wxString tmp = content.Trim();
        if (tmp.IsEmpty())
            toadd += wxT(" node: [null]");
        else 
            toadd += wxT(" node: ") + content;


    } else {        // if it's not a text node, then add the properties...

        wxXml2Property prop(oNode.GetProperties());
        while (prop != wxXml2EmptyProperty) {
            toadd += wxT(" ") + prop.GetName() + wxT("=");
            toadd += prop.GetValue();
            prop = prop.GetNext();
        }
    }
        
    sTree += spaces;

#define SHOW_ANNOYING_NEWLINES
#ifdef SHOW_ANNOYING_NEWLINES   

    // text nodes with newlines and/or spaces will be shown as [null]
    sTree += toadd;
#else

    // text nodes with newlines won't be shown at all
    if (toadd != wxT("textnode: [null]")) sTree += toadd;
#endif

    // go one line down
    sTree += wxT("\n");

    // do we must add the close tag ?
    bool bClose = FALSE;

    // and then, a step more indented, its children
    wxXml2Node child(oNode.GetFirstChild());
    while (child != wxXml2EmptyNode) {

        DumpNode(child, sTree, n+STEP);
        child = child.GetNext();

        // add a close tag because at least one child is present...
        bClose = TRUE;
    }

    if (bClose) sTree += wxString(wxT(' '), n) + wxT("/") + oNode.GetName() + wxT("\n");
}


