///////////////////////////////////////////////////////////////////////////////
// Name:        generic/htmllbox.cpp
// Purpose:     implementation of lmHtmlListBox
// Author:      Vadim Zeitlin
// Modified by:
// Created:     31.05.03
// RCS-ID:      $Id: htmllbox.cpp,v 1.26 2006/11/04 12:18:53 VZ Exp $
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
#endif //WX_PRECOMP

#include "HtmlListBox.h"

#include "wx/html/htmlcell.h"
#include "wx/html/winpars.h"

// this hack forces the linker to always link in m_* files
#include "wx/html/forcelnk.h"
FORCE_WXHTML_MODULES()

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// small border always added to the cells:
static const wxCoord CELL_BORDER = 2;

const wxString lmHtmlListBoxNameStr = wxT("htmlListBox");

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
        lmHLB_TagHandler() : wxHtmlWinTagHandler()
        {
        }
        wxString GetSupportedTags() { return wxT("AX,TOCITEM"); }
        bool HandleTag(const wxHtmlTag& tag);

    DECLARE_NO_COPY_CLASS(lmHLB_TagHandler)
};


bool lmHLB_TagHandler::HandleTag(const wxHtmlTag& tag)
{
    if (tag.GetName() == wxT("AX"))
    {
        if (tag.HasParam( wxT("HREF") ))
        {
            wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
            //wxColour oldclr = m_WParser->GetActualColor();
            //int oldund = m_WParser->GetFontUnderlined();
            wxString name(tag.GetParam( wxT("HREF") )), target;

            if (tag.HasParam( wxT("TARGET") )) target = tag.GetParam( wxT("TARGET") );
            //m_WParser->SetActualColor(m_WParser->GetLinkColor());
            //m_WParser->GetContainer()->InsertCell(new wxHtmlColourCell(m_WParser->GetLinkColor()));
            //m_WParser->SetFontUnderlined(true);
            //m_WParser->GetContainer()->InsertCell(new wxHtmlFontCell(m_WParser->CreateCurrentFont()));
            m_WParser->SetLink(wxHtmlLinkInfo(name, target));
            //m_WParser->GetContainer()->SetBackgroundColour(*wxRED);

            ParseInner(tag);

            m_WParser->SetLink(oldlnk);
            //m_WParser->SetFontUnderlined(oldund);
            //m_WParser->GetContainer()->InsertCell(new wxHtmlFontCell(m_WParser->CreateCurrentFont()));
            //m_WParser->SetActualColor(oldclr);
            //m_WParser->GetContainer()->InsertCell(new wxHtmlColourCell(oldclr));

            return true;
        }
        else return false;
    }
    else if (tag.GetName() == wxT("TOCITEM"))
    {
        // Get all parameters

        // item level
        long nLevel;
        if (!tag.HasParam( _T("LEVEL") )) wxASSERT(false);
        wxString sLevel(tag.GetParam( _T("LEVEL") ));
        if (!sLevel.ToLong(&nLevel)) wxASSERT(false);

        // item number
        long nItem;
        if (!tag.HasParam( _T("ITEM") )) wxASSERT(false);
        wxString sItem(tag.GetParam( _T("ITEM") ));
        if (!sItem.ToLong(&nItem)) wxASSERT(false);

        // expand icon
        if (!tag.HasParam( _T("EXPAND") )) wxASSERT(false);
        wxString sExpand(tag.GetParam( _T("EXPAND") ));

        // item icon
        if (!tag.HasParam( _T("ICON") )) wxASSERT(false);
        wxString sIcon(tag.GetParam( _T("ICON") ));

        // image (optional)
        wxString sImage = wxEmptyString;
        if (tag.HasParam( _T("IMG") )) {
            sImage = tag.GetParam( _T("IMG") );
        }

            //Create the entry

        // indentation
        int nIndent = nLevel * 24;
        if (sExpand == _T("no")) nIndent += 16;
        m_WParser->GetContainer()->SetIndent(nIndent, wxHTML_INDENT_LEFT);

        // aligment
        m_WParser->GetContainer()->SetAlignVer(wxHTML_ALIGN_CENTER);

        wxString sPath = _T("c:\\usr\\desarrollo_wx\\lenmus\\res\\icons\\");
        // expand / collapse image
        if (sExpand == _T("+")) {
            wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
            wxString name = wxString::Format(_T("open%d"), nItem);
            m_WParser->SetLink(wxHtmlLinkInfo(name, wxEmptyString));

            m_WParser->SetSourceAndSaveState(_T("<img src='") + sPath + _T("nav_plus_16.png' width='16' height='16' />"));
            m_WParser->DoParsing();
            m_WParser->RestoreState();

            m_WParser->SetLink(oldlnk);
        }
        else if (sExpand == _T("-")) {
            wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
            wxString name = wxString::Format(_T("close%d"), nItem);
            m_WParser->SetLink(wxHtmlLinkInfo(name, wxEmptyString));

            m_WParser->SetSourceAndSaveState(_T("<img src='") + sPath + _T("nav_minus_16.png' width='16' height='16' />"));
            m_WParser->DoParsing();
            m_WParser->RestoreState();

            m_WParser->SetLink(oldlnk);

        }

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
        else {
            wxASSERT(false);
        }
        m_WParser->SetSourceAndSaveState(_T("<img src='") + sIconImg + _T("' />"));
        m_WParser->DoParsing();
        m_WParser->RestoreState();
        m_WParser->GetContainer()->InsertCell(new lmHtmlSpacerCell(8));

        // item image
        // Only drawn in final items
        bool fDrawImage = (sImage != wxEmptyString && sIcon == _T("page"));
        if (fDrawImage) {
            wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
            wxString name = wxString::Format(_T("item%d"), nItem);
            m_WParser->SetLink(wxHtmlLinkInfo(name, wxEmptyString));

            m_WParser->SetSourceAndSaveState(_T("<img src='") + sImage + _T("' height='36' />"));
            m_WParser->DoParsing();
            m_WParser->RestoreState();

            m_WParser->SetLink(oldlnk);
        }

        // item text
        if (fDrawImage) {
            m_WParser->CloseContainer();
            m_WParser->OpenContainer();
            m_WParser->GetContainer()->SetIndent(nIndent+25, wxHTML_INDENT_LEFT);
        }
        m_WParser->GetContainer()->SetWidthFloat(10000, wxHTML_UNITS_PIXELS);   //force no wrap

        wxHtmlLinkInfo oldlnk = m_WParser->GetLink();
        wxString name = wxString::Format(_T("item%d"), nItem);
        m_WParser->SetLink(wxHtmlLinkInfo(name, wxEmptyString));

        ParseInner(tag);
        m_WParser->SetLink(oldlnk);

        return true;

    }
    return false;   //to get compiler happy
}



