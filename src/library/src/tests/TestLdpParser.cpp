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
#include <iostream>

//classes related to these tests
#include "../parser/LdpReader.h"
#include "../parser/LdpTokenizer.h"
#include "../parser/LdpParser.h"


//-------------------------------------------------------------------------------------
// tests
//
// Elements
//  - Can create an element
//

using namespace UnitTest;
using namespace std;
using namespace lenmus;

class LdpTextReaderTestFixture
{
public:

    LdpTextReaderTestFixture()     //SetUp fixture
    {
    }

    ~LdpTextReaderTestFixture()    //TearDown fixture
    {
    }
};

SUITE(LdpTextReaderTest)
{
    TEST_FIXTURE(LdpTextReaderTestFixture, CanCreateTextReader)
    {
        LdpTextReader reader(_T("123 abc"));
        CHECK( reader.is_ready() );
    }

    TEST_FIXTURE(LdpTextReaderTestFixture, CanReadFromTextReader)
    {
        LdpTextReader reader(_T("123 abc"));
        CHECK( reader.get_next_char() == L'1' );
        CHECK( reader.get_next_char() == L'2' );
        CHECK( reader.get_next_char() == L'3' );
        CHECK( reader.get_next_char() == L' ' );
        CHECK( reader.get_next_char() == L'a' );
        CHECK( reader.get_next_char() == L'b' );
        CHECK( reader.get_next_char() == L'c' );
    }

    TEST_FIXTURE(LdpTextReaderTestFixture, TextReaderReturnsEOF)
    {
        LdpTextReader reader(_T("abc"));
        CHECK( reader.get_next_char() == L'a' );
        CHECK( reader.get_next_char() == L'b' );
        CHECK( reader.get_next_char() == L'c' );
        CHECK( reader.get_next_char() == EOF );
    }

    TEST_FIXTURE(LdpTextReaderTestFixture, TextReaderEndOfDataWorks)
    {
        LdpTextReader reader(_T("abc"));
        CHECK( reader.get_next_char() == L'a' );
        CHECK( reader.get_next_char() == L'b' );
        CHECK( reader.get_next_char() == L'c' );
        CHECK( reader.end_of_data() );
    }

    TEST_FIXTURE(LdpTextReaderTestFixture, TextReaderCanUnreadOneChar)
    {
        LdpTextReader reader(_T("abc"));
        reader.get_next_char();
        CHECK( reader.get_next_char() == L'b' );
        reader.repeat_last_char();
        CHECK( reader.get_next_char() == L'b' );
        CHECK( reader.get_next_char() == L'c' );
        CHECK( reader.end_of_data() );
    }

}

//---------------------------------------------------------------------------
class LdpFileReaderTestFixture
{
public:

    LdpFileReaderTestFixture()     //SetUp fixture
    {
    }

    ~LdpFileReaderTestFixture()    //TearDown fixture
    {
    }
};

SUITE(LdpFileReaderTest)
{
    TEST_FIXTURE(LdpFileReaderTestFixture, InvalidFileThrowException)
    {
        bool fOk = false;
        try
        {
            LdpFileReader reader(_T("../../invalid_path/no-score.lms"));
        }
        catch(...)
        {
            fOk = true;
        }
        CHECK(fOk);
    }

    TEST_FIXTURE(LdpFileReaderTestFixture, FileReaderCanOpenFile)
    {
        try
        {
            LdpFileReader reader(_T("../test-scores/00011-empty-fill-page.lms"));
            CHECK( reader.is_ready() );
        }
        catch(const exception& e)
        {
            tcout << e.what() << endl;
        }
    }

    TEST_FIXTURE(LdpFileReaderTestFixture, CanReadFromFileReader)
    {
        LdpFileReader reader(_T("../test-scores/00011-empty-fill-page.lms"));
        CHECK( reader.get_next_char() == L'(' );
    }

    TEST_FIXTURE(LdpFileReaderTestFixture, FileReaderCanUnreadOneChar)
    {
        LdpFileReader reader(_T("../test-scores/00011-empty-fill-page.lms"));
        reader.get_next_char();
        CHECK( reader.get_next_char() == L's' );
        reader.repeat_last_char();
        CHECK( reader.get_next_char() == L's' );
        CHECK( reader.get_next_char() == L'c' );
    }

    TEST_FIXTURE(LdpFileReaderTestFixture, FileReaderReachesEndOfData)
    {
        LdpFileReader reader(_T("../test-scores/00011-empty-fill-page.lms"));
        CHECK( reader.end_of_data() == false );
        CHECK( reader.get_next_char() != EOF );
        while( !reader.end_of_data())
            reader.get_next_char();
        CHECK( reader.end_of_data() );
    }

}


//---------------------------------------------------------------------------
class LdpTokenizerTestFixture
{
public:

    LdpTokenizerTestFixture()     //SetUp fixture
    {
    }

    ~LdpTokenizerTestFixture()    //TearDown fixture
    {
    }
};

