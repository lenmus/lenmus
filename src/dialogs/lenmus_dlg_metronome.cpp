//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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

#include <wx/xrc/xmlres.h>
#include <wx/hashmap.h>


namespace lenmus
{

static const long k_id_button = wxNewId();
static const long k_id_choice_italian_tempo = wxNewId();
static const long k_id_button_increment = wxNewId();
static const long k_id_button_decrement = wxNewId();
static const long k_id_button_tap_tempo = wxNewId();
static const long k_id_button_start = wxNewId();
static const long k_id_metronome_number = wxNewId();
static const long k_id_tempo_slider = wxNewId();

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
    {_T("Larghissimo"),   1, 19, 15},
    {_T("Grave"),        20, 39, 30},
    {_T("Lento"),        40, 44, 42},
    {_T("Largo"),        45, 49, 47},
    {_T("Larghetto"),    50, 54, 52},
    {_T("Adagio"),       55, 64, 60},
    {_T("Adagietto"),    65, 68, 66},
    {_T("Andante moderato"), 69, 72, 70},
    {_T("Andante"),      73, 77, 75},
    {_T("Andantino"),    78, 85, 82},
    {_T("Moderato"),     86, 97, 93},
    {_T("Allegretto"),   98, 109, 105},
    {_T("Allegro"),      110, 132, 120},
    {_T("Vivace"),       133, 139, 136},
    {_T("Vivacissimo"),  140, 149, 145},
    {_T("Allegrissimo"), 150, 167, 160},
    {_T("Presto"),       168, 178, 174},
    {_T("Prestissimo"),  178, 999, 200}
};

static int numTempi = sizeof(m_tempi) / sizeof(ItalianTempo);


//---------------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(DlgMetronome, wxDialog)
    EVT_COMMAND_RANGE (k_id_button, k_id_button_start,
                       wxEVT_COMMAND_BUTTON_CLICKED, DlgMetronome::on_button)
//	EVT_CHAR_HOOK(DlgMetronome::on_key_down)
	EVT_CHOICE(k_id_choice_italian_tempo, DlgMetronome::on_tempo_choice)
    EVT_TEXT(k_id_metronome_number, DlgMetronome::on_update_number)
    EVT_SLIDER(k_id_tempo_slider, DlgMetronome::on_tempo_slider)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
