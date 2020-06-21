//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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

#include "lenmus_zip_reader.h"

#include "lenmus_string.h"

//wxWidgets
#include <wx/filesys.h>

#include <iostream>
#include <sstream>
using namespace std;

namespace lenmus
{

//=======================================================================================
// LdpZipReader implementation
//=======================================================================================
LdpZipReader::LdpZipReader(const string& filename)
    : LdpReader()
    , m_FS( LENMUS_NEW wxFileSystem() )
    , m_repeating_last_char(false)
    , m_locator(filename)
{
    //filename format is "<full path to lmb file>#zip:<lms file to read>"
    //Win: "C:\lenmus\locale\books\en\TheoryHarmony.lmb#zip:TheoryHarmony_cover.lms"
    //Linux: "/data/lenmus/locale/books/en/TheoryHarmony.lmb#zip:TheoryHarmony_cover.lms"

    m_file = m_FS->OpenFile( to_wx_string(filename) );
    read_file(*m_file);
    m_pTextReader = LENMUS_NEW LdpTextReader( to_std_string(m_content) );
}

//---------------------------------------------------------------------------------------
LdpZipReader::~LdpZipReader()
{
    delete m_FS;
    delete m_file;
    delete m_pTextReader;
}

//---------------------------------------------------------------------------------------
void LdpZipReader::read_file(const wxFSFile& file)
{
    wxInputStream *s = file.GetStream();

    if (s == nullptr)
    {
        wxLogError(_("Cannot open HTML document: %s"), file.GetLocation().wx_str());
        m_content = "";
        return;
    }

    //convert input file to wchar_t here in Unicode mode
    wxCSConv conv("utf-8");
    read_string(m_content, s, conv);
}

//---------------------------------------------------------------------------------------
void LdpZipReader::read_string(wxString& str, wxInputStream* s, wxMBConv& conv)
{
    // utility function: read a wxString from a wxInputStream

    size_t streamSize = s->GetSize();

    if (streamSize == ~(size_t)0)
    {
        const size_t bufSize = 4095;
        char buffer[bufSize+1];
        size_t lastRead;

        do
        {
            s->Read(buffer, bufSize);
            lastRead = s->LastRead();
            buffer[lastRead] = 0;
            str.Append(wxString(buffer, conv));
        }
        while (lastRead == bufSize);
    }
    else
    {
        char* src = LENMUS_NEW char[streamSize+1];
        s->Read(src, streamSize);
        src[streamSize] = 0;
        str = wxString(src, conv);
        delete[] src;
    }
}


}   //namespace lenmus
