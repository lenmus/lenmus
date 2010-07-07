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

#ifndef __LM_DOCUMENT_CURSOR_H__
#define __LM_DOCUMENT_CURSOR_H__

#include <stack>
#include "lenmus_document_iterator.h"
#include "lenmus_staffobjs_table.h"
#include "lenmus_time.h"

using namespace std;

namespace lenmus
{

//forward declarations
class Document;
class LdpElement;
class ScoreIterator;
class ImScore;


//-------------------------------------------------------------------------------------
// interfaces for traversing specific elements
//-------------------------------------------------------------------------------------

class ScoreCursorInterface
{
public:
    //move to next object. time doesn't matter
    virtual void move_next()=0;
    // move to next object but with dfferent time than current one
    virtual void move_next_new_time()=0;
    //move to prev object. time doesn't matter
    virtual void move_prev()=0;
    // move to prev object but with dfferent time than current one
    virtual void move_prev_new_time()=0;
    //to first obj in instr nInstr
    virtual void to_start_of_instrument(int nInstr)=0;
    //to first obj in specified segment and staff
    virtual void to_start_of_segment(int nSegment, int nStaff)=0;
    //while pointing clef, key or time, move next    
    virtual void skip_clef_key_time()=0;

    //access to pointed position/object
    virtual int instrument()=0;
    virtual int segment()=0;
    virtual int staff()=0;
    virtual float timepos()=0;
    virtual bool is_pointing_object()=0;

    //direct positioning
    virtual void point_to_barline(long nId, int nStaff)=0;



};


//-------------------------------------------------------------------------------------
// ElementCursor: base class for any cursor
//-------------------------------------------------------------------------------------

class ElementCursor
{
public:
    ElementCursor() {}
    virtual ~ElementCursor() {}

    //positioning
    inline void operator ++() { next(); }
    inline void operator --() { prev(); }
    virtual void point_to(LdpElement* pElm)=0;
    virtual void point_to(long nId)=0;

    virtual ElementCursor* enter_element() { return this; }
    virtual LdpElement* get_pointee()=0;
    virtual void next()=0;
    virtual void prev()=0;

};


//-------------------------------------------------------------------------------------
// ScoreCursor
//-------------------------------------------------------------------------------------

class ScoreCursor : public ElementCursor, public ScoreCursorInterface
{
protected:
    Document*           m_pDoc;
    ImScore*            m_pScore;
    ColStaffObjs*       m_pColStaffObjs;

    //state variables
    int                     m_nInstr;       //instrument (0..n-1)
    int				        m_nStaff;       //staff (0..n-1)
	float			        m_rTime;     //timepos
    int                     m_nSegment;     //segment number (0..n-1)
    ColStaffObjs::iterator  m_it;           //iterator pointing to ref.object

public:
    ScoreCursor(Document* pDoc, LdpElement* pScoreElm);
    virtual ~ScoreCursor();

    void next();
    void prev();
    void point_to(LdpElement* pElm);
    void point_to(long nId);
    LdpElement* get_pointee();

    //ScoreCursorInterface

    //ScoreCursorInterface: access to pointed position/object
    inline int instrument() { return m_nInstr; }
    inline int segment() { return m_nSegment; }
    inline int staff() { return m_nStaff; }
    inline float timepos() { return m_rTime; }
    inline bool is_pointing_object() { 
        return there_is_ref_object() 
               && ref_object_is_on_instrument(m_nInstr)
               && ref_object_is_on_segment(m_nSegment)
               && ref_object_is_on_time(m_rTime)
               && ref_object_is_on_staff(m_nStaff);
    }

    //ScoreCursorInterface: move cursor
    void move_next();
    void move_next_new_time();
    void move_prev();
    void move_prev_new_time();
    void to_start_of_instrument(int nInstr);
    void to_start_of_segment(int nSegment, int nStaff);
    void skip_clef_key_time();

    //ScoreCursorInterface: direct positioning
    void point_to_barline(long nId, int nStaff);


protected:
    void start();
    inline bool is_at_end_of_staff() { return m_it == m_pColStaffObjs->end(); }
    inline bool is_at_start_of_score() { return m_it == m_pColStaffObjs->begin(); }
    void update_state();
    bool more_staves_in_instrument();
    void to_start_of_next_staff();
    bool more_instruments();
    void to_start_of_next_instrument();
    void to_state(int nInstr, int nSegment, int nStaff, float rTime);

