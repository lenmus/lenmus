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

#ifndef __LM_AUXOBJ_H__        //to avoid nested includes
#define __LM_AUXOBJ_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "AuxObj.cpp"
#endif

#include "defs.h"
#include "StaffObj.h"
#include "Text.h"

class lmBox;
class lmScoreObj;
class lmPaper;

//========================================================================================
// lmScoreLine: a line (graphic)
//========================================================================================

class lmScoreLine : public lmAuxObj
{
public:
    lmScoreLine(lmScoreObj* pOwner,
             lmTenths xStart, lmTenths yStart, 
             lmTenths xEnd, lmTenths yEnd, lmTenths nWidth, wxColour nColor);
    ~lmScoreLine() {}

    //implementation of virtual methods from base class
    lmEAuxObjType GetAuxObjType() { return eAXOT_Line; }

    wxString Dump();
    void LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC);

private:
    lmLUnits    m_uxStart;
    lmLUnits    m_uyStart; 
    lmLUnits    m_uxEnd;
    lmLUnits    m_uyEnd;
    lmLUnits    m_uWidth;
    wxColour    m_nColor;


};


//========================================================================================
// lmFermata
//========================================================================================

class lmNoteRest;
class lmVStaff;

class lmFermata : public lmAuxObj
{
public:
    lmFermata(lmNoteRest* pOwner, lmEPlacement nPlacement);
    ~lmFermata() {}

    // overrides for pure virtual methods of base class lmNoteRestObj
    void LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC);
    void SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
                         lmLUnits xPos, lmLUnits yPos);
    lmEAuxObjType GetAuxObjType() { return eAXOT_Fermata; }

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
    lmLyric(lmNoteRest* pOwner, wxString sText, ESyllabicTypes nSyllabic = eSyllabicSingle,
            int nNumLine=1, wxString sLanguage=_T("it") );
    ~lmLyric() {}

    // implementation of pure virtual methods in base class
    void LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC);
    void SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
                         lmLUnits xPos, lmLUnits yPos);
    lmEAuxObjType GetAuxObjType() { return eAXOT_Lyric; }


    // overrides for virtual methods of base class lmNoteRestObj
    void SetOwner(lmNoteRest* pOwner);

    // overrides for virtual methods of base class lmScoreObj
    void SetFont(lmPaper* pPaper);
    wxString Dump();


private:
    int             m_nNumLine;
    lmVStaff*       m_pVStaff;          // lmVStaff to which the owner NoterRest belongs
    int             m_nStaffNum;        // Staff (1..n) on which owner NoterRest is located

};

#endif    // __LM_AUXOBJ_H__

