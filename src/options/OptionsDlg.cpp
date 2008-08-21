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

//-----------------------------------------------------------------------------------
//AWARE
//
//    Things to do to add a new options panel to the Options Dialog:
//     1. Create a new panel class derived from lmOptionsPanel
//     2. Add the XRC panel to TheApp.cpp
//     3. Look for "//TO_ADD:" tags in OptionsDlg.h and follow instructions there
//     4. Look for "//TO_ADD:" tags in this file and follow instructions there
//
//-----------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "OptionsDlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"

#include "OptionsDlg.h"
#include "../app/ArtProvider.h"        // to use ArtProvider for managing icons

#include "wx/xrc/xmlres.h"

// Panels
#include "OptionsPanel.h"
#include "LangOptionsPanel.h"
#include "ColorsOptPanel.h"
#include "ToolbarsOptPanel.h"
#include "InternetOptPanel.h"
#include "OtherOptionsPanel.h"
//TO_ADD: add here the new panel include file

//access to preferences object
#include "wx/config.h"
extern wxConfigBase* g_pPrefs;

//Enum to give the icons indexes a name
enum
{
    eIconRoot = 0,        //AWARE: first one must be 0. The others consecutive.
    eIconLanguages,
    eIconColors,
    eIconToolbars,
    eIconInternet,
    eIconOther,
    //TO_ADD: add the new element
    TreeCtrlIcon_EOF        //AWARE: Must be the last one. Just to know how many items
};

//String IDs to select the icons to use. Must match previous enum
static wxString sImageID[] = {
    _T("tool_options"),
    _T("opt_language"),
    _T("opt_colors"),
    _T("opt_tools"),
    _T("opt_internet"),
    _T("opt_other")
//TO_ADD: Add image identifier here
};



BEGIN_EVENT_TABLE( lmOptionsDlg, wxDialog )
    EVT_TREE_SEL_CHANGING( ID_TREECTRL, lmOptionsDlg::OnTreectrlItemSelected )
    EVT_BUTTON( ID_BUTTON_ACCEPT, lmOptionsDlg::OnButtonAcceptClick )
END_EVENT_TABLE()


lmOptionsDlg::~lmOptionsDlg()
{
    WX_CLEAR_ARRAY( m_cPanels );
}

lmOptionsDlg::lmOptionsDlg( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    // dialog controls initialisation
    m_pSplitWindow = NULL;
    m_pTreeCtrl = NULL;
    m_pPanel = NULL;
    m_pBtnOK = NULL;
    m_pBtnCancel = NULL;
    m_pBtnHelp = NULL;

    //initialize panel's array
    lmOptionsPanel* pPanel = (lmOptionsPanel*)NULL;
    int i;
    for (i=0; i < eOptMaxValue; i++) {
        m_cPanels.Add(pPanel);
    }

    //Get last used panel
    m_nCurPanel = g_pPrefs->Read(_T("/UserOptions/OptionsPanel"), 0L);
    if (m_nCurPanel < 0 || m_nCurPanel >= eOptMaxValue)
        m_nCurPanel = 0;


    /* Code removed
        I was trying to create the dialog using XML resorces but I did not get 
        the panels working propely. So, as this dialog is very simple (the complexity
        is on the panels) I will leave the old code rendering the dialog by program

    // create the Options Dialog
    wxXmlResource::Get()->LoadDialog(this, parent, _T("OptionsDlg"));
    CentreOnParent();

    //create the selected panel (panel number m_nCurPanel)
    m_pPanel = CreatePanel((EOptionsPanels) m_nCurPanel);
    m_cPanels[m_nCurPanel] = m_pPanel;

    //get the pointers to the controls
    m_pTreeCtrl = XRCCTRL(*this, "idTreeCtrl", wxTreeCtrl);
    m_pPanel = XRCCTRL(*this, "idPanel", lmOptionsPanel);
    m_pSplitWindow = XRCCTRL(*this, "idSplitterWindow", wxSplitterWindow);
    m_pBtnOK  = XRCCTRL(*this, "idBtnOK", wxButton);
    m_pBtnCancel  = XRCCTRL(*this, "idBtnCancel", wxButton);
    m_pBtnHelp  = XRCCTRL(*this, "idBtnHelp", wxButton);

    */


    // create the Options Dialog and panel # m_nCurPanel
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );
    CreateControls();
    Centre();

    // create all other panels but the one currently shown, previously created
    for (i=0; i < eOptMaxValue; i++) {
        if (i == m_nCurPanel) continue;
        pPanel = CreatePanel((EOptionsPanels)i);
        pPanel->Show(false);
        m_cPanels[i] = pPanel;
    }

        //
        // Populate Tree Ctrol
        //

    // Root node
    CreateImageList();
    wxTreeItemId rootId = m_pTreeCtrl->AddRoot(_("Preferences"),
                    eIconRoot, eIconRoot,
                    new lmTreeItemData(-1) );

    // Language options
    wxTreeItemId languageId = m_pTreeCtrl->AppendItem(rootId, _("Language"),
                    eIconLanguages, eIconLanguages,
                    new lmTreeItemData((long)eOptLanguage) );

    // Colors options
    //wxTreeItemId colorsId = m_pTreeCtrl->AppendItem(rootId, _("Colors"),
    //                eIconColors, eIconColors,
    //                new lmTreeItemData((long)eOptColors) );

    // Toolbars options
    wxTreeItemId ToolbarsId = m_pTreeCtrl->AppendItem(rootId, _("Toolbars"),
                    eIconToolbars, eIconToolbars,
                    new lmTreeItemData((long)eOptToolbars) );

    // Internet options
    wxTreeItemId InternetId = m_pTreeCtrl->AppendItem(rootId, _("Internet"),
                    eIconInternet, eIconInternet,
                    new lmTreeItemData((long)eOptInternet) );

    // Other options
    wxTreeItemId otherId = m_pTreeCtrl->AppendItem(rootId, _("Other"),
                    eIconOther, eIconOther,
                    new lmTreeItemData((long)eOptOther) );

    //TO_ADD: add a new code block to add a new entry in the Tree Control

    //Select in TreeCtrl the item that goes with selected panel
    wxTreeItemId itemId;
    switch(m_nCurPanel) {
        case eOptLanguage:
            itemId = languageId;
            break;
        //case eOptColors:
        //    itemId = colorsId;
        //    break;
        case eOptToolbars:
            itemId = ToolbarsId;
            break;
        case eOptInternet:
            itemId = InternetId;
            break;
        case eOptOther:
            itemId = otherId;
            break;
        //TO_ADD: Add a new case block for selecting the right item
        default:
            itemId = rootId;
    }
    m_pTreeCtrl->Expand(itemId);
    m_pTreeCtrl->SelectItem(itemId);

}

