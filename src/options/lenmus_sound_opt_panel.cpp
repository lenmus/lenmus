//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2020 LenMus project
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

//lenmus
#include "lenmus_sound_opt_panel.h"
#include "lenmus_injectors.h"
#include "lenmus_midi_server.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

#include <wx/statline.h>


namespace lenmus
{

//=======================================================================================
//  SoundOptionsPanel implementation
//=======================================================================================
SoundOptionsPanel::SoundOptionsPanel(wxWindow* parent, ApplicationScope& appScope)
    : OptionsPanel(parent, appScope)
{
    create_controls();
    initialize_controls();
}

//---------------------------------------------------------------------------------------
SoundOptionsPanel::~SoundOptionsPanel()
{
}

//---------------------------------------------------------------------------------------
void SoundOptionsPanel::create_controls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	m_pHeaderPanel = LENMUS_NEW wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* pHeaderSizer;
	pHeaderSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pTxtTitle = LENMUS_NEW wxStaticText( m_pHeaderPanel, wxID_ANY, _("Sound configuration"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_pTxtTitle->Wrap( -1 );
	m_pTxtTitle->SetFont( wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma" ) );

	pHeaderSizer->Add( m_pTxtTitle, 0, wxALIGN_TOP|wxALL, 5 );

	pHeaderSizer->Add( 5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pBmpIconTitle = LENMUS_NEW wxStaticBitmap( m_pHeaderPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pHeaderSizer->Add( m_pBmpIconTitle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pHeaderPanel->SetSizer( pHeaderSizer );
	m_pHeaderPanel->Layout();
	pHeaderSizer->Fit( m_pHeaderPanel );
	pMainSizer->Add( m_pHeaderPanel, 0, wxEXPAND|wxBOTTOM, 5 );

	//-----------
	wxBoxSizer* pTopSizer;
	pTopSizer = new wxBoxSizer( wxHORIZONTAL );

	wxString m_pRadSynthesizerChoices[] = { _("Use external synthesizer"), _("Use internal synthesizer") };
	int m_pRadSynthesizerNChoices = sizeof( m_pRadSynthesizerChoices ) / sizeof( wxString );
	m_pRadSynthesizer = new wxRadioBox( this, id_new_synthesizer, _("Choose synthesizer to use"), wxDefaultPosition, wxDefaultSize, m_pRadSynthesizerNChoices, m_pRadSynthesizerChoices, 1, wxRA_SPECIFY_COLS );
	m_pRadSynthesizer->SetSelection( 0 );
	pTopSizer->Add( m_pRadSynthesizer, 2, wxALL, 5 );


	pTopSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pTestSoundButton = new wxButton( this, id_test_sound, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
	pTopSizer->Add( m_pTestSoundButton, 1, wxALL, 5 );

	m_pResetDefaultsButton = new wxButton( this, id_reset_to_defaults, _("Reset to defaults"), wxDefaultPosition, wxDefaultSize, 0 );
	pTopSizer->Add( m_pResetDefaultsButton, 1, wxALL, 5 );

	pMainSizer->Add( pTopSizer, 0, wxEXPAND, 5 );

    //--------
	m_pTabs = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_pPanelIntSynth = new OptExternalSynthPanel(m_appScope, m_pTabs);
	m_pTabs->AddPage( m_pPanelIntSynth, _("Internal synth"), false );
	m_pPanelExtSynth = new OptInternalSynthPanel(m_appScope, m_pTabs);
	m_pTabs->AddPage( m_pPanelExtSynth, _("External synth"), false );
	m_pPanelInstrument = new OptInstrumentsPanel(m_appScope, m_pTabs);
	m_pTabs->AddPage( m_pPanelInstrument, _("Config. Instrument"), false );
	m_pPanelMetronome = new OptMetronomePanel(m_appScope, m_pTabs);
	m_pTabs->AddPage( m_pPanelMetronome, _("Config. Metronome"), false );

	pMainSizer->Add( m_pTabs, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( pMainSizer );
	this->Layout();

	// Connect Events
	m_pRadSynthesizer->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( SoundOptionsPanel::on_new_synthesizer ), NULL, this );
	m_pTestSoundButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SoundOptionsPanel::on_test_sound ), NULL, this );
	m_pResetDefaultsButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SoundOptionsPanel::on_reset_to_defaults ), NULL, this );
}

//---------------------------------------------------------------------------------------
void SoundOptionsPanel::initialize_controls()
{
    //load icon
    m_pBmpIconTitle->SetBitmap( wxArtProvider::GetIcon("opt_sound", wxART_TOOLBAR, wxSize(24,24)) );

   //Select current settings
    MidiServer* pMidi = m_appScope.get_midi_server();
    bool fUseInternalSynth = pMidi->is_using_internal_synth();
    m_pRadSynthesizer->SetSelection(fUseInternalSynth ? 1 : 0);
}

//---------------------------------------------------------------------------------------
void SoundOptionsPanel::on_new_synthesizer(wxCommandEvent& WXUNUSED(event))
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    bool value = m_pRadSynthesizer->GetSelection() == 1;
    pMidi->use_internal_synth(value);
}

//---------------------------------------------------------------------------------------
void SoundOptionsPanel::on_test_sound(wxCommandEvent& WXUNUSED(event))
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->do_sound_test();
}

//---------------------------------------------------------------------------------------
void SoundOptionsPanel::on_reset_to_defaults(wxCommandEvent& WXUNUSED(event))
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->use_internal_synth(true);        //use internal synthesizer
    pMidi->VoiceChange(0, 0);               //channel 1, instr 1 (grand piano)
    pMidi->set_metronome_program(9, 0);     //channel 10, instrument 1
    pMidi->set_metronome_tones(76L, 77L);   //76-High Wood Block, 77-Low Wood Block
    initialize_controls();

