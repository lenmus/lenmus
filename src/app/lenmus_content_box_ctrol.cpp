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
//    This code is based on files generic/htmllbox.cpp and generic/vlbox.cpp from
//    wxWidgets 2.8.0 project. wxWidgets licence is compatible with GNU GPL.
//    Author of these files is Vadim Zeitlin,
//    Copyright (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
//
//    Merged and modified by:
//    Nov-2006 - Cecilio Salmeron. Implementation of a tree list box with images
//
//---------------------------------------------------------------------------------------

#include "lenmus_content_box_ctrol.h"
#include "lenmus_standard_header.h"

#include "lenmus_paths.h"

//wxWidgets
#include <wx/wxprec.h>


#ifndef WX_PRECOMP
    #include <wx/settings.h>
    #include <wx/dcclient.h>
    #include <wx/listbox.h>
#endif //WX_PRECOMP

#include <wx/dcbuffer.h>
#include <wx/selstore.h>

#include <wx/html/htmlcell.h>
#include <wx/html/winpars.h>

//// this hack forces the linker to always link in m_* files
//#include <wx/html/forcelnk.h>
//FORCE_WXHTML_MODULES()


namespace lenmus
{


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// small border always added to the cells:
static const wxCoord CELL_BORDER = 0;

// ============================================================================
// private classes
// ============================================================================

//-----------------------------------------------------------------------------
// HtmlSpacerCell
//-----------------------------------------------------------------------------

class HtmlSpacerCell : public wxHtmlCell
{
    public:
        HtmlSpacerCell(int width) : wxHtmlCell() {m_Width = width; }
        void Draw(wxDC& WXUNUSED(dc), int WXUNUSED(x), int WXUNUSED(y),
                  int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                  wxHtmlRenderingInfo& WXUNUSED(info)) {};
        void Layout(int WXUNUSED(w))
            { wxHtmlCell::Layout(m_Width); }

    wxDECLARE_NO_COPY_CLASS(HtmlSpacerCell);
};


//-----------------------------------------------------------------------------
// LMB_TagHandler
//-----------------------------------------------------------------------------

class LMB_TagHandler : public wxHtmlWinTagHandler
{
protected:
    ApplicationScope& m_appScope;

public:
    LMB_TagHandler(ApplicationScope& appScope)
        : wxHtmlWinTagHandler()
        , m_appScope(appScope)
    {}

    wxString GetSupportedTags() { return "TOCITEM"; }
    bool HandleTag(const wxHtmlTag& tag);

