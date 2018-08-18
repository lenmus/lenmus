//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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

#include <UnitTest++.h>
#include <sstream>
#include "lenmus_config.h"

//classes related to these tests
#include "lenmus_injectors.h"
#include "lenmus_paths.h"
#include "lenmus_string.h"
#include "lenmus_score_corrector.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_document.h>
#include <lomse_internal_model.h>
using namespace lomse;

using namespace UnitTest;
using namespace std;
using namespace lenmus;


#define CHECK_MESSAGE(message) \
    UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), (std::string(message)).c_str());

//---------------------------------------------------------------------------------------
// Helper, for accessing protected members
class MyScoreComparer : public ScoreComparer
{
public:
    MyScoreComparer() : ScoreComparer() {}
    ~MyScoreComparer() {}

    void my_encode_A(ImoScore* pScore) { m_N = encode(pScore, &m_A); }
    void my_encode_B(ImoScore* pScore) { m_M = encode(pScore, &m_B); }
    void my_allocate_v_matrix() { allocate_v_matrix(); }
    void my_find_lcs() { find_lcs(); }
    void my_find_optimal_path() { find_optimal_path(); }
    void my_compute_differences() { compute_differences(); }
    int* my_get_v(int i) { return m_v.get_row(i); }
    int* my_get_A() { return m_A; }
    int* my_get_B() { return m_B; }
    int my_get_N() { return m_N; }
    int my_get_M() { return m_M; }
    int* my_get_path_x() { return m_xPath; }
    int* my_get_path_y() { return m_yPath; }

};

//---------------------------------------------------------------------------------------
class ScoreComparerTestFixture
{
public:
    ApplicationScope m_appScope;
    LibraryScope* m_pLibScope;
    string m_scores_path;
    stringstream m_msg;

    ScoreComparerTestFixture()     //SetUp fixture
        : m_appScope(cout)
        , m_scores_path(LENMUS_TEST_SCORES_PATH)
    {
        LomseDoorway& door = m_appScope.get_lomse();
        m_pLibScope = door.get_library_scope();
    }

    ~ScoreComparerTestFixture()    //TearDown fixture
    {
        m_msg.str("");
        m_msg << endl;
    }

    void write_msg()
    {
        m_msg << endl;
        CHECK_MESSAGE( m_msg.str() );
    }

    void dump_vector(int* X, int size)
    {
        m_msg.str("");
        m_msg << endl;
        for (int i=0; i < size; ++i)
            m_msg << *(X+i) << ",";
        m_msg << endl;
        CHECK_MESSAGE( m_msg.str() );
    }

    bool check_vector(int* X, int size, int* Y)
    {
        for (int i=0; i < size; ++i)
        {
            if (*(X+i) != *(Y+i))
                return false;
        }
        return true;
    }

};

SUITE(ScoreComparerTest)
{

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_1)
    {
        //01. encode
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (clef G)(key C)(n c4 q)(n e4 q)(n g4 e. g+)(n e4 s g-)(r q)(barline) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);

        CHECK( cmp.my_get_N() == 8 );
        int* A = cmp.my_get_A();
//        CHECK( A[0] == 2000 );  //clef
//        CHECK( A[1] == 2000 );  //key
//        CHECK( A[2] == 240 );   //n q
//        CHECK( A[3] == 240 );   //n q
//        CHECK( A[4] == 251 );   //n e.
//        CHECK( A[5] == 260 );   //n s
//        CHECK( A[6] == 40 );    //r q
//        CHECK( A[7] == 1000 );  //barline
        int expected[8] = {2000, 2000, 240, 240, 251, 260, 40, 1000};
        CHECK( check_vector(A, 8, expected) == true );
    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_10)
    {
        //10. find lcs. Paper example
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (n a4 q)(n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (n c4 s)(n b4 e)(n a4 q)(n b4 e)(n a4 q)(n c4 s) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        int D = cmp.get_number_of_differences();

//        int N = cmp.my_get_N();
//        int M = cmp.my_get_M();
//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        for (int i = 0; i <= D; ++i)
//            dump_vector( cmp.my_get_v(i), 2*(N+M)+1 );

        int V[27] = {0,0,0,0,0,0,0,0,3,3,4,4,5,5,7,7,5,7,0,0,0,0,0,0,0,0,0};
        CHECK( D == 5 );
        CHECK( check_vector(cmp.my_get_v(D), 27, V) );

    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_11)
    {
        //11. find lcs: common start
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n a4 q)(n b4 e)(n a4 q)(n c4 s) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        int D = cmp.get_number_of_differences();

//        int N = cmp.my_get_N();
//        int M = cmp.my_get_M();
//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        for (int i = 0; i <= D; ++i)
//            dump_vector( cmp.my_get_v(i), 2*(N+M)+1 );

        int V[27] = {0,0,0,0,0,0,0,0,0,0,3,2,5,5,7,7,0,0,0,0,0,0,0,0,0,0,0};
        CHECK( D == 3 );
        CHECK( check_vector(cmp.my_get_v(D), 27, V) );

    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_12)
    {
        //12. find lcs: common end
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n a4 q)(n b4 e)(n a4 q) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        int D = cmp.get_number_of_differences();

