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

#ifndef __LM_SHAPE_LINE_H__        //to avoid nested includes
#define __LM_SHAPE_LINE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ShapeLine.cpp"
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
#include "../app/Paper.h"       // enum lmELineEdges & line start/endings
#include "GMObject.h"
#include "Shapes.h"

class lmScoreObj;
class lmPaper;
class lmHandlerSquare;
class lmHandlerLine;


//line points
enum lmELinePoint
{
    lmLINE_START = 0,
    lmLINE_END,
};

//------------------------------------------------------------------------------------

class lmArrowHead
{
 public:
    lmArrowHead(lmLUnits uHeight = 0.0, lmLUnits uWidth = 0.0);
    ~lmArrowHead();

private:
    lmLUnits        m_uHeight;
    lmLUnits        m_uWidth;
};


//------------------------------------------------------------------------------------

class lmCircleHead
{
 public:
    lmCircleHead(lmLUnits uRadius = 0.0, lmLUnits uLineWidth = 0.0);
    ~lmCircleHead();

private:
    lmLUnits        m_uRadius;
    lmLUnits        m_uLineWidth;
};


//------------------------------------------------------------------------------------

class lmShapeLine : public lmSimpleShape
{
public:
    lmShapeLine(lmScoreObj* pOwner, int nShapeIdx,
                lmLUnits uxStart, lmLUnits uyStart, lmLUnits uxEnd, lmLUnits uyEnd,
                lmLUnits uWidth, lmLUnits uBoundsExtraWidth, lmELineStyle nStyle,
                wxColour nColor, lmELineEdges nEdge = lm_eEdgeNormal,
                bool fDraggable = true, bool fSelectable = true,
                bool fVisible = true, wxString sName = _T("Line"));      

    lmShapeLine(lmScoreObj* pOwner);

    virtual ~lmShapeLine();

    //properties and options
    void SetAsControlled(lmELinePoint nPointID);
    inline void SetHeadType(lmELineCap nHeadType) { m_nStartCap = nHeadType; }
    inline void SetTailType(lmELineCap nHeadType) { m_nEndCap = nHeadType; }

    //renderization
    void Render(lmPaper* pPaper, wxColour color = *wxBLACK);
    void RenderWithHandlers(lmPaper* pPaper);

    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);
    lmUPoint GetPointForHandler(long nHandlerID);

    //shape dragging
    wxBitmap* OnBeginDrag(double rScale, wxDC* pDC);
	lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
	void OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos);

    //handlers dragging
    lmUPoint OnHandlerDrag(lmPaper* pPaper, const lmUPoint& uPos, long nHandlerID);
    void OnHandlerEndDrag(lmController* pCanvas, const lmUPoint& uPos, long nHandlerID);

    //call backs
    void MovePoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts, bool fAddShifts);

    //Handler IDs. AWARE: Used also as array indexes
    enum
    {
        lmID_START = 0,
        lmID_END,
        //
        lmID_NUM_HANDLERS
    };

    //overrides
    bool HitTest(lmUPoint& uPoint);


protected:
	void Create(lmLUnits uxStart, lmLUnits uyStart, lmLUnits uxEnd, lmLUnits uyEnd,
				lmLUnits uWidth, lmLUnits uBoundsExtraWidth, wxColour nColor,
				lmELineEdges nEdge, lmELineCap nStartCap, lmELineCap nEndCap);
    void DrawLine(lmPaper* pPaper, wxColour colorC, bool fSketch);
    void UpdateBounds();
    lmLUnits GetDistanceToLine(lmUPoint uPoint);


    //some vector algebra

    typedef struct lmUVectorStruct
    {
        lmLUnits x;
        lmLUnits y;
    }
    lmUVector;

    lmLUnits VectorDotProduct(lmUVector& v0, lmUVector& v1);
    void SubtractVectors(lmUVector& v0, lmUVector& v1, lmUVector& v);
    lmLUnits VectorMagnitude(lmUVector& v);


    lmUPoint            m_uPoint[lmID_NUM_HANDLERS];       //start and end points, absolute paper position   
    lmUPoint            m_uSavePoint[lmID_NUM_HANDLERS];   //to save start and end points when dragging/moving 
    lmHandlerSquare*    m_pHandler[lmID_NUM_HANDLERS];     //handlers
    bool                m_fIsControlled[lmID_NUM_HANDLERS];    //the point is controlled
    bool                m_fIsFixed[lmID_NUM_HANDLERS];     //the point is fixed
    lmLUnits		    m_uWidth;
	lmLUnits		    m_uBoundsExtraWidth;
	lmELineEdges	    m_nEdge;
    lmELineStyle        m_nStyle;
    lmELineCap         m_nStartCap;
    lmELineCap         m_nEndCap;

};

//------------------------------------------------------------------------------------

class lmFiguredBass;

class lmShapeFBLine : public lmShapeLine
{
public:
    lmShapeFBLine(lmScoreObj* pOwner, int nShapeIdx, lmFiguredBass* pEndFB,
                  lmLUnits uxStart, lmLUnits uyStart, lmLUnits uxEnd, lmLUnits uyEnd,
                  lmTenths tWidth, lmShapeFiguredBass* pShapeStartFB,
                  lmShapeFiguredBass* pShapeEndFB, wxColour nColor, bool fVisible);
    ~lmShapeFBLine();

	//layout changes
	void OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag,
								lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent);
	//splitting
	inline void SetBrotherLine(lmShapeFBLine* pBrotherLine)
                    { m_pBrotherLine = pBrotherLine; }

    //access to information
    //lmFiguredBass* GetStartFB();
    lmFiguredBass* GetEndFB();

    //changing values
    inline void SetStartPoint(lmUPoint uPos) { m_uPoint[lmID_START] = uPos; }
    inline void SetEndPoint(lmUPoint uPos) { m_uPoint[lmID_END] = uPos; }

    //access to information
    inline lmLUnits GetStartPosX() const { return m_uPoint[lmID_START].x; }
    inline lmLUnits GetStartPosY() const { return m_uPoint[lmID_START].y; }
    inline lmLUnits GetEndPosX() const { return m_uPoint[lmID_END].x; }
    inline lmLUnits GetEndPosY() const { return m_uPoint[lmID_END].y; }

private:
	lmShapeFBLine*  m_pBrotherLine;		    //when line is splitted
    lmFiguredBass*  m_pEndFB;
    bool            m_fUserShiftsApplied;
    lmUPoint        m_uUserShifts[4];

};


#endif    // __LM_SHAPE_LINE_H__

