//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

//lenmus
#include "lenmus_scores_constrains.h"
#include "lenmus_generators.h"
#include "lenmus_utilities.h"

//lomse
#include <lomse_internal_model.h>
#include <lomse_im_factory.h>
#include <lomse_document.h>
#include <lomse_staffobjs_table.h>
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>


namespace lenmus
{

//=======================================================================================
// ScoreConstrains implementation
//=======================================================================================
ScoreConstrains::ScoreConstrains(ApplicationScope& appScope)
    : m_appScope(appScope)
    , m_sSection("")
    , m_nMaxInterval(4)
    , m_nMM(0)              // zero means: no predefined setting
    , m_aFragmentsTable(appScope)
{
    //ScoreConstrains objetc is used in MusicReading exercise and Dictation exercise.
    //The exercise must provide all necessary values.
    //When a 'section key' is provided - by invoking method SetSection() - this object
    //values are loaded from configuration file (or with default values if the key
    //does not exists).
    //If no 'section key' is provided, the exercise must be configure with the
    //settings provided in the exercise params (Default values are no clef, key and
    //time signature authorized)
    //So, no initialization of this object is needed (as long as we verify that
    //all necessary params are included in the exercise)
}

//---------------------------------------------------------------------------------------
void ScoreConstrains::save_settings()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    if (m_sSection == "")
        return;

    //allowed clefs and notes range
    wxString sKey;
    for (int i = k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++)
    {
        sKey = wxString::Format("/Constrains/ScoreConstrains/%s/Clef%d",
                                m_sSection.wx_str(), i);
        pPrefs->Write(sKey, m_oClefs.IsValid( (EClef)i ));
        sKey = wxString::Format("/Constrains/ScoreConstrains/%s/MinNote%d",
                                m_sSection.wx_str(), i);
        pPrefs->Write(sKey, m_oClefs.GetLowerPitch( (EClef)i ));
        sKey = wxString::Format("/Constrains/ScoreConstrains/%s/MaxNote%d",
                                m_sSection.wx_str(), i);
        pPrefs->Write(sKey, m_oClefs.GetUpperPitch( (EClef)i ));
    }

    //max interval in two consecutive notes
    sKey = wxString::Format("/Constrains/ScoreConstrains/%s/MaxInterval",
                            m_sSection.wx_str());
    pPrefs->Write(sKey, (long)m_nMaxInterval);

    //flag to signal that this key has data
    sKey = wxString::Format("/Constrains/ScoreConstrains/%s/HasData",
                            m_sSection.wx_str());
    pPrefs->Write(sKey, true);

    //allowed time signatures
    for (int i = k_min_time_signature; i <= k_max_time_signature; i++)
    {
        sKey = wxString::Format("/Constrains/ScoreConstrains/%s/Time%d",
                                m_sSection.wx_str(), i);
        pPrefs->Write(sKey, m_oValidTimeSign.IsValid( (ETimeSignature)i ));
    }

    //allowed key signatures
    for (int i=k_min_key; i <= k_max_key; i++)
    {
        sKey = wxString::Format("/Constrains/ScoreConstrains/%s/KeySignature%d",
                                m_sSection.wx_str(), i);
        pPrefs->Write(sKey, m_oValidKeys.IsValid((EKeySignature)i) );
    }

    //TODO save remaining data: fragments
}

//---------------------------------------------------------------------------------------
void ScoreConstrains::load_settings()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

     //When arriving here, default values are already stored in this object. So we must check
     //if the section key is stored in the configuration file. If it is, then we have
     //to load config data from there; otherwise, do nothing as default values are in place.


    if (m_sSection == "")
    {
        //leave default values
        return;
    }

    //check if the section key is stored in the configuration file.
    wxString sKey = wxString::Format("/Constrains/ScoreConstrains/%s/HasData",
                                     m_sSection.wx_str());
    bool fHasData = false;
    pPrefs->Read(sKey, &fHasData, false);

    //If no data saved, return. Default values are already in place.
    if (!fHasData)
        return;


     //Load confiuration data from config file. Default values are meaningless
     //as they never should be used


    //allowed clefs and notes range
    //default values: only G clef allowed
    bool fValue;
    for (int i = k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++)
    {
        sKey = wxString::Format("/Constrains/ScoreConstrains/%s/Clef%d",
                                m_sSection.wx_str(), i);
        pPrefs->Read(sKey, &fValue, ((EClef)i == k_clef_G2));
        m_oClefs.SetValid((EClef)i, fValue);
        sKey = wxString::Format("/Constrains/ScoreConstrains/%s/MinNote%d",
                                m_sSection.wx_str(), i);
        m_oClefs.SetLowerPitch((EClef)i, pPrefs->Read(sKey, "a3"));
        sKey = wxString::Format("/Constrains/ScoreConstrains/%s/MaxNote%d",
                                m_sSection.wx_str(), i);
        m_oClefs.SetUpperPitch((EClef)i, pPrefs->Read(sKey, "a5"));
    }

    //max interval in two consecutive notes
    sKey = wxString::Format("/Constrains/ScoreConstrains/%s/MaxInterval",
                            m_sSection.wx_str());
    m_nMaxInterval = (int)pPrefs->Read(sKey, 4L);     //default value 4

    //allowed time signatures
    for (int i = k_min_time_signature; i <= k_max_time_signature; i++)
    {
        sKey = wxString::Format("/Constrains/ScoreConstrains/%s/Time%d",
                                m_sSection.wx_str(), i);
        pPrefs->Read(sKey, &fValue, true);
        m_oValidTimeSign.SetValid((ETimeSignature)i, fValue);
    }

    //allowed key signatures
    for (int i=k_min_key; i <= k_max_key; i++)
    {
        sKey = wxString::Format("/Constrains/ScoreConstrains/%s/KeySignature%d",
                                m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &fValue, (bool)((EKeySignature)i == k_key_C) );
        m_oValidKeys.SetValid((EKeySignature)i, fValue);
    }

    //TODO load remaining data: fragments
}

