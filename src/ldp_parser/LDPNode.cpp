
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

/*! @class lmLDPNode
    @ingroup ldp_parser
    @brief lmLDPNode represents an element of the LDP representation language.

    There are two types of nodes: simples and compounds.
    Simple nodes are strings
    Compound nodes are (node ... node)

*/
#ifdef __GNUG__
#pragma implementation "LDPNode.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "LDPNode.h"

// Definition of the LMNodeList class
//#include <wx/listimpl.cpp>
//WX_DEFINE_LIST(LMNodeList);

lmLDPNode::lmLDPNode(wxString sData)
{
    m_sName = sData;
    m_fIsSimple = true;
    //m_nID = AssignID();
}

lmLDPNode::~lmLDPNode()
{
    long i = m_cNodes.GetCount();

    //wxLogMessage(
    //    _T("**TRACE** Entering lmLDPNode destructor: node=<%s>, ID=%d, nParms=%d"), m_sName, m_nID, i);

    for(; i > 0; i--) {
        delete m_cNodes.Item(i-1);
        m_cNodes.RemoveAt(i-1);
    }
}

//void lmLDPNode::Copy(lmLDPNode* pNode)
//{
//    m_fIsSimple = pNode->IsSimple();
//    for(long i=1; i <= pNode->GetNumParms(); i++) {
//        m_cNodes.Add(pNode->GetParameter(i));
//    }
//}
//
void lmLDPNode::AddParameter(wxString sData)
{
    //wxLogMessage(
    //    _T("**TRACE** Adding parameter name=<%s> to node name <%s>"),
    //    sData, m_sName);
    lmLDPNode* pNode = new lmLDPNode(sData);
    AddNode(pNode);
}

void lmLDPNode::AddNode(lmLDPNode* pNode)
{
    //wxLogMessage(
    //    _T("**TRACE** Adding node name=<%s> to node name <%s> as parm num %d"),
    //    pNode->GetName(), m_sName, m_cNodes.GetCount()+1);
    m_cNodes.Add(pNode);
    m_fIsSimple = false;
}

int lmLDPNode::GetNumParms()
{
    return m_cNodes.GetCount();
}

//int lmLDPNode::AssignID()
//{
//    static int nCounter=0;
//    return ++nCounter;
//}
//
lmLDPNode* lmLDPNode::GetParameter(long i)
{
    // parameter numbers are 1 based
    wxASSERT(i > 0 && i <= (long)m_cNodes.GetCount());
    return (lmLDPNode*)m_cNodes.Item(i-1);
}

void lmLDPNode::DumpNode(wxString sIndent)
{
    lmLDPNode* pX;
    long i;
    wxString sName = m_sName;
    wxString sMsg = wxString::Format(_T("%sNode: %s, num.parms=%d"),
                        sIndent.c_str(), sName.c_str(), m_cNodes.GetCount());
    wxLogMessage( _T("**DUMP OF NODE** %s"), sMsg.c_str() );
    wxString sSpaces = sIndent;
    sSpaces += _T("   ");
    for (i = 1; i <= (long)m_cNodes.GetCount(); i++) {
        pX = this->GetParameter(i);
        if (pX->IsSimple()) {
            sName = pX->GetName();
            sMsg = wxString::Format(_T("%s   Parm %d : %s"),
                        sIndent.c_str(), i, sName.c_str());
            wxLogMessage( _T("**DUMP OF NODE** %s"), sMsg.c_str() );
        } else {
            pX->DumpNode(sSpaces);
        }
    }

}

wxString lmLDPNode::ToString()
{
    lmLDPNode* pX;
    wxString sResp = wxString::Format( _T("(%s"), m_sName.c_str() );
    for (int i = 1; i <= (int)m_cNodes.GetCount(); i++) {
        pX = GetParameter(i);
        if (pX->IsSimple()) {
            sResp = wxString::Format( _T("%s %s"), sResp.c_str(), pX->GetName().c_str() );
        } else {
            sResp = wxString::Format( _T("%s %s"), sResp.c_str(), pX->ToString().c_str() );
        }
    }

    return wxString::Format(_T("%s)"), sResp.c_str());

}
