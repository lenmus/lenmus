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

#ifndef __LM_CORE_TABLE_H__
#define __LM_CORE_TABLE_H__

#include <vector>
#include <ostream>
#include <map>
#include "lenmus_document.h"

using namespace std;

namespace lenmus
{

//forward declarations
class DocIterator;
class Document;
class ImObj;
class ImStaffObj;
class ImNoteRest;
class ImGoBackFwd;


//-------------------------------------------------------------------------------------
// CoreTableEntry: an entry in the Core table
//-------------------------------------------------------------------------------------

class CoreTableEntry
{
protected:
    int                 m_segment;
    float               m_time;
    int                 m_instr;
    int                 m_line;
    int                 m_staff;
    LdpElement*         m_pElm;

public:
    CoreTableEntry(int segment, float time, int instr, int line, int staff,
                   LdpElement* pElm)    
            : m_segment(segment), m_time(time), m_instr(instr), m_line(line)
            , m_staff(staff), m_pElm(pElm) {}

    int segment() const { return m_segment; }
    float time() const { return m_time; }
    int num_instrument() const { return m_instr; }
    int line() const { return m_line; }
    int staff() const { return m_staff; }

    //debug
    void dump();
    std::string to_string() { return m_pElm->to_string(); }


protected:

};


//-------------------------------------------------------------------------------------
// CoreTable: encapsulates staff objects table for a score
//-------------------------------------------------------------------------------------

class CoreTable
{
protected:
    std::vector<CoreTableEntry*> m_table;

public:
    CoreTable();
    ~CoreTable();

    //void build_table();
    int num_entries() { return static_cast<int>(m_table.size()); }

    //table management
    void AddEntry(int segment, float time, int instr, int voice, int staff,
                  LdpElement* pElm);

    class iterator
    {
        protected:
            friend class CoreTable;
            std::vector<CoreTableEntry*>::iterator m_it;

        public:
            iterator() {}
			iterator(std::vector<CoreTableEntry*>::iterator& it) { m_it = it; }
            virtual ~iterator() {}

	        CoreTableEntry* operator *() const { return *m_it; }
            iterator& operator ++() { ++m_it; return *this; }
            iterator& operator --() { --m_it; return *this; }
		    bool operator ==(const iterator& it) const { return m_it == it.m_it; }
		    bool operator !=(const iterator& it) const { return m_it != it.m_it; }
    };

	iterator begin() { return iterator( m_table.begin() ); }
	iterator end() { return iterator( m_table.end() ); }

    //debug
    void dump();

protected:

};


//-------------------------------------------------------------------------------------
// StaffVoiceLineTable: algorithm assign and manage line number to voices/staves
//-------------------------------------------------------------------------------------

class StaffVoiceLineTable
{
protected:
    int                 m_lastAssignedLine;
    std::map<int, int>  m_lineForStaffVoice;    //key = 100*staff + voice
    std::vector<int>    m_firstVoiceForStaff;   //key = staff

public:
    StaffVoiceLineTable();

    int get_line_assigned_to(int nVoice, int nStaff);

private:
    int assign_line_to(int nVoice, int nStaff);
    inline int form_key(int nVoice, int nStaff) { return 100 * nStaff + nVoice; }

};


//-------------------------------------------------------------------------------------
// CoreTableBuilder: algorithm to create a CoreTable
//-------------------------------------------------------------------------------------

class CoreTableBuilder
{
protected:
    CoreTable*      m_pTable;
    DocIterator*    m_pItScore;
    Document*       m_pDoc;

public:
    CoreTableBuilder(Document* pDoc);
    ~CoreTableBuilder() {}

    CoreTable* build_table(DocIterator* pItScore);

private:
    //global counters to assign segment, timepos and staff
    int     m_nCurSegment;
    float   m_rCurTime;
    float   m_rMaxTime;
    int     m_nCurStaff;
    StaffVoiceLineTable m_lines;

    int find_number_of_instruments();
    void find_voices_per_staff(int nInstr);
    void create_entries(int nInstr);
    void sort_table();
    void reset_counters();
    int get_line_for(int nVoice, int nStaff);
    float determine_timepos(ImStaffObj* pSO);
    void update_segment(LdpElement* pElm);
    void update_time_counter(ImGoBackFwd* pGBF);
    void add_entry_for_staffobj(ImObj* pImo, int nInstr, LdpElement* pElm);
    void add_entries_for_key_signature(ImObj* pImo, int nInstr, LdpElement* pElm);


};


}   //namespace lenmus

#endif      //__LM_CORE_TABLE_H__
