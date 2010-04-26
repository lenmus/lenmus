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

#ifndef __LM_SHAPETUPLET_H__        //to avoid nested includes
#define __LM_SHAPETUPLET_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ShapeTuplet.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "../score/defs.h"      // lmLUnits
#include "../app/Paper.h"
#include "GMObject.h"

class lmNoteRest;

enum lmETupletDesign
{
	lm_eSquared = 0,
};


class lmShapeTuplet : public lmSimpleShape
{
public:
    lmShapeTuplet(lmNoteRest* pStartNR, lmNoteRest* pEndNR, int nNumNotes, bool fAbove,
				  bool fShowNumber, wxString sNumber, wxFont* pFont, 
				  wxColour color = *wxBLACK, lmETupletDesign nDesign = lm_eSquared);

	~lmShapeTuplet();

	//implementation of pure virtual methods in base class
    void Render(lmPaper* pPaper, wxColour color);
    void DrawControlPoints(lmPaper* pPaper);
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

	//layout changes
	void OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag,
								lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent);


protected:
	//parent notes/rests
	lmNoteRest*		m_pStartNR;
	lmNoteRest*		m_pEndNR;
	int				m_nNumNotes;

    //common attributes for any tuplet bracket
    lmLUnits		m_uxStart, m_uyStart;
    lmLUnits		m_uxEnd, m_uyEnd;
    wxString		m_sNumber;
	bool			m_fAbove;
    bool			m_fShowNumber;
    lmETupletDesign	m_nTupletDesign;
	wxFont*			m_pFont;			//for tuplet number

	//geometry for type 'squared'
	lmLUnits	m_uLineWidth;		//line width
    lmLUnits    m_dyBorder;			//lenght of vertical borders

};

#endif    // __LM_SHAPETUPLET_H__

