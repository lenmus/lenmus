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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "NotesConstrains.h"
#endif

// For compilers that support precompilation, includes <wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "NotesConstrains.h"
#include "Generators.h"

// the config object
extern wxConfigBase *g_pPrefs;


lmNotesConstrains::lmNotesConstrains(wxString sSection)
    : lmExerciseOptions(sSection)
{
    //
    // default settings
    //

    //clef. Default: G
    m_nClef = lmE_Sol;

    //For settings dlg: how are notes selected. Default: from C major scale
    m_fFromKeySignature = true;
    m_nKeySignature = earmDo;

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

    // how to start exercise. Default: play all valid notes
    m_fStartWithNotes = true;

    //how many octaves. Default 1
    m_nOctaves = 1;
}

void lmNotesConstrains::SaveSettings()
{
    //
    // save settings in user configuration data file
    //

    //clef to use
    wxString sKey;
    sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/Clef"), m_sSection.c_str());
    g_pPrefs->Write(sKey, m_nClef);

    //For settings dlg: how are notes selected
    sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/DlgNotesFromKeySignature"),
        m_sSection.c_str());
    g_pPrefs->Write(sKey, m_fFromKeySignature);
    sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/KeySignature"), m_sSection.c_str());
    g_pPrefs->Write(sKey, m_nKeySignature);

    // valid notes
    for (int i=0; i < 12; i++)
    {
        sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/Note%dAllowed"),
            m_sSection.c_str(), i );
        g_pPrefs->Write(sKey, m_fValidNotes[i]);
    }

    // how to start exercise
    sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/StartMode"), m_sSection.c_str());
    g_pPrefs->Write(sKey, m_fStartWithNotes);

    //how many octaves
    sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/Octaves"), m_sSection.c_str());
    g_pPrefs->Write(sKey, m_nOctaves);
}

void lmNotesConstrains::LoadSettings()
{
    //
    // load settings form user configuration data or default values
    //

    //clef to use. Default: G
    wxString sKey;
    sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/Clef"), m_sSection.c_str());
    long nClef; 
    g_pPrefs->Read(sKey, &nClef, static_cast<long>(lmE_Sol));
    m_nClef = static_cast<lmEClefType>(nClef);

    //For settings dlg: how are notes selected. Default: from C major scale
    sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/DlgNotesFromKeySignature"),
        m_sSection.c_str());
    g_pPrefs->Read(sKey, &m_fFromKeySignature, true);
    sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/KeySignature"), m_sSection.c_str());
    long nKey;
    g_pPrefs->Read(sKey, &nKey, static_cast<long>(earmDo));
    m_nKeySignature = static_cast<lmEKeySignatures>(nKey);

    // Valid notes. Default: C major natural notes
    bool fDefault[12] = {true, false, true, false, true, true,
                         false, true, false, true, false, true };
    for (int i=0; i < 12; i++)
    {
        sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/Note%dAllowed"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &m_fValidNotes[i], fDefault[i]);
    }

    // how to start exercise. . Default: play all valid notes
    sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/StartMode"), m_sSection.c_str());
    g_pPrefs->Read(sKey, &m_fStartWithNotes, true);

    //how many octaves. Default 1
    sKey = wxString::Format(_T("/Constrains/IdfyNotes/%s/Octaves"), m_sSection.c_str());
    long nOctaves;
    g_pPrefs->Read(sKey, &nOctaves, 1);
    m_nOctaves = nOctaves;
}

int lmNotesConstrains::GetRandomNoteIndex()
{
    lmRandomGenerator oGenerator;
    int nWatchDog = 0;
    int note = oGenerator.RandomNumber(0, 11);
    while (!IsValidNote(note))
    {
        note = oGenerator.RandomNumber(0, 11);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmNotesConstrains::GetRandomNoteIndex."));
            return 0;
        }
    }
    return note;
}

