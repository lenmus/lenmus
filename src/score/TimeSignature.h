// RCS-ID: $Id: TimeSignature.h,v 1.6 2006/02/23 19:24:42 cecilios Exp $
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
/*! @file TimeSignature.h
    @brief Header file for class lmTimeSignature
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __TIMESIGNATURE_H__        //to avoid nested includes
#define __TIMESIGNATURE_H__

//------------------------------------------------------------------------------------------------
// lmTimeSignature object
//------------------------------------------------------------------------------------------------

class lmTimeSignature: public lmSimpleObj
{
public:
    //constructors and destructor

        //constructor for types eTS_Common, eTS_Cut and eTS_SenzaMisura
    lmTimeSignature(ETimeSignatureType nType, lmVStaff* pVStaff, bool fVisible = true);
        //constructor for type eTS_SingleNumber
    lmTimeSignature(int nSingleNumber, lmVStaff* pVStaff, bool fVisible = true);
        //constructors for type eTS_Normal
    lmTimeSignature(int nBeats, int nBeatType, lmVStaff* pVStaff, bool fVisible = true);
    lmTimeSignature(ETimeSignature nTimeSign, lmVStaff* pVStaff, bool fVisible = true);
        //constructor for type eTS_Composite
    lmTimeSignature(int nNumBeats, int nBeats[], int nBeatType, lmVStaff* pVStaff,
                  bool fVisible = true);
        //constructor for type eTS_Multiple
    lmTimeSignature(int nNumFractions, int nBeats[], int nBeatType[], lmVStaff* pVStaff,
                  bool fVisible = true);

    ~lmTimeSignature() {}

    //other methods
    //ETimeSignature GetType() {return m_nTimeSignature;}

    //implementation of virtual methods defined in abstract base class lmStaffObj
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC);
    wxBitmap* GetBitmap(double rScale);
    void MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, wxPoint& ptOffset, 
                         const wxPoint& ptLog, const wxPoint& dragStartPosL, const wxPoint& ptPixels);
    wxPoint EndDrag(const wxPoint& pos);


    //    debugging
    wxString Dump();
    wxString SourceLDP();
    wxString SourceXML();

    //rendering related methods
    lmMicrons DrawAt(bool fMeasuring, wxDC* pDC, wxPoint pos, wxColour colorC = *wxBLACK);

    //sound related methods
    void AddMidiEvent(lmSoundManager* pSM, float rMeasureStartTime, int nMeasure);


private:
    void PrepareGlyphs();

    // get fixed measures and values that depend on lmTimeSignature type
    lmTenths GetSelRectHeight();
    lmTenths GetSelRectShift();
    lmTenths GetGlyphOffset();
    wxString GetLenMusChar();
    lmMicrons DrawTimeSignature(bool fMeasuring, lmPaper* pPaper, wxColour colorC = *wxBLACK);

        // member variables

    // attributes
    ETimeSignatureType    m_nType;

    int        m_nNumFractions;    // for type eTS_Multiple and eTS_Normal
        //eTS_Normal = 1,        // it is a single fraction
        //eTS_Common,            // it is 4/4 but represented by a C symbol
        //eTS_Cut,            // it is 2/4 but represented by a C/ simbol
        //sTS_SingleNumber,    // it is a single number with an implied denominator
        //eTS_Multiple,        // multiple fractions, i.e.: 2/4 + 3/8
        //eTS_Composite,        // composite fraction, i.e.: 3+2/8
        //eTS_SenzaMisura        // no time signature is present

    int        m_nBeats;
    int        m_nBeatType;

};

//
// global functions related to TimeSignatures
//
extern int GetNumUnitsFromTimeSignType(ETimeSignature nTimeSign);
extern int GetBeatTypeFromTimeSignType(ETimeSignature nTimeSign);
extern int GetNumBeatsFromTimeSignType(ETimeSignature nTimeSign);
extern float GetBeatDuration(ETimeSignature nTimeSign);
extern float GetBeatDuration(int nBeatType);
extern float GetMeasureDuration(ETimeSignature nTimeSign);
//extern bool IsBinaryTimeSignature(ETimeSignature nTimeSign);


#endif    // __TIMESIGNATURE_H__
