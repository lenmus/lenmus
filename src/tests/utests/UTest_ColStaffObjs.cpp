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

#ifdef _LM_DEBUG_

#include <wx/wxprec.h>

//classes related to these tests
#include <UnitTest++.h>
#include "../../score/Score.h"
#include "../../score/VStaff.h"
#include "../../score/Instrument.h"
#include "../../score/Clef.h"
#include "../../ldp_parser/LDPParser.h"

//access to MIDI manager to get default settings for instrument to use
#include "../../sound/MidiManager.h"

//// access to paths
//#include "../../globals/Paths.h"
//extern lmPaths* g_pPaths;

using namespace std;
using namespace UnitTest;

class lmColStaffObjsTestFixture
{
public:

    lmColStaffObjsTestFixture()     //SetUp fixture
    {
        m_pScore = (lmScore*)NULL;
    }

    ~lmColStaffObjsTestFixture()    //TearDown fixture
    {
        DeleteTestData();
    }

    lmScore* m_pScore;
    lmInstrument* m_pInstr;
    lmVStaff* m_pVStaff;
    lmColStaffObjs* m_pCol;

    void CreateEmptyScore()
    {
        m_pScore = new_score();
        m_pInstr = m_pScore->AddInstrument(g_pMidi->DefaultVoiceChannel(),
							    g_pMidi->DefaultVoiceInstr(), _T(""));
        m_pVStaff = m_pInstr->GetVStaff();
        m_pCol = m_pVStaff->GetCollection();
    }

    void DeleteTestData()
    {
        if (m_pScore)
        {
            delete m_pScore;
            m_pScore = (lmScore*)NULL;
        }
    }
};

