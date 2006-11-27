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

#include "ebook_processor.h"
#include "wx/xml2.h"


#define ltNO_INDENT false

ltEbookProcessor::ltEbookProcessor()
{
    m_pTocFile = (wxFile*) NULL;
    m_pHtmlFile = (wxFile*) NULL;

}

ltEbookProcessor::~ltEbookProcessor()
{
    if (m_pTocFile) delete m_pTocFile;
    if (m_pHtmlFile) delete m_pHtmlFile;
}

bool ltEbookProcessor::GenerateLMB(wxString sFilename)
{
    // returns false if error

    // Prepare for a new XML file processing
    m_fProcessingBookinfo = false;
    m_nNumHtmlPage = 0;

    // load the XML file as tree of nodes
    wxXml2Document oDoc;
    wxString sError;
    if (!oDoc.Load(sFilename, &sError)) {
        wxLogMessage(_T("Error parsing file %s\nError:%s"), sFilename, sError.c_str());
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

    // Prepare the TOC file
    if (!StartTocFile( sFilename )) {
        wxLogMessage(_T("Error: toc file can not be created"));
        oRoot.DestroyIfUnlinked();
        oDoc.DestroyIfUnlinked();
        return false;        //error
    }

    bool fError = BookTag(oRoot);
    if (fError)
        wxMessageBox(_T("There are errors in conversion to HTML"));

    // Close files
    TerminateTocFile();

    oRoot.DestroyIfUnlinked();
    oDoc.DestroyIfUnlinked();

    return !fError;

}
bool ltEbookProcessor::ProcessChildren(const wxXml2Node& oNode)
{
    //get first child
    wxXml2Node oChild(oNode.GetFirstChild());
    //oNode = m_pParser->GetFirstChild(oNode);
    //const wxXml2Node& oNode = oNode;

    //loop to process children
    bool fError = false;            //assume no errors
    //while (pElement)
    while (oChild != wxXml2EmptyNode)
    {
        fError |= ProcessTag(oChild);

        // Get next sibling
        oChild = oChild.GetNext();
        //oNode = m_pParser->GetNextSibling(oNode);
        //pElement = oNode;
    }

    return fError;
}

bool ltEbookProcessor::ProcessTag(const wxXml2Node& oNode)
{
    if (oNode == wxXml2EmptyNode) return false;
    wxString sElement = oNode.GetName();
    wxLogDebug(wxT("[ProcessTag] - parsing [%s]"), sElement.c_str());

    if (oNode.GetType() != wxXML_ELEMENT_NODE) return false;

    if (sElement == _T("bookinfo")) {
        return BookinfoTag(oNode);
    }
    else if (sElement == _T("chapter")) {
        return ChapterTag(oNode);
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
    else if (sElement == _T("object")) {
        return ObjectTag(oNode);
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
        wxLogMessage(_T("Error: Found tag <%s>. No treatment defined."), sElement);
        return true;
    }

}

//------------------------------------------------------------------------------------
// Tags' processors
//------------------------------------------------------------------------------------

bool ltEbookProcessor::BookTag(const wxXml2Node& oNode)
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
    bool fError = ProcessChildren(oNode);

    ////convert tag: output to close html tags
    //WriteToHtml(_T("</html>\n"));

    return fError;

}

bool ltEbookProcessor::BookinfoTag(const wxXml2Node& oNode)
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
    bool fError = ProcessChildren(oNode);

    //end tag: processing implications
    m_fProcessingBookinfo = false;

    return fError;
}

bool ltEbookProcessor::ChapterTag(const wxXml2Node& oNode)
{
    // convert tag: output to open html tags
    WriteToToc(_T("<entry id=\"xxxx\">\n"));
    m_nTocIndentLevel++;
 
    // tag processing implications
    //m_nHeaderLevel = 1;
    m_fTitleToToc = true;

    //process tag's children
    bool fError = ProcessChildren(oNode);

    //convert tag: output to close html tags
    m_nTocIndentLevel--;
    WriteToToc(_T("</entry>\n"));

    return fError;
}

bool ltEbookProcessor::ItemizedlistTag(const wxXml2Node& oNode)
{
    // openning tag
    WriteToHtml(_T("<ul>\n"));

    // tag processing implications
    // no

    //process tag's children
    bool fError = ProcessChildren(oNode);

    // closing tag
    WriteToHtml(_T("</ul>\n"));

    return fError;
}

