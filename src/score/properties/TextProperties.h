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

#ifndef WX_PRECOMP
#include "wx/wx.h"

#else
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/clrpicker.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/choice.h>
#include <wx/toolbar.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>

#endif

#include "../../widgets/Button.h"
#include "DlgProperties.h"


//--------------------------------------------------------------------------------------
/// Class lmTextProperties
//--------------------------------------------------------------------------------------

class lmScoreText;
class lmController;

class lmTextProperties : public lmPropertiesPage 
{
public:
	lmTextProperties(wxWindow* parent, lmScoreText* pParentText);
	~lmTextProperties();

    //implementation of pure virtual methods in base class
    void OnAcceptChanges(lmController* pController);

    // event handlers
    void OnBold(wxCommandEvent& event);
    void OnItalic(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnColor(wxColourPickerEvent& event);
    void OnStyle(wxCommandEvent& event);


protected:
    void CreateControls();
    void DoChangeFont(const wxFont& font, const wxColour& color = wxNullColour);

    //controls
	lmBitmapButton*          m_pBtCut;
	wxBitmapButton*          m_pBtCopy;
	wxBitmapButton*          m_pBtPaste;
	lmCheckButton*          m_pBtBold;
	wxBitmapButton*          m_pBtItalic;
	wxColourPickerCtrl*     m_pColourPicker;
	wxChoice*               m_pCboTextStyle;
	wxTextCtrl*             m_pTxtCtrl;
	
    //other variables
    lmScoreText*        m_pParentText;
    size_t              m_fontSize;     // in points


    DECLARE_EVENT_TABLE()
};


#endif //__LM_TEXTPROPERTIES_H__
