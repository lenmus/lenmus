//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file VirtualBooks.cpp
    @brief Implementation file for virtual books object
    @ingroup html_management
*/
#ifdef __GNUG__
// #pragma implementation
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "VirtualBooks.h"
#include "..\app\TheApp.h"             //access to get version

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;



static bool  m_fBooksLoaded = false;

//introduction/welcome book
static wxString m_sIntroHHP;        //book description
static wxString m_sIntroHHC;        //book conten
static wxString m_sIntroHHK;        //book index
static wxString m_sIntroHTM;        //book pages

//single exercises book
static wxString m_sSingleHHP;        //book description
static wxString m_sSingleHHC;        //book conten
static wxString m_sSingleHHK;        //book index
static wxString m_sSingleHTM;        //book pages


lmVirtualBooks::lmVirtualBooks() : wxFileSystemHandler()
{
    //initialize book content. It can not be static because then _() macro doesn't
    //work
    if (!m_fBooksLoaded) {
        LoadIntroBook();
        m_fBooksLoaded = true;
    }
}

lmVirtualBooks::~lmVirtualBooks()
{
}

bool lmVirtualBooks::CanOpen(const wxString& location)
{
    return (GetProtocol(location) == _T("lmVirtualBooks"));
}


wxFSFile* lmVirtualBooks::OpenFile(wxFileSystem& WXUNUSED(fs), const wxString& location)
{
    wxFSFile *f;
    wxInputStream *str;
    wxString* pBook;

    //wxLogMessage(_T("[lmVirtualBooks::OpenFile] Request to open virtual file '%s'"), location);

    wxString sExt(location.Right(4).Lower());
    if (sExt == _T(".jpg") || sExt == _T(".gif") || sExt == _T(".png") ) {
        wxString sMimeType;
        int i = location.Find(_T(':'));
        wxString sPath = g_pPaths->GetVBookImagesPath();
        wxFileName oFilename(sPath, location.Mid(i+1), wxPATH_NATIVE);
        str = new wxFileStream( oFilename.GetFullPath());
        if (sExt == _T(".jpg"))
            sMimeType = _T("image/jpeg");
        else if (sExt == _T(".gif"))
            sMimeType = _T("image/gif");
        else if (sExt == _T(".png"))
            sMimeType = _T("image/png");
        f = new wxFSFile(str, location, sMimeType, wxEmptyString, wxDateTime::Today());
        return f;
    }

    if (location == _T("lmVirtualBooks:intro.hhp")) {
        pBook = &m_sIntroHHP;
    }
    else if (location == _T("lmVirtualBooks:intro.hhc")) {
        pBook = &m_sIntroHHC;
    }
    else if (location == _T("lmVirtualBooks:intro.hhk")) {
        pBook = &m_sIntroHHK;
    }
    else if (location == _T("lmVirtualBooks:intro_welcome.htm")) {
        pBook = &m_sIntroHTM;
    }
    else {
        return (wxFSFile*) NULL;    //error
    }

    str = new wxMemoryInputStream(pBook->c_str(), pBook->Length());
    f = new wxFSFile(str, location, wxT("text/html"), wxEmptyString, wxDateTime::Today());
    
    return f;
}

