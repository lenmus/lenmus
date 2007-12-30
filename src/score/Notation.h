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

#ifndef __LM_NOTATION_H__        //to avoid nested includes
#define __LM_NOTATION_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "Notation.cpp"
#endif

// lmNotations are graphical StaffObjs, i.e.: breath mark, spacer, symbol
// This is an abstract class so specific notations must derive from this one
// lmNotations do not consume time

enum lmENotationType
{
    eNT_Anchor = 1,         // anchor to attach AuxObjs to a VStaff
    eNT_ScoreAnchor,        // anchor to attach AuxObjs to the score
    eNT_Spacer,             // spacer (lmSpacer)
};

class lmNotation : public lmStaffObj
{
public:
    virtual ~lmNotation() {}

    // properties related to the clasification of this lmStaffObj
    float GetTimePosIncrement() { return 0; }
    virtual lmENotationType GetNotationType() = 0;

	wxString GetName() const { return _T("notation"); }



protected:
    lmNotation(lmVStaff* pVStaff, int nStaff=1,
               bool fVisible = true, 
               bool fIsDraggable = false);

};

// lmSpacer is a graphical StaffObj with no graphical representation. It is just an empty
// space on the staff. The width can be zero.
class lmSpacer : public lmNotation
{
public:
    lmSpacer(lmVStaff* pStaff, lmTenths nWidth, int nStaff=1);
    ~lmSpacer() {}

    // properties
    lmENotationType GetNotationType() { return eNT_Spacer; }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //    debugging
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);

    //specific methods of this object


private:
    lmTenths        m_nSpacerWidth;

};

class lmAnchor: public lmNotation
{
public:
    lmAnchor(lmVStaff* pStaff, int nStaff=1);
    ~lmAnchor() {}

    // properties
    lmENotationType GetNotationType() { return eNT_Anchor; }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //    debugging
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);

};

class lmScoreAnchor: public lmNotation
{
public:
    lmScoreAnchor(lmVStaff* pStaff, int nStaff=1);
    ~lmScoreAnchor() {}

    // properties
    lmENotationType GetNotationType() { return eNT_ScoreAnchor; }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //    debugging
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);

};



#endif    // __LM_NOTATION_H__

