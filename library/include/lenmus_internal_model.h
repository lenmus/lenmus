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

#ifndef __LML_INTERNAL_MODEL_H__        //to avoid nested includes
#define __LML_INTERNAL_MODEL_H__

#include <string>
#include <list>
#include <vector>
#include "lenmus_visitor.h"
#include "lenmus_tree.h"
#include "lenmus_basic.h"

using namespace std;

namespace lenmus
{





//forward declarations
class ColStaffObjs;
class ImoInstrument;
class ImoNote;
class ImoNoteRest;
class ImoMusicData;
class ImoSystemMargins;
class ImoSystemLayout;
class LdpElement;
class ImoTextString;
class ImoColorInfo;
class ImoObjVisitor;
class ImoAuxObj;
class ImoOption;

class DtoObj;
class DtoDocObj;
class DtoComponentObj;
class DtoStaffObj;
class DtoBarline;
class DtoClef;
class DtoGoBackFwd;
class DtoKeySignature;
class DtoSpacer;
class DtoTimeSignature;


// a struct to contain note/rest figure and dots
struct NoteTypeAndDots
{
    NoteTypeAndDots(int nt, int d) : noteType(nt), dots(d) {}

    int noteType;   //ImoNoteRest enum
    int dots;       //0..n
};




// Model classes
//================================


//===================================================
// Abstarct hierachy
//===================================================

// the root. Any object must derive from it
//----------------------------------------------------------------------------------
//class ImoObj : public Visitable
//{
//protected:
//    long m_id;
//    int m_objtype;
//
//    ImoObj(int objtype) : m_id(-1L), m_objtype(objtype) {}
//    ImoObj(long id, int objtype) : m_id(id), m_objtype(objtype) {}
//
//public:
//    virtual ~ImoObj() {}
//
//    enum {
//        // ImoObj (A)
//        k_obj=0,
//            // ImoSimpleObj (A)
//            k_simpleobj, k_beam_info, k_bezier, k_color_info, k_instr_group, k_midi_info, k_option,
//            k_system_layout, k_system_margins, k_tie_info, k_tuplet_info,
//
//            // ImoDocObj (A)
//            k_docobj,
//
//                // ImoContainerObj
//                k_containerobj, k_document, k_content, k_music_data, k_instrument, k_score,
//
//                // ImoComponentObj (A)
//                k_componentobj,
//
//                    // ImoStaffObj (A)
//                    k_staffobj, k_barline, k_clef, k_key_signature, k_time_signature, k_note, k_rest,
//                    k_go_back_fwd, k_metronome_mark, k_control, k_spacer, k_figured_bass,
//
//                    // ImoAuxObj (A)
//                    k_auxobj, k_text_string, k_fermata,
//                        //ImoRelObj (A)
//                        k_relobj,
//                            // BinaryRelObj (A)
//                            k_binary_relobj, k_tie,
//                            // MultiRelObj (A)
//                            k_multi_relobj, k_beam, k_tuplet,
//    };
//
//
//    //getters
//    inline long get_id() { return m_id; }
//
//    //setters
//    inline void set_id(long id) { m_id = id; }
//
//    //overrides for Visitable class
//	void accept_in(BaseVisitor& v);
//	void accept_out(BaseVisitor& v);
//
//    //object classification
//    inline int get_obj_type() { return m_objtype; }
//
//    //simple objs
//    inline bool is_simpleobj() { return m_objtype >= k_simpleobj && m_objtype < k_docobj; }
//    inline bool is_beam_info() { return m_objtype == k_beam_info; }
//    inline bool is_bezier() { return m_objtype == k_bezier; }
//    inline bool is_color_info() { return m_objtype == k_color_info; }
//    inline bool is_instr_group() { return m_objtype == k_instr_group; }
//    inline bool is_midi_info() { return m_objtype == k_midi_info; }
//    inline bool is_option() { return m_objtype == k_option; }
//    inline bool is_system_layout() { return m_objtype == k_system_layout; }
//    inline bool is_system_margins() { return m_objtype == k_system_margins; }
//    inline bool is_tie_info() { return m_objtype == k_tie_info; }
//    inline bool is_tuplet_info() { return m_objtype == k_tuplet_info; }
//
//    // doc objs
//    inline bool is_docobj() { return m_objtype >= k_docobj; }
//
//    // container objs
//    inline bool is_containerobj() { return m_objtype >= k_containerobj && m_objtype < k_componentobj; }
//    inline bool is_document() { return m_objtype == k_document; }
//    inline bool is_content() { return m_objtype == k_content; }
//    inline bool is_music_data() { return m_objtype == k_music_data; }
//    inline bool is_instrument() { return m_objtype == k_instrument; }
//	inline bool is_score() { return m_objtype == k_score; }
//
//    // component objs
//	inline bool is_componentobj() { return m_objtype >= k_componentobj; }
//
//	// staff objs
//	inline bool is_staffobj() { return m_objtype >= k_staffobj && m_objtype < k_auxobj; }
//
//    //StaffObjs
//    inline bool is_barline() { return m_objtype == k_barline; }
//    inline bool is_clef() { return m_objtype == k_clef; }
//    inline bool is_key_signature() { return m_objtype == k_key_signature; }
//    inline bool is_time_signature() { return m_objtype == k_time_signature; }
//    inline bool is_note_rest() { return m_objtype == k_note || m_objtype == k_rest; }
//    inline bool is_note() { return m_objtype == k_note; }
//    inline bool is_rest() { return m_objtype == k_rest; }
//    inline bool is_go_back_fwd() { return m_objtype == k_go_back_fwd; }
//    inline bool is_metronome_mark() { return m_objtype == k_metronome_mark; }
//    inline bool is_control() { return m_objtype == k_control; }
//    inline bool is_spacer() { return m_objtype == k_spacer; }
//    inline bool is_figured_bass() { return m_objtype == k_figured_bass; }
//
//    // aux objs
//	inline bool is_auxobj() { return m_objtype >= k_auxobj; }
//    inline bool is_text_string() { return m_objtype == k_text_string; }
//    inline bool is_fermata() { return m_objtype == k_fermata; }
//
//    // rel objs
//    inline bool is_relobj() { return m_objtype >= k_relobj && m_objtype < k_binary_relobj; }
//
//    // binary rel objs
//    inline bool is_binary_relobj() { return m_objtype >= k_binary_relobj
//                                          && m_objtype < k_multi_relobj; }
//    inline bool is_tie() { return m_objtype == k_tie; }
//
//    // multi rel objs
//    inline bool is_multi_relobj() { return m_objtype >= k_multi_relobj; }
//    inline bool is_beam() { return m_objtype == k_beam; }
//    inline bool is_tuplet() { return m_objtype == k_tuplet; }
//
//};


//-------------------------------------------------------------------------------
// A generic IMO representation.
//
// An ImoObj is the content of a node in the internal model tree. I combines
// links to other nodes as well as the actual object data.
//-------------------------------------------------------------------------------

class ImoObj : public Visitable, public NodeInTree<ImoObj>
{
protected:
    long m_id;
    int m_objtype;