// ----------------------------------------------------------------------------
// lmHtmlListBoxCache
// ----------------------------------------------------------------------------

// this class is used by lmHtmlListBox to cache the parsed representation of
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
// lmHtmlListBoxStyle
// ----------------------------------------------------------------------------

// just forward wxDefaultHtmlRenderingStyle callbacks to the main class so that
// they could be overridden by the user code
class lmHtmlListBoxStyle : public wxDefaultHtmlRenderingStyle
{
public:
    lmHtmlListBoxStyle(const lmHtmlListBox& hlbox) : m_hlbox(hlbox) { }

    virtual wxColour GetSelectedTextColour(const wxColour& colFg)
    {
        return m_hlbox.GetSelectedTextColour(colFg);
    }

    virtual wxColour GetSelectedTextBgColour(const wxColour& colBg)
    {
        return m_hlbox.GetSelectedTextBgColour(colBg);
    }

private:
    const lmHtmlListBox& m_hlbox;

    DECLARE_NO_COPY_CLASS(lmHtmlListBoxStyle)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmHtmlListBox, wxVListBox)
    EVT_SIZE(lmHtmlListBox::OnSize)
    EVT_MOTION(lmHtmlListBox::OnMouseMove)
    EVT_LEFT_DOWN(lmHtmlListBox::OnLeftDown)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_ABSTRACT_CLASS(lmHtmlListBox, wxVListBox)


// ----------------------------------------------------------------------------
// lmHtmlListBox creation
// ----------------------------------------------------------------------------

lmHtmlListBox::lmHtmlListBox()
    : wxHtmlWindowMouseHelper(this)
{
    Init();
}

