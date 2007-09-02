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

#ifndef __TIMESIGNATURE_H__        //to avoid nested includes
#define __TIMESIGNATURE_H__

#ifdef __GNUG__
#pragma interface "TimeSignature.cpp"
#endif

//------------------------------------------------------------------------------------------------
// lmTimeSignature object
//------------------------------------------------------------------------------------------------

class lmTimeSignature: public lmStaffObj
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

    //implementation of virtual methods defined in abstract base class lmStaffObj
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight);
    wxBitmap* GetBitmap(double rScale);
    void MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& ptOffset,
                         const lmUPoint& ptLog, const lmUPoint& uDragStartPos, const lmDPoint& ptPixels);
    lmUPoint EndDrag(const lmUPoint& uPos);


    //    debugging
    wxString Dump();
    wxString SourceLDP();
    wxString SourceXML();

    //rendering related methods
    lmLUnits DrawAt(bool fMeasuring, lmPaper* pPaper, lmUPoint uPos, wxColour colorC = *wxBLACK);

    //sound related methods
    void AddMidiEvent(lmSoundManager* pSM, float rMeasureStartTime, int nMeasure);

    // access to information
    int GetNumBeats() { return m_nBeats; }
    int GetBeatType() { return m_nBeatType; }

private:
    void PrepareGlyphs();

    // get fixed measures and values that depend on lmTimeSignature type
    lmTenths GetSelRectHeight();
    lmTenths GetSelRectShift();
    lmTenths GetGlyphOffset();
    wxString GetLenMusChar();
    lmLUnits DrawTimeSignature(bool fMeasuring, lmPaper* pPaper, wxColour colorC = *wxBLACK);

        // member variables

    // attributes
    ETimeSignatureType    m_nType;

    int        m_nNumFractions;    // for type eTS_Multiple and eTS_Normal
        //eTS_Normal = 1,       // it is a single fraction
        //eTS_Common,           // it is 4/4 but represented by a C symbol
        //eTS_Cut,              // it is 2/4 but represented by a C/ simbol
        //sTS_SingleNumber,     // it is a single number with an implied denominator
        //eTS_Multiple,         // multiple fractions, i.e.: 2/4 + 3/8
        //eTS_Composite,        // composite fraction, i.e.: 3+2/8
        //eTS_SenzaMisura       // no time signature is present

    int         m_nBeats;
    int         m_nBeatType;
    lmLUnits    m_xPosTop;      //x position for Beats or single char (i.e. common time)
    lmLUnits    m_xPosBottom;   //x position for BeatsType

};

//
// global functions related to TimeSignatures, and some definitions used by them
//
#define lmNON_CHORD_NOTE    -1

extern int GetNumUnitsFromTimeSignType(ETimeSignature nTimeSign);
extern int GetBeatTypeFromTimeSignType(ETimeSignature nTimeSign);
extern int GetNumBeatsFromTimeSignType(ETimeSignature nTimeSign);
extern float GetBeatDuration(ETimeSignature nTimeSign);
extern float GetBeatDuration(int nBeatType);
extern float GetMeasureDuration(ETimeSignature nTimeSign);
//extern bool IsBinaryTimeSignature(ETimeSignature nTimeSign);
extern int AssignVolume(float rTimePos, int nBeats, int nBeatType);
extern int GetNoteBeatPosition(float rTimePos, int nBeats, int nBeatType);
extern int GetChordPosition(float rTimePos, float rDuration, int nBeats, int nBeatType);

#endif    // __TIMESIGNATURE_H__

