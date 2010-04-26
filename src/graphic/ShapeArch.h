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

#ifndef __LM_SHAPEARCH_H__        //to avoid nested includes
#define __LM_SHAPEARCH_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ShapeArch.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "../score/defs.h"      // lmLUnits
#include "GMObject.h"

class lmPaper;
class lmScoreObj;
class lmStaff;
class lmStaffObj;
class lmHandlerSquare;
class lmHandlerLine;


class lmShapeArch : public lmSimpleShape
{
public:
    lmShapeArch(lmScoreObj* pOwner, int nShapeIdx, lmUPoint uStart, lmUPoint uEnd,
                lmUPoint uCtrol1, lmUPoint uCtrol2, wxColour nColor,
                wxString sName = _T("Arch"), bool fDraggable = true,
				bool fVisible = true);
    lmShapeArch(lmScoreObj* pOwner, int nShapeIdx, lmUPoint uStart, lmUPoint uEnd,
                bool fArchUnder, wxColour nColor, wxString sName = _T("Arch"),
				bool fDraggable = true, bool fVisible = true);
    lmShapeArch(lmScoreObj* pOwner, int nShapeIdx, bool fArchUnder,
                wxColour nColor, wxString sName = _T("Arch"), bool fDraggable = true,
				bool fVisible = true);

    virtual ~lmShapeArch();

    //implementation of virtual methods from base class
    virtual void Render(lmPaper* pPaper, wxColour color);
    virtual void RenderWithHandlers(lmPaper* pPaper);
    virtual wxString Dump(int nIndent);
    virtual void Shift(lmLUnits xIncr, lmLUnits yIncr);

    //creation
    void SetStartPoint(lmLUnits xPos, lmLUnits yPos);
    void SetEndPoint(lmLUnits xPos, lmLUnits yPos);
    void SetCtrolPoint1(lmLUnits xPos, lmLUnits yPos);
    void SetCtrolPoint2(lmLUnits xPos, lmLUnits yPos);

    //shape dragging
    wxBitmap* OnBeginDrag(double rScale, wxDC* pDC);
	lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
	void OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos);

    //handlers dragging
    lmUPoint OnHandlerDrag(lmPaper* pPaper, const lmUPoint& uPos, long nHandlerID);
    void OnHandlerEndDrag(lmController* pCanvas, const lmUPoint& uPos, long nHandlerID);

    //access to information
    inline lmLUnits GetStartPosX() const { return m_uPoint[lmBEZIER_START].x; }
    inline lmLUnits GetStartPosY() const { return m_uPoint[lmBEZIER_START].y; }
    inline lmLUnits GetEndPosX() const { return m_uPoint[lmBEZIER_END].x; }
    inline lmLUnits GetEndPosY() const { return m_uPoint[lmBEZIER_END].y; }
    inline lmLUnits GetCtrol1PosX() const { return m_uPoint[lmBEZIER_CTROL1].x; }
    inline lmLUnits GetCtrol1PosY() const { return m_uPoint[lmBEZIER_CTROL1].y; }
    inline lmLUnits GetCtrol2PosX() const { return m_uPoint[lmBEZIER_CTROL2].x; }
    inline lmLUnits GetCtrol2PosY() const { return m_uPoint[lmBEZIER_CTROL2].y; }

    //call backs
    void MovePoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts, bool fAddShifts);



protected:
    void SetDefaultControlPoints();
    void Create();
    void Draw(lmPaper* pPaper, wxColour colorC, bool fSketch);

    bool                m_fArchUnder;                   //arch under notes (like a 'U')
    lmUPoint            m_uPoint[lmBEZIER_MAX];         //start, end and control points coordinates, absolute paper position   
    lmUPoint            m_uSavePoint[lmBEZIER_MAX];     //to save start, end and control points when dragging/moving 
    lmHandlerSquare*    m_pHandler[lmBEZIER_MAX];       //handlers


};


class lmNote;
class lmShape;
class lmShapeNote;
class lmStaff;

class lmShapeTie : public lmShapeArch
{
public:
    lmShapeTie(lmTie* pOwner, int nShapeIdx, lmNote* pEndNote, lmUPoint* pPoints,
               lmShapeNote* pShapeStart, lmShapeNote* pShapeEnd,
               bool fTieUnderNote, wxColour color = *wxBLACK, bool fVisible = true);
	~lmShapeTie();

	//implementation of virtual methods in base class
    void Render(lmPaper* pPaper, wxColour color);
    void DrawControlPoints(lmPaper* pPaper);

    //overrides
    void Shift(lmLUnits xIncr, lmLUnits yIncr) {}       //any shift is taken into account in method OnAttachmentPointMoved()

	//layout changes
	void OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag,
								lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent);
    void ApplyUserShifts();

	//splitting
	void SetBrotherTie(lmShapeTie* pBrotherTie) { m_pBrotherTie = pBrotherTie; }

    //access to information
    lmNote* GetStartNote();
    lmNote* GetEndNote();


private:
    bool			m_fTieUnderNote;
    bool            m_fUserShiftsApplied;
	lmShapeTie*		m_pBrotherTie;		    //when tie is splitted
    lmNote*         m_pEndNote;
    lmUPoint        m_uUserShifts[4];

};


#endif    // __LM_SHAPEARCH_H__

