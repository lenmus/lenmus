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

#ifndef __LENMUS_IDFY_CADENCES_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_IDFY_CADENCES_CTROL_PARAMS_H__

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_params.h"
#include "lenmus_cadences_constrains.h"


namespace lenmus
{

//---------------------------------------------------------------------------------------
// This class pack all parameters to set up a Cadence identification exercise,
// The settings must be read/setup by the IdfyCadencesCtrol object.
class IdfyCadencesCtrolParams : public ExerciseParams
{
public:
    IdfyCadencesCtrolParams(EBookCtrolOptions* pConstrains);
    ~IdfyCadencesCtrolParams();

protected:
    void do_final_settings() override;
    void process(ImoParamInfo* pParam) override;
    void parse_answer_buttons(const string& value, bool* pfValidButtons);
    void parse_cadences(const string& value, bool* pfValidCadences);

};


//---------------------------------------------------------------------------------------
IdfyCadencesCtrolParams::IdfyCadencesCtrolParams(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{
    m_sParamErrors = "";    //no errors
}

//---------------------------------------------------------------------------------------
IdfyCadencesCtrolParams::~IdfyCadencesCtrolParams()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != "")
        delete m_pConstrains;
}

//---------------------------------------------------------------------------------------
void IdfyCadencesCtrolParams::process(ImoParamInfo* pParam)
{
    //    Params for lmIdfyCadenceCtrol - html object type="Application/LenMusIdfyChord"
    //
    //    keys        Keyword "all", "allMajor", "allMinor" or a list of allowed
    //                key signatures, i.e.: "Do,Fas". Default: all
    //
    //    cadences    Keywords "all", "all_perfect", "all_imperfect", "all_plagal",
    //                  "all_deceptive", "all_half", "all_terminal", "all_transient"
    //                  or a list of allowed cadences:
    //                  Perfect authentic:
    //                      V_I_Perfect, V7_I, Va5_I, Vd5_I
    //                  Plagal:
    //                      IV_I, IVm_I, IIc6_I, IImc6_I
    //                  Imperfect authentic:
	//                      V_I_Imperfect
    //                  Deceptive:
    //                      V_IV, V_IVm, V_VI, V_VIm, V_IIm, V_III, V_VII
    //                  Half cadences:
    //                      IImc6_V, IV_V, I_V, Ic64_V, IV6_V, II_V, IIdimc6_V, VdeVdim5c64_V
    //
    //                Default: "all"
    //
    //    mode        'theory' | 'earTraining'  Keyword indicating type of exercise
    //
    //    play_key     'A4 | tonic_chord' Default: 'tonic_chord'
    //
    //    cadence_buttons   list of answer buttons to display:
    //                 'terminal, transient, perfect, plagal, imperfect, deceptive, half'
    //
    //
    //    control_settings    Value="[key for storing the settings]"
    //                        By coding this param it is forced the inclusion of
    //                        the 'settings' link. Its value will be used
    //                        as the key for saving the user settings.

    //    Example:
    //    ------------------------------------
    //    <object type="Application/LenMus" class="IdfyScale" width="100%" height="300" border="0">
    //        <param name="mode" value="earTraining">
    //        <param name="scales" value="mT,MT,aT,dT,m7,M7,dom7">
    //        <param name="keys" value="all">
    //    </object>


    CadencesConstrains* pConstrains = dynamic_cast<CadencesConstrains*>( m_pConstrains );

    string& name = pParam->get_name();
    string& value = pParam->get_value();

        // Process the parameters

    // cadences      list of allowed cadences:
    if (name == "cadences")
        parse_cadences(value, pConstrains->GetValidCadences());

    // cadence_buttons      list of answer buttons to display
    else if (name == "cadence_buttons")
        parse_answer_buttons(value, pConstrains->GetValidButtons());

    //keys        keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
    else if (name == "keys")
        parse_keys(value, pConstrains->GetKeyConstrains());

    // Unknown param
    else
        ExerciseParams::process(pParam);
}

