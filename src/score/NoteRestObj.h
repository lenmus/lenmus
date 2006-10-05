// RCS-ID: $Id: NoteRestObj.h,v 1.3 2006/02/23 19:23:54 cecilios Exp $
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
/*! @file NoteRestObj.h
    @brief Header file for class lmNoteRestObj
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __NOTERESTOBJ_H__        //to avoid nested includes
#define __NOTERESTOBJ_H__

enum ESymbolType {
    eST_Fermata = 0,        // ESP: calderón
    eST_Lyric,
    eST_Accidental            // ESP: alteración
    //Public Enum EGrafObjs
    //    eGO_Espacio = 1     'espaciado fijo
    //    eGO_Respiracion     'marca de respiración
};

enum ESyllabicTypes {
    eSyllabicSingle = 0,
    eSyllabicBegin,
    eSyllabicMiddle,
    eSyllabicEnd
};



class lmNoteRestObj : public lmAuxObj
{
public:
    lmNoteRestObj(ESymbolType nType, lmNoteRest* pOwner);
    ~lmNoteRestObj() {}

    // overrides for pure virtual methods of base classes
        // lmScoreObj
    virtual void SetFont(lmPaper* pPaper) {}
    virtual void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC,
                            bool fHighlight)=0;
    virtual wxBitmap* GetBitmap(double rScale) { return (wxBitmap*)NULL; }
    virtual wxString Dump() { return _T(""); }

    // specific methods of this class
    ESymbolType GetSymbolType() { return m_nSymbolType; }
    virtual void SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
                         lmLUnits xPos, lmLUnits yPos)=0;
    virtual void UpdateMeasurements();

    virtual void SetOwner(lmNoteRest* pOwner) { m_pOwner = pOwner; }


protected:
    ESymbolType        m_nSymbolType;
    lmNoteRest*        m_pOwner;            // lmNoteRest to which this lmNoteRestObj is associated

    // specific data for lmFermata symbol
    bool    m_fOverNote;
};

class lmFermata : public lmNoteRestObj
{
public:
    lmFermata(lmNoteRest* pOwner, bool fOverNote);
    ~lmFermata() {}

    // overrides for pure virtual methods of base class lmNoteRestObj
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight);
    void SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
                         lmLUnits xPos, lmLUnits yPos);

private:
    bool    m_fOverNote;

};




class lmLyric : public lmNoteRestObj, public lmBasicText
{
public:
    lmLyric(lmNoteRest* pOwner, wxString sText, ESyllabicTypes nSyllabic = eSyllabicSingle,
            int nNumLine=1, wxString sLanguage=_T("it") );
    ~lmLyric() {}

    // definitions for pure virtual methods of base class lmNoteRestObj
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight);
    void SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
                         lmLUnits xPos, lmLUnits yPos);

    // overrides for virtual methods of base class lmNoteRestObj
    void SetOwner(lmNoteRest* pOwner);

    // overrides for virtual methods of base class lmScoreObj
    void SetFont(lmPaper* pPaper);
    wxString Dump();


private:
    int                m_nNumLine;
    lmVStaff*            m_pVStaff;            // lmVStaff to which the owner NoterRest belongs
    int            m_nStaffNum;        // Staff (1..n) on which owner NoterRest is located

};

#endif    // __NOTERESTOBJ_H__

