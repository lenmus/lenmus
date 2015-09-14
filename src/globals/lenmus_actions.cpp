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
#include "lenmus_actions.h"
#include "lenmus_standard_header.h"

#include "lenmus_string.h"

#include <sstream>
using namespace std;

namespace lenmus
{


//=======================================================================================
// Shortcut implementation
//=======================================================================================
Shortcut::Shortcut(int flags, int key)
    : m_keyCode(key)
    , m_keyFlags(flags)
{
}

//---------------------------------------------------------------------------------------
Shortcut::Shortcut()
    : m_keyCode(0)
    , m_keyFlags(0)
{
}

//---------------------------------------------------------------------------------------
Shortcut::Shortcut(const Shortcut& s)
    : m_keyCode(s.m_keyCode)
    , m_keyFlags(s.m_keyFlags)
{
}

//---------------------------------------------------------------------------------------
wxString Shortcut::to_string()
{
    if (is_empty())
        return _("(Undefined)");
    else
    {
        wxString name = _T("");
        if (m_keyFlags & wxMOD_CMD)
            name += _T("Ctrl+");
        if (m_keyFlags & wxMOD_ALT)
            name += _T("Alt+");
        if (m_keyFlags & wxMOD_SHIFT)
            name += _T("Shift+");
        if (m_keyFlags & wxMOD_META)
            name += _T("Meta+");

        name += key_code_to_name();
        return name;
    }

}

//---------------------------------------------------------------------------------------
wxString Shortcut::key_code_to_name()
{
    wxString sKey;
    switch ( m_keyCode )
    {
        case WXK_BACK: sKey = _T("Back"); break;
        case WXK_TAB: sKey = _T("Tab"); break;
        case WXK_RETURN: sKey = _T("Return"); break;
        case WXK_ESCAPE: sKey = _T("Escape"); break;
        case WXK_SPACE: sKey = _T("Spacebar"); break;
        case WXK_DELETE: sKey = _T("Delete"); break;

        case WXK_START: sKey = _T("START"); break;
        case WXK_LBUTTON: sKey = _T("LBUTTON"); break;
        case WXK_RBUTTON: sKey = _T("RBUTTON"); break;
        case WXK_CANCEL: sKey = _T("CANCEL"); break;
        case WXK_MBUTTON: sKey = _T("MBUTTON"); break;
        case WXK_CLEAR: sKey = _T("CLEAR"); break;
        case WXK_SHIFT: sKey = _T("SHIFT"); break;
        case WXK_ALT: sKey = _T("ALT"); break;
        case WXK_CONTROL: sKey = _T("CONTROL"); break;
        case WXK_MENU: sKey = _T("MENU"); break;
        case WXK_PAUSE: sKey = _T("Pause"); break;
        case WXK_CAPITAL: sKey = _T("CAPITAL"); break;
        case WXK_END: sKey = _T("End"); break;
        case WXK_HOME: sKey = _T("Home"); break;
        case WXK_LEFT: sKey = _T("Left arrow"); break;
        case WXK_UP: sKey = _T("Up arrow"); break;
        case WXK_RIGHT: sKey = _T("Right arrow"); break;
        case WXK_DOWN: sKey = _T("Down arrow"); break;
        case WXK_SELECT: sKey = _T("SELECT"); break;
        case WXK_PRINT: sKey = _T("Print"); break;
        case WXK_EXECUTE: sKey = _T("EXECUTE"); break;
        case WXK_SNAPSHOT: sKey = _T("SNAPSHOT"); break;
        case WXK_INSERT: sKey = _T("Insert"); break;
        case WXK_HELP: sKey = _T("HELP"); break;
        case WXK_NUMPAD0: sKey = _T("NUMPAD0"); break;
        case WXK_NUMPAD1: sKey = _T("NUMPAD1"); break;
        case WXK_NUMPAD2: sKey = _T("NUMPAD2"); break;
        case WXK_NUMPAD3: sKey = _T("NUMPAD3"); break;
        case WXK_NUMPAD4: sKey = _T("NUMPAD4"); break;
        case WXK_NUMPAD5: sKey = _T("NUMPAD5"); break;
        case WXK_NUMPAD6: sKey = _T("NUMPAD6"); break;
        case WXK_NUMPAD7: sKey = _T("NUMPAD7"); break;
        case WXK_NUMPAD8: sKey = _T("NUMPAD8"); break;
        case WXK_NUMPAD9: sKey = _T("NUMPAD9"); break;
        case WXK_MULTIPLY: sKey = _T("MULTIPLY"); break;
        case WXK_ADD: sKey = _T("ADD"); break;
        case WXK_SEPARATOR: sKey = _T("SEPARATOR"); break;
        case WXK_SUBTRACT: sKey = _T("SUBTRACT"); break;
        case WXK_DECIMAL: sKey = _T("DECIMAL"); break;
        case WXK_DIVIDE: sKey = _T("DIVIDE"); break;
        case WXK_F1: sKey = _T("F1"); break;
        case WXK_F2: sKey = _T("F2"); break;
        case WXK_F3: sKey = _T("F3"); break;
        case WXK_F4: sKey = _T("F4"); break;
        case WXK_F5: sKey = _T("F5"); break;
        case WXK_F6: sKey = _T("F6"); break;
        case WXK_F7: sKey = _T("F7"); break;
        case WXK_F8: sKey = _T("F8"); break;
        case WXK_F9: sKey = _T("F9"); break;
        case WXK_F10: sKey = _T("F10"); break;
        case WXK_F11: sKey = _T("F11"); break;
        case WXK_F12: sKey = _T("F12"); break;
        case WXK_F13: sKey = _T("F13"); break;
        case WXK_F14: sKey = _T("F14"); break;
        case WXK_F15: sKey = _T("F15"); break;
        case WXK_F16: sKey = _T("F16"); break;
        case WXK_F17: sKey = _T("F17"); break;
        case WXK_F18: sKey = _T("F18"); break;
        case WXK_F19: sKey = _T("F19"); break;
        case WXK_F20: sKey = _T("F20"); break;
        case WXK_F21: sKey = _T("F21"); break;
        case WXK_F22: sKey = _T("F22"); break;
        case WXK_F23: sKey = _T("F23"); break;
        case WXK_F24: sKey = _T("F24"); break;
        case WXK_NUMLOCK: sKey = _T("NumLock"); break;
        case WXK_SCROLL: sKey = _T("SCROLL"); break;
        case WXK_PAGEUP: sKey = _T("Page Up"); break;
        case WXK_PAGEDOWN: sKey = _T("Page Down"); break;

        case WXK_NUMPAD_SPACE: sKey = _T("NUMPAD_SPACE"); break;
        case WXK_NUMPAD_TAB: sKey = _T("NUMPAD_TAB"); break;
        case WXK_NUMPAD_ENTER: sKey = _T("NUMPAD_ENTER"); break;
        case WXK_NUMPAD_F1: sKey = _T("NUMPAD_F1"); break;
        case WXK_NUMPAD_F2: sKey = _T("NUMPAD_F2"); break;
        case WXK_NUMPAD_F3: sKey = _T("NUMPAD_F3"); break;
        case WXK_NUMPAD_F4: sKey = _T("NUMPAD_F4"); break;
        case WXK_NUMPAD_HOME: sKey = _T("NUMPAD_HOME"); break;
        case WXK_NUMPAD_LEFT: sKey = _T("NUMPAD_LEFT"); break;
        case WXK_NUMPAD_UP: sKey = _T("NUMPAD_UP"); break;
        case WXK_NUMPAD_RIGHT: sKey = _T("NUMPAD_RIGHT"); break;
        case WXK_NUMPAD_DOWN: sKey = _T("NUMPAD_DOWN"); break;
        case WXK_NUMPAD_PAGEUP: sKey = _T("NUMPAD_PAGEUP"); break;
        case WXK_NUMPAD_PAGEDOWN: sKey = _T("NUMPAD_PAGEDOWN"); break;
        case WXK_NUMPAD_END: sKey = _T("NUMPAD_END"); break;
        case WXK_NUMPAD_BEGIN: sKey = _T("NUMPAD_BEGIN"); break;
        case WXK_NUMPAD_INSERT: sKey = _T("NUMPAD_INSERT"); break;
        case WXK_NUMPAD_DELETE: sKey = _T("NUMPAD_DELETE"); break;
        case WXK_NUMPAD_EQUAL: sKey = _T("NUMPAD_EQUAL"); break;
        case WXK_NUMPAD_MULTIPLY: sKey = _T("NUMPAD_MULTIPLY"); break;
        case WXK_NUMPAD_ADD: sKey = _T("NUMPAD_ADD"); break;
        case WXK_NUMPAD_SEPARATOR: sKey = _T("NUMPAD_SEPARATOR"); break;
        case WXK_NUMPAD_SUBTRACT: sKey = _T("NUMPAD_SUBTRACT"); break;
        case WXK_NUMPAD_DECIMAL: sKey = _T("NUMPAD_DECIMAL"); break;
        case WXK_NUMPAD_DIVIDE: sKey = _T("NUMPAD_DIVIDE"); break;

        // the following key codes are only generated under Windows currently
         case WXK_WINDOWS_LEFT: sKey = _T("WINDOWS_LEFT"); break;
         case WXK_WINDOWS_RIGHT: sKey = _T("WINDOWS_RIGHT"); break;
         case WXK_WINDOWS_MENU: sKey = _T("WINDOWS_MENU"); break;
         //case WXK_COMMAND: sKey = _T("COMMAND"); break;

        // Hardware-specific buttons
         case WXK_SPECIAL1: sKey = _T("SPECIAL1"); break;
         case WXK_SPECIAL2: sKey = _T("SPECIAL2"); break;
         case WXK_SPECIAL3: sKey = _T("SPECIAL3"); break;
         case WXK_SPECIAL4: sKey = _T("SPECIAL4"); break;
         case WXK_SPECIAL5: sKey = _T("SPECIAL5"); break;
         case WXK_SPECIAL6: sKey = _T("SPECIAL6"); break;
         case WXK_SPECIAL7: sKey = _T("SPECIAL7"); break;
         case WXK_SPECIAL8: sKey = _T("SPECIAL8"); break;
         case WXK_SPECIAL9: sKey = _T("SPECIAL9"); break;
         case WXK_SPECIAL10: sKey = _T("SPECIAL10"); break;
         case WXK_SPECIAL11: sKey = _T("SPECIAL11"); break;
         case WXK_SPECIAL12: sKey = _T("SPECIAL12"); break;
         case WXK_SPECIAL13: sKey = _T("SPECIAL13"); break;
         case WXK_SPECIAL14: sKey = _T("SPECIAL14"); break;
         case WXK_SPECIAL15: sKey = _T("SPECIAL15"); break;
         case WXK_SPECIAL16: sKey = _T("SPECIAL16"); break;
         case WXK_SPECIAL17: sKey = _T("SPECIAL17"); break;
         case WXK_SPECIAL18: sKey = _T("SPECIAL18"); break;
         case WXK_SPECIAL19: sKey = _T("SPECIAL19"); break;
         case WXK_SPECIAL20: sKey = _T("SPECIAL20"); break;


        default:
        {
            if ( wxIsprint(m_keyCode ) )
                sKey.Printf(_T("%c"), char(m_keyCode));
            else if ( m_keyCode > 0 && m_keyCode < 27 )
                sKey.Printf(_T("Ctrl-%c"), _T('A') + m_keyCode - 1);
            else
                sKey.Printf(_T("unknown (%d)"), m_keyCode);
        }
    }
//    wxString num;
//    num.Printf(_T(" (%d)"), m_keyCode);
//    sKey += num;
    return sKey;
}


//=======================================================================================
// AppAction implementation
//=======================================================================================
AppAction::AppAction(int code, int category, long contexts,
                     const wxString& description,
                     const wxString& tag, const Shortcut& s)
    : m_code(code)
    , m_context(contexts)
    , m_category(category)
    , m_description(description)
    , m_tag(tag)
    , m_shortcut(s)
    , m_default(s)
{
}

//---------------------------------------------------------------------------------------
AppAction::~AppAction()
{
}


//=======================================================================================
// KeyTranslator implementation
//=======================================================================================
KeyTranslator::KeyTranslator(ApplicationScope& appScope)
    : m_appScope(appScope)
{
    load_defaults();
}

//---------------------------------------------------------------------------------------
KeyTranslator::~KeyTranslator()
{
    save_user_preferences();
    clear_actions();
}

//---------------------------------------------------------------------------------------
void KeyTranslator::clear_actions()
{
    vector<AppAction*>::iterator it;
    for (it = m_actions.begin(); it != m_actions.end(); ++it)
        delete *it;
    m_actions.clear();
}

//---------------------------------------------------------------------------------------
AppAction* KeyTranslator::find(int code)
{
    vector<AppAction*>::iterator it;
    for (it = m_actions.begin(); it != m_actions.end(); ++it)
    {
        if ((*it)->code() == code)
            return *it;
    }
    return NULL;
}

//---------------------------------------------------------------------------------------
AppAction* KeyTranslator::find(const Shortcut& s)
{
    vector<AppAction*>::iterator it;
    for (it = m_actions.begin(); it != m_actions.end(); ++it)
    {
        if ((*it)->shortcut_is(s))
            return *it;
    }
    return NULL;
}

//---------------------------------------------------------------------------------------
AppAction* KeyTranslator::find(const wxString& description)
{
    vector<AppAction*>::iterator it;
    for (it = m_actions.begin(); it != m_actions.end(); ++it)
    {
        if ((*it)->description() == description)
            return *it;
    }
    return NULL;
}

//---------------------------------------------------------------------------------------
int KeyTranslator::get_action_for(long context, int key, int flags)
{
    Shortcut s(flags, key);
    vector<AppAction*>::iterator it;
    for (it = m_actions.begin(); it != m_actions.end(); ++it)
    {
        if ((*it)->shortcut_is(s) && (*it)->is_valid_for(context))
            return (*it)->code();
    }
    return k_cmd_null;
}

//---------------------------------------------------------------------------------------
int KeyTranslator::get_max_category()
{
    return k_cat_max;
}

//---------------------------------------------------------------------------------------
wxString KeyTranslator::get_category_name(int i)
{
    switch (i)
    {
        case k_cat_general:
            return _("General actions on documents");
        case k_cat_gui:
            return _("Application appearance and layout");
        case k_cat_toolbox:
            return _("Toolbox: select tools and options");
        case k_cat_edition_score:
            return _("Score edition");
        case k_cat_edition_common:
            return _("Edition: general actions");
        default:
            return _T("Error: Category not defined!!!");
    }
}

//---------------------------------------------------------------------------------------
void KeyTranslator::get_user_preferences(AppAction* a)
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    wxString tagname = _T("/AppActions/") + a->tag() + _T("/");
    long key = pPrefs->Read(tagname + _T("Key"), 0L);
    if (key != 0L)
    {
        long flags = pPrefs->Read(tagname + _T("Flags"), 0L);
        a->set_shortcut( Shortcut(int(flags), int(key)) );
    }
}

