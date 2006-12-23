//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
//
//    This file was initially is based on file helpdata.h from wxWidgets 2.6.3 project
//    although now it must be something totally different!!
//    wxWidgets licence is compatible with GNU GPL.
//    Author:      Harm van der Heijden and Vaclav Slavik
//    Copyright (c) Harm van der Heijden and Vaclav Slavik
// 
//    Modified by:
//        Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BookData.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../app/global.h"

#if lmUSE_LENMUS_EBOOK_FORMAT

#include "wx/defs.h"
#include "wx/zipstrm.h"

#include "BookData.h"

// access to paths. Needed only for ByPass code
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;


// BookData object stores and manages all book indexes.
// Html pages are not processed. When a page display is requested, the page is
// directtly loaded by the wxHtmlWindowd, LoadPage() method.




static int wxHtmlHelpIndexCompareFunc(lmBookIndexItem **a, lmBookIndexItem **b)
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
// lmBookRecord
//-----------------------------------------------------------------------------

lmBookRecord::lmBookRecord(const wxString& bookfile, const wxString& basepath,
                     const wxString& title, const wxString& start)
{
    m_sBookFile = bookfile;
    m_sBasePath = basepath;
    m_sTitle = title;
    m_sPageFile = start;
    // for debugging, give the contents index obvious default values
    m_ContentsStart = m_ContentsEnd = -1;
}

lmBookRecord::~lmBookRecord()
{
}

wxString lmBookRecord::GetFullPath(const wxString &page) const
{
    // returns full filename of page (which is part of the book),
    // i.e. with book's basePath prepended. If page is already absolute
    // path, basePath is _not_ prepended.

    if (wxIsAbsolutePath(page))
        return page;
    else
        return m_sBasePath + page;
}

//-----------------------------------------------------------------------------
// lmBookIndexItem
//-----------------------------------------------------------------------------

wxString lmBookIndexItem::GetIndentedName() const
{
    wxString s;
    for (int i = 1; i < level; i++)
        s << _T("   ");
    s << name;
    return s;
}


//-----------------------------------------------------------------------------
// lmBookData
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(lmBookData, wxObject)

lmBookData::lmBookData()
{
    m_pParser = new lmXmlParser();
}

lmBookData::~lmBookData()
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

