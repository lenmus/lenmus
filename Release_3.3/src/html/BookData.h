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
#ifndef __LM_BOOKDATA_H__
#define __LM_BOOKDATA_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "helpdata.h"
#endif

#include "../app/global.h"

#if !lmUSE_LENMUS_EBOOK_FORMAT

#define lmBookData          wxHtmlHelpData
#define lmBookRecord        wxHtmlBookRecord
#define lmBookIndexArray    wxHtmlHelpDataItems
#define lmBookIndexItem     wxHtmlHelpDataItem
#define lmBookRecArray      wxHtmlBookRecArray

#else

#include "wx/defs.h"

#include "wx/object.h"
#include "wx/string.h"
#include "wx/filesys.h"
#include "wx/dynarray.h"
#include "wx/font.h"

#include "wx/filename.h"


#include "../xml_parser/XMLParser.h"       // to use XML parser

class lmBookData;

//--------------------------------------------------------------------------------
// helper classes & structs
//--------------------------------------------------------------------------------


// lmBookRecord: a record per book, to contain the info from the hhb file

class lmBookRecord
{
public:
    lmBookRecord(const wxString& bookfile, const wxString& basepath,
                     const wxString& title, const wxString& start);

    wxString GetBookFile() const { return m_sBookFile; }
    wxString GetTitle() const { return m_sTitle; }
    wxString GetStart() const { return m_sPageFile; }
    wxString GetBasePath() const { return m_sBasePath; }
    /* SetContentsRange: store in the bookrecord where in the index/contents lists the
     * book's records are stored. This to facilitate searching in a specific book.
     * This code will have to be revised when loading/removing books becomes dynamic.
     * (as opposed to appending only)
     * Note that storing index range is pointless, because the index is alphab. sorted. */
    void SetContentsRange(int start, int end) { m_ContentsStart = start; m_ContentsEnd = end; }
    int GetContentsStart() const { return m_ContentsStart; }
    int GetContentsEnd() const { return m_ContentsEnd; }

    void SetTitle(const wxString& title) { m_sTitle = title; }
    void SetBasePath(const wxString& path) { m_sBasePath = path; }
    void SetStart(const wxString& start) { m_sPageFile = start; }

    // returns full filename of page (which is part of the book),
    // i.e. with book's basePath prepended. If page is already absolute
    // path, basePath is _not_ prepended.
    wxString GetFullPath(const wxString &page) const;

protected:
    wxString    m_sBookFile;
    wxString    m_sBasePath;
    wxString    m_sTitle;
    wxString    m_sPageFile;
    int         m_ContentsStart;    //index to content table for the first entry of this book
    int         m_ContentsEnd;      //index to content table for the last entry of this book
};


WX_DECLARE_USER_EXPORTED_OBJARRAY(lmBookRecord, lmBookRecArray,
                                  WXDLLIMPEXP_HTML);
//WX_DEFINE_ARRAY(lmLDPNode*, ArrayNodePtrs);


// lmBookIndexItem: an entry of the index and contents tables
// The only difference between content entries and glossary entries is that the glossary
// don't have image.
struct lmBookIndexItem
{
    lmBookIndexItem() : level(0), parent(NULL), id(wxEmptyString), pBookRecord(NULL) {}

    int                 level;          // level of this entry. 0: book, 1-n: pages
    lmBookIndexItem*    parent;         // parent entry if this is a sub-entry
    wxString            id;             // prefix for title (number/letter)
    wxString            name;           // text for this entry
    wxString            page;           // html page to display
    lmBookRecord*       pBookRecord;    // ptr to book record

    // returns full filename of page, i.e. with book's basePath prepended
    wxString GetFullPath() const { return pBookRecord->GetFullPath(page); }

    // returns item indented with spaces if it has level > 1
    wxString GetIndentedName() const;
};

