// RCS-ID: $Id: LDPNode.h,v 1.3 2006/02/23 19:21:45 cecilios Exp $
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
/*! @file LDPNode.h
    @brief Header file for class lmLDPNode
    @ingroup ldp_parser
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __LMNODE_H        //to avoid nested includes
#define __LMNODE_H

class lmLDPNode;
//// node parameters (also LMNodes) will be maintained in a list structure. Let's declare it
//#include "wx/list.h"
//WX_DECLARE_LIST(lmLDPNode, LMNodeList);

#include "wx/dynarray.h"
WX_DEFINE_ARRAY(lmLDPNode*, ArrayNodePtrs);




class lmLDPNode
{
public:
    lmLDPNode(wxString sData);
    ~lmLDPNode();
    //void Copy(lmLDPNode* pNode);
    void        DumpNode(wxString sIndent=_T(""));
    wxString    ToString();


    bool        IsSimple() { return m_fIsSimple; }
    wxString    GetName() { return m_sName; }
    int            GetNumParms();
    lmLDPNode*        GetParameter(long i);
    void        AddParameter(wxString sData);
    void        AddNode(lmLDPNode* pNode);

private:
    //int AssignID();

    wxString        m_sName;            // node name
    bool            m_fIsSimple;        // the node is simple (just a string)
    ArrayNodePtrs    m_cNodes;            // Parameters of this node
    int                m_nID;

};

#endif    // __LMNODE_H
