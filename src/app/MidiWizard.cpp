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

////@begin includes
////@end includes

#include "MidiWizard.h"

// MIDI support throgh Portmidi lib
#include "../../wxMidi/include/wxMidi.h"

//access to Midi configuration
#include "../sound/MidiManager.h"

////@begin XPM images
////@end XPM images

/*!
 * lmMidiWizard type definition
 */

IMPLEMENT_DYNAMIC_CLASS( lmMidiWizard, wxWizard )

/*!
 * lmMidiWizard event table definition
 */

BEGIN_EVENT_TABLE( lmMidiWizard, wxWizard )

////@begin lmMidiWizard event table entries
    EVT_WIZARD_CANCEL( ID_WIZARD, lmMidiWizard::OnWizardCancel )
    EVT_WIZARD_FINISHED( ID_WIZARD, lmMidiWizard::OnWizardFinished )

////@end lmMidiWizard event table entries

END_EVENT_TABLE()

/*!
 * lmMidiWizard constructors
 */

lmMidiWizard::lmMidiWizard( )
{
}

lmMidiWizard::lmMidiWizard( wxWindow* parent, wxWindowID id, const wxPoint& pos )
{
    Create(parent, id, pos);
}

/*!
 * lmMidiWizard creator
 */

bool lmMidiWizard::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos )
{
////@begin lmMidiWizard member initialisation
    m_pMtrChannelCombo = NULL;
////@end lmMidiWizard member initialisation

////@begin lmMidiWizard creation
    SetExtraStyle(GetExtraStyle()|wxWIZARD_EX_HELPBUTTON);
    wxBitmap wizardBitmap(wxNullBitmap);
    wxWizard::Create( parent, id, _("MIDI configuration wizard"), wizardBitmap, pos );

    CreateControls();
////@end lmMidiWizard creation

    //save current Midi configuration to restore it if the wizard is cancelled
    m_nOldInDevId = g_pMidi->InDevId();
    m_nOldOutDevId = g_pMidi->OutDevId();
    m_nOldVoiceInstr = g_pMidi->VoiceInstr();
    m_nOldVoiceChannel = g_pMidi->VoiceChannel();
    m_nOldMtrInstr = g_pMidi->MtrInstr();
    m_nOldMtrChannel = g_pMidi->MtrChannel();
    m_nOldMtrTone1 = g_pMidi->MtrTone1();
    m_nOldMtrTone2 = g_pMidi->MtrTone2();

    return TRUE;
}

/*!
 * Control creation for lmMidiWizard
 */

void lmMidiWizard::CreateControls()
{    
////@begin lmMidiWizard content construction
    wxWizard* itemWizard1 = this;

    WizardDevicesPage* itemWizardPageSimple2 = new WizardDevicesPage( itemWizard1 );

    itemWizard1->FitToPage(itemWizardPageSimple2);
    WizardInstrumentsPage* itemWizardPageSimple16 = new WizardInstrumentsPage( itemWizard1 );

    itemWizard1->FitToPage(itemWizardPageSimple16);
    m_pMtrChannelCombo = new WizardMetronomePage( itemWizard1 );

    itemWizard1->FitToPage(m_pMtrChannelCombo);
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
////@end lmMidiWizard content construction
}

/*!
 * Runs the wizard.
 */

bool lmMidiWizard::Run()
{
    wxWizardPage* startPage = NULL;
    wxWindowListNode* node = GetChildren().GetFirst();
    while (node)
    {
        wxWizardPage* startPage = wxDynamicCast(node->GetData(), wxWizardPage);
        if (startPage) return RunWizard(startPage);
        node = node->GetNext();
    }
    return FALSE;
}

/*!
 * Should we show tooltips?
 */

bool lmMidiWizard::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap lmMidiWizard::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin lmMidiWizard bitmap retrieval
    return wxNullBitmap;
////@end lmMidiWizard bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon lmMidiWizard::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin lmMidiWizard icon retrieval
    return wxNullIcon;
////@end lmMidiWizard icon retrieval
}

/*!
 * wxEVT_WIZARD_FINISHED event handler for ID_WIZARD
 */

