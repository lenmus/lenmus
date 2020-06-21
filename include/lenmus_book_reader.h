//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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

#ifndef __LENMUS_BOOK_READER_H__
#define __LENMUS_BOOK_READER_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_xml_parser.h"

//wxWidgets
#include <wx/defs.h>
#include <wx/object.h>
#include <wx/string.h>
#include <wx/filesys.h>
#include <wx/dynarray.h>
#include <wx/font.h>
#include <wx/filename.h>
#include <wx/zipstrm.h>

//other
#include <memory>
#include <iostream>
#include <sstream>
#include <map>
using namespace std;

namespace lenmus
{

//forward declarations
class BooksCollection;


//---------------------------------------------------------------------------------------
// BookRecord: a record per book, to contain the info from the lmb file
class BookRecord
{
protected:
    wxString    m_sBookFile;
    wxString    m_sBasePath;
    wxString    m_sTitle;
    wxString    m_sCoverPage;
    int         m_ContentsStart;    //index to content table for the first entry of this book
    int         m_ContentsEnd;      //index to content table for the last entry of this book

public:
    BookRecord(const wxString& bookfile, const wxString& basepath,
               const wxString& title, const wxString& start);
    ~BookRecord();

    inline wxString GetBookFile() const { return m_sBookFile; }

    inline wxString GetTitle() const { return m_sTitle; }
    inline void SetTitle(const wxString& title) { m_sTitle = title; }

    inline wxString GetCoverPage() const { return m_sCoverPage; }

    wxString GetBasePath() const { return m_sBasePath; }
    inline void SetBasePath(const wxString& path) { m_sBasePath = path; }
    wxString GetFullPath(const wxString &page) const;

    // Contents related methods
    inline void SetContentsRange(int start, int end) {
        m_ContentsStart = start;
        m_ContentsEnd = end;
    }
    inline int GetContentsStart() const { return m_ContentsStart; }
    inline int GetContentsEnd() const { return m_ContentsEnd; }

};

//---------------------------------------------------------------------------------------
// BookIndexItem: an entry of the index and contents tables
// The only difference between content entries and index entries is that the index
// entries don't have an image.
struct BookIndexItem
{
    BookIndexItem() : level(0), parent(nullptr), id(wxEmptyString),
                        titlenum(wxEmptyString), pBookRecord(nullptr) {}

    int                 level;          // level of this entry. 0: book, 1-n: pages
    BookIndexItem*    parent;         // parent entry if this is a sub-entry (level > 0)
    wxString            id;             // page id (to search pages by id)
    wxString            titlenum;       // prefix for title (number/letter)
    wxString            title;          // text for this entry
    wxString            page;           // html page to display
    wxString            image;          // image to display
    BookRecord*       pBookRecord;    // ptr to book record

    // returns full filename of page, i.e. with book's basePath prepended
    inline wxString GetFullPath() const { return pBookRecord->GetFullPath(page); }

    // returns item indented with spaces if it has level > 1
    wxString GetIndentedName() const;
};

//---------------------------------------------------------------------------------------
// PageIndexItem: an entry of the page index table
// The page index is a global table with all the available lmd pages. It is used to
// search for a page. This table is needed as all other tables only contains
// information about the pages that are in a book's TOC
struct PageIndexItem
{
    PageIndexItem() : page(wxEmptyString), book(wxEmptyString) {}

    wxString    page;       // lms/lmd page to display
    wxString    book;       // book path

