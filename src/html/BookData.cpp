/////////////////////////////////////////////////////////////////////////////
// Name:        helpdata.cpp
// Purpose:     lmBookData
// Notes:       Based on htmlhelp.cpp, implementing a monolithic
//              HTML Help controller class,  by Vaclav Slavik
// Author:      Harm van der Heijden and Vaclav Slavik
// RCS-ID:      $Id: helpdata.cpp,v 1.79 2005/01/06 11:52:05 RR Exp $
// Copyright:   (c) Harm van der Heijden and Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BookData.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_HTML && wxUSE_STREAMS

#ifndef WXPRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include <ctype.h>

#include "BookData.h"
#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/busyinfo.h"
#include "wx/encconv.h"
#include "wx/fontmap.h"
#include "wx/log.h"
#include "wx/html/htmlpars.h"
#include "wx/html/htmldefs.h"
#include "wx/html/htmlfilt.h"
#include "wx/filename.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxHtmlBookRecArray)
WX_DEFINE_OBJARRAY(wxHtmlHelpDataItems)

// BookData object stores and manages all book indexes.
// Html pages are not processed. When a page display is requested, the page is
// directtly loaded by the wxHtmlWindowd, LoadPage() method.



//-----------------------------------------------------------------------------
// static helper functions
//-----------------------------------------------------------------------------

// Reads one line, stores it into buf and returns pointer to new line or NULL.
static const wxChar* ReadLine(const wxChar *line, wxChar *buf, size_t bufsize)
{
    wxChar *writeptr = buf;
    wxChar *endptr = buf + bufsize - 1;
    const wxChar *readptr = line;

    while (*readptr != 0 && *readptr != _T('\r') && *readptr != _T('\n') &&
           writeptr != endptr)
        *(writeptr++) = *(readptr++);
    *writeptr = 0;
    while (*readptr == _T('\r') || *readptr == _T('\n'))
        readptr++;
    if (*readptr == 0)
        return NULL;
    else
        return readptr;
}



static int
wxHtmlHelpIndexCompareFunc(lmBookIndexItem **a, lmBookIndexItem **b)
{
    lmBookIndexItem *ia = *a;
    lmBookIndexItem *ib = *b;

    if (ia == NULL)
        return -1;
    if (ib == NULL)
        return 1;

    if (ia->parent == ib->parent)
    {
        return ia->name.CmpNoCase(ib->name);
    }
    else if (ia->level == ib->level)
    {
        return wxHtmlHelpIndexCompareFunc(&ia->parent, &ib->parent);
    }
    else
    {
        lmBookIndexItem *ia2 = ia;
        lmBookIndexItem *ib2 = ib;

        while (ia2->level > ib2->level)
        {
            ia2 = ia2->parent;
        }
        while (ib2->level > ia2->level)
        {
            ib2 = ib2->parent;
        }

        wxASSERT(ia2);
        wxASSERT(ib2);
        int res = wxHtmlHelpIndexCompareFunc(&ia2, &ib2);
        if (res != 0)
            return res;
        else if (ia->level > ib->level)
            return 1;
        else
            return -1;
    }
}

//-----------------------------------------------------------------------------
// HP_Parser
//-----------------------------------------------------------------------------

class HP_Parser : public wxHtmlParser
{
public:
    HP_Parser()
    {
        GetEntitiesParser()->SetEncoding(wxFONTENCODING_ISO8859_1);
    }

    wxObject* GetProduct() { return NULL; }

protected:
    virtual void AddText(const wxChar* WXUNUSED(txt)) {}

    DECLARE_NO_COPY_CLASS(HP_Parser)
};


//-----------------------------------------------------------------------------
// HP_TagHandler
//-----------------------------------------------------------------------------

class HP_TagHandler : public wxHtmlTagHandler
{
    private:
        wxString m_name, m_page;
        int m_level;
        int m_id;
        int m_index;
        int m_count;
        lmBookIndexItem *m_parentItem;
        lmBookRecord *m_book;

