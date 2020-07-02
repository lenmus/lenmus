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

#ifndef __LENMUS_DICTATION_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_DICTATION_CTROL_PARAMS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_params.h"
#include "lenmus_music_reading_constrains.h"
#include "lenmus_scores_constrains.h"

//lomse
#include <lomse_ldp_analyser.h>
using namespace lomse;

namespace lenmus
{

//---------------------------------------------------------------------------------------
/*
    Four different ways for choosing the settings:

    1. By level and lesson: Useful for what? It was necessary with old non-html
        organization. Now for each lesson there will be an html page and the
        score object will have all necessary information for that lesson. So method
        3 will replace this one.

    2. Personal settings dialog: the user selects the rithmic patterns to generate.
        Valid only to generate simple repetitive patterns. Composer must have
        knowledge for completing bars with rest or single notes.

    3. Pararameters in <dynamic> object: the parameters must include all necesary data.
        Score generation based only on received fragments.

    GeneralExercises will be based only on method 2 in coherence with its purpose
    (free exercises, user customizable). For practising an specific level/lesson the
    user must choose the corresponding book/page. In these pages it would be allowed
    to customize settings by adding/removing fragments or changing clefs and note ranges.

*/

//---------------------------------------------------------------------------------------
// This class pack all parameters to set up a Music Reading exercise.
// The contained ScoreConstrains object has the constraints for the 'ByProgram'
// settings mode (default mode). For other modes ('UserSettings' and 'ReadingNotes')
// the settings must be read/setup by the TheoMusicReadingCtrol object.

class DictationCtrolParams : public ExerciseParams
{
public:
    DictationCtrolParams(EBookCtrolOptions* pConstrains);
    ~DictationCtrolParams();

    // Options for Music Reading Ctrol
    void SetControlPlay(bool fValue, wxString sLabels = "")
        {
            fPlayCtrol = fValue;
            if (sLabels != "")
                set_labels(sLabels, &sPlayLabel, &sStopPlayLabel);
        }
    void SetControlSolfa(bool fValue, wxString sLabels = "")
        {
            fSolfaCtrol = fValue;
            if (sLabels != "")
                set_labels(sLabels, &sSolfaLabel, &sStopSolfaLabel);
        }


    bool        fPlayCtrol;             //Instert "Play" link
    wxString    sPlayLabel;             //label for "Play" link
    wxString    sStopPlayLabel;         //label for "Stop playing" link

    bool        fSolfaCtrol;            //insert a "Sol-fa" link
    wxString    sSolfaLabel;            //label for "Sol-fa" link
    wxString    sStopSolfaLabel;        //label for "Stop sol-fa" link

    bool        fBorder;


protected:
    void do_final_settings() override;
    void process(ImoParamInfo* pParam) override;

    bool AnalyzeClef(wxString sLine);
    bool AnalyzeTime(wxString sLine);
    bool AnalyzeKeys(wxString sLine);
    bool AnalyzeFragments(wxString sLine);

    void set_labels(wxString& sLabel, wxString* pStart, wxString* pStop);


    ScoreConstrains* m_pScoreConstrains;
};


//---------------------------------------------------------------------------------------
DictationCtrolParams::DictationCtrolParams(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{
    m_sParamErrors = "";    //no errors

    // control options initializations
    fPlayCtrol = false;
    fSolfaCtrol = false;
    fBorder = false;
    sPlayLabel = _("Play");
    sStopPlayLabel = _("Stop");
    sSolfaLabel = _("Read");
    sStopSolfaLabel = _("Stop");
}

//---------------------------------------------------------------------------------------
DictationCtrolParams::~DictationCtrolParams()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != "")
        delete m_pConstrains;
}

