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
#include "lenmus_command_event_handler.h"
#include "lenmus_tool_page_notes.h"
#include "lenmus_tool_page_clefs.h"
#include "lenmus_tool_page_barlines.h"
#include "lenmus_tool_page_symbols.h"
#include "lenmus_tool_box_events.h"
#include "lenmus_document_canvas.h"
#include "lenmus_string.h"
#include "lenmus_dlg_clefs.h"
#include "lenmus_edit_interface.h"

//lomse
#include <lomse_command.h>
#include <lomse_ldp_exporter.h>
#include <lomse_shapes.h>
#include <lomse_glyphs.h>
#include <lomse_tasks.h>
#include <lomse_clef_engraver.h>
#include <lomse_barline_engraver.h>
#include <lomse_note_engraver.h>
#include <lomse_rest_engraver.h>
#include <lomse_score_meter.h>
#include <lomse_document_cursor.h>
using namespace lomse;

////do not re-draw the score after executing the command
//#define lmNO_REDRAW    false


namespace lenmus
{

//=======================================================================================
// CommandEventHandler implementation
//=======================================================================================
CommandEventHandler::CommandEventHandler(ApplicationScope& appScope,
                                         DocumentWindow* pController,
                                         ToolsInfo& toolsInfo,
                                         SelectionSet* selection,
                                         DocCursor* cursor)
    : m_appScope(appScope)
    , m_pController(pController)
    , m_toolsInfo(toolsInfo)
    , m_selection(selection)
    , m_cursor(cursor)
    , m_executer(pController, selection, cursor)
{
}

//---------------------------------------------------------------------------------------
CommandEventHandler::~CommandEventHandler()
{
//    //delete mouse cursors
//    std::vector<wxCursor*>::iterator it;
//    for (it = m_MouseCursors.begin(); it != m_MouseCursors.end(); ++it)
//        delete *it;
//
//    //delete cursor drag images
//    if (m_pToolBitmap)
//        delete m_pToolBitmap;
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::process_key_event(wxKeyEvent& event)
{
    m_key = event.GetKeyCode();
    m_keyFlags = event.GetModifiers();

    check_commands_for_current_toolbox_context();
    if (event_processed())
        return;

    check_always_valid_edition_commands();

#if (LENMUS_DEBUG_BUILD == 1)
    if (event_processed())
        return;

    Shortcut s(event.GetModifiers(), event.GetKeyCode());
    wxString name = s.to_string();
    wxString msg = wxString::Format(
        "[CommandEventHandler::process_key_event] Key not processed. keyCmd=%d - %s",
        m_keyCmd, name.wx_str());
    wxMessageBox(msg);
//        LogKeyEvent("Key Press", event, nTool);
#endif
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::check_commands_for_current_toolbox_context()
{
    EditInterface* pGui = m_appScope.get_edit_gui();
    m_keyCmd = pGui->translate_key(m_key, m_keyFlags);
    if (m_keyCmd == k_cmd_null)
        return;

    m_fEventProcessed = true;
    switch (m_keyCmd)
    {
        case k_cmd_note_step_a:
            add_note("a");
            break;

        case k_cmd_note_step_b:
            add_note("b");
            break;

        case k_cmd_note_step_c:
            add_note("c");
            break;

        case k_cmd_note_step_d:
            add_note("d");
            break;

        case k_cmd_note_step_e:
            add_note("e");
            break;

        case k_cmd_note_step_f:
            add_note("f");
            break;

        case k_cmd_note_step_g:
            add_note("g");
            break;

        case k_cmd_chord_step_a:
            add_chord_note("a");
            break;

        case k_cmd_chord_step_b:
            add_chord_note("b");
            break;

        case k_cmd_chord_step_c:
            add_chord_note("c");
            break;

        case k_cmd_chord_step_d:
            add_chord_note("d");
            break;

        case k_cmd_chord_step_e:
            add_chord_note("e");
            break;

        case k_cmd_chord_step_f:
            add_chord_note("f");
            break;

        case k_cmd_chord_step_g:
            add_chord_note("g");
            break;


        case k_cmd_rest:
        {
            //do insert rest
            ScoreCursor* pCursor = static_cast<ScoreCursor*>( m_cursor->get_inner_cursor() );
            m_executer.insert_rest(m_toolsInfo.noteType, m_toolsInfo.dots,
                                   m_toolsInfo.voice, pCursor->staff());
            break;
        }

        case k_cmd_clef_ask:
            ask_and_add_clef();
            break;

        case k_cmd_barline:
            m_executer.insert_barline(m_toolsInfo.barlineType);
            break;

        default:
            m_fEventProcessed = false;
    }

//    int nKeyCode = event.GetKeyCode();
//
//    //general automata structure:
//    //    if terminal key
//    //        add_to_command_string()
//    //        process_command_string()
//    //    else
//    //        add_to_command_buffer()
//
////            bool fTiedPrev = false;
////
////            //if terminal symbol, analyze full command
////            if ((nKeyCode >= int('A') && nKeyCode <= int('G')) ||
////                (nKeyCode >= int('a') && nKeyCode <= int('g')) ||
////                nKeyCode == int(' ') )
////            {
////                if (m_sCmd != "")
////                {
////                    lmKbdCmdParser oCmdParser;
////                    if (oCmdParser.ParserCommand(m_sCmd))
////                    {
////                        m_nSelAcc = oCmdParser.GetAccidentals();
////                        m_nSelDots = oCmdParser.GetDots();
////                        fTiedPrev = oCmdParser.GetTiedPrev();
////                    }
////                }
////            }
////
////            //compute note/rest duration
////            float rDuration = lmLDPParser::GetDefaultDuration(m_nSelNoteType, m_nSelDots, 0, 0);
////
//            //insert note
//            if ((nKeyCode >= int('A') && nKeyCode <= int('G')) ||
//                (nKeyCode >= int('a') && nKeyCode <= int('g')) )
//            {
//                //get step
//                if (nKeyCode > int('G'))
//                    nKeyCode -= 32;          //convert key to upper case
//                static wxString sSteps = "abcdefg";
//                wxString stepLetter(sSteps.GetChar( nKeyCode - int('A') ));
//                string step = to_std_string(stepLetter);
//
////                // determine octave
//                int octave = m_toolsInfo.octave;
////                if (event.ShiftDown())
////                    ++octave;
////                else if (event.CmdDown())
////                    --octave;
////
////                //limit octave 0..9
////                if (octave < 0)
////                    octave = 0;
////                else if (octave > 9)
////                    octave = 9;
////
//    //TODO: Should transfer octave to toolbox?
////
////                //check if the note is added to form a chord and determine base note
////                ImoNote* pBaseOfChord = (ImoNote*)nullptr;
////                if (event.AltDown())
////                {
////                    lmStaffObj* pSO = m_pDoc->GetScore()->GetCursor()->GetStaffObj();
////                    if (pSO && pSO->IsNote())
////                        pBaseOfChord = (ImoNote*)pSO;
////                }
////
////                //do insert note
////                InsertNote(lm_ePitchRelative, nStep, m_nOctave, m_nSelNoteType, rDuration,
////                           m_nSelDots, m_nSelNotehead, m_nSelAcc, m_nSelVoice, pBaseOfChord,
////                           fTiedPrev, lmSTEM_DEFAULT);
//
//                ScoreCursor* pCursor = static_cast<ScoreCursor*>( m_cursor->get_inner_cursor() );
//                m_executer.insert_note(step, octave, m_toolsInfo.acc, m_toolsInfo.noteType,
//                                       m_toolsInfo.dots, m_toolsInfo.voice, pCursor->staff());
//                m_fEventProcessed = true;
//            }
//
//            //insert rest
//            else if (nKeyCode == int(' '))
//            {
//                //do insert rest
//                ScoreCursor* pCursor = static_cast<ScoreCursor*>( m_cursor->get_inner_cursor() );
//                m_executer.insert_rest(m_toolsInfo.noteType, m_toolsInfo.dots,
//                                       m_toolsInfo.voice, pCursor->staff());
//                m_fEventProcessed = true;
//            }
//
////            //commands to change options in Tool Box
////
////
////
////
////
////                //commands requiring to have a note/rest selected
////
////                ////change selected note pitch
////                //case WXK_UP:
////                //	if (nAuxKeys==0)
////                //		ChangeNotePitch(1);		//step up
////                //	else if (nAuxKeys && lmKEY_SHIFT)
////                //		ChangeNotePitch(7);		//octave up
////                //	else
////                //		fUnknown = true;
////                //	break;
////
////                //case WXK_DOWN:
////                //	if (nAuxKeys==0)
////                //		ChangeNotePitch(-1);		//step down
////                //	else if (nAuxKeys && lmKEY_SHIFT)
////                //		ChangeNotePitch(-7);		//octave down
////                //	else
////                //		fUnknown = true;
////                //	break;
////
////
////               // //invalid key
////               // default:
////                  //  fUnknown = true;
////            //}
////
////            //save char if unused
////            if (fUnknown && wxIsprint(nKeyCode))
////                m_sCmd += wxString::Format("%c", (char)nKeyCode);
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::add_note(string step)
{
    int octave = m_toolsInfo.octave;
    ScoreCursor* pCursor = static_cast<ScoreCursor*>( m_cursor->get_inner_cursor() );
    m_executer.insert_note(step, octave, m_toolsInfo.acc, m_toolsInfo.noteType,
                           m_toolsInfo.dots, m_toolsInfo.voice, pCursor->staff());
    m_fEventProcessed = true;
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::add_chord_note(string step)
{
    int octave = m_toolsInfo.octave;
    m_executer.add_chord_note(step, octave);
    m_fEventProcessed = true;
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::ask_and_add_clef()
{
    DlgClefs dlg(m_appScope, m_pController);
    if (dlg.ShowModal() == wxID_OK)
    {
        int clef = dlg.get_selected_button();
        ScoreCursor* pCursor = static_cast<ScoreCursor*>( m_cursor->get_inner_cursor() );
        m_executer.insert_clef(clef, pCursor->staff());
        m_fEventProcessed = true;
    }
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::process_on_click_event(SpEventMouse event)
{
    m_fEventProcessed = false;
    if (m_pController->is_edition_enabled())
    {
        if (m_toolsInfo.is_mouse_data_entry_mode())
        {
            //TODO: Add code to deal with dragging:
            //      Following code assumes a click for inserting something, but old
            //      code also deals with dragging, changing mouse pointer and other issues.
            //      See:
            //      CommandEventHandler::OnMouseEventToolMode(wxMouseEvent& event, wxDC* pDC)

            ClickHandler handler(m_pController, m_toolsInfo, m_selection, m_cursor);
            handler.process_click(event);

#if (LENMUS_DEBUG_BUILD == 1)
            if (!handler.event_processed())
                wxMessageBox("[CommandEventHandler::process_on_click_event] Click event not processed.");
#endif
        }
        else
        {
            //select object or move caret to click point
            unsigned flags = event->get_flags();
            ImoObj* pImo = event->get_imo_object();
            if (pImo->is_staffobj())
            {
                bool fAddToSeleccion = flags & k_kbd_ctrl;
                m_executer.select_object(pImo->get_id(), !fAddToSeleccion);
            }
            else
                m_executer.move_caret_to_click_point(event);
        }
        m_fEventProcessed = true;
    }
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::process_action_event(SpEventAction event)
{
    m_fEventProcessed = false;
    if (m_pController->is_edition_enabled())
    {
        if (event->is_control_point_moved_event())
        {
            SpEventControlPointMoved pEv(
                    static_pointer_cast<EventControlPointMoved>(event) );
            UPoint shift = pEv->get_shift();
            int iPoint = pEv->get_handler_index();
//            int gmoType = pEv->get_gmo_type();
//            ShapeId idx = pEv->get_shape_index();
            m_executer.move_object_point(iPoint, shift);
            m_fEventProcessed = true;
        }
    }
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::move_caret_to_click_point(SpEventMouse event)
{
    SpInteractor spIntor = m_pController->get_interactor_shared_ptr();
    DocCursorState state = spIntor->click_event_to_cursor_state(event);
    if (state.get_parent_level_id() != k_no_imoid)
        m_pController->exec_lomse_command(
            LENMUS_NEW CmdCursor(state), k_show_busy);
}

////---------------------------------------------------------------------------------------
//ClickHandler* CommandEventHandler::new_click_handler_for_current_context()
//{
//    //TODO: Redesign. Is context needed?
//    //factory method for generating a mouse click event handler
//
////    ImoObj* pImo = m_cursor->get_parent_object();
////    if (m_cursor->is_inside_terminal_node() && pImo->is_score())
////    {
////        switch(m_toolsInfo.pageID)
////        {
////            case k_page_notes:
////                return LENMUS_NEW NoteRestClickHandler(m_pController, m_toolsInfo,
////                                                    m_selection, m_cursor);
////
////    //        case k_page_sysmbols:
////    //        {
////    //            switch (groupID)
////    //            {
////    //                case k_grp_Symbols:
////    //                    return OnToolSymbolsClick(pGMO, uPagePos, rGridTime);
////    //                case k_grp_Harmony:
////    //                    return OnToolHarmonyClick(pGMO, uPagePos, rGridTime);
////    //                default:
////    //                    wxLogMessage("[CommandEventHandler::OnToolClick] Missing value (%d) in switch statement", groupID);
////    //                    return;
////    //            }
////    //        }
////
////            case k_page_barlines:
////                return LENMUS_NEW BarlineClickHandler(m_pController, m_toolsInfo,
////                                                      m_selection, m_cursor);
////
////            case k_page_clefs:
////            {
////                switch (m_toolsInfo.groupID)
////                {
////                    case k_grp_ClefType:
////                        return LENMUS_NEW ClefClickHandler(m_pController, m_toolsInfo,
////                                                           m_selection, m_cursor);
////    //                case k_grp_TimeType:
////    //                    return OnToolTimeSignatureClick(pGMO, uPagePos, rGridTime);
////    //                case k_grp_KeyType:
////    //                    return OnToolKeySignatureClick(pGMO, uPagePos, rGridTime);
////                    default:
////                        stringstream msg;
////                        msg << "Missing value (" << m_toolsInfo.groupID
////                            << ") in switch statement.";
////                        LOMSE_LOG_ERROR(msg.str());
////                        return LENMUS_NEW NullClickHandler(m_pController, m_toolsInfo,
////                                                           m_selection, m_cursor);
////                }
////            }
////
////            default:
////                return LENMUS_NEW NullClickHandler(m_pController, m_toolsInfo,
////                                                   m_selection, m_cursor);
////        }
////    }
////    else
//        return LENMUS_NEW NullClickHandler(m_pController, m_toolsInfo,
//                                           m_selection, m_cursor);
//}

//---------------------------------------------------------------------------------------
void CommandEventHandler::process_page_changed_in_toolbox_event(ToolBox* pToolBox)
{
    common_tasks_for_toolbox_event(pToolBox);
    m_fEventProcessed = true;
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::process_tool_event(EToolID toolID, ToolBox* pToolBox)
{
    m_fEventProcessed = false;
    common_tasks_for_toolbox_event(pToolBox);
    if (toolID == k_tool_mouse_mode)
    {
        switch_interactor_mode_for_current_mouse_mode();
        set_drag_image_for_tool(toolID);
    }
    else
    {
        set_drag_image_for_tool(toolID);
        if (!m_selection->empty())
            command_on_selection(toolID);
        else
        {
            command_on_caret_pointed_object(toolID);
            if (!m_fEventProcessed && toolID == k_tool_voice)
            {
                SpInteractor spInteractor = m_pController->get_interactor_shared_ptr();
                //spInteractor->highlight_voice( m_toolsInfo.voice );
                spInteractor->select_voice( m_toolsInfo.voice );
                spInteractor->force_redraw();
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::common_tasks_for_toolbox_event(ToolBox* pToolBox)
{
//    SpInteractor spInteractor = m_pController->get_interactor_shared_ptr();
    m_toolsInfo.update_toolbox_info(pToolBox);
    pToolBox->synchronize_tools(m_selection, m_cursor);

//    //determine valid areas and change icons
//    UpdateValidAreasAndMouseIcons();
//
//    //update status bar: mouse mode and selected tool
//    UpdateStatusBarToolBox(); --> update_status_bar_toolbox_data();
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::command_on_caret_pointed_object(EToolID toolID)
{
    switch(toolID)
    {
        case k_tool_beams_cut:
        {
            m_executer.break_beam();
            m_fEventProcessed = true;
            return;
        }

        default:
            return;
    }
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::command_on_selection(EToolID toolID)
{
    switch (toolID)
    {
        case k_tool_accidentals:
        {
            EAccidentals nAcc = m_toolsInfo.acc;
            m_executer.change_note_accidentals(nAcc);
            m_fEventProcessed = true;
            return;
        }

        case k_tool_dots:
        {
            m_executer.change_dots(m_toolsInfo.dots);
            m_fEventProcessed = true;
            return;
        }

        case k_tool_note_tie:
        {
            ImoNote* pStartNote;
            ImoNote* pEndNote;
            if (m_selection->is_valid_to_add_tie(&pStartNote, &pEndNote))
            {
                m_executer.add_tie();
                m_fEventProcessed = true;
            }
            else if (m_selection->is_valid_to_remove_tie())
            {
                m_executer.delete_tie();
                m_fEventProcessed = true;
            }
            return;
        }

        case k_tool_note_tuplet:
        {
            if (m_selection->is_valid_to_add_tuplet())
            {
                m_executer.add_tuplet();
                m_fEventProcessed = true;
            }
            else if (m_selection->is_valid_to_remove_tuplet())
            {
                m_executer.delete_tuplet();
                m_fEventProcessed = true;
            }
            return;
        }

        case k_tool_note_toggle_stem:
        {
            m_executer.toggle_stem();
            m_fEventProcessed = true;
            return;
        }

        case k_tool_beams_join:
        {
            m_executer.join_beam();
            m_fEventProcessed = true;
            return;
        }

        case k_tool_beams_flatten:
        case k_tool_beams_subgroup:
        {
            //TODO
            return;
        }

        default:
            return;
    }
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::switch_interactor_mode_for_current_mouse_mode()
{
    SpInteractor spInteractor = m_pController->get_interactor_shared_ptr();
    switch(m_toolsInfo.mouseMode)
    {
        case k_mouse_mode_data_entry:
            spInteractor->switch_task(TaskFactory::k_task_data_entry);
            break;

        case k_mouse_mode_pointer:
        default:
            spInteractor->switch_task(TaskFactory::k_task_selection);
    }
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::set_drag_image_for_tool(EToolID toolID)
{
    SpInteractor spInteractor = m_pController->get_interactor_shared_ptr();

    ImoObj* pImo = m_cursor->get_parent_object();
    if (m_cursor->is_inside_terminal_node() && pImo->is_score()
        && m_toolsInfo.is_mouse_data_entry_mode())
    {
        LibraryScope& libScope = m_pController->get_library_scope();

        GmoShape* pShape = nullptr;
        UPoint offset(0.0, 0.0);
        switch(toolID)
        {
            case k_tool_clef:
            {
                ClefEngraver engraver(libScope);
                int clefType = int(m_toolsInfo.clefType);
                pShape = engraver.create_tool_dragged_shape(clefType);
                offset = engraver.get_drag_offset();
                m_toolsInfo.clickCmd = k_cmd_clef;
                break;
            }

            case k_tool_note_duration:
            case k_tool_note_or_rest:
            case k_tool_accidentals:
            case k_tool_octave:
            case k_tool_voice:
            {
                ImoScore* pScore = static_cast<ImoScore*>(pImo);
                ScoreMeter scoreMeter(pScore);
                int noteType = int(m_toolsInfo.noteType);
                int dots = m_toolsInfo.dots;
                EAccidentals acc = m_toolsInfo.acc;
                if (m_toolsInfo.fIsNote)
                {
                    NoteEngraver engraver(libScope, &scoreMeter, nullptr, 0, 0);
                    pShape = engraver.create_tool_dragged_shape(noteType, acc, dots);
                    offset = engraver.get_drag_offset();
                    m_toolsInfo.clickCmd = k_cmd_note;
                }
                else
                {
                    RestEngraver engraver(libScope, &scoreMeter, nullptr, 0, 0);
                    pShape = engraver.create_tool_dragged_shape(noteType, dots);
                    offset = engraver.get_drag_offset();
                    m_toolsInfo.clickCmd = k_cmd_rest;
                }
                break;
            }

//            case k_tool_symbols:
//            {
//                //TODO: create drag shape
//                break;
//            }

            case k_tool_barline:
            {
                BarlineEngraver engraver(libScope);
                int barlineType = int(m_toolsInfo.barlineType);
                pShape = engraver.create_tool_dragged_shape(barlineType);
                offset = engraver.get_drag_offset();
                m_toolsInfo.clickCmd = k_cmd_barline;
                break;
            }

            default:
                m_toolsInfo.clickCmd = k_cmd_null;
                ;   //TODO: Create questión mark shape
        }
        spInteractor->set_drag_image(pShape, k_get_ownership, offset);
        spInteractor->show_drag_image(true);
    }
    else
    {
        spInteractor->set_drag_image(nullptr, k_get_ownership, UPoint(0.0, 0.0));
        spInteractor->show_drag_image(false);
    }
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::delete_selection_or_pointed_object()
{
    if (!m_selection->empty())
        m_executer.delete_selection();
    else
    {
        if (!m_cursor->is_inside_terminal_node())
            m_pController->exec_command("d ");      //delete top level
        else
            m_executer.delete_staffobj();
    }
    m_fEventProcessed = true;
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::check_always_valid_edition_commands()
{
    //TODO: how to validate commands valid only for scores (or other top levels?):
    //      - different contexts?
//    k_cmd_select_to_start_of_measure,
//    k_cmd_select_to_end_of_measure,
//    k_cmd_select_to_start_of_system,
//    k_cmd_select_to_end_of_system,

    //TODO: This was just a test. Now implement better ----------------------------------
    static DocCursor m_selectCursor(m_cursor->get_document());
    static bool m_fLastOpWasSelect = false;
    //End of declarations for test code -------------------------------------------------

    //commands only valid if document edition is enabled
    if (m_pController->is_edition_enabled())
    {
        KeyTranslator* pTr = m_appScope.get_key_translator();
        m_keyCmd = pTr->translate(k_key_context_edition_any, m_key, m_keyFlags);

        //TODO: This was just a test. Now implement better ------------------------------
        bool fIsSelectCmd = m_keyCmd == k_cmd_select_prev_note
                            || m_keyCmd == k_cmd_select_next_note
                            || m_keyCmd == k_cmd_select_to_start_of_measure
                            || m_keyCmd == k_cmd_select_to_end_of_measure
                            || m_keyCmd == k_cmd_select_to_start_of_system
                            || m_keyCmd == k_cmd_select_to_end_of_system;

        if (!m_fLastOpWasSelect && fIsSelectCmd)
            m_selectCursor.point_to( m_cursor->get_pointee_id() );

        m_fLastOpWasSelect = fIsSelectCmd;
        //End of test code --------------------------------------------------------------


        switch (m_keyCmd)
        {
            //cursor keys
            case k_cmd_cursor_move_prev:
                m_pController->exec_lomse_command(
                    LENMUS_NEW CmdCursor(CmdCursor::k_move_prev), k_no_show_busy);
                m_fEventProcessed = true;
                return;

            case k_cmd_cursor_move_next:
                m_pController->exec_lomse_command(
                    LENMUS_NEW CmdCursor(CmdCursor::k_move_next), k_no_show_busy);
                m_fEventProcessed = true;
                return;

            case k_cmd_cursor_exit:
                m_pController->exec_lomse_command(
                    LENMUS_NEW CmdCursor(CmdCursor::k_exit), k_no_show_busy);
                m_fEventProcessed = true;
                return;

            case k_cmd_cursor_enter:
                enter_top_level_and_edit();
                m_fEventProcessed = true;
                return;

            case k_cmd_cursor_to_next_measure:
            case k_cmd_cursor_to_prev_measure:
            case k_cmd_cursor_to_first_measure:
            case k_cmd_cursor_to_last_measure:
            {
                ImoObj* pImo = m_cursor->get_parent_object();
                if (pImo && pImo->is_score())
                {
                    ScoreCursor* pSC =
                        static_cast<ScoreCursor*>(m_cursor->get_inner_cursor());
                    int measure = pSC->measure();
                    if (m_keyCmd == k_cmd_cursor_to_next_measure)
                        ++measure;
                    else if (m_keyCmd == k_cmd_cursor_to_prev_measure)
                    {
                        if (measure > 0) --measure;
                    }
                    else if (m_keyCmd == k_cmd_cursor_to_first_measure)
                        measure = 0;
                    else if (m_keyCmd == k_cmd_cursor_to_last_measure)
                        measure = 9999999;
                    else
                    {
                    }
                    m_pController->exec_lomse_command(
                        LENMUS_NEW CmdCursor(measure, -1, -1), k_no_show_busy);
                    m_fEventProcessed = true;
                }
                return;
            }

            case k_cmd_cursor_to_start_of_system:
            case k_cmd_cursor_to_end_of_system:
            case k_cmd_cursor_to_next_page:
            case k_cmd_cursor_to_prev_page:
            case k_cmd_cursor_to_first_staff:
            case k_cmd_cursor_to_last_staff:
                //TODO
                m_fEventProcessed = false;
                return;

            case k_cmd_cursor_move_up:
            case k_cmd_cursor_move_down:
                move_cursor_up_down();
                m_fEventProcessed = true;
                return;

            case k_cmd_delete_selection_or_pointed_object:
                delete_selection_or_pointed_object();
                return;

            case k_cmd_move_prev_and_delete_pointed_object:
                m_pController->exec_lomse_command(
                    LENMUS_NEW CmdCursor(CmdCursor::k_move_prev), k_no_show_busy);
                delete_selection_or_pointed_object();
                return;

            //selection commands
            case k_cmd_remove_selection:
                m_pController->exec_lomse_command(
                    LENMUS_NEW CmdSelection(CmdSelection::k_clear), k_no_show_busy);
                m_fEventProcessed = true;
                return;

            case k_cmd_select_prev_note:
            {
                m_selectCursor.move_prev();
                ImoId id = m_selectCursor.get_pointee_id();
                int op = m_selection->contains(id) ? CmdSelection::k_remove
                                                   : CmdSelection::k_add;
                m_pController->exec_lomse_command(
                    LENMUS_NEW CmdSelection(op, id), k_no_show_busy);
                m_fEventProcessed = true;
                return;
            }

            case k_cmd_select_next_note:
            {
                ImoId id = m_selectCursor.get_pointee_id();
                m_selectCursor.move_next();
                int op = m_selection->contains(id) ? CmdSelection::k_remove
                                                   : CmdSelection::k_add;
                m_pController->exec_lomse_command(
                    LENMUS_NEW CmdSelection(op, id), k_no_show_busy);
                m_fEventProcessed = true;
                return;
            }

            default:
                m_fEventProcessed = false;
        }
    }
}


////---------------------------------------------------------------------------------------
//void CommandEventHandler::LogKeyEvent(wxString name, wxKeyEvent& event, int nTool)
//{
//    wxString key = KeyCodeToName( event.GetKeyCode() );
//    key += wxString::Format(" (Unicode: %#04x)", event.GetUnicodeKey());
//
//    wxLogMessage( wxString::Format( "[CommandEventHandler::LogKeyEvent] Event: %s - %s, nKeyCode=%d, (flags = %c%c%c%c). Tool=%d",
//            name.wx_str(), key.wx_str(), event.GetKeyCode(),
//            (event.CmdDown() ? _T('C') : _T('-') ),
//            (event.AltDown() ? _T('A') : _T('-') ),
//            (event.ShiftDown() ? _T('S') : _T('-') ),
//            (event.MetaDown() ? _T('M') : _T('-') ),
//            nTool ));
//}
//

//---------------------------------------------------------------------------------------
void CommandEventHandler::enter_top_level_and_edit()
{
    if (!m_cursor->is_inside_terminal_node())
    {
        ImoObj* pImo = m_cursor->get_pointee();
        if (pImo)
        {
            m_pController->edit_top_level(pImo->get_obj_type());
    //        if (pImo->is_score())
    //            m_pController->exec_lomse_command(
    //                LENMUS_NEW CmdCursor(CmdCursor::k_enter), k_no_show_busy);
    //        else if (pImo->is_paragraph())
    //        {
    //            m_pController->edit_top_level(k_imo_para);
    //        }
        }
    }
}

//---------------------------------------------------------------------------------------
void CommandEventHandler::move_cursor_up_down()
{
//    ImoObj* pImo = m_cursor->get_parent_object();
//    if (!pImo || !pImo->is_score())
//        return;

    //TODO: treatment for other top level objects different from scores

//        ScoreCursor* pSC =
//            static_cast<ScoreCursor*>(m_cursor->get_inner_cursor());
//        if (instr==0 && staff==0 && not (note_in_chord or last_note_in_chord)
//        {
//            determine timepos on previous system()
//            to_time()
//        }
//        else
//            move_up()
//
    if (m_keyCmd == k_cmd_cursor_move_up)
        m_pController->exec_lomse_command(
            LENMUS_NEW CmdCursor(CmdCursor::k_move_up), k_no_show_busy);
    else
        m_pController->exec_lomse_command(
            LENMUS_NEW CmdCursor(CmdCursor::k_move_down), k_no_show_busy);
}


//=======================================================================================
// CommandGenerator implementation
//=======================================================================================
CommandGenerator::CommandGenerator(DocumentWindow* pController, SelectionSet* selection,
                                   DocCursor* cursor)
    : m_pController(pController)
    , m_selection(selection)
    , m_cursor(cursor)
{
}

//---------------------------------------------------------------------------------------
void CommandGenerator::add_chord_note(const string& stepLetter, int octave)
{
    //Add note to chord

    stringstream src;
    src << stepLetter << octave;
    string pitch = src.str();
	string name = to_std_string(_("Add chord note")) + "(" + pitch + ")";
	m_pController->exec_lomse_command( LENMUS_NEW CmdAddChordNote(pitch, name) );
}

//---------------------------------------------------------------------------------------
void CommandGenerator::add_tie()
{
    //Tie the selected notes

	string name = to_std_string(_("Add tie"));
	m_pController->exec_lomse_command( LENMUS_NEW CmdAddTie(name) );
}

////---------------------------------------------------------------------------------------
//void CommandGenerator::AddTitle()
//{
//    //Create a new block of text and attach it to the score
//
//    //create the new text.
//    //Text creation requires to create an empty TextItem and editing it using the properties
//    //dialog. And this, in turn, requires the TextItem to edit to be already included in the
//    //score. Therefore, I will attach it provisionally to the score
//
//    lmScore* pScore = m_pDoc->GetScore();
//    lmTextStyle* pStyle = pScore->GetStyleInfo(_("Title"));
//    wxASSERT(pStyle);
//    wxString sTitle = "";
//    lmScoreTitle* pNewTitle
//        = new lmScoreTitle(pScore, lmNEW_ID, sTitle, lmBLOCK_ALIGN_BOTH,
//                           lmHALIGN_DEFAULT, lmVALIGN_DEFAULT, pStyle);
//	pScore->AttachAuxObj(pNewTitle);
//
//    //show dialog to create the text
//	DlgProperties dlg((DocumentWindow*)nullptr);
//	pNewTitle->OnEditProperties(&dlg);
//	dlg.Layout();
//	if (dlg.ShowModal() == wxID_OK)
//        pScore->OnPropertiesChanged();
//
//    //get title info
//    sTitle = pNewTitle->GetText();
//    pStyle = pNewTitle->GetStyle();
//    lmEHAlign nAlign = pNewTitle->GetAlignment();
//
//	//dettach the text from the score and delete the text item
//	pScore->DetachAuxObj(pNewTitle);
//    delete pNewTitle;
//
//    //Now issue the command to attach the title to to the score
//	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//    if (sTitle != "")
//	    m_pController->exec_lomse_command(LENMUS_NEW CmdAddTitle(lmCMD_NORMAL, m_pDoc, sTitle, pStyle,
//                                      nAlign));
//}

//---------------------------------------------------------------------------------------
void CommandGenerator::add_tuplet()
{
    // Add a tuplet to the selected notes/rests (there could be other objects selected
    // beetween the notes)
    //
    // Precondition:
    //      it has been checked that all notes/rest in the seleccion are not in a tuplet,
    //      are consecutive and are in the same voice.

    ImoNoteRest* pStart = nullptr;
    ImoNoteRest* pEnd = nullptr;
	m_selection->get_start_end_note_rests(&pStart, &pEnd);
	if (pStart && pEnd)
	{
        string name = to_std_string(_("Add tuplet"));
        m_pController->exec_lomse_command(
            LENMUS_NEW CmdAddTuplet("(t + 2 3)", name) );
	}
}

////---------------------------------------------------------------------------------------
//void CommandGenerator::AttachNewText(lmComponentObj* pTarget)
//{
//    //Create a new text and attach it to the received object
//
//    //create the new text.
//    //Text creation requires to create an empty TextItem and editing it using the properties
//    //dialog. And this, in turn, requires the TextItem to edit to be already included in the
//    //score. Therefore, I will attach it provisionally to the score
//
//    lmScore* pScore = m_pDoc->GetScore();
//    lmTextStyle* pStyle = pScore->GetStyleInfo(_("Normal text"));
//    wxASSERT(pStyle);
//    wxString sText = "";
//    lmTextItem* pNewText = new lmTextItem(pScore, lmNEW_ID, sText, lmHALIGN_DEFAULT, pStyle);
//	pScore->AttachAuxObj(pNewText);
//
//    //show dialog to edit the text
//	DlgProperties dlg((DocumentWindow*)nullptr);
//	pNewText->OnEditProperties(&dlg);
//	dlg.Layout();
//	dlg.ShowModal();
//
//    //get text info
//    sText = pNewText->GetText();
//    pStyle = pNewText->GetStyle();
//    lmEHAlign nAlign = pNewText->GetAlignment();
//
//	//dettach the text from the score and delete the text item
//	pScore->DetachAuxObj(pNewText);
//    delete pNewText;
//
//    //Now issue the command to attach the text to the received target object
//	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//    if (sText != "")
//	    m_pController->exec_lomse_command(LENMUS_NEW CmdAttachText(lmCMD_NORMAL, m_pDoc, sText, pStyle,
//                                        nAlign, pTarget));
//}

//---------------------------------------------------------------------------------------
void CommandGenerator::break_beam()
{
    //Break beamed group at selected note (the one pointed by cursor)

    ImoNoteRest* pNR = dynamic_cast<ImoNoteRest*>( m_cursor->get_pointee() );
    if (pNR)
    {
        string name = to_std_string(_("Break a beam"));
        m_pController->exec_lomse_command( LENMUS_NEW CmdBreakBeam(name) );
    }
}

//---------------------------------------------------------------------------------------
void CommandGenerator::change_attribute(ImoObj* pImo, int attrb, int newValue)
{
	string name = to_std_string(_("Change numeric property"));
    m_pController->exec_lomse_command(
        LENMUS_NEW CmdChangeAttribute(pImo, EImoAttribute(attrb), newValue) );
}

//---------------------------------------------------------------------------------------
void CommandGenerator::change_attribute(ImoObj* pImo, int attrb, Color newValue)
{
	string name = to_std_string(_("Change color property"));
    m_pController->exec_lomse_command(
        LENMUS_NEW CmdChangeAttribute(pImo, EImoAttribute(attrb), newValue) );
}

//---------------------------------------------------------------------------------------
void CommandGenerator::change_attribute(ImoObj* pImo, int attrb, double newValue)
{
	string name = to_std_string(_("Change numeric property"));
    m_pController->exec_lomse_command(
        LENMUS_NEW CmdChangeAttribute(pImo, EImoAttribute(attrb), newValue) );
}

//---------------------------------------------------------------------------------------
void CommandGenerator::change_attribute(ImoObj* pImo, int attrb, const string& newValue)
{
	string name = to_std_string(_("Change string property"));
    m_pController->exec_lomse_command(
        LENMUS_NEW CmdChangeAttribute(pImo, EImoAttribute(attrb), newValue) );
}

////---------------------------------------------------------------------------------------
//void CommandGenerator::ChangeBarline(lmBarline* pBL, lmEBarline nType, bool fVisible)
//{
//    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	m_pController->exec_lomse_command(LENMUS_NEW CmdChangeBarline(lmCMD_NORMAL, m_pDoc, pBL, nType, fVisible) );
//}

//---------------------------------------------------------------------------------------
void CommandGenerator::change_dots(int dots)
{
	//change dots for current selected notes/rests

	string name = to_std_string(_("Change notes/rests dots"));
    m_pController->exec_lomse_command(
        LENMUS_NEW CmdChangeDots(dots, name) );
}

////---------------------------------------------------------------------------------------
//void CommandGenerator::ChangeFiguredBass(lmFiguredBass* pFB, wxString& sFigBass)
//{
//    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	m_pController->exec_lomse_command(LENMUS_NEW CmdChangeFiguredBass(lmCMD_NORMAL, m_pDoc, pFB, sFigBass) );
//}

////---------------------------------------------------------------------------------------
//void CommandGenerator::ChangeMidiSettings(lmInstrument* pInstr, int nMidiChannel,
//                                       int nMidiInstr)
//{
//    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	m_pController->exec_lomse_command(LENMUS_NEW CmdChangeMidiSettings(lmCMD_NORMAL, m_pDoc, pInstr, nMidiChannel,
//                                            nMidiInstr) );
//}

//---------------------------------------------------------------------------------------
void CommandGenerator::change_note_accidentals(EAccidentals acc)
{
	//change note accidentals for current selected notes

	string name = to_std_string(_("Change note accidentals"));
    m_pController->exec_lomse_command(
        LENMUS_NEW CmdChangeAccidentals(acc, name));
}

////---------------------------------------------------------------------------------------
//void CommandGenerator::ChangeNotePitch(int nSteps)
//{
//	//change pith of note at current cursor position
//    lmScoreCursor* pCursor = m_pDoc->GetScore()->GetCursor();
//	wxASSERT(pCursor);
//    lmStaffObj* pCursorSO = pCursor->GetStaffObj();
//	wxASSERT(pCursorSO);
//	wxASSERT(pCursorSO->IsNote());
//    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	string name = to_std_string(_("Change note pitch"));
//	m_pController->exec_lomse_command(LENMUS_NEW CmdChangeNotePitch(lmCMD_NORMAL, sName, m_pDoc,
//                                         (ImoNote*)pCursorSO, nSteps) );
//}
//
////---------------------------------------------------------------------------------------
//void CommandGenerator::ChangePageMargin(GmoObj* pGMO, int nIdx, int nPage, lmLUnits uPos)
//{
//	//Updates the position of a margin
//
//	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	string name = to_std_string(_("Change margin"));
//	m_pController->exec_lomse_command(LENMUS_NEW CmdChangePageMargin(lmCMD_NORMAL, name, m_pDoc,
//                                          pGMO, nIdx, nPage, uPos));
//}

////---------------------------------------------------------------------------------------
//void CommandGenerator::ChangeText(lmScoreText* pST, wxString sText, lmEHAlign nAlign,
//                               lmLocation tPos, lmTextStyle* pStyle, int nHintOptions)
//{
//	//change properties of a lmTextItem object
//
//    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	string name = to_std_string(_("Change text"));
//	m_pController->exec_lomse_command(LENMUS_NEW CmdChangeText(lmCMD_NORMAL, sName, m_pDoc, pST, sText,
//                                    nAlign, tPos, pStyle, nHintOptions) );
//}

//---------------------------------------------------------------------------------------
void CommandGenerator::delete_selection()
{
    //Delete all objects in the selection.

    list<ImoObj*>& objects = m_selection->get_all_objects();

    //if no object, ignore command
    if (objects.size() > 0)
    {
        string name = to_std_string(_("Delete selection"));
        m_pController->exec_lomse_command( LENMUS_NEW CmdDeleteSelection(name) );
    }
}

//---------------------------------------------------------------------------------------
void CommandGenerator::delete_staffobj()
{
	//delete the StaffObj at current caret position

	//get object pointed by the cursor
    ImoStaffObj* pSO = dynamic_cast<ImoStaffObj*>( m_cursor->get_pointee() );

    //if no object, ignore command. i.e. user clicking 'Del' key on no object
    if (pSO)
    {
        string name = to_std_string(
            wxString::Format(_("Delete %s"), to_wx_string(pSO->get_name()).wx_str() ));
        m_pController->exec_lomse_command( LENMUS_NEW CmdDeleteStaffObj(name) );
    }
}

//---------------------------------------------------------------------------------------
void CommandGenerator::delete_tie()
{
    //remove tie between the selected notes

	string name = to_std_string(_("Delete tie"));
    m_pController->exec_lomse_command( LENMUS_NEW CmdDeleteRelation(k_tie, name) );
}

//---------------------------------------------------------------------------------------
void CommandGenerator::delete_tuplet()
{
    // Remove all selected tuplet

    string name = to_std_string(_("Delete tuplet"));
    m_pController->exec_lomse_command(
        LENMUS_NEW CmdDeleteRelation(k_imo_tuplet, name) );
}

//---------------------------------------------------------------------------------------
void CommandGenerator::insert_barline(int barlineType)
{
	//insert a barline at current cursor position

    stringstream src;
    src << "(barline "
        << LdpExporter::barline_type_to_ldp(barlineType)
        << ")";
    string name = to_std_string(_("Insert barline"));
    insert_staffobj(src.str(), name);
}

//---------------------------------------------------------------------------------------
void CommandGenerator::insert_clef(int clefType, int staff)
{
	//insert a Clef at current cursor position

    stringstream src;
    src << "(clef "
        << LdpExporter::clef_type_to_ldp(clefType)
        << " p"
        << staff+1
        << ")";
    string name = to_std_string(_("Insert clef"));
    insert_staffobj(src.str(), name);
}

////---------------------------------------------------------------------------------------
//void CommandGenerator::InsertFiguredBass()
//{
//    //Create a new figured bass and add it to the VStaff
//
//    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	wxString sFigBass = "5 3";
//	m_pController->exec_lomse_command(LENMUS_NEW CmdInsertFiguredBass(lmCMD_NORMAL, m_pDoc, sFigBass) );
//}

////---------------------------------------------------------------------------------------
//void CommandGenerator::InsertFiguredBassLine()
//{
//    //Create a new figured bass line and add it to the VStaff
//
//    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	m_pController->exec_lomse_command(LENMUS_NEW CmdInsertFBLine(lmCMD_NORMAL, m_pDoc) );
//}

////---------------------------------------------------------------------------------------
//void CommandGenerator::InsertKeySignature(int nFifths, bool fMajor, bool fVisible)
//{
//    //insert a key signature at current cursor position
//
//    //wxLogMessage("[CommandGenerator::InsertKeySignature] fifths=%d, %s",
//    //             nFifths, (fMajor ? "major" : "minor") );
//
//    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	string name = to_std_string(_("Insert key signature"));
//	m_pController->exec_lomse_command(LENMUS_NEW CmdInsertKeySignature(lmCMD_NORMAL, sName, m_pDoc, nFifths,
//                                            fMajor, fVisible) );
//}

//---------------------------------------------------------------------------------------
void CommandGenerator::insert_note(string stepLetter, int octave, EAccidentals acc,
                                   ENoteType noteType, int dots,
                                   int voice, int staff)
{
	//insert a note at current cursor position

    stringstream src;
    src << "(n ";
    if (acc != k_no_accidentals)
        src << LdpExporter::accidentals_to_string(acc);
    src << stepLetter << octave
        << " " << LdpExporter::notetype_to_string(noteType, dots)
        << " v" << voice
        << " p" << staff+1
        << ")";
    string name = to_std_string(_("Insert note"));
//    insert_staffobj(src.str(), name);

    int editMode = k_edit_mode_replace;     //TODO: user selectable
    m_pController->exec_lomse_command(
                        LENMUS_NEW CmdAddNoteRest(src.str(), editMode, name) );
}

//---------------------------------------------------------------------------------------
void CommandGenerator::insert_rest(ENoteType noteType, int dots, int voice, int staff)
{
	//insert a rest at current cursor position

    stringstream src;
    src << "(r "
        " " << LdpExporter::notetype_to_string(noteType, dots)
        << " v" << voice
        << " p" << staff+1
        << ")";
    string name = to_std_string(_("Insert rest"));
    insert_staffobj(src.str(), name);
}

//---------------------------------------------------------------------------------------
void CommandGenerator::insert_staffobj(string ldpSrc, string name)
{
	//insert an staffobj at current cursor position

    m_pController->exec_lomse_command( LENMUS_NEW CmdInsertStaffObj(ldpSrc, name) );
}

////---------------------------------------------------------------------------------------
//void CommandGenerator::InsertTimeSignature(int nBeats, int nBeatType, bool fVisible)
//{
//    //insert a time signature at current cursor position
//
//    //wxLogMessage("[CommandGenerator::InsertTimeSignature] nBeats=%d, nBeatType=%d", nBeats, nBeatType);
//
//    wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	string name = to_std_string(_("Insert time signature"));
//	m_pController->exec_lomse_command(LENMUS_NEW CmdInsertTimeSignature(lmCMD_NORMAL, sName, m_pDoc,
//                                             nBeats, nBeatType, fVisible) );
//}

//---------------------------------------------------------------------------------------
void CommandGenerator::join_beam()
{
    //depending on current selection content, either:
    // - create a beamed group with the selected notes,
    // - join two or more beamed groups
    // - or add a note to a beamed group

	string name = to_std_string(_("Add beam"));
	m_pController->exec_lomse_command(LENMUS_NEW CmdJoinBeam(name) );
}

//---------------------------------------------------------------------------------------
void CommandGenerator::move_caret_to_click_point(SpEventMouse event)
{
    SpInteractor spIntor = m_pController->get_interactor_shared_ptr();
    DocCursorState state = spIntor->click_event_to_cursor_state(event);
    m_pController->exec_lomse_command(LENMUS_NEW CmdCursor(state),
                                      k_no_show_busy );
}

////---------------------------------------------------------------------------------------
//void CommandGenerator::MoveObject(GmoObj* pGMO, const UPoint& uPos)
//{
//	//Generate move command to move the lmComponentObj and update the document
//
//	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	wxString sName = wxString::Format(_("Move %s"), pGMO->GetName().wx_str() );
//	m_pController->exec_lomse_command(LENMUS_NEW CmdMoveObject(lmCMD_NORMAL, sName, m_pDoc, pGMO, uPos));
//}

//---------------------------------------------------------------------------------------
void CommandGenerator::move_object_point(int iPoint, UPoint shift)
{
	string name = to_std_string(_("Move control point"));
    m_pController->exec_lomse_command( LENMUS_NEW CmdMoveObjectPoint(iPoint, shift, name),
                                       k_show_busy );
}

////---------------------------------------------------------------------------------------
//void CommandGenerator::MoveNote(GmoObj* pGMO, const UPoint& uPos, int nSteps)
//{
//	//Generate move command to move the note and change its pitch
//
//	wxCommandProcessor* pCP = m_pDoc->GetCommandProcessor();
//	m_pController->exec_lomse_command(LENMUS_NEW CmdMoveNote(lmCMD_NORMAL, m_pDoc, (ImoNote*)pGMO->GetScoreOwner(), uPos, nSteps));
//}

//---------------------------------------------------------------------------------------
void CommandGenerator::select_object(ImoId id, bool fClearSelection)
{
	string name = to_std_string(_("Select object"));
    int op = fClearSelection ? CmdSelection::k_set : CmdSelection::k_add;
    m_pController->exec_lomse_command( LENMUS_NEW CmdSelection(op, id, name),
                                       k_show_busy );
}

//---------------------------------------------------------------------------------------
void CommandGenerator::toggle_stem()
{
    //toggle stem in all selected notes.

//TODO
//	m_pController->exec_lomse_command(LENMUS_NEW CmdToggleNoteStem(lmCMD_NORMAL, m_pDoc, m_pView->GetSelection()) );
}







////-------------------------------------------------------------------------------------
//// Mouse click on a valid area while dragging a tool
////-------------------------------------------------------------------------------------

//=======================================================================================
// ClickHandler implementation
//=======================================================================================
ClickHandler::ClickHandler(DocumentWindow* pController, ToolsInfo& toolsInfo,
                       SelectionSet* selection, DocCursor* cursor)
    : m_pController(pController)
    , m_toolsInfo(toolsInfo)
    , m_selection(selection)
    , m_cursor(cursor)
    , m_fEventProcessed(false)
    , m_executer(pController, selection, cursor)
{
}

//---------------------------------------------------------------------------------------
void ClickHandler::process_click(SpEventMouse event)
{
    switch(m_toolsInfo.clickCmd)
    {
        case k_cmd_barline:
            add_barline(event);
            return;

        case k_cmd_clef:
            add_clef(event);
            return;

        case k_cmd_note:
        case k_cmd_rest:
            add_note_rest(event);
            return;

        default:
            return;
    }
}

//---------------------------------------------------------------------------------------
void ClickHandler::add_barline(SpEventMouse event)
{
//    //click only valid if on staff
//    if (m_pCurShapeStaff)
    {
        m_executer.move_caret_to_click_point(event);
        m_executer.insert_barline(m_toolsInfo.barlineType);
        m_fEventProcessed = true;
    }
}

//---------------------------------------------------------------------------------------
void ClickHandler::add_clef(SpEventMouse event)
{
//    //click only valid if on staff
//    if (m_pCurShapeStaff)
    {
        m_executer.move_caret_to_click_point(event);
        ScoreCursor* pCursor = static_cast<ScoreCursor*>( m_cursor->get_inner_cursor() );
        m_executer.insert_clef(m_toolsInfo.clefType, pCursor->staff());
        m_fEventProcessed = true;
    }
}

//---------------------------------------------------------------------------------------
void ClickHandler::add_note_rest(SpEventMouse event)
{
//    //Click on staff
//    if (m_pCurShapeStaff)
//    {
        //move cursor to insertion point and get staff number
        m_executer.move_caret_to_click_point(event);
        ScoreCursor* pCursor = static_cast<ScoreCursor*>( m_cursor->get_inner_cursor() );
        int staff = pCursor->staff() + 1;
        int voice = m_toolsInfo.voice;

//        //in 'TheoHarmonyCtrol' edit mode, force staff depending on voice
//        lmEditorMode* pEditorMode = m_pDoc->GetEditMode();
//        if (pEditorMode && pEditorMode->GetModeName() == "TheoHarmonyCtrol")
//        {
//            if (voice == 1 || voice == 2)
//                staff = 1;
//            else
//                staff = 2;
//        }

        string name;
        stringstream src;
        if (m_toolsInfo.fIsNote)
        {
            name = to_std_string( _("Insert note") );
            src << "(n ";
            if (m_toolsInfo.acc != k_no_accidentals)
                src << LdpExporter::accidentals_to_string(m_toolsInfo.acc);

            //get pitch from mouse position on staff
            DiatonicPitch dp = m_pController->get_pitch_at(event->get_x(), event->get_y());
            if (dp == DiatonicPitch(k_no_pitch))
                src << "* ";
            else
                src << dp.get_ldp_name() << " ";
        }
        else
        {
            name = to_std_string( _("Insert rest") );
            src << "(r ";
        }

        src << LdpExporter::notetype_to_string(m_toolsInfo.noteType, m_toolsInfo.dots);
        src << " v" << voice;
        src << " p" << staff << ")";

        m_executer.insert_staffobj(src.str(), name);
        m_fEventProcessed = true;
}




////---------------------------------------------------------------------------------------
//void CommandEventHandler::OnToolHarmonyClick(GmoObj* pGMO, UPoint uPagePos,
//                                       TimeUnits rGridTime)
//{
//    //Click on Note/rest: Add figured bass
//    if (pGMO->IsShape())
//    {
//        ToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
//	    wxASSERT(pToolBox);
//        ToolPageSymbols* pPage = (ToolPageSymbols*)pToolBox->get_selected_page();
//        lmEToolID nTool = pPage->get_selected_tool_id();
//        lmScoreObj* pSCO = ((lmShape*)pGMO)->GetScoreOwner();
//        switch(nTool)
//        {
//            case lmTOOL_FIGURED_BASS:
//            {
//                //Move cursor to insertion position and insert figured bass
//                wxASSERT(pSCO->IsNote() || pSCO->IsRest());
//                m_pDoc->GetScore()->GetCursor()->MoveCursorToObject((lmStaffObj*)pSCO);
//                InsertFiguredBass();
//                break;
//            }
//
//            case lmTOOL_FB_LINE:
//                //Move cursor to insertion position and insert figured bass line
//                wxASSERT(pSCO->IsNote() || pSCO->IsRest());
//                m_pDoc->GetScore()->GetCursor()->MoveCursorToObject((lmStaffObj*)pSCO);
//                InsertFiguredBassLine();
//                break;
//
//            default:
//                wxASSERT(false);
//        }
//    }
//}
//
////---------------------------------------------------------------------------------------
//void CommandEventHandler::OnToolSymbolsClick(GmoObj* pGMO, UPoint uPagePos,
//                                       TimeUnits rGridTime)
//{
//    //TODO
//    //Click on Note/rest: Add symbol
//    if (pGMO->IsShape())
//    {
//        ToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
//	    wxASSERT(pToolBox);
//        ToolPageSymbols* pPage = (ToolPageSymbols*)pToolBox->get_selected_page();
//        lmEToolID nTool = pPage->get_selected_tool_id();
//        lmScoreObj* pSCO = ((lmShape*)pGMO)->GetScoreOwner();
//        switch(nTool)
//        {
//            case lmTOOL_LINES:
//                lmTODO("[CommandEventHandler::OnToolSymbolsClick] TODO: handle LINES tool");
//                break;
//
//            case lmTOOL_TEXTBOX:
//                lmTODO("[CommandEventHandler::OnToolSymbolsClick] TODO: handle TEXTBOX tool");
//                break;
//
//            case lmTOOL_TEXT:
//                lmTODO("[CommandEventHandler::OnToolSymbolsClick] TODO: handle TEXT tool");
//                break;
//
//            default:
//                wxASSERT(false);
//        }
//    }
//}

////---------------------------------------------------------------------------------------
//void CommandEventHandler::OnToolTimeSignatureClick(GmoObj* pGMO, UPoint uPagePos,
//                                             TimeUnits rGridTime)
//{
//    //Click on staff
//    if (m_pCurShapeStaff)
//    {
//        //Move cursor to insertion position (start of pointed measure)
//	    int nStaff = m_pCurShapeStaff->GetNumStaff();
//        MoveCursorTo(m_pCurBSI, nStaff, 0.0f, false);    //true: move to end of time
//
//        //do insert Time Signature
//        ToolPageClefs* pPage = (ToolPageClefs*)m_pToolBox->get_selected_page();
//        int nBeats = pPage->GetTimeBeats();
//        int nBeatType = pPage->GetTimeBeatType();
//        InsertTimeSignature(nBeats, nBeatType);
//    }
//}
//
////---------------------------------------------------------------------------------------
//void CommandEventHandler::OnToolKeySignatureClick(GmoObj* pGMO, UPoint uPagePos,
//                                            TimeUnits rGridTime)
//{
//    //Click on staff
//    if (m_pCurShapeStaff)
//    {
//        //Move cursor to insertion position (start of pointed measure)
//	    int nStaff = m_pCurShapeStaff->GetNumStaff();
//        MoveCursorTo(m_pCurBSI, nStaff, 0.0f, false);    //true: move to end of time
//
//        //do insert Key Signature
//        ToolPageClefs* pPage = (ToolPageClefs*)m_pToolBox->get_selected_page();
//        bool fMajor = pPage->IsMajorKeySignature();
//        int nFifths = pPage->GetFifths();
//        InsertKeySignature(nFifths, fMajor);
//    }
//}




////-------------------------------------------------------------------------------------
//// implementation of DocumentWindow
////-------------------------------------------------------------------------------------
//
//
//
//wxBEGIN_EVENT_TABLE(DocumentWindow, wxEvtHandler)
//	EVT_CHAR(DocumentWindow::OnKeyPress)
//	EVT_KEY_DOWN(DocumentWindow::OnKeyDown)
//    EVT_ERASE_BACKGROUND(DocumentWindow::OnEraseBackground)
//
//	//contextual menus
//	EVT_MENU	(lmPOPUP_Cut, DocumentWindow::OnCut)
//    EVT_MENU	(lmPOPUP_Copy, DocumentWindow::OnCopy)
//    EVT_MENU	(lmPOPUP_Paste, DocumentWindow::OnPaste)
//    EVT_MENU	(lmPOPUP_Color, DocumentWindow::OnColor)
//    EVT_MENU	(lmPOPUP_Properties, DocumentWindow::OnProperties)
//    EVT_MENU	(lmPOPUP_DeleteTiePrev, DocumentWindow::OnDeleteTiePrev)
//    EVT_MENU	(lmPOPUP_AttachText, DocumentWindow::OnAttachText)
//    EVT_MENU	(lmPOPUP_Score_Titles, DocumentWindow::OnScoreTitles)
//    EVT_MENU	(lmPOPUP_View_Page_Margins, DocumentWindow::OnViewPageMargins)
//    EVT_MENU	(lmPOPUP_ToggleStem, DocumentWindow::OnToggleStem)
//#ifdef _LM_DEBUG_
//    EVT_MENU	(lmPOPUP_DumpShape, DocumentWindow::OnDumpShape)
//#endif
//    EVT_MENU	(lmTOOL_VOICE_SOPRANO, DocumentWindow::OnToolPopUpMenuEvent)
//	EVT_MENU	(lmTOOL_VOICE_ALTO, DocumentWindow::OnToolPopUpMenuEvent)
//	EVT_MENU	(lmTOOL_VOICE_TENOR, DocumentWindow::OnToolPopUpMenuEvent)
//	EVT_MENU	(lmTOOL_VOICE_BASS, DocumentWindow::OnToolPopUpMenuEvent)
//
//
//wxEND_EVENT_TABLE()

////----------------------------------------------------------------------------
//// Helper class to display popup window with information about dragged tool
////----------------------------------------------------------------------------
//class lmInfoWindow : public wxPopupTransientWindow
//{
//public:
//    lmInfoWindow( wxWindow *parent );
//    virtual ~lmInfoWindow();
//
//    wxScrolledWindow* GetChild() { return m_panel; }
//
//private:
//    wxScrolledWindow *m_panel;
//
//    wxDECLARE_CLASS(lmInfoWindow);
//};
//
////----------------------------------------------------------------------------
//// lmInfoWindow
////----------------------------------------------------------------------------
//IMPLEMENT_CLASS(lmInfoWindow,wxPopupTransientWindow)
//
//lmInfoWindow::lmInfoWindow( wxWindow *parent )
//    : wxPopupTransientWindow( parent )
//{
//    m_panel = new wxScrolledWindow( this, wxID_ANY );
//    m_panel->SetBackgroundColour( wxColour(255,255,170) );    //pale yellow
//
//    wxStaticText *text = new wxStaticText( m_panel, wxID_ANY,
//                          "Hola. Nota C4" );
//
//
//    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
//    topSizer->Add( text, 0, wxALL, 5 );
//
//    m_panel->SetAutoLayout( true );
//    m_panel->SetSizer( topSizer );
//    topSizer->Fit(m_panel);
//    topSizer->Fit(this);
//}
//
//lmInfoWindow::~lmInfoWindow()
//{
//}
//
//
//
//
////-------------------------------------------------------------------------------------
//// implementation of CommandEventHandler
////-------------------------------------------------------------------------------------
//
//IMPLEMENT_CLASS(CommandEventHandler, DocumentWindow)
//
//// keys pressed when a mouse event
//enum {
//    lmKEY_NONE = 0x0000,
//    lmKEY_ALT = 0x0001,
//    lmKEY_CTRL = 0x0002,
//    lmKEY_SHIFT = 0x0004,
//};
//
//// Dragging states
//enum
//{
//	lmDRAG_NONE = 0,
//	lmDRAG_START_LEFT,
//	lmDRAG_CONTINUE_LEFT,
//	lmDRAG_START_RIGHT,
//	lmDRAG_CONTINUE_RIGHT,
//};
//
//
//
//void CommandEventHandler::CaptureTheMouse()
//{
//    wxLogMessage("[CommandEventHandler::CaptureTheMouse] HasCapture=%s",
//                 (HasCapture() ? "yes" : "no") );
//    if (!HasCapture())
//        CaptureMouse();
//}
//
//void CommandEventHandler::ReleaseTheMouse()
//{
//    wxLogMessage("[CommandEventHandler::ReleaseTheMouse] HasCapture=%s",
//                 (HasCapture() ? "yes" : "no") );
//    if (HasCapture())
//        ReleaseMouse();
//}
//
////#ifdef _LM_WINDOWS_
//void CommandEventHandler::OnMouseCaptureLost(wxMouseCaptureLostEvent& event)
//{
//    //Any application which captures the mouse in the beginning of some operation
//    //must handle wxMouseCaptureLostEvent and cancel this operation when it receives
//    //the event. The event handler must not recapture mouse.
//    wxLogMessage("[CommandEventHandler::OnMouseCaptureLost] HasCapture=%s",
//                 (HasCapture() ? "yes" : "no") );
//    //m_pView->OnImageEndDrag();>OnObjectEndDragLeft(event, pDC, vCanvasPos, vCanvasOffset,
//    //                             uPagePos, nKeys);
//    //SetDraggingObject(false);
//    //m_nDragState = lmDRAG_NONE;
//    //SetDraggingObject(false);
//}
////#endif
//
//void CommandEventHandler::OnPaint(wxPaintEvent &WXUNUSED(event))
//{
//    // In a paint event handler, the application must always create a wxPaintDC object,
//    // even if it is not used. Otherwise, under MS Windows, refreshing for this and
//    // other windows will go wrong.
//    wxPaintDC dc(this);
//    if (!m_pView) return;
//
//    // get the updated rectangles list
//    wxRegionIterator upd(GetUpdateRegion());
//
//    // iterate to redraw each damaged rectangle
//    // The rectangles are in pixels, referred to the client area, and are unscrolled
//    m_pView->PrepareForRepaint(&dc);
//    while (upd)
//    {
//        wxRect rect = upd.GetRect();
//        m_pView->RepaintScoreRectangle(&dc, rect);
//        upd++;
//    }
//    m_pView->TerminateRepaint(&dc);
//}
//
//void CommandEventHandler::OnMouseEvent(wxMouseEvent& event)
//{
//    //handle mouse event
//
//    //if no view, nothimg to do
//    if (!m_pView)
//        return;
//
//    wxClientDC dc(this);
//
//        //First, for better performance, filter out non-used events
//
//    //filter out non-handled events
//    wxEventType nEventType = event.GetEventType();
//    if (nEventType==wxEVT_MIDDLE_DOWN || nEventType==wxEVT_MIDDLE_UP ||
//        nEventType==wxEVT_MIDDLE_DCLICK)
//    {
//        return;
//    }
//
//
//        //Now deal with events that do not require to compute mouse position and/or which graphical
//        //object is under the mouse
//
//
//    // check for mouse entering/leaving the window events
//	if (event.Entering())    //wxEVT_ENTER_WINDOW
//	{
//		//the mouse is entering the window. Change mouse icon as appropriate
//		//TODO
//		return;
//	}
//	if (event.Leaving())    //wxEVT_LEAVE_WINDOW
//	{
//		//the mouse is leaving the window. Change mouse icon as appropriate
//		//TODO
//		return;
//	}
//
//    //deal with mouse wheel events
//	if (nEventType == wxEVT_MOUSEWHEEL)
//    {
//        m_pView->OnMouseWheel(event);
//		return;
//    }
//
//
//        //From this point we need information about mouse position. Let's compute it an
//        //update GUI (rules markers, status bar, etc.). Get also information about any possible
//        //key being pressed whil mouse is moving, and about dragging
//
//
//	// get mouse point (pixels, referred to CommandEventHandler origin)
//    m_vMouseCanvasPos = event.GetPosition();
//
//    // Set DC in logical units and scaled, so that
//    // transformations logical/device and viceversa can be computed
//    m_pView->ScaleDC(&dc);
//
//    //compute mouse point in logical units. Get also different origins and values
//    bool fInInterpageGap;           //mouse click out of page
//	m_pView->DeviceToLogical(m_vMouseCanvasPos, m_uMousePagePos, &m_vMousePagePos,
//                             &m_vPageOrg, &m_vCanvasOffset, &m_nNumPage,
//                             &fInInterpageGap);
//
//	#ifdef _LM_DEBUG_
//	bool fDebugMode = g_pLogger->IsAllowedTraceMask("OnMouseEvent");
//	#endif
//
//    //update mouse num page
//    m_pView->UpdateNumPage(m_nNumPage);
//
//	////for testing and debugging methods DeviceToLogical [ok] and LogicalToDevice [ok]
//	//lmDPoint tempPagePosD;
//	//LogicalToDevice(tempPagePosL, tempPagePosD);
//
//    // draw markers on the rulers
//    m_pView->UpdateRulerMarkers(m_vMousePagePos);
//
//    // check if dragging (moving with a button pressed), and filter out mouse movements small
//    //than tolerance
//	bool fDragging = event.Dragging();
//	if (fDragging && m_fCheckTolerance)
//	{
//		// Check if we're within the tolerance for mouse movements.
//		// If we're very close to the position we started dragging
//		// from, this may not be an intentional drag at all.
//		lmLUnits uAx = abs(dc.DeviceToLogicalXRel((long)(m_vMouseCanvasPos.x - m_vStartDrag.x)));
//		lmLUnits uAy = abs(dc.DeviceToLogicalYRel((long)(m_vMouseCanvasPos.y - m_vStartDrag.y)));
//        lmLUnits uTolerance = m_pView->GetMouseTolerance();
//		if (uAx <= uTolerance && uAy <= uTolerance)
//			return;
//		else
//            //I will not allow for a second involuntary small movement. Therefore
//			//if we have ignored a drag, smaller than tolerance, then do not check for
//            //tolerance the next time in this drag.
//			m_fCheckTolerance = false;
//	}
//
//    //At this point it has been determined all basic mouse position information. Now we
//    //start dealing with mouse moving, mouse clicks and dragging events. Behaviour from
//    //this point is different, depending on mouse mode (pointer, data entry, eraser, etc.).
//    //Therefore, processing is splitted at this point
//
//    if (mouseMode == k_mouse_mode_data_entry)
//        OnMouseEventToolMode(event, &dc);
//    else
//        OnMouseEventSelectMode(event, &dc);
//
//}
//
//void CommandEventHandler::OnMouseEventSelectMode(wxMouseEvent& event, wxDC* pDC)
//{
//    //If we reach this point is because it is a mouse dragging or a mouse click event.
//    //Let's deal with them.
//
//	#ifdef _LM_DEBUG_
//	bool fDebugMode = g_pLogger->IsAllowedTraceMask("OnMouseEvent");
//	#endif
//
//	bool fDragging = event.Dragging();
//
//
//    //determine type of area pointed by mouse. Also collect information
//    //about current staff for point pointed by mouse and about related BoxSliceInstr.
//    GetPointedAreaInfo();
//
//    //check moving events
//    if (event.GetEventType() == wxEVT_MOTION && !fDragging)
//    {
//	    if (m_pCurGMO)
//        {
//            //Mouse is currently pointing to an object (shape or box)
//            if (m_pMouseOverGMO)
//            {
//                //mouse was previously over an object. If it is the same than current one there is
//                //nothing to do
//                if (m_pMouseOverGMO == m_pCurGMO)
//                    return;     //nothing to do. Mouse continues over object
//
//                //It is a new object. Inform previous object that it is left
//                m_pMouseOverGMO->OnMouseOut(this, m_uMousePagePos);
//            }
//            m_pMouseOverGMO = m_pCurGMO;
//            m_pCurGMO->OnMouseIn(this, m_uMousePagePos);
//        }
//
//        //mouse is not pointing neither to a shape nor to a box. If mouse was poining to an object
//        //inform it that it has been left
//        else
//        {
//            if (m_pMouseOverGMO)
//            {
//                //mouse was previously over an object. Inform it that it is left
//                m_pMouseOverGMO->OnMouseOut(this, m_uMousePagePos);
//                m_pMouseOverGMO = (GmoObj*)nullptr;
//            }
//        }
//        return;
//    }
//
//    // check if a key is pressed
//    int nKeysPressed = lmKEY_NONE;
//    if (event.ShiftDown())
//        nKeysPressed |= lmKEY_SHIFT;
//    if (event.CmdDown())
//        nKeysPressed |= lmKEY_CTRL;
//    if (event.AltDown())
//        nKeysPressed |= lmKEY_ALT;
//
//	if (!fDragging)
//	{
//		// Non-dragging events.
//        // In MS Windows the 'end of drag' event is a non-dragging event
//
//		m_fCheckTolerance = true;
//
//		#ifdef _LM_DEBUG_
//		if(fDebugMode) g_pLogger->LogDebug("Non-dragging event");
//		#endif
//
//		if (event.IsButton())
//		{
//			#ifdef _LM_DEBUG_
//			if(fDebugMode) g_pLogger->LogDebug("button event");
//			#endif
//
//			//find the object pointed with the mouse
//			GmoObj* m_pCurGMO = m_pView->FindShapeAt(m_nNumPage, m_uMousePagePos, false);
//			if (m_pCurGMO) // Object event
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("button on object event");
//				#endif
//
//				if (event.LeftDown())
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on object: event.LeftDown()");
//					#endif
//
//					//Save data for a possible start of dragging
//					m_pDraggedGMO = m_pCurGMO;
//					m_nDragState = lmDRAG_START_LEFT;
//					m_vStartDrag.x = m_vMouseCanvasPos.x;
//					m_vStartDrag.y = m_vMouseCanvasPos.y;
//                    m_uStartDrag = m_uMousePagePos;
//
//					m_uDragStartPos = m_uMousePagePos;	// save mouse position (page logical coordinates)
//					// compute the location of the drag position relative to the upper-left
//					// corner of the image (pixels)
//					m_uHotSpotShift = m_uMousePagePos - m_pDraggedGMO->GetObjectOrigin();
//					m_vDragHotSpot.x = pDC->LogicalToDeviceXRel((int)m_uHotSpotShift.x);
//					m_vDragHotSpot.y = pDC->LogicalToDeviceYRel((int)m_uHotSpotShift.y);
//				}
//				else if (event.LeftUp())
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on object: event.LeftUp()");
//					#endif
//
//			        if (m_nDragState == lmDRAG_CONTINUE_LEFT)
//			        {
//                        if (m_fDraggingObject)
//                        {
//                            //draggin. Finish left object dragging
//				            OnObjectContinueDragLeft(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				            OnObjectEndDragLeft(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//                        }
//                        else
//                        {
//                            //draggin. Finish left canvas dragging
//				            OnCanvasContinueDragLeft(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
//				            OnCanvasEndDragLeft(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//                       }
//			        }
//                    else
//                    {
//					    //click on object. Only send a click event if the same object
//                        //was involved in 'down' and 'up' events
//					    if (m_pCurGMO == m_pDraggedGMO)
//						    OnLeftClickOnObject(m_pCurGMO, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//                    }
//					m_pDraggedGMO = (GmoObj*)nullptr;
//					m_nDragState = lmDRAG_NONE;
//                    m_fCheckTolerance = true;
//				}
//				else if (event.LeftDClick())
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on object: event.LeftDClick()");
//					#endif
//
//					OnLeftDoubleClickOnObject(m_pCurGMO, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//					m_pDraggedGMO = (GmoObj*)nullptr;
//					m_nDragState = lmDRAG_NONE;
//				}
//				else if (event.RightDown())
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on object: event.RightDown()");
//					#endif
//
//					//Save data for a possible start of dragging
//					m_pDraggedGMO = m_pCurGMO;
//					m_nDragState = lmDRAG_START_RIGHT;
//					m_vStartDrag.x = m_vMouseCanvasPos.x;
//					m_vStartDrag.y = m_vMouseCanvasPos.y;
//                    m_uStartDrag = m_uMousePagePos;
//
//					m_uDragStartPos = m_uMousePagePos;	// save mouse position (page logical coordinates)
//					// compute the location of the drag position relative to the upper-left
//					// corner of the image (pixels)
//					m_uHotSpotShift = m_uMousePagePos - m_pDraggedGMO->GetObjectOrigin();
//					m_vDragHotSpot.x = pDC->LogicalToDeviceXRel((int)m_uHotSpotShift.x);
//					m_vDragHotSpot.y = pDC->LogicalToDeviceYRel((int)m_uHotSpotShift.y);
//				}
//				else if (event.RightUp())
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on object: event.RightUp()");
//					#endif
//
//			        if (m_nDragState == lmDRAG_CONTINUE_RIGHT)
//			        {
//                        if (m_fDraggingObject)
//                        {
//                            //draggin. Finish right object dragging
//				            OnObjectContinueDragRight(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				            OnObjectEndDragRight(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//                        }
//                        else
//                        {
//                            //draggin. Finish right canvas dragging
//				            OnCanvasContinueDragRight(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
//				            OnCanvasEndDragRight(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//                       }
//			        }
//                    else
//                    {
//					    //click on object. Only send a click event if the same object
//                        //was involved in 'down' and 'up' events
//					    if (m_pCurGMO == m_pDraggedGMO)
//						    OnRightClickOnObject(m_pCurGMO, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//                    }
//					m_pDraggedGMO = (GmoObj*)nullptr;
//					m_nDragState = lmDRAG_NONE;
//                    m_fCheckTolerance = true;
//				}
//				else if (event.RightDClick())
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on object: event.RightDClick()");
//					#endif
//
//					OnRightDoubleClickOnObject(m_pCurGMO, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//					m_pDraggedGMO = (GmoObj*)nullptr;
//					m_nDragState = lmDRAG_NONE;
//				}
//				else
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on object: no identified event");
//					#endif
//				}
//
//			}
//			else // Canvas event (no pointed object)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("button on canvas event");
//				#endif
//
//				if (event.LeftDown())
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on canvas: event.LeftDown()");
//					#endif
//
//					m_pDraggedGMO = (GmoObj*)nullptr;
//					m_nDragState = lmDRAG_START_LEFT;
//					m_vStartDrag.x = m_vMouseCanvasPos.x;
//					m_vStartDrag.y = m_vMouseCanvasPos.y;
//                    m_uStartDrag = m_uMousePagePos;
//				}
//				else if (event.LeftUp())
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on canvas: event.LeftUp()");
//					#endif
//
//			        if (m_nDragState == lmDRAG_CONTINUE_LEFT)
//			        {
//                        if (m_pDraggedGMO)
//                        {
//							#ifdef _LM_DEBUG_
//							if(fDebugMode) g_pLogger->LogDebug("dragging object: Finish left dragging");
//							#endif
//
//	                            //draggin. Finish left dragging
//				            OnObjectContinueDragLeft(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				            OnObjectEndDragLeft(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//                        }
//                        else
//                        {
//							#ifdef _LM_DEBUG_
//							if(fDebugMode) g_pLogger->LogDebug("dragging on canvas: Finish left dragging");
//							#endif
//
//                            //draggin. Finish left dragging
//				            OnCanvasContinueDragLeft(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
//				            OnCanvasEndDragLeft(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//                        }
//			        }
//                    else
//                    {
//						#ifdef _LM_DEBUG_
//						if(fDebugMode) g_pLogger->LogDebug("button on canvas: non-dragging. Left click on object");
//						#endif
//
//                        //non-dragging. Left click on object
//					    OnLeftClickOnCanvas(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//                    }
//					m_pDraggedGMO = (GmoObj*)nullptr;
//					m_nDragState = lmDRAG_NONE;
//                    m_fCheckTolerance = true;
//				}
//				else if (event.RightDown())
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on canvas: event.RightDown()");
//					#endif
//
//					m_pDraggedGMO = (GmoObj*)nullptr;
//					m_nDragState = lmDRAG_START_RIGHT;
//					m_vStartDrag.x = m_vMouseCanvasPos.x;
//					m_vStartDrag.y = m_vMouseCanvasPos.y;
//                    m_uStartDrag = m_uMousePagePos;
//				}
//				else if (event.RightUp())
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on canvas: event.RightUp()");
//					#endif
//
//			        if (m_nDragState == lmDRAG_CONTINUE_RIGHT)
//			        {
//                        if (m_pDraggedGMO)
//                        {
//							#ifdef _LM_DEBUG_
//							if(fDebugMode) g_pLogger->LogDebug("dragging object: Finish right dragging");
//							#endif
//
//	                            //draggin. Finish right dragging
//				            OnObjectContinueDragRight(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				            OnObjectEndDragRight(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//                        }
//                        else
//                        {
//							#ifdef _LM_DEBUG_
//							if(fDebugMode) g_pLogger->LogDebug("dragging on canvas: Finish right dragging");
//							#endif
//
//                            //draggin. Finish right dragging
//				            OnCanvasContinueDragRight(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
//				            OnCanvasEndDragRight(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//                        }
//			        }
//                    else
//                    {
//						#ifdef _LM_DEBUG_
//						if(fDebugMode) g_pLogger->LogDebug("button on canvas: non-dragging. Right click on object");
//						#endif
//
//                        //non-dragging. Right click on object
//					    OnRightClickOnCanvas(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//                    }
//					m_pDraggedGMO = (GmoObj*)nullptr;
//					m_nDragState = lmDRAG_NONE;
//                    m_fCheckTolerance = true;
//				}
//                else
//				{
//					#ifdef _LM_DEBUG_
//					if(fDebugMode) g_pLogger->LogDebug("button on canvas: no identified event");
//					#endif
//				}
//			}
//		}
//        else
//		{
//			#ifdef _LM_DEBUG_
//			if(fDebugMode) g_pLogger->LogDebug("non-dragging: no button event");
//			#endif
//		}
//	}
//
//	else	//dragging events
//	{
//		#ifdef _LM_DEBUG_
//		if(fDebugMode) g_pLogger->LogDebug("dragging event");
//		#endif
//
//		if (m_pDraggedGMO)
//		{
//			#ifdef _LM_DEBUG_
//			if(fDebugMode) g_pLogger->LogDebug("draggin an object");
//			#endif
//
//			//draggin an object
//			if (event.LeftUp() && m_nDragState == lmDRAG_CONTINUE_LEFT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("object: event.LeftUp() && m_nDragState == lmDRAG_CONTINUE_LEFT");
//				#endif
//
//				m_nDragState = lmDRAG_NONE;
//				m_fCheckTolerance = true;
//				OnObjectContinueDragLeft(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				OnObjectEndDragLeft(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				m_pDraggedGMO = (GmoObj*)nullptr;
//			}
//			else if (event.RightUp() && m_nDragState == lmDRAG_CONTINUE_RIGHT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("object: event.RightUp() && m_nDragState == lmDRAG_CONTINUE_RIGHT");
//				#endif
//
//				m_nDragState = lmDRAG_NONE;
//				m_fCheckTolerance = true;
//				OnObjectContinueDragRight(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				OnObjectEndDragRight(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				m_pDraggedGMO = (GmoObj*)nullptr;
//			}
//			else if (m_nDragState == lmDRAG_START_LEFT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("object: m_nDragState == lmDRAG_START_LEFT");
//				#endif
//
//				m_nDragState = lmDRAG_CONTINUE_LEFT;
//
//				if (m_pDraggedGMO->IsLeftDraggable())
//                {
//					OnObjectBeginDragLeft(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//                }
//				else
//				{
//					//the object is not draggable: transfer message to canvas
//				    #ifdef _LM_DEBUG_
//				    if(fDebugMode) g_pLogger->LogDebug("object is not left draggable. Drag cancelled");
//				    #endif
//					m_pDraggedGMO = (GmoObj*)nullptr;
//					OnCanvasBeginDragLeft(m_vStartDrag, m_uMousePagePos, nKeysPressed);
//				}
//				m_vEndDrag = m_vMouseCanvasPos;
//			}
//			else if (m_nDragState == lmDRAG_CONTINUE_LEFT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("object: m_nDragState == lmDRAG_CONTINUE_LEFT");
//				#endif
//
//				// Continue dragging
//				OnObjectContinueDragLeft(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				OnObjectContinueDragLeft(event, pDC, true, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				m_vEndDrag = m_vMouseCanvasPos;
//			}
//			else if (m_nDragState == lmDRAG_START_RIGHT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("object: m_nDragState == lmDRAG_START_RIGHT");
//				#endif
//
//				m_nDragState = lmDRAG_CONTINUE_RIGHT;
//
//				if (m_pDraggedGMO->IsRightDraggable())
//				{
//					OnObjectBeginDragRight(event, pDC, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//                }
//				else
//				{
//					//the object is not draggable: transfer message to canvas
//					m_pDraggedGMO = (GmoObj*)nullptr;
//					OnCanvasBeginDragRight(m_vStartDrag, m_uMousePagePos, nKeysPressed);
//				}
//				m_vEndDrag = m_vMouseCanvasPos;
//			}
//			else if (m_nDragState == lmDRAG_CONTINUE_RIGHT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("object: m_nDragState == lmDRAG_CONTINUE_RIGHT");
//				#endif
//
//				// Continue dragging
//				OnObjectContinueDragRight(event, pDC, false, m_vEndDrag, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				OnObjectContinueDragRight(event, pDC, true, m_vMouseCanvasPos, m_vCanvasOffset, m_uMousePagePos, nKeysPressed);
//				m_vEndDrag = m_vMouseCanvasPos;
//			}
//            else
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("object: no identified event");
//				#endif
//			}
//		}
//
//		else	// dragging but no object: events sent to canvas
//		{
//			#ifdef _LM_DEBUG_
//			if(fDebugMode) g_pLogger->LogDebug("dragging but no object: canvas");
//			#endif
//
//			if (event.LeftUp() && m_nDragState == lmDRAG_CONTINUE_LEFT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("canvas: event.LeftUp() && m_nDragState == lmDRAG_CONTINUE_LEFT");
//				#endif
//
//				m_nDragState = lmDRAG_NONE;
//				m_fCheckTolerance = true;
//
//				OnCanvasContinueDragLeft(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
//				OnCanvasEndDragLeft(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//				m_pDraggedGMO = (GmoObj*)nullptr;
//			}
//			else if (event.RightUp() && m_nDragState == lmDRAG_CONTINUE_RIGHT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("canvas: event.RightUp() && m_nDragState == lmDRAG_CONTINUE_RIGHT");
//				#endif
//
//				m_nDragState = lmDRAG_NONE;
//				m_fCheckTolerance = true;
//
//				OnCanvasContinueDragRight(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
//				OnCanvasEndDragRight(m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//				m_pDraggedGMO = (GmoObj*)nullptr;
//			}
//			else if (m_nDragState == lmDRAG_START_LEFT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("canvas: m_nDragState == lmDRAG_START_LEFT");
//				#endif
//
//				m_nDragState = lmDRAG_CONTINUE_LEFT;
//				OnCanvasBeginDragLeft(m_vStartDrag, m_uMousePagePos, nKeysPressed);
//				m_vEndDrag = m_vMouseCanvasPos;
//			}
//			else if (m_nDragState == lmDRAG_CONTINUE_LEFT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("canvas: m_nDragState == lmDRAG_CONTINUE_LEFT");
//				#endif
//
//				// Continue dragging
//				OnCanvasContinueDragLeft(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
//				OnCanvasContinueDragLeft(true, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//				m_vEndDrag = m_vMouseCanvasPos;
//			}
//			else if (m_nDragState == lmDRAG_START_RIGHT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("canvas: m_nDragState == lmDRAG_START_RIGHT");
//				#endif
//
//				m_nDragState = lmDRAG_CONTINUE_RIGHT;
//				OnCanvasBeginDragRight(m_vStartDrag, m_uMousePagePos, nKeysPressed);
//				m_vEndDrag = m_vMouseCanvasPos;
//			}
//			else if (m_nDragState == lmDRAG_CONTINUE_RIGHT)
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("canvas: m_nDragState == lmDRAG_CONTINUE_RIGHT");
//				#endif
//
//				// Continue dragging
//				OnCanvasContinueDragRight(false, m_vEndDrag, m_uMousePagePos, nKeysPressed);
//				OnCanvasContinueDragRight(true, m_vMouseCanvasPos, m_uMousePagePos, nKeysPressed);
//				m_vEndDrag = m_vMouseCanvasPos;
//			}
//            else
//			{
//				#ifdef _LM_DEBUG_
//				if(fDebugMode) g_pLogger->LogDebug("canvas: no identified event");
//				#endif
//			}
//		}
//	}
//
//}

////---------------------------------------------------------------------------------------
//void CommandEventHandler::OnMouseEventToolMode(wxMouseEvent& event, wxDC* pDC)
//{
//    //this method deals with mouse clicks when mouse is in data entry mode
//
//    //At this point mouse position and page information has been already
//    //computed
//
//    //determine type of area pointed by mouse. Also collect information
//    //about current staff for point pointed by mouse and about related BoxSliceInstr.
//    long nOldMousePointedArea = m_nMousePointedArea;
//    GetPointedAreaInfo();
//
//    //if harmony exercise, allow notes data entry only valid staff for current voice
//    lmEditorMode* pEditorMode = m_pDoc->GetEditMode();
//    if (pEditorMode && pEditorMode->GetModeName() == "TheoHarmonyCtrol"
//        && (m_nMousePointedArea == lmMOUSE_OnStaff
//            || m_nMousePointedArea == lmMOUSE_OnBelowStaff
//            || m_nMousePointedArea == lmMOUSE_OnAboveStaff) )
//    {
//        int nStaff = m_pCurShapeStaff->GetNumStaff();
//        if ( ((m_nSelVoice == 1 || m_nSelVoice == 2) && (nStaff != 1))
//             || ((m_nSelVoice == 3 || m_nSelVoice == 4) && (nStaff != 2)) )
//            m_nMousePointedArea = lmMOUSE_OnOther;
//    }
//
//    long nNowOnValidArea = m_nMousePointedArea & m_nValidAreas;
//
//    //Now we start dealing with mouse moving and mouse clicks. Dragging (moving the mouse
//    //with a mouse button clicked) is a meaningless operation and will be treated as
//    //moving. Therefore, only two type of events will be considered: mouse click and
//    //mouse move. Let's start with mouse click events.
//
//	if (event.IsButton() && nNowOnValidArea != 0L)
//	{
//        //mouse click: terminate any possible tool drag operation, process the click, and
//        //restart the tool drag operation
//
//        //first, terminate any possible tool drag operation
//        if (m_fDraggingTool)
//            TerminateToolDrag(pDC);
//
//////        THIS IS NOW IMPLEMENTED USING ClickHandler class
//////        //now process the click. To avoid double data entry (first, on button down and then on
//////        //button up) only button up will trigger the processing
//////        if (event.ButtonUp(wxMOUSE_BTN_LEFT))
//////        {
//////            OnToolClick(m_pCurGMO, m_uMousePagePos, m_rCurGridTime);
//////            //AWARE: after processing the click the graphical model could have been chaged.
//////            //Therefore all pointers to GMObjects are no longer valid!!!
//////        }
//////        else if (event.ButtonUp(wxMOUSE_BTN_RIGHT))
//////        {
//////            //show too contextual menu
//////            wxMenu* pMenu = GetContextualMenuForTool();
//////            if (pMenu)
//////	            PopupMenu(pMenu);
//////        }
//
//        //finally, set up information to restart the tool drag operation when moving again
//        //the mouse
//        m_nMousePointedArea = 0;
//        m_pLastShapeStaff = m_pCurShapeStaff;
//        m_pLastBSI = m_pCurBSI;
//
//        return;
//    }
//
//
//    //process mouse moving events
//
//    if (event.GetEventType() != wxEVT_MOTION)
//        return;
//
//
//
//    //check if pointed area has changed from valid to invalid or vice versa
//
//    long nBeforeOnValidArea = nOldMousePointedArea & m_nValidAreas;
//    if (nBeforeOnValidArea != 0 && nNowOnValidArea == 0 || nBeforeOnValidArea == 0 && nNowOnValidArea != 0)
//    {
//        //change from valida area to invalid, or vice versa, change drag status
//        if (nNowOnValidArea)
//        {
//            //entering on staff influence area. Start dragging tool
//            StartToolDrag(pDC);
//        }
//        else
//        {
//            //exiting staff influence area
//            TerminateToolDrag(pDC);
//        }
//    }
//    else
//    {
//        //no change valid<->invalida area. If we continue in a valid area drag marks
//        //lines if necessary
//        if (nNowOnValidArea)
//            ContinueToolDrag(event, pDC);
//    }
//
//    //update saved information
//    m_pLastShapeStaff = m_pCurShapeStaff;
//    m_pLastBSI = m_pCurBSI;
//
//    //determine needed icon and change mouse icon if necessary
//    wxCursor* pNeeded = m_pCursorElse;
//    if (nNowOnValidArea)
//        pNeeded = m_pCursorOnValidArea;
//
//    if (m_pCursorCurrent != pNeeded)
//    {
//        //change cursor
//        m_pCursorCurrent = pNeeded;
//        const wxCursor& oCursor = *pNeeded;
//        SetCursor( oCursor );
//    }
//
//    //update status bar info
//    UpdateToolInfoString();
//}

//void CommandEventHandler::GetPointedAreaInfo()
//{
//    //determine type of area pointed by mouse and classify it. Also collect information
//    //about current staff for point pointed by mouse and about related BoxSliceInstr.
//    //Returns the pointed box/shape
//    //Save found data in member variables:
//    //          m_pCurShapeStaff
//    //          m_pCurBSI
//    //          m_pCurGMO
//    //          m_nMousePointedArea
//    //          m_rCurGridTime
//
//    m_pCurShapeStaff = (lmShapeStaff*)nullptr;
//    m_pCurBSI = (lmBoxSliceInstr*)nullptr;
//
//    //check if pointing to a shape (not to a box)
//	m_pCurGMO = m_pView->FindShapeAt(m_nNumPage, m_uMousePagePos, true);
//    if (m_pCurGMO)
//    {
//        //pointing to a selectable shape. Get pointed area type
//        if (m_pCurGMO->IsShapeStaff())
//        {
//            m_nMousePointedArea = lmMOUSE_OnStaff;
//            m_pCurShapeStaff = (lmShapeStaff*)m_pCurGMO;
//        }
//        else if (m_pCurGMO->IsShapeNote() || m_pCurGMO->IsShapeRest())
//        {
//            m_nMousePointedArea = lmMOUSE_OnNotesRests;
//        }
//        else
//            m_nMousePointedArea = lmMOUSE_OnOtherShape;
//
//        //get the SliceInstr.
//        GmoObj* pBox = m_pView->FindBoxAt(m_nNumPage, m_uMousePagePos);
//        //AWARE: Returned box is the smallest one containig to mouse point. If point is
//        //only in lmBoxPage nullptr is returned.
//        if (pBox)
//        {
//            if (pBox->IsBoxSliceInstr())
//                m_pCurBSI = (lmBoxSliceInstr*)pBox;
//            else if (pBox->IsBoxSystem())
//            {
//                //empty score. No BoxSliceInstr.
//                //for now assume it is pointing to first staff.
//                //TODO: check if mouse point is over a shape staff and select it
//                //m_pCurShapeStaff = ((lmBoxSystem*)m_pCurGMO)->GetStaffShape(1);
//            }
//            else
//            {
//                wxLogMessage("[CommandEventHandler::GetPointedAreaInfo] Unknown case '%s'",
//                            m_pCurGMO->GetName().wx_str());
//                wxASSERT(false);    //Unknown case. Is it possible??????
//            }
//        }
//
//        //get the ShapeStaff
//        if (m_pCurBSI && !m_pCurShapeStaff)
//        {
//            if (m_pCurGMO->GetScoreOwner()->IsStaffObj())
//            {
//                int nStaff = ((lmStaffObj*)m_pCurGMO->GetScoreOwner())->GetStaffNum();
//                m_pCurShapeStaff = m_pCurBSI->GetStaffShape(nStaff);
//            }
//            else if (m_pCurGMO->GetScoreOwner()->IsAuxObj())
//            {
//                lmStaff* pStaff = ((lmAuxObj*)m_pCurGMO->GetScoreOwner())->GetStaff();
//                if (pStaff)
//                    m_pCurShapeStaff = m_pCurBSI->GetStaffShape( pStaff->GetNumberOfStaff() );
//            }
//        }
//    }
//
//    //not pointing to a shape
//    else
//    {
//        //check if pointing to a box
//        m_pCurGMO = m_pView->FindBoxAt(m_nNumPage, m_uMousePagePos);
//        if (m_pCurGMO)
//        {
//            if (m_pCurGMO->IsBoxSliceInstr())
//            {
//                m_pCurBSI = (lmBoxSliceInstr*)m_pCurGMO;
//                //determine staff
//                if (m_pLastBSI != m_pCurBSI)
//                {
//                    //first time on this BoxInstrSlice, between two staves
//                    m_pCurShapeStaff = m_pCurBSI->GetNearestStaff(m_uMousePagePos);
//                }
//                else
//                {
//                    //continue in this BoxInstrSlice, in same inter-staves area
//                    m_pCurShapeStaff = m_pLastShapeStaff;
//                }
//                //determine position (above/below) relative to staff
//                if (m_uMousePagePos.y > m_pCurShapeStaff->GetBounds().GetLeftBottom().y)
//                    m_nMousePointedArea = lmMOUSE_OnBelowStaff;
//                else
//                    m_nMousePointedArea = lmMOUSE_OnAboveStaff;
//            }
//            else
//                m_nMousePointedArea = lmMOUSE_OnOtherBox;
//        }
//        else
//            m_nMousePointedArea = lmMOUSE_OnOther;
//    }
//
//    //determine timepos at mouse point, by using time grid
//    if (m_pCurBSI)
//    {
//        lmBoxSlice* pBSlice = (lmBoxSlice*)m_pCurBSI->GetParentBox();
//        m_rCurGridTime = pBSlice->GetGridTimeForPosition(m_uMousePagePos.x);
//        GetMainFrame()->SetStatusBarMouseData(m_nNumPage, m_rCurGridTime,
//                                              pBSlice->GetNumMeasure(),
//                                              m_uMousePagePos);
//    }
//    ////DBG --------------------------------------
//    //wxString sSO = (m_pCurGMO ? m_pCurGMO->GetName() : "No object");
//    //wxLogMessage("[CommandEventHandler::GetPointedAreaInfo] LastBSI=0x%x, CurBSI=0x%x, LastStaff=0x%x, CurStaff=0x%x, Area=%d, Object=%s",
//    //             m_pLastBSI, m_pCurBSI, m_pLastShapeStaff, m_pCurShapeStaff,
//    //             m_nMousePointedArea, sSO.wx_str() );
//    ////END DBG ----------------------------------
//
//}
//
//wxMenu* CommandEventHandler::GetContextualMenuForTool()
//{
//	ToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
//	if (!pToolBox)
//        return (wxMenu*)nullptr;
//
//	return pToolBox->GetContextualMenuForSelectedPage();
//}
//
//void CommandEventHandler::StartToolDrag(wxDC* pDC)
//{
//    PrepareToolDragImages();
//    wxBitmap* pCursorDragImage = (wxBitmap*)nullptr;
//    if (m_pToolBitmap)
//    {
//        pCursorDragImage = new wxBitmap(*m_pToolBitmap);
//        m_vDragHotSpot = m_vToolHotSpot;
//
//        //wxLogMessage("[CommandEventHandler::StartToolDrag] OnImageBeginDrag. m_nMousePointedArea=%d, MousePagePos=(%.2f, %.2f)",
//        //                m_nMousePointedArea, m_uMousePagePos.x, m_uMousePagePos.y);
//
//        m_fDraggingTool = true;
//    }
//    else
//        m_fDraggingTool = false;
//
//    m_pView->OnImageBeginDrag(true, pDC, m_vCanvasOffset, m_uMousePagePos,
//                            (GmoObj*)nullptr, m_vDragHotSpot, m_uHotSpotShift,
//                            pCursorDragImage );
//}
//
//void CommandEventHandler::ContinueToolDrag(wxMouseEvent& event, wxDC* pDC)
//{
//    //wxLogMessage("[CommandEventHandler::ContinueToolDrag] OnImageContinueDrag. m_nMousePointedArea=%d, MousePagePos=(%.2f, %.2f)",
//    //                m_nMousePointedArea, m_uMousePagePos.x, m_uMousePagePos.y);
//
//    m_pView->OnImageContinueDrag(event, true, pDC, m_vCanvasOffset,
//                                 m_uMousePagePos, m_vMouseCanvasPos);
//}
//
//void CommandEventHandler::TerminateToolDrag(wxDC* pDC)
//{
//    //wxLogMessage("[CommandEventHandler::TerminateToolDrag] Terminate drag. m_nMousePointedArea=%d, MousePagePos=(%.2f, %.2f)",
//    //                m_nMousePointedArea, m_uMousePagePos.x, m_uMousePagePos.y);
//
//    if (!m_fDraggingTool)
//        return;
//
//    m_pView->OnImageEndDrag(true, pDC, m_vCanvasOffset, m_uMousePagePos);
//    m_fDraggingTool = false;
//}
//
//void CommandEventHandler::TerminateToolDrag()
//{
//    // Set a DC in logical units and scaled, so that
//    // transformations logical/device and viceversa can be computed
//    wxClientDC dc(this);
//    m_pView->ScaleDC(&dc);
//    TerminateToolDrag(&dc);
//}
//
//void CommandEventHandler::StartToolDrag()
//{
//    // Set a DC in logical units and scaled, so that
//    // transformations logical/device and viceversa can be computed
//    wxClientDC dc(this);
//    m_pView->ScaleDC(&dc);
//    StartToolDrag(&dc);
//}
//
////------------------------------------------------------------------------------------------
////call backs from lmScoreView to paint marks for mouse dragged tools.
////
////  - pPaper is already prepared for dirct XOR paint and is scaled and the origin set.
////  - uPos is the mouse current position and they must return the nearest
////    valid notehead position
////------------------------------------------------------------------------------------------
//
//UPoint CommandEventHandler::OnDrawToolMarks(lmPaper* pPaper, const UPoint& uPos)
//{
//    UPoint uFinalPos = uPos;
//
//    //draw ledger lines
//    if (RequiresLedgerLines())
//    {
//        if (m_pCurShapeStaff)
//            uFinalPos = m_pCurShapeStaff->OnMouseStartMoving(pPaper, uPos);
//    }
//
//    ////show tool tip
//    //m_pInfoWindow = new lmInfoWindow(this);
//    //wxSize sz = m_pInfoWindow->GetSize();
//    //wxPoint pos(m_vMouseCanvasPos.x + m_vCanvasOffset.x,
//    //            m_vMouseCanvasPos.y + m_vCanvasOffset.y );
//    //m_pInfoWindow->Position(pos, sz );
//    //m_pInfoWindow->Popup();
//    //this->SetFocus();
//
//
//
//    //draw time grid
//    if (m_pCurBSI && RequiresTimeGrid())
//        m_pCurBSI->DrawTimeGrid(pPaper);
//
//    //draw measure frame
//    if (m_pCurBSI && RequiresMeasureFrame())
//        m_pCurBSI->DrawMeasureFrame(pPaper);
//
//    return uFinalPos;
//}
//
//UPoint CommandEventHandler::OnRedrawToolMarks(lmPaper* pPaper, const UPoint& uPos)
//{
//    UPoint uFinalPos = uPos;
//
//    //remove and redraw ledger lines
//    if (RequiresLedgerLines())
//    {
//        if (m_pLastShapeStaff && m_pLastShapeStaff != m_pCurShapeStaff)
//        {
//            m_pLastShapeStaff->OnMouseEndMoving(pPaper, uPos);
//            if (m_pCurShapeStaff)
//                uFinalPos = m_pCurShapeStaff->OnMouseStartMoving(pPaper, uPos);
//        }
//        else if (m_pCurShapeStaff)
//            uFinalPos = m_pCurShapeStaff->OnMouseMoving(pPaper, uPos);
//    }
//
//    //remove previous grid
//    if (RequiresTimeGrid())
//    {
//        if (m_pLastBSI && m_pLastBSI != m_pCurBSI)
//            m_pLastBSI->DrawTimeGrid(pPaper);
//
//        //draw new grid
//        if (m_pCurBSI && (!m_pLastBSI || m_pLastBSI != m_pCurBSI))
//            m_pCurBSI->DrawTimeGrid(pPaper);
//    }
//
//    //remove previous measure frame
//    if (RequiresMeasureFrame())
//    {
//        if (m_pLastBSI && m_pLastBSI != m_pCurBSI)
//            m_pLastBSI->DrawMeasureFrame(pPaper);
//
//        //draw measure frame
//        if (m_pCurBSI && (!m_pLastBSI || m_pLastBSI != m_pCurBSI))
//            m_pCurBSI->DrawMeasureFrame(pPaper);
//    }
//
//    return uFinalPos;
//}
//
//UPoint CommandEventHandler::OnRemoveToolMarks(lmPaper* pPaper, const UPoint& uPos)
//{
//    UPoint uFinalPos = uPos;
//
//    //remove ledger lines
//    if (RequiresLedgerLines())
//    {
//        if (m_pLastShapeStaff && m_pLastShapeStaff != m_pCurShapeStaff)
//            m_pLastShapeStaff->OnMouseEndMoving(pPaper, uPos);
//        else if (m_pCurShapeStaff)
//            m_pCurShapeStaff->OnMouseEndMoving(pPaper, uPos);
//    }
//
//    //remove previous time grid
//    if (m_pLastBSI && RequiresTimeGrid())
//        m_pLastBSI->DrawTimeGrid(pPaper);
//
//    //remove previous measure frame
//    if (m_pLastBSI && RequiresMeasureFrame())
//        m_pLastBSI->DrawMeasureFrame(pPaper);
//
//    return uFinalPos;
//}
//
//
//
////------------------------------------------------------------------------------------------
//
//void CommandEventHandler::ChangeMouseIcon()
//{
//    //change mouse icon if necessary. Type of area currently pointed by mouse is in global variable
//    //m_nMousePointedArea.
//
//    //determine needed icon
//    wxCursor* pNeeded = m_pCursorElse;
//    if (m_nMousePointedArea & m_nValidAreas)
//        pNeeded = m_pCursorOnValidArea;
//
//    //get current cursor
//    if (m_pCursorCurrent != pNeeded)
//    {
//        m_pCursorCurrent = pNeeded;
//        const wxCursor& oCursor = *pNeeded;
//        SetCursor( oCursor );
//    }
//}
//
//void CommandEventHandler::PlayScore(bool fFromCursor, bool fCountOff)
//{
//    //get the score
//    lmScore* pScore = m_pDoc->GetScore();
//
//	//determine measure from cursor or start of selection
//	int nMeasure = 1;
//	lmGMSelection* pSel = m_pView->GetSelection();
//	bool fFromMeasure = fFromCursor || pSel->NumObjects() > 0;
//	if (pSel->NumObjects() > 0)
//	{
//		nMeasure = ((ImoNote*)pSel->GetFirst()->GetScoreOwner())->GetSegment()->GetNumSegment() + 1;
//		m_pView->DeselectAllGMObjects(true);	//redraw, to remove selection highlight
//	}
//	else
//		nMeasure = m_pView->GetCursorMeasure();
//
//	//play back the score
//	if (fFromMeasure)
//		pScore->PlayFromMeasure(nMeasure, lmVISUAL_TRACKING, fCountOff,
//                                ePM_NormalInstrument, 0, this);
//	else
//		pScore->Play(lmVISUAL_TRACKING, fCountOff, ePM_NormalInstrument, 0, this);
//}
//
//void CommandEventHandler::StopPlaying(bool fWait)
//{
//    //get the score
//    lmScore* pScore = m_pDoc->GetScore();
//    if (!pScore) return;
//
//    //request it to stop playing
//    pScore->Stop();
//    if (fWait)
//        pScore->WaitForTermination();
//}
//
//void CommandEventHandler::PausePlaying()
//{
//    //get the score
//    lmScore* pScore = m_pDoc->GetScore();
//
//    //request it to pause playing
//    pScore->Pause();
//}
//
//void CommandEventHandler::OnVisualHighlight(lmVisualTrackingEvent& event)
//{
//    m_pView->OnVisualHighlight(event);
//}
//




////---------------------------------------------------------------------------------------
//
//wxCursor* CommandEventHandler::LoadMouseCursor(wxString sFile, int nHotSpotX, int nHotSpotY)
//{
//    //loads all mouse cursors used in CommandEventHandler
//
//    wxString sPath = g_pPaths->GetCursorsPath();
//    wxFileName oFilename(sPath, sFile, wxPATH_NATIVE);
//    wxCursor* pCursor;
//
//    //load image
//    wxImage oImage(oFilename.GetFullPath(), wxBITMAP_TYPE_PNG);
//    if (!oImage.IsOk())
//    {
//        wxLogMessage("[CommandEventHandler::LoadMouseCursor] Failure loading mouse cursor image '%s'",
//                     oFilename.GetFullPath().wx_str());
//        return nullptr;
//    }
//
//    //set hot spot point
//    oImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, nHotSpotX);
//    oImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, nHotSpotY);
//
//    //create the cursor
//    pCursor = new wxCursor(oImage);
//    if (!pCursor->IsOk())
//    {
//        wxLogMessage("[CommandEventHandler::LoadMouseCursor] Failure creating mouse cursor from image '%s'",
//                     oFilename.GetFullPath().wx_str());
//        delete pCursor;
//        return nullptr;
//    }
//
//    return pCursor;
//}
//
//void CommandEventHandler::LoadAllMouseCursors()
//{
//    //loads all mouse cursors used in CommandEventHandler
//
//    //AWARE: Cursors MUST BE LOADED in the same order than enum lmEMouseCursor.
//    //Therefore, this has been implemented as a 'for' loop, to ensure previous
//    //requirement and simplify code maintenance
//
//    wxCursor* pCursor;
//    for (int iCursor = 0; iCursor < (int)lm_eCursor_Max; iCursor++)
//    {
//        switch((lmEMouseCursor)iCursor)
//        {
//            case lm_eCursor_Pointer:
//                pCursor = new wxCursor(wxCURSOR_ARROW);
//                break;
//            case lm_eCursor_Cross:
//                pCursor = new wxCursor(wxCURSOR_CROSS);
//                break;
//            case lm_eCursor_BullsEye:
//                pCursor = new wxCursor(wxCURSOR_BULLSEYE);
//                break;
//            case lm_eCursor_Hand:
//                pCursor = new wxCursor(wxCURSOR_HAND);
//                break;
//            case lm_eCursor_Note:
//                pCursor = LoadMouseCursor("cursor-note.png", 8, 8);
//                break;
//            case lm_eCursor_Note_Forbidden:
//                pCursor = new wxCursor(wxCURSOR_NO_ENTRY);
//                break;
//            default:
//                wxLogMessage("[CommandEventHandler::LoadAllMouseCursors] Missing value (%d) in swith statement",
//                             iCursor);
//                pCursor = new wxCursor(wxCURSOR_ARROW);
//        }
//        m_MouseCursors.push_back(pCursor);
//    }
//
//    ////lm_eCursor_Pointer
//    //wxCursor* pCursor = new wxCursor(wxCURSOR_BULLSEYE );   //wxCURSOR_ARROW);
//    //m_MouseCursors.push_back(pCursor);
//
//    ////lm_eCursor_Cross
//    //pCursor = new wxCursor(wxCURSOR_CROSS);
//    //m_MouseCursors.push_back(pCursor);
//
//    ////lm_eCursor_BullsEye
//    //wxCursor* pCursor = new wxCursor(wxCURSOR_BULLSEYE );   //wxCURSOR_ARROW);
//    //m_MouseCursors.push_back(pCursor);
//
//    //// lm_eCursor_Note
//    //m_MouseCursors.push_back( LoadMouseCursor("cursor-note.png", 8, 8) );
//
//    //// lm_eCursor_Note_Forbidden,
//    //pCursor = new wxCursor(wxCURSOR_NO_ENTRY);
//    //m_MouseCursors.push_back(pCursor);
//    ////m_MouseCursors.push_back( LoadMouseCursor("cursor-note-forbidden.png", 8, 8) );
//
//
//    //set default cursors
//    m_pCursorOnSelectedObject = GetMouseCursor(lm_eCursor_Pointer);
//    m_pCursorOnValidArea = GetMouseCursor(lm_eCursor_Pointer);
//    m_pCursorElse = GetMouseCursor(lm_eCursor_Pointer);
//
//}
//
//wxCursor* CommandEventHandler::GetMouseCursor(lmEMouseCursor nCursorID)
//{
//    return m_MouseCursors[nCursorID];
//}
//
//void CommandEventHandler::UpdateValidAreasAndMouseIcons()
//{
//    //Determine valid areas and cursor icons. This will depend on selected tool
//
//    //default values
//    lmEMouseCursor nSelected = lm_eCursor_Pointer;
//    lmEMouseCursor nValidArea = lm_eCursor_Pointer;
//    lmEMouseCursor nElse = lm_eCursor_Pointer;
//
//
//    if (mouseMode == k_mouse_mode_data_entry)
//    {
//        //Determine valid areas and cursor icons. This will depend on the
//        //selected tool
//        switch(pageID)
//        {
//            case k_page_clefs:
//                switch (groupID)
//                {
//                    case k_grp_ClefType:
//                        m_nValidAreas = lmMOUSE_OnStaff;
//                        break;
//                    case k_grp_TimeType:
//                    case k_grp_KeyType:
//                    default:
//                        m_nValidAreas = lmMOUSE_OnStaff | lmMOUSE_OnNotesRests;
//                }
//                nValidArea = lm_eCursor_Hand;
//                nSelected = lm_eCursor_Pointer;
//                nElse = lm_eCursor_Note_Forbidden;
//                break;
//
//            case k_page_notes:
//                m_nValidAreas = lmMOUSE_OnStaff | lmMOUSE_OnAboveStaff | lmMOUSE_OnBelowStaff;
//                nSelected = lm_eCursor_Pointer;
//                nValidArea = lm_eCursor_Pointer;
//                nElse = lm_eCursor_Note_Forbidden;
//                break;
//
//            case k_page_barlines:
//                m_nValidAreas = lmMOUSE_OnStaff;
//                nValidArea = lm_eCursor_Hand;
//                nSelected = lm_eCursor_Pointer;
//                nElse = lm_eCursor_Note_Forbidden;
//                break;
//
//            case k_page_sysmbols:
//                m_nValidAreas = lmMOUSE_OnNotesRests;
//                nSelected = lm_eCursor_Pointer;
//                nValidArea = lm_eCursor_Hand;
//                nElse = lm_eCursor_Note_Forbidden;
//                break;
//
//            default:
//                wxASSERT(false);
//        }
//
//        //hide caret
//        m_pView->CaretOff();
//    }
//    else
//    {
//        m_nValidAreas = lmMOUSE_OnAny;
//        //show caret
//        m_pView->CaretOn();
//    }
//
//    //set cursors
//    m_pCursorOnSelectedObject = GetMouseCursor(nSelected);
//    m_pCursorOnValidArea = GetMouseCursor(nValidArea);
//    m_pCursorElse = GetMouseCursor(nElse);
//    ChangeMouseIcon();
//}
//
//
//
//void CommandEventHandler::UpdateStatusBarToolBox(wxString sMoreInfo)
//{
//    //update status bar: mouse mode and selected tool
//
//    wxString sMsg = "";
//    if (mouseMode == k_mouse_mode_pointer)
//        sMsg = _("Pointer mode");
//    else if (mouseMode == k_mouse_mode_data_entry)
//    {
//	    ToolBox* pToolBox = GetMainFrame()->GetActiveToolBox();
//	    wxASSERT(pToolBox);
//        sMsg = pToolBox->GetToolShortDescription();
//        sMsg += sMoreInfo;
//    }
//
//    GetMainFrame()->SetStatusBarMsg(sMsg);
//}

//void CommandEventHandler::PrepareToolDragImages()
//{
//    //prepare drag image for current selected tool.
//    //This method must set variables m_pToolBitmap and m_vToolHotSpot
//    //If m_pToolBitmap is set to nullptr it will imply that no drag image will be used.
//
//    //TODO: It should be responsibility of each tool to provide this information. Move
//    //this code to each ToolPage in the ToolBox. Later,move to each tool group (not to
//    //options groups)
//
//    //TODO: Drag image is related to mouse cursor to use. Should we merge this method with
//    //the one used to determine mouse cursor to use? --> Yes, as hotspot position depends
//    //on chosen mouse cursor
//
//
//    //delete previous bitmaps
//    if (m_pToolBitmap)
//        delete m_pToolBitmap;
//
//    //create new ones
//    wxColour colorF = *wxBLUE;
//    wxColour colorB = *wxWHITE;
//    lmStaff* pStaff = m_pView->GetDocument()->GetScore()->GetFirstInstrument()->GetVStaff()->GetFirstStaff();
//    double rPointSize = pStaff->GetMusicFontSize();
//    double rScale = m_pView->GetScale() * lmSCALE;
//
//    //Determine glyph to use for current tool. Value GLYPH_NONE means "Do not use a drag image".
//    //And in this case only mouse cursor will be used
//    lmEGlyphIndex nGlyph = GLYPH_NONE;      //Default: Do not use a drag image
//    switch (pageID)
//    {
//        case k_page_notes:
//            {
//                //in 'TheoHarmonyCtrol' edit mode, force stem depending on voice
//                bool fStemDown = false;
//                lmEditorMode* pEditorMode = m_pDoc->GetEditMode();
//                if (pEditorMode && pEditorMode->GetModeName() == "TheoHarmonyCtrol")
//                {
//                    switch(m_nSelVoice)
//                    {
//                        case 1: fStemDown = false;  break;
//                        case 2: fStemDown = true;   break;
//                        case 3: fStemDown = false;  break;
//                        case 4: fStemDown = true;   break;
//                        default:
//                            fStemDown = false;
//                    }
//                }
//
//                //select glyph
//                nGlyph = lmGetGlyphForNoteRest(m_nSelNoteType, m_fSelIsNote, fStemDown);
//                break;
//            }
//
//        case k_page_sysmbols:
//            {
//                switch (m_nToolID)
//                {
//                    case lmTOOL_FIGURED_BASS:
//                    case lmTOOL_TEXT:
//                    case lmTOOL_LINES:
//                    case lmTOOL_TEXTBOX:
//                        nGlyph = GLYPH_NONE;    //GLYPH_TOOL_GENERIC;
//                        break;
//                    default:
//                        nGlyph = GLYPH_NONE;    //GLYPH_TOOL_GENERIC;
//                }
//                break;
//            }
//
//        case k_page_clefs:
//            {
//                switch (groupID)
//                {
//                    case k_grp_ClefType:
//                        nGlyph = lmGetGlyphForCLef(m_nClefType);
//                        break;
//                    case k_grp_TimeType:
//                    case k_grp_KeyType:
//                        nGlyph = GLYPH_NONE;    //GLYPH_TOOL_GENERIC;
//                        break;
//                    default:
//                        nGlyph = GLYPH_NONE;    //GLYPH_TOOL_GENERIC;
//                }
//                break;
//            }
//
//        case k_page_barlines:
//            {
//                nGlyph = GLYPH_NONE;    //GLYPH_TOOL_GENERIC;
//                break;
//            }
//    }
//
//    //create the bitmap
//    if (nGlyph != GLYPH_NONE)
//    {
//        m_pToolBitmap = GetBitmapForGlyph(rScale, nGlyph, rPointSize, colorF, colorB);
//        float rxScale = aGlyphsInfo[nGlyph].txDrag / aGlyphsInfo[nGlyph].thWidth;
//        float ryScale = aGlyphsInfo[nGlyph].tyDrag / aGlyphsInfo[nGlyph].thHeight;
//        m_vToolHotSpot = lmDPoint(m_pToolBitmap->GetWidth() * rxScale,
//                                m_pToolBitmap->GetHeight() * ryScale );
//    }
//    else
//    {
//        //No drag image. Only mouse cursor
//        m_pToolBitmap = (wxBitmap*)nullptr;
//    }
//
//}
//
//void CommandEventHandler::UpdateToolInfoString()
//{
//    //Add note pitch in status bar inofo
//    if (!(pageID == k_page_notes && m_fSelIsNote) || m_pCurShapeStaff == nullptr)
//        return;
//
//    lmDPitch dpNote = GetNotePitchFromPosition(m_pCurShapeStaff, m_uMousePagePos);
//    wxString sMoreInfo = wxString::Format(" %s", DPitch_ToLDPName(dpNote).wx_str() );
//    UpdateStatusBarToolBox(sMoreInfo);
//}
//
//lmDPitch CommandEventHandler::GetNotePitchFromPosition(lmShapeStaff* pShapeStaff, UPoint uPagePos)
//{
//    //return pitch for specified mouse position on staff
//
//    //get step and octave from mouse position on staff
//    int nLineSpace = pShapeStaff->GetLineSpace(uPagePos.y);     //0=first ledger line below staff
//    //to determine octave and step it is necessary to know the clef. As caret is
//    //placed at insertion point we could get these information from caret
//    lmContext* pContext = m_pDoc->GetScore()->GetCursor()->GetCurrentContext();
//    lmEClefType nClefType = (pContext ? pContext->GetClefType() : k_clef_undefined);
//    if (nClefType == k_clef_undefined)
//        nClefType = ((lmStaff*)(pShapeStaff->GetScoreOwner()))->GetDefaultClef();
//    lmDPitch dpNote = ::GetFirstLineDPitch(nClefType);  //get diatonic pitch for first line
//    dpNote += (nLineSpace - 2);     //pitch for note to insert
//
//    return dpNote;
//}
//
//void CommandEventHandler::OnKeyDown(wxKeyEvent& event)
//{
//    //wxLogMessage("EVT_KEY_DOWN");
//    switch ( event.GetKeyCode() )
//    {
//        case WXK_SHIFT:
//        case WXK_ALT:
//        case WXK_CONTROL:
//            break;      //do nothing
//
//        default:
//            //save key down info
//            m_nKeyDownCode = event.GetKeyCode();
//            m_fShift = event.ShiftDown();
//            m_fAlt = event.AltDown();
//            m_fCmd = event.CmdDown();
//
//            //If a key down (EVT_KEY_DOWN) event is caught and the event handler does not
//            //call event.Skip() then the corresponding char event (EVT_CHAR) will not happen.
//            //This is by design of wxWidgets and enables the programs that handle both types of
//            //events to be a bit simpler.
//
//            //event.Skip();       //to generate Key char event
//            process_key(event);
//    }
//}
//
//void CommandEventHandler::OnKeyPress(wxKeyEvent& event)
//{
//    //wxLogMessage("[CommandEventHandler::OnKeyPress] KeyCode=%s (%d), KeyDown data: Keycode=%s (%d), (flags = %c%c%c%c)",
//    //        KeyCodeToName(event.GetKeyCode()).wx_str(), event.GetKeyCode(),
//    //        KeyCodeToName(m_nKeyDownCode).wx_str(), m_nKeyDownCode,
//    //        (m_fCmd ? _T('C') : _T('-') ),
//    //        (m_fAlt ? _T('A') : _T('-') ),
//    //        (m_fShift ? _T('S') : _T('-') ),
//    //        (event.MetaDown() ? _T('M') : _T('-') )
//    //        );
//    //process_key(event);
//}



//void CommandEventHandler::SetDraggingObject(bool fValue)
//{
//    if (m_fDraggingObject != fValue)
//    {
//        //change of state. Capture or release mouse
//        if (m_fDraggingObject)
//            ReleaseTheMouse();
//        else
//            CaptureTheMouse();
//    }
//
//    m_fDraggingObject = fValue;
//}
//
////dragging on canvas with left button: selection
//void CommandEventHandler::OnCanvasBeginDragLeft(lmDPoint vCanvasPos, UPoint uPagePos,
//                                          int nKeys)
//{
//    //Begin a selection with left button
//
//	WXUNUSED(nKeys);
//
//    wxClientDC dc(this);
//	dc.SetLogicalFunction(wxINVERT);
//    SetDraggingObject(false);
//
//	m_pView->DrawSelectionArea(dc, m_vStartDrag.x, m_vStartDrag.y, vCanvasPos.x, vCanvasPos.y);
//}
//
//void CommandEventHandler::OnCanvasContinueDragLeft(bool fDraw, lmDPoint vCanvasPos,
//                                             UPoint uPagePos, int nKeys)
//{
//    //Continue a selection with left button
//	//fDraw:  true -> draw a rectangle, false -> remove rectangle
//
//    WXUNUSED(fDraw);
//    WXUNUSED(nKeys);
//
//    wxClientDC dc(this);
//    dc.SetLogicalFunction(wxINVERT);
//    SetDraggingObject(false);
//
//    m_pView->DrawSelectionArea(dc, m_vStartDrag.x, m_vStartDrag.y, vCanvasPos.x, vCanvasPos.y);
//}
//
//void CommandEventHandler::OnCanvasEndDragLeft(lmDPoint vCanvasPos, UPoint uPagePos,
//                                        int nKeys)
//{
//    //End a selection with left button
//
//    WXUNUSED(nKeys);
//
//	//remove selection rectangle
//    //dc.SetLogicalFunction(wxINVERT);
//    //DrawSelectionArea(dc, m_vStartDrag.x, m_vStartDrag.y, vCanvasPos.x, vCanvasPos.y);
//    SetDraggingObject(false);
//
//	//save final point
//	m_vEndDrag = vCanvasPos;
//
//    //select all objects within the selection area
//    lmLUnits uXMin, uXMax, uYMin, uYMax;
//    uXMin = wxMin(uPagePos.x, m_uStartDrag.x);
//    uXMax = wxMax(uPagePos.x, m_uStartDrag.x);
//    uYMin = wxMin(uPagePos.y, m_uStartDrag.y);
//    uYMax = wxMax(uPagePos.y, m_uStartDrag.y);
//
//    //find all objects whithin the selected area and create a selection
//    //
//    //TODO
//    //  The selected area could cross page boundaries. Therefore it is necessary
//    //  to locate the affected pages and invoke CreateSelection / AddToSelecction
//    //  for each affected page
//    //
//    if (nKeys == lmKEY_NONE)
//    {
//        m_pView->SelectGMObjectsInArea(m_nNumPage, uXMin, uXMax, uYMin, uYMax, true);     //true: redraw view content
//    }
//    //else if (nKeys & lmKEY_CTRL)
//    //{
//    //    //find all objects in drag area and add them to 'selection'
//    //    m_graphMngr.AddToSelection(m_nNumPage, uXMin, uXMax, uYMin, uYMax);
//    //    //mark as 'selected' all objects in the selection
//    //    m_pCanvas->SelectObjects(lmSELECT, m_graphMngr.GetSelection());
//    //}
//}
//
////dragging on canvas with right button
//void CommandEventHandler::OnCanvasBeginDragRight(lmDPoint vCanvasPos, UPoint uPagePos,
//                                           int nKeys)
//{
//    WXUNUSED(vCanvasPos);
//    WXUNUSED(uPagePos);
//    WXUNUSED(nKeys);
//    SetDraggingObject(false);
//}
//
//void CommandEventHandler::OnCanvasContinueDragRight(bool fDraw, lmDPoint vCanvasPos,
//                                              UPoint uPagePos, int nKeys)
//{
//    WXUNUSED(fDraw);
//    WXUNUSED(vCanvasPos);
//    WXUNUSED(uPagePos);
//    WXUNUSED(nKeys);
//    SetDraggingObject(false);
//}
//
//void CommandEventHandler::OnCanvasEndDragRight(lmDPoint vCanvasPos, UPoint uPagePos,
//                                         int nKeys)
//{
//    WXUNUSED(vCanvasPos);
//    WXUNUSED(uPagePos);
//    WXUNUSED(nKeys);
//
//    SetDraggingObject(false);
//    SetFocus();
//}
//
//
////dragging object with left button
//void CommandEventHandler::OnObjectBeginDragLeft(wxMouseEvent& event, wxDC* pDC,
//                                          lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
//                                          UPoint uPagePos, int nKeys)
//{
//    SetDraggingObject(true);
//    if (!m_pView->OnObjectBeginDragLeft(event, pDC, vCanvasPos, vCanvasOffset,
//                                        uPagePos, nKeys, m_pDraggedGMO,
//                                        m_vDragHotSpot, m_uHotSpotShift) )
//    {
//        m_nDragState = lmDRAG_NONE;
//        SetDraggingObject(false);
//    }
//}
//
//void CommandEventHandler::OnObjectContinueDragLeft(wxMouseEvent& event, wxDC* pDC,
//                                             bool fDraw, lmDPoint vCanvasPos,
//                                             lmDPoint vCanvasOffset, UPoint uPagePos,
//                                             int nKeys)
//{
//    SetDraggingObject(true);
//    m_pView->OnObjectContinueDragLeft(event, pDC, fDraw, vCanvasPos,
//                                      vCanvasOffset, uPagePos, nKeys);
//}
//
//void CommandEventHandler::OnObjectEndDragLeft(wxMouseEvent& event, wxDC* pDC,
//                                        lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
//                                        UPoint uPagePos, int nKeys)
//{
//    m_pView->OnObjectEndDragLeft(event, pDC, vCanvasPos, vCanvasOffset,
//                                 uPagePos, nKeys);
//    SetDraggingObject(false);
//}
//
//
////dragging object with right button
//void CommandEventHandler::OnObjectBeginDragRight(wxMouseEvent& event, wxDC* pDC,
//                                           lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
//                                           UPoint uPagePos, int nKeys)
//{
//    WXUNUSED(event);
//    WXUNUSED(pDC);
//    WXUNUSED(vCanvasPos);
//    WXUNUSED(vCanvasOffset);
//    WXUNUSED(nKeys);
//    WXUNUSED(uPagePos);
//
//    SetDraggingObject(true);
//	m_pView->HideCaret();
//    SetFocus();
//
//	#ifdef _LM_DEBUG_
//	g_pLogger->LogTrace("OnMouseEvent", "OnObjectBeginDragRight()");
//	#endif
//
//}
//
//void CommandEventHandler::OnObjectContinueDragRight(wxMouseEvent& event, wxDC* pDC,
//                                              bool fDraw, lmDPoint vCanvasPos,
//                                              lmDPoint vCanvasOffset,
//                                              UPoint uPagePos, int nKeys)
//{
//    WXUNUSED(event);
//    WXUNUSED(pDC);
//    WXUNUSED(fDraw);
//    WXUNUSED(vCanvasPos);
//    WXUNUSED(vCanvasOffset);
//    WXUNUSED(uPagePos);
//    WXUNUSED(nKeys);
//
//	#ifdef _LM_DEBUG_
//	g_pLogger->LogTrace("OnMouseEvent", "OnObjectContinueDragRight()");
//	#endif
//
//    SetDraggingObject(true);
//}
//
//void CommandEventHandler::OnObjectEndDragRight(wxMouseEvent& event, wxDC* pDC,
//                                         lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
//                                         UPoint uPagePos, int nKeys)
//{
//    WXUNUSED(event);
//    WXUNUSED(pDC);
//    WXUNUSED(vCanvasPos);
//    WXUNUSED(vCanvasOffset);
//    WXUNUSED(uPagePos);
//    WXUNUSED(nKeys);
//
//	#ifdef _LM_DEBUG_
//	g_pLogger->LogTrace("OnMouseEvent", "OnObjectEndDragRight()");
//	#endif
//
//	m_pView->ShowCaret();
//    SetDraggingObject(false);
//}
//
//void CommandEventHandler::MoveCursorTo(lmBoxSliceInstr* pBSI, int nStaff, float rTime,
//                                 bool fEndOfTime)
//{
//    //Move cursor to specified position
//
//    if (pBSI)
//    {
//	    lmVStaff* pVStaff = pBSI->GetInstrument()->GetVStaff();
//	    int nMeasure = pBSI->GetNumMeasure();
//        m_pView->MoveCursorTo(pVStaff, nStaff, nMeasure, rTime, fEndOfTime);
//    }
//    else
//    {
//        //empty score. Move to start
//        //TODO
//    }
//}
//
//void CommandEventHandler::MoveCursorNearTo(lmBoxSliceInstr* pBSI, UPoint uPagePos, int nStaff)
//{
//    //Move cursor to nearest object after position uPagePos, constrained to specified
//    //segment (specified by BoxSliceInstr) and staff. This method is mainly to position
//    //cursor at mouse click point
//
//    if (pBSI)
//    {
//	    lmVStaff* pVStaff = pBSI->GetInstrument()->GetVStaff();
//	    int nMeasure = pBSI->GetNumMeasure();
//	    m_pView->MoveCursorNearTo(uPagePos, pVStaff, nStaff, nMeasure);
//    }
//    else
//    {
//        //empty score. Move to start
//        //TODO
//    }
//}
//
////non-dragging events: click on an object
//void CommandEventHandler::OnLeftClickOnObject(GmoObj* pGMO, lmDPoint vCanvasPos,
//                                        UPoint uPagePos, int nKeys)
//{
//    // mouse left click on object
//    // uPagePos: click point, referred to current page origin
//
//    WXUNUSED(vCanvasPos);
//    WXUNUSED(nKeys);
//
//    //AWARE: pGMO must exist and it must be a shape. It can not be an lmBox as, since renderization
//    //of shapes is organized in layers, lmBoxes are no longer taken into account in hit testing.
//    wxASSERT(pGMO && pGMO->IsShape());
//
//
//	m_pView->HideCaret();
//
//	#ifdef _LM_DEBUG_
//	g_pLogger->LogTrace("OnMouseEvent", "OnLeftClickOnObject()");
//	#endif
//
//    m_pView->DeselectAllGMObjects(true);
//    SetFocus();
//
//    if (pGMO->IsShapeStaff())
//    {
//	    //Click on a staff. Move cursor to that staff and nearest note/rest to click point
//        lmShapeStaff* pSS = (lmShapeStaff*)pGMO;
//        lmBox* pBox = m_pView->FindBoxAt(m_nNumPage, uPagePos);
//        wxASSERT(pBox && pBox->IsBoxSliceInstr());
//        lmBoxSliceInstr* pBSI = (lmBoxSliceInstr*)pBox;
//	    lmVStaff* pVStaff = pBSI->GetInstrument()->GetVStaff();
//	    int nMeasure = pBSI->GetNumMeasure();
//	    int nStaff = pSS->GetNumStaff();
//	    m_pView->MoveCaretNearTo(uPagePos, pVStaff, nStaff, nMeasure);
//    }
//    else
//    {
//        //if it is a staffobj move cursor to it. Else do nothing
//        //wxLogMessage("[CommandEventHandler::OnLeftClickOnObject] Click on shape");
//        lmScoreObj* pSCO = pGMO->GetScoreOwner();
//        if (pSCO->IsComponentObj())
//            m_pView->MoveCaretToObject(pGMO);
//    }
//
//    m_pView->ShowCaret();
//}
//
//void CommandEventHandler::OnRightClickOnObject(GmoObj* pGMO, lmDPoint vCanvasPos,
//                                         UPoint uPagePos, int nKeys)
//{
//    // mouse right click on object: show contextual menu for that object
//
//    WXUNUSED(uPagePos);
//
//	#ifdef _LM_DEBUG_
//	g_pLogger->LogTrace("OnMouseEvent", "OnRightClickOnObject()");
//	#endif
//
//	m_pView->HideCaret();
//    m_pView->DeselectAllGMObjects();
//    SetFocus();
//
//    if (pGMO->IsSelectable())
//        m_pView->SelectGMObject(pGMO, true);     //true: redraw view content
//    pGMO->OnRightClick(this, vCanvasPos, nKeys);
//	m_pView->ShowCaret();
//}
//
//void CommandEventHandler::OnLeftDoubleClickOnObject(GmoObj* pGMO, lmDPoint vCanvasPos,
//                                              UPoint uPagePos, int nKeys)
//{
//    // mouse left double click: Select/deselect the object pointed by mouse
//
//    WXUNUSED(vCanvasPos);
//    WXUNUSED(uPagePos);
//    WXUNUSED(nKeys);
//
//	#ifdef _LM_DEBUG_
//	g_pLogger->LogTrace("OnMouseEvent", "OnLeftDoubleClickOnObject()");
//	#endif
//
//	m_pView->HideCaret();
//    SetFocus();
//
//    //ComponentObjs and other score objects (lmBoxXXXX) has all its measurements
//    //relative to each page start position
//
//    //select/deselect the object
//    if (pGMO->IsSelectable())
//        m_pView->SelectGMObject(pGMO, true);     //true: redraw view content
//
//	m_pView->ShowCaret();
//}
//
//void CommandEventHandler::OnRightDoubleClickOnObject(GmoObj* pGMO, lmDPoint vCanvasPos,
//                                               UPoint uPagePos, int nKeys)
//{
//    // mouse right double click: To be defined
//
//    WXUNUSED(vCanvasPos);
//    WXUNUSED(uPagePos);
//    WXUNUSED(nKeys);
//
//	#ifdef _LM_DEBUG_
//	g_pLogger->LogTrace("OnMouseEvent", "OnRightDoubleClickOnObject()");
//	#endif
//
//	m_pView->HideCaret();
//    m_pView->DeselectAllGMObjects(true);
//    SetFocus();
//	m_pView->ShowCaret();
//}
//
//
////non-dragging events: click on canvas
//void CommandEventHandler::OnRightClickOnCanvas(lmDPoint vCanvasPos, UPoint uPagePos,
//                                         int nKeys)
//{
//    WXUNUSED(uPagePos);
//    WXUNUSED(nKeys);
//
//    m_pView->DeselectAllGMObjects(true);     //true: redraw view content
//
//    lmScore* pScore = m_pDoc->GetScore();
//    pScore->PopupMenu(this, (GmoObj*)nullptr, vCanvasPos);
//}
//
//void CommandEventHandler::OnLeftClickOnCanvas(lmDPoint vCanvasPos, UPoint uPagePos,
//                                        int nKeys)
//{
//    WXUNUSED(vCanvasPos);
//    WXUNUSED(uPagePos);
//    WXUNUSED(nKeys);
//
//    m_pView->DeselectAllGMObjects(true);     //true: redraw view content
//    SetFocus();
//}
//
//void CommandEventHandler::OnViewUpdated()
//{
//    //The view informs that it has updated the display
//
//    //clear mouse information
//    m_nDragState = lmDRAG_NONE;
//	m_vEndDrag = lmDPoint(0, 0);
//	m_vStartDrag.x = 0;
//	m_vStartDrag.y = 0;
//}
//
//void CommandEventHandler::OnNewGraphicalModel()
//{
//    //Called by the view when the graphical model has been recreated.
//    //This implies that any saved pointer to a lmObject is no longer valid.
//    //This method should deal with these pointer.
//
//	m_pDraggedGMO = (GmoObj*)nullptr;	    //object being dragged
//	m_pMouseOverGMO = (GmoObj*)nullptr;	//object on which mouse was flying over
//}


}   // namespace lenmus
