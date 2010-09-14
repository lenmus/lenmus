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

#ifndef __LML_BASIC_OBJECTS_H__        //to avoid nested includes
#define __LML_BASIC_OBJECTS_H__

#include <string>
#include <list>
#include <vector>
#include "lenmus_visitor.h"
#include "lenmus_basic.h"

using namespace std;

namespace lenmus
{

////forward declarations
//class DtoInstrument;
//class DtoNote;
//class DtoNoteRest;
//class DtoMusicData;
//class DtoSystemMargins;
//class DtoSystemLayout;
//class DtoTextString;
//class DtoColorInfo;
//class DtoAuxObj;
//class DtoOption;
//
//



// Model classes
//================================


//----------------------------------------------------------------------------------
class DtoObj
{
protected:
    long m_id;
    int m_objtype;

public:
    DtoObj() : m_id(-1L), m_objtype(0) {}
    DtoObj(long id, int objtype) : m_id(id), m_objtype(objtype) {}
    virtual ~DtoObj() {}


    //getters
    inline long get_id() { return m_id; }

    //setters
    inline void set_id(long id) { m_id = id; }

};

//----------------------------------------------------------------------------------
class DtoDocObj : public DtoObj
{
protected:
    Tenths m_txUserLocation;
    Tenths m_tyUserLocation;

public:
    DtoDocObj() : DtoObj(), m_txUserLocation(0.0f), m_tyUserLocation(0.0f) {}
    virtual ~DtoDocObj() {}

    //getters
    inline Tenths get_user_location_x() { return m_txUserLocation; }
    inline Tenths get_user_location_y() { return m_tyUserLocation; }

    //setters
    inline void set_user_location_x(Tenths tx) { m_txUserLocation = tx; }
    inline void set_user_location_y(Tenths ty) { m_tyUserLocation = ty; }
};

//----------------------------------------------------------------------------------
class DtoComponentObj : public DtoDocObj
{
protected:
    bool m_fVisible;
    rgba16 m_color;

public:
    DtoComponentObj() : DtoDocObj(), m_fVisible(true), m_color(0,0,0,255) {}
    virtual ~DtoComponentObj() {}

    //getters
    inline bool is_visible() { return m_fVisible; }
    inline rgba16& get_color() { return m_color; }

    //setters
    inline void set_visible(bool visible) { m_fVisible = visible; }
//    void set_color(DtoColorInfo* pColor);
    void set_color(rgba16 color);
};

//----------------------------------------------------------------------------------
class DtoStaffObj : public DtoComponentObj
{
protected:
    int m_staff;

public:
    DtoStaffObj() : DtoComponentObj(), m_staff(0) {}
    virtual ~DtoStaffObj() {}

    //getters
    inline int get_staff() { return m_staff; }