void lmMidiWizard::OnWizardFinished( wxWizardEvent& event )
{
    //take note that user has set Midi preferences
    g_pMidi->SetConfigured(true);

    //and save user MIDI preferences
    g_pMidi->SaveUserPreferences();

}

/*!
 * wxEVT_WIZARD_CANCEL event handler for ID_WIZARD
 */

void lmMidiWizard::OnWizardCancel( wxWizardEvent& event )
{
    /*
    restore old configuration if any
    */

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


/*!
 * WizardDevicesPage type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WizardDevicesPage, wxWizardPageSimple )

/*!
 * WizardDevicesPage event table definition
 */

BEGIN_EVENT_TABLE( WizardDevicesPage, wxWizardPageSimple )

////@begin WizardDevicesPage event table entries
////@end WizardDevicesPage event table entries

END_EVENT_TABLE()

/*!
 * WizardDevicesPage constructors
 */

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
////@begin WizardDevicesPage member initialisation
    m_pOutCombo = NULL;
    m_pInCombo = NULL;
////@end WizardDevicesPage member initialisation

////@begin WizardDevicesPage creation
    wxBitmap wizardBitmap(GetBitmapResource(wxT("wizard.png")));
    wxWizardPageSimple::Create( parent, NULL, NULL, wizardBitmap );

    CreateControls();
    GetSizer()->Fit(this);
////@end WizardDevicesPage creation

    // populate combo boxes with available Midi devices
    int nItem, nInput=0, nOutput=0;
    int nNumDevices = g_pMidiSystem->CountDevices();
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

    return TRUE;
}

/*!
 * Control creation for WizardPage
 */

