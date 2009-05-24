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

#ifndef __LM_SHAPES_H__        //to avoid nested includes
#define __LM_SHAPES_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Shapes.cpp"
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
#include "../app/Paper.h"       // enum lmELineEdges
#include "GMObject.h"

class lmScoreObj;
class lmStaff;
class lmStaffObj;
class lmPaper;
class lmHandlerSquare;
class lmHandlerLine;
class lmShapeLine;


//------------------------------------------------------------------------------------

class lmShapeSimpleLine : public lmSimpleShape
{
public:
    lmShapeSimpleLine(lmScoreObj* pOwner, lmLUnits xStart, lmLUnits yStart,
                lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth,
				lmLUnits uBoundsExtraWidth, wxColour nColor, wxString sName = _T("Line"),
				lmELineEdges nEdge = lm_eEdgeNormal);
    ~lmShapeSimpleLine() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, wxColour color = *wxBLACK);
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

protected:
	void Create(lmLUnits xStart, lmLUnits yStart, lmLUnits xEnd, lmLUnits yEnd,
				lmLUnits uWidth, lmLUnits uBoundsExtraWidth, wxColour nColor,
				lmELineEdges nEdge);


    lmLUnits		m_xStart, m_yStart;
    lmLUnits		m_xEnd, m_yEnd;
    lmLUnits		m_uWidth;
	lmLUnits		m_uBoundsExtraWidth;
	lmELineEdges	m_nEdge;

};

//------------------------------------------------------------------------------------

class lmShapeRectangle : public lmSimpleShape
{
public:
    //TODO: remove this backwards compatibility constructor
    lmShapeRectangle(lmScoreObj* pOwner, lmLUnits xLeft, lmLUnits yTop,
                     lmLUnits xRight, lmLUnits yBottom, lmLUnits uWidth,
                     wxColour color = *wxBLACK, wxString sName = _T("Rectangle"),
				     bool fDraggable = true, bool fSelectable = true, 
                     bool fVisible = true);

    //new rectangle constructor
    lmShapeRectangle(lmScoreObj* pOwner,
                     //position and size
                     lmLUnits uxLeft, lmLUnits uyTop, lmLUnits uxRight, lmLUnits uyBottom,
                     //border
                     lmLUnits uBorderWidth, wxColour nBorderColor,
                     //content
                     wxColour nBgColor = *wxWHITE,
                     //other
                     int nShapeIdx = 0, wxString sName = _T("Rectangle"),
				     bool fDraggable = true, bool fSelectable = true, 
                     bool fVisible = true);

    virtual ~lmShapeRectangle();

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, wxColour color = *wxBLACK);
    void RenderNormal(lmPaper* pPaper, wxColour color);
    void RenderWithHandlers(lmPaper* pPaper);
    wxString Dump(int nIndent);
    void Shift(lmLUnits uxIncr, lmLUnits uyIncr);

    //Handler IDs. AWARE: Used also as array indexes
    enum
    {
        lmID_TOP_LEFT = 0,
        lmID_TOP_RIGHT,
        lmID_BOTTOM_RIGHT,
        lmID_BOTTOM_LEFT,
        //
        lmID_LEFT_CENTER,
        lmID_TOP_CENTER,
        lmID_RIGHT_CENTER,
        lmID_BOTTOM_CENTER,
        //
        lmID_NUM_HANDLERS
    };

    //settings
    void SetCornerRadius(lmLUnits uRadius);
    inline void SetBorderStyle(lmELineStyle nBorderStyle) { m_nBorderStyle = nBorderStyle; }

    //shape dragging
    wxBitmap* OnBeginDrag(double rScale, wxDC* pDC);
	lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
	void OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos);

    //handlers dragging
    lmUPoint OnHandlerDrag(lmPaper* pPaper, const lmUPoint& uPos, long nHandlerID);
    void OnHandlerEndDrag(lmController* pCanvas, const lmUPoint& uPos, long nHandlerID);

    //call backs
    void MovePoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts, bool fAddShifts);