    //setters
    virtual void set_staff(int staff) { m_staff = staff; }
};

//----------------------------------------------------------------------------------
class DtoAuxObj : public DtoComponentObj
{
public:
    DtoAuxObj() : DtoComponentObj() {}
    virtual ~DtoAuxObj() {}

};

////An abstract AuxObj relating at least two StaffObjs
////----------------------------------------------------------------------------------
//class DtoRelObj : public DtoAuxObj
//{
//protected:
//    DtoRelObj(int objtype) : DtoAuxObj(objtype) {}
//    //DtoRelObj(DtoDocObj* pOwner, long id, int objtype)
//    //    : DtoAuxObj(pOwner, id, objtype) {}
//
//public:
//	virtual ~DtoRelObj() {}
//
//    ////building/destroying the relationship
//    //virtual void include(DtoStaffObj* pSO)=0;
//    //virtual void remove(DtoStaffObj* pSO)=0;
//	//virtual void on_relationship_modified()=0;
//
//    //information
//    virtual DtoStaffObj* get_start_object()=0;
//    virtual DtoStaffObj* get_end_object()=0;
//
//};
//
//
////An abstract AuxObj relating two and only two StaffObjs
////----------------------------------------------------------------------------------
//class DtoBinaryRelObj : public DtoRelObj
//{
//protected:
//    DtoStaffObj* m_pStartSO;     //StaffObjs related by this DtoRelObj
//    DtoStaffObj* m_pEndSO;
//
//    DtoBinaryRelObj(int objtype) : DtoRelObj(objtype) {}
//    //DtoBinaryRelObj(DtoDocObj* pOwner, long id, int objtype,
//    //               DtoStaffObj* pStartSO, DtoStaffObj* pEndSO);
//
//public:
//    virtual ~DtoBinaryRelObj();
//
//    //implementation of DtoRelObj pure virtual methods
//    //virtual void include(DtoStaffObj* pSO) {};
//    //virtual void remove(DtoStaffObj* pSO);
//    //virtual void on_relationship_modified() {};
//    virtual DtoStaffObj* get_start_object() { return m_pStartSO; }
//    virtual DtoStaffObj* get_end_object() { return m_pEndSO; }
//
//};
//
////An abstract AuxObj relating two or more StaffObjs
////----------------------------------------------------------------------------------
//class DtoMultiRelObj : public DtoRelObj
//{
//protected:
//    std::list<DtoStaffObj*> m_relatedObjects;
//
//    DtoMultiRelObj(int objtype) : DtoRelObj(objtype) {}
//    //DtoMultiRelObj(DtoDocObj* pOwner, long id, int objtype);
//
//public:
//    virtual ~DtoMultiRelObj();
//
//    //implementation of DtoRelObj pure virtual methods
//    //virtual void include(DtoStaffObj* pSO, int index = -1);
//    //virtual void remove(DtoStaffObj* pSO);
//    //virtual void on_relationship_modified() {};
//    DtoStaffObj* get_start_object() { return m_relatedObjects.front(); }
//    DtoStaffObj* get_end_object() { return m_relatedObjects.back(); }
//
//
//    //specific methods
//    void push_back(DtoStaffObj* pSO);
//    inline int get_num_objects() { return static_cast<int>( m_relatedObjects.size() ); }
//    //int get_object_index(DtoStaffObj* pSO);
//    std::list<DtoStaffObj*>& get_related_objects() { return m_relatedObjects; }
//
//};
//
//
//
////===================================================
//// Real objects
////===================================================

//----------------------------------------------------------------------------------
class DtoBarline : public DtoStaffObj
{
protected:
    int m_barlineType;

public:
    DtoBarline(int barlineType);
    ~DtoBarline() {}

    //getters and setters
    inline int get_barline_type() { return m_barlineType; }
    inline void set_barline_type(int type) { m_barlineType = type; }

    //overrides: barlines always in staff 0
    void set_staff(int staff) { m_staff = 0; }

};

////----------------------------------------------------------------------------------
//class DtoBeam : public DtoMultiRelObj
//{
//public:
//    DtoBeam() : DtoMultiRelObj(DtoObj::k_beam) {}
//    ~DtoBeam() {}
//
//    //type of beam
//    enum { k_none = 0, k_begin, k_continue, k_end, k_forward, k_backward, };
//
//};
//
//// raw info about a note beam
////----------------------------------------------------------------------------------
//class DtoBeamInfo : public DtoObj
//{
//protected:
//    int m_beamType[6];
//    int m_beamNum;
//    int m_line;
//    bool m_repeat[6];
//    DtoNoteRest* m_pNR;
//
//public:
//    DtoBeamInfo();
//    ~DtoBeamInfo() {}
//
//    //getters
//    inline int get_beam_number() { return m_beamNum; }
//    inline DtoNoteRest* get_note_rest() { return m_pNR; }
//    inline int get_line_number() { return m_line; }
//    int get_beam_type(int level);
//    bool get_repeat(int level);
//
//    //setters
//    inline void set_beam_number(int num) { m_beamNum = num; }
//    inline void set_note_rest(DtoNoteRest* pNR) { m_pNR = pNR; }
//    inline void set_line_number(int line) { m_line = line; }
//    void set_beam_type(int level, int type);
//    void set_repeat(int level, bool value);
//
//    //properties
//    bool is_end_of_beam();
//
//};
//
////----------------------------------------------------------------------------------
//class DtoBezier : public DtoObj
//{
//protected:
//    TPoint m_tPoints[4];   //start, end, ctrol1, ctrol2
//
//public:
//    DtoBezier() : DtoObj(DtoObj::k_bezier) {}
//    ~DtoBezier() {}
//
//	enum { k_start=0, k_end, k_ctrol1, k_ctrol2, };     // point number
//
//    //points
//    inline void set_point(int i, TPoint& value) { m_tPoints[i] = value; }
//    inline TPoint& get_point(int i) { return m_tPoints[i]; }
//
//};

//----------------------------------------------------------------------------------


class DtoClef : public DtoStaffObj
{
protected:
    int m_clefType;

public:
    DtoClef(int type) : DtoStaffObj(), m_clefType(type) {}
    ~DtoClef() {}