    wxDECLARE_NO_COPY_CLASS(LMB_TagHandler);
};


bool LMB_TagHandler::HandleTag(const wxHtmlTag& tag)
{
    if (tag.GetName() == "TOCITEM")
    {
        // Get all parameters

        // item level
        long nLevel;
        wxASSERT(tag.HasParam( "LEVEL" ));
        wxString sLevel(tag.GetParam( "LEVEL" ));
        if (!sLevel.ToLong(&nLevel)) {
            wxASSERT(false);
        }

        // item number
        long nItem;
        wxASSERT(tag.HasParam( "ITEM" ));
        wxString sItem(tag.GetParam( "ITEM" ));
        if (!sItem.ToLong(&nItem)) {
            wxASSERT(false);
        }

        // expand icon
        wxASSERT(tag.HasParam( "EXPAND" ));
        wxString sExpand(tag.GetParam( "EXPAND" ));

        // item icon
        wxASSERT(tag.HasParam( "ICON" ));
        wxString sIcon(tag.GetParam( "ICON" ));

        // image (optional)
        wxString sImage = wxEmptyString;
        if (tag.HasParam( "IMG" )) {
            sImage = tag.GetParam( "IMG" );
        }

        // titlenum
        wxString sTitlenum = wxEmptyString;
        if (tag.HasParam( "TITLENUM" )) {
            sTitlenum = tag.GetParam( "TITLENUM" );
        }


            //Create the entry

        // indentation
        int nIndent = nLevel * 24;
        if (sExpand == "no" && (nLevel > 0)) nIndent += 16;
        m_WParser->GetContainer()->SetIndent(nIndent, wxHTML_INDENT_LEFT);

        // aligment
        m_WParser->GetContainer()->SetAlignVer(wxHTML_ALIGN_CENTER);

        Paths* pPaths = m_appScope.get_paths();
        wxString sPath = pPaths->GetImagePath();
        wxString sItemLink = wxString::Format("item%d", int(nItem));

        // expand / collapse image
        if (sExpand == "+")
        {
            wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
            wxString name;
            if (nLevel == 0)
                name = wxString::Format("open&go%d", int(nItem));
            else
                name = wxString::Format("open%d", int(nItem));
            m_WParser->SetLink(wxHtmlLinkInfo(name, wxEmptyString));

            m_WParser->SetSourceAndSaveState("<img src='" + sPath + "nav_plus_16.png' width='16' height='16' />");
            m_WParser->DoParsing();
            m_WParser->RestoreState();

            m_WParser->SetLink(oldlnk);
            sItemLink = name;
        }
        else if (sExpand == "-")
        {
            wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
            wxString name;
            if (nLevel == 0)
                //name = wxString::Format("close&blank%d", int(nItem));
                name = wxString::Format("close&go%d", int(nItem));
            else
                name = wxString::Format("close%d", int(nItem));
            m_WParser->SetLink(wxHtmlLinkInfo(name, wxEmptyString));

            m_WParser->SetSourceAndSaveState("<img src='" + sPath + "nav_minus_16.png' width='16' height='16' />");
            m_WParser->DoParsing();
            m_WParser->RestoreState();

            m_WParser->SetLink(oldlnk);
            sItemLink = name;
        }

        // start link to item page
        wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
        m_WParser->SetLink(wxHtmlLinkInfo(sItemLink, wxEmptyString));

        // item icon
        wxString sIconImg = sPath;
        if (sIcon == "closed_book")
            sIconImg += "nav_book_closed_16.png";
        else if (sIcon == "open_book")
            sIconImg += "nav_book_open_16.png";
        else if (sIcon == "closed_folder")
            sIconImg += "nav_folder_closed_16.png";
        else if (sIcon == "open_folder")
            sIconImg += "nav_folder_open_16.png";
        else if (sIcon == "page")
            sIconImg += "nav_page_16.png";
        else if (sIcon == "leaflet")
            sIconImg += "nav_intro_16.png";
        else {
            wxASSERT(false);
        }
        m_WParser->SetSourceAndSaveState("<img src='" + sIconImg + "' />");
        m_WParser->DoParsing();
        m_WParser->RestoreState();
        m_WParser->GetContainer()->InsertCell(LENMUS_NEW HtmlSpacerCell(8));

        // title num
        if (sTitlenum != wxEmptyString) {
            m_WParser->SetSourceAndSaveState(sTitlenum);
            m_WParser->DoParsing();
            m_WParser->RestoreState();
        }

        // item image
        // Only drawn in final items
        bool fDrawImage = (sImage != wxEmptyString && sIcon == "page");
        if (fDrawImage) {
            m_WParser->GetContainer()->SetWidthFloat(10000, wxHTML_UNITS_PIXELS);   //force no wrap
            m_WParser->SetSourceAndSaveState("<img src='" + sImage + "' height='36' />");
            m_WParser->DoParsing();
            m_WParser->RestoreState();
        }

        // item text
        if (fDrawImage) {
            m_WParser->CloseContainer();
            m_WParser->OpenContainer();
            m_WParser->GetContainer()->SetIndent(nIndent+25, wxHTML_INDENT_LEFT);
        }
        m_WParser->GetContainer()->SetWidthFloat(10000, wxHTML_UNITS_PIXELS);   //force no wrap
        ParseInner(tag);

        //close link to item page
        m_WParser->SetLink(oldlnk);

        return true;

    }
    return false;   //to get compiler happy
}



// ----------------------------------------------------------------------------
// lmHtmlListBoxCache
// ----------------------------------------------------------------------------

// this class is used by ContentBoxCtrol to cache the parsed representation of
// the items to avoid doing it anew each time an item must be drawn
class lmHtmlListBoxCache
{
private:
    // invalidate a single item, used by Clear() and InvalidateRange()
    void InvalidateItem(size_t n)
    {
        m_items[n] = (size_t)-1;
        delete m_cells[n];
        m_cells[n] = nullptr;
    }

public:
    lmHtmlListBoxCache()
    {
        for ( size_t n = 0; n < SIZE; n++ )
        {
            m_items[n] = (size_t)-1;
            m_cells[n] = nullptr;
        }

        m_next = 0;
    }

