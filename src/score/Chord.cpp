// RCS-ID: $Id: Chord.cpp,v 1.3 2006/02/23 19:22:56 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file Chord.cpp
    @brief Implementation file for class lmChord
    @ingroup score_kernel
*/
/*! @class lmChord 
    @ingroup score_kernel
    @brief Information and methods to group several notes into a chord.

    Contains the information needed to group several notes into a chord.
    For each chord there exist a lmChord object and a pointer to it is stored on each
    note of the chord

    The class is named lmChord instead of Chord because there is a function named "Chord"
    defined in WinGDI.h (Microsoft), and it causes compilation problems (with MS VC) 
    as the names collide.

*/

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"

/*! Creates the chord object, with only the base note.
*/
lmChord::lmChord(lmNote* pBaseNote)
{
    m_cNotes.Append(pBaseNote);

    // as this is the only note it is the max and the min one
    m_pMinNote = pBaseNote;
    m_pMaxNote = pBaseNote;
}

/*! Destructor. When invoked, only from lmNote destructor, there must be only one 
    note: the base note.
*/
lmChord::~lmChord()
{
    wxASSERT(m_cNotes.GetCount() == 1);

    wxNotesListNode *pNode = m_cNotes.GetFirst();
    lmNote* pNote = (lmNote*)pNode->GetData();
    pNote->ClearChordInformation();

    m_cNotes.DeleteObject(pNote);

    //notes will not be deleted when deleting the list, as they are part of a lmScore
    // and will be deled there.
    m_cNotes.DeleteContents(false);
    m_cNotes.Clear();
}

/*!  @brief Add a note to the chord.
*/
void lmChord::AddNote(lmNote* pNote)
{
    m_cNotes.Append(pNote);
    
    //Update Max and Min note 
    if (m_pMinNote->GetPitch() > pNote->GetPitch()) {
        m_pMinNote = pNote;
    } else if (m_pMaxNote->GetPitch() < pNote->GetPitch()) {
        m_pMaxNote = pNote;
    }
    
}

/*! @brief Removes a note from a chord.
    
    When invoked there must be at least two notes as 
    otherwise it can not be a chord. 
*/
void lmChord::RemoveNote(lmNote* pNote)
{
    wxASSERT(m_cNotes.GetCount() > 1);

    m_cNotes.DeleteObject(pNote);

    //Update Max and Min note 
    wxNotesListNode *pNode = m_cNotes.GetFirst();
    pNote = (lmNote*)pNode->GetData();
    m_pMinNote = pNote;
    m_pMaxNote = pNote;

    pNode=pNode->GetNext();
    for(; pNode; pNode=pNode->GetNext()) 
    {
        pNote = (lmNote*)pNode->GetData();
        if (m_pMinNote->GetPitch() > pNote->GetPitch()) {
            m_pMinNote = pNote;
        } else if (m_pMaxNote->GetPitch() < pNote->GetPitch()) {
            m_pMaxNote = pNote;
        }
    }

}

lmNote* lmChord::GetBaseNote()
{
    return (lmNote*)(m_cNotes.GetFirst())->GetData();
}

int lmChord::GetNumNotes()
{ 
    return (int)m_cNotes.GetCount(); 
}

/*!    @brief Draw the stem of the chord.

    Once the last notehead of a chord has been drawn this method is invoked (from the
    last lines of lmNote::DrawObject() ) to draw the stem of the chord.
    The stem position is stored in the base note.
*/
void lmChord::DrawStem(wxDC* pDC)
{
    wxASSERT(pDC);

    lmNote* pBaseNote = (lmNote*)(m_cNotes.GetFirst())->GetData();
    lmMicrons xStem = pBaseNote->GetXStem();
    lmMicrons yStemStart=0, yStemEnd=0;
    
    if (pBaseNote->StemGoesDown()) {
        //stem down: line at left of noteheads
        if (pBaseNote->IsBeamed()) {
            // chord beamed: use base note information
            yStemStart = m_pMaxNote->GetYStem();
            yStemEnd = yStemStart + pBaseNote->GetStemLength();
        }
        else {
            // chord not beamed. Use max and min notes information
            yStemStart = m_pMaxNote->GetYStem();
            yStemEnd = m_pMinNote->GetFinalYStem();
        }
    } else {
        //stem up: line at right of noteheads
        if (pBaseNote->IsBeamed()) {
            // chord beamed: use base note information
            yStemStart = m_pMinNote->GetYStem();
            yStemEnd = yStemStart - pBaseNote->GetStemLength();
        }
        else {
            // chord not beamed. Use max and min notes information
            yStemStart = m_pMinNote->GetYStem();
            yStemEnd = m_pMaxNote->GetFinalYStem();
        }
    }
     pDC->DrawLine(xStem, yStemStart, xStem, yStemEnd);

    if (!pBaseNote->IsBeamed() && pBaseNote->GetType() > eQuarter) {
        //! @todo Draw the stem for chords not beamed
        //pPaper->PintarCorchete fMeasuring, pBaseNote.Tipo, pBaseNote->StemGoesDown(), xStem, yStemEnd)
    }
    
}

bool lmChord::IsLastNoteOfChord(lmNote* pNote)
{
    wxNotesListNode *pNode = m_cNotes.GetLast();
    lmNote* pLastNote = (lmNote*)pNode->GetData();
    return (pLastNote->GetID() == pNote->GetID());
    
}

//void lmChord::Duracion() As Long
//    //Devuelve la duracion del acorde (por ahora, la de la nota base)
//    Duracion = m_cNotes(1).Duracion
//    
//}