    //getters and setters
    inline int get_clef_type() { return m_clefType; }
    inline void set_clef_type(int type) { m_clefType = type; }

};

////----------------------------------------------------------------------------------
//class DtoContent : public DtoContainerObj
//{
//protected:
//    std::list<DtoDocObj*> m_contents;
//
//public:
//    DtoContent() : DtoContainerObj(DtoObj::k_content) {}
//    ~DtoContent();
//
//    //contents
//    inline void add_content_item(DtoDocObj* pDO) { m_contents.push_back(pDO); }
//    DtoDocObj* get_content_item(int iItem);   //0..n-1
//    inline int get_num_content_items() { return static_cast<int>(m_contents.size()); }
//
//};
//
////----------------------------------------------------------------------------------
//class DtoControl : public DtoStaffObj
//{
//protected:
//
//public:
//    DtoControl() : DtoStaffObj(DtoObj::k_control) {}
//    ~DtoControl() {}
//
//    //getters & setters
//};
//
////----------------------------------------------------------------------------------
//class DtoDocument : public DtoContainerObj
//{
//protected:
//    string m_version;
//    DtoContent* m_pContent;
//
//public:
//    DtoDocument() : DtoContainerObj(DtoObj::k_document), m_version(""), m_pContent(NULL) {}
//    ~DtoDocument();
//
//    //getters and setters
//    inline std::string& get_version() { return m_version; }
//    inline void set_version(const std::string& version) { m_version = version; }
//
//    //content
//    inline void set_content(DtoContent* pContent) { m_pContent = pContent; }
//    inline DtoDocObj* get_content_item(int iItem) { return m_pContent->get_content_item(iItem); }
//    inline int get_num_content_items() {
//        return (m_pContent != NULL ? m_pContent->get_num_content_items() : 0);
//    }
//    inline DtoContent* get_content() { return m_pContent; }
//
//};

//----------------------------------------------------------------------------------
class DtoFermata : public DtoAuxObj
{
protected:
    int m_placement;
    int m_symbol;

public:
    DtoFermata();
    ~DtoFermata() {}

    //getters
    inline int get_placement() { return m_placement; }
    inline int get_symbol() { return m_symbol; }

    //setters
    inline void set_placement(int placement) { m_placement = placement; }
    inline void set_symbol(int symbol) { m_symbol = symbol; }

};

////----------------------------------------------------------------------------------
//class DtoFiguredBass : public DtoStaffObj
//{
//protected:
//    long m_type;
//
//public:
//    DtoFiguredBass() : DtoStaffObj(DtoObj::k_figured_bass) {}
//    ~DtoFiguredBass() {}
//
//	enum { kSimple=0, kDouble, kStart, kEnd, kEndRepetition, kStartRepetition,
//           k_double_repetition, };
//
//    //getters and setters
//    inline int get_type() { return m_type; }
//    inline void set_type(int type) { m_type = type; }
//
//};

//----------------------------------------------------------------------------------
class DtoGoBackFwd : public DtoStaffObj
{
protected:
    bool    m_fFwd;
    float   m_rTimeShift;

    const float SHIFT_START_END;     //any too big value

public:
    DtoGoBackFwd(bool fFwd) : DtoStaffObj(), m_fFwd(fFwd), m_rTimeShift(0.0f),
                             SHIFT_START_END(100000000.0f) {}
    ~DtoGoBackFwd() {}

