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

#ifndef __LENMUS_EAR_INTERVALS_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_EAR_INTERVALS_CTROL_PARAMS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_params.h"
#include "lenmus_theo_intervals_constrains.h"


namespace lenmus
{

//=======================================================================================
class EarIntervalsCtrolParms : public ExerciseParams
{
public:
    EarIntervalsCtrolParms(EBookCtrolOptions* pConstrains);
    ~EarIntervalsCtrolParms();

protected:
    void do_final_settings() override;
    void process(ImoParamInfo* pParam) override;

};


//=======================================================================================
EarIntervalsCtrolParms::EarIntervalsCtrolParms(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{
}

//---------------------------------------------------------------------------------------
EarIntervalsCtrolParms::~EarIntervalsCtrolParms()
{
    //Constrains will be deleted by the Ctrol. DO NOT DELETE IT HERE
    //if (m_pConstrains) delete m_pConstrains;

}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrolParms::process(ImoParamInfo* pParam)
{
    /*
        max_interval    num         default: 8
    */

//    EarIntervalsConstrains* pConstrains
//        = dynamic_cast<EarIntervalsConstrains*>( m_pConstrains );

    string& name = pParam->get_name();
//    string& value = pParam->get_value();


    // max_interval    num         default: 8
    if (name == "max_interval")
    {
        //TODO: (Also in 4,2) check value and set constrains. where?
    }

    // Unknown param
    else
        ExerciseParams::process(pParam);

}

//---------------------------------------------------------------------------------------
void EarIntervalsCtrolParms::do_final_settings()
{
//    //TODO 5.0
//    TheoIntervalsConstrains* pConstrains
//        = dynamic_cast<TheoIntervalsConstrains*>( m_pConstrains );
//
//    // ensure that at least an interval is selected
//    bool fIntervalSpecified = false;
//    for (int i=0; i < 25; i++) {
//        fIntervalSpecified = fIntervalSpecified || pConstrains->IsIntervalAllowed(i);
//        if (fIntervalSpecified) break;
//    }
//    if (!fIntervalSpecified) {
//        pConstrains->SetIntervalAllowed(0, true);
//    }
}


}   // namespace lenmus

#endif  // __LENMUS_EAR_INTERVALS_CTROL_PARAMS_H__
