//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2019 LenMus project
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

//---------------------------------------------------------------------------------------
//AWARE
//
//    Things to do to add a new options panel to the Options Dialog:
//     1. Create a new panel class derived from OptionsPanel
//     2. Add the XRC panel to TheApp.cpp
//     3. Look for "//TO_ADD:" tags in OptionsDlg.h and follow instructions there
//     4. Look for "//TO_ADD:" tags in this file and follow instructions there
//
//---------------------------------------------------------------------------------------

//lenmus
#include "lenmus_options_dlg.h"
#include "lenmus_art_provider.h"

// Panels
#include "lenmus_options_panel.h"
#include "lenmus_colors_opt_panel.h"
#include "lenmus_internet_opt_panel.h"
#include "lenmus_lang_opt_panel.h"
#include "lenmus_other_opt_panel.h"
#include "lenmus_playback_opt_panel.h"
#include "lenmus_shortcuts_opt_panel.h"
#include "lenmus_toolbar_opt_panel.h"
#include "lenmus_sound_opt_panel.h"
//TO_ADD: add here the LENMUS_NEW panel include file


//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/treectrl.h>
#include <wx/xrc/xmlres.h>
#include <wx/config.h>
#include <wx/utils.h>

namespace lenmus
{

//---------------------------------------------------------------------------------------
//Enum to give the icons index a name
enum
{
    eIconRoot = 0,        //AWARE: first one must be 0. The others consecutive.
    eIconLanguages,
    eIconSound,
    eIconColors,
    eIconToolbars,
    eIconInternet,
    eIconShortcuts,
    eIconPlayback,
    eIconOther,
    //TO_ADD: add the LENMUS_NEW element
    TreeCtrlIcon_EOF        //AWARE: Must be the last one. Just to know how many items
};

//---------------------------------------------------------------------------------------
//String IDs to select the icons to use. Must match previous enum
static wxString sImageID[] = {
    "tool_options",
    "opt_language",
    "opt_sound",
    "opt_colors",
    "opt_tools",
    "opt_internet",
    "opt_shortcuts",
    "opt_playback",
    "opt_other"
    //TO_ADD: Add image identifier here
};

//---------------------------------------------------------------------------------------
// IDs for controls
const long ID_DIALOG = wxNewId();
const long ID_SPLITTERWINDOW = wxNewId();
const long ID_TREECTRL = wxNewId();
const long ID_PANEL = wxNewId();
const long ID_BUTTON_ACCEPT = wxNewId();
const long ID_BUTTON_CANCEL = wxNewId();


//---------------------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE( OptionsDlg, wxDialog )
    EVT_TREE_SEL_CHANGING( ID_TREECTRL, OptionsDlg::OnTreectrlItemSelected )
    EVT_BUTTON( ID_BUTTON_ACCEPT, OptionsDlg::OnButtonAcceptClick )
    EVT_BUTTON( ID_BUTTON_CANCEL, OptionsDlg::OnButtonCancelClick )
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
OptionsDlg::OptionsDlg(wxWindow* parent, ApplicationScope& appScope)
    : wxDialog(parent, ID_DIALOG, _("Configuration options"),
               wxDefaultPosition, wxSize(900, 650),
               wxCAPTION | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCLOSE_BOX )
    , m_appScope(appScope)
    , m_pSplitWindow(nullptr)
    , m_pTreeCtrl(nullptr)
    , m_pPanel(nullptr)
    , m_pBtnOK(nullptr)
    , m_pBtnCancel(nullptr)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);

    //initialize panel's array
    for (int i=0; i < eOptMaxValue; i++)
        m_cPanels[i] = nullptr;

    //Get last used panel
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    m_nCurPanel = pPrefs->Read("/UserOptions/OptionsPanel", 0L);
    if (m_nCurPanel < 0L || m_nCurPanel >= eOptMaxValue)
        m_nCurPanel = 0L;

    // create the Options Dialog and panel # m_nCurPanel
    CreateControls();
    Centre();

    // create all other panels except the one currently shown, previously created
    for (int i=0; i < eOptMaxValue; i++)
    {
        if (i != m_nCurPanel)
        {
            OptionsPanel* pPanel = CreatePanel((EOptionsPanels)i);
            pPanel->Show(false);
            m_cPanels[i] = pPanel;
        }
    }

        //
        // Populate Tree Ctrol
        //

    // Root node
    CreateImageList();
    wxTreeItemId rootId = m_pTreeCtrl->AddRoot(_("Preferences"),
                    eIconRoot, eIconRoot,
                    LENMUS_NEW TreeItemData(-1) );

    // Language options
    wxTreeItemId languageId = m_pTreeCtrl->AppendItem(rootId, _("Language"),
                    eIconLanguages, eIconLanguages,
                    LENMUS_NEW TreeItemData((long)eOptLanguage) );

    // Sound options
    wxTreeItemId soundId = m_pTreeCtrl->AppendItem(rootId, _("Sound"),
                    eIconSound, eIconSound,
                    LENMUS_NEW TreeItemData((long)eOptSound) );

    // Colors options
    //wxTreeItemId colorsId = m_pTreeCtrl->AppendItem(rootId, _("Colors"),
    //                eIconColors, eIconColors,
    //                LENMUS_NEW TreeItemData((long)eOptColors) );

    // Toolbars options
    wxTreeItemId ToolbarsId = m_pTreeCtrl->AppendItem(rootId, _("Toolbars"),
                    eIconToolbars, eIconToolbars,
                    LENMUS_NEW TreeItemData((long)eOptToolbars) );

    // Internet options
    wxTreeItemId InternetId = m_pTreeCtrl->AppendItem(rootId, _("Internet"),
                    eIconInternet, eIconInternet,
                    LENMUS_NEW TreeItemData((long)eOptInternet) );

    wxTreeItemId ShortcutsId = 0;
    if (m_appScope.are_experimental_features_enabled())
    {
        // Keyboard shortcuts
       ShortcutsId = m_pTreeCtrl->AppendItem(rootId, _("Shortcuts"),
                        eIconShortcuts, eIconShortcuts,
                        LENMUS_NEW TreeItemData((long)eOptShortcuts) );
    }

    // Playback options
    wxTreeItemId PlaybackId = m_pTreeCtrl->AppendItem(rootId, _("Playback"),
                    eIconPlayback, eIconPlayback,
                    LENMUS_NEW TreeItemData((long)eOptPlayback) );
    // Other options
    wxTreeItemId otherId = m_pTreeCtrl->AppendItem(rootId, _("Other"),
                    eIconOther, eIconOther,
                    LENMUS_NEW TreeItemData((long)eOptOther) );

    //TO_ADD: add a new code block to add a new entry in the Tree Control

    //Select in TreeCtrl the item that goes with selected panel
    wxTreeItemId itemId;
    switch(m_nCurPanel)
    {
        case eOptLanguage:
            itemId = languageId;
            break;
        case eOptSound:
            itemId = soundId;
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
        case eOptShortcuts:
            itemId = ShortcutsId;
            break;
        case eOptPlayback:
            itemId = PlaybackId;
            break;
        case eOptOther:
            itemId = otherId;
            break;
        //TO_ADD: Add a LENMUS_NEW case block for selecting the right item
        default:
            itemId = rootId;
    }
    m_pTreeCtrl->Expand(itemId);
    m_pTreeCtrl->SelectItem(itemId);

}

