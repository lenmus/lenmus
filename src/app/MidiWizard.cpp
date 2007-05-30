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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "MidiWizard.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "MidiWizard.h"

// MIDI support throgh Portmidi lib
#include "../../wxMidi/include/wxMidi.h"

//access to Midi configuration
#include "../sound/MidiManager.h"


//--------------------------------------------------------------------------------
//  lmMidiWizard implementation
//--------------------------------------------------------------------------------

// lmMidiWizard type definition
IMPLEMENT_DYNAMIC_CLASS( lmMidiWizard, wxWizard )

BEGIN_EVENT_TABLE( lmMidiWizard, wxWizard )
    EVT_WIZARD_CANCEL( ID_WIZARD, lmMidiWizard::OnWizardCancel )
    EVT_WIZARD_FINISHED( ID_WIZARD, lmMidiWizard::OnWizardFinished )
END_EVENT_TABLE()


lmMidiWizard::lmMidiWizard( )
{
}

lmMidiWizard::lmMidiWizard( wxWindow* parent, wxWindowID id, const wxPoint& pos )
{
    Create(parent, id, pos);
}

bool lmMidiWizard::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos )
{
    // member initialisation
    m_pMtrChannelCombo = NULL;

    // creation
    SetExtraStyle(GetExtraStyle()|wxWIZARD_EX_HELPBUTTON);
    wxBitmap wizardBitmap(wxNullBitmap);
    wxWizard::Create( parent, id, _("MIDI configuration wizard"), wizardBitmap, pos );
    CreateControls();

    //save current Midi configuration to restore it if the wizard is cancelled
    m_nOldInDevId = g_pMidi->InDevId();
    m_nOldOutDevId = g_pMidi->OutDevId();
    m_nOldVoiceInstr = g_pMidi->VoiceInstr();
    m_nOldVoiceChannel = g_pMidi->VoiceChannel();
    m_nOldMtrInstr = g_pMidi->MtrInstr();
    m_nOldMtrChannel = g_pMidi->MtrChannel();
    m_nOldMtrTone1 = g_pMidi->MtrTone1();
    m_nOldMtrTone2 = g_pMidi->MtrTone2();

    return true;
}


void lmMidiWizard::CreateControls()
{
    //
    // Control creation for lmMidiWizard
    //

    WizardDevicesPage* itemWizardPageSimple2 = new WizardDevicesPage(this);

    FitToPage(itemWizardPageSimple2);
    WizardInstrumentsPage* itemWizardPageSimple16 = new WizardInstrumentsPage(this);

    FitToPage(itemWizardPageSimple16);
    m_pMtrChannelCombo = new WizardMetronomePage(this);

    FitToPage(m_pMtrChannelCombo);
    wxWizardPageSimple* lastPage = NULL;
    if (lastPage)
        wxWizardPageSimple::Chain(lastPage, itemWizardPageSimple2);
    lastPage = itemWizardPageSimple2;
    if (lastPage)
        wxWizardPageSimple::Chain(lastPage, itemWizardPageSimple16);
    lastPage = itemWizardPageSimple16;
    if (lastPage)
        wxWizardPageSimple::Chain(lastPage, m_pMtrChannelCombo);
    lastPage = m_pMtrChannelCombo;

}


bool lmMidiWizard::Run()
{
    //
    // Runs the wizard.
    //

    wxWindowListNode* node = GetChildren().GetFirst();
    while (node)
    {
        wxWizardPage* startPage = wxDynamicCast(node->GetData(), wxWizardPage);
        if (startPage) return RunWizard(startPage);
        node = node->GetNext();
    }
    return false;
}

bool lmMidiWizard::ShowToolTips()
{
    // Should we show tooltips?
    return true;
}

wxBitmap lmMidiWizard::GetBitmapResource( const wxString& name )
{
    // Get bitmap resources
    return wxNullBitmap;
}

wxIcon lmMidiWizard::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return wxNullIcon;
}

void lmMidiWizard::OnWizardFinished( wxWizardEvent& event )
{
    //take note that user has set Midi preferences
    g_pMidi->SetConfigured(true);

    //and save user MIDI preferences
    g_pMidi->SaveUserPreferences();

}