bool lmBookData::AddBook(const wxFileName& oFilename)
{
    //Reads a book (either a .lmb or .toc file) and loads its content
    //Returns true if success.

    //wxString sFullName, sFileName, sPath;
    //if (oFilename.GetExt() == _T("lmb")) {
    //    //lenmus compressed book  (zip file)
    //    sFileName = oFilename.GetName();
    //    sPath = oFilename.GetFullPath() + _T("#zip:");
    //    sFullName = sPath + sFileName + _T(".toc");
    //}
    //else {
    //    sFullName = oFilename.GetFullPath();
    //    sFileName = oFilename.GetName();
    //    sPath = oFilename.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR , wxPATH_NATIVE);
    //}

    // Process the TOC file (.toc)
    lmBookRecord* pBookr = ProcessTOCFile(oFilename);
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

bool lmBookData::ProcessIndexFile(const wxFileName& oFilename, lmBookRecord* pBookr)
{
    // Returns true if success.


    wxLogMessage(_T("[lmBookData::ProcessIndexFile] Processing file %s"),
            oFilename.GetFullPath() );

    wxString sTitle = wxEmptyString,
             sDefaultPage = wxEmptyString,
             sContentsFile = wxEmptyString,
             sIndexFile = wxEmptyString,
             sCharset = wxEmptyString;

    // load the XML file as tree of nodes
    wxXmlDocument xdoc;
    if (!xdoc.Load(oFilename.GetFullPath()) ) {
        wxLogMessage(_T("Loading eBook. Error parsing index file %s"),
            oFilename.GetFullPath() );
        return false;   //error
    }

    //Verify type of document. Must be <lmBookIndex>
    wxXmlNode *pNode = xdoc.GetRoot();
    wxString sTag = _T("lmBookIndex");
    wxString sElement = pNode->GetName();
    if (sElement != sTag) {
        wxLogMessage(_T("Loading eBook. Error: First tag is not <%s> but <%s>"),
            sTag, sElement);
        return false;   //error
    }
    
    //process children nodes: <entry>
    pNode = m_pParser->GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    sElement = pElement->GetName();
    sTag = _T("entry");
    if (sElement != sTag) {
        wxLogMessage(_T("Loading eBook. Error: Expected tag <%s> but found <%s>"),
            sTag, sElement);
        return false;   //error
    }
    ProcessIndexEntries(pElement, pBookr);

    // Sort index table
    if (!m_index.empty()) {
        m_index.Sort(wxHtmlHelpIndexCompareFunc);
    }

    return true;
}


void lmBookData::ProcessIndexEntries(wxXmlNode* pNode, lmBookRecord *pBookr)
{
    // Parse the index entries and adds its data to the m_index array
    // pNode points to <entry> node

    //get first index entry
    wxXmlNode* pElement = pNode;
    wxString sTag = _T("entry");
    while (pElement) {
        if (sTag == pElement->GetName()) {
            lmBookIndexItem *pItem = new lmBookIndexItem();
            pItem->parent = NULL;
            pItem->level = 1;               //todo
            pItem->id = m_pParser->GetAttribute(pElement, _T("id"));
            pItem->page = m_pParser->GetAttribute(pElement, _T("page"));
            pItem->name = m_pParser->GetText(pElement);
            pItem->titlenum = wxEmptyString;
            pItem->image = wxEmptyString;
            pItem->pBookRecord = pBookr;
            m_index.Add(pItem);
        }

        // Find next entry
        pNode = m_pParser->GetNextSibling(pNode);
        pElement = pNode;
    }

}

lmBookRecord* lmBookData::ProcessTOCFile(const wxFileName& oFilename)
{
    // Returns ptr to created book record if success, NULL if failure

    wxLogMessage(_T("[lmBookData::ProcessTOCFile] Processing file %s"),
                 oFilename.GetFullPath());

    wxString sTitle = wxEmptyString,
             sPage = wxEmptyString,
             sContents = wxEmptyString,
             sIndex = wxEmptyString,
             sCharset = wxEmptyString;


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
            wxLogMessage(_T("Loading eBook. Error: TOC file '%s' not found."), oFilename.GetFullPath());
            return (lmBookRecord*) NULL;   //error
        }

        // call GetNextEntry() until the required internal name is found
        wxZipEntry* pEntry;
        do {
            pEntry = zip.GetNextEntry();
        }
        while (pEntry && pEntry->GetInternalName() != sInternalName);

        if (!pEntry) {
            wxLogMessage(_T("Loading eBook. Error: TOC file '%s' not found."), sFullName);
            return (lmBookRecord*) NULL;   //error
        }
        zip.OpenEntry(*pEntry);
        fLmbFile = true;
        fOK = xdoc.Load(zip);    //asumes utf-8
    }
    else {
        wxLogMessage(_T("Loading eBook. Error in TOC file '%s'. Extension is neither LMB nor TOC."), oFilename.GetFullPath());
        return (lmBookRecord*) NULL;   //error
    }

    // load the XML file as tree of nodes
    if (!fOK)   
    {
        wxLogMessage(_T("Loading eBook. Error parsing TOC file ") + sFullName);
        return (lmBookRecord*) NULL;   //error
    }

    //Verify type of document. Must be <lmBookTOC>
    wxXmlNode *pNode = xdoc.GetRoot();
    wxString sTag = _T("lmBookTOC");
    wxString sElement = pNode->GetName();
    if (sElement != sTag) {
        wxLogMessage(_T("Loading eBook. Error: First tag is not <%s> but <%s>"),
            sTag, sElement);
        return (lmBookRecord*) NULL;   //error
    }
    
    // firts node: title
    pNode = m_pParser->GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    sElement = pElement->GetName();
    sTag = _T("title");
    if (sElement != sTag) {
        wxLogMessage(_T("Loading eBook. Error: Expected tag <%s> but found <%s>"),
            sTag, sElement);
        return (lmBookRecord*) NULL;   //error
    }
    sTitle = m_pParser->GetText(pNode);

    // next node: coverpage
    pNode = m_pParser->GetNextSibling(pNode);
    pElement = pNode;
    sElement = pElement->GetName();
    sTag = _T("coverpage");
    if (sElement != sTag) {
        wxLogMessage(_T("Loading eBook. Error: Expected tag <%s> but found <%s>"),
            sTag, sElement);
        return (lmBookRecord*) NULL;   //error
    }
    sPage = m_pParser->GetText(pNode);

    //Create the book record object
    lmBookRecord *pBookr;
    pBookr = new lmBookRecord(sFileName, sPath, sTitle, sPage);

    // creates the book entry in the contents table
    int nContentStart = m_contents.size();          // save the contents index for later
    lmBookIndexItem *bookitem = new lmBookIndexItem;
    bookitem->level = 0;
    bookitem->id = wxEmptyString;
    bookitem->page = sPage;
    bookitem->name = sTitle;
    bookitem->titlenum = wxEmptyString;
    bookitem->image = wxEmptyString;
    bookitem->pBookRecord = pBookr;
    m_contents.Add(bookitem);

    //process other children nodes: <entry>
    pNode = m_pParser->GetNextSibling(pNode);
    pElement = pNode;
    sTag = _T("entry");
    while (pElement) {
        sElement = pElement->GetName();
        if (sElement != sTag) {
            wxLogMessage(_T("Loading eBook. Error: Expected tag <%s> but found <%s>"),
                sTag, sElement);
            delete pBookr;
            return (lmBookRecord*) NULL;   //error
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

bool lmBookData::ProcessTOCEntry(wxXmlNode* pNode, lmBookRecord *pBookr, int nLevel)
{
    // Parse one entry. Recursive for sub-entries
    // Add entry data to the m_contents array

        //process children nodes

    wxString sTitle = wxEmptyString,
             sPage = wxEmptyString,
             sName = wxEmptyString,
             sImage = wxEmptyString,
             sTitlenum = wxEmptyString,
             sId = wxEmptyString;

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
        wxLogMessage(_T("Loading eBook. Error: Expected tag <title>/<Image> but none of them found."));
        return false;   //error
    }

    // create the entry in the contents table
    lmBookIndexItem *bookitem = new lmBookIndexItem;
    bookitem->level = nLevel;
    bookitem->id = sId;
    bookitem->page = sPage;
    bookitem->name = sTitle;
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
            wxLogMessage(_T("Loading eBook. Error: Expected tag <%s> but found <%s>"),
                sTag, sElement);
            return false;   //error
        }
        if (!ProcessTOCEntry(pElement, pBookr, nLevel)) return false;     //error

        // get next
        pNode = m_pParser->GetNextSibling(pNode);
        pElement = pNode;
    }

    return true;        // no error

}


