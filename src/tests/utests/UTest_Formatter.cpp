//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
//-------------------------------------------------------------------------------------

#include "wx/wxprec.h"
#include "wx/cppunit.h"
#include "wx/filename.h"

//classes related to these tests
#include "../../score/Score.h"
#include "../../score/VStaff.h"
#include "../../app/ScoreDoc.h"
#include "../../graphic/GraphicManager.h"
#include "../../graphic/AggDrawer.h"
#include "../../graphic/Formatter4.h"
#include "../../graphic/BoxScore.h"
#include "../../graphic/SystemFormatter.h"
#include "../../ldp_parser/LDPParser.h"
#include "../../graphic/SystemFormatter.h"

// access to paths
#include "../../globals/Paths.h"
extern lmPaths* g_pPaths;

//-------------------------------------------------------------------------------------
// User stories and tests for score layoutting
//
// Empty scores
// -------------
//
// 0. Depending on options, an empty score will be rendered either as a single system
//  or as page filled with empty systmed (manuscript paper).
//      + 10 EmptyScoreRendersOneStaff );
//      + 11 EmptyScoreFillPage );
//
//
// Spacing a single line (lmLineSpacer)
// -------------------------------------
//
// 1. At start of score there is some spacing before the clef. The key signature and
//  the time signature follows, also with some space between them. Finally, before the
//  first note, there is some greater space.
//      + 20 space-before-clef
//      + 21 spacing-in-prolog
//      + 22 spacing-in-prolog-one-note
//
// 2. Notes of same duration are equally spaced. If some notes have accidentals, notes
//  spacing is not altered if there is enought space between notes. For notes with
//  different duration spacing is proportional to duration
//      + 23 same-duration-notes-equally-spaced
//      + 24 notes-spacing-proportional-to-notes-duration
//      + 25 accidentals-do-no-alter-spacing
//      + 26 accidentals-do-no-alter-fixed-spacing
//
// 3. Notes in chord are vertically aligned. If one note is reversed, that note should
//  not alter chords spacing unless not enough space
//      + 30 chord-notes-are-aligned
//      + 31 chord-stem-up-note-reversed-no-flag
//      + 32 chord-stem-down-note-reversed-no-flag
//      + 33 chords-with-reversed-notes-do-not-overlap
//  *** + 34 chord-with-accidentals-aligned
//  *** + 38 chords-with-accidentals-and-reversed-notes-aligned
//
// 4. Non-timed objects behave as if they were right aligned, joined to next timed
//  object to arrive. Spacing between the timed objects that enclose these non-timed
//  should be maintained as if the non-timed objs didn't exist. If not enough space,
//  variable space should be removed, starting from last non-timed and continuing
//  backwards.
//      + 40 clef-between-notes-properly-spaced-when-enough-space
//      + 41 clef-between-notes-properly-spaced-when-removing-variable-space
//      + 42 clef-between-notes-adds-little-space-when-not-enough-space
//      - 43 two-clefs-between-notes-adds-more-space
//      - 44 accidental-in-next-note-shifts-back-previous-clef
//      - 45 reversed-note-in-next-chord-shifts-back-previous-clef
//      - 46 accidental-in-next-chord-shifts-back-previous-clef
//
//
// Vertical alignment when the system has more than one staff (lmColumnFormatter)
// ------------------------------------------------------------------------------
//
// 10. All notes at the same timepos must be vertically aligned
//      + 101 vertical-right-alignment-prolog-one-note
//      + 102 vertical-right-alignment-same-time-positions
//      + 103 vertical-right-alignment-different-time-positions
//  *** - 104 vertical-right-alignment-when-accidental-requires-more-space
//      + 105 vertical-right-alignment-when-clefs-between-notes
//      + 106 clef-follows-note-when-note-displaced
//      - 107 prolog-properly-aligned-in-second-system
//
//
// Systems justification (lmColumnFormatter, lmLineResizer)
// --------------------------------------------------------
//
// 20. A system usually contains a few bars. If not enough space for a single bar
//  split the system at timepos a common to all staves
//      + 200 bars-go-one-after-the-other
//      + 201 systems-are-justified
//      - 202 long-single-bar-is-splitted
//      - 203 attached-objs-repositioned-at-justification
//
//
//
// Scores for regression tests
// ----------------------------
//      - 80010-accidental-after-barline
//      - 80011-accidentals
//      - 80020-chord-no-stem-no-flag
//      - 80021-chord-stem-up-no-flag
//      - 80022-chord-stem-down-no-flag
//      - 80023-chord-stem-up-note-reversed-no-flag
//      - 80024-chord-stem-down-note-reversed-no-flag
//      - 80025-chord-stem-up-no-flag-accidental
//      - 80026-chord-flags
//      - 80027-chord-spacing
//      - 80028-chord-notes-ordering
//      - 80030-tuplet-triplets
//      - 80031-tuplet-duplets
//      - 80032-tuplet-tuplet
//      - 80040-beams
//      - 80041-chords-beamed
//      - 80042-beams
//      - 80043-beam-4s-q
//      - 80050-ties
//      - 80051-tie-bezier
//      - 80052-tie-bezier-break
//      - 80053-tie-bezier-barline
//      - 80054-tie-after-barline
//      - 80060-go-back
//      - 80070-some-time-signatures
//      - 80071-12-8-time-signature
//      - 80072-2-4-time-signature
//      - 80080-one-instr-2-staves
//      - 80081-two-instr-3-staves
//      - 80082-choir-STB-piano
//      - 80083-critical-line
//      - 80090-all-rests
//      - 80091-rests-in-beam
//      - 80092-short-rests-in-beam
//      - 80100-spacer
//      - 80110-graphic-line-text
//      - 80111-line-after-barline
//      - 80120-fermatas
//      - 80130-metronome
//      - 80131-metronome
//      - 80132-metronome
//      - 80140-text
//      - 80141-text-titles
//      - 80150-all-clefs
//      - 80151-all-clefs
//      - 80160-textbox
//      - 80161-textbox-with-anchor-line
//      - 80162-stacked-textboxes
//      - 80170-figured-bass
//      - 80171-figured-bass-several
//      - 80172-figured-bass-line
//      - 80180-new-system-tag
//
// Scores for other unit tests - 9xxxx
// --------------------------------------------------------
//
// tests for class lmTimeGridTable
//      + 90001-two-notes-different-duration
//      + 90002-several-lines-with-different-durations
//





