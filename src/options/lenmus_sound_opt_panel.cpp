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
#include "lenmus_paths.h"

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
    , m_fSettingsChanged(false)
{
    create_controls();
    initialize_controls();
}

//---------------------------------------------------------------------------------------
SoundOptionsPanel::~SoundOptionsPanel()
{
    if (m_fSettingsChanged)
        restore_old_settings();
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
	m_pTxtTitle->SetFont( wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma" ) );

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
	OptInternalSynthPanel* pPanelIntSynth = new OptInternalSynthPanel(m_appScope, m_pTabs);
	m_pTabs->AddPage( pPanelIntSynth, _("Internal synth."), false );
	OptExternalSynthPanel* pPanelExtSynth = new OptExternalSynthPanel(m_appScope, m_pTabs);
	m_pTabs->AddPage( pPanelExtSynth, _("External synth."), false );
	OptInstrumentsPanel* pPanelInstrument = new OptInstrumentsPanel(m_appScope, m_pTabs);
	m_pTabs->AddPage( pPanelInstrument, _("Instrument sound"), false );
	OptMetronomePanel* pPanelMetronome = new OptMetronomePanel(m_appScope, m_pTabs);
	m_pTabs->AddPage( pPanelMetronome, _("Metronome sound"), false );

	pMainSizer->Add( m_pTabs, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( pMainSizer );
	this->Layout();

	// Connect Events
	m_pRadSynthesizer->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( SoundOptionsPanel::on_new_synthesizer ), nullptr, this );
	m_pTestSoundButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SoundOptionsPanel::on_test_sound ), nullptr, this );
	m_pResetDefaultsButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SoundOptionsPanel::on_reset_to_defaults ), nullptr, this );
}

//---------------------------------------------------------------------------------------
void SoundOptionsPanel::initialize_controls()
{
    //load icon
    m_pBmpIconTitle->SetBitmap( wxArtProvider::GetIcon("opt_sound", wxART_TOOLBAR, wxSize(24,24)) );

   //Select current settings
    MidiServer* pMidi = m_appScope.get_midi_server();
    m_fOldUseInternalSynth = pMidi->is_using_internal_synth();
    m_pRadSynthesizer->SetSelection(m_fOldUseInternalSynth ? 1 : 0);
}

//---------------------------------------------------------------------------------------
void SoundOptionsPanel::on_new_synthesizer(wxCommandEvent& WXUNUSED(event))
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    bool value = m_pRadSynthesizer->GetSelection() == 1;
    pMidi->use_internal_synth(value);

    m_fSettingsChanged = true;
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
    pMidi->set_metronome_tones(76, 77);     //76-High Wood Block, 77-Low Wood Block
    FluidSynthesizer* pSynth = pMidi->get_internal_synth();
    if (pSynth)
        pSynth->reset_to_defaults();

    initialize_controls();
    size_t maxPage = m_pTabs->GetPageCount();
    for(size_t i=0; i < maxPage; ++i)
    {
        OptionsTab* pPage = static_cast<OptionsTab*>(m_pTabs->GetPage(i));
        pPage->initialize_controls();
        pPage->save_current_settings();
    }

	m_fSettingsChanged = false;
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

    m_fOldUseInternalSynth = m_pRadSynthesizer->GetSelection() == 1;
    pMidi->use_internal_synth(m_fOldUseInternalSynth);

    size_t maxPage = m_pTabs->GetPageCount();
    for(size_t i=0; i < maxPage; ++i)
    {
        OptionsTab* pPage = static_cast<OptionsTab*>(m_pTabs->GetPage(i));
        pPage->apply_settings();
    }

	m_fSettingsChanged = false;
}

//---------------------------------------------------------------------------------------
void SoundOptionsPanel::restore_old_settings()
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->use_internal_synth(m_fOldUseInternalSynth);

    size_t maxPage = m_pTabs->GetPageCount();
    for(size_t i=0; i < maxPage; ++i)
    {
        OptionsTab* pPage = static_cast<OptionsTab*>(m_pTabs->GetPage(i));
        pPage->restore_old_settings();
    }
}


//=======================================================================================
//  OptInternalSynthPanel implementation
//=======================================================================================
OptInternalSynthPanel::OptInternalSynthPanel(ApplicationScope& appScope, wxWindow* parent)
	: OptionsTab(appScope, parent)
{
    create_controls();
    initialize_controls();
}

