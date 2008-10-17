//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "Slur.h"
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
#include "../graphic/GMObject.h"
#include "../graphic/ShapeNote.h"
#include "../graphic/ShapeArch.h"


//---------------------------------------------------------
//   lmTie
//---------------------------------------------------------

lmTie::lmTie(lmNote* pStartNote, lmNote* pEndNote)
{
    m_pStartNote = pStartNote;
    m_pEndNote   = pEndNote;
}

lmTie::~lmTie()
{
}

lmShape* lmTie::LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour color)
{
	// Create two arch shapes, one onwed by start note and the other one by 
	// end note.
	// Both notes' shapes will have attached both tie shapes.
	// One of the tie shapes will be invisible

    //prepare information
    lmShapeNote* pShapeStart = (lmShapeNote*)m_pStartNote->GetShape();
    lmShapeNote* pShapeEnd = (lmShapeNote*)m_pEndNote->GetShape();
    bool fTieUnderNote = !m_pStartNote->StemGoesDown();

	//create the first tie shape, attached to both notes' shapes
    lmShapeTie* pShape1 = new lmShapeTie(m_pEndNote, pShapeStart, pShapeEnd,
                                        fTieUnderNote, color, lmVISIBLE);
	pBox->AddShape(pShape1);
	pShapeStart->Attach(pShape1, eGMA_StartNote);
	pShapeEnd->Attach(pShape1, eGMA_EndNote);

	//create the second tie shape, attached to both notes' shapes
    lmShapeTie* pShape2 = new lmShapeTie(m_pStartNote, pShapeStart, pShapeEnd,
                                        fTieUnderNote, color, lmNO_VISIBLE);
	pBox->AddShape(pShape2);
	pShapeStart->Attach(pShape2, eGMA_StartNote);
	pShapeEnd->Attach(pShape2, eGMA_EndNote);

	//link both ties
	pShape1->SetBrotherTie(pShape2);
	pShape2->SetBrotherTie(pShape1);

	//return the visible shape
	return pShape1;
}

void lmTie::Remove(lmNote* pNote)
{
    if (m_pStartNote == pNote) {
        m_pStartNote = (lmNote*)NULL;
        m_pEndNote->RemoveTie(this);
        m_pEndNote = (lmNote*)NULL;
    }
    else if (m_pEndNote == pNote) {
        m_pEndNote = (lmNote*)NULL;
        m_pStartNote->RemoveTie(this);
        m_pStartNote = (lmNote*)NULL;
    }
}

void lmTie::PropagateNotePitchChange(lmNote* pNote, int nStep, int nOctave, int nAlter, bool fForward)
{
    if (pNote == m_pStartNote && fForward) {
        // propagate forwards
        m_pEndNote->PropagateNotePitchChange(nStep, nOctave, nAlter, lmFORWARDS);
    }
    else if (pNote == m_pEndNote && !fForward) {
        // propagate backwards
        m_pStartNote->PropagateNotePitchChange(nStep, nOctave, nAlter, lmBACKWARDS);
    }
    //other cases are for notes whose pitch is already changed 
}

