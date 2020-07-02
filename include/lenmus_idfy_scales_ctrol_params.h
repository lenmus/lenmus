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

#ifndef __LENMUS_IDFY_SCALES_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_IDFY_SCALES_CTROL_PARAMS_H__

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_params.h"
#include "lenmus_scales_constrains.h"


namespace lenmus
{


//---------------------------------------------------------------------------------------
// This class pack all parameters to set up a Scale Identification exercise,
// The settings must be read/setup by the IdfyScalesCtrol object.

class IdfyScalesCtrolParams : public ExerciseParams
{
public:
    IdfyScalesCtrolParams(EBookCtrolOptions* pConstrains);
    ~IdfyScalesCtrolParams();

protected:
    void do_final_settings() override;
    void process(ImoParamInfo* pParam) override;

};



//---------------------------------------------------------------------------------------
IdfyScalesCtrolParams::IdfyScalesCtrolParams(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{
    m_sParamErrors = "";    //no errors
}


//---------------------------------------------------------------------------------------
IdfyScalesCtrolParams::~IdfyScalesCtrolParams()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != "")
        delete m_pConstrains;
}

//---------------------------------------------------------------------------------------
void IdfyScalesCtrolParams::process(ImoParamInfo* pParam)
{
    /*! @page IdfyScalesCtrolParams
        @verbatim

        Params for IdfyChordCtrol - html object type="Application/LenMusIdfyChord"

        keys        Keyword "all", "allMajor", "allMinor" or a list of allowed
                    key signatures, i.e.: "C,F+". Default: all
                    AWARE: if major and minor scales selected, the chosen key signatures
                    will be automatically transformed into major/minor, even if it is
                    not specified in the keys list. For example, <keys>C</keys> will
                    transform C major into A minor for minor scales.

        scales      Keyword "all" or a list of allowed scales:
                        major: MN (natural), MH (harmonic), M3 (type III), MM (mixolydian)
                        minor: mN (natural), mM (melodic), mD (dorian), mH (harmonic)
                        medieval modes: Do (Dorian), Ph (Phrygian), Ly (Lydian),
                                        Mx (Mixolydian), Ae (Aeolian), Io (Ionian),
                                        Lo (Locrian)
                        other: Pm (Pentatonic minor), PM (Pentatonic Major), Bl (Blues)
                        non-tonal: WT (Whole Tones), Ch (Chromatic)


                    Default: "MN,mN,mH,mM"

        mode        'theory' | 'earTraining'  Keyword indicating type of exercise

        play_mode   'ascending | descending | both' allowed play modes. Default: ascending

        show_key     '0 | 1' Default: 0 (do not display key signature)

        control_settings    Value="[key for storing the settings]"
                            By coding this param it is forced the inclusion of
                            the 'settings' link. Its value will be used
                            as the key for saving the user settings.

        Example:
        ------------------------------------
        <exercise type="IdfyScales" width="100%" height="300" border="0">
            <control_go_back>th1_140_scales</control_go_back>
            <keys>all</keys>
            <scales>MN,mN</scales>
            <mode>theory</mode>
        </exercise>

    */

    ScalesConstrains* pConstrains
        = dynamic_cast<ScalesConstrains*>( m_pConstrains );

    string& name = pParam->get_name();
    string& value = pParam->get_value();

    // show Key signature
    if (name == "show_key")
    {
        int nValue;
        bool fOK = pParam->get_value_as_int(&nValue);
        if (!fOK)
            error_invalid_param(name, value, "1 | 0 \n");
        else
            pConstrains->SetDisplayKey( nValue != 0 );
    }

    // play mode
    else if (name == "play_mode")
    {
        if (value == "ascending")
            pConstrains->SetPlayMode(0);
        else if (value == "descending")
            pConstrains->SetPlayMode(1);
        else if (value == "both")
            pConstrains->SetPlayMode(2);
        else {
            error_invalid_param(name, value, "ascending | descending | both");
        }
    }

    // scales      Keyword "all" or a list of allowed scales:
    else if (name == "scales")
        parse_scales(value, pConstrains->GetValidScales());

    //keys        keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
    else if (name == "keys")
        parse_keys(value, pConstrains->GetKeyConstrains());

    // Unknown param
    else
        ExerciseParams::process(pParam);
}

//---------------------------------------------------------------------------------------
void IdfyScalesCtrolParams::do_final_settings()
{
    //Nothing to validate or initializate in m_pConstrains
}


}   // namespace lenmus

#endif  // __LENMUS_IDFY_SCALES_CTROL_PARAMS_H__
