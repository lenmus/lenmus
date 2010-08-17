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

#ifndef __LML_IM_NOTE_H__        //to avoid nested includes
#define __LML_IM_NOTE_H__

#include "lenmus_internal_model.h"

using namespace std;

namespace lenmus
{

//----------------------------------------------------------------------------------
class ImNoteRest : public ImStaffObj
{
protected:
    int     m_nNoteType;
    float   m_rDuration;
    int     m_nDots;
    int     m_nVoice;
    bool    m_fVisible;
    bool    m_fBeamed;
    BeamInfo m_beamInfo[6];

public:
    ImNoteRest();
    ImNoteRest(long id, int type, int nNoteType, float rDuration, int nDots, int nStaff,
               int nVoice, bool fVisible, bool fBeamed, BeamInfo* pBeamInfo);
    virtual ~ImNoteRest() {}

    enum    { k_longa=0, k_breve=1, k_whole=2, k_half=3, k_quarter=4, k_eighth=5,
              k_16th=6, k_32th=7, k_64th=8, k_128th=9, k_256th=10, };

    //getters
    inline int get_note_type() { return m_nNoteType; }
    inline float get_duration() { return m_rDuration; }
    inline int get_dots() { return m_nDots; }
    inline int get_voice() { return m_nVoice; }
    inline bool get_visible() { return m_fVisible; }
    inline bool get_beamed() { return m_fBeamed; }
    inline BeamInfo* get_beam_info() { return &m_beamInfo[0]; }

    //setters
    inline void set_note_type(int noteType) { m_nNoteType = noteType; }
    inline void set_duration(float duration) { m_rDuration = duration; }
    inline void get_dots(int dots) { m_nDots = dots; }
    inline void set_voice(int voice) { m_nVoice = voice; }
    inline void set_visible(bool visible) { m_fVisible = visible; }
    inline void set_beamed(bool beamed) { m_fBeamed = beamed; }
    inline void set_beam_info(int i, BeamInfo beamInfo) { m_beamInfo[i] = beamInfo; }
    void set_duration_and_dots(int noteType, int dots);

};

//----------------------------------------------------------------------------------
class ImRest : public ImNoteRest
{
protected:

public:
    ImRest() : ImNoteRest() {}
    ImRest(long id, int nNoteType, float rDuration, int nDots, int nStaff, 
           int nVoice, bool fVisible = true, bool fBeamed = false,
           BeamInfo* pBeamInfo = NULL);

    virtual ~ImRest() {}

};

//----------------------------------------------------------------------------------
class ImNote : public ImNoteRest
{
protected:
    int m_step;
    int m_octave;
    int m_accidentals;


public:
    ImNote();
    ImNote(long id, int nNoteType, float rDuration, int nDots, int nStaff, int nVoice,
           bool fVisible, bool fBeamed, BeamInfo* pBeamInfo);
    ~ImNote() {}

    enum    { C=0, D=1, E=2, F=3, G=4, A=5, B=6, last=B, NoPitch=-1, };     //steps
    enum    { NoAccidentals=0, Sharp, SharpSharp, DoubleSharp, NaturalSharp,
              Flat, FlatFlat, NaturalFlat, Natural, };

    //getters
    inline int get_step() { return m_step; }
    inline int get_octave() { return m_octave; }
    inline int get_accidentals() { return m_accidentals; }

    //setters
    inline void set_step(int step) { m_step = step; }
    inline void set_octave(int octave) { m_octave = octave; }
    inline void set_accidentals(int accidentals) { m_accidentals = accidentals; }
    inline void set_pitch(int step, int octave, int accidentals) {
        m_step = step;
        m_octave = octave;
        m_accidentals = accidentals;
    }
};

//----------------------------------------------------------------------------------
// global functions

extern int to_step(const char& letter);
extern int to_octave(const char& letter);
extern int to_accidentals(const std::string& accidentals);
extern int to_note_type(const char& letter);
extern bool ldp_pitch_to_components(const string& pitch, int *step, int* octave, int* accidentals);
extern bool ldp_duration_to_components(const string& duration, int* noteType, int* dots);
extern float to_duration(int nNoteType, int nDots);


}   //namespace lenmus

#endif    // __LML_IM_NOTE_H__

