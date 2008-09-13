//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_HANDLERS_H__        //to avoid nested includes
#define __LM_HANDLERS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Handlers.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/defs.h"      // lmLUnits
#include "../app/Paper.h"
#include "GMObject.h"

class lmController;

class lmHandler : public lmSimpleShape
{
public:
    virtual ~lmHandler() {}

protected:
    lmHandler(lmScoreObj* pOwner);

};


#define lmVERTICAL      true
#define lmHORIZONTAL    false


class lmShapeMargin : public lmHandler
{
public:
    lmShapeMargin(lmScore* pScore, int nIdx, int nPage, bool fVertical, lmLUnits uPos,
                  lmLUnits uLenght, wxColour color = *wxBLACK);
	~lmShapeMargin();

	//implementation of pure virtual methods in base class
    void Render(lmPaper* pPaper, wxColour color);
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

    //call backs
    void OnMouseIn(wxWindow* pWindow, lmUPoint& pointL);
	lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
    void OnEndDrag(lmController* pCanvas, const lmUPoint& uPos);


protected:
    void DrawLine(lmPaper* pPaper, wxColour color);
    void DrawHandlers(lmPaper* pPaper, wxColour color);

    //reference
    int         m_nIdx;         //used by the owner to identify which margin is this one
	int			m_nPage;		//page in which this margin is placed

    //position and size
    bool        m_fVertical;    //vertical line
    lmLUnits    m_uPos;         //xPos for vertical line, yPos for horizontal
    lmLUnits    m_uLenght;      //paper height for vertical, paper width for horizontal

    //geometry
    lmLUnits    m_uThighness;   //handler thighness
    lmLUnits    m_uWidth;       //handler width

};

#endif    // __LM_HANDLERS_H__