bool ltEbookProcessor::LinkTag(const wxXml2Node& oNode)
{
    // openning tag
    WriteToHtml(_T("<a href=\"#LenMusPage/SingleExercises_1.htm\">"));

    // tag processing implications
    WriteToHtml(oNode.GetContent());     //write text in item

    //process tag's children
    bool fError = ProcessChildren(oNode);

    // closing tag
    WriteToHtml(_T("</a>"));

    return fError;
}

bool ltEbookProcessor::ListitemTag(const wxXml2Node& oNode)
{
    // openning tag
    WriteToHtml(_T("<li>"));

    // tag processing implications
    //write text in paragraph
    bool fError = false;
    if (oNode.GetType() == wxXML_ELEMENT_NODE) {
        WriteToHtml( GetText(oNode) );
    }
    else {
        //process tag's children
        fError = ProcessChildren(oNode);
    }
    //WriteToHtml(oNode.GetContent());     //write text in item

    ////process tag's children
    //bool fError = ProcessChildren(oNode);

    // closing tag
    WriteToHtml(_T("</li>\n"));

    return fError;
}

bool ltEbookProcessor::ObjectTag(const wxXml2Node& WXUNUSED(oNode))
{
    // openning tag
    WriteToHtml(_T("<object>"));

    // tag processing implications
    //WriteToHtml( oNode.GetContent() );    //write text in paragraph

    //process tag's children
    //bool fError = ProcessChildren(oNode);

    //convert tag: output to close html tags
    WriteToHtml(_T("</object>\n"));

    return false;
}

bool ltEbookProcessor::ParaTag(const wxXml2Node& oNode)
{
    // openning tag
    WriteToHtml(_T("<p>"));

    // tag processing implications
    bool fError = false;
    //write text in paragraph
    if (oNode.GetType() == wxXML_ELEMENT_NODE) {
        WriteToHtml( GetText(oNode) );
        //if (m_pPoFile) AddToPoFile(sText);      //add text to PO file
    }
    else {
        //process tag's children
        fError = ProcessChildren(oNode);
    }

    //convert tag: output to close html tags
    WriteToHtml(_T("</p>\n"));

    return fError;
}

bool ltEbookProcessor::PartTag(const wxXml2Node& oNode)
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
    bool fError = ProcessChildren(oNode);

    //closing tag:
    // HTML:
    WriteToHtml(_T("</div>\n"));
    // TOC:
    // no toc content
    // processing implications:
    m_nHeaderLevel--;

    return fError;
}

bool ltEbookProcessor::SectionTag(const wxXml2Node& oNode)
{
    // openning tag
    WriteToHtml(_T("<div id=\"xxxx\">\n"));
 
    // tag processing implications
    m_fTitleToToc = true;
    //m_nHeaderLevel = nLevel+1;

    //process tag's children
    bool fError = ProcessChildren(oNode);

    //convert tag: output to close html tags
    WriteToHtml(_T("</div>\n"));

    return fError;
}

bool ltEbookProcessor::ThemeTag(const wxXml2Node& oNode)
{
    // openning tag
    // HTML:
    StartHtmlFile(m_sFilename);
    // TOC
    WriteToToc(_T("<entry id=\"xxxx\">\n"));
    m_nTocIndentLevel++;
    WriteToToc(_T("<page>") + m_sHtmlPagename + _T("</page>\n"));
 
    // tag processing implications
    m_nHeaderLevel = 1;
    m_fTitleToToc = true;

    //process tag's children
    bool fError = ProcessChildren(oNode);

    //closing tag:
    // HTML:
    TerminateHtmlFile();    // Close previous html page
    // TOC:
    m_nTocIndentLevel--;
    WriteToToc(_T("</entry>\n"));

    return fError;
}

