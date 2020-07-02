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

#ifndef __LENMUS_EAR_COMPARE_INTV_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_EAR_COMPARE_INTV_CTROL_PARAMS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_params.h"
#include "lenmus_ear_intervals_constrains.h"

namespace lenmus
{

//---------------------------------------------------------------------------------------
// This class pack all parameters to set up a TheoKeySign Identification exercise,
// The settings must be read/setup by the TheoKeySignCtrol object.
//===============================================================================================
class EarCompareIntvCtrolParms : public ExerciseParams
{
public:
    EarCompareIntvCtrolParms(EBookCtrolOptions* pConstrains);
    ~EarCompareIntvCtrolParms();

protected:
    void do_final_settings() override;
    void process(ImoParamInfo* pParam) override;
};



EarCompareIntvCtrolParms::EarCompareIntvCtrolParms(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{
}


EarCompareIntvCtrolParms::~EarCompareIntvCtrolParms()
{
    //Constrains will be deleted by the Ctrol. DO NOT DELETE IT HERE
    //if (m_pConstrains) delete m_pConstrains;

}


void EarCompareIntvCtrolParms::process(ImoParamInfo* pParam)
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
        //TODO 5.0: Was this commented out for 5.0? Doesn't match comment!
        //wxString sAccidentals = pParam->GetParam("VALUE");
        //sAccidentals.MakeUpper();
        //if (sAccidentals == "NONE") {
        //    m_pConstrains->SetAccidentals(false);
        //    m_pConstrains->SetDoubleAccidentals(false);
        //}
        //else if (sAccidentals == "SIMPLE")
        //    m_pConstrains->SetAccidentals(true);
        //else if (sAccidentals == "DOUBLE")
        //    m_pConstrains->SetDoubleAccidentals(true);
        //else
        //    LogError(wxString::Format( wxGetTranslation(
        //        _T("Invalid param value in:\n<param %s >\n")
        //        _T("Invalid value = %s \n")
        //        "Acceptable values: none | simple | double" ),
        //        pParam->GetAllParams(), pParam->GetParam("VALUE") ));
    }

    // Unknown param
    else
        ExerciseParams::process(pParam);

}

void EarCompareIntvCtrolParms::do_final_settings()
{
    EarIntervalsConstrains* pConstrains
        = dynamic_cast<EarIntervalsConstrains*>( m_pConstrains );

    // ensure that at least an interval is selected
    bool fIntervalSpecified = false;
    for (int i=0; i < 25; i++) {
        fIntervalSpecified = fIntervalSpecified || pConstrains->IsIntervalAllowed(i);
        if (fIntervalSpecified) break;
    }
    if (!fIntervalSpecified) {
        pConstrains->SetIntervalAllowed(0, true);
    }
}


}   // namespace lenmus

#endif  // __LENMUS_EAR_COMPARE_INTV_CTROL_PARAMS_H__


