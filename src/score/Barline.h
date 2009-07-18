//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#ifndef __LM_BARLINE_H__        //to avoid nested includes
#define __LM_BARLINE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Barline.cpp"
#endif

#include "wx/dc.h"
#include "wx/bmpcbox.h"

class lmDlgProperties;

//------------------------------------------------------------------------------------------------
// lmBarline object
//------------------------------------------------------------------------------------------------

class lmBarline: public lmStaffObj
{
public:
    //constructor and destructor
    lmBarline(lmEBarline nBarlineType, lmVStaff* pStaff, long nID, bool fVisible);
    ~lmBarline();

	wxString GetName() const { return _T("barline"); }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);

        //specific methods for barline

    //info
    inline lmEBarline GetBarlineType() { return m_nBarlineType; }
	inline void SetBarlineType(lmEBarline nType) { m_nBarlineType = nType; }

    //source code and debugging
    wxString Dump();
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

	//edit properties
	void OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName = wxEmptyString);


private:
    lmEBarline          m_nBarlineType;             //type of barline

};


//-----------------------------------------------------------------------------------------
// Utility global definitions and functions related to barlines
//-----------------------------------------------------------------------------------------

//info about barlines, to centralize data about barlines
class lmBarlinesDBEntry
{
public:
    lmBarlinesDBEntry() {}
    lmBarlinesDBEntry(wxString name, lmEBarline type)
        : sBarlineName(name), nBarlineType(type) {}


    wxString		sBarlineName;
    lmEBarline		nBarlineType;
};

// Utility global functions to use a wxBitmapComboBox control to display barlines
extern void LoadBarlinesBitmapComboBox(wxBitmapComboBox* pCtrol, lmBarlinesDBEntry tBarlines[]);
extern void SelectBarlineBitmapComboBox(wxBitmapComboBox* pCtrol, lmEBarline nType);

// other global functions
extern wxString GetBarlineLDPNameFromType(lmEBarline nBarlineType);
extern const wxString& GetBarlineName(lmEBarline nBarlineType);


#endif    // __LM_BARLINE_H__