wxString lmBookData::FindPageByName(const wxString& x)
{
    int nNumBooks;
    int i;
    wxFileSystem fsys;
    wxFSFile *f;

    // 1. try to open given file:

    nNumBooks = m_bookRecords.GetCount();
    for (i = 0; i < nNumBooks; i++)
    {
        f = fsys.OpenFile(m_bookRecords[i]->GetFullPath(x));
        if (f)
        {
            wxString url = m_bookRecords[i]->GetFullPath(x);
            delete f;
            return url;
        }
    }


    ///* 2. try to find a book: */

    //for (i = 0; i < nNumBooks; i++)
    //{
    //    if (m_bookRecords[i].GetTitle() == x)
    //        return m_bookRecords[i].GetFullPath(m_bookRecords[i].GetStart());
    //}

    ///* 3. try to find in contents: */

    //nNumBooks = m_contents.size();
    //for (i = 0; i < nNumBooks; i++)
    //{
    //    if (m_contents[i].name == x)
    //        return m_contents[i].GetFullPath();
    //}


    ///* 4. try to find in index: */

    //nNumBooks = m_index.size();
    //for (i = 0; i < nNumBooks; i++)
    //{
    //    if (m_index[i].name == x)
    //        return m_index[i].GetFullPath();
    //}

    return wxEmptyString;
}

