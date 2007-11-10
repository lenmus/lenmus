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
#pragma implementation "TupletBracket.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"

//---------------------------------------------------------
//   lmTupletBracket implementation
//---------------------------------------------------------

lmTupletBracket::lmTupletBracket(bool fShowNumber, int nNumber, bool fBracket, bool fAbove,
                                 int nActualNotes, int nNormalNotes)
{
    m_fShowNumber = fShowNumber;
    m_nTupletNumber = nNumber;
    m_fBracket = fBracket;
    m_fAbove = fAbove;
    m_nActualNotes = nActualNotes;
    m_nNormalNotes = nNormalNotes;
	m_pShape = (lmShapeTuplet*)NULL;

    //TODO: Allow user to change this values
    m_sFontName = _T("Arial");
    m_nFontSize = PointsToLUnits(8);
    m_fBold = false;
    m_fItalic = true;

}

lmTupletBracket::~lmTupletBracket()
{
    //do not delete note/rests. They are owned by the VStaff
    //m_cNotes.clear();
}

void lmTupletBracket::Include(lmNoteRest* pNR)
{
	m_cNotes.push_back(pNR);
}

lmShape* lmTupletBracket::LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour color)
{
	//AWARE: Althoug shape pointer is initialized to NULL never assume that there is
	//a shape if not NULL, as the shape is deleted in the graphic model.
	m_pShape = (lmShapeTuplet*)NULL;
    if (!m_fBracket) return m_pShape;

	//lmShape* pNoteShape = GetStartNote()->GetShap2();
 //   lmLUnits xStart = pNoteShape->GetXLeft();
 //   lmLUnits yStart = (m_fAbove ? pNoteShape->GetYTop() : pNoteShape->GetYBottom() );
	//pNoteShape = GetEndNote()->GetShap2();
 //   lmLUnits xEnd = pNoteShape->GetXRight();
 //   lmLUnits yEnd = (m_fAbove ? pNoteShape->GetYTop() : pNoteShape->GetYBottom() );

	wxString sNumber = wxString::Format(_T("%d"), m_nTupletNumber);

	//prepare the font
    int nWeight = (m_fBold ? wxBOLD : wxNORMAL);
    int nStyle = (m_fItalic ? wxITALIC : wxNORMAL);
    wxFont* pFont = pPaper->GetFont(m_nFontSize, m_sFontName, wxDEFAULT, nStyle,
									nWeight, false);
    if (!pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmTupletBracket::SetFont"), wxOK);
        ::wxExit();
    }

	//create the shape
	m_pShape = new lmShapeTuplet(GetStartNote(), GetEndNote(), NumNotes(), m_fAbove,
								 m_fShowNumber, sNumber, pFont, color, lm_eSquared);
	pBox->AddShape(m_pShape);

	//attach the tuplet to start and end notes
	GetStartNote()->GetShap2()->Attach(m_pShape, eGMA_StartNote);
	GetEndNote()->GetShap2()->Attach(m_pShape, eGMA_EndNote);

	return m_pShape;
}

int lmTupletBracket::FindNote(lmNoteRest* pNR)
{
    //find a note/rest
    for (int i=0; i < (int)m_cNotes.size(); i++)
    {
        if (m_cNotes[i]->GetID() == pNR->GetID())
			return i;
	}
	return -1;
}


void lmTupletBracket::Remove(lmNoteRest* pNR)
{
    //find note/rest to remove
	int i = FindNote(pNR);

	//if found, remove note
   if (i != -1)
		m_cNotes.erase(m_cNotes.begin()+i);

}

