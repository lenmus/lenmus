//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Lomse is copyrighted work (c) 2010-2020. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#include "lomse_font_storage.h"

#include "lomse_build_options.h"
#include "lomse_logger.h"

//std
#include <locale>           //to upper conversion
#include <cstdlib>          //getenv()
using namespace std;

namespace lomse
{

//=======================================================================================
// FontSelector::find_font implementation for Windows
//=======================================================================================
std::string FontSelector::find_font(const std::string& language,
                                    const std::string& fontFile,
                                    const std::string& name,
                                    bool fBold, bool fItalic)
{
    //Priority is given to font file.
    //For generic families (i.e.: sans, serif, monospace, ...) priority is given to
    //language

    //search in cache
    string key=language + fontFile + name + (fBold ? "1" : "0") + (fItalic ? "1" : "0");
    map<string, string>::iterator it = m_cache.find(key);
    if (it != m_cache.end())
        return it->second;

    //get Windows fonts path
    string fullpath = std::getenv("WINDIR");
    fullpath += "\\Fonts\\";
    LOMSE_LOG_INFO("Fonts path=%s", fullpath.c_str());

    //transform font name to capital letters for comparisons
    const locale& loc = locale();
    string fontname;
    for (string::value_type a : name)
        fontname += std::toupper(a, loc);

    //music font
    if (fontname == "BRAVURA")
    {
        fullpath = m_pLibScope->fonts_path();
        fullpath += "Bravura.otf";
        LOMSE_LOG_INFO("key=%s, Path=%s", key.c_str(), fullpath.c_str());
        m_cache.insert(make_pair(key, fullpath));
        return fullpath;
    }

    //Chinese fonts
    if (language == "zh_CN" || fontFile == "wqy-zenhei.ttc")
    {
        if (fBold && fItalic)
            fullpath += "msjhbd.ttc";
        else if (fBold)
            fullpath += "msjhbd.ttc";
        else if (fItalic)
            fullpath += "msjhl.ttc";
        else
            fullpath += "msjhl.ttc";
        LOMSE_LOG_INFO("key=%s, Path=%s", key.c_str(), fullpath.c_str());
        m_cache.insert(make_pair(key, fullpath));
        return fullpath;
    }

    //Latin fonts
    if (fontname == "LIBERATION SERIF" || fontname == "SERIF"
             || fontname == "TIMES NEW ROMAN")
    {
        if (fBold && fItalic)
            fullpath += "timesbi.ttf";
        else if (fBold)
            fullpath += "timesbd.ttf";
        else if (fItalic)
            fullpath += "timesi.ttf";
        else
            fullpath += "times.ttf";
        LOMSE_LOG_INFO("key=%s, Path=%s", key.c_str(), fullpath.c_str());
        m_cache.insert(make_pair(key, fullpath));
        return fullpath;
    }

    else if (fontname == "LIBERATION SANS" || fontname == "SANS-SERIF" || fontname == "SANS"
             || fontname == "HELVETICA")
    {
        if (fBold && fItalic)
            fullpath += "arialbi.ttf";
        else if (fBold)
            fullpath += "arialbd.ttf";
        else if (fItalic)
            fullpath += "ariali.ttf";
        else
            fullpath += "arial.ttf";
        LOMSE_LOG_INFO("key=%s, Path=%s", key.c_str(), fullpath.c_str());
        m_cache.insert(make_pair(key, fullpath));
        return fullpath;
    }

    //Other fonts: ask user program
    else
        fullpath = m_pLibScope->get_font(name, fBold, fItalic);
        LOMSE_LOG_INFO("key=%s, Path=%s", key.c_str(), fullpath.c_str());
        m_cache.insert(make_pair(key, fullpath));
        return fullpath;
}


}   //namespace lomse
