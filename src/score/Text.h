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

#ifndef __LM_TEXT_H__        //to avoid nested includes
#define __LM_TEXT_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Text.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/font.h>


#include "defs.h"
#include "StaffObj.h"

class lmBox;
class lmPaper;
class lmGMObject;
class lmShapeText;
class lmShapeTitle;
class lmBox;


//------------------------------------------------------------------------------------

class lmBasicText
{
public:
    lmBasicText(wxString& sText, lmLocation& tPos, lmTextStyle* pStyle,
                const wxString& sLanguage = _T("Unknown") );
    virtual ~lmBasicText();

    //access to properties
    inline void SetText(wxString& text) { m_sText = text; }
	inline wxString& GetText() {return m_sText; }

    inline void SetLanguage(wxString sLanguage) { m_sLanguage = sLanguage; }
    inline wxString& GetLanguage() { return m_sLanguage; }

    inline lmTextStyle* GetStyle() { return m_pStyle; }
    inline void SetStyle(lmTextStyle* pStyle) { m_pStyle = pStyle; }
    inline lmFontInfo GetFontInfo() { return m_pStyle->tFont; }
    inline wxString& GetFontName() { return m_pStyle->tFont.sFontName; }
    inline int GetFontSize() { return m_pStyle->tFont.nFontSize; }
    inline bool IsBold() { return m_pStyle->tFont.nFontWeight == wxFONTWEIGHT_BOLD; }
    inline bool IsItalic() { return m_pStyle->tFont.nFontStyle == wxFONTSTYLE_ITALIC; }
    inline wxColour GetColour() { return m_pStyle->nColor; }

    inline void SetLocation(lmLocation tPos) { m_tTextPos = tPos; }
    inline lmLocation GetLocation() { return m_tTextPos; }


protected:
    wxString        m_sText;
    wxString        m_sLanguage;

    // position
    lmLocation      m_tTextPos;

    // font
    lmTextStyle*    m_pStyle;

};

//------------------------------------------------------------------------------------
// lmBaseText: Single piece of text plus its style

class lmBaseText
{
public:
    lmBaseText(const wxString& sText, lmTextStyle* pStyle);
    virtual ~lmBaseText() {}

    //text
    inline void SetText(wxString& text) { m_sText = text; }
	inline wxString& GetText() {return m_sText; }

    //manage style
    inline lmTextStyle* GetStyle() { return m_pStyle; }
    inline void SetStyle(lmTextStyle* pStyle) { m_pStyle = pStyle; }

    //style information
    inline lmFontInfo GetFontInfo() { return m_pStyle->tFont; }
    inline wxString& GetFontName() { return m_pStyle->tFont.sFontName; }
    inline int GetFontSize() { return m_pStyle->tFont.nFontSize; }
    inline bool IsBold() { return m_pStyle && m_pStyle->tFont.nFontWeight == wxFONTWEIGHT_BOLD; }
    inline bool IsItalic() { return m_pStyle && m_pStyle->tFont.nFontStyle == wxFONTSTYLE_ITALIC; }
    inline wxColour GetColour() { return m_pStyle->nColor; }

    //layout related
    wxFont* GetSuitableFont(lmPaper* pPaper);

    // source code related methods
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

    // debug methods
    wxString Dump();



protected:
    wxString        m_sText;
    lmTextStyle*    m_pStyle;       //font and style

};

//------------------------------------------------------------------------------------
// lmScoreBlock - An AuxObj modelling an abstract block: box (a rectangle) plus
//      alligment attributes + anchor line (optional)


class lmScoreBlock : public lmAuxObj
{
public:
    virtual ~lmScoreBlock() {}

    //operations
    virtual void AddAnchorLine(lmLocation tPoint, lmTenths ntWidth, lmELineStyle nStyle,
                               lmELineCap nEndStyle, wxColour nColor);

    //rectangle attributes
    inline void SetWidth(lmTenths ntWidth) { m_ntWidth = ntWidth; }
    inline void SetHeight(lmTenths ntHeight) { m_ntHeight = ntHeight; }
    inline void SetBgColour(wxColour nColor) { m_nBgColor = nColor; }
    //border
    inline void SetBorderWidth(lmTenths ntBorderWidth) { m_ntBorderWidth = ntBorderWidth; }
    inline void SetBorderColor(wxColour nBorderColor) { m_nBorderColor = nBorderColor; }
    inline void SetBorderStyle(lmELineStyle nBorderStyle) { m_nBorderStyle = nBorderStyle; }
    //anchor line
    inline void SetAnchorLineStyle(lmELineStyle nAnchorLineStyle) { m_nAnchorLineStyle = nAnchorLineStyle; }
    inline void SetAnchorLineColor(wxColour nAnchorLineColor) { m_nAnchorLineColor = nAnchorLineColor; }
    inline void SetAnchorLineWidth(lmTenths ntAnchorLineWidth) { m_ntAnchorLineWidth = ntAnchorLineWidth; }

    //undoable edition commands
    void MoveObjectPoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts, bool fAddShifts);

    //lmAuxObj overrides for multi-shaped objects
    void OnParentComputedPositionShifted(lmLUnits uxShift, lmLUnits uyShift);
    void OnParentMoved(lmLUnits uxShift, lmLUnits uyShift);

