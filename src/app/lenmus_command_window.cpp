//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2013 LenMus project
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

//lenmus
#include "lenmus_command_window.h"
#include "lenmus_standard_header.h"

//#include "lenmus_paths.h"
#include "lenmus_main_frame.h"
//#include "lenmus_string.h"

//wxWidgets
#ifndef WX_PRECOMP
    #include <wx/utils.h>
    #include <wx/textctrl.h>
    #include <wx/intl.h>
    #include <wx/sizer.h>
#endif
#include <wx/msgdlg.h>

//other
#include <boost/program_options.hpp>
#include <boost/program_options/positional_options.hpp>
namespace po = boost::program_options;

#include <iterator>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <string>
#include <vector>
#include <sstream>
using namespace std;


namespace lenmus
{

//=======================================================================================
// CommandWindow implementation
// http://stackoverflow.com/questions/4747944/output-readonly-input-editable-with-wx-textctrl
//=======================================================================================

//IDs for controls
const int k_id_text = wxNewId();

//constants for controlling buffer space
const size_t k_buffer_size = 16000;
const size_t k_buffer_margin = 100;

//---------------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CommandWindow, wxPanel)
    EVT_TEXT_ENTER(k_id_text, CommandWindow::on_enter)
END_EVENT_TABLE()


//---------------------------------------------------------------------------------------
CommandWindow::CommandWindow(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
    wxBeginBusyCursor();

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    m_output = new wxTextCtrl(this, k_id_text, wxEmptyString,
                                wxDefaultPosition, wxSize(-1, 60),
                                wxTE_MULTILINE | wxTE_READONLY );

    topsizer->Add(m_output, wxSizerFlags(1).Expand().FixedMinSize());

    m_input = new wxTextCtrl(this, k_id_text, wxEmptyString,
                                wxDefaultPosition, wxSize(-1, 60),
                                wxTE_MULTILINE | wxTE_PROCESS_ENTER);

    topsizer->Add(m_input, wxSizerFlags(0).Expand().FixedMinSize());

    SetAutoLayout( true );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    m_input->SetSelection(-1, -1);
    m_input->SetFocus();
    m_input->SetBackgroundColour(*wxBLACK);
    m_input->SetForegroundColour(*wxGREEN);

    m_output->SetBackgroundColour(*wxBLACK);
    m_output->SetForegroundColour(*wxGREEN);

    wxEndBusyCursor();
}

//---------------------------------------------------------------------------------------
void CommandWindow::on_enter(wxCommandEvent& WXUNUSED(event))
{
    wxCommandEvent myEvent(LM_EVT_EDIT_COMMAND, k_id_edit_command);
    wxString cmd = get_value();
    if (!cmd.empty())
    {
        myEvent.SetString( cmd );
        ::wxPostEvent(this, myEvent);
    }
    m_input->SetValue(wxEmptyString);
}

//---------------------------------------------------------------------------------------
wxString CommandWindow::get_value() const
{
    return m_input->GetValue();
}

//---------------------------------------------------------------------------------------
void CommandWindow::display_message(const wxString& msg)
{
    ensure_there_is_space_for(msg);
    m_output->SetDefaultStyle(wxTextAttr(*wxCYAN));
    m_output->AppendText(_T("\n"));
    append_text(msg);
}

//---------------------------------------------------------------------------------------
void CommandWindow::display_command(const wxString& msg)
{
    ensure_there_is_space_for(msg);
    m_output->SetDefaultStyle(wxTextAttr(*wxGREEN));
    m_output->AppendText(_T("\n> "));
    append_text(msg);
}

//---------------------------------------------------------------------------------------
void CommandWindow::display_error(const wxString& msg)
{
    ensure_there_is_space_for(msg);
    m_output->SetDefaultStyle(wxTextAttr(*wxRED));
    m_output->AppendText(_T("\nERROR: "));
    append_text(msg);
}

//---------------------------------------------------------------------------------------
void CommandWindow::ensure_there_is_space_for(const wxString& msg)
{
    wxString s = m_output->GetValue();
    size_t current = s.size();
    size_t needed = msg.size() + k_buffer_margin;
    if (current + needed > k_buffer_size)
    {
        if (needed > k_buffer_size)
        {
            size_t keep = needed - k_buffer_size;
            s = msg.substr(msg.size()-keep, keep);
            m_output->SetValue(s);
        }
        else
        {
            size_t keep = k_buffer_size - k_buffer_margin;
            s = msg.substr(msg.size()-keep, keep);
            m_output->SetValue(s);
        }
    }
}

//---------------------------------------------------------------------------------------
void CommandWindow::append_text(const wxString& msg)
{
    size_t needed = msg.size() + k_buffer_margin;
    if (needed > k_buffer_size)
    {
        size_t keep = needed - k_buffer_size;
        m_output->SetValue( msg.substr(msg.size()-keep, keep) );
    }
    else
    {
        m_output->AppendText(msg);
    }
}



//=======================================================================================
// CommandParser implementation
//=======================================================================================
CommandParser::CommandParser()
{
}

//---------------------------------------------------------------------------------------
DocCommand* CommandParser::create_command(const string& cmd)
{
    // http://stackoverflow.com/questions/865668/parse-command-line-arguments
    //
    //      boost program_options
    // http://www.radmangames.com/programming/how-to-use-boost-program_options
    // http://www.boost.org/doc/libs/1_53_0/doc/html/program_options/overview.html
    // http://wakeupnuts.blogspot.com.es/2013/01/parse-command-string-using.html
    //
    // Boost requires a library. And command line parameters depends on command type.
    // Perhaps it is better to define line format for each command. Use a header only
    // library:
    // http://optionparser.sourceforge.net/index.html
    //


    m_error = "";
    const string errorMsg = "Unknown command. Ignored.";

        //first, try simple commands:

    //cursor
    if (cmd.at(0) == 'c')       //C++11 front() == 'c')
    {
        //cursor: move prev
        if (cmd == "c -" || cmd == "c-")
            return LENMUS_NEW CmdCursor(CmdCursor::k_move_prev);

        //cursor: move next
        if (cmd == "c +" || cmd == "c+")
            return LENMUS_NEW CmdCursor(CmdCursor::k_move_next);

        //cursor: enter
        if (cmd == "c in" || cmd == "cin")
            return LENMUS_NEW CmdCursor(CmdCursor::k_enter);

        //cursor: exit
        if (cmd == "c out" || cmd == "cout")
            return LENMUS_NEW CmdCursor(CmdCursor::k_exit);

        m_error = errorMsg;
        return NULL;
    }

    //insertion
    else if (cmd.at(0) == 'i')
    {
        if (cmd.compare(0,2, "i ") == 0)
        {
            if (cmd.compare(2,3, "so ") == 0)
            {
                //staff obj
                return LENMUS_NEW CmdInsertStaffObj( cmd.substr(5) );
            }
            if (cmd.compare(2,4, "mso ") == 0)
            {
                //staff obj
                return LENMUS_NEW CmdInsertManyStaffObjs( cmd.substr(6) );
            }
            else
            {
                //paragraph
                ostringstream txt;
                txt << "<para>" << cmd.substr(2) << "</para>";
                return LENMUS_NEW CmdInsertBlockLevelObj(txt.str());
            }
        }
        else if (cmd.compare(0,3, "ih ") == 0)
        {
            //<section level='1' style='eBook_heading_1'>
            ostringstream txt;
            txt << "<section level='1'>" << cmd.substr(3) << "</section>";
            return LENMUS_NEW CmdInsertBlockLevelObj(txt.str());
        }
        else if (cmd.compare(0,2, "is") == 0)
        {
            //score
            ostringstream txt;
            txt << "<ldpmusic>"
                << "(score (vers 1.6) (instrument (musicData )))"
                << "</ldpmusic>";
            return LENMUS_NEW CmdInsertBlockLevelObj(txt.str());
        }

        m_error = errorMsg;
        return NULL;
    }

    //deletion
    else if (cmd.at(0) == 'd')
    {
        if (cmd == "d so")
        {
            //block level obj
            return LENMUS_NEW CmdDeleteStaffObj();
        }
        else if (cmd.compare(0,2, "d ") == 0)
        {
            //block level obj
            return LENMUS_NEW CmdDeleteBlockLevelObj();
        }

        m_error = errorMsg;
        return NULL;
    }

    m_error = errorMsg;
    return NULL;
}


}   //namespace lenmus
