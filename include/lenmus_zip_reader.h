//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

#ifndef __LENMUS_ZIP_READER_H__
#define __LENMUS_ZIP_READER_H__

//lenmus
#include "lenmus_standard_header.h"

//lomse
#include <lomse_reader.h>
using namespace lomse;

//wxWidgets
#include <wx/filesys.h>
class wxFileSystem;


namespace lenmus
{

//---------------------------------------------------------------------------------------
// LdpZipReader: a reader for zip files
class LdpZipReader : public LdpReader
{
protected:
    wxFileSystem*   m_FS;
    wxFSFile*       m_file;
    bool            m_repeating_last_char;
    wxString        m_content;
    stringstream    m_stream;
    LdpTextReader*  m_pTextReader;
    const std::string m_locator;

public:
    LdpZipReader(const string& filename);
    virtual ~LdpZipReader();

	char get_next_char() { return m_pTextReader->get_next_char(); }
    void repeat_last_char() { return m_pTextReader->repeat_last_char(); }
    bool is_ready() { return m_pTextReader->is_ready(); }
	bool end_of_data() { return m_pTextReader->end_of_data(); }
    int get_line_number() { return m_pTextReader->get_line_number(); }
    string get_locator() { return m_locator; }

protected:
    void read_file(const wxFSFile& file);
    void read_string(wxString& str, wxInputStream* s, wxMBConv& conv);

};


}   //namespace lenmus

#endif      // __LENMUS_ZIP_READER_H__
