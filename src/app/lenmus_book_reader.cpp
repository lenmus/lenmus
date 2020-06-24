//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

#include "lenmus_book_reader.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/defs.h>
#include <wx/log.h>

namespace lenmus
{


//---------------------------------------------------------------------------------------
static int wxHtmlHelpIndexCompareFunc(BookIndexItem **a, BookIndexItem **b)
{
    BookIndexItem *ia = *a;
    BookIndexItem *ib = *b;

    if (ia == nullptr)
        return -1;
    if (ib == nullptr)
        return 1;

    if (ia->parent == ib->parent)
    {
        return ia->title.CmpNoCase(ib->title);
    }
    else if (ia->level == ib->level)
    {
        return wxHtmlHelpIndexCompareFunc(&ia->parent, &ib->parent);
    }
    else
    {
        BookIndexItem *ia2 = ia;
        BookIndexItem *ib2 = ib;

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


//---------------------------------------------------------------------------------------
// BookRecord
//---------------------------------------------------------------------------------------

BookRecord::BookRecord(const wxString& bookfile, const wxString& basepath,
                     const wxString& title, const wxString& start)
{
    m_sBookFile = bookfile;
    m_sBasePath = basepath;
    m_sTitle = title;
    m_sCoverPage = start;
    // for debugging, give the contents index obvious default values
    m_ContentsStart = m_ContentsEnd = -1;
}

//---------------------------------------------------------------------------------------
BookRecord::~BookRecord()
{
}

//---------------------------------------------------------------------------------------
wxString BookRecord::GetFullPath(const wxString &page) const
{
    // returns full filename of page (which is part of the book),
    // i.e. with book's basePath prepended. If page is already absolute
    // path, basePath is _not_ prepended.

    if (page != "")
    {
        if (wxIsAbsolutePath(page))
            return page;
        else
            return m_sBasePath + page;
        }
    else
        return wxEmptyString;
}

//---------------------------------------------------------------------------------------
// BookIndexItem
//---------------------------------------------------------------------------------------

wxString BookIndexItem::GetIndentedName() const
{
    wxString s;
    for (int i = 1; i < level; i++)
        s << "   ";
    s << title;
    return s;
}


//---------------------------------------------------------------------------------------
// BooksCollection
// BooksCollection object stores and manages all book indexes.
// Html pages are not processed. When a page display is requested, the page is
// directtly loaded by the wxHtmlWindowd, LoadPage() method.
//---------------------------------------------------------------------------------------
BooksCollection::BooksCollection()
{
    m_pParser = LENMUS_NEW XmlParser();
}

//---------------------------------------------------------------------------------------
BooksCollection::~BooksCollection()
{
    delete m_pParser;
    int i;
    for(i = m_bookRecords.GetCount(); i > 0; i--) {
        delete m_bookRecords[i-1];
        m_bookRecords.RemoveAt(i-1);
    }
    for (i = m_index.GetCount(); i > 0; i--) {
        delete m_index[i-1];
        m_index.RemoveAt(i-1);
    }
    for(i = m_contents.GetCount(); i > 0; i--) {
        delete m_contents[i-1];
        m_contents.RemoveAt(i-1);
    }
    for(i = m_pagelist.GetCount(); i > 0; i--) {
        delete m_pagelist[i-1];
        m_pagelist.RemoveAt(i-1);
    }

}

//---------------------------------------------------------------------------------------
void BooksCollection::SetTempDir(const wxString& path)
{
    if (path.empty())
        m_tempPath = path;
    else
    {
        if (wxIsAbsolutePath(path)) m_tempPath = path;
        else m_tempPath = wxGetCwd() + "/" + path;

        if (m_tempPath[m_tempPath.length() - 1] != _T('/'))
            m_tempPath << _T('/');
    }
}

//---------------------------------------------------------------------------------------
BookRecord* BooksCollection::add_book(const wxFileName& oFilename)
{
    //Reads a book and loads its content
    //Returns pointer to created book record, or nullptr if errors

    add_pages_to_list(oFilename);
    BookRecord* pBookr = add_book_toc(oFilename);
    delete_book_entries();
    return pBookr;
}

//---------------------------------------------------------------------------------------
bool BooksCollection::add_pages_to_list(const wxFileName& oFilename)
{
    // Returns true if error.
//    wxLogMessage("[BooksCollection::add_pages_to_list] starting");

    // open the zip file and load entries
    wxString sBookPath = oFilename.GetFullPath();
    wxFFileInputStream in(sBookPath);
    wxZipInputStream zip(in);
    if (!zip.IsOk())
    {
        LOMSE_LOG_ERROR("Loading eBook. Error: can not open file '%s'.",
                        sBookPath.ToStdString().c_str() );
        return true;   //error
    }
    load_book_entries(zip);

    determine_book_format(zip);
    switch(m_bookFormat)
    {
        case k_format_0:
            return add_lms_pages(zip, sBookPath);

        case k_format_1:
            return add_lmd_pages(zip, sBookPath);

        default:
            LOMSE_LOG_ERROR("Loading eBook. Error: file '%s' has invalid format.",
                            sBookPath.ToStdString().c_str() );
            return true;   //error
    }
}

//---------------------------------------------------------------------------------------
void BooksCollection::load_book_entries(wxZipInputStream& zip)
{
    wxZipEntry* pEntry = zip.GetNextEntry();
    while (pEntry != nullptr)
    {
        m_bookEntries[pEntry->GetInternalName()] = pEntry;
        pEntry = zip.GetNextEntry();
    }
}

//---------------------------------------------------------------------------------------
void BooksCollection::delete_book_entries()
{
	map<wxString, wxZipEntry*>::const_iterator it;
    for (it = m_bookEntries.begin(); it != m_bookEntries.end(); ++it)
        delete it->second;

    m_bookEntries.clear();
}

//---------------------------------------------------------------------------------------
wxZipEntry* BooksCollection::find_entry(const wxString& name)
{
	map<wxString, wxZipEntry*>::const_iterator it = m_bookEntries.find(name);
	if (it != m_bookEntries.end())
        return it->second;
    else
        return nullptr;
}

//---------------------------------------------------------------------------------------
bool BooksCollection::add_lms_pages(wxZipInputStream& WXUNUSED(zip), const wxString& sBookPath)
{
	map<wxString, wxZipEntry*>::const_iterator it;
    for (it = m_bookEntries.begin(); it != m_bookEntries.end(); ++it)
    {
        wxZipEntry* pEntry = it->second;
        wxString sPageName = pEntry->GetName();
        if (sPageName.Find(".lms") != wxNOT_FOUND)
        {
            //add entry to pagelist
//            wxLogMessage("[BooksCollection::add_lms_pages] Adding page '%s'", sPageName.wx_str());
            PageIndexItem *pItem = LENMUS_NEW PageIndexItem();
            pItem->page = sPageName;
            pItem->book = sBookPath;
            m_pagelist.Add(pItem);
        }
    }

    return false;   //no error
}

//---------------------------------------------------------------------------------------
bool BooksCollection::add_lmd_pages(wxZipInputStream& WXUNUSED(zip), const wxString& sBookPath)
{
	map<wxString, wxZipEntry*>::const_iterator it;
    for (it = m_bookEntries.begin(); it != m_bookEntries.end(); ++it)
    {
        wxZipEntry* pEntry = it->second;
        wxString sPageName = pEntry->GetName();
        if (sPageName.Find(".lmd") != wxNOT_FOUND)
        {
            //add entry to pagelist
            PageIndexItem *pItem = LENMUS_NEW PageIndexItem();
            pItem->page = sPageName.substr(8);    // remove folder "content/"
            //wxLogMessage("[BooksCollection::add_lmd_pages] Adding page '%s'", (pItem->page).wx_str());
            pItem->book = sBookPath;
            m_pagelist.Add(pItem);
        }
    }

    return false;   //no error
}

//---------------------------------------------------------------------------------------
void BooksCollection::determine_book_format(wxZipInputStream& WXUNUSED(zip))
{
    wxZipEntry* pEntry = find_entry("mimetype");
    if (pEntry)
    {
        //TODO: read the entry's data and verify it is ""
        m_bookFormat = k_format_1;     //new style, LMD files
    }
    else
        m_bookFormat = k_format_0;
}

//---------------------------------------------------------------------------------------
BookRecord* BooksCollection::add_book_toc(const wxFileName& oFilename)
{
    // Returns ptr to created book record if success, nullptr if failure

//    LOMSE_LOG_ERROR("Processing file %s",
//                     oFilename.GetFullPath().ToStdString().c_str());

    wxString sTitle = "",
             sPage = "",
             sContents = "",
             sIndex = "";


    // wxXmlDocument::Load(filename) uses a wxTextStreamFile and it doesn't support
    // virtual files. So, when using LMB files we have to allocate
    // a wxZipTextStream and pass it to wxXmlDocument::Load(stream)
    wxXmlDocument xdoc;
    bool fOK;
    wxString sFullName, sFileName, sPath, sNameExt;
    if (oFilename.GetExt() == "toc")
    {
        sFullName = oFilename.GetFullPath();
        sFileName = oFilename.GetName();
        sPath = oFilename.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR , wxPATH_NATIVE);
        fOK = xdoc.Load(sFullName);
    }
    else if (oFilename.GetExt() == "lmb")
    {
        //lenmus compressed book (zip file)
        sFileName = oFilename.GetName();
        sPath = oFilename.GetFullPath() + "#zip:";
        sNameExt = sFileName + ".toc";
        wxString sTocName;
        if (m_bookFormat == k_format_0)
        {
            sFullName = sPath + sNameExt;
            sTocName = sFileName + ".toc";
        }
        else    // k_format_1
        {
            sFullName = sPath + "META-INF/" + sNameExt;
            sTocName = "META-INF/" + sFileName + ".toc";
        }
        wxZipEntry* pEntry = find_entry( sTocName );

        //// convert the local name we are looking for into the zip internal format
        //wxString sInternalName = wxZipEntry::GetInternalName( sNameExt );

        //// open the zip
        //wxFFileInputStream in( oFilename.GetFullPath() );
        //wxZipInputStream zip(in);
        //if (!zip.IsOk()) {
        //    LOMSE_LOG_ERROR("Loading eBook. Error: TOC file '%s' not found.",
        //        oFilename.GetFullPath().ToStdString().c_str());
        //    return (BookRecord*) nullptr;   //error
        //}

        //// call GetNextEntry() until the required internal name is found
        //wxZipEntry* pEntry = (wxZipEntry*)nullptr;
        //do {
        //    if (pEntry) delete pEntry;      //delete previous entry
        //    pEntry = zip.GetNextEntry();    //now we have ownership of object *pEntry
        //}
        //while (pEntry && pEntry->GetInternalName() != sInternalName);

        if (!pEntry) {
            LOMSE_LOG_ERROR("Loading eBook. Error: TOC file '%s' not found.",
                            sFullName.ToStdString().c_str() );
            return (BookRecord*) nullptr;   //error
        }

        // open a new zip stream
        wxFFileInputStream in( oFilename.GetFullPath() );
        wxZipInputStream zip(in);
        if (!zip.IsOk()) {
            LOMSE_LOG_ERROR("Loading eBook. Error: TOC file '%s' not found.",
                            oFilename.GetFullPath().ToStdString().c_str() );
            return (BookRecord*) nullptr;   //error
        }

        zip.OpenEntry(*pEntry);
        fOK = xdoc.Load(zip);    //asumes utf-8
        zip.CloseEntry();
        //delete pEntry;
    }
    else
    {
        LOMSE_LOG_ERROR(
            "Loading eBook. Error in TOC file '%s'. Extension is neither LMB nor TOC.",
            oFilename.GetFullPath().ToStdString().c_str() );
        return (BookRecord*) nullptr;   //error
    }

    // load the XML file as tree of nodes
    if (!fOK)
    {
        LOMSE_LOG_ERROR("Loading eBook. Error parsing TOC file %s",
                        sFullName.ToStdString().c_str() );
        return (BookRecord*) nullptr;   //error
    }

    //Verify type of document. Must be <BookTOC>
    wxXmlNode *pNode = xdoc.GetRoot();
    wxString sTag = "lmBookTOC";
    wxString sElement = pNode->GetName();
    if (sElement != sTag)
    {
        LOMSE_LOG_ERROR("Loading eBook. Error: First tag is not <%s> but <%s>",
                        sTag.ToStdString().c_str(), sElement.ToStdString().c_str() );
        return (BookRecord*) nullptr;   //error
    }

    // firts node: title
    pNode = m_pParser->GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    sElement = pElement->GetName();
    sTag = "title";
    if (sElement != sTag)
    {
        LOMSE_LOG_ERROR("Loading eBook. Error: Expected tag <%s> but found <%s>",
                        sTag.ToStdString().c_str(), sElement.ToStdString().c_str() );
        return (BookRecord*) nullptr;   //error
    }
    sTitle = m_pParser->GetText(pNode);

    // next node: coverpage
    pNode = m_pParser->GetNextSibling(pNode);
    pElement = pNode;
    sElement = pElement->GetName();
    sTag = "coverpage";
    if (sElement != sTag)
    {
        LOMSE_LOG_ERROR("Loading eBook. Error: Expected tag <%s> but found <%s>",
                        sTag.ToStdString().c_str(), sElement.ToStdString().c_str() );
        return (BookRecord*) nullptr;   //error
    }
    sPage = m_pParser->GetText(pNode);

    //Create the book record object
    BookRecord *pBookr;
    if (m_bookFormat == k_format_0)
        pBookr = LENMUS_NEW BookRecord(sFileName, sPath, sTitle, sPage);
    else    // k_format_1
        pBookr = LENMUS_NEW BookRecord(sFileName, sPath + "content/", sTitle, sPage);

    // creates the book entry in the contents table
    int nContentStart = m_contents.size();          // save the contents index for later
    BookIndexItem *bookitem = LENMUS_NEW BookIndexItem;
    bookitem->level = 0;
    bookitem->id = "";
    bookitem->page = sPage;
    bookitem->title = sTitle;
    bookitem->titlenum = "";
    bookitem->image = "";
    bookitem->pBookRecord = pBookr;
    m_contents.Add(bookitem);

    //process other children nodes: <entry>
    pNode = m_pParser->GetNextSibling(pNode);
    pElement = pNode;
    sTag = "entry";
    while (pElement) {
        sElement = pElement->GetName();
        if (sElement != sTag)
        {
            LOMSE_LOG_ERROR("Loading eBook. Error: Expected tag <%s> but found <%s>",
                            sTag.ToStdString().c_str(), sElement.ToStdString().c_str() );
            delete pBookr;
            return (BookRecord*) nullptr;   //error
        }
        if (!ProcessTOCEntry(pElement, pBookr, 1)) return (BookRecord*) nullptr;   //error

        // get next entry
        pNode = m_pParser->GetNextSibling(pNode);
        pElement = pNode;
    }

    // store the contents range in the book record
    pBookr->SetContentsRange(nContentStart, m_contents.size());

    // Add the LENMUS_NEW book record to the table
    m_bookRecords.Add(pBookr);

    return pBookr;        // no error
}

//---------------------------------------------------------------------------------------
bool BooksCollection::ProcessTOCEntry(wxXmlNode* pNode, BookRecord *pBookr, int nLevel)
{
    // Parse one entry. Recursive for sub-entries
    // Add entry data to the m_contents array

        //process children nodes

    wxString sTitle = "",
             sPage = "",
             sName = "",
             sImage = "",
             sTitlenum = "",
             sId = "";

    // Get entry id
    sId = m_pParser->GetAttribute(pNode, "id");

    // process children
    pNode = m_pParser->GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    bool fTitleImage = false;        //to control that title or image exists
    wxString sElement;
    while (pElement)
    {
        sElement = pElement->GetName();
        if (sElement == "image") {
            sImage = m_pParser->GetText(pElement);
            fTitleImage = true;
        }
        else if (sElement == "title") {
            sTitle = m_pParser->GetText(pElement);
            fTitleImage = true;
            #ifdef _MBCS    //if Win95/98/Me release
                //change encoding from utf-8 to local encoding
                wxCSConv conv("utf-8");
                sTitle = wxString(sTitle.wc_str(conv), wxConvLocal);
            #endif
        }
        else if (sElement == "page") {
            sPage = m_pParser->GetText(pElement);
        }
        else if (sElement == "titlenum") {
            sTitlenum = m_pParser->GetText(pElement);
        }
        else {
            break;
        }
        pNode = m_pParser->GetNextSibling(pNode);
        pElement = pNode;
    }
    if (!fTitleImage)
    {
        LOMSE_LOG_ERROR(
            "Loading eBook. Error: Expected tag <title>/<Image> but none of them found.");
        return false;   //error
    }

    // create the entry in the contents table
    BookIndexItem *bookitem = LENMUS_NEW BookIndexItem;
    bookitem->level = nLevel;
    bookitem->id = sId;
    bookitem->page = sPage;
    bookitem->title = sTitle;
    bookitem->titlenum = sTitlenum;
    bookitem->image = sImage;
    bookitem->pBookRecord = pBookr;
    m_contents.Add(bookitem);

    //process sub-entries, if exist
    nLevel++;
    wxString sTag = "entry";
    while (pElement)
    {
        sElement = pElement->GetName();
        if (sElement != sTag)
        {
            LOMSE_LOG_ERROR("Loading eBook. Error: Expected tag <%s> but found <%s>",
                            sTag.ToStdString().c_str(), sElement.ToStdString().c_str() );
            return false;   //error
        }
        if (!ProcessTOCEntry(pElement, pBookr, nLevel)) return false;     //error

        // get next
        pNode = m_pParser->GetNextSibling(pNode);
        pElement = pNode;
    }

    return true;        // no error

}


//---------------------------------------------------------------------------------------
wxString BooksCollection::find_page_by_name(const wxString& x)
{
    // Find a page:
    // - By book filename: i.e. 'SingleExercises.lmb' (returns the cover page)
    // - By page filename: i.e. 'SingleExercises_ch0.lms'
    // - By page title: i.e. 'Exercises for aural training'
    // - By index enty:
    //
    // Returns the url to the page (the full path)
    //    i.e. 'c:\lenmus\books\en\SingleExercises.lmb#zip:SingleExercises_ch0.lms'

    int i;
    wxFileSystem fsys;
    wxFSFile* pFile;

    // 1. try to interpret x as a file name (i.e. 'SingleExercises.lmb')
    if (x.Right(4) == ".lmb") {
        // Try to open it
        int nNumBooks = m_bookRecords.GetCount();
        for (i = 0; i < nNumBooks; i++)
        {
            pFile = fsys.OpenFile(m_bookRecords[i]->GetFullPath(x));
            if (pFile) {
                wxString url = m_bookRecords[i]->GetFullPath(m_bookRecords[i]->GetCoverPage());
                delete pFile;
                return url;
            }
        }
    }

    // 2. Try to interpret x as the filename of a book page (i.e. 'SingleExercises_0.lms')
    if (x.Right(4) == ".lmd" || x.Right(4) == ".lms") {
        // Try to find the book page
        int nNumEntries = m_pagelist.size();
        for (i = 0; i < nNumEntries; i++)
        {
            //wxLogMessage("[BooksCollection::find_page_by_name] page %d, name = %s",
            //    i, (m_pagelist[i]->page).wx_str() );
            if (m_pagelist[i]->page == x)
                return m_pagelist[i]->GetFullPath();
        }
    }

    // 3. Try to interpret it as a title, and so try find in toc (i.e. 'Exercises for
    //    aural training'). This is the less secure method as titles can be repeated
    //    in different books. In these cases this will retutn the first one found
    int nNumEntries = m_contents.size();
    for (i = 0; i < nNumEntries; i++)
    {
        if (m_contents[i]->title == x)
            return m_contents[i]->GetFullPath();
    }

    // 4. try to find it in index
    nNumEntries = m_index.size();
    for (i = 0; i < nNumEntries; i++)
    {
        if (m_index[i]->title == x)
            return m_index[i]->GetFullPath();
    }

    //wxLogMessage("[BooksCollection::find_page_by_name] Page '%s' not found.", x.wx_str());
    return "";
}

//---------------------------------------------------------------------------------------
wxString BooksCollection::FindPageById(int WXUNUSED(id))
{
    //size_t cnt = m_contents.size();
    //for (size_t i = 0; i < cnt; i++)
    //{
    //    if (m_contents[i].id == id)
    //    {
    //        return m_contents[i].GetFullPath();
    //    }
    //}

    return "";
}

//---------------------------------------------------------------------------------------
wxString BooksCollection::get_path_for_toc_item(int item)
{
    BookIndexItem* bookitem = m_contents[item];
    if (bookitem && !bookitem->page.empty())
        return bookitem->GetFullPath();
    else
        return wxEmptyString;
}


////---------------------------------------------------------------------------------------
//// lmSearchStatus functions
////---------------------------------------------------------------------------------------
//
//lmSearchStatus::lmSearchStatus(BooksCollection* data, const wxString& keyword,
//                                       bool case_sensitive, bool whole_words_only,
//                                       const wxString& book)
//{
//    m_Data = data;
//    m_Keyword = keyword;
//    BookRecord* bookr = nullptr;
//    if (book != "")
//    {
//        // we have to search in a specific book. Find it first
//        int i, cnt = data->m_bookRecords.GetCount();
//        for (i = 0; i < cnt; i++)
//            if (data->m_bookRecords[i]->GetTitle() == book)
//            {
//                bookr = data->m_bookRecords[i];
//                m_CurIndex = bookr->GetContentsStart();
//                m_MaxIndex = bookr->GetContentsEnd();
//                break;
//            }
//        // check; we won't crash if the book doesn't exist, but it's Bad Anyway.
//        wxASSERT(bookr);
//    }
//    if (! bookr)
//    {
//        // no book specified; search all books
//        m_CurIndex = 0;
//        m_MaxIndex = m_Data->m_contents.size();
//    }
//    m_Engine.LookFor(keyword, case_sensitive, whole_words_only);
//    m_Active = (m_CurIndex < m_MaxIndex);
//}
//
////---------------------------------------------------------------------------------------
//bool lmSearchStatus::Search()
//{
//    //wxFSFile *file;
//    //int i = m_CurIndex;  // shortcut
//    //bool found = false;
//    //wxString thepage;
//
//    //if (!m_Active)
//    //{
//    //    // sanity check. Illegal use, but we'll try to prevent a crash anyway
//    //    wxASSERT(m_Active);
//    //    return false;
//    //}
//
//    //m_Name = "";
//    //m_CurItem = nullptr;
//    //thepage = m_Data->m_contents[i].page;
//
//    //m_Active = (++m_CurIndex < m_MaxIndex);
//    //// check if it is same page with different anchor:
//    //if (!m_LastPage.empty())
//    //{
//    //    const wxChar *p1, *p2;
//    //    for (p1 = thepage.wx_str(), p2 = m_LastPage.wx_str();
//    //         *p1 != 0 && *p1 != _T('#') && *p1 == *p2; p1++, p2++) {}
//
//    //    m_LastPage = thepage;
//
//    //    if (*p1 == 0 || *p1 == _T('#'))
//    //        return false;
//    //}
//    //else m_LastPage = thepage;
//
//    //wxFileSystem fsys;
//    //file = fsys.OpenFile(m_Data->m_contents[i].pBookRecord->GetFullPath(thepage));
//    //if (file)
//    //{
//    //    if (m_Engine.Scan(*file))
//    //    {
//    //        m_Name = m_Data->m_contents[i].title;
//    //        m_CurItem = &m_Data->m_contents[i];
//    //        found = true;
//    //    }
//    //    delete file;
//    //}
//    //return found;
//    return false;
//}
//
//
//
//
//
//
//
//
////---------------------------------------------------------------------------------------
//// BookSearchEngine
////---------------------------------------------------------------------------------------
//
//void BookSearchEngine::LookFor(const wxString& keyword, bool case_sensitive, bool whole_words_only)
//{
//    m_CaseSensitive = case_sensitive;
//    m_WholeWords = whole_words_only;
//    m_Keyword = keyword;
//
//    if (!m_CaseSensitive)
//        m_Keyword.MakeLower();
//}
//
//
//static inline bool WHITESPACE(wxChar c)
//{
//    return c == _T(' ') || c == _T('\n') || c == _T('\r') || c == _T('\t');
//}
//
////---------------------------------------------------------------------------------------
//bool BookSearchEngine::Scan(const wxFSFile& file)
//{
//    //wxASSERT_MSG(!m_Keyword.empty(), "BookSearchEngine::LookFor must be called before scanning!");
//
//    //int i, j;
//    //int wrd = m_Keyword.length();
//    //bool found = false;
//    //wxHtmlFilterHTML filter;
//    //wxString tmp = filter.ReadFile(file);
//    //int lng = tmp.length();
//    //const wxChar *buf = tmp.wx_str();
//
//    //if (!m_CaseSensitive)
//    //    tmp.MakeLower();
//
//    //const wxChar *kwd = m_Keyword.wx_str();
//
//    //if (m_WholeWords)
//    //{
//    //    for (i = 0; i < lng - wrd; i++)
//    //    {
//    //        if (WHITESPACE(buf[i])) continue;
//    //        j = 0;
//    //        while ((j < wrd) && (buf[i + j] == kwd[j])) j++;
//    //        if (j == wrd && WHITESPACE(buf[i + j])) { found = true; break; }
//    //    }
//    //}
//
//    //else
//    //{
//    //    for (i = 0; i < lng - wrd; i++)
//    //    {
//    //        j = 0;
//    //        while ((j < wrd) && (buf[i + j] == kwd[j])) j++;
//    //        if (j == wrd) { found = true; break; }
//    //    }
//    //}
//
//    //return found;
//    return false;
//}


}   //namespace lenmus
