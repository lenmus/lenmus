//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2012 Cecilio Salmeron. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this 
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#ifdef _LM_DEBUG_

#include <UnitTest++.h>
#include <iostream>

//classes related to these tests
#include "lomse_injectors.h"
#include "lomse_compiler.h"

//to delete singletons
#include "lomse_factory.h"
#include "lomse_elements.h"


using namespace UnitTest;
using namespace std;
using namespace lomse;


class AnalyserCaseTestFixture
{
public:

    AnalyserCaseTestFixture()     //SetUp fixture
    {
    }

    ~AnalyserCaseTestFixture()    //TearDown fixture
    {
        delete Factory::instance();
    }

    void CheckScore(const string& filename)
    {
        string path = "../../../../../test-scores/" + filename;
        stringstream errormsg;
        DocumentScope documentScope(errormsg);
        LdpCompiler compiler(documentScope);
        LdpTree* tree = compiler.compile_file(path);
        CHECK( tree != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //CHECK( errormsg.str() == "" );
        delete tree->get_root();
        delete tree;
    }

};

SUITE(AnalyserCaseTest)
{
    TEST_FIXTURE(AnalyserCaseTestFixture, T00010_EmptyScoreRendersOneStaff)
    {
        CheckScore("00010-empty-renders-one-staff.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00011_EmptyScoreFillPage)
    {
        CheckScore("00011-empty-fill-page.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00020_SpaceBeforeClef)
    {
        CheckScore("00020-space-before-clef.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00021_SpacingInProlog)
    {
        CheckScore("00021-spacing-in-prolog.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00022_SpaceAfterPrologOneNote)
    {
        CheckScore("00022-spacing-in-prolog-one-note.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00023_SameDurationNotesEquallySpaced)
    {
        CheckScore("00023-same-duration-notes-equally-spaced.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00024_NotesSpacingProportionalToNotesDuration)
    {
        CheckScore("00024-notes-spacing-proportional-to-notes-duration.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00025_AccidentalsDoNotAlterSpacing)
    {
        CheckScore("00025-accidentals-do-no-alter-spacing.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00026_AccidentalsDoNotAlterFixedSpacing)
    {
        CheckScore("00026-accidentals-do-no-alter-fixed-spacing.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00027_Spacing_notes_with_figured_bass)
    {
        CheckScore("00027-spacing-notes-with-figured-bass.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00030_ChordNotesAreAligned)
    {
        CheckScore("00030-chord-notes-are-aligned.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00031_ChordStemUpNoteReversedNoFlag)
    {
        CheckScore("00031-chord-stem-up-note-reversed-no-flag.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00032_ChordStemDownNoteReversedNoFlag)
    {
        CheckScore("00032-chord-stem-down-note-reversed-no-flag.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00033_ChordsWithReversedNotesDoNotOverlap)
    {
        CheckScore("00033-chords-with-reversed-notes-do-not-overlap.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00034_ChordWithAccidentalsAligned)
    {
        CheckScore("00034-chord-with-accidentals-aligned.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00038_ChordsWithAccidentalsAndReversedNotesAligned)
    {
        CheckScore("00038-chords-with-accidentals-and-reversed-notes-aligned.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00040_ClefBetweenNotesProperlySpacedWhenEnoughSpace)
    {
        CheckScore("00040-clef-between-notes-properly-spaced-when-enough-space.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00041_ClefBetweenNotesProperlySpacedWhenRemovingVariableSpace)
    {
        CheckScore("00041-clef-between-notes-properly-spaced-when-removing-variable-space.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00042_ClefBetweenNotesAddsLittleSpacedWhenNotEnoughSpace)
    {
        CheckScore("00042-clef-between-notes-adds-little-space-when-not-enough-space.lms");
    }

        // vertical alignment

    TEST_FIXTURE(AnalyserCaseTestFixture, T00101_VerticalRightAlignmentPrologOneNote)
    {
        CheckScore("00101-vertical-right-alignment-prolog-one-note.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00102_VerticalRightAlignmentSameTimePositions)
    {
        CheckScore("00102-vertical-right-alignment-same-time-positions.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00103_VerticalRightAlignmentDifferentTimePositions)
    {
        CheckScore("00103-vertical-right-alignment-different-time-positions.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00105_VerticalRightAlignmentWhenClefsBetweenNotes)
    {
        CheckScore("00105-vertical-right-alignment-when-clefs-between-notes.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00106_ClefFollowsNoteWhenNoteDisplaced)
    {
        CheckScore("00106-clef-follows-note-when-note-displaced.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00107_PrologProperlyAlignedInSecondSystem)
    {
        CheckScore("00107-prolog-properly-aligned-in-second-system.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00200_BarsGoOneAfterTheOther)
    {
        CheckScore("00200-bars-go-one-after-the-other.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00201_SystemsAreJustified)
    {
        CheckScore("00201-systems-are-justified.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00202_LongSingleBarIsSplitted)
    {
        CheckScore("00202-long-single-bar-is-splitted.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00203_repositioning_at_justification)
    {
        CheckScore("00203-repositioning-at-justification.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T00000_ErrorAlignRests)
    {
        CheckScore("00000-error-align-rests.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80010_accidental_after_barline)
    {
        CheckScore("80010-accidental-after-barline.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80011_accidentals)
    {
        CheckScore("80011-accidentals.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80020_chord_no_stem_no_flag)
    {
        CheckScore("80020-chord-no-stem-no-flag.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80021_chord_stem_up_no_flag)
    {
        CheckScore("80021-chord-stem-up-no-flag.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80022_chord_stem_down_no_flag)
    {
        CheckScore("80022-chord-stem-down-no-flag.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80023_chord_stem_up_note_reversed_no_flag)
    {
        CheckScore("80023-chord-stem-up-note-reversed-no-flag.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80024_chord_stem_down_note_reversed_no_flag)
    {
        CheckScore("80024-chord-stem-down-note-reversed-no-flag.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80025_chord_stem_up_no_flag_accidental)
    {
        CheckScore("80025-chord-stem-up-no-flag-accidental.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80026_chord_flags)
    {
        CheckScore("80026-chord-flags.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80027_chord_spacing)
    {
        CheckScore("80027-chord-spacing.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80028_chord_notes_ordering)
    {
        CheckScore("80028-chord-notes-ordering.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80030_tuplet_triplets)
    {
        CheckScore("80030-tuplet-triplets.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80031_tuplet_duplets)
    {
        CheckScore("80031-tuplet-duplets.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80032_tuplet_tuplet)
    {
        CheckScore("80032-tuplet-tuplet.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80040_beams)
    {
        CheckScore("80040-beams.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80041_chords_beamed)
    {
        CheckScore("80041-chords-beamed.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80042_beams)
    {
        CheckScore("80042-beams.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80043_beam_4s_q)
    {
        CheckScore("80043-beam-4s-q.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80050_ties)
    {
        CheckScore("80050-ties.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80051_tie_bezier)
    {
        CheckScore("80051-tie-bezier.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80052_tie_bezier_break)
    {
        CheckScore("80052-tie-bezier-break.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80053_tie_bezier_barline)
    {
        CheckScore("80053-tie-bezier-barline.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80054_tie_after_barline)
    {
        CheckScore("80054-tie-after-barline.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80060_go_back)
    {
        CheckScore("80060-go-back.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80070_some_time_signatures)
    {
        CheckScore("80070-some-time-signatures.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80071_12_8_time_signature)
    {
        CheckScore("80071-12-8-time-signature.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80072_2_4_time_signature)
    {
        CheckScore("80072-2-4-time-signature.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80080_one_instr_2_staves)
    {
        CheckScore("80080-one-instr-2-staves.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80081_two_instr_3_staves)
    {
        CheckScore("80081-two-instr-3-staves.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80082_choir_STB_piano)
    {
        CheckScore("80082-choir-STB-piano.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80083_critical_line)
    {
        CheckScore("80083-critical-line.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80090_all_rests)
    {
        CheckScore("80090-all-rests.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80091_rests_in_beam)
    {
        CheckScore("80091-rests-in-beam.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80092_short_rests_in_beam)
    {
        CheckScore("80092-short-rests-in-beam.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80100_spacer)
    {
        CheckScore("80100-spacer.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80110_graphic_line_text)
    {
        CheckScore("80110-graphic-line-text.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80111_line_after_barline)
    {
        CheckScore("80111-line-after-barline.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80120_fermatas)
    {
        CheckScore("80120-fermatas.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80130_metronome)
    {
        CheckScore("80130-metronome.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80131_metronome)
    {
        CheckScore("80131-metronome.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80132_metronome)
    {
        CheckScore("80132-metronome.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80140_text)
    {
        CheckScore("80140-text.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80141_text_titles)
    {
        CheckScore("80141-text-titles.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80150_all_clefs)
    {
        CheckScore("80150-all-clefs.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80151_all_clefs)
    {
        CheckScore("80151-all-clefs.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80160_textbox)
    {
        CheckScore("80160-textbox.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80161_textbox_with_anchor_line)
    {
        CheckScore("80161-textbox-with-anchor-line.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80162_stacked_textboxes)
    {
        CheckScore("80162-stacked-textboxes.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80170_figured_bass)
    {
        CheckScore("80170-figured-bass.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80171_figured_bass_several)
    {
        CheckScore("80171-figured-bass-several.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80172_figured_bass_line)
    {
        CheckScore("80172-figured-bass-line.lms");
    }

    TEST_FIXTURE(AnalyserCaseTestFixture, T80180_new_system_tag)
    {
        CheckScore("80180-new-system-tag.lms");
    }

};

#endif  // _LM_DEBUG_