//---------------------------------------------------------------------------------------
void KeyTranslator::save_user_preferences()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    vector<AppAction*>::iterator it;
    for (it = m_actions.begin(); it != m_actions.end(); ++it)
    {
        wxString tagname = _T("/AppActions/") + (*it)->tag() + _T("/");
        if ((*it)->has_shortcut())
        {
            pPrefs->Write(tagname + _T("Key"), (*it)->key());
            pPrefs->Write(tagname + _T("Flags"), (*it)->flags());
        }
        else
            pPrefs->DeleteGroup(tagname);
    }
}

//---------------------------------------------------------------------------------------
void KeyTranslator::register_action(int id, int category, long contexts,
                                    const wxString &description,
                                    const wxString &tag, Shortcut s)
{
    m_actions.push_back(
        LENMUS_NEW AppAction(id, category, contexts, description, tag, s)
    );
    get_user_preferences(m_actions.back());
}

//---------------------------------------------------------------------------------------
void KeyTranslator::load_defaults()
{
    clear_actions();

    //General actions on documents ------------------------------------------------------
    register_action(
        k_cmd_file_new, k_cat_general,
        k_key_context_main_menu,
        _("Create a new blank document"), _T("FileNew"), Shortcut(wxMOD_CMD, int('N'))
    );
    register_action(
        k_cmd_file_open, k_cat_general,
        k_key_context_main_menu,
        _("Open a existing document"), _T("FileOpen"), Shortcut(wxMOD_CMD, int('O'))
    );
    register_action(
        k_cmd_file_reload, k_cat_general,
        k_key_context_main_menu,
        _("Reload current document"), _T("FileReload"), Shortcut(wxMOD_CMD, int('R'))
    );
    register_action(
        k_cmd_open_books, k_cat_general,
        k_key_context_main_menu,
        _("Open a music book"), _T("OpenBooks"), Shortcut()
    );
    register_action(
        k_cmd_file_close, k_cat_general,
        k_key_context_main_menu,
        _("Close current document"), _T("FileClose"), Shortcut(wxMOD_CMD, int('W'))
    );
    register_action(
        k_cmd_file_save, k_cat_general,
        k_key_context_main_menu,
        _("Save current document to file"), _T("FileSave"), Shortcut(wxMOD_CMD, int('S'))
    );
    register_action(
        k_cmd_file_save_as, k_cat_general,
        k_key_context_main_menu,
        _("Save current document with a different name"), _T("FileSaveAs"), Shortcut()
    );
    register_action(
        k_cmd_file_convert, k_cat_general,
        k_key_context_main_menu,
        _("Convert document to latest format"), _T("FileConvert"), Shortcut()
    );
    register_action(
        k_cmd_file_close_all, k_cat_general,
        k_key_context_main_menu,
        _("Close all documents"), _T("FileCloseAll"), Shortcut(wxMOD_SHIFT | wxMOD_CMD, int('W'))
    );
    register_action(
        k_cmd_print, k_cat_general,
        k_key_context_main_menu,
        _("Print current document"), _T("Print"), Shortcut(wxMOD_CMD, int('P'))
    );
    register_action(
        k_cmd_zoom_in, k_cat_general,
        k_key_context_main_menu,
        _("Zoom in"), _T("ZoomIn"), Shortcut(wxMOD_CMD, int('+'))
    );
    register_action(
        k_cmd_zoom_out, k_cat_general,
        k_key_context_main_menu,
        _("Zoom out"), _T("ZoomOut"), Shortcut(wxMOD_CMD, int('-'))
    );
    register_action(
        k_cmd_edit_enable_edition, k_cat_general,
        k_key_context_main_menu,
        _("Allow edition of current document"), _T("EditEnable"), Shortcut(wxMOD_CMD, int('E'))
    );

    //Application appearance and layout -------------------------------------------------
    register_action(
        k_cmd_exit_app, k_cat_gui,
        k_key_context_main_menu,
        _("Exit program"), _T("Exit"), Shortcut(wxMOD_CMD, int('Q'))
    );
    register_action(
        k_cmd_print_setup, k_cat_gui,
        k_key_context_main_menu,
        _("Configure printer options"), _T("PrintSetup"), Shortcut()
    );

    //Always valid in edition mode ------------------------------------------------------
        //undo & redo
    register_action(
        k_cmd_edit_undo, k_cat_edition_common,
        k_key_context_edition_any,
        _("Undo last action"), _T("Undo"), Shortcut(wxMOD_CMD, int('Z'))
    );
    register_action(
        k_cmd_edit_redo, k_cat_edition_common,
        k_key_context_edition_any,
        _("Redo last undo"), _T("Redo"), Shortcut()
    );
        //move cursor
    register_action(
        k_cmd_cursor_move_prev, k_cat_edition_common,
        k_key_context_edition_any,
        _("Move cursor back"), _T("CursorPrev"), Shortcut(wxMOD_NONE, WXK_LEFT)
    );
    register_action(
        k_cmd_cursor_move_next, k_cat_edition_common,
        k_key_context_edition_any,
        _("Move cursor forward"), _T("CursorNext"), Shortcut(wxMOD_NONE, WXK_RIGHT)
    );
    register_action(
        k_cmd_cursor_move_down, k_cat_edition_common,
        k_key_context_edition_any,
        _("Move cursor down"), _T("CursorDown"), Shortcut(wxMOD_NONE, WXK_DOWN)
    );
    register_action(
        k_cmd_cursor_move_up, k_cat_edition_common,
        k_key_context_edition_any,
        _("Move cursor up"), _T("CursorUp"), Shortcut(wxMOD_NONE, WXK_UP)
    );
    register_action(
        k_cmd_cursor_enter, k_cat_edition_common,
        k_key_context_edition_any,
        _("Cursor: enter object"), _T("CursorEnter"), Shortcut(wxMOD_NONE, WXK_RETURN)
    );
    register_action(
        k_cmd_cursor_exit, k_cat_edition_common,
        k_key_context_edition_any,
        _("Cursor: exit object"), _T("CursorExit"), Shortcut(wxMOD_CMD, WXK_RETURN)
    );
    register_action(
        k_cmd_cursor_to_next_page, k_cat_edition_common,
        k_key_context_edition_any,
        _("Move cursor to next page"), _T("CursorNextPage"), Shortcut(wxMOD_NONE, WXK_PAGEDOWN)
    );
    register_action(
        k_cmd_cursor_to_prev_page, k_cat_edition_common,
        k_key_context_edition_any,
        _("Move cursor to previous page"), _T("CursorPrevPage"), Shortcut(wxMOD_NONE, WXK_PAGEUP)
    );
        //delete objects
    register_action(
        k_cmd_delete_selection_or_pointed_object, k_cat_edition_common,
        k_key_context_edition_any,
        _("Delete selection or pointed object"), _T("DeleteSelection"), Shortcut(wxMOD_NONE, WXK_DELETE)
    );
    register_action(
        k_cmd_move_prev_and_delete_pointed_object, k_cat_edition_common,
        k_key_context_edition_any,
        _("Move cursor back and delete object"), _T("MoveBackDelete"), Shortcut(wxMOD_NONE, WXK_BACK)
    );
        //insert objects at top level ---------------------------------------------------
    register_action(
        k_cmd_insert_header, k_cat_edition_common,
        k_key_context_edition_any,
        _("Insert a header"), _T("InsertHeader"), Shortcut(wxMOD_ALT, int('H'))
    );
    register_action(
        k_cmd_insert_paragraph, k_cat_edition_common,
        k_key_context_edition_any,
        _("Insert a paragraph"), _T("InsertParagraph"), Shortcut(wxMOD_ALT, int('P'))
    );
    register_action(
        k_cmd_insert_score, k_cat_edition_common,
        k_key_context_edition_any,
        _("Insert a score"), _T("InsertScore"), Shortcut(wxMOD_ALT, int('S'))
    );
        //selection
    register_action(
        k_cmd_remove_selection, k_cat_edition_common,
        k_key_context_edition_any,
        _("Remove selection"), _T("RemoveSelection"), Shortcut(wxMOD_NONE, WXK_ESCAPE)
    );

    //Only valid when editing a score ---------------------------------------------------
        //move cursor
    register_action(
        k_cmd_cursor_to_next_measure, k_cat_edition_score,
        k_key_context_edition_any,
        _("Move cursor to next measure"), _T("CursorNextMeasure"), Shortcut(wxMOD_CMD, WXK_RIGHT)
    );
    register_action(
        k_cmd_cursor_to_prev_measure, k_cat_edition_score,
        k_key_context_edition_any,
        _("Move cursor to previous measure"), _T("CursorPrevMeasure"), Shortcut(wxMOD_CMD, WXK_LEFT)
    );
    register_action(
        k_cmd_cursor_to_first_measure, k_cat_edition_score,
        k_key_context_edition_any,
        _("Move cursor to first measure"), _T("CursorFirstMeasure"), Shortcut(wxMOD_CMD, WXK_HOME)
    );
    register_action(
        k_cmd_cursor_to_last_measure, k_cat_edition_score,
        k_key_context_edition_any,
        _("Move cursor to last measure"), _T("CursorLastMeasure"), Shortcut(wxMOD_CMD, WXK_END)
    );
    register_action(
        k_cmd_cursor_to_start_of_system, k_cat_edition_score,
        k_key_context_edition_any,
        _("Move cursor to start of system"), _T("CursorStartSystem"), Shortcut(wxMOD_NONE, WXK_HOME)
    );
    register_action(
        k_cmd_cursor_to_end_of_system, k_cat_edition_score,
        k_key_context_edition_any,
        _("Move cursor end of system"), _T("CursorEndSystem"), Shortcut(wxMOD_NONE, WXK_END)
    );
    register_action(
        k_cmd_cursor_to_first_staff, k_cat_edition_score,
        k_key_context_edition_any,
        _("Move cursor to first staff"), _T("CursorFirstStaff"), Shortcut(wxMOD_CMD, WXK_UP)
    );
    register_action(
        k_cmd_cursor_to_last_staff, k_cat_edition_score,
        k_key_context_edition_any,
        _("Move cursor to last staff"), _T("CursorLastStaff"), Shortcut(wxMOD_CMD, WXK_DOWN)
    );
        //add notes (Page notes, Page dictation)
    register_action(
        k_cmd_note_step_a, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note A"), _T("AddNoteA"), Shortcut(wxMOD_NONE, int('A'))
    );
    register_action(
        k_cmd_note_step_b, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note B"), _T("AddNoteB"), Shortcut(wxMOD_NONE, int('B'))
    );
    register_action(
        k_cmd_note_step_c, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note C"), _T("AddNoteC"), Shortcut(wxMOD_NONE, int('C'))
    );
    register_action(
        k_cmd_note_step_d, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note D"), _T("AddNoteD"), Shortcut(wxMOD_NONE, int('D'))
    );
    register_action(
        k_cmd_note_step_e, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note E"), _T("AddNoteE"), Shortcut(wxMOD_NONE, int('E'))
    );
    register_action(
        k_cmd_note_step_f, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note F"), _T("AddNoteF"), Shortcut(wxMOD_NONE, int('F'))
    );
    register_action(
        k_cmd_note_step_g, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note G"), _T("AddNoteG"), Shortcut(wxMOD_NONE, int('G'))
    );
        //add clefs
    register_action(
        k_cmd_clef_ask, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Display dialog for adding a clef"), _T("AddClef"), Shortcut(wxMOD_NONE, int('Y'))
    );
        //add rest
    register_action(
        k_cmd_rest, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add a rest"), _T("AddRest"), Shortcut(wxMOD_NONE, int('0'))
    );
        //barlines
    register_action(
        k_cmd_barline, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add barline"), _T("AddBarline"), Shortcut(wxMOD_NONE, int('L'))
    );
        //add notes to chord
    register_action(
        k_cmd_chord_step_a, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note A to chord"), _T("AddChordNoteA"), Shortcut(wxMOD_SHIFT, int('A'))
    );
    register_action(
        k_cmd_chord_step_b, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note B to chord"), _T("AddChordNoteB"), Shortcut(wxMOD_SHIFT, int('B'))
    );
    register_action(
        k_cmd_chord_step_c, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note C to chord"), _T("AddChordNoteC"), Shortcut(wxMOD_SHIFT, int('C'))
    );
    register_action(
        k_cmd_chord_step_d, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note D to chord"), _T("AddChordNoteD"), Shortcut(wxMOD_SHIFT, int('D'))
    );
    register_action(
        k_cmd_chord_step_e, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note E to chord"), _T("AddChordNoteE"), Shortcut(wxMOD_SHIFT, int('E'))
    );
    register_action(
        k_cmd_chord_step_f, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note F to chord"), _T("AddChordNoteF"), Shortcut(wxMOD_SHIFT, int('F'))
    );
    register_action(
        k_cmd_chord_step_g, k_cat_edition_score,
        k_key_context_notes | k_key_context_dictation,
        _("Add note G to chord"), _T("AddChordNoteG"), Shortcut(wxMOD_SHIFT, int('G'))
    );
        //selection
    register_action(
        k_cmd_select_prev_note, k_cat_edition_score,
        k_key_context_edition_any,
        _("Select prev. note/rest"), _T("SelectPrevNote"), Shortcut(wxMOD_SHIFT, WXK_LEFT)
    );
    register_action(
        k_cmd_select_next_note, k_cat_edition_score,
        k_key_context_edition_any,
        _("Select next note/rest"), _T("SelectNextNote"), Shortcut(wxMOD_SHIFT, WXK_RIGHT)
    );
    register_action(
        k_cmd_select_to_start_of_measure, k_cat_edition_score,
        k_key_context_edition_any,
        _("Select all to start of measure"), _T("SelectToStartMeasure"),
        Shortcut(wxMOD_CMD | wxMOD_SHIFT, WXK_LEFT)
    );
    register_action(
        k_cmd_select_to_end_of_measure, k_cat_edition_score,
        k_key_context_edition_any,
        _("Select all to end of measure"), _T("SelectToEndMeasure"),
        Shortcut(wxMOD_CMD | wxMOD_SHIFT, WXK_RIGHT)
    );
    register_action(
        k_cmd_select_to_start_of_system, k_cat_edition_score,
        k_key_context_edition_any,
        _("Select all to start of system"), _T("SelectToStartSystem"),
        Shortcut(wxMOD_SHIFT, WXK_HOME)
    );
    register_action(
        k_cmd_select_to_end_of_system, k_cat_edition_score,
        k_key_context_edition_any,
        _("Select all to end of system"), _T("SelectToEndSystem"),
        Shortcut(wxMOD_SHIFT, WXK_END)
    );
        //increment/decrement/change octave

    //Toolbox: select pages, tools and options ------------------------------------------
        //select pages
    register_action(
        k_cmd_select_toolpage_notes, k_cat_toolbox,
        k_key_context_notes,
        _("Toolbox: select notes tools"), _T("ToolboxPageNotes"), Shortcut(wxMOD_NONE, WXK_F2)
    );
    register_action(
        k_cmd_select_toolpage_clefs, k_cat_toolbox,
        k_key_context_notes,
        _("Toolbox: select clef tools"), _T("ToolboxPageClefs"), Shortcut(wxMOD_NONE, WXK_F3)
    );
    register_action(
        k_cmd_select_toolpage_barlines, k_cat_toolbox,
        k_key_context_notes,
        _("Toolbox: select barline tools"), _T("ToolboxPageBarlines"), Shortcut(wxMOD_NONE, WXK_F4)
    );
    register_action(
        k_cmd_select_toolpage_symbols, k_cat_toolbox,
        k_key_context_notes,
        _("Toolbox: select symbols tools"), _T("ToolboxPageSymbols"), Shortcut(wxMOD_NONE, WXK_F5)
    );
        //Select options in page notes & dictation
    register_action(
        k_cmd_note_duration_longa, k_cat_toolbox,
        k_key_context_notes,
        _("Select note/rest duration: longa"), _T("NoteDurationLonga"), Shortcut()
    );
    register_action(
        k_cmd_note_duration_breve, k_cat_toolbox,
        k_key_context_notes,
        _("Select note/rest duration: breve"), _T("NoteDurationBreve"), Shortcut(wxMOD_NONE, int('8'))
    );
    register_action(
        k_cmd_note_duration_whole, k_cat_toolbox,
        k_key_context_notes,
        _("Select note/rest duration: whole"), _T("NoteDurationWhole"), Shortcut(wxMOD_NONE, int('7'))
    );
    register_action(
        k_cmd_note_duration_half, k_cat_toolbox,
        k_key_context_notes,
        _("Select note/rest duration: half"), _T("NoteDurationHalf"), Shortcut(wxMOD_NONE, int('6'))
    );
    register_action(
        k_cmd_note_duration_quarter, k_cat_toolbox,
        k_key_context_notes,
        _("Select note/rest duration: quarter"), _T("NoteDurationQuarter"), Shortcut(wxMOD_NONE, int('5'))
    );
    register_action(
        k_cmd_note_duration_eight, k_cat_toolbox,
        k_key_context_notes,
        _("Select note/rest duration: eight"), _T("NoteDurationEight"), Shortcut(wxMOD_NONE, int('4'))
    );
    register_action(
        k_cmd_note_duration_16th, k_cat_toolbox,
        k_key_context_notes,
        _("Select note/rest duration: 16th"), _T("NoteDuration16th"), Shortcut(wxMOD_NONE, int('3'))
    );
    register_action(
        k_cmd_note_duration_32th, k_cat_toolbox,
        k_key_context_notes,
        _("Select note/rest duration: 32th"), _T("NoteDuration32th"), Shortcut(wxMOD_NONE, int('2'))
    );
    register_action(
        k_cmd_note_duration_64th, k_cat_toolbox,
        k_key_context_notes,
        _("Select note/rest duration: 64th"), _T("NoteDuration64th"), Shortcut(wxMOD_NONE, int('1'))
    );
    register_action(
        k_cmd_note_duration_128th, k_cat_toolbox,
        k_key_context_notes,
        _("Select note/rest duration: 128th"), _T("NoteDuration128th"), Shortcut()
    );
    register_action(
        k_cmd_note_duration_256th, k_cat_toolbox,
        k_key_context_notes,
        _("Select note/rest duration: 256th"), _T("NoteDuration256th"), Shortcut()
    );
    register_action(
        k_cmd_note_next_accidental, k_cat_toolbox,
        k_key_context_notes,
        _("Select next accidental"), _T("NextAccidental"), Shortcut(wxMOD_ALT, int('+'))
    );
    register_action(
        k_cmd_note_prev_accidental, k_cat_toolbox,
        k_key_context_notes,
        _("Select prev accidental"), _T("PrevAccidental"), Shortcut(wxMOD_ALT, int('-'))
    );
    register_action(
        k_cmd_note_next_voice, k_cat_toolbox,
        k_key_context_notes,
        _("Select next voice"), _T("NextVoice"), Shortcut()
    );
    register_action(
        k_cmd_note_prev_voice, k_cat_toolbox,
        k_key_context_notes,
        _("Select prev voice"), _T("PrevVoice"), Shortcut()
    );
    register_action(
        k_cmd_note_next_octave, k_cat_toolbox,
        k_key_context_notes,
        _("Select next octave"), _T("NextOctave"), Shortcut()
    );
    register_action(
        k_cmd_note_prev_octave, k_cat_toolbox,
        k_key_context_notes,
        _("Select prev octave"), _T("PrevOctave"), Shortcut()
    );
    register_action(
        k_cmd_note_next_dot, k_cat_toolbox,
        k_key_context_notes,
        _("Increment note/rest dots"), _T("NextDot"), Shortcut(wxMOD_NONE, int('.'))
    );
    register_action(
        k_cmd_note_prev_dot, k_cat_toolbox,
        k_key_context_notes,
        _("Decrement note/rest dots"), _T("PrevDot"), Shortcut(wxMOD_ALT, int('.'))
    );

}

//---------------------------------------------------------------------------------------
int KeyTranslator::translate(long context, int key, int flags)
{
    Shortcut s(flags, key);
    vector<AppAction*>::iterator it;
    for (it = m_actions.begin(); it != m_actions.end(); ++it)
    {
        if ((*it)->shortcut_is(s) && (*it)->is_valid_for(context))
            return (*it)->code();
    }
    return k_cmd_null;
}

//---------------------------------------------------------------------------------------
wxString KeyTranslator::get_action_name(int actionId)
{
    AppAction* action = find(actionId);
    if (action)
        return action->description();
    else
        return _("Action code not defined.");
}


}   //namespace lenmus
