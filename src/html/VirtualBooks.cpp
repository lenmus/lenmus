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
#include "TextBookController.h"
#include "..\app\TheApp.h"             //access to get version
extern lmTheApp* g_pTheApp;

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;



lmVirtualBooks::lmVirtualBooks(wxString sLang) : wxFileSystemHandler()
{
    //initialize book content for requested language
    m_sPrevLang = _T("");
    ReloadBooks(sLang);
}

lmVirtualBooks::~lmVirtualBooks()
{
}

void lmVirtualBooks::ReloadBooks(wxString sLang)
{
    //initialize book content if language changes
    if ((sLang != m_sPrevLang)) {
        m_sPrevLang = sLang;
        LoadIntroBook();
        LoadSingleExercisesBook();
    }
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

    //wxLogMessage(*pBook);

#ifndef _UNICODE
    str = new wxMemoryInputStream(pBook->c_str(), pBook->Length());
#else
    //by pass for bug in wxHtmlHelpController in Unicode build:
    //wxHtmlFilter::ReadFile() expects an ANSI string. So here I must
    //translate from unicode to ANSI.
        size_t nLong = pBook->Length();
        char* pBookAnsi = new char[4*nLong];  //just in case...
        //wxMBConvUTF8 oConv;
        //size_t nSize = oConv.WC2MB(pBookAnsi, pBook->c_str(), nLong);
        size_t nSize = wxConvUTF8.WC2MB(pBookAnsi, pBook->c_str(), nLong);
        str = new wxMemoryInputStream(pBookAnsi, nSize);
    //
    //wxMBConvUTF8 oConv;
    //wxCharBuffer sBookMB = pBook->mb_str(oConv);
    //str = new wxMemoryInputStream(sBookMB.data(), strlen(sBookMB.data()) );
#endif
    f = new wxFSFile(str, location, wxT("text/html"), wxEmptyString, wxDateTime::Today());
    
    return f;
}

void lmVirtualBooks::LoadIntroBook()
{
    // there must be a bug in wxWidgets and index doesn't work if hhc and hhk goes with
    // UTF-8. So, after a lot of testing, the following combination works
    // 
    // a) for Unicode build:
    //      hhp         -> charset=utf-8, saved as utf-8
    //      hhk, hhc    -> charset=iso-8859-1, saved as utf-8
    //      html        -> charset=any, saved as specified charset
    // 
    // b) for Win98 build:
    //      hhp         -> charset=utf-8, saved as utf-8
    //      hhk, hhc    -> charset=iso-8859-1, saved as utf-8
    //      html        -> charset=any, saved as specified charset


    wxString sNil = _T("");
    wxString sCharset1 = _T("utf-8");
    wxString sCharset2 = GetCharset();

    //book description
    //-----------------------------------------------------------------------------
    m_sIntroHHP = sNil +
        _T("[OPTIONS]\n")
        _T("Contents file=intro.hhc\n")
        _T("Charset=") + sCharset1 + _T("\n")
        _T("Index file=intro.hhk\n")
        _T("Title=") + _("LenMus. Introduction") + _T("\n")
        _T("Default topic=intro_welcome.htm\n");

    
    //Book content
    //-----------------------------------------------------------------------------
    m_sIntroHHC = sNil +
        _T("<html>\n<head>\n")
        _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") + 
            sCharset2 + _T("\">\n")
        _T("</head>\n<body>\n\n<ul>\n")
        _T("  <li><object type=\"text/sitemap\">\n")
        _T("          <param name=\"Name\" value=\"") + _("Welcome") + _T("\">\n")
        _T("          <param name=\"Local\" value=\"intro_welcome.htm\">\n</object></li>\n")
        _T("</ul>\n\n</body>\n</html>\n");
    
    //book index
    //-----------------------------------------------------------------------------
    m_sIntroHHK = m_sIntroHHC;
    

    //book page
    //-----------------------------------------------------------------------------
    wxString sVersionNumber = wxGetApp().GetVersionNumber();

    wxString sBookRef = sNil +
        _T("<a href='#LenMusPage/SingleExercises_0.htm'>") +
        _("Single exercises") + _T("</a>");

    wxString sBook1 = wxString::Format( _("Book %s contains \
some basic exercises for aural training and to practise theoretical concepts, such as \
intervals and scales. Unlike exercises included in the other books, you can customize all \
single exercises included in this book. This will allow you meet \
your specific needs at each moment and to practise specific points."), sBookRef );

    m_sIntroHTM = sNil +
      _T("<html><head>")
      _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") + sCharset2 +
      _T("\">")
      _T("</head><body><table width='100%'><tr><td align='left'>") 
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
      _T("</p><p>") +
      _("Teaching material is organized into levels, and each level into lessons.") +
      sBook1 + 
      _T("</p><p>") +
      _("We hope that LenMus become a useful tool in your studies.") +
      _T("</p><p>") +
      _("The LenMus team.") +
      _T("</p><p>&nbsp;&nbsp;</p>")
      _T("</body></html>");

}