//---------------------------------------------------------------------------------------
void OptInternalSynthPanel::create_controls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* pTitleSpacer;
	pTitleSpacer = new wxBoxSizer( wxHORIZONTAL );


	pTitleSpacer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pTitleText = new wxStaticText( this, wxID_ANY, _("SoundFont to use"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_pTitleText->Wrap( -1 );
	m_pTitleText->SetFont( wxFont( 14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial" ) );

	pTitleSpacer->Add( m_pTitleText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	pTitleSpacer->Add( 0, 0, 1, wxEXPAND, 5 );


	pMainSizer->Add( pTitleSpacer, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );

	m_pHelpText1 = new wxStaticText( this, wxID_ANY, _("A Soundfont is a file (.sf2/.sf3) with a collection of sample-based instrument sounds. The internal synthesizer needs a SoundFont to generate the sounds."), wxDefaultPosition, wxDefaultSize, 0 );
	m_pHelpText1->Wrap( -1 );
	pMainSizer->Add( m_pHelpText1, 1, wxALL, 5 );

	wxBoxSizer* pSoundFontTextSizer;
	pSoundFontTextSizer = new wxBoxSizer( wxVERTICAL );

	pSoundfontText = new wxStaticText( this, wxID_ANY, _("Soundfont to use"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	pSoundfontText->Wrap( -1 );
	pSoundFontTextSizer->Add( pSoundfontText, 0, wxLEFT|wxRIGHT|wxTOP, 5 );

	wxBoxSizer* pEntryFieldsSizer;
	pEntryFieldsSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pTxtSoundfont = new wxTextCtrl( this, wxID_ANY, "FluidR3_GM.sf2", wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	pEntryFieldsSizer->Add( m_pTxtSoundfont, 1, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5 );


	pSoundFontTextSizer->Add( pEntryFieldsSizer, 0, wxEXPAND, 5 );


	pMainSizer->Add( pSoundFontTextSizer, 1, wxRIGHT|wxEXPAND, 5 );

	m_pButtonChange = new wxButton( this, wxID_ANY, _("Change"), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add( m_pButtonChange, 0, wxALIGN_RIGHT|wxBOTTOM|wxRIGHT, 5 );


	pMainSizer->Add( 0, 0, 4, wxEXPAND, 5 );


	this->SetSizer( pMainSizer );
	this->Layout();

	// Connect Events
	m_pButtonChange->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OptInternalSynthPanel::on_button_change ), nullptr, this );
}

OptInternalSynthPanel::~OptInternalSynthPanel()
{
	// Disconnect Events
	m_pButtonChange->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OptInternalSynthPanel::on_button_change ), nullptr, this );
}

//---------------------------------------------------------------------------------------
void OptInternalSynthPanel::initialize_controls()
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    FluidSynthesizer* pSynth = pMidi->get_internal_synth();

    m_pTxtSoundfont->SetValue( to_wx_string(pSynth->get_soundfont()) );

	m_fSettingsChanged = false;
}

//---------------------------------------------------------------------------------------
void OptInternalSynthPanel::change_settings()
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    FluidSynthesizer* pSynth = pMidi->get_internal_synth();

    pSynth->load_soundfont( to_std_string(m_pTxtSoundfont->GetValue()) );
}

//---------------------------------------------------------------------------------------
void OptInternalSynthPanel::restore_old_settings()
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    FluidSynthesizer* pSynth = pMidi->get_internal_synth();

    pSynth->load_soundfont(m_OldSoundfont);
}

//---------------------------------------------------------------------------------------
void OptInternalSynthPanel::save_current_settings()
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    FluidSynthesizer* pSynth = pMidi->get_internal_synth();

    m_OldSoundfont = pSynth->get_soundfont();
}

//---------------------------------------------------------------------------------------
void OptInternalSynthPanel::on_button_change(wxCommandEvent& event)
{
    Paths* pPaths = m_appScope.get_paths();
    wxString soundsPath = pPaths->GetSoundFontsPath();

    // ask for the file to open/import
    wxString sFilter = "All supported SoundFonts|*.sf2;*.sf3";

    wxString sFile = ::wxFileSelector(_("Choose the SoundFont to load"),
                                      soundsPath,   //default path
                                      "",        //default filename
                                      "",        //default_extension
                                      sFilter,
                                      wxFD_OPEN,      //flags
                                      this);

    if (!sFile.empty())
    {
        m_pTxtSoundfont->SetValue( sFile );
        on_change_settings(event);
    }

    event.Skip();
}