	m_pPanelIntSynth->initialize_controls();
	m_pPanelExtSynth->initialize_controls();
	m_pPanelInstrument->initialize_controls();
	m_pPanelMetronome->initialize_controls();
}

//---------------------------------------------------------------------------------------
bool SoundOptionsPanel::Verify()
{
    return false;
}

//---------------------------------------------------------------------------------------
void SoundOptionsPanel::Apply()
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    bool value = m_pRadSynthesizer->GetSelection() == 1;
    pMidi->use_internal_synth(value);
    pPrefs->Write("/Midi/UseInternalSynth", value);
}


////=======================================================================================
////  MidiWizard implementation
////=======================================================================================
////---------------------------------------------------------------------------------------
//bool MidiWizard::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos )
//{
//    // member initialisation
//    m_pMtrChannelCombo = nullptr;
//
//    // creation
//    SetExtraStyle(GetExtraStyle()|wxWIZARD_EX_HELPBUTTON);
//    wxBitmap wizardBitmap(wxNullBitmap);
//    SoundOptionsPanel::Create( parent, id, _("MIDI configuration wizard"), wizardBitmap, pos );
//    CreateControls();
//
//    //save current Midi configuration to restore it if the wizard is cancelled
//    MidiServer* pMidi = m_appScope.get_midi_server();
//    ExternalSynthesizer* pExtSynth = pMidi->get_external_synth();
//    m_nOldInDevId = pExtSynth->InDevId();
//    m_nOldOutDevId = pExtSynth->OutDevId();
//
//    m_nOldVoiceInstr = pMidi->get_voice_instr();
//    m_nOldVoiceChannel = pMidi->get_voice_channel();
//    m_nOldMtrInstr = pMidi->MtrInstr();
//    m_nOldMtrChannel = pMidi->MtrChannel();
//    m_nOldMtrTone1 = pMidi->MtrTone1();
//    m_nOldMtrTone2 = pMidi->MtrTone2();
//
//    return true;
//}
//
////---------------------------------------------------------------------------------------
//void MidiWizard::OnWizardFinished( wxWizardEvent& WXUNUSED(event))
//{
//    MidiServer* pMidi = m_appScope.get_midi_server();
//    ExternalSynthesizer* pSynth = pMidi->get_external_synth();
//    pSynth->set_configured(true);
//    pSynth->save_user_preferences();
//}
//
////---------------------------------------------------------------------------------------
//void MidiWizard::OnWizardCancel( wxWizardEvent& WXUNUSED(event))
//{
//    // restore old configuration if any
//
//    MidiServer* pMidi = m_appScope.get_midi_server();
//    ExternalSynthesizer* pExtSynth = pMidi->get_external_synth();
//    if (pExtSynth && pExtSynth->is_configured())
//    {
//        //devices
//        MidiServer* pMidi = m_appScope.get_midi_server();
//        pExtSynth->SetInDevice(m_nOldInDevId);
//        pExtSynth->SetOutDevice(m_nOldOutDevId);
//
//        //voice instruments
//        pMidi->VoiceChange(m_nOldVoiceChannel, m_nOldVoiceInstr);
//
//        //metronome configuration
//        pMidi->VoiceChange(m_nOldMtrChannel, m_nOldMtrInstr);
//        pMidi->set_metronome_tones(m_nOldMtrTone1, m_nOldMtrTone2);
//    }
//}


