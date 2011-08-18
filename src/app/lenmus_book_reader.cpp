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

#include "lenmus_book_reader.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/defs.h>
#include <wx/log.h>
#include <wx/zipstrm.h>


namespace lenmus
{


//---------------------------------------------------------------------------------------
static int wxHtmlHelpIndexCompareFunc(BookIndexItem **a, BookIndexItem **b)
{
    BookIndexItem *ia = *a;
    BookIndexItem *ib = *b;

    if (ia == NULL)
        return -1;
    if (ib == NULL)
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

    if (wxIsAbsolutePath(page))
        return page;
    else
        return m_sBasePath + page;
}

//---------------------------------------------------------------------------------------
// BookIndexItem
//---------------------------------------------------------------------------------------

wxString BookIndexItem::GetIndentedName() const
{
    wxString s;
    for (int i = 1; i < level; i++)
        s << _T("   ");
    s << title;
    return s;
}


//---------------------------------------------------------------------------------------
// BookReader
// BookReader object stores and manages all book indexes.
// Html pages are not processed. When a page display is requested, the page is
// directtly loaded by the wxHtmlWindowd, LoadPage() method.
//---------------------------------------------------------------------------------------
BookReader::BookReader()
{
    m_pParser = new XmlParser();
}

//---------------------------------------------------------------------------------------
BookReader::~BookReader()
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
void BookReader::SetTempDir(const wxString& path)
{
    if (path.empty())
        m_tempPath = path;
    else
    {
        if (wxIsAbsolutePath(path)) m_tempPath = path;
        else m_tempPath = wxGetCwd() + _T("/") + path;

        if (m_tempPath[m_tempPath.length() - 1] != _T('/'))
            m_tempPath << _T('/');
    }
}

//---------------------------------------------------------------------------------------
bool BookReader::AddBook(const wxFileName& oFilename)
{
    //Reads a book (either a .lmb or .toc file) and loads its content
    //Returns true if success.

   if (oFilename.GetExt() == _T("lmb")) {
        //add html page names to the pagelist table
        AddBookPagesToList(oFilename);
    }

    // Process the TOC file (.toc)
    BookRecord* pBookr = ProcessTOCFile(oFilename);
    if (!pBookr) {
        return false;       //error
    }

    // process an optional index file
    wxFileName* pFN = new wxFileName(oFilename);
    pFN->SetExt(_T("idx"));
    bool fSuccess = true;
    if (pFN->FileExists())
        fSuccess = ProcessIndexFile(*pFN, pBookr);

    delete pFN;
    return fSuccess;
}

//---------------------------------------------------------------------------------------
bool BookReader::AddBookPagesToList(const wxFileName& oFilename)
{
    // Returns true if error.

    // open the zip file
    wxString sBookPath = oFilename.GetFullPath();
    wxFFileInputStream in(sBookPath);
    wxZipInputStream zip(in);
    if (!zip.IsOk()) {
        wxLogMessage(_T("[BookReader::AddBookPagesToList] Loading eBook. Error: can not open file '%s'."),
            sBookPath.c_str());
        return true;   //error
    }

    // loop to get all files
    wxZipEntry* pEntry = zip.GetNextEntry();
    while (pEntry)
    {
        //get its name
        wxString sPageName = pEntry->GetName();
        if (sPageName.Find(_T(".htm")) != wxNOT_FOUND) {
            //add entry to pagelist
            //wxLogMessage(_T("[BookReader::AddBookPagesToList] Adding page '%s'"), sPageName);
            lmPageIndexItem *pItem = new lmPageIndexItem();
            pItem->page = sPageName;
            pItem->book = sBookPath;
            m_pagelist.Add(pItem);
        }
        delete pEntry;      //we have ownership of entry object
        pEntry = zip.GetNextEntry();
    }

    return false;   //no error

}

//---------------------------------------------------------------------------------------
bool BookReader::ProcessIndexFile(const wxFileName& oFilename, BookRecord* pBookr)
{
    // Returns true if success.


    wxLogMessage(_T("[BookReader::ProcessIndexFile] Processing file %s"),
            oFilename.GetFullPath().c_str() );

    wxString sTitle = _T(""),
             sDefaultPage = _T(""),
             sContentsFile = _T(""),
             sIndexFile = _T("");

    // load the XML file as tree of nodes
    wxXmlDocument xdoc;
    if (!xdoc.Load(oFilename.GetFullPath()) ) {
        wxLogMessage(_T("[BookReader::ProcessIndexFile] Loading eBook. Error parsing index file %s"),
            oFilename.GetFullPath().c_str() );
        return false;   //error
    }

    //Verify type of document. Must be <BookIndex>
    wxXmlNode *pNode = xdoc.GetRoot();
    wxString sTag = _T("BookIndex");
    wxString sElement = pNode->GetName();
    if (sElement != sTag) {
        wxLogMessage(_T("[BookReader::ProcessIndexFile] Loading eBook. Error: First tag is not <%s> but <%s>"),
            sTag.c_str(), sElement.c_str());
        return false;   //error
    }

    //process children nodes: <entry>
    pNode = m_pParser->GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    sElement = pElement->GetName();
    sTag = _T("entry");
    if (sElement != sTag) {
        wxLogMessage(_T("[BookReader::ProcessIndexFile] Loading eBook. Error: Expected tag <%s> but found <%s>"),
            sTag.c_str(), sElement.c_str());
        return false;   //error
    }
    ProcessIndexEntries(pElement, pBookr);

    // Sort index table
    if (!m_index.empty()) {
        m_index.Sort(wxHtmlHelpIndexCompareFunc);
    }

    return true;
}


//---------------------------------------------------------------------------------------
void BookReader::ProcessIndexEntries(wxXmlNode* pNode, BookRecord *pBookr)
{
    // Parse the index entries and adds its data to the m_index array
    // pNode points to <entry> node

    //get first index entry
    wxXmlNode* pElement = pNode;
    wxString sTag = _T("entry");
    while (pElement) {
        if (sTag == pElement->GetName()) {
            BookIndexItem *pItem = new BookIndexItem();
            pItem->parent = NULL;
            pItem->level = 1;               //todo
            pItem->id = m_pParser->GetAttribute(pElement, _T("id"));
            pItem->page = m_pParser->GetAttribute(pElement, _T("page"));
            pItem->title = m_pParser->GetText(pElement);
            pItem->titlenum = _T("");
            pItem->image = _T("");
            pItem->pBookRecord = pBookr;
            m_index.Add(pItem);
        }

        // Find next entry
        pNode = m_pParser->GetNextSibling(pNode);
        pElement = pNode;
    }

}

//---------------------------------------------------------------------------------------
BookRecord* BookReader::ProcessTOCFile(const wxFileName& oFilename)
{
    // Returns ptr to created book record if success, NULL if failure

//    wxLogMessage(_T("[BookReader::ProcessTOCFile] Processing file %s"),
//                 oFilename.GetFullPath().c_str());

    wxString sTitle = _T(""),
             sPage = _T(""),
             sContents = _T(""),
             sIndex = _T("");


    // wxXmlDocument::Load(filename) uses a wxTextStreamFile and it doesn't support
    // virtual files. So, when using LMB files we have to allocate
    // a wxZipTextStream and pass it to wxXmlDocument::Load(stream)
    wxXmlDocument xdoc;
    bool fOK;
    wxString sFullName, sFileName, sPath, sNameExt;
    bool fLmbFile = false;
    if (oFilename.GetExt() == _T("toc"))
    {
        sFullName = oFilename.GetFullPath();
        sFileName = oFilename.GetName();
        sPath = oFilename.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR , wxPATH_NATIVE);
        fOK = xdoc.Load(sFullName);
    }
    else if (oFilename.GetExt() == _T("lmb"))
    {
        //lenmus compressed book (zip file)
        sFileName = oFilename.GetName();
        sPath = oFilename.GetFullPath() + _T("#zip:");
        sNameExt = sFileName + _T(".toc");
        sFullName = sPath + sNameExt;

        // convert the local name we are looking for into the zip internal format
        wxString sInternalName = wxZipEntry::GetInternalName( sNameExt );

        // open the zip
        wxFFileInputStream in( oFilename.GetFullPath() );
        wxZipInputStream zip(in);
        if (!zip.IsOk()) {
            wxLogMessage(_T("[BookReader::ProcessTOCFile] Loading eBook. Error: TOC file '%s' not found."),
                oFilename.GetFullPath().c_str());
            return (BookRecord*) NULL;   //error
        }

        // call GetNextEntry() until the required internal name is found
        wxZipEntry* pEntry = (wxZipEntry*)NULL;
        do {
            if (pEntry) delete pEntry;      //delete previous entry
            pEntry = zip.GetNextEntry();    //now we have ownership of object *pEntry
        }
        while (pEntry && pEntry->GetInternalName() != sInternalName);

        if (!pEntry) {
            wxLogMessage(_T("[BookReader::ProcessTOCFile] Loading eBook. Error: TOC file '%s' not found."),
                sFullName.c_str());
            return (BookRecord*) NULL;   //error
        }
        zip.OpenEntry(*pEntry);
        fLmbFile = true;
        fOK = xdoc.Load(zip);    //asumes utf-8
        zip.CloseEntry();
        delete pEntry;
    }
    else {
        wxLogMessage(_T("[BookReader::ProcessTOCFile] Loading eBook. Error in TOC file '%s'. Extension is neither LMB nor TOC."),
            oFilename.GetFullPath().c_str());
        return (BookRecord*) NULL;   //error
    }