//---------------------------------------------------------------------------------------
wxString ScoreConstrains::Verify()
{
    wxString sError = "";

    //ensure that at least a Clef is selected
    bool fAtLeastOne = false;
    for (int i=k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++)
    {
        fAtLeastOne = fAtLeastOne || m_oClefs.IsValid((EClef)i);
        if (fAtLeastOne)
            break;
    }
    if (!fAtLeastOne)
        sError += "Global error: No clef constraints specified\n";

    //ensure that at least a time signature is selected
    fAtLeastOne = false;
    for (int i = k_min_time_signature; i <= k_max_time_signature; i++)
    {
        fAtLeastOne = fAtLeastOne || m_oValidTimeSign.IsValid((ETimeSignature)i);
        if (fAtLeastOne)
            break;
    }
    if (!fAtLeastOne)
        sError += "Global error: No time signature constraints specified\n";

    //ensure that at least a key signature is selected
    fAtLeastOne = false;
    for (int i=k_min_key; i <= k_max_key; i++)
    {
        fAtLeastOne = fAtLeastOne || m_oValidKeys.IsValid((EKeySignature)i);
        if (fAtLeastOne)
            break;
    }
    if (!fAtLeastOne)
        sError += "Global error: No key signature constraints specified\n";

    //TODO verify remaining data: fragments

    return sError;
}


//=======================================================================================
// FragmentsTable implementation
//=======================================================================================
FragmentsTable::FragmentsTable(ApplicationScope& appScope)
    : m_appScope(appScope)
{
}

//---------------------------------------------------------------------------------------
FragmentsTable::~FragmentsTable()
{
    for (int i=0; i < (int)m_aFragment.Count(); i++)
        delete m_aFragment[i];
}

//---------------------------------------------------------------------------------------
/*! Receives a fragment pattern. Analyzes it, breaking it into segments and
    computing times tab, tam and ts, and stores the fragment in the fragments table
*/
void FragmentsTable::AddEntry(TimeSignConstrains* pValidTimeSigns, wxString sPattern)
{
    //create the fragment's entry
    lmFragmentEntry* pFragment = LENMUS_NEW lmFragmentEntry(pValidTimeSigns);
    m_aFragment.Add(pFragment);

    //split the pattern into segments and add each segment to the segments table
    int iEnd;
    wxString sSegment;
    TimeUnits rSegmentDuration;         // ts
    TimeUnits rTimeAlignMeasure = 0.0;  // tam
    TimeUnits rTimeAlignBeat = 0.0;     // tab
    bool fFirstSegment = true;          //it is the firts segment
    bool fBarMark = false;              //bar alignment mark found
    bool fBarTimeComputed = false;      //computation of 'tam' finished
    wxString sSource = sPattern;

    while (sSource != "" )
    {
        //extract the segment and remove it from source
        iEnd = SplitFragment(sSource);
        if (iEnd == -1)
        {
            //last segment
            sSegment = sSource;
            sSource = "";
       }
        else
        {
            //intermediate segment
            sSegment = sSource.substr(0, iEnd);
            if (!fBarMark)
                fBarMark = (sSource.substr(iEnd, 1) == "|");
            sSource = sSource.substr(iEnd+1);
        }

        //compute segment duration and time to align to beat (tab)
        if (fFirstSegment)
        {
            sSegment = GetFirstSegmentDuracion(sSegment, &rSegmentDuration, &rTimeAlignBeat);
            fFirstSegment = false;
        }
        else
        {
            rSegmentDuration = GetPatternDuracion(sSegment, pValidTimeSigns);
            rTimeAlignBeat = 0.0;
        }

        //add up time to bar alignment
        if (!fBarTimeComputed)
        {
            rTimeAlignMeasure += rSegmentDuration;
            fBarTimeComputed = fBarMark;
        }

        //add segment to table
//        wxLogMessage("[FragmentsTable::AddEntry] adding segment %s", sSegment.wx_str());
        SegmentEntry* pSegment = LENMUS_NEW SegmentEntry(sSegment, rTimeAlignBeat,
                                                  rSegmentDuration);
        pFragment->AddSegment(pSegment);
    }

    //update tam (time to align fragment to barline)
    if (fBarTimeComputed)
        pFragment->SetTimeToAlignToBarline(rTimeAlignMeasure);
}