//---------------------------------------------------------------------------------------
OptionsDlg::~OptionsDlg()
{
    for (int i=0; i < eOptMaxValue; i++)
        delete m_cPanels[i];
}

//---------------------------------------------------------------------------------------
void OptionsDlg::CreateControls()
{
    wxBoxSizer* pMainSizer = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    this->SetSizer(pMainSizer);

    m_pSplitWindow = LENMUS_NEW wxSplitterWindow( this, ID_SPLITTERWINDOW, wxDefaultPosition,
                                           wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxBORDER_NONE );

    m_pTreeCtrl = LENMUS_NEW wxTreeCtrl( m_pSplitWindow, ID_TREECTRL,
                            wxDefaultPosition, wxSize(180, 180),
                            wxTR_SINGLE | wxTR_HAS_BUTTONS | wxBORDER_NONE  );

    m_pPanel = CreatePanel((EOptionsPanels) m_nCurPanel);
    m_cPanels[m_nCurPanel] = m_pPanel;

    m_pSplitWindow->SplitVertically(m_pTreeCtrl, m_pPanel, 50);
    pMainSizer->Add(m_pSplitWindow, 1, wxGROW|wxALL, 5);

    wxBoxSizer* pButtonsSizer = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);
    pMainSizer->Add(pButtonsSizer, 0, wxALIGN_RIGHT|wxALL, 5);

    m_pBtnOK = LENMUS_NEW wxButton( this, ID_BUTTON_ACCEPT, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add(m_pBtnOK, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_pBtnCancel = LENMUS_NEW wxButton( this, ID_BUTTON_CANCEL , _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    pButtonsSizer->Add(m_pBtnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

}

//---------------------------------------------------------------------------------------
void OptionsDlg::CreateImageList()
{
    // This methods creates the icons to be used with the TreeCtrol.
    //
    // Icon size is controlled by value assigned to  nImageSize. For now it is a
    // fixed value. Perhaps in future could be a user option for accesibility.
    //
    // For each item in TreeCtrol you have to:
    //   1. define an icon id string (unique)
    //   2. Add a PNG file to res/icons
    //   3. Modify ArtProvider to deal with it and return the bitmap


    int nImageSize = 24;    //image size in pixels

    // Make an image list containing the icons
    wxImageList *images = LENMUS_NEW wxImageList(nImageSize, nImageSize, true);

    wxSize nSize(nImageSize, nImageSize);
    for (int i = 0; i < TreeCtrlIcon_EOF; i++ )
        images->Add(wxArtProvider::GetBitmap(sImageID[i], wxART_OTHER, nSize));

    m_pTreeCtrl->AssignImageList(images);
}

//---------------------------------------------------------------------------------------
bool OptionsDlg::SelectPanel(int nPanel)
{
    // returns false if LENMUS_NEW panel can not be selected

    if (nPanel == m_nCurPanel)
        return true;        //panel already shown

    //Verify input in current panel and
    //do not change panel if current panel input is not valid
    if (m_pPanel->Verify())
        return false;

    //changes applied. Change to LENMUS_NEW panel
    m_nCurPanel = nPanel;
    OptionsPanel* pNewPanel = m_cPanels[nPanel];
    pNewPanel->Show(true);
    m_pSplitWindow->ReplaceWindow(m_pPanel, pNewPanel);
    m_pPanel->Show(false);
    m_pPanel = pNewPanel;

    return true;
}

//---------------------------------------------------------------------------------------
OptionsPanel* OptionsDlg::CreatePanel(EOptionsPanels nPanel)
{
    switch(nPanel)
    {
        case eOptLanguage:
            return LENMUS_NEW LangOptionsPanel(m_pSplitWindow, m_appScope);
        case eOptSound:
            return LENMUS_NEW SoundOptionsPanel(m_pSplitWindow, m_appScope);
        case eOptColors:
            return LENMUS_NEW ColorsOptPanel(m_pSplitWindow, m_appScope);
        case eOptToolbars:
            return LENMUS_NEW ToolbarsOptPanel(m_pSplitWindow, m_appScope);
        case eOptInternet:
            return LENMUS_NEW InternetOptPanel(m_pSplitWindow, m_appScope);
        case eOptShortcuts:
            return LENMUS_NEW ShortcutsOptPanel(m_pSplitWindow, m_appScope);
        case eOptPlayback:
            return LENMUS_NEW PlaybackOptPanel(m_pSplitWindow, m_appScope);
        case eOptOther:
            return LENMUS_NEW OtherOptionsPanel(m_pSplitWindow, m_appScope);
        //TO_ADD: Add a LENMUS_NEW case block for creating the panel
        default:
            wxASSERT(false);
    }
    return (OptionsPanel*)nullptr;

}

//---------------------------------------------------------------------------------------
void OptionsDlg::OnTreectrlItemSelected( wxTreeEvent& event )
{
    // show panel for selected item
    wxTreeItemId itemId = event.GetItem();
    TreeItemData* pData = (TreeItemData*)(m_pTreeCtrl->GetItemData(itemId));
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

//---------------------------------------------------------------------------------------
void OptionsDlg::OnButtonAcceptClick(wxCommandEvent& WXUNUSED(event))
{
    //save current panel id and TreeCtrl itemId, so to start next time with this panel
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Write("/UserOptions/OptionsPanel", m_nCurPanel);

    // The dialog doesn't end until all the input is valid
    if (m_pPanel->Verify())
        return;

    //Apply all changes
    for(int i=0; i < eOptMaxValue; i++)
        m_cPanels[i]->Apply();

    EndModal(0);
}

//---------------------------------------------------------------------------------------
void OptionsDlg::OnButtonCancelClick(wxCommandEvent& WXUNUSED(event))
{
    EndModal(0);
}


}   //namespace lenmus
