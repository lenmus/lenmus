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

#ifndef __LENMUS_THEO_MUSIC_READING_CTROL_PARAMS_H__        //to avoid nested includes
#define __LENMUS_THEO_MUSIC_READING_CTROL_PARAMS_H__

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
//! enum assigning name to the score generation settings source.
/*
    Four different ways for choosing the settings:

    1. By level and lesson: Useful for what? It was necessary with old non-html
        organization. Now for each lesson there will be an html page and the
        score object will have all necessary information for that lesson. So method
        3 will replace this one.

    2. Personal settings dialog: the user selects the rithmic patterns to generate.
        Valid only to generate simple repetitive patterns. Composer must have
        knowledge for completing bars with rest or single notes.

    3. Pararameters in html object: the parameters must include all necesary data.
        Score generation based only on received fragments.

    4. Reading notes exercises: the parameters are fixed for this exercise (parameters
        the html object) but certain values (clefs, notes range) would need
        user settings

    Free exercises page will be based only on method 2 in coherence with its purpose
    (free exercises, user customizable). For practising an specific level/lesson the
    user must choose the corresponding book/page. In these pages it would be allowed
    to customize settings by adding/removing fragments or changing clefs and note ranges.

*/

//---------------------------------------------------------------------------------------
// This class pack all parameters to set up a Music Reading exercise.
// The contained ScoreConstrains object has the constraints for the 'ByProgram'
// settings mode (default mode). For other modes ('UserSettings' and 'ReadingNotes')
// the settings must be read/setup by the TheoMusicReadingCtrol object.

class TheoMusicReadingCtrolParams : public ExerciseParams
{
public:
    TheoMusicReadingCtrolParams(EBookCtrolOptions* pConstrains);
    ~TheoMusicReadingCtrolParams();