    ImoObj(int objtype) : m_id(-1L), m_objtype(objtype) {}
    ImoObj(long id, int objtype) : m_id(id), m_objtype(objtype) {}
    ImoObj(int objtype, DtoObj& dto);

public:
    virtual ~ImoObj() {}

    enum {
        // ImoObj (A)
        k_obj=0,
            // ImoSimpleObj (A)
            k_simpleobj, k_beam_info, k_bezier, k_color_info, k_content, k_instr_group, k_midi_info,
            k_music_data, k_option, k_system_layout, k_system_margins, k_tie_info, k_tuplet_info,

            // ImoDocObj (A)
            k_docobj,

                // ImoContainerObj
                k_containerobj, k_document, k_instrument, k_score,

                // ImoComponentObj (A)
                k_componentobj,

                    // ImoStaffObj (A)
                    k_staffobj, k_barline, k_clef, k_key_signature, k_time_signature, k_note, k_rest,
                    k_go_back_fwd, k_metronome_mark, k_control, k_spacer, k_figured_bass,

                    // ImoAuxObj (A)
                    k_auxobj, k_text_string, k_fermata,
                        //ImoRelObj (A)
                        k_relobj,
                            // BinaryRelObj (A)
                            k_binary_relobj, k_tie,
                            // MultiRelObj (A)
                            k_multi_relobj, k_beam, k_tuplet,
    };


    //getters
    inline long get_id() { return m_id; }

    //setters
    inline void set_id(long id) { m_id = id; }

    //overrides to Visitable class members
	virtual void accept_in(BaseVisitor& v);
	virtual void accept_out(BaseVisitor& v);


    //object classification
    inline int get_obj_type() { return m_objtype; }
	inline bool has_children() { return !is_terminal(); }

    //simple objs
    inline bool is_simpleobj() { return m_objtype >= k_simpleobj && m_objtype < k_docobj; }
    inline bool is_beam_info() { return m_objtype == k_beam_info; }
    inline bool is_bezier() { return m_objtype == k_bezier; }
    inline bool is_color_info() { return m_objtype == k_color_info; }
    inline bool is_content() { return m_objtype == k_content; }
    inline bool is_instr_group() { return m_objtype == k_instr_group; }
    inline bool is_music_data() { return m_objtype == k_music_data; }
    inline bool is_midi_info() { return m_objtype == k_midi_info; }
    inline bool is_option() { return m_objtype == k_option; }
    inline bool is_system_layout() { return m_objtype == k_system_layout; }
    inline bool is_system_margins() { return m_objtype == k_system_margins; }
    inline bool is_tie_info() { return m_objtype == k_tie_info; }
    inline bool is_tuplet_info() { return m_objtype == k_tuplet_info; }

    // doc objs
    inline bool is_docobj() { return m_objtype >= k_docobj; }

    // container objs
    inline bool is_containerobj() { return m_objtype >= k_containerobj && m_objtype < k_componentobj; }
    inline bool is_document() { return m_objtype == k_document; }
    inline bool is_instrument() { return m_objtype == k_instrument; }
	inline bool is_score() { return m_objtype == k_score; }

    // component objs
	inline bool is_componentobj() { return m_objtype >= k_componentobj; }

	// staff objs
	inline bool is_staffobj() { return m_objtype >= k_staffobj && m_objtype < k_auxobj; }

    //StaffObjs
    inline bool is_barline() { return m_objtype == k_barline; }
    inline bool is_clef() { return m_objtype == k_clef; }
    inline bool is_key_signature() { return m_objtype == k_key_signature; }
    inline bool is_time_signature() { return m_objtype == k_time_signature; }
    inline bool is_note_rest() { return m_objtype == k_note || m_objtype == k_rest; }
    inline bool is_note() { return m_objtype == k_note; }
    inline bool is_rest() { return m_objtype == k_rest; }
    inline bool is_go_back_fwd() { return m_objtype == k_go_back_fwd; }
    inline bool is_metronome_mark() { return m_objtype == k_metronome_mark; }
    inline bool is_control() { return m_objtype == k_control; }
    inline bool is_spacer() { return m_objtype == k_spacer; }
    inline bool is_figured_bass() { return m_objtype == k_figured_bass; }

    // aux objs
	inline bool is_auxobj() { return m_objtype >= k_auxobj; }
    inline bool is_text_string() { return m_objtype == k_text_string; }
    inline bool is_fermata() { return m_objtype == k_fermata; }

