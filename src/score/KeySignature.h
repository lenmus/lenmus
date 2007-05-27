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

#ifdef __GNUG__
#pragma interface "KeySignature.cpp"
#endif

#ifndef __KEYSIGNATURE_H__        //to avoid nested includes
#define __KEYSIGNATURE_H__

//------------------------------------------------------------------------------------------------
// lmKeySignature object
//------------------------------------------------------------------------------------------------

class lmKeySignature: public lmStaffObj
{
public:
    //constructors and destructor

        //constructor for traditional key signatures
    lmKeySignature(int nFifths, bool fMajor, lmVStaff* pVStaff, bool fVisible=true);

    ~lmKeySignature() {}

    //other methods
    EKeySignatures GetKeyType() { return m_nKeySignature; }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight);
    wxBitmap* GetBitmap(double rScale);
    void MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& ptOffset,
                         const lmUPoint& ptLog, const lmUPoint& dragStartPosL, const lmDPoint& ptPixels);
    lmUPoint EndDrag(const lmUPoint& pos);


    //    debugging
    wxString Dump();
    wxString SourceLDP();
    wxString SourceXML();

    //rendering related methods
    lmLUnits DrawAt(bool fMeasuring, lmPaper* pPaper, lmUPoint pos, EClefType nClef,
                     int nStaff, wxColour colorC = *wxBLACK);

    //methods for hiding the key in prologs
    void Hide(bool fHide) { m_fHidden = fHide; }



private:
    void SetKeySignatureType();
    lmLUnits DrawAccidental(bool fMeasuring, lmPaper* pPaper, EAccidentals nAlter,
                    lmLUnits nxLeft, lmLUnits nyTop, int nStaff);

    // get fixed measures and values that depend on lmKeySignature type
    lmLUnits DrawKeySignature(bool fMeasuring, lmPaper* pPaper, wxColour colorC = *wxBLACK);

        // member variables

    bool              m_fHidden;          //to hide it in system prolog
    bool              m_fTraditional;     //it's a traditional signature. Encoded by the
                                          // redundant enumaeration and fifths/mode pair
    EKeySignatures    m_nKeySignature;
    int               m_nFifths;
    bool              m_fMajor;

};

//
// global functions related to KeySignatures
//
extern void ComputeAccidentals(EKeySignatures nKeySignature, int nAccidentals[]);
extern int GetRootNoteIndex(EKeySignatures nKeySignature);
extern bool IsMajor(EKeySignatures nKeySignature);
extern const wxString& GetKeySignatureName(EKeySignatures nKeySignature);
extern int KeySignatureToNumFifths(EKeySignatures nKeySignature);
extern EKeySignatures GetRelativeMinorKey(EKeySignatures nMajorKey);

#endif    // __KEYSIGNATURE_H__