        wxHtmlHelpDataItems *m_data;

    public:
        HP_TagHandler(lmBookRecord *b) : wxHtmlTagHandler()
        {
            m_data = NULL;
            m_book = b;
            m_name = m_page = wxEmptyString;
            m_level = 0;
            m_id = wxID_ANY;
            m_count = 0;
            m_parentItem = NULL;
        }
        wxString GetSupportedTags() { return wxT("UL,OBJECT,PARAM"); }
        bool HandleTag(const wxHtmlTag& tag);

        void Reset(wxHtmlHelpDataItems& data)
        {
            m_data = &data;
            m_count = 0;
            m_level = 0;
            m_parentItem = NULL;
        }

    DECLARE_NO_COPY_CLASS(HP_TagHandler)
};


bool HP_TagHandler::HandleTag(const wxHtmlTag& tag)
{
    if (tag.GetName() == wxT("UL"))
    {
        lmBookIndexItem *oldparent = m_parentItem;
        m_level++;
        m_parentItem = (m_count > 0) ? &(*m_data)[m_data->size()-1] : NULL;
        ParseInner(tag);
        m_level--;
        m_parentItem = oldparent;
        return true;
    }
    else if (tag.GetName() == wxT("OBJECT"))
    {
        m_name = m_page = wxEmptyString;
        ParseInner(tag);

#if 0
         if (!page.IsEmpty())
        /* Valid HHW's file may contain only two object tags:

           <OBJECT type="text/site properties">
               <param name="ImageType" value="Folder">
           </OBJECT>

           or

           <OBJECT type="text/sitemap">
               <param name="Name" value="main page">
               <param name="Local" value="another.htm">
           </OBJECT>

           We're interested in the latter. !page.IsEmpty() is valid
           condition because text/site properties does not contain Local param
        */
#endif
        if (tag.GetParam(wxT("TYPE")) == wxT("text/sitemap"))
        {
            lmBookIndexItem *item = new lmBookIndexItem();
            item->parent = m_parentItem;
            item->level = m_level;
            item->id = m_id;
            item->page = m_page;
            item->name = m_name;

            item->book = m_book;
            m_data->Add(item);
            m_count++;
        }

        return true;
    }
    else
    { // "PARAM"
        if (m_name.empty() && tag.GetParam(wxT("NAME")) == wxT("Name"))
            m_name = tag.GetParam(wxT("VALUE"));
        if (tag.GetParam(wxT("NAME")) == wxT("Local"))
            m_page = tag.GetParam(wxT("VALUE"));
        if (tag.GetParam(wxT("NAME")) == wxT("ID"))
            tag.GetParamAsInt(wxT("VALUE"), &m_id);
        return false;
    }
}


//-----------------------------------------------------------------------------
// lmBookData
//-----------------------------------------------------------------------------

wxString lmBookRecord::GetFullPath(const wxString &page) const
{
    if (wxIsAbsolutePath(page))
        return page;
    else
        return m_BasePath + page;
}

wxString lmBookIndexItem::GetIndentedName() const
{
    wxString s;
    for (int i = 1; i < level; i++)
        s << _T("   ");
    s << name;
    return s;
}


IMPLEMENT_DYNAMIC_CLASS(lmBookData, wxObject)

lmBookData::lmBookData()
{
}

lmBookData::~lmBookData()
{
}

