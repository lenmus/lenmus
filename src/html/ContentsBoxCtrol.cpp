//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This code is based on files generic/htmllbox.cpp and generic/vlbox.cpp from
//    wxWidgets 2.8.0 project. wxWidgets licence is compatible with GNU GPL.
//    Author of these files is Vadim Zeitlin, 
//    Copyright (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// 
//    Merged and modified by:
//    Nov-2006 - Cecilio Salmeron. Implementation of a tree list box with images
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
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ContentsBoxCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LISTBOX

#include "ContentsBoxCtrol.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/listbox.h"
#endif //WX_PRECOMP

#include "wx/dcbuffer.h"
#include "wx/selstore.h"

#include "wx/html/htmlcell.h"
#include "wx/html/winpars.h"

// this hack forces the linker to always link in m_* files
#include "wx/html/forcelnk.h"
FORCE_WXHTML_MODULES()


// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// small border always added to the cells:
static const wxCoord CELL_BORDER = 0;

const wxString lmHtmlListBoxNameStr = wxT("lmContentsBoxCtrol");

// ============================================================================
// private classes
// ============================================================================

//-----------------------------------------------------------------------------
// lmHtmlSpacerCell
//-----------------------------------------------------------------------------

class lmHtmlSpacerCell : public wxHtmlCell
{
    public:
        lmHtmlSpacerCell(int width) : wxHtmlCell() {m_Width = width; }
        void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
            wxHtmlRenderingInfo& info) {};
        void Layout(int w)
            { wxHtmlCell::Layout(m_Width); }

      DECLARE_NO_COPY_CLASS(lmHtmlSpacerCell)
};


//-----------------------------------------------------------------------------
// lmHLB_TagHandler
//-----------------------------------------------------------------------------

class lmHLB_TagHandler : public wxHtmlWinTagHandler
{
public:
        lmHLB_TagHandler() : wxHtmlWinTagHandler() {}
        wxString GetSupportedTags() { return wxT("TOCITEM"); }
        bool HandleTag(const wxHtmlTag& tag);

    DECLARE_NO_COPY_CLASS(lmHLB_TagHandler)
};


