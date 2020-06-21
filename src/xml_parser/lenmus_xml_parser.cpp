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

//lenmus
#include "lenmus_xml_parser.h"

#include "lenmus_string.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/textfile.h>
#include <wx/log.h>
#include <wx/dir.h>             // to search directories
#include <wx/filename.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
wxXmlNode* XmlParser::GetNextSibling(wxXmlNode* pNode)
{
    // Return next sibling element or nullptr if no more
    pNode = pNode->GetNext();
    while (pNode && pNode->GetType() != wxXML_ELEMENT_NODE)
        pNode = pNode->GetNext();
    return pNode;
}

//---------------------------------------------------------------------------------------
wxXmlNode* XmlParser::GetFirstChild(wxXmlNode* pNode)
{
    // Return first child element or nullptr if no more
    pNode = pNode->GetChildren();
    while (pNode && pNode->GetType() != wxXML_ELEMENT_NODE)
        pNode = pNode->GetNext();
    return pNode;
}

//---------------------------------------------------------------------------------------
wxString XmlParser::GetAttribute(wxXmlNode* pNode, wxString sName, wxString sDefault)
{
    wxXmlAttribute* pAttrib = pNode->GetAttributes();
    while(pAttrib) {
        if (pAttrib->GetName() == sName)
            return pAttrib->GetValue();
        pAttrib = pAttrib->GetNext();
    }

    if (sDefault == "")
        ParseError(
            _T("Attribute \"%s\" not found in tag <%s>."),
            sName.wx_str(), pNode->GetName().wx_str() );

    return sDefault;
}

//---------------------------------------------------------------------------------------
bool XmlParser::GetYesNoAttribute(wxXmlNode* pNode, wxString sName, bool fDefault)
{
    wxXmlAttribute* pAttrib = pNode->GetAttributes();
    while(pAttrib) {
        if (pAttrib->GetName() == sName) {
            wxString sValue = pAttrib->GetValue();
            if (sValue == "yes")
                return true;
            else if (sValue == "no")
                return false;
            else {
                ParseError(
                    _T("Yes-no attribute \"%s\" has an invalid value \"%s\"."),
                    sName.wx_str(), sValue.wx_str() );
                return fDefault;
            }
        }
        pAttrib = pAttrib->GetNext();
    }
    return fDefault;
}

//---------------------------------------------------------------------------------------
wxString XmlParser::GetText(wxXmlNode* pElement)
{
    //    Receives node of type ELEMENT and returns its text content
    wxASSERT(pElement->GetType() == wxXML_ELEMENT_NODE);

    wxXmlNode* pNode = pElement->GetChildren();
    wxString sName = pElement->GetName();
    wxString sValue = "";

    if (pNode && pNode->GetType() == wxXML_TEXT_NODE) {
        sValue = pNode->GetContent();
    }
    return sValue;
}

//---------------------------------------------------------------------------------------
void XmlParser::DumpXMLTree(wxXmlNode *pRoot)
{
    if (pRoot == nullptr) return;
    if (pRoot->GetType() != wxXML_ELEMENT_NODE) return;

    wxXmlNode *pNode = pRoot->GetChildren();
    while (pNode)
    {
        // if final node dump content
        if ((pNode->GetType() == wxXML_TEXT_NODE ||
             pNode->GetType() == wxXML_CDATA_SECTION_NODE))
        {
            wxLogMessage("Node: [%s] = \"%s\"", pNode->GetName().wx_str(),
                    pNode->GetContent().wx_str() );
        }

        // dump subnodes:
        if (pNode->GetType() == wxXML_ELEMENT_NODE)
        {
            wxLogMessage("Element: [%s]", pNode->GetName().wx_str() );
            DumpXMLTree(pNode);
        }

        pNode = pNode->GetNext();
    }
}

//---------------------------------------------------------------------------------------
void XmlParser::ParseError(const wxChar* szFormat, ...)
{
    m_nErrors++;
    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg = wxString::FormatV(szFormat, argptr);
    LOMSE_LOG_ERROR( to_std_string(sMsg) );
    va_end(argptr);
}

//---------------------------------------------------------------------------------------
void XmlParser::TagError(const wxString sElement, const wxString sTagName, wxXmlNode* pElement)
{
    m_nErrors++;
    wxString sMsg = wxString::Format(
        "Parsing <%s>: tag <%s> not supported.",
        sElement.wx_str(), sTagName.wx_str() );
    wxLogMessage(sMsg);

    if (pElement) {
        //TODO Log source code or reference to source line. But how?
        //wxLogMessage(pElement->));
    }
}


}   //namespace lenmus
