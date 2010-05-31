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

#include "lenmus_core_table.h"
#include "lenmus_document_cursor.h"
#include "lenmus_elements.h"
#include "lenmus_internal_model.h"
#include "lenmus_im_note.h"

using namespace std;

namespace lenmus
{

//-------------------------------------------------------------------------------------
// CoreTableEntry implementation
//-------------------------------------------------------------------------------------

void CoreTableEntry::dump()
{
    //cout << to_string() << ", time=" << m_time << ", staff=" << m_staff 
    //     << ", line=" << m_line << endl;
    //segment     time       instr     line     staff     object
    cout << m_segment << "\t" << m_time << "\t" << m_instr << "\t" 
         << m_line << "\t" << m_staff << "\t" << to_string() << endl;

}

//-------------------------------------------------------------------------------------
// CoreTable implementation
//-------------------------------------------------------------------------------------

CoreTable::CoreTable()
{
} 

CoreTable::~CoreTable()
{
    std::vector<CoreTableEntry*>::iterator it;
    for (it=m_table.begin(); it != m_table.end(); ++it)
        delete *it;
    m_table.clear();
}

void CoreTable::AddEntry(int segment, float time, int instr, int voice, int staff,
                         LdpElement* pElm)
{
    CoreTableEntry* pEntry = 
        new CoreTableEntry(segment, time, instr, voice, staff, pElm);
    m_table.push_back(pEntry);
}

void CoreTable::dump()
{
    std::vector<CoreTableEntry*>::iterator it;
    cout << "Num.entries = " << num_entries() << endl;
    //       +.......+.......+.......+.......+.......+.......+
    cout << "seg.    time    instr   line    staff   object" << endl;
    cout << "----------------------------------------------------------------" << endl;
    for (it=m_table.begin(); it != m_table.end(); ++it)
    {
        (*it)->dump();
    }
}




//-------------------------------------------------------------------------------------
// CoreTableBuilder implementation: algorithm to create a CoreTable
//-------------------------------------------------------------------------------------

CoreTableBuilder::CoreTableBuilder(Document* pDoc)
    : m_pDoc(pDoc)
{
    m_pTable = new CoreTable();
}

CoreTable* CoreTableBuilder::build_table(DocIterator* pItScore)
{
    m_pItScore = pItScore;

    int nTotalInstruments = find_number_of_instruments();
    for (int nInstr = 0; nInstr < nTotalInstruments; nInstr++)
    {
        find_voices_per_staff(nInstr);
        create_entries(nInstr);
    }
    sort_table();

    return m_pTable;
}

void CoreTableBuilder::find_voices_per_staff(int nInstr)
{
}

void CoreTableBuilder::create_entries(int nInstr)
{
    DocIterator cursor(m_pDoc);
    cursor.point_to(**m_pItScore);
    cursor.enter_element();

    cursor.start_of_instrument(nInstr);
    reset_counters();
    while(*cursor)
    {
        ImObj* pImo = (*cursor)->get_imobj();
        if ((*cursor)->is_type(k_goFwd) || (*cursor)->is_type(k_goBack) )
        {
            ImGoBackFwd* pGBF = static_cast<ImGoBackFwd*>(pImo);
            update_time_counter(pGBF);
        }
        else if ((*cursor)->is_type(k_key))
        {
            add_entries_for_key_signature(pImo, nInstr, *cursor);
        }
        else
        {
            add_entry_for_staffobj(pImo, nInstr, *cursor);
            update_segment(*cursor);
        }
        ++cursor;
    }
}

void CoreTableBuilder::add_entry_for_staffobj(ImObj* pImo, int nInstr,
                                                 LdpElement* pElm)
{
    ImStaffObj* pSO = static_cast<ImStaffObj*>(pImo);
    float rTime = determine_timepos(pSO);
    int nStaff = pSO->get_staff();
    int nVoice = 0;
    ImNoteRest* pNR = dynamic_cast<ImNoteRest*>(pSO);
    if (pNR)
        nVoice = pNR->get_voice();
    int nLine = get_line_for(nVoice, nStaff);
    m_pTable->AddEntry(m_nCurSegment, rTime, nInstr, nLine, nStaff, pElm);
}

void CoreTableBuilder::add_entries_for_key_signature(ImObj* pImo, int nInstr,
                                                 LdpElement* pElm)
{
    DocIterator cursor(m_pDoc);
    cursor.point_to(**m_pItScore);
    cursor.enter_element();

    cursor.find_instrument(nInstr);
    ImInstrument* pInstr = static_cast<ImInstrument*>((*cursor)->get_imobj());
    int numStaves = pInstr->get_num_staves();

    ImStaffObj* pSO = static_cast<ImStaffObj*>(pImo);
    float rTime = determine_timepos(pSO);
    for (int nStaff=0; nStaff < numStaves; nStaff++)
    {
        int nLine = get_line_for(0, nStaff);
        m_pTable->AddEntry(m_nCurSegment, rTime, nInstr, nLine, nStaff, pElm);
    }
}

void CoreTableBuilder::sort_table()
{
}

void CoreTableBuilder::reset_counters()
{
    m_nCurSegment = 0;
    m_rCurTime = 0.0f;
    m_nCurStaff = 0;
    m_rMaxTime = 0.0f;
}

int CoreTableBuilder::get_line_for(int nVoice, int nStaff)
{
    return m_lines.get_line_assigned_to(nVoice, nStaff);
}

float CoreTableBuilder::determine_timepos(ImStaffObj* pSO)
{
    float rTime = m_rCurTime;
    //ImStaffObj* pSO = static_cast<ImStaffObj*>(pImo);
    m_rCurTime += pSO->get_duration();
    m_rMaxTime = max(m_rMaxTime, m_rCurTime);
    return rTime;
}

void CoreTableBuilder::update_segment(LdpElement* pElm)
{
    if (pElm->get_type() == k_barline)
    {
        ++m_nCurSegment;
        m_rMaxTime = 0.0f;
        m_rCurTime = 0.0f;
    }
}

int CoreTableBuilder::find_number_of_instruments()
{
    DocIterator cursor(m_pDoc);
    cursor.point_to(**m_pItScore);
    ImScore* pScore = static_cast<ImScore*>( (*cursor)->get_imobj() );
    cursor.enter_element();

    int nInstr=0;
    cursor.start_of_instrument(nInstr++);
    while (!cursor.is_out_of_range())
        cursor.start_of_instrument(nInstr++);
    nInstr--;

    pScore->set_num_instruments(nInstr);
    return nInstr;
}

void CoreTableBuilder::update_time_counter(ImGoBackFwd* pGBF)
{
    if (pGBF->is_to_start())
        m_rCurTime = 0.0f;
    else if (pGBF->is_to_end())
        m_rCurTime = m_rMaxTime;
    else
    {
        m_rCurTime += pGBF->get_time_shift();
        m_rMaxTime = max(m_rMaxTime, m_rCurTime);
    }
}



//-------------------------------------------------------------------------------------
// StaffVoiceLineTable implementation
//-------------------------------------------------------------------------------------

StaffVoiceLineTable::StaffVoiceLineTable()
    : m_lastAssignedLine(-1)
{
    assign_line_to(0, 0);

    //first voice in each staff not yet known
    for (int i=0; i < 10; i++)
        m_firstVoiceForStaff.push_back(0);
}

int StaffVoiceLineTable::get_line_assigned_to(int nVoice, int nStaff)
{
    int key = form_key(nVoice, nStaff);
    std::map<int, int>::iterator it = m_lineForStaffVoice.find(key);
    if (it != m_lineForStaffVoice.end())
        return it->second;
    else
        return assign_line_to(nVoice, nStaff);
}

int StaffVoiceLineTable::assign_line_to(int nVoice, int nStaff)
{
    int key = form_key(nVoice, nStaff);
    if (nVoice != 0)
    {
        if (m_firstVoiceForStaff[nStaff] == 0)
        {
            //first voice found in this staff. Same line as voice 0 (nStaff)
            m_firstVoiceForStaff[nStaff] = nVoice;
            m_lineForStaffVoice[key] = nStaff;
            return nStaff;
        }
        else if (m_firstVoiceForStaff[nStaff] == nVoice)
            //first voice found in this staff. Same line as voice 0 (nStaff)
            return nStaff;
    }
    int line = ++m_lastAssignedLine;
    m_lineForStaffVoice[key] = line;
    return line;
}


}  //namespace lenmus