    ~lmHtmlListBoxCache()
    {
        for ( size_t n = 0; n < SIZE; n++ )
        {
            delete m_cells[n];
        }
    }

    // completely invalidate the cache
    void Clear()
    {
        for ( size_t n = 0; n < SIZE; n++ )
        {
            InvalidateItem(n);
        }
    }

    // return the cached cell for this index or nullptr if none
    wxHtmlCell *Get(size_t item) const
    {
        for ( size_t n = 0; n < SIZE; n++ )
        {
            if ( m_items[n] == item )
                return m_cells[n];
        }

        return nullptr;
    }

    // returns true if we already have this item cached
    bool Has(size_t item) const { return Get(item) != nullptr; }

    // ensure that the item is cached
    void Store(size_t item, wxHtmlCell *cell)
    {
        delete m_cells[m_next];
        m_cells[m_next] = cell;
        m_items[m_next] = item;

        // advance to the next item wrapping around if there are no more
        if ( ++m_next == SIZE )
            m_next = 0;
    }

    // forget the cached value of the item(s) between the given ones (inclusive)
    void InvalidateRange(size_t from, size_t to)
    {
        for ( size_t n = 0; n < SIZE; n++ )
        {
            if ( m_items[n] >= from && m_items[n] <= to )
            {
                InvalidateItem(n);
            }
        }
    }

private:
    // the max number of the items we cache
    enum { SIZE = 50 };

    // the index of the LRU (oldest) cell
    size_t m_next;

    // the parsed representation of the cached item or nullptr
    wxHtmlCell *m_cells[SIZE];

    // the index of the currently cached item (only valid if m_cells != nullptr)
    size_t m_items[SIZE];
};

// ----------------------------------------------------------------------------
// ContentBoxStyle
// ----------------------------------------------------------------------------

// just forward wxDefaultHtmlRenderingStyle callbacks to the main class so that
// they could be overridden by the user code
class ContentBoxStyle : public wxDefaultHtmlRenderingStyle
{
public:
    ContentBoxStyle(const ContentBoxCtrol& hlbox) : m_hlbox(hlbox) { }

    virtual wxColour GetSelectedTextColour(const wxColour& colFg)
    {
        return m_hlbox.GetSelectedTextColour(colFg);
    }

    virtual wxColour GetSelectedTextBgColour(const wxColour& colBg)
    {
        return m_hlbox.GetSelectedTextBgColour(colBg);
    }

private:
    const ContentBoxCtrol& m_hlbox;