    // Options for Music Reading Ctrol
    inline void SetControlPlay(bool fValue, wxString sLabels = "") {
        fPlayCtrol = fValue;
        if (sLabels != "")
            set_labels(sLabels, &sPlayLabel, &sStopPlayLabel);
    }
    inline void SetControlSolfa(bool fValue, wxString sLabels = "") {
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
    void do_final_settings();
    void process(ImoParamInfo* pParam) override;

    bool AnalyzeClef(wxString sLine);
    bool AnalyzeTime(wxString sLine);
    bool AnalyzeKeys(wxString sLine);
    bool AnalyzeFragments(wxString sLine);

    void set_labels(wxString& sLabel, wxString* pStart, wxString* pStop);

    ScoreConstrains* m_pScoreConstrains;
};


//---------------------------------------------------------------------------------------
TheoMusicReadingCtrolParams::TheoMusicReadingCtrolParams(EBookCtrolOptions* pConstrains)
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
TheoMusicReadingCtrolParams::~TheoMusicReadingCtrolParams()
{
    //Constrains and options will be deleted by the Ctrol. DO NOT DELETE THEM HERE
    //IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != "")
        delete m_pConstrains;
}

//---------------------------------------------------------------------------------------
void TheoMusicReadingCtrolParams::process(ImoParamInfo* pParam)
{
    /*! @page MusicReadingCtrolParams
        @verbatim

        Params for ScoreCtrol - exercise type "TheoMusicReading""


        optional params to include controls:
        --------------------------------------

        control_play    Include 'play' link. Default: do not include it.
                        Value="play label|stop playing label". i.e.: "Play|Stop" Stop label
                        is optional. Default labels: "Play|Stop"

        control_solfa   Include 'solfa' link. Default: do not include it.
                        Value="music read label|stop music reading label". i.e.:
                        "Play|Stop". Stop label is optional.
                        Default labels: "Read|Stop"

        control_settings    Value="[key for storing the settings]"
                            This param forces to include the 'settings' link. The
                            key will be used both as the key for saving the user settings
                            and as a tag to select the Setting Dialog options to allow.

        control_go_back    URL, i.e.: "v2_L2_MusicReading_203.htm"

        metronome       Set a fixed metronome rate to play this piece of music
                        Value="MM number". Default: user value in metronome control


        params to set up the score composer (ScoreConstrains)
        ---------------------------------------------------------

        fragment*           One param for each fragment to use

        clef*               One param for each allowed clef. It includes the pitch scope.

        time                A list of allowed time signatures, i.e.: "68,98,128"

        key                 Keyword "all" or a list of allowed key signatures, i.e.: "Do,Fas"

        max_interval        A number indicating the maximum allowed interval for two consecutive notes
                            Default: 4

        pickup_measure      Keyword "always | never | random". By default, pickup measures will be
                            randomly generated (pickup_measure = random)

        pickup_min_note     Minimum note to use in anacrusis measure. It is the LDP character
                            for note type "q | e | s". Default value is "e" (eighth note).

        pickup_no_fraction  Do not start anacrusis in beat fraction. By default, pickup
                            measure is allowed to start in beat fraction


        Example:
        ------------------------------------

        <exercise type="TheoMusicReading">
            <control_go_back>v2-33</control_go_back>
            <control_play />
            <control_solfa />
            <clef>G;a3;c6</clef>
            <clef>F4;a2;e4</clef>
            <key>C</key>
            <key>a</key>
            <key>G</key>
            <key>e</key>
            <time>68,98</time>
            <fragment>68,98;(n * n)(n * c +l)(g (n * c)(n * c)(n * c))</fragment>
	        <fragment>68,98;(n * c)(n * n +l)(g (n * c)(n * c)(n * c))</fragment>
	        <fragment>68,98;(n * n)(n * c)</fragment>
	        <fragment>68,98;(g (n * c)(n * c)(n * c))</fragment>
        </exercise>

        @endverbatim

    */

    MusicReadingConstrains* pConstrains
        = dynamic_cast<MusicReadingConstrains*>( m_pConstrains );
    m_pScoreConstrains = pConstrains->GetScoreConstrains();

    string& name = pParam->get_name();
    string& value = pParam->get_value();

    // control_solfa
    if (name == "control_solfa")
        pConstrains->SetControlSolfa(true, to_wx_string(value));

    // "pickup_measure": options for generating pickup measures
    else if (name == "pickup_measure")
    {
        if (value == "always")
            m_pScoreConstrains->allow_pickup_measure(k_pickup_always);
        else if (value == "never")
            m_pScoreConstrains->allow_pickup_measure(k_pickup_never);
        else if (value == "random")
            m_pScoreConstrains->allow_pickup_measure(k_pickup_random);
        else
            error_invalid_param(name, value, "always | never | random");
    }

    // "pickup_min_note": minimum note to use in anacrusis measure
    else if (name == "pickup_min_note")
    {
        if (value == "q")
            m_pScoreConstrains->set_pickup_min_note(k_quarter);
        else if (value == "e")
            m_pScoreConstrains->set_pickup_min_note(k_eighth);
        else if (value == "s")
            m_pScoreConstrains->set_pickup_min_note(k_16th);
        else
            error_invalid_param(name, value, "q | e | s");
    }

    // "pickup_no_fraction": do not start anacrusis in beat fraction
    else if (name == "pickup_no_fraction")
    {
        m_pScoreConstrains->allow_pickup_fraction(false);
    }

    // control_settings
    if ( name == "control_settings")
    {
        pConstrains->SetSettingsLink(true);
        pConstrains->set_section(value);
    }


    // metronome
    else if (name == "metronome")
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
bool TheoMusicReadingCtrolParams::AnalyzeClef(wxString sLine)
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
bool TheoMusicReadingCtrolParams::AnalyzeTime(wxString sLine)
{
    //i.e.: "98" , "38,68,128" , "24,44"

    //build time signatures constraints object
    TimeSignConstrains* pTimeSigns = LENMUS_NEW TimeSignConstrains();
    if (pTimeSigns->SetConstrains(sLine))
        return true;

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
bool TheoMusicReadingCtrolParams::AnalyzeKeys(wxString sLine)
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
bool TheoMusicReadingCtrolParams::AnalyzeFragments(wxString sLine)
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
        LOMSE_LOG_ERROR(m_sParamErrors);

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
        LOMSE_LOG_ERROR(m_sParamErrors);
        delete pTimeSigns;
        return true;
    }

    // build the entry
    m_pScoreConstrains->AddFragment(pTimeSigns, sFragment);

    return false;   //no error

}

//---------------------------------------------------------------------------------------
void TheoMusicReadingCtrolParams::do_final_settings()
{
    //The <object> tag has been read. If param 'control_settings' has been specified
    // configuration values must be loaded from the specified section key

    m_pScoreConstrains->SetSection( m_pConstrains->GetSection() );
    m_pScoreConstrains->load_settings();
}


}   // namespace lenmus

#endif  // __LENMUS_THEO_MUSIC_READING_CTROL_PARAMS_H__