    // rel objs
    inline bool is_relobj() { return m_objtype >= k_relobj && m_objtype < k_binary_relobj; }

    // binary rel objs
    inline bool is_binary_relobj() { return m_objtype >= k_binary_relobj
                                          && m_objtype < k_multi_relobj; }
    inline bool is_tie() { return m_objtype == k_tie; }

    // multi rel objs
    inline bool is_multi_relobj() { return m_objtype >= k_multi_relobj; }
    inline bool is_beam() { return m_objtype == k_beam; }
    inline bool is_tuplet() { return m_objtype == k_tuplet; }

};

// just auxiliary objects or data transfer objects
//----------------------------------------------------------------------------------
class ImoSimpleObj : public ImoObj
{
protected:
    ImoSimpleObj(long id, int objtype) : ImoObj(id, objtype) {}
    ImoSimpleObj(int objtype) : ImoObj(objtype) {}

public:
    virtual ~ImoSimpleObj() {}
};

// Any object for a music score, including the score itself
//----------------------------------------------------------------------------------
class ImoDocObj : public ImoObj
{
protected:
    Tenths m_txUserLocation;
    Tenths m_tyUserLocation;
    std::list<ImoAuxObj*> m_attachments;

    ImoDocObj(int objtype);
    ImoDocObj(long id, int objtype);
    ImoDocObj(int objtype, DtoDocObj& dto);

public:
    virtual ~ImoDocObj();

    //getters
    inline Tenths get_user_location_x() { return m_txUserLocation; }
    inline Tenths get_user_location_y() { return m_tyUserLocation; }

    //setters
    inline void set_user_location_x(Tenths tx) { m_txUserLocation = tx; }
    inline void set_user_location_y(Tenths ty) { m_tyUserLocation = ty; }

//    //attachments
//    inline void attach(ImoAuxObj* pAO) { m_attachments.push_back(pAO); }
//    inline bool has_attachments() { return m_attachments.size() > 0; }
//    inline int get_num_attachments() { return static_cast<int>( m_attachments.size() ); }
//    ImoAuxObj* get_attachment(int i);   //0..n-1
//    inline std::list<ImoAuxObj*>& get_attachments() { return m_attachments; }

    //attachments (first child)
    inline bool has_attachments() { return get_first_child()->has_children(); }
    inline int get_num_attachments() { return get_first_child()->get_num_children(); }
    ImoAuxObj* get_attachment(int i);
    void attach(ImoAuxObj* pAO);

};

// ContainerObj: A collection of containers and contained objs.
//----------------------------------------------------------------------------------
class ImoContainerObj : public ImoDocObj
{
protected:
    ImoContainerObj(int objtype) : ImoDocObj(objtype) {}

public:
    virtual ~ImoContainerObj() {}

    //getters

    //setters
};

// ComponentObj: Any atomic displayable object. Must be attached to
//               containers (ContainerObj) or to other ComponentObj
//----------------------------------------------------------------------------------
class ImoComponentObj : public ImoDocObj
{
protected:
    bool m_fVisible;
    rgba16 m_color;

    ImoComponentObj(long id, int objtype) : ImoDocObj(id, objtype), m_fVisible(true) {}
    ImoComponentObj(int objtype) : ImoDocObj(objtype), m_fVisible(true) {}
    ImoComponentObj(int objtype, DtoComponentObj& dto);

public:
    virtual ~ImoComponentObj() {}

    //getters
    inline bool is_visible() { return m_fVisible; }
    inline rgba16& get_color() { return m_color; }

    //setters
    inline void set_visible(bool visible) { m_fVisible = visible; }
    void set_color(ImoColorInfo* pColor);
    void set_color(rgba16 color);

};

// StaffObj: A ComponentObj that is attached to an Staff. Consume time
//----------------------------------------------------------------------------------
class ImoStaffObj : public ImoComponentObj
{
protected:
    int m_staff;
    //- timePos

    ImoStaffObj(int objtype) : ImoComponentObj(objtype), m_staff(0) {}
    ImoStaffObj(long id, int objtype) : ImoComponentObj(id, objtype), m_staff(0) {}
    ImoStaffObj(int objtype, DtoStaffObj& dto);

public:
    virtual ~ImoStaffObj() {}

    //getters
    virtual float get_duration() { return 0.0f; }
    inline int get_staff() { return m_staff; }

    //setters
    virtual void set_staff(int staff) { m_staff = staff; }


};

// AuxObj: a ComponentObj that must be attached to other objects but not
//         directly to an staff. Do not consume time
//----------------------------------------------------------------------------------
class ImoAuxObj : public ImoComponentObj
{
protected:
    ImoAuxObj(int objtype) : ImoComponentObj(objtype) {}

public:
    virtual ~ImoAuxObj() {}

protected:
    ImoAuxObj(ImoDocObj* pOwner, long id, int objtype) : ImoComponentObj(id, objtype) {}

};

//An abstract AuxObj relating at least two StaffObjs
//----------------------------------------------------------------------------------
class ImoRelObj : public ImoAuxObj
{
protected:
    ImoRelObj(int objtype) : ImoAuxObj(objtype) {}
    //ImoRelObj(ImoDocObj* pOwner, long id, int objtype)
    //    : ImoAuxObj(pOwner, id, objtype) {}

public:
	virtual ~ImoRelObj() {}

    ////building/destroying the relationship
    //virtual void include(ImoStaffObj* pSO)=0;
    //virtual void remove(ImoStaffObj* pSO)=0;
	//virtual void on_relationship_modified()=0;

