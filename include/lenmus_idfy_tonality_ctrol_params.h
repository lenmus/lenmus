//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2020 LenMus project
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

#ifndef __LENMUS_IDFY_TONALITY_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_IDFY_TONALITY_CTROL_PARAMS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_params.h"
#include "lenmus_tonality_constrains.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
// This class pack all parameters to set up a Tonality identification exercise,
// The settings must be read/setup by the IdfyTonalityCtrol object.

class IdfyTonalityCtrolParams : public ExerciseParams
{
public:
    IdfyTonalityCtrolParams(EBookCtrolOptions* pConstrains);
    ~IdfyTonalityCtrolParams();

protected:
    void do_final_settings() override;
    void process(ImoParamInfo* pParam) override;
    void ParseAnswerButtons(const string& value,  TonalityConstrains* pConstrains);

};



//---------------------------------------------------------------------------------------
IdfyTonalityCtrolParams::IdfyTonalityCtrolParams(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{
    m_sParamErrors = "";    //no errors
}

//---------------------------------------------------------------------------------------
IdfyTonalityCtrolParams::~IdfyTonalityCtrolParams()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != "")
        delete m_pConstrains;
}

//---------------------------------------------------------------------------------------
void IdfyTonalityCtrolParams::process(ImoParamInfo* pParam)
{
    //    Params for IdfyTonalityCtrol - html object type="Application/LenMusIdfyTonality"
    //
    //    keys        Keyword "all", "allMajor", "allMinor" or a list of allowed
    //                key signatures, i.e.: "Do,Fas". Default: all
    //
    //    tonality_buttons    Either "majorMinor" or "keySignatures".
    //                        Default: "keySignatures"
    //
    //    control_settings    Value="[key for storing the settings]"
    //                        By coding this param it is forced the inclusion of
    //                        the 'settings' link. Its value will be used
    //                        as the key for saving the user settings.
    //
    //    Example:
    //    ------------------------------------
    //    <exercise type="IdfyTonality" width="100%" height="300" border="0">
    //        <control_settings>EarIdfyTonality<control_settings/>
    //        <keys>all</keys>
    //    </exercise>


    TonalityConstrains* pConstrains = dynamic_cast<TonalityConstrains*>( m_pConstrains );

    string& name = pParam->get_name();
    string& value = pParam->get_value();

        // Process the parameters

    //keys        keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
    if (name == "keys")
        parse_keys(value, pConstrains->GetKeyConstrains());

    //tonality_buttons
    else if (name == "tonality_buttons")
        ParseAnswerButtons(value, pConstrains);

    // Unknown param
    else
        ExerciseParams::process(pParam);
}

//---------------------------------------------------------------------------------------
void IdfyTonalityCtrolParams::do_final_settings()
{
}

//---------------------------------------------------------------------------------------
void IdfyTonalityCtrolParams::ParseAnswerButtons(const string& value,
                                                 TonalityConstrains* pConstrains)
{
    // tonality_buttons    Either "majorMinor" or "keySignatures".

    if (value != "majorMinor")
        pConstrains->UseMajorMinorButtons(true);
    else if (value != "keySignatures")
        pConstrains->UseMajorMinorButtons(false);
    else
        error_invalid_param("tonality_buttons", value,
                            "either 'majorMinor' or 'keySignatures'.");
}


}   // namespace lenmus

#endif  // __LENMUS_IDFY_TONALITY_CTROL_PARAMS_H__
