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

#include "lenmus_dlg_books.h"
#include "lenmus_standard_header.h"

#include "lenmus_paths.h"
#include "lenmus_book_reader.h"
#include "lenmus_main_frame.h"      //event identifiers
#include "lenmus_help_system.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/intl.h>
#include <wx/html/htmlwin.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/dir.h>
#include <wx/filename.h>


namespace lenmus
{



//=======================================================================================
// BooksDlg implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE(BooksDlg, wxDialog)
   EVT_BUTTON(wxID_OK, BooksDlg::OnClose)
   EVT_HTML_LINK_CLICKED(wxID_ANY, BooksDlg::OnLinkClicked)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
BooksDlg::BooksDlg(wxWindow* pParent, ApplicationScope& appScope)
    : wxDialog(pParent, wxID_ANY, _("Available music books"),
               wxDefaultPosition, wxSize(600,400),
               wxDEFAULT_DIALOG_STYLE)
    , m_appScope(appScope)
    , m_pParent(pParent)
    , m_fullName("")
{
    CreateControls();
    load_available_books();
    CentreOnScreen();
}

//---------------------------------------------------------------------------------------
BooksDlg::~BooksDlg()
{
}

//---------------------------------------------------------------------------------------
void BooksDlg::CreateControls()
{
    //AWARE: Code created with wxFormBuilder and copied here.
    //Modifications:
    // - near line 178: add 'if' to hide Save button

    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* pMainSizer;
	pMainSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pHtmlWnd = LENMUS_NEW wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	pMainSizer->Add( m_pHtmlWnd, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* pButtonsSizer;
	pButtonsSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pBtnAccept = LENMUS_NEW wxButton( this, wxID_OK, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtnAccept, 0, wxALL, 5 );


	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	pMainSizer->Add( pButtonsSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
void BooksDlg::OnClose(wxCommandEvent& WXUNUSED(event))
{
   EndModal(wxID_CANCEL);
}

//---------------------------------------------------------------------------------------
void BooksDlg::load_available_books()
{
    Paths* pPaths = m_appScope.get_paths();
    wxString sPath = pPaths->GetBooksPath();
    wxString sPattern = "*.lmb";

    wxString sRead = _("Please read the <a-1>Study guide</a-1> for information about \
the best way to use LenMus Phonascus and the books.");

    sRead.Replace("<a-1>", "<a href=\"lenmus#study-guide\">'");
    sRead.Replace("</a-1>", "'</a>");


    wxString sHeader = "<html><body>";
    wxString sContent = sHeader +
        "<p>" + sRead +
        "</p>"
        "<h3>" + _("Available books:") + "</h3><ul>";

//    wxLogMessage("[BooksDlg::load_available_books] Scanning path <%s>", sPath.wx_str());
    wxDir dir(sPath);
    if ( !dir.IsOpened() )
    {
        wxMessageBox( wxString::Format(_("Error when trying to move to folder %s"),
                                       sPath.wx_str() ));
        LOMSE_LOG_ERROR("Error when trying to move to folder %s",
                        sPath.ToStdString().c_str() );
        return;
    }

    //Scan the books folder and load all books found
    wxString filename;
    bool fFound = dir.GetFirst(&filename, sPattern, wxDIR_FILES);
    while (fFound)
    {
        if ((filename != "TestingNewExercises.lmb" &&
             filename != "L1_Dictation.lmb" &&
             filename != "L3_MusicReading.lmb" )
            || m_appScope.are_experimental_features_enabled() )
        {
            wxFileName oFilename(sPath, filename, wxPATH_NATIVE);
            wxString name = get_book_name(oFilename);
            wxString link = wxString::Format("book-%s", filename.wx_str());

            sContent += "<li><a href=\"lenmus#"
                     + link
                     + "\">"
                     + name
                     + "</a></li>";
        }
        fFound = dir.GetNext(&filename);
    }
    sContent += "</ul></body></html>";

    m_pHtmlWnd->SetPage(sContent);
}

//---------------------------------------------------------------------------------------
wxString BooksDlg::get_book_name(wxFileName& oFilename)
{
    //create a books collection containing only this book
    BooksCollection books;
    BookRecord *pRecord = books.add_book(oFilename);

    //read the TOC file and get book title
    return pRecord->GetTitle();
}

//---------------------------------------------------------------------------------------
void BooksDlg::OnLinkClicked(wxHtmlLinkEvent& event)
{
    const wxHtmlLinkInfo& link = event.GetLinkInfo();
    wxString sLocation = link.GetHref();

    //study guide
    if (sLocation == "lenmus#study-guide")
    {
//        HelpSystem* pHelp = m_appScope.get_help_controller();
//        pHelp->display_section(10101);    //study-guide
        show_html_document("study-guide.htm");
        EndModal(wxID_CANCEL);
        return;
    }

    //verify if it is a LenMus command link
    int iPos = sLocation.Find("lenmus#");
    if (iPos == wxNOT_FOUND)
    {
        // external link
        ::wxLaunchDefaultBrowser(sLocation);
    }
    else
    {
        wxString filename = sLocation.substr(12);
        Paths* pPaths = m_appScope.get_paths();
        wxString sPath = pPaths->GetBooksPath();
        wxFileName oFile(sPath, filename, wxPATH_NATIVE);
        if (!oFile.FileExists())
        {
            //try to use the english version
            sPath = pPaths->GetLocaleRootPath();
            oFile.AssignDir(sPath);
            oFile.AppendDir("en");
            oFile.AppendDir("books");
            oFile.SetFullName(filename);
            if (!oFile.FileExists())
            {
                wxMessageBox(_("Sorry: File not found!"));
                LOMSE_LOG_WARN("File '%s' not found!",
                               oFile.GetFullPath().ToStdString().c_str() );
                return;
            }
        }

        m_fullName = oFile.GetFullPath();
        EndModal(wxID_OK);
    }
}

//---------------------------------------------------------------------------------------
void BooksDlg::show_html_document(const wxString& sDocName)
{
    Paths* pPaths = m_appScope.get_paths();
    wxString sPath = pPaths->GetLocalePath();
    wxFileName oFile(sPath, sDocName, wxPATH_NATIVE);
	if (!oFile.FileExists())
	{
		//try to display the english version
		sPath = pPaths->GetLocaleRootPath();
		oFile.AssignDir(sPath);
		oFile.AppendDir("en");
		oFile.SetFullName(sDocName);
        if (!oFile.FileExists())
        {
            wxMessageBox(_("Sorry: File not found!"));
            LOMSE_LOG_ERROR("File %s' not found!",
                            oFile.GetFullPath().ToStdString().c_str() );
            return;
        }
	}
    ::wxLaunchDefaultBrowser( oFile.GetFullPath() );
}


}   // namespace lenmus