protected:
    lmScoreBlock(lmScoreObj* pOwner, long nID, lmEScoreObjType nType,
                 lmTenths ntWidth = 160.0f,
                 lmTenths ntHeight = 100.0f,
                 lmTPoint ntPos = lmTPoint(0.0f, 0.0f),
                 lmEBlockAlign nBlockAlign = lmBLOCK_ALIGN_DEFAULT,
                 lmEHAlign nHAlign = lmHALIGN_DEFAULT,
                 lmEVAlign nVAlign = lmVALIGN_DEFAULT);
    bool ComputeAnchorJoinPoint(lmTPoint* ptAttachment);
    void MoveJoinPoint();

    //block attributtes
    lmEBlockAlign       m_nBlockAlign;
    lmEHAlign           m_nHAlign;
    lmEVAlign           m_nVAlign;

    //block pos and size
    lmTenths    m_ntHeight;
    lmTenths    m_ntWidth;
    lmTPoint    m_ntPos;        //top-left corner 

    //block looking 
    wxColour        m_nBgColor;
    wxColour        m_nBorderColor;
    lmTenths        m_ntBorderWidth;
    lmELineStyle    m_nBorderStyle;

    //anchor line
    bool            m_fHasAnchorLine;
    lmTPoint        m_ntAnchorPoint;
    lmTPoint        m_ntAnchorJoinPoint;        //point on the rectangle
    lmELineStyle    m_nAnchorLineStyle;
    lmELineCap m_nAnchorLineEndStyle;
    wxColour        m_nAnchorLineColor;
    lmTenths        m_ntAnchorLineWidth;

    //shapes
    enum {
        lmIDX_RECTANGLE = 0,
        lmIDX_ANCHORLINE
    };

    lmShapeRectangle*       m_pShapeRectangle; 
    lmShapeLine*            m_pShapeLine;

};

//------------------------------------------------------------------------------------
// lmScoreTextParagraph:

//To be implemented




//------------------------------------------------------------------------------------
// lmScoreTextBox: box + alignment + collection of lmScoreTextParagraph

class lmScoreTextBox : public lmScoreBlock
{
public:
    lmScoreTextBox(lmScoreObj* pOwner, long nID, lmTenths ntWidth,
                         lmTenths ntHeight, lmTPoint tPos);
    virtual ~lmScoreTextBox();

    // source code related methods
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

    // debug methods
    wxString Dump();

    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);

    //operations
    void Defragment();
    void InsertTextUnit(lmBaseText* pText);


protected:
    //collection of text units
    std::list<lmBaseText*>      m_texts;

    //TODO: replace text units by text paragraphs
    ////collection of text paragraphs
    //std::list<lmScoreTextBox*>      m_paragraphs;

};



//------------------------------------------------------------------------------------

class lmScoreText : public lmAuxObj, public lmBasicText
{
public:
    virtual ~lmScoreText() {}

    //implementation of virtual methods defined in abstract base class
    virtual lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos,
                                  wxColour colorC)=0;
	virtual wxFont* GetSuitableFont(lmPaper* pPaper);
	virtual lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //implementation of virtual methods from base class
	void OnProperties(lmController* pController, lmGMObject* pGMO);

    //properties
    inline lmEHAlign GetAlignment() { return m_nHAlign; }
    inline void SetAlignment(lmEHAlign nHAlign) { m_nHAlign = nHAlign; }

    //layout
	virtual lmShape* CreateShape(lmPaper* pPaper, lmUPoint uPos)=0;

    //edit commands
    virtual void Cmd_ChangeText(wxString& sText, lmEHAlign nHAlign, lmLocation tPos,
                                lmTextStyle* pTS);

    //edition
	void OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName = wxEmptyString);


protected:
    lmScoreText(lmScoreObj* pOwner, long nID, lmEScoreObjType nType, wxString& sTitle,
                lmEHAlign nHAlign, lmTextStyle* pStyle);

    lmEBlockAlign   m_nBlockAlign;
    lmEHAlign       m_nHAlign;
    lmEVAlign       m_nVAlign;

};


//------------------------------------------------------------------------------------

class lmTextItem : public lmScoreText
{
public:
    //simple text constructor
    lmTextItem(lmScoreObj* pOwner, long nID, wxString& sTitle, lmEHAlign nHAlign,
               lmTextStyle* pStyle);

    virtual ~lmTextItem() {}

    //implementation of virtual methods defined in abstract base class
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);

    //implementation of virtual methods from base class
    wxString Dump();
    virtual wxString SourceLDP(int nIndent, bool fUndoData);
    virtual wxString SourceXML(int nIndent);

    //layout
	lmShape* CreateShape(lmPaper* pPaper, lmUPoint uPos);


private:

};


class lmInstrNameAbbrev : public lmTextItem
{
public:
    lmInstrNameAbbrev(lmScoreObj* pOwner, long nID, wxString& sTitle, lmTextStyle* pStyle)
                            : lmTextItem(pOwner, nID, sTitle, lmHALIGN_LEFT, pStyle) {};

    //specific methods
    wxString SourceLDP(wxString sTag, bool fUndoData);
};

//------------------------------------------------------------------------------------

class lmScoreTitle : public lmScoreText
{
public:
    lmScoreTitle(lmScoreObj* pOwner, long nID, wxString& sTitle, lmEBlockAlign nBlockAlign,
                 lmEHAlign nHAlign, lmEVAlign nVAlign, lmTextStyle* pStyle);

    ~lmScoreTitle() {}

    //implementation of virtual methods defined in abstract base class
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);

    //implementation of virtual methods from base class
    wxString Dump();
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

    //layout
	lmShape* CreateShape(lmPaper* pPaper, lmUPoint uPos);


private:

};

//------------------------------------------------------------------------------------

//global functions defined in this module
extern lmLUnits PointsToLUnits(int nPoints);
extern int LUnitsToPoints(lmLUnits uUnits);

#endif    // __LM_TEXT_H__

