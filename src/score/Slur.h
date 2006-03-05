// RCS-ID: $Id: Slur.h,v 1.3 2006/02/23 19:24:42 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file Slur.h
    @brief Header file for classes lmArch and lmTie
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __SLUR_H__        //to avoid nested includes
#define __SLUR_H__


//---------------------------------------------------------
//   lmArch
//---------------------------------------------------------

class lmArch
{
public:
    lmArch();
    ~lmArch() {}

    void SetStartPoint(lmMicrons xPos, lmMicrons yPos);
    void SetEndPoint(lmMicrons xPos, lmMicrons yPos);
    void SetCtrolPoint1(lmMicrons xPos, lmMicrons yPos);
    void SetCtrolPoint2(lmMicrons xPos, lmMicrons yPos);
    inline lmMicrons GetStartPosX() { return m_xStart; }
    inline lmMicrons GetStartPosY() { return m_yStart; }
    inline lmMicrons GetEndPosX() { return m_xEnd; }
    inline lmMicrons GetEndPosY() { return m_yEnd; }
    inline lmMicrons GetCtrol1PosX() { return m_xCtrol1; }
    inline lmMicrons GetCtrol1PosY() { return m_yCtrol1; }
    inline lmMicrons GetCtrol2PosX() { return m_xCtrol2; }
    inline lmMicrons GetCtrol2PosY() { return m_yCtrol2; }

    void Draw(wxDC* pDC, wxColour colorC);

protected:
    void CubicBezier(double* x, double* y, int nNumPoints);


    // start, end and control poins coordinates, absolute paper position
    lmMicrons    m_xStart, m_yStart;
    lmMicrons    m_xEnd, m_yEnd;
    lmMicrons    m_xCtrol1, m_yCtrol1;
    lmMicrons    m_xCtrol2, m_yCtrol2;

};


//---------------------------------------------------------
//   lmTie
//---------------------------------------------------------

class lmTie : public lmAuxObj
{
public:
    lmTie(lmNote* pStartNote, lmNote* pEndNote);
    ~lmTie();

    void Remove(lmNote* pNote);
    lmNote* GetStartNote() const { return m_pStartNote; }
    lmNote* GetEndNote() const { return m_pEndNote; }
    void SetStartPoint(lmMicrons xPos, lmMicrons yPos, lmMicrons xPaperRight, bool fUnderNote);
    void SetEndPoint(lmMicrons xPos, lmMicrons yPos, lmMicrons xPaperLeft);
    bool IsUnderNote() { return m_fTieUnderNote; }

    void UpdateMeasurements();

    // overrides for pure virtual methods of base classes
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC);

    wxBitmap* GetBitmap(double rScale) { return (wxBitmap*)NULL; }


protected:

    lmNote*     m_pStartNote;        //notes tied by this lmTie object
    lmNote*     m_pEndNote;
    lmArch      m_mainArc;          // the arc that normally renders the tie
    lmArch*     m_pExtraArc;        // in case the tie continues in the next system
    lmMicrons   m_xPaperLeft;
    lmMicrons   m_xPaperRight;
    bool        m_fTieUnderNote;    // tie must go under note

};

#endif    // __SLUR_H__