bool lmBookData::LoadMSProject(lmBookRecord *book, wxFileSystem& fsys,
                                   const wxString& indexfile,
                                   const wxString& contentsfile)
{
    wxFSFile *f;
    wxHtmlFilterHTML filter;
    wxString buf;
    wxString string;

    HP_Parser parser;
    HP_TagHandler *handler = new HP_TagHandler(book);
    parser.AddTagHandler(handler);

    f = ( contentsfile.IsEmpty() ? (wxFSFile*) NULL : fsys.OpenFile(contentsfile) );
    if (f)
    {
        buf.clear();
        buf = filter.ReadFile(*f);
        delete f;
        handler->Reset(m_contents);
        parser.Parse(buf);
    }
    else
    {
        wxLogError(_("Cannot open contents file: %s"), contentsfile.c_str());
    }

    f = ( indexfile.IsEmpty() ? (wxFSFile*) NULL : fsys.OpenFile(indexfile) );
    if (f)
    {
        buf.clear();
        buf = filter.ReadFile(*f);
        delete f;
        handler->Reset(m_index);
        parser.Parse(buf);
    }
    else if (!indexfile.IsEmpty())
    {
        wxLogError(_("Cannot open index file: %s"), indexfile.c_str());
    }
    return true;
}

inline static void CacheWriteInt32(wxOutputStream *f, wxInt32 value)
{
    wxInt32 x = wxINT32_SWAP_ON_BE(value);
    f->Write(&x, sizeof(x));
}

inline static wxInt32 CacheReadInt32(wxInputStream *f)
{
    wxInt32 x;
    f->Read(&x, sizeof(x));
    return wxINT32_SWAP_ON_BE(x);
}

inline static void CacheWriteString(wxOutputStream *f, const wxString& str)
{
    const wxWX2MBbuf mbstr = str.mb_str(wxConvUTF8);
    size_t len = strlen((const char*)mbstr)+1;
    CacheWriteInt32(f, len);
    f->Write((const char*)mbstr, len);
}

inline static wxString CacheReadString(wxInputStream *f)
{
    size_t len = (size_t)CacheReadInt32(f);
    wxCharBuffer str(len-1);
    f->Read(str.data(), len);
    return wxString(str, wxConvUTF8);
}

#define CURRENT_CACHED_BOOK_VERSION     5

// Additional flags to detect incompatibilities of the runtime environment:
#define CACHED_BOOK_FORMAT_FLAGS \
                     (wxUSE_UNICODE << 0)


bool lmBookData::LoadCachedBook(lmBookRecord *book, wxInputStream *f)
{
    int i, st, newsize;
    wxInt32 version;

    /* load header - version info : */
    version = CacheReadInt32(f);

    if (version != CURRENT_CACHED_BOOK_VERSION)
    {
        // NB: We can just silently return false here and don't worry about
        //     it anymore, because AddBookParam will load the MS project in
        //     absence of (properly versioned) .cached file and automatically
        //     create new .cached file immediately afterward.
        return false;
    }

    if (CacheReadInt32(f) != CACHED_BOOK_FORMAT_FLAGS)
        return false;

    /* load contents : */
    st = m_contents.size();
    newsize = st + CacheReadInt32(f);
    m_contents.Alloc(newsize);
    for (i = st; i < newsize; i++)
    {
        lmBookIndexItem *item = new lmBookIndexItem;
        item->level = CacheReadInt32(f);
        item->id = CacheReadInt32(f);
        item->name = CacheReadString(f);
        item->page = CacheReadString(f);
        item->book = book;
        m_contents.Add(item);
    }

    /* load index : */
    st = m_index.size();
    newsize = st + CacheReadInt32(f);
    m_index.Alloc(newsize);
    for (i = st; i < newsize; i++)
    {
        lmBookIndexItem *item = new lmBookIndexItem;
        item->name = CacheReadString(f);
        item->page = CacheReadString(f);
        item->level = CacheReadInt32(f);
        item->book = book;
        int parentShift = CacheReadInt32(f);
        if (parentShift != 0)
            item->parent = &m_index[m_index.size() - parentShift];
        m_index.Add(item);
    }
    return true;
}


