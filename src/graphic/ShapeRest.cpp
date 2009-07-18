//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
#pragma implementation "ShapeRest.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "ShapeRest.h"
#include "ShapeBeam.h"
#include "../score/Score.h"
#include "../app/ScoreCanvas.h"



//-------------------------------------------------------------------------------------
// Implementation of lmShapeRest


lmShapeRest::lmShapeRest(lmScoreObj* pOwner, wxColour color, bool fDraggable, bool fVisible,
                         lmEGMOType nType, wxString sName)
	: lmCompositeShape(pOwner, 0, color, sName, fDraggable, nType, fVisible)
{
    m_pBeamShape = (lmShapeBeam*)NULL;
}

lmShapeRest::~lmShapeRest()
{
}

void lmShapeRest::Render(lmPaper* pPaper, wxColour color)
{
    //base class method is overrided to deal with rests inside a beamed group.
    //The beam and the stems are rendered *after* noteheads and rests are rendered.
    //Therefore, when rendering the beam there is no option to adjust rests positions
    //to fit gracefuly inside the beamed group. 
    //By overriding this method, if the rest is inside a beamed group and it is
    //the first rest in that beamed group, will force the beam shape to compute stems,
    //and, therefore, to adjust all rests' positions.


    //if the rest is inside of a beamed group ensure that beam is layouted
    if (m_pBeamShape)
        m_pBeamShape->AdjustStemsIfNeeded();

    //now, we can safely render the rest
    lmCompositeShape::Render(pPaper, color);
}