    //information
    virtual ImoStaffObj* get_start_object()=0;
    virtual ImoStaffObj* get_end_object()=0;

};


//An abstract AuxObj relating two and only two StaffObjs
//----------------------------------------------------------------------------------
class ImoBinaryRelObj : public ImoRelObj
{
protected:
    ImoStaffObj* m_pStartSO;     //StaffObjs related by this ImoRelObj
    ImoStaffObj* m_pEndSO;

    ImoBinaryRelObj(int objtype) : ImoRelObj(objtype) {}
    //ImoBinaryRelObj(ImoDocObj* pOwner, long id, int objtype,
    //               ImoStaffObj* pStartSO, ImoStaffObj* pEndSO);

public:
    virtual ~ImoBinaryRelObj();

    //implementation of ImoRelObj pure virtual methods
    //virtual void include(ImoStaffObj* pSO) {};
    //virtual void remove(ImoStaffObj* pSO);
    //virtual void on_relationship_modified() {};
    virtual ImoStaffObj* get_start_object() { return m_pStartSO; }
    virtual ImoStaffObj* get_end_object() { return m_pEndSO; }

};

//An abstract AuxObj relating two or more StaffObjs
//----------------------------------------------------------------------------------
class ImoMultiRelObj : public ImoRelObj
{
protected:
    std::list<ImoStaffObj*> m_relatedObjects;

    ImoMultiRelObj(int objtype) : ImoRelObj(objtype) {}
    //ImoMultiRelObj(ImoDocObj* pOwner, long id, int objtype);

public:
    virtual ~ImoMultiRelObj();

    //implementation of ImoRelObj pure virtual methods
    //virtual void include(ImoStaffObj* pSO, int index = -1);
    //virtual void remove(ImoStaffObj* pSO);
    //virtual void on_relationship_modified() {};
    ImoStaffObj* get_start_object() { return m_relatedObjects.front(); }
    ImoStaffObj* get_end_object() { return m_relatedObjects.back(); }


    //specific methods
    void push_back(ImoStaffObj* pSO);
    inline int get_num_objects() { return static_cast<int>( m_relatedObjects.size() ); }
    //int get_object_index(ImoStaffObj* pSO);
    std::list<ImoStaffObj*>& get_related_objects() { return m_relatedObjects; }

};



//===================================================
// Real objects
//===================================================

//----------------------------------------------------------------------------------
class ImoBarline : public ImoStaffObj
{
protected:
    long m_type;

public:
    ImoBarline(int type) : ImoStaffObj(ImoObj::k_barline) , m_type(type) {}
    ImoBarline(DtoBarline& dto);
    ~ImoBarline() {}

	enum { k_simple=0, k_double, k_start, k_end, k_end_repetition, k_start_repetition,
           k_double_repetition, };

    //getters and setters
    inline int get_type() { return m_type; }
    //inline void set_type(int type) { m_type = type; }

    //overrides: barlines always in staff 0
    void set_staff(int staff) { m_staff = 0; }

};

//----------------------------------------------------------------------------------
class ImoBeam : public ImoMultiRelObj
{
public:
    ImoBeam() : ImoMultiRelObj(ImoObj::k_beam) {}
    ~ImoBeam() {}

    //type of beam
    enum { k_none = 0, k_begin, k_continue, k_end, k_forward, k_backward, };

};

// raw info about a note beam
//----------------------------------------------------------------------------------
class ImoBeamInfo : public ImoSimpleObj
{
protected:
    int m_beamType[6];
    int m_beamNum;
    bool m_repeat[6];
    LdpElement* m_pBeamElm;
    ImoNoteRest* m_pNR;

public:
    ImoBeamInfo();
    ImoBeamInfo(LdpElement* pBeamElm);
    ~ImoBeamInfo() {}

    //getters
    inline int get_beam_number() { return m_beamNum; }
    inline LdpElement* get_beam_element() { return m_pBeamElm; }
    inline ImoNoteRest* get_note_rest() { return m_pNR; }
    int get_line_number();
    int get_beam_type(int level);
    bool get_repeat(int level);

    //setters
    inline void set_beam_number(int num) { m_beamNum = num; }
    inline void set_note_rest(ImoNoteRest* pNR) { m_pNR = pNR; }
    inline void set_beam_element(LdpElement* pElm) { m_pBeamElm = pElm; }
    void set_beam_type(int level, int type);
    void set_repeat(int level, bool value);

    //properties
    bool is_end_of_beam();

};

//----------------------------------------------------------------------------------
class ImoBezier : public ImoSimpleObj
{
protected:
    TPoint m_tPoints[4];   //start, end, ctrol1, ctrol2

public:
    ImoBezier() : ImoSimpleObj(ImoObj::k_bezier) {}
    ~ImoBezier() {}

	enum { k_start=0, k_end, k_ctrol1, k_ctrol2, };     // point number

    //points
    inline void set_point(int i, TPoint& value) { m_tPoints[i] = value; }
    inline TPoint& get_point(int i) { return m_tPoints[i]; }

};

//----------------------------------------------------------------------------------


class ImoClef : public ImoStaffObj
{
protected:
    int m_clefType;

public:
    ImoClef(DtoClef& dto);
    ImoClef(int clefType);
    ~ImoClef() {}

    enum {
        k_undefined=-1,
        k_G3 = 0,
        k_F4,
        k_F3,
        k_C1,
        k_C2,
        k_C3,
        k_C4,
        k_Percussion,
        // other clefs not available for exercises
        k_C5,
        k_F5,
        k_G1,
        k_8_G3,        //8 above
        k_G3_8,        //8 below
        k_8_F4,        //8 above
        k_F4_8,        //8 below
        k_15_G3,       //15 above
        k_G3_15,       //15 below
        k_15_F4,       //15 above
        k_F4_15,       //15 below
    };

