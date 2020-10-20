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
#include "lenmus_composer.h"

#include "lenmus_generators.h"
#include "lenmus_utilities.h"

//lomse
#include <lomse_logger.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_im_factory.h>
#include <lomse_score_utilities.h>
#include <lomse_staffobjs_table.h>
#include <lomse_staffobjs_cursor.h>
#include <lomse_time.h>
using namespace lomse;

////wxWidgets
//#include <wx/wxprec.h>
//#include <wx/arrstr.h>      // to use wxArrayString

//other
#include <algorithm>
//#include <vector>

#define TRACE_COMPOSER  0
#define TRACE_PITCH     0


namespace lenmus
{

//---------------------------------------------------------------------------------------
enum lmEHarmonicFunction
{
    lmTONIC             = 0x00000001,   // I
    lmSUPERTONIC        = 0x00000002,   // ii
    lmMEDIANT           = 0x00000003,   // iii
    lmSUBDOMINANT       = 0x00000004,   // IV
    lmDOMINANT          = 0x00000005,   // V
    lmSUBMEDIANT        = 0x00000006,   // vi
    lmLEADING           = 0x00000007,   // vii
    lmSUBTONIC          = 0x00000007,   // vii

    // alias
    lm_I                = 0x00000001,
    lm_II               = 0x00000002,
    lm_III              = 0x00000003,
    lm_IV               = 0x00000004,
    lm_V                = 0x00000005,
    lm_VI               = 0x00000006,
    lm_VII              = 0x00000007,


    lmROOT_POSITION     = 0x00000008,
    lmFIRST_INVERSION   = 0x00000010,
    lmSECOND_INVERSION  = 0x00000020,
    lmTHIRD_INVERSION   = 0x00000040,

    lmSEVENTH_ADDED     = 0x00000100,