//=======================================================================================
//  OptInternalSynthPanel implementation
//=======================================================================================
OptInternalSynthPanel::OptInternalSynthPanel(ApplicationScope& appScope, wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
    , m_appScope(appScope)
{
    create_controls();
    initialize_controls();
}

//---------------------------------------------------------------------------------------
void OptInternalSynthPanel::create_controls()
{
}

//---------------------------------------------------------------------------------------
void OptInternalSynthPanel::initialize_controls()
{
}


//=======================================================================================
//  OptExternalSynthPanel implementation
//=======================================================================================
OptExternalSynthPanel::OptExternalSynthPanel(ApplicationScope& appScope, wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
    , m_appScope(appScope)
    , m_pOutCombo(nullptr)
    , m_pInCombo(nullptr)
{
    create_controls();
    initialize_controls();
}

//---------------------------------------------------------------------------------------
void OptExternalSynthPanel::create_controls()
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

    wxString* m_pOutComboStrings = nullptr;
    m_pOutCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_OUT_DEVICES, "", wxDefaultPosition, wxSize(250, -1), 0, m_pOutComboStrings, wxCB_READONLY );
    itemBoxSizer11->Add(m_pOutCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

	//TODO: Un-comment when ready to use MIDI input
    //wxStaticText* itemStaticText14 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Input device:"), wxDefaultPosition, wxDefaultSize, 0 );
    //itemBoxSizer11->Add(itemStaticText14, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

	//TODO: Un-comment when ready to use MIDI input
    //wxString* m_pInComboStrings = nullptr;
    //m_pInCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_IN_DEVICES, "", wxDefaultPosition, wxSize(250, -1), 0, m_pInComboStrings, wxCB_READONLY );
    //itemBoxSizer11->Add(m_pInCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);


	// Connect Events
	m_pOutCombo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptExternalSynthPanel::on_out_device_selected ),
	                     NULL, this );
}

//---------------------------------------------------------------------------------------
void OptExternalSynthPanel::initialize_controls()
{
    // populate combo boxes with available Midi devices
    MidiServer* pMidi = m_appScope.get_midi_server();
    ExternalSynthesizer* pExtSynth = pMidi->get_external_synth();
    //int nInput=0;
    int nItem, nOutput=0;
    int nNumDevices = pExtSynth->CountDevices();
    int nOutDevId = pExtSynth->OutDevId();
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
            wxString name = pMidiDev->DeviceName();
            nItem = m_pOutCombo->Append(name);
            m_pOutCombo->SetClientData(nItem, (void*)(size_t)i);
            //wxLogMessage("[OptExternalSynthPanel::Create] nItem=%d, i=%d", nItem, i);
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
}

//---------------------------------------------------------------------------------------
void OptExternalSynthPanel::on_out_device_selected( wxCommandEvent& WXUNUSED(event))
{
    // A new section selected. Save data and open temporary Midi Devices

    //get number of Midi device to use for output
    int nIndex = m_pOutCombo->GetSelection();
    #if defined(__ia64__) || defined(__amd64__)
        //In 64bits architectures the next sentence produces a compilation error: cast
        //from 'void*' to 'int' loses precision. This double cast fixes the issue.
        int nOutDevId = static_cast<int>(
                reinterpret_cast<long long>(m_pOutCombo->GetClientData(nIndex) ) );
    #else
        int nOutDevId = (int)(size_t) m_pOutCombo->GetClientData(nIndex);
    #endif
    MidiServer* pMidi = m_appScope.get_midi_server();
    ExternalSynthesizer* pExtSynth = pMidi->get_external_synth();
    pExtSynth->SetOutDevice(nOutDevId);

    //open input device
    int nInDevId = -1;
	//TODO: Un-comment when ready to use MIDI input
    //if (m_pInCombo->GetStringSelection() != _("None") ) {
    //    nIndex = m_pInCombo->GetSelection();
    //    nInDevId = (int) m_pInCombo->GetClientData(nIndex);
    //}
    pExtSynth->SetInDevice(nInDevId);
}



