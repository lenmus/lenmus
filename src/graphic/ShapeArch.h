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

#ifndef __LM_SHAPEARCH_H__        //to avoid nested includes
#define __LM_SHAPEARCH_H__

#ifdef __GNUG__
#pragma interface "ShapeArch.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/defs.h"      // lmLUnits
#include "GMObject.h"

class lmPaper;
class lmScoreObj;
class lmStaff;
class lmStaffObj;


class lmShapeArch : public lmSimpleShape
{
public:
    lmShapeArch(lmScoreObj* pOwner, lmUPoint uStart, lmUPoint uEnd,
                lmUPoint uCtrol1, lmUPoint uCtrol2, wxColour nColor,
                wxString sName = _T("Arch"), bool fDraggable = true,
				bool fVisible = true);
    lmShapeArch(lmScoreObj* pOwner, lmUPoint uStart, lmUPoint uEnd, bool fArchUnder,
                wxColour nColor, wxString sName = _T("Arch"),
				bool fDraggable = true, bool fVisible = true);
    lmShapeArch(lmScoreObj* pOwner, bool fArchUnder, wxColour nColor,
                wxString sName = _T("Arch"), bool fDraggable = true,
				bool fVisible = true);

    virtual ~lmShapeArch() {}

    //implementation of virtual methods from base class
    virtual void Render(lmPaper* pPaper, wxColour color = *wxBLACK);
    virtual wxString Dump(int nIndent);
    virtual void Shift(lmLUnits xIncr, lmLUnits yIncr);

    //creation
    void SetStartPoint(lmLUnits xPos, lmLUnits yPos);
    void SetEndPoint(lmLUnits xPos, lmLUnits yPos);
    void SetCtrolPoint1(lmLUnits xPos, lmLUnits yPos);
    void SetCtrolPoint2(lmLUnits xPos, lmLUnits yPos);

    //access to information
    inline lmLUnits GetStartPosX() const { return m_uStart.x; }
    inline lmLUnits GetStartPosY() const { return m_uStart.y; }
    inline lmLUnits GetEndPosX() const { return m_uEnd.x; }
    inline lmLUnits GetEndPosY() const { return m_uEnd.y; }
    inline lmLUnits GetCtrol1PosX() const { return m_uCtrol1.x; }
    inline lmLUnits GetCtrol1PosY() const { return m_uCtrol1.y; }
    inline lmLUnits GetCtrol2PosX() const { return m_uCtrol2.x; }
    inline lmLUnits GetCtrol2PosY() const { return m_uCtrol2.y; }



protected:
    void SetDefaultControlPoints();
    void Create();
    void CubicBezier(double* x, double* y, int nNumPoints);

    // start, end and control points coordinates, absolute paper position
    lmUPoint        m_uStart;
    lmUPoint        m_uEnd;
    lmUPoint        m_uCtrol1;
    lmUPoint        m_uCtrol2;
    bool            m_fArchUnder;       //arch under notes (like an U)

};


class lmNote;
class lmShape;
class lmShapeNote;
class lmStaff;

class lmShapeTie : public lmShapeArch
{
public:
    lmShapeTie(lmNote* pOwner, lmShapeNote* pShapeStart, lmShapeNote* pShapeEnd,
                bool fTieUnderNote, wxColour color = *wxBLACK,
				bool fVisible = true);
	~lmShapeTie();

	//implementation of virtual methods in base class
    void Render(lmPaper* pPaper, wxColour color = *wxBLACK);

	//layout changes
	void OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag,
								lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent);

	//splitting
	void SetBrotherTie(lmShapeTie* pBrotherTie) { m_pBrotherTie = pBrotherTie; }

private:
    bool			m_fTieUnderNote;
	lmShapeTie*		m_pBrotherTie;		//when tie is splitted

};


#endif    // __LM_SHAPEARCH_H__