    // masks
    lmGRADE_MASK        = 0x00000007,   // to extract grade
};

//---------------------------------------------------------------------------------------
// table of typical harmonic progressions, to base compositions on them
static lmEHarmonicFunction m_aProgression[][8] =
{
    {lm_I,  lm_V,   lm_I,   lm_IV,  lm_II,  lm_III, lm_IV,  lm_I },
    {lm_I,  lm_II,  lm_V,   lm_I,   lm_III, lm_IV,  lm_V,   lm_I },

};



//=======================================================================================
// Composer implementation
//=======================================================================================
Composer::Composer(ADocument doc)
    : m_doc(doc)
    , m_midiVoice(0)    //Acoustic Grand Piano
{
    m_pDoc = doc.internal_object();
}

//---------------------------------------------------------------------------------------
Composer::~Composer()
{
}

//---------------------------------------------------------------------------------------
ImoScore* Composer::generate_score(ScoreConstrains* pConstrains)
{
    //Synthesises a score
    //
    //Algorithm 1 to generate N measures
    //----------------------------------
    //
    //    1. Determine how may beats we have to generate:
    //            BeatsToGenerate = Beats_per_measure x N
    //            NumBeats = 0
    //    2. while (NumBeats < BeatsToGenerate) {
    //        2.1 Randomly choose a pattern satisfying the constraints (lesson, level, time
    //            signature, etc.).
    //        2.2 Instantiate the choosen pattern by assingning note pitches
    //        2.3 NumBeats = NumBeats + num. beats in choosen pattern
    //    }
    //    3. Divide generated beats string into measures (just assign to each measure the
    //       next n beats -- where n=Beats_per_measure -- )
    //
    //    Problems:
    //        - difficulties for using patterns that are not full measures
    //        - difficulties for aligning patterns to barlines at specific points
    //        - strong limitation: requires patterns divided into beats. It would be
    //            impossible, for example, include a dotted quarter note in a binary
    //            measure.
    //
    //
    //
    //Algorithm 2 to generate N measures
    //----------------------------------
    //    Rationale:
    //        Instead of generating beats, let's organize the main loop around generating
    //        measures.
    //        Lets use patterns that contains musical phrases instead of full measures.
    //        Patterns will contain, when necessary, barline alignment information.
    //        Before adding a pattern to the current measure, lets align the pattern by
    //        inserting rests, if required.
    //
    //
    //    while (NumMeasures < MeasuresToGenerate) {
    //        If (no measure is opened) start a measure
    //        If (no beats in pattern) {
    //            Randomly choose a pattern satisfying the constraints (lesson,
    //                level, time signature, etc.).
    //            Instantiate the choosen pattern by assingning note pitches.
    //            Align pattern to measure by adding any required rest to the measure
    //        }
    //        While (there are beats in the choosen pattern and measure not full) {
    //            add beats.
    //        }
    //        If measure full {
    //            Close measure
    //            NumMeasures++
    //        }
    //    }
    //
    //
    //Algorithm 3 to generate N measures (8/Feb/06)
    //---------------------------------------------
    //
    //    Rationale:
    //        Algorithm 2 generates scores with mis-aligments and irregular measures.
    //        The problem is due to lack of information about alignment. So in this
    //        improved algorithm I will also use patterns containing musical phrases
    //        instead of full measures.
    //        These patterns will be called 'fragments'. Fragments are divided into
    //        'segments' (a segment is a group of elements - notes and rests - that must
    //        go together and occupies one or more full beats).
    //        Fragments will contain barline alignment information.
    //
    //        The algorithm is essentialy the same, but the method to add beats is
    //        improved by taking into account alignment and duration information. Also
    //        it is necessary to take into account that a segment may contain one or
    //        more beats (not necessarily full beats).
    //
    //        Another improvement is generating the final measure not by using fragments
    //        but by generation a note lasting one ore more beats. And its pitch is
    //        the root pitch of the used key signature.
    //
    //        Notes.
    //        - It is assumed that all beats are equal. Therefore, current algorithm
    //          works only for regular time signature rhythms. It will
    //          not work, for example, with 7/8.
    //
    //
    //
    //    while (NumMeasures < MeasuresToGenerate - 1 ) {
    //        If (no measure is opened) start a measure
    //        If (no beats in pattern) {
    //            Randomly choose a pattern satisfying the constraints (lesson,
    //                level, time signature, etc.).
    //            Instantiate the choosen pattern by assingning note pitches.
    //        }
    //        While (there are beats in the choosen pattern and measure not full) {
    //            add beats.
    //        }
    //        If measure full {
    //            Close measure
    //            NumMeasures++
    //        }
    //    }
    //    Add final measure
    //
    //

    //Save parameters
    m_pConstrains = pConstrains;

    //Generate a random key, time signature and clef satisfying the constrains
    m_nClef = RandomGenerator::generate_clef(m_pConstrains->GetClefConstrains());
    m_nKey = RandomGenerator::generate_key(m_pConstrains->GetKeyConstrains());
    m_nTimeSign = RandomGenerator::GenerateTimeSign(m_pConstrains->GetTimeSignConstrains());
    bool fCompound = get_num_ref_notes_per_pulse_for(m_nTimeSign) != 1;

    // prepare and initialize the score
    AScore score = m_doc.create_object(k_obj_score).downcast_to_score();
    ImoScore* pScore = score.internal_object();
    ImoInstrument* pInstr = pScore->add_instrument();
    pScore->set_version(200);
    ImoSoundInfo* pSound = pInstr->get_sound_info(0);
    ImoMidiInfo* pMidi = pSound->get_midi_info();
    pMidi->set_midi_program(m_midiVoice);
    ImoSystemInfo* pInfo = pScore->get_first_system_info();
    pInfo->set_top_system_distance( pInstr->tenths_to_logical(30) );     // 3 lines
    pInstr->add_clef(m_nClef);
    pInstr->add_key_signature(m_nKey);

#if 0   //useful for debugging and to generate scores with a chosen rhythm line to write documentation
    pInstr->add_time_signature(2, 4);
    pInstr->add_staff_objects("(r e)(n * e)(n * e g+)(n * e l g-)(barline simple)");
    pInstr->add_staff_objects("(n * e g+)(n * s)(n * s g-)(n * e g+)(n * e g-)(barline simple)");
    pInstr->add_staff_objects("(n * q)(r e)(n * e)(barline simple)");
    pInstr->add_staff_objects("(n * s g+)(n * s)(n * s)(n * s g-)(n * e g+)(n * e l g-)(barline simple)");
    pInstr->add_staff_objects("(n * e g+)(n * s)(n * s g-)(n * s g+)(n * s)(n * s)(n * s g-)(barline simple)");
    pInstr->add_staff_objects("(n * s g+)(n * s)(n * s)(n * s g-)(n * e g+)(n * e l g-)(barline simple)");
    pInstr->add_staff_objects("(n * q)(r e)(n * e)(barline simple)");
    pInstr->add_staff_objects("(n * b)(barline end)");
    pScore->end_of_changes();
    return pScore;
#else
    int beats = get_top_number_for(m_nTimeSign);
    int type = get_bottom_number_for(m_nTimeSign);
    pInstr->add_time_signature(beats, type);
#endif

    //
    // Content generation
    //

    // Determine how may measures we have to generate:
    #define NUM_MEASURES   8        //num of measures to generate
    int nMeasuresToGenerate = NUM_MEASURES - 1;
    int nNumMeasures = 0;
    TimeUnits rMeasureDuration = get_measure_duration_for(m_nTimeSign);       //tm
    TimeUnits rBeatDuration = get_ref_note_duration_for(m_nTimeSign);             //tb
    TimeUnits rTimeRemaining;           //tr
    TimeUnits rSegmentDuration;         //ts
    TimeUnits rConsumedBeatTime;        //tcb
    TimeUnits rSegmentAlignBeatTime;    //tab


    // Loop to generate the required measures
    wxString sMeasure;            //source code of current measure
    bool fFits;                   //current segment fits in current measure
    TimeUnits rOccupiedDuration;  //consumed time in current measure (tc)
    bool fMeasure = false;        //there is a measure started
    SegmentEntry* pSegment;       //segment to add to measure

    //select all usable fragments for current time signature
    if (m_pConstrains->SelectFragments(m_nTimeSign) == 0)
    {
        LOMSE_LOG_ERROR("No usable fragments!");
        return pScore;
    }

    //chose ramdomly a fragment satisfying the constraints, and take the first segment
    pConstrains->ChooseRandomFragment();
    pSegment = pConstrains->GetNextSegment();
    //TODO: what if no fragment satisfies the constraints?


    // Randomly decide whether or not to create an anacrux measure
    TimeUnits rPickupDuration = 0;	// Pickup measure duration
#if (LENMUS_DEBUG_BUILD == 1)   //CAMILLA
    if (RandomGenerator::flip_coin())
    {
        rPickupDuration = rBeatDuration;	// 1 pickup beat

	// Assuming the pickup measure doesn't count against the measure count,
	// because its length is compensated for in the last measure
        sMeasure = CreateAnacruxMeasure(nNumMeasures, m_nTimeSign, rPickupDuration);
        pInstr->add_staff_objects( to_std_string(sMeasure) );
    }
#endif




    int nSegmentLoopCounter = 0;
    while (nNumMeasures < nMeasuresToGenerate)
    {
        //If no measure is open start a new measure
        if (!fMeasure)
        {
            sMeasure = "";
            rOccupiedDuration = 0.0;
            fMeasure = true;
        }

        //If there are no more segments in current fragment, choose a new fragment
        if (!pSegment)
        {
            //Randomly choose a new fragment satisfying the constraints
            pConstrains->ChooseRandomFragment();
            pSegment = pConstrains->GetNextSegment();
            wxASSERT(pSegment);     //there must exits a fragment satisfying
                                    //the constraints. Otherwise this would have been
                                    //detected before entering the main while loop
        }

        //While (there are segments in the current fragment and the measure is not full) {
        while (pSegment && rOccupiedDuration < rMeasureDuration)
        {
            //check if segment fits in. A segment S will fit in the measure
            //only when (tr >= ts && tcb <= tab)

            rTimeRemaining = rMeasureDuration - rOccupiedDuration;
            rSegmentDuration = pSegment->GetSegmentDuration();
            rConsumedBeatTime = rOccupiedDuration;  //this line and next two ones compute tcb = tc % tb;
            while (is_greater_time(rConsumedBeatTime, 0.0))
                rConsumedBeatTime -= rBeatDuration;
            if (rConsumedBeatTime < 0.0)
                rConsumedBeatTime += rBeatDuration;
            rSegmentAlignBeatTime = pSegment->GetTimeAlignBeat();
            fFits = (!is_lower_time(rTimeRemaining, rSegmentDuration)
                     && !is_greater_time(rConsumedBeatTime, rSegmentAlignBeatTime));

            #if (TRACE_COMPOSER == 1)
            wxLogMessage("[Composer::generate_score] sMeasure=%s, pSegment=%s, tr=%.2f, ts=%.2f, tcb=%.2f, tab=%.2f, tc=%.2f, tb=%.2f, fits=%s",
                    sMeasure.wx_str(),
                    (pSegment->GetSource()).wx_str(), rTimeRemaining, rSegmentDuration,
                    rConsumedBeatTime, rSegmentAlignBeatTime,
                    rOccupiedDuration, rBeatDuration,
                    (fFits ? "yes" : "no") );
            #endif

            //if segment fits add it to current measure
            if (fFits)
            {
                //it fits. Add it to current measure
                TimeUnits rNoteTime = rSegmentAlignBeatTime - rConsumedBeatTime;
                if (is_greater_time(rNoteTime, 0.0))
                {
                    if (rConsumedBeatTime > 0.0)
                        sMeasure += CreateNote((int)rNoteTime, fCompound, false /*not final note*/);
                    else
                        sMeasure += CreateRest((int)rNoteTime, fCompound, false /*not final rest*/);
                }

                //add segment
                sMeasure += pSegment->GetSource();
                #if (TRACE_COMPOSER == 1)
                wxLogMessage("[Composer::generate_score] Adding segment. Measure = '%s')",
                             sMeasure.wx_str());
                #endif

                //update tr
                rOccupiedDuration += rSegmentDuration + rNoteTime;

                //get next segment
                pSegment = pConstrains->GetNextSegment();
                nSegmentLoopCounter = 0;
            }
            else
            {
                //does not fit.
                #if (TRACE_COMPOSER == 1)
                wxLogMessage("[Composer::generate_score] Segment does not fit. Ignored");
                #endif
                if (nSegmentLoopCounter++ > 100)
                {
                    //let's assume that no segment fits. Fill the measure with a note
                    sMeasure += CreateNote((int)rTimeRemaining, fCompound, false /*not final note*/);
                    rOccupiedDuration += rTimeRemaining;
                    nSegmentLoopCounter = 0;
                }
                else
                {
                    // Ignore segment and take a new one
                    pSegment = pConstrains->GetNextSegment();
                }
            }
        }

        // if measure is full, close it and increment measures count
       if (rOccupiedDuration >= rMeasureDuration)
       {

            // close current measure
            fMeasure = false;   // no measure opened
            sMeasure += "(barline simple)";

            // increment measures counter
            nNumMeasures++;

            // Instantiate the notes by assigning note pitches and add
            // the measure to the score
            #if (TRACE_COMPOSER == 1)
            wxLogMessage("[Composer::generate_score] Adding measure = '%s')",
                         sMeasure.wx_str());
            #endif
            pInstr->add_staff_objects( to_std_string(sMeasure) );
        }

    }

    pScore->end_of_changes();    //generate ColStaffObjs, to traverse it in following code lines

    // In Music Reading, level 1, introduction lessons use only quarter notes. In those
    // exercises we should not use half notes in the last measure. So lets check if
    // only quarter notes are used in the composed piece of music.
    bool fOnlyQuarterNotes = true;
    ColStaffObjs* pColStaffObjs = pScore->get_staffobjs_table();
    ColStaffObjsIterator it = pColStaffObjs->begin();
    while(it != pColStaffObjs->end())
    {
        ImoObj* pImo = (*it)->imo_object();
        if (pImo->is_note())
        {
            //Note fount. Take duration
            ImoNote* pNote = static_cast<ImoNote*>(pImo);
            fOnlyQuarterNotes &= is_equal_time(pNote->get_duration(),
                                               TimeUnits(k_duration_quarter) );
            if (!fOnlyQuarterNotes)
                break;
        }
        ++it;
    }

    #if (TRACE_COMPOSER == 1)
    wxLogMessage("[Composer::generate_score] fOnlyQuarterNotes=%s)",
            (fOnlyQuarterNotes ? "True" : "False") );
    #endif

    // add a final measure with a root pitch note lasting, at least, one beat
    sMeasure = CreateLastMeasure(++nNumMeasures, m_nTimeSign, fOnlyQuarterNotes, rPickupDuration);
    pInstr->add_staff_objects( to_std_string(sMeasure) );

    pScore->end_of_changes();

    #if (TRACE_COMPOSER == 1)
    wxLogMessage("[Composer::generate_score] Adding final measure = '%s')", sMeasure.wx_str());
    #endif


     //Score is built but pitches are not yet defined.
     //Proceed to instatiate pitches according to key signature
    GetNotesRange();
    //pScore->Dump("lemus_score_dump.txt");

    //assign pitch
    if (m_nClef == k_clef_percussion)
        InstantiateWithNote(pScore, FPitch("a4") );
    else
        InstantiateNotes(pScore, m_nKey, nNumMeasures);

    // done
    //pScore->Dump("lemus_score_dump.txt");
    return pScore;
}

//---------------------------------------------------------------------------------------
void Composer::GetNotesRange()
{
    //get the minimum and maximum notes
    wxString sMinPitch = (m_pConstrains->GetClefConstrains())->GetLowerPitch(m_nClef);
    m_fpMinPitch = FPitch( to_std_string(sMinPitch) );

    wxString sMaxPitch = (m_pConstrains->GetClefConstrains())->GetUpperPitch(m_nClef);
    m_fpMaxPitch = FPitch( to_std_string(sMaxPitch) );
}

//---------------------------------------------------------------------------------------
wxString Composer::CreateNoteRest(int nNoteRestDuration, bool fNote, bool fCompound,
                                  bool fFinal)
{
    //Returns a string with one or more LDP elements containing notes o rests up to a total
    //duration nNoteDuration. They will be notes if fNote==true; otherwise they will be rests.
    //For example, for nNoteDuration=64 it will return "(n * n)"

    wxString sElement = "";
    int nDuration;
    int nTimeNeeded = nNoteRestDuration;

    if (fCompound && fFinal)
    {
        while (nTimeNeeded > 0)
        {
            sElement += (fNote ? "(n * " : "(r " );
            if (nTimeNeeded >= k_duration_whole_dotted)
            {
                sElement += "w.)";
                nDuration = k_duration_whole_dotted;
            }
            else if (nTimeNeeded >= k_duration_half_dotted)
            {
                sElement += "h.)";
                nDuration = k_duration_half_dotted;
            }
            else if (nTimeNeeded >= k_duration_quarter_dotted)
            {
                sElement += "q.)";
                nDuration = k_duration_quarter_dotted;
            }
            else if (nTimeNeeded >= k_duration_eighth_dotted)
            {
                sElement += "e.)";
                nDuration = k_duration_eighth_dotted;
            }
            else if (nTimeNeeded >= k_duration_16th_dotted)
            {
                sElement += "s.)";
                nDuration = k_duration_16th_dotted;
            }
            else if (nTimeNeeded >= k_duration_32nd_dotted)
            {
                sElement += "t.)";
                nDuration = k_duration_32nd_dotted;
            }
            else if (nTimeNeeded >= k_duration_64th_dotted)
            {
                sElement += "i.)";
                nDuration = k_duration_64th_dotted;
            }
            else if (nTimeNeeded >= k_duration_128th_dotted)
            {
                sElement += "o.)";
                nDuration = k_duration_128th_dotted;
            }
            else
            {
                sElement += "f)";
                nDuration = k_duration_256th;
            }

            nTimeNeeded -= nDuration;
        }
    }
    else
    {
        while (nTimeNeeded > 0)
        {
            sElement += (fNote ? "(n * " : "(r " );
            if (nTimeNeeded >= k_duration_whole_dotted)
            {
                sElement += "w.)";
                nDuration = k_duration_whole_dotted;
            }
            else if (nTimeNeeded >= k_duration_whole)
            {
                sElement += "w)";
                nDuration = k_duration_whole;
            }
            else if (nTimeNeeded >= k_duration_half_dotted)
            {
                sElement += "h.)";
                nDuration = k_duration_half_dotted;
            }
            else if (nTimeNeeded >= k_duration_half)
            {
                sElement += "h)";
                nDuration = k_duration_half;
            }
            else if (nTimeNeeded >= k_duration_quarter_dotted)
            {
                sElement += "q.)";
                nDuration = k_duration_quarter_dotted;
            }
            else if (nTimeNeeded >= k_duration_quarter)
            {
                sElement += "q)";
                nDuration = k_duration_quarter;
            }
            else if (nTimeNeeded >= k_duration_eighth_dotted)
            {
                sElement += "e.)";
                nDuration = k_duration_eighth_dotted;
            }
            else if (nTimeNeeded >= k_duration_eighth)
            {
                sElement += "e)";
                nDuration = k_duration_eighth;
            }
            else if (nTimeNeeded >= k_duration_16th_dotted)
            {
                sElement += "s.)";
                nDuration = k_duration_16th_dotted;
            }
            else if (nTimeNeeded >= k_duration_16th)
            {
                sElement += "s)";
                nDuration = k_duration_16th;
            }
            else if (nTimeNeeded >= k_duration_32nd_dotted)
            {
                sElement += "t.)";
                nDuration = k_duration_32nd_dotted;
            }
            else if (nTimeNeeded >= k_duration_32nd)
            {
                sElement += "t)";
                nDuration = k_duration_32nd;
            }
            else if (nTimeNeeded >= k_duration_64th_dotted)
            {
                sElement += "i.)";
                nDuration = k_duration_64th_dotted;
            }
            else if (nTimeNeeded >= k_duration_64th)
            {
                sElement += "i)";
                nDuration = k_duration_64th;
            }
            else if (nTimeNeeded >= k_duration_128th_dotted)
            {
                sElement += "o.)";
                nDuration = k_duration_128th_dotted;
            }
            else if (nTimeNeeded >= k_duration_128th)
            {
                sElement += "o)";
                nDuration = k_duration_128th;
            }
            else
            {
                sElement += "f)";
                nDuration = k_duration_256th;
            }

            nTimeNeeded -= nDuration;
        }
    }

    #if (TRACE_COMPOSER == 1)
    wxLogMessage("[Composer::CreateNoteRest] Needed duration= %d, added=%s",
        nNoteRestDuration, sElement.wx_str());
    #endif

    return sElement;
}

//---------------------------------------------------------------------------------------
wxString Composer::CreateLastMeasure(int WXUNUSED(nNumMeasure), ETimeSignature nTimeSign,
                                     bool fOnlyQuarterNotes, TimeUnits rPickupDuration)
{
    // Returns a final meaure. This final measure has only a note, long enough, and
    // a final bar

    wxString sMeasure = "";
    TimeUnits rMeasureDuration = get_measure_duration_for(nTimeSign) - rPickupDuration;	// Compensate for pickup measure
    TimeUnits rPulseDuration = get_ref_note_duration_for(nTimeSign) *
                            get_num_ref_notes_per_pulse_for(nTimeSign);
    TimeUnits rNoteDuration = rPulseDuration;
    bool fCompound = (get_num_ref_notes_per_pulse_for(nTimeSign) != 1);
    if (!fOnlyQuarterNotes && rMeasureDuration / rPulseDuration >= 2.0)
    {
        //flip coin to randomly add a one-beat note or a two-beats note
        if (RandomGenerator::flip_coin())
            rNoteDuration += rPulseDuration;
    }

    sMeasure += CreateNote((int)rNoteDuration, fCompound, true /*final note*/);
    rNoteDuration = rMeasureDuration - rNoteDuration;
    if (is_greater_time(rNoteDuration, 0.0))
#if (LENMUS_DEBUG_BUILD == 1)   //CAMILLA
        sMeasure += CreateRest((int)rNoteDuration, fCompound, false /*final rest*/);
		//'true' didn't work well with Pickup beats
#else
        sMeasure += CreateRest((int)rNoteDuration, fCompound, true /*final rest*/);
#endif

    sMeasure += "(barline end)";
    return sMeasure;
}

//---------------------------------------------------------------------------------------
wxString Composer::CreateAnacruxMeasure(int WXUNUSED(nNumMeasure), ETimeSignature nTimeSign,
                                        TimeUnits rPickupDuration)
{
	wxString sMeasure = "";

	TimeUnits newMeasureDuration = rPickupDuration; //(get_measure_duration_for(nTimeSign)); //Shorter measure duration by half, since it's a pickup measure.
	//TimeUnits rMeasureDuration = newMeasureDuration;

	bool fCompound = (get_num_ref_notes_per_pulse_for(nTimeSign) != 1);

	TimeUnits rNoteDuration = newMeasureDuration; //or rNoteDuration = newMeasureDuration (k_duration_quarter)
	sMeasure += CreateNote((int)rNoteDuration, fCompound, false);

	sMeasure += "(barline simple)";
	return sMeasure;
}

//---------------------------------------------------------------------------------------
// Methods to deal with tonality
//---------------------------------------------------------------------------------------

bool Composer::InstantiateNotes(ImoScore* pScore, EKeySignature nKey, int nNumMeasures)
{
    // Returns true if error

    #if (TRACE_PITCH == 1)
        LOMSE_LOG_INFO("Starting. nNumMeasures=%d", nNumMeasures);
    #endif

    // Choose a chord progression, based on key signature: nChords[]
    std::vector<long> nChords(nNumMeasures);
    GetRandomHarmony(nNumMeasures, nChords);

    // Lets compute the notes in the natural scale of the key signature to use
    // This will be used later in various places
    FPitch scale[7];             // the notes in the scale
    GenerateScale(nKey, scale);

    // In a later step we are going to choose and compute a contour curve.
    // The contour curve will have as many points as on-chord notes in the music line.
    // So first we have to compute the number of on-chord notes. The following code is a
    // loop to count on-chord notes in first staff of first instrument
    // and to locate last note. This is necessary to assign it the root pitch (later)
    // ImoNote* pLastNote = nullptr;
    int nNumPoints = 0;
    ImoTimeSignature* pTS = nullptr;
    StaffObjsCursor cursor(pScore);
    int numNotes = 0;   //DBG
    while(!cursor.is_end())
    {
        ImoStaffObj* pSO = cursor.get_staffobj();
        if (pSO->is_note())
        {
            ++numNotes;     //DBG
//            ImoNote* pNote = static_cast<ImoNote*>(pSO);
            int pos = k_off_beat;
            if (pTS)
                pos = get_beat_position(cursor.time(), pTS, cursor.anacruxis_missing_time());

            if(pos != k_off_beat)
                nNumPoints++;   // on beat note

//            pLastNote = pNote;
        }
        else if (pSO->is_time_signature())
        {
            pTS = static_cast<ImoTimeSignature*>( pSO );
        }

        cursor.move_next();
    }
    #if (TRACE_PITCH == 1)
        LOMSE_LOG_INFO("Num. notes=%d, num.on-beat notes=%d", numNotes, nNumPoints);
    #endif


    // If number of points is small (i.e < 8) forget about this. Instantiate notes
    // with random pitch and finish. This bypasses the only problem found, when a
    // score has rests in all beat positions (L2_musicReading, Lesson 17, exercise 1)
    if (nNumPoints < 8)
    {
        InstantiateNotesRandom(pScore);
        return false;       // no error
    }


    // Now we are going to choose at random a contour curve and compute its points
    // The curve will always start and finish in the root note. Its amplitude will
    // be adjusted to satisfy the constrains (min and max pitch)
    std::vector<DiatonicPitch> aContour(nNumPoints);
    GenerateContour(nNumPoints, aContour);

    // allocate a vector for valid notes in chord (all notes in valid notes range)
    int iC = 0;                                         //index to current chord (ic)
    DiatonicPitch dnMinPitch = m_fpMinPitch.to_diatonic_pitch();
    DiatonicPitch dnMaxPitch = m_fpMaxPitch.to_diatonic_pitch();
    #if (TRACE_PITCH == 1)
        LOMSE_LOG_INFO("min pitch %d (%s), max pitch %d (%s)",
            int(dnMinPitch), dnMinPitch.get_ldp_name().c_str(),
            int(dnMaxPitch), dnMaxPitch.get_ldp_name().c_str() );
    #endif
    std::vector<FPitch> aOnChordPitch;
    aOnChordPitch.reserve((int(dnMaxPitch) - int(dnMinPitch))/2);    // Reserve space. Upper limit estimation
    FPitch nRootNote = GenerateInChordList(nKey, nChords[iC], aOnChordPitch);

    // Loop to process notes/rests in first staff of first instrument
    ImoNote* pOnChord1 = nullptr;      //Pair of on-chord notes. First one
    ImoNote* pOnChord2 = nullptr;      //Pair of on-chord notes. Second one
    ImoNote* pNonChord[20];         //non-chord notes between the two on-chord notes
    int nCount = 0;                 //number of non-chord notes between the two on-chord notes

    ImoNote* pNotePrev = nullptr;
    ImoNote* pNoteCur;
    int iPt = 0;
    FPitch fpNew;
    string sDbg = "";   //DBG
    numNotes=0;   //DBG
    StaffObjsCursor cursor2(pScore);
    while(!cursor2.is_end())
    {
        ImoStaffObj* pImo = cursor2.get_staffobj();
        if (pImo->is_note_rest())
        {
            // 1. It is a note or a rest
            if (pImo->is_note())
            {
                ++numNotes;    //DBG
                // It is a note. Get its chord position
                pNoteCur = static_cast<ImoNote*>(pImo);
                int pos = k_off_beat;
                ImoTimeSignature* pTS = cursor2.get_applicable_time_signature();
                if (pTS)
                    pos = get_beat_position(cursor2.time(), pTS, cursor2.anacruxis_missing_time());
                #if (TRACE_PITCH == 1)
                    LOMSE_LOG_INFO("note %d, pos=%d, time=%f, anacrux.time=%f",
                        numNotes, pos, cursor2.time(), cursor2.anacruxis_missing_time());
                #endif
                if (pos != k_off_beat)
                {
                    // on beat note. Pitch must be on chord.
                    // Assign a pitch from nChords[iC].
                    #if (TRACE_PITCH == 1)
                        for(int k=0; k < (int)aOnChordPitch.size(); k++)
                            LOMSE_LOG_INFO("OnChord %d = %s",
                                           k, aOnChordPitch[k].to_abs_ldp_name().c_str() );
                    #endif
                    fpNew = NearestNoteOnChord(aContour[iPt++], pNotePrev, pNoteCur,
                                                    aOnChordPitch);
                    #if (TRACE_PITCH == 1)
                        for(int k=0; k < (int)aOnChordPitch.size(); k++)
                            LOMSE_LOG_INFO("OnChord %d = %s",
                                           k, aOnChordPitch[k].to_abs_ldp_name().c_str() );
                        string sNoteName = fpNew.to_abs_ldp_name();
                        LOMSE_LOG_INFO(
                            "note %d, on-chord note %d. Assigned pitch = %d (%s), chord=%d",
                            numNotes, iPt, int(fpNew.to_diatonic_pitch()), sNoteName.c_str(),
                            int(nChords[iC] & lmGRADE_MASK) );
                            pNoteCur->set_color(Color(255,0,0));
                    #endif

                    set_pitch(pNoteCur, fpNew);

                    // assign pitch to non-chord notes between previous on-chord one
                    // and this one
                    sDbg += ",c";   //DBG
                    if (nCount != 0)
                    {
                        pOnChord2 = pNoteCur;
                        AssignNonChordNotes(nCount, pOnChord1, pOnChord2,
                                            pNonChord, scale);
                    }

                    // Prepare data for next pair processing
                    nCount = 0;
                    pOnChord1 = pNoteCur;
                }

                else
                {
                    // non-chord note. Save it to be processed later
                    if (pNoteCur->get_fpitch() == k_undefined_fpitch)
                    {
                        pNonChord[nCount++] = pNoteCur;
                        sDbg += ",nc";   //DBG
                    }
                    else
                        sDbg += ",(nc)";   //DBG
                }
                pNotePrev = pNoteCur;
            }
        }

        else if (pImo->is_barline())
        {
            // End of measure: choose the next chord in progression
            iC++;
            nRootNote = GenerateInChordList(nKey, nChords[iC % 8], aOnChordPitch);
        }

        cursor2.move_next();
    }

    #if (TRACE_PITCH == 1)
    LOMSE_LOG_INFO("Notes processed = %d", numNotes);
    LOMSE_LOG_INFO("%s", sDbg.c_str() );
    #endif

    return false;       // no error
}

//---------------------------------------------------------------------------------------
void Composer::InstantiateNotesRandom(ImoScore* pScore)
{
    ColStaffObjs* pColStaffObjs = pScore->get_staffobjs_table();
    ColStaffObjsIterator it = pColStaffObjs->begin();
    while(it != pColStaffObjs->end())
    {
        ImoObj* pImo = (*it)->imo_object();
        if (pImo->is_note())
        {
            FPitch fp = RandomPitch();
            ImoNote* pNote = static_cast<ImoNote*>(pImo);
            set_pitch(pNote, fp);
            #if (TRACE_PITCH == 1)
                string sNoteName = fp.to_abs_ldp_name();
                LOMSE_LOG_INFO(
                    "random note. Assigned pitch = %d (%s)",
                    int(fp.to_diatonic_pitch()), sNoteName.c_str() );
            #endif
        }
        ++it;
    }
}

//---------------------------------------------------------------------------------------
FPitch Composer::RandomPitch()
{
    int nMinPitch = (int)m_fpMinPitch.to_diatonic_pitch();
    int nMaxPitch = (int)m_fpMaxPitch.to_diatonic_pitch();
    static int nLastPitch = 0;

    if (nLastPitch == 0)
        nLastPitch = (nMinPitch + nMaxPitch) / 2;

    int nRange = m_pConstrains->GetMaxInterval();
    int nLowLimit = wxMax(nLastPitch - nRange, nMinPitch);
    int nUpperLimit = wxMin(nLastPitch + nRange, nMaxPitch);
    int nNewPitch;
    if (nUpperLimit - nLowLimit < 2)
        nNewPitch = nLowLimit;
    else
        nNewPitch = RandomGenerator::random_number(nLowLimit, nUpperLimit);

    // save value
    nLastPitch = nNewPitch;

    DiatonicPitch dp(nNewPitch);
    return FPitch(dp.step(), dp.octave(), 0);
}

//---------------------------------------------------------------------------------------
void Composer::GetRandomHarmony(int WXUNUSED(nFunctions), std::vector<long>& aFunction)
{
    //Fills array 'pFunction' with an ordered set of harmonic functions to
    //build a melody. i.e.: I,V,I,IV,II,III,IV,I

    int nNumProgs = sizeof(m_aProgression) / (8 * sizeof(long));
    int iP = RandomGenerator::random_number(0, nNumProgs-1);
    for(int i=0; i < 8; i++)
        aFunction[i] = m_aProgression[iP][i];
}

//---------------------------------------------------------------------------------------
void Composer::FunctionToChordNotes(EKeySignature nKey, long nFunction,
                                    FPitch notes[4])
{
    //Given a key signature and an harmonic function returns the notes to build the
    //chord (four notes per chord). The first chord note is always in octave 4
    //i.e.:
    //C Major, II --> d4, f4, a4
    //D Major, I  --> d4, +f4, a4

    //generate natural scale for key signature
    int nAcc[7];
    KeyUtilities::get_accidentals_for_key(nKey, nAcc);
    int step = KeyUtilities::get_step_for_root_note(nKey);
    FPitch scale[15];
    int octave = k_octave_4;
    for (int iN=0; iN < 15; iN++)
    {
        scale[iN] = FPitch(step, octave, nAcc[step]);
        if(++step == 7)
        {
            step = 0;
            octave++;
        }
    }


    // Compute the triad
    long iF = (nFunction & lmGRADE_MASK) - 1L;
    notes[0] = FPitch( scale[iF] );
    notes[1] = FPitch( scale[iF+2] );
    notes[2] = FPitch( scale[iF+4] );

//    #if (TRACE_COMPOSER == 1)
//    wxLogMessage("[Composer::FunctionToChordNotes] Function %d, Key=%d, note0 %d (%s), note1 %d (%s), note2 %d (%s).",
//        iF, nKey,
//        notes[0].to_diatonic_pitch(), notes[0].to_abs_ldp_name().wx_str(),
//        notes[1].to_diatonic_pitch(), notes[1].to_abs_ldp_name().wx_str(),
//        notes[2].to_diatonic_pitch(), notes[2].to_abs_ldp_name().wx_str() );
//    #endif
}

//---------------------------------------------------------------------------------------
FPitch Composer::MoveByStep(bool fUpStep, FPitch nPitch, FPitch scale[7])
{
    // Generates a new note by moving up/down one step in the scale
    // The new pitch must be on the key signature natural scale

    // extract pitch components
    int nStep = nPitch.step();
    int nOctave = nPitch.octave();

    // find current note in scale
    int i;
    for (i=0; i < 7; i++) {
        if (scale[i].step() == nStep) break;
    }

    //coverity scan sanity check
    if (i >= 7)
    {
        stringstream msg;
        msg << "Logic error. i should be lower than 7, but it is "
            << i << ", scale={";
        for (int j=0; j < i; j++)
            msg << scale[j] << ",";

        msg << "}";
        LOMSE_LOG_ERROR(msg.str());

        i = 0;
    }

    if (fUpStep) {
        // increment note
        if (++i == 7) i = 0;
        if (nStep == k_step_B) nOctave++;
    }
    else {
        // decrement note
        if (--i == -1) i = 6;
        if (nStep == k_step_C) nOctave--;
    }

    nStep = scale[i].step();
    int nAcc = scale[i].accidentals();
    return FPitch(nStep, nOctave, nAcc);

}

//---------------------------------------------------------------------------------------
FPitch Composer::MoveByChromaticStep(bool fUpStep, FPitch pitch)
{
    // Generates a new note by moving up/down one chromatic step in the scale

    // extract pitch accidentals
    int acc = pitch.accidentals();

    if (fUpStep)
        acc++;
    else
        acc--;

    return FPitch(pitch.step(), pitch.octave(), acc);
}

//---------------------------------------------------------------------------------------
void Composer::GenerateScale(EKeySignature nKey, FPitch notes[7])
{
    int acc[7];
    KeyUtilities::get_accidentals_for_key(nKey, acc);
    int step = KeyUtilities::get_step_for_root_note(nKey);
    for (int i=0; i < 7; ++i)
    {
        notes[i] = FPitch(step, k_octave_4, acc[step]);
        if(++step == 7)
            step = 0;
    }
}

//---------------------------------------------------------------------------------------
FPitch Composer::GenerateInChordList(EKeySignature nKey, long nChord,
                                     std::vector<FPitch>& fpValidPitch)
{
    // Returns the root note in octave 4
    // Generates a list with all allowed notes in the chord, satisfying the
    // constraints for notes range. For instance:
    // D Major chord: d4, +f4, a4
    // notes range: a3 to a5
    // returns: a3, d4, +f4, a4, d5, +f5, a5

    // allocate an array for notes in chord (basic chord, octave 4)
    FPitch notes[4];                          // notes in current chord
    FunctionToChordNotes(nKey, nChord, notes);

    // extract valid steps, to simplify
    //TODO: review logic. Value NO_DPITCH is never returned.
    int nValidStep[4];
    for (int i=0; i < 4; i++)
    {
        if (notes[i].to_diatonic_pitch() == NO_DPITCH)
            nValidStep[i] = -1;        //you can assign any non valid value for a step
        else
            nValidStep[i] = notes[i].step();
    }

    // empty valid pitches array
    fpValidPitch.clear();

    // scan notes range and select those in chord
    DiatonicPitch dnMinPitch = m_fpMinPitch.to_diatonic_pitch();
    DiatonicPitch dnMaxPitch = m_fpMaxPitch.to_diatonic_pitch();
    for (int i=int(dnMinPitch); i <= int(dnMaxPitch); i++)
    {
        DiatonicPitch dp(i);
        int nStep = dp.step();
        for (int j=0; j < 4; j++)
        {
            if (nStep == nValidStep[j])
            {
                // Note in chord. Add it to the list
                FPitch nPitch(nStep, dp.octave(), notes[j].num_accidentals());
                fpValidPitch.push_back(nPitch);
            }
        }
    }

    return notes[0];
}

//---------------------------------------------------------------------------------------
void Composer::GenerateContour(int nNumPoints, std::vector<DiatonicPitch>& aContour)
{
    // In this method we choose at random a contour curve and compute its points
    // The curve will always start and finish in the root note. Its amplitude will
    // be adjusted to satisfy the constrains (min and max pitch), and will be
    // a value between one and two octaves, depending on the valid notes range and
    // the type of contour.
    // In case the valid notes range is lower than one octave, arch like curves will
    // be forced

    // First, we will determine the root note
    int nRootStep = KeyUtilities::get_step_for_root_note(m_nKey);

    // Now lets do some computations to determine a suitable octave
    DiatonicPitch dnMinPitch = m_fpMinPitch.to_diatonic_pitch();
    DiatonicPitch dnMaxPitch = m_fpMaxPitch.to_diatonic_pitch();
    int nAmplitude = int(dnMaxPitch) - int(dnMinPitch) + 1;
//    #if (TRACE_COMPOSER == 1)
//    wxLogMessage("[Composer::GenerateContour] minPitch %d  (%s), max pitch %d (%s), amplitude %d",
//        dnMinPitch, dnMinPitch.get_ldp_name().wx_str(),
//        dnMaxPitch, dnMaxPitch.get_ldp_name().wx_str(),
//        nAmplitude );
//    #endif


        // determine minimum root pitch
    int nOctave = dnMinPitch.octave();
    if (dnMinPitch.step() > nRootStep)
        nOctave++;
    DiatonicPitch dnMinRoot(nRootStep, nOctave);

        // determine maximum root pitch
    DiatonicPitch dnMaxRoot = dnMinRoot;
    while (dnMaxRoot+7 <= dnMaxPitch)
        dnMaxRoot+=7;

        // if range greater than two octaves reduce it and reposition
    if (int(dnMaxRoot)-int(dnMinRoot) > 14)
    {
        int nRange = (int(dnMaxRoot)-int(dnMinRoot)) / 7;
        int nShift = RandomGenerator::random_number(0, nRange-2);
        dnMinRoot += 7*nShift;
        dnMaxRoot = dnMinRoot + 14;
    }


//    #if (TRACE_COMPOSER == 1)
//    wxLogMessage("[Composer::GenerateContour] min root %d  (%s), max root %d (%s)",
//        dnMinRoot, DiatonicPitch_ToLDPName(dnMinRoot).wx_str(),
//        dnMaxRoot, DiatonicPitch_ToLDPName(dnMaxRoot).wx_str() );
//    #endif



    // Choose a contour curve
    enum
    {
        lmCONTOUR_TRIANGLE = 0,
        lmCONTOUR_TRIANGLE_RAMP,
        lmCONTOUR_RAMP_TRIANGLE,
        lmSTART_RESTRICTED_CONTOURS,
        lmCONTOUR_ZIG_ZAG = lmSTART_RESTRICTED_CONTOURS,
        lmCONTOUR_RAMP,
        lmMAX_CONTOUR,
        lmCONTOUR_ARCH,             //Bad results. Top is very flat so the sensation of
                                    //raeching a peak is very poor
    };



    // Choose a contour curve. If range is not at least an octave, do not allow
    // ramp curves
    int nCurve;
    bool fUp;
    if (dnMaxRoot == dnMinRoot)
    {
        // Pitch range is not at least an octave. Only some curves allowed.
        nCurve = RandomGenerator::random_number(0, lmSTART_RESTRICTED_CONTOURS-1);
        // Lets force curve direction to better use the avalable notes range
        if (dnMaxPitch < dnMinRoot)
        {
            // allowed notes does not include root note. Use all notes range.
            // Curve direction doesn't matter
            dnMinRoot = dnMinPitch;
            dnMaxRoot = dnMaxPitch;
            fUp = RandomGenerator::flip_coin();
        }
        else if (int(dnMinRoot)-int(dnMinPitch) < int(dnMaxPitch)-int(dnMinRoot))
        {
            // Use upper part of range. Curve going up
            fUp = true;
            dnMaxRoot = dnMaxPitch;
        }
        else
        {
            // use lower part of range. Curve going down.
            fUp = false;
            dnMinRoot = dnMinPitch;
        }
    }
    else
    {
        // range is grater than one octave. Any curve and direction allowed
        nCurve = RandomGenerator::random_number(0, lmMAX_CONTOUR-1);
        fUp = RandomGenerator::flip_coin();
    }

    #if (TRACE_PITCH == 1)
    LOMSE_LOG_INFO("type=%d, nNumPoints=%d, up=%s",
                   nCurve, nNumPoints, (fUp ? "Yes" : "No") );
    #endif

    // prepare curve parameters and compute the curve points
    DiatonicPitch dnLowPitch, dnHighPitch, dnStartRamp, dnEndRamp;
    switch (nCurve)
    {
        case lmCONTOUR_ARCH:
            //----------------------------------------------------------------------------
            // Arch. An arch will be defined by the amplitude, the center beat, and
            //the direction
            //
            if (fUp)
            {
                dnLowPitch = dnMinRoot;
                nAmplitude = dnMaxPitch - dnLowPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnHighPitch = dnLowPitch + nAmplitude;
            }
            else
            {
                dnHighPitch = dnMaxRoot;
                nAmplitude = dnHighPitch - dnMinPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnLowPitch = dnHighPitch - nAmplitude;
            }
            ComputeArch(fUp, 0, nNumPoints, dnLowPitch, dnHighPitch, aContour);
            break;

        case lmCONTOUR_TRIANGLE:
            //----------------------------------------------------------------------------
            // Triangle. Defined the amplitude, the center beat, and the direction
            if (fUp)
            {
                dnLowPitch = dnMinRoot;
                nAmplitude = dnMaxPitch - dnLowPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnHighPitch = dnLowPitch + nAmplitude;
            }
            else
            {
                dnHighPitch = dnMaxRoot;
                nAmplitude = dnHighPitch - dnMinPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnLowPitch = dnHighPitch - nAmplitude;
            }
            ComputeTriangle(fUp, 0, nNumPoints, dnLowPitch, dnHighPitch, aContour);
            break;

        case lmCONTOUR_RAMP:
            //----------------------------------------------------------------------------
            // Ramp.
            // Amplitude will move two octaves from root to root note unless not possible
            if (fUp)
                ComputeRamp(0, nNumPoints, dnMinRoot, dnMaxRoot, aContour);
            else
                ComputeRamp(0, nNumPoints, dnMaxRoot, dnMinRoot, aContour);
            break;

        case lmCONTOUR_TRIANGLE_RAMP:
            //----------------------------------------------------------------------------
            // Triangle+Ramp
            // Triangle occupies two thirds and ramp one third
        {
            // Triangle: set up amplitude and last point
            int nPoints = 2* nNumPoints / 3;
            if (fUp)
            {
                dnLowPitch = dnMinRoot;
                nAmplitude = dnMaxPitch - dnLowPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnHighPitch = dnLowPitch + nAmplitude;
                dnStartRamp = dnHighPitch;
                dnEndRamp = dnLowPitch;
            }
            else
            {
                dnHighPitch = dnMaxRoot;
                nAmplitude = dnHighPitch - dnMinPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnLowPitch = dnHighPitch - nAmplitude;
                dnStartRamp = dnLowPitch;
                dnEndRamp = dnHighPitch;
            }
            ComputeTriangle(fUp, 0, nPoints, dnLowPitch, dnHighPitch, aContour);
            ComputeRamp(nPoints, nNumPoints, dnStartRamp, dnEndRamp, aContour);
            break;
        }

        case lmCONTOUR_ZIG_ZAG:
            //----------------------------------------------------------------------------
            // Triangle+Ramp
            // Triangle occupies two thirds and ramp one third
        {
            // Triangle: set up amplitude and last point
            int nPoints = 2* nNumPoints / 3;
            if (fUp)
            {
                dnLowPitch = dnMinRoot;
                nAmplitude = dnMaxPitch - dnLowPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnHighPitch = dnLowPitch + nAmplitude;
                dnStartRamp = dnLowPitch;
                dnEndRamp = dnMaxRoot;
            }
            else
            {
                dnHighPitch = dnMaxRoot;
                nAmplitude = dnHighPitch - dnMinPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnLowPitch = dnHighPitch - nAmplitude;
                dnStartRamp = dnHighPitch;
                dnEndRamp = dnMinRoot;
            }
            ComputeTriangle(fUp, 0, nPoints, dnLowPitch, dnHighPitch, aContour);
            ComputeRamp(nPoints, nNumPoints, dnStartRamp, dnEndRamp, aContour);
            break;
        }

        case lmCONTOUR_RAMP_TRIANGLE:
            //----------------------------------------------------------------------------
            // Ramp+Triangle
        {
            // Ramp: num points
            int nPoints = nNumPoints / 3;

            // Triangle: set up amplitude and last point
            if (fUp)
            {
                dnLowPitch = dnMinRoot;
                nAmplitude = dnMaxPitch - dnLowPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnHighPitch = dnLowPitch + nAmplitude;
                dnStartRamp = dnLowPitch;
                dnEndRamp = dnMaxRoot;
            }
            else
            {
                dnHighPitch = dnMaxRoot;
                nAmplitude = dnHighPitch - dnMinPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnLowPitch = dnHighPitch - nAmplitude;
                dnStartRamp = dnHighPitch;
                dnEndRamp = dnMinRoot;
            }

            ComputeRamp(0, nPoints, dnStartRamp, dnEndRamp, aContour);
            ComputeTriangle(fUp, nPoints, nNumPoints, dnLowPitch, dnHighPitch, aContour);
            break;
        }
    }

//    #if (TRACE_COMPOSER == 1)
//    for (int i=0; i < nNumPoints; i++)
//        wxLogMessage("[Composer::GenerateContour] point[%d] = %d", i, aContour[i]);
//    #endif
}

//---------------------------------------------------------------------------------------
void Composer::ComputeTriangle(bool fUp, int iStart, int nPoints, DiatonicPitch dnLowPitch,
                                  DiatonicPitch dnHighPitch, std::vector<DiatonicPitch>& aPoints)
{
    // Triangle. Defined the amplitude, the center beat, and the direction
    // (up/down). Also by start pitch, top/bottom pitch, and end pitch

    // first ramp
    float rNumPoints = (float)((nPoints-iStart)/2);
    float rStep = (float)(dnHighPitch - dnLowPitch) / rNumPoints;
    if (!fUp) rStep = -rStep;
    float yValue = (float)(fUp ? dnLowPitch : dnHighPitch);
//    #if (TRACE_COMPOSER == 1)
//    wxLogMessage("[Composer::ComputeTriangle] fUp=%s, iStart=%d, nPoints=%d, dnLowPitch=%d, dnHighPitch=%d, rStep=%.5f",
//        (fUp ? "Yes" : "No"), iStart, nPoints, dnLowPitch, dnHighPitch, rStep);
//    #endif
    int i = iStart;
    int nCenter = (nPoints+iStart)/2;
    for (; i < nCenter; i++)
    {
        aPoints[i] = (int)floor(yValue+0.5);
        yValue += rStep;
    }
    yValue -= rStep;

    // second ramp
    rStep = (float)(dnHighPitch - dnLowPitch) / ((float)(nPoints-iStart) - rNumPoints);
    if (fUp)
        rStep = -rStep;
//    #if (TRACE_COMPOSER == 1)
//    wxLogMessage("[Composer::ComputeTriangle] fUp=%s, iStart=%d, nPoints=%d, dnLowPitch=%d, dnHighPitch=%d, rStep=%.5f",
//        (fUp ? "Yes" : "No"), i, nPoints, dnLowPitch, dnHighPitch, rStep);
//    #endif
    for (; i < nPoints; i++)
    {
        aPoints[i] = (int)floor(yValue+0.5);
        yValue += rStep;
    }

    //force last point to be a root note
    aPoints[nPoints-1] = (fUp ? dnLowPitch : dnHighPitch);

}

//---------------------------------------------------------------------------------------
void Composer::ComputeRamp(int iStart, int nPoints, DiatonicPitch dnStartPitch,
                              DiatonicPitch dnEndPitch, std::vector<DiatonicPitch>& aPoints)
{
    // Ramp
    float rNumPoints = (float)(nPoints-iStart);
    float rStep = (float)(dnEndPitch - dnStartPitch) / rNumPoints;
    float yValue = (float)dnStartPitch;
//    #if (TRACE_COMPOSER == 1)
//    wxLogMessage("[Composer::ComputeRamp] iStart=%d, nPoints=%d, dnStartPitch=%d, dnEndPitch=%d, rStep=%.5f",
//        iStart, nPoints, dnStartPitch, dnEndPitch, rStep);
//    #endif
    for (int i=iStart; i < nPoints; i++)
    {
        aPoints[i] = (int)floor(yValue+0.5);
        yValue += rStep;
    }
    //force last point to be a root note
    aPoints[nPoints-1] = dnEndPitch;

}

//---------------------------------------------------------------------------------------
void Composer::ComputeArch(bool fUp, int iStart, int nPoints, DiatonicPitch dnLowPitch,
                              DiatonicPitch dnHighPitch, std::vector<DiatonicPitch>& aPoints)
{
    // Arch. An arch will be defined the amplitude, the center beat, and the direction
    // (up/down). Also by start pitch, top/bottom pitch, and end pitch
    // I will use a second degree polinimio. I approximate it by using the Lagrange method.
    // The resulting polinom is
    //      P(x) = RootPitch + ((4 * maxPitch * x * (numPoints - x))/numPoints**2)
    //           = a3 + ((a1 * x * (numPoints - x))/ a2)
    //      a1 = 4 * maxPitch
    //      a2 = numPoints**2
    //      a3 = RootPitch
    //      a4 = numPoints

//    #if (TRACE_COMPOSER == 1)
//    wxLogMessage("[Composer::ComputeArch] fUp=%s, iStart=%d, nPoints=%d, dnLowPitch=%d, dnHighPitch=%d",
//        (fUp ? "Yes" : "No"), iStart, nPoints, dnLowPitch, dnHighPitch);
//    #endif
    float a1 = 4.0 * (float)(dnHighPitch-dnLowPitch);
    float a2 = (float)(nPoints * nPoints);
    float a3 = (float)dnLowPitch;
    float a4 = (float)nPoints;
    float x = 0.0;
    for (int i=iStart; i < iStart+nPoints; i++, x+=1.0)
    {
        float y = a3 + ((a1 * x * (a4 - x)) / a2);
        aPoints[i] = (int)floor(y + 0.5);
    }
    //force last point to be a root note
    aPoints[iStart+nPoints-1] = (fUp ? dnLowPitch : dnHighPitch);

}

//---------------------------------------------------------------------------------------
FPitch Composer::NearestNoteOnChord(DiatonicPitch nPoint, ImoNote* pNotePrev,
                                    ImoNote* WXUNUSED(pNoteCur),
                                    std::vector<FPitch>& aOnChordPitch)
{
//    #if (TRACE_COMPOSER == 1)
//    wxLogMessage("[Composer::NearestNoteOnChord] nPoint=%d", nPoint );
//    #endif
//
    // if note is tied to previous one, return previous note pitch
    if (pNotePrev && pNotePrev->is_tied_next() && pNotePrev->is_pitch_defined())
    {
//    #if (TRACE_COMPOSER == 1)
//        wxLogMessage("[Composer::NearestNoteOnChord] Previous note = %s", (pNotePrev->get_fpitch()).to_abs_ldp_name().wx_str());
//    #endif
        return pNotePrev->get_fpitch();
    }

    for (int i=0; i < (int)aOnChordPitch.size(); i++)
    {
        DiatonicPitch dnCur = aOnChordPitch[i].to_diatonic_pitch();
        if (nPoint == dnCur)
            return aOnChordPitch[i];
        else if (nPoint < dnCur)
        {
            // The nearest one is this one or the previous one
            // If no previous one, return this one
            if (i == 0)
                return aOnChordPitch[i];
            // there is a 'previous one'. So lets compute differences
            DiatonicPitch dnPrev = aOnChordPitch[i-1].to_diatonic_pitch();
            if (nPoint - dnPrev < dnCur - nPoint)
                return aOnChordPitch[i-1];
            else
                return aOnChordPitch[i];
        }
    }

    //requested note is out of range. Return maximum allowed one
    //return aOnChordPitch[aOnChordPitch.size()-1];
    return aOnChordPitch[aOnChordPitch.size()-1];
}

//---------------------------------------------------------------------------------------
void Composer::InstantiateWithNote(ImoScore* pScore, FPitch fp)
{
    // This method is used only to generate images for documentation.
    // The idea is to instantiate the score with the same pitch for all
    // notes, to create scores with the rhymth pattern

    // Loop to instantiate notes
    ColStaffObjs* pColStaffObjs = pScore->get_staffobjs_table();
    ColStaffObjsIterator it = pColStaffObjs->begin();
    while(it != pColStaffObjs->end())
    {
        ImoObj* pImo = (*it)->imo_object();
        if (pImo->is_note())
        {
            // It is a note. Instantiate it
            ImoNote* pNote = static_cast<ImoNote*>(pImo);
            set_pitch(pNote, fp);
        }
        ++it;
    }
}

//---------------------------------------------------------------------------------------
void Composer::AssignNonChordNotes(int nNumNotes, ImoNote* pOnChord1, ImoNote* pOnChord2,
                                      ImoNote* pNonChord[], FPitch scale[7])
{
    // Receives the two on-chord notes and the non-chord notes between them, and assign
    // the pitch to all notes
    // The first on-chord note can be nullptr (first anacruxis measure)
    // The number of non-chord notes received is in 'nNumNotes'

    //case: no non-chord notes. Nothing to do
    if (nNumNotes == 0)
    {
    #if (TRACE_PITCH == 1)
        LOMSE_LOG_INFO("No non-chord notes. Nothing to do.");
    #endif
        return;
    }

    //case: anacruxis measure
    if (!pOnChord1)
    {
    #if (TRACE_PITCH == 1)
        LOMSE_LOG_INFO("Anacruxis measure");
    #endif

        //we are going to assign an ascending sequence, by step, to finish in the root
        //note (the first on chord note)
        if (nNumNotes == 1)
        {
            //assign root pitch
            #if (TRACE_PITCH == 1)
                FPitch fp = pOnChord2->get_fpitch();
                string sNoteName = fp.to_abs_ldp_name();
                LOMSE_LOG_INFO(
                    "Anacrusis, single non-chord note. Assigned pitch = %d (%s)",
                    int(fp.to_diatonic_pitch()), sNoteName.c_str() );
            #endif
            set_pitch(pNonChord[0], pOnChord2->get_fpitch());
        }
        else
        {
            // ascending sequence of steps
            FPitch nPitch = pOnChord2->get_fpitch();
            for(int i=nNumNotes-1; i >= 0; i--)
            {
                if (!pNonChord[i]->is_pitch_defined())
                {
                    nPitch = MoveByStep(k_down, nPitch, scale);
                    #if (TRACE_PITCH == 1)
                        FPitch fp = nPitch;
                        string sNoteName = fp.to_abs_ldp_name();
                        LOMSE_LOG_INFO(
                            "Anacrusis, non-chord note %d. Assigned pitch = %d (%s)",
                            i, int(fp.to_diatonic_pitch()), sNoteName.c_str() );
                    #endif
                    set_pitch(pNonChord[i], nPitch);
                }
            }
        }
        return;
    }

    // Compute inteval formed by on-chord notes
    FPitch ap1 = pOnChord1->get_fpitch();
    FPitch ap2 = pOnChord2->get_fpitch();
    int nDIntval = ap2.to_diatonic_pitch() - ap1.to_diatonic_pitch();
    int nAbsIntval = abs(nDIntval) + 1;

    //Choose non-chord notes type depending on interval formed by on-chord notes
    if (nAbsIntval == 1)
    {
        //unison
        //If one or two notes use neighboring notes. If more notes
        //choose between neighboring notes or on-chord arpege
        if (nNumNotes < 3)
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        return;
    }

    else if (nAbsIntval == 2)
    {
        //second
        //If one note there are several possibilities (anticipation / suspension /
        //retardation / appogiatura) but I will just use a on-chord tone (a third apart)
        if (nNumNotes == 1)
            ThirdFifthNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        return;
    }

    else if (nAbsIntval == 3)
    {
        //third
        //If one note use a passing note, else we could use two passing notes by
        //chromatic step, but chromatic accidentals could not be appropriate for
        //lower music reading levels; therefore I will use a neighboring tone
        if (nNumNotes == 1)
            PassingNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        return;
    }

    else if (nAbsIntval == 4)
    {
        //fourth
        //interval 1:   Third / Appoggiatura
        //interval 2:   Two passing tones (by step) / Appoggiatura
        if (nNumNotes == 1)
            ThirdFifthNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        else if (nNumNotes == 1)
            PassingNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        return;
    }

    else if (nAbsIntval == 5)
    {
        //fifth
        //interval 1:   Third / Appoggiatura
        //interval 2:   Third+Fifth / Double appoggiatura
        //interval 3:   three passing tones (by step)
        if (nNumNotes < 3)
            ThirdFifthNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        else if (nNumNotes == 3)
            PassingNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        return;
    }

    else if (nAbsIntval == 6)
    {
        //sixth
        //interval 1:   Appoggiatura
        //interval 2:   Double appoggiatura
        //interval 4:   four passing tones (by step)
        if (nNumNotes < 3)
            ThirdFifthNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        else if (nNumNotes == 4)
            PassingNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        return;
    }

    else
    {
        LOMSE_LOG_ERROR("[Composer::AssignNonChordNotes] Program error: "
                        "case not defined: Intval %d, num.notes %d",
                        nAbsIntval, nNumNotes );
        NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, scale);
        return;
    }
}