    //getters and setters
    inline int get_clef_type() { return m_clefType; }
    inline void set_clef_type(int type) { m_clefType = type; }

};

//----------------------------------------------------------------------------------
class ImoContent : public ImoSimpleObj
{
public:
    ImoContent() : ImoSimpleObj(ImoObj::k_content) {}
    ~ImoContent() {}

    //contents
    ImoDocObj* get_content_item(int iItem) {   //iItem = 0..n-1
        return dynamic_cast<ImoDocObj*>( get_child(iItem) );
    }
    inline int get_num_content_items() { return get_num_children(); }

};

//----------------------------------------------------------------------------------
class ImoControl : public ImoStaffObj
{
protected:

public:
    ImoControl() : ImoStaffObj(ImoObj::k_control) {}
    ~ImoControl() {}

    //getters & setters
};

//----------------------------------------------------------------------------------
class ImoDocument : public ImoContainerObj
{
protected:
    string m_version;

public:
    ImoDocument(const std::string& version)
        : ImoContainerObj(ImoObj::k_document)
        , m_version(version) {}
    ImoDocument() : ImoContainerObj(ImoObj::k_document), m_version("") {}
    ~ImoDocument() {}

    //getters and setters
    inline std::string& get_version() { return m_version; }
//    inline void set_version(const std::string& version) { m_version = version; }

    //content (2nd child)
    ImoDocObj* get_content_item(int iItem);
    int get_num_content_items();
    ImoContent* get_content();

};

//----------------------------------------------------------------------------------
class ImoFermata : public ImoAuxObj
{
protected:
    int m_placement;
    int m_symbol;

public:
    ImoFermata() : ImoAuxObj(ImoObj::k_fermata), m_placement(k_above), m_symbol(k_normal) {}
    ~ImoFermata() {}

	enum { k_above=0, k_below, };               //placement
    enum { k_normal, k_angled, k_square, };     //symbol

    //getters
    inline int get_placement() { return m_placement; }
    inline int get_symbol() { return m_symbol; }

    //setters
    inline void set_placement(int placement) { m_placement = placement; }
    inline void set_symbol(int symbol) { m_symbol = symbol; }

};

//----------------------------------------------------------------------------------
class ImoFiguredBass : public ImoStaffObj
{
protected:
    long m_type;

public:
    ImoFiguredBass() : ImoStaffObj(ImoObj::k_figured_bass) {}
    ~ImoFiguredBass() {}

	enum { kSimple=0, kDouble, kStart, kEnd, kEndRepetition, kStartRepetition,
           k_double_repetition, };

    //getters and setters
    inline int get_type() { return m_type; }
    inline void set_type(int type) { m_type = type; }

};

//----------------------------------------------------------------------------------
class ImoGoBackFwd : public ImoStaffObj
{
protected:
    bool    m_fFwd;
    float   m_rTimeShift;

    const float SHIFT_START_END;     //any too big value

public:
//    ImoGoBackFwd(bool fFwd) : ImoStaffObj(ImoObj::k_go_back_fwd), m_fFwd(fFwd), m_rTimeShift(0.0f),
//                             SHIFT_START_END(100000000.0f) {}
    ImoGoBackFwd(DtoGoBackFwd& dto);
    ~ImoGoBackFwd() {}

    //getters and setters
    inline bool is_forward() { return m_fFwd; }
    inline bool is_to_start() { return !m_fFwd && (m_rTimeShift == -SHIFT_START_END); }
    inline bool is_to_end() { return m_fFwd && (m_rTimeShift == SHIFT_START_END); }
    inline float get_time_shift() { return m_rTimeShift; }
    inline void set_to_start() { set_time_shift(SHIFT_START_END); }
    inline void set_to_end() { set_time_shift(SHIFT_START_END); }
    inline void set_time_shift(float rTime) { m_rTimeShift = (m_fFwd ? rTime : -rTime); }
};

//----------------------------------------------------------------------------------
class ImoTextString : public ImoAuxObj
{
protected:
    string  m_text;

public:
    ImoTextString() : ImoAuxObj(ImoObj::k_text_string) {}
    ~ImoTextString() {}

    //getters and setters
    inline string& get_text() { return m_text; }
    inline void set_text(const string& text) { m_text = text; }

};

//----------------------------------------------------------------------------------
class ImoMidiInfo : public ImoSimpleObj
{
protected:
    int     m_instr;
    int     m_channel;

public:
    ImoMidiInfo() : ImoSimpleObj(ImoObj::k_midi_info), m_instr(0), m_channel(0) {}
    ~ImoMidiInfo() {}

    //getters
    inline int get_instrument() { return m_instr; }
    inline int get_channel() { return m_channel; }

    //setters
    inline void set_instrument(int value) { m_instr = value; }
    inline void set_channel(int value) { m_channel = value; }

};

//----------------------------------------------------------------------------------
class ImoInstrument : public ImoContainerObj
{
protected:
    int     m_nStaves;
    ImoMusicData* m_pMusicData;
    ImoTextString m_name;
    ImoTextString m_abbrev;
    ImoMidiInfo m_midi;

public:
    ImoInstrument();
    ~ImoInstrument();

    //getters
    inline int get_num_staves() { return m_nStaves; }
    inline const std::string& get_name() { return m_name.get_text(); }
    inline const std::string& get_abbrev() { return m_abbrev.get_text(); }
    inline int get_instrument() { return m_midi.get_instrument(); }
    inline int get_channel() { return m_midi.get_channel(); }
    inline ImoMusicData* get_musicdata() { return m_pMusicData; }