void WizardDevicesPage::CreateControls()
{    
////@begin WizardDevicesPage content construction
    WizardDevicesPage* itemWizardPageSimple2 = this;

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemWizardPageSimple2->SetSizer(itemBoxSizer3);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( itemWizardPageSimple2, wxID_STATIC, _("Midi devices to use"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText5->SetFont(wxFont(14, wxSWISS, wxNORMAL, wxBOLD, FALSE, _T("Arial")));
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer6, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer6->Add(itemBoxSizer7, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemWizardPageSimple2, wxID_STATIC, _("To generate sounds the program needs a MIDI synthesizer device. Normally, one of these devices is included in the sound board of the PC, but your PC might have more than one."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer7->Add(itemStaticText8, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText9 = new wxStaticText( itemWizardPageSimple2, wxID_STATIC, _("If your PC has more than one device, choose one of them. You can test all of them and choose the one whose sound you prefer."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer7->Add(itemStaticText9, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticLine* itemStaticLine10 = new wxStaticLine( itemWizardPageSimple2, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer6->Add(itemStaticLine10, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer6->Add(itemBoxSizer11, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText12 = new wxStaticText( itemWizardPageSimple2, wxID_STATIC, _("Output device:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText12, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pOutComboStrings = NULL;
    m_pOutCombo = new wxComboBox( itemWizardPageSimple2, ID_COMBO_OUT_DEVICES, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pOutComboStrings, wxCB_READONLY );
    itemBoxSizer11->Add(m_pOutCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemWizardPageSimple2, wxID_STATIC, _("Input device:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText14, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pInComboStrings = NULL;
    m_pInCombo = new wxComboBox( itemWizardPageSimple2, ID_COMBO_IN_DEVICES, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pInComboStrings, wxCB_READONLY );
    itemBoxSizer11->Add(m_pInCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

////@end WizardDevicesPage content construction
}

/*!
 * Should we show tooltips?
 */

bool WizardDevicesPage::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap WizardDevicesPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin WizardDevicesPage bitmap retrieval
    if (name == wxT("wizard.png"))
    {
        wxBitmap bitmap(_T("wizard.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    return wxNullBitmap;
////@end WizardDevicesPage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WizardDevicesPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin WizardDevicesPage icon retrieval
    return wxNullIcon;
////@end WizardDevicesPage icon retrieval
}

bool WizardDevicesPage::TransferDataFromWindow()
{
    /*
    Save temporary data and open temporary Midi Devices
    */

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



/*!
 * WizardInstrumentsPage type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WizardInstrumentsPage, wxWizardPageSimple )

/*!
 * WizardInstrumentsPage event table definition
 */

BEGIN_EVENT_TABLE( WizardInstrumentsPage, wxWizardPageSimple )

////@begin WizardInstrumentsPage event table entries
    EVT_COMBOBOX( ID_COMBO_SECTION, WizardInstrumentsPage::OnComboSection )

    EVT_COMBOBOX( ID_COMBO_INSTRUMENT, WizardInstrumentsPage::OnComboInstrument )

    EVT_BUTTON( ID_BUTTON_TEST_SOUND, WizardInstrumentsPage::OnButtonTestSoundClick )

////@end WizardInstrumentsPage event table entries

END_EVENT_TABLE()

/*!
 * WizardInstrumentsPage constructors
 */

WizardInstrumentsPage::WizardInstrumentsPage( )
{
}

WizardInstrumentsPage::WizardInstrumentsPage( wxWizard* parent )
{
    Create( parent );
}

/*!
 * WizardPage creator
 */

bool WizardInstrumentsPage::Create( wxWizard* parent )
{
////@begin WizardInstrumentsPage member initialisation
    m_pVoiceChannelCombo = NULL;
    m_pSectCombo = NULL;
    m_pInstrCombo = NULL;
////@end WizardInstrumentsPage member initialisation

////@begin WizardInstrumentsPage creation
    wxBitmap wizardBitmap(wxNullBitmap);
    wxWizardPageSimple::Create( parent, NULL, NULL, wizardBitmap );

    CreateControls();
    GetSizer()->Fit(this);
////@end WizardInstrumentsPage creation

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
    int nSect = pMidiGM->PopulateWithSections(m_pSectCombo, nInstr );
    pMidiGM->PopulateWithInstruments(m_pInstrCombo, nSect, nInstr);

    return TRUE;
}

/*!
 * Control creation for WizardPage
 */

void WizardInstrumentsPage::CreateControls()
{    
////@begin WizardInstrumentsPage content construction
    WizardInstrumentsPage* itemWizardPageSimple16 = this;

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxVERTICAL);
    itemWizardPageSimple16->SetSizer(itemBoxSizer17);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer17->Add(itemBoxSizer18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText19 = new wxStaticText( itemWizardPageSimple16, wxID_STATIC, _("Voice channel and instrument"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText19->SetFont(wxFont(14, wxSWISS, wxNORMAL, wxBOLD, FALSE, _T("Arial")));
    itemBoxSizer18->Add(itemStaticText19, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer17->Add(itemBoxSizer20, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer20->Add(itemBoxSizer21, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText22 = new wxStaticText( itemWizardPageSimple16, wxID_STATIC, _("Channels 10 and 16 are specialized in percussion sounds. So it is recommended to choose any other channel (it doesn't matter wich one)."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer21->Add(itemStaticText22, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText23 = new wxStaticText( itemWizardPageSimple16, wxID_STATIC, _("To facilitate access to the instruments they are grouped into sections. First choose a section and then choose the desired instrument."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer21->Add(itemStaticText23, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticLine* itemStaticLine24 = new wxStaticLine( itemWizardPageSimple16, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer20->Add(itemStaticLine24, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer20->Add(itemBoxSizer25, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText26 = new wxStaticText( itemWizardPageSimple16, wxID_STATIC, _("Channel:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText26, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pVoiceChannelComboStrings = NULL;
    m_pVoiceChannelCombo = new wxComboBox( itemWizardPageSimple16, ID_COMBO_CHANNEL, _T(""), wxDefaultPosition, wxSize(70, -1), 0, m_pVoiceChannelComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pVoiceChannelCombo, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer25->Add(itemBoxSizer28, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText29 = new wxStaticText( itemWizardPageSimple16, wxID_STATIC, _("Section:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText29, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pSectComboStrings = NULL;
    m_pSectCombo = new wxComboBox( itemWizardPageSimple16, ID_COMBO_SECTION, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pSectComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pSectCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText31 = new wxStaticText( itemWizardPageSimple16, wxID_STATIC, _("Instrument:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText31, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pInstrComboStrings = NULL;
    m_pInstrCombo = new wxComboBox( itemWizardPageSimple16, ID_COMBO_INSTRUMENT, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pInstrComboStrings, wxCB_READONLY );
    itemBoxSizer25->Add(m_pInstrCombo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxButton* itemButton33 = new wxButton( itemWizardPageSimple16, ID_BUTTON_TEST_SOUND, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemButton33, 0, wxALIGN_LEFT|wxALL, 5);

////@end WizardInstrumentsPage content construction
}

/*!
 * Should we show tooltips?
 */

bool WizardInstrumentsPage::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap WizardInstrumentsPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin WizardInstrumentsPage bitmap retrieval
    return wxNullBitmap;
////@end WizardInstrumentsPage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WizardInstrumentsPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin WizardInstrumentsPage icon retrieval
    return wxNullIcon;
////@end WizardInstrumentsPage icon retrieval
}

bool WizardInstrumentsPage::TransferDataFromWindow()
{
    /*
    Save temporary data and set temporary Midi program
    */

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


/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBO_SECTION
 */

void WizardInstrumentsPage::OnComboSection( wxCommandEvent& event )
{
    // A new section selected. Reload Instruments combo with the instruments in the
    //selected section

    wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
    int nSect = m_pSectCombo->GetSelection();
    pMidiGM->PopulateWithInstruments(m_pInstrCombo, nSect);
    DoProgramChange();

}


/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBO_INSTRUMENT
 */

void WizardInstrumentsPage::OnComboInstrument( wxCommandEvent& event )
{
    // A new instrument selected. Change Midi program
    DoProgramChange();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TEST_SOUND
 */

void WizardInstrumentsPage::OnButtonTestSoundClick( wxCommandEvent& event )
{
    //play a scale
    g_pMidi->TestOut();
}


/*!
 * WizardMetronomePage type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WizardMetronomePage, wxWizardPageSimple )

/*!
 * WizardMetronomePage event table definition
 */

BEGIN_EVENT_TABLE( WizardMetronomePage, wxWizardPageSimple )

////@begin WizardMetronomePage event table entries
    EVT_COMBOBOX( ID_COMBO_MTR_INSTR1, WizardMetronomePage::OnComboMtrInstr1Selected )

    EVT_COMBOBOX( ID_COMBO_MTR_INSTR2, WizardMetronomePage::OnComboMtrInstr2Selected )

    EVT_BUTTON( ID_BUTTON, WizardMetronomePage::OnButtonClick )

////@end WizardMetronomePage event table entries

END_EVENT_TABLE()

/*!
 * WizardMetronomePage constructors
 */

WizardMetronomePage::WizardMetronomePage( )
{
}

WizardMetronomePage::WizardMetronomePage( wxWizard* parent )
{
    Create( parent );
}

/*!
 * WizardMetronomePage creator
 */

bool WizardMetronomePage::Create( wxWizard* parent )
{
////@begin WizardMetronomePage member initialisation
    m_pMtrChannelCombo = NULL;
    m_pMtrInstr1Combo = NULL;
    m_pMtrInstr2Combo = NULL;
////@end WizardMetronomePage member initialisation

////@begin WizardMetronomePage creation
    wxBitmap wizardBitmap(wxNullBitmap);
    wxWizardPageSimple::Create( parent, NULL, NULL, wizardBitmap );

    CreateControls();
    GetSizer()->Fit(this);
////@end WizardMetronomePage creation

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
    pMidiGM->PopulateWithPercusionInstr(m_pMtrInstr1Combo, nTone1);
    pMidiGM->PopulateWithPercusionInstr(m_pMtrInstr2Combo, nTone2);

    return TRUE;
}

/*!
 * Control creation for WizardMetronomePage
 */

void WizardMetronomePage::CreateControls()
{    
////@begin WizardMetronomePage content construction
    WizardMetronomePage* itemWizardPageSimple34 = this;

    wxBoxSizer* itemBoxSizer35 = new wxBoxSizer(wxVERTICAL);
    itemWizardPageSimple34->SetSizer(itemBoxSizer35);

    wxBoxSizer* itemBoxSizer36 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer35->Add(itemBoxSizer36, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText37 = new wxStaticText( itemWizardPageSimple34, wxID_STATIC, _("lmMetronome channel and sounds"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText37->SetFont(wxFont(14, wxSWISS, wxNORMAL, wxBOLD, FALSE, _T("Arial")));
    itemBoxSizer36->Add(itemStaticText37, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer38 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer35->Add(itemBoxSizer38, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer39 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer38->Add(itemBoxSizer39, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText40 = new wxStaticText( itemWizardPageSimple34, wxID_STATIC, _("Channels 10 and 16 are specialized in percussion sounds. So it is recommended to choose one of these (it doesn't matter wich one)."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer39->Add(itemStaticText40, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText41 = new wxStaticText( itemWizardPageSimple34, wxID_STATIC, _("To better identify the first beat of each measure it is possible to assign a different sound to it. But you can also choose the same sound for both, the first beat and the others."), wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizer39->Add(itemStaticText41, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticLine* itemStaticLine42 = new wxStaticLine( itemWizardPageSimple34, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer38->Add(itemStaticLine42, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer43 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer38->Add(itemBoxSizer43, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText44 = new wxStaticText( itemWizardPageSimple34, wxID_STATIC, _("Channel:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText44, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pMtrChannelComboStrings = NULL;
    m_pMtrChannelCombo = new wxComboBox( itemWizardPageSimple34, ID_COMBO_MTR_CHANNEL, _T(""), wxDefaultPosition, wxSize(70, -1), 0, m_pMtrChannelComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrChannelCombo, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer46 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer43->Add(itemBoxSizer46, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText47 = new wxStaticText( itemWizardPageSimple34, wxID_STATIC, _("Sound for first beat of each measure:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText47, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pMtrInstr1ComboStrings = NULL;
    m_pMtrInstr1Combo = new wxComboBox( itemWizardPageSimple34, ID_COMBO_MTR_INSTR1, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pMtrInstr1ComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrInstr1Combo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText49 = new wxStaticText( itemWizardPageSimple34, wxID_STATIC, _("Sound for other beats of each measure:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemStaticText49, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_pMtrInstr2ComboStrings = NULL;
    m_pMtrInstr2Combo = new wxComboBox( itemWizardPageSimple34, ID_COMBO_MTR_INSTR2, _T(""), wxDefaultPosition, wxSize(250, -1), 0, m_pMtrInstr2ComboStrings, wxCB_READONLY );
    itemBoxSizer43->Add(m_pMtrInstr2Combo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxButton* itemButton51 = new wxButton( itemWizardPageSimple34, ID_BUTTON, _("Test sound"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(itemButton51, 0, wxALIGN_LEFT|wxALL, 5);

////@end WizardMetronomePage content construction
}

/*!
 * Should we show tooltips?
 */

bool WizardMetronomePage::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap WizardMetronomePage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin WizardMetronomePage bitmap retrieval
    return wxNullBitmap;
////@end WizardMetronomePage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WizardMetronomePage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin WizardMetronomePage icon retrieval
    return wxNullIcon;
////@end WizardMetronomePage icon retrieval
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBO_MTR_INSTR1
 */

void WizardMetronomePage::OnComboMtrInstr1Selected( wxCommandEvent& event )
{
    //Change metronome sound, tone1, to the one selected in combo Instr1
    int nTone1 = m_pMtrInstr1Combo->GetSelection() + 35;
    g_pMidi->SetMetronomeTones(nTone1, g_pMidi->MtrTone2());

}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBO_MTR_INSTR2
 */

void WizardMetronomePage::OnComboMtrInstr2Selected( wxCommandEvent& event )
{
    //Change metronome sound, tone2, to the one selected in combo Instr2
    int nTone2 = m_pMtrInstr2Combo->GetSelection() + 35;
    g_pMidi->SetMetronomeTones(g_pMidi->MtrTone1(), nTone2);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

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


