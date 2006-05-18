//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file OtherOptionsPanel.cpp
    @brief Implementation file for class lmOtherOptionsPanel
    @ingroup options_management
*/
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "OtherOptionsPanel.h"
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

#include "OtherOptionsPanel.h"

//access to preferences object
#include "wx/config.h"
extern wxConfigBase* g_pPrefs;


lmOtherOptionsPanel::lmOtherOptionsPanel(wxWindow* parent)
{
    // create the panel
    wxXmlResource::Get()->LoadPanel(this, parent, _T("OtherOptionsPanel"));
   
    //load icon
    wxStaticBitmap* pBmpIcon = XRCCTRL(*this, "bmpIconTitle", wxStaticBitmap);
    pBmpIcon->SetBitmap( wxArtProvider::GetIcon(_T("opt_other"), wxART_TOOLBAR, wxSize(24,24)) );

    //store pointer to wxChoice control
    m_pChkUpdates = XRCCTRL(*this, "chkUpdates", wxCheckBox);

    //Get current selected value
    g_pPrefs->Read(_T("/Options/CheckForUpdates"), &m_fCheckForUpdates, true );
    m_pChkUpdates->SetValue(m_fCheckForUpdates);


}

lmOtherOptionsPanel::~lmOtherOptionsPanel()
{
}

bool lmOtherOptionsPanel::Verify()
{
    return false;
}

void lmOtherOptionsPanel::Apply()
{
    m_fCheckForUpdates = m_pChkUpdates->GetValue();
    g_pPrefs->Write(_T("/Options/CheckForUpdates"), m_fCheckForUpdates );
}