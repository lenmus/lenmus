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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ToolbarsOptPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/xrc/xmlres.h"


#include "ToolbarsOptPanel.h"

//access to preferences object
#include "wx/config.h"
extern wxConfigBase* g_pPrefs;

#include "../app/MainFrame.h"
extern lmMainFrame* g_pMainFrame;        //Access to MainFrame



IMPLEMENT_DYNAMIC_CLASS(lmToolbarsOptPanel, lmOptionsPanel);

BEGIN_EVENT_TABLE(lmToolbarsOptPanel, wxPanel)
    //EVT_BUTTON(wxID_OK, lmToolbarsOptPanel::OnOk)
END_EVENT_TABLE()

lmToolbarsOptPanel::lmToolbarsOptPanel(wxWindow* parent)
{
    // create the panel
    wxXmlResource::Get()->LoadPanel(this, parent, _T("ToolbarsOptPanel"));

    //load icon
    wxStaticBitmap* pBmpIcon = XRCCTRL(*this, "bmpIconTitle", wxStaticBitmap);
    pBmpIcon->SetBitmap( wxArtProvider::GetIcon(_T("tool_properties"), wxART_TOOLBAR, wxSize(24,24)) );

    //
    // set panel controls according to current user selected options
    //

    //icons' size
    wxRadioBox* pOptIconSize = XRCCTRL(*this, "optIconSize", wxRadioBox);
    long nIconSize = g_pPrefs->Read(_T("/Toolbars/IconSize"), 16);
    if (nIconSize == 32)
        m_nSizeIndex = 2;
    else if (nIconSize == 24)
        m_nSizeIndex = 1;
    else
        m_nSizeIndex = 0;
    pOptIconSize->SetSelection(m_nSizeIndex);

    //For now isable large size option
    //TODO create large icons set for toolbar buttons
    pOptIconSize->Enable(2, false);


    // labels
    m_nLabelsIndex = (int) g_pPrefs->Read(_T("/Toolbars/Labels"), 0L);
    wxRadioBox* pOptLabels = XRCCTRL(*this, "optLabels", wxRadioBox);
    pOptLabels->SetSelection(m_nLabelsIndex);
}

lmToolbarsOptPanel::~lmToolbarsOptPanel()
{
}

bool lmToolbarsOptPanel::Verify()
{
    return false;
}

void lmToolbarsOptPanel::Apply()
{
    // icons' size
    wxRadioBox* pOptIconSize = XRCCTRL(*this, "optIconSize", wxRadioBox);
    int nSizeIndex = pOptIconSize->GetSelection();
    if (nSizeIndex != m_nSizeIndex) {
        long nIconSize;
        if (nSizeIndex == 2)
            nIconSize = 32;
        else if (nSizeIndex == 1)
            nIconSize = 24;
        else
            nIconSize = 16;
        g_pPrefs->Write(_T("/Toolbars/IconSize"), nIconSize);
    }

    // labels
    wxRadioBox* pOptLabels = XRCCTRL(*this, "optLabels", wxRadioBox);
    int nLabelsIndex = pOptLabels->GetSelection();
    if (nLabelsIndex != m_nLabelsIndex) {
        g_pPrefs->Write(_T("/Toolbars/Labels"), nLabelsIndex);
    }

    // update toolbars
    if ((nSizeIndex != m_nSizeIndex) || (nLabelsIndex != m_nLabelsIndex)) {
        g_pMainFrame->UpdateToolbarsLayout();
    }

}
