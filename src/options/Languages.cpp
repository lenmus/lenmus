//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "Languages.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dir.h"

#include <wx/intl.h>
#include <wx/stattext.h>

#include <wx/hashmap.h>
WX_DECLARE_STRING_HASH_MAP(wxString, LangHash);

#include "Languages.h"
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;

bool TranslationExists(wxArrayString& pathList, wxString code)
{
    wxArrayString results;
    wxString sPattern = code;

    if (sPattern == _T("")) return false;

    //DBG: To have a place to put a break
    if (sPattern == _T("es") ) {
        int kk;
        kk=23;
    }

    wxString sFileName;
    bool fFound;
    for(unsigned int i=0; i < pathList.GetCount(); i++) {
        wxString path = pathList[i];

        wxDir oDir(path);
        if (!oDir.IsOpened()) continue;    //directory does not exists!
        fFound = oDir.GetFirst(&sFileName, sPattern, wxDIR_FILES | wxDIR_DIRS);
        while(fFound) {
            //wxLogMessage(wxString::Format(_T("[Languages::TranslationExists] code=%s, sFileName=%s"),
            //    code, sFileName));
            results.Add(sFileName);
            fFound = oDir.GetNext(&sFileName);
        }
    }
    return (results.GetCount() > 0);
}

wxString GetSystemLanguageCode()
{
    /*
    Tries to guess current machine system language and returns it
    */
    wxArrayString langCodes;
    wxArrayString langNames;

    GetLanguages(langCodes, langNames);
    int sysLang = wxLocale::GetSystemLanguage();
    const wxLanguageInfo *info = wxLocale::GetLanguageInfo(sysLang);

    if (info) {
        wxString fullCode = info->CanonicalName;
        if (fullCode.Length() < 2)
            return _T("en");

        wxString code = fullCode.Left(2);
        unsigned int i;

        for(i=0; i<langCodes.GetCount(); i++) {
            if (langCodes[i] == fullCode)
                return fullCode;

            if (langCodes[i] == code)
                return code;
        }
    }

    return _T("en");
}

void GetLanguages(wxArrayString &langCodes, wxArrayString &langNames)
{
    /*
    Figure out what translations are installed and return a list
    of language codes (like "es", "fr", or "pt_BR") and corresponding
    language names (like "Español", "Français", and "Português").
    */

    /*! @todo
        The language names are translated to the locale name (in occidental chars) but
        I have not a clear idea of how these translated strings will be displayed in a system
        whose system language is, for example, chinesse. Will display correctly
        in latin characters or will display garbage?
    */

    /*
    Language files are expected to be found in lenmus\locale\ folder.
    */

    /*! @todo
        This code is designed to work well with all languages that wxWindows supports.
        Other languages will only be supported if they're added to
        the database using wxLocale::AddLanguage.
    */

    /*
    But for the most part, this means that anybody could add a new
    translation and have it work immediately.
    */

    //AWARE using string translation ( macro _() ) is not possible as locale could
    //not yet be set, as this code is used at first run.

    wxArrayString tempNames, tempCodes;
    LangHash localLanguageName;
    LangHash reverseHash;

    //List of local translations. //! @todo Add more languages

    // AWARE: I am having problems with GCC as it doesn't 
    // Look for list of entities codes in file include/wx/html/htmlpars.h
    //method wxHtmlEntitiesParser::GetEntityChar(const wxString& entity)

    static wxChar ch_ntilde = 241;
    static wxChar ch_ccedil = 231;
    static wxChar ch_ecirc = 234;
    static wxChar ch_uuml = 252;
    wxString sLangName;

    localLanguageName[_T("bg")] = _T("Balgarski");
    localLanguageName[_T("ca")] = _T("Catalan");
    localLanguageName[_T("da")] = _T("Dansk");
    localLanguageName[_T("de")] = _T("Deutsch");
    localLanguageName[_T("en")] = _T("English");

    // _T("Español");
    sLangName = _T("Espa");
    sLangName.Append(ch_ntilde);
    sLangName.Append( _T("ol") );
    localLanguageName[_T("es")] = sLangName;    // _T("Español");

    localLanguageName[_T("fi")] = _T("Suomi");

    // _T("Français"); 
    sLangName = _T("Fran");
    sLangName.Append(ch_ccedil);
    sLangName.Append( _T("ais") );
    localLanguageName[_T("fr")] = sLangName;    // _T("Français");

    localLanguageName[_T("it")] = _T("Italiano");
    localLanguageName[_T("ja")] = _T("Nihongo");
    localLanguageName[_T("hu")] = _T("Magyar");
    localLanguageName[_T("mk")] = _T("Makedonski");
    localLanguageName[_T("nl")] = _T("Nederlands");
    localLanguageName[_T("nb")] = _T("Norsk");
    localLanguageName[_T("pl")] = _T("Polski");

    // _T("Português");
    sLangName = _T("Portugu");
    sLangName.Append(ch_ecirc);
    sLangName.Append( _T("s") );
    localLanguageName[_T("pt")] = sLangName;    // _T("Português");

    localLanguageName[_T("ru")] = _T("Russky");
    localLanguageName[_T("sl")] = _T("Slovenscina");
    localLanguageName[_T("sv")] = _T("Svenska");

    // _T("Türkçe")
    sLangName = _T("T");
    sLangName.Append(ch_uuml);
    sLangName.Append( _T("rk") );
    sLangName.Append(ch_ccedil);
    sLangName.Append( _T("e") );
    localLanguageName[_T("tr")] = sLangName;    // _T("Türkçe");

    localLanguageName[_T("uk")] = _T("Ukrainska");
    localLanguageName[_T("zh_CN")] = _T("Chinese(Simplified)");

    wxArrayString pathList;
    wxString sLocalePath = g_pPaths->GetLocaleRootPath();
    pathList.Add( sLocalePath );
    wxString lastCode = _T("");

    //explore wxLanguages list to form file names
    int i;
    for(i=wxLANGUAGE_UNKNOWN; i < wxLANGUAGE_USER_DEFINED; i++) {
        const wxLanguageInfo *info = wxLocale::GetLanguageInfo(i);

        if (!info)
            continue;

        wxString fullCode = info->CanonicalName;
        wxString code = fullCode.Left(2);
        wxString name = info->Description;
        bool found = false;

        if (localLanguageName[fullCode] != _T("")) {
            name = localLanguageName[fullCode];
        }
        if (localLanguageName[code] != _T("")) {
            name = localLanguageName[code];
        }

        if (fullCode.Length() < 2)
            continue;

        if (TranslationExists(pathList, fullCode)) {
            tempCodes.Add(fullCode);
            tempNames.Add(name);
            found = true;
        }

        if (code != lastCode && code != fullCode) {
            if (TranslationExists(pathList, code)) {
                tempCodes.Add(code);
                tempNames.Add(name);
                found = true;
            }

            if (code == _T("en") && !found) {
                tempCodes.Add(code);
                tempNames.Add(name);
                found = true;
            }
        }

        lastCode = code;
    }

    // Sort

    unsigned int j;
    for(j=0; j<tempNames.GetCount(); j++)
        reverseHash[tempNames[j]] = tempCodes[j];

    tempNames.Sort();

    for(j=0; j<tempNames.GetCount(); j++) {
        langNames.Add(tempNames[j]);
        langCodes.Add(reverseHash[tempNames[j]]);
    }
}

