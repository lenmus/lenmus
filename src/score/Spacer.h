//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_SPACER__H_        //to avoid nested includes
#define __LM_SPACER__H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Spacer.cpp"
#endif

// lmSpacer is a graphical StaffObj with no graphical representation. It is just an empty
// space on the staff. The width can be zero.
class lmSpacer : public lmStaffObj
{
public:
    lmSpacer(lmVStaff* pStaff, long nID, lmTenths nWidth, int nStaff=1);
    ~lmSpacer() {}

    // properties
    inline float GetTimePosIncrement() { return 0; }
	inline wxString GetName() const { return _T("spacer"); }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //    debugging
    wxString Dump();
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

    //specific methods of this object


private:
    lmTenths        m_nSpacerWidth;

};


//lmAnchor: a spacer of 0 width
class lmAnchor: public lmSpacer
{
public:
    lmAnchor(lmVStaff* pVStaff, long nID, int nStaff=1)
        : lmSpacer(pVStaff, nID, 0.0f, nStaff) {}
};

class lmScoreAnchor: public lmStaffObj
{
public:
    lmScoreAnchor(lmVStaff* pVStaff, long nID, int nStaff=1);
    ~lmScoreAnchor() {}

    // properties
    inline float GetTimePosIncrement() { return 0; }
	inline wxString GetName() const { return _T("score anchor"); }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //    debugging
    wxString Dump();
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

};



#endif    // __LM_SPACER__H_