void lmMidiWizard::OnWizardCancel( wxWizardEvent& event )
{
    //
    // restore old configuration if any
    //

    if (g_pMidi->IsConfigured()) {
        //devices
        g_pMidi->SetInDevice(m_nOldInDevId);
        g_pMidi->SetOutDevice(m_nOldOutDevId);

        //voice instruments
        g_pMidi->VoiceChange(m_nOldVoiceChannel, m_nOldVoiceInstr);

        //metronome configuration
        g_pMidi->VoiceChange(m_nOldMtrChannel, m_nOldMtrInstr);
        g_pMidi->SetMetronomeTones(m_nOldMtrTone1, m_nOldMtrTone2);
    }
}


//--------------------------------------------------------------------------------
//  WizardDevicesPage implementation
//--------------------------------------------------------------------------------

// WizardDevicesPage type definition
IMPLEMENT_DYNAMIC_CLASS( WizardDevicesPage, wxWizardPageSimple )

// WizardDevicesPage event table definition
BEGIN_EVENT_TABLE( WizardDevicesPage, wxWizardPageSimple )
    //
END_EVENT_TABLE()

WizardDevicesPage::WizardDevicesPage( )
{
}

WizardDevicesPage::WizardDevicesPage( wxWizard* parent )
{
    Create( parent );
}

/*!
 * WizardPage creator
 */

bool WizardDevicesPage::Create( wxWizard* parent )
{
    // member initialisation
    m_pOutCombo = NULL;
    m_pInCombo = NULL;

    // page creation
    wxBitmap wizardBitmap(GetBitmapResource(wxT("wizard.png")));
    wxWizardPageSimple::Create( parent, NULL, NULL, wizardBitmap );
    CreateControls();
    GetSizer()->Fit(this);

    // populate combo boxes with available Midi devices
    int nItem, nInput=0, nOutput=0;
    int nNumDevices = g_pMidi->CountDevices();
    for (int i = 0; i < nNumDevices; i++) {
        wxMidiOutDevice* pMidiDev = new wxMidiOutDevice(i);
        if (pMidiDev->IsOutputPort()) {
            nOutput++;
            nItem = m_pOutCombo->Append( pMidiDev->DeviceName() );
            m_pOutCombo->SetClientData(nItem, (void *)i);
        }
        if (pMidiDev->IsInputPort()) {
            nInput++;
            nItem = m_pInCombo->Append( pMidiDev->DeviceName() );
            m_pInCombo->SetClientData(nItem, (void *)i);
        }
        delete pMidiDev;
    }
    if (nInput == 0) {
        nItem = m_pInCombo->Append( _("None") );
        m_pInCombo->SetClientData(nItem, (void *)(-1));
    }
    if (nOutput == 0) {
        nItem = m_pOutCombo->Append( _("None") );
        m_pOutCombo->SetClientData(nItem, (void *)(-1));
    }
    m_pOutCombo->SetSelection(0);
    m_pInCombo->SetSelection(0);

    return true;
}

