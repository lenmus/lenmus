//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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
#include "lenmus_dlg_metronome.h"
#include "lenmus_utilities.h"
#include "lenmus_metronome.h"
#include "lenmus_art_provider.h"
#include "lenmus_main_frame.h"


//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/radiobox.h>

#include <wx/xrc/xmlres.h>
#include <wx/hashmap.h>

#include <lomse_document.h>         //EBeatDuration
#include <lomse_internal_model.h>   //ENoteDuration
#include <lomse_time.h>             //is_equal_time()
using namespace lomse;

namespace lenmus
{

static const long k_id_button = wxNewId();
static const long k_id_button_increment = wxNewId();
static const long k_id_button_decrement = wxNewId();
static const long k_id_button_tap_tempo = wxNewId();
static const long k_id_button_start = wxNewId();
static const long k_id_button_reset = wxNewId();

static const long k_id_choice_italian_tempo = wxNewId();
static const long k_id_metronome_number = wxNewId();
static const long k_id_tempo_slider = wxNewId();
static const long k_id_beat_specified = wxNewId();
static const long k_id_beat_implied = wxNewId();
static const long k_id_metronome_note = wxNewId();

static const int k_max_bpm = 400;
static const int k_min_bpm = 1;

// an entry for the table of italian tempo terms
struct ItalianTempo
{
    wxString name;
    int minTempo;
    int maxTempo;
    int defaultTempo;
};


// the table.
static const ItalianTempo m_tempi[] = {
    {"Larghissimo",  1, 19, 15},
    {"Grave",        20, 39, 30},
    {"Lento",        40, 44, 42},
    {"Largo",        45, 49, 47},
    {"Larghetto",    50, 54, 52},
    {"Adagio",       55, 64, 60},
    {"Adagietto",    65, 68, 66},
    {"Andante moderato", 69, 72, 70},
    {"Andante",      73, 77, 75},
    {"Andantino",    78, 85, 82},
    {"Moderato",     86, 97, 93},
    {"Allegretto",   98, 109, 105},
    {"Allegro",      110, 132, 120},
    {"Vivace",       133, 139, 136},
    {"Vivacissimo",  140, 149, 145},
    {"Allegrissimo", 150, 167, 160},
    {"Presto",       168, 178, 174},
    {"Prestissimo",  178, 999, 200}
};

static int numTempi = sizeof(m_tempi) / sizeof(ItalianTempo);


//---------------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(DlgMetronome, wxDialog)
    EVT_COMMAND_RANGE (k_id_button, k_id_button_reset,
                       wxEVT_COMMAND_BUTTON_CLICKED, DlgMetronome::on_button)
	EVT_CHAR_HOOK(DlgMetronome::on_key_down)
	EVT_CHOICE(k_id_choice_italian_tempo, DlgMetronome::on_tempo_choice)
    EVT_TEXT(k_id_metronome_number, DlgMetronome::on_update_number)
    EVT_SLIDER(k_id_tempo_slider, DlgMetronome::on_tempo_slider)
    EVT_RADIOBUTTON(k_id_beat_specified, DlgMetronome::on_beat_type)
    EVT_RADIOBUTTON(k_id_beat_implied, DlgMetronome::on_beat_type)
    EVT_COMBOBOX(k_id_metronome_note, DlgMetronome::on_beat_type)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
DlgMetronome::DlgMetronome(ApplicationScope& appScope, wxWindow* parent,
                           GlobalMetronome* pMtr)
    : wxDialog(parent, wxID_ANY, _("Metronome settings"), wxDefaultPosition,
               wxSize(-1,500), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    , m_appScope(appScope)
    , m_pMtr(pMtr)
    , m_tempo(pMtr->get_mm())
    , m_prevTempo(m_tempo)
    , m_totalTime(0.0)
    , m_count(0)
    , m_min(0.0)
    , m_max(0.0)
{
    if (m_pMtr->is_running())
        m_pMtr->stop();

    load_italian_tempi();
    create_dialog();
    load_beat_notes();
    load_current_values();
    display_tempo();
    update_count_max_min();
    SetFocus();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::create_dialog()
{
    //content generated with wxFormBuilder
    //Manual changes:
    // - delete line:	wxArrayString m_pItalianTempoChoices;
    // - replace m_pItalianTempoChoices -> m_italianTempoChoices (1 replz.)
    // - delete line: 	wxArrayString m_beatNoteChoiceChoices;
    // - replace:
    //	    m_beatNoteChoice = new wxBitmapComboBox( this, k_id_metronome_note,
    //                            wxDefaultPosition, wxDefaultSize, m_beatNoteChoices,
    //                            0 );
    //   by
    //	    m_beatNoteChoice = new wxBitmapComboBox( this, k_id_metronome_note,
    //                         wxEmptyString, wxDefaultPosition, wxSize(60, -1),
    //                         0, nullptr, wxCB_READONLY);
    // - in header, replace wxChoice -> wxBitmapComboBox (1 replz.)

	this->SetSizeHints( wxSize( -1,500 ), wxSize( -1,500 ) );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );


	bSizer1->Add( 0, 0, 0, 0, 5 );

	m_lblTempo = new wxStaticText( this, wxID_ANY, _("Tempo:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblTempo->Wrap( -1 );
	bSizer1->Add( m_lblTempo, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_pTempoDisplay = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_pTempoDisplay, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );


	bSizer1->Add( 0, 0, 0, wxRIGHT|wxLEFT, 10 );

	m_pItalianTempo = new wxChoice( this, k_id_choice_italian_tempo, wxDefaultPosition, wxDefaultSize, m_italianTempoChoices, 0 );
//	m_pItalianTempo->SetSelection( 0 );
	bSizer1->Add( m_pItalianTempo, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer1->Add( 0, 0, 0, wxEXPAND, 5 );

	bSizer17->Add( bSizer1, 0, wxTOP|wxRIGHT|wxLEFT|wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* pTempoSliderSizer;
	pTempoSliderSizer = new wxBoxSizer( wxHORIZONTAL );


	pTempoSliderSizer->Add( 0, 0, 1, 0, 5 );

	m_pTempoSlider = new wxSlider( this, k_id_tempo_slider, 70, 1, 400, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	pTempoSliderSizer->Add( m_pTempoSlider, 5, wxALL, 5 );


	pTempoSliderSizer->Add( 0, 0, 1, 0, 5 );

	bSizer17->Add( pTempoSliderSizer, 0, wxTOP|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );


	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pIncrementButton = new wxButton( this, k_id_button_increment, "+", wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_pIncrementButton, 0, wxALL, 5 );

	m_pDecrementButton = new wxButton( this, k_id_button_decrement, "-", wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_pDecrementButton, 0, wxALL, 5 );


	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );

	bSizer17->Add( bSizer2, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	pMainSizer->Add( bSizer17, 0, wxTOP|wxEXPAND, 5 );

	m_pTapButton = new wxButton( this, k_id_button_tap_tempo, _("or tap tempo with this button or with space bar"), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add( m_pTapButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxHORIZONTAL );


	bSizer111->Add( 0, 0, 1, wxEXPAND, 5 );

	m_lblCount = new wxStaticText( this, wxID_ANY, _("Count:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblCount->Wrap( -1 );
	bSizer111->Add( m_lblCount, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_pTxtCount = new wxStaticText( this, wxID_ANY, "12", wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtCount->Wrap( -1 );
	bSizer111->Add( m_pTxtCount, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );


	bSizer111->Add( 0, 0, 1, wxEXPAND, 5 );

	bSizer5->Add( bSizer111, 0, wxEXPAND, 5 );

	m_pResetButton = new wxButton( this, k_id_button_reset, _("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_pResetButton, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_HORIZONTAL, 5 );

	bSizer15->Add( bSizer5, 0, wxEXPAND, 5 );

	bSizer4->Add( bSizer15, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );


	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );

	m_lblMax = new wxStaticText( this, wxID_ANY, _("Max:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblMax->Wrap( -1 );
	bSizer7->Add( m_lblMax, 0, wxRIGHT|wxLEFT, 5 );

	m_pTxtMax = new wxStaticText( this, wxID_ANY, "207 ms (50 BPM)", wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtMax->Wrap( -1 );
	bSizer7->Add( m_pTxtMax, 0, wxRIGHT|wxLEFT, 5 );

	bSizer6->Add( bSizer7, 0, wxTOP, 5 );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );

	m_lblMin = new wxStaticText( this, wxID_ANY, _("Min:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblMin->Wrap( -1 );
	bSizer8->Add( m_lblMin, 0, wxRIGHT|wxLEFT, 5 );

	m_pTxtMin = new wxStaticText( this, wxID_ANY, "203 ms (51 BPM)", wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtMin->Wrap( -1 );
	bSizer8->Add( m_pTxtMin, 0, wxRIGHT|wxLEFT, 5 );

	bSizer6->Add( bSizer8, 0, wxEXPAND, 5 );


	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );

	bSizer4->Add( bSizer6, 1, wxEXPAND|wxLEFT, 5 );

	pMainSizer->Add( bSizer4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Metronome beat is:") ), wxVERTICAL );

	wxBoxSizer* bSizer161;
	bSizer161 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer1511;
	bSizer1511 = new wxBoxSizer( wxVERTICAL );

	m_beatSelected = new wxRadioButton( this, k_id_beat_specified, _("the note value selected at right"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	bSizer1511->Add( m_beatSelected, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_beatImplied = new wxRadioButton( this, k_id_beat_implied, _("implied by time signature"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1511->Add( m_beatImplied, 0, wxRIGHT|wxLEFT, 5 );

	bSizer161->Add( bSizer1511, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );

    m_beatNoteChoice = new wxBitmapComboBox( this, k_id_metronome_note,
                             wxEmptyString, wxDefaultPosition, wxSize(60, -1),
                             0, nullptr, wxCB_READONLY);
//	m_beatNoteChoice->SetSelection( 0 );
	bSizer161->Add( m_beatNoteChoice, 0, wxALL, 5 );

	sbSizer1->Add( bSizer161, 0, 0, 5 );

	pMainSizer->Add( sbSizer1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );


	bSizer11->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pStartButton = new wxButton( this, k_id_button_start, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_pStartButton, 0, wxALL, 5 );


	bSizer11->Add( 0, 0, 1, wxEXPAND, 5 );

	pMainSizer->Add( bSizer11, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
	pMainSizer->Fit( this );
}

//---------------------------------------------------------------------------------------
void DlgMetronome::load_italian_tempi()
{
    int i=0;
    for (; i < numTempi-1; ++i)
    {
        m_italianTempoChoices.Add( wxString::Format("%s (%d-%d)",
                                                    m_tempi[i].name.wx_str(),
                                                    m_tempi[i].minTempo,
                                                    m_tempi[i].maxTempo) );
    }
    m_italianTempoChoices.Add( wxString::Format("%s (> %d)",
                                                m_tempi[i].name.wx_str(),
                                                m_tempi[i].minTempo) );
}

//---------------------------------------------------------------------------------------
void DlgMetronome::load_beat_notes()
{
    m_beatNoteChoice->Clear();
    m_beatNoteChoice->Append(wxEmptyString,
                             wxArtProvider::GetBitmap("beat_whole",
                                 wxART_TOOLBAR, wxSize(32,32)) );
    m_beatNoteChoice->Append(wxEmptyString,
                             wxArtProvider::GetBitmap("beat_half_dotted",
                                 wxART_TOOLBAR, wxSize(32,32)) );
    m_beatNoteChoice->Append(wxEmptyString,
                             wxArtProvider::GetBitmap("beat_half",
                                 wxART_TOOLBAR, wxSize(32,32)) );
    m_beatNoteChoice->Append(wxEmptyString,
                             wxArtProvider::GetBitmap("beat_quarter_dotted",
                                 wxART_TOOLBAR, wxSize(32,32)) );
    m_beatNoteChoice->Append(wxEmptyString,
                             wxArtProvider::GetBitmap("beat_quarter",
                                 wxART_TOOLBAR, wxSize(32,32)) );
    m_beatNoteChoice->Append(wxEmptyString,
                             wxArtProvider::GetBitmap("beat_eighth_dotted",
                                 wxART_TOOLBAR, wxSize(32,32)) );
    m_beatNoteChoice->Append(wxEmptyString,
                             wxArtProvider::GetBitmap("beat_eighth",
                                 wxART_TOOLBAR, wxSize(32,32)) );
    m_beatNoteChoice->Append(wxEmptyString,
                             wxArtProvider::GetBitmap("beat_sexteenth",
                                 wxART_TOOLBAR, wxSize(32,32)) );

    m_beatNoteChoice->SetSelection(4);
}

//---------------------------------------------------------------------------------------
void DlgMetronome::on_button(wxCommandEvent& event)
{
    int button = event.GetId();
    if (button == k_id_button_increment)
    {
        increment_tempo();
    }
    else if (button == k_id_button_decrement)
    {
        decrement_tempo();
    }
    else if (button == k_id_button_tap_tempo)
    {
        compute_tapped_tempo();
    }
    else if (button == k_id_button_reset)
    {
        reset_tapped_tempo();
    }
    else if (button == k_id_button_start)
    {
        if (m_pMtr->is_running())
        {
            m_pMtr->stop();
            m_pStartButton->SetLabel(_("Start"));
        }
        else
        {
            m_pMtr->start();
            m_pStartButton->SetLabel(_("Stop"));
        }
    }
    return;
}

//---------------------------------------------------------------------------------------
void DlgMetronome::compute_tapped_tempo()
{
    using namespace std::chrono;

    high_resolution_clock::time_point now = high_resolution_clock::now();
    duration<double> diff = duration_cast<duration<double>>(now - m_prevTime);
    double period = duration_cast<milliseconds>(diff).count();  //in millisecs
    m_prevTime = now;

    if (m_count == 0 || period > 7000.0)
    {
        reset_tapped_tempo();
    }
    else
    {
        if (m_count == 1)
        {
            m_max = period;
            m_min = period;
        }
        m_totalTime += period;
        m_max = max(m_max, period);
        m_min = min(m_min, period);
        if (m_count > 3)
            period = (m_totalTime - m_max - m_min) / (m_count - 2);
        else
            period = m_totalTime / m_count;
        set_tempo(60000.0 / period);
    }
    m_count++;
    update_count_max_min();
    display_tempo();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::reset_tapped_tempo()
{
    m_count = 0;
    m_min = 0.0;
    m_max = 0.0;
    m_totalTime = 0.0;
    update_count_max_min();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::increment_tempo()
{
    m_tempo++;
    if (m_tempo > k_max_bpm)
        m_tempo = k_max_bpm;
    set_tempo(m_tempo);
    display_tempo();
    reset_tapped_tempo();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::decrement_tempo()
{
    m_tempo--;
    if (m_tempo < k_min_bpm)
        m_tempo = k_min_bpm;
    set_tempo(m_tempo);
    display_tempo();
    reset_tapped_tempo();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::display_tempo()
{
    m_pTempoDisplay->ChangeValue( wxString::Format("%d", m_tempo) );
    m_pTempoDisplay->SetInsertionPointEnd();
    m_prevTempo = m_tempo;
    m_pTempoSlider->SetValue(m_tempo);

    for (int i=0; i < numTempi; i++)
    {
        if (m_tempo >= m_tempi[i].minTempo && m_tempo <= m_tempi[i].maxTempo)
        {
            m_pItalianTempo->SetSelection(i);
            return;
        }
    }
    m_pItalianTempo->SetSelection(8);
}

//---------------------------------------------------------------------------------------
void DlgMetronome::on_tempo_choice(wxCommandEvent& WXUNUSED(event))
{
    int i = m_pItalianTempo->GetSelection();
    set_tempo( m_tempi[i].defaultTempo );
    display_tempo();
    reset_tapped_tempo();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::on_update_number(wxCommandEvent& WXUNUSED(event))
{
    wxString value = m_pTempoDisplay->GetValue();
    if (value.empty())
    {
        return;
    }

    long num = 0L;
    if (value.ToLong(&num))
    {
        m_tempo = num;
        if (m_tempo < k_min_bpm)
            m_tempo = k_min_bpm;
        else if (m_tempo > k_max_bpm)
            m_tempo = k_max_bpm;

    }
    else
    {
        m_tempo = m_prevTempo;
    }
    set_tempo(m_tempo);
    display_tempo();
        reset_tapped_tempo();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::update_count_max_min()
{
    wxString sCount;
    sCount << m_count;
    m_pTxtCount->SetLabel(sCount);

    wxString sMax;
    if (m_max > 0.0)
        sMax << m_max << " ms (" << int(60000.0 / m_max) << " BPM)";
    else
        sMax = " ";
    m_pTxtMax->SetLabel(sMax);

    wxString sMin;
    if (m_min > 0.0)
        sMin << m_min << " ms (" << int(60000.0 / m_min) << " BPM)";
    else
        sMin = " ";
    m_pTxtMin->SetLabel(sMin);
}

//---------------------------------------------------------------------------------------
void DlgMetronome::on_tempo_slider(wxCommandEvent& WXUNUSED(event))
{
    set_tempo( m_pTempoSlider->GetValue() );
    display_tempo();
    reset_tapped_tempo();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::on_key_down(wxKeyEvent& event)
{
    int ch = event.GetKeyCode();
    if (ch == WXK_SPACE)
        compute_tapped_tempo();
    else if (ch == WXK_NUMPAD_ADD || ch == '+')
        increment_tempo();
    else if (ch == WXK_NUMPAD_SUBTRACT || ch == '-')
        decrement_tempo();
    else if (m_pTempoDisplay->HasFocus())
        event.Skip(true);
}

//---------------------------------------------------------------------------------------
void DlgMetronome::on_beat_type(wxCommandEvent& WXUNUSED(event))
{
    set_beat_options();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::set_tempo(int nMM)
{
    m_tempo = nMM;
    m_pMtr->set_mm(nMM);
}

//---------------------------------------------------------------------------------------
void DlgMetronome::set_beat_options()
{
    TimeUnits duration = k_duration_quarter;
    switch(m_beatNoteChoice->GetSelection())
    {
        case 0: duration = k_duration_whole;            break;
        case 1: duration = k_duration_half_dotted;      break;
        case 2: duration = k_duration_half;             break;
        case 3: duration = k_duration_quarter_dotted;   break;
        case 4: duration = k_duration_quarter;          break;
        case 5: duration = k_duration_eighth_dotted;    break;
        case 6: duration = k_duration_eighth;           break;
        case 7: duration = k_duration_16th;             break;
        default:
            duration = k_duration_quarter;
    }

    int beatType = k_beat_implied;
    if (m_beatSelected->GetValue())
        beatType = k_beat_specified;
    else if (m_beatImplied->GetValue())
        beatType = k_beat_implied;

    m_pMtr->set_beat_type(beatType, duration);
    (static_cast<MainFrame*>(GetParent()))->update_metronome_beat();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::load_current_values()
{
    TimeUnits duration = m_pMtr->get_beat_duration();
    int sel;
    if (is_equal_time(duration, TimeUnits(k_duration_whole)))
        sel = 0;
    else if (is_equal_time(duration, TimeUnits(k_duration_half_dotted)))
        sel = 1;
    else if(is_equal_time(duration, TimeUnits(k_duration_half)))
        sel = 2;
    else if (is_equal_time(duration, TimeUnits(k_duration_quarter_dotted)))
        sel = 3;
    else if (is_equal_time(duration, TimeUnits(k_duration_quarter)))
        sel = 4;
    else if (is_equal_time(duration, TimeUnits(k_duration_eighth_dotted)))
        sel = 5;
    else if (is_equal_time(duration, TimeUnits(k_duration_eighth)))
        sel = 6;
    else if (is_equal_time(duration, TimeUnits(k_duration_16th)))
        sel = 7;
    else
        sel = 4;    //quarter;
    m_beatNoteChoice->SetSelection(sel);

    int beatType = m_pMtr->get_beat_type();
    if (beatType == k_beat_specified)
    {
        m_beatSelected->SetValue(true);
        m_beatImplied->SetValue(false);
    }
    else
    {
        m_beatImplied->SetValue(true);
        m_beatSelected->SetValue(false);
    }
}


}  //namespace lenmus