//        int N = cmp.my_get_N();
//        int M = cmp.my_get_M();
//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        for (int i = 0; i <= D; ++i)
//            dump_vector( cmp.my_get_v(i), 2*(N+M)+1 );

        int V[25] = {0,0,0,0,0,0,0,0,0,0,2,2,5,5,7,0,0,0,0,0,0,0,0,0,0};
        CHECK( D == 2 );
        CHECK( check_vector(cmp.my_get_v(D), 25, V) );
    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_13)
    {
        //13. find lcs: start inserting
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        int D = cmp.get_number_of_differences();

//        int N = cmp.my_get_N();
//        int M = cmp.my_get_M();
//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        for (int i = 0; i <= D; ++i)
//            dump_vector( cmp.my_get_v(i), 2*(N+M)+1 );

        int V[23] = {0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0};
        CHECK( D == 1 );
        CHECK( check_vector(cmp.my_get_v(D), 23, V) );
    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_30)
    {
        //30. find optimal path. Paper example
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (n a4 q)(n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (n c4 s)(n b4 e)(n a4 q)(n b4 e)(n a4 q)(n c4 s) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        cmp.my_find_optimal_path();
        int D = cmp.get_number_of_differences();

//        int N = cmp.my_get_N();
//        int M = cmp.my_get_M();
//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        dump_vector( cmp.my_get_path_x(), D+1 );
//        dump_vector( cmp.my_get_path_y(), D+1 );

        int x[6] = {0,1,3,5,7,7};
        int y[6] = {0,0,1,4,5,6};
        CHECK( D == 5 );
        CHECK( check_vector(cmp.my_get_path_x(), D+1, x) );
        CHECK( check_vector(cmp.my_get_path_y(), D+1, y) );
    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_31)
    {
        //31. find optimal path: common start
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n a4 q)(n b4 e)(n a4 q)(n c4 s) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        cmp.my_find_optimal_path();
        int D = cmp.get_number_of_differences();

//        int N = cmp.my_get_N();
//        int M = cmp.my_get_M();
//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        dump_vector( cmp.my_get_path_x(), D+1 );
//        dump_vector( cmp.my_get_path_y(), D+1 );

        int x[4] = {2,5,7,7};
        int y[4] = {2,4,5,6};
        CHECK( D == 3 );
        CHECK( check_vector(cmp.my_get_path_x(), D+1, x) );
        CHECK( check_vector(cmp.my_get_path_y(), D+1, y) );
    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_32)
    {
        //32. find optimal path: common end
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n a4 q)(n b4 e)(n a4 q) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        cmp.my_find_optimal_path();
        int D = cmp.get_number_of_differences();

