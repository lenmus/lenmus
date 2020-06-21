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
//    This code is based on files wx/htmllbox.h and wx/vlbox.h from wxWidgets 2.8.0
//    project. wxWidgets licence is compatible with GNU GPL.
//    Author of these files is Vadim Zeitlin,
//    Copyright (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
//
//    Merged and modified by:
//    Nov-2006 - Cecilio Salmeron. Implementation of a tree list box with images
//
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_CONTENT_BOX_CTROL_H__
#define __LENMUS_CONTENT_BOX_CTROL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/vscroll.h>         // base class
#include <wx/bitmap.h>

class wxSelectionStore;

#define ContentBoxCtrolStr "ContentBoxCtrol"

// ----------------------------------------------------------------------------
// ContentBoxCtrol
// ----------------------------------------------------------------------------

#include <wx/html/htmlwin.h>
#include <wx/ctrlsub.h>
#include <wx/filesys.h>

class wxHtmlCell;
class wxHtmlWinParser;


namespace lenmus
{

//forward declarations
class lmHtmlListBoxCache;
class ContentBoxStyle;
class lmHLB_TagHandler;


class ContentBoxCtrol : public wxVScrolledWindow,
                        public wxHtmlWindowInterface,
                        public wxHtmlWindowMouseHelper
{
public:
    ContentBoxCtrol(wxWindow *parent, ApplicationScope& appScope,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = ContentBoxCtrolStr);

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = ContentBoxCtrolStr);

    virtual ~ContentBoxCtrol();


    // accessors
    size_t GetItemCount() const { return GetRowCount(); }
    int GetSelection() const { return m_current; }

    bool IsCurrent(size_t item) const { return item == (size_t)m_current; }
    #ifdef __WXUNIVERSAL__
    bool IsCurrent() const { return wxVScrolledWindow::IsCurrent(); }
    #endif

    bool IsSelected(size_t item) const;

    wxPoint GetMargins() const { return m_ptMargins; }

    const wxColour& GetSelectionBackground() const { return m_colBgSel; }
    wxColour GetSelectedTextColour(const wxColour& colFg) const;
    wxColour GetSelectedTextBgColour(const wxColour& WXUNUSED(colBg)) const;


    // operations
    virtual void SetItemCount(size_t count);
    void Clear() { SetItemCount(0); }
    void SetSelection(int selection);

    void SetMargins(const wxPoint& pt);
    void SetMargins(wxCoord x, wxCoord y) { SetMargins(wxPoint(x, y)); }
    void SetSelectionBackground(const wxColour& col);



    wxString OnGetItemMarkup(size_t n) const;
    void CacheItem(size_t n) const;
    void OnSize(wxSizeEvent& event);
    void RefreshRow(size_t line);
    void RefreshRows(size_t from, size_t to);
    void RefreshAll();
    void SetHTMLWindowTitle(const wxString& WXUNUSED(title));
    void OnHTMLLinkClicked(const wxHtmlLinkInfo& link);
    void OnLinkClicked(size_t WXUNUSED(n), const wxHtmlLinkInfo& link);
    wxHtmlOpeningStatus OnHTMLOpeningURL(wxHtmlURLType WXUNUSED(type),
                                    const wxString& WXUNUSED(url),
                                    wxString *WXUNUSED(redirect)) const;
    wxPoint HTMLCoordsToWindow(wxHtmlCell *cell, const wxPoint& pos) const;
    wxWindow* GetHTMLWindow();
    wxColour GetHTMLBackgroundColour() const;
    void SetHTMLBackgroundColour(const wxColour& WXUNUSED(clr));
    void SetHTMLBackgroundImage(const wxBitmap& WXUNUSED(bmpBg));
    void SetHTMLStatusText(const wxString& WXUNUSED(text));
    wxCursor GetHTMLCursor(HTMLCursor type) const;
    wxPoint GetRootCellCoords(size_t n) const;
    bool PhysicalCoordsToCell(wxPoint& pos, wxHtmlCell*& cell) const;
    size_t GetItemForCell(const wxHtmlCell *cell) const;
    wxPoint CellCoordsToPhysical(const wxPoint& pos, wxHtmlCell *cell) const;
    virtual wxString OnGetItem(size_t n) const = 0;
    virtual void OnInternalIdle();

    // retrieve the file system used by the wxHtmlWinParser: if you use
    // relative paths in your HTML, you should use its ChangePathTo() method
    wxFileSystem& GetFileSystem() { return m_filesystem; }
    const wxFileSystem& GetFileSystem() const { return m_filesystem; }





protected:
    virtual void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;
    virtual wxCoord OnMeasureItem(size_t n) const;
    virtual void OnDrawSeparator(wxDC& dc, wxRect& rect, size_t n) const;
    virtual void OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const;
    virtual wxCoord OnGetRowHeight(size_t row) const;

    // flags for DoHandleItemClick
    enum
    {
        ItemClick_Shift = 1,        // item shift-clicked
        ItemClick_Ctrl  = 2,        //       ctrl
        ItemClick_Kbd   = 4         // item selected from keyboard
    };

    // common part of keyboard and mouse handling processing code
    virtual void DoHandleItemClick(int item, int flags);

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftDClick(wxMouseEvent& event);

    // common part of all ctors
    void Init();

    // send the wxEVT_COMMAND_LISTBOX_SELECTED event
    void SendSelectedEvent();

    // change the current item (in single selection listbox it also implicitly
    // changes the selection); current may be wxNOT_FOUND in which case there
    // will be no current item any more
    //
    // return true if the current item changed, false otherwise
    bool DoSetCurrent(int current);


private:
    ApplicationScope& m_appScope;

    // the current item or wxNOT_FOUND
    //
    // if m_selStore == nullptr this is also the selected item, otherwise the
    // selections are managed by m_selStore
    int m_current;

    // the anchor of the selection for the multiselection listboxes:
    // shift-clicking an item extends the selection from m_anchor to the item
    // clicked, for example
    //
    // always wxNOT_FOUND for single selection listboxes
    int m_anchor;

    // margins
    wxPoint m_ptMargins;

    // the selection bg colour
    wxColour m_colBgSel;

    // this class caches the pre-parsed HTML to speed up display
    lmHtmlListBoxCache* m_cache;

    // HTML parser we use
    wxHtmlWinParser*        m_htmlParser;
    // file system used by m_htmlParser
    wxFileSystem m_filesystem;

    // rendering style for the parser which allows us to customize our colours
    ContentBoxStyle* m_htmlRendStyle;


    // it calls our GetSelectedTextColour() and GetSelectedTextBgColour()
    friend class wxHtmlListBoxStyle;
    friend class wxHtmlListBoxWinInterface;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(ContentBoxCtrol);
    wxDECLARE_ABSTRACT_CLASS(ContentBoxCtrol);
};


}   //namespace lenmus

#endif // __LENMUS_CONTENT_BOX_CTROL_H__

