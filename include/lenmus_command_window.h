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

#ifndef __LENMUS_COMMAND_WINDOW_H__        //to avoid nested includes
#define __LENMUS_COMMAND_WINDOW_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_tokenizer.h"

//wxWidgets
#include <wx/panel.h>


//forward declarations
class wxTextCtrl;
class wxWindow;

namespace lenmus
{

//---------------------------------------------------------------------------------------
class CommandWindow : public wxPanel
{
protected:
    wxTextCtrl* m_input;
    wxTextCtrl* m_output;

public:
    CommandWindow(wxWindow* parent);

    void on_enter(wxCommandEvent& WXUNUSED(event));
    wxString get_value() const;
    void display_message(const wxString& msg);
    void display_command(const wxString& cmd);
    void display_error(const wxString& msg);

private:
    void ensure_there_is_space_for(const wxString& msg);
    void append_text(const wxString& msg);

    wxDECLARE_EVENT_TABLE();
};


}   // namespace lenmus


//=======================================================================================
//This code probably will be moved, in future, to another file
//=======================================================================================


//lomse
#include <lomse_command.h>
using namespace lomse;

//other
#include <string>
using namespace std;

namespace lenmus
{


//---------------------------------------------------------------------------------------
class CommandParser
{
protected:
    string m_error;     //last error msg
    Tokenizer m_tok;

public:
    CommandParser();

    DocCommand* create_command(const string& cmd);
    inline string get_last_error() { return m_error; }

protected:
    string parse_command(const string& cmd);
    inline bool more_tokens() { return !m_tok.at_end(); }
    inline string get_next_token() { ++m_tok; return *m_tok; }
    bool token_is_number();
    ImoId token_as_imoid();
    DocCommand* error_no_more_tokens();
    DocCommand* error_bad_syntax();

};


}   // namespace lenmus

#endif    // __LENMUS_COMMAND_WINDOW_H__