    // load the XML file as tree of nodes
    if (!fOK)
    {
        wxLogMessage(_T("[BookReader::ProcessTOCFile] Loading eBook. Error parsing TOC file ") + sFullName);
        return (BookRecord*) NULL;   //error
    }

    //Verify type of document. Must be <BookTOC>
    wxXmlNode *pNode = xdoc.GetRoot();
    wxString sTag = _T("lmBookTOC");
    wxString sElement = pNode->GetName();
    if (sElement != sTag) {
        wxLogMessage(_T("[BookReader::ProcessTOCFile] Loading eBook. Error: First tag is not <%s> but <%s>"),
            sTag.c_str(), sElement.c_str());
        return (BookRecord*) NULL;   //error
    }

    // firts node: title
    pNode = m_pParser->GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    sElement = pElement->GetName();
    sTag = _T("title");
    if (sElement != sTag) {
        wxLogMessage(_T("[BookReader::ProcessTOCFile] Loading eBook. Error: Expected tag <%s> but found <%s>"),
            sTag.c_str(), sElement.c_str());
        return (BookRecord*) NULL;   //error
    }
    sTitle = m_pParser->GetText(pNode);

    // next node: coverpage
    pNode = m_pParser->GetNextSibling(pNode);
    pElement = pNode;
    sElement = pElement->GetName();
    sTag = _T("coverpage");
    if (sElement != sTag) {
        wxLogMessage(_T("[BookReader::ProcessTOCFile] Loading eBook. Error: Expected tag <%s> but found <%s>"),
            sTag.c_str(), sElement.c_str());
        return (BookRecord*) NULL;   //error
    }
    sPage = m_pParser->GetText(pNode);

