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

//lenmus
#include "lenmus_exercise_params.h"

#include "lenmus_string.h"
#include "lenmus_chords_constrains.h"   //EChordType
#include "lenmus_scale.h"              //EScaleType, scale name conversion

//lomse
#include <lomse_score_utilities.h>
#include <lomse_internal_model.h>
#include <lomse_ldp_analyser.h>
#include <lomse_logger.h>
using namespace lomse;


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
    LOMSE_LOG_ERROR(sMsg);
}

//---------------------------------------------------------------------------------------
void EBookCtrolParams::error_invalid_param(const string& name, const string& value,
                                           const string& acceptableValues)
{
    stringstream msg;
    if (acceptableValues.empty())
    {
        msg << "Invalid param:" << endl << "(param " << name.c_str() << " \""
            << value.c_str() << "\")" << endl;
    }
    else
    {
        msg << "Invalid param:" << endl << "(param " << name.c_str() << " \""
            << value.c_str() << "\")" << endl << "Acceptable values:  '"
            << acceptableValues.c_str() << "'" << endl;
    }
    LogError(msg.str());
    m_sParamErrors += msg.str();
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
    //
    // control_go_back     Include a 'Go back to theory' link. Value is an URL,
    //                     i.e.: "v2_L2_MusicReading_203.htm"
    //
    // width               Value= string, logical units.
    //                     Minimum width for the exercise display.
    //
    // height              Value= string, logical units.
    //                     Minimum height for the exercise display.


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

    // width
    else if ( name == "width")
        pConstrains->set_width( get_float_value(value, 0.0f) );

    // height
    else if ( name == "height")
        pConstrains->set_height( get_float_value(value, 0.0f) );

    // Unknown param
    else
    {
        stringstream msg;
        msg << "EBookCtrolParams. Unknown param: " << name.c_str() << " >" << endl;
        LogError(msg.str());
    }

}

//---------------------------------------------------------------------------------------
float EBookCtrolParams::get_float_value(const string& value, float rDefault)
{
    float rNumber;
    std::istringstream iss(value);
    if ((iss >> std::dec >> rNumber).fail())
    {
        stringstream msg;
        msg << "EBookCtrolParams. Invalid value for float param: " << value.c_str()
            << " >" << endl;
        LogError(msg.str());
        return rDefault;
    }
    else
        return rNumber;
}

//---------------------------------------------------------------------------------------
void EBookCtrolParams::parse_keys(const string& value, KeyConstrains* pKeys)
{
    //keys        Keyword "all", "allMajor", "allMinor" or a list of allowed
    //            key signatures, i.e.: "C,c,A+,B-". Default: "all"

    bool fError = false;

    if (value == "all")
    {
        // allow all key signatures
        for (int i=0; i <= k_max_key; i++)
            pKeys->SetValid((EKeySignature)i, true);
    }

    else if (value == "allMajor")
    {
        // allow all major key signatures
        for (int i=k_min_major_key; i <= k_max_major_key; i++)
            pKeys->SetValid((EKeySignature)i, true);
    }

    else if (value == "allMinor")
    {
        // allow all minor key signatures
        for (int i=k_min_minor_key; i <= k_max_minor_key; i++)
            pKeys->SetValid((EKeySignature)i, true);
    }

    else
    {
        //disable all key signatures
        for (int i=0; i <= k_max_key; i++)
            pKeys->SetValid((EKeySignature)i, false);

        //loop to get all keys
        int iColon;
        wxString sKey;
        EKeySignature nKey;
        wxString sValue = to_wx_string(value);
        while (sValue != "")
        {
            //get key
            iColon = sValue.Find(",");
            if (iColon != -1)
            {
                sKey = sValue.Left(iColon);
                sValue = sValue.substr(iColon + 1);      //skip the colon
            }
            else
            {
                sKey = sValue;
                sValue = "";
            }
            nKey = (EKeySignature)LdpAnalyser::ldp_name_to_key_type( to_std_string(sKey) );
            if (nKey == k_key_undefined)
            {
                fError = true;
                break;
            }
            pKeys->SetValid(nKey, true);
        }
    }

    if (fError)
        error_invalid_param("keys", value, "list of key signatures or keywords 'all', 'allMajor', 'allMinor'.");
}

