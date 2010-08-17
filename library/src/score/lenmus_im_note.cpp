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

#include <math.h>               //pow
#include "lenmus_im_note.h"

namespace lenmus
{


//-------------------------------------------------------------------------------------
// ImNoteRest implementation
//-------------------------------------------------------------------------------------

ImNoteRest::ImNoteRest()
    : ImStaffObj()
    , m_nNoteType(ImNote::k_quarter)
    , m_nDots(0)
    , m_nVoice(1)
{
} 

ImNoteRest::ImNoteRest(long id, int type, int nNoteType, float rDuration, int nDots,
               int nStaff, int nVoice, bool fVisible,
               bool fBeamed, BeamInfo* pBeamInfo)
    : ImStaffObj(id, type)
    , m_nNoteType(nNoteType)
    , m_rDuration(rDuration)
    , m_nDots(nDots)
    , m_nVoice(nVoice)
    , m_fVisible(fVisible)
    , m_fBeamed(fBeamed)
{
    set_staff(nStaff);
    for (int i=0; i < 6; ++i)
        m_beamInfo[i] = *(pBeamInfo+i);
} 

void ImNoteRest::set_duration_and_dots(int noteType, int dots)
{
    m_nNoteType = noteType;
    m_nDots = dots;
    m_rDuration = to_duration(m_nNoteType, m_nDots);
}


//-------------------------------------------------------------------------------------
// ImRest implementation
//-------------------------------------------------------------------------------------

ImRest::ImRest(long id, int nNoteType, float rDuration, int nDots, int nStaff, 
               int nVoice, bool fVisible, bool fBeamed, BeamInfo* pBeamInfo) 
    : ImNoteRest(id, ImObj::k_rest, nNoteType, rDuration, nDots,
                 nStaff, nVoice, fVisible, fBeamed, pBeamInfo)
{
}


//-------------------------------------------------------------------------------------
// ImNote implementation
//-------------------------------------------------------------------------------------

ImNote::ImNote()
    : ImNoteRest()
    , m_step(ImNote::C)
    , m_octave(4)
    , m_accidentals(ImNote::NoAccidentals)
{
} 

ImNote::ImNote(long id, int nNoteType, float rDuration, int nDots, int nStaff, int nVoice,
               bool fVisible, bool fBeamed, BeamInfo* pBeamInfo)
    : ImNoteRest(id, ImObj::k_note, nNoteType, rDuration, nDots, nStaff, nVoice,
                 fVisible, fBeamed, pBeamInfo)
    , m_step(ImNote::C)
    , m_octave(4)
    , m_accidentals(ImNote::NoAccidentals)
{
} 



//-------------------------------------------------------------------------------------
// global functions related to notes
//-------------------------------------------------------------------------------------

int to_step(const char& letter)
{
	switch (letter)
    {
		case 'a':	return ImNote::A;
		case 'b':	return ImNote::B;
		case 'c':	return ImNote::C;
		case 'd':	return ImNote::D;
		case 'e':	return ImNote::E;
		case 'f':	return ImNote::F;
		case 'g':	return ImNote::G;
	}
	return -1;
}

int to_octave(const char& letter)
{
	switch (letter)
    {
		case '0':	return 0;
		case '1':	return 1;
		case '2':	return 2;
		case '3':	return 3;
		case '4':	return 4;
		case '5':	return 5;
		case '6':	return 6;
		case '7':	return 7;
		case '8':	return 8;
		case '9':	return 9;
	}
	return -1;
}

int to_accidentals(const std::string& accidentals)
{
    switch (accidentals.length())
    {
        case 0:
            return ImNote::NoAccidentals;
            break;

        case 1:
            if (accidentals[0] == '+')
                return ImNote::Sharp;
            else if (accidentals[0] == '-')
                return ImNote::Flat;
            else if (accidentals[0] == '=')
                return ImNote::Natural;
            else if (accidentals[0] == 'x')
                return ImNote::DoubleSharp;
            else
                return -1;
            break;

        case 2:
            if (accidentals.compare(0, 2, "++"))
                return ImNote::SharpSharp;
            else if (accidentals.compare(0, 2, "--"))
                return ImNote::FlatFlat;
            else if (accidentals.compare(0, 2, "=-"))
                return ImNote::NaturalFlat;
            else
                return -1;
            break;

        default:
            return -1;
    }
}

int to_note_type(const char& letter)
{
    //  USA           UK                      ESP               LDP     NoteType
    //  -----------   --------------------    -------------     ---     ---------
    //  long          longa                   longa             l       k_longa = 0
    //  double whole  breve                   cuadrada, breve   b       k_breve = 1
    //  whole         semibreve               redonda           w       k_whole = 2
    //  half          minim                   blanca            h       k_half = 3
    //  quarter       crochet                 negra             q       k_quarter = 4
    //  eighth        quaver                  corchea           e       k_eighth = 5
    //  sixteenth     semiquaver              semicorchea       s       k_16th = 6
    //  32nd          demisemiquaver          fusa              t       k_32th = 7
    //  64th          hemidemisemiquaver      semifusa          i       k_64th = 8
    //  128th         semihemidemisemiquaver  garrapatea        o       k_128th = 9
    //  256th         ???                     semigarrapatea    f       k_256th = 10

    switch (letter)
    {
        case 'l':     return ImNote::k_longa;
        case 'b':     return ImNote::k_breve;
        case 'w':     return ImNote::k_whole;
        case 'h':     return ImNote::k_half;
        case 'q':     return ImNote::k_quarter;
        case 'e':     return ImNote::k_eighth;
        case 's':     return ImNote::k_16th;
        case 't':     return ImNote::k_32th;
        case 'i':     return ImNote::k_64th;
        case 'o':     return ImNote::k_128th;
        case 'f':     return ImNote::k_256th;
        default:
            return -1;
    }
}

bool ldp_pitch_to_components(const string& pitch, int *step, int* octave, int* accidentals)
{
    //    Analyzes string pitch (LDP format), extracts its parts (step, octave and
    //    accidentals) and stores them in the corresponding parameters.
    //    Returns true if error (pitch is not a valid pitch name)
    //
    //    In LDP pitch is represented as a combination of the step of the diatonic scale, the
    //    chromatic alteration, and the octave.
    //      - The accidentals parameter represents chromatic alteration (does not include tonal
    //        key alterations)
    //      - The octave element is represented by the numbers 0 to 9, where 4 indicates
    //        the octave started by middle C.
    //
    //    pitch must be trimed (no spaces before or after real data) and lower case

    size_t i = pitch.length() - 1;
    if (i < 1) 
        return true;   //error

    *octave = to_octave(pitch[i--]);
    if (*step == -1)
        return true;   //error

    *step = to_step(pitch[i--]);
    if (*step == -1)
        return true;   //error

    if (++i == 0)
    {
        *accidentals = ImNote::NoAccidentals;
        return false;   //no error
    }
    else
        *accidentals = to_accidentals(pitch.substr(0, i));
    if (*accidentals == -1)
        return true;   //error

    return false;  //no error
}

bool ldp_duration_to_components(const string& duration, int* noteType, int* dots)
{
    // Receives a string (duration) with the LDP letter for the type of note and, 
    // optionally, dots "."
    // Set up variables noteType and dots, and returns true if error in parsing

    size_t size = duration.length();
    if (size == 0) 
        return true;   //error

    //duration
    *noteType = to_note_type(duration[0]);
    if (*noteType == -1)
        return true;   //error

    //dots
    *dots = 0;
    for (size_t i=1; i < size; i++)
    {
        if (duration[i] == '.')
            (*dots)++;
        else
            return true;    //error
    }

    return false;   //no error
}

float to_duration(int nNoteType, int nDots)
{
    //compute duration without modifiers
    float rDuration = pow(2.0f, (10 - nNoteType));

    //take dots into account
    switch (nDots)
    {
        case 0:                             break;
        case 1: rDuration *= 1.5f;          break;
        case 2: rDuration *= 1.75f;         break;
        case 3: rDuration *= 1.875f;        break;
        case 4: rDuration *= 1.9375f;       break;
        case 5: rDuration *= 1.96875f;      break;
        case 6: rDuration *= 1.984375f;     break;
        case 7: rDuration *= 1.9921875f;    break;
        case 8: rDuration *= 1.99609375f;   break;
        case 9: rDuration *= 1.998046875f;  break;
        default:
            ;
            //wxLogMessage(_T("[to_duration] Program limit: do you really need more than nine dots?"));
    }

    return rDuration;
}


}  //namespace lenmus
