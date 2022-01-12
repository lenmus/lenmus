//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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

//lenmus
#include "lenmus_notes_constrains.h"

#include "lenmus_generators.h"
#include "lenmus_constrains.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;


namespace lenmus
{

NotesConstrains::NotesConstrains(wxString sSection, ApplicationScope& appScope)
    : ExerciseOptions(sSection, appScope)
{
    //
    // default settings
    //

    //clef. Default: G
    m_nClef = k_clef_G2;

    //For settings dlg: how are notes selected. Default: from C major scale
    m_fFromKeySignature = true;
    m_nKeySignature = k_key_C;

    // Valid notes. Default: C major natural notes
    m_fValidNotes[0] = true;    // c
    m_fValidNotes[1] = false;   // +c
    m_fValidNotes[2] = true;    // d
    m_fValidNotes[3] = false;   // +d
    m_fValidNotes[4] = true;    // e
    m_fValidNotes[5] = true;    // f
    m_fValidNotes[6] = false;   // +f
    m_fValidNotes[7] = true;    // g
    m_fValidNotes[8] = false;   // +g
    m_fValidNotes[9] = true;    // a
    m_fValidNotes[10] = false;  // +a
    m_fValidNotes[11] = true;   // b

    //how many octaves. Default 1
    m_nOctaves = 1;
}

//---------------------------------------------------------------------------------------
void NotesConstrains::save_settings()
{
    //save settings in user configuration data file

    if (m_appScope.get_exercises_level() != 100)
        return;

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    //clef to use
    wxString sKey;
    sKey = wxString::Format("/Constrains/IdfyNotes/%s/Clef", GetSection());
    pPrefs->Write(sKey, int(m_nClef));

    //For settings dlg: how are notes selected
    sKey = wxString::Format("/Constrains/IdfyNotes/%s/DlgNotesFromKeySignature",
        GetSection());
    pPrefs->Write(sKey, m_fFromKeySignature);
    sKey = wxString::Format("/Constrains/IdfyNotes/%s/KeySignature", GetSection());
    pPrefs->Write(sKey, int(m_nKeySignature));

    // valid notes
    for (int i=0; i < 12; i++)
    {
        sKey = wxString::Format("/Constrains/IdfyNotes/%s/Note%dAllowed",
            GetSection(), i );
        pPrefs->Write(sKey, m_fValidNotes[i]);
    }

    //how many octaves
    sKey = wxString::Format("/Constrains/IdfyNotes/%s/Octaves", GetSection());
    pPrefs->Write(sKey, m_nOctaves);
}

//---------------------------------------------------------------------------------------
void NotesConstrains::load_settings()
{
    // load settings form user configuration data or default values

    switch (m_appScope.get_exercises_level())
    {
        case 1: load_settings_for_level_1();        break;
        case 2: load_settings_for_level_2();        break;
        default:
            load_settings_for_customized_level();
    }
}

//---------------------------------------------------------------------------------------
void NotesConstrains::load_settings_for_customized_level()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    //clef to use. Default: G
    wxString sKey;
    sKey = wxString::Format("/Constrains/IdfyNotes/%s/Clef", GetSection());
    long nClef;
    pPrefs->Read(sKey, &nClef, static_cast<long>(k_clef_G2));
    m_nClef = static_cast<EClef>(nClef);

    //For settings dlg: how are notes selected. Default: from C major scale
    sKey = wxString::Format("/Constrains/IdfyNotes/%s/DlgNotesFromKeySignature",
        GetSection());
    pPrefs->Read(sKey, &m_fFromKeySignature, true);
    sKey = wxString::Format("/Constrains/IdfyNotes/%s/KeySignature", GetSection());
    long nKey;
    pPrefs->Read(sKey, &nKey, static_cast<long>(k_key_C));
    m_nKeySignature = static_cast<EKeySignature>(nKey);

    // Valid notes. Default: C major natural notes
    bool fDefault[12] = {true, false, true, false, true, true,
                         false, true, false, true, false, true };
    for (int i=0; i < 12; i++)
    {
        sKey = wxString::Format("/Constrains/IdfyNotes/%s/Note%dAllowed",
            GetSection(), i );
        pPrefs->Read(sKey, &m_fValidNotes[i], fDefault[i]);
    }

    //how many octaves. Default 1
    sKey = wxString::Format("/Constrains/IdfyNotes/%s/Octaves", GetSection());
    long nOctaves;
    pPrefs->Read(sKey, &nOctaves, 1);
    m_nOctaves = nOctaves;
}

//---------------------------------------------------------------------------------------
void NotesConstrains::load_settings_for_level_1()
{
    //clef to use. Default: G
    m_nClef = k_clef_G2;

    //Select notes from C major scale
    m_nKeySignature = k_key_C;

    // Valid notes. Default: C major natural notes
    m_fValidNotes[0] = true;    // c
    m_fValidNotes[1] = false;   // +c
    m_fValidNotes[2] = true;    // d
    m_fValidNotes[3] = false;   // +d
    m_fValidNotes[4] = true;    // e
    m_fValidNotes[5] = true;    // f
    m_fValidNotes[6] = false;   // +f
    m_fValidNotes[7] = true;    // g
    m_fValidNotes[8] = false;   // +g
    m_fValidNotes[9] = true;    // a
    m_fValidNotes[10] = false;  // +a
    m_fValidNotes[11] = true;   // b

    //how many octaves. Default 1
    m_nOctaves = 1;

    SetSettingsLink(false);
}

//---------------------------------------------------------------------------------------
void NotesConstrains::load_settings_for_level_2()
{
    //clef to use: F4
    m_nClef = k_clef_F4;

    //Select notes from D major scale
    m_nKeySignature = k_key_a;

    // Valid notes A minor natural notes
    m_fValidNotes[0] = true;    // c
    m_fValidNotes[1] = false;   // +c
    m_fValidNotes[2] = true;    // d
    m_fValidNotes[3] = false;   // +d
    m_fValidNotes[4] = true;    // e
    m_fValidNotes[5] = true;    // f
    m_fValidNotes[6] = false;   // +f
    m_fValidNotes[7] = true;    // g
    m_fValidNotes[8] = false;   // +g
    m_fValidNotes[9] = true;    // a
    m_fValidNotes[10] = false;  // +a
    m_fValidNotes[11] = true;   // b

    //how many octaves: 2
    m_nOctaves = 2;

    SetSettingsLink(false);
}

//---------------------------------------------------------------------------------------
int NotesConstrains::GetRandomNoteIndex()
{
    RandomGenerator oGenerator;
    int nWatchDog = 0;
    static int prevNote = 0;
    int note = oGenerator.random_number(0, 11);
    while (!IsValidNote(note) || note == prevNote)
    {
        note = oGenerator.random_number(0, 11);
        if (nWatchDog++ == 1000) {
            LOMSE_LOG_ERROR("Program error: Loop detected");
            return 0;
        }
    }
    prevNote = note;
    return note;
}


}   // namespace lenmus
