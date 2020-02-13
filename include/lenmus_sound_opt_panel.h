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

#ifndef __LENMUS_SOUND_OPT_PANEL_H__
#define __LENMUS_SOUND_OPT_PANEL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_options_panel.h"

//wxWidgets
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/radiobox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/sizer.h>


namespace lenmus
{

//forward declarations
class OptExternalSynthPanel;
class OptInstrumentsPanel;
class OptMetronomePanel;
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


//forward declarations
class OptExternalSynthPanel;
class OptInternalSynthPanel;
class OptInstrumentsPanel;
class OptMetronomePanel;


//---------------------------------------------------------------------------------------
class SoundOptionsPanel : public OptionsPanel
{
public:
    SoundOptionsPanel(wxWindow* parent, ApplicationScope& appScope);
    ~SoundOptionsPanel();
    bool Verify();
    void Apply();

private:
    void create_controls();
    void initialize_controls();

    //event handlers
	void on_new_synthesizer(wxCommandEvent& event);
    void on_test_sound(wxCommandEvent& event);
    void on_reset_to_defaults(wxCommandEvent& event);

    enum
    {
        id_new_synthesizer = 1000,
        id_test_sound,
        id_reset_to_defaults
    };

    // controls
	wxPanel*        m_pHeaderPanel;
	wxStaticText*   m_pTxtTitle;
	wxStaticBitmap* m_pBmpIconTitle;
    wxRadioBox*     m_pRadSynthesizer;
    wxButton*       m_pTestSoundButton;
    wxButton*       m_pResetDefaultsButton;
    wxNotebook*     m_pTabs;

    OptExternalSynthPanel* m_pPanelIntSynth;
    OptInternalSynthPanel* m_pPanelExtSynth;
    OptInstrumentsPanel* m_pPanelInstrument;
    OptMetronomePanel* m_pPanelMetronome;


	//other

    //old Midi configuration
    int        m_nOldInDevId;
    int        m_nOldOutDevId;
    int        m_nOldVoiceInstr;
    int        m_nOldVoiceChannel;
    int        m_nOldMtrInstr;
    int        m_nOldMtrChannel;
    int        m_nOldMtrTone1;
    int        m_nOldMtrTone2;
};

//---------------------------------------------------------------------------------------
class OptExternalSynthPanel: public wxPanel
{
protected:
    ApplicationScope& m_appScope;
    wxComboBox* m_pOutCombo;
    wxComboBox* m_pInCombo;

public:
    OptExternalSynthPanel(ApplicationScope& appScope, wxWindow* parent);
    ~OptExternalSynthPanel() {}

    void initialize_controls();

protected:
    void create_controls();

    //event handlers
    void on_out_device_selected(wxCommandEvent& event);

};

//---------------------------------------------------------------------------------------
class OptInternalSynthPanel: public wxPanel
{
protected:
    ApplicationScope& m_appScope;

public:
    OptInternalSynthPanel(ApplicationScope& appScope, wxWindow* parent);
    ~OptInternalSynthPanel() {}

    void initialize_controls();

protected:
    void create_controls();

//    //event handlers
//    void on_combo_channel(wxCommandEvent& event);
//    void OnComboMtrInstr1Selected( wxCommandEvent& event );
//    void OnComboMtrInstr2Selected( wxCommandEvent& event );
//    void on_test_sound(wxCommandEvent& event);
//
//    wxComboBox* m_pMtrChannelCombo;
//    wxComboBox* m_pMtrInstr1Combo;
//    wxComboBox* m_pMtrInstr2Combo;
};

//---------------------------------------------------------------------------------------
class OptInstrumentsPanel: public wxPanel
{
protected:
    ApplicationScope& m_appScope;

public:
    OptInstrumentsPanel(ApplicationScope& appScope, wxWindow* parent);
    ~OptInstrumentsPanel() {}

    void initialize_controls();

protected:
    void create_controls();

    //event handlers
    void on_combo_channel(wxCommandEvent& event);
    void on_combo_section(wxCommandEvent& event);
    void on_combo_instrument(wxCommandEvent& event);

    void DoProgramChange();

    wxComboBox* m_pVoiceChannelCombo;
    wxComboBox* m_pSectCombo;
    wxComboBox* m_pInstrCombo;
};

//---------------------------------------------------------------------------------------
class OptMetronomePanel: public wxPanel
{
protected:
    ApplicationScope& m_appScope;

public:
    OptMetronomePanel(ApplicationScope& appScope, wxWindow* parent);
    ~OptMetronomePanel() {}

    void initialize_controls();

protected:
    void create_controls();

    //event handlers
    void on_combo_channel(wxCommandEvent& event);
    void OnComboMtrInstr1Selected( wxCommandEvent& event );
    void OnComboMtrInstr2Selected( wxCommandEvent& event );
    void on_test_sound(wxCommandEvent& event);

    wxComboBox* m_pMtrChannelCombo;
    wxComboBox* m_pMtrInstr1Combo;
    wxComboBox* m_pMtrInstr2Combo;
};


}   //namespace lenmus

#endif    // __LENMUS_SOUND_OPT_PANEL_H__
