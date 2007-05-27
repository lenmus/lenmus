//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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
#pragma implementation "XmlParser.h"
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

#include "XMLParser.h"


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
            sName.c_str(), pNode->GetName().c_str() );

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
                    sName.c_str(), sValue.c_str() );
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
            wxLogMessage(_T("Node: [%s] = \"%s\""), pNode->GetName().c_str(),
                    pNode->GetContent().c_str() );
        }

        // dump subnodes:
        if (pNode->GetType() == wxXML_ELEMENT_NODE)
        {
            wxLogMessage(_T("Element: [%s]"), pNode->GetName().c_str() );
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
        sElement.c_str(), sTagName.c_str() );
    wxLogMessage(sMsg);

    if (pElement) {
        //! @todo Log source code or reference to source line. But how?
        //wxLogMessage(pElement->));
    }
}

