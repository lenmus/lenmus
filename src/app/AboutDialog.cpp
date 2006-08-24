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
/*! @file AboutDialog.cpp
    @brief Implementation file for class lmAboutDialog
    @ingroup app_gui
*/
//for GCC
#ifdef __GNUG__
    #pragma implementation "ErrorDlg.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/dialog.h>
#include <wx/button.h>
#include "wx/xrc/xmlres.h"

#include "AboutDialog.h"

//to determine debug/release version
extern bool g_fReleaseVersion;        // to enable/disable debug features
extern bool g_fReleaseBehaviour;    // This flag is only used to force release behaviour when
                                    // in debug mode, and only for some functions (the ones using this flag)

//to get wxMidi version
#include "wxMidi.h"

#include "TheApp.h"         //to get access to locale info.


BEGIN_EVENT_TABLE(lmAboutDialog, wxDialog)
    EVT_BUTTON( XRCID( "btnAccept" ), lmAboutDialog::OnAccept )
    EVT_BUTTON( XRCID( "btnPurpose" ), lmAboutDialog::OnPurpose )
    EVT_BUTTON( XRCID( "btnLicense" ), lmAboutDialog::OnLicense )
    EVT_BUTTON( XRCID( "btnDevelopers" ), lmAboutDialog::OnDevelopers )
    EVT_BUTTON( XRCID( "btnArtCredits" ), lmAboutDialog::OnArtCredits )
    EVT_BUTTON( XRCID( "btnSoftwareCredits" ), lmAboutDialog::OnSoftwareCredits )
    EVT_BUTTON( XRCID( "btnBuildInfo" ), lmAboutDialog::OnBuildInfo )

END_EVENT_TABLE()



lmAboutDialog::lmAboutDialog(wxWindow* pParent)
{
    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, pParent, _T("AboutDialog"));

        //
        //get pointers to all controls
        //

    m_pTxtTitle = XRCCTRL(*this, "txtTitle", wxStaticText);
    m_pTxtSubtitle = XRCCTRL(*this, "txtSubtitle", wxStaticText);
    m_pHtmlWindow = XRCCTRL(*this, "htmlWindow", wxHtmlWindow);

    //load error icon
    wxStaticBitmap* pBmpError = XRCCTRL(*this, "bmpLogo", wxStaticBitmap);
    pBmpError->SetBitmap( wxArtProvider::GetIcon(_T("logo50x67"), wxART_OTHER) );

    //initializations
    m_sHeader =
      _T("<html>")
      _T("<head><META http-equiv=\"Content-Type\" content=\"text/html; charset=") +
      wxLocale::GetSystemEncodingName() +
      _T("\"></head>")
      _T("<body bgcolor=\"#ffffff\">")
      _T("<font size=1>");

    //version
    m_sVersionNumber = wxGetApp().GetVersionNumber();

    //title and subtitle
    m_pTxtTitle->SetLabel(m_pTxtTitle->GetLabel() + m_sVersionNumber);
    m_pTxtSubtitle->SetLabel(_("A free program for music language learning"));

    CentreOnScreen();

    //display purpose information
    wxCommandEvent event;       //no need to initialize it as it is not used
    OnPurpose(event);

}

lmAboutDialog::~lmAboutDialog()
{
}

void lmAboutDialog::OnAccept(wxCommandEvent& WXUNUSED(event))
{
   EndModal(wxID_OK);
}

void lmAboutDialog::OnPurpose(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
      _T("<center>")
      _T("<h3>") + _("Purpose") + _T("</h3>")
      _T("</center><p>") +
      _("LenMus is a free program to help you in the study of music theory and ear training. \
It is available for Windows but soon it will be also available \
for Mac OS X, Linux, and other Unix-like operating systems.") +
      _T("</p><p>") +
      _("The LenMus Project is an open project, committed to the principles of \
Open Source, free education, and free access to information. It has no comercial \
purpose. It is an open workbench for working on all areas related to teaching music, \
and music representation and management with computers. It aims at developping \
public knowledge, methods and algorithms related to all these areas and at the \
same time provides free quality software for music students, amateurs, and \
teachers.") +
      _T("</p><p>") +
      _("If you find a bug or have a suggestion, please contact us. \
We depend on your feedback in order to continue to improve \
LenMus. So please visit our website and give us your bug reports  \
and feature requests. Thank you very much.") +
      _T("</p></body></html>");

    m_pHtmlWindow->SetPage(sContent);

}

void lmAboutDialog::OnLicense(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
        _T("<center>")
        _T("<h3>") + _("License") + _T("</h3></center><p>")
        _T("LenMus ") + m_sVersionNumber + _T(" ") +
        _("Copyright &copy; 2002-2006 Cecilio Salmer&oacute;n.") +
        _T("</p><p>") +
        _("This program is free software; you can redistribute it and/or modify it \
under the terms of the GNU General Public License as published by the Free \
Software Foundation; either version 2 of the License, or (at your option) \
any later version.") +
        _T("</p><p>") +
        _("This program is distributed in the hope that it will be useful, but \
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY \
or FITNESS FOR A PARTICULAR PURPOSE.  For more details see the GNU General \
Public License at http://www.gnu.org/licenses/licenses.html") +
        _T("</p></body></html>");

        m_pHtmlWindow->SetPage(sContent);

}

