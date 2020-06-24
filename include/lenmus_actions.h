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

#ifndef _LENMUS_ACTIONS_H_
#define _LENMUS_ACTIONS_H_

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/string.h>


namespace lenmus
{


//---------------------------------------------------------------------------------------
// context for interpreting a keystroke
// AWARE: When adding a context you have to
//        update method KeyTranslator::get_category_name(int i)
enum EKeyContext
{
    k_key_context_none              = 0x00000000,    //context not defined
    k_key_context_main_menu         = 0x00000001,    //main menu

    //Toolbox pages
    k_key_context_clefs             = 0x00000002,    //toolpage clefs
    k_key_context_notes             = 0x00000004,    //toolpage notes in default Toolbox
    k_key_context_dictation         = 0x00000008,    //toolpage notes in dictation exercises
    k_key_context_barlines          = 0x00000010,    //toolpage barlines
    k_key_context_symbols           = 0x00000020,

    //document
    k_key_context_edition_any       = 0x01000000,    //valid for any toolpage (or no toolpage)

    //pop-up windows

};

//---------------------------------------------------------------------------------------
// category for classifying actions
// AWARE: When adding a category to
//        update method KeyTranslator::get_category_name(int i)
enum EActionCategory
{
    k_cat_general,              //General actions on documents
    k_cat_gui,                  //Application appearance and layout
    k_cat_toolbox,              //Selecting options in Toolbox
    k_cat_edition_score,        //Actions when editing a score
    k_cat_edition_common,       //Always valid in edition mode

    k_cat_max                   //must be the last one
};

//---------------------------------------------------------------------------------------
// all possible actions for which shortcuts can be defined/used
enum EAppActions
{
    //AWARE: Used as indices for vector. Must be in numeric sequence

    k_cmd_null=0,

    //app
        //GUI options
    k_cmd_view_rulers,
    k_cmd_view_toolBar,
    k_cmd_view_statusBar,
    k_cmd_view_page_margins,
    k_cmd_view_welcome_page,
    k_cmd_view_counters,
    k_cmd_view_virtual_keyboard,
    k_cmd_view_voices_in_colours,
    k_cmd_view_toc,
    k_cmd_view_console,
        //preferences
    k_cmd_preferences,
        //tools
    k_cmd_tools_metronome,
        //help
    k_cmd_help_users_guide,
    k_cmd_help_search,
    k_cmd_help_visit_website,
        //other
    k_cmd_check_for_updates,
    k_cmd_exit_app,
    k_cmd_print_setup,
    k_cmd_open_books,

    //document
        //file
    k_cmd_file_new,
    k_cmd_file_open,
    k_cmd_file_reload,
    k_cmd_file_close,
    k_cmd_file_close_all,
    k_cmd_file_save,
    k_cmd_file_save_as,
    k_cmd_file_convert,
    k_cmd_file_export,
    k_cmd_file_export_MusicXML,
    k_cmd_file_export_bmp,
    k_cmd_file_export_jpg,
        //print
    k_cmd_print,
        //zoom
    k_cmd_zoom_in,
    k_cmd_zoom_out,
    k_cmd_zoom_100,
    k_cmd_zoom_fit_full,
    k_cmd_zoom_fit_width,
    k_cmd_zoom_other,

    //edition: general and top level actions
    k_cmd_edit_enable_edition,
    k_cmd_edit_copy,
    k_cmd_edit_cut,
    k_cmd_edit_paste,
    k_cmd_edit_undo,
    k_cmd_edit_redo,
    k_cmd_edit_insert,
    k_cmd_insert_header,
    k_cmd_insert_paragraph,
    k_cmd_insert_score,
        //cursor positioning
    k_cmd_cursor_move_prev,
    k_cmd_cursor_move_next,
    k_cmd_cursor_enter,
    k_cmd_cursor_exit,
    k_cmd_cursor_move_up,
    k_cmd_cursor_move_down,
    k_cmd_cursor_to_next_page,
    k_cmd_cursor_to_prev_page,
        //delete objects
    k_cmd_delete_selection_or_pointed_object,
    k_cmd_move_prev_and_delete_pointed_object,
        //selection
    k_cmd_remove_selection,

	//edition: scores
	k_cmd_score_titles,
        //cursor positioning
    k_cmd_cursor_to_start_of_system,
    k_cmd_cursor_to_end_of_system,
    k_cmd_cursor_to_next_measure,
    k_cmd_cursor_to_prev_measure,
    k_cmd_cursor_to_first_staff,
    k_cmd_cursor_to_last_staff,
    k_cmd_cursor_to_first_measure,
    k_cmd_cursor_to_last_measure,
        //instruments
    k_cmd_instr_properties,
        //insert objects
    k_cmd_clef_ask,
    k_cmd_note_step_a,
    k_cmd_note_step_b,
    k_cmd_note_step_c,
    k_cmd_note_step_d,
    k_cmd_note_step_e,
    k_cmd_note_step_f,
    k_cmd_note_step_g,
    k_cmd_chord_step_a,
    k_cmd_chord_step_b,
    k_cmd_chord_step_c,
    k_cmd_chord_step_d,
    k_cmd_chord_step_e,
    k_cmd_chord_step_f,
    k_cmd_chord_step_g,
        //insert objects with mouse click
    k_cmd_note,
    k_cmd_rest,
    k_cmd_clef,
    k_cmd_barline,
        //selection
    k_cmd_select_prev_note,
    k_cmd_select_next_note,
    k_cmd_select_to_start_of_measure,
    k_cmd_select_to_end_of_measure,
    k_cmd_select_to_start_of_system,
    k_cmd_select_to_end_of_system,

