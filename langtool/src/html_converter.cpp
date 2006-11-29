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
#pragma implementation html_converter.h
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

#include "html_converter.h"
//#include "parser.h"

lmHtmlConverter::lmHtmlConverter()
{
    //m_pParser = new lmXmlParser();
    m_fIncludeObjects = false;

    m_pPoFile = (wxFile*)NULL;

}

lmHtmlConverter::~lmHtmlConverter()
{
    //delete m_pParser;
}

bool lmHtmlConverter::ConvertToHtml(const wxString& sFilename, bool fIncludeObjects,
                                    wxFile* pPoFile)
{
    m_fIncludeObjects = fIncludeObjects;
    m_pPoFile = pPoFile;

    // load the XML file as tree of nodes
    wxXmlDocument xdoc;
    if (!xdoc.Load(sFilename)) {
        wxLogMessage(_T("Error parsing eBook XML file ") + sFilename);
        return true;
    }

    //Verify type of document. Must be <book>
    wxXmlNode *pRoot = xdoc.GetRoot();
    if (pRoot->GetName() != _T("book")) {
        wxLogMessage(
            _T("Error. First tag is not <book> but <%s>"),
            pRoot->GetName() );
        return true;
    }

    //Process the _OBJ.XML file
    if (m_fIncludeObjects) {
        wxFileName oFNO( sFilename + _T("_obj") );
        oFNO.SetExt(_T("xml"));
        LoadObjectsFile( oFNO.GetFullName() );
    }

    // Process the XML file
    wxFileName oFNX( sFilename );
    oFNX.SetExt(_T("htm"));
    m_nIndentLevel = 0;

    wxFile* pFile = new wxFile(oFNX.GetFullName(), wxFile::write);
    if (!pFile->IsOpened()) {
        wxLogMessage(_T("Error: File %s can not be created"), sFilename);
        delete pFile;
        return true;        //error
    }
    bool fError = BookToHtml(pRoot, pFile);
    if (fError)
        wxMessageBox(_T("There are errors in conversion to HTML"));
    delete pFile;
    return fError;

}

bool lmHtmlConverter::ProcessChildren(wxXmlNode* pNode, wxFile* pFile)
{
    m_nIndentLevel++;

    //get first child
    //pNode = m_pParser->GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;

    //loop to process children
    bool fError = false;            //assume no errors
    while (pElement)
    {
        fError |= ProcessTag(pElement, pFile);

        // Get next sibling
        //pNode = m_pParser->GetNextSibling(pNode);
        pElement = pNode;
    }

    return fError;
}

bool lmHtmlConverter::ProcessTag(wxXmlNode* pElement, wxFile* pFile)
{
    // receives an xml tag node.
    // return true if error

    //
    //<book id="xxxx"> = <html><head> | </html>
    //<bookinfo> = nil / nLevel = 0
    //<chapter id="xxxx"> = </head><body><div id="Cxxxx"> / nLevel=1 | </div></body>
    //<sect1> = <div id="S1nnn"> / nLevel=2 | </div>
    //<sect2> = <div id="S2nnn"> / nLevel=3 | </div>
    //<para> = <p> | </p>
    //<title> = if (nLevel == 0)
    //              = <title>
    //          else
    //              = <h[nLevel]>
    //          | </title>
    //
    //
    //<object id="xxxx"> = Replace("xxxx")


    wxString sElement = pElement->GetName();
    if (sElement == _T("bookinfo")) {
        return BookinfoToHtml(pElement, pFile);
    }
    else if (sElement == _T("chapter")) {
        return ChapterToHtml(pElement, pFile);
    }
    else if (sElement == _T("sect1")) {
        return SectToHtml(pElement, pFile, 1);
    }
    else if (sElement == _T("sect2")) {
        return SectToHtml(pElement, pFile, 2);
    }
    else if (sElement == _T("para")) {
        return ParaToHtml(pElement, pFile);
    }
    else if (sElement == _T("title")) {
        return TitleToHtml(pElement, pFile);
    }
    else if (sElement == _T("object")) {
        return ObjectToHtml(pElement, pFile);
    }
    else {
        wxLogMessage(_T("Error: Found tag <%s>. No treatment defined."), sElement);
        return true;
    }

}

