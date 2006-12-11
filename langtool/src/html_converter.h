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
// #pragma interface
#endif

#ifndef __HTML_CONVERTER_H__        //to avoid nested includes
#define __HTML_CONVERTER_H__

#include "wx/wfstream.h"
#include "wx/xml/xml.h"          // to use wxXmlDocument

//#include "parser.h"

enum lmHtmlFlags
{
    lmHTML_NOGENERATE = 0,    // don't generate html files
    lmHTML_GENERATE = 1       // generate html files
};

class lmHtmlConverter
{
public:
    lmHtmlConverter();
    ~lmHtmlConverter();

    bool ConvertToHtml(const wxString& sFilename, bool fIncludeObjects, 
                       wxFile* pPoFile);
    wxFile* StartLangFile(wxString sFilename);


private:

    // Tags' processors
    bool BookToHtml(wxXmlNode* pNode, wxFile* pFile);
    bool BookinfoToHtml(wxXmlNode* pNode, wxFile* pFile);
    bool ChapterToHtml(wxXmlNode* pNode, wxFile* pFile);
    bool SectToHtml(wxXmlNode* pNode, wxFile* pFile, int nLevel);
    bool ParaToHtml(wxXmlNode* pNode, wxFile* pFile);
    bool TitleToHtml(wxXmlNode* pNode, wxFile* pFile);
    bool ObjectToHtml(wxXmlNode* pNode, wxFile* pFile);

    // Parsing methods
    bool ProcessChildren(wxXmlNode* pNode, wxFile* pFile);
    bool ProcessTag(wxXmlNode* pElement, wxFile* pFile);

    // Objects processing
    bool LoadObjectsFile(const wxString& sFilename);

    //PO file processing
    void AddToPoFile(wxString& sText);


    //lmXmlParser*    m_pParser;
    int             m_nHeaderLevel;
    int             m_nIndentLevel;         // to indent html output
    bool            m_fIncludeObjects;
    wxFile*         m_pLangFile;

};

#endif    // __HTML_CONVERTER_H__