class lmFormatter5Test : public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( lmFormatter5Test );

        //empty scores
        CPPUNIT_TEST( T00010_EmptyScoreRendersOneStaff );
        CPPUNIT_TEST( T00011_EmptyScoreFillPage );

        //spacing a single line
        CPPUNIT_TEST( T00020_SpaceBeforeClef );
        CPPUNIT_TEST( T00021_SpacingInProlog );
        CPPUNIT_TEST( T00022_SpaceAfterPrologOneNote );
        CPPUNIT_TEST( T00023_SameDurationNotesEquallySpaced );
        CPPUNIT_TEST( T00024_NotesSpacingProportionalToNotesDuration );
        CPPUNIT_TEST( T00025_AccidentalsDoNotAlterSpacing );
        CPPUNIT_TEST( T00026_AccidentalsDoNotAlterFixedSpacing );
        CPPUNIT_TEST( T00030_ChordNotesAreAligned );
        CPPUNIT_TEST( T00031_ChordStemUpNoteReversedNoFlag );
        CPPUNIT_TEST( T00032_ChordStemDownNoteReversedNoFlag );
        CPPUNIT_TEST( T00033_ChordsWithReversedNotesDoNotOverlap );
        CPPUNIT_TEST( T00034_ChordWithAccidentalsAligned );
        CPPUNIT_TEST( T00038_ChordsWithAccidentalsAndReversedNotesAligned );

        CPPUNIT_TEST( T00040_ClefBetweenNotesProperlySpacedWhenEnoughSpace );
        CPPUNIT_TEST( T00041_ClefBetweenNotesProperlySpacedWhenRemovingVariableSpace );
        CPPUNIT_TEST( T00042_ClefBetweenNotesAddsLittleSpacedWhenNotEnoughSpace );

        // vertical alignment
        CPPUNIT_TEST( T00101_VerticalRightAlignmentPrologOneNote );
        CPPUNIT_TEST( T00102_VerticalRightAlignmentSameTimePositions );
        CPPUNIT_TEST( T00103_VerticalRightAlignmentDifferentTimePositions );
        //CPPUNIT_TEST( T00104_VerticalRightAlignmentWhenAccidentalRequiresMoreSpace );
        CPPUNIT_TEST( T00105_VerticalRightAlignmentWhenClefsBetweenNotes );
        CPPUNIT_TEST( T00106_ClefFollowsNoteWhenNoteDisplaced );
        CPPUNIT_TEST( T00107_PrologProperlyAlignedInSecondSystem );

        // systems justification (lmLineResizer object)
        CPPUNIT_TEST( T00200_BarsGoOneAfterTheOther );
        CPPUNIT_TEST( T00201_SystemsAreJustified );
        CPPUNIT_TEST( T00202_LongSingleBarIsSplitted );

        //other
        CPPUNIT_TEST( T00000_ErrorAlignRests );

        // not used to drive development. Consider them 'regression tests'
        CPPUNIT_TEST( T80010_accidental_after_barline );
        CPPUNIT_TEST( T80011_accidentals );
        CPPUNIT_TEST( T80020_chord_no_stem_no_flag );
        CPPUNIT_TEST( T80021_chord_stem_up_no_flag );
        CPPUNIT_TEST( T80022_chord_stem_down_no_flag );
        CPPUNIT_TEST( T80023_chord_stem_up_note_reversed_no_flag );
        CPPUNIT_TEST( T80024_chord_stem_down_note_reversed_no_flag );
        CPPUNIT_TEST( T80025_chord_stem_up_no_flag_accidental );
        CPPUNIT_TEST( T80026_chord_flags );
        CPPUNIT_TEST( T80027_chord_spacing );
        CPPUNIT_TEST( T80028_chord_notes_ordering );
        CPPUNIT_TEST( T80030_tuplet_triplets );
        CPPUNIT_TEST( T80031_tuplet_duplets );
        CPPUNIT_TEST( T80032_tuplet_tuplet );
        CPPUNIT_TEST( T80040_beams );
        CPPUNIT_TEST( T80041_chords_beamed );
        CPPUNIT_TEST( T80042_beams );
        CPPUNIT_TEST( T80043_beam_4s_q );
        CPPUNIT_TEST( T80050_ties );
        CPPUNIT_TEST( T80051_tie_bezier );
        CPPUNIT_TEST( T80052_tie_bezier_break );
        CPPUNIT_TEST( T80053_tie_bezier_barline );
        CPPUNIT_TEST( T80054_tie_after_barline );
        CPPUNIT_TEST( T80060_go_back );
        CPPUNIT_TEST( T80070_some_time_signatures );
        CPPUNIT_TEST( T80071_12_8_time_signature );
        CPPUNIT_TEST( T80072_2_4_time_signature );
        CPPUNIT_TEST( T80080_one_instr_2_staves );
        CPPUNIT_TEST( T80081_two_instr_3_staves );
        CPPUNIT_TEST( T80082_choir_STB_piano );
        CPPUNIT_TEST( T80083_critical_line );
        CPPUNIT_TEST( T80090_all_rests );
        CPPUNIT_TEST( T80091_rests_in_beam );
        CPPUNIT_TEST( T80092_short_rests_in_beam );
        CPPUNIT_TEST( T80100_spacer );
        CPPUNIT_TEST( T80110_graphic_line_text );
        CPPUNIT_TEST( T80111_line_after_barline );
        CPPUNIT_TEST( T80120_fermatas );
        CPPUNIT_TEST( T80130_metronome );
        CPPUNIT_TEST( T80131_metronome );
        CPPUNIT_TEST( T80132_metronome );
        CPPUNIT_TEST( T80140_text );
        CPPUNIT_TEST( T80141_text_titles );
        CPPUNIT_TEST( T80150_all_clefs );
        CPPUNIT_TEST( T80151_all_clefs );
        CPPUNIT_TEST( T80160_textbox );
        CPPUNIT_TEST( T80161_textbox_with_anchor_line );
        CPPUNIT_TEST( T80162_stacked_textboxes );
        CPPUNIT_TEST( T80170_figured_bass );
        CPPUNIT_TEST( T80171_figured_bass_several );
        CPPUNIT_TEST( T80172_figured_bass_line );
        CPPUNIT_TEST( T80180_new_system_tag );


    CPPUNIT_TEST_SUITE_END();

    wxSize m_ScoreSize;
    double m_rScale;
    lmScore* m_pScore;
    wxString m_sTestName;
    wxString m_sTestNum;
    lmFormatter5* m_pFormatter;
    lmBoxScore* m_pBoxScore;
    lmSystemFormatter* m_pSysFmt;

    //void SaveScoreBitmap(lmGraphicManager* pGM)
    //{
    //    wxBitmap* pBitmap = pGM->RenderScore(1);
    //    wxString sPath = g_pPaths->GetTestScoresPath();
    //    wxFileName oFilename(sPath, _T("zimg-")+m_sTestName, _T("bmp"), wxPATH_NATIVE);
    //    pBitmap->SaveFile(oFilename.GetFullPath(), wxBITMAP_TYPE_BMP);
    //}

    void LoadScoreForTest(const wxString& sTestNum, const wxString& sTestName)
    {
        DeleteTestData();
        m_sTestNum = sTestNum;
        m_sTestName = sTestName;
        wxString sPath = g_pPaths->GetTestScoresPath();
        wxString sFilename = m_sTestNum + _T("-") + m_sTestName;
        wxFileName oFilename(sPath, sFilename, _T("lms"), wxPATH_NATIVE);
        lmLDPParser parser;
        m_pScore = parser.ParseFile( oFilename.GetFullPath() );
        CPPUNIT_ASSERT( m_pScore != NULL );

        lmAggDrawer* pDrawer = new lmAggDrawer(m_ScoreSize.x, m_ScoreSize.y, m_rScale);
        lmPaper m_oPaper;
        m_oPaper.SetDrawer(pDrawer);
        m_pFormatter = new lmFormatter5(&m_oPaper);
        m_pBoxScore = m_pScore->Layout(&m_oPaper, m_pFormatter);
        m_pSysFmt = m_pFormatter->GetSystemFormatter(0);
    }

    void CheckLineDataEqual(int iSys, int iCol, CppUnit::SourceLine nSrcLine )
    {
        CPPUNIT_ASSERT( m_pBoxScore != NULL );
        CPPUNIT_ASSERT( m_pBoxScore->GetNumPages() > 0 );
        CPPUNIT_ASSERT( m_pBoxScore->GetNumSystems() > iSys );
        CPPUNIT_ASSERT( m_pSysFmt->GetNumColumns() > iCol );
        CPPUNIT_ASSERT( m_pSysFmt->GetNumLinesInColumn(iCol) > 0 );

        //get actual data
        wxString sActualData =
            m_pFormatter->GetSystemFormatter(iSys)->DumpColumnData(iCol);

        //read reference file to get expected data
        wxString sPath = g_pPaths->GetTestScoresPath();
        wxString sInFile = wxString::Format(_T("ref-%s-%d-%d-%s"),
                                m_sTestNum.c_str(), iSys, iCol, m_sTestName.c_str() );
        wxFileName oFilename(sPath, sInFile, _T("txt"), wxPATH_NATIVE);
        wxString sExpectedData;
        wxFFile file(oFilename.GetFullPath());
        if ( !file.IsOpened() || !file.ReadAll(&sExpectedData) )
        {
            SaveAsActualData(sActualData, iSys, iCol);
            ::CppUnit::Asserter::fail("Reference LineTable data cannot be read", nSrcLine );
        }

        //compare data
        if (sExpectedData != sActualData)
        {
            SaveAsActualData(sActualData, iSys, iCol);
            ::CppUnit::Asserter::fail("No match with expected LineTable data", nSrcLine );
        }
    }

    #define LM_ASSERT_LINE_DATA_EQUAL( iSys, iCol) \
        CheckLineDataEqual(iSys, iCol, CPPUNIT_SOURCELINE())

    void CheckScoreDataEqual(CppUnit::SourceLine nSrcLine )
    {
        CPPUNIT_ASSERT( m_pBoxScore != NULL );
        CPPUNIT_ASSERT( m_pBoxScore->GetNumPages() > 0 );
        CPPUNIT_ASSERT( m_pBoxScore->GetNumSystems() > 0 );
        CPPUNIT_ASSERT( m_pSysFmt->GetNumColumns() > 0 );

        //get actual data
        wxString sActualData = _T("");

        for (int iSys=0; iSys < m_pBoxScore->GetNumSystems(); iSys++)
        {
            lmSystemFormatter* pSysFmt = m_pFormatter->GetSystemFormatter(iSys);
            for (int iCol=0; iCol < pSysFmt->GetNumColumns(); iCol++)
            {
                sActualData +=
                    m_pFormatter->GetSystemFormatter(iSys)->DumpColumnData(iCol);
            }
        }

        //read reference file to get expected data
        wxString sPath = g_pPaths->GetTestScoresPath();
        wxString sInFile = wxString::Format(_T("ref-%s-%s"),
                                            m_sTestNum.c_str(), m_sTestName.c_str() );
        wxFileName oFilename(sPath, sInFile, _T("txt"), wxPATH_NATIVE);
        wxString sExpectedData;
        wxFFile file(oFilename.GetFullPath());
        if ( !file.IsOpened() || !file.ReadAll(&sExpectedData) )
        {
            SaveAsActualScoreData(sActualData);
            ::CppUnit::Asserter::fail("Reference score data cannot be read", nSrcLine );
        }

        //compare data
        if (sExpectedData != sActualData)
        {
            SaveAsActualScoreData(sActualData);
            ::CppUnit::Asserter::fail("No match with expected score data", nSrcLine );
        }
    }

    #define LM_ASSERT_SCORE_DATA_EQUAL() \
        CheckScoreDataEqual(CPPUNIT_SOURCELINE());


    void SaveAsActualData(const wxString& sActualData, int iSys, int iCol)
    {
        wxString sPath = g_pPaths->GetTestScoresPath();
        wxString sOutFile = wxString::Format(_T("dat-%s-%d-%d-%s"),
                                                m_sTestNum.c_str(),
                                                iSys, iCol, m_sTestName.c_str() );
        wxFileName oFilename(sPath, sOutFile, _T("txt"), wxPATH_NATIVE);
        wxFile oFile;
        oFile.Create(oFilename.GetFullPath(), true);    //true=overwrite
        oFile.Open(oFilename.GetFullPath(), wxFile::write);
        if (!oFile.IsOpened())
        {
            wxLogMessage(_T("[lmFormatter5Test::SaveAsActualData] File '%s' could not be openned. Write to file cancelled"),
                oFilename.GetFullPath().c_str());
        }
        else
        {
            oFile.Write(sActualData);
            oFile.Close();
        }
    }

    void SaveAsActualScoreData(const wxString& sActualData)
    {
        wxString sPath = g_pPaths->GetTestScoresPath();
        wxString sOutFile = wxString::Format(_T("dat-%s-%s"),
                                             m_sTestNum.c_str(),
                                             m_sTestName.c_str() );
        wxFileName oFilename(sPath, sOutFile, _T("txt"), wxPATH_NATIVE);
        wxFile oFile;
        oFile.Create(oFilename.GetFullPath(), true);    //true=overwrite
        oFile.Open(oFilename.GetFullPath(), wxFile::write);
        if (!oFile.IsOpened())
        {
            wxLogMessage(_T("[lmFormatter5Test::SaveAsActualScoreData] File '%s' could not be openned. Write to file cancelled"),
                oFilename.GetFullPath().c_str());
        }
        else
        {
            oFile.Write(sActualData);
            oFile.Close();
        }
    }

    void DeleteTestData()
    {
        if (m_pScore)
        {
            delete m_pScore;
            m_pScore = (lmScore*)NULL;
        }
        if (m_pFormatter)
        {
            delete m_pFormatter;
            m_pFormatter = (lmFormatter5*)NULL;
        }
        if (m_pBoxScore)
        {
            delete m_pBoxScore;
            m_pBoxScore = (lmBoxScore*)NULL;
        }
    }


