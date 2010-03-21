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

#include <UnitTest++.h>
#include <sstream>

//classes related to these tests
#include "../elements/Elements.h"
#include "../elements/Factory.h"


//-------------------------------------------------------------------------------------
// tests
//
// Elements
//  - Can create an element
//

using namespace UnitTest;
using namespace std;
using namespace lenmus;

class LdpElementsTestFixture
{
public:

    LdpElementsTestFixture()     //SetUp fixture
    {
    }

    ~LdpElementsTestFixture()    //TearDown fixture
    {
    }
};

SUITE(LdpElementsTest)
{
    TEST_FIXTURE(LdpElementsTestFixture, CanCreateElementFromName)
    {
        SpLdpElement clef = Factory::instance().create(_T("clef"));
        CHECK( clef->get_type() == k_clef );
        CHECK( clef->get_name() == _T("clef") );
    }

    TEST_FIXTURE(LdpElementsTestFixture, CanCreateElementFromType)
    {
        SpLdpElement clef = Factory::instance().create(k_clef);
        CHECK( clef->get_type() == k_clef );
        CHECK( clef->get_name() == _T("clef") );
    }

    TEST_FIXTURE(LdpElementsTestFixture, InvalidElementNameThowsException)
    {
        bool fOk = false;
        try
        {
            SpLdpElement clef = Factory::instance().create(_T("invalid"));
        }
        catch(runtime_error e)
        {
            fOk = true;
        }
        catch(...)    //handle all other exceptions
        {
            throw;
        }
        CHECK(fOk);
    }

    TEST_FIXTURE(LdpElementsTestFixture, CanAddSimpleSubElements)
    {
        SpLdpElement note = Factory::instance().create(_T("n"));
        note->push( new_value(k_pitch, _T("c4")) );
        //tcout << note->to_string() << endl;
        CHECK( note->to_string() == _T("(n c4)") );
    }

    TEST_FIXTURE(LdpElementsTestFixture, CanAddCompositeSubElements)
    {
        SpLdpElement note = Factory::instance().create(_T("n"));
        note->push( new_value(k_pitch, _T("c4")) );
        note->push( new_value(k_duration, _T("q")) );
        note->push( new_element(k_stem, new_label(_T("up"))) );
        //tcout << note->to_string() << endl;
        CHECK( note->to_string() == _T("(n c4 q (stem up))") );
    }

    TEST_FIXTURE(LdpElementsTestFixture, CanAddCompositeWithManySubElements)
    {
        SpLdpElement note = Factory::instance().create(_T("n"));
        note->push( new_value(k_pitch, _T("c4")) );
        note->push( new_value(k_duration, _T("q")) );
        note->push( new_element(k_stem, new_label(_T("up"))) );
        SpLdpElement text = Factory::instance().create(_T("text"));
        text->push( new_string(_T("This is a text")) );
        text->push( new_element(k_dx, new_number(_T("12"))) );
        text->push( new_element(k_dy, new_number(_T("20.5"))) );
        note->push(text);
        //tcout << note->to_string() << "\n";
        CHECK( note->to_string() == _T("(n c4 q (stem up) (text \"This is a text\" (dx 12) (dy 20.5)))") );
    }

}