DlgMetronome::DlgMetronome(ApplicationScope& appScope, wxWindow* parent,
                           GlobalMetronome* pMtr)
    : wxDialog(parent, wxID_ANY, _("Metronome"))
    , m_appScope(appScope)
    , m_pMtr(pMtr)
    , m_tempo(pMtr->get_mm())
{
    if (m_pMtr->is_running())
        m_pMtr->stop();

    load_italian_tempi();
    create_dialog();
    SetFocus();
    display_tempo();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::create_dialog()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText8 = new wxStaticText( this, wxID_ANY, _("Tempo:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer13->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_pTempoDisplay = new wxTextCtrl( this, k_id_metronome_number, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_pTempoDisplay, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_pItalianTempo = new wxChoice( this, k_id_choice_italian_tempo, wxDefaultPosition, wxDefaultSize, m_italianTempoChoices, 0 );
	m_pItalianTempo->SetSelection( 0 );
	bSizer13->Add( m_pItalianTempo, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	pMainSizer->Add( bSizer13, 1, wxEXPAND, 5 );

	wxBoxSizer* pTempoSliderSizer;
	pTempoSliderSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pTempoSlider = new wxSlider( this, k_id_tempo_slider, 60, 1, 300, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	pTempoSliderSizer->Add( m_pTempoSlider, 1, wxALL, 5 );

	pMainSizer->Add( pTempoSliderSizer, 1, wxEXPAND, 5 );


	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );

	m_pIncrementButton = new wxButton( this, k_id_button_increment, wxT("+"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_pIncrementButton, 0, wxALL, 5 );

	m_pDecrementButton = new wxButton( this, k_id_button_decrement, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_pDecrementButton, 0, wxALL, 5 );

	m_pTapButton = new wxButton( this, k_id_button_tap_tempo, _("or tap tempo with this button"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_pTapButton, 0, wxALL, 5 );

	pMainSizer->Add( bSizer12, 1, wxEXPAND, 5 );

	wxBoxSizer* pCloseSizer;
	pCloseSizer = new wxBoxSizer( wxHORIZONTAL );

	pCloseSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pStartButton = new wxButton( this, k_id_button_start, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	pCloseSizer->Add( m_pStartButton, 0, wxALL, 5 );

	pCloseSizer->Add( 0, 0, 1, wxEXPAND, 5 );



	pMainSizer->Add( pCloseSizer, 1, wxEXPAND, 5 );

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
        m_italianTempoChoices.Add( wxString::Format(_T("%s (%d-%d)"),
                                                    m_tempi[i].name.wx_str(),
                                                    m_tempi[i].minTempo,
                                                    m_tempi[i].maxTempo) );
    }
    m_italianTempoChoices.Add( wxString::Format(_T("%s (> %d)"),
                                                m_tempi[i].name.wx_str(),
                                                m_tempi[i].minTempo) );
}

//---------------------------------------------------------------------------------------
void DlgMetronome::on_button(wxCommandEvent& event)
{
    int button = event.GetId();
    if (button == k_id_button_increment)
    {
        increment_tempo();
        display_tempo();
    }
    else if (button == k_id_button_decrement)
    {
        decrement_tempo();
        display_tempo();
    }
    else if (button == k_id_button_tap_tempo)
    {
        compute_tapped_tempo();
        display_tempo();
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
    ptime now = microsec_clock::universal_time();
    time_duration diff = now - m_prevTime;
    double period( diff.total_milliseconds() );     //millisecods
    m_prevTime = now;

    if (period > 10000.0)
        return;

    set_tempo(60000.0 / period);
}

//---------------------------------------------------------------------------------------
void DlgMetronome::increment_tempo()
{
    m_tempo++;
    if (m_tempo > 300)
        m_tempo = 300;
    set_tempo(m_tempo);
}

//---------------------------------------------------------------------------------------
void DlgMetronome::decrement_tempo()
{
    m_tempo--;
    if (m_tempo < 1)
        m_tempo = 1;
    set_tempo(m_tempo);
}

//---------------------------------------------------------------------------------------
void DlgMetronome::display_tempo()
{
    m_pTempoDisplay->ChangeValue( wxString::Format(_T("%d"), m_tempo) );
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
void DlgMetronome::on_tempo_choice(wxCommandEvent& event)
{
    int i = m_pItalianTempo->GetSelection();
    set_tempo( m_tempi[i].defaultTempo );
    display_tempo();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::on_update_number(wxCommandEvent& WXUNUSED(event))
{
    wxString value = m_pTempoDisplay->GetValue();
    long num = 0L;
    if (value.ToLong(&num))
    {
        m_tempo = num;
        if (m_tempo < 1)
            m_tempo = 1;
        else if (m_tempo > 300)
            m_tempo = 300;
    }
    set_tempo(m_tempo);
    display_tempo();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::on_tempo_slider(wxCommandEvent& WXUNUSED(event))
{
    set_tempo( m_pTempoSlider->GetValue() );
    display_tempo();
}

//---------------------------------------------------------------------------------------
void DlgMetronome::on_key_down(wxKeyEvent& event)
{
//    int ch = event.GetKeyCode();
//    if (ch == WXK_ESCAPE)
//    {
//        EndModal(wxID_CANCEL);
//        return;
//    }
//
//    int i = sLetters.Find(ch);
//    if (i == wxNOT_FOUND)
//    {
//        wxString letters = sLetters.Upper();
//        i = letters.Find(ch);
//    }
//
//    //wxMessageBox(wxString::Format(_T("OnKeyDown in DlgMetronome. key=%d, i=%d"), ch, i));
//    if (i == wxNOT_FOUND)
//        return;     //ignore key
//
//    m_index = m_tClefs[i].nClefType;
    EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
void DlgMetronome::set_tempo(int nMM)
{
    m_tempo = nMM;
    m_pMtr->set_mm(nMM);
}


}  //namespace lenmus