wxFile* lmHtmlConverter::StartPoFile(wxString sFilename)
{
    wxFileName oFNP( sFilename );
    oFNP.SetExt(_T("po"));
    wxFile* pPoFile = new wxFile(oFNP.GetFullName(), wxFile::write);
    if (!pPoFile->IsOpened()) {
        wxLogMessage(_T("Error: File %s can not be created"), oFNP.GetFullName());
        return (wxFile*)NULL;        //error
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

    pPoFile->Write(sHeader);

    return pPoFile;

}

void lmHtmlConverter::AddToPoFile(wxString& sText)
{
    //add text to PO file

    if (!m_pPoFile || sText == _T("")) return;

    m_pPoFile->Write(_T("msgid \""));
    m_pPoFile->Write(sText + _T("\"\n"));
    m_pPoFile->Write(_T("msgstr \"\"\n"));


}

//------------------------------------------------------------------------------------
// Methods related to processing objects
//------------------------------------------------------------------------------------

bool lmHtmlConverter::LoadObjectsFile(const wxString& sFilename)
{
    // returns true if error

    // load the XML file as tree of nodes
    wxXmlDocument xdoc;
    if (!xdoc.Load(sFilename)) {
        wxLogMessage(_T("Error parsing eBook OBJ file ") + sFilename);
        return true;
    }

    //Verify type of document. Must be <book_object>
    wxXmlNode *pRoot = xdoc.GetRoot();
    if (pRoot->GetName() != _T("book_objects")) {
        wxLogMessage(
            _T("Error. First tag is not <book_objects> but <%s>"),
            pRoot->GetName() );
        return true;
    }

    //Create array of objects
    //TODO
    return false;

}

//------------------------------------------------------------------------------------
// Tags' processors
//------------------------------------------------------------------------------------


bool lmHtmlConverter::BookToHtml(wxXmlNode* pNode, wxFile* pFile)
{
    // receives and processes a <book> node and its children.
    // return true if error
    //<book id="xxxx"> = <html><head> | </html>

    // convert tag: output to open html tags
    wxString sNil = _T("");
    wxString sCharset = _T("utf-8");
    wxString sHtml = sNil +
        _T("<html>\n<head>\n")
        _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") + sCharset +
        _T("\">\n");
        _T("</head>\n<body>\n\n<ul>\n");
    pFile->Write(sHtml);

    // tag processing implications
    m_nHeaderLevel = -1;        //not initialized

    //process tag's children
    bool fError = ProcessChildren(pNode, pFile);

    //convert tag: output to close html tags
    pFile->Write(_T("</html>\n"));

    return fError;

}

bool lmHtmlConverter::BookinfoToHtml(wxXmlNode* pNode, wxFile* pFile)
{
    // receives and processes a <bookinfo> node and its children.
    // return true if error
    //<bookinfo> = nil / nLevel = 0

    // convert tag: output to open html tags
    // -- no output implied by this tag --

    // tag processing implications
    m_nHeaderLevel = 0;

    //process tag's children
    bool fError = ProcessChildren(pNode, pFile);

    //convert tag: output to close html tags
    // -- no output implied by this tag --

    return fError;
}

bool lmHtmlConverter::SectToHtml(wxXmlNode* pNode, wxFile* pFile, int nLevel)
{
    // receives and processes a <sectN> node and its children.
    // return true if error
    //<sect1> = <div id="SNnnn"> / nLevel=N | </div>

    wxLogMessage(_T("HTML Converter: Entering coverter for tag <sect>"));

    // convert tag: output to open html tags
    pFile->Write( wxString::Format(_T("<div id=\"S%dxxxx\">\n"), nLevel));
 
    // tag processing implications
    m_nHeaderLevel = nLevel+1;

    //process tag's children
    bool fError = ProcessChildren(pNode, pFile);

    //convert tag: output to close html tags
    pFile->Write(_T("</div>\n"));

    return fError;
}

bool lmHtmlConverter::ParaToHtml(wxXmlNode* pNode, wxFile* pFile)
{
    // receives and processes a <bookinfo> node and its children.
    // return true if error
    //<bookinfo> = nil / nLevel = 0

    wxLogMessage(_T("HTML Converter: Entering coverter for tag <para>"));

    // convert tag: output to open html tags
    pFile->Write(_T("<p>"));

    // tag processing implications
    wxString sText = wxEmptyString; //m_pParser->GetText(pNode);
    pFile->Write(sText);                        //write text in paragraph
    if (m_pPoFile) AddToPoFile(sText);      //add text to PO file

    //process tag's children
    bool fError = ProcessChildren(pNode, pFile);

    //convert tag: output to close html tags
    pFile->Write(_T("</p>\n"));

    return fError;
}

bool lmHtmlConverter::ChapterToHtml(wxXmlNode* pNode, wxFile* pFile)
{
    // receives and processes a <chapter> node and its children.
    // return true if error
    //<chapter id="xxxx"> = </head><body><div id="Cxxxx"> / nLevel=1 | </div></body>

    wxLogMessage(_T("HTML Converter: Entering coverter for tag <chapter>"));

    // convert tag: output to open html tags
    pFile->Write(_T("</head>\n\n<body>\n<div id=\"Cxxxx\">\n"));
 
    // tag processing implications
    m_nHeaderLevel = 1;

    //process tag's children
    bool fError = ProcessChildren(pNode, pFile);

    //convert tag: output to close html tags
    pFile->Write(_T("</div>\n\n</body>\n"));

    return fError;
}

bool lmHtmlConverter::TitleToHtml(wxXmlNode* pNode, wxFile* pFile)
{
    // receives and processes a <title> node and its children.
    // return true if error
    //<title> = if (nLevel == 0)
    //              = <title>
    //          else
    //              = <h[nLevel]>
    //          | </title>

    wxLogMessage(_T("HTML Converter: Entering coverter for tag <title>"));

    // convert tag: output to open html tags
    if (m_nHeaderLevel == 0)
        pFile->Write(_T("<title>"));
    else
        pFile->Write(wxString::Format(_T("<h%d>"), m_nHeaderLevel));

    // tag processing implications
    pFile->Write( _T(""));  //m_pParser->GetText(pNode) );    //write text in paragraph

    //process tag's children
    bool fError = ProcessChildren(pNode, pFile);

    //convert tag: output to close html tags
    if (m_nHeaderLevel == 0)
        pFile->Write(_T("</title>\n"));
    else
        pFile->Write(wxString::Format(_T("</h%d>\n"), m_nHeaderLevel));

    return fError;
}

bool lmHtmlConverter::ObjectToHtml(wxXmlNode* pNode, wxFile* pFile)
{
    // receives and processes a <object> node and its children.
    // return true if error
    //<object id="xxxx"> = Replace("xxxx")

    wxLogMessage(_T("HTML Converter: Entering coverter for tag <object>"));

    // convert tag: output to open html tags
    pFile->Write(_T("<object>"));

    // tag processing implications
    //pFile->Write( m_pParser->GetText(pNode) );    //write text in paragraph

    //process tag's children
    //bool fError = ProcessChildren(pNode, pFile);

    //convert tag: output to close html tags
    pFile->Write(_T("</object>\n"));

    return false;
}