//=======================================================================================
//  OptExternalSynthPanel implementation
//=======================================================================================
OptExternalSynthPanel::OptExternalSynthPanel(ApplicationScope& appScope, wxWindow* parent)
	: OptionsTab(appScope, parent)
    , m_nOldOutDevId(-1)
    , m_nOldInDevId(-1)
    , m_pOutCombo(nullptr)
    , m_pInCombo(nullptr)
{
    create_controls();
    initialize_controls();
    save_current_settings();
}

//---------------------------------------------------------------------------------------
void OptExternalSynthPanel::create_controls()
{
    wxBoxSizer* pMainSizer = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);

    wxBoxSizer* itemBoxSizer4 = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);
    pMainSizer->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText5 = LENMUS_NEW wxStaticText( this, wxID_STATIC, _("Midi devices to use"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText5->SetFont(wxFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial"));
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer6 = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);
    pMainSizer->Add(itemBoxSizer6, 1, wxGROW|wxALL, 5);

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
	m_pOutCombo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptionsTab::on_change_settings ), nullptr, this );
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
    int nOldOutDevId = pExtSynth->OutDevId();
    int iSelOut = 0;
//    //TODO: Un-comment when ready to use MIDI input
//    int nOldInDevId = pMidi->InDevId();
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
            if (nOldOutDevId == i)
                iSelOut = nItem;
        }
		////TODO: Un-comment when ready to use MIDI input
        //if (pMidiDev->IsInputPort()) {
        //    nInput++;
        //    nItem = m_pInCombo->Append( pMidiDev->DeviceName() );
        //    m_pInCombo->SetClientData(nItem, (void *)i);
        //    if (nOldInDevId == i)
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

	m_fSettingsChanged = false;
}

//---------------------------------------------------------------------------------------
void OptExternalSynthPanel::change_settings()
{
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
    if (pExtSynth)
    {
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
}

//---------------------------------------------------------------------------------------
void OptExternalSynthPanel::save_current_settings()
{
    m_nOldOutDevId = -1;
    m_nOldInDevId = -1;

    MidiServer* pMidi = m_appScope.get_midi_server();
    ExternalSynthesizer* pExtSynth = pMidi->get_external_synth();
    if (pExtSynth)
    {
        m_nOldOutDevId = pExtSynth->OutDevId();
//        m_nOldInDevId = pExtSynth->InDevId();
    }
}

//---------------------------------------------------------------------------------------
void OptExternalSynthPanel::restore_old_settings()
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    ExternalSynthesizer* pExtSynth = pMidi->get_external_synth();
    if (pExtSynth)
    {
        pExtSynth->SetInDevice(m_nOldInDevId);
        pExtSynth->SetOutDevice(m_nOldOutDevId);
    }
}



//=======================================================================================
// OptInstrumentsPanel implementation
//=======================================================================================
OptInstrumentsPanel::OptInstrumentsPanel(ApplicationScope& appScope, wxWindow* parent)
	: OptionsTab(appScope, parent)
    , m_nOldVoiceInstr(0)
    , m_nOldVoiceChannel(0)
    , m_pVoiceChannelCombo(nullptr)
    , m_pSectCombo(nullptr)
    , m_pInstrCombo(nullptr)
{
    create_controls();
    initialize_controls();
    save_current_settings();
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

    wxButton* pButtonTest = LENMUS_NEW wxButton( this, ID_BUTTON, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(pButtonTest, 0, wxALIGN_LEFT|wxALL, 5);

	// Connect Events
	m_pVoiceChannelCombo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptionsTab::on_change_settings ), nullptr, this );
	m_pSectCombo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptionsTab::on_change_settings ), nullptr, this );
	m_pInstrCombo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptionsTab::on_change_settings ), nullptr, this );
	pButtonTest->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OptInstrumentsPanel::on_test_sound ), nullptr, this );
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

	m_fSettingsChanged = false;
}

//---------------------------------------------------------------------------------------
void OptInstrumentsPanel::change_settings()
{
    MidiServer* pMidi = m_appScope.get_midi_server();

    int nChannel = m_pVoiceChannelCombo->GetSelection();

    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nSect = m_pSectCombo->GetSelection();
    int nInstr = m_pInstrCombo->GetSelection();
    pMidiGM->PopulateWithInstruments((wxControlWithItems*)m_pInstrCombo, nSect);
    m_pInstrCombo->SetSelection(nInstr);

    int nVoiceInstr = pMidiGM->GetInstrFromSection(nSect, nInstr);

    pMidi->VoiceChange(nChannel, nVoiceInstr);
}