    wxDECLARE_NO_COPY_CLASS(ContentBoxStyle);
};

//=======================================================================================
// event tables
//=======================================================================================

wxBEGIN_EVENT_TABLE(ContentBoxCtrol, wxVScrolledWindow)
    EVT_PAINT       (ContentBoxCtrol::OnPaint)
    EVT_KEY_DOWN    (ContentBoxCtrol::OnKeyDown)
    EVT_LEFT_DOWN   (ContentBoxCtrol::OnLeftDown)
    EVT_LEFT_DCLICK (ContentBoxCtrol::OnLeftDClick)
wxEND_EVENT_TABLE()

IMPLEMENT_ABSTRACT_CLASS(ContentBoxCtrol, wxVScrolledWindow)

// ----------------------------------------------------------------------------
// ContentBoxCtrol creation
// ----------------------------------------------------------------------------

ContentBoxCtrol::ContentBoxCtrol(wxWindow *parent, ApplicationScope& appScope,
                                 wxWindowID id, const wxPoint& pos,
                                 const wxSize& size, long style, const wxString& name)
    : wxHtmlWindowMouseHelper(this)
    , m_appScope(appScope)
{
    Init();

    (void)Create(parent, id, pos, size, style, name);
}

void ContentBoxCtrol::Init()
{
    m_current = m_anchor = wxNOT_FOUND;

    m_htmlParser = nullptr;
    m_htmlRendStyle = LENMUS_NEW ContentBoxStyle(*this);
    m_cache = LENMUS_NEW lmHtmlListBoxCache;

}

bool ContentBoxCtrol::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                        const wxSize& size, long style, const wxString& name)
{
    style |= wxWANTS_CHARS | wxFULL_REPAINT_ON_RESIZE;
    if ( !wxVScrolledWindow::Create(parent, id, pos, size, style, name) )
        return false;

    // make sure the native widget has the right colour since we do
    // transparent drawing by default
    SetBackgroundColour( *wxWHITE );    //GetBackgroundColour());
    m_colBgSel = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);

    // flicker-free drawing requires this
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    return true;
}

ContentBoxCtrol::~ContentBoxCtrol()
{
    delete m_cache;

    if ( m_htmlParser )
    {
        delete m_htmlParser->GetDC();
        delete m_htmlParser;
    }

    delete m_htmlRendStyle;
}

void ContentBoxCtrol::SetItemCount(size_t count)
{
    // the items are going to change, forget the old ones
    m_cache->Clear();

    SetRowCount(count);
}

// ----------------------------------------------------------------------------
// selection handling
// ----------------------------------------------------------------------------

bool ContentBoxCtrol::IsSelected(size_t line) const
{
    return (int)line == m_current;
}

bool ContentBoxCtrol::DoSetCurrent(int current)
{
    wxASSERT_MSG( current == wxNOT_FOUND ||
                    (current >= 0 && (size_t)current < GetItemCount()),
                  "ContentBoxCtrol::DoSetCurrent(): invalid item index" );

    if ( current == m_current )
    {
        // nothing to do
        return false;
    }

    if ( m_current != wxNOT_FOUND )
        wxVScrolledWindow::RefreshRow(m_current);

    m_current = current;

    if ( m_current != wxNOT_FOUND )
    {
        // if the line is not visible at all, we scroll it into view but we
        // don't need to refresh it -- it will be redrawn anyhow
        if ( !IsVisible(m_current) )
        {
            ScrollToRow(m_current);
        }
        else // line is at least partly visible
        {
            // it is, indeed, only partly visible, so scroll it into view to
            // make it entirely visible
            while ( (size_t)m_current == GetVisibleRowsEnd() &&
                    ScrollToRow(GetVisibleBegin()+1) ) ;

            // but in any case refresh it as even if it was only partly visible
            // before we need to redraw it entirely as its background changed
            wxVScrolledWindow::RefreshRow(m_current);
        }
    }

    return true;
}

void ContentBoxCtrol::SendSelectedEvent()
{
    wxASSERT_MSG( m_current != wxNOT_FOUND,
                    "SendSelectedEvent() shouldn't be called" );

    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, GetId());
    event.SetEventObject(this);
    event.SetInt(m_current);

    (void)GetEventHandler()->ProcessEvent(event);
}

void ContentBoxCtrol::SetSelection(int selection)
{
    wxCHECK_RET( selection == wxNOT_FOUND ||
                  (selection >= 0 && (size_t)selection < GetItemCount()),
                  "ContentBoxCtrol::SetSelection(): invalid item index" );

    DoSetCurrent(selection);
}


// ----------------------------------------------------------------------------
// ContentBoxCtrol appearance parameters
// ----------------------------------------------------------------------------