void lmVirtualBooks::LoadIntroBook()
{
    wxString sNil = _T("");

    //book description
    //-----------------------------------------------------------------------------
    m_sIntroHHP = sNil +
        _T("[OPTIONS]\n")
        _T("Contents file=intro.hhc\n")
        _T("Charset=ISO-8859-1\n")
        _T("Index file=intro.hhk\n")
        _T("Title=") + _("LenMus. Introduction") + _T("\n")
        _T("Default topic=intro_welcome.htm\n");

    
    //Book content
    //-----------------------------------------------------------------------------
    m_sIntroHHC = sNil +
        _T("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">")
        _T("<html><head></head><body><ul>")
        _T("<li><object type=\"text/sitemap\">")
        _T("<param name=\"Name\" value=\"") + _("Welcome") + _T("\">")
        _T("<param name=\"Local\" value=\"intro_welcome.htm\">")
        _T("</object></li></ul></body></html>");
    
    //book index
    //-----------------------------------------------------------------------------
    m_sIntroHHK = sNil +
        _T("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">")
        _T("<html><head></head><body><ul>")
        _T("<li><object type=\"text/sitemap\">")
        _T("<param name=\"Name\" value=\"") + _("Welcome") + _T("\">")
        _T("<param name=\"Local\" value=\"intro_welcome.htm\">")
        _T("</object></li></ul></body></html>");
    

    //book page
    //-----------------------------------------------------------------------------
    wxString sVersionNumber = wxGetApp().GetVersionNumber();

    wxString sBookRef = sNil +
        _T("<a href='#LenMusPage/Introduction to single exercises'>") +
        _("Single exercises") + _T("</a>");

    wxString sBook1 = wxString::Format( _("Book %s contains \
some basic exercises for ear training and to practise theoretical concepts, such as \
intervals and scales. Unlike exercises included in the other books, you can customize all \
single exercises included in this book. This will allow you meet \
your specific needs at each moment and to practise specific points."), sBookRef );

    m_sIntroHTM = sNil +
      _T("<html><body><table width='100%'><tr><td align='left'>") 
      _T("<img src='Phonascus-277x63.gif' width='277' height='63' /></td>") 
      _T("<td align='right'><img src='LenMus-170x44.gif' width='170' height='44' /></td>") 
      _T("</tr></table><p>&nbsp;</p><h1>") +
      _("Welcome to LenMus Phonascus version ") + sVersionNumber +
      _T("</h1><p>") +
      _("Phonascus, from Latin 'the teacher of music', is a music education software that you can \
use to practice your music reading skills, improve your ear training abilities, or just learn the \
fundamental principles of music theory and language. \
From version 3.1 it includes new functions that, I hope, \
will end up with full support for the creation, display, \
printing, play back and interactive edition of music scores. Visit the \
LenMus web site for more information about the current status of the score editor.") +
      _T("</p><p>") +
      _("LenMus Phonascus allows you to focus on specific skills and exercises, on \
both theory and ear training. The different activities can be customized to meet \
your specific needs at each moment and it allows you to work at your own pace.") +
      _T("</p><p>") +
      _("LenMus is structured as a collection of fully interactive textbooks (eBooks), \
including theory and exercises. All exercises are fully \
integrated into the text. Music scores are not just \
images or pictures but fully interactive operational music scores that you can hear, \
in whole or just the measures you choose. This enables you \
to put concepts into practice with immediate feedback and makes the subject matter both \
more accessible and more rewarding, as you can hear, test and put in practise \
immediately any new concept introduced.") +
      _T("</p><ul><li>") +
      _("Building and spelling intervals, scales and tonal keys.") +
      _T("</li><li>") +
      _("Ear training: identification of intervals and scales.") +
      _T("</li><li>") +
      _("Rhythm and music reading exercises.") +
      _T("</li></ul><p>") +
      _("Teaching material is organized into levels, and each level into lessons. In this \
version the following books are is included:") +
      _T("</p>") + sBook1 + 
      _T("</body></html>");
//<ul><li>") +

//</li>
//<li>Book <a href="#LenMusPage/Music Reading. Level 2">Music Reading. Level 2</a>
//    is a set of music reading lessons, in difficulty progression, covering the syllabus
//    of a second course at elemental level.</a></li>
//<li>Finally, book <a href="#">LenMus. Introduction</a>
//    is just this text.</a></li>
//</ul>
//

}



//-------------------------------------------------------------------------------------
// global functions
//-------------------------------------------------------------------------------------
void LoadVirtualBooks(lmTextBookController* pBookController)
{
    wxString sFilename;

    //load 'Introduction' book
    sFilename = _T("lmVirtualBooks:intro.hhp");
    if (!pBookController->AddBook(sFilename)) {
        //! @todo better error handling
        wxMessageBox(wxString::Format(_("Failed adding virtual book %s"), sFilename ));
    }

}


