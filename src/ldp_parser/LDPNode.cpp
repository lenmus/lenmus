//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#include <algorithm>
#include "../score/defs.h"
#include "LDPNode.h"
#if lmUSE_LIBRARY
    #include "lmlib_Elements.h"
#endif

// lmLDPNode represents an element of the LDP representation language.
//
//	There are two types of nodes: simples and compounds.
//	Simple nodes are strings
//	Compound nodes are (node ... node)

#if lmUSE_LIBRARY

//wxString lmLDPNode::ToString()
//{
//    return lmToWxString( to_string() );
//}
//
//lmLDPNode lmLDPNode::GetParameter(int i)
//{
//    return static_cast<lmLDPNode>(get_parameter(i));
//}
//
//lmLDPNode lmLDPNode::GetParameter(wxString& sName) const
//{
//    return static_cast<lmLDPNode>( get_parameter( lmToStdString(sName) ) );
//}
//
//lmLDPNode lmLDPNode::StartIterator(long iP, bool fOnlyNotProcessed)
//{
//    return static_cast<lmLDPNode>(start_iterator(iP, fOnlyNotProcessed));
//}
//
//lmLDPNode lmLDPNode::GetNextParameter(bool fOnlyNotProcessed)
//{
//    return static_cast<lmLDPNode>(get_next_parameter(fOnlyNotProcessed));
//}


//-------------------------------------------------------------------------------
#else

lmLDPNode::lmLDPNode(wxString sData, long nNumLine, bool fIsParameter)
    : m_nID(lmNEW_ID)
    , m_nNumLine(nNumLine)
    , m_fIsSimple(true)
    , m_fProcessed(false)
{
    if (!fIsParameter)
    {
        //it is the name of an element. Find its ID
        int i = sData.Find(_T('#'));
        if (i > 0)
        {
            m_sName = sData.Mid(0, i);
            sData.Mid(i+1).ToLong(&m_nID);
        }
        else
            m_sName = sData;
    }
    else
        m_sName = sData;
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
    lmLDPNode* pNode = new lmLDPNode(sData, m_nNumLine, true);  //true -> it's a parameter node
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

lmLDPNode* lmLDPNode::GetParameter(int i)
{
    // parameter numbers are 1 based
    wxASSERT(i > 0 && i <= (int)m_cNodes.size());
    return m_cNodes[i-1];
}

lmLDPNode* lmLDPNode::StartIterator(long iP, bool fOnlyNotProcessed)
{
    //Set initial position
    if (iP > (int)m_cNodes.size())
        m_it = m_cNodes.end();
    else if (iP == 1)
        m_it = m_cNodes.begin();
    else
        m_it = std::find(m_cNodes.begin(), m_cNodes.end(), m_cNodes[iP-1]);

    //return object
    if (m_it == m_cNodes.end()) 
        return (lmLDPNode*)NULL;

    if (fOnlyNotProcessed && !(*m_it)->IsProcessed())
            return *m_it;
    
    return GetNextParameter(fOnlyNotProcessed);
}

lmLDPNode* lmLDPNode::GetNextParameter(bool fOnlyNotProcessed)
{
    if (m_it == m_cNodes.end()) 
        return (lmLDPNode*)NULL;

    //advance to next one
    ++m_it;
    while (m_it != m_cNodes.end()) 
    {
        if (fOnlyNotProcessed)
        {
            if (!(*m_it)->IsProcessed())
                return *m_it;
        }
        else
            return *m_it;

        ++m_it;
    }
    //no more items or all processed
    return (lmLDPNode*)NULL;
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


#endif