    //setters
    inline void set_num_staves(int staves) { m_nStaves = staves; }
    inline void set_musicdata(ImoMusicData* pMD) { m_pMusicData = pMD; }
    void set_name(ImoTextString* pText);
    void set_abbrev(ImoTextString* pText);
    void set_midi_info(ImoMidiInfo* pInfo);

};

//----------------------------------------------------------------------------------
class ImoInstrGroup : public ImoSimpleObj
{
protected:
    bool m_fJoinBarlines;
    int m_symbol;           // enum k_none, k_brace, k_bracket, ...
    ImoTextString m_name;
    ImoTextString m_abbrev;
    std::list<ImoInstrument*> m_instruments;

public:
    ImoInstrGroup() : ImoSimpleObj(ImoObj::k_instr_group), m_fJoinBarlines(true), m_symbol(k_brace)
                   , m_name(), m_abbrev() {}
    ~ImoInstrGroup();

    enum { k_none=0, k_brace, k_bracket, };

    //getters
    inline bool join_barlines() { return m_fJoinBarlines; }
    inline int get_symbol() { return m_symbol; }
    inline const std::string& get_name() { return m_name.get_text(); }
    inline const std::string& get_abbrev() { return m_abbrev.get_text(); }

    //setters
    void set_name(ImoTextString* pText);
    void set_abbrev(ImoTextString* pText);
    inline void set_symbol(int symbol) { m_symbol = symbol; }
    inline void set_join_barlines(bool value) { m_fJoinBarlines = value; }

    //instruments
    inline void add_instrument(ImoInstrument* pInstr) { m_instruments.push_back(pInstr); }
    ImoInstrument* get_instrument(int iInstr);   //0..n-1
    inline int get_num_instruments() { return static_cast<int>(m_instruments.size()); }

protected:
    void delete_instruments();

};

//----------------------------------------------------------------------------------
class ImoKeySignature : public ImoStaffObj
{
protected:
    int m_keyType;

public:
//    ImoKeySignature() : ImoStaffObj(ImoObj::k_key_signature) , m_keyType(ImoKeySignature::Undefined) {}
    ImoKeySignature(DtoKeySignature& dto);
    ~ImoKeySignature() {}

	enum { Undefined=-1, C=0, G, D, A, E, B, Fs, Cs, Cf, Gf, Df, Af, Ef, Bf, F,
           a, e, b, fs, cs, gs, ds, as, af, ef, bf, f, c, g, d };

    //getters and setters
    inline int get_key_type() { return m_keyType; }
//    inline void set_key_type(int type) { m_keyType = type; }

    //overrides: key signatures always in staff 0
    void set_staff(int staff) { m_staff = 0; }

};

//----------------------------------------------------------------------------------
class ImoMetronomeMark : public ImoStaffObj
{
protected:
    int     m_markType;
    int     m_ticksPerMinute;
    int     m_leftNoteType;
    int     m_leftDots;
    int     m_rightNoteType;
    int     m_rightDots;
    bool    m_fParenthesis;

public:
    ImoMetronomeMark() : ImoStaffObj(ImoObj::k_metronome_mark), m_markType(k_value),
        m_ticksPerMinute(60),
        m_leftNoteType(0), m_leftDots(0),
        m_rightNoteType(0), m_rightDots(0),
        m_fParenthesis(false) {}
    ~ImoMetronomeMark() {}

    enum { k_note_value=0, k_note_note, k_value, };

    //getters
    inline int get_left_note_type() { return m_leftNoteType; }
    inline int get_right_note_type() { return m_rightNoteType; }
    inline int get_left_dots() { return m_leftDots; }
    inline int get_right_dots() { return m_rightDots; }
    inline int get_ticks_per_minute() { return m_ticksPerMinute; }
    inline int get_mark_type() { return m_markType; }
    inline bool has_parenthesis() { return m_fParenthesis; }

    //setters
    inline void set_left_note_type(int noteType) { m_leftNoteType = noteType; }
    inline void set_right_note_type(int noteType) { m_rightNoteType = noteType; }
    inline void set_left_dots(int dots) { m_leftDots = dots; }
    inline void set_right_dots(int dots) { m_rightDots = dots; }
    inline void set_ticks_per_minute(int ticks) { m_ticksPerMinute = ticks; }
    inline void set_mark_type(int type) { m_markType = type; }
    inline void set_parenthesis(bool fValue) { m_fParenthesis = fValue; }

    inline void set_right_note_dots(const NoteTypeAndDots& figdots) {
        m_rightNoteType = figdots.noteType;
        m_rightDots = figdots.dots;
    }
    inline void set_left_note_dots(const NoteTypeAndDots& figdots) {
        m_leftNoteType = figdots.noteType;
        m_leftDots = figdots.dots;
    }

};

//----------------------------------------------------------------------------------
class ImoMusicData : public ImoSimpleObj
{
protected:
    std::list<ImoStaffObj*> m_staffobjs;

public:
    ImoMusicData() : ImoSimpleObj(ImoObj::k_music_data) {}
    ~ImoMusicData();

    //music data
    inline void add_staffobj(ImoStaffObj* pSO) { m_staffobjs.push_back(pSO); }
    inline std::list<ImoStaffObj*>& get_staffobjs() { return m_staffobjs; }
};

//----------------------------------------------------------------------------------
class ImoOption : public ImoSimpleObj
{
protected:
    int         m_type;
    string      m_name;
    string      m_sValue;
    bool        m_fValue;
    long        m_nValue;
    float       m_rValue;

public:
    ImoOption(string& name) : ImoSimpleObj(ImoObj::k_option), m_type(k_boolean), m_name(name)
                            , m_fValue(false) {}
    ~ImoOption() {}

    enum { k_boolean=0, k_number_long, k_number_float, k_string };

    //getters
    inline string get_name() { return m_name; }
    inline int get_type() { return m_type; }
    inline bool get_bool_value() { return m_fValue; }
    inline long get_long_value() { return m_nValue; }
    inline float get_float_value() { return m_rValue; }
    inline string& get_string_value() { return m_sValue; }

