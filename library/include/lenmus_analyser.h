//--------------------------------------------------------------------------------------
//  LenMus Library
//  Copyright (c) 2010 LenMus project
//
//  This program is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along
//  with this library; if not, see <http://www.gnu.org/licenses/> or write to the
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
//  MA  02111-1307,  USA.
//
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LML_ANALYSER_H__ 
#define __LML_ANALYSER_H__

#include "lenmus_ldp_elements.h"

using namespace std;

namespace lenmus
{

//forward declarations
class ElementAnalyser;
class ImObj;
class ImStaffObj;
class ImNoteRest;
class LdpFactory;


class Analyser
{
protected:
    ostream&    m_reporter;
    LdpFactory* m_pLdpFactory;
    LdpTree*    m_pTree;
    int         m_curStaff;
    int         m_curVoice;

public:
    Analyser(ostream& reporter, LdpFactory* pFactory);

    void analyse_tree(LdpTree* tree);
    void analyse_node(LdpTree::iterator itNode);
    void analyse_node(LdpElement* pNode);
    ElementAnalyser* new_analyser(ELdpElements type);

    //auxiliary
    void erase_node(LdpElement* pNode);
    //void replace_node(LdpElement* pNode, const std::string& newContent);
    void replace_node(LdpElement* pOldNode, LdpElement* pNewNode);

    //inherited values management
    inline void change_staff(int nStaff) { m_curStaff = nStaff; }
    void set_staff(ImStaffObj* pSO);
    inline void change_voice(int nVoice) { m_curVoice = nVoice; }
    void set_voice(ImNoteRest* pNR);

};



}   //namespace lenmus

#endif      //__LML_ANALYSER_H__