//---------------------------------------------------------------------------------------
/*! Receives an string formed by concatenated elements, for example:
    "(n * n)(n * s g+)(n * s)(n * c g-)"
    sSource must be normalized (lower case, no extra spaces)
    @return the index to the end (closing parenthesis) or first element
*/
int FragmentsTable::SplitPattern(wxString sSource)
{
    int iMax = sSource.length();
    wxASSERT(iMax > 0);                         //sSource must not be empty
    wxASSERT(sSource.substr(0, 1) == "(" );    //must start with parenthesis

    int nAPar = 1;       //let's count first parenthesis
    //look for the matching closing parenthesis
    bool fFound = false;
    int i;
    for (i=1; i < iMax; i++)
    {
        if (sSource.substr(i, 1) == "(" )
            nAPar++;
        else if (sSource.substr(i, 1) == ")" )
        {
            nAPar--;
            if (nAPar == 0)
            {
                //matching parenthesis found. Exit loop
                fFound = true;
                break;
            }
        }
    }
    if (fFound)
        return i;

    wxASSERT(fFound);
    return i;

}

//---------------------------------------------------------------------------------------
/*! Select the fragments that are usable with requested time signature.
    A reference to the usable framents is stored in the fragment table
    variable m_aSelectionSet so that
    @return The number of fragments selected
*/
int FragmentsTable::SelectFragments(ETimeSignature nTimeSign)
{
    m_aSelectionSet.Clear();
    for (int i=0; i < (int)m_aFragment.Count(); i++)
    {
        if ((m_aFragment[i]->m_pValidTimeSigns)->IsValid(nTimeSign))
        {
            m_aSelectionSet.Add(i);
//            //DBG -----------------------------------------------------------------------
//            ArrayOfSegments* pSegments =  m_aFragment[i]->GetSegments();
//            stringstream msg;
//            msg << "entry=" << i << ", nTimeSign=" << nTimeSign;
//            LOMSE_LOG_INFO(msg.str());
//            for (int s=0; s < (int)pSegments->Count(); ++s)
//            {
//                stringstream msg;
//                SegmentEntry* pEntry = pSegments->Item(s);
//                msg << "    " << to_std_string(pEntry->GetSource());
//                LOMSE_LOG_INFO(msg.str());
//            }
//            //END DBG -------------------------------------------------------------------
        }
    }
    m_nNextSegment = 0;
    return m_aSelectionSet.Count();

}

//---------------------------------------------------------------------------------------
/*! Select one fragment, at random, from the set of selected fragments.
    Also, all variables related to retrieving the segments composing the
    selected fragment are initialized.
*/
void FragmentsTable::ChooseRandom()
{
    RandomGenerator oGenerator;
    int i = oGenerator.random_number(0, m_aSelectionSet.Count()-1);
    m_nSelItem = m_aSelectionSet.Item(i);
    lmFragmentEntry* pFragmentEntry = m_aFragment.Item( m_nSelItem );
    m_pSegments = pFragmentEntry->GetSegments();
    m_nNextSegment = 0;     //point to first one
}

//---------------------------------------------------------------------------------------
//! Returns the next segment entry for the chosen fragment or nullptr if no more available
SegmentEntry* FragmentsTable::GetNextSegment()
{
    if (m_nNextSegment < (int)m_pSegments->Count())
        return m_pSegments->Item(m_nNextSegment++);
    else
        return nullptr;
}

//---------------------------------------------------------------------------------------
int FragmentsTable::SplitFragment(wxString sSource)
{
    int iMax = sSource.length();
    wxASSERT(iMax > 0);                         //sSource must not be empty
	if (sSource.substr(0, 1) != "(" )
    {
        //must start with parenthesis
		LOMSE_LOG_ERROR("Error in fragment '%s'", sSource.ToStdString().c_str());
		wxASSERT(false);
	}

    //look for a comma (,) or a barline (|)
    for (int i=1; i < iMax; i++)
    {
        if (sSource.substr(i, 1) == "," || sSource.substr(i, 1) == "|" )
        {
            //found. Exit loop
            return i;
        }
    }

    return -1;      // not found

}