    //getters and setters
    inline bool is_forward() { return m_fFwd; }
    inline bool is_to_start() { return !m_fFwd && (m_rTimeShift == -SHIFT_START_END); }
    inline bool is_to_end() { return m_fFwd && (m_rTimeShift == SHIFT_START_END); }
    inline float get_time_shift() { return m_rTimeShift; }
    inline void set_to_start() { set_time_shift(SHIFT_START_END); }
    inline void set_to_end() { set_time_shift(SHIFT_START_END); }
    inline void set_time_shift(float rTime) { m_rTimeShift = (m_fFwd ? rTime : -rTime); }
};

////----------------------------------------------------------------------------------
//class DtoTextString : public DtoAuxObj
//{
//protected:
//    string  m_text;
//
//public:
//    DtoTextString() : DtoAuxObj(DtoObj::k_text_string) {}
//    ~DtoTextString() {}
//
//    //getters and setters
//    inline string& get_text() { return m_text; }
//    inline void set_text(const string& text) { m_text = text; }
//
//};
//
////----------------------------------------------------------------------------------
//class DtoMidiInfo : public DtoObj
//{
//protected:
//    int     m_instr;
//    int     m_channel;
//
//public:
//    DtoMidiInfo() : DtoObj(DtoObj::k_midi_info), m_instr(0), m_channel(0) {}
//    ~DtoMidiInfo() {}
//
//    //getters
//    inline int get_instrument() { return m_instr; }
//    inline int get_channel() { return m_channel; }
//
//    //setters
//    inline void set_instrument(int value) { m_instr = value; }
//    inline void set_channel(int value) { m_channel = value; }
//
//};
//
////----------------------------------------------------------------------------------
//class DtoInstrument : public DtoContainerObj
//{
//protected:
//    int     m_nStaves;
//    DtoMusicData* m_pMusicData;
//    DtoTextString m_name;
//    DtoTextString m_abbrev;
//    DtoMidiInfo m_midi;
//
//public:
//    DtoInstrument();
//    ~DtoInstrument();
//
//    //getters
//    inline int get_num_staves() { return m_nStaves; }
//    inline const std::string& get_name() { return m_name.get_text(); }
//    inline const std::string& get_abbrev() { return m_abbrev.get_text(); }
//    inline int get_instrument() { return m_midi.get_instrument(); }
//    inline int get_channel() { return m_midi.get_channel(); }
//    inline DtoMusicData* get_musicdata() { return m_pMusicData; }
//
//    //setters
//    inline void set_num_staves(int staves) { m_nStaves = staves; }
//    inline void set_musicdata(DtoMusicData* pMD) { m_pMusicData = pMD; }
//    void set_name(DtoTextString* pText);
//    void set_abbrev(DtoTextString* pText);
//    void set_midi_info(DtoMidiInfo* pInfo);
//
//};
//
////----------------------------------------------------------------------------------
//class DtoInstrGroup : public DtoObj
//{
//protected:
//    bool m_fJoinBarlines;
//    int m_symbol;           // enum k_none, k_brace, k_bracket, ...
//    DtoTextString m_name;
//    DtoTextString m_abbrev;
//    std::list<DtoInstrument*> m_instruments;
//
//public:
//    DtoInstrGroup() : DtoObj(DtoObj::k_instr_group), m_fJoinBarlines(true), m_symbol(k_brace)
//                   , m_name(), m_abbrev() {}
//    ~DtoInstrGroup();
//
//    enum { k_none=0, k_brace, k_bracket, };
//
//    //getters
//    inline bool join_barlines() { return m_fJoinBarlines; }
//    inline int get_symbol() { return m_symbol; }
//    inline const std::string& get_name() { return m_name.get_text(); }
//    inline const std::string& get_abbrev() { return m_abbrev.get_text(); }
//
//    //setters
//    void set_name(DtoTextString* pText);
//    void set_abbrev(DtoTextString* pText);
//    inline void set_symbol(int symbol) { m_symbol = symbol; }
//    inline void set_join_barlines(bool value) { m_fJoinBarlines = value; }
//
//    //instruments
//    inline void add_instrument(DtoInstrument* pInstr) { m_instruments.push_back(pInstr); }
//    DtoInstrument* get_instrument(int iInstr);   //0..n-1
//    inline int get_num_instruments() { return static_cast<int>(m_instruments.size()); }
//
//protected:
//    void delete_instruments();
//
//};

//----------------------------------------------------------------------------------
class DtoKeySignature : public DtoStaffObj
{
protected:
    int m_keyType;

public:
    DtoKeySignature(int type) : DtoStaffObj() , m_keyType(type) {}
    ~DtoKeySignature() {}

