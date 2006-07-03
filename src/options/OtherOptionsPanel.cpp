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
    m_pCboCheckFreq = XRCCTRL(*this, "cboCheckFreq", wxComboBox);
    m_pTxtLastCheck = XRCCTRL(*this, "txtLastCheckDate", wxStaticText);

    // populate combo box
    m_pCboCheckFreq->Append( _("Never") );
    m_pCboCheckFreq->Append( _("Daily") );
    m_pCboCheckFreq->Append( _("Weekly") );
    m_pCboCheckFreq->Append( _("Monthly") );

    //Select current setting
    wxString sCheckFreq = g_pPrefs->Read(_T("/Options/CheckForUpdates/Frequency"), _T("Weekly") );
    if (sCheckFreq == _T("Never"))
        m_pCboCheckFreq->SetSelection(0);
    else if (sCheckFreq == _T("Daily"))
        m_pCboCheckFreq->SetSelection(1);
    else if (sCheckFreq == _T("Weekly"))
        m_pCboCheckFreq->SetSelection(2);
    else if (sCheckFreq == _T("Monthly"))
        m_pCboCheckFreq->SetSelection(3);
    else {
        m_pCboCheckFreq->SetSelection(2);       // assume weekly
        wxLogMessage(_T("[lmOtherOptionsPanel] Invalid value in ini file. Key '/Options/CheckForUpdates/Frequency', value='%s'"),
            sCheckFreq );
    }

    //display last check date
    wxString sLastCheckDate = g_pPrefs->Read(_T("/Options/CheckForUpdates/LastCheck"), _T(""));
    if (sLastCheckDate == _T("")) {
        sLastCheckDate = _("Never");
    }
    m_pTxtLastCheck->SetLabel(sLastCheckDate);


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
    wxString sCheckFreq = m_pCboCheckFreq->GetValue();
    wxString sValue;
    //prefs. value is stored iun English
    if (sCheckFreq == _("Never"))
        sValue = _T("Never");
    else if (sCheckFreq == _("Daily"))
        sValue = _T("Daily");
    else if (sCheckFreq == _("Weekly"))
        sValue = _T("Weekly");
    else if (sCheckFreq == _("Monthly"))
        sValue = _T("Monthly");
    else {
        sValue = _T("Weekly");      //assume weekly
        wxLogMessage(_T("[lmOtherOptionsPanel] Invalid value for ini file. Key '/Options/CheckForUpdates/Frequency', value='%s'"),
            sCheckFreq );
    }
    g_pPrefs->Write(_T("/Options/CheckForUpdates/Frequency"), sValue);
}