void WizardDevicesPage::CreateControls()
{
    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    SetSizer(itemBoxSizer3);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( this, wxID_STATIC, _("Midi devices to use"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText5->SetFont(wxFont(14, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer6, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer6->Add(itemBoxSizer7, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( this, wxID_STATIC, _("To generate sounds the program needs a MIDI synthesizer device. Normally, one of these devices is included in the sound board of the PC, but your PC might have more than one."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer7->Add(itemStaticText8, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText9 = new wxStaticText( this, wxID_STATIC, _("If your PC has more than one device, choose one of them. You can test all of them and choose the one whose sound you prefer."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer7->Add(itemStaticText9, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticLine* itemStaticLine10 = new wxStaticLine( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer6->Add(itemStaticLine10, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer6->Add(itemBoxSizer11, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText12 = new wxStaticText( this, wxID_STATIC, _("Output device:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText12, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pOutComboStrings = NULL;
    m_pOutCombo = new wxComboBox( this, ID_COMBO_OUT_DEVICES, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pOutComboStrings, wxCB_READONLY );
    itemBoxSizer11->Add(m_pOutCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( this, wxID_STATIC, _("Input device:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText14, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pInComboStrings = NULL;
    m_pInCombo = new wxComboBox( this, ID_COMBO_IN_DEVICES, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pInComboStrings, wxCB_READONLY );
    itemBoxSizer11->Add(m_pInCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

}

bool WizardDevicesPage::ShowToolTips()
{
    // Should we show tooltips?
    return true;
}

wxBitmap WizardDevicesPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    if (name == wxT("wizard.png"))
    {
        wxBitmap bitmap(_T("wizard.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    return wxNullBitmap;
}

wxIcon WizardDevicesPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return wxNullIcon;
}

bool WizardDevicesPage::TransferDataFromWindow()
{
    //
    // Save temporary data and open temporary Midi Devices
    //

    //get number of Midi device to use for output
    int nIndex = m_pOutCombo->GetSelection();
    int nOutDevId = (int) m_pOutCombo->GetClientData(nIndex);
    g_pMidi->SetOutDevice(nOutDevId);

    //open input device
    int nInDevId = -1;
    if (m_pInCombo->GetStringSelection() != _("None") ) {
        nIndex = m_pInCombo->GetSelection();
        nInDevId = (int) m_pInCombo->GetClientData(nIndex);
    }
    g_pMidi->SetInDevice(nInDevId);

    return true;

}



//--------------------------------------------------------------------------------------
// WizardInstrumentsPage implementation
//--------------------------------------------------------------------------------------


IMPLEMENT_DYNAMIC_CLASS( WizardInstrumentsPage, wxWizardPageSimple )

BEGIN_EVENT_TABLE( WizardInstrumentsPage, wxWizardPageSimple )
    EVT_COMBOBOX( ID_COMBO_SECTION, WizardInstrumentsPage::OnComboSection )
    EVT_COMBOBOX( ID_COMBO_INSTRUMENT, WizardInstrumentsPage::OnComboInstrument )
    EVT_BUTTON( ID_BUTTON_TEST_SOUND, WizardInstrumentsPage::OnButtonTestSoundClick )
END_EVENT_TABLE()

WizardInstrumentsPage::WizardInstrumentsPage( )
{
}

WizardInstrumentsPage::WizardInstrumentsPage( wxWizard* parent )
{
    Create( parent );
}

bool WizardInstrumentsPage::Create( wxWizard* parent )
{
    // member initialisation
    m_pVoiceChannelCombo = NULL;
    m_pSectCombo = NULL;
    m_pInstrCombo = NULL;

    // page creation
    wxBitmap wizardBitmap(wxNullBitmap);
    wxWizardPageSimple::Create( parent, NULL, NULL, wizardBitmap );
    CreateControls();
    GetSizer()->Fit(this);

    // populate channel combo
    m_pVoiceChannelCombo->Clear();
    for(int i=1; i <= 16; i++) {
        m_pVoiceChannelCombo->Append(wxString::Format(_T("%d"), i));
    }
    //Set selection according to current user prefs
    m_pVoiceChannelCombo->SetSelection( g_pMidi->VoiceChannel() );

    //populate sections and instruments combos
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nInstr = g_pMidi->VoiceInstr();
    int nSect = pMidiGM->PopulateWithSections((wxControlWithItems*)m_pSectCombo, nInstr );
    pMidiGM->PopulateWithInstruments((wxControlWithItems*)m_pInstrCombo, nSect, nInstr);

    return true;
}

void WizardInstrumentsPage::CreateControls()
{
    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxVERTICAL);
    SetSizer(itemBoxSizer17);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer17->Add(itemBoxSizer18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText19 = new wxStaticText( this, wxID_STATIC, _("Voice channel and instrument"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText19->SetFont(wxFont(14, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
    itemBoxSizer18->Add(itemStaticText19, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer17->Add(itemBoxSizer20, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer20->Add(itemBoxSizer21, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText22 = new wxStaticText( this, wxID_STATIC, _("Channels 10 and 16 are specialized in percussion sounds. So it is recommended to choose any other channel (it doesn't matter wich one)."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer21->Add(itemStaticText22, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText23 = new wxStaticText( this, wxID_STATIC, _("To facilitate access to the instruments they are grouped into sections. First choose a section and then choose the desired instrument."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer21->Add(itemStaticText23, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticLine* itemStaticLine24 = new wxStaticLine( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer20->Add(itemStaticLine24, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer20->Add(itemBoxSizer25, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText26 = new wxStaticText( this, wxID_STATIC, _("Channel:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText26, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pVoiceChannelComboStrings = NULL;
    m_pVoiceChannelCombo = new wxComboBox( this, ID_COMBO_CHANNEL, _T(""), wxDefaultPosition, wxSize(70, -1), 0, m_pVoiceChannelComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pVoiceChannelCombo, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer25->Add(itemBoxSizer28, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText29 = new wxStaticText( this, wxID_STATIC, _("Section:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText29, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pSectComboStrings = NULL;
    m_pSectCombo = new wxComboBox( this, ID_COMBO_SECTION, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pSectComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pSectCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText31 = new wxStaticText( this, wxID_STATIC, _("Instrument:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText31, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pInstrComboStrings = NULL;
    m_pInstrCombo = new wxComboBox( this, ID_COMBO_INSTRUMENT, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pInstrComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pInstrCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxButton* itemButton33 = new wxButton( this, ID_BUTTON_TEST_SOUND, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemButton33, 0, wxALIGN_LEFT|wxALL, 5);
}

bool WizardInstrumentsPage::ShowToolTips()
{
    // Should we show tooltips?
    return true;
}

wxBitmap WizardInstrumentsPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return wxNullBitmap;
}

wxIcon WizardInstrumentsPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return wxNullIcon;
}

bool WizardInstrumentsPage::TransferDataFromWindow()
{
    //
    // Save temporary data and set temporary Midi program
    //

    DoProgramChange();
    return true;

}

void WizardInstrumentsPage::DoProgramChange()
{
    //Change Midi instrument to the one selected in combo Instruments
    int nInstr = m_pInstrCombo->GetSelection();
    int nSect = m_pSectCombo->GetSelection();
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nVoiceInstr = pMidiGM->GetInstrFromSection(nSect, nInstr);
    int nVoiceChannel = m_pVoiceChannelCombo->GetSelection();
    g_pMidi->VoiceChange(nVoiceChannel, nVoiceInstr);

}

void WizardInstrumentsPage::OnComboSection( wxCommandEvent& event )
{
    // A new section selected. Reload Instruments combo with the instruments in the
    //selected section

    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nSect = m_pSectCombo->GetSelection();
    pMidiGM->PopulateWithInstruments((wxControlWithItems*)m_pInstrCombo, nSect);
    DoProgramChange();

}

void WizardInstrumentsPage::OnComboInstrument( wxCommandEvent& event )
{
    // A new instrument selected. Change Midi program
    DoProgramChange();
}

void WizardInstrumentsPage::OnButtonTestSoundClick( wxCommandEvent& event )
{
    //play a scale
    g_pMidi->TestOut();
}


//---------------------------------------------------------------------------------------
// WizardMetronomePage implementation
//---------------------------------------------------------------------------------------

// WizardMetronomePage type definition
IMPLEMENT_DYNAMIC_CLASS( WizardMetronomePage, wxWizardPageSimple )

// WizardMetronomePage event table definition
BEGIN_EVENT_TABLE( WizardMetronomePage, wxWizardPageSimple )
    EVT_COMBOBOX( ID_COMBO_MTR_INSTR1, WizardMetronomePage::OnComboMtrInstr1Selected )
    EVT_COMBOBOX( ID_COMBO_MTR_INSTR2, WizardMetronomePage::OnComboMtrInstr2Selected )
    EVT_BUTTON( ID_BUTTON, WizardMetronomePage::OnButtonClick )
END_EVENT_TABLE()


WizardMetronomePage::WizardMetronomePage( )
{
}

WizardMetronomePage::WizardMetronomePage( wxWizard* parent )
{
    Create( parent );
}

bool WizardMetronomePage::Create( wxWizard* parent )
{
    // member initialisation
    m_pMtrChannelCombo = NULL;
    m_pMtrInstr1Combo = NULL;
    m_pMtrInstr2Combo = NULL;

    // Page creation
    wxBitmap wizardBitmap(wxNullBitmap);
    wxWizardPageSimple::Create( parent, NULL, NULL, wizardBitmap );
    CreateControls();
    GetSizer()->Fit(this);

    // populate channel combo
    m_pMtrChannelCombo->Clear();
    for(int i=1; i <= 16; i++) {
        m_pMtrChannelCombo->Append(wxString::Format(_T("%d"), i));
    }

    //Set selection according to current user prefs
    m_pMtrChannelCombo->SetSelection( g_pMidi->MtrChannel() );

    //populate metronome sounds combos
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nTone1 = g_pMidi->MtrTone1();
    int nTone2 = g_pMidi->MtrTone2();
    pMidiGM->PopulateWithPercusionInstr((wxControlWithItems*)m_pMtrInstr1Combo, nTone1);
    pMidiGM->PopulateWithPercusionInstr((wxControlWithItems*)m_pMtrInstr2Combo, nTone2);

    return true;
}

void WizardMetronomePage::CreateControls()
{
    wxBoxSizer* itemBoxSizer35 = new wxBoxSizer(wxVERTICAL);
    SetSizer(itemBoxSizer35);

    wxBoxSizer* itemBoxSizer36 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer35->Add(itemBoxSizer36, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText37 = new wxStaticText( this, wxID_STATIC, _("lmMetronome channel and sounds"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText37->SetFont(wxFont(14, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
    itemBoxSizer36->Add(itemStaticText37, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer38 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer35->Add(itemBoxSizer38, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer39 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer38->Add(itemBoxSizer39, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText40 = new wxStaticText( this, wxID_STATIC, _("Channels 10 and 16 are specialized in percussion sounds. So it is recommended to choose one of these (it doesn't matter wich one)."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer39->Add(itemStaticText40, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText41 = new wxStaticText( this, wxID_STATIC, _("To better identify the first beat of each measure it is possible to assign a different sound to it. But you can also choose the same sound for both, the first beat and the others."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer39->Add(itemStaticText41, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticLine* itemStaticLine42 = new wxStaticLine( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer38->Add(itemStaticLine42, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer43 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer38->Add(itemBoxSizer43, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText44 = new wxStaticText( this, wxID_STATIC, _("Channel:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText44, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pMtrChannelComboStrings = NULL;
    m_pMtrChannelCombo = new wxComboBox( this, ID_COMBO_MTR_CHANNEL, _T(""), wxDefaultPosition, wxSize(70, -1), 0, m_pMtrChannelComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrChannelCombo, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer46 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer43->Add(itemBoxSizer46, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText47 = new wxStaticText( this, wxID_STATIC, _("Sound for first beat of each measure:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText47, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pMtrInstr1ComboStrings = NULL;
    m_pMtrInstr1Combo = new wxComboBox( this, ID_COMBO_MTR_INSTR1, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pMtrInstr1ComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrInstr1Combo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText49 = new wxStaticText( this, wxID_STATIC, _("Sound for other beats of each measure:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText49, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pMtrInstr2ComboStrings = NULL;
    m_pMtrInstr2Combo = new wxComboBox( this, ID_COMBO_MTR_INSTR2, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pMtrInstr2ComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrInstr2Combo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxButton* itemButton51 = new wxButton( this, ID_BUTTON, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemButton51, 0, wxALIGN_LEFT|wxALL, 5);

}

bool WizardMetronomePage::ShowToolTips()
{
    // Should we show tooltips?
    return true;
}

wxBitmap WizardMetronomePage::GetBitmapResource( const wxString& name )
{
    // Get bitmap resources
    return wxNullBitmap;
}

wxIcon WizardMetronomePage::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return wxNullIcon;
}

void WizardMetronomePage::OnComboMtrInstr1Selected( wxCommandEvent& event )
{
    //Change metronome sound, tone1, to the one selected in combo Instr1
    int nTone1 = m_pMtrInstr1Combo->GetSelection() + 35;
    g_pMidi->SetMetronomeTones(nTone1, g_pMidi->MtrTone2());

}

void WizardMetronomePage::OnComboMtrInstr2Selected( wxCommandEvent& event )
{
    //Change metronome sound, tone2, to the one selected in combo Instr2
    int nTone2 = m_pMtrInstr2Combo->GetSelection() + 35;
    g_pMidi->SetMetronomeTones(g_pMidi->MtrTone1(), nTone2);
}

void WizardMetronomePage::OnButtonClick( wxCommandEvent& event )
{
    if (!g_pMidiOut) return;

    //two measures, 3/4 time signature
    for (int i=0; i < 2; i++) {
        //firts beat
        g_pMidiOut->NoteOn(g_pMidi->MtrChannel(), g_pMidi->MtrTone1(), 127);
        ::wxMilliSleep(500);    // wait 500ms
        g_pMidiOut->NoteOff(g_pMidi->MtrChannel(), g_pMidi->MtrTone1(), 127);
        // two more beats
        for (int j=0; j < 2; j++) {
            g_pMidiOut->NoteOn(g_pMidi->MtrChannel(), g_pMidi->MtrTone2(), 127);
            ::wxMilliSleep(500);    // wait 500ms
            g_pMidiOut->NoteOff(g_pMidi->MtrChannel(), g_pMidi->MtrTone2(), 127);
        }
    }

}


