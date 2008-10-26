//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_LDPNODE_H__        //to avoid nested includes
#define __LM_LDPNODE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "LDPNode.cpp"
#endif

#include <vector>

class lmLDPNode;
//#include "wx/dynarray.h"
//WX_DEFINE_ARRAY(lmLDPNode*, lmArrayNodePtrs);



class lmLDPNode
{
public:
    lmLDPNode(wxString sData, long nNumLine);
    ~lmLDPNode();

	void DumpNode(wxString sIndent=_T(""));
    wxString ToString();

    inline bool IsSimple() const { return m_fIsSimple; }
    inline bool IsProcessed() const { return m_fProcessed; }
    inline void SetProcessed(bool fValue) { m_fProcessed = fValue; }

    inline wxString GetName() const { return m_sName; }
    inline long GetNumLine() { return m_nNumLine; }
    int GetNumParms();

    //random access
    lmLDPNode* GetParameter(long i);
    lmLDPNode* GetParameter(wxString& sName) const;

    //iteration
    lmLDPNode* StartIterator(long iP=1, bool fOnlyNotProcessed = true);
    lmLDPNode* GetNextParameter(bool fOnlyNotProcessed = true);

    void AddParameter(wxString sData);
    void AddNode(lmLDPNode* pNode);


private:
    wxString        m_sName;            //node name
    long            m_nNumLine;         //LDP source file: line number
    bool            m_fIsSimple;        //the node is simple (just a string)
    bool            m_fProcessed;       //the node has been processed
	std::vector<lmLDPNode*> m_cNodes;	//Parameters of this node
    std::vector<lmLDPNode*>::iterator   m_it;       //for sequential accsess
};

#endif    // __LM_LDPNODE_H__