    //helper: dealing with ref.object
    inline float ref_object_time() { return (*m_it)->time(); }
    inline int ref_object_segment() { return (*m_it)->segment(); }
    inline int ref_object_staff() { return (*m_it)->staff(); }
    inline int ref_object_instrument() { return (*m_it)->num_instrument(); }
    inline bool there_is_ref_object() { return m_it != m_pColStaffObjs->end() && (*m_it != NULL); }
    inline bool there_is_not_ref_object() { return m_it != m_pColStaffObjs->end() || (*m_it == NULL); }
    inline bool ref_object_is_on_segment(int segment) {
        return ref_object_segment() == segment;
    }
    inline bool ref_object_is_on_staff(int staff) {
        return ref_object_staff() == staff
               || ref_object_is_barline();
    }
    inline bool ref_object_is_on_instrument(int instr) {
        return ref_object_instrument() == instr;
    }
    inline bool ref_object_is_on_time(float rTime) {
        return is_equal_time(rTime, ref_object_time());
    }
    float ref_object_duration();
    bool ref_object_is_barline();
    bool is_pointing_barline() {
        return is_pointing_object() && ref_object_is_barline();
    }

    //helper: for move_next
    void forward_to_next_time();
    float determine_next_target_time();
    int determine_next_target_segment();
    void forward_to_instr_segment_with_time_not_lower_than(float rTargetTime);
    void forward_to_current_staff();
    bool find_current_staff_at_current_ref_object_time();
    void forward_to_state(int nInstr, int nSegment, int nStaff, float rTime);
    bool try_next_at_same_time();

    //helper: for move_prev
    inline bool is_first_staff_of_current_instrument() { return (m_nStaff == 0); }
    void backward_to_prev_time();
    bool try_prev_at_same_time();
    bool is_at_start_of_staff();
    void to_end_of_prev_staff();
    inline bool is_first_instrument() { return m_nInstr == 0; }
    void to_end_of_prev_instrument();
    void to_end_of_staff();

};


//-------------------------------------------------------------------------------------
// DocCursor 
//-------------------------------------------------------------------------------------

class DocCursor : public ScoreCursorInterface
{
protected:
    Document*       m_pDoc;
    ElementCursor*  m_pCursor;
    DocIterator     m_it;
    LdpElement*     m_pFirst;

public:
    DocCursor(Document* pDoc);
    virtual ~DocCursor();

    DocCursor(const DocCursor& cursor);
    DocCursor& operator= (DocCursor const& cursor);

    inline LdpElement* operator *() { return get_pointee(); }
    LdpElement* get_pointee();

    //basic positioning
    void enter_element();
    inline void operator ++() { next(); }
    inline void operator --() { prev(); }
    void point_to(long nId);

	//info
	inline bool is_at_end_of_child() { return is_delegating() && get_pointee() == NULL; }
    inline LdpElement* get_top_level_element() { return *m_it; }

    //ScoreCursorInterface
    int instrument() { 
        ScoreCursor* pCursor = dynamic_cast<ScoreCursor*>(m_pCursor);
        return (pCursor ? pCursor->instrument() : 0);
    }
    int segment() { 
        ScoreCursor* pCursor = dynamic_cast<ScoreCursor*>(m_pCursor);
        return (pCursor ? pCursor->segment() : 0);
    }
    int staff() { 
        ScoreCursor* pCursor = dynamic_cast<ScoreCursor*>(m_pCursor);
        return (pCursor ? pCursor->staff() : 0);
    }
    float timepos() { 
        ScoreCursor* pCursor = dynamic_cast<ScoreCursor*>(m_pCursor);
        return (pCursor ? pCursor->timepos() : 0.0f);
    }
    bool is_pointing_object() { 
        ScoreCursor* pCursor = dynamic_cast<ScoreCursor*>(m_pCursor);
        return (pCursor ? pCursor->is_pointing_object() : true);
    }
    inline void move_next() { 
        ScoreCursor* pCursor = dynamic_cast<ScoreCursor*>(m_pCursor);
        if (pCursor)
            pCursor->move_next();
    }
    void move_next_new_time() {}
    void move_prev() { 
        ScoreCursor* pCursor = dynamic_cast<ScoreCursor*>(m_pCursor);
        if (pCursor)
            pCursor->move_prev();
    }
    void move_prev_new_time() {}
    void to_start_of_instrument(int nInstr) {}
    void to_start_of_segment(int nSegment, int nStaff) {}
    void skip_clef_key_time() {}

    //ScoreCursorInterface: direct positioning
    void point_to_barline(long nId, int nStaff) { 
        ScoreCursor* pCursor = dynamic_cast<ScoreCursor*>(m_pCursor);
        if (pCursor)
            pCursor->point_to_barline(nId, nStaff);
    }

protected:
    void next();
    void prev();
    void start_delegation();
    void stop_delegation();
    inline bool is_delegating() { return m_pCursor != NULL; }

};


}   //namespace lenmus

#endif      //__LM_DOCUMENT_CURSOR_H__
