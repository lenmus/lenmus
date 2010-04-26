//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
//-------------------------------------------------------------------------------------

#ifndef __LM_NOTESCONSTRAINS_H__        //to avoid nested includes
#define __LM_NOTESCONSTRAINS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "NotesConstrains.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Generators.h"
#include "Constrains.h"


class lmNotesConstrains : public lmExerciseOptions
{
public:
    lmNotesConstrains(wxString sSection);
    ~lmNotesConstrains() {}

    int GetRandomNoteIndex();
    inline bool IsValidNote(int n) { return m_fValidNotes[n]; }
    inline void SetValidNote(int n, bool fValue) { m_fValidNotes[n] = fValue; }
    inline bool SelectNotesFromKeySignature() { return m_fFromKeySignature; }
    inline void SetSelectNotesFromKeySignature(bool value) { m_fFromKeySignature = value; }
    inline lmEKeySignatures GetKeySignature() { return m_nKeySignature; }
    inline void SetKeySignature(lmEKeySignatures key) { m_nKeySignature = key; }

    inline lmEClefType GetClef() { return m_nClef; }
    inline void SetClef(lmEClefType nClef) { m_nClef = nClef; }

    inline bool StartWithNotes() { return m_fStartWithNotes; }
    inline bool StartWithA4() { return !m_fStartWithNotes; }
    inline void SetStartWithNotes(bool value) { m_fStartWithNotes = value; }

    inline int GetOctaves() { return m_nOctaves; }
    inline void SetOctaves(int nOctaves) { m_nOctaves = nOctaves; }

    void SetSection(wxString sSection) {
                m_sSection = sSection;
                LoadSettings();
            }

    void SaveSettings();
    



private:
    void LoadSettings();

    bool                m_fValidNotes[12];
    bool                m_fFromKeySignature;
    lmEKeySignatures    m_nKeySignature;
    lmEClefType         m_nClef;
    bool                m_fStartWithNotes;      // true=play notes, false=play A4 note
    int                 m_nOctaves;

};

#endif  // __LM_NOTESCONSTRAINS_H__
