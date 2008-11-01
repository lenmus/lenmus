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

#ifndef __LM_TEXTPROPERTIES_H__        //to avoid nested includes
#define __LM_TEXTPROPERTIES_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TextProperties.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

class wxRichTextCtrl;

#ifndef WX_PRECOMP
#include "wx/wx.h"

#else
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/panel.h>

#endif

#include "DlgProperties.h"
#include "../../score/defs.h"


//--------------------------------------------------------------------------------------
/// Class lmTextProperties
//--------------------------------------------------------------------------------------

class lmScoreText;
class lmController;
class lmScore;

class lmTextProperties : public lmPropertiesPage
{
public:
	lmTextProperties(wxWindow* parent, lmScoreText* pParentText);
	~lmTextProperties();

    //implementation of pure virtual methods in base class
    void OnAcceptChanges(lmController* pController);

    // event handlers
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnLeft(wxCommandEvent& event);
    void OnCenter(wxCommandEvent& event);
    void OnRight(wxCommandEvent& event);
    void OnStyle(wxCommandEvent& event);
    void OnEditStyles(wxCommandEvent& event);


protected:
    void CreateControls();
    void DoChangeFont(const wxFont& font, const wxColour& color = wxNullColour);
	void DoChangeAlignment();
	void DoChangeStyle(lmTextStyle* pStyle);

    //controls
	wxBitmapButton*     m_pBtCut;
	wxBitmapButton*     m_pBtCopy;
	wxBitmapButton*     m_pBtPaste;
	wxBitmapButton*		m_pBtLeft;
	wxBitmapButton*		m_pBtCenter;
	wxBitmapButton*		m_pBtRight;
	wxStaticText*       m_pTxtStyle;
	wxChoice*           m_pCboTextStyle;
    wxButton*           m_pBtnChangeStyles;
	wxRichTextCtrl*		m_pTxtCtrl;

    //other variables
    lmScoreText*        m_pParentText;
    lmScore*            m_pScore;

	lmEHAlign			m_nHAlign;


    DECLARE_EVENT_TABLE()
};


#endif //__LM_TEXTPROPERTIES_H__