//---------------------------------------------------------------------------------------
void OptInstrumentsPanel::save_current_settings()
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    m_nOldVoiceInstr = pMidi->get_voice_instr();
    m_nOldVoiceChannel = pMidi->get_voice_channel();
}

//---------------------------------------------------------------------------------------
void OptInstrumentsPanel::restore_old_settings()
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->VoiceChange(m_nOldVoiceChannel, m_nOldVoiceInstr);
}

//---------------------------------------------------------------------------------------
void OptInstrumentsPanel::on_test_sound( wxCommandEvent& WXUNUSED(event))
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->do_sound_test();
}


//=======================================================================================
// OptMetronomePanel implementation
//=======================================================================================
OptMetronomePanel::OptMetronomePanel(ApplicationScope& appScope, wxWindow* parent)
	: OptionsTab(appScope, parent)
    , m_nOldMtrInstr(0)
    , m_nOldMtrChannel(9)
    , m_nOldMtrTone1(76)
    , m_nOldMtrTone2(77)
    , m_pMtrChannelCombo(nullptr)
    , m_pMtrInstr1Combo(nullptr)
    , m_pMtrInstr2Combo(nullptr)
{
    create_controls();
    initialize_controls();
    save_current_settings();
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
	m_pMtrChannelCombo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptionsTab::on_change_settings ), nullptr, this );
	m_pMtrInstr1Combo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptionsTab::on_change_settings ), nullptr, this );
	m_pMtrInstr2Combo->Connect(wxEVT_COMBOBOX, wxCommandEventHandler( OptionsTab::on_change_settings ), nullptr, this );
	pButtonTest->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OptMetronomePanel::on_test_sound ), nullptr, this );
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

	m_fSettingsChanged = false;
}

//---------------------------------------------------------------------------------------
void OptMetronomePanel::on_test_sound( wxCommandEvent& WXUNUSED(event))
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    Synthesizer* pSynth = pMidi->get_current_synth();
    if (!pSynth)
        return;

    //two measures, 3/4 time signature
    int channel = pMidi->MtrChannel();
    int instr = pMidi->MtrInstr();
    pSynth->program_change(channel, instr);
    for (int i=0; i < 2; i++) {
        //firts beat
        pSynth->note_on(channel, pMidi->MtrTone1(), 127);
        ::wxMilliSleep(500);    // wait 500ms
        pSynth->note_off(channel, pMidi->MtrTone1(), 127);
        // two more beats
        for (int j=0; j < 2; j++) {
            pSynth->note_on(channel, pMidi->MtrTone2(), 127);
            ::wxMilliSleep(500);    // wait 500ms
            pSynth->note_off(channel, pMidi->MtrTone2(), 127);
        }
    }
}

//---------------------------------------------------------------------------------------
void OptMetronomePanel::change_settings()
{
    MidiServer* pMidi = m_appScope.get_midi_server();

    int nChannel = m_pMtrChannelCombo->GetSelection();
    int nInstr = pMidi->MtrInstr();
    pMidi->set_metronome_program(nChannel, nInstr);

    int nTone1 = m_pMtrInstr1Combo->GetSelection() + 35;
    pMidi->set_metronome_tones(nTone1, pMidi->MtrTone2());

    int nTone2 = m_pMtrInstr2Combo->GetSelection() + 35;
    pMidi->set_metronome_tones(pMidi->MtrTone1(), nTone2);
}

//---------------------------------------------------------------------------------------
void OptMetronomePanel::save_current_settings()
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    m_nOldMtrInstr = pMidi->MtrInstr();
    m_nOldMtrChannel = pMidi->MtrChannel();
    m_nOldMtrTone1 = pMidi->MtrTone1();
    m_nOldMtrTone2 = pMidi->MtrTone2();
}

//---------------------------------------------------------------------------------------
void OptMetronomePanel::restore_old_settings()
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    pMidi->set_metronome_program(m_nOldMtrChannel, m_nOldMtrInstr);
    pMidi->set_metronome_tones(m_nOldMtrTone1, m_nOldMtrTone2);
}


}   //namespace lenmus