//---------------------------------------------------------------------------------------
void EBookCtrolParams::parse_chords(const string& value, bool* pfValidChords)
{
    //chords      Keyword "all" or a list of allowed chords:
    //                m-minor, M-major, a-augmented, d-diminished, s-suspended
    //                T-triad, dom-dominant, hd-half diminished

    //                triads: mT, MT, aT, dT, s4, s2
    //                sevenths: m7, M7, a7, d7, mM7, aM7 dom7, hd7
    //                sixths: m6, M6, a6

    bool fError = false;

    if (value == "all")
    {
        // allow all chords
        for (int i=0; i <= ect_Max; i++)
            *(pfValidChords+i) = true;
    }
    else
    {
        //loop to get allowed chords
        int iColon;
        wxString sChord;
        EChordType nType;
        wxString sValue = to_wx_string(value);
        while (sValue != "")
        {
            //get chord
            iColon = sValue.Find(",");
            if (iColon != -1)
            {
                sChord = sValue.Left(iColon);
                sValue = sValue.substr(iColon + 1);      //skip the colon
            }
            else
            {
                sChord = sValue;
                sValue = "";
            }
            nType = Chord::short_name_to_type(sChord);
            if (nType == (EChordType)-1)
            {
                fError = true;
                break;
            }
            *(pfValidChords + (int)nType) = true;
        }
    }

    if (fError)
        error_invalid_param("chords", value, "Keyword 'all' or a list of allowed chords.");
}

//---------------------------------------------------------------------------------------
void EBookCtrolParams::parse_scales(const string& value, bool* pfValidScales)
{
    //scales      Keyword "all" or a list of allowed scales:
    //              major: MN (natural), MH (harmonic), M3 (type III), MM (mixolydian)
    //              minor: mN (natural), mM (melodic), mD (dorian), mH (harmonic)
    //              medieval modes: Do (Dorian), Ph (Phrygian), Ly (Lydian),
    //                              Mx (Mixolydian), Ae (Aeolian), Io (Ionian),
    //                              Lo (Locrian)
    //              other: Pm (Pentatonic minor), PM (Pentatonic Major), Bl (Blues)
    //              non-tonal: WT (Whole Tones), Ch (Chromatic)
    //
    //
    //            Default: "MN,mN,mH,mM"

    bool fError = false;

    if (value == "all")
    {
        // allow all scales
        for (int i=0; i <= est_Max; i++)
            *(pfValidScales+i) = true;
    }
    else
    {
        //disable all scales
        for (int i=0; i <= est_Max; i++)
            *(pfValidScales+i) = false;

        //loop to get allowed chords
        wxString sValue = to_wx_string(value);
        while (sValue != "")
        {
            //get scale
            wxString sScale;
            int iColon = sValue.Find(",");
            if (iColon != -1)
            {
                sScale = sValue.Left(iColon);
                sValue = sValue.substr(iColon + 1);      //skip the colon
            }
            else
            {
                sScale = sValue;
                sValue = "";
            }
            EScaleType nType = Scale::short_name_to_type(sScale);
            if (nType == (EScaleType)-1)
            {
                fError = true;
                break;
            }
            *(pfValidScales + (int)nType) = true;
        }
    }

    if (fError)
        error_invalid_param("scales", value, "Keyword 'all' or a list of allowed scales.");
}

//---------------------------------------------------------------------------------------
void EBookCtrolParams::parse_clef(const string& value, EClef* pClef)
{
    // clef       'G | F4 | F3 | C4 | C3 | C2 | C1'

    if (value == "G")
        *pClef = k_clef_G2;
    else if (value == "F4" || value == "F")     //"F" backwards compatibility
        *pClef = k_clef_F4;
    else if (value == "F3")
        *pClef = k_clef_F3;
    else if (value == "C1")
        *pClef = k_clef_C1;
    else if (value == "C2")
        *pClef = k_clef_C2;
    else if (value == "C3")
        *pClef = k_clef_C3;
    else if (value == "C4")
        *pClef = k_clef_C4;
    else
        error_invalid_param("clef", value, "G | F4 | F3 | C4 | C3 | C2 | C1");
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