//---------------------------------------------------------------------------------------
void Composer::NeightboringNotes(int nNumNotes, ImoNote* pOnChord1, ImoNote* WXUNUSED(pOnChord2),
                                    ImoNote* pNonChord[], FPitch scale[7])
{
    // Receives the two on-chord notes and the non-chord notes between them, and assign
    // the pitch to all notes. The number of non-chord notes received is in 'nNumNotes'

    if(nNumNotes < 1 || nNumNotes > 3)
    {
        LOMSE_LOG_ERROR("[Composer::NeightboringNotes] nNumNotes (%d) not between 1 and 3",
                        nNumNotes);
    }

    bool fUpStep = RandomGenerator::flip_coin();
    FPitch ap = pOnChord1->get_fpitch();
    FPitch pitch[2];
    pitch[0] = MoveByStep(fUpStep, ap, scale);
    pitch[1] = MoveByStep(!fUpStep, ap, scale);
    for (int i=0; nNumNotes > 0; --nNumNotes, ++i)
    {
        #if (TRACE_PITCH == 1)
            FPitch fp = pitch[nNumNotes % 2];
            string sNoteName = fp.to_abs_ldp_name();
            LOMSE_LOG_INFO(
                "Neightboring notes, note %d. Assigned pitch = %d (%s)",
                i, int(fp.to_diatonic_pitch()), sNoteName.c_str() );
        #endif
        set_pitch(pNonChord[i], pitch[nNumNotes % 2]);
    }
}