bool lmBookData::SaveCachedBook(lmBookRecord *book, wxOutputStream *f)
{
    int i;
    wxInt32 cnt;

    /* save header - version info : */
    CacheWriteInt32(f, CURRENT_CACHED_BOOK_VERSION);
    CacheWriteInt32(f, CACHED_BOOK_FORMAT_FLAGS);

    /* save contents : */
    int len = m_contents.size();
    for (cnt = 0, i = 0; i < len; i++)
        if (m_contents[i].book == book && m_contents[i].level > 0)
            cnt++;
    CacheWriteInt32(f, cnt);

    for (i = 0; i < len; i++)
    {
        if (m_contents[i].book != book || m_contents[i].level == 0)
            continue;
        CacheWriteInt32(f, m_contents[i].level);
        CacheWriteInt32(f, m_contents[i].id);
        CacheWriteString(f, m_contents[i].name);
        CacheWriteString(f, m_contents[i].page);
    }

    /* save index : */
    len = m_index.size();
    for (cnt = 0, i = 0; i < len; i++)
        if (m_index[i].book == book && m_index[i].level > 0)
            cnt++;
    CacheWriteInt32(f, cnt);

    for (i = 0; i < len; i++)
    {
        if (m_index[i].book != book || m_index[i].level == 0)
            continue;
        CacheWriteString(f, m_index[i].name);
        CacheWriteString(f, m_index[i].page);
        CacheWriteInt32(f, m_index[i].level);
        // save distance to parent item, if any:
        if (m_index[i].parent == NULL)
        {
            CacheWriteInt32(f, 0);
        }
        else
        {
            int cnt = 0;
            lmBookIndexItem *parent = m_index[i].parent;
            for (int j = i-1; j >= 0; j--)
            {
                if (m_index[j].book == book && m_index[j].level > 0)
                    cnt++;
                if (&m_index[j] == parent)
                    break;
            }
            wxASSERT(cnt > 0);
            CacheWriteInt32(f, cnt);
        }
    }
    return true;
}


void lmBookData::SetTempDir(const wxString& path)
{
    if (path.empty())
        m_tempPath = path;
    else
    {
        if (wxIsAbsolutePath(path)) m_tempPath = path;
        else m_tempPath = wxGetCwd() + _T("/") + path;

        if (m_tempPath[m_tempPath.Length() - 1] != _T('/'))
            m_tempPath << _T('/');
    }
}



static wxString SafeFileName(const wxString& s)
{
    wxString res(s);
    res.Replace(wxT("#"), wxT("_"));
    res.Replace(wxT(":"), wxT("_"));
    res.Replace(wxT("\\"), wxT("_"));
    res.Replace(wxT("/"), wxT("_"));
    return res;
}

