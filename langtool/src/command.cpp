//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "command.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "command.h"
#include "ebook_processor.h"


// All actions that is posible to do via command line

void CmdCompileBook(wxString sBook, wxString sOut, wxString sLangs, wxString sLocalePath)
{
    //
    // Compiles the specified eBook, for the specified list of languages
    // Compiled eBooks are stored in specified folder
    // params:
    //      -c      [compile ebook]
    //      -book: <full-path>
    //      -out: <full-path>
    //      <lang-list> or 'all'
    //
    // example:
    //      langtool -c -book ..\books\src\L1_MusicReading.xml -out ..\books es en nl
    //

    wxMessageBox(
        wxString::Format(_T("Compliling eBook '%s' for languages '%s'. Output will\n")
                        _T("be left in folder '%s'"), sBook, sLangs, sOut ));

    //Get book name
    const wxString sBookName = sBook;
    //LogMessage(_T("Preparing to process eMusicBook %s\n"), rOptions.sSrcPath );

    // Get languages
    //bool fLanguage[eLangLast];

    int nDbgOpt = 0;
    //if (rOptions.fDump) nDbgOpt |= eDumpTree;
    //if (rOptions.fLogTree) nDbgOpt |= eLogTree;
    lmEbookProcessor oEBP(nDbgOpt, (wxTextCtrl*)NULL);
    //Loop to use each selected language
    //for(int i=0; i < eLangLast; i++) 
    for(int i=0; i < 1; i++) 
    {
        //if (fLanguage[i]) {
        if (true)
        {
            wxLocale* pLocale = (wxLocale*)NULL;
            //wxString sLang = g_tLanguages[i].sLang;
            //wxString sCharCode = g_tLanguages[i].sCharCode;
            wxString sLang = _T("es");
            wxString sCharCode = _T("iso-8859-1");
            if (i != 0)
            {
                pLocale = new wxLocale();
                wxString sNil = _T("");
                //wxString sLangName = g_tLanguages[i].sLangName;
                wxString sLangName = _T("Spanish");

                pLocale->Init(_T(""), sLang, _T(""), true, true);
                pLocale->AddCatalogLookupPathPrefix( sLocalePath );
                wxString sCatalogName = sBookName + _T("_") + pLocale->GetName();
                pLocale->AddCatalog(sCatalogName);
                
                //LogMessage(_T("Locale changed to %s language (using %s)."),
                //            pLocale->GetName(), sCatalogName + _T(".mo\n") );
            }
            //if (m_pOptMenu->IsChecked(MENU_UTF8)) sCharCode = _T("utf-8");
            oEBP.GenerateLMB(sBook, sLang, sCharCode);

            if (i != 0) delete pLocale;
        }
    }
    return;

}
