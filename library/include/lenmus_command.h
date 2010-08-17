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

#ifndef __LML_COMMAND_H__
#define __LML_COMMAND_H__


using namespace std;

namespace lenmus
{

//forward declarations
class DocCursor;
class LdpElement;


class CmdInsertElement : public UserCommand
{
public:
    CmdInsertElement(const string& name, DocCursor& cursor, LdpElement* pElm,
                     LdpCompiler* pCompiler);
    virtual ~CmdInsertElement() {};

protected:
    bool do_actions(DocCommandExecuter* pExec);
    virtual LdpElement* determine_source_insertion_point(DocCursor& cursor, LdpElement* pElm);
    virtual LdpElement* determine_if_go_back_needed(DocCursor& cursor, LdpElement* pElm);
    virtual LdpElement* determine_if_go_fwd_needed(DocCursor& cursor, LdpElement* pElm);
    void execute_insert(DocCommandExecuter* pExec, Document::iterator& it, LdpElement* pNewElm);

    Document::iterator  m_it;
    LdpElement*         m_pElm;
    bool                m_fPushBack;
    LdpElement*         m_pGoBackElm;
    LdpElement*         m_pGoFwdElm;
    LdpCompiler*        m_pCompiler;

};



}   //namespace lenmus

#endif      //__LML_COMMAND_H__
