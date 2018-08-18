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

#ifndef __LENMUS_COMMAND_EVENT_HANDLER_H__        //to avoid nested includes
#define __LENMUS_COMMAND_EVENT_HANDLER_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_tool_box.h"
#include "lenmus_tool_group.h"
#include "lenmus_tool_page_barlines.h"

//lomse
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_pitch.h>        //EAccidentals
#include <lomse_events.h>
#include <lomse_interactor.h>
#include <lomse_selections.h>
#include <lomse_document_cursor.h>
using namespace lomse;


//#include <wx/docview.h>
//class wxTipWindow;

namespace lenmus
{

//forward declarations
class DocumentWindow;
class ClickHandler;

//#define lmUNSELECT      false       //remove selection
//#define lmSELECT        true        //select objects
//





//---------------------------------------------------------------------------------------
// Helper class for generating and executing Lomse commands
class CommandGenerator
{
protected:
    DocumentWindow* m_pController;
    SelectionSet* m_selection;
    DocCursor* m_cursor;

public:
    CommandGenerator(DocumentWindow* pController, SelectionSet* selection,
                     DocCursor* cursor);
    ~CommandGenerator() {}


    //add/insert commands
    void add_chord_note(const string& stepLetter, int octave);
    void add_tie();
//	void AddTitle();
//    void AttachNewText(lmComponentObj* pCO);
    void add_tuplet();
    void insert_barline(int barlineType);
    void insert_clef(int clefType, int staff=0);
//	void InsertFiguredBass();
//	void InsertFiguredBassLine();
    void insert_note(string stepLetter, int octave, EAccidentals acc,
                     ENoteType noteType, int dots,
                     int voice, int staff);
    void insert_rest(ENoteType noteType, int dots, int voice, int staff);
    void insert_staffobj(string ldpSrc, string name="");
//    void InsertTimeSignature(int nBeats, int nBeatType, bool fVisible = true);    //for type eTS_Normal
//    void InsertKeySignature(int nFifths, bool fMajor, bool fVisible = true);

    //delete commands
	void delete_staffobj();
    void delete_tie();
    void delete_tuplet();

    //change/move commands
    void change_attribute(ImoObj* pImo, int attrb, int newValue);
    void change_attribute(ImoObj* pImo, int attrb, Color newValue);
    void change_attribute(ImoObj* pImo, int attrb, double newValue);
    void change_attribute(ImoObj* pImo, int attrb, const string& newValue);
//	void ChangeBarline(lmBarline* pBL, lmEBarline nBarlineType, bool fVisible);
	void change_dots(int dots);
//    void ChangeFiguredBass(lmFiguredBass* pFB, wxString& sFigBass);
//    void ChangeMidiSettings(lmInstrument* pInstr, int nMidiChannel, int nMidiInstr);
	void change_note_accidentals(EAccidentals nAcc);
//	void ChangeNotePitch(int nSteps);
//    void ChangePageMargin(lmGMObject* pGMO, int nIdx, int nPage, lmLUnits uPos);
//    void ChangeText(lmScoreText* pST, wxString sText, lmEHAlign nAlign,
//                    lmLocation tPos, lmTextStyle* pStyle, int nHintOptions=0);
//	void MoveNote(lmGMObject* pGMO, const lmUPoint& uPos, int nSteps);
//	void MoveObject(lmGMObject* pGMO, const lmUPoint& uPos);
    void move_object_point(int iPoint, UPoint newPos);
    void move_caret_to_click_point(SpEventMouse event);
    void break_beam();
    void join_beam();
    void toggle_stem();

    //selection commands
    void delete_selection();
    void select_object(ImoId id, bool fClearSelection=true);

};


//---------------------------------------------------------------------------------------
// CommandEventHandler
//    Responsible for handling toolbox events, keyboard events, mouse clicks events and
//    lomse cmmand events (control point moved events), generating commands for doing
//    the implicated actions, such as:
//        * changing view options (zooming, scrolling)
//        * editing the document
//        * modifiying toolbox selected options
//
class CommandEventHandler
{
private:
    ApplicationScope& m_appScope;
    DocumentWindow* m_pController;
    ToolsInfo& m_toolsInfo;
    SelectionSet* m_selection;
    DocCursor* m_cursor;
    bool m_fEventProcessed;
    CommandGenerator m_executer;

public:
    CommandEventHandler(ApplicationScope& appScope, DocumentWindow* pController,
                        ToolsInfo& toolsInfo, SelectionSet* selection, DocCursor* cursor);
    ~CommandEventHandler();

    void process_tool_event(EToolID toolID, ToolBox* pToolBox);
    void process_page_changed_in_toolbox_event(ToolBox* pToolBox);
    void process_key_event(wxKeyEvent& event);
    void process_on_click_event(SpEventMouse event);
    void process_action_event(SpEventAction event);

    inline bool event_processed() { return m_fEventProcessed; }

protected:
    int m_key;
    int m_keyFlags;
    int m_keyCmd;

    void command_on_selection(EToolID toolID);
    void command_on_caret_pointed_object(EToolID toolID);
    void check_commands_for_current_toolbox_context();
    void check_always_valid_edition_commands();
//    ClickHandler* new_click_handler_for_current_context();
    void delete_selection_or_pointed_object();
    void move_caret_to_click_point(SpEventMouse event);
    void switch_interactor_mode_for_current_mouse_mode();
    void set_drag_image_for_tool(EToolID toolID);
    void common_tasks_for_toolbox_event(ToolBox* pToolBox);
    void enter_top_level_and_edit();
    void move_cursor_up_down();
    void add_note(string step);
    void add_chord_note(string step);
    void ask_and_add_clef();

};


//=======================================================================================
// Handler for mouse clicks
//=======================================================================================
class ClickHandler
{
protected:
    DocumentWindow* m_pController;
    ToolsInfo& m_toolsInfo;
    SelectionSet* m_selection;
    DocCursor* m_cursor;
    bool m_fEventProcessed;
    CommandGenerator m_executer;

public:
    ClickHandler(DocumentWindow* pController, ToolsInfo& toolsInfo,
                 SelectionSet* selection, DocCursor* cursor);
    virtual ~ClickHandler() {}

    void process_click(SpEventMouse event);
    inline bool event_processed() { return m_fEventProcessed; }

protected:
    void add_barline(SpEventMouse event);
    void add_clef(SpEventMouse event);
    void add_note_rest(SpEventMouse event);

};


}   // namespace lenmus


#endif  // __LENMUS_COMMAND_EVENT_HANDLER_H__
