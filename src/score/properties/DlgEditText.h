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

#ifndef __LM_DLGEDITTEXT_H__        //to avoid nested includes
#define __LM_DLGEDITTEXT_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgEditText.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


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
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

//--------------------------------------------------------------------------------------
/// Class lmDlgEditText
//--------------------------------------------------------------------------------------

class lmScoreText;
class lmController;

class lmDlgEditText : public wxDialog 
{
public:
	lmDlgEditText(lmController* pController, lmScoreText* pText);
	~lmDlgEditText();

    // event handlers
    void OnBold(wxCommandEvent& event);
    void OnItalic(wxCommandEvent& event);
    void OnUnderline(wxCommandEvent& event);

    void OnAccept(wxCommandEvent& WXUNUSED(event));
    void OnCancel(wxCommandEvent& WXUNUSED(event));

    //void OnwxPointerFont(wxCommandEvent& event);

    //void OnTestTextValue(wxCommandEvent& event);
    //void OnViewMsg(wxCommandEvent& event);
    //void OnSelectFont(wxCommandEvent& event);
    //void OnEnumerateFamiliesForEncoding(wxCommandEvent& event);
    //void OnEnumerateFamilies(wxCommandEvent& WXUNUSED(event))
    //    { DoEnumerateFamilies(false); }
    //void OnEnumerateFixedFamilies(wxCommandEvent& WXUNUSED(event))
    //    { DoEnumerateFamilies(true); }
    //void OnEnumerateEncodings(wxCommandEvent& event);

    //void OnSetNativeDesc(wxCommandEvent& event);
    //void OnSetNativeUserDesc(wxCommandEvent& event);
    //void OnSetFaceName(wxCommandEvent& event);
    //void OnSetEncoding(wxCommandEvent& event);

protected:
    void CreateControls();

    //bool DoEnumerateFamilies(bool fixedWidthOnly,
    //                         wxFontEncoding encoding = wxFONTENCODING_SYSTEM,
    //                         bool silent = false);

    //void DoResizeFont(int diff);
    void DoChangeFont(const wxFont& font, const wxColour& color = wxNullColour);

    // ask the user to choose an encoding and return it or
    // wxFONTENCODING_SYSTEM if the dialog was cancelled
    //wxFontEncoding GetEncodingFromUser();


    //controls
	wxToolBar* m_pToolBar;
	wxColourPickerCtrl* m_pColourPicker;
	wxChoice* m_pCboFontName;
	wxChoice* m_pCboFontSize;
	wxTextCtrl* m_pTxtCtrl;
	
	wxButton* m_pBtAccept;
	
	wxButton* m_pBtCancel;
	
    //other variables
    lmController*       m_pController;
    lmScoreText*        m_pParentText;
    size_t              m_fontSize;     // in points


    DECLARE_EVENT_TABLE()
};

#endif //__noname__
