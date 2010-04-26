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

#ifndef __LM_AUXOBJ_H__        //to avoid nested includes
#define __LM_AUXOBJ_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "AuxObj.cpp"
#endif

#include "defs.h"
#include "StaffObj.h"
#include "Text.h"

class lmBox;
class lmComponentObj;
class lmPaper;

//========================================================================================
// lmScoreLine: a line (graphic)
//========================================================================================

class lmScoreLine : public lmAuxObj
{
public:
    lmScoreLine(lmScoreObj* pOwner, long nID, lmTenths xStart, lmTenths yStart,
                lmTenths xEnd, lmTenths yEnd, lmTenths nWidth, lmELineCap nStartCap,
                lmELineCap nEndCap, lmELineStyle nStyle, wxColour nColor);
    ~lmScoreLine() {}

    // source code related methods
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

    // debug methods
    wxString Dump();

    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //undoable edition commands
    void MoveObjectPoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts, bool fAddShifts);

	//edit properties
	void OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName = wxEmptyString);

private:
    lmTenths        m_txStart;
    lmTenths        m_tyStart; 
    lmTenths        m_txEnd;
    lmTenths        m_tyEnd;
    lmTenths        m_tWidth;
    wxColour        m_nColor;
	lmELineEdges    m_nEdge;
    lmELineStyle    m_nStyle;
    lmELineCap     m_nStartCap;
    lmELineCap     m_nEndCap;

};


//========================================================================================
// lmFermata
//========================================================================================

class lmNoteRest;
class lmVStaff;

class lmFermata : public lmAuxObj
{
public:
    lmFermata(lmScoreObj* pOwner, long nID, lmEPlacement nPlacement);
    ~lmFermata() {}

    // implementation of pure virtual methods of base class
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    // source code related methods
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

    // debug methods
    wxString Dump();

	//other
	bool IsAbove();


private:
    lmEPlacement    m_nPlacement;

};


//========================================================================================
// lmLyric
//========================================================================================

enum ESyllabicTypes {
    eSyllabicSingle = 0,
    eSyllabicBegin,
    eSyllabicMiddle,
    eSyllabicEnd
};

class lmLyric : public lmAuxObj, public lmBasicText
{
public:
    lmLyric(lmScoreObj* pOwner, wxString& sText, lmTextStyle* pStyle,
            ESyllabicTypes nSyllabic = eSyllabicSingle,
            int nNumLine=1, wxString sLanguage=_T("it") );
    ~lmLyric() {}

    // implementation of pure virtual methods in base classes
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	wxFont* GetSuitableFont(lmPaper* pPaper);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //// overrides for virtual methods of base class lmNoteRestObj
    //void SetOwner(lmNoteRest* pOwner);

    // overrides for virtual methods of base class lmComponentObj
    //void SetFont(lmPaper* pPaper);

    // source code related methods
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

    // debug methods
    wxString Dump();


private:
    int             m_nNumLine;
    lmVStaff*       m_pVStaff;          // lmVStaff to which the owner NoterRest belongs
    int             m_nStaffNum;        // Staff (1..n) on which owner NoterRest is located

};

#endif    // __LM_AUXOBJ_H__