void lmVirtualBooks::LoadSingleExercisesBook()
{
    wxString sNil = _T("");
    wxString sCharset1 = _T("utf-8");
    wxString sCharset2 = GetCharset();

    //book description
    //-----------------------------------------------------------------------------
    m_sSingleHHP = sNil +
        _T("[OPTIONS]\n")
        _T("Contents file=SingleExercises.hhc\n")
        _T("Charset=") + sCharset1 + _T("\n")
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
    sIndexTitle[0] = _("0. Introduction to single exercises");
    sPageTitle[0] = _("Single exercises");
    sContent[0] = sNil;   //content will be created at the end

        //Section 0: Aural training

    i=1;        
    iSecStart[0] = i;   
    sSectionTitle[0] = _("1. Exercises for aural training");

    //EarCompareIntervals
    sIndexTitle[i] = _("1.1. Aural training. Intervals' comparison");
    sPageTitle[i] = _("Aural training. Intervals' comparison");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Lesson two intervals and identify which one is greater") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"EarCompareIntervals\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param  name=\"max_interval\" value=\"8\">")
        _T("</object>");
    i++;

    //EarIntervals
    sIndexTitle[i] = _("1.2. Aural training. Intervals' identification");
    sPageTitle[i] = _("Aural training. Intervals' identification");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Lesson an interval and identify it") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"EarIntervals\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param  name=\"max_interval\" value=\"8\">")
        _T("</object>");
    i++;

    //Ear: Identify a chord
    sIndexTitle[i] = _("1.3. Aural training. Chords' identification");
    sPageTitle[i] = _("Aural training. Chords' identification");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Lesson a chord and identify its type") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"IdfyChord\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param name=\"control_settings\" value=\"EarIdfyChord\">")
        _T("<param name=\"mode\" value=\"earTraining\">")
        _T("</object>");
    i++;

    //Ear: Identify a scale
    sIndexTitle[i] = _("1.4. Aural training. Scales' identification");
    sPageTitle[i] = _("Aural training. Scales' identification");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Lesson a scale and identify its type") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"IdfyScales\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param name=\"control_settings\" value=\"EarIdfyScale\">")
        _T("<param name=\"mode\" value=\"earTraining\">")
        _T("</object>");
    i++;

        //Section 1: Theory

    iSecStart[1] = i;   
    sSectionTitle[1] = _("2. Exercises to practise theory");

    //TheoKeySignatures
    sIndexTitle[i] = _("2.1. Key signatures' identification (in score)");
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
    sIndexTitle[i] = _("2.2. Interval's identification (in score) and construction");
    sPageTitle[i] = _("Interval's identification and construction");
    sContent[i] = sNil +
        _T("<object type=\"Application/LenMus\" classid=\"TheoIntervals\" width=\"100%\" height=\"300\" border=\"0\">")
            _T("<param  name=\"accidentals\" value=\"simple\">")
        _T("</object>");
    i++;

    //TheoScales
    sIndexTitle[i] = _("2.3. Scales' identification (in score)");
    sPageTitle[i] = _("Scales' identification");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Identify the scale shown") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"IdfyScales\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param name=\"control_settings\" value=\"TheoIdfyScale\">")
        _T("<param name=\"mode\" value=\"theory\">")
        _T("</object>");
    i++;

    //Theo: Chords' identification
    sIndexTitle[i] = _("2.4. Chords' identification (in score)");
    sPageTitle[i] = _("Chords' identification");
    sContent[i] = sNil + _T("<p>") +
        _("Exercise: Identify the type of the shown chord") +
        _T("</p><p>&nbsp;</p>")
        _T("<object type=\"Application/LenMus\" classid=\"IdfyChord\" width=\"100%\" height=\"300\" border=\"0\">")
        _T("<param name=\"control_settings\" value=\"TheoIdfyChord\">")
        _T("<param name=\"mode\" value=\"theory\">")
        _T("</object>");
    i++;


        //Section 2: Other exercises
    iSecStart[2] = i;   
    sSectionTitle[2] = _("3. Other exercises");

    //ClefsReading
    sIndexTitle[i] = _("3.1. Clefs Reading");
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
        _T("<html>\n<head>\n")
        _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") + 
            sCharset2 + _T("\">\n")
        _T("</head>\n<body>\n\n<ul>\n");

    for (i=0; i < lmNUM_SINGLE_PAGES; i++) {
        m_sSingleHHC +=
            _T("  <li><object type=\"text/sitemap\">\n")
            _T("         <param name=\"Name\" value=\"") +      //this is the name for links
                sIndexTitle[i]  + _T("\">\n")
            _T("         <param name=\"Local\" value=\"SingleExercises_") +
                wxString::Format(_T("%d.htm"), i) +
            _T("\">\n  </object></li>\n\n");
    }
    m_sSingleHHC += _T("</ul>\n\n</body>\n</html>\n");

    m_sSingleHHK = m_sSingleHHC;
    

    //Book pages
    //-----------------------------------------------------------------------------
    for (i=1; i < lmNUM_SINGLE_PAGES; i++) {
        m_sSingleHTM[i] = sNil +
            _T("<html>\n<head>\n")
            _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") + 
            sCharset2 + _T("\">\n")
            _T("<title>") + sPageTitle[i] + 
            _T("</title>\n</head>\n<body>\n\n<h1>") + sPageTitle[i] +
            _T("</h1>") + sContent[i] + _T("\n</body>\n</html>\n");
    }

    //Now the intro page
    m_sSingleHTM[0] = sNil +
        _T("<html>\n<head>\n")
        _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") + 
        sCharset2 + _T("\">\n")
        _T("<title>") + sPageTitle[0] + 
        _T("</title>\n</head>\n<body>\n\n<h1>") + sPageTitle[0] +
        _T("</h1>\n<p>") +
        _("Appart of exercises included in the eBooks, the following general exercises \
(customizable to suit your needs at any moment) are available:") + _T("</p>\n");
    
    int iItem;
    m_sSingleHTM[0] += _T("<h2>") + sSectionTitle[0] + _T("</h2>\n<ul>\n");
    for (iItem=iSecStart[0]; iItem < iSecStart[1]; iItem++) {
        //m_sSingleHTM[0] += _T("<li><a href=\"#LenMusPage/") +
        //    sIndexTitle[iItem] + _T("\">") + sIndexTitle[iItem] + _T("</a></li>");
        m_sSingleHTM[0] += _T("  <li><a href=\"#LenMusPage/SingleExercises_") +
            wxString::Format(_T("%d.htm"), iItem) +
            _T("\">") + sIndexTitle[iItem] + _T("</a></li>\n\n");
    }
    m_sSingleHTM[0] += _T("</ul>\n\n<h2>") + sSectionTitle[1] + _T("</h2>\n<ul>\n");
    for (iItem=iSecStart[1]; iItem < iSecStart[2]; iItem++) {
        //m_sSingleHTM[0] += _T("<li><a href=\"#LenMusPage/") +
        //    sIndexTitle[iItem] + _T("\">") + sIndexTitle[iItem] + _T("</a></li>");
        m_sSingleHTM[0] += _T("  <li><a href=\"#LenMusPage/SingleExercises_") +
            wxString::Format(_T("%d.htm"), iItem) +
            _T("\">") + sIndexTitle[iItem] + _T("</a></li>\n\n");
    }
    m_sSingleHTM[0] += _T("</ul>\n\n<h2>") + sSectionTitle[2] + _T("</h2><ul>");
    for (iItem=iSecStart[2]; iItem < lmNUM_SINGLE_PAGES; iItem++) {
        //m_sSingleHTM[0] += _T("<li><a href=\"#LenMusPage/") +
        //    sIndexTitle[iItem] + _T("\">") + sIndexTitle[iItem] + _T("</a></li>");
        m_sSingleHTM[0] += _T("  <li><a href=\"#LenMusPage/SingleExercises_") +
            wxString::Format(_T("%d.htm"), iItem) +
            _T("\">") + sIndexTitle[iItem] + _T("</a></li>\n\n");
    }
    m_sSingleHTM[0] += _T("</ul>\n\n</body>\n</html>\n");

}