void ContentBoxCtrol::SetMargins(const wxPoint& pt)
{
    if ( pt != m_ptMargins )
    {
        m_ptMargins = pt;

        Refresh();
    }
}

void ContentBoxCtrol::SetSelectionBackground(const wxColour& col)
{
    m_colBgSel = col;
}

// ----------------------------------------------------------------------------
// ContentBoxCtrol appearance
// ----------------------------------------------------------------------------

wxColour ContentBoxCtrol::GetSelectedTextColour(const wxColour& WXUNUSED(colFg)) const
{
    return *wxWHITE;
}

wxColour ContentBoxCtrol::GetSelectedTextBgColour(const wxColour& WXUNUSED(colBg)) const
{
    return wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT); //GetSelectionBackground();
}

// ----------------------------------------------------------------------------
// ContentBoxCtrol items markup
// ----------------------------------------------------------------------------

wxString ContentBoxCtrol::OnGetItemMarkup(size_t n) const
{
    // we don't even need to wrap the value returned by OnGetItem() inside
    // "<html><body>" and "</body></html>" because wxHTML can parse it even
    // without these tags
    return OnGetItem(n);
}


// ----------------------------------------------------------------------------
// ContentBoxCtrol cache handling
// ----------------------------------------------------------------------------

void ContentBoxCtrol::CacheItem(size_t n) const
{
    if ( !m_cache->Has(n) )
    {
        if ( !m_htmlParser )
        {
            ContentBoxCtrol *self = wxConstCast(this, ContentBoxCtrol);

            self->m_htmlParser = LENMUS_NEW wxHtmlWinParser(self);
            LMB_TagHandler* pTagHandler = LENMUS_NEW LMB_TagHandler(m_appScope);
            m_htmlParser->AddTagHandler(pTagHandler);

            m_htmlParser->SetDC(LENMUS_NEW wxClientDC(self));
            m_htmlParser->SetFS(&self->m_filesystem);

            // use system's default GUI font by default:
            m_htmlParser->SetFontFace("Tahoma");
            m_htmlParser->SetFontSize(10);
            //m_htmlParser->SetStandardFonts();
        }

        wxHtmlContainerCell *cell = (wxHtmlContainerCell *)m_htmlParser->
                Parse(OnGetItemMarkup(n));
        wxCHECK_RET( cell, "wxHtmlParser::Parse() returned nullptr?" );

        // set the cell's ID to item's index so that CellCoordsToPhysical()
        // can quickly find the item:
        cell->SetId(wxString::Format("%lu", (unsigned long)n));

        cell->Layout(GetClientSize().x - 2*GetMargins().x);

        m_cache->Store(n, cell);
    }
}

void ContentBoxCtrol::OnSize(wxSizeEvent& event)
{
    // we need to relayout all the cached cells
    m_cache->Clear();

    event.Skip();      //continue processing the  event
}

void ContentBoxCtrol::RefreshRow(size_t line)
{
    m_cache->InvalidateRange(line, line);

    wxVScrolledWindow::RefreshRow(line);
}

void ContentBoxCtrol::RefreshRows(size_t from, size_t to)
{
    m_cache->InvalidateRange(from, to);

    wxVScrolledWindow::RefreshRows(from, to);
}

void ContentBoxCtrol::RefreshAll()
{
    m_cache->Clear();

    wxVScrolledWindow::RefreshAll();
}

// ----------------------------------------------------------------------------
// ContentBoxCtrol implementation of wxVListBox pure virtuals
// ----------------------------------------------------------------------------

