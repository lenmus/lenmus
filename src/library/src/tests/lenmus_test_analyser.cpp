//--------------------------------------------------------------------------------------
//  LenMus Library
//  Copyright (c) 2010 LenMus project
//
//  This program is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along
//  with this library; if not, see <http://www.gnu.org/licenses/> or write to the
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
//  MA  02111-1307,  USA.
//
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifdef _LM_DEBUG_

#include <UnitTest++.h>
#include <iostream>

//classes related to these tests
#include "lenmus_parser.h"
#include "lenmus_analyser.h"
#include "lenmus_internal_model.h"
#include "lenmus_im_note.h"

//to delete singletons
#include "lenmus_factory.h"
#include "lenmus_elements.h"


using namespace UnitTest;
using namespace std;
using namespace lenmus;


class AnalyserTestFixture
{
public:

    AnalyserTestFixture()     //SetUp fixture
    {
    }

    ~AnalyserTestFixture()    //TearDown fixture
    {
        delete Factory::instance();
    }
};

SUITE(AnalyserTest)
{

    TEST_FIXTURE(AnalyserTestFixture, AnalyserMissingMandatoryElementNoElements)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. score: missing mandatory element 'vers'." << endl
                 << "Line 0. score: missing mandatory element 'instrument'." << endl;
        SpLdpTree tree = parser.parse_text("(score )");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( tree->get_root()->get_imobj() != NULL );
        CHECK( errormsg.str() == expected.str() );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserMissingMandatoryElementMoreElements)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. score: missing mandatory element 'vers'." << endl
                 << "Line 0. score: missing mandatory element 'instrument'." << endl;
        SpLdpTree tree = parser.parse_text("(score)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << score->get_root()->to_string() << endl;
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( tree->get_root()->get_imobj() != NULL );
        CHECK( errormsg.str() == expected.str() );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserLanguageRemoved)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. score: missing mandatory element 'vers'." << endl
                 << "Line 0. score: missing mandatory element 'instrument'." << endl;
        SpLdpTree tree = parser.parse_text("(score (language en utf-8))");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << tree->get_root()->to_string() << endl;
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( tree->get_root()->get_imobj() != NULL );
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(score )" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserHasMandatoryElement)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. score: missing mandatory element 'instrument'." << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6))");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        ImScore* pScore = dynamic_cast<ImScore*>( tree->get_root()->get_imobj() );
        CHECK( pScore != NULL );
        CHECK( pScore->get_version() == "1.6" );
        CHECK( pScore->get_num_instruments() == 0 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOptionalElementMissing)
    {
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(barline)");
        Analyser a(tree, cout);
        a.analyse(tree->get_root());  
        ImBarline* pBarline = dynamic_cast<ImBarline*>( tree->get_root()->get_imobj() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImBarline::kSimple );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOptionalElementPresent)
    {
        LdpParser parser(cout);
        SpLdpTree tree = parser.parse_text("(barline double)");
        Analyser a(tree, cout);
        a.analyse(tree->get_root());  
        ImBarline* pBarline = dynamic_cast<ImBarline*>( tree->get_root()->get_imobj() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImBarline::kDouble );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserInvalidBarlineType)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Unknown barline type 'invalid'. 'simple' barline assumed." << endl;
        SpLdpTree tree = parser.parse_text("(barline invalid)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImBarline* pBarline = dynamic_cast<ImBarline*>( tree->get_root()->get_imobj() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImBarline::kSimple );
        CHECK( tree->get_root()->to_string() == "(barline simple)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMoreMissingFirst)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. score: missing mandatory element 'instrument'." << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6))");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << score->get_root()->to_string() << endl;
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImScore* pScore = dynamic_cast<ImScore*>( tree->get_root()->get_imobj() );
        CHECK( pScore != NULL );
        CHECK( pScore->get_num_instruments() == 0 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMorePresentOne)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6)(instrument (musicData)))");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImScore* pScore = dynamic_cast<ImScore*>( tree->get_root()->get_imobj() );
        CHECK( pScore != NULL );
        //CHECK( pScore->get_num_instruments() == 1 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMorePresentMore)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6)(instrument (musicData))(instrument (musicData)))");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImScore* pScore = dynamic_cast<ImScore*>( tree->get_root()->get_imobj() );
        CHECK( pScore != NULL );
        //CHECK( pScore->get_num_instruments() == 2 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMoreOptionalAlternativesError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Element 'instrument' is not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q)(instrument 3))");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 q))" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMoreOptionalAlternativesErrorRemoved)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Element 'instrument' is not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q)(instrument 3)(n d4 e))");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 q) (n d4 e))" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMoreOptionalAlternativesTwo)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q)(n d4 e.))");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserNote)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(n +d3 e.)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImNote* pNote = dynamic_cast<ImNote*>( tree->get_root()->get_imobj() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImNote::Sharp );
        CHECK( pNote->get_dots() == 1 );
        CHECK( pNote->get_note_type() == ImNote::Eighth );
        CHECK( pNote->get_octave() == 3 );
        CHECK( pNote->get_step() == ImNote::D );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserRemoveBadNode)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Element 'label:instrument' is not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 q instrument)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImNote* pNote = dynamic_cast<ImNote*>( tree->get_root()->get_imobj() );
        CHECK( pNote != NULL );
        CHECK( tree->get_root()->to_string() == "(n c4 q)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserNotePitchError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Unknown note pitch 'j17'. Replaced by 'c4'." << endl;
        SpLdpTree tree = parser.parse_text("(n j17 q)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImNote* pNote = dynamic_cast<ImNote*>( tree->get_root()->get_imobj() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImNote::NoAccidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == ImNote::Quarter );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == ImNote::C );
        CHECK( tree->get_root()->to_string() == "(n c4 q)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserNoteDurationErrorLetter)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Unknown note/rest duration 'j.'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 j.)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImNote* pNote = dynamic_cast<ImNote*>( tree->get_root()->get_imobj() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImNote::NoAccidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == ImNote::Quarter );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == ImNote::C );
        CHECK( tree->get_root()->to_string() == "(n c4 q)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserNoteDurationErrorDots)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Unknown note/rest duration 'e.1'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e.1)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImNote* pNote = dynamic_cast<ImNote*>( tree->get_root()->get_imobj() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImNote::NoAccidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == ImNote::Quarter );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == ImNote::C );
        CHECK( tree->get_root()->to_string() == "(n c4 q)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserNoteStaff)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "Line 0. Unknown note/rest duration 'e.1'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e p7)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImNote* pNote = dynamic_cast<ImNote*>( tree->get_root()->get_imobj() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImNote::NoAccidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == ImNote::Eighth );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == ImNote::C );
        CHECK( pNote->get_staff() == 6 );
        CHECK( tree->get_root()->to_string() == "(n c4 e p7)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserNoteStaffError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Invalid staff 'pz'. Replaced by 'p1'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e pz)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImNote* pNote = dynamic_cast<ImNote*>( tree->get_root()->get_imobj() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_staff() == 0 );
        CHECK( tree->get_root()->to_string() == "(n c4 e p1)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserNoteVoice)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "Line 0. Unknown note/rest duration 'e.1'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e v3)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImNote* pNote = dynamic_cast<ImNote*>( tree->get_root()->get_imobj() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImNote::NoAccidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == ImNote::Eighth );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == ImNote::C );
        CHECK( pNote->get_voice() == 3 );
        CHECK( tree->get_root()->to_string() == "(n c4 e v3)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserNoteVoiceError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Invalid voice 'vx'. Replaced by 'v1'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e vx)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImNote* pNote = dynamic_cast<ImNote*>( tree->get_root()->get_imobj() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_voice() == 1 );
        CHECK( tree->get_root()->to_string() == "(n c4 e v1)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoBackStart)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "Line 0. Invalid voice 'vx'. Replaced by 'v1'." << endl;
        SpLdpTree tree = parser.parse_text("(goBack start)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImGoBackFwd* pGBF = dynamic_cast<ImGoBackFwd*>( tree->get_root()->get_imobj() );
        CHECK( pGBF != NULL );
        CHECK( pGBF->is_to_start() );
        CHECK( tree->get_root()->to_string() == "(goBack start)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoBackEnd)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Element 'goBack' has an incoherent value: go backwards to end?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goBack end)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoBackQ)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "Line 0. Element 'goBack' has an incoherent value: go backwards to end?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goBack q)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImGoBackFwd* pGBF = dynamic_cast<ImGoBackFwd*>( tree->get_root()->get_imobj() );
        CHECK( pGBF != NULL );
        CHECK( !pGBF->is_to_start() );
        CHECK( !pGBF->is_to_end() );
        CHECK( pGBF->get_time_shift() == -64.0f );
        CHECK( tree->get_root()->to_string() == "(goBack q)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoFwdEnd)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "Line 0. Invalid voice 'vx'. Replaced by 'v1'." << endl;
        SpLdpTree tree = parser.parse_text("(goFwd end)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImGoBackFwd* pGBF = dynamic_cast<ImGoBackFwd*>( tree->get_root()->get_imobj() );
        CHECK( pGBF != NULL );
        CHECK( pGBF->is_to_end() );
        CHECK( tree->get_root()->to_string() == "(goFwd end)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoFwdStart)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Element 'goFwd' has an incoherent value: go forward to start?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goFwd start)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoFwdH)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "Line 0. Element 'goBack' has an incoherent value: go backwards to end?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goFwd h)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImGoBackFwd* pGBF = dynamic_cast<ImGoBackFwd*>( tree->get_root()->get_imobj() );
        CHECK( pGBF != NULL );
        CHECK( !pGBF->is_to_start() );
        CHECK( !pGBF->is_to_end() );
        CHECK( pGBF->get_time_shift() == 128.0f );
        CHECK( tree->get_root()->to_string() == "(goFwd h)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoFwdNum)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "Line 0. Element 'goBack' has an incoherent value: go backwards to end?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goFwd 128)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImGoBackFwd* pGBF = dynamic_cast<ImGoBackFwd*>( tree->get_root()->get_imobj() );
        CHECK( pGBF != NULL );
        CHECK( !pGBF->is_to_start() );
        CHECK( !pGBF->is_to_end() );
        CHECK( pGBF->get_time_shift() == 128.0f );
        CHECK( tree->get_root()->to_string() == "(goFwd 128)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoFwdBadNumber)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Negative value for element 'goFwd/goBack'. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goFwd -128.3)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoBackNum)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "Line 0. Element 'goBack' has an incoherent value: go backwards to end?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goBack 128)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImGoBackFwd* pGBF = dynamic_cast<ImGoBackFwd*>( tree->get_root()->get_imobj() );
        CHECK( pGBF != NULL );
        CHECK( !pGBF->is_to_start() );
        CHECK( !pGBF->is_to_end() );
        CHECK( pGBF->get_time_shift() == -128.0f );
        CHECK( tree->get_root()->to_string() == "(goBack 128)" );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserRest)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(r e.)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImRest* pRest = dynamic_cast<ImRest*>( tree->get_root()->get_imobj() );
        CHECK( pRest != NULL );
        CHECK( pRest->get_dots() == 1 );
        CHECK( pRest->get_note_type() == ImNoteRest::Eighth );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserStaffNum)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(r e. p2)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImRest* pRest = dynamic_cast<ImRest*>( tree->get_root()->get_imobj() );
        CHECK( pRest != NULL );
        CHECK( pRest->get_dots() == 1 );
        CHECK( pRest->get_note_type() == ImNoteRest::Eighth );
        CHECK( pRest->get_staff() == 1 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserDefaultStaffNum)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(r e.)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImRest* pRest = dynamic_cast<ImRest*>( tree->get_root()->get_imobj() );
        CHECK( pRest != NULL );
        CHECK( pRest->get_dots() == 1 );
        CHECK( pRest->get_note_type() == ImNoteRest::Eighth );
        CHECK( pRest->get_staff() == 0 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserStaffNumInherited)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(musicData (r e. p2)(n c4 q)(n d4 e p3)(r q))");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        LdpTree::iterator it = tree->begin();
        ++it;
        ImRest* pRest = dynamic_cast<ImRest*>( (*it)->get_imobj() );
        CHECK( pRest != NULL );
        CHECK( pRest->get_staff() == 1 );
        ++it;
        ++it;
        ++it;
        ImNote* pNote = dynamic_cast<ImNote*>( (*it)->get_imobj() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_staff() == 1 );
        ++it;
        ++it;
        ++it;
        pNote = dynamic_cast<ImNote*>( (*it)->get_imobj() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_staff() == 2 );
        ++it;
        ++it;
        ++it;
        ++it;
        pRest = dynamic_cast<ImRest*>( (*it)->get_imobj() );
        CHECK( pRest != NULL );
        CHECK( pRest->get_staff() == 2 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserClef)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(clef G)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImClef* pClef = dynamic_cast<ImClef*>( tree->get_root()->get_imobj() );
        CHECK( pClef != NULL );
        CHECK( pClef->get_type() == ImClef::kG3 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserClefError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Unknown clef type 'Fa4'. Assumed 'G'." << endl;
        SpLdpTree tree = parser.parse_text("(clef Fa4)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImClef* pClef = dynamic_cast<ImClef*>( tree->get_root()->get_imobj() );
        CHECK( pClef != NULL );
        CHECK( pClef->get_type() == ImClef::kG3 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserKey)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(key G)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImKeySignature* pKeySignature = dynamic_cast<ImKeySignature*>( tree->get_root()->get_imobj() );
        CHECK( pKeySignature != NULL );
        CHECK( pKeySignature->get_type() == ImKeySignature::G );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserKeyError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Unknown key 'Sol'. Assumed 'C'." << endl;
        SpLdpTree tree = parser.parse_text("(key Sol)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImKeySignature* pKeySignature = dynamic_cast<ImKeySignature*>( tree->get_root()->get_imobj() );
        CHECK( pKeySignature != NULL );
        CHECK( pKeySignature->get_type() == ImKeySignature::C );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserInstrumentStaves)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(instrument (staves 2)(musicData))");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImInstrument* pInstrument = dynamic_cast<ImInstrument*>( tree->get_root()->get_imobj() );
        CHECK( pInstrument != NULL );
        //cout << "num.staves=" << pInstrument->get_num_staves() << endl;
        CHECK( pInstrument->get_num_staves() == 2 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserInstrumentStavesError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. Invalid value 'two' for staves. Replaced by 1." << endl;
        SpLdpTree tree = parser.parse_text("(instrument (staves two)(musicData))");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImInstrument* pInstrument = dynamic_cast<ImInstrument*>( tree->get_root()->get_imobj() );
        CHECK( pInstrument != NULL );
        //cout << "num.staves=" << pInstrument->get_num_staves() << endl;
        CHECK( pInstrument->get_num_staves() == 1 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserTimeSignature)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(time 6 8)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImTimeSignature* pTimeSignature = dynamic_cast<ImTimeSignature*>( tree->get_root()->get_imobj() );
        CHECK( pTimeSignature != NULL );
        CHECK( pTimeSignature->get_beats() == 6 );
        CHECK( pTimeSignature->get_beat_type() == 8 );
        delete tree->get_root();
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserTimeSignatureError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg);
        stringstream expected;
        expected << "Line 0. time: missing mandatory element 'number'." << endl;
        SpLdpTree tree = parser.parse_text("(time 2)");
        Analyser a(tree, errormsg);
        a.analyse(tree->get_root());  
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImTimeSignature* pTimeSignature = dynamic_cast<ImTimeSignature*>( tree->get_root()->get_imobj() );
        CHECK( pTimeSignature != NULL );
        CHECK( pTimeSignature->get_beats() == 2 );
        CHECK( pTimeSignature->get_beat_type() == 4 );
        delete tree->get_root();
    }

}

#endif  // _LM_DEBUG_

