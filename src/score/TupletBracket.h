// RCS-ID: $Id: TupletBracket.h,v 1.3 2006/02/23 19:25:14 cecilios Exp $
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
/*! @file TupletBracket.h
    @brief Header file for class lmTupletBracket
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __TUPLETBRACKET_H__        //to avoid nested includes
#define __TUPLETBRACKET_H__


/*
    A lmTupletBracket represents the optional bracket graphically associated
    to tuplets. The lmTupletBracket object does not have any effect on sound. It is only to  
    describe how a tuplet must be displayed.

    As it is a pure graphical object there is no need to keep links to the notes/rests
    that are grouped by this bracket, unless needed for score edition. So I will pospone
    any decision until I have more information. So, for now I will treat the bracket as
    a pure graphical element, not associated to any note, and I will record here the new
    ideas as they happen.

    1. I'm affraid that there is a need to keep, at least, links to start and end NoteRests
    so that it is possible to stablish start and end positioning points (as if the bracket
    where a tie).

    2. For bracket positioning it is necessry to take into consideration intermediate notes,
    as if a tuplet where a beamed group (this is usually the case)

    So for now, I will treat TupleBracket analogously to a beamed group

    
*/

class lmTupletBracket : public lmSimpleObj
{
public:
    lmTupletBracket(bool fShowNumber, int nNumber, bool fBracket, bool fAbove);
    ~lmTupletBracket();

    //overrides of virtual methods defined in base classes
        //lmScoreObj
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC);
    wxBitmap* GetBitmap(double rScale) { return (wxBitmap*)NULL; }
    void SetFont(lmPaper* pPaper);
        //lmStaffObj
    wxString Dump();
    wxString SourceLDP();
    wxString SourceXML();

    //specific method of this object
    void        Include(lmNoteRest* pNR);
    void        Remove(lmNoteRest* pNR);
    int            NumNotes();
    lmNoteRest*    GetStartNote();
    lmNoteRest*    GetEndNote();
    void        AutoPosition();



protected:
    void ComputePosition();


    NoteRestsList    m_cNotes;    // list of notes/rest grouped by this bracket. 
                                // For chords only the base note of the chord 
                                // is included in this list
    lmMicrons    m_xPaperLeft;
    lmMicrons    m_xPaperRight;

    // graphical attributes
    bool    m_fShowNumber;        // display tuplet number
    int        m_nTupletNumber;    // number to display
    bool    m_fBracket;            // display bracket
    bool    m_fAbove;            // bracket positioned above the notes

    // start and end poins coordinates, relative to start note current paper position
    lmMicrons    m_xStart;
    lmMicrons    m_yStart;
    lmMicrons    m_xEnd;
    lmMicrons    m_yEnd;

    //For rendering tuplet number. TODO: Replace in future by lmBasicText derivation?
    wxString    m_sFontName;
    int            m_nFontSize;
    bool        m_fBold;
    bool        m_fItalic;


};

#endif    // __TUPLETBRACKET_H__

