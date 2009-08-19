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

#ifndef __LM_SLUR_H__        //to avoid nested includes
#define __LM_SLUR_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Slur.cpp"
#endif

#include "defs.h"
#include "StaffObj.h"


//===================================================================================
// lmBezier: a helper class to manage the four points of a Bezier curve
//===================================================================================

//IDs for the points
enum lmEBezierPoint
{
    lmBEZIER_START = 0,
    lmBEZIER_END,
    lmBEZIER_CTROL1,
    lmBEZIER_CTROL2,
    //
    lmBEZIER_MAX
};


class lmBezier
{
public:
    lmBezier();
    ~lmBezier();

    //points
    void SetPoint(int nPointID, lmTPoint& tPoint);
    lmTPoint& GetPoint(int nPointID);

    // source code related methods
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);


protected:
    lmTPoint    m_tPoints[4];   //start, end, ctrol1, ctrol2
};


//===================================================================================
//   lmTie
//===================================================================================

//constants for PropagateNotePitchChange() method
#define lmBACKWARDS false
#define lmFORWARDS  true

class lmShapeNote;
class lmShape;
class lmBox;
class lmShapeTie;

//Rest are never tied (http://www.dolmetsch.com/musictheory2.htm)
class lmTie: public lmBinaryRelObj
{
public:
    lmTie(lmScoreObj* pOwner, long nID, lmNote* pStartNote, lmNote* pEndNote);
    ~lmTie();

    //creation
    void SetBezierPoints(int nBezier, lmTPoint* ptPoints);
    void SetBezier(int nBezier, lmBezier* pBezier);

    //implementation of pure virtual methods of base class
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);
    int GetNumPoints() { return lmBEZIER_MAX; }

    //overrides
    void OnParentComputedPositionShifted(lmLUnits uxShift, lmLUnits uyShift) {}
    void OnParentMoved(lmLUnits uxShift, lmLUnits uyShift) {}

    //source code related methods. Implementation of needed virtual methods in lmRelObj
    wxString SourceLDP_First(int nIndent, bool fUndoData, lmNoteRest* pNR);
    wxString SourceLDP_Last(int nIndent, bool fUndoData, lmNoteRest* pNR);
    wxString SourceXML_First(int nIndent, lmNoteRest* pNR);
    wxString SourceXML_Last(int nIndent, lmNoteRest* pNR);

    //information
    inline lmBezier& GetBezier(int nBezier) { return m_Bezier[nBezier]; }

    // debug methods
    wxString Dump();

    void PropagateNotePitchChange(lmNote* pNote, int nStep, int nOctave, int nAlter, bool fForward);

    //undoable edition commands
    void MoveObjectPoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts, bool fAddShifts);

protected:
    lmBezier        m_Bezier[2];    //points for main and secondary archs
};



//===================================================================================
// lmSlur
//===================================================================================

//class lmSlur : public lmAuxObj
//{
//public:
//    lmSlur(lmScoreObj* pOwner, lmNoteRest* pStartNR, lmNoteRest* pEndNR);
//    ~lmSlur();
//
//    // overrides for pure virtual methods of base class
//    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
//    lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);
//
//    // source code related methods
//    wxString SourceLDP(int nIndent, bool fUndoData);
//    wxString SourceXML(int nIndent);
//
//    // debug methods
//    wxString Dump();
//
//    //n-relationship methods
//    void Remove(lmNoteRest* pNR);
//    lmNoteRest* GetStartNoteRest() const { return m_pStartNR; }
//    lmNoteRest* GetEndNoteRest() const { return m_pEndNR; }
//
//    //undoable edition commands
//    void MoveObjectPoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts, bool fAddShifts);
//
//protected:
//
//    lmNoteRest*     m_pStartNR;     //noteRests joined by this slur
//    lmNoteRest*     m_pEndNR;
//
//    lmUPoint        m_Points[4];    //main arch bezier points (start, end, ctrol1, ctrol2)
//    lmShapeArch*    m_pShape;       //main arch
//
//};

#endif    // __LM_SLUR_H__
