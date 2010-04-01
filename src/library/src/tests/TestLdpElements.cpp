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
        SpLdpElement clef = Factory::instance().create("clef");
        CHECK( clef->get_type() == k_clef );
        CHECK( clef->get_name() == "clef" );
    }

    TEST_FIXTURE(LdpElementsTestFixture, CanCreateElementFromType)
    {
        SpLdpElement clef = Factory::instance().create(k_clef);
        CHECK( clef->get_type() == k_clef );
        CHECK( clef->get_name() == "clef" );
    }

    TEST_FIXTURE(LdpElementsTestFixture, InvalidElementName)
    {
        SpLdpElement clef = Factory::instance().create("invalid");
        CHECK( clef->get_type() == k_undefined );
        CHECK( clef->get_name() == "undefined" );
    }

    TEST_FIXTURE(LdpElementsTestFixture, CanAddSimpleSubElements)
    {
        SpLdpElement note = Factory::instance().create("n");
        note->append_child( new_value(k_pitch, "c4") );
        //cout << note->to_string() << endl;
        CHECK( note->to_string() == "(n c4)" );
    }

    TEST_FIXTURE(LdpElementsTestFixture, CanAddCompositeSubElements)
    {
        SpLdpElement note = Factory::instance().create("n");
        note->append_child( new_value(k_pitch, "c4") );
        note->append_child( new_value(k_duration, "q") );
        note->append_child( new_element(k_stem, new_label("up")) );
        //cout << note->to_string() << endl;
        CHECK( note->to_string() == "(n c4 q (stem up))" );
    }

    TEST_FIXTURE(LdpElementsTestFixture, CanAddCompositeWithManySubElements)
    {
        SpLdpElement note = Factory::instance().create("n");
        note->append_child( new_value(k_pitch, "c4") );
        note->append_child( new_value(k_duration, "q") );
        note->append_child( new_element(k_stem, new_label("up")) );
        SpLdpElement text = Factory::instance().create("text");
        text->append_child( new_string("This is a text") );
        text->append_child( new_element(k_dx, new_number("12")) );
        text->append_child( new_element(k_dy, new_number("20.5")) );
        note->append_child(text);
        //cout << note->to_string() << "\n";
        CHECK( note->to_string() == "(n c4 q (stem up) (text \"This is a text\" (dx 12) (dy 20.5)))" );
    }

    TEST_FIXTURE(LdpElementsTestFixture, FactoryReturnsName)
    {
        const std::string& name = Factory::instance().get_name(k_score);
        CHECK( name == "score" );
    }

    TEST_FIXTURE(LdpElementsTestFixture, FactoryGetNameThrowsException)
    {
        bool fOk = false;
        try
        {
            const std::string& name 
                = Factory::instance().get_name(static_cast<ELdpElements>(99999));
        }
        catch(exception& e)
        {
            //cout << e.what() << endl;
            e.what();
            fOk = true;
        }
        CHECK( fOk );
    }

}