SUITE(lmColStaffObjsTest)
{
    TEST_FIXTURE(lmColStaffObjsTestFixture, VStaff_creates_empty_collection_with_one_segment)
    {
        CreateEmptyScore();
        CHECK( m_pCol->GetNumSegments() == 1 );
        CHECK( m_pCol->GetFirstSO() == NULL );
        CHECK( m_pCol->GetLastSO() == NULL );
        DeleteTestData();
    }

    TEST_FIXTURE(lmColStaffObjsTestFixture, StaffObjs_can_be_Added)
    {
        CreateEmptyScore();
        lmClef* pClef = m_pVStaff->AddClef(lmE_Sol);
        m_pVStaff->AddKeySignature(earmRe);
        m_pVStaff->AddTimeSignature(4, 4);
        std::string sPattern = "(n a4 w)";
        lmLDPParser parserLDP;
        lmLDPNode* pNode = parserLDP.ParseText( sPattern );
        lmNote* pNote = parserLDP.AnalyzeNote(pNode, m_pVStaff);

        CHECK( m_pCol->GetNumSegments() == 1 );
        CHECK( m_pCol->GetFirstSO() == pClef );
        CHECK( m_pCol->GetLastSO() == pNote );

        DeleteTestData();
    }

    TEST_FIXTURE(lmColStaffObjsTestFixture, Adding_barline_creates_another_segment)
    {
        CreateEmptyScore();
        lmBarline* pBarline = m_pVStaff->AddBarline(lm_eBarlineSimple);
        CHECK( m_pCol->GetNumSegments() == 2 );
        CHECK( m_pCol->GetFirstSO() == pBarline );
        CHECK( m_pCol->GetLastSO() == pBarline );
        DeleteTestData();
    }

    TEST_FIXTURE(lmColStaffObjsTestFixture, Removing_a_barline_joins_segments)
    {
        CreateEmptyScore();
        lmClef* pClef = m_pVStaff->AddClef(lmE_Sol);
        m_pVStaff->AddKeySignature(earmRe);
        m_pVStaff->AddTimeSignature(2, 4);

        lmStaffObj* pNote1 = m_pInstr->push_back( "(n a4 q)" );
        lmStaffObj* pNote2 = m_pInstr->push_back( "(n g4 q)" );
        lmStaffObj* pBarline1 = m_pInstr->push_back( "(barline simple)" );
        lmStaffObj* pNote3 = m_pInstr->push_back( "(n f4 q)" );
        lmStaffObj* pNote4 = m_pInstr->push_back( "(n e4 q)" );
        lmStaffObj* pBarline2 = m_pInstr->push_back( "(barline simple)" );
        lmStaffObj* pNote5 = m_pInstr->push_back( "(n d4 q)" );
        lmStaffObj* pNote6 = m_pInstr->push_back( "(n c4 q)" );
        lmStaffObj* pBarline3 = m_pInstr->push_back( "(barline simple)" );

        CHECK( m_pCol->GetNumSegments() == 4 );

        m_pVStaff->Cmd_DeleteStaffObj(pBarline2);

        CHECK( m_pCol->GetNumSegments() == 3 );
        CHECK( m_pCol->GetFirstSO() == pClef );
        CHECK( m_pCol->GetLastSO() == pBarline3 );

        CHECK( pNote1->GetNumSegment() == 0 );
        CHECK( pNote2->GetNumSegment() == 0 );
        CHECK( pBarline1->GetNumSegment() == 0 );
        CHECK( pNote1->GetTimePos() == 0.0f );
        CHECK( pNote2->GetTimePos() == 64.0f );
        CHECK( pBarline1->GetTimePos() == 128.0f );

        CHECK( pNote3->GetNumSegment() == 1 );
        CHECK( pNote4->GetNumSegment() == 1 );
        CHECK( pNote5->GetNumSegment() == 1 );
        CHECK( pNote6->GetNumSegment() == 1 );
        CHECK( pBarline3->GetNumSegment() == 1 );
        CHECK( pNote3->GetTimePos() == 0.0f );
        CHECK( pNote4->GetTimePos() == 64.0f );
        CHECK( pNote5->GetTimePos() == 128.0f );
        CHECK( pNote6->GetTimePos() == 192.0f );
        CHECK( pBarline3->GetTimePos() == 256.0f );
        DeleteTestData();
    }

    TEST_FIXTURE(lmColStaffObjsTestFixture, Inserting_barline_splits_segment)
    {
        CreateEmptyScore();
        lmClef* pClef = m_pVStaff->AddClef(lmE_Sol);
        m_pVStaff->AddKeySignature(earmRe);
        m_pVStaff->AddTimeSignature(2, 4);

        lmStaffObj* pNote1 = m_pInstr->push_back( "(n a4 q)" );
        lmStaffObj* pNote2 = m_pInstr->push_back( "(n g4 q)" );
        lmStaffObj* pBarline1 = m_pInstr->push_back( "(barline simple)" );
        lmStaffObj* pNote3 = m_pInstr->push_back( "(n f4 q)" );
        lmStaffObj* pNote4 = m_pInstr->push_back( "(n e4 q)" );
        lmStaffObj* pNote5 = m_pInstr->push_back( "(n d4 q)" );
        lmStaffObj* pNote6 = m_pInstr->push_back( "(n c4 q)" );
        lmStaffObj* pBarline3 = m_pInstr->push_back( "(barline simple)" );

        CHECK( m_pCol->GetNumSegments() == 3 );
        CHECK( m_pCol->GetFirstSO() == pClef );
        CHECK( m_pCol->GetLastSO() == pBarline3 );

        lmInstrIterator it = m_pInstr->find(pNote5);
        lmStaffObj* pBarline2 = m_pInstr->insert(it, "(barline simple)");

        CHECK( m_pCol->GetNumSegments() == 4 );
        CHECK( m_pCol->GetFirstSO() == pClef );
        CHECK( m_pCol->GetLastSO() == pBarline3 );

        CHECK( pNote1->GetNumSegment() == 0 );
        CHECK( pNote2->GetNumSegment() == 0 );
        CHECK( pBarline1->GetNumSegment() == 0 );
        CHECK( pNote1->GetTimePos() == 0.0f );
        CHECK( pNote2->GetTimePos() == 64.0f );
        CHECK( pBarline1->GetTimePos() == 128.0f );

        CHECK( pNote3->GetNumSegment() == 1 );
        CHECK( pNote4->GetNumSegment() == 1 );
        CHECK( pBarline2->GetNumSegment() == 1 );
        CHECK( pNote3->GetTimePos() == 0.0f );
        CHECK( pNote4->GetTimePos() == 64.0f );
        CHECK( pBarline2->GetTimePos() == 128.0f );

        CHECK( pNote5->GetNumSegment() == 2 );
        CHECK( pNote6->GetNumSegment() == 2 );
        CHECK( pBarline3->GetNumSegment() == 2 );
        CHECK( pNote5->GetTimePos() == 0.0f );
        CHECK( pNote6->GetTimePos() == 64.0f );
        CHECK( pBarline3->GetTimePos() == 128.0f );

        DeleteTestData();
    }

    TEST_FIXTURE(lmColStaffObjsTestFixture, Add_before_barline_in_full_bar)
    {
        CreateEmptyScore();
        lmClef* pClef = m_pVStaff->AddClef(lmE_Sol);
        m_pVStaff->AddKeySignature(earmRe);
        m_pVStaff->AddTimeSignature(2, 4);

        lmStaffObj* pNote1 = m_pInstr->push_back( "(n a4 q)" );
        lmStaffObj* pNote2 = m_pInstr->push_back( "(n g4 q)" );
        lmStaffObj* pBarline1 = m_pInstr->push_back( "(barline simple)" );

        CHECK( m_pCol->GetNumSegments() == 2 );
        CHECK( m_pCol->GetFirstSO() == pClef );
        CHECK( m_pCol->GetLastSO() == pBarline1 );

        lmInstrIterator it = m_pInstr->find(pBarline1);
        lmStaffObj* pNote3 = m_pInstr->insert(it, "(n e4 q)");

        //wxLogMessage( m_pScore->Dump());
        CHECK( m_pCol->GetNumSegments() == 2 );
        //cout << "num.segments: " << m_pCol->GetNumSegments() << endl;
        CHECK( m_pCol->GetFirstSO() == pClef );
        CHECK( m_pCol->GetLastSO() == pBarline1 );

        CHECK( pNote1->GetNumSegment() == 0 );
        CHECK( pNote2->GetNumSegment() == 0 );
        CHECK( pNote3->GetNumSegment() == 0 );
        CHECK( pBarline1->GetNumSegment() == 0 );
        CHECK( pNote1->GetTimePos() == 0.0f );
        CHECK( pNote2->GetTimePos() == 64.0f );
        CHECK( pNote3->GetTimePos() == 128.0f );
        //cout << "barline time:" << pBarline1->GetTimePos() << endl;
        CHECK( pBarline1->GetTimePos() == 192.0f );

        DeleteTestData();
    }

}

