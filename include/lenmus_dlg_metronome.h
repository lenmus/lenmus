//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2018 LenMus project
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

#ifndef __LENMUS_DLG_METRONOME_H__
#define __LENMUS_DLG_METRONOME_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/bmpcbox.h>

//other
#include <ctime>   //clock
#include <chrono>
using namespace std;

namespace lenmus
{

class GlobalMetronome;

//---------------------------------------------------------------------------------------
class DlgMetronome : public wxDialog
{
protected:
    ApplicationScope& m_appScope;
    GlobalMetronome* m_pMtr;
    int m_tempo;
    int m_prevTempo;
    chrono::time_point<chrono::high_resolution_clock> m_prevTime;
    double m_totalTime;
    int m_count;
    double m_min;
    double m_max;

	wxArrayString m_italianTempoChoices;
	wxArrayString m_beatNoteChoices;

		wxStaticText* m_lblTempo;
		wxTextCtrl* m_pTempoDisplay;

		wxChoice* m_pItalianTempo;


		wxSlider* m_pTempoSlider;


		wxButton* m_pIncrementButton;
		wxButton* m_pDecrementButton;

		wxButton* m_pTapButton;

		wxStaticText* m_lblCount;
		wxStaticText* m_pTxtCount;

		wxButton* m_pResetButton;

		wxStaticText* m_lblMax;
		wxStaticText* m_pTxtMax;
		wxStaticText* m_lblMin;
		wxStaticText* m_pTxtMin;

		wxRadioButton* m_beatSelected;
		wxRadioButton* m_beatImplied;
		wxBitmapComboBox* m_beatNoteChoice;

		wxButton* m_pStartButton;



public:

    DlgMetronome(ApplicationScope& appScope, wxWindow* parent, GlobalMetronome* pMtr);
    ~DlgMetronome() {}

    void load_current_values();

protected:
    void create_dialog();
    void load_italian_tempi();
    void load_beat_notes();

    void on_button(wxCommandEvent& event);
    void on_key_down(wxKeyEvent& event);
    void on_tempo_choice(wxCommandEvent& event);
    void on_update_number(wxCommandEvent& WXUNUSED(event));
    void on_tempo_slider(wxCommandEvent& WXUNUSED(event));
    void on_beat_type(wxCommandEvent& WXUNUSED(event));
    void compute_tapped_tempo();
    void reset_tapped_tempo();
    void increment_tempo();
    void decrement_tempo();
    void display_tempo();
    void set_tempo(int nMM);
    void update_count_max_min();
    void set_beat_options();

    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif // __LENMUS_DLG_METRONOME_H__