void lmOptionsDlg::CreateControls()
{
    /*
    Controls creation for OptionDlg
    */

    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(pMainSizer);

    m_pSplitWindow = new wxSplitterWindow( this, ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxBORDER_NONE );

    m_pTreeCtrl = new wxTreeCtrl( m_pSplitWindow, ID_TREECTRL,
                            wxDefaultPosition, wxSize(180, 180),
                            wxTR_SINGLE | wxTR_HAS_BUTTONS | wxBORDER_NONE  );

    m_pPanel = CreatePanel((EOptionsPanels) m_nCurPanel);
    m_cPanels[m_nCurPanel] = m_pPanel;

    m_pSplitWindow->SplitVertically(m_pTreeCtrl, m_pPanel, 50);
    pMainSizer->Add(m_pSplitWindow, 1, wxGROW|wxALL, 5);

    wxBoxSizer* pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
    pMainSizer->Add(pButtonsSizer, 0, wxALIGN_RIGHT|wxALL, 5);

    m_pBtnOK = new wxButton( this, ID_BUTTON_ACCEPT, _("&Accept"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add(m_pBtnOK, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_pBtnCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add(m_pBtnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_pBtnHelp = new wxButton( this, wxID_HELP, _("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add(m_pBtnHelp, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

}

void lmOptionsDlg::CreateImageList()
{
    /*
    This methods creates the icons to be used with the TreeCtrol.

    Icon size is controlled by value assigned to  nImageSize. For now it is a fixed value.
    Perhaps in future could be a user option for accesibility.

    For each item in TreeCtrol you have to:
        1. define an icon id string (unique)
        2. Add a PNG file to res/icons
        3. Modify lmArtProvider to deal with it and return the bitmap

    */

    int nImageSize = 24;    //image size in pixels

    // Make an image list containing the icons
    wxImageList *images = new wxImageList(nImageSize, nImageSize, true);

    wxSize nSize(nImageSize, nImageSize);
    int i;
    for (i = 0; i < TreeCtrlIcon_EOF; i++ ) {
        images->Add(wxArtProvider::GetBitmap(sImageID[i], wxART_OTHER, nSize));
    }

    m_pTreeCtrl->AssignImageList(images);
}

bool lmOptionsDlg::SelectPanel(int nPanel)
{
    // returns false if new panel can not be selected

    if (nPanel == m_nCurPanel) return true;        //panel already shown

    //Verify input in current panel and
    //do not change panel if current panel input is not valid
    if (m_pPanel->Verify()) return false;

    //changes applied. Change to new panel
    m_nCurPanel = nPanel;
    lmOptionsPanel* pNewPanel = m_cPanels[nPanel];
    pNewPanel->Show(true);
    m_pSplitWindow->ReplaceWindow(m_pPanel, pNewPanel);
    m_pPanel->Show(false);
    m_pPanel = pNewPanel;

    return true;
}

lmOptionsPanel* lmOptionsDlg::CreatePanel(EOptionsPanels nPanel)
{
    switch(nPanel) {
        case eOptLanguage:
            return new lmLangOptionsPanel( m_pSplitWindow );
        case eOptColors:
            return new lmColorsOptPanel( m_pSplitWindow );
        case eOptToolbars:
            return new lmToolbarsOptPanel( m_pSplitWindow );
        case eOptInternet:
            return new lmInternetOptPanel( m_pSplitWindow );
        case eOptOther:
            return new lmOtherOptionsPanel( m_pSplitWindow );
        //TO_ADD: Add a new case block for creating the panel
        default:
            wxASSERT(false);
    }
    return (lmOptionsPanel*)NULL;

}

void lmOptionsDlg::OnTreectrlItemSelected( wxTreeEvent& event )
{
    // show panel for selected item
    wxTreeItemId itemId = event.GetItem();
    lmTreeItemData* pData = (lmTreeItemData*)(m_pTreeCtrl->GetItemData(itemId));
    long nOpt = pData->GetOptId();
    if (nOpt != -1) {
        if (!SelectPanel(nOpt)) {
            // the panel can not be selected. Move focus to previous treectrol item
            // and inform user
            wxMessageBox(_("Please correct the errors before moving to another section."));
            event.Veto();
        }
    }

}

void lmOptionsDlg::OnButtonAcceptClick(wxCommandEvent& event)
{
   //save current panel id and TreeCtrl itemId, so to start next time with this panel
   g_pPrefs->Write(_T("/UserOptions/OptionsPanel"), m_nCurPanel);

    // The dialog doesn't end until all the input is valid
    if (m_pPanel->Verify()) return;

    //Apply all changes
    int i;
    for(i=0; i < eOptMaxValue; i++) {
        m_cPanels[i]->Apply();
    }

   EndModal(0);
}