//-------------------------------------------------------------------------------------
// static methods
//-------------------------------------------------------------------------------------
void lmVirtualBooks::LoadVirtualBooks(lmTextBookController* pBookController)
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

void lmVirtualBooks::GenerateEBooks()
{
    // intro eBook
    WriteEBook(_T("intro.hhp"), m_sIntroHHP, wxConvUTF8);
    WriteEBook(_T("intro.hhc"), m_sIntroHHC, wxConvUTF8);
    WriteEBook(_T("intro.hhk"), m_sIntroHHK, wxConvUTF8);
    WriteEBook(_T("intro_welcome.htm"), m_sIntroHTM, wxConvUTF8);

    //SingleExercises eBook
    for (int i=0; i < lmNUM_SINGLE_PAGES; i++) {
        WriteEBook( wxString::Format(_T("SingleExercises_%d.htm"), i), m_sSingleHTM[i], wxConvUTF8);
    }
    WriteEBook(_T("SingleExercises.hhp"), m_sSingleHHP, wxConvUTF8);
    WriteEBook(_T("SingleExercises.hhc"), m_sSingleHHC, wxConvUTF8);
    WriteEBook(_T("SingleExercises.hhk"), m_sSingleHHK, wxConvUTF8);

}

void lmVirtualBooks::WriteEBook(wxString sBookName, wxString sContent, wxMBConv& conv)
{
    wxFile* pFile = new wxFile(sBookName, wxFile::write);
    if (!pFile->IsOpened()) {
        //todo
        pFile = (wxFile*)NULL;
        return;
    }
    pFile->Write(sContent, conv);
    pFile->Close();
    delete pFile;

}

wxString lmVirtualBooks::GetCharset()
{
    wxString sCharset;
    if (m_sPrevLang == _T("en")) {
        sCharset = _T("iso-8859-1");
    }
    else if (m_sPrevLang == _T("es")) {
        sCharset = _T("iso-8859-1");
    }
    else if (m_sPrevLang == _T("fr")) {
        sCharset = _T("iso-8859-1");
    }
    else if (m_sPrevLang == _T("tr")) {
        sCharset = _T("iso-8859-9");        //cp-1254
    }
    else {
        wxASSERT(false);
    }
    return sCharset;
}



