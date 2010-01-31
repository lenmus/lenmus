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

//classes related to these tests
#include "../../score/defs.h"


class lmURectTest : public CppUnit::TestCase
{
public:
    lmURectTest() {}

private:
    CPPUNIT_TEST_SUITE( lmURectTest );
        CPPUNIT_TEST( Union );
    CPPUNIT_TEST_SUITE_END();

    void Union();

    DECLARE_NO_COPY_CLASS(lmURectTest)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( lmURectTest );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( lmURectTest, "lmURectTest" );

void lmURectTest::Union()
{
    static const struct RectData
    {
        int x1, y1, w1, h1;
        int x2, y2, w2, h2;
        int x, y, w, h;

        lmURect GetFirst() const { return lmURect(x1, y1, w1, h1); }
        lmURect GetSecond() const { return lmURect(x2, y2, w2, h2); }
        lmURect GetResult() const { return lmURect(x, y, w, h); }
    } s_rects[] =
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 4, 4, 1, 1, 1, 1, 4, 4 },
        { 1, 1, 2, 2, 4, 4, 1, 1, 1, 1, 4, 4 },
        { 2, 2, 2, 2, 4, 4, 4, 4, 2, 2, 6, 6 },
        { 1, 1, 4, 4, 4, 4, 1, 1, 1, 1, 4, 4 }
    };

    for ( size_t n = 0; n < WXSIZEOF(s_rects); n++ )
    {
        const RectData& data = s_rects[n];

        CPPUNIT_ASSERT(
            data.GetFirst().Union(data.GetSecond()) == data.GetResult()
        );

        CPPUNIT_ASSERT(
            data.GetSecond().Union(data.GetFirst()) == data.GetResult()
        );
    }
}
