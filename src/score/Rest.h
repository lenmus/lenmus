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
/*! @file Rest.h
    @brief Header file for class lmRest
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __REST_H__        //to avoid nested includes
#define __REST_H__

#include "Glyph.h"


class lmRest: public lmNoteRest
{
public:
    lmRest(lmVStaff* pVStaff, 
        ENoteType nNoteType, float rDuration,
        bool fDotted, bool fDoubleDotted,
        int nStaff,
        lmContext* pContext, 
        bool fBeamed, lmTBeamInfo BeamInfo[]);

    ~lmRest();

    //implementation of virtual methods of base classes
        // lmScoreObj
    void SetLeft(lmLUnits nLeft);
        // lmStaffObj
    wxBitmap*   GetBitmap(double rScale);
    void        DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight);
    void        MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& ptOffset, 
                            const lmUPoint& ptLog, const lmUPoint& dragStartPosL, const lmDPoint& ptPixels);
    lmUPoint    EndDrag(const lmUPoint& pos);

    wxString    Dump();
    wxString    SourceLDP();
    wxString    SourceXML();

        //lmCompositeObj
    lmScoreObj* FindSelectableObject(lmUPoint& pt);

        //lmNoteRest
    bool IsInChord() { return false; }        

    //lmRest methods
    void SetDisplacement(lmTenths yShift) { m_yShift = yShift; }



private:
    // access to glyph data to define character to use and selection rectangle 
    lmEGlyphIndex GetGlyphIndex();

        // member variables

    lmTenths    m_yShift;        


};


#endif    // __REST_H__
