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

#ifndef __LENMUS_IDFY_CHORD_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_IDFY_CHORD_CTROL_PARAMS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_params.h"
#include "lenmus_chords_constrains.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

namespace lenmus
{

//---------------------------------------------------------------------------------------
// This class pack all parameters to set up a chord identification exercise,
// either for ear training or for theory.
// The settings must be read/setup by the IdfyChordCtrol object.

//---------------------------------------------------------------------------------------
class IdfyChordCtrolParams : public ExerciseParams
{
public:
    IdfyChordCtrolParams(EBookCtrolOptions* pConstrains);
    ~IdfyChordCtrolParams();

protected:
    void do_final_settings() override;
    void process(ImoParamInfo* pParam) override;

};


//---------------------------------------------------------------------------------------
IdfyChordCtrolParams::IdfyChordCtrolParams(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{
    m_sParamErrors = "";    //no errors
}

//---------------------------------------------------------------------------------------
IdfyChordCtrolParams::~IdfyChordCtrolParams()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != "")
        delete m_pConstrains;
}

//---------------------------------------------------------------------------------------
void IdfyChordCtrolParams::process(ImoParamInfo* pParam)
{
    /*! @page IdfyChordCtrolParams
        @verbatim

        Params for IdfyChordCtrol - html object type="Application/LenMusIdfyChord"

        keys        Keyword "all", "allMajor", "allMinor" or a list of allowed
                    key signatures, i.e.: "Do,Fas". Default: all

        chords      Keyword "all" or a list of allowed chords:
                        m-minor, M-major, a-augmented, d-diminished, s-suspended
                        T-triad, dom-dominant, hd-half diminished

                        triads: mT, MT, aT, dT, s4, s2
                        sevenths: m7, M7, a7, d7, mM7, aM7 dom7, hd7
                        sixths: m6, M6, a6

                    Default: "mT,MT,aT,dT,m7,M7"

        mode        'theory' | 'earTraining'  Keyword indicating type of exercise

        play_mode*   'chord | ascending | descending' allowed play modes. Default: chord

        show_key     '0 | 1' Default: 0 (do not display key signature)

        inversions  '0 | 1' Default: 0 (do not allow inversions)

        control_settings    Value="[key for storing the settings]"
                            By coding this param it is forced the inclusion of
                            the 'settings' link. Its value will be used
                            as the key for saving the user settings.

        Example:
        ------------------------------------
        <object type="Application/LenMusIdfyChord" width="100%" height="300" border="0">
            <param name="mode" value="earTraining">
            <param name="chords" value="mT,MT,aT,dT,m7,M7,dom7">
            <param name="keys" value="all">
        </object>

        @endverbatim

    */

    ChordConstrains* pConstrains
        = dynamic_cast<ChordConstrains*>( m_pConstrains );

    string& name = pParam->get_name();
    string& value = pParam->get_value();

    // allow inversions
    if (name == "inversions")
    {
        int nValue;
        bool fOK = pParam->get_value_as_int(&nValue);
        if (!fOK)
            error_invalid_param(name, value, "1 | 0");
        else
            pConstrains->SetInversionsAllowed( nValue != 0 );
    }

    // show Key signature
    else if (name == "show_key")
    {
        int nValue;
        bool fOK = pParam->get_value_as_int(&nValue);
        if (!fOK)
            error_invalid_param(name, value, "1 | 0");
        else
            pConstrains->SetDisplayKey( nValue != 0 );
    }

    // play mode
    else if ( name == "play_mode")
    {
        if (value == "chord")
            pConstrains->SetModeAllowed(0, true);
        else if (value == "ascending")
            pConstrains->SetModeAllowed(1, true);
        else if (value == "descending")
            pConstrains->SetModeAllowed(2, true);
        else
            error_invalid_param(name, value, "chord | ascending | descending");
    }

    // chords      Keyword "all" or a list of allowed chords:
    else if (name == "chords")
        parse_chords(value, pConstrains->GetValidChords());

    //keys        keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
    else if (name == "keys")
        parse_keys(value, pConstrains->GetKeyConstrains());

    // Unknown param
    else
        ExerciseParams::process(pParam);
}

//---------------------------------------------------------------------------------------
void IdfyChordCtrolParams::do_final_settings()
{
    ChordConstrains* pConstrains
        = dynamic_cast<ChordConstrains*>( m_pConstrains );

    // ensure that at least a play mode is selected
    bool fModeSpecified = false;
    for (int i=0; i < 3; i++)
    {
        fModeSpecified = fModeSpecified || pConstrains->IsModeAllowed(i);
        if (fModeSpecified)
            break;
    }
    if (!fModeSpecified)
        pConstrains->SetModeAllowed(0, true /*harmonic*/ );
}


}   // namespace lenmus

#endif  // __LENMUS_IDFY_CHORD_CTROL_PARAMS_H__
