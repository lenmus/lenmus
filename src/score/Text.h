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

#ifndef __LM_TEXT_H__        //to avoid nested includes
#define __LM_TEXT_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Text.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/font.h>


#include "defs.h"
#include "StaffObj.h"

class lmBox;
class lmPaper;
class lmGMObject;
class lmUndoItem;
class lmShapeText;
class lmBox;

// lmScoreText types
enum
{
    lmSIMPLE_TEXT = 0,
    lmBLOCK_TEXT,
};

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
    bool            m_fIsTitle;     //to identify titles

    // position
    lmLocation      m_tTextPos;

    // font
    lmTextStyle*    m_pStyle;

};


//------------------------------------------------------------------------------------

class lmScoreText :  public lmAuxObj, public lmBasicText
{
public:
    //simple text constructor
    lmScoreText(wxString& sTitle, lmEHAlign nHAlign, lmLocation& tPos,
                lmTextStyle* pStyle, bool fTitle=false);

    //block text constructor
    lmScoreText(wxString& sTitle, lmEBlockAlign nBlockAlign, lmEHAlign nHAlign,
                lmEVAlign nVAlign, lmLocation& tPos, lmTextStyle* pStyle,
                bool fTitle=false);

    ~lmScoreText() {}

    //implementation of virtual methods defined in abstract base class
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	wxFont* GetSuitableFont(lmPaper* pPaper);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //implementation of virtual methods from base class
    inline lmEAuxObjType GetAuxObjType() { return eAXOT_Text; }
	void OnProperties(lmController* pController, lmGMObject* pGMO);
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);

    //properties
    inline lmEHAlign GetAlignment() { return m_nHAlign; }
    inline void SetAlignment(lmEHAlign nHAlign) { m_nHAlign = nHAlign; }

    //layout
	lmShapeText* CreateShape(lmPaper* pPaper, lmUPoint uPos);

    //edit commands
    void Cmd_ChangeText(lmUndoItem* pUndoItem, wxString& sText, lmEHAlign nHAlign,
                        lmLocation tPos, lmTextStyle* pTS);
    void UndoCmd_ChangeText(lmUndoItem* pUndoItem, wxString& sText, lmEHAlign nHAlign,
                            lmLocation tPos, lmTextStyle* pTS);


private:
    lmEBlockAlign   m_nBlockAlign;
    lmEHAlign       m_nHAlign;
    lmEVAlign       m_nVAlign;

};


//------------------------------------------------------------------------------------

//global functions defined in this module
extern lmLUnits PointsToLUnits(int nPoints);
extern int LUnitsToPoints(lmLUnits uUnits);

#endif    // __LM_TEXT_H__