    //Create the book record object
    BookRecord *pBookr;
    pBookr = new BookRecord(sFileName, sPath, sTitle, sPage);

    // creates the book entry in the contents table
    int nContentStart = m_contents.size();          // save the contents index for later
    BookIndexItem *bookitem = new BookIndexItem;
    bookitem->level = 0;
    bookitem->id = _T("");
    bookitem->page = sPage;
    bookitem->title = sTitle;
    bookitem->titlenum = _T("");
    bookitem->image = _T("");
    bookitem->pBookRecord = pBookr;
    m_contents.Add(bookitem);

    //process other children nodes: <entry>
    pNode = m_pParser->GetNextSibling(pNode);
    pElement = pNode;
    sTag = _T("entry");
    while (pElement) {
        sElement = pElement->GetName();
        if (sElement != sTag) {
            wxLogMessage(_T("[BookReader::ProcessTOCFile] Loading eBook. Error: Expected tag <%s> but found <%s>"),
                sTag.c_str(), sElement.c_str());
            delete pBookr;
            return (BookRecord*) NULL;   //error
        }
        if (!ProcessTOCEntry(pElement, pBookr, 1)) return false;     //error

        // get next entry
        pNode = m_pParser->GetNextSibling(pNode);
        pElement = pNode;
    }

    // store the contents range in the book record
    pBookr->SetContentsRange(nContentStart, m_contents.size());

    // Add the new book record to the table
    m_bookRecords.Add(pBookr);

    return pBookr;        // no error

}

