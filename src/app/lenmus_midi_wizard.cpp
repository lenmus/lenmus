//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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

#include "lenmus_midi_wizard.h"

#include "lenmus_injectors.h"
#include "lenmus_midi_server.h"

#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//=======================================================================================
//  MidiWizard implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE( MidiWizard, wxWizard )
    EVT_WIZARD_CANCEL( ID_WIZARD, MidiWizard::OnWizardCancel )
    EVT_WIZARD_FINISHED( ID_WIZARD, MidiWizard::OnWizardFinished )
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
MidiWizard::MidiWizard(ApplicationScope& appScope, wxWindow* parent, wxWindowID id,
                       const wxPoint& pos)
    : m_appScope(appScope)
{
    Create(parent, id, pos);
}

//---------------------------------------------------------------------------------------
bool MidiWizard::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos )
{
    // member initialisation
    m_pMtrChannelCombo = NULL;

    // creation
    SetExtraStyle(GetExtraStyle()|wxWIZARD_EX_HELPBUTTON);
    wxBitmap wizardBitmap(wxNullBitmap);
    wxWizard::Create( parent, id, _("MIDI configuration wizard"), wizardBitmap, pos );
    CreateControls();

    //save current Midi configuration to restore it if the wizard is cancelled
    MidiServer* pMidi = m_appScope.get_midi_server();
    m_nOldInDevId = pMidi->InDevId();
    m_nOldOutDevId = pMidi->OutDevId();
    m_nOldVoiceInstr = pMidi->VoiceInstr();
    m_nOldVoiceChannel = pMidi->VoiceChannel();
    m_nOldMtrInstr = pMidi->MtrInstr();
    m_nOldMtrChannel = pMidi->MtrChannel();
    m_nOldMtrTone1 = pMidi->MtrTone1();
    m_nOldMtrTone2 = pMidi->MtrTone2();

    return true;
}

