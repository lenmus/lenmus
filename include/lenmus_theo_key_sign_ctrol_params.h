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

#ifndef __LENMUS_THEO_KEY_SIGN_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_THEO_KEY_SIGN_CTROL_PARAMS_H__

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
class TheoKeySignCtrolParams : public ExerciseParams
{
public:
    TheoKeySignCtrolParams(EBookCtrolOptions* pConstrains);
    ~TheoKeySignCtrolParams();

protected:
    void do_final_settings();
    void process(ImoParamInfo* pParam);

};



TheoKeySignCtrolParams::TheoKeySignCtrolParams(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{

    // html object window attributes
    m_nWindowStyle = nStyle;

    // create constraints object (construtor initilizes it with default values for attributes)
    m_pConstrains = new TheoKeySignConstrains(_T("TheoKeys"));
    m_pOptions = m_pConstrains;

}


TheoKeySignCtrolParams::~TheoKeySignCtrolParams()
{
    //Constrains will be deleted by the Ctrol. DO NOT DELETE IT HERE
    //if (m_pConstrains) delete m_pConstrains;

}

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
    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.MakeUpper();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute

    // max_accidentals        num (0..7)
    if ( sName == _T("MAX_ACCIDENTALS") ) {
        wxString sAccidentals = tag.GetParam(_T("VALUE"));
        long nAccidentals;
        bool fOK = sAccidentals.ToLong(&nAccidentals);
        if (!fOK || nAccidentals < 0 || nAccidentals > 7) {
            LogError( wxString::Format(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values: numeric, 0..7"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() ));
        }
        else {
            m_pConstrains->SetMaxAccidentals((int)nAccidentals);
        }
    }

    // problem_type        DeduceKey | WriteKey | Both                 [Both]
    else if ( sName == _T("PROBLEM_TYPE") ) {
        wxString sProblem = tag.GetParam(_T("VALUE"));
        sProblem.MakeUpper();
        if (sProblem == _T("DEDUCEKEY"))
            m_pConstrains->SetProblemType( eIdentifyKeySignature );
        else if (sProblem == _T("WRITEKEY"))
            m_pConstrains->SetProblemType( eWriteKeySignature );
        else if (sProblem == _T("BOTH"))
            m_pConstrains->SetProblemType( eBothKeySignProblems );
        else
            LogError(wxString::Format(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values: DeduceKey | WriteKey | Both"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() ));
    }

    // clef        G | F4 | F3 | C4 | C3 | C2 | C1
    else if ( sName == _T("CLEF") )
    {
        wxString sClef = tag.GetParam(_T("VALUE"));
        EClefExercise nClef = lmE_Sol;        //default value
        m_sParamErrors += ParseClef(tag.GetParam(_T("VALUE")), tag.GetAllParams(),
                                      &nClef);
        m_pConstrains->SetClef(nClef, true);
    }
    //    wxString sClef = tag.GetParam(_T("VALUE"));
    //    EClefExercise nClef = LDPNameToClef(sClef);
    //    if (nClef != -1)
    //        m_pConstrains->SetClef(nClef, true);
    //    else
    //        LogError(wxString::Format(
    //            _T("Invalid param value in:\n<param %s >\n")
    //            _T("Invalid value = %s \n")
    //            _T("Acceptable values: G | F4 | F3 | C4 | C3 | C2 | C1"),
    //            tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() ));
    //}

    // mode         Major | Minor | Both                        [Both]
    else if ( sName == _T("MODE") ) {
        wxString sProblem = tag.GetParam(_T("VALUE"));
        sProblem.MakeUpper();
        if (sProblem == _T("MAJOR"))
            m_pConstrains->SetScaleMode( eMajorMode );
        else if (sProblem == _T("MINOR"))
            m_pConstrains->SetScaleMode( eMinorMode );
        else if (sProblem == _T("BOTH"))
            m_pConstrains->SetScaleMode( eMayorAndMinorModes );
        else
            LogError(wxString::Format(
                _T("Invalid param value in:\n<param %s >\n")
                _T("Invalid value = %s \n")
                _T("Acceptable values: Major | Minor | Both"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() ));
    }

    // Unknown param
    else
        ExerciseParams::AddParam(tag);

}


}   // namespace lenmus

#endif  // __LENMUS_THEO_KEY_SIGN_CTROL_PARAMS_H__