bool lmBookData::AddBookParam(const wxFSFile& bookfile,
                                  wxFontEncoding encoding,
                                  const wxString& title, const wxString& contfile,
                                  const wxString& indexfile, const wxString& deftopic,
                                  const wxString& path)
{
    //Creates a BookRecord with the info obtained from the hhb file.
    //Then, loads the content file (hhc) by calling m_contents.Add(bookitem)
    //And finally loads the index file (hhk) by calling

    wxFileSystem fsys;
    wxFSFile *fi;
    lmBookRecord *bookr;

    int IndexOld = m_index.size(),
        ContentsOld = m_contents.size();

    if (!path.IsEmpty())
        fsys.ChangePathTo(path, true);

    size_t booksCnt = m_bookRecords.GetCount();
    for (size_t i = 0; i < booksCnt; i++)
    {
        if ( m_bookRecords[i].GetBookFile() == bookfile.GetLocation() )
            return true; // book is (was) loaded
    }

    bookr = new lmBookRecord(bookfile.GetLocation(), fsys.GetPath(), title, deftopic);

    lmBookIndexItem *bookitem = new lmBookIndexItem;
    bookitem->level = 0;
    bookitem->id = 0;
    bookitem->page = deftopic;
    bookitem->name = title;
    bookitem->book = bookr;

    // store the contents index for later
    int cont_start = m_contents.size();

    m_contents.Add(bookitem);

    // Try to find cached binary versions:
    // 1. save file as book, but with .hhp.cached extension
    // 2. same as 1. but in temp path
    // 3. otherwise or if cache load failed, load it from MS.

    fi = fsys.OpenFile(bookfile.GetLocation() + wxT(".cached"));

    if (fi == NULL ||
#if wxUSE_DATETIME
          fi->GetModificationTime() < bookfile.GetModificationTime() ||
#endif // wxUSE_DATETIME
          !LoadCachedBook(bookr, fi->GetStream()))
    {
        if (fi != NULL) delete fi;
        fi = fsys.OpenFile(m_tempPath + wxFileNameFromPath(bookfile.GetLocation()) + wxT(".cached"));
        if (m_tempPath.empty() || fi == NULL ||
#if wxUSE_DATETIME
            fi->GetModificationTime() < bookfile.GetModificationTime() ||
#endif // wxUSE_DATETIME
            !LoadCachedBook(bookr, fi->GetStream()))
        {
            LoadMSProject(bookr, fsys, indexfile, contfile);
            if (!m_tempPath.empty())
            {
                wxFileOutputStream *outs = new wxFileOutputStream(m_tempPath +
                                                  SafeFileName(wxFileNameFromPath(bookfile.GetLocation())) + wxT(".cached"));
                SaveCachedBook(bookr, outs);
                delete outs;
            }
        }
    }

    if (fi != NULL) delete fi;

    // Now store the contents range
    bookr->SetContentsRange(cont_start, m_contents.size());

#if wxUSE_WCHAR_T
    // MS HTML Help files [written by MS HTML Help Workshop] are broken
    // in that the data are iso-8859-1 (including HTML entities), but must
    // be interpreted as being in language's windows charset. Correct the
    // differences here and also convert to wxConvLocal in ANSI build
    if (encoding != wxFONTENCODING_SYSTEM)
    {
        #if wxUSE_UNICODE
            #define CORRECT_STR(str, conv) \
                str = wxString((str).mb_str(wxConvISO8859_1), conv)
        #else
            #define CORRECT_STR(str, conv) \
                str = wxString((str).wc_str(conv), wxConvLocal)
        #endif
        wxCSConv conv(encoding);
        size_t IndexCnt = m_index.size();
        size_t ContentsCnt = m_contents.size();
        size_t i;
        for (i = IndexOld; i < IndexCnt; i++)
        {
            CORRECT_STR(m_index[i].name, conv);
        }
        for (i = ContentsOld; i < ContentsCnt; i++)
        {
            CORRECT_STR(m_contents[i].name, conv);
        }
        #undef CORRECT_STR
    }
#else
    wxUnusedVar(IndexOld);
    wxUnusedVar(ContentsOld);
    wxASSERT_MSG(encoding == wxFONTENCODING_SYSTEM, wxT("Help files need charset conversion, but wxUSE_WCHAR_T is 0"));
#endif // wxUSE_WCHAR_T/!wxUSE_WCHAR_T

    m_bookRecords.Add(bookr);
    if (!m_index.empty())
    {
        m_index.Sort(wxHtmlHelpIndexCompareFunc);
    }

    return true;
}


