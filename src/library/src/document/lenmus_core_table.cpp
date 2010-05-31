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

#include "lenmus_document.h"
#include "lenmus_document_cursor.h"
#include "lenmus_core_table.h"

using namespace std;

namespace lenmus
{

//-------------------------------------------------------------------------------------
// CoreTable implementation
//-------------------------------------------------------------------------------------

CoreTable::CoreTable(Document* pDoc, Document::iterator itScore)
    : m_pDoc(pDoc)
    , m_itScore(itScore)
{
    build_table();
} 

CoreTable::~CoreTable()
{
    std::vector<CoreTableEntry*>::iterator it;
    for (it=m_table.begin(); it != m_table.end(); ++it)
        delete *it;
    m_table.clear();
}

void CoreTable::build_table()
{
    if (*m_itScore == NULL)
        return;

    int nSegment = 0;
    float rTime = 0.0f;
    int nInstr = 0;
    int nVoice = 0;
    int nStaff = 0;

    DocIterator cursor(m_pDoc);
    cursor.point_to(*m_itScore);
    cursor.enter_element();
    cursor.start_of_instrument(0);
    while(*cursor)
    {
        CoreTableEntry* pEntry = 
            new CoreTableEntry(nSegment, rTime, nInstr, nVoice, nStaff,
                               cursor.get_iterator());
        m_table.push_back(pEntry);
        if ((*cursor)->get_type() == k_barline)
            ++nSegment;
        ++cursor;
    }
}
void CoreTable::dump()
{
    std::vector<CoreTableEntry*>::iterator it;
    cout << "Num.entries = " << num_entries() << endl;
    for (it=m_table.begin(); it != m_table.end(); ++it)
    {
        (*it)->dump();
    }
}



}  //namespace lenmus