    //getters and setters
    inline int get_key_type() { return m_keyType; }
    inline void set_key_type(int type) { m_keyType = type; }

    //overrides: key signatures always in staff 0
    void set_staff(int staff) { m_staff = 0; }

};

////----------------------------------------------------------------------------------
//class DtoMetronomeMark : public DtoStaffObj
//{
//protected:
//    int     m_markType;
//    int     m_ticksPerMinute;
//    int     m_leftNoteType;
//    int     m_leftDots;
//    int     m_rightNoteType;
//    int     m_rightDots;
//    bool    m_fParenthesis;
//
//public:
//    DtoMetronomeMark() : DtoStaffObj(DtoObj::k_metronome_mark), m_markType(k_value),
//        m_ticksPerMinute(60),
//        m_leftNoteType(0), m_leftDots(0),
//        m_rightNoteType(0), m_rightDots(0),
//        m_fParenthesis(false) {}
//    ~DtoMetronomeMark() {}
//
//    enum { k_note_value=0, k_note_note, k_value, };
//
//    //getters
//    inline int get_left_note_type() { return m_leftNoteType; }
//    inline int get_right_note_type() { return m_rightNoteType; }
//    inline int get_left_dots() { return m_leftDots; }
//    inline int get_right_dots() { return m_rightDots; }
//    inline int get_ticks_per_minute() { return m_ticksPerMinute; }
//    inline int get_mark_type() { return m_markType; }
//    inline bool has_parenthesis() { return m_fParenthesis; }
//
//    //setters
//    inline void set_left_note_type(int noteType) { m_leftNoteType = noteType; }
//    inline void set_right_note_type(int noteType) { m_rightNoteType = noteType; }
//    inline void set_left_dots(int dots) { m_leftDots = dots; }
//    inline void set_right_dots(int dots) { m_rightDots = dots; }
//    inline void set_ticks_per_minute(int ticks) { m_ticksPerMinute = ticks; }
//    inline void set_mark_type(int type) { m_markType = type; }
//    inline void set_parenthesis(bool fValue) { m_fParenthesis = fValue; }
//
//    inline void set_right_note_dots(const NoteTypeAndDots& figdots) {
//        m_rightNoteType = figdots.noteType;
//        m_rightDots = figdots.dots;
//    }
//    inline void set_left_note_dots(const NoteTypeAndDots& figdots) {
//        m_leftNoteType = figdots.noteType;
//        m_leftDots = figdots.dots;
//    }
//
//};
//
////----------------------------------------------------------------------------------
//class DtoMusicData : public DtoContainerObj
//{
//protected:
//    std::list<DtoStaffObj*> m_staffobjs;
//
//public:
//    DtoMusicData() : DtoContainerObj(DtoObj::k_music_data) {}
//    ~DtoMusicData();
//
//    //music data
//    inline void add_staffobj(DtoStaffObj* pSO) { m_staffobjs.push_back(pSO); }
//    inline std::list<DtoStaffObj*>& get_staffobjs() { return m_staffobjs; }
//};

//----------------------------------------------------------------------------------
class DtoNoteRest : public DtoStaffObj
{
protected:
    int     m_nNoteType;
    int     m_nDots;
    int     m_nVoice;
    bool    m_fInTuplet;

public:
    DtoNoteRest();
    virtual ~DtoNoteRest() {}

    //getters
    inline int get_note_type() { return m_nNoteType; }
    inline int get_dots() { return m_nDots; }
    inline int get_voice() { return m_nVoice; }
    inline bool is_in_tuplet() { return m_fInTuplet; }

    //setters
    inline void set_note_type(int noteType) { m_nNoteType = noteType; }
    inline void get_dots(int dots) { m_nDots = dots; }
    inline void set_voice(int voice) { m_nVoice = voice; }
    inline void set_in_tuplet(bool value) { m_fInTuplet = value; }
    void set_note_type_and_dots(int noteType, int dots);

};

//----------------------------------------------------------------------------------
class DtoRest : public DtoNoteRest
{
protected:

public:
    DtoRest() : DtoNoteRest() {}
    ~DtoRest() {}

};

//----------------------------------------------------------------------------------
class DtoNote : public DtoNoteRest
{
protected:
    int     m_step;
    int     m_octave;
    int     m_accidentals;
    int     m_stemDirection;
    bool    m_tiedNext;
    bool    m_tiedPrev;
    bool    m_inChord;


public:
    DtoNote();
    ~DtoNote() {}

