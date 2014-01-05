//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

#ifndef __LENMUS_MIDI_WIZARD_H__
#define __LENMUS_MIDI_WIZARD_H__

//lenmus
#include "lenmus_standard_header.h"

//wxWidgets
#include "wx/wxprec.h"
#include <wx/wizard.h>
#include <wx/statline.h>
#include <wx/combobox.h>

namespace lenmus
{

//forward declarations
class WizardDevicesPage;
class WizardInstrumentsPage;
class WizardMetronomePage;
class ApplicationScope;

//Control identifiers
#define ID_WIZARD 10000
#define SYMBOL_MIDIWIZARD_IDNAME ID_WIZARD
#define ID_WIZARD_DEVICES_PAGE 10001
#define ID_COMBO_OUT_DEVICES 10004
#define ID_COMBO_IN_DEVICES 10005
#define ID_WIZARD_INSTRUMENTS_PAGE 10002
#define ID_COMBO_CHANNEL 10008
#define ID_COMBO_SECTION 10006
#define ID_COMBO_INSTRUMENT 10007
#define ID_BUTTON_TEST_SOUND 10009
#define ID_WIZARD_METRONOME_PAGE 10003
#define ID_COMBO_MTR_CHANNEL 10010
#define ID_COMBO_MTR_INSTR1 10011
#define ID_COMBO_MTR_INSTR2 10012
#define ID_BUTTON 10013

//Compatibility
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif


//---------------------------------------------------------------------------------------
class MidiWizard: public wxWizard
{
    //DECLARE_DYNAMIC_CLASS( MidiWizard )
    DECLARE_EVENT_TABLE()

protected:
    ApplicationScope& m_appScope;

    //old Midi configuration
    int        m_nOldInDevId;
    int        m_nOldOutDevId;
    int        m_nOldVoiceInstr;
    int        m_nOldVoiceChannel;
    int        m_nOldMtrInstr;
    int        m_nOldMtrChannel;
    int        m_nOldMtrTone1;
    int        m_nOldMtrTone2;

public:
    //MidiWizard();
    MidiWizard(ApplicationScope& appScope, wxWindow* parent,
               wxWindowID id = SYMBOL_MIDIWIZARD_IDNAME,
               const wxPoint& pos = wxDefaultPosition);

    bool Create(wxWindow* parent, wxWindowID id = SYMBOL_MIDIWIZARD_IDNAME,
                const wxPoint& pos = wxDefaultPosition);
    void CreateControls();
    bool Run();

    void OnWizardCancel(wxWizardEvent& event);
    void OnWizardFinished(wxWizardEvent& event);

    wxBitmap GetBitmapResource(const wxString& name);
    wxIcon GetIconResource(const wxString& name);
    static bool ShowToolTips();
    WizardMetronomePage* m_pMtrChannelCombo;

};

//---------------------------------------------------------------------------------------
class WizardDevicesPage: public wxWizardPageSimple
{
    //DECLARE_DYNAMIC_CLASS( WizardDevicesPage )
    DECLARE_EVENT_TABLE()

protected:
    ApplicationScope& m_appScope;

public:
    //WizardDevicesPage( );
    WizardDevicesPage(ApplicationScope& appScope, wxWizard* parent);

    bool Create( wxWizard* parent );
    void CreateControls();

    wxBitmap GetBitmapResource( const wxString& name );
    wxIcon GetIconResource( const wxString& name );
    static bool ShowToolTips();
    bool TransferDataFromWindow();

    wxComboBox* m_pOutCombo;
    wxComboBox* m_pInCombo;
};

//---------------------------------------------------------------------------------------
class WizardInstrumentsPage: public wxWizardPageSimple
{
    //DECLARE_DYNAMIC_CLASS( WizardInstrumentsPage )
    DECLARE_EVENT_TABLE()

protected:
    ApplicationScope& m_appScope;

public:
    //WizardInstrumentsPage( );
    WizardInstrumentsPage(ApplicationScope& appScope, wxWizard* parent);

    bool Create( wxWizard* parent );
    void CreateControls();

    void OnComboSection( wxCommandEvent& event );
    void OnComboInstrument( wxCommandEvent& event );
    void OnButtonTestSoundClick( wxCommandEvent& event );

    wxBitmap GetBitmapResource( const wxString& name );
    wxIcon GetIconResource( const wxString& name );

    static bool ShowToolTips();
    bool TransferDataFromWindow();
    void DoProgramChange();

    wxComboBox* m_pVoiceChannelCombo;
    wxComboBox* m_pSectCombo;
    wxComboBox* m_pInstrCombo;
};

//---------------------------------------------------------------------------------------
class WizardMetronomePage: public wxWizardPageSimple
{
    //DECLARE_DYNAMIC_CLASS( WizardMetronomePage )
    DECLARE_EVENT_TABLE()

protected:
    ApplicationScope& m_appScope;

public:
    //WizardMetronomePage( );
    WizardMetronomePage(ApplicationScope& appScope, wxWizard* parent);

    bool Create( wxWizard* parent );
    void CreateControls();

    void OnComboMtrInstr1Selected( wxCommandEvent& event );
    void OnComboMtrInstr2Selected( wxCommandEvent& event );
    void OnButtonClick( wxCommandEvent& event );

    wxBitmap GetBitmapResource( const wxString& name );
    wxIcon GetIconResource( const wxString& name );
    static bool ShowToolTips();

    wxComboBox* m_pMtrChannelCombo;
    wxComboBox* m_pMtrInstr1Combo;
    wxComboBox* m_pMtrInstr2Combo;
};


}   //namespace lenmus

#endif    // __LENMUS_MIDI_WIZARD_H__
