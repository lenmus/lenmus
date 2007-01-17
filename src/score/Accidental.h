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
/*! @file Accidental.h
    @brief Header file for class lmAccidental
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __ACCIDENTAL_H__        //to avoid nested includes
#define __ACCIDENTAL_H__

#include "../graphic/Shape.h"


/*
    An lmAccidental represents the accidental sign associated to notes.
<!ATTLIST accidental
    cautionary %yes-no; #IMPLIED
    editorial %yes-no; #IMPLIED
    size %symbol-size; #IMPLIED
    %position;
>


*/

class lmAccidental : public lmNoteRestObj
{
public:
    lmAccidental(lmNoteRest* pOwner, EAccidentals nType);
    ~lmAccidental();

    // overrides for pure virtual methods of base class lmNoteRestObj
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight);
    void SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
                         lmLUnits xPos, lmLUnits yPos);

    lmLUnits GetWidth();
    void SetAfterspace(lmLUnits nSpace) { m_nAfterSpace = nSpace; }

    void Measure(lmPaper* pPaper, lmStaff* pStaff, lmUPoint shift);
    void Render(lmPaper* pPaper, lmUPoint pos, wxColour color = *wxBLACK);


private:
    void CreateShapes();

    EAccidentals    m_nType;            //accidental type
    lmLUnits        m_nAfterSpace;

    lmShapeGlyph*   m_pShape[2];        //the shapes to render the accidental


};

#endif    // __ACCIDENTAL_H__

