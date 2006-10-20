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

#ifndef __HTMLPARSER_H__        //to avoid nested includes
#define __HTMLPARSER_H__

#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include "wx/xml/xml.h"          // to use wxXmlDocument


class lmXmlParser
{
public:
    lmXmlParser();
    ~lmXmlParser();

    void ParseFile(const wxString& filename); 
    void ParseBook(const wxString& sPath);


private:
    // auxiliary XML methods
    wxXmlNode* GetFirstChild(wxXmlNode* pNode);
    wxXmlNode* GetNextSibling(wxXmlNode* pNode);
    wxString GetAttribute(wxXmlNode* pNode, wxString sName, wxString sDefault = _T(""));
    bool GetYesNoAttribute(wxXmlNode* pNode, wxString sName, bool fDefault=true);
    wxString GetText(wxXmlNode* pElement);

    void DumpXMLTree(wxXmlNode *pRoot);
    void ParseError(const wxChar* szFormat, ...);
    void TagError(const wxString sElement, const wxString sTagName, wxXmlNode* pElement = NULL);

    // processing methods
    bool CreateBookIndex(wxXmlNode* pNode, const wxString& filename);
    wxString GetTitle(wxXmlNode* pNode);

    //output methods
    void WriteHHC(const wxString& sBookname, const wxString& sExt = _T(".hhc"));
    void WriteHHK(const wxString& sBookname);
    void WriteHHP(const wxString& sBookname);

        // member variables

    //index generation
    wxString        m_sBookTitle;
    wxArrayString   m_cChapterTitle;
    wxArrayString   m_cFileName;

    //error management
    int        m_nErrors;        // number of parsing errors


};
    
#endif    // __HTMLPARSER_H__