    //pitch
    inline int get_step() { return m_step; }
    inline int get_octave() { return m_octave; }
    inline int get_accidentals() { return m_accidentals; }
    inline void set_step(int step) { m_step = step; }
    inline void set_octave(int octave) { m_octave = octave; }
    inline void set_accidentals(int accidentals) { m_accidentals = accidentals; }
    inline void set_pitch(int step, int octave, int accidentals) {
        m_step = step;
        m_octave = octave;
        m_accidentals = accidentals;
    }

    //ties
    inline bool is_tied_next() { return m_tiedNext; }
    inline bool is_tied_prev() { return m_tiedPrev; }
    inline void set_tied_next(bool value) { m_tiedNext = value; }
    inline void set_tied_prev(bool value) { m_tiedPrev = value; }

    //stem
    inline void set_stem_direction(int value) { m_stemDirection = value; }
    inline int get_stem_direction() { return m_stemDirection; }

    //in chord
    inline void set_in_chord(bool value) { m_inChord = value; }
    inline bool is_in_chord() { return m_inChord; }


};

////----------------------------------------------------------------------------------
//class DtoOption : public DtoObj
//{
//protected:
//    int         m_type;
//    string      m_name;
//    string      m_sValue;
//    bool        m_fValue;
//    long        m_nValue;
//    float       m_rValue;
//
//public:
//    DtoOption(string& name) : DtoObj(DtoObj::k_option), m_type(k_boolean), m_name(name)
//                           , m_fValue(false) {}
//    ~DtoOption() {}
//
//    enum { k_boolean=0, k_number_long, k_number_float, k_string };
//
//    //getters
//    inline string get_name() { return m_name; }
//    inline int get_type() { return m_type; }
//    inline bool get_bool_value() { return m_fValue; }
//    inline long get_long_value() { return m_nValue; }
//    inline float get_float_value() { return m_rValue; }
//    inline string& get_string_value() { return m_sValue; }
//
//    //setters
//    inline void set_type(int type) { m_type = type; }
//    inline void set_bool_value(bool value) { m_fValue = value; }
//    inline void set_long_value(long value) { m_nValue = value; }
//    inline void set_float_value(float value) { m_rValue = value; }
//    inline void set_string_value(const string& value) { m_sValue = value; }
//
//};

//----------------------------------------------------------------------------------
class DtoSpacer : public DtoStaffObj
{
protected:
    Tenths  m_space;

public:
    DtoSpacer(Tenths space=0.0f) : DtoStaffObj(), m_space(space) {}
    ~DtoSpacer() {}

    //getters
    inline Tenths get_width() { return m_space; }