void lmAboutDialog::OnArtCredits(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
        _T("<center>")
        _T("<h3>") + _("Art desing credits") + _T("</h3></center><p>") +
        _("Some icons are original artwork for LenMus project; all these original icons are released \
under GNU GPL licence and under Creative Commons Attribution-ShareAlike \
license, you can choose the one you prefer for the intended usage. The other \
icons are taken from different sources:") +
        _T("<br /><br /></p><ul><li>") +
        _("Taken or derived from those available in the Ximian collection \
(http://www.novell.com/coolsolutions/feature/1637.html). \
Novell, who retains the copyright, has released these icons under the LGPL license. \
This means that you can use the icons in your \
programs free of charge. If you want to fork the icons into your own icon collection, you need \
to retain the license and the original copyrights, but from there you're free to do what \
you want.") +
        _T("<br /></li><li>") +
        _("Taken or derived from the GNOME project (http://art.gnome.org/themes/icon/1150), \
Tango theme. These icons are released under the Creative Commons Attribution-ShareAlike \
license.") +
        _T("<br /></li><li>") +
        _("These icons are taken or derived from KDE project \
(http://kde.openoffice.org/servlets/ProjectDocumentList?folderID=314&amp;expandFolder=314). \
These icons are released under the GNU GPL license.") +
        _T("<br /></li><li>") +
        _("Taken or derived from eMule project (http://sourceforge.net/projects/emule) \
icons. eMule is a GNU GPL project so I assume that its icons are also available under GNU GPL.") +
        _T("<br /></li><li>") +
_("Taken or derived from John Zaitseff's icons \
(http://www.zap.org.au/documents/icons/dirtree-icons/sample.html). According to what \
is said there, these icons are released under the terms of the GNU General Public License.") +
        _T("<br /><br /></li></ul>") +
        _("To LenMus developers knowledge, all used icons are freely usable either \
under GNU GPL, \
LGPL licence, or Creative Commons. If this were not the case, please, let us \
know to stop using them. \
See file \"icons.htm\" in folder \"res/icons\" for more details.") +
        _T("</p></body></html>");

        m_pHtmlWindow->SetPage(sContent);

}

void lmAboutDialog::OnSoftwareCredits(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
        _T("<center>")
        _T("<h3>") + _("Software credits") + _T("</h3></center><p>") +
        _("LenMus Phonascus uses <b>PortMidi</b> the Portable Real-Time MIDI Library \
(http://www.cs.cmu.edu/~music/portmusic/). PortMidi is copyright (c) 1999-2000 Ross \
Bencina and Phil Burk and copyright (c) 2001 Roger B. Dannenberg. Its licence permits \
free use.") +
        _T("</p><p>") +
        _("LenMus Phonascus is build using the <b>wxWidgets</b> application framework \
(http://www.wxwidgets.org). It is 'Open Source', has multi-platform support, it is \
ease to learn and extend, it has a helpful community, and also has the possibility \
to use it in commercial products without licencing.") +
        _T("</p><p>") +
        _("Most of 'lmbasic.ttf' font glyphs are taken from <b>LilyPond</b> GNU GPL project \
(http://lilypond.org), Feta font.") +
        _T("</p></body></html>");

        m_pHtmlWindow->SetPage(sContent);

}

void lmAboutDialog::OnBuildInfo(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
        _T("<center>")
        _T("<h3>") + _("Build information") + _T("</h3></center><p>") +
        _("Program build date:") +
        _T(" ") __TDATE__ _T("<br>") +
        wxVERSION_STRING + _T("<br>") +
        _("wxMidi Version ") + wxMIDI_VERSION + _T("<br><br><br>") +
        _("Charset encoding: ") + wxLocale::GetSystemEncodingName() + _T("<br>") +
        _("System locale name: ") + wxGetApp().GetLocaleSysName() + _T("<br>") +
        _("Canonical locale name: ") + wxGetApp().GetLanguageCanonicalName() +
        _T("<br></body></html>");

        m_pHtmlWindow->SetPage(sContent);

}

void lmAboutDialog::OnDevelopers(wxCommandEvent& WXUNUSED(event))
{
    wxString sContent = m_sHeader +
      _T("<center>")
      _T("<h3>") + _("Developers") + _T("</h3>")
      _T("</center><p>") +
      _("This program has been designed and developed by Cecilio Salmer&oacute;n. \
If you would like to help developing this program or join the project to help in any \
other way, you are indeed welcome; please, visit the LenMus website at www.lenmus.org and \
leave a message in the 'contact' page. Thank you and welcome.") +
      _T("</p></body></html>");

    m_pHtmlWindow->SetPage(sContent);

}