protected:
    void Create(lmLUnits xLeft, lmLUnits yTop, lmLUnits xRight, lmLUnits yBottom,
                lmLUnits uBorderWidth, wxColour nBorderColor, wxColour nBgColor);
    void DrawRectangle(lmPaper* pPaper, wxColour colorC, bool fSketch);
    void ComputeNewPointsAndHandlersPositions(const lmUPoint& uPos, long nHandlerID);
    void ComputeCenterPoints();
    void UpdateBounds();
    void SavePoints();

    //rectangle
    wxColour        m_nBgColor;
    lmLUnits        m_uCornerRadius;

    //border
    lmLUnits        m_uBorderWidth;
    wxColour        m_nBorderColor;
    lmELineStyle    m_nBorderStyle;

    //rectangle points and handlers
    lmUPoint            m_uPoint[lmID_NUM_HANDLERS];       //four corners + anchor point + centers of rectangle sides   
    lmUPoint            m_uSavePoint[lmID_NUM_HANDLERS];   //to save start and end points when dragging/moving 
    lmHandlerSquare*    m_pHandler[lmID_NUM_HANDLERS];     //handlers

};

//------------------------------------------------------------------------------------

//represents a glyph from LenMus font
class lmShapeGlyph : public lmSimpleShape
{
public:
    lmShapeGlyph(lmScoreObj* pOwner, int nShapeIdx, int nGlyph, 
                 lmPaper* pPaper, lmUPoint offset, wxString sName=_T("ShapeGlyph"),
				 bool fDraggable = false, wxColour color = *wxBLACK);
    virtual ~lmShapeGlyph() {}

    //implementation of virtual methods from base class
    virtual void Render(lmPaper* pPaper, wxColour color = *wxBLACK);
    void RenderHighlighted(wxDC* pDC, wxColour colorC);

    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);
	virtual wxBitmap* OnBeginDrag(double rScale, wxDC* pDC);
    virtual lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
	virtual void OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos);
	lmUPoint GetObjectOrigin();


protected:
    wxBitmap* GetBitmapFromShape(double rScale, wxColour colorF, wxColour colorB = *wxWHITE);
    virtual double GetPointSize();

    int         m_nGlyph;
    lmUPoint    m_uGlyphPos;   //glyph position

};

//------------------------------------------------------------------------------------

class lmShapeClef : public lmShapeGlyph
{
public:
    lmShapeClef(lmScoreObj* pOwner, int nShapeIdx, int nGlyph, lmPaper* pPaper,
                lmUPoint offset, bool fSmallClef, wxString sName=_T("Clef"),
				bool fDraggable = false, wxColour color = *wxBLACK); 
    ~lmShapeClef() {}

	//overrides
    lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
    void OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos);
    double GetPointSize();

protected:
    bool        m_fSmallClef;
};

//------------------------------------------------------------------------------------

class lmShapeInvisible : public lmSimpleShape
{
public:
    lmShapeInvisible(lmScoreObj* pOwner, int nShapeIdx, lmUPoint uPos, lmUSize uSize,
                     wxString sName=_T("Invisible")); 
    ~lmShapeInvisible() {}

	//overrides
    wxString Dump(int nIndent);
	void Render(lmPaper* pPaper, wxColour color);


};

//------------------------------------------------------------------------------------

class lmShapeStem : public lmShapeSimpleLine
{
public:
    lmShapeStem(lmScoreObj* pOwner, lmLUnits xPos, lmLUnits yStart, lmLUnits uExtraLength,
                lmLUnits yEnd, bool fStemDown, lmLUnits uWidth, wxColour nColor);
    ~lmShapeStem() {}

	//specific methods
	void SetLength(lmLUnits uLenght, bool fModifyTop);
	inline bool StemDown() const { return m_fStemDown; }
	void Adjust(lmLUnits xPos, lmLUnits yStart, lmLUnits yEnd, bool fStemDown);
	lmLUnits GetYStartStem();
	lmLUnits GetYEndStem();
	lmLUnits GetXCenterStem();
    inline lmLUnits GetExtraLenght() { return m_uExtraLength; }

private:
	bool	    m_fStemDown;
    lmLUnits    m_uExtraLength;

};

//------------------------------------------------------------------------------------

class lmShapeWindow : public lmShapeRectangle
{
public:
    lmShapeWindow(lmScoreObj* pOwner, int nShapeIdx, 
                  //position and size
                  lmLUnits uxLeft, lmLUnits uyTop, lmLUnits uxRight, lmLUnits uyBottom,
                  //border
                  lmLUnits uBorderWidth, wxColour nBorderColor,
                  //content
                  wxColour nBgColor = *wxWHITE,
                  //other
                  wxString sName = _T("Window"),
				  bool fDraggable = true, bool fSelectable = true, 
                  bool fVisible = true);
    virtual ~lmShapeWindow() {}

    //renderization
    void Render(lmPaper* pPaper, wxColour color = *wxBLACK);

	//specific methods

protected:

    wxWindow*       m_pWidget;      //the window to embbed
};


#endif    // __LM_SHAPES_H__