//---------------------------------------------------------------------------------------
void Composer::PassingNotes(bool fUp, int nNumNotes, ImoNote* pOnChord1, ImoNote* WXUNUSED(pOnChord2),
                               ImoNote* pNonChord[], FPitch scale[7])
{
    // Receives the two on-chord notes and the non-chord notes between them, and assign
    // the pitch to all notes. The number of non-chord notes received is in 'nNumNotes'

    wxASSERT(nNumNotes > 0);

    // passing note
    FPitch apNewPitch = pOnChord1->get_fpitch();
    #if (TRACE_PITCH == 1)
        FPitch fp = apNewPitch;
        string sNoteName = fp.to_abs_ldp_name();
        LOMSE_LOG_INFO(
            "Passing notes, note 0. Assigned pitch = %d (%s)",
            int(fp.to_diatonic_pitch()), sNoteName.c_str() );
    #endif
    set_pitch(pNonChord[0], MoveByStep(fUp, apNewPitch, scale));

    // two passing notes
    for (int i=1; i < nNumNotes; i++)
    {
        apNewPitch = MoveByStep(fUp, apNewPitch, scale);
        set_pitch(pNonChord[i], apNewPitch);
        #if (TRACE_PITCH == 1)
            FPitch fp = apNewPitch;
            string sNoteName = fp.to_abs_ldp_name();
            LOMSE_LOG_INFO(
                "Passing notes, note 0. Assigned pitch = %d (%s)",
                i, int(fp.to_diatonic_pitch()), sNoteName.c_str() );
        #endif
    }
}