//---------------------------------------------------------------------------------------
void MidiWizard::CreateControls()
{
    WizardDevicesPage* itemWizardPageSimple2 = LENMUS_NEW WizardDevicesPage(m_appScope, this);

    FitToPage(itemWizardPageSimple2);
    WizardInstrumentsPage* itemWizardPageSimple16 = LENMUS_NEW WizardInstrumentsPage(m_appScope, this);

    FitToPage(itemWizardPageSimple16);
    m_pMtrChannelCombo = LENMUS_NEW WizardMetronomePage(m_appScope, this);

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

//---------------------------------------------------------------------------------------
bool MidiWizard::Run()
{
    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
    while (node)
    {
        wxWizardPage* startPage = wxDynamicCast(node->GetData(), wxWizardPage);
        if (startPage) return RunWizard(startPage);
        node = node->GetNext();
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool MidiWizard::ShowToolTips()
{
    return true;
}

//---------------------------------------------------------------------------------------
wxBitmap MidiWizard::GetBitmapResource( const wxString& name )
{
    return wxNullBitmap;
}

//---------------------------------------------------------------------------------------
wxIcon MidiWizard::GetIconResource( const wxString& name )
{
    return wxNullIcon;
}

//---------------------------------------------------------------------------------------
void MidiWizard::OnWizardFinished( wxWizardEvent& event )
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->SetConfigured(true);
    pMidi->SaveUserPreferences();
}

//---------------------------------------------------------------------------------------
void MidiWizard::OnWizardCancel( wxWizardEvent& event )
{
    // restore old configuration if any

    MidiServer* pMidi = m_appScope.get_midi_server();
    if (pMidi->is_configured())
    {
        //devices
        pMidi->SetInDevice(m_nOldInDevId);
        pMidi->SetOutDevice(m_nOldOutDevId);

        //voice instruments
        pMidi->VoiceChange(m_nOldVoiceChannel, m_nOldVoiceInstr);

        //metronome configuration
        pMidi->VoiceChange(m_nOldMtrChannel, m_nOldMtrInstr);
        pMidi->SetMetronomeTones(m_nOldMtrTone1, m_nOldMtrTone2);
    }
}


//=======================================================================================
//  WizardDevicesPage implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE( WizardDevicesPage, wxWizardPageSimple )
    //
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
WizardDevicesPage::WizardDevicesPage(ApplicationScope& appScope, wxWizard* parent)
    : m_appScope(appScope)
{
    Create( parent );
}

//---------------------------------------------------------------------------------------
bool WizardDevicesPage::Create( wxWizard* parent )
{
    // member initialisation
    m_pOutCombo = NULL;
    m_pInCombo = NULL;

    // page creation
    wxBitmap wizardBitmap(GetBitmapResource("wizard.png"));
    wxWizardPageSimple::Create( parent, NULL, NULL, wizardBitmap );
    CreateControls();
    GetSizer()->Fit(this);

    // populate combo boxes with available Midi devices
    MidiServer* pMidi = m_appScope.get_midi_server();
    //int nInput=0;
    int nItem, nOutput=0;
    int nNumDevices = pMidi->CountDevices();
    int nOutDevId = pMidi->OutDevId();
    int iSelOut = 0;
//    //TODO: Un-comment when ready to use MIDI input
//    int nInDevId = pMidi->InDevId();
//    int iSelIn = 0;
    for (int i = 0; i < nNumDevices; i++)
    {
        wxMidiOutDevice* pMidiDev = LENMUS_NEW wxMidiOutDevice(i);
        if (pMidiDev->IsOutputPort())
        {
            nOutput++;
            nItem = m_pOutCombo->Append( pMidiDev->DeviceName() );
            m_pOutCombo->SetClientData(nItem, (void*)(size_t)i);
            //wxLogMessage("[WizardDevicesPage::Create] nItem=%d, i=%d", nItem, i);
            if (nOutDevId == i)
                iSelOut = nItem;
        }
		////TODO: Un-comment when ready to use MIDI input
        //if (pMidiDev->IsInputPort()) {
        //    nInput++;
        //    nItem = m_pInCombo->Append( pMidiDev->DeviceName() );
        //    m_pInCombo->SetClientData(nItem, (void *)i);
        //    if (nInDevId == i)
        //        iSelIn = nItem;
        //}
        delete pMidiDev;
    }

    //set current selection
    if (nOutput == 0)
    {
        nItem = m_pOutCombo->Append( _("None") );
        m_pOutCombo->SetClientData(nItem, (void *)(-1));
        iSelOut = 0;
    }
    m_pOutCombo->SetSelection(iSelOut);

	////TODO: Un-comment when ready to use MIDI input
	////set current selection
    //if (nInput == 0) {
    //    nItem = m_pInCombo->Append( _("None") );
    //    m_pInCombo->SetClientData(nItem, (void *)(-1));
    //    iSelIn = 0;
    //}
    //m_pInCombo->SetSelection(iSelIn);

    return true;
}

//---------------------------------------------------------------------------------------
void WizardDevicesPage::CreateControls()
{
    wxBoxSizer* itemBoxSizer3 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    SetSizer(itemBoxSizer3);

    wxBoxSizer* itemBoxSizer4 = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText5 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Midi devices to use"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText5->SetFont(wxFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial"));
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer6 = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer6, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    itemBoxSizer6->Add(itemBoxSizer7, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText8 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("To generate sounds the program needs a MIDI synthesizer device. Normally, one of these devices is included in the sound board of the PC, but your PC might have more than one."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer7->Add(itemStaticText8, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText9 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("If your PC has more than one device, choose one of them. You can test all of them and choose the one whose sound you prefer."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer7->Add(itemStaticText9, 1, wxGROW|wxALL, 5);

    wxStaticLine* itemStaticLine10 = LENMUS_NEW wxStaticLine( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer6->Add(itemStaticLine10, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer11 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    itemBoxSizer6->Add(itemBoxSizer11, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText12 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Output device:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText12, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_pOutComboStrings = NULL;
    m_pOutCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_OUT_DEVICES, "", wxDefaultPosition, wxSize(250, -1), 0, m_pOutComboStrings, wxCB_READONLY );
    itemBoxSizer11->Add(m_pOutCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

	//TODO: Un-comment when ready to use MIDI input
    //wxStaticText* itemStaticText14 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Input device:"), wxDefaultPosition, wxDefaultSize, 0 );
    //itemBoxSizer11->Add(itemStaticText14, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

	//TODO: Un-comment when ready to use MIDI input
    //wxString* m_pInComboStrings = NULL;
    //m_pInCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_IN_DEVICES, "", wxDefaultPosition, wxSize(250, -1), 0, m_pInComboStrings, wxCB_READONLY );
    //itemBoxSizer11->Add(m_pInCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

}

//---------------------------------------------------------------------------------------
bool WizardDevicesPage::ShowToolTips()
{
    // Should we show tooltips?
    return true;
}

//---------------------------------------------------------------------------------------
wxBitmap WizardDevicesPage::GetBitmapResource( const wxString& name )
{
    return wxNullBitmap;
}

//---------------------------------------------------------------------------------------
wxIcon WizardDevicesPage::GetIconResource( const wxString& name )
{
    return wxNullIcon;
}

//---------------------------------------------------------------------------------------
bool WizardDevicesPage::TransferDataFromWindow()
{
    // Save temporary data and open temporary Midi Devices

    //get number of Midi device to use for output
    int nIndex = m_pOutCombo->GetSelection();
    #if defined(__ia64__) || defined(__amd64__)
        //In Linux 64bits next sentence produces a compilation error: cast from 'void*' to
        //'int' loses precision. This double cast fixes the issue.
        int nOutDevId = static_cast<int>(
                reinterpret_cast<long long>(m_pOutCombo->GetClientData(nIndex) ) );
    #else
        int nOutDevId = (int)(size_t) m_pOutCombo->GetClientData(nIndex);
    #endif
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->SetOutDevice(nOutDevId);

    //open input device
    int nInDevId = -1;
	//TODO: Un-comment when ready to use MIDI input
    //if (m_pInCombo->GetStringSelection() != _("None") ) {
    //    nIndex = m_pInCombo->GetSelection();
    //    nInDevId = (int) m_pInCombo->GetClientData(nIndex);
    //}
    pMidi->SetInDevice(nInDevId);

    return true;

}



//=======================================================================================
// WizardInstrumentsPage implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE( WizardInstrumentsPage, wxWizardPageSimple )
    EVT_COMBOBOX( ID_COMBO_SECTION, WizardInstrumentsPage::OnComboSection )
    EVT_COMBOBOX( ID_COMBO_INSTRUMENT, WizardInstrumentsPage::OnComboInstrument )
    EVT_BUTTON( ID_BUTTON_TEST_SOUND, WizardInstrumentsPage::OnButtonTestSoundClick )
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
WizardInstrumentsPage::WizardInstrumentsPage(ApplicationScope& appScope, wxWizard* parent)
    : m_appScope(appScope)
{
    Create( parent );
}

//---------------------------------------------------------------------------------------
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
        m_pVoiceChannelCombo->Append(wxString::Format("%d", i));
    }
    //Set selection according to current user prefs
    MidiServer* pMidi = m_appScope.get_midi_server();
    m_pVoiceChannelCombo->SetSelection( pMidi->VoiceChannel() );

    //populate sections and instruments combos
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nInstr = pMidi->VoiceInstr();
    int nSect = pMidiGM->PopulateWithSections((wxControlWithItems*)m_pSectCombo, nInstr );
    pMidiGM->PopulateWithInstruments((wxControlWithItems*)m_pInstrCombo, nSect, nInstr);

    return true;
}

//---------------------------------------------------------------------------------------
void WizardInstrumentsPage::CreateControls()
{
    wxBoxSizer* itemBoxSizer17 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    SetSizer(itemBoxSizer17);

    wxBoxSizer* itemBoxSizer18 = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer17->Add(itemBoxSizer18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText19 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Voice channel and instrument"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText19->SetFont(wxFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial"));
    itemBoxSizer18->Add(itemStaticText19, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer20 = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer17->Add(itemBoxSizer20, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer21 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    itemBoxSizer20->Add(itemBoxSizer21, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText22 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Channels 10 and 16 are specialized in percussion sounds. So it is recommended to choose any other channel (it doesn't matter which one)."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer21->Add(itemStaticText22, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText23 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("To facilitate access to the instruments they are grouped into sections. First choose a section and then choose the desired instrument."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer21->Add(itemStaticText23, 1, wxGROW|wxALL, 5);

    wxStaticLine* itemStaticLine24 = LENMUS_NEW wxStaticLine( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer20->Add(itemStaticLine24, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer25 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    itemBoxSizer20->Add(itemBoxSizer25, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText26 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Channel:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText26, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_pVoiceChannelComboStrings = NULL;
    m_pVoiceChannelCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_CHANNEL, "", wxDefaultPosition, wxSize(70, -1), 0, m_pVoiceChannelComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pVoiceChannelCombo, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    itemBoxSizer25->Add(itemBoxSizer28, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText29 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Section:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText29, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_pSectComboStrings = NULL;
    m_pSectCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_SECTION, "", wxDefaultPosition, wxSize(250, -1), 0, m_pSectComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pSectCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText31 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Instrument:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText31, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_pInstrComboStrings = NULL;
    m_pInstrCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_INSTRUMENT, "", wxDefaultPosition, wxSize(250, -1), 0, m_pInstrComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pInstrCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxButton* itemButton33 = LENMUS_NEW wxButton( this, ID_BUTTON_TEST_SOUND, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemButton33, 0, wxALIGN_LEFT|wxALL, 5);
}

//---------------------------------------------------------------------------------------
bool WizardInstrumentsPage::ShowToolTips()
{
    // Should we show tooltips?
    return true;
}

//---------------------------------------------------------------------------------------
wxBitmap WizardInstrumentsPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return wxNullBitmap;
}

//---------------------------------------------------------------------------------------
wxIcon WizardInstrumentsPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return wxNullIcon;
}

//---------------------------------------------------------------------------------------
bool WizardInstrumentsPage::TransferDataFromWindow()
{
    //
    // Save temporary data and set temporary Midi program
    //

    DoProgramChange();
    return true;

}

//---------------------------------------------------------------------------------------
void WizardInstrumentsPage::DoProgramChange()
{
    //Change Midi instrument to the one selected in combo Instruments

    int nInstr = m_pInstrCombo->GetSelection();
    int nSect = m_pSectCombo->GetSelection();
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nVoiceInstr = pMidiGM->GetInstrFromSection(nSect, nInstr);
    int nVoiceChannel = m_pVoiceChannelCombo->GetSelection();
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->VoiceChange(nVoiceChannel, nVoiceInstr);
}

//---------------------------------------------------------------------------------------
void WizardInstrumentsPage::OnComboSection( wxCommandEvent& event )
{
    // A new section selected. Reload Instruments combo with the instruments in the
    //selected section

    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nSect = m_pSectCombo->GetSelection();
    pMidiGM->PopulateWithInstruments((wxControlWithItems*)m_pInstrCombo, nSect);
    DoProgramChange();

}

//---------------------------------------------------------------------------------------
void WizardInstrumentsPage::OnComboInstrument( wxCommandEvent& event )
{
    // A new instrument selected. Change Midi program
    DoProgramChange();
}

//---------------------------------------------------------------------------------------
void WizardInstrumentsPage::OnButtonTestSoundClick( wxCommandEvent& event )
{
    //play a scale
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->TestOut();
}


//=======================================================================================
// WizardMetronomePage implementation
//=======================================================================================

//wxIMPLEMENT_DYNAMIC_CLASS( WizardMetronomePage, wxWizardPageSimple );

wxBEGIN_EVENT_TABLE( WizardMetronomePage, wxWizardPageSimple )
    EVT_COMBOBOX( ID_COMBO_MTR_INSTR1, WizardMetronomePage::OnComboMtrInstr1Selected )
    EVT_COMBOBOX( ID_COMBO_MTR_INSTR2, WizardMetronomePage::OnComboMtrInstr2Selected )
    EVT_BUTTON( ID_BUTTON, WizardMetronomePage::OnButtonClick )
wxEND_EVENT_TABLE()

////---------------------------------------------------------------------------------------
//WizardMetronomePage::WizardMetronomePage( )
//{
//}

//---------------------------------------------------------------------------------------
WizardMetronomePage::WizardMetronomePage(ApplicationScope& appScope, wxWizard* parent)
    : m_appScope(appScope)
{
    Create( parent );
}

//---------------------------------------------------------------------------------------
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
        m_pMtrChannelCombo->Append(wxString::Format("%d", i));
    }

    //Set selection according to current user prefs
    MidiServer* pMidi = m_appScope.get_midi_server();
    m_pMtrChannelCombo->SetSelection( pMidi->MtrChannel() );

    //populate metronome sounds combos
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nTone1 = pMidi->MtrTone1();
    int nTone2 = pMidi->MtrTone2();
    pMidiGM->PopulateWithPercusionInstr((wxControlWithItems*)m_pMtrInstr1Combo, nTone1);
    pMidiGM->PopulateWithPercusionInstr((wxControlWithItems*)m_pMtrInstr2Combo, nTone2);

    return true;
}

//---------------------------------------------------------------------------------------
void WizardMetronomePage::CreateControls()
{
    wxBoxSizer* itemBoxSizer35 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    SetSizer(itemBoxSizer35);

    wxBoxSizer* itemBoxSizer36 = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer35->Add(itemBoxSizer36, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText37 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Metronome channel and sounds"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText37->SetFont(wxFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial"));
    itemBoxSizer36->Add(itemStaticText37, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer38 = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer35->Add(itemBoxSizer38, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer39 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    itemBoxSizer38->Add(itemBoxSizer39, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText40 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Channels 10 and 16 are specialized in percussion sounds. So it is recommended to choose one of these (it doesn't matter which one)."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer39->Add(itemStaticText40, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText41 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("To better identify the first beat of each measure it is possible to assign a different sound to it. But you can also choose the same sound for both, the first beat and the others."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer39->Add(itemStaticText41, 1, wxGROW|wxALL, 5);

    wxStaticLine* itemStaticLine42 = LENMUS_NEW wxStaticLine( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer38->Add(itemStaticLine42, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer43 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    itemBoxSizer38->Add(itemBoxSizer43, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText44 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Channel:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText44, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_pMtrChannelComboStrings = NULL;
    m_pMtrChannelCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_MTR_CHANNEL, "", wxDefaultPosition, wxSize(70, -1), 0, m_pMtrChannelComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrChannelCombo, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer46 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    itemBoxSizer43->Add(itemBoxSizer46, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText47 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Sound for first beat of each measure:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText47, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_pMtrInstr1ComboStrings = NULL;
    m_pMtrInstr1Combo = LENMUS_NEW wxComboBox( this, ID_COMBO_MTR_INSTR1, "", wxDefaultPosition, wxSize(250, -1), 0, m_pMtrInstr1ComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrInstr1Combo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText49 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Sound for other beats of each measure:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText49, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_pMtrInstr2ComboStrings = NULL;
    m_pMtrInstr2Combo = LENMUS_NEW wxComboBox( this, ID_COMBO_MTR_INSTR2, "", wxDefaultPosition, wxSize(250, -1), 0, m_pMtrInstr2ComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrInstr2Combo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxButton* itemButton51 = LENMUS_NEW wxButton( this, ID_BUTTON, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemButton51, 0, wxALIGN_LEFT|wxALL, 5);

}

//---------------------------------------------------------------------------------------
bool WizardMetronomePage::ShowToolTips()
{
    // Should we show tooltips?
    return true;
}

//---------------------------------------------------------------------------------------
wxBitmap WizardMetronomePage::GetBitmapResource( const wxString& name )
{
    // Get bitmap resources
    return wxNullBitmap;
}

//---------------------------------------------------------------------------------------
wxIcon WizardMetronomePage::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return wxNullIcon;
}

//---------------------------------------------------------------------------------------
void WizardMetronomePage::OnComboMtrInstr1Selected( wxCommandEvent& event )
{
    //Change metronome sound, tone1, to the one selected in combo Instr1
    int nTone1 = m_pMtrInstr1Combo->GetSelection() + 35;
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->SetMetronomeTones(nTone1, pMidi->MtrTone2());
}

//---------------------------------------------------------------------------------------
void WizardMetronomePage::OnComboMtrInstr2Selected( wxCommandEvent& event )
{
    //Change metronome sound, tone2, to the one selected in combo Instr2
    int nTone2 = m_pMtrInstr2Combo->GetSelection() + 35;
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->SetMetronomeTones(pMidi->MtrTone1(), nTone2);
}

//---------------------------------------------------------------------------------------
void WizardMetronomePage::OnButtonClick( wxCommandEvent& event )
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    wxMidiOutDevice* pMidiOut = pMidi->get_out_device();
    if (!pMidiOut) return;

    //two measures, 3/4 time signature
    for (int i=0; i < 2; i++) {
        //firts beat
        pMidiOut->NoteOn(pMidi->MtrChannel(), pMidi->MtrTone1(), 127);
        ::wxMilliSleep(500);    // wait 500ms
        pMidiOut->NoteOff(pMidi->MtrChannel(), pMidi->MtrTone1(), 127);
        // two more beats
        for (int j=0; j < 2; j++) {
            pMidiOut->NoteOn(pMidi->MtrChannel(), pMidi->MtrTone2(), 127);
            ::wxMilliSleep(500);    // wait 500ms
            pMidiOut->NoteOff(pMidi->MtrChannel(), pMidi->MtrTone2(), 127);
        }
    }
}


}   //namespace lenmus
