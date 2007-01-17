//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __COMPOSERV5_H__        //to avoid nested includes
#define __COMPOSERV5_H__

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
    wxString        InstantiateNotes(wxString sBeats, bool fRootPtich=false);
    lmPitch         RootNote(EKeySignatures nKey);

    void AddSegment(wxString* pMeasure, lmSegmentEntry* pSegment, float rNoteTime);
    wxString CreateNoteRest(int nNoteRestDuration, bool fNote);
    wxString CreateNote(int nNoteDuration) { return CreateNoteRest(nNoteDuration, true); }
    wxString CreateRest(int nRestDuration) { return CreateNoteRest(nRestDuration, false); }
    wxString CreateLastMeasure(int nNumMeasure, ETimeSignature nTimeSign);


    ETimeSignature      m_nTimeSign;
    EClefType           m_nClef;
    EKeySignatures      m_nKey;
    lmScoreConstrains*  m_pConstrains;

    //variables to control note pitch generation
    int         m_nNumNotes;                // num of notes generated
    lmPitch     m_minPitch, m_maxPitch;     // range of notes to generate
    lmPitch     m_lastPitch;                // last note pitch
    bool        m_fTied;                    // next note tied to m_lastPitch


};



#endif    // __COMPOSERV5_H__



