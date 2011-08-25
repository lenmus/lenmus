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

#ifndef __LENMUS_EAR_COMPARE_INTV_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_EAR_COMPARE_INTV_CTROL_PARAMS_H__

//lenmus
#include "lenmus_exercise_params.h"

#include "lenmus_theo_intervals_constrains.h"
//#include "lenmus_exercise_params.h"
//#include "../ldp_parser/AuxString.h"
//#include "lenmus_params_parser.h"
//

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
    void do_final_settings();
    void process(ImoParamInfo* pParam);
};



EarCompareIntvCtrolParms::EarCompareIntvCtrolParms(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{
    m_nWindowStyle = nStyle;
    m_pConstrains = new EarIntervalsConstrains(_T("EarCompare"));
    m_pOptions = m_pConstrains;

    //force aural training mode
    m_pConstrains->SetTheoryMode(false);

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

    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.MakeUpper();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute

    // max_interval    num         default: 8
    if ( sName == _T("MAX_INTERVAL") ) {
        //wxString sAccidentals = tag.GetParam(_T("VALUE"));
        //sAccidentals.MakeUpper();
        //if (sAccidentals == _T("NONE")) {
        //    m_pConstrains->SetAccidentals(false);
        //    m_pConstrains->SetDoubleAccidentals(false);
        //}
        //else if (sAccidentals == _T("SIMPLE"))
        //    m_pConstrains->SetAccidentals(true);
        //else if (sAccidentals == _T("DOUBLE"))
        //    m_pConstrains->SetDoubleAccidentals(true);
        //else
        //    LogError(wxString::Format( wxGetTranslation(
        //        _T("Invalid param value in:\n<param %s >\n")
        //        _T("Invalid value = %s \n")
        //        _T("Acceptable values: none | simple | double") ),
        //        tag.GetAllParams(), tag.GetParam(_T("VALUE")) ));
    }

    // Unknown param
    else
        ExerciseParams::process(pParam);

}


}   // namespace lenmus

#endif  // __LENMUS_EAR_COMPARE_INTV_CTROL_PARAMS_H__


