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

#include <sstream>

#include "lenmus_ldp_elements.h"
#include "lenmus_document.h"
#include "lenmus_user_command.h"
#include "lenmus_command.h"
#include "lenmus_mvc_builder.h"
#include "lenmus_document_cursor.h"
#include "lenmus_im_note.h"
#include "lenmus_compiler.h"

using namespace std;

namespace lenmus
{

//-------------------------------------------------------------------------------------
// Command implementation
//-------------------------------------------------------------------------------------

CmdInsertElement::CmdInsertElement(const string& name, DocCursor& cursor,
                                   LdpElement* pElm, LdpCompiler* pCompiler) 
    : UserCommand(name)
    , m_pElm(pElm)
    , m_pCompiler(pCompiler)
{
    if (cursor.is_at_end_of_staff())
    {
        m_fPushBack = true;
        m_it = cursor.get_musicData_for_current_instrument();
        m_pGoBackElm = NULL;
        m_pGoFwdElm = NULL;
    }
    else
    {
        m_fPushBack = false;
        m_it = determine_source_insertion_point(cursor, pElm);
        m_pGoBackElm = determine_if_go_back_needed(cursor, pElm);
        m_pGoFwdElm = determine_if_go_fwd_needed(cursor, pElm);
    }
}

bool CmdInsertElement::do_actions(DocCommandExecuter* pExec)
{
    if (m_pGoBackElm)
        execute_insert(pExec, m_it, m_pGoBackElm);
    if (m_pGoFwdElm)
        execute_insert(pExec, m_it, m_pGoFwdElm);
    execute_insert(pExec, m_it, m_pElm);
    return true;
}

LdpElement* CmdInsertElement::determine_source_insertion_point(DocCursor& cursor, LdpElement* pElm)
{
    if (pElm->is_type(k_note) || pElm->is_type(k_rest))
    {
        ImNoteRest* pNR = dynamic_cast<ImNoteRest*>( (*cursor)->get_imobj() );
        int nCursorVoice = pNR->get_voice();
        ImNoteRest* pNewNR = dynamic_cast<ImNoteRest*>( pElm->get_imobj() );
        int nNewVoice = pNewNR->get_voice();
        if (nCursorVoice == nNewVoice)
            return *cursor;
        else
        {
            DocCursor cursor2(cursor);
            //advance to barline or to end of staff
            while (*cursor2 != NULL && (*cursor2)->get_type() != k_barline)
                cursor2.move_next();
            //if at end of staff, change command to push back
            if (*cursor2 == NULL)
            {
                m_fPushBack = true;
                return cursor2.get_musicData_for_current_instrument();
            }
            else
                return *cursor2;
        }
    }
    else
        return *cursor;
}

LdpElement* CmdInsertElement::determine_if_go_back_needed(DocCursor& cursor, LdpElement* pElm)
{
    if (pElm->is_type(k_note) || pElm->is_type(k_rest))
    {
        ImNoteRest* pNR = dynamic_cast<ImNoteRest*>( (*cursor)->get_imobj() );
        int nCursorVoice = pNR->get_voice();
        ImNoteRest* pNewNR = dynamic_cast<ImNoteRest*>( pElm->get_imobj() );
        int nNewVoice = pNewNR->get_voice();
        if (nCursorVoice == nNewVoice)
            return NULL;
        else
        {
            LdpElement* goBack = m_pCompiler->create_element("(goBack start)");
            return goBack;
        }
    }
    else
    {
        LdpElement* goBack = m_pCompiler->create_element("(goBack start)");
        return goBack;
    }
}

LdpElement* CmdInsertElement::determine_if_go_fwd_needed(DocCursor& cursor, LdpElement* pElm)
{
    if (m_pGoBackElm && is_greater_time(cursor.time(), 0.0f))
    {
        stringstream s;
        s << "(goFwd " << cursor.time() << ")";
        LdpElement* goFwd = m_pCompiler->create_element(s.str());
        return goFwd;
    }
    else
        return NULL;
}

void CmdInsertElement::execute_insert(DocCommandExecuter* pExec,
                                      Document::iterator& it, LdpElement* pNewElm)
{
    if (m_fPushBack)
        pExec->execute( new DocCommandPushBack(it, pNewElm) );
    else
        pExec->execute( new DocCommandInsert(it, pNewElm) );
}


}  //namespace lenmus
