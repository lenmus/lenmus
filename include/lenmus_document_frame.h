//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

#ifndef __LENMUS_DOCUMENT_FRAME_H__        //to avoid nested includes
#define __LENMUS_DOCUMENT_FRAME_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_canvas.h"
#include "lenmus_injectors.h"
#include "lenmus_events.h"

//wxWidgets
#include "wx/wxprec.h"
#include <wx/frame.h>
#include <wx/msgdlg.h>
#include <wx/event.h>
#include <wx/html/htmlwin.h>

//lomse
#include <lomse_doorway.h>
#include <lomse_document.h>
#include <lomse_graphic_view.h>
#include <lomse_interactor.h>
#include <lomse_presenter.h>
#include <lomse_events.h>
#include <lomse_internal_model.h>
#include <lomse_analyser.h>
using namespace lomse;

//other
#include <iostream>
#include <UnitTest++.h>


namespace lenmus
{

//forward declarations
class DocumentWindow;
class TextBookHelpMergedIndex;
class BookReader;
class BookContentBox;

//---------------------------------------------------------------------------------------
// DocumentLoader: responsible for creating the canvas for the document to load
class DocumentLoader
{
protected:
    ContentWindow* m_pContentWindow;
    ApplicationScope& m_appScope;
    LomseDoorway& m_lomse;

public:
    DocumentLoader(ContentWindow* parent, ApplicationScope& appScope, LomseDoorway& lomse);
    virtual ~DocumentLoader() {}

    CanvasInterface* create_canvas(const string& filename,
                                   int viewType = ViewFactory::k_view_horizontal_book);

};

//---------------------------------------------------------------------------------------
// DocumentFrame is a window on which we show one lenmus book
class DocumentFrame : public wxSplitterWindow
                    , public CanvasInterface
{
protected:
    ApplicationScope& m_appScope;
    LomseDoorway& m_lomse;
    BookContentBox* m_left;
    DocumentWindow* m_right;
    BookReader* m_pBookReader;
    wxString m_bookPath;

    bool m_UpdateContents;


public:
    DocumentFrame(ContentWindow* parent, ApplicationScope& appScope, LomseDoorway& lomse);
    virtual ~DocumentFrame();

    //creation
    void display_document(const string& filename, int viewType);

    //events and commands received
    void on_hyperlink_event(SpEventInfo pEvent);

    //accessors
    ImoScore* get_active_score();
    Interactor* get_interactor();
    DocumentWindow* get_document_window() { return m_right; }

    void NotifyPageChanged() {}
    //wxString GetOpenedPageWithAnchor();
    //void UpdateMergedIndex();
    //TextBookHelpMergedIndex* m_mergedIndex;
    void load_page(const string& filename);
    void load_page(int iTocItem);
    //void clear_page();

protected:
    void create_content_pane(const string& filename);
    void create_content_pane(int iTocItem);
    void create_toc_pane();
    wxString get_path_for_toc_item(int iItem);

    // event handlers
    void on_splitter_moved(wxSplitterEvent& WXUNUSED(event));
    //void on_show_toc(wxCommandEvent& WXUNUSED(event));
    //void on_hide_toc(wxCommandEvent& WXUNUSED(event));

    //overrides
    //This overrides is to prevent unsplit when double click on splitter
    void OnDoubleClickSash(int x, int y) {}

private:

    //scrolling steps
    int m_xPageScroll;
    int m_xLineScroll;
    int m_xOffset;
    int m_xRight;
    int m_yPageScroll;
    int m_yLineScroll;
    int m_yBottom;

    int m_xMargin;
    int m_xPxPerUnit;
    int m_xPageSize;
    int m_xMinPxPos;
    int m_xMaxPxPos;
    int m_xThumb;
    int m_xMaxUnits;

    DECLARE_EVENT_TABLE()
};


}   // namespace lenmus

#endif    // __LENMUS_DOCUMENT_FRAME_H__

