//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_XML_PARSER_H__        //to avoid nested includes
#define __LENMUS_XML_PARSER_H__

//lenmus
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/xml/xml.h>          // to use wxXmlDocument


namespace lenmus
{


//---------------------------------------------------------------------------------------
class XmlParser
{
private:
    int m_nErrors;        // number of parsing errors

public:
    XmlParser() {}
    ~XmlParser() {}

    // auxiliary XML methods
    wxXmlNode* GetFirstChild(wxXmlNode* pNode);
    wxXmlNode* GetNextSibling(wxXmlNode* pNode);
    wxString GetAttribute(wxXmlNode* pNode, wxString sName, wxString sDefault = "");
    bool GetYesNoAttribute(wxXmlNode* pNode, wxString sName, bool fDefault=true);
    wxString GetText(wxXmlNode* pElement);

    void DumpXMLTree(wxXmlNode *pRoot);
    void ParseError(const wxChar* szFormat, ...);
    void TagError(const wxString sElement, const wxString sTagName, wxXmlNode* pElement = nullptr);

};



}   //namespace lenmus

#endif    // __LENMUS_XML_PARSER_H__