bool ltEbookProcessor::TitleTag(const wxXml2Node& oNode)
{
    // openning tag
    //TOC
    if (m_fProcessingBookinfo || m_fTitleToToc) WriteToToc(_T("<title>"));
    //HTML
    WriteToHtml( wxString::Format(_T("<h%d>"), m_nHeaderLevel));

    // tag processing implications
    wxString sTitle = GetText(oNode);
    WriteToHtml(sTitle);
    if (m_fProcessingBookinfo || m_fTitleToToc) WriteToToc(sTitle, ltNO_INDENT);

    //process tag's children
    //bool fError = ProcessChildren(oNode);

    // End of tag processing implications
    //TOC:
    if (m_fProcessingBookinfo || m_fTitleToToc) WriteToToc(_T("</title>\n"), ltNO_INDENT );
    //HTML:
    WriteToHtml( wxString::Format(_T("</h%d>\n"), m_nHeaderLevel));

    return true;
}


//------------------------------------------------------------------------------------
// File managing
//------------------------------------------------------------------------------------

bool ltEbookProcessor::StartTocFile(wxString sFilename)
{
    // returns false if error

    wxFileName oTOC( sFilename );
    oTOC.SetExt(_T("toc"));
    m_pTocFile = new wxFile(oTOC.GetFullName(), wxFile::write);
    if (!m_pTocFile->IsOpened()) {
        wxLogMessage(_T("Error: File %s can not be created"), oTOC.GetFullName());
        return false;        //error
    }

    //Generate header
    wxString sNil = _T("");
    wxString sHeader = sNil +
        _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
        _T("<lmBookTOC>\n");

    m_pTocFile->Write(sHeader);

    //initializations
    m_nTocIndentLevel = 1;

    return true;

}

void ltEbookProcessor::TerminateTocFile()
{
    if (!m_pTocFile) return;

    wxString sNil = _T("");
    wxString sHeader = sNil +
        _T("</lmBookTOC>\n");

    m_pTocFile->Write(sHeader);
    m_pTocFile->Close();

    delete m_pTocFile;
    m_pTocFile = (wxFile*) NULL;

}

bool ltEbookProcessor::StartHtmlFile(wxString sFilename)
{
    // returns false if error

    wxFileName oHTM( sFilename );
    wxString sName = oHTM.GetName();
    sName += wxString::Format(_T("_%d"), m_nNumHtmlPage++);
    oHTM.SetName(sName);
    oHTM.SetExt(_T("htm"));
    m_sHtmlPagename = oHTM.GetFullName();
    m_pHtmlFile = new wxFile(oHTM.GetFullName(), wxFile::write);
    if (!m_pHtmlFile->IsOpened()) {
        wxLogMessage(_T("Error: File %s can not be created"), oHTM.GetFullName());
        return false;        //error
    }

    //Generate header
    wxString sNil = _T("");
    wxString sCharset = _T("utf-8");
    wxString sHtml = sNil +
        _T("<html>\n<head>\n")
        _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") + sCharset +
        _T("\">\n")
        _T("</head>\n<body>\n\n");

    m_pHtmlFile->Write(sHtml);

    //initializations
    m_nHtmlIndentLevel = 1;

    return true;

}

void ltEbookProcessor::TerminateHtmlFile()
{
    if (!m_pHtmlFile) return;

    m_pHtmlFile->Write(_T("\n</body>\n"));
    m_pHtmlFile->Close();

    delete m_pHtmlFile;
    m_pHtmlFile = (wxFile*) NULL;

}

void ltEbookProcessor::WriteToToc(wxString sText, bool fIndent)
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

void ltEbookProcessor::WriteToHtml(wxString sText)
{
    if (!m_pHtmlFile) return;

    //wxString sIndent;
    //sIndent.Append(_T(' '), 3 * m_nHtmlIndentLevel);
    //m_pHtmlFile->Write(sIndent + sText);

    m_pHtmlFile->Write(sText);
}



//------------------------------------------------------------------------------------
// XML parsing helpers
//------------------------------------------------------------------------------------

wxString ltEbookProcessor::GetText(const wxXml2Node& oNode)
{
    if (oNode.GetType() == wxXML_TEXT_NODE) {
        wxString sText = oNode.GetContent();
        if (sText.Last() == _T('\n')) sText.RemoveLast();
        if (sText.GetChar(0) == _T('\n')) sText.Remove(0, 1);
        return sText;
    }
    else if (oNode.GetType() == wxXML_ELEMENT_NODE) {
        return GetText(oNode.GetFirstChild());
    }
    else
        return wxEmptyString;
}