bool lmBookData::AddBook(const wxString& book)
{
    //Reads a book (either a .zip, .htb ot .hhp file) and loads its content

    //In this loop we deal with htb files, reading its hhp sub-file
    wxString extension(book.Right(4).Lower());
    if (extension == wxT(".zip") ||
        extension == wxT(".htb") /*html book*/)
    {
        wxFileSystem fsys;
        wxString s;
        bool rt = false;

        s = fsys.FindFirst(book + wxT("#zip:*.hhp"), wxFILE);

        while (!s.IsEmpty())
        {
            if (AddBook(s)) rt = true;
            s = fsys.FindNext();
        }

        return rt;
    }

    //In this part we deal with only with the hhp file, to get its information
    //The book info is added by calling AddBookParam()
    wxFSFile *fi;
    wxFileSystem fsys;

    wxString title = _("noname"),
             safetitle,
             start = wxEmptyString,
             contents = wxEmptyString,
             index = wxEmptyString,
             charset = wxEmptyString;

    fi = fsys.OpenFile(book);
    if (fi == NULL)
    {
        wxLogError(_("Cannot open HTML help book: %s"), book.c_str());
        return false;
    }
    fsys.ChangePathTo(book);

    const wxChar *lineptr;
    wxChar linebuf[300];
    wxString tmp;
    wxHtmlFilterPlainText filter;
    tmp = filter.ReadFile(*fi);
    lineptr = tmp.c_str();

    do
    {
        lineptr = ReadLine(lineptr, linebuf, 300);

        for (wxChar *ch = linebuf; *ch != wxT('\0') && *ch != wxT('='); ch++)
           *ch = (wxChar)wxTolower(*ch);

        if (wxStrstr(linebuf, _T("title=")) == linebuf)
            title = linebuf + wxStrlen(_T("title="));
        if (wxStrstr(linebuf, _T("default topic=")) == linebuf)
            start = linebuf + wxStrlen(_T("default topic="));
        if (wxStrstr(linebuf, _T("index file=")) == linebuf)
            index = linebuf + wxStrlen(_T("index file="));
        if (wxStrstr(linebuf, _T("contents file=")) == linebuf)
            contents = linebuf + wxStrlen(_T("contents file="));
        if (wxStrstr(linebuf, _T("charset=")) == linebuf)
            charset = linebuf + wxStrlen(_T("charset="));
    } while (lineptr != NULL);

    wxFontEncoding enc = wxFONTENCODING_SYSTEM;
#if wxUSE_FONTMAP
    if (charset != wxEmptyString)
        enc = wxFontMapper::Get()->CharsetToEncoding(charset);
#endif

    bool rtval = AddBookParam(*fi, enc,
                              title, contents, index, start, fsys.GetPath());
    delete fi;

    return rtval;
}

wxString lmBookData::FindPageByName(const wxString& x)
{
    int cnt;
    int i;
    wxFileSystem fsys;
    wxFSFile *f;

    /* 1. try to open given file: */

    cnt = m_bookRecords.GetCount();
    for (i = 0; i < cnt; i++)
    {
        f = fsys.OpenFile(m_bookRecords[i].GetFullPath(x));
        if (f)
        {
            wxString url = m_bookRecords[i].GetFullPath(x);
            delete f;
            return url;
        }
    }


    /* 2. try to find a book: */

    for (i = 0; i < cnt; i++)
    {
        if (m_bookRecords[i].GetTitle() == x)
            return m_bookRecords[i].GetFullPath(m_bookRecords[i].GetStart());
    }

    /* 3. try to find in contents: */

    cnt = m_contents.size();
    for (i = 0; i < cnt; i++)
    {
        if (m_contents[i].name == x)
            return m_contents[i].GetFullPath();
    }


    /* 4. try to find in index: */

    cnt = m_index.size();
    for (i = 0; i < cnt; i++)
    {
        if (m_index[i].name == x)
            return m_index[i].GetFullPath();
    }

    return wxEmptyString;
}

wxString lmBookData::FindPageById(int id)
{
    size_t cnt = m_contents.size();
    for (size_t i = 0; i < cnt; i++)
    {
        if (m_contents[i].id == id)
        {
            return m_contents[i].GetFullPath();
        }
    }

    return wxEmptyString;
}


//----------------------------------------------------------------------------------
// wxHtmlSearchStatus functions
//----------------------------------------------------------------------------------