//        int N = cmp.my_get_N();
//        int M = cmp.my_get_M();
//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        dump_vector( cmp.my_get_path_x(), D+1 );
//        dump_vector( cmp.my_get_path_y(), D+1 );

        int x[3] = {2,5,7};
        int y[3] = {2,4,5};
        CHECK( D == 2 );
        CHECK( check_vector(cmp.my_get_path_x(), D+1, x) );
        CHECK( check_vector(cmp.my_get_path_y(), D+1, y) );
    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_33)
    {
        //33. find optimal path: start inserting
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        cmp.my_find_optimal_path();
        int D = cmp.get_number_of_differences();

//        int N = cmp.my_get_N();
//        int M = cmp.my_get_M();
//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        dump_vector( cmp.my_get_path_x(), D+1 );
//        dump_vector( cmp.my_get_path_y(), D+1 );

        int x[2] = {0,5};
        int y[2] = {0,6};
        CHECK( D == 1 );
        CHECK( check_vector(cmp.my_get_path_x(), D+1, x) );
        CHECK( check_vector(cmp.my_get_path_y(), D+1, y) );
    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_50)
    {
        //50. compute differences. Paper example
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (n a4 q)(n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (n c4 s)(n b4 e)(n a4 q)(n b4 e)(n a4 q)(n c4 s) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        cmp.my_find_optimal_path();
        cmp.my_compute_differences();
        int D = cmp.get_number_of_differences();
        int N = cmp.my_get_N();
        int M = cmp.my_get_M();

//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        dump_vector( cmp.my_get_A(), N );
//        dump_vector( cmp.my_get_B(), M );

        int A[7] = {1,1,0,0,0,1,0};
        int B[6] = {0,1,0,0,0,1};
        CHECK( D == 5 );
        CHECK( check_vector(cmp.my_get_A(), N, A) );
        CHECK( check_vector(cmp.my_get_B(), M, B) );
    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_51)
    {
        //51. compute differences: common start
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n a4 q)(n b4 e)(n a4 q)(n c4 s) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        cmp.my_find_optimal_path();
        cmp.my_compute_differences();
        int D = cmp.get_number_of_differences();
        int N = cmp.my_get_N();
        int M = cmp.my_get_M();

//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        dump_vector( cmp.my_get_A(), N );
//        dump_vector( cmp.my_get_B(), M );

        int A[7] = {0,0,1,0,0,1,0};
        int B[6] = {0,0,0,0,0,1};
        CHECK( D == 3 );
        CHECK( check_vector(cmp.my_get_A(), N, A) );
        CHECK( check_vector(cmp.my_get_B(), M, B) );
    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_52)
    {
        //52. compute differences: common end
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (clef G2)(n b4 e)(n a4 q)(n b4 e)(n a4 q) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        cmp.my_find_optimal_path();
        cmp.my_compute_differences();
        int D = cmp.get_number_of_differences();
        int N = cmp.my_get_N();
        int M = cmp.my_get_M();

//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        dump_vector( cmp.my_get_A(), N );
//        dump_vector( cmp.my_get_B(), M );

        int A[7] = {0,0,1,0,0,1,0};
        int B[5] = {0,0,0,0,0};
        CHECK( D == 2 );
        CHECK( check_vector(cmp.my_get_A(), N, A) );
        CHECK( check_vector(cmp.my_get_B(), M, B) );
    }

    TEST_FIXTURE(ScoreComparerTestFixture, comparer_53)
    {
        //53. compute differences. start inserting
        Document doc(*m_pLibScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content "
            "(score (vers 2.0) "
                "(instrument (musicData (n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "(score (vers 2.0) "
                "(instrument (musicData (n b4 e)(n c4 s)(n a4 q)(n b4 e)(n b4 e)(n a4 q) )))"
            "))" );
        ImoScore* pScore1 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(0) );
        ImoScore* pScore2 = static_cast<ImoScore*>( doc.get_im_root()->get_content_item(1) );

        MyScoreComparer cmp;
        cmp.my_encode_A(pScore1);
        cmp.my_encode_B(pScore2);
        cmp.my_allocate_v_matrix();
        cmp.my_find_lcs();
        cmp.my_find_optimal_path();
        cmp.my_compute_differences();
        int D = cmp.get_number_of_differences();
        int N = cmp.my_get_N();
        int M = cmp.my_get_M();

//        m_msg << "max D=" << D << ", N=" << N << ", M=" << M << endl;
//        CHECK_MESSAGE( m_msg.str() );
//        dump_vector( cmp.my_get_A(), N );
//        dump_vector( cmp.my_get_B(), M );

        int A[5] = {0,0,0,0,0};
        int B[6] = {1,0,0,0,0,0};
        CHECK( D == 1 );
        CHECK( check_vector(cmp.my_get_A(), N, A) );
        CHECK( check_vector(cmp.my_get_B(), M, B) );
    }

};

