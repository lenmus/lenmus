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

#ifndef __LENMUS_THEO_INTERVALS_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_THEO_INTERVALS_CTROL_PARAMS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_params.h"
#include "lenmus_theo_intervals_constrains.h"
#include "lenmus_exercise_params.h"


namespace lenmus
{

//---------------------------------------------------------------------------------------
// This class pack all parameters to set up a Theo Intervals Identification exercise,
// The settings must be read/setup by the TheoIntervalsCtrol object.
class TheoIntervalsCtrolParams : public ExerciseParams
{
public:
    TheoIntervalsCtrolParams(EBookCtrolOptions* pConstrains);
    ~TheoIntervalsCtrolParams();


protected:
    void do_final_settings();
    void process(ImoParamInfo* pParam) override;

};


//=======================================================================================
// Implementation
//=======================================================================================
TheoIntervalsCtrolParams::TheoIntervalsCtrolParams(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{
}


//---------------------------------------------------------------------------------------
TheoIntervalsCtrolParams::~TheoIntervalsCtrolParams()
{
}

//---------------------------------------------------------------------------------------
void TheoIntervalsCtrolParams::process(ImoParamInfo* pParam)
{
    // problem_level     0 | 1 | 2 | 3                                  [2]
    // accidentals       none | simple | double                         [none]
    // problem_type      DeduceInterval | BuildInterval
    // clef*             G | F4 | F3 | C4 | C3 | C2 | C1                [G]
    // control_settings  Value="[key for storing the settings]"
    //                      By coding this param it is forced the inclusion of
    //                      the 'settings' link. Its value will be used
    //                      as the key for saving the user settings.

    TheoIntervalsConstrains* pConstrains
        = dynamic_cast<TheoIntervalsConstrains*>( m_pConstrains );

    string& name = pParam->get_name();
    string& value = pParam->get_value();


    // accidentals        none | simple | double
    if (name == "accidentals")
    {
        if (value == "none")
        {
            pConstrains->SetAccidentals(false);
            pConstrains->SetDoubleAccidentals(false);
        }
        else if (value == "simple")
            pConstrains->SetAccidentals(true);
        else if (value == "double")
            pConstrains->SetDoubleAccidentals(true);
        else
            error_invalid_param(name, value, "none | simple | double");
    }

    //problem_type    DeduceInterval | BuildInterval
    else if ( name == "problem_type")
    {
        if (value == "DeduceInterval")
            pConstrains->SetProblemType( TheoIntervalsConstrains::k_deduce_interval );
        else if (value == "BuildInterval")
            pConstrains->SetProblemType( TheoIntervalsConstrains::k_build_interval );
        else
            error_invalid_param(name, value, "DeduceInterval | BuildInterval");
    }

    //problem_level     0 | 1 | 2 | 3
    else if ( name == "problem_level")
    {
        int nLevel;
        bool fOK = pParam->get_value_as_int(&nLevel);
        if (!fOK || nLevel < 0 || nLevel > 3)
            error_invalid_param(name, value, "0 | 1 | 2 | 3");
        else
            pConstrains->SetProblemLevel( nLevel );
    }

    // clef        G | F4 | F3 | C4 | C3 | C2 | C1
    else if ( name == "clef")
    {
        EClef nClef = k_clef_G2;        //default value
        parse_clef(value, &nClef);
        pConstrains->SetClef(nClef, true);
    }

    // Unknown param
    else
        ExerciseParams::process(pParam);

}

//---------------------------------------------------------------------------------------
void TheoIntervalsCtrolParams::do_final_settings()
{
    TheoIntervalsConstrains* pConstrains
        = dynamic_cast<TheoIntervalsConstrains*>( m_pConstrains );

    // ensure that at least a Clef is selected
    bool fClefSpecified = false;
    for (int i = k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++)
    {
        fClefSpecified = fClefSpecified || pConstrains->IsValidClef((EClef)i);
        if (fClefSpecified) break;
    }
    if (!fClefSpecified) {
        pConstrains->SetClef(k_clef_G2, true);
    }
}


}   // namespace lenmus

#endif  // __LENMUS_THEO_INTERVALS_CTROL_PARAMS_H__