//---------------------------------------------------------------------------------------
TimeUnits FragmentsTable::GetPatternDuracion(wxString sPattern,
                                             TimeSignConstrains* WXUNUSED(pValidTimeSigns))
{
    //return the total duration of the pattern

    if (sPattern.Contains("(n h"))
        LOMSE_LOG_ERROR("Invalid pattern %s", sPattern.ToStdString().c_str());

    //prepare source with a measure and instatiate note pitches
    sPattern.Replace("*", "a4");
    string source = to_std_string(sPattern);
    source += "(barline end)";

    //create a score to measure fragment duration
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* libScope = lib.get_library_scope();
    Document doc(*libScope);
    doc.create_empty();
    ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
    ImoContent* pContent = doc.get_content();
    pContent->append_child(pScore);
    ImoInstrument* pInstr = pScore->add_instrument();
    pInstr->add_clef(k_clef_G2);
    pInstr->add_staff_objects(source);

    pScore->end_of_changes();

    //The score is built. Get last barline timepos to get total duration
    ColStaffObjs* pColStaffObjs = pScore->get_staffobjs_table();
    ColStaffObjsEntry* entry = pColStaffObjs->back();
    TimeUnits duration = entry->time();
//    wxLogMessage("[FragmentsTable::GetPatternDuracion] '%s', duration=%.2f",
//                 to_wx_string(source).wx_str(), duration);
    return duration;
}

//---------------------------------------------------------------------------------------
wxString FragmentsTable::GetFirstSegmentDuracion(wxString sSegment,
                                TimeUnits* pSegmentDuration, TimeUnits* pTimeAlignBeat)
{
    //  - Removes any rests at the beginig of the segment and returns the
    //    remaining elements.
    //  - Computes the duration of the removed rests and updates with this
    //    value variable pointed by pTimeAlignBeat
    //  - Computes the duration of the remaining elements and updates with this
    //    value variable pointed by pSegmentDuration

    ////TODO 5.0
    //g_pLogger->LogTrace("FragmentsTable::GetFirstSegmentDuracion",
    //        "[FragmentsTable::GetFirstSegmentDuracion] analyzing='%s'",
    //        sSegment.wx_str() );

    //prepare source with a measure and instatiate note pitches
    sSegment.Replace(" * ", " a4 ");
    string source = to_std_string(sSegment);
    source += "(barline end)";

    //create a score to measure segment duration
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* libScope = lib.get_library_scope();
    Document doc(*libScope);
    doc.create_empty();
    ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
    ImoContent* pContent = doc.get_content();
    pContent->append_child(pScore);
    ImoInstrument* pInstr = pScore->add_instrument();
    pInstr->add_clef(k_clef_G2);
    pInstr->add_staff_objects(source);

    pScore->end_of_changes();

    //The score is built. Get initial rests duration
    ColStaffObjs* pColStaffObjs = pScore->get_staffobjs_table();
    TimeUnits rRestsDuration = 0.0;
    ColStaffObjsIterator it = pColStaffObjs->begin();
    while(it != pColStaffObjs->end())
    {
        ImoObj* pImo = (*it)->imo_object();
        if (pImo->is_note_rest())
        {
            if (pImo->is_rest())
            {
                ImoNoteRest* pR = static_cast<ImoNoteRest*>(pImo);
                rRestsDuration += pR->get_duration();
            }
            else
                break;
        }
        ++it;
    }

    //Get last barline timepos to get total segment duration
    ColStaffObjsEntry* entry = pColStaffObjs->back();
    TimeUnits rSegmentDuration = entry->time() - rRestsDuration;

    //Now remove any rests from the begining of the pattern
    wxString sSource = sSegment;
    while (sSource != "" )
    {
        //extract an element
        int iEnd = split_ldp_pattern(sSource) + 1;
        wxString sElement = sSource.substr(0, iEnd);

        //if it is a rest remove it from pattern; otherwise finish loop
        if (ldp_pattern_is_rest(sElement))
            sSource = sSource.substr(iEnd);
        else
            break;
    }

    //return results
//    wxLogMessage("[FragmentsTable::GetFirstSegmentDuracion] sSource='%s', ts=%.2f, tab=%.2f",
//                 sSource.wx_str(), rSegmentDuration, rRestsDuration );

    *pSegmentDuration = rSegmentDuration;
    *pTimeAlignBeat = rRestsDuration;
    sSource.Replace(" a4 ", " * ");
    return sSource;
}


}   // namespace lenmus