    // returns full filename of page, i.e. with book's basePath prepended
    wxString GetFullPath() const {
        if (page.Contains(".lmd"))
            return book + "#zip:content/" + page;
        else
            return book + "#zip:" + page;
    }

};


#include <wx/dynarray.h>
WX_DEFINE_ARRAY(BookRecord*, BookRecArray);
WX_DEFINE_ARRAY(BookIndexItem*, BookIndexArray);
WX_DEFINE_ARRAY(PageIndexItem*, lmPageIndexArray);


////---------------------------------------------------------------------------------------
//// BookSearchEngine
////                  This class takes input streams and scans them for occurrence
////                  of keyword(s)
//////---------------------------------------------------------------------------------------
//
//class BookSearchEngine
//{
//public:
//    BookSearchEngine() {}
//    virtual ~BookSearchEngine() {}
//
//    // Sets the keyword we will be searching for
//    virtual void LookFor(const wxString& keyword, bool case_sensitive, bool whole_words_only);
//
//    // Scans the stream for the keyword.
//    // Returns true if the stream contains keyword, fALSE otherwise
//    virtual bool Scan(const wxFSFile& file);
//
//private:
//    wxString m_Keyword;
//    bool m_CaseSensitive;
//    bool m_WholeWords;
//
//    wxDECLARE_NO_COPY_CLASS(BookSearchEngine);
//};
//
//
//// State information of a search action. I'd have preferred to make this a
//// nested class inside BooksCollection, but that's against coding standards :-(
//// Never construct this class yourself, obtain a copy from
//// BooksCollection::PrepareKeywordSearch(const wxString& key)
//class lmSearchStatus
//{
//public:
//    // constructor; supply BooksCollection ptr, the keyword and (optionally) the
//    // title of the book to search. By default, all books are searched.
//    lmSearchStatus(BooksCollection* base, const wxString& keyword,
//                       bool case_sensitive, bool whole_words_only,
//                       const wxString& book = wxEmptyString);
//    bool Search();  // do the next iteration
//    bool IsActive() { return m_Active; }
//    int GetCurIndex() { return m_CurIndex; }
//    int GetMaxIndex() { return m_MaxIndex; }
//    const wxString& GetName() { return m_Name; }
//
//    const BookIndexItem *GetCurItem() const { return m_CurItem; }
//
//private:
//    BooksCollection* m_Data;
//    BookSearchEngine m_Engine;
//    wxString m_Keyword, m_Name;
//    wxString m_LastPage;
//    BookIndexItem* m_CurItem;
//    bool m_Active;   // search is not finished
//    int m_CurIndex;  // where we are now
//    int m_MaxIndex;  // number of files we search
//    // For progress bar: 100*curindex/maxindex = % complete
//
//    wxDECLARE_NO_COPY_CLASS(lmSearchStatus);
//};


//---------------------------------------------------------------------------------------
// Class BooksCollection contains the information about all loaded books
class BooksCollection
{
private:
    wxString            m_tempPath;
    XmlParser*          m_pParser;
    BookRecArray        m_bookRecords;  // each book has one record in this array
    BookIndexArray      m_contents;     // list of all available books and their TOCs
    BookIndexArray      m_index;        // list of all index items
    lmPageIndexArray    m_pagelist;     // list of all pages (whether in TOC or not in TOC)
    map<wxString, wxZipEntry*> m_bookEntries;

    wxDECLARE_NO_COPY_CLASS(BooksCollection);

public:
    BooksCollection();
    ~BooksCollection();

    void SetTempDir(const wxString& path);

    // Adds new book.
    BookRecord* add_book(const wxFileName& book);

    // Page search methods
    wxString find_page_by_name(const wxString& page);
    wxString FindPageById(int id);

    // accessors to the tables
    inline const BookRecArray& GetBookRecArray() const { return m_bookRecords; }
    inline const BookIndexArray& GetContentsArray() const { return m_contents; }
    inline const BookIndexArray& GetIndexArray() const { return m_index; }
    wxString get_path_for_toc_item(int item);


private:
    bool add_pages_to_list(const wxFileName& oFilename);
    BookRecord* add_book_toc(const wxFileName& oFilename);
    bool ProcessTOCEntry(wxXmlNode* pNode, BookRecord *pBookr, int nLevel);

    void determine_book_format(wxZipInputStream& zip);
    bool add_lms_pages(wxZipInputStream& zip, const wxString& sBookPath);
    bool add_lmd_pages(wxZipInputStream& zip, const wxString& sBookPath);

    void load_book_entries(wxZipInputStream& zip);
    wxZipEntry* find_entry(const wxString& name);
    void delete_book_entries();

    //eBook formats
    int m_bookFormat;
    enum {
        k_invalid=0,
        k_format_0,     //old style, LMS files
        k_format_1,     //new style, LMD files
    };

};

//---------------------------------------------------------------------------------------
// Class BookReader: Understands book format and knows how to get its content.
class BookReader
{
protected:

public:
    BookReader() {}
    ~BookReader() {}

};



}   //namespace lenmus

#endif      // __LENMUS_BOOK_READER_H__