void ContentBoxCtrol::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    CacheItem(n);

    wxHtmlCell *cell = m_cache->Get(n);
    wxCHECK_RET( cell, "this cell should be cached!" );

    wxHtmlRenderingInfo htmlRendInfo;

    // draw the selected cell in selected state
    if ( IsSelected(n) )
    {
        wxHtmlSelection htmlSel;
        htmlSel.Set(wxPoint(0,0), cell, wxPoint(INT_MAX, INT_MAX), cell);
        htmlRendInfo.SetSelection(&htmlSel);
        if ( m_htmlRendStyle )
            htmlRendInfo.SetStyle(m_htmlRendStyle);
        htmlRendInfo.GetState().SetSelectionState(wxHTML_SEL_IN);
    }
    else {

    }

    // note that we can't stop drawing exactly at the window boundary as then
    // even the visible cells part could be not drawn, so always draw the
    // entire cell
    cell->Draw(dc,
               rect.x + CELL_BORDER, rect.y + CELL_BORDER,
               0, INT_MAX, htmlRendInfo);
}

wxCoord ContentBoxCtrol::OnMeasureItem(size_t n) const
{
    CacheItem(n);

    wxHtmlCell *cell = m_cache->Get(n);
    wxCHECK_MSG( cell, 0, "this cell should be cached!" );

    return cell->GetHeight() + cell->GetDescent(); // + 4;
}

void ContentBoxCtrol::OnDrawBackground(wxDC& dc, const wxRect& WXUNUSED(rect), size_t n) const
{
    if ( IsSelected(n) )
    {
        dc.SetBrush( wxBrush(GetSelectionBackground(), wxBRUSHSTYLE_SOLID) );
    }
    else // !selected
    {
        dc.SetBrush( wxBrush(*wxWHITE, wxBRUSHSTYLE_SOLID) );
    }

}


// ----------------------------------------------------------------------------
// ContentBoxCtrol painting
// ----------------------------------------------------------------------------

wxCoord ContentBoxCtrol::OnGetRowHeight(size_t row) const
{
    return OnMeasureItem(row) + 2*m_ptMargins.y;
}

void ContentBoxCtrol::OnDrawSeparator(wxDC& WXUNUSED(dc),
                                 wxRect& WXUNUSED(rect),
                                 size_t WXUNUSED(n)) const
{
}

void ContentBoxCtrol::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxSize clientSize = GetClientSize();

    wxAutoBufferedPaintDC dc(this);

    // the update rectangle
    wxRect rectUpdate = GetUpdateClientRect();

    // fill it with background colour
    dc.SetBackground(GetBackgroundColour());
    dc.Clear();

    // the bounding rectangle of the current line
    wxRect rectLine;
    rectLine.width = clientSize.x;

    // iterate over all visible lines
    const size_t lineMax = GetVisibleEnd();
    for ( size_t line = GetVisibleRowsBegin(); line < lineMax; line++ )
    {
        const wxCoord hLine = OnGetRowHeight(line);
        rectLine.height = hLine;

        // and draw the ones which intersect the update rect
        if ( rectLine.Intersects(rectUpdate) )
        {
            // don't allow drawing outside of the lines rectangle
            wxDCClipper clip(dc, rectLine);

            wxRect rect = rectLine;
            OnDrawBackground(dc, rect, line);

            OnDrawSeparator(dc, rect, line);

            rect.Deflate(m_ptMargins.x, m_ptMargins.y);
            OnDrawItem(dc, rect, line);
        }
        else // no intersection
        {
            if ( rectLine.GetTop() > rectUpdate.GetBottom() )
            {
                // we are already below the update rect, no need to continue
                // further
                break;
            }
            //else: the next line may intersect the update rect
        }

        rectLine.y += hLine;
    }
}

// ============================================================================
// ContentBoxCtrol keyboard/mouse handling
// ============================================================================

void ContentBoxCtrol::DoHandleItemClick(int item, int WXUNUSED(flags))
{
    // has anything worth telling the client code about happened?
    bool notify = false;

    // the item should become the current one only if it is a final node
    DoSetCurrent(item);

    if ( notify )
    {
        // notify the user about the selection change
        SendSelectedEvent();
    }
    //else: nothing changed at all
}

// ----------------------------------------------------------------------------
// keyboard handling
// ----------------------------------------------------------------------------

