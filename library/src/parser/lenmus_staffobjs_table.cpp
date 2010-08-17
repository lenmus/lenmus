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

#include <algorithm>
#include "lenmus_staffobjs_table.h"
#include "lenmus_document_iterator.h"
#include "lenmus_ldp_elements.h"
#include "lenmus_internal_model.h"
#include "lenmus_im_note.h"

using namespace std;

namespace lenmus
{

//-------------------------------------------------------------------------------------
// ColStaffObjsEntry implementation
//-------------------------------------------------------------------------------------

void ColStaffObjsEntry::dump()
{
    //cout << to_string() << ", time=" << m_time << ", staff=" << m_staff 
    //     << ", line=" << m_line << endl;
    //segment     time       instr     line     staff     object
    cout << m_segment << "\t" << m_time << "\t" << m_instr << "\t" 
         << m_line << "\t" << m_staff << "\t" << to_string_with_ids() << endl;

}

//-------------------------------------------------------------------------------------
// ColStaffObjs implementation
//-------------------------------------------------------------------------------------

//auxiliary, for sort: by segment, time, line and staff
bool is_lower_entry(ColStaffObjsEntry* a, ColStaffObjsEntry* b)
{
    return a->segment() < b->segment()
        || (a->segment() == b->segment() && a->time() < b->time())
        || (a->segment() == b->segment() && a->time() == b->time() 
            && a->line() < b->line())
        || (a->segment() == b->segment() && a->time() == b->time() 
            && a->line() == b->line() && a->staff() < b->staff()) ;
}

ColStaffObjs::ColStaffObjs()
{
} 

ColStaffObjs::~ColStaffObjs()
{
    std::vector<ColStaffObjsEntry*>::iterator it;
    for (it=m_table.begin(); it != m_table.end(); ++it)
        delete *it;
    m_table.clear();
}

void ColStaffObjs::AddEntry(int segment, float time, int instr, int voice, int staff,
                         LdpElement* pElm)
{
    ColStaffObjsEntry* pEntry = 
        new ColStaffObjsEntry(segment, time, instr, voice, staff, pElm);
    m_table.push_back(pEntry);
}

void ColStaffObjs::dump()
{
    std::vector<ColStaffObjsEntry*>::iterator it;
    cout << "Num.entries = " << num_entries() << endl;
    //       +.......+.......+.......+.......+.......+.......+
    cout << "seg.    time    instr   line    staff   object" << endl;
    cout << "----------------------------------------------------------------" << endl;
    for (it=m_table.begin(); it != m_table.end(); ++it)
    {
        (*it)->dump();
    }
}

void ColStaffObjs::sort()
{
    std::stable_sort(m_table.begin(), m_table.end(), is_lower_entry);
}



//-------------------------------------------------------------------------------------
// ColStaffObjsBuilder implementation: algorithm to create a ColStaffObjs
//-------------------------------------------------------------------------------------

ColStaffObjsBuilder::ColStaffObjsBuilder(LdpTree* pTree)
    : m_pTree(pTree)
{
}

ColStaffObjs* ColStaffObjsBuilder::build(LdpElement* pScore, bool fSort)
{
    //param fSort is to prevent sorting the table for unit tests

    m_pColStaffObjs = new ColStaffObjs();
    m_pScore = pScore;
    create_table( find_number_of_instruments() );
    sort_table(fSort);
    ImScore* pImScore = dynamic_cast<ImScore*>(m_pScore->get_imobj());
    pImScore->set_staffobjs_table(m_pColStaffObjs);
    return m_pColStaffObjs;
}

void ColStaffObjsBuilder::create_table(int nTotalInstruments)
{
    for (int nInstr = 0; nInstr < nTotalInstruments; nInstr++)
    {
        find_voices_per_staff(nInstr);
        create_entries(nInstr);
        prepare_for_next_instrument();
    }
}

void ColStaffObjsBuilder::find_voices_per_staff(int nInstr)
{
}

void ColStaffObjsBuilder::create_entries(int nInstr)
{
    DocIterator it(m_pTree);
    it.point_to(m_pScore);
    it.enter_element();

    it.start_of_instrument(nInstr);
    reset_counters();
    while(*it)
    {
        ImObj* pImo = (*it)->get_imobj();
        if ((*it)->is_type(k_goFwd) || (*it)->is_type(k_goBack) )
        {
            ImGoBackFwd* pGBF = static_cast<ImGoBackFwd*>(pImo);
            update_time_counter(pGBF);
        }
        else if ((*it)->is_type(k_key))
        {
            add_entries_for_key_signature(pImo, nInstr, *it);
        }
        else
        {
            ImAuxObj* pAO = dynamic_cast<ImAuxObj*>(pImo);
            if (pAO)
            {
                ImAnchor* pAnchor = anchor_object(pAO);
                (*it)->set_imobj(pAnchor);
                pImo = pAnchor;
            }
            add_entry_for_staffobj(pImo, nInstr, *it);
            update_segment(*it);
        }
        ++it;
    }
}

void ColStaffObjsBuilder::add_entry_for_staffobj(ImObj* pImo, int nInstr,
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
    m_pColStaffObjs->AddEntry(m_nCurSegment, rTime, nInstr, nLine, nStaff, pElm);
}

void ColStaffObjsBuilder::add_entries_for_key_signature(ImObj* pImo, int nInstr,
                                                 LdpElement* pElm)
{
    DocIterator it(m_pTree);
    it.point_to(m_pScore);
    it.enter_element();

    it.find_instrument(nInstr);
    ImInstrument* pInstr = static_cast<ImInstrument*>((*it)->get_imobj());
    int numStaves = pInstr->get_num_staves();

    ImStaffObj* pSO = static_cast<ImStaffObj*>(pImo);
    float rTime = determine_timepos(pSO);
    for (int nStaff=0; nStaff < numStaves; nStaff++)
    {
        int nLine = get_line_for(0, nStaff);
        m_pColStaffObjs->AddEntry(m_nCurSegment, rTime, nInstr, nLine, nStaff, pElm);
    }
}

void ColStaffObjsBuilder::prepare_for_next_instrument()
{
    m_lines.new_instrument();
}

void ColStaffObjsBuilder::sort_table(bool fSort)
{
    if (fSort)
        m_pColStaffObjs->sort();
}

void ColStaffObjsBuilder::reset_counters()
{
    m_nCurSegment = 0;
    m_rCurTime = 0.0f;
    m_nCurStaff = 0;
    m_rMaxTime = 0.0f;
}

int ColStaffObjsBuilder::get_line_for(int nVoice, int nStaff)
{
    return m_lines.get_line_assigned_to(nVoice, nStaff);
}

float ColStaffObjsBuilder::determine_timepos(ImStaffObj* pSO)
{
    float rTime = m_rCurTime;
    m_rCurTime += pSO->get_duration();
    m_rMaxTime = max(m_rMaxTime, m_rCurTime);
    return rTime;
}

void ColStaffObjsBuilder::update_segment(LdpElement* pElm)
{
    if (pElm->is_type(k_barline))
    {
        ++m_nCurSegment;
        m_rMaxTime = 0.0f;
        m_rCurTime = 0.0f;
    }
}

int ColStaffObjsBuilder::find_number_of_instruments()
{
    DocIterator it(m_pTree);
    it.point_to(m_pScore);
    ImScore* pScore = static_cast<ImScore*>( (*it)->get_imobj() );
    it.enter_element();

    int nInstr=0;
    it.find_instrument(nInstr);
    while (!it.is_out_of_range())
    {
        ImInstrument* pInstr = dynamic_cast<ImInstrument*>( (*it)->get_imobj() );
        pScore->add_instrument(pInstr);
        it.find_instrument(++nInstr);
    }

    return nInstr;
}

void ColStaffObjsBuilder::update_time_counter(ImGoBackFwd* pGBF)
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

void ColStaffObjsBuilder::update(LdpElement* pScore)
{
    ImScore* pImScore = dynamic_cast<ImScore*>( pScore->get_imobj() );
    ColStaffObjs* pOldColStaffObjs = pImScore->get_staffobjs_table();
    delete pOldColStaffObjs;

    //For now, rebuild the table
    this->build(pScore);
}

ImAnchor* ColStaffObjsBuilder::anchor_object(ImAuxObj* pAux)
{
    ImAnchor* pAnchor = new ImAnchor();
    pAnchor->attach(pAux);
    return pAnchor;
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
            int line = get_line_assigned_to(0, nStaff);
            m_lineForStaffVoice[key] = line;
            return line;
        }
        else if (m_firstVoiceForStaff[nStaff] == nVoice)
            //first voice found in this staff. Same line as voice 0 (nStaff)
            return get_line_assigned_to(0, nStaff);
    }
    int line = ++m_lastAssignedLine;
    m_lineForStaffVoice[key] = line;
    return line;
}

void StaffVoiceLineTable::new_instrument()
{
    m_lineForStaffVoice.clear();

    assign_line_to(0, 0);

    //first voice in each staff not yet known
    for (int i=0; i < 10; i++)
        m_firstVoiceForStaff[i] = 0;
}


}  //namespace lenmus
