//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2020 LenMus project
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
//---------------------------------------------------------------------------------------

#ifndef _LENMUS_OPTIONS_DLG_H_
#define _LENMUS_OPTIONS_DLG_H_

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_options_panel.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>


// Forward declarations
class wxSplitterWindow;
class wxTreeCtrl;


namespace lenmus
{


// Compatibility
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

//---------------------------------------------------------------------------------------
//To give options' panels a name
enum EOptionsPanels
{
    eOptLanguage = 0,
    eOptSound,
    eOptColors,
    eOptToolbars,
    eOptInternet,
    eOptShortcuts,
    eOptPlayback,
    eOptOther,
    //TO_ADD: Add, before this line, a new eOptXXXXXX code for the new options panel
    eOptMaxValue    // end of table
};


//---------------------------------------------------------------------------------------
class TreeItemData : public wxTreeItemData
{
public:
    TreeItemData(long nOpt) : m_opt(nOpt) { }
    long GetOptId() { return m_opt; }

private:
    long m_opt;
};

//---------------------------------------------------------------------------------------
// OptionsDlg: main frame for option panels
class OptionsDlg : public wxDialog
{
private:
    ApplicationScope&    m_appScope;

    // dialog controls
    wxSplitterWindow*    m_pSplitWindow;
    wxTreeCtrl*          m_pTreeCtrl;
    OptionsPanel*        m_pPanel;        // current displayed panel
    wxButton*            m_pBtnOK;
    wxButton*            m_pBtnCancel;

    //other member variables
    OptionsPanel*   m_cPanels[eOptMaxValue];        // options' panels collection
    long            m_nCurPanel;    // index to current displayed panel

public:
    OptionsDlg(wxWindow* parent, ApplicationScope& appScope);
    ~OptionsDlg();

    // event handler declarations
    void OnTreectrlItemSelected( wxTreeEvent& event );
    void OnButtonAcceptClick(wxCommandEvent& event);
    void OnButtonCancelClick(wxCommandEvent& event);

private:
    void CreateControls();
    bool SelectPanel(int nPanel);
    void CreateImageList();
    OptionsPanel* CreatePanel(EOptionsPanels nPanel);

    wxDECLARE_EVENT_TABLE();
};


}   // namespace lenmus

#endif		// _LENMUS_OPTIONS_DLG_H_