    //setters
    inline void set_type(int type) { m_type = type; }
    inline void set_bool_value(bool value) { m_fValue = value; }
    inline void set_long_value(long value) { m_nValue = value; }
    inline void set_float_value(float value) { m_rValue = value; }
    inline void set_string_value(const string& value) { m_sValue = value; }

};

//----------------------------------------------------------------------------------
class ImoSpacer : public ImoStaffObj
{
protected:
    Tenths  m_space;

public:
    ImoSpacer(DtoSpacer& dto);
    ImoSpacer(Tenths space=0.0f) : ImoStaffObj(ImoObj::k_spacer), m_space(space) {}
    ~ImoSpacer() {}

    //getters
    inline Tenths get_width() { return m_space; }

    //setters
    inline void set_width(Tenths space) { m_space = space; }

};

//----------------------------------------------------------------------------------
class ImoScore : public ImoContainerObj
{
protected:
    string          m_version;
    ColStaffObjs*   m_pColStaffObjs;
    std::list<ImoInstrument*> m_instruments;
    std::list<ImoOption*>     m_options;
    ImoSystemLayout* m_pSystemLayoutFirst;
    ImoSystemLayout* m_pSystemLayoutOther;

public:
    ImoScore();
    ~ImoScore();

    //getters and setters
    inline std::string& get_version() { return m_version; }
    inline void set_version(const std::string& version) { m_version = version; }

    inline ColStaffObjs* get_staffobjs_table() { return m_pColStaffObjs; }
    inline void set_staffobjs_table(ColStaffObjs* pColStaffObjs) { m_pColStaffObjs = pColStaffObjs; }

    //instruments
    inline void add_instrument(ImoInstrument* pInstr) { m_instruments.push_back(pInstr); }
    ImoInstrument* get_instrument(int iInstr);   //0..n-1
    inline int get_num_instruments() { return static_cast<int>(m_instruments.size()); }

    //options
    inline void add_option(ImoOption* pOpt) { m_options.push_back(pOpt); }
    inline bool has_options() { return m_options.size() > 0; }
    inline void delete_last_option() { m_options.pop_back(); }
    ImoOption* get_option(const std::string& name);

    //systems layout
    void add_sytem_layout(ImoSystemLayout* pSL);

protected:
    void delete_options();
    void delete_instruments();
    void delete_systems_layout();
    void delete_staffobjs_collection();

};

//----------------------------------------------------------------------------------
class ImoSystemLayout : public ImoSimpleObj
{
protected:
    bool    m_fFirst;   //true=first, false=other
    ImoSystemMargins* m_pMargins;

public:
    ImoSystemLayout() : ImoSimpleObj(ImoObj::k_system_layout), m_pMargins(NULL) {}
    ~ImoSystemLayout();

    //getters and setters
    inline int is_first() { return m_fFirst; }
    inline void set_first(bool fValue) { m_fFirst = fValue; }

    //margins
    inline void set_margins(ImoSystemMargins* pMargins) { m_pMargins = pMargins; }
};

//----------------------------------------------------------------------------------
class ImoSystemMargins : public ImoSimpleObj
{
protected:
    float   m_leftMargin;
    float   m_rightMargin;
    float   m_systemDistance;
    float   m_topSystemDistance;

public:
    ImoSystemMargins() : ImoSimpleObj(ImoObj::k_system_margins) {}
    ~ImoSystemMargins() {}

    //getters and setters
    inline float get_left_margin() { return m_leftMargin; }
    inline float get_right_margin() { return m_rightMargin; }
    inline float get_system_distance() { return m_systemDistance; }
    inline float get_top_system_distance() { return m_topSystemDistance; }
    inline void set_left_margin(float rValue) { m_leftMargin = rValue; }
    inline void set_right_margin(float rValue) { m_rightMargin = rValue; }
    inline void set_system_distance(float rValue) { m_systemDistance = rValue; }
    inline void set_top_system_distance(float rValue) { m_topSystemDistance = rValue; }
};

//----------------------------------------------------------------------------------
class ImoTie : public ImoAuxObj
{
protected:
    bool        m_fStart;
    int         m_tieNum;
    ImoNote*     m_pStartNote;
    ImoNote*     m_pEndNote;
    ImoBezier*   m_pStartBezier;
    ImoBezier*   m_pEndBezier;

public:
    ImoTie() : ImoAuxObj(ImoObj::k_tie), m_fStart(true), m_tieNum(0), m_pStartNote(NULL)
            , m_pEndNote(NULL), m_pStartBezier(NULL), m_pEndBezier(NULL) {}
    ~ImoTie();

    //getters
    inline bool is_start() { return m_fStart; }
    inline int get_tie_number() { return m_tieNum; }
    inline ImoNote* get_start_note() { return m_pStartNote; }
    inline ImoNote* get_end_note() { return m_pEndNote; }
    inline ImoBezier* get_start_bezier() { return m_pStartBezier; }
    inline ImoBezier* get_stop_bezier() { return m_pEndBezier; }

    //setters
    inline void set_start(bool value) { m_fStart = value; }
    inline void set_tie_number(int num) { m_tieNum = num; }
    inline void set_start_note(ImoNote* pNote) { m_pStartNote = pNote; }
    inline void set_end_note(ImoNote* pNote) { m_pEndNote = pNote; }
    inline void set_start_bezier(ImoBezier* pBezier) { m_pStartBezier = pBezier; }
    inline void set_stop_bezier(ImoBezier* pBezier) { m_pEndBezier = pBezier; }

};

// raw info about a pending tie
//----------------------------------------------------------------------------------
class ImoTieInfo : public ImoSimpleObj
{
protected:
    bool        m_fStart;
    int         m_tieNum;
    ImoNote*     m_pNote;
    ImoBezier*   m_pBezier;
    LdpElement* m_pTieElm;

public:
    ImoTieInfo() : ImoSimpleObj(ImoObj::k_tie_info), m_fStart(true), m_tieNum(0), m_pNote(NULL)
                 , m_pBezier(NULL), m_pTieElm(NULL) {}
    ~ImoTieInfo();