//---------------------------------------------------------------------------------------
void Composer::ThirdFifthNotes(bool fUp, int nNumNotes, ImoNote* pOnChord1,
                               ImoNote* WXUNUSED(pOnChord2),
                               ImoNote* pNonChord[], FPitch scale[7])
{
    // Receives the two on-chord notes and the non-chord notes between them, and assign
    // the pitch to all notes. The number of non-chord notes received is in 'nNumNotes'

    wxASSERT(nNumNotes == 1 || nNumNotes == 2);

    // third
    FPitch pitch = MoveByStep(fUp, pOnChord1->get_fpitch(), scale);  //second
    pitch = MoveByStep(fUp, pitch, scale);     //third
    set_pitch(pNonChord[0], pitch);
    #if (TRACE_PITCH == 1)
        FPitch fp = pitch;
        string sNoteName = fp.to_abs_ldp_name();
        LOMSE_LOG_INFO(
            "Third note. Assigned pitch = %d (%s)",
            int(fp.to_diatonic_pitch()), sNoteName.c_str() );
    #endif
    if (nNumNotes == 1) return;

    // fifth
    pitch = MoveByStep(fUp, pitch, scale);     //fourth
    pitch = MoveByStep(fUp, pitch, scale);     //fifth
    set_pitch(pNonChord[1], pitch);
    #if (TRACE_PITCH == 1)
        fp = pitch;
        sNoteName = fp.to_abs_ldp_name();
        LOMSE_LOG_INFO(
            "Fifth note. Assigned pitch = %d (%s)",
            int(fp.to_diatonic_pitch()), sNoteName.c_str() );
    #endif
}

//---------------------------------------------------------------------------------------
void Composer::set_pitch(ImoNote* pNote, FPitch fp)
{
    int nAccidentals[7];
    KeyUtilities::get_accidentals_for_key(m_nKey, nAccidentals);
    EAccidentals acc = EAccidentals( nAccidentals[fp.step()] );
    if (!pNote->is_pitch_defined())
    {
        pNote->set_notated_pitch(fp.step(), fp.octave(), k_no_accidentals);
        pNote->set_actual_accidentals(acc);
    }

    if (pNote->is_tied_next())
    {
        ImoTie* pTie = pNote->get_tie_next();
        pNote = pTie->get_end_note();
        pNote->set_notated_pitch(fp.step(), fp.octave(), k_no_accidentals);
        pNote->set_actual_accidentals(acc);
    }
}



}   //namespace lenmus
