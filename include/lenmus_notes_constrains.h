//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_NOTES_CONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_NOTES_CONSTRAINS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_generators.h"
#include "lenmus_constrains.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

class NotesConstrains : public ExerciseOptions
{
public:
    NotesConstrains(wxString sSection, ApplicationScope& appScope);
    ~NotesConstrains() {}

    int GetRandomNoteIndex();
    inline bool IsValidNote(int n) { return m_fValidNotes[n]; }
    inline void SetValidNote(int n, bool fValue) { m_fValidNotes[n] = fValue; }
    inline bool SelectNotesFromKeySignature() { return m_fFromKeySignature; }
    inline void SetSelectNotesFromKeySignature(bool value) { m_fFromKeySignature = value; }
    inline EKeySignature GetKeySignature() { return m_nKeySignature; }
    inline void SetKeySignature(EKeySignature key) { m_nKeySignature = key; }

    inline EClef GetClef() { return m_nClef; }
    inline void SetClef(EClef nClef) { m_nClef = nClef; }

    inline int GetOctaves() { return m_nOctaves; }
    inline void SetOctaves(int nOctaves) { m_nOctaves = nOctaves; }

    void SetSection(wxString sSection) {
                m_sSection = sSection;
                load_settings();
            }

    void save_settings();




private:
    void load_settings();

    bool                m_fValidNotes[12];
    bool                m_fFromKeySignature;
    EKeySignature    m_nKeySignature;
    EClef         m_nClef;
    int                 m_nOctaves;

};


}   // namespace lenmus

#endif  // __LENMUS_NOTES_CONSTRAINS_H__