    //setters
    inline void set_width(Tenths space) { m_space = space; }

};

////----------------------------------------------------------------------------------
//class DtoScore : public DtoContainerObj
//{
//protected:
//    string          m_version;
//    std::list<DtoInstrument*> m_instruments;
//    std::list<DtoOption*>     m_options;
//    DtoSystemLayout* m_pSystemLayoutFirst;
//    DtoSystemLayout* m_pSystemLayoutOther;
//
//public:
//    DtoScore();
//    ~DtoScore();
//
//    //getters and setters
//    inline std::string& get_version() { return m_version; }
//    inline void set_version(const std::string& version) { m_version = version; }
//
//    //instruments
//    inline void add_instrument(DtoInstrument* pInstr) { m_instruments.push_back(pInstr); }
//    DtoInstrument* get_instrument(int iInstr);   //0..n-1
//    inline int get_num_instruments() { return static_cast<int>(m_instruments.size()); }
//
//    //options
//    inline void add_option(DtoOption* pOpt) { m_options.push_back(pOpt); }
//    inline bool has_options() { return m_options.size() > 0; }
//    inline void delete_last_option() { m_options.pop_back(); }
//    DtoOption* get_option(const std::string& name);
//
//    //systems layout
//    void add_sytem_layout(DtoSystemLayout* pSL);
//
//protected:
//    void delete_options();
//    void delete_instruments();
//    void delete_systems_layout();
//    void delete_staffobjs_collection();
//
//};
//
////----------------------------------------------------------------------------------
//class DtoSystemLayout : public DtoObj
//{
//protected:
//    bool    m_fFirst;   //true=first, false=other
//    DtoSystemMargins* m_pMargins;
//
//public:
//    DtoSystemLayout() : DtoObj(DtoObj::k_system_layout), m_pMargins(NULL) {}
//    ~DtoSystemLayout();
//
//    //getters and setters
//    inline int is_first() { return m_fFirst; }
//    inline void set_first(bool fValue) { m_fFirst = fValue; }
//
//    //margins
//    inline void set_margins(DtoSystemMargins* pMargins) { m_pMargins = pMargins; }
//};
//
////----------------------------------------------------------------------------------
//class DtoSystemMargins : public DtoObj
//{
//protected:
//    float   m_leftMargin;
//    float   m_rightMargin;
//    float   m_systemDistance;
//    float   m_topSystemDistance;
//
//public:
//    DtoSystemMargins() : DtoObj(DtoObj::k_system_margins) {}
//    ~DtoSystemMargins() {}
//
//    //getters and setters
//    inline float get_left_margin() { return m_leftMargin; }
//    inline float get_right_margin() { return m_rightMargin; }
//    inline float get_system_distance() { return m_systemDistance; }
//    inline float get_top_system_distance() { return m_topSystemDistance; }
//    inline void set_left_margin(float rValue) { m_leftMargin = rValue; }
//    inline void set_right_margin(float rValue) { m_rightMargin = rValue; }
//    inline void set_system_distance(float rValue) { m_systemDistance = rValue; }
//    inline void set_top_system_distance(float rValue) { m_topSystemDistance = rValue; }
//};
//
////----------------------------------------------------------------------------------
//class DtoTie : public DtoAuxObj
//{
//protected:
//    bool        m_fStart;
//    int         m_tieNum;
//    DtoNote*     m_pStartNote;
//    DtoNote*     m_pEndNote;
//    DtoBezier*   m_pStartBezier;
//    DtoBezier*   m_pEndBezier;
//
//public:
//    DtoTie() : DtoAuxObj(DtoObj::k_tie), m_fStart(true), m_tieNum(0), m_pStartNote(NULL)
//            , m_pEndNote(NULL), m_pStartBezier(NULL), m_pEndBezier(NULL) {}
//    ~DtoTie();
//
//    //getters
//    inline bool is_start() { return m_fStart; }
//    inline int get_tie_number() { return m_tieNum; }
//    inline DtoNote* get_start_note() { return m_pStartNote; }
//    inline DtoNote* get_end_note() { return m_pEndNote; }
//    inline DtoBezier* get_start_bezier() { return m_pStartBezier; }
//    inline DtoBezier* get_stop_bezier() { return m_pEndBezier; }
//
//    //setters
//    inline void set_start(bool value) { m_fStart = value; }
//    inline void set_tie_number(int num) { m_tieNum = num; }
//    inline void set_start_note(DtoNote* pNote) { m_pStartNote = pNote; }
//    inline void set_end_note(DtoNote* pNote) { m_pEndNote = pNote; }
//    inline void set_start_bezier(DtoBezier* pBezier) { m_pStartBezier = pBezier; }
//    inline void set_stop_bezier(DtoBezier* pBezier) { m_pEndBezier = pBezier; }
//
//};
//
//// raw info about a pending tie
////----------------------------------------------------------------------------------
//class DtoTieInfo : public DtoObj
//{
//protected:
//    bool        m_fStart;
//    int         m_tieNum;
//    DtoNote*     m_pNote;
//    DtoBezier*   m_pBezier;
//
//public:
//    DtoTieInfo() : DtoObj(DtoObj::k_tie_info), m_fStart(true), m_tieNum(0), m_pNote(NULL)
//                 , m_pBezier(NULL) {}
//    ~DtoTieInfo();
//
//    //getters
//    inline bool is_start() { return m_fStart; }
//    inline int get_tie_number() { return m_tieNum; }
//    inline DtoNote* get_note() { return m_pNote; }
//    inline DtoBezier* get_bezier() { return m_pBezier; }
//    inline int get_line_number() { return 0; }      //TODO
//
//    //setters
//    inline void set_start(bool value) { m_fStart = value; }
//    inline void set_tie_number(int num) { m_tieNum = num; }
//    inline void set_note(DtoNote* pNote) { m_pNote = pNote; }
//    inline void set_bezier(DtoBezier* pBezier) { m_pBezier = pBezier; }
//
//};

//----------------------------------------------------------------------------------
class DtoTimeSignature : public DtoStaffObj
{
protected:
    int     m_beats;
    int     m_beatType;

public:
    DtoTimeSignature() : DtoStaffObj() , m_beats(2) , m_beatType(4) {}
    ~DtoTimeSignature() {}

