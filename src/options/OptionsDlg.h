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

#ifndef _OPTIONSDLG_H_
#define _OPTIONSDLG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "OptionsDlg.cpp"
#endif

#include "wx/splitter.h"
#include "wx/treectrl.h"

// Forward declarations
class wxSplitterWindow;
class wxTreeCtrl;

//Panels
#include "OptionsPanel.h"

// control identifiers
enum {
    ID_DIALOG = 2100,
    ID_SPLITTERWINDOW,
    ID_TREECTRL,
    ID_PANEL,
    ID_BUTTON_ACCEPT
};

// Compatibility
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

//declare an array for the options' panels
#include "wx/dynarray.h"
WX_DEFINE_ARRAY_PTR(lmOptionsPanel*, ArrayOfPanels);

//To give options' panels a name
enum EOptionsPanels {
    eOptLanguage = 0,
    eOptColors,
    eOptToolbars,
    eOptInternet,
    eOptOther,
    //TO_ADD: Add, before this line, a new eOptXXXXXX code for the new options panel
    eOptMaxValue    // end of table 
};


class lmTreeItemData : public wxTreeItemData
{
public:
    lmTreeItemData(long nOpt) : m_opt(nOpt) { }
    long GetOptId() { return m_opt; }

private:
    long m_opt;
};

// lmOptionsDlg class declaration

class lmOptionsDlg: public wxDialog
{    
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    lmOptionsDlg(wxWindow* parent, wxWindowID id = ID_DIALOG,
        const wxString& caption = _("Configuration options"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(640, 480),
        long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_MODAL );
    ~lmOptionsDlg();

    // event handler declarations
    void OnTreectrlItemSelected( wxTreeEvent& event );
    void OnButtonAcceptClick(wxCommandEvent& event);

private:
    void CreateControls();
    bool SelectPanel(int nPanel);
    void CreateImageList();
    lmOptionsPanel* CreatePanel(EOptionsPanels nPanel);

    // dialog controls
    wxSplitterWindow*    m_pSplitWindow;
    wxTreeCtrl*          m_pTreeCtrl;
    lmOptionsPanel*      m_pPanel;        // current displayed panel
    wxButton*            m_pBtnOK;
    wxButton*            m_pBtnCancel;
    wxButton*            m_pBtnHelp;

    //other member variables
    ArrayOfPanels   m_cPanels;        // options' panels collection
    long            m_nCurPanel;    // index to current displayed panel

};

#endif
    // _OPTIONSDLG_H_