//---------------------------------------------------------------------------------------
void DictationCtrolParams::process(ImoParamInfo* pParam)
{
    /*! @page DictationCtrolParams
        @verbatim

        Params for all classes derived from DictationCtrol:
            MelodicDictationCtrol, HarmonicDictationCtrol & RhythmicDictationCtrol


        params to include controls (all are optional):
        -------------------------------------------------

        control_settings    Value="[key for storing the settings]"
                            This param forces to include the 'settings' link. The
                            key will be used both as the key for saving the user settings
                            and as a tag to select the Setting Dialog options to allow.

        control_go_back     theme id, i.e.: "ch0"

        metronome       Set a fixed metronome rate to play the exercise
                        Value="MM number". Default: user value in metronome control


        params for controlling exercise behaviour:
        ---------------------------------------------------------

        show_key        No values. Specifying this param forces to include key signature
                        in user score. Default value: do not include key signature.

        show_time       No values. Specifying this param forces to include time signature
                        in user score. Default value: do not include time signature.

        num_dictation_fragments     Value="1, 2 or 4". The dictation will be splitted
                                    into dictation fragments. This parameter specifies
                                    how many. Value 1 means 'Do not split'.

		midi_instrument Value: number 0..255. MIDI instrument to use for playing problem
                        score. If not specified, Acoustic Grand Piano (midi instr. 0) will
                        be used.

		tonal_context   For melodic and harmonic dictations, specifies the tonal context
                        to be played as dictation introduction. Values:
                        A4 - Play an A4 note (three times)
                        scale - Play the scale for key signature plus chords I-IV-V-I


        params to drive the score composer (melodic & rhythmic dictations):
        --------------------------------------------------------------------

        fragment*   one param for each fragment to use

        clef*       one param for each allowed clef. It includes the pitch scope.

        time        a list of allowed time signatures, i.e.: "68,98,128"

        key         keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"

        max_interval    a number indicating the maximum allowed interval for two consecutive notes
                        Default: 4

        TODO: To be defined: parameters for melodic dictation


        params for harmonic dictation
        ---------------------------------

        TODO: To be defined



        Example:
        ------------------------------------

        <exercise type="RhythmicDictation">
            <control_go_back>ch0</control_go_back>
            <clef>G;e4;g4</clef>
            <key>C</key>
            <key>a</key>
            <time>24</time>
            <fragment>24;(n * q)(n * q),(n * q)(n * q)</fragment>
            <fragment>24;(n * q)(n * q),(n * q)(r q)</fragment>
            <fragment>24;(n * q)(r q),(n * q)(r q)</fragment>
            <fragment>24;(n * q)(r q),(r q)(n * q)</fragment>
            <fragment>24;(n * q)(n * q),(r q)(n * q)</fragment>
        </exercise>

        @endverbatim

    */

    DictationConstrains* pConstrains
        = dynamic_cast<DictationConstrains*>( m_pConstrains );
    m_pScoreConstrains = pConstrains->get_score_constrains();

    string& name = pParam->get_name();
    string& value = pParam->get_value();

    // metronome
    if (name == "metronome")
    {
        wxString sMM = to_wx_string(value);
        long nMM;
        bool fOK = sMM.ToLong(&nMM);
        if (!fOK || nMM < 0 )
            error_invalid_param(name, value, "numeric, greater than 0");
        else
            m_pScoreConstrains->SetMetronomeMM(nMM);
    }

    //fragments   the list of fragmens to use
    else if (name == "fragment")
    {
        wxString sFragments = to_wx_string(value);
        AnalyzeFragments(sFragments);
    }

    //clef*        one param for each allowed clef. It includes the pitch scope.
    else if (name == "clef")
    {
        wxString sClef = to_wx_string(value);
        if (AnalyzeClef(sClef))
            error_invalid_param(name, value,
                "clef values: G | F4 | F3 | C4 | C3 | C2 | C1 \n"
                "note pitch: c0 - c9");
    }

    //time        a list of allowed time signatures, i.e.: "68,98,128"
    else if (name == "time")
    {
        wxString sTime = to_wx_string(value);
        if (AnalyzeTime(sTime))
            error_invalid_param(name, value, "list of time signatures");
    }

    //key         keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"
    else if (name == "key")
    {
        wxString sKeys = to_wx_string(value);
        if (AnalyzeKeys(sKeys))
            error_invalid_param(name, value, "list of key signatures or keyword 'all'.");
    }

    //max_interval    a number
    else if (name == "max_interval")
    {
        wxString sMaxInterval = to_wx_string(value);
        long nMaxInterval;
        bool fOK = sMaxInterval.ToLong(&nMaxInterval);
        if (!fOK || nMaxInterval < 0 )
            error_invalid_param(name, value, "numeric, greater than 0");
        else
            m_pScoreConstrains->SetMaxInterval((int)nMaxInterval);
    }

    // Unknown param
    else
        ExerciseParams::process(pParam);
}

//---------------------------------------------------------------------------------------
/// returns true if error
bool DictationCtrolParams::AnalyzeClef(wxString sLine)
{
    //i.e.: "G;c4;c5"

    //get clef
    int iSemicolon = sLine.Find(";");
    string value = to_std_string( sLine.Left(iSemicolon) );
    EClef nClef;
    parse_clef(value, &nClef);

    //get lower scope
    sLine = sLine.substr(iSemicolon+1);
    iSemicolon = sLine.Find(";");
    wxString sLowerScope = sLine.Left(iSemicolon);

    //get upper scope
    wxString sUpperScope = sLine.substr(iSemicolon + 1);

    //Update information for this clef
    ClefConstrains* pClefs = m_pScoreConstrains->GetClefConstrains();
    pClefs->SetValid(nClef, true);
    pClefs->SetLowerPitch(nClef, sLowerScope);
    pClefs->SetUpperPitch(nClef, sUpperScope);

    return false;   //no error
}