void ContentBoxCtrol::OnKeyDown(wxKeyEvent& event)
{
    // flags for DoHandleItemClick()
    int flags = ItemClick_Kbd;

    int current;
    switch ( event.GetKeyCode() )
    {
        case WXK_HOME:
            current = 0;
            break;

        case WXK_END:
            current = GetRowCount() - 1;
            break;

        case WXK_DOWN:
            if ( m_current == (int)GetRowCount() - 1 )
                return;

            current = m_current + 1;
            break;

        case WXK_UP:
            if ( m_current == wxNOT_FOUND )
                current = GetRowCount() - 1;
            else if ( m_current != 0 )
                current = m_current - 1;
            else // m_current == 0
                return;
            break;

        case WXK_PAGEDOWN:
            PageDown();
            current = GetVisibleRowsBegin();
            break;

        case WXK_PAGEUP:
            if ( m_current == (int)GetVisibleRowsBegin() )
            {
                PageUp();
            }

            current = GetVisibleRowsBegin();
            break;

        case WXK_SPACE:
            // hack: pressing space should work like a mouse click rather than
            // like a keyboard arrow press, so trick DoHandleItemClick() in
            // thinking we were clicked
            flags &= ~ItemClick_Kbd;
            current = m_current;
            break;

#if (LENMUS_PLATFORM_WIN32 == 1)
        case WXK_TAB:
            // Since we are using wxWANTS_CHARS we need to send navigation
            // events for the tabs on MSW
            {
                wxNavigationKeyEvent ne;
                ne.SetDirection(!event.ShiftDown());
                ne.SetCurrentFocus(this);
                ne.SetEventObject(this);
                GetParent()->GetEventHandler()->ProcessEvent(ne);
            }
            // fall through to default
#endif
        default:
            event.Skip();      //continue processing the  event
            current = 0; // just to silent the stupid compiler warnings
            wxUnusedVar(current);
            return;
    }

    if ( event.ShiftDown() )
       flags |= ItemClick_Shift;
    if ( event.ControlDown() )
        flags |= ItemClick_Ctrl;

    DoHandleItemClick(current, flags);
}

// ----------------------------------------------------------------------------
// ContentBoxCtrol mouse handling
// ----------------------------------------------------------------------------

void ContentBoxCtrol::OnLeftDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    wxHtmlCell *cell;

    if ( !PhysicalCoordsToCell(pos, cell) )
    {
        event.Skip();      //continue processing the  event
        return;
    }

    SetFocus();

    int item = wxVarScrollHelperBase::VirtualHitTest(event.GetPosition().y);

    if ( item != wxNOT_FOUND )
    {
        int flags = 0;
        if ( event.ShiftDown() )
           flags |= ItemClick_Shift;

        if ( event.ControlDown() )
            flags |= ItemClick_Ctrl;

        DoHandleItemClick(item, flags);
    }
    wxHtmlWindowMouseHelper::HandleMouseClick(cell, pos, event);

}

void ContentBoxCtrol::OnLeftDClick(wxMouseEvent& eventMouse)
{
    int item = HitTest(eventMouse.GetPosition());
    if ( item != wxNOT_FOUND )
    {

        // if item double-clicked was not yet selected, then treat
        // this event as a left-click instead
        if ( item == m_current )
        {
            wxCommandEvent event(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, GetId());
            event.SetEventObject(this);
            event.SetInt(item);

            (void)GetEventHandler()->ProcessEvent(event);
        }
        else
        {
            OnLeftDown(eventMouse);
        }

    }
}

// ----------------------------------------------------------------------------
// ContentBoxCtrol implementation of wxHtmlListBoxWinInterface
// ----------------------------------------------------------------------------

void ContentBoxCtrol::SetHTMLWindowTitle(const wxString& WXUNUSED(title))
{
    // nothing to do
}

void ContentBoxCtrol::OnHTMLLinkClicked(const wxHtmlLinkInfo& link)
{
    OnLinkClicked(GetItemForCell(link.GetHtmlCell()), link);
}