// normal constructor which calls Create() internally
lmHtmlListBox::lmHtmlListBox(wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
    : wxHtmlWindowMouseHelper(this)
{
    Init();

    (void)Create(parent, id, pos, size, style, name);
}

void lmHtmlListBox::Init()
{
    m_htmlParser = NULL;
    m_htmlRendStyle = new lmHtmlListBoxStyle(*this);
    m_cache = new lmHtmlListBoxCache;
    m_pTagHandler = (wxHtmlWinTagHandler*)NULL;

}

bool lmHtmlListBox::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
{
    return wxVListBox::Create(parent, id, pos, size, style, name);
}

lmHtmlListBox::~lmHtmlListBox()
{
    delete m_cache;

    if ( m_htmlParser )
    {
        delete m_htmlParser->GetDC();
        delete m_htmlParser;
    }
    if (m_pTagHandler)
        delete m_pTagHandler;

    delete m_htmlRendStyle;
}

// ----------------------------------------------------------------------------
// lmHtmlListBox appearance
// ----------------------------------------------------------------------------

wxColour lmHtmlListBox::GetSelectedTextColour(const wxColour& colFg) const
{
    return *wxWHITE; //m_htmlRendStyle->
                //wxDefaultHtmlRenderingStyle::GetSelectedTextColour(colFg);
}

wxColour
lmHtmlListBox::GetSelectedTextBgColour(const wxColour& WXUNUSED(colBg)) const
{
    return wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT); //GetSelectionBackground();
}

// ----------------------------------------------------------------------------
// lmHtmlListBox items markup
// ----------------------------------------------------------------------------

wxString lmHtmlListBox::OnGetItemMarkup(size_t n) const
{
    // we don't even need to wrap the value returned by OnGetItem() inside
    // "<html><body>" and "</body></html>" because wxHTML can parse it even
    // without these tags
    return OnGetItem(n);
}

// ----------------------------------------------------------------------------
// lmHtmlListBox cache handling
// ----------------------------------------------------------------------------