//---------------------------------------------------------------------------------------
void IdfyCadencesCtrolParams::parse_answer_buttons(const string& value,
                                                   bool* pfValidButtons)
{
    //    cadence_buttons   list of answer buttons to display:
    //                 'terminal, transient, perfect, plagal, imperfect, deceptive, half'

    bool fError = false;

    //disable all buttons
    for (long i=0; i < lm_eCadMaxButton; i++)
        *(pfValidButtons + i) = false;

    //loop to get allowed buttons
    ECadenceButtons nButton;
    int iColon;
    wxString sParamValue = to_wx_string(value);
    wxString sButton;
    while (sParamValue != "")
    {
        //get button
        iColon = sParamValue.Find(",");
        if (iColon != -1)
        {
            sButton = sParamValue.Left(iColon);
            sParamValue = sParamValue.substr(iColon + 1);      //skip the colon
        }
        else
        {
            sButton = sParamValue;
            sParamValue = "";
        }

        if (sButton == "terminal")
            nButton = lm_eCadButtonTerminal;
        else if (sButton == "transient")
            nButton = lm_eCadButtonTransient;
        else if (sButton == "perfect")
            nButton = lm_eCadButtonPerfect;
        else if (sButton == "plagal")
            nButton = lm_eCadButtonPlagal;
        else if (sButton == "imperfect")
            nButton = lm_eCadButtonImperfect;
        else if (sButton == "deceptive")
            nButton = lm_eCadButtonDeceptive;
        else if (sButton == "half")
            nButton = lm_eCadButtonHalf;
        else
        {
            fError = true;
            break;
        }
        *(pfValidButtons + (int)nButton) = true;
    }

    if (fError)
        error_invalid_param("cadence_buttons", value, "a list of allowed buttons.");
}

//---------------------------------------------------------------------------------------
void IdfyCadencesCtrolParams::parse_cadences(const string& value,
                                             bool* pfValidCadences)
{
    //    cadences    Keywords "all", "all_perfect", "all_imperfect", "all_plagal",
    //                  "all_deceptive", "all_half"
    //                  or a list of allowed cadences:
    //                  Perfect authentic:
    //                      V_I_Perfect, V7_I, Va5_I, Vd5_I
    //                  Plagal:
    //                      IV_I, IVm_I, IIc6_I, IImc6_I
    //                  Imperfect authentic:
	//                      V_I_Imperfect
    //                  Deceptive:
    //                      V_IV, V_IVm, V_VI, V_VIm, V_IIm, V_III, V_VII
    //                  Half cadences:
    //                      IImc6_V, IV_V, I_V, Ic64_V, IV6_V, II_V, IIdimc6_V, VdeVdim5c64_V
    //
    //                Default: "all"

    bool fError = false;

    // disable all cadences
    for (int i=0; i < k_cadence_max; i++)
        *(pfValidCadences+i) = false;

    //loop to get allowed cadences
    int iColon;
    wxString sParamValue = to_wx_string(value);
    wxString sCadence;
    ECadenceType nType;
    while (sParamValue != "")
    {
        //get cadence
        iColon = sParamValue.Find(",");
        if (iColon != -1)
        {
            sCadence = sParamValue.Left(iColon);
            sParamValue = sParamValue.substr(iColon + 1);      //skip the colon
        }
        else
        {
            sCadence = sParamValue;
            sParamValue = "";
        }

        //determine cadence
        if (sCadence == "all")
        {
            // allow all cadences
            for (int i=0; i < k_cadence_max; i++)
                *(pfValidCadences+i) = true;
        }
        else if (sCadence == "all_perfect")
        {
            // allow all Perfect cadences
            for (int i=k_cadence_perfect; i < k_cadence_last_perfect; i++)
                *(pfValidCadences+i) = true;
        }
        else if (sCadence == "all_plagal") {
            // allow all Plagal cadences
            for (int i=k_cadence_plagal; i < k_cadence_last_plagal; i++)
                *(pfValidCadences+i) = true;
        }
        else if (sCadence == "all_deceptive")
        {
            // allow all Deceptive cadences
            for (int i=k_cadence_deceptive; i < k_cadence_last_deceptive; i++)
                *(pfValidCadences+i) = true;
        }
        else if (sCadence == "all_half")
        {
            // allow all Half cadences
            for (int i=k_cadence_half; i < k_cadence_last_half; i++)
                *(pfValidCadences+i) = true;
        }
        else if (sCadence == "all_imperfect")
        {
            // allow all Imperfect cadences
            for (int i=k_cadence_imperfect; i < k_cadence_last_imperfect; i++)
                *(pfValidCadences+i) = true;
        }
        else
        {
            nType = Cadence::name_to_type(sCadence);
            if (nType == (ECadenceType)-1)
            {
                fError = true;
                break;
            }
            *(pfValidCadences + (int)nType) = true;
        }
    }

    if (fError)
        error_invalid_param("cadences", value, "Keywords 'all', 'all_xxxx' or a list of allowed cadences.");
}

//---------------------------------------------------------------------------------------
void IdfyCadencesCtrolParams::do_final_settings()
{
    //Nothing to validate or initializate in m_pConstrains
}


}   // namespace lenmus

#endif  // __LENMUS_IDFY_CADENCES_CTROL_PARAMS_H__
