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

//array sizes
enum { lmNUM_SECTIONS = 3 };
enum { lmNUM_SINGLE_PAGES = 9 };


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
static wxString m_sSingleHTM[lmNUM_SINGLE_PAGES];        //book pages


lmVirtualBooks::lmVirtualBooks() : wxFileSystemHandler()
{
    //initialize book content. It can not be static because then _() macro doesn't
    //work
    if (!m_fBooksLoaded) {
        LoadIntroBook();
        LoadSingleExercisesBook();
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

    int iColon = location.Find(_T(':'));
    wxString sBookName = location.Mid(iColon+1, location.Length()-iColon-5);
    wxString sExt(location.Right(4).Lower());
    //wxLogMessage(_T("[lmVirtualBooks::OpenFile] Request to open virtual file '%s'. Book='%s', ext='%s'"),
    //    location, sBookName, sExt);

    if (sExt == _T("ched"))
        return (wxFSFile*) NULL;    //cached file

    if (sExt == _T(".jpg") || sExt == _T(".gif") || sExt == _T(".png") ) {
        wxString sMimeType;
        wxString sPath = g_pPaths->GetVBookImagesPath();
        wxFileName oFilename(sPath, location.Mid(iColon+1), wxPATH_NATIVE);
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

    wxString sNumPage;
    if (sBookName == _T("intro") || sBookName.StartsWith(_T("intro_")) ) {
        if (location == _T("lmVirtualBooks:intro.hhp")) {
            pBook = &m_sIntroHHP;
            wxLogMessage(m_sIntroHHP);
        }
        else if (location == _T("lmVirtualBooks:intro.hhc")) {
            pBook = &m_sIntroHHC;
            wxLogMessage(m_sIntroHHC);
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
    }
    else if (sBookName == _T("SingleExercises") || 
             sBookName.StartsWith(_T("SingleExercises_"), &sNumPage) )
    {
        if (sExt == _T(".hhp")) {
            pBook = &m_sSingleHHP;
        }
        else if (sExt == _T(".hhc")) {
            pBook = &m_sSingleHHC;
        }
        else if (sExt == _T(".hhk")) {
            pBook = &m_sSingleHHK;
        }
        else if (sExt == _T(".htm")) {
            long nNumPage;
            sNumPage.ToLong(&nNumPage);
            pBook = &m_sSingleHTM[nNumPage];
        }
        else {
            return (wxFSFile*) NULL;    //error
        }
    }
    else
        return (wxFSFile*) NULL;    //error

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
        _T("<param name=\"Local\" value=\"intro_welcome.htm\"></object></li>")
        _T("</ul></body></html>");
    
    //book index
    //-----------------------------------------------------------------------------
    m_sIntroHHK = sNil +
        _T("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">")
        _T("<html><head></head><body><ul>")
        _T("<li><object type=\"text/sitemap\">")
        _T("<param name=\"Name\" value=\"") + _("Welcome") + _T("\">")
        _T("<param name=\"Local\" value=\"intro_welcome.htm\"></object></li>")
        _T("</ul></body></html>");
    

    //book page
    //-----------------------------------------------------------------------------
    wxString sVersionNumber = wxGetApp().GetVersionNumber();

    wxString sBookRef = sNil +
        _T("<a href='#LenMusPage/Introduction to single exercises'>") +
        _("Single exercises") + _T("</a>");

    wxString sBook1 = wxString::Format( _("Book %s contains \
some basic exercises for aural training and to practise theoretical concepts, such as \
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
use to practice your music reading skills, improve your aural recognition abilities, or just learn the \
fundamental principles of music theory and language. \
From version 3.1 it includes new functions that, I hope, \
will end up with full support for the creation, display, \
printing, play back and interactive edition of music scores. Visit the \
LenMus web site for more information about the current status of the score editor.") +
      _T("</p><p>") +
      _("LenMus Phonascus allows you to focus on specific skills and exercises, on \
both theory and aural training. The different activities can be customized to meet \
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
      _("Aural training: identification of intervals and scales.") +
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

void lmVirtualBooks::LoadSingleExercisesBook()
{
    wxString sNil = _T("");

    //book description
    //-----------------------------------------------------------------------------
    m_sSingleHHP = sNil +
        _T("[OPTIONS]\n")
        _T("Contents file=SingleExercises.hhc\n")
        _T("Charset=ISO-8859-1\n")
        _T("Index file=SingleExercises.hhk\n")
        _T("Title=") + _("Single exercises") + _T("\n")
        _T("Default topic=SingleExercises_0.htm\n");

    
    //tables describing the content
    //-----------------------------------------------------------------------------
    wxString sIndexTitle[lmNUM_SINGLE_PAGES];
    wxString sPageTitle[lmNUM_SINGLE_PAGES];
    wxString sContent[lmNUM_SINGLE_PAGES];

    wxString sSectionTitle[lmNUM_SECTIONS];
    int iSecStart[lmNUM_SECTIONS];

    //Page 0: Introduction
    int i=0;
    sIndexTitle[0] = _("Introduction to single exercises");
    sPageTitle[0] = _("Single exercises");
    sContent[0] = sNil;   //content will be created at the end

        //Section 0: Aural training

    i=1;        
    iSecStart[0] = i;   
    sSectionTitle[0] = _("Exercises for aural training");

    //EarCompareIntervals
    sIndexTitle[i] = _("Aural training. Intervals' comparison");
    sPageTitle[i] = _("Aural training. Intervals' comparison");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Lesson two intervals and identify which one is greater") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"EarCompareIntervals\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param  name=\"max_interval\" value=\"8\">")
        _T("</object>");
    i++;

    //EarIntervals
    sIndexTitle[i] = _("Aural training. Intervals' identification");
    sPageTitle[i] = _("Aural training. Intervals' identification");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Lesson an interval and identify it") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"EarIntervals\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param  name=\"max_interval\" value=\"8\">")
        _T("</object>");
    i++;

    //Ear: Identify a chord
    sIndexTitle[i] = _("Aural training. Chords' identification");
    sPageTitle[i] = _("Aural training. Chords' identification");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Lesson a chord and identify its type") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"IdfyChord\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param name=\"control_settings\" value=\"EarIdfyChord\">")
        _T("<param name=\"mode\" value=\"earTraining\">")
        _T("<param name=\"chords\" value=\"mT,MT,aT,dT,m7,M7,dom7\">")
        _T("<param name=\"keys\" value=\"all\">")
        _T("</object>");
    i++;

        //Section 1: Theory

    iSecStart[1] = i;   
    sSectionTitle[1] = _("Exercises to practise theory");

    //TheoKeySignatures
    sIndexTitle[i] = _("Key signatures' identification (in score)");
    sPageTitle[i] = _("Key signatures' identification");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Identify a key signature or indicate the number of accidentals it has") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"TheoKeySignatures\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param  name=\"max_accidentals\" value=\"7\">")
        _T("<param  name=\"problem_type\" value=\"both\">")
        _T("<param  name=\"clef\" value=\"sol\">")
        _T("<param  name=\"mode\" value=\"both\">")
        _T("</object>");
    i++;

    //TheoIntervals
    sIndexTitle[i] = _("Interval's identification (in score) and construction");
    sPageTitle[i] = _("Interval's identification and construction");
    sContent[i] = sNil +
        _T("<object type=\"Application/LenMus\" classid=\"TheoIntervals\" width=\"100%\" height=\"300\" border=\"0\">")
            _T("<param  name=\"accidentals\" value=\"simple\">")
        _T("</object>");
    i++;

    //TheoScales
    sIndexTitle[i] = _("Scales' identification (in score)");
    sPageTitle[i] = _("Scales' identification");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Identify the scale shown") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"TheoScales\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param  name=\"problem_type\" value=\"DeduceScale\">")
        _T("<param  name=\"control\" value=\"chkKeySignature\">")
        _T("<param  name=\"clef\" value=\"sol\">")
        _T("</object>");
    i++;

    //Theo: Chords' identification
    sIndexTitle[i] = _("Chords' identification (in score)");
    sPageTitle[i] = _("Chords' identification");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Identify the type of the shown chord") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"IdfyChord\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param name=\"control_settings\" value=\"TheoIdfyChord\">")
        _T("<param name=\"mode\" value=\"theory\">")
        _T("<param name=\"chords\" value=\"mT,MT,aT,dT,m7,M7,dom7\">")
        _T("<param name=\"keys\" value=\"all\">")
        _T("</object>");
    i++;


        //Section 2: Other exercises
    iSecStart[2] = i;   
    sSectionTitle[2] = _("Other exercises");

    //ClefsReading
    sIndexTitle[i] = _("Clefs Reading");
    sPageTitle[i] = _("Clefs Reading");
    sContent[i] = sNil + 
        _T("<object type=\"Application/LenMus\" classid=\"TheoMusicReading\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param name=\"control_play\" value=\"\">")
        _T("<param name=\"control_solfa\" value=\"\">")
        _T("<param name=\"control_settings\" value=\"single_clefs_reading\">")
        _T("<param name=\"fragment\" value=\"24;(n * s g+)(n * s)(n * s)(n * s g-)\">")
        _T("<param name=\"clef\" value=\"Sol;a3;a5\">")
        _T("<param name=\"clef\" value=\"Fa4;a2;e4\">")
        _T("<param name=\"time\" value=\"24\">")
        _T("<param name=\"key\" value=\"all\">")
        _T("<param name=\"maxInterval\" value=\"4\">")
        _T("</object>");
    i++;

    wxASSERT(i == lmNUM_SINGLE_PAGES);

    //Now, lets build the pages

    //Book content and index
    //-----------------------------------------------------------------------------
    m_sSingleHHC = sNil +
        _T("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">")
        _T("<html><head></head><body><ul>");

    for (i=0; i < lmNUM_SINGLE_PAGES; i++) {
        m_sSingleHHC +=
            _T("<li><object type=\"text/sitemap\">")
            _T("<param name=\"Name\" value=\"") +
            sIndexTitle[i]  + _T("\">")
            _T("<param name=\"Local\" value=\"SingleExercises_") +
            wxString::Format(_T("%d.htm"), i) +
            _T("\"></object></li>");
    }
    m_sSingleHHC += _T("</ul></body></html>");

    m_sSingleHHK = m_sSingleHHC;
    

    //Book pages
    //-----------------------------------------------------------------------------
    for (i=1; i < lmNUM_SINGLE_PAGES; i++) {
        m_sSingleHTM[i] = sNil +
            _T("<html><head><title>") + sPageTitle[i] + 
            _T("</title></head><body><h1>") + sPageTitle[i] +
            _T("</h1>") + sContent[i] + _T("</body></html>");
    }

    //Now the intro page
    m_sSingleHTM[0] = sNil +
        _T("<html><head><title>") + sPageTitle[0] + 
        _T("</title></head><body><h1>") + sPageTitle[0] +
        _T("</h1><p>") +
        _("Appart of exercises included in the eBooks, the following general exercises \
(customizable to suit your needs at any moment) are available:") + _T("</p>");
    
    int iItem;
    m_sSingleHTM[0] += _T("<h2>") + sSectionTitle[0] + _T("</h2><ul>");
    for (iItem=iSecStart[0]; iItem < iSecStart[1]; iItem++) {
        m_sSingleHTM[0] += _T("<li><a href=\"#LenMusPage/") +
            sIndexTitle[iItem] + _T("\">") + sIndexTitle[iItem] + _T("</a></li>");
    }
    m_sSingleHTM[0] += _T("</ul><h2>") + sSectionTitle[1] + _T("</h2><ul>");
    for (iItem=iSecStart[1]; iItem < iSecStart[2]; iItem++) {
        m_sSingleHTM[0] += _T("<li><a href=\"#LenMusPage/") +
            sIndexTitle[iItem] + _T("\">") + sIndexTitle[iItem] + _T("</a></li>");
    }
    m_sSingleHTM[0] += _T("</ul><h2>") + sSectionTitle[2] + _T("</h2><ul>");
    for (iItem=iSecStart[2]; iItem < lmNUM_SINGLE_PAGES; iItem++) {
        m_sSingleHTM[0] += _T("<li><a href=\"#LenMusPage/") +
            sIndexTitle[iItem] + _T("\">") + sIndexTitle[iItem] + _T("</a></li>");
    }
    m_sSingleHTM[0] += _T("</ul></body></html>");

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

    //load 'Single Exercises' book
    sFilename = _T("lmVirtualBooks:SingleExercises.hhp");
    if (!pBookController->AddBook(sFilename)) {
        //! @todo better error handling
        wxMessageBox(wxString::Format(_("Failed adding virtual book %s"), sFilename ));
    }

}


