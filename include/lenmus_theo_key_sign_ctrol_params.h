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

#ifndef __LENMUS_THEO_KEY_SIGN_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_THEO_KEY_SIGN_CTROL_PARAMS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_params.h"
#include "lenmus_theo_intervals_constrains.h"
#include "lenmus_key_sign_constrains.h"


namespace lenmus
{

//=======================================================================================
// Class definition
// This class pack all parameters to set up a TheoKeySign Identification exercise,
// The settings must be read/setup by the TheoKeySignCtrol object.
//=======================================================================================
class TheoKeySignCtrolParams : public ExerciseParams
{
public:
    TheoKeySignCtrolParams(EBookCtrolOptions* pConstrains);
    ~TheoKeySignCtrolParams();

protected:
    void do_final_settings();
    void process(ImoParamInfo* pParam) override;

};


//=======================================================================================
// Implementation
//=======================================================================================
TheoKeySignCtrolParams::TheoKeySignCtrolParams(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{
}

//---------------------------------------------------------------------------------------
TheoKeySignCtrolParams::~TheoKeySignCtrolParams()
{
    //Constrains will be deleted by the Ctrol. DO NOT DELETE IT HERE
    //if (m_pConstrains) delete m_pConstrains;
}

//---------------------------------------------------------------------------------------
void TheoKeySignCtrolParams::process(ImoParamInfo* pParam)
{
    /*! @page KeySignParms
        @verbatim

        Params for TheoKeySignCtrolParams - html object type="Application/LenMusTheoKeySignatures"

        param name          value                                       default value
        -----------------------------------------------------------------------------
        max_accidentals     num (0..7)                                  [5]
        problem_type        DeduceKey | WriteKey | Both                 [Both]
        clef*               G | F4 | F3 | C4 | C3 | C2 | C1             [G]
        mode                Major | Minor | Both                        [Both]

        Example

        <object type="Application/LenMusTheoKeySignatures" width="100%" height="300" border="0">
            <param  name="max_accidentals" value="7" />
            <param  name="problem_type" value="both" />
            <param  name="clef" value="G" />
            <param  name="mode" value="both" />
        </object>


        @endverbatim
   */

    TheoKeySignConstrains* pConstrains
        = dynamic_cast<TheoKeySignConstrains*>( m_pConstrains );

    string& name = pParam->get_name();
    string& value = pParam->get_value();

    // max_accidentals        num (0..7)
    if ( name == "max_accidentals")
    {
        wxString sAccidentals = to_wx_string(value);
        long nAccidentals;
        bool fOK = sAccidentals.ToLong(&nAccidentals);
        if (!fOK || nAccidentals < 0 || nAccidentals > 7)
        {
            error_invalid_param(name, value, "numeric, 0..7");
        }
        else
            pConstrains->SetMaxAccidentals((int)nAccidentals);
    }

    // problem_type        DeduceKey | WriteKey | Both                 [Both]
    else if ( name == "problem_type")
    {
        if (value == "DeduceKey")
            pConstrains->SetProblemType( eIdentifyKeySignature );
        else if (value == "WriteKey")
            pConstrains->SetProblemType( eWriteKeySignature );
        else if (value == "Both")
            pConstrains->SetProblemType( eBothKeySignProblems );
        else
            error_invalid_param(name, value, "DeduceKey | WriteKey | Both");
    }

    // clef        G | F4 | F3 | C4 | C3 | C2 | C1
    else if (name == "clef")
    {
        EClef nClef = k_clef_G2;        //default value
        parse_clef(value, &nClef);
        pConstrains->SetClef(nClef, true);
    }

    // mode         Major | Minor | Both                        [Both]
    else if ( name == "mode" )
    {
        if (value == "Major")
            pConstrains->SetScaleMode( k_scale_major );
        else if (value == "Minor")
            pConstrains->SetScaleMode( k_scale_minor );
        else if (value == "Both")
            pConstrains->SetScaleMode( k_scale_both );
        else
            error_invalid_param(name, value, "Major | Minor | Both");
    }

    // Unknown param
    else
        ExerciseParams::process(pParam);

}

//---------------------------------------------------------------------------------------
void TheoKeySignCtrolParams::do_final_settings()
{
    TheoKeySignConstrains* pConstrains
        = dynamic_cast<TheoKeySignConstrains*>( m_pConstrains );

    // ensure that at least one Clef is selected
    bool fClefSpecified = false;
    for (int i=k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++)
    {
        fClefSpecified = fClefSpecified || pConstrains->IsValidClef((EClef)i);
        if (fClefSpecified) break;
    }
    if (!fClefSpecified)
        pConstrains->SetClef(k_clef_G2, true);
}


}   // namespace lenmus

#endif  // __LENMUS_THEO_KEY_SIGN_CTROL_PARAMS_H__