//---------------------------------------------------------------------------------------
/// returns true if error
bool DictationCtrolParams::AnalyzeTime(wxString sLine)
{
    //i.e.: "98" , "38,68,128" , "24,44"

    //build time signatures constraints object
    TimeSignConstrains* pTimeSigns = LENMUS_NEW TimeSignConstrains();
    if (pTimeSigns->SetConstrains(sLine))
    {
        delete pTimeSigns;
        return true;
    }

    //Replace information about allowed time signatures
    TimeSignConstrains* pOldTimeSigns = m_pScoreConstrains->GetTimeSignConstrains();
    for (int i=k_min_time_signature; i <= k_max_time_signature; i++)
    {
        ETimeSignature nTime = (ETimeSignature)i;
        pOldTimeSigns->SetValid(nTime, pTimeSigns->IsValid(nTime) );
    }

    delete pTimeSigns;
    return false;   //no error
}

//---------------------------------------------------------------------------------------
/// returns true if error
bool DictationCtrolParams::AnalyzeKeys(wxString sLine)
{
    //i.e.: "all" , "C,G,F4"

    if (sLine == "all")
    {
        // allow all key signatures
        KeyConstrains* pKeys = m_pScoreConstrains->GetKeyConstrains();
        for (int i=0; i <= k_key_F; i++)
            pKeys->SetValid((EKeySignature)i, true);
    }

    else
    {
        //analyze and set key signatures
        KeyConstrains* pKeys = m_pScoreConstrains->GetKeyConstrains();

        //loop to get all keys
        int iColon;
        wxString sKey;
        EKeySignature nKey;
        while (sLine != "")
        {
            //get key
            iColon = sLine.Find(",");
            if (iColon != -1) {
                sKey = sLine.Left(iColon);
                sLine = sLine.substr(iColon + 1);      //skip the colon
            }
            else {
                sKey = sLine;
                sLine = "";
            }
            nKey = (EKeySignature)LdpAnalyser::ldp_name_to_key_type(to_std_string(sKey));
            if (nKey == (EKeySignature)-1) return true;
            pKeys->SetValid(nKey, true);
        }
    }

    return false;   //no error

}

//---------------------------------------------------------------------------------------
/// returns true if error
bool DictationCtrolParams::AnalyzeFragments(wxString sLine)
{
    //i.e.: "24,34;(s c)(n * n)(n * s g+)(n * s )(n * c g-)(s c)"

    //get time signatures
    int iSemicolon = sLine.Find(";");
    wxString sTimeSign = sLine.Left(iSemicolon);

    //get fragment
    wxString sFragment = sLine.substr(iSemicolon + 1);   //skip the semicolon and take the rest

    //build time signatures constraints object
    TimeSignConstrains* pTimeSigns = LENMUS_NEW TimeSignConstrains();
    if (pTimeSigns->SetConstrains(sTimeSign))
    {
        m_sParamErrors += to_std_string(
            wxString::Format("Error in fragment. Invalid time signature list '%s'\nIn fragment: '%s'\n",
                             sTimeSign.wx_str(), sFragment.wx_str())
        );
        delete pTimeSigns;
        return true;
    }

    ////TODO 5.0
    //// verify fragment to avoid program failures
    ////TODO strong verification
    ////lmLDPNode* pRoot = parserLDP.ParseText(sFragment);
    ////if (! pRoot) {
    //if (!parserLDP.ParenthesisMatch(sFragment))
    //{
    //    m_sParamErrors += to_std_string(
    //        wxString::Format(_T("Invalid fragment. Unmatched parenthesis: '%s'\n"),
    //                         sFragment.wx_str())
    //    );
    //    return true;
    // }
    if (sFragment.Find("(g (") != -1 || sFragment.Find("(g(") != -1)
    {
        m_sParamErrors += to_std_string(
            wxString::Format("Invalid fragment. Old G syntax: '%s'\n",
                             sFragment.wx_str())
        );
        delete pTimeSigns;
        return true;
    }

    // build the entry
    m_pScoreConstrains->AddFragment(pTimeSigns, sFragment);

    return false;   //no error

}

//---------------------------------------------------------------------------------------
void DictationCtrolParams::do_final_settings()
{
    //The <object> tag has been read. If param 'control_settings' has been specified
    // configuration values must be loaded from the specified section key

    m_pScoreConstrains->SetSection( m_pConstrains->GetSection() );
    m_pScoreConstrains->load_settings();
}


}   // namespace lenmus

#endif  // __LENMUS_DICTATION_CTROL_PARAMS_H__