wxString lmBookData::FindPageById(int id)
{
    //size_t cnt = m_contents.size();
    //for (size_t i = 0; i < cnt; i++)
    //{
    //    if (m_contents[i].id == id)
    //    {
    //        return m_contents[i].GetFullPath();
    //    }
    //}

    return wxEmptyString;
}


//----------------------------------------------------------------------------------
// lmSearchStatus functions
//----------------------------------------------------------------------------------

lmSearchStatus::lmSearchStatus(lmBookData* data, const wxString& keyword,
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
            if (data->m_bookRecords[i]->GetTitle() == book)
            {
                bookr = data->m_bookRecords[i];
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

bool lmSearchStatus::Search()
{
    //wxFSFile *file;
    //int i = m_CurIndex;  // shortcut
    //bool found = false;
    //wxString thepage;

    //if (!m_Active)
    //{
    //    // sanity check. Illegal use, but we'll try to prevent a crash anyway
    //    wxASSERT(m_Active);
    //    return false;
    //}

    //m_Name = wxEmptyString;
    //m_CurItem = NULL;
    //thepage = m_Data->m_contents[i].page;

    //m_Active = (++m_CurIndex < m_MaxIndex);
    //// check if it is same page with different anchor:
    //if (!m_LastPage.empty())
    //{
    //    const wxChar *p1, *p2;
    //    for (p1 = thepage.c_str(), p2 = m_LastPage.c_str();
    //         *p1 != 0 && *p1 != _T('#') && *p1 == *p2; p1++, p2++) {}

    //    m_LastPage = thepage;

    //    if (*p1 == 0 || *p1 == _T('#'))
    //        return false;
    //}
    //else m_LastPage = thepage;

    //wxFileSystem fsys;
    //file = fsys.OpenFile(m_Data->m_contents[i].pBookRecord->GetFullPath(thepage));
    //if (file)
    //{
    //    if (m_Engine.Scan(*file))
    //    {
    //        m_Name = m_Data->m_contents[i].name;
    //        m_CurItem = &m_Data->m_contents[i];
    //        found = true;
    //    }
    //    delete file;
    //}
    //return found;
    return false;
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
    //wxASSERT_MSG(!m_Keyword.empty(), wxT("lmBookSearchEngine::LookFor must be called before scanning!"));

    //int i, j;
    //int wrd = m_Keyword.Length();
    //bool found = false;
    //wxHtmlFilterHTML filter;
    //wxString tmp = filter.ReadFile(file);
    //int lng = tmp.length();
    //const wxChar *buf = tmp.c_str();

    //if (!m_CaseSensitive)
    //    tmp.LowerCase();

    //const wxChar *kwd = m_Keyword.c_str();

    //if (m_WholeWords)
    //{
    //    for (i = 0; i < lng - wrd; i++)
    //    {
    //        if (WHITESPACE(buf[i])) continue;
    //        j = 0;
    //        while ((j < wrd) && (buf[i + j] == kwd[j])) j++;
    //        if (j == wrd && WHITESPACE(buf[i + j])) { found = true; break; }
    //    }
    //}

    //else
    //{
    //    for (i = 0; i < lng - wrd; i++)
    //    {
    //        j = 0;
    //        while ((j < wrd) && (buf[i + j] == kwd[j])) j++;
    //        if (j == wrd) { found = true; break; }
    //    }
    //}

    //return found;
    return false;
}


#endif      // lmUSE_LENMUS_EBOOK_FORMAT