wxHtmlSearchStatus::wxHtmlSearchStatus(lmBookData* data, const wxString& keyword,
                                       bool case_sensitive, bool whole_words_only,
                                       const wxString& book)
{
    m_Data = data;
    m_Keyword = keyword;
    lmBookRecord* bookr = NULL;
    if (book != wxEmptyString)
    {
        // we have to search in a specific book. Find it first
        int i, cnt = data->m_bookRecords.GetCount();
        for (i = 0; i < cnt; i++)
            if (data->m_bookRecords[i].GetTitle() == book)
            {
                bookr = &(data->m_bookRecords[i]);
                m_CurIndex = bookr->GetContentsStart();
                m_MaxIndex = bookr->GetContentsEnd();
                break;
            }
        // check; we won't crash if the book doesn't exist, but it's Bad Anyway.
        wxASSERT(bookr);
    }
    if (! bookr)
    {
        // no book specified; search all books
        m_CurIndex = 0;
        m_MaxIndex = m_Data->m_contents.size();
    }
    m_Engine.LookFor(keyword, case_sensitive, whole_words_only);
    m_Active = (m_CurIndex < m_MaxIndex);
}

bool wxHtmlSearchStatus::Search()
{
    wxFSFile *file;
    int i = m_CurIndex;  // shortcut
    bool found = false;
    wxString thepage;

    if (!m_Active)
    {
        // sanity check. Illegal use, but we'll try to prevent a crash anyway
        wxASSERT(m_Active);
        return false;
    }

    m_Name = wxEmptyString;
    m_CurItem = NULL;
    thepage = m_Data->m_contents[i].page;

    m_Active = (++m_CurIndex < m_MaxIndex);
    // check if it is same page with different anchor:
    if (!m_LastPage.empty())
    {
        const wxChar *p1, *p2;
        for (p1 = thepage.c_str(), p2 = m_LastPage.c_str();
             *p1 != 0 && *p1 != _T('#') && *p1 == *p2; p1++, p2++) {}

        m_LastPage = thepage;

        if (*p1 == 0 || *p1 == _T('#'))
            return false;
    }
    else m_LastPage = thepage;

    wxFileSystem fsys;
    file = fsys.OpenFile(m_Data->m_contents[i].book->GetFullPath(thepage));
    if (file)
    {
        if (m_Engine.Scan(*file))
        {
            m_Name = m_Data->m_contents[i].name;
            m_CurItem = &m_Data->m_contents[i];
            found = true;
        }
        delete file;
    }
    return found;
}








//--------------------------------------------------------------------------------
// lmBookSearchEngine
//--------------------------------------------------------------------------------

void lmBookSearchEngine::LookFor(const wxString& keyword, bool case_sensitive, bool whole_words_only)
{
    m_CaseSensitive = case_sensitive;
    m_WholeWords = whole_words_only;
    m_Keyword = keyword;

    if (!m_CaseSensitive)
        m_Keyword.LowerCase();
}


static inline bool WHITESPACE(wxChar c)
{
    return c == _T(' ') || c == _T('\n') || c == _T('\r') || c == _T('\t');
}

bool lmBookSearchEngine::Scan(const wxFSFile& file)
{
    wxASSERT_MSG(!m_Keyword.empty(), wxT("lmBookSearchEngine::LookFor must be called before scanning!"));

    int i, j;
    int wrd = m_Keyword.Length();
    bool found = false;
    wxHtmlFilterHTML filter;
    wxString tmp = filter.ReadFile(file);
    int lng = tmp.length();
    const wxChar *buf = tmp.c_str();

    if (!m_CaseSensitive)
        tmp.LowerCase();

    const wxChar *kwd = m_Keyword.c_str();

    if (m_WholeWords)
    {
        for (i = 0; i < lng - wrd; i++)
        {
            if (WHITESPACE(buf[i])) continue;
            j = 0;
            while ((j < wrd) && (buf[i + j] == kwd[j])) j++;
            if (j == wrd && WHITESPACE(buf[i + j])) { found = true; break; }
        }
    }

    else
    {
        for (i = 0; i < lng - wrd; i++)
        {
            j = 0;
            while ((j < wrd) && (buf[i + j] == kwd[j])) j++;
            if (j == wrd) { found = true; break; }
        }
    }

    return found;
}



#endif