//=======================================================================================
// OptInstrumentsPanel implementation
//=======================================================================================
OptInstrumentsPanel::OptInstrumentsPanel(ApplicationScope& appScope, wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
    , m_appScope(appScope)
    , m_pVoiceChannelCombo(nullptr)
    , m_pSectCombo(nullptr)
    , m_pInstrCombo(nullptr)
{
    create_controls();
    initialize_controls();
}

//---------------------------------------------------------------------------------------
void OptInstrumentsPanel::create_controls()
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

    wxString* m_pVoiceChannelComboStrings = nullptr;
    m_pVoiceChannelCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_CHANNEL, "", wxDefaultPosition, wxSize(70, -1), 0, m_pVoiceChannelComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pVoiceChannelCombo, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    itemBoxSizer25->Add(itemBoxSizer28, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText29 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Section:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText29, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_pSectComboStrings = nullptr;
    m_pSectCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_SECTION, "", wxDefaultPosition, wxSize(250, -1), 0, m_pSectComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pSectCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText31 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Instrument:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText31, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_pInstrComboStrings = nullptr;
    m_pInstrCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_INSTRUMENT, "", wxDefaultPosition, wxSize(250, -1), 0, m_pInstrComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pInstrCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

	// Connect Events
	m_pVoiceChannelCombo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptInstrumentsPanel::on_combo_channel ), NULL, this );
	m_pSectCombo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptInstrumentsPanel::on_combo_section ), NULL, this );
	m_pInstrCombo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptInstrumentsPanel::on_combo_instrument ), NULL, this );
}

//---------------------------------------------------------------------------------------
void OptInstrumentsPanel::initialize_controls()
{
    // populate channel combo
    m_pVoiceChannelCombo->Clear();
    for(int i=1; i <= 16; i++) {
        m_pVoiceChannelCombo->Append(wxString::Format("%d", i));
    }
    //Set selection according to current user prefs
    MidiServer* pMidi = m_appScope.get_midi_server();
    m_pVoiceChannelCombo->SetSelection( pMidi->get_voice_channel() );

    //populate sections and instruments combos
    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nInstr = pMidi->get_voice_instr();
    int nSect = pMidiGM->PopulateWithSections((wxControlWithItems*)m_pSectCombo, nInstr );
    pMidiGM->PopulateWithInstruments((wxControlWithItems*)m_pInstrCombo, nSect, nInstr);
}

//---------------------------------------------------------------------------------------
void OptInstrumentsPanel::DoProgramChange()
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
void OptInstrumentsPanel::on_combo_channel(wxCommandEvent& WXUNUSED(event))
{
    //A new channel has been selected

    int nChannel = m_pVoiceChannelCombo->GetSelection();
    MidiServer* pMidi = m_appScope.get_midi_server();
    int nInstr = pMidi->get_voice_instr();
    pMidi->VoiceChange(nChannel, nInstr);
}

//---------------------------------------------------------------------------------------
void OptInstrumentsPanel::on_combo_section(wxCommandEvent& WXUNUSED(event))
{
    // A new section selected. Reload Instruments combo with the instruments in the
    //selected section

    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nSect = m_pSectCombo->GetSelection();
    pMidiGM->PopulateWithInstruments((wxControlWithItems*)m_pInstrCombo, nSect);
    DoProgramChange();
}

//---------------------------------------------------------------------------------------
void OptInstrumentsPanel::on_combo_instrument( wxCommandEvent& WXUNUSED(event))
{
    // A new instrument selected. Change Midi program
    DoProgramChange();
}


