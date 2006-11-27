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
// #pragma interface
#endif

#ifndef __EBOOK_PROCESSOR_H__        //to avoid nested includes
#define __EBOOK_PROCESSOR_H__

#include "wx/wfstream.h"


class wxXml2Node;

class ltEbookProcessor
{
public:
    ltEbookProcessor();
    ~ltEbookProcessor();

    bool GenerateLMB(wxString sfilename);

private:
    // Tags' processors
    bool BookTag(const wxXml2Node &oNode);
    bool BookinfoTag(const wxXml2Node &oNode);
    bool ChapterTag(const wxXml2Node &oNode);
    bool ItemizedlistTag(const wxXml2Node &oNode);
    bool LinkTag(const wxXml2Node &oNode);
    bool ListitemTag(const wxXml2Node &oNode);
    bool ObjectTag(const wxXml2Node &oNode);
    bool ParaTag(const wxXml2Node &oNode);
    bool PartTag(const wxXml2Node &oNode);
    bool SectionTag(const wxXml2Node &oNode);
    bool ThemeTag(const wxXml2Node &oNode);
    bool TitleTag(const wxXml2Node &oNode);


    // Parsing methods
    bool ProcessChildren(const wxXml2Node &oNode);
    bool ProcessTag(const wxXml2Node& oNode);

    // File generation methods
    bool StartTocFile(wxString sFilename);
    void TerminateTocFile();
    void WriteToToc(wxString sText, bool fIndent=true);
    bool StartHtmlFile(wxString sFilename);
    void TerminateHtmlFile();
    void WriteToHtml(wxString sText);

    //XML parsing
    wxString GetText(const wxXml2Node &oNode);



    // member variables
    bool            m_fIncludeObjects;
    wxFile*         m_pPoFile;

    //files
    wxString        m_sFilename;            // full path & name of xml file being processed
    wxFile*         m_pTocFile;
    wxFile*         m_pHtmlFile;

    // variables for toc processing
    int             m_nTocIndentLevel;      // to indent output
    bool            m_fTitleToToc;          // write title to toc

    // variables for html processing
    int             m_nHtmlIndentLevel;     // to indent output
    int             m_nNumHtmlPage;         // to generate html file number
    wxString        m_sHtmlPagename;        
    int             m_nHeaderLevel;

    // bookinfo data
    bool            m_fProcessingBookinfo;
    wxString        m_sBookTitle;

    // page/id cross-reference table


};


#endif    // __EBOOK_PROCESSOR_H__