void lmHtmlListBox::CacheItem(size_t n) const
{
    if ( !m_cache->Has(n) )
    {
        if ( !m_htmlParser )
        {
            lmHtmlListBox *self = wxConstCast(this, lmHtmlListBox);

            self->m_htmlParser = new wxHtmlWinParser(self);
            lmHLB_TagHandler* handler = new lmHLB_TagHandler();
            //m_pTagHandler = (wxHtmlWinTagHandler*)handler;
            m_htmlParser->AddTagHandler(handler);

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

void lmHtmlListBox::OnSize(wxSizeEvent& event)
{
    // we need to relayout all the cached cells
    m_cache->Clear();

    event.Skip();
}

void lmHtmlListBox::RefreshLine(size_t line)
{
    m_cache->InvalidateRange(line, line);

    wxVListBox::RefreshLine(line);
}

void lmHtmlListBox::RefreshLines(size_t from, size_t to)
{
    m_cache->InvalidateRange(from, to);

    wxVListBox::RefreshLines(from, to);
}

void lmHtmlListBox::RefreshAll()
{
    m_cache->Clear();

    wxVListBox::RefreshAll();
}

void lmHtmlListBox::SetItemCount(size_t count)
{
    // the items are going to change, forget the old ones
    m_cache->Clear();

    wxVListBox::SetItemCount(count);
}

// ----------------------------------------------------------------------------
// lmHtmlListBox implementation of wxVListBox pure virtuals
// ----------------------------------------------------------------------------

void lmHtmlListBox::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
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

    // note that we can't stop drawing exactly at the window boundary as then
    // even the visible cells part could be not drawn, so always draw the
    // entire cell
    cell->Draw(dc,
               rect.x + CELL_BORDER, rect.y + CELL_BORDER,
               0, INT_MAX, htmlRendInfo);
}

wxCoord lmHtmlListBox::OnMeasureItem(size_t n) const
{
    CacheItem(n);

    wxHtmlCell *cell = m_cache->Get(n);
    wxCHECK_MSG( cell, 0, _T("this cell should be cached!") );

    return cell->GetHeight() + cell->GetDescent(); // + 4;
}

void lmHtmlListBox::OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const
{
    // we need to render selected and current items differently
    const bool isSelected = IsSelected(n),
               isCurrent = IsCurrent(n);
    if ( isSelected || isCurrent )
    {
        if ( isSelected )
        {
            dc.SetBrush( wxBrush(GetSelectionBackground(), wxSOLID) );
        }
        else // !selected
        {
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
        }

        dc.SetPen(*(isCurrent ? wxBLACK_PEN : wxTRANSPARENT_PEN));

        //dc.DrawRectangle(rect);
    }
    //else: do nothing for the normal items
}


// ----------------------------------------------------------------------------
// lmHtmlListBox implementation of wxHtmlListBoxWinInterface
// ----------------------------------------------------------------------------

void lmHtmlListBox::SetHTMLWindowTitle(const wxString& WXUNUSED(title))
{
    // nothing to do
}

void lmHtmlListBox::OnHTMLLinkClicked(const wxHtmlLinkInfo& link)
{
    OnLinkClicked(GetItemForCell(link.GetHtmlCell()), link);
}

void lmHtmlListBox::OnLinkClicked(size_t WXUNUSED(n),
                                  const wxHtmlLinkInfo& link)
{
    wxHtmlLinkEvent event(GetId(), link);
    GetEventHandler()->ProcessEvent(event);
}

wxHtmlOpeningStatus
lmHtmlListBox::OnHTMLOpeningURL(wxHtmlURLType WXUNUSED(type),
                                const wxString& WXUNUSED(url),
                                wxString *WXUNUSED(redirect)) const
{
    return wxHTML_OPEN;
}

wxPoint lmHtmlListBox::HTMLCoordsToWindow(wxHtmlCell *cell,
                                          const wxPoint& pos) const
{
    return CellCoordsToPhysical(pos, cell);
}

wxWindow* lmHtmlListBox::GetHTMLWindow() { return this; }

wxColour lmHtmlListBox::GetHTMLBackgroundColour() const
{
    return GetBackgroundColour();
}

void lmHtmlListBox::SetHTMLBackgroundColour(const wxColour& WXUNUSED(clr))
{
    // nothing to do
}

void lmHtmlListBox::SetHTMLBackgroundImage(const wxBitmap& WXUNUSED(bmpBg))
{
    // nothing to do
}

void lmHtmlListBox::SetHTMLStatusText(const wxString& WXUNUSED(text))
{
    // nothing to do
}

wxCursor lmHtmlListBox::GetHTMLCursor(HTMLCursor type) const
{
    // we don't want to show text selection cursor in listboxes
    if (type == HTMLCursor_Text)
        return wxHtmlWindow::GetDefaultHTMLCursor(HTMLCursor_Default);

    // in all other cases, use the same cursor as wxHtmlWindow:
    return wxHtmlWindow::GetDefaultHTMLCursor(type);
}

// ----------------------------------------------------------------------------
// lmHtmlListBox handling of HTML links
// ----------------------------------------------------------------------------

wxPoint lmHtmlListBox::GetRootCellCoords(size_t n) const
{
    wxPoint pos(CELL_BORDER, CELL_BORDER);
    pos += GetMargins();
    pos.y += GetLinesHeight(GetFirstVisibleLine(), n);
    return pos;
}

bool lmHtmlListBox::PhysicalCoordsToCell(wxPoint& pos, wxHtmlCell*& cell) const
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

size_t lmHtmlListBox::GetItemForCell(const wxHtmlCell *cell) const
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

wxPoint
lmHtmlListBox::CellCoordsToPhysical(const wxPoint& pos, wxHtmlCell *cell) const
{
    return pos + GetRootCellCoords(GetItemForCell(cell));
}

void lmHtmlListBox::OnInternalIdle()
{
    wxVListBox::OnInternalIdle();

    if ( wxHtmlWindowMouseHelper::DidMouseMove() )
    {
        wxPoint pos = ScreenToClient(wxGetMousePosition());
        wxHtmlCell *cell;

        if ( !PhysicalCoordsToCell(pos, cell) )
            return;

        wxHtmlWindowMouseHelper::HandleIdle(cell, pos);
    }
}

void lmHtmlListBox::OnMouseMove(wxMouseEvent& event)
{
    wxHtmlWindowMouseHelper::HandleMouseMoved();
    event.Skip();
}

void lmHtmlListBox::OnLeftDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    wxHtmlCell *cell;

    if ( !PhysicalCoordsToCell(pos, cell) )
    {
        event.Skip();
        return;
    }

    if ( !wxHtmlWindowMouseHelper::HandleMouseClick(cell, pos, event) )
    {
        // no link was clicked, so let the listbox code handle the click (e.g.
        // by selecting another item in the list):
        event.Skip();
    }
}