WX_DECLARE_USER_EXPORTED_OBJARRAY(lmBookIndexItem, lmBookIndexArray,
                                  WXDLLIMPEXP_HTML);



//------------------------------------------------------------------------------
// lmBookSearchEngine
//                  This class takes input streams and scans them for occurence
//                  of keyword(s)
//------------------------------------------------------------------------------

class lmBookSearchEngine : public wxObject
{
public:
    lmBookSearchEngine() : wxObject() {}
    ~lmBookSearchEngine() {}

    // Sets the keyword we will be searching for
    virtual void LookFor(const wxString& keyword, bool case_sensitive, bool whole_words_only);

    // Scans the stream for the keyword.
    // Returns true if the stream contains keyword, fALSE otherwise
    virtual bool Scan(const wxFSFile& file);

private:
    wxString m_Keyword;
    bool m_CaseSensitive;
    bool m_WholeWords;

    DECLARE_NO_COPY_CLASS(lmBookSearchEngine)
};


// State information of a search action. I'd have preferred to make this a
// nested class inside lmBookData, but that's against coding standards :-(
// Never construct this class yourself, obtain a copy from
// lmBookData::PrepareKeywordSearch(const wxString& key)
class lmSearchStatus
{
public:
    // constructor; supply lmBookData ptr, the keyword and (optionally) the
    // title of the book to search. By default, all books are searched.
    lmSearchStatus(lmBookData* base, const wxString& keyword,
                       bool case_sensitive, bool whole_words_only,
                       const wxString& book = wxEmptyString);
    bool Search();  // do the next iteration
    bool IsActive() { return m_Active; }
    int GetCurIndex() { return m_CurIndex; }
    int GetMaxIndex() { return m_MaxIndex; }
    const wxString& GetName() { return m_Name; }

    const lmBookIndexItem *GetCurItem() const { return m_CurItem; }

private:
    lmBookData* m_Data;
    lmBookSearchEngine m_Engine;
    wxString m_Keyword, m_Name;
    wxString m_LastPage;
    lmBookIndexItem* m_CurItem;
    bool m_Active;   // search is not finished
    int m_CurIndex;  // where we are now
    int m_MaxIndex;  // number of files we search
    // For progress bar: 100*curindex/maxindex = % complete

    DECLARE_NO_COPY_CLASS(lmSearchStatus)
};


// Class lmBookData contains the information about all loaded books
class lmBookData : public wxObject
{
    DECLARE_DYNAMIC_CLASS(lmBookData)
    friend class lmSearchStatus;

public:
    lmBookData();
    ~lmBookData();

    void SetTempDir(const wxString& path);

    // Adds new book. 
    bool AddBook(const wxFileName& book);

    // Page search methods
    wxString FindPageByName(const wxString& page);
    wxString FindPageById(int id);

    // accessors to the tables
    const lmBookRecArray& GetBookRecArray() const { return m_bookRecords; }
    const lmBookIndexArray& GetContentsArray() const { return m_contents; }
    const lmBookIndexArray& GetIndexArray() const { return m_glossary; }


private:
    bool ProcessIndexFile(const wxFileName& oFilename, lmBookRecord* pBookr);
    void ProcessGlossaryEntries(wxXmlNode* pNode, lmBookRecord *pBookr);
    lmBookRecord* ProcessTOCFile(const wxFileName& oFilename);
    bool ProcessTOCEntry(wxXmlNode* pNode, lmBookRecord *pBookr, int nLevel);

    wxString            m_tempPath;
    lmXmlParser*        m_pParser;
    lmBookRecArray      m_bookRecords;  // each book has one record in this array
    lmBookIndexArray    m_contents;     // list of all available books and their TOCs
    lmBookIndexArray    m_glossary;     // list of all glossary items

    DECLARE_NO_COPY_CLASS(lmBookData)
};

#endif      // lmUSE_LENMUS_EBOOK_FORMAT

#endif      // __LM_BOOKDATA_H__