//----------------------------------------------------------------------------------
class lmScoreCursorTestFixture
{
public:

    lmScoreCursorTestFixture()     //SetUp fixture
    {
        m_pScore = (lmScore*)NULL;
    }

    ~lmScoreCursorTestFixture()    //TearDown fixture
    {
        DeleteTestData();
    }

    lmScore* m_pScore;
    lmInstrument* m_pInstr;
    lmVStaff* m_pVStaff;
    lmColStaffObjs* m_pCol;

    void CreateEmptyScore()
    {
        m_pScore = new_score();
        m_pInstr = m_pScore->AddInstrument(g_pMidi->DefaultVoiceChannel(),
							    g_pMidi->DefaultVoiceInstr(), _T(""));
        m_pVStaff = m_pInstr->GetVStaff();
        m_pCol = m_pVStaff->GetCollection();
    }

    void DeleteTestData()
    {
        if (m_pScore)
        {
            delete m_pScore;
            m_pScore = (lmScore*)NULL;
        }
    }
};

SUITE(lmScoreCursorTest)
{
    TEST_FIXTURE(lmScoreCursorTestFixture, ScoreCursor_in_empty_score)
    {
        CreateEmptyScore();
        lmScoreCursor* pCursor = m_pScore->GetCursor();
        CHECK( pCursor->GetSegment() == 0 );
        CHECK( pCursor->GetCursorInstrumentNumber() == 1 );
        CHECK( pCursor->GetCursorNumStaff() == 1 );
        CHECK( pCursor->GetCursorTime() == 0.0f );
        CHECK( pCursor->GetStaffObj() == NULL );
        DeleteTestData();
    }

}

#endif  // _LM_DEBUG_