    //getters and setters
    inline int get_beats() { return m_beats; }
    inline void set_beats(int beats) { m_beats = beats; }
    inline int get_beat_type() { return m_beatType; }
    inline void set_beat_type(int beatType) { m_beatType = beatType; }

    //overrides: time signatures always in staff 0
    void set_staff(int staff) { m_staff = 0; }

};

////----------------------------------------------------------------------------------
//class DtoTuplet : public DtoMultiRelObj
//{
//public:
//    DtoTuplet() : DtoMultiRelObj(DtoObj::k_tuplet) {}
//    ~DtoTuplet() {}
//
//};
//
//// raw info about a tuplet
////----------------------------------------------------------------------------------
//class DtoTupletInfo : public DtoObj
//{
//protected:
//    bool m_fStartOfTuplet;
//    int m_nActualNum;
//    int m_nNormalNum;
//    bool m_fShowBracket;
//    bool m_fShowNumber;
//    int m_nPlacement;
//    DtoNoteRest* m_pNR;
//
//public:
//    DtoTupletInfo();
//    ~DtoTupletInfo() {}
//
//    enum { k_default=0, };
//
//    //getters
//    inline DtoNoteRest* get_note_rest() { return m_pNR; }
//    inline bool is_start_of_tuplet() { return m_fStartOfTuplet; }
//    inline bool is_end_of_tuplet() { return !m_fStartOfTuplet; }
//    inline int get_actual_number() { return m_nActualNum; }
//    inline int get_normal_number() { return m_nNormalNum; }
//    inline bool get_show_bracket() { return m_fShowBracket; }
//    inline bool get_show_number() { return m_fShowNumber; }
//    inline int get_line_number() { return 0; }      //TODO
//
//    //setters
//    inline void set_note_rest(DtoNoteRest* pNR) { m_pNR = pNR; }
//    inline void set_start_of_tuplet(bool value) { m_fStartOfTuplet = value; }
//    inline void set_actual_number(int value) { m_nActualNum = value; }
//    inline void set_normal_number(int value) { m_nNormalNum = value; }
//    inline void set_show_bracket(bool value) { m_fShowBracket = value; }
//    inline void set_show_number(bool value) { m_fShowNumber = value; }
//    inline void set_placement(int value) { m_nPlacement = value; }
//
//};
//
//
//// a color in rgba16 format
////----------------------------------------------------------------------------------
//class DtoColorInfo : public DtoObj
//{
//protected:
//    rgba16 m_color;
//    bool m_ok;
//
//public:
//    DtoColorInfo() : DtoObj(DtoObj::k_color_info), m_color(0, 0, 0, 255), m_ok(true) {}
//    DtoColorInfo(int16u r, int16u g, int16u b, int16u a = 255);
//    ~DtoColorInfo() {}
//
//    rgba16& get_from_rgb_string(const std::string& rgb);
//    rgba16& get_from_rgba_string(const std::string& rgba);
//    rgba16& get_from_string(const std::string& hex);
//    inline bool is_ok() { return m_ok; }
//
//    inline int16u red() { return m_color.r; }
//    inline int16u blue() { return m_color.b; }
//    inline int16u green() { return m_color.g; }
//    inline int16u alpha() { return m_color.a; }
//
//
//protected:
//    int16u convert_from_hex(const std::string& hex);
//
//};
//



}   //namespace lenmus

#endif    // __LML_BASIC_OBJECTS_H__