bool lmHLB_TagHandler::HandleTag(const wxHtmlTag& tag)
{
    if (tag.GetName() == wxT("TOCITEM"))
    {
        // Get all parameters
        //wxLogMessage(tag.GetAllParams());

        // item level
        long nLevel;
        wxASSERT(tag.HasParam( _T("LEVEL") ));
        wxString sLevel(tag.GetParam( _T("LEVEL") ));
        if (!sLevel.ToLong(&nLevel)) {
            wxASSERT(false);
        }

        // item number
        long nItem;
        wxASSERT(tag.HasParam( _T("ITEM") ));
        wxString sItem(tag.GetParam( _T("ITEM") ));
        if (!sItem.ToLong(&nItem)) {
            wxASSERT(false);
        }

        // expand icon
        wxASSERT(tag.HasParam( _T("EXPAND") ));
        wxString sExpand(tag.GetParam( _T("EXPAND") ));

        // item icon
        wxASSERT(tag.HasParam( _T("ICON") ));
        wxString sIcon(tag.GetParam( _T("ICON") ));

        // image (optional)
        wxString sImage = wxEmptyString;
        if (tag.HasParam( _T("IMG") )) {
            sImage = tag.GetParam( _T("IMG") );
        }

        // titlenum
        wxString sTitlenum = wxEmptyString;
        if (tag.HasParam( _T("TITLENUM") )) {
            sTitlenum = tag.GetParam( _T("TITLENUM") );
        }


            //Create the entry

        // indentation
        int nIndent = nLevel * 24;
        if (sExpand == _T("no") && (nLevel > 0)) nIndent += 16;
        m_WParser->GetContainer()->SetIndent(nIndent, wxHTML_INDENT_LEFT);

        // aligment
        m_WParser->GetContainer()->SetAlignVer(wxHTML_ALIGN_CENTER);

        wxString sPath = g_pPaths->GetImagePath();
        wxString sItemLink = wxString::Format(_T("item%d"), nItem);

        // expand / collapse image
        if (sExpand == _T("+")) {
            wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
            wxString name;
            if (nLevel == 0)
                name = wxString::Format(_T("open&go%d"), nItem);
            else
                name = wxString::Format(_T("open%d"), nItem);
            m_WParser->SetLink(wxHtmlLinkInfo(name, wxEmptyString));

            m_WParser->SetSourceAndSaveState(_T("<img src='") + sPath + _T("nav_plus_16.png' width='16' height='16' />"));
            m_WParser->DoParsing();
            m_WParser->RestoreState();

            m_WParser->SetLink(oldlnk);
            sItemLink = name;
        }
        else if (sExpand == _T("-")) {
            wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
            wxString name;
            if (nLevel == 0)
                //name = wxString::Format(_T("close&blank%d"), nItem);
                name = wxString::Format(_T("close&go%d"), nItem);
            else
                name = wxString::Format(_T("close%d"), nItem);
            m_WParser->SetLink(wxHtmlLinkInfo(name, wxEmptyString));

            m_WParser->SetSourceAndSaveState(_T("<img src='") + sPath + _T("nav_minus_16.png' width='16' height='16' />"));
            m_WParser->DoParsing();
            m_WParser->RestoreState();

            m_WParser->SetLink(oldlnk);
            //sItemLink = name;
        }

        // start link to item page
        wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
        m_WParser->SetLink(wxHtmlLinkInfo(sItemLink, wxEmptyString));

        // item icon
        wxString sIconImg = sPath;
        if (sIcon == _T("closed_book"))
            sIconImg += _T("nav_book_closed_16.png");
        else if (sIcon == _T("open_book"))
            sIconImg += _T("nav_book_open_16.png");
        else if (sIcon == _T("closed_folder"))
            sIconImg += _T("nav_folder_closed_16.png");
        else if (sIcon == _T("open_folder"))
            sIconImg += _T("nav_folder_open_16.png");
        else if (sIcon == _T("page"))
            sIconImg += _T("nav_page_16.png");
        else if (sIcon == _T("leaflet"))
            sIconImg += _T("nav_intro_16.png");
        else {
            wxASSERT(false);
        }
        m_WParser->SetSourceAndSaveState(_T("<img src='") + sIconImg + _T("' />"));
        m_WParser->DoParsing();
        m_WParser->RestoreState();
        m_WParser->GetContainer()->InsertCell(new lmHtmlSpacerCell(8));

        // title num
        if (sTitlenum != wxEmptyString) {
            m_WParser->SetSourceAndSaveState(sTitlenum);
            m_WParser->DoParsing();
            m_WParser->RestoreState();
        }

        // item image
        // Only drawn in final items
        bool fDrawImage = (sImage != wxEmptyString && sIcon == _T("page"));
        if (fDrawImage) {
            m_WParser->GetContainer()->SetWidthFloat(10000, wxHTML_UNITS_PIXELS);   //force no wrap
            m_WParser->SetSourceAndSaveState(_T("<img src='") + sImage + _T("' height='36' />"));
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

// this class is used by lmContentsBoxCtrol to cache the parsed representation of
// the items to avoid doing it anew each time an item must be drawn
class lmHtmlListBoxCache
{
private:
    // invalidate a single item, used by Clear() and InvalidateRange()
    void InvalidateItem(size_t n)
    {
        m_items[n] = (size_t)-1;
        delete m_cells[n];
        m_cells[n] = NULL;
    }

public:
    lmHtmlListBoxCache()
    {
        for ( size_t n = 0; n < SIZE; n++ )
        {
            m_items[n] = (size_t)-1;
            m_cells[n] = NULL;
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

    // return the cached cell for this index or NULL if none
    wxHtmlCell *Get(size_t item) const
    {
        for ( size_t n = 0; n < SIZE; n++ )
        {
            if ( m_items[n] == item )
                return m_cells[n];
        }

        return NULL;
    }

    // returns true if we already have this item cached
    bool Has(size_t item) const { return Get(item) != NULL; }

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

    // the parsed representation of the cached item or NULL
    wxHtmlCell *m_cells[SIZE];

    // the index of the currently cached item (only valid if m_cells != NULL)
    size_t m_items[SIZE];
};

// ----------------------------------------------------------------------------
// lmContentsBoxStyle
// ----------------------------------------------------------------------------

// just forward wxDefaultHtmlRenderingStyle callbacks to the main class so that
// they could be overridden by the user code
class lmContentsBoxStyle : public wxDefaultHtmlRenderingStyle
{
public:
    lmContentsBoxStyle(const lmContentsBoxCtrol& hlbox) : m_hlbox(hlbox) { }

    virtual wxColour GetSelectedTextColour(const wxColour& colFg)
    {
        return m_hlbox.GetSelectedTextColour(colFg);
    }

    virtual wxColour GetSelectedTextBgColour(const wxColour& colBg)
    {
        return m_hlbox.GetSelectedTextBgColour(colBg);
    }

private:
    const lmContentsBoxCtrol& m_hlbox;

    DECLARE_NO_COPY_CLASS(lmContentsBoxStyle)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmContentsBoxCtrol, wxVScrolledWindow)
    EVT_PAINT(lmContentsBoxCtrol::OnPaint)

    EVT_KEY_DOWN(lmContentsBoxCtrol::OnKeyDown)
    EVT_LEFT_DOWN(lmContentsBoxCtrol::OnLeftDown)
    EVT_LEFT_DCLICK(lmContentsBoxCtrol::OnLeftDClick)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_ABSTRACT_CLASS(lmContentsBoxCtrol, wxVScrolledWindow)

// ----------------------------------------------------------------------------
// lmContentsBoxCtrol creation
// ----------------------------------------------------------------------------

lmContentsBoxCtrol::lmContentsBoxCtrol()
    : wxHtmlWindowMouseHelper(this)
{
    Init();
}

// normal constructor which calls Create() internally
lmContentsBoxCtrol::lmContentsBoxCtrol(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                             const wxSize& size, long style, const wxString& name)
    : wxHtmlWindowMouseHelper(this)
{
    Init();

    (void)Create(parent, id, pos, size, style, name);
}

void lmContentsBoxCtrol::Init()
{
    m_current =
    m_anchor = wxNOT_FOUND;

    m_htmlParser = NULL;
    m_htmlRendStyle = new lmContentsBoxStyle(*this);
    m_cache = new lmHtmlListBoxCache;

}

bool lmContentsBoxCtrol::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos,
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

lmContentsBoxCtrol::~lmContentsBoxCtrol()
{
    delete m_cache;

    if ( m_htmlParser )
    {
        delete m_htmlParser->GetDC();
        delete m_htmlParser;
    }

    delete m_htmlRendStyle;
}

void lmContentsBoxCtrol::SetItemCount(size_t count)
{
    // the items are going to change, forget the old ones
    m_cache->Clear();

    SetLineCount(count);
}

// ----------------------------------------------------------------------------
// selection handling
// ----------------------------------------------------------------------------

bool lmContentsBoxCtrol::IsSelected(size_t line) const
{
    return (int)line == m_current;
}

bool lmContentsBoxCtrol::DoSetCurrent(int current)
{
    wxASSERT_MSG( current == wxNOT_FOUND ||
                    (current >= 0 && (size_t)current < GetItemCount()),
                  _T("lmContentsBoxCtrol::DoSetCurrent(): invalid item index") );

    if ( current == m_current )
    {
        // nothing to do
        return false;
    }

    if ( m_current != wxNOT_FOUND )
        wxVScrolledWindow::RefreshLine(m_current);

    m_current = current;

    if ( m_current != wxNOT_FOUND )
    {
        // if the line is not visible at all, we scroll it into view but we
        // don't need to refresh it -- it will be redrawn anyhow
        if ( !IsVisible(m_current) )
        {
            ScrollToLine(m_current);
        }
        else // line is at least partly visible
        {
            // it is, indeed, only partly visible, so scroll it into view to
            // make it entirely visible
            while ( (size_t)m_current == GetLastVisibleLine() &&
                    ScrollToLine(GetVisibleBegin()+1) ) ;

            // but in any case refresh it as even if it was only partly visible
            // before we need to redraw it entirely as its background changed
            wxVScrolledWindow::RefreshLine(m_current);
        }
    }

    return true;
}

void lmContentsBoxCtrol::SendSelectedEvent()
{
    wxASSERT_MSG( m_current != wxNOT_FOUND,
                    _T("SendSelectedEvent() shouldn't be called") );

    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, GetId());
    event.SetEventObject(this);
    event.SetInt(m_current);

    (void)GetEventHandler()->ProcessEvent(event);
}

void lmContentsBoxCtrol::SetSelection(int selection)
{
    wxCHECK_RET( selection == wxNOT_FOUND ||
                  (selection >= 0 && (size_t)selection < GetItemCount()),
                  _T("lmContentsBoxCtrol::SetSelection(): invalid item index") );

    DoSetCurrent(selection);
}


// ----------------------------------------------------------------------------
// lmContentsBoxCtrol appearance parameters
// ----------------------------------------------------------------------------

void lmContentsBoxCtrol::SetMargins(const wxPoint& pt)
{
    if ( pt != m_ptMargins )
    {
        m_ptMargins = pt;

        Refresh();
    }
}

void lmContentsBoxCtrol::SetSelectionBackground(const wxColour& col)
{
    m_colBgSel = col;
}

// ----------------------------------------------------------------------------
// lmContentsBoxCtrol appearance
// ----------------------------------------------------------------------------

wxColour lmContentsBoxCtrol::GetSelectedTextColour(const wxColour& colFg) const
{
    return *wxWHITE;
}

wxColour lmContentsBoxCtrol::GetSelectedTextBgColour(const wxColour& WXUNUSED(colBg)) const
{
    return wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT); //GetSelectionBackground();
}

// ----------------------------------------------------------------------------
// lmContentsBoxCtrol items markup
// ----------------------------------------------------------------------------

wxString lmContentsBoxCtrol::OnGetItemMarkup(size_t n) const
{
    // we don't even need to wrap the value returned by OnGetItem() inside
    // "<html><body>" and "</body></html>" because wxHTML can parse it even
    // without these tags
    return OnGetItem(n);
}


// ----------------------------------------------------------------------------
// lmContentsBoxCtrol cache handling
// ----------------------------------------------------------------------------

void lmContentsBoxCtrol::CacheItem(size_t n) const
{
    if ( !m_cache->Has(n) )
    {
        if ( !m_htmlParser )
        {
            lmContentsBoxCtrol *self = wxConstCast(this, lmContentsBoxCtrol);

            self->m_htmlParser = new wxHtmlWinParser(self);
            lmHLB_TagHandler* pTagHandler = new lmHLB_TagHandler();
            m_htmlParser->AddTagHandler(pTagHandler);

            m_htmlParser->SetDC(new wxClientDC(self));
            m_htmlParser->SetFS(&self->m_filesystem);

            // use system's default GUI font by default:
            m_htmlParser->SetFontFace(_T("Tahoma"));
            m_htmlParser->SetFontSize(10);
            //m_htmlParser->SetStandardFonts();
        }

        wxHtmlContainerCell *cell = (wxHtmlContainerCell *)m_htmlParser->
                Parse(OnGetItemMarkup(n));
        wxCHECK_RET( cell, _T("wxHtmlParser::Parse() returned NULL?") );

        // set the cell's ID to item's index so that CellCoordsToPhysical()
        // can quickly find the item:
        cell->SetId(wxString::Format(_T("%lu"), (unsigned long)n));

        cell->Layout(GetClientSize().x - 2*GetMargins().x);

        m_cache->Store(n, cell);
    }
}

void lmContentsBoxCtrol::OnSize(wxSizeEvent& event)
{
    // we need to relayout all the cached cells
    m_cache->Clear();

    event.Skip();      //continue processing the  event
}

void lmContentsBoxCtrol::RefreshLine(size_t line)
{
    m_cache->InvalidateRange(line, line);

    wxVScrolledWindow::RefreshLine(line);
}

void lmContentsBoxCtrol::RefreshLines(size_t from, size_t to)
{
    m_cache->InvalidateRange(from, to);

    wxVScrolledWindow::RefreshLines(from, to);
}

void lmContentsBoxCtrol::RefreshAll()
{
    m_cache->Clear();

    wxVScrolledWindow::RefreshAll();
}

// ----------------------------------------------------------------------------
// lmContentsBoxCtrol implementation of wxVListBox pure virtuals
// ----------------------------------------------------------------------------

void lmContentsBoxCtrol::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    CacheItem(n);

    wxHtmlCell *cell = m_cache->Get(n);
    wxCHECK_RET( cell, _T("this cell should be cached!") );

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

wxCoord lmContentsBoxCtrol::OnMeasureItem(size_t n) const
{
    CacheItem(n);

    wxHtmlCell *cell = m_cache->Get(n);
    wxCHECK_MSG( cell, 0, _T("this cell should be cached!") );

    return cell->GetHeight() + cell->GetDescent(); // + 4;
}

void lmContentsBoxCtrol::OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const
{
    if ( IsSelected(n) )
    {
        dc.SetBrush( wxBrush(GetSelectionBackground(), wxSOLID) );
    }
    else // !selected
    {
        dc.SetBrush( wxBrush(*wxWHITE, wxSOLID) );
    }

}


// ----------------------------------------------------------------------------
// lmContentsBoxCtrol painting
// ----------------------------------------------------------------------------

wxCoord lmContentsBoxCtrol::OnGetLineHeight(size_t line) const
{
    return OnMeasureItem(line) + 2*m_ptMargins.y;
}

void lmContentsBoxCtrol::OnDrawSeparator(wxDC& WXUNUSED(dc),
                                 wxRect& WXUNUSED(rect),
                                 size_t WXUNUSED(n)) const
{
}

void lmContentsBoxCtrol::OnPaint(wxPaintEvent& WXUNUSED(event))
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
    for ( size_t line = GetFirstVisibleLine(); line < lineMax; line++ )
    {
        const wxCoord hLine = OnGetLineHeight(line);

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
// lmContentsBoxCtrol keyboard/mouse handling
// ============================================================================

void lmContentsBoxCtrol::DoHandleItemClick(int item, int flags)
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

void lmContentsBoxCtrol::OnKeyDown(wxKeyEvent& event)
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
            current = GetLineCount() - 1;
            break;

        case WXK_DOWN:
            if ( m_current == (int)GetLineCount() - 1 )
                return;

            current = m_current + 1;
            break;

        case WXK_UP:
            if ( m_current == wxNOT_FOUND )
                current = GetLineCount() - 1;
            else if ( m_current != 0 )
                current = m_current - 1;
            else // m_current == 0
                return;
            break;

        case WXK_PAGEDOWN:
            PageDown();
            current = GetFirstVisibleLine();
            break;

        case WXK_PAGEUP:
            if ( m_current == (int)GetFirstVisibleLine() )
            {
                PageUp();
            }

            current = GetFirstVisibleLine();
            break;

        case WXK_SPACE:
            // hack: pressing space should work like a mouse click rather than
            // like a keyboard arrow press, so trick DoHandleItemClick() in
            // thinking we were clicked
            flags &= ~ItemClick_Kbd;
            current = m_current;
            break;

#ifdef __WXMSW__
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
// lmContentsBoxCtrol mouse handling
// ----------------------------------------------------------------------------

void lmContentsBoxCtrol::OnLeftDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    wxHtmlCell *cell;

    if ( !PhysicalCoordsToCell(pos, cell) )
    {
        event.Skip();      //continue processing the  event
        return;
    }

    //if ( !wxHtmlWindowMouseHelper::HandleMouseClick(cell, pos, event) )
    //{
    //    // no link was clicked, so let the listbox code handle the click (e.g.
    //    // by selecting another item in the list):
    //    event.Skip();      //continue processing the  event
    //}

    SetFocus();

    int item = HitTest(event.GetPosition());

    if ( item != wxNOT_FOUND )
    {
        int flags = 0;
        if ( event.ShiftDown() )
           flags |= ItemClick_Shift;

        // under Mac Apple-click is used in the same way as Ctrl-click
        // elsewhere
#ifdef __WXMAC__
        if ( event.MetaDown() )
#else
        if ( event.ControlDown() )
#endif
            flags |= ItemClick_Ctrl;

        DoHandleItemClick(item, flags);
    }
    wxHtmlWindowMouseHelper::HandleMouseClick(cell, pos, event);

}

void lmContentsBoxCtrol::OnLeftDClick(wxMouseEvent& eventMouse)
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
// lmContentsBoxCtrol implementation of wxHtmlListBoxWinInterface
// ----------------------------------------------------------------------------

void lmContentsBoxCtrol::SetHTMLWindowTitle(const wxString& WXUNUSED(title))
{
    // nothing to do
}

void lmContentsBoxCtrol::OnHTMLLinkClicked(const wxHtmlLinkInfo& link)
{
    OnLinkClicked(GetItemForCell(link.GetHtmlCell()), link);
}

void lmContentsBoxCtrol::OnLinkClicked(size_t WXUNUSED(n),
                                  const wxHtmlLinkInfo& link)
{
    wxHtmlLinkEvent event(GetId(), link);
    GetEventHandler()->ProcessEvent(event);
}

wxHtmlOpeningStatus
lmContentsBoxCtrol::OnHTMLOpeningURL(wxHtmlURLType WXUNUSED(type),
                                const wxString& WXUNUSED(url),
                                wxString *WXUNUSED(redirect)) const
{
    return wxHTML_OPEN;
}

wxPoint lmContentsBoxCtrol::HTMLCoordsToWindow(wxHtmlCell *cell,
                                          const wxPoint& pos) const
{
    return CellCoordsToPhysical(pos, cell);
}

wxWindow* lmContentsBoxCtrol::GetHTMLWindow() { return this; }

wxColour lmContentsBoxCtrol::GetHTMLBackgroundColour() const
{
    return GetBackgroundColour();
}

void lmContentsBoxCtrol::SetHTMLBackgroundColour(const wxColour& WXUNUSED(clr))
{
    // nothing to do
}

void lmContentsBoxCtrol::SetHTMLBackgroundImage(const wxBitmap& WXUNUSED(bmpBg))
{
    // nothing to do
}

void lmContentsBoxCtrol::SetHTMLStatusText(const wxString& WXUNUSED(text))
{
    // nothing to do
}

wxCursor lmContentsBoxCtrol::GetHTMLCursor(HTMLCursor type) const
{
    // we don't want to show text selection cursor in listboxes
    if (type == HTMLCursor_Text)
        return wxHtmlWindow::GetDefaultHTMLCursor(HTMLCursor_Default);

    // in all other cases, use the same cursor as wxHtmlWindow:
    return wxHtmlWindow::GetDefaultHTMLCursor(type);
}

// ----------------------------------------------------------------------------
// lmContentsBoxCtrol handling of HTML links
// ----------------------------------------------------------------------------

wxPoint lmContentsBoxCtrol::GetRootCellCoords(size_t n) const
{
    wxPoint pos(CELL_BORDER, CELL_BORDER);
    pos += GetMargins();
    pos.y += GetLinesHeight(GetFirstVisibleLine(), n);
    return pos;
}

bool lmContentsBoxCtrol::PhysicalCoordsToCell(wxPoint& pos, wxHtmlCell*& cell) const
{
    int n = HitTest(pos);
    if ( n == wxNOT_FOUND )
        return false;

    // convert mouse coordinates to coords relative to item's wxHtmlCell:
    pos -= GetRootCellCoords(n);

    CacheItem(n);
    cell = m_cache->Get(n);

    return true;
}

size_t lmContentsBoxCtrol::GetItemForCell(const wxHtmlCell *cell) const
{
    wxCHECK_MSG( cell, 0, _T("no cell") );

    cell = cell->GetRootCell();

    wxCHECK_MSG( cell, 0, _T("no root cell") );

    // the cell's ID contains item index, see CacheItem():
    unsigned long n;
    if ( !cell->GetId().ToULong(&n) )
    {
        wxFAIL_MSG( _T("unexpected root cell's ID") );
        return 0;
    }

    return n;
}

wxPoint lmContentsBoxCtrol::CellCoordsToPhysical(const wxPoint& pos, wxHtmlCell *cell) const
{
    return pos + GetRootCellCoords(GetItemForCell(cell));
}

void lmContentsBoxCtrol::OnInternalIdle()
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

#endif
