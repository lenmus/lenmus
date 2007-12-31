//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street,
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LM_COMPOSERV5_H__        //to avoid nested includes
#define __LM_COMPOSERV5_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ComposerV5.cpp"
#endif

#include "Composer.h"

class lmComposer5 : lmComposer
{
public:
    lmComposer5();
    ~lmComposer5();

    lmScore* GenerateScore(lmScoreConstrains* pConstrains);

private:
    void            ChooseRangeOfNotes();
    wxString        GenerateNewNote(bool fRepeat, bool fRootPitch);

    void AddSegment(wxString* pMeasure, lmSegmentEntry* pSegment, float rNoteTime);
    wxString CreateNoteRest(int nNoteRestDuration, bool fNote);
    wxString CreateNote(int nNoteDuration) { return CreateNoteRest(nNoteDuration, true); }
    wxString CreateRest(int nRestDuration) { return CreateNoteRest(nRestDuration, false); }
    wxString CreateLastMeasure(int nNumMeasure, lmETimeSignature nTimeSign, bool fOnlyQuarterNotes);

    // pitch related methods
    wxString    InstantiateNotes(wxString sBeats, bool fRootPtich=false);
    lmDPitch    RootNote(lmEKeySignatures nKey);




    lmETimeSignature      m_nTimeSign;
    lmEClefType           m_nClef;
    lmEKeySignatures      m_nKey;
    lmScoreConstrains*  m_pConstrains;

    //variables to control note pitch generation
    int         m_nNumNotes;                // num of notes generated
    lmDPitch    m_minPitch, m_maxPitch;     // range of notes to generate
    lmDPitch    m_lastPitch;                // last note pitch
    bool        m_fTied;                    // next note tied to m_lastPitch


};



#endif    // __LM_COMPOSERV5_H__