//=======================================================================================
// OptMetronomePanel implementation
//=======================================================================================
OptMetronomePanel::OptMetronomePanel(ApplicationScope& appScope, wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
    , m_appScope(appScope)
    , m_pMtrChannelCombo(nullptr)
    , m_pMtrInstr1Combo(nullptr)
    , m_pMtrInstr2Combo(nullptr)
{
    create_controls();
    initialize_controls();
}

//---------------------------------------------------------------------------------------
void OptMetronomePanel::create_controls()
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

    wxString* m_pMtrChannelComboStrings = nullptr;
    m_pMtrChannelCombo = LENMUS_NEW wxComboBox( this, ID_COMBO_MTR_CHANNEL, "", wxDefaultPosition, wxSize(70, -1), 0, m_pMtrChannelComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrChannelCombo, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer46 = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    itemBoxSizer43->Add(itemBoxSizer46, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText47 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Sound for first beat of each measure:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText47, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_pMtrInstr1ComboStrings = nullptr;
    m_pMtrInstr1Combo = LENMUS_NEW wxComboBox( this, ID_COMBO_MTR_INSTR1, "", wxDefaultPosition, wxSize(250, -1), 0, m_pMtrInstr1ComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrInstr1Combo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText49 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Sound for other beats of each measure:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText49, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxString* m_pMtrInstr2ComboStrings = nullptr;
    m_pMtrInstr2Combo = LENMUS_NEW wxComboBox( this, ID_COMBO_MTR_INSTR2, "", wxDefaultPosition, wxSize(250, -1), 0, m_pMtrInstr2ComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrInstr2Combo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxButton* pButtonTest = LENMUS_NEW wxButton( this, ID_BUTTON, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(pButtonTest, 0, wxALIGN_LEFT|wxALL, 5);

	// Connect Events
	m_pMtrChannelCombo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptMetronomePanel::on_combo_channel ), NULL, this );
	m_pMtrInstr1Combo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptMetronomePanel::OnComboMtrInstr1Selected ), NULL, this );
	m_pMtrInstr2Combo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptMetronomePanel::OnComboMtrInstr2Selected ), NULL, this );
	pButtonTest->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OptMetronomePanel::on_test_sound ), NULL, this );
}

//---------------------------------------------------------------------------------------
void OptMetronomePanel::initialize_controls()
{
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
}

//---------------------------------------------------------------------------------------
void OptMetronomePanel::on_combo_channel(wxCommandEvent& WXUNUSED(event))
{
    //A new channel has been selected

    int nChannel = m_pMtrChannelCombo->GetSelection();
    MidiServer* pMidi = m_appScope.get_midi_server();
    int nInstr = pMidi->MtrInstr();
    pMidi->set_metronome_program(nChannel, nInstr);
}

//---------------------------------------------------------------------------------------
void OptMetronomePanel::OnComboMtrInstr1Selected( wxCommandEvent& WXUNUSED(event))
{
    //Change metronome sound, tone1, to the one selected in combo Instr1
    int nTone1 = m_pMtrInstr1Combo->GetSelection() + 35;
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->set_metronome_tones(nTone1, pMidi->MtrTone2());
}

//---------------------------------------------------------------------------------------
void OptMetronomePanel::OnComboMtrInstr2Selected( wxCommandEvent& WXUNUSED(event))
{
    //Change metronome sound, tone2, to the one selected in combo Instr2
    int nTone2 = m_pMtrInstr2Combo->GetSelection() + 35;
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->set_metronome_tones(pMidi->MtrTone1(), nTone2);
}

//---------------------------------------------------------------------------------------
void OptMetronomePanel::on_test_sound( wxCommandEvent& WXUNUSED(event))
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    Synthesizer* pSynth = pMidi->get_current_synth();
    if (!pSynth)
        return;

    //two measures, 3/4 time signature
    for (int i=0; i < 2; i++) {
        //firts beat
        pSynth->note_on(pMidi->MtrChannel(), pMidi->MtrTone1(), 127);
        ::wxMilliSleep(500);    // wait 500ms
        pSynth->note_off(pMidi->MtrChannel(), pMidi->MtrTone1(), 127);
        // two more beats
        for (int j=0; j < 2; j++) {
            pSynth->note_on(pMidi->MtrChannel(), pMidi->MtrTone2(), 127);
            ::wxMilliSleep(500);    // wait 500ms
            pSynth->note_off(pMidi->MtrChannel(), pMidi->MtrTone2(), 127);
        }
    }
}


}   //namespace lenmus
