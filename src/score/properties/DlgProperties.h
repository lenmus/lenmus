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

#ifndef __LM_DLGPROPERTIES_H__        //to avoid nested includes
#define __LM_DLGPROPERTIES_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgProperties.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//#ifndef WX_PRECOMP
//#include "wx/wx.h"

 //#else
 #include <wx/intl.h>
 #include <wx/string.h>
 #include <wx/textctrl.h>
 #include <wx/gdicmn.h>
 #include <wx/font.h>
 #include <wx/colour.h>
 #include <wx/settings.h>
 #include <wx/sizer.h>
 #include <wx/panel.h>
 #include <wx/bitmap.h>
 #include <wx/image.h>
 #include <wx/icon.h>
 #include <wx/notebook.h>
 #include <wx/button.h>
 #include <wx/dialog.h>

//#endif

#include <list>

class lmTextItem;
class lmController;


//---------------------------------------------------------------------------------------
// Abstract class lmPropertiesPage: the property pages to display
//---------------------------------------------------------------------------------------

class lmPropertiesPage : public wxPanel
{
public:
    virtual ~lmPropertiesPage() {}

    virtual void OnAcceptChanges(lmController* pController)=0;
    virtual void OnCancelChanges() {}

protected:
    lmPropertiesPage(wxWindow* parent) :
         wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 250), wxTAB_TRAVERSAL) {}

};



//---------------------------------------------------------------------------------------
// class lmDlgProperties: the dialog to display property pages
//---------------------------------------------------------------------------------------

class lmDlgProperties : public wxDialog
{
public:
	lmDlgProperties(lmController* pController);
    ~lmDlgProperties();

    // event handlers
    void OnAccept(wxCommandEvent& WXUNUSED(event));
    void OnCancel(wxCommandEvent& WXUNUSED(event));

    //panels
    inline wxNotebook* GetNotebook() { return m_pNotebook; }
    void AddPanel(lmPropertiesPage* pPanel, const wxString& sTabName);


protected:
    void CreateControls();

    lmController*   m_pController;
    wxNotebook*     m_pNotebook;
    wxButton*       m_pBtAccept;
    wxButton*       m_pBtCancel;
    wxBoxSizer*     m_pMainSizer;
    std::list<lmPropertiesPage*>    m_pPages;


    DECLARE_EVENT_TABLE()
};


#endif //__LM_DLGPROPERTIES_H__
