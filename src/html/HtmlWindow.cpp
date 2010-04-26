//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "HtmlWindow.h"
#endif

#include <wx/wxprec.h>
#include <wx/defs.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "HtmlWindow.h"

// access to textBookController
#include "../app/MainFrame.h"
extern lmMainFrame* g_pMainFrame;

extern bool LaunchDefaultBrowser(const wxString& url);      // in Updater.cpp


#include "TextBookController.h"


/*! @page InternalLinks

	<h2>Internal links: rationale and syntax</h2>
    <!-- ====================================== -->

    There are cases in which it is necessary to have links containing commands for the
    program. For example, embbeding a MusicReadingCtrol in music reading books is not a
    good solution as normally there are two or more exercises associated to a lesson.
    So, it is necessary to jump to a MusicReading dialog customized
	according exercises needs; and this dialog must have a "return to theory" link. All
    these forward/backward links must be managed by the program, as well as the dynamic
    generation of the html page.

    Although wxHtmlHelpController Display() method deals with books, pages IDs and other
    posibilities to specify the page to display, once a page is displayed the URLs specified
    in \<a href=xx\> tags are normal links, that is, they will be interpreted as references
    to html pages.

    Then, for links in pages, it is required that either the relative path
    is included or that all html pages are stored in the same folder. Moreover,
    if .htb books (zip files) are used, the html page is only looked for inside
    the zip file.

    In order to allow for including
    commands in an URL the OnLinkClicked() handler analyzes the requested URL, identifies
    those for LenMus commnads, and execute them.

    Normal URLs are only useful for links to html documents (not the case for this
    application) or for links to other book pages. In this second case, either it
    must be a reference to an html document including the relative path (not useful
    when, as it is here the case, the pages of each book are in different folder).
    And it does not work when the book is in zip format (.htb)

    So, to allow for a method to refer to other books or to pages in other books the
    following URL syntax is defined:

    <code>
    href="#[LenMusType]/[parm_1]/[parm_2]/.../[parm_n]
    </code>

    where:

    [LenMusType]  - a string to identify the the link as an internal one and its type:
        "LenMusPage"        - a link to a page in this/other book
        "LenMusEmbedded"    - an html page embedded into the url

        //"LenMusCmd"     - a command for an embedded object (score, control, ...)
        //"LenMusLink"    - a command to close the book and open a window

    The syntax for each type of internal link is as follows:

    a) LenMusPage links
    -------------------

        Format: #LenMusPage/[PageName.htm]

        Example:
            <a href="#LenMusPage/book3_page2.htm">Link to a page</a>

    b) Embedded html content
    ------------------------

        Format: #LenMusEmbedded/[html content using "{" and "}" instead of "<" and ">"]

        Example:
            <a href="#LenMusEmbedded/
                        {html}{body}
                        {h1}Page title{/h1}
                        {p}This is an example{/p}
                        {/body}{/hmtl}
                ">Link to an embedded page</a>



//    c) LenMusCmd links
//
//    href="#LenMusCmd/[ObjId]/[object]/[command]"
//
//    [ObjId]        - a number. It must be the index (0 based) for locating the object in
//                    the global array of embedded controls
//    [object]    - a string identifying object class, i.e. "TeoArmaduras", "TeoEscalas",
//                    "TeoIntervalos", "Solfeo", "OidoIntervalos", "OidoComparar"
//    [command]    - a string with the command name, optionally followed by parameters:
//                    [cmdName]/[parm_1]/[parm_2]/.../[parm_n]
//
//                    [cmdName]    - a string with no blanks
//                    [parm_i]    - a string with no blanks
//
//    Examples:
//
//    "#LenMusCmd/3/ScoreAuxCtrol/Play"
//    "#LenMusCmd/3/ScoreAuxCtrol/PlayMeassure/5"
//
//
//    The sytax for links is:
//
//    href="#LenMusLink/[object]/[command]"
//
//    [object]    - a string identifying object class, i.e. "TeoArmaduras", "TeoEscalas",
//                    "TeoIntervalos", "Solfeo", "OidoIntervalos", "OidoComparar"
//    [command]    - a string with the command name, optionally followed by parameters:
//                    [cmdName]/[parm_1]/[parm_2]/.../[parm_n]
//
//                    [cmdName]    - a string with no blanks
//                    [parm_i]    - a string with no blanks
//
//
//<Phonascus migration notes>
//    - Los enlaces de tipos D y P no son necesarios pues están cubiertos por la funcionalidad
//        normal de un link html.
//
//    - Los enlaces O corresponden a URLs de tipo LenMusCmd
//    - Los enlaces T son un caso particular de los O
//
//    - Los enlaces de tipo F corresponden a URLs de tipo LenMusLink
//
//
//    ' Tipos de enlaces para etiquetas <a>
//    ' -------------------------------------
//    ' El primer caracter indica el tipo de enlace:
//    ' D - link a otro documento
//    '       D<documento>
//    '       D<documento>#<Ref.tema>
//    ' F - link a un formulario
//    '       F<nombre del formulario>
//    ' O - orden para objeto empotrado
//    '       O<?>
//    ' P - link a otra página de este documento
//    '       P<Ref.tema>
//    ' T - Tocar partitura/compas
//    '       T<?>
//    '
//    ' donde:
//    '   <documento> = nombre archivo sin path ni extension. Ej: "Curso2Teoria"
//    '   <Ref.tema> = <capitulo><seccion><tema>. Ej: "C1S2T17"

*/

void lmHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link)
{
    wxString sLocation = link.GetHref();
    //verify if it is a LenMus command link
    int iPos = sLocation.Find(wxT("#LenMus"));
    if (iPos == wxNOT_FOUND) {
        // normal link.
        // Open web browser as wxHtml doesn't work
        LaunchDefaultBrowser(sLocation);

        // Forward it to controller for normal processing
        //wxHtmlWindow::OnLinkClicked(link);
        return;
    }

    //
    // lenMus internal link processing
    //

    //wxLogMessage(_T("[lmHtmlWindow::OnLinkClicked] Internal link. url '%s'"), sLocation);

    //
    // analyze command type and process it
    //

    // embedded html content: an html page embedded into the url
    // Format: #LenMusEmbedded/[html content using "{" and "}" instead of "<" and ">"]
    wxString sTag = _T("#LenMusEmbedded/");
    iPos = sLocation.Find(sTag);
    if (iPos != wxNOT_FOUND) {
        //Get content
        wxString sContent = sLocation.substr(sTag.length());

        // replace "{" and "}" by "<" and ">", respectively
        sContent.Replace(_T("{"), _T("<"));
        sContent.Replace(_T("}"), _T(">"));
        //wxLogMessage(_T("[lmHtmlWindow::OnLinkClicked] Embedded content = %s"), sContent);

        // show the embedded page
        SetPage(sContent);
        //wxLogMessage(_T("[lmHtmlWindow::OnLinkClicked] \n") + sContent);
        return;
    }

    // link to an html page in this/other book
    // Format: #LenMusPage/[PageName.htm]
    sTag = _T("#LenMusPage/");
    iPos = sLocation.Find(sTag);
    if (iPos != wxNOT_FOUND) {
        // extract page name
        wxString sPageName = sLocation.substr(sTag.length());

        // get text book controller
        lmTextBookController* pBook = g_pMainFrame->GetBookController();

        // jump to the requested page
        //wxLogMessage(_T("[lmHtmlWindow::OnLinkClicked] Display PageName <%s>"), sPageName);
        pBook->Display(sPageName);
        return;
    }

    else {
        wxString sMsg = wxString::Format(_T("Bad syntax in link href <%s>"),
                            sLocation.c_str());
        wxLogMessage(_T("[lmHtmlWindow::OnLinkClicked] %s"), sMsg.c_str());
        return;     // ignore the link
    }


}

