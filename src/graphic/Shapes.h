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
#include "../app/Paper.h"
#include "GMObject.h"

class lmScoreObj;
class lmStaff;
class lmStaffObj;


class lmShapeLine : public lmSimpleShape
{
public:
    lmShapeLine(lmScoreObj* pOwner, lmLUnits xStart, lmLUnits yStart,
                lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth,
				lmLUnits uBoundsExtraWidth, wxColour nColor, wxString sName = _T("Line"),
				lmELineEdges nEdge = eEdgeNormal);
    ~lmShapeLine() {}

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

//represents a glyph from LenMus font
class lmShapeGlyph : public lmSimpleShape
{
public:
    lmShapeGlyph(lmScoreObj* pOwner, int nGlyph, wxFont* pFont, lmPaper* pPaper,
                 lmUPoint offset, wxString sName=_T("ShapeGlyp2"),
				 bool fDraggable = false, wxColour color = *wxBLACK);
    virtual ~lmShapeGlyph() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, wxColour color = *wxBLACK);
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);
	virtual wxBitmap* OnBeginDrag(double rScale);
    virtual lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
    virtual void OnEndDrag(wxCommandProcessor* pCP, const lmUPoint& uPos) {};
	lmUPoint GetObjectOrigin();


    //specific methods
    void SetFont(wxFont *pFont);


protected:
    int         m_nGlyph;
    wxFont*     m_pFont;
    lmUPoint    m_uGlyphPos;   //glyph position

};


//represents a text with the same font
class lmShapeText : public lmSimpleShape
{
public:
    lmShapeText(lmScoreObj* pOwner, wxString sText, wxFont* pFont);
    ~lmShapeText() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, wxColour color = *wxBLACK);
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

    //specific methods
    void Measure(lmPaper* pPaper, lmStaff* pStaff, lmUPoint shift);
    void SetFont(wxFont *pFont);
    wxString* GetText() { return &m_sText; }


private:
    wxString    m_sText;
    wxFont*     m_pFont;
    lmUPoint    m_uShift;        // to correctly position the text (relative to shape offset point)

};

//represents a text with the same font
class lmShapeTex2 : public lmSimpleShape
{
public:
    lmShapeTex2(lmScoreObj* pOwner, wxString sText, wxFont* pFont, lmPaper* pPaper,
                lmUPoint offset, wxString sName=_T("ShapeTex2"),
				bool fDraggable = false, wxColour color = *wxBLACK);
    ~lmShapeTex2() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, wxColour color = *wxBLACK);
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

    //specific methods
    void SetFont(wxFont *pFont);
    wxString* GetText() { return &m_sText; }


private:
    wxString    m_sText;
    wxFont*     m_pFont;
    lmUPoint    m_uPos;        // text position (absolute)

};

class lmShapeStem : public lmShapeLine
{
public:
    lmShapeStem(lmScoreObj* pOwner, lmLUnits xPos, lmLUnits yStart, lmLUnits yEnd,
				bool fStemDown, lmLUnits uWidth, wxColour nColor);
    ~lmShapeStem() {}

	//specific methods
	void SetLength(lmLUnits uLenght, bool fModifyTop);
	inline bool StemDown() const { return m_fStemDown; }
	void Adjust(lmLUnits xPos, lmLUnits yStart, lmLUnits yEnd, bool fStemDown);
	lmLUnits GetYStartStem();
	lmLUnits GetYEndStem();
	lmLUnits GetXCenterStem();

private:
	bool	m_fStemDown;

};

class lmShapeClef : public lmShapeGlyph
{
public:
    lmShapeClef(lmScoreObj* pOwner, int nGlyph, wxFont* pFont, lmPaper* pPaper,
                lmUPoint offset, wxString sName=_T("Clef"),
				bool fDraggable = false, wxColour color = *wxBLACK) 
				: lmShapeGlyph(pOwner, nGlyph, pFont, pPaper, offset, sName,
				               fDraggable, color) {}
    ~lmShapeClef() {}

	//overrides
    lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
    void OnEndDrag(wxCommandProcessor* pCP, const lmUPoint& uPos);

};

class lmShapeInvisible : public lmSimpleShape
{
public:
    lmShapeInvisible(lmScoreObj* pOwner, lmUPoint offset, wxString sName=_T("Invisible")); 
    ~lmShapeInvisible() {}

	//overrides
    wxString Dump(int nIndent);
	void Render(lmPaper* pPaper, wxColour color) {};


};



#endif    // __LM_SHAPES_H__

