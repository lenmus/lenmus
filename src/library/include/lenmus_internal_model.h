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

#ifndef __LM_INTERNAL_MODEL_H__        //to avoid nested includes
#define __LM_INTERNAL_MODEL_H__


using namespace std;

namespace lenmus
{

//forward declarations
class LdpElement;
class ColStaffObjs;


//----------------------------------------------------------------------------------
class ImObj
{
protected:

public:
    ImObj() {}
    virtual ~ImObj() {}

};

//----------------------------------------------------------------------------------
class ImStaffObj : public ImObj
{
protected:
    int m_staff;

public:
    ImStaffObj() : ImObj(), m_staff(0) {}
    virtual ~ImStaffObj() {}

    virtual float get_duration() { return 0.0f; }
    inline int get_staff() { return m_staff; }
    inline void set_staff(int staff) { m_staff = staff; }

};

//----------------------------------------------------------------------------------
class ImScore : public ImObj
{
protected:
    string          m_version;
    int             m_nInstruments;
    ColStaffObjs*   m_pColStaffObjs;

public:
    ImScore();
    ~ImScore() {}

    //getters and setters
    inline std::string& get_version() { return m_version; }
    inline void set_version(const std::string& version) { m_version = version; }
    inline int get_num_instruments() { return m_nInstruments; }
    inline void set_num_instruments(int num) { m_nInstruments = num; }
    inline void add_instrument() { m_nInstruments++; }

    inline ColStaffObjs* get_staffobjs_table() { return m_pColStaffObjs; }
    inline void set_staffobjs_table(ColStaffObjs* pColStaffObjs) { m_pColStaffObjs = pColStaffObjs; }
};

//----------------------------------------------------------------------------------
class ImBarline : public ImStaffObj
{
protected:
    long m_type;

public:
    ImBarline();
    ~ImBarline() {}

	enum { kSimple=0, kDouble, kStart, kEnd, kEndRepetition, kStartRepetition,
           kDoubleRepetition, };

    //getters and setters
    inline int get_type() { return m_type; }
    inline void set_type(int type) { m_type = type; }

};

//----------------------------------------------------------------------------------
class ImClef : public ImStaffObj
{
protected:
    long m_type;

public:
    ImClef() : ImStaffObj() {}
    ~ImClef() {}

    enum { 
        undefined=-1,
        kG3 = 0,
        kF4,
        kF3,
        kC1,
        kC2,
        kC3,
        kC4,
        kPercussion,
        // other clefs not available for exercises
        kC5,
        kF5,
        kG1,
        k8_G3,        //8 above
        kG3_8,        //8 below
        k8_F4,        //8 above
        kF4_8,        //8 below
        k15_G3,       //15 above
        kG3_15,       //15 below
        k15_F4,       //15 above
        kF4_15,       //15 below
    };

    //getters and setters
    inline int get_type() { return m_type; }
    inline void set_type(int type) { m_type = type; }

};

//----------------------------------------------------------------------------------
class ImGoBackFwd : public ImStaffObj
{
protected:
    bool    m_fFwd;
    float   m_rTimeShift;

    const float SHIFT_START_END;     //any too big value

public:
    ImGoBackFwd(bool fFwd) : ImStaffObj(), m_fFwd(fFwd), m_rTimeShift(0.0f),
                             SHIFT_START_END(100000000.0f) {}
    ~ImGoBackFwd() {}

    //getters and setters
    inline bool is_to_start() { return !m_fFwd && (m_rTimeShift == -SHIFT_START_END); }
    inline bool is_to_end() { return m_fFwd && (m_rTimeShift == SHIFT_START_END); }
    inline float get_time_shift() { return m_rTimeShift; }
    inline void set_to_start() { set_time_shift(SHIFT_START_END); }
    inline void set_to_end() { set_time_shift(SHIFT_START_END); }
    inline void set_time_shift(float rTime) { m_rTimeShift = (m_fFwd ? rTime : -rTime); }
};

//----------------------------------------------------------------------------------
class ImInstrument : public ImObj
{
protected:
    string  m_name;
    string  m_abbrev;
    int     m_midiInstr;
    int     m_midiChannel;
    int     m_nStaves;

public:
    ImInstrument();
    ~ImInstrument() {}

    //getters and setters
    inline int get_num_staves() { return m_nStaves; }
    inline void set_num_staves(int staves) { m_nStaves = staves; }
};

//----------------------------------------------------------------------------------
class ImInstrGroup : public ImObj
{
protected:

public:
    ImInstrGroup() : ImObj() {}
    ~ImInstrGroup() {}

};

//----------------------------------------------------------------------------------
class ImKeySignature : public ImStaffObj
{
protected:
    int m_type;

public:
    ImKeySignature();
    ~ImKeySignature() {}

	enum { Undefined=-1, C=0, G, D, A, E, B, Fs, Cs, Cf, Gf, Df, Af, Ef, Bf, F,
           a, e, b, fs, cs, gs, ds, as, af, ef, bf, f, c, g, d };

    //getters and setters
    inline int get_type() { return m_type; }
    inline void set_type(int type) { m_type = type; }

};

//----------------------------------------------------------------------------------
class ImTimeSignature : public ImStaffObj
{
protected:
    int     m_beats;
    int     m_beatType;

public:
    ImTimeSignature();
    ~ImTimeSignature() {}

    //getters and setters
    inline int get_beats() { return m_beats; }
    inline void set_beats(int beats) { m_beats = beats; }
    inline int get_beat_type() { return m_beatType; }
    inline void set_beat_type(int beatType) { m_beatType = beatType; }

};



}   //namespace lenmus

#endif    // __LM_INTERNAL_MODEL_H__

