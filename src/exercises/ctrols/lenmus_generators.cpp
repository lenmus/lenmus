//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2020 LenMus project
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

//lenmus
#include "lenmus_generators.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/longlong.h>
#include <wx/debug.h>       //wxASSERT

#include <wx/arrstr.h>      //AWARE: Required by wxsqlite3. In Linux GCC complains
                            //about wxArrayString not defined in wxsqlite3.h
#include <wx/wxsqlite3.h>

//lomse
#include <lomse_score_utilities.h>
#include <lomse_logger.h>
using namespace lomse;

//std
#include <algorithm>
using namespace std;


namespace lenmus
{


//=======================================================================================
// RandomGenerator implementation
//  Random generators for various elementes: clefs, notes, keys, time signatures, etc.
//  Generation methods accept as parameter a constrain object of appropriate class
//=======================================================================================
int RandomGenerator::random_number(int nMin, int nMax)
{
    // Generates a random number in the closed interval [nMin, nMax].

    //notice that rand() returns an int in the range 0 to RAND_MAX (= 0x7fff)
    int nRange = nMax - nMin + 1;
    int nRnd = rand() % nRange;     // 0..nRange-1 = 0..(nMax-nMin+1)-1 = 0..(nMax-nMin)
    return nRnd + nMin;             // nMin ... (nMax-nMin)+nMin = nMin...nMax
}

//---------------------------------------------------------------------------------------
bool RandomGenerator::flip_coin()
{
    return ((rand() & 0x01) == 0x01);     //true in odd number, false if even
}

//---------------------------------------------------------------------------------------
void RandomGenerator::shuffle(int nNum, int* pIdx)
{
    //fill array pointed by pIdx ( int idx[nNum] ) with unique integers
    //in the range 0..nNum-1, arranged at random

    for (int i=0; i < nNum; ++i)
        *(pIdx + i) = i;

    random_shuffle(pIdx, pIdx+nNum-1);
}

//---------------------------------------------------------------------------------------
EClef RandomGenerator::generate_clef(ClefConstrains* pValidClefs)
{
    // Generates a random clef, choosen to satisfy the received constraints

    int nWatchDog = 0;
    int nClef = random_number(k_min_clef_in_exercises, k_max_clef_in_exercises);
    while (!pValidClefs->IsValid((EClef)nClef))
    {
        nClef = random_number(k_min_clef_in_exercises, k_max_clef_in_exercises);
        if (nWatchDog++ == 1000)
        {
            LOMSE_LOG_ERROR("Program error: Loop detected");
            return k_min_clef_in_exercises;
        }
    }
    return (EClef)nClef;
}

//---------------------------------------------------------------------------------------
EKeySignature RandomGenerator::generate_key(KeyConstrains* pValidKeys)
{
    // Generates a random key signature, choosen to satisfy the received constraints

    int nWatchDog = 0;
    int nKey = random_number(k_min_key, k_max_key);
    while (!pValidKeys->IsValid((EKeySignature)nKey))
    {
        nKey = random_number(k_min_key, k_max_key);
        if (nWatchDog++ == 1000)
        {
            LOMSE_LOG_ERROR("Program error: Loop detected");
            return k_min_key;
        }
    }
    return (EKeySignature)nKey;
}

//---------------------------------------------------------------------------------------
EKeySignature RandomGenerator::random_key_signature()
{
    return (EKeySignature)random_number(k_min_key, k_max_key);
}

//---------------------------------------------------------------------------------------
ETimeSignature RandomGenerator::GenerateTimeSign(TimeSignConstrains* pValidTimeSignatures)
{
    // Generates a random time signature, choosen to satisfy the received constraints

    int nWatchDog = 0;
    int nKey = random_number(k_min_time_signature, k_max_time_signature);
    while (!pValidTimeSignatures->IsValid((ETimeSignature)nKey))
    {
        nKey = random_number(k_min_time_signature, k_max_time_signature);
        if (nWatchDog++ == 1000)
        {
            LOMSE_LOG_ERROR("Program error: Loop detected");
            return k_min_time_signature;
        }
    }
    return (ETimeSignature)nKey;
}

//---------------------------------------------------------------------------------------
ETimeSignature RandomGenerator::RandomTimeSignature()
{
    return (ETimeSignature)random_number(k_min_time_signature, k_max_time_signature);
}

//---------------------------------------------------------------------------------------
DiatonicPitch RandomGenerator::GenerateRandomDiatonicPitch(int nMinLine, int nRange,
                                                    bool fRests, EClef nClef)
{
    // Generates a random pitch in the range nMinLine to nMinLine+nRange-1,
    // both included.
    // If fRest==true also pitch = 0 (rest) can be generated.

    int nPitch;

    if (fRests)
    {
        //also generate rests
        nPitch = random_number(0, nRange);
        nPitch = (nPitch == nRange ? 0 : nPitch + nMinLine);
    }
    else
    {
        //do not generate rests
        nPitch = random_number(0, nRange-1) + nMinLine;
    }

    //correct note pitch to suit key signature base line
    nPitch += get_diatonic_pitch_for_first_line(static_cast<EClef>(nClef)) - 2;

    return nPitch;
}

//---------------------------------------------------------------------------------------
FPitch RandomGenerator::get_best_root_note(EClef nClef, EKeySignature nKey)
{
    //Returns the root pitch for natural scale in nKeySignature. The octave is
    //selected for best fit when using clef nClef. 'Best fit' means the natural
    //scale can be represented with a minimal number of leger lines.

    int step = KeyUtilities::get_step_for_root_note(nKey);

    // Get the accidentals implied by the key signature.
    // Each element of the array refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
    // and its value can be one of: 0=no accidental, -1 = a flat, 1 = a sharp
    int nAccidentals[7];
    KeyUtilities::get_accidentals_for_key(nKey, nAccidentals);
    int acc = nAccidentals[step];

    //choose octave for best fit
    int octave = 4;
    switch (nClef)
    {
        case k_clef_G2:   octave = (step > 6 ? 3 : 4);    break;
        case k_clef_F4:   octave = (step > 1 ? 2 : 3);    break;
        case k_clef_F3:   octave = (step > 3 ? 2 : 3);    break;
        case k_clef_C1:   octave = (step > 4 ? 3 : 4);    break;
        case k_clef_C2:   octave = (step > 2 ? 3 : 4);    break;
        case k_clef_C3:   octave = (step > 0 ? 3 : 4);    break;
        case k_clef_C4:   octave = (step > 5 ? 3 : 4);    break;
        default:
            octave = 4;
    }

    return FPitch(step, octave, acc);
}




//=======================================================================================
// helper functions
// wxTimeSpan::GetDays() returns an int. This limits precission to 89.78 years. It
// should return a long. Therefore I will define some helper functions to fix this
// and to simplify usage
//=======================================================================================
long GetDays(wxTimeSpan& ts)
{
    wxLongLong nDays = ts.GetSeconds() / (60 * 60 * 24);
    return nDays.ToLong();
}

//---------------------------------------------------------------------------------------
long GetAdditionalSeconds(wxTimeSpan& ts, long nDays)
{
    //Returns seconds after removing days

    wxLongLong nSeconds = (ts - wxTimeSpan::Days(nDays)).GetSeconds();
    return nSeconds.ToLong();
}



//=======================================================================================
// helper functions to centralize DB operations
//=======================================================================================
void CreateTable_Questions(wxSQLite3Database* pDB)
{
    //Create Questions table

    pDB->ExecuteUpdate("CREATE TABLE Questions ("
                            "SpaceID INTEGER"
                            ", SetID INTEGER"
                            ", QuestionID INTEGER"
                            ", Param0 INTEGER"
                            ", Param1 INTEGER"
                            ", Param2 INTEGER"
                            ", Param3 INTEGER"
                            ", Param4 INTEGER"
                            ", Grp INTEGER"
                            ", Asked INTEGER"
                            ", Success INTEGER"
                            ", Repetitions INTEGER"
                            ", LastAsked INTEGER"       //TimeSpan
                            ", DaysRepIntv INTEGER"     //TimeSpan
                            ");");
}

//---------------------------------------------------------------------------------------
void CreateTable_Sets(wxSQLite3Database* pDB)
{
    //Create Sets table

    pDB->ExecuteUpdate("CREATE TABLE Sets ("
                            "SetID INTEGER PRIMARY KEY AUTOINCREMENT"
                            ", SpaceID INTEGER"
                            ", SetName char(200)"
                            ");");
}


//---------------------------------------------------------------------------------------
void CreateTable_Spaces(wxSQLite3Database* pDB)
{
    //Create Spaces table

    pDB->ExecuteUpdate("CREATE TABLE Spaces ("
                            "SpaceID INTEGER PRIMARY KEY AUTOINCREMENT"
                            ", SpaceName char(200)"
                            ", User char(40)"
                            ", Repetitions INTEGER"
                            ", MandatoryParams INTEGER"
                            ", LastUsed INTEGER"        //DateTime
                            ", Creation INTEGER"        //DateTime
                            ", TotalRespTime INTEGER"   //TimeSpan
                            ", TotalAsked INTEGER"
                            ");");
}


//=======================================================================================
// Question implementation
//=======================================================================================
Question::Question(ApplicationScope& appScope, long nSpaceID, long nDeckID, long nParam0,
                   long nParam1, long nParam2, long nParam3, long nParam4,
                   int nBox, int nAskedTotal, int nSuccessTotal,
                   int nRepetitions, wxTimeSpan tsLastAsked, long nDaysRepIntv)
    : m_appScope(appScope)
    , m_nSpaceID(nSpaceID)
    , m_nDeckID(nDeckID)
    , m_nParam0(nParam0)
    , m_nParam1(nParam1)
    , m_nParam2(nParam2)
    , m_nParam3(nParam3)
    , m_nParam4(nParam4)
    , m_nBox(nBox)
    , m_nRepetitions(nRepetitions)
    , m_nAskedTotal(nAskedTotal)
    , m_nSuccessTotal(nSuccessTotal)
    , m_tsLastAsked(tsLastAsked)
{
    wxASSERT(nBox >=0 && nBox < k_num_boxes);
    wxASSERT(nAskedTotal >= 0);
    wxASSERT(nSuccessTotal >= 0 && nSuccessTotal <= nAskedTotal);
    wxASSERT(nSpaceID > 0);
    wxASSERT(nDeckID > 0);

    m_tsDaysRepIntv = wxTimeSpan::Days(nDaysRepIntv);
    m_nIndex = -1;      //not yet assigned
}

//---------------------------------------------------------------------------------------
Question::~Question()
{
}

//---------------------------------------------------------------------------------------
void Question::SaveQuestion(int nSpaceID)
{
    //Save data to DB

    wxSQLite3Database* pDB = m_appScope.get_database();

    //Create Questions table if it does not exist
    if (!pDB->TableExists("Questions"))
        CreateTable_Questions(pDB);

    //Get row from database table
    wxString sSQL = wxString::Format(
        "SELECT * FROM Questions WHERE (SpaceID = %d AND SetID = %d AND QuestionID = %d);",
        (int)nSpaceID, (int)m_nDeckID, (int)m_nIndex);
    wxSQLite3ResultSet q = pDB->ExecuteQuery(sSQL);
    if (!q.NextRow())
    {
        //Didn't exits. Insert this question data
        wxSQLite3Statement stmt = pDB->PrepareStatement(
            "INSERT INTO Questions VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
        stmt.Bind(1, nSpaceID);
        stmt.Bind(2, (int)m_nDeckID);
        stmt.Bind(3, (int)m_nIndex);
        stmt.Bind(4, (int)m_nParam0);
        stmt.Bind(5, (int)m_nParam1);
        stmt.Bind(6, (int)m_nParam2);
        stmt.Bind(7, (int)m_nParam3);
        stmt.Bind(8, (int)m_nParam4);
        stmt.Bind(9, (int)m_nBox);
        stmt.Bind(10, m_nAskedTotal);
        stmt.Bind(11, m_nSuccessTotal);
        stmt.Bind(12, m_nRepetitions);
        stmt.Bind(13, m_tsLastAsked.GetValue());
        stmt.Bind(14, m_tsDaysRepIntv.GetValue());
        stmt.ExecuteUpdate();
    }
    else
    {
        //Update saved data
        wxSQLite3Statement stmt = pDB->PrepareStatement(
            "UPDATE Questions SET Param0 = ?, Param1 = ?, Param2 = ?, "
            "Param3 = ?, Param4 = ?, Grp = ?, Asked = ?, Success = ?, "
            "Repetitions = ?, LastAsked = ?, DaysRepIntv = ? "
            "WHERE (SpaceID = ? AND SetID = ? AND QuestionID = ?);");
        stmt.Bind(1, (int)m_nParam0);
        stmt.Bind(2, (int)m_nParam1);
        stmt.Bind(3, (int)m_nParam2);
        stmt.Bind(4, (int)m_nParam3);
        stmt.Bind(5, (int)m_nParam4);
        stmt.Bind(6, (int)m_nBox);
        stmt.Bind(7, m_nAskedTotal);
        stmt.Bind(8, m_nSuccessTotal);
        stmt.Bind(9, m_nRepetitions);
        stmt.Bind(10, m_tsLastAsked.GetValue());
        stmt.Bind(11, m_tsDaysRepIntv.GetValue());
        stmt.Bind(12, nSpaceID);
        stmt.Bind(13, (int)m_nDeckID);
        stmt.Bind(14, m_nIndex);
        stmt.ExecuteUpdate();
    }
}

//---------------------------------------------------------------------------------------
bool Question::LoadQuestions(wxSQLite3Database* pDB, long nDeckID, ProblemSpace* pPS)
{
    //Load all questions for requested problem space and set, and add the question
    //to the problem space.
    //Returns true if data loaded

    try
    {
        //Get rows from database table
        long nSpaceID = pPS->GetSpaceID();
        wxString sSQL = wxString::Format(
            "SELECT * FROM Questions WHERE (SpaceID = %d AND SetID = %d);",
            (int)nSpaceID, (int)nDeckID);
        wxSQLite3ResultSet q = pDB->ExecuteQuery(sSQL);
        bool fThereIsData = false;
        while (q.NextRow())
        {
            long nQuestionID = (long)q.GetInt("QuestionID");
            long nParam0 = (long)q.GetInt("Param0");
            long nParam1 = (long)q.GetInt("Param1");
            long nParam2 = (long)q.GetInt("Param2");
            long nParam3 = (long)q.GetInt("Param3");
            long nParam4 = (long)q.GetInt("Param4");
            int nBox = q.GetInt("Grp");
            int nAsked = q.GetInt("Asked");
            int nSuccess = q.GetInt("Success");
            int nRepetitions = q.GetInt("Repetitions");
            wxTimeSpan tsLastAsked = wxTimeSpan( q.GetInt64("LastAsked") );
            long nDaysRepIntv = wxTimeSpan( q.GetInt64("DaysRepIntv") ).GetDays();

            Question* pQ = pPS->AddQuestion(nParam0, nParam1, nParam2, nParam3, nParam4,
                                            nBox, nAsked, nSuccess, nRepetitions,
                                            tsLastAsked, nDaysRepIntv);
            pQ->SetIndex(nQuestionID);

            fThereIsData = true;
        }
        return fThereIsData;    //Data loaded
    }
    catch (wxSQLite3Exception& e)
    {
        LOMSE_LOG_ERROR("Error in DB. Error code: %d, Message: '%s'",
                        e.GetErrorCode(), e.GetMessage().ToStdString().c_str() );
        return false;       //error
    }
}

//---------------------------------------------------------------------------------------
void Question::UpdateAsked(ProblemSpace* pPS)
{
    m_nAskedTotal++;
    m_tsLastAsked = wxDateTime::Now() - pPS->GetCreationDate();
}

//---------------------------------------------------------------------------------------
void Question::UpdateSuccess(ProblemSpace* pPS, bool fSuccess)
{
    if (fSuccess)
    {
        m_nSuccessTotal++;
        m_nRepetitions++;

        //promote question if repetitions threshold reached
        if (m_nRepetitions == pPS->RepetitionsThreshold())
        {
            m_nBox++;
            m_nRepetitions = 0;
            if (m_nBox == k_num_boxes)
                m_nBox--;
        }
    }
    else
    {
        m_nBox = 0;       //demote question
        m_nRepetitions = 0;
    }
}

//---------------------------------------------------------------------------------------
long Question::GetParam(int nNumParam)
{
    wxASSERT(nNumParam >=0 && nNumParam < 5);
    switch (nNumParam)
    {
        case 0: return m_nParam0;
        case 1: return m_nParam1;
        case 2: return m_nParam2;
        case 3: return m_nParam3;
        case 4: return m_nParam4;
    }
    return 0L;  //compiler happy
}



//=======================================================================================
// ProblemSpace implementation
//=======================================================================================
ProblemSpace::ProblemSpace(ApplicationScope& appScope)
    : m_appScope(appScope)
    , m_sSpaceName("")
    , m_nSpaceID(0)
    , m_tmCreation(wxDateTime::Now())
    , m_tmLastUsed(wxDateTime::Now())
    , m_nRepetitions(1)
    , m_nMandatoryParams(0)
{
}

//---------------------------------------------------------------------------------------
ProblemSpace::~ProblemSpace()
{
    ClearSpace();
}

//---------------------------------------------------------------------------------------
void ProblemSpace::ClearSpace()
{
    //delete all questions
    std::vector<Question*>::iterator it;
    for (it= m_questions.begin(); it != m_questions.end(); ++it)
        delete *it;
    m_questions.clear();

    //delete other data
    m_decks.clear();
    m_sSpaceName = "";
    m_nMandatoryParams = 0;
}

//---------------------------------------------------------------------------------------
bool ProblemSpace::load_deck(wxString& sDeckName)
{
    //load from DB all question for current space and set sDeckName.
    //Returns false if error (data not found)
    //AWARE: External representation of wxTimeSpan will be two
    //       32 bits fields: Days+Seconds

    wxASSERT (sDeckName != "");

    //get new set ID and save data
    m_nDeckID = get_deck_id(m_nSpaceID, sDeckName);
    m_sDeckName = sDeckName;

    //Check if this set is already loaded
    if (is_deck_loaded(m_nDeckID))
        return true;            //already loaded. Return no error.

    //load data from SQLite3 database
    wxSQLite3Database* pDB = m_appScope.get_database();
    bool fLoadOK = Question::LoadQuestions(pDB, m_nDeckID, this);
    if (fLoadOK)
        m_decks.push_back(m_nDeckID);

    return fLoadOK;
}

//---------------------------------------------------------------------------------------
bool ProblemSpace::is_deck_loaded(long nDeckID)
{
    //returns true if set is already loaded in this space

    std::list<long>::iterator it = std::find(m_decks.begin(), m_decks.end(), nDeckID);
    return it != m_decks.end();
}

//---------------------------------------------------------------------------------------
void ProblemSpace::SaveAndClear()
{
    //save problem space to configuration file
    //AWARE: External representation of wxTimeSpan will be two 32 bits fields: Days+Seconds

    if (m_sSpaceName == "")
    {
        ClearSpace();
        return;
    }

    //save data to database
    wxSQLite3Database* pDB = m_appScope.get_database();
    try
    {
        wxString sSQL;

        pDB->Begin();

        //Create Spaces table if it does not exist
        if (!pDB->TableExists("Spaces"))
            CreateTable_Spaces(pDB);

        //save Space data
        int nKey;
        sSQL = wxString::Format(
            "SELECT * FROM Spaces WHERE (SpaceName = '%s' AND User = '%s');",
            m_sSpaceName.wx_str(), m_sUser.wx_str() );

        wxSQLite3ResultSet q = pDB->ExecuteQuery(sSQL);
        if (q.NextRow())
        {
            //data found in table. Update data.
            nKey = q.GetInt(0);
            wxSQLite3Statement stmt = pDB->PrepareStatement(
                "UPDATE Spaces SET User = ?, Repetitions = ?, MandatoryParams = ?, "
                "LastUsed = ?, Creation = ?, TotalRespTime = ?, TotalAsked = ? "
                "WHERE (SpaceID = ?);");
            stmt.Bind(1, m_sUser);
            stmt.Bind(2, (int)m_nRepetitions);
            stmt.Bind(3, (int)m_nMandatoryParams);
            stmt.BindDateTime(4, m_tmLastUsed);
            stmt.BindDateTime(5, m_tmCreation);
            stmt.Bind(6, m_tsTotalRespTime.GetValue());
            stmt.Bind(7, (int)m_nTotalAsked);
            stmt.Bind(8, nKey);
            stmt.ExecuteUpdate();
        }
        else
        {
            //the problem space name was never stored. Do it now and get its key
            wxSQLite3Statement stmt =
                pDB->PrepareStatement("INSERT INTO Spaces VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
            stmt.Bind(1, m_sSpaceName);
            stmt.Bind(2, m_sUser);
            stmt.Bind(3, (int)m_nRepetitions);
            stmt.Bind(4, (int)m_nMandatoryParams);
            stmt.BindDateTime(5, m_tmLastUsed);
            stmt.BindDateTime(6, m_tmCreation);
            stmt.Bind(7, m_tsTotalRespTime.GetValue());
            stmt.Bind(8, (int)m_nTotalAsked);
            stmt.ExecuteUpdate();
            nKey = pDB->GetLastRowId().ToLong();
        }

        //save questions
        std::vector<Question*>::iterator it;
        for (it= m_questions.begin(); it != m_questions.end(); ++it)
            (*it)->SaveQuestion(nKey);

        pDB->Commit();

        //clear space
        ClearSpace();
    }
    catch (wxSQLite3Exception& e)
    {
        LOMSE_LOG_ERROR("Error in DB. Error code: %d, Message: '%s'",
                        e.GetErrorCode(), e.GetMessage().ToStdString().c_str() );
    }
}

//---------------------------------------------------------------------------------------
Question* ProblemSpace::GetQuestion(int iQ)
{
    wxASSERT(iQ >= 0 && iQ < GetSpaceSize());
    return m_questions[iQ];
}

//---------------------------------------------------------------------------------------
void ProblemSpace::NewSpace(wxString& sSpaceName, int nRepetitionsThreshold,
                              int nNumMandatoryParams)
{
    //Clear current data and prepares to load a new collection of questions. Loads Space from
    //DB if exists. Otherwise, creates it in DB

    ClearSpace();

    LoadSpace(sSpaceName, nRepetitionsThreshold, nNumMandatoryParams);
    m_sDeckName = "";
    m_tmLastUsed = wxDateTime::Now();
}

//---------------------------------------------------------------------------------------
void ProblemSpace::LoadSpace(wxString& sSpaceName, int nRepetitionsThreshold,
                               int nNumMandatoryParams)
{
    //If exists, load space data
    wxString sUser = ::wxGetUserId();
    wxASSERT (sUser != "" && sUser.Len() < 40);
    wxASSERT (sSpaceName != "" && sSpaceName.Len() < 200);

    //load data from database
    wxSQLite3Database* pDB = m_appScope.get_database();
    try
    {
        wxString sSQL;

        //if Spaces table doesn't exist create it
        if (!pDB->TableExists("Spaces"))
            CreateTable_Spaces(pDB);

        //Get data for problem space
        sSQL = wxString::Format(
            "SELECT * FROM Spaces WHERE (SpaceName = '%s' AND User = '%s');",
            sSpaceName.wx_str(), sUser.wx_str() );

        wxSQLite3ResultSet q = pDB->ExecuteQuery(sSQL);
        if (q.NextRow())
        {
            //data found in table
            m_nSpaceID = q.GetInt(0);
            m_sSpaceName = sSpaceName;
            m_sUser = sUser;
            m_nRepetitions = q.GetInt("Repetitions");
            m_nMandatoryParams = q.GetInt("MandatoryParams");
            m_tmLastUsed = q.GetDateTime("LastUsed");
            m_tmCreation = q.GetDateTime("Creation");
            m_tsTotalRespTime = wxTimeSpan( q.GetInt64("TotalRespTime") );
            m_nTotalAsked = q.GetInt("TotalAsked");

            wxASSERT(m_nRepetitions == nRepetitionsThreshold);
            wxASSERT(m_nMandatoryParams == nNumMandatoryParams);
        }
        else
        {
            //the problem space name was never stored. Do it now

            //Initialize problem space data
            m_sSpaceName = sSpaceName;
            m_sUser = sUser;
            m_nRepetitions = nRepetitionsThreshold;
            m_nMandatoryParams = nNumMandatoryParams;
            m_tmLastUsed = wxDateTime::Now();
            m_tmCreation = wxDateTime::Now();
            m_tsTotalRespTime = wxTimeSpan::Seconds(0);
            m_nTotalAsked = 0;

            //Store Space in DB and get its key
            wxSQLite3Statement stmt =
                pDB->PrepareStatement("INSERT INTO Spaces  (SpaceName, User, Repetitions, "
                                        "MandatoryParams, LastUsed, Creation, TotalRespTime, "
                                        "TotalAsked) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

            stmt.Bind(1, m_sSpaceName);
            stmt.Bind(2, m_sUser);
            stmt.Bind(3, (int)m_nRepetitions);
            stmt.Bind(4, (int)m_nMandatoryParams);
            stmt.BindDateTime(5, m_tmLastUsed);
            stmt.BindDateTime(6, m_tmCreation);
            stmt.Bind(7, m_tsTotalRespTime.GetValue());
            stmt.Bind(8, (int)m_nTotalAsked);
            stmt.ExecuteUpdate();
            m_nSpaceID = pDB->GetLastRowId().ToLong();
        }
    }
    catch (wxSQLite3Exception& e)
    {
        LOMSE_LOG_ERROR("Error in DB. Error code: %d, Message: '%s'",
                        e.GetErrorCode(), e.GetMessage().ToStdString().c_str() );
    }
}

//---------------------------------------------------------------------------------------
long ProblemSpace::get_deck_id(long nSpaceID, wxString& sDeckName)
{
    //Returns set ID. If set does not exist, create it in DB

    wxASSERT(nSpaceID > 0L);
    wxASSERT(sDeckName != "");
    wxASSERT(sDeckName.Len() < 200);

    //load data from SQLite3 database
    wxSQLite3Database* pDB = m_appScope.get_database();
    try
    {
        wxString sSQL;

        //if Sets table doesn't exist create it
        if (!pDB->TableExists("Sets"))
            CreateTable_Sets(pDB);

        //Get SetID for this set
        long nDeckID;
        sSQL = wxString::Format(
            "SELECT * FROM Sets WHERE (SetName = '%s' AND SpaceID = %d);",
            sDeckName.wx_str(), (int)nSpaceID);

        wxSQLite3ResultSet q = pDB->ExecuteQuery(sSQL);
        if (q.NextRow())
        {
            //key found in table
            nDeckID = q.GetInt(0);
            //wxLogMessage("[ProblemSpace::get_deck_id] SpaceID %d: SetName '%s' found in table. nDeckID: %d",
            //             nSpaceID, sDeckName.wx_str(), nDeckID );
        }
        else
        {
            //the set was never stored. Do it now and get its ID
            sSQL = wxString::Format(
                "INSERT INTO Sets (SpaceID, SetName) VALUES (%d, '%s');",
                (int)nSpaceID, sDeckName.wx_str());
            pDB->ExecuteUpdate(sSQL);
            nDeckID = pDB->GetLastRowId().ToLong();
            //wxLogMessage("[ProblemSpace::get_deck_id] SpaceID %d: SetName '%s' NOT found in table. Created. ID: %d",
            //             nSpaceID, sDeckName.wx_str(), nDeckID );
        }
        return nDeckID;
    }
    catch (wxSQLite3Exception& e)
    {
        LOMSE_LOG_ERROR("Error in DB. Error code: %d, Message: '%s'",
                        e.GetErrorCode(), e.GetMessage().ToStdString().c_str() );
    }
    return 0;   //error. //TODO: Replace by trow ?
}

//---------------------------------------------------------------------------------------
void ProblemSpace::start_new_deck(wxString& sDeckName)
{
    //Prepare to add LENMUS_NEW questions to LENMUS_NEW set

    wxASSERT(sDeckName != "");

    //Get ID and save data for current Set
    m_nDeckID = get_deck_id(m_nSpaceID, sDeckName);
    m_sDeckName = sDeckName;
    m_nQIndexForDeck = 0;
    wxASSERT(!is_deck_loaded(m_nDeckID));
}

//---------------------------------------------------------------------------------------
Question* ProblemSpace::AddQuestion(long nParam0, long nParam1,
                                    long nParam2, long nParam3, long nParam4,
                                    int nBox, int nAskedTotal, int nSuccessTotal,
                                    int nRepetitions, wxTimeSpan tsLastAsked,
                                    long nDaysRepIntv)
{
    //Adds question to space, to current set. It does not save data as this will
    //be done when saving the space

    wxASSERT(m_nDeckID > 0 && m_sDeckName != "");

    Question* pQ = LENMUS_NEW Question(m_appScope, m_nSpaceID, m_nDeckID, nParam0, nParam1,
                                nParam2, nParam3, nParam4, nBox, nAskedTotal,
                                nSuccessTotal, nRepetitions, tsLastAsked,
                                nDaysRepIntv);
    m_questions.push_back(pQ);
    return pQ;
}

//---------------------------------------------------------------------------------------
void ProblemSpace::AddNewQuestion(long nParam0, long nParam1, long nParam2, long nParam3,
                                  long nParam4)
{
    Question* pQ = AddQuestion(nParam0, nParam1, nParam2, nParam3, nParam4);
    pQ->SetIndex( ++m_nQIndexForDeck );
}

//---------------------------------------------------------------------------------------
long ProblemSpace::GetQuestionParam(int iQ, int nNumParam)
{
    //Returns value for param nNumParam in question iQ

    wxASSERT(iQ >= 0 && iQ < GetSpaceSize());
    wxASSERT(nNumParam >= 0 && nNumParam < m_nMandatoryParams);
    return m_questions[iQ]->GetParam(nNumParam);

}

//---------------------------------------------------------------------------------------
bool ProblemSpace::IsQuestionParamMandatory(int nNumParam)
{
    //Returns true if for current space value for param nNumParam must be taken
    //from question params. Returns false in opposite case, that is, if value for
    //param must be generated by the exercise Ctrol.

    return nNumParam < m_nMandatoryParams;
}


//---------------------------------------------------------------------------------------
// ProblemManager implementation
//-------------------------------------------------------------------------------------------------

ProblemManager::ProblemManager(ApplicationScope& appScope)
    : m_ProblemSpace(appScope)
{
}

//---------------------------------------------------------------------------------------
ProblemManager::~ProblemManager()
{
    m_ProblemSpace.SaveAndClear();
}

//---------------------------------------------------------------------------------------
void ProblemManager::save_problem_space()
{
    m_ProblemSpace.SaveAndClear();
}

//---------------------------------------------------------------------------------------
bool ProblemManager::load_deck(wxString& sDeckName)
{
    //Reads all questions from requested set and adds them to current problem space.
    //Returns false space does not exist.

    return m_ProblemSpace.load_deck(sDeckName);
}

//---------------------------------------------------------------------------------------
void ProblemManager::add_question_to_deck(long nParam0, long nParam1, long nParam2, long nParam3,
                                        long nParam4)
{
    //Adds a question to current set. It does not save data as this will be done when
    //saving the space

    m_ProblemSpace.AddNewQuestion(nParam0, nParam1, nParam2, nParam3, nParam4);
}

//---------------------------------------------------------------------------------------
bool ProblemManager::IsQuestionParamMandatory(int nNumParam)
{
    //Returns true if for current space value for param nNumParam must be taken
    //from question params. Returns false in opposite case, that is, if value for
    //param must be generated by the exercise Ctrol.

    return m_ProblemSpace.IsQuestionParamMandatory(nNumParam);
}

//---------------------------------------------------------------------------------------
long ProblemManager::GetQuestionParam(int iQ, int nNumParam)
{
    //Returns value for param nNumParam in question iQ
    return m_ProblemSpace.GetQuestionParam(iQ, nNumParam);
}



//=======================================================================================
// LeitnerManager implementation
//=======================================================================================

//weighting factors for questions
double LeitnerManager::m_w[k_num_boxes] = {
    0.0, 0.1, 0.2, 0.3, 0.4,
    1.0, 1.1, 1.2, 1.3, 1.4,
    2.0, 2.1, 2.2, 2.3, 2.4, 2.5
};

//ranges for boxes sets (Short, Medium, Long)
enum {
    k_min_S = 0,
    k_max_S = 4,
    k_min_M = 5,
    k_max_M = 9,
    k_min_L = 10,
    k_max_L = 15,
};

//---------------------------------------------------------------------------------------
LeitnerManager::LeitnerManager(ApplicationScope& appScope, bool fLearningMode)
    : ProblemManager(appScope)
    , m_fLearningMode(fLearningMode)
{
    //reset counters for statistics
    m_nUnlearned = 0;
    m_nToReview = 0;
    m_nTotal = 0;
    m_nRight = 0;
    m_nWrong = 0;

    m_fThereWhereQuestions = false;
}

//---------------------------------------------------------------------------------------
LeitnerManager::~LeitnerManager()
{
}

//---------------------------------------------------------------------------------------
void LeitnerManager::OnProblemSpaceChanged()
{
    update_problem_space();
}

//---------------------------------------------------------------------------------------
void LeitnerManager::update_problem_space()
{
    if (m_fLearningMode)
        update_problem_space_for_learning();
    else
        update_problem_space_for_practising();
}

//---------------------------------------------------------------------------------------
void LeitnerManager::update_problem_space_for_learning()
{
    //reset counters for statistics
    m_nUnlearned = 0;
    m_nToReview = 0;
    m_nTotal = 0;

    //clear boxes
    for (int i=0; i < k_num_boxes; i++)
        m_NumQuestions[i] = 0;

    //Explore all questions, compute statistics and move to Set0 all those questions whose
    //sheduled time is <= Today
    m_set0.clear();
    int nMaxQuestion = m_ProblemSpace.GetSpaceSize();
    for (int iQ=0; iQ < nMaxQuestion; iQ++)
    {
        Question* pQ = m_ProblemSpace.GetQuestion(iQ);
        int nBox = pQ->get_box_index();
        wxDateTime tsScheduled = m_ProblemSpace.GetCreationDate() + pQ->GetSheduledTimeSpan();
        if (tsScheduled <= wxDateTime::Today() || nBox == 0)
        {
            //scheduled for today. Add to set
            m_fThereWhereQuestions = true;
            int times = m_ProblemSpace.RepetitionsThreshold() - pQ->GetRepetitions();
            for (int i=0; i < times; ++i)
                m_set0.push_back(iQ);

            //statistics
            if (nBox == 0)
                m_nUnlearned += times;
            else
                m_nToReview += times;
        }

        //Create the groups
        int iG = pQ->get_box_index();
        wxASSERT(iG >=0 && iG < k_num_boxes);
        m_NumQuestions[iG]++;
    }
    m_nTotal = m_nUnlearned + m_nToReview;

    //Check if there are questions for today
    if (m_set0.size() == 0)
    {
        change_to_practise_mode();
        return;
    }
}

//---------------------------------------------------------------------------------------
void LeitnerManager::update_problem_space_for_practising()
{
    //reset counters for statistics
    m_nRight = 0;
    m_nWrong = 0;

    //clear boxes
    for (int iB=0; iB < k_num_boxes; ++iB)
    {
        if (m_box[iB].size() > 0)
            m_box[iB].clear();
    }

    //fill boxes with the indices to contained questions
    int nMaxQuestion = m_ProblemSpace.GetSpaceSize();
    for (int iQ=0; iQ < nMaxQuestion; iQ++)
    {
        int iB = m_ProblemSpace.get_box_index(iQ);
        wxASSERT(iB >=0 && iB < k_num_boxes);
        m_box[iB].push_back(iQ);
    }

    //compute groups probability range

    //determine TBi and Total
    //
    //  TBi = i
    //
    //          n
    //          --
    //  Total = > (if box is empty ? 0 : TBi)
    //          --
    //          i=0
    //

    double rTotal = 0.0;
    for (int i=0; i < k_num_boxes; i++)
    {
        rTotal += (m_box[i].empty() ? 0 : i);
    }

    // And assign probability to each box as follows:
    //
    //      If box i is empty
    //          probability = 0.0
    //
    //      Else
    //          P(i) = (Total - TBi)/Total
    //          PFROM(i)=PTO(i-1)
    //          PTO(i) = PFROM(i) + P(i)
    //
    double rLastRange = 0.0;
    for (int i=0; i < k_num_boxes; i++)
    {
        if (m_box[i].empty())
            m_range[i] = -1.0;
        else
        {
            m_range[i] = rLastRange + (rTotal - double(i)) / rTotal;
            if (m_range[i] == 0.0) m_range[i] = 1.0;
            rLastRange = m_range[i];
            LOMSE_LOG_ERROR("m_range[%d] = %.4f", i, m_range[i]);
        }
    }

    //fix any truncation error in last valid range
    for (int i=k_num_boxes-1; i >= 0; i--)
    {
        if (m_range[i] != -1.0)
        {
            m_range[i] = 1.0;
            break;
        }
    }
}

//---------------------------------------------------------------------------------------
void LeitnerManager::change_to_practise_mode()
{
    wxString sStartOfMsg = "";      //without this, compiler complains: cannot add two pointers
    if (m_fThereWhereQuestions)
        wxMessageBox(sStartOfMsg + _("No more scheduled work for today.") + " "
            + _("Exercise will be changed to 'Practise' mode."));
    else
        wxMessageBox(sStartOfMsg + _("No scheduled work for today.") + " "
            + _("Exercise will be changed to 'Practise' mode."));

    m_fLearningMode = false;     //change to practise mode
    update_problem_space_for_practising();
}

//---------------------------------------------------------------------------------------
int LeitnerManager::ChooseQuestion()
{
    //Method to choose a question. Returns question index or -1 if no more questions

    if (m_fLearningMode)
        return choose_question_for_learning();
    else
        return choose_question_for_practising();
}

//---------------------------------------------------------------------------------------
bool LeitnerManager::more_questions()
{
    return (!IsLearningMode() || m_set0.size() > 0);
}

//---------------------------------------------------------------------------------------
int LeitnerManager::choose_question_for_learning()
{
    if (!IsLearningMode() || m_set0.size() == 0)
    {
        change_to_practise_mode();
        return choose_question_for_practising();
    }
    else
    {
        m_iQ = RandomGenerator::random_number(0, int(m_set0.size())-1);
        //wxLogMessage("[LeitnerManager::choose_question_for_learning] set size=%d, m_iQ=%d",
        //             m_set0.size(), m_iQ);
        return m_set0[m_iQ];
    }
}

//---------------------------------------------------------------------------------------
void LeitnerManager::remove_current_question()
{
    m_set0.erase (m_set0.begin() + m_iQ);
}

//---------------------------------------------------------------------------------------
int LeitnerManager::choose_question_for_practising()
{
    //Method to choose a question. Returns question index
    //The algorithm to select a question is as follows:
    // 1. Select at random a box, with probabilities defined table m_range[iB]
    // 2. Select at random a question from selected box

    //select group
    float rB = (float)RandomGenerator::random_number(0, 10000) / 10000.0f;
    int iB;
    for (iB=0; iB < k_num_boxes; iB++)
    {
        if (rB <= m_range[iB]) break;
    }
    //coverity scan sanity check
    if (iB >= k_num_boxes)
    {
        stringstream msg;
        msg << "Logic error. iB should be lower than k_num_boxes, but not. rB="
            << rB << ", m_range={";
        for (int i=0; i < iB; i++)
            msg << m_range[i] << ",";

        msg << "}";
        LOMSE_LOG_ERROR(msg.str());

        iB = 0;
    }

    //select question from box
    int nBoxSize = (int)m_box[iB].size();
    wxASSERT(nBoxSize > 0);
    int iQ = RandomGenerator::random_number(0, nBoxSize-1);

    //return index to selected question
    return m_box[iB].at(iQ);
}

//---------------------------------------------------------------------------------------
void LeitnerManager::UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse)
{
    //Method to account for the answer

    wxASSERT(iQ >= 0 && iQ < m_ProblemSpace.GetSpaceSize());

    if (m_fLearningMode)
        return update_question_for_learning(iQ, fSuccess, tsResponse);
    else
        return update_question_for_practising(iQ, fSuccess, tsResponse);
}

//---------------------------------------------------------------------------------------
void LeitnerManager::update_question_for_learning(int iQ, bool fSuccess, wxTimeSpan tsResponse)
{
    //update question statistics and promote/demote question
    Question* pQ = m_ProblemSpace.GetQuestion(iQ);
    int iOldBox = pQ->get_box_index();
    pQ->UpdateAsked(&m_ProblemSpace);
    pQ->UpdateSuccess(&m_ProblemSpace, fSuccess);

    //schedule next repetition and update statistics
    if (fSuccess)
    {
        //Question answered right. If repetition threshold reached, schedule it for
        //repetition after some time
        if (pQ->GetRepetitions() >= m_ProblemSpace.RepetitionsThreshold())
        {
            wxTimeSpan tsDaysInvtal = get_repetition_interval(pQ->get_box_index());
            pQ->SetRepetitionInterval( wxDateTime::Today() + tsDaysInvtal - m_ProblemSpace.GetCreationDate() );
        }

        remove_current_question();

        //statistics: compute success question as learned/reviewed
        if (iOldBox == 0)
            m_nUnlearned--;
        else
            m_nToReview--;
    }
    else
    {
        //Question answered wrong. Schedule it for inmmediate repetition.
        pQ->SetRepetitionInterval( wxDateTime::Today() - m_ProblemSpace.GetCreationDate() );

        //statistics: compute failed question as 'unlearned'
        if (iOldBox > 0)
        {
            m_nUnlearned++;
            m_nToReview--;
        }
    }

    //update boxes
    int iNewBox = pQ->get_box_index();
    if (iOldBox != iNewBox)
    {
        m_NumQuestions[iOldBox]--;
        m_NumQuestions[iNewBox]++;
    }

    //update times
    m_ProblemSpace.AddTotalRespTime( tsResponse );    //total response time since start
    m_ProblemSpace.IncrementTotalAsked();             //total num questions asked since start
}

//---------------------------------------------------------------------------------------
void LeitnerManager::update_question_for_practising(int WXUNUSED(iQ), bool fSuccess,
                                                    wxTimeSpan WXUNUSED(tsResponse))
{
    //in practise mode no performance data is updated/saved. Only update displayed statistics

    if (fSuccess)
        m_nRight++;
    else
        m_nWrong++;
}

//---------------------------------------------------------------------------------------
wxTimeSpan LeitnerManager::get_repetition_interval(int nBox)
{
    //return repetion interval (days) for received box

    static wxTimeSpan tsInterval[k_num_boxes] =
    {
        wxTimeSpan::Days(1),        //Box 0
        wxTimeSpan::Days(4),        //Box 1
        wxTimeSpan::Days(7),        //Box 2
        wxTimeSpan::Days(12),       //Box 3
        wxTimeSpan::Days(20),       //Box 4
        wxTimeSpan::Days(30),       //Box 5
        wxTimeSpan::Days(60),       //Box 6
        wxTimeSpan::Days(90),       //Box 7
        wxTimeSpan::Days(150),      //Box 8
        wxTimeSpan::Days(270),      //Box 9
        wxTimeSpan::Days(480),      //Box 10
        wxTimeSpan::Days(720),      //Box 11
        wxTimeSpan::Days(1440),     //Box 12
        wxTimeSpan::Days(2160),     //Box 13
        wxTimeSpan::Days(3960),     //Box 14
        wxTimeSpan::Days(6120),     //Box 15
    };

    if (nBox >= k_num_boxes)
        nBox = k_num_boxes-1;
    return tsInterval[nBox];
}

//---------------------------------------------------------------------------------------
int LeitnerManager::get_new()
{
    return m_nUnlearned;
}

//---------------------------------------------------------------------------------------
int LeitnerManager::get_expired()
{
    return m_nToReview;
}

//---------------------------------------------------------------------------------------
int LeitnerManager::get_total()
{
    return m_nTotal;
}

//---------------------------------------------------------------------------------------
void LeitnerManager::ResetPractiseCounters()
{
    m_nRight = 0;
    m_nWrong = 0;
}

//---------------------------------------------------------------------------------------
float LeitnerManager::get_global_progress()
{
    int nPoints = 0;
    int nTotal = 0;
    for (int iG=0; iG < k_num_boxes; iG++)
    {
        nPoints  += iG * m_NumQuestions[iG];
        nTotal += m_NumQuestions[iG];
    }
    if (nTotal == 0)
        return 0.0f;
    else
        return (float)(100 * nPoints) / (float)((k_num_boxes-1) * nTotal);
}

//---------------------------------------------------------------------------------------
float LeitnerManager::get_session_progress()
{
    if (m_nTotal == 0)
        return 0.0f;
    else
        return (float)(100 * (m_nTotal - m_nUnlearned - m_nToReview)) / (float)m_nTotal;
}

//---------------------------------------------------------------------------------------
void LeitnerManager::compute_achievement_indicators()
{
    //compute S_S and TQ_S
    double SS = 0.0;
    int TQS = 0;
    for (int i=k_min_S; i <= k_max_S; i++)
    {
       SS += m_w[i] * m_NumQuestions[i];
       TQS += m_NumQuestions[i];
    }

    //compute S_M and TQ_M
    double SM = 0.0;
    int TQM = 0;
    for (int i=k_min_M; i <= k_max_M; i++)
    {
       SM += m_w[i] * m_NumQuestions[i];
       TQM += m_NumQuestions[i];
    }

    //compute S_L and TQ_L
    double SL = 0.0;
    int TQL = 0;
    for (int i=k_min_L; i <= k_max_L; i++)
    {
       SL += m_w[i] * m_NumQuestions[i];
       TQL += m_NumQuestions[i];
    }

    int TQT = TQS + TQM + TQL;

    m_short = (SS + m_w[k_max_S]*(TQM + TQL)) / (m_w[k_max_S] * TQT);
    m_medium = TQM+TQL > 0 ? (SS + SM + m_w[k_max_M]*TQL) / (m_w[k_max_M] * TQT) : 0.0;
    m_long = SL / (m_w[k_max_L] * TQT);

    ////DEBUG -----------------------------------------------------------------------------
    //wxString msg = "";
    //for (int i=0; i < k_num_boxes; i++)
    //    msg += wxString::Format("%d, ", m_NumQuestions[i]);
    //wxLogMessage("[LeitnerManager::compute_achievement_indicators] TQT=%d, TQS=%d, "
    //             "TQM=%d, TQL=%d, SS=%.01f, SM=%.01f, SL=%.01f, q=%s",
    //             TQT, TQS, TQM, TQL, SS, SM, SL, msg.wx_str());
    ////END DEBUG -------------------------------------------------------------------------
}

//---------------------------------------------------------------------------------------
float LeitnerManager::get_short_term_progress()
{
    return m_short;
}

//---------------------------------------------------------------------------------------
float LeitnerManager::get_medium_term_progress()
{
    return m_medium;
}

//---------------------------------------------------------------------------------------
float LeitnerManager::get_long_term_progress()
{
    return m_long;
}

//---------------------------------------------------------------------------------------
const wxString LeitnerManager::get_progress_report()
{
    //get average user response time
    wxString sAvrgRespTime = " ";
    sAvrgRespTime += _("Unknown");
    int nAsked = m_ProblemSpace.GetTotalAsked();
    if (nAsked > 0)
    {
        double rMillisecs = m_ProblemSpace.GetTotalRespTime().GetMilliseconds().ToDouble() / (double)nAsked;
        sAvrgRespTime = wxString::Format("%.0f ", rMillisecs);
        sAvrgRespTime += _("milliseconds");
    }

    //Prepare message
    wxString m_sHeader = "<html><body>";
    wxString sContent = m_sHeader +
        "<center><h3>" + _("Session report") + "</h3></center><p>" +
        _("New questions:") + wxString::Format(" %d", m_nUnlearned) + "<br>" +
        _("Questions to review:") + wxString::Format(" %d", m_nToReview) + "<br>" +
        _("Average answer time:") + sAvrgRespTime + "<br>" +
       //"</p><center><h3>" + _("Progress report") + "</h3></center><p>" +
       // _("Program build date:") + " " __TDATE__ "<br>" +
       // _("Your computer information:") +
        "</p></body></html>";

#if 0
    wxString sContent = m_sHeader +
        "<center><h3>" + _("Session report") + "</h3></center><p>" +
        _("New questions:") + wxString::Format(" %d", m_nUnlearned) + "<br>" +
        _("Questions to review:") + wxString::Format(" %d", m_nToReview) + "<br>" +
        _("Average answer time:") + sAvrgRespTime + "<br>" +
       //"</p><center><h3>" + _("Progress report") + "</h3></center><p>" +
       // _("Program build date:") + " " __TDATE__ "<br>" +
       // _("Your computer information:") +
        "</p></body></html>";

    //**
Questions:
    Two numbers:
         * The first one is the number of unlearned questions: those that are in group 0.
         * The second one is the number of expired questions: those in higher groups whose repetition interval has arrived.

EST (Estimated Session Time):
    The estimated remaining time to review all questions in today assignment (unlearned + expired) at current answering pace.

Session progress:
    It is an indicator of your achievement in current session: The ratio (percentage) between learned today and total for today

Achievement indicators:
    It is a global indicator of your achievement. It is a subjective evaluation of your achieved long term knowledge level.
    Three percentages:
         * The first one (red) is the number of unlearned questions: those that are in group 0.
         * The second one (orange) is the number of expired questions: those in higher groups whose repetition interval has arrived.
         * The third one (green) is the number of expired questions: those in higher groups whose repetition interval has arrived.

#endif


    return sContent;
}

//---------------------------------------------------------------------------------------
wxTimeSpan LeitnerManager::get_estimated_session_time()
{
    //Return the estimated time span for answering all unknown + expired questions
    //After some testing using current session data for the estimation produces estimations that
    //vary greatly from one question to the next one. Therefore, for the estimation I will use
    //only historical data. It is more conservative (greater times) but times are more consistent.

    //get average user response time
    double rMillisecs;
    int nAsked = m_ProblemSpace.GetTotalAsked();
    if (nAsked > 0)
        rMillisecs = m_ProblemSpace.GetTotalRespTime().GetMilliseconds().ToDouble() / (double)nAsked;
    else
        rMillisecs = 10000;          //assume 10 secs per question if no data available

    //return estimation
    return wxTimeSpan::Milliseconds( (wxLongLong)(double(m_nUnlearned + m_nToReview) * rMillisecs) );
}




//=======================================================================================
// QuizManager implementation
//=======================================================================================
QuizManager::QuizManager(ApplicationScope& appScope)
    : ProblemManager(appScope)
{
    //initializations
    m_nMaxTeam = 0;
    m_nCurrentTeam = 0;
    ResetCounters();
}

//---------------------------------------------------------------------------------------
QuizManager::~QuizManager()
{
}

//---------------------------------------------------------------------------------------
int QuizManager::ChooseQuestion()
{
    //Method to choose a question. Returns question index
    //The algorithm to select a question is just to choose a question at random.

    //select question at random.
    int nSize = m_ProblemSpace.GetSpaceSize();
    wxASSERT(nSize > 0);
    return RandomGenerator::random_number(0, nSize-1);
}

//---------------------------------------------------------------------------------------
void QuizManager::UpdateQuestion(int WXUNUSED(iQ), bool fSuccess,
                                 wxTimeSpan WXUNUSED(tsResponse))
{
    if (fSuccess)
        m_nRight[m_nCurrentTeam]++;
    else
        m_nWrong[m_nCurrentTeam]++;
}

//---------------------------------------------------------------------------------------
void QuizManager::ResetCounters()
{
    for (int i=0; i < 2; i++)
    {
        m_nRight[i] = 0;
        m_nWrong[i] = 0;
    }
    m_fStart = true;
}

//---------------------------------------------------------------------------------------
void QuizManager::NextTeam()
{
    // move to next team
    if (m_nMaxTeam > 1)
    {
        m_nCurrentTeam++;
        m_nCurrentTeam = m_nCurrentTeam % m_nMaxTeam;
    }

    //ensure that first time after a reset we start with first team
    if (m_fStart)
    {
        m_nCurrentTeam = 0;
        m_fStart = false;
    }
}


}   //namespace lenmus
