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

#ifndef __LM_IM_NOTE_H__        //to avoid nested includes
#define __LM_IM_NOTE_H__

#include "lenmus_internal_model.h"

using namespace std;

namespace lenmus
{

//----------------------------------------------------------------------------------
class ImNoteRest : public ImStaffObj
{
protected:
    int m_noteType;
    int m_dots;
    int m_voice;

public:
    ImNoteRest();
    virtual ~ImNoteRest() {}

    enum    { Longa=0, Breve=1, Whole=2, Half=3, Quarter=4, Eighth=5, D16th=6,
              D32th=7, D64th=8, D128th=9, D256th=10, };

    void set_duration(int noteType, int dots) {
        m_noteType = noteType;
        m_dots = dots;
    }
    inline int get_note_type() { return m_noteType; }
    inline void set_note_type(int noteType) { m_noteType = noteType; }
    inline int get_dots() { return m_dots; }
    inline void set_dots(int dots) { m_dots = dots; }
    inline int get_voice() { return m_voice; }
    inline void set_voice(int voice) { m_voice = voice; }

    //ImStaffObj overrides
    float get_duration();

};

//----------------------------------------------------------------------------------
class ImRest : public ImNoteRest
{
protected:

public:
    ImRest() : ImNoteRest() {}
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
    ~ImNote() {}

    enum    { C=0, D=1, E=2, F=3, G=4, A=5, B=6, last=B, NoPitch=-1, };     //steps
    enum    { NoAccidentals=0, Sharp, SharpSharp, DoubleSharp, NaturalSharp,
              Flat, FlatFlat, NaturalFlat, Natural, };

    //getters and setters
    void set_pitch(int step, int octave, int accidentals) {
        m_step = step;
        m_octave = octave;
        m_accidentals = accidentals;
    }
    inline int get_step() { return m_step; }
    inline void set_step(int step) { m_step = step; }
    inline int get_octave() { return m_octave; }
    inline void set_octave(int octave) { m_octave = octave; }
    inline int get_accidentals() { return m_accidentals; }
    inline void set_accidentals(int accidentals) { m_accidentals = accidentals; }

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

#endif    // __LM_IM_NOTE_H__

