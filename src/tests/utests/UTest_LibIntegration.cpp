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

#include <UnitTest++.h>
#include "../../score/defs.h"

#if defined _LM_DEBUG_ && lmUSE_LIBRARY

#include <wx/wxprec.h>

//classes related to these tests
#include "../../ldp_parser/LDPNode.h"
#include "../../ldp_parser/LDPParser.h"
#include "../../score/Score.h"
#include "../../score/Instrument.h"
#include "../../score/VStaff.h"
#include "../../score/Clef.h"
#include "../../score/Note.h"

#include "lomse_document.h"


using namespace std;
using namespace UnitTest;
using namespace lomse;

class lmLibIntegrationTestFixture
{
public:

    lmLibIntegrationTestFixture()     //SetUp fixture
    {
    }

    ~lmLibIntegrationTestFixture()    //TearDown fixture
    {
    }

};

SUITE(lmLibIntegrationTest)
{
    //TEST_FIXTURE(lmLibIntegrationTestFixture, IntegrationParserCreatesNode)
    //{
    //    lmLDPParser parser;
    //    lmLDPNode* pNode = parser.ParseText("(n c4 q)");
    //    CHECK( pNode );
    //    CHECK( pNode->to_string() == "(n c4 q)" );
    //}

    //TEST_FIXTURE(lmLibIntegrationTestFixture, IntegrationCreateScore)
    //{
    //    lmScore* pScore = new_score(); 
    //    CHECK( pScore );
    //    Document* pDoc = pScore->GetOwnerDocument();
    //    CHECK( pDoc );
    //    if (pDoc)
    //    {
    //        CHECK( pDoc->to_string() == "(lenmusdoc (vers 0.0) (score (vers 1.6) (language en utf-8)))" );
    //        Document::iterator itScore = pDoc->get_score();
    //        CHECK( pDoc->to_string(itScore) == "(score (vers 1.6) (language en utf-8))" );
    //    }
    //    delete pScore;
    //}

    //TEST_FIXTURE(lmLibIntegrationTestFixture, NodesHaveReferenceToNode)
    //{
    //    lmScore* pScore = new_score(); 
    //    lmInstrument* pInstr = pScore->AddInstrument(1, 1, _T(""));
    //    lmVStaff* pVStaff = pInstr->GetVStaff();
    //    pVStaff->AddClef( lmE_Sol );
    //    lmLDPParser parser;
    //    lmLDPNode* pNode = parser.ParseText("(n c4 q)");
    //    lmNote* pNote = parser.AnalyzeNote(pNode, pVStaff);
    //    CHECK( pNote );
    //    CHECK( pNote->GetLdpElement() == pNode );
    //    delete pScore;
    //}

}

#endif  // _LM_DEBUG_