public:
    void setUp()
    {
        m_ScoreSize = wxSize(700, 1000);
        m_rScale = 1.0f * lmSCALE;
        m_pScore = (lmScore*)NULL;
        m_pFormatter = (lmFormatter5*)NULL;
        m_pBoxScore = (lmBoxScore*)NULL;
    }

    void tearDown()
    {
        DeleteTestData();
    }

    void T00010_EmptyScoreRendersOneStaff()
    {
        //an empty score with no options only renders one staff
        LoadScoreForTest(_T("00010"), _T("empty-renders-one-staff"));
        CPPUNIT_ASSERT( m_pSysFmt->GetNumObjectsInColumnLine(0, 0) == 1 );
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00011_EmptyScoreFillPage()
    {
        //an empty score with fill option renders a page full of staves (manuscript paper)

        LoadScoreForTest(_T("00011"), _T("empty-fill-page"));
        CPPUNIT_ASSERT( m_pScore != NULL );

        lmGraphicManager oGraphMngr;
        lmPaper m_oPaper;
        oGraphMngr.PrepareToRender(m_pScore, m_ScoreSize.x, m_ScoreSize.y, m_rScale, &m_oPaper,
                                    lmHINT_FORCE_RELAYOUT);

        lmBoxScore* pBoxScore = oGraphMngr.GetBoxScore();

        CPPUNIT_ASSERT( pBoxScore != NULL );
        CPPUNIT_ASSERT( pBoxScore->GetNumPages() == 1 );
        CPPUNIT_ASSERT( pBoxScore->GetNumSystems() > 1 );

        DeleteTestData();
    }

    void T00020_SpaceBeforeClef()
    {
        LoadScoreForTest(_T("00020"), _T("space-before-clef"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00021_SpacingInProlog()
    {
        LoadScoreForTest(_T("00021"), _T("spacing-in-prolog"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00022_SpaceAfterPrologOneNote()
    {
        LoadScoreForTest(_T("00022"), _T("spacing-in-prolog-one-note"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00023_SameDurationNotesEquallySpaced()
    {
        LoadScoreForTest(_T("00023"), _T("same-duration-notes-equally-spaced"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00024_NotesSpacingProportionalToNotesDuration()
    {
        LoadScoreForTest(_T("00024"), _T("notes-spacing-proportional-to-notes-duration"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00025_AccidentalsDoNotAlterSpacing()
    {
        LoadScoreForTest(_T("00025"), _T("accidentals-do-no-alter-spacing"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00026_AccidentalsDoNotAlterFixedSpacing()
    {
        LoadScoreForTest(_T("00026"), _T("accidentals-do-no-alter-fixed-spacing"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00030_ChordNotesAreAligned()
    {
        LoadScoreForTest(_T("00030"), _T("chord-notes-are-aligned"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00031_ChordStemUpNoteReversedNoFlag()
    {
        LoadScoreForTest(_T("00031"), _T("chord-stem-up-note-reversed-no-flag"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00032_ChordStemDownNoteReversedNoFlag()
    {
        LoadScoreForTest(_T("00032"), _T("chord-stem-down-note-reversed-no-flag"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00033_ChordsWithReversedNotesDoNotOverlap()
    {
        LoadScoreForTest(_T("00033"), _T("chords-with-reversed-notes-do-not-overlap"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00034_ChordWithAccidentalsAligned()
    {
        LoadScoreForTest(_T("00034"), _T("chord-with-accidentals-aligned"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00038_ChordsWithAccidentalsAndReversedNotesAligned()
    {
        LoadScoreForTest(_T("00038"), _T("chords-with-accidentals-and-reversed-notes-aligned"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        LM_ASSERT_LINE_DATA_EQUAL(0, 1);
        DeleteTestData();
    }

    void T00040_ClefBetweenNotesProperlySpacedWhenEnoughSpace()
    {
        LoadScoreForTest(_T("00040"), _T("clef-between-notes-properly-spaced-when-enough-space"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00041_ClefBetweenNotesProperlySpacedWhenRemovingVariableSpace()
    {
        LoadScoreForTest(_T("00041"), _T("clef-between-notes-properly-spaced-when-removing-variable-space"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00042_ClefBetweenNotesAddsLittleSpacedWhenNotEnoughSpace()
    {
        LoadScoreForTest(_T("00042"), _T("clef-between-notes-adds-little-space-when-not-enough-space"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00101_VerticalRightAlignmentPrologOneNote()
    {
        LoadScoreForTest(_T("00101"), _T("vertical-right-alignment-prolog-one-note"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00102_VerticalRightAlignmentSameTimePositions()
    {
        LoadScoreForTest(_T("00102"), _T("vertical-right-alignment-same-time-positions"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00103_VerticalRightAlignmentDifferentTimePositions()
    {
        LoadScoreForTest(_T("00103"), _T("vertical-right-alignment-different-time-positions"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00104_VerticalRightAlignmentWhenAccidentalRequiresMoreSpace()
    {
        LoadScoreForTest(_T("00104"), _T("vertical-right-alignment-when-accidental-requires-more-space"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00105_VerticalRightAlignmentWhenClefsBetweenNotes()
    {
        LoadScoreForTest(_T("00105"), _T("vertical-right-alignment-when-clefs-between-notes"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00106_ClefFollowsNoteWhenNoteDisplaced()
    {
        LoadScoreForTest(_T("00106"), _T("clef-follows-note-when-note-displaced"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    void T00107_PrologProperlyAlignedInSecondSystem()
    {
        LoadScoreForTest(_T("00107"), _T("prolog-properly-aligned-in-second-system"));
        LM_ASSERT_LINE_DATA_EQUAL(1, 0);
        DeleteTestData();
    }

    void T00200_BarsGoOneAfterTheOther()
    {
        LoadScoreForTest(_T("00200"), _T("bars-go-one-after-the-other"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        LM_ASSERT_LINE_DATA_EQUAL(0, 1);
        DeleteTestData();
    }

    void T00201_SystemsAreJustified()
    {
        LoadScoreForTest(_T("00201"), _T("systems-are-justified"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        LM_ASSERT_LINE_DATA_EQUAL(0, 1);
        DeleteTestData();
    }

    void T00202_LongSingleBarIsSplitted()
    {
        LoadScoreForTest(_T("00202"), _T("long-single-bar-is-splitted"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        LM_ASSERT_LINE_DATA_EQUAL(1, 0);
        DeleteTestData();
    }

    void T00000_ErrorAlignRests()
    {
        LoadScoreForTest(_T("00000"), _T("error-align-rests"));
        LM_ASSERT_LINE_DATA_EQUAL(0, 0);
        DeleteTestData();
    }

    //Regression tests -----------------------------------------------------------

    void T80010_accidental_after_barline()
    {
        LoadScoreForTest(_T("80010"), _T("accidental-after-barline"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80011_accidentals()
    {
        LoadScoreForTest(_T("80011"), _T("accidentals"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80020_chord_no_stem_no_flag()
    {
        LoadScoreForTest(_T("80020"), _T("chord-no-stem-no-flag"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80021_chord_stem_up_no_flag()
    {
        LoadScoreForTest(_T("80021"), _T("chord-stem-up-no-flag"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80022_chord_stem_down_no_flag()
    {
        LoadScoreForTest(_T("80022"), _T("chord-stem-down-no-flag"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80023_chord_stem_up_note_reversed_no_flag ()
    {
        LoadScoreForTest(_T("80023"), _T("chord-stem-up-note-reversed-no-flag"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80024_chord_stem_down_note_reversed_no_flag()
    {
        LoadScoreForTest(_T("80024"), _T("chord-stem-down-note-reversed-no-flag"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80025_chord_stem_up_no_flag_accidental()
    {
        LoadScoreForTest(_T("80025"), _T("chord-stem-up-no-flag-accidental"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80026_chord_flags()
    {
        LoadScoreForTest(_T("80026"), _T("chord-flags"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80027_chord_spacing()
    {
        LoadScoreForTest(_T("80027"), _T("chord-spacing"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80028_chord_notes_ordering()
    {
        LoadScoreForTest(_T("80028"), _T("chord-notes-ordering"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80030_tuplet_triplets()
    {
        LoadScoreForTest(_T("80030"), _T("tuplet-triplets"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80031_tuplet_duplets()
    {
        LoadScoreForTest(_T("80031"), _T("tuplet-duplets"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80032_tuplet_tuplet()
    {
        LoadScoreForTest(_T("80032"), _T("tuplet-tuplet"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80040_beams()
    {
        LoadScoreForTest(_T("80040"), _T("beams"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80041_chords_beamed()
    {
        LoadScoreForTest(_T("80041"), _T("chords-beamed"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80042_beams()
    {
        LoadScoreForTest(_T("80042"), _T("beams"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80043_beam_4s_q()
    {
        LoadScoreForTest(_T("80043"), _T("beam-4s-q"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80050_ties()
    {
        LoadScoreForTest(_T("80050"), _T("ties"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80051_tie_bezier()
    {
        LoadScoreForTest(_T("80051"), _T("tie-bezier"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80052_tie_bezier_break()
    {
        LoadScoreForTest(_T("80052"), _T("tie-bezier-break"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80053_tie_bezier_barline()
    {
        LoadScoreForTest(_T("80053"), _T("tie-bezier-barline"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80054_tie_after_barline()
    {
        LoadScoreForTest(_T("80054"), _T("tie-after-barline"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80060_go_back()
    {
        LoadScoreForTest(_T("80060"), _T("go-back"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80070_some_time_signatures()
    {
        LoadScoreForTest(_T("80070"), _T("some-time-signatures"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80071_12_8_time_signature()
    {
        LoadScoreForTest(_T("80071"), _T("12-8-time-signature"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80072_2_4_time_signature()
    {
        LoadScoreForTest(_T("80072"), _T("2-4-time-signature"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80080_one_instr_2_staves()
    {
        LoadScoreForTest(_T("80080"), _T("one-instr-2-staves"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80081_two_instr_3_staves()
    {
        LoadScoreForTest(_T("80081"), _T("two-instr-3-staves"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80082_choir_STB_piano()
    {
        LoadScoreForTest(_T("80082"), _T("choir-STB-piano"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80083_critical_line()
    {
        LoadScoreForTest(_T("80083"), _T("critical-line"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80090_all_rests()
    {
        LoadScoreForTest(_T("80090"), _T("all-rests"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80091_rests_in_beam()
    {
        LoadScoreForTest(_T("80091"), _T("rests-in-beam"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80092_short_rests_in_beam()
    {
        LoadScoreForTest(_T("80092"), _T("short-rests-in-beam"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80100_spacer()
    {
        LoadScoreForTest(_T("80100"), _T("spacer"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80110_graphic_line_text()
    {
        LoadScoreForTest(_T("80110"), _T("graphic-line-text"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80111_line_after_barline()
    {
        LoadScoreForTest(_T("80111"), _T("line-after-barline"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80120_fermatas()
    {
        LoadScoreForTest(_T("80120"), _T("fermatas"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80130_metronome()
    {
        LoadScoreForTest(_T("80130"), _T("metronome"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80131_metronome()
    {
        LoadScoreForTest(_T("80131"), _T("metronome"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80132_metronome()
    {
        LoadScoreForTest(_T("80132"), _T("metronome"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80140_text()
    {
        LoadScoreForTest(_T("80140"), _T("text"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80141_text_titles()
    {
        LoadScoreForTest(_T("80141"), _T("text-titles"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80150_all_clefs()
    {
        LoadScoreForTest(_T("80150"), _T("all-clefs"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80151_all_clefs()
    {
        LoadScoreForTest(_T("80151"), _T("all-clefs"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80160_textbox()
    {
        LoadScoreForTest(_T("80160"), _T("textbox"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80161_textbox_with_anchor_line()
    {
        LoadScoreForTest(_T("80161"), _T("textbox-with-anchor-line"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80162_stacked_textboxes()
    {
        LoadScoreForTest(_T("80162"), _T("stacked-textboxes"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80170_figured_bass()
    {
        LoadScoreForTest(_T("80170"), _T("figured-bass"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80171_figured_bass_several()
    {
        LoadScoreForTest(_T("80171"), _T("figured-bass-several"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80172_figured_bass_line()
    {
        LoadScoreForTest(_T("80172"), _T("figured-bass-line"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

    void T80180_new_system_tag()
    {
        LoadScoreForTest(_T("80180"), _T("new-system-tag"));
        LM_ASSERT_SCORE_DATA_EQUAL();
        DeleteTestData();
    }

};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( lmFormatter5Test );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( lmFormatter5Test, "lmFormatter5Test" );


//-------------------------------------------------------------------------------------
// Unit tests for class lmTimeGridTable
//-------------------------------------------------------------------------------------

class lmTimeGridTableTest : public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( lmTimeGridTableTest );
        CPPUNIT_TEST( EmptyScoreBuildsEmptyTable );
        CPPUNIT_TEST( ScoreWithOneNoteCreatesOneEntry );
        CPPUNIT_TEST( ScoreWithThreeNotesCreatesThreeEntries );
        CPPUNIT_TEST( ScoreWithOneChordAndBarlineCreatesTwoEntries );
        CPPUNIT_TEST( WhenTwoNotesAtSameTimeChooseTheShortestOne );
        CPPUNIT_TEST( SeveralLinesWithDifferentDurations );
    CPPUNIT_TEST_SUITE_END();


    wxSize m_ScoreSize;
    double m_rScale;
    lmScore* m_pScore;
    lmFormatter5* m_pFormatter;
    lmBoxScore* m_pBoxScore;
    lmTimeGridTable* m_pTable;


    void LoadScoreForTest(const wxString& sFilename)
    {
        DeleteTestData();
        wxString sPath = g_pPaths->GetTestScoresPath();
        wxFileName oFilename(sPath, sFilename, _T("lms"), wxPATH_NATIVE);
        lmLDPParser parser;
        m_pScore = parser.ParseFile( oFilename.GetFullPath() );
        CPPUNIT_ASSERT( m_pScore != NULL );

        lmAggDrawer* pDrawer = new lmAggDrawer(m_ScoreSize.x, m_ScoreSize.y, m_rScale);
        lmPaper m_oPaper;
        m_oPaper.SetDrawer(pDrawer);
        m_pFormatter = new lmFormatter5(&m_oPaper);
        m_pBoxScore = m_pScore->Layout(&m_oPaper, m_pFormatter);
        lmSystemFormatter* pSysFmt = (lmSystemFormatter*) m_pFormatter->GetSystemFormatter(0);
        lmColumnStorage* pColStorage = pSysFmt->GetColumnData(0);
        m_pTable = new lmTimeGridTable(pColStorage);
    }

    void DeleteTestData()
    {
        if (m_pScore)
        {
            delete m_pScore;
            m_pScore = (lmScore*)NULL;
        }
        if (m_pFormatter)
        {
            delete m_pFormatter;
            m_pFormatter = (lmFormatter5*)NULL;
        }
        if (m_pBoxScore)
        {
            delete m_pBoxScore;
            m_pBoxScore = (lmBoxScore*)NULL;
        }
        if (m_pTable)
        {
            delete m_pTable;
            m_pTable = (lmTimeGridTable*)NULL;
        }
    }


public:
    void setUp()
    {
        m_ScoreSize = wxSize(700, 1000);
        m_rScale = 1.0f * lmSCALE;
        m_pScore = (lmScore*)NULL;
        m_pFormatter = (lmFormatter5*)NULL;
        m_pBoxScore = (lmBoxScore*)NULL;
        m_pTable = (lmTimeGridTable*)NULL;
    }

    void tearDown()
    {
        DeleteTestData();
    }


    void EmptyScoreBuildsEmptyTable()
    {
        LoadScoreForTest(_T("00010-empty-renders-one-staff"));
        CPPUNIT_ASSERT( m_pTable->GetSize() == 0 );
        DeleteTestData();
    }

    void ScoreWithOneNoteCreatesOneEntry()
    {
        LoadScoreForTest(_T("00022-spacing-in-prolog-one-note"));
        CPPUNIT_ASSERT( m_pTable->GetSize() == 1 );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(0) == 0.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(0) == 64.0f );
        DeleteTestData();
    }

    void ScoreWithThreeNotesCreatesThreeEntries()
    {
        LoadScoreForTest(_T("00023-same-duration-notes-equally-spaced"));
        CPPUNIT_ASSERT( m_pTable->GetSize() == 3 );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(0) == 0.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(0) == 64.0f );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(1) == 64.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(1) == 64.0f );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(2) == 128.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(2) == 64.0f );
        DeleteTestData();
    }

    void ScoreWithOneChordAndBarlineCreatesTwoEntries()
    {
        LoadScoreForTest(_T("00030-chord-notes-are-aligned"));
        CPPUNIT_ASSERT( m_pTable->GetSize() == 2 );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(0) == 0.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(0) == 256.0f );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(1) == 256.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(1) == 0.0f );
        DeleteTestData();
    }

    void WhenTwoNotesAtSameTimeChooseTheShortestOne()
    {
        LoadScoreForTest(_T("90001-two-notes-different-duration"));
        CPPUNIT_ASSERT( m_pTable->GetSize() == 1 );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(0) == 0.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(0) == 32.0f );
        DeleteTestData();
    }

    void SeveralLinesWithDifferentDurations()
    {
        LoadScoreForTest(_T("90002-several-lines-with-different-durations"));
        CPPUNIT_ASSERT( m_pTable->GetSize() == 5 );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(0) == 0.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(0) == 64.0f );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(1) == 64.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(1) == 32.0f );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(2) == 96.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(2) == 16.0f );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(3) == 112.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(3) == 16.0f );
        CPPUNIT_ASSERT( m_pTable->GetTimepos(4) == 128.0f );
        CPPUNIT_ASSERT( m_pTable->GetDuration(4) == 0.0f );
        DeleteTestData();
    }

};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( lmTimeGridTableTest );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( lmTimeGridTableTest, "lmTimeGridTableTest" );