    //getters
    inline bool is_start() { return m_fStart; }
    inline int get_tie_number() { return m_tieNum; }
    inline ImoNote* get_note() { return m_pNote; }
    inline ImoBezier* get_bezier() { return m_pBezier; }
    inline LdpElement* get_tie_element() { return m_pTieElm; }
    int get_line_number();

    //setters
    inline void set_start(bool value) { m_fStart = value; }
    inline void set_tie_number(int num) { m_tieNum = num; }
    inline void set_note(ImoNote* pNote) { m_pNote = pNote; }
    inline void set_bezier(ImoBezier* pBezier) { m_pBezier = pBezier; }
    inline void set_tie_element(LdpElement* pElm) { m_pTieElm = pElm; }

};

//----------------------------------------------------------------------------------
class ImoTimeSignature : public ImoStaffObj
{
protected:
    int     m_beats;
    int     m_beatType;

public:
//    ImoTimeSignature() : ImoStaffObj(ImoObj::k_time_signature) , m_beats(2) , m_beatType(4) {}
    ImoTimeSignature(DtoTimeSignature& dto);
    ~ImoTimeSignature() {}

    //getters and setters
    inline int get_beats() { return m_beats; }
    inline void set_beats(int beats) { m_beats = beats; }
    inline int get_beat_type() { return m_beatType; }
    inline void set_beat_type(int beatType) { m_beatType = beatType; }

    //overrides: time signatures always in staff 0
    void set_staff(int staff) { m_staff = 0; }

};

//----------------------------------------------------------------------------------
class ImoTuplet : public ImoMultiRelObj
{
public:
    ImoTuplet() : ImoMultiRelObj(ImoObj::k_tuplet) {}
    ~ImoTuplet() {}

};

// raw info about a tuplet
//----------------------------------------------------------------------------------
class ImoTupletInfo : public ImoSimpleObj
{
protected:
    bool m_fStartOfTuplet;
    int m_nActualNum;
    int m_nNormalNum;
    bool m_fShowBracket;
    bool m_fShowNumber;
    int m_nPlacement;
    LdpElement* m_pTupletElm;
    ImoNoteRest* m_pNR;

public:
    ImoTupletInfo();
    ImoTupletInfo(LdpElement* pBeamElm);
    ~ImoTupletInfo() {}

    enum { k_default=0, };

    //getters
    inline LdpElement* get_tuplet_element() { return m_pTupletElm; }
    inline ImoNoteRest* get_note_rest() { return m_pNR; }
    inline bool is_start_of_tuplet() { return m_fStartOfTuplet; }
    inline bool is_end_of_tuplet() { return !m_fStartOfTuplet; }
    inline int get_actual_number() { return m_nActualNum; }
    inline int get_normal_number() { return m_nNormalNum; }
    inline bool get_show_bracket() { return m_fShowBracket; }
    inline bool get_show_number() { return m_fShowNumber; }
    int get_line_number();

    //setters
    inline void set_note_rest(ImoNoteRest* pNR) { m_pNR = pNR; }
    inline void set_tuplet_element(LdpElement* pElm) { m_pTupletElm = pElm; }
    inline void set_start_of_tuplet(bool value) { m_fStartOfTuplet = value; }
    inline void set_actual_number(int value) { m_nActualNum = value; }
    inline void set_normal_number(int value) { m_nNormalNum = value; }
    inline void set_show_bracket(bool value) { m_fShowBracket = value; }
    inline void set_show_number(bool value) { m_fShowNumber = value; }
    inline void set_placement(int value) { m_nPlacement = value; }

};


// a color in rgba16 format
//----------------------------------------------------------------------------------
class ImoColorInfo : public ImoSimpleObj
{
protected:
    rgba16 m_color;
    bool m_ok;

public:
    ImoColorInfo() : ImoSimpleObj(ImoObj::k_color_info), m_color(0, 0, 0, 255), m_ok(true) {}
    ImoColorInfo(int16u r, int16u g, int16u b, int16u a = 255);
    ~ImoColorInfo() {}

    rgba16& get_from_rgb_string(const std::string& rgb);
    rgba16& get_from_rgba_string(const std::string& rgba);
    rgba16& get_from_string(const std::string& hex);
    inline bool is_ok() { return m_ok; }

    inline int16u red() { return m_color.r; }
    inline int16u blue() { return m_color.b; }
    inline int16u green() { return m_color.g; }
    inline int16u alpha() { return m_color.a; }
    inline rgba16& get_color() { return m_color; }


protected:
    int16u convert_from_hex(const std::string& hex);

};


// A tree of ImoObj objects
typedef Tree<ImoObj>            ImoTree;
typedef NodeInTree<ImoObj>      ImoNode;



//----------------------------------------------------------------------------------
// global functions

extern int to_step(const char& letter);
extern int to_octave(const char& letter);
extern int to_accidentals(const std::string& accidentals);
extern int to_note_type(const char& letter);
extern bool ldp_pitch_to_components(const string& pitch, int *step, int* octave, int* accidentals);
extern NoteTypeAndDots ldp_duration_to_components(const string& duration);
extern float to_duration(int nNoteType, int nDots);


}   //namespace lenmus

#endif    // __LML_INTERNAL_MODEL_H__

