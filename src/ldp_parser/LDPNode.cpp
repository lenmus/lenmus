
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

// lmLDPNode represents an element of the LDP representation language.
//
//	There are two types of nodes: simples and compounds.
//	Simple nodes are strings
//	Compound nodes are (node ... node)


lmLDPNode::lmLDPNode(wxString sData)
{
    m_sName = sData;
    m_fIsSimple = true;
}

lmLDPNode::~lmLDPNode()
{
	for(int i=0; i < (int)m_cNodes.size(); i++)
	{	
		delete m_cNodes[i];
	}
	m_cNodes.clear();
}

void lmLDPNode::AddParameter(wxString sData)
{
    lmLDPNode* pNode = new lmLDPNode(sData);
    AddNode(pNode);
}

void lmLDPNode::AddNode(lmLDPNode* pNode)
{
	m_cNodes.push_back(pNode);
    m_fIsSimple = false;
}

int lmLDPNode::GetNumParms()
{
	return (int)m_cNodes.size();
}

lmLDPNode* lmLDPNode::GetParameter(long i)
{
    // parameter numbers are 1 based
    wxASSERT(i > 0 && i <= (int)m_cNodes.size());
    return m_cNodes[i-1];
}

lmLDPNode* lmLDPNode::GetParameter(wxString& sName) const
{
	for(int i=0; i < (int)m_cNodes.size(); i++)
	{	
        if (m_cNodes[i]->GetName() == sName)
            return m_cNodes[i];
    }
    return (lmLDPNode*)NULL;
}

void lmLDPNode::DumpNode(wxString sIndent)
{
    lmLDPNode* pX;
    wxString sName = m_sName;
    wxString sMsg = wxString::Format(_T("%sNode: %s, num.parms=%d"),
                        sIndent.c_str(), sName.c_str(), (int)m_cNodes.size());
    wxLogMessage( _T("**DUMP OF NODE** %s"), sMsg.c_str() );
    wxString sSpaces = sIndent;
    sSpaces += _T("   ");
	for(int i=1; i <= (int)m_cNodes.size(); i++)
	{	
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
    for (int i = 1; i <= (int)m_cNodes.size(); i++) {
        pX = GetParameter(i);
        if (pX->IsSimple()) {
            sResp = wxString::Format( _T("%s %s"), sResp.c_str(), pX->GetName().c_str() );
        } else {
            sResp = wxString::Format( _T("%s %s"), sResp.c_str(), pX->ToString().c_str() );
        }
    }

    return wxString::Format(_T("%s)"), sResp.c_str());

}