SUITE(LdpTokenizerTest)
{
    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadTokens)
    {
        LdpTextReader reader(_T("(score blue)"));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token != NULL );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadRightTokens)
    {
        LdpTextReader reader(_T("(score blue)"));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkStartOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "score" );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "blue" );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkEndOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkEndOfFile );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerCanReadFile)
    {
        LdpFileReader reader(_T("../test-scores/00011-empty-fill-page.lms"));
        LdpTokenizer tokenizer(reader, tcout);
        int numTokens = 0;
        for (LdpToken* token = tokenizer.read_token();
             token->get_type() != tkEndOfFile;
             token = tokenizer.read_token())
        {
            numTokens++;
            //tcout << token->get_value() << endl;
        }
        CHECK( reader.end_of_data() );
        CHECK( numTokens == 50 );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerCanReadUnicodeString)
    {
        //tcout << _T("'") << _T("Текст на кирилица") << _T("'") << endl;
        LdpFileReader reader(_T("../test-scores/00002-unicode-text.lms"));
        LdpTokenizer tokenizer(reader, tcout);
        int numTokens = 0;
        LdpToken* token = tokenizer.read_token();
        for (; token->get_type() != tkEndOfFile; token = tokenizer.read_token())
        {
            numTokens++;
            if (token->get_type() == tkString)
                break;
        }
        //tcout << _T("'") << token->get_value() << _T("'") << endl;
        CHECK( token->get_type() == tkString );
        CHECK( numTokens == 59 );
        CHECK( token->get_value() == _T("Текст на кирилица") );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadIntegerNumber)
    {
        LdpTextReader reader(_T("(dx 15)"));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkStartOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "dx" );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkIntegerNumber );
        CHECK( token->get_value() == "15" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadCompactNotation)
    {
        LdpTextReader reader(_T("dx:15"));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkStartOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "dx" );
        token = tokenizer.read_token();
        //tcout << _T("type='") << token->get_type() << _T("' value='") 
        //      << _T("' value='") << token->get_value() << _T("'") << endl;
        CHECK( token->get_type() == tkIntegerNumber );
        CHECK( token->get_value() == "15" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadCompactNotationTwo)
    {
        LdpTextReader reader(_T("dx:15 dy:12.77"));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkStartOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "dx" );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkIntegerNumber );
        CHECK( token->get_value() == "15" );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkEndOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkStartOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "dy" );
        token = tokenizer.read_token();
        //tcout << _T("type='") << token->get_type() << _T("' value='") 
        //      << _T("' value='") << token->get_value() << _T("'") << endl;
        CHECK( token->get_type() == tkRealNumber );
        CHECK( token->get_value() == "12.77" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadStringSimpleQuotes)
    {
        LdpTextReader reader(_T(" ''this is a string'' "));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkString );
        CHECK( token->get_value() == _T("this is a string") );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadStringDoubleQuotes)
    {
        LdpTextReader reader(_T(" \"this is a string\" "));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkString );
        CHECK( token->get_value() == _T("this is a string") );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadStringSimpleQuotesTranslatable)
    {
        LdpTextReader reader(_T(" _''this is a string'' "));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkString );
        CHECK( token->get_value() == _T("this is a string") );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadStringDoubleQuotesTranslatable)
    {
        LdpTextReader reader(_T(" _\"this is a string\" "));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkString );
        CHECK( token->get_value() == _T("this is a string") );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadLabel_1)
    {
        LdpTextReader reader(_T(" #00ff45 "));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == _T("#00ff45") );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadLabel_2)
    {
        LdpTextReader reader(_T(" score "));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == _T("score") );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadLabel_3)
    {
        LdpTextReader reader(_T(" 45a "));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == _T("45a") );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerPositiveIntegerNumber)
    {
        LdpTextReader reader(_T(" +45 "));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkIntegerNumber );
        CHECK( token->get_value() == _T("+45") );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerNegativeIntegerNumber)
    {
        LdpTextReader reader(_T(" -45 "));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkIntegerNumber );
        CHECK( token->get_value() == _T("-45") );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerPositiveRealNumber)
    {
        LdpTextReader reader(_T(" +45.98 "));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkRealNumber );
        CHECK( token->get_value() == _T("+45.98") );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerNegativeRealNumber)
    {
        LdpTextReader reader(_T(" -45.70 "));
        LdpTokenizer tokenizer(reader, tcout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkRealNumber );
        CHECK( token->get_value() == _T("-45.70") );
    }

}



//---------------------------------------------------------------------------
class LdpParserTestFixture
{
public:

    LdpParserTestFixture()     //SetUp fixture
    {
    }

    ~LdpParserTestFixture()    //TearDown fixture
    {
    }

    //lmScore* m_pScore;
    //lmInstrument* m_pInstr;
    //lmVStaff* m_pVStaff;
    //lmColStaffObjs* m_pCol;

    //void CreateEmptyScore()
    //{
    //}

    //void DeleteTestData()
    //{
    //}
};

SUITE(LdpParserTest)
{
    TEST_FIXTURE(LdpParserTestFixture, ParserReadTokensFromText)
    {
        LdpParser parser(tcout);
        SpLdpElement score = parser.parse_text(_T("(score (vers 1.7))"));
        //tcout << score->to_string() << endl;
        CHECK( score->to_string() == _T("(score (vers 1.7))") );
    }

    TEST_FIXTURE(LdpParserTestFixture, ParserReadScoreFromFile)
    {
        LdpParser parser(tcout);
        SpLdpElement score = parser.parse_file(_T("../test-scores/00011-empty-fill-page.lms"));
        //tcout << score->to_string() << endl;
        CHECK( score->to_string() == _T("(score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))") );
    }

    TEST_FIXTURE(LdpParserTestFixture, ParserReadScoreFromUnicodeFile)
    {
        LdpParser parser(tcout);
        SpLdpElement score = parser.parse_file(_T("../test-scores/00002-unicode-text.lms"));
        //tcout << score->to_string() << endl;
        CHECK( score->to_string() == _T("(score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData (clef G) (n c4 q) (text \"Текст на кирилица\" (dx 15) (dy -10) (font normal 10)))))") );
    }

}