void ContentBoxCtrol::OnLinkClicked(size_t WXUNUSED(n),
                                  const wxHtmlLinkInfo& link)
{
    wxHtmlLinkEvent event(GetId(), link);
    GetEventHandler()->ProcessEvent(event);
}

wxHtmlOpeningStatus
ContentBoxCtrol::OnHTMLOpeningURL(wxHtmlURLType WXUNUSED(type),
                                const wxString& WXUNUSED(url),
                                wxString *WXUNUSED(redirect)) const
{
    return wxHTML_OPEN;
}

wxPoint ContentBoxCtrol::HTMLCoordsToWindow(wxHtmlCell *cell,
                                          const wxPoint& pos) const
{
    return CellCoordsToPhysical(pos, cell);
}

wxWindow* ContentBoxCtrol::GetHTMLWindow() { return this; }

wxColour ContentBoxCtrol::GetHTMLBackgroundColour() const
{
    return GetBackgroundColour();
}

void ContentBoxCtrol::SetHTMLBackgroundColour(const wxColour& WXUNUSED(clr))
{
    // nothing to do
}

void ContentBoxCtrol::SetHTMLBackgroundImage(const wxBitmap& WXUNUSED(bmpBg))
{
    // nothing to do
}

void ContentBoxCtrol::SetHTMLStatusText(const wxString& WXUNUSED(text))
{
    // nothing to do
}

wxCursor ContentBoxCtrol::GetHTMLCursor(HTMLCursor type) const
{
    // we don't want to show text selection cursor in listboxes
    if (type == HTMLCursor_Text)
        return wxHtmlWindow::GetDefaultHTMLCursor(HTMLCursor_Default);

    // in all other cases, use the same cursor as wxHtmlWindow:
    return wxHtmlWindow::GetDefaultHTMLCursor(type);
}

// ----------------------------------------------------------------------------
// ContentBoxCtrol handling of HTML links
// ----------------------------------------------------------------------------

wxPoint ContentBoxCtrol::GetRootCellCoords(size_t n) const
{
    wxPoint pos(CELL_BORDER, CELL_BORDER);
    pos += GetMargins();
    //pos.y += GetRowHeight(GetVisibleRowsBegin(), n);
    pos.y += GetRowsHeight(GetVisibleBegin(), n);
    return pos;
}

bool ContentBoxCtrol::PhysicalCoordsToCell(wxPoint& pos, wxHtmlCell*& cell) const
{
    int n = VirtualHitTest(pos.y);
    if ( n == wxNOT_FOUND )
        return false;

    // convert mouse coordinates to coords relative to item's wxHtmlCell:
    pos -= GetRootCellCoords(n);

    CacheItem(n);
    cell = m_cache->Get(n);

    return true;
}

size_t ContentBoxCtrol::GetItemForCell(const wxHtmlCell *cell) const
{
    wxCHECK_MSG( cell, 0, "no cell" );

    cell = cell->GetRootCell();

    wxCHECK_MSG( cell, 0, "no root cell" );

    // the cell's ID contains item index, see CacheItem():
    unsigned long n;
    if ( !cell->GetId().ToULong(&n) )
    {
        wxFAIL_MSG( "unexpected root cell's ID" );
        return 0;
    }

    return n;
}

wxPoint ContentBoxCtrol::CellCoordsToPhysical(const wxPoint& pos, wxHtmlCell *cell) const
{
    return pos + GetRootCellCoords(GetItemForCell(cell));
}

void ContentBoxCtrol::OnInternalIdle()
{
    wxVScrolledWindow::OnInternalIdle();

    if ( wxHtmlWindowMouseHelper::DidMouseMove() )
    {
        wxPoint pos = ScreenToClient(wxGetMousePosition());
        wxHtmlCell *cell;

        if ( !PhysicalCoordsToCell(pos, cell) )
            return;

        wxHtmlWindowMouseHelper::HandleIdle(cell, pos);
    }
}


}   //namespace lenmus
