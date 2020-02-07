//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2019 LenMus project
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

#include "lenmus_languages.h"
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/intl.h>
#include <wx/stattext.h>

#include <wx/hashmap.h>
WX_DECLARE_STRING_HASH_MAP(wxString, LangHash);


namespace lenmus
{


//---------------------------------------------------------------------------------------
bool TranslationExists(wxArrayString& pathList, wxString code)
{
    wxArrayString results;
    wxString sPattern = code;

    if (sPattern == "") return false;

//    //DBG: To have a place to put a break
//    if (sPattern == "es" ) {
//        int kk;
//        kk=23;
//    }

    wxString sFileName;
    bool fFound;
    for(unsigned int i=0; i < pathList.GetCount(); i++) {
        wxString path = pathList[i];

        wxDir oDir(path);
        if (!oDir.IsOpened()) continue;    //directory does not exists!
        fFound = oDir.GetFirst(&sFileName, sPattern, wxDIR_FILES | wxDIR_DIRS);
        while(fFound) {
            //wxLogMessage(wxString::Format("[Languages::TranslationExists] code=%s, sFileName=%s",
            //    code, sFileName));
            results.Add(sFileName);
            fFound = oDir.GetNext(&sFileName);
        }
    }
    return (results.GetCount() > 0);
}

//---------------------------------------------------------------------------------------
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
        if (fullCode.length() < 2)
            return "en";

        wxString code = fullCode.Left(2);
        unsigned int i;

        for(i=0; i<langCodes.GetCount(); i++) {
            if (langCodes[i] == fullCode)
                return fullCode;

            if (langCodes[i] == code)
                return code;
        }
    }

    return "en";
}

//---------------------------------------------------------------------------------------
void GetLanguages(wxArrayString &langCodes, wxArrayString &langNames)
{
    //Get the list of languages for existing LenMus translations.
    //Return a list of language codes (like "es", "fr", or "pt_BR")
    //and corresponding language names in original language, not in English,
    //like "Español" or "Português".

    //AWARE It is not possible to use string translation macro _() as locale could
    //not yet be set, as this code is used at first run.

    // supported languages table
    typedef struct lmLangDataStruct {
        wxString sLangCode;
        wxString sLangName;
    } lmLangData;

    //Languages currently supported by translators. Ordered by ISO code
    static const lmLangData tLanguages[] = {
        { "de",     "Deutsch" },                    //German
        { "el",     wxString("Ελληνικά", wxConvUTF8) }, //Greek
        { "en",     "English" },                    //English
        { "es",     wxString("Español", wxConvUTF8) },  //Spanish
        { "eu",     "Euskara" },                    //Basque
        { "fr",     wxString("Français", wxConvUTF8) }, //French
        { "gl_ES",  "Galego" },                     //Galician
        { "it",     "Italiano" },                   //Italian
        { "nl",     "Nederlands" },                 //Dutch
#if (0)     //Russian language not ready
        { "ru",     wxString("Русский", wxConvUTF8) },  //Russian
#endif
        { "tr",     wxString("Türkçe", wxConvUTF8) },   //Turkish
        { "zh_CN",  wxString("简体中文", wxConvUTF8) },  //simplified Chinese
    };

    for(int j=0; j < (int)(sizeof(tLanguages)/sizeof(lmLangData)); j++)
    {
        langNames.Add(tLanguages[j].sLangName);
        langCodes.Add(tLanguages[j].sLangCode);
    }
}


}   //namespace lenmus
