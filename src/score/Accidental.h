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

#ifndef __LM_ACCIDENTAL_H__        //to avoid nested includes
#define __LM_ACCIDENTAL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Accidental.cpp"
#endif

#include "../graphic/Shapes.h"


// Helper class:
//		An lmAccidental represents an accidental sign associated to a note

class lmAccidental
{
public:
    lmAccidental(lmNote* pOwner, lmEAccidentals nType);
    ~lmAccidental();

    lmLUnits GetWidth();
    //void SetAfterspace(lmLUnits nSpace) { m_uAfterSpace = nSpace; }
	inline lmEAccidentals GetType() { return m_nType; }
	wxString GetLDPEncoding();

    void Layout(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyPos);
	void MoveTo(lmLUnits uxPos, lmLUnits uyPos);
	lmShape* GetShape() { return m_pShape; }


private:
    void CreateShapes(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyPos);

	lmNote*		    m_pOwner;
    lmEAccidentals    m_nType;            //accidental type
	lmStaff*		m_pStaff;			 
    lmLUnits        m_uAfterSpace;
	lmLUnits        m_uxPos;
	lmLUnits        m_uyPos;
    lmShape*		m_pShape;			//the shape to render the accidentals


};

#endif    // __LM_ACCIDENTAL_H__