    //sound options
    k_cmd_sound_test,
    k_cmd_sound_off,

    //play back scores
    k_cmd_play_start,
	k_cmd_play_cursor_start,
    k_cmd_play_stop,
    k_cmd_play_pause,
    k_cmd_play_countoff,

    //metronome
    k_cmd_metronome,

    //Toolbox: page selection
	k_cmd_select_toolpage_notes,
	k_cmd_select_toolpage_clefs,
	k_cmd_select_toolpage_barlines,
	k_cmd_select_toolpage_symbols,

    //Toolbox. Page notes
    k_cmd_note_duration_longa,
    k_cmd_note_duration_breve,
    k_cmd_note_duration_whole,
    k_cmd_note_duration_half,
    k_cmd_note_duration_quarter,
    k_cmd_note_duration_eight,
    k_cmd_note_duration_16th,
    k_cmd_note_duration_32nd,
    k_cmd_note_duration_64th,
    k_cmd_note_duration_128th,
    k_cmd_note_duration_256th,
    k_cmd_note_next_accidental,
    k_cmd_note_prev_accidental,
    k_cmd_note_next_voice,
    k_cmd_note_prev_voice,
    k_cmd_note_next_octave,
    k_cmd_note_prev_octave,
    k_cmd_note_next_dot,
    k_cmd_note_prev_dot,
        //increment/decrement/change octave


    //AWARE: MUST BE LAST ONE
    k_cmd_max
};


//=======================================================================================
// Shortcut: data for one shortcut
class Shortcut
{
protected:
    int m_keyCode;      //key code, as returned by event.GetKeyCode()
    int m_keyFlags;     //key modifiers, as defined enum wxKeyModifier

public:
    Shortcut(int flags, int key);
    Shortcut();
    Shortcut(const Shortcut& s);

    //accessors
    inline int key() { return m_keyCode; }
    inline int flags() { return m_keyFlags; }

    //operators
    inline void operator = (const Shortcut& s)
    {
        m_keyCode = s.m_keyCode;
        m_keyFlags = s.m_keyFlags;
    }

    inline bool operator == (const Shortcut& s)
    {
        return s.m_keyFlags == m_keyFlags && s.m_keyCode == m_keyCode;
    }

    //empty shortcut (undefined)
    inline bool is_empty() { return m_keyCode == 0; }
    inline void set_empty() { m_keyCode = 0; m_keyFlags = 0; }

    //conversions
    wxString to_string();

protected:
    wxString key_code_to_name();

};

//=======================================================================================
// AppAction: All data for one action
class AppAction
{
protected:
    int      m_code;        //as defined in EAppActions
    long     m_context;     //as defined in EKeyContext
    int      m_category;    //as defined in EActionCategory
    wxString m_description; //text for descriptions and tooltips
    wxString m_tag;         //tag name for configuration file
    Shortcut m_shortcut;    //current shortcut
    Shortcut m_default;     //default shortcut

public:
    AppAction(int code, int category, long contexts,
              const wxString& description,
              const wxString& tag, const Shortcut& s=Shortcut());
    ~AppAction();

    //accesors
    inline int code() { return m_code; }
    inline wxString& tag() { return m_tag; }
    inline wxString& description() { return m_description; }
    inline int category() { return m_category; }
    inline int key() { return m_shortcut.key(); }
    inline int flags() { return m_shortcut.flags(); }

    //shortcut related
    inline wxString shortcut_as_string() { return m_shortcut.to_string(); }
    inline void remove_shortcut() { m_shortcut.set_empty(); }
    inline void restore_default_shortcut() { m_shortcut = m_default; }
    inline void set_shortcut(const Shortcut& s) { m_shortcut = s; }
    inline bool has_shortcut() { return !m_shortcut.is_empty(); }
    inline bool shortcut_is(const Shortcut& s) { return m_shortcut == s; }

    //context related
    void add_context(long context) { m_context |= context; }
    inline bool is_valid_for(long context) { return m_context & context; }
};


//=======================================================================================
// KeyTranslator: responsible for managing the table of application actions and
// translating shortcuts into commands
class KeyTranslator
{
private:
    ApplicationScope& m_appScope;
    vector<AppAction*> m_actions;

public:
    KeyTranslator(ApplicationScope& appScope);
    ~KeyTranslator();

    int translate(long context, int key, int flags);
    wxString get_action_name(int actionId);
    int get_num_actions() { return int(m_actions.size()); }

    inline AppAction* get_action(int i) {
        return i >=0 && i < int(m_actions.size()) ? m_actions[i] : nullptr;
    }
    AppAction* find(int code);
    AppAction* find(const Shortcut& s);
    AppAction* find(const wxString& description);

    int get_action_for(long context, int key, int flags);

    int get_max_category();
    wxString get_category_name(int i);

    void load_defaults();
    void save_user_preferences();

private:
    void get_user_preferences(AppAction* a);
    void clear_actions();
    void register_action(int id, int category, long contexts,
                         const wxString &description,
                         const wxString &tag, Shortcut s);


};


}   // namespace lenmus

#endif		// _LENMUS_ACTIONS_H_
