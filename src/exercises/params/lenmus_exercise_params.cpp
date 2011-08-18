//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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
#include "lenmus_exercise_params.h"

#include "lenmus_constrains.h"
#include "lenmus_string.h"

////wxWidgets
//#include <wx/wxprec.h>
//#include <wx/html/winpars.h>
//#include <wx/html/htmlwin.h>

//other
#include <boost/format.hpp>

namespace lenmus
{

//=======================================================================================
// EBookCtrolParams implementation
//=======================================================================================
EBookCtrolParams::EBookCtrolParams(EBookCtrolOptions* pConstrains)
    : m_pConstrains(pConstrains)
{
}

//---------------------------------------------------------------------------------------
void EBookCtrolParams::process_params(std::list<ImoParamInfo*>& params)
{
    std::list<ImoParamInfo*>::iterator it;
    for (it = params.begin(); it != params.end(); ++it)
    {
        process(*it);
    }
    do_final_settings();
}

//---------------------------------------------------------------------------------------
void EBookCtrolParams::LogError(const string& sMsg)
{
    //TODO do something else with the error
    wxLogMessage( to_wx_string(sMsg) );
}

//---------------------------------------------------------------------------------------
void EBookCtrolParams::error_invalid_param(const string& name, const string& value,
                                           const string& acceptableValues)
{
    string msg;
    if (acceptableValues.empty())
    {
        msg = str( boost::format(
                "Invalid param:\n(param %s \"%s\")\n")
                % name.c_str() % value.c_str() );
    }
    else
    {
        msg = str( boost::format(
                "Invalid param:\n(param %s \"%s\")\n"
                "Acceptable values:  '%s'\n")
                % name.c_str() % value.c_str() % acceptableValues.c_str() );
    }
    LogError(msg);
    m_sParamErrors += msg;
}

//---------------------------------------------------------------------------------------
void EBookCtrolParams::process(ImoParamInfo* pParam)
{
    // Parse common parameters to all controls (EBookCtrolOptions)
    //
    // control_play        Include 'play' link. Default: do not include it.
    //                     Value="play label|stop playing label". i.e.: "Play|Stop"
    //                     Stop label is optional. Default labels: "Play|Stop"
    //
    // control_settings    Value="[key for storing the settings]"
    //                     By coding this param it is forced the inclusion of
    //                     the 'settings' link. Its value will be used
    //                     as the key for saving the user settings.
    // control_go_back     Include a 'Go back to theory' link. Value is an URL,
    //                     i.e.: "v2_L2_MusicReading_203.htm"
    //


    EBookCtrolOptions* pConstrains
        = dynamic_cast<EBookCtrolOptions*>( m_pConstrains );

    string& name = pParam->get_name();
    string& value = pParam->get_value();

    // control_settings
    if ( name == "control_settings")
    {
        pConstrains->SetSettingsLink(true);
        pConstrains->set_section(value);
    }

    // "Go back to theory" link
    else if ( name == "control_go_back")
        pConstrains->set_go_back_link(value);

    // control_play
    else if ( name == "control_play")
        pConstrains->SetPlayLink(true);

    // Unknown param
    else
    {
        LogError( str( boost::format("EBookCtrolParams. Unknown param: %s >\n")
            % name.c_str() ) );
    }

}



//=======================================================================================
// ExerciseParams implementation
//=======================================================================================
ExerciseParams::ExerciseParams(EBookCtrolOptions* pConstrains)
    : EBookCtrolParams(pConstrains)
{
}

//---------------------------------------------------------------------------------------
void ExerciseParams::process(ImoParamInfo* pParam)
{
    // Parse common parameters to all ExerciseParams
    //
    // mode                'theory' | 'earTraining'  Keyword indicating type of exercise
    //

    ExerciseOptions* pConstrains
        = dynamic_cast<ExerciseOptions*>( m_pConstrains );

    string& name = pParam->get_name();
    string& value = pParam->get_value();

    // mode        'theory | earTraining'  Keyword indicating type of exercise
    if (name == "mode")
    {
        if (value == "theory")
            pConstrains->set_theory_mode(true);
        else if (value == "earTraining")
            pConstrains->set_theory_mode(false);
        else
            error_invalid_param(name, value, "theory | earTraining");
    }

    // Unknown param
    else
        EBookCtrolParams::process(pParam);

}


}   // namespace lenmus
