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
/*! @file KeySignature.h
    @brief Header file for class lmKeySignature
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __KEYSIGNATURE_H__        //to avoid nested includes
#define __KEYSIGNATURE_H__

//------------------------------------------------------------------------------------------------
// lmKeySignature object
//------------------------------------------------------------------------------------------------

class lmKeySignature: public lmSimpleObj
{
public:
    //constructors and destructor

        //constructor for traditional key signatures
    lmKeySignature(int nFifths, bool fMajor, lmVStaff* pVStaff, bool fVisible=true);

    ~lmKeySignature() {}

    //other methods
    EKeySignatures GetType() { return m_nKeySignature; }

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
    lmLUnits DrawAt(bool fMeasuring, wxDC* pDC, wxPoint pos, EClefType nClef, 
                     int nStaff, wxColour colorC = *wxBLACK);

    //methods for hiding the key in prologs
    void Hide(bool fHide) { m_fHidden = fHide; }



private:
    void SetKeySignatureType();
    lmLUnits DrawAccidental(bool fMeasuring, wxDC* pDC, EAccidentals nAlter,
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


#endif    // __KEYSIGNATURE_H__