//---------------------------------------------------------------------------------------
bool BookReader::ProcessTOCEntry(wxXmlNode* pNode, BookRecord *pBookr, int nLevel)
{
    // Parse one entry. Recursive for sub-entries
    // Add entry data to the m_contents array

        //process children nodes

    wxString sTitle = _T(""),
             sPage = _T(""),
             sName = _T(""),
             sImage = _T(""),
             sTitlenum = _T(""),
             sId = _T("");

    // Get entry id
    sId = m_pParser->GetAttribute(pNode, _T("id"));

    // process children
    pNode = m_pParser->GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    bool fTitleImage = false;        //to control that title or image exists
    wxString sElement;
    while (pElement)
    {
        sElement = pElement->GetName();
        if (sElement == _T("image")) {
            sImage = m_pParser->GetText(pElement);
            fTitleImage = true;
        }
        else if (sElement == _T("title")) {
            sTitle = m_pParser->GetText(pElement);
            fTitleImage = true;
            #ifdef _MBCS    //if Win95/98/Me release
                //change encoding from utf-8 to local encoding
                wxCSConv conv(_T("utf-8"));
                sTitle = wxString(sTitle.wc_str(conv), convLocal);     //wxConvLocal);
            #endif
        }
        else if (sElement == _T("page")) {
            sPage = m_pParser->GetText(pElement);
        }
        else if (sElement == _T("titlenum")) {
            sTitlenum = m_pParser->GetText(pElement);
        }
        else {
            break;
        }
        pNode = m_pParser->GetNextSibling(pNode);
        pElement = pNode;
    }
    if (!fTitleImage) {
        wxLogMessage(_T("[BookReader::ProcessTOCEntry] Loading eBook. Error: Expected tag <title>/<Image> but none of them found."));
        return false;   //error
    }

    // create the entry in the contents table
    BookIndexItem *bookitem = new BookIndexItem;
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
    wxString sTag = _T("entry");
    while (pElement)
    {
        sElement = pElement->GetName();
        if (sElement != sTag) {
            wxLogMessage(_T("[BookReader::ProcessTOCEntry] Loading eBook. Error: Expected tag <%s> but found <%s>"),
                sTag.c_str(), sElement.c_str());
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
wxString BookReader::FindPageByName(const wxString& x)
{
    // Find a page:
    // - By book filename: i.e. 'SingleExercises.lmb' (returns the cover page)
    // - By page filename: i.e. 'SingleExercises_ch0.htm'
    // - By page title: i.e. 'Exercises for aural training'
    // - By index enty:
    //
    // Returns the url to the page (the full path)
    //    i.e. 'c:\lenmus\books\en\SingleExercises.lmb#zip:SingleExercises_ch0.htm'

    int i;
    wxFileSystem fsys;
    wxFSFile* pFile;

    // 1. try to interpret x as a file name (i.e. 'SingleExercises.lmb')
    if (x.Right(4) == _T(".lmb")) {
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

    // 2. Try to interpret x as the filename of a book page (i.e. 'SingleExercises_0.htm')
    if (x.Right(4) == _T(".htm")) {
        // Try to find the book page
        int nNumEntries = m_pagelist.size();
        for (i = 0; i < nNumEntries; i++)
        {
            //wxLogMessage(_T("[BookReader::FindPageByName] page %d, name = %s"),
            //    i, (m_pagelist[i]->page).c_str() );
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

    wxLogMessage(_T("[BookReader::FindPageByName] Page '%s' not found."), x.c_str());
    return _T("");
}

//---------------------------------------------------------------------------------------
wxString BookReader::FindPageById(int id)
{
    //size_t cnt = m_contents.size();
    //for (size_t i = 0; i < cnt; i++)
    //{
    //    if (m_contents[i].id == id)
    //    {
    //        return m_contents[i].GetFullPath();
    //    }
    //}

    return _T("");
}


////---------------------------------------------------------------------------------------
//// lmSearchStatus functions
////---------------------------------------------------------------------------------------
//
//lmSearchStatus::lmSearchStatus(BookReader* data, const wxString& keyword,
//                                       bool case_sensitive, bool whole_words_only,
//                                       const wxString& book)
//{
//    m_Data = data;
//    m_Keyword = keyword;
//    BookRecord* bookr = NULL;
//    if (book != _T(""))
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
//    //m_Name = _T("");
//    //m_CurItem = NULL;
//    //thepage = m_Data->m_contents[i].page;
//
//    //m_Active = (++m_CurIndex < m_MaxIndex);
//    //// check if it is same page with different anchor:
//    //if (!m_LastPage.empty())
//    //{
//    //    const wxChar *p1, *p2;
//    //    for (p1 = thepage.c_str(), p2 = m_LastPage.c_str();
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
//    //wxASSERT_MSG(!m_Keyword.empty(), wxT("BookSearchEngine::LookFor must be called before scanning!"));
//
//    //int i, j;
//    //int wrd = m_Keyword.length();
//    //bool found = false;
//    //wxHtmlFilterHTML filter;
//    //wxString tmp = filter.ReadFile(file);
//    //int lng = tmp.length();
//    //const wxChar *buf = tmp.c_str();
//
//    //if (!m_CaseSensitive)
//    //    tmp.MakeLower();
//
//    //const wxChar *kwd = m_Keyword.c_str();
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
