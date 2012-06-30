//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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

#include "lenmus_exercise_ctrol.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/longlong.h>

#include <wx/arrstr.h>      //AWARE: Required by wxsqlite3. In Linux GCC complains
                            //about wxArrayString not defined in wxsqlite3.h
#include <wx/wxsqlite3.h>

//#include <algorithm>
//#include <numeric>
//#include <vector>
//#include <iterator>

//lomse
#include <lomse_score_utilities.h>
using namespace lomse;


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
EClefExercise RandomGenerator::generate_clef(ClefConstrains* pValidClefs)
{
    // Generates a random clef, choosen to satisfy the received constraints

    int nWatchDog = 0;
    int nClef = random_number(lmMIN_CLEF, lmMAX_CLEF);
    while (!pValidClefs->IsValid((EClefExercise)nClef))
    {
        nClef = random_number(lmMIN_CLEF, lmMAX_CLEF);
        if (nWatchDog++ == 1000)
        {
            wxMessageBox(_("Program error: Loop detected in RandomGenerator::generate_clef."));
            return lmMIN_CLEF;
        }
    }
    return (EClefExercise)nClef;
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
            wxMessageBox(_("Program error: Loop detected in RandomGenerator::generate_key."));
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
            wxMessageBox(_("Program error: Loop detected in RandomGenerator::GenerateTime."));
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
                                                    bool fRests, EClefExercise nClef)
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
FPitch RandomGenerator::get_best_root_note(EClefExercise nClef, EKeySignature nKey)
{
    //Returns the root pitch for natural scale in nKeySignature. The octave is 
    //selected for best fit when using clef nClef. 'Best fit' means the natural
    //scale can be represented with a minimal number of leger lines.

    int step = get_step_for_root_note(nKey);

    // Get the accidentals implied by the key signature.
    // Each element of the array refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
    // and its value can be one of: 0=no accidental, -1 = a flat, 1 = a sharp
    int nAccidentals[7];
    get_accidentals_for_key(nKey, nAccidentals);
    int acc = nAccidentals[step];

    //choose octave for best fit
    int octave = 4;
    switch (nClef)
    {
        case lmE_G:   octave = (step > 6 ? 3 : 4);    break;
        case lmE_Fa4:   octave = (step > 1 ? 2 : 3);    break;
        case lmE_Fa3:   octave = (step > 3 ? 2 : 3);    break;
        case lmE_Do1:   octave = (step > 4 ? 3 : 4);    break;
        case lmE_Do2:   octave = (step > 2 ? 3 : 4);    break;
        case lmE_Do3:   octave = (step > 0 ? 3 : 4);    break;
        case lmE_Do4:   octave = (step > 5 ? 3 : 4);    break;
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

    pDB->ExecuteUpdate(_T("CREATE TABLE Questions (")
                            _T("SpaceID INTEGER")
                            _T(", SetID INTEGER")
                            _T(", QuestionID INTEGER")
                            _T(", Param0 INTEGER")
                            _T(", Param1 INTEGER")
                            _T(", Param2 INTEGER")
                            _T(", Param3 INTEGER")
                            _T(", Param4 INTEGER")
                            _T(", Grp INTEGER")
                            _T(", Asked INTEGER")
                            _T(", Success INTEGER")
                            _T(", Repetitions INTEGER")
                            _T(", LastAsked INTEGER")       //TimeSpan
                            _T(", DaysRepIntv INTEGER")     //TimeSpan
                            _T(");"));
}

//---------------------------------------------------------------------------------------
void CreateTable_Sets(wxSQLite3Database* pDB)
{
    //Create Sets table

    pDB->ExecuteUpdate(_T("CREATE TABLE Sets (")
                            _T("SetID INTEGER PRIMARY KEY AUTOINCREMENT")
                            _T(", SpaceID INTEGER")
                            _T(", SetName char(200)")
                            _T(");"));
}


//---------------------------------------------------------------------------------------
void CreateTable_Spaces(wxSQLite3Database* pDB)
{
    //Create Spaces table

    pDB->ExecuteUpdate(_T("CREATE TABLE Spaces (")
                            _T("SpaceID INTEGER PRIMARY KEY AUTOINCREMENT")
                            _T(", SpaceName char(200)")
                            _T(", User char(40)")
                            _T(", Repetitions INTEGER")
                            _T(", MandatoryParams INTEGER")
                            _T(", LastUsed INTEGER")        //DateTime
                            _T(", Creation INTEGER")        //DateTime
                            _T(", TotalRespTime INTEGER")   //TimeSpan
                            _T(", TotalAsked INTEGER")
                            _T(");"));
}


//=======================================================================================
// Question implementation
//=======================================================================================
Question::Question(ApplicationScope& appScope, long nSpaceID, long nSetID, long nParam0,
                   long nParam1, long nParam2, long nParam3, long nParam4,
                   int nGroup, int nAskedTotal, int nSuccessTotal,
                   int nRepetitions, wxTimeSpan tsLastAsked, long nDaysRepIntv)
    : m_appScope(appScope)
    , m_nSpaceID(nSpaceID)
    , m_nSetID(nSetID)
    , m_nParam0(nParam0)
    , m_nParam1(nParam1)
    , m_nParam2(nParam2)
    , m_nParam3(nParam3)
    , m_nParam4(nParam4)
    , m_nGroup(nGroup)
    , m_nRepetitions(nRepetitions)
    , m_nAskedTotal(nAskedTotal)
    , m_nSuccessTotal(nSuccessTotal)
    , m_tsLastAsked(tsLastAsked)
{
    wxASSERT(nGroup >=0 && nGroup < lmNUM_GROUPS);
    wxASSERT(nAskedTotal >= 0);
    wxASSERT(nSuccessTotal >= 0 && nSuccessTotal <= nAskedTotal);
    wxASSERT(nSpaceID > 0);
    wxASSERT(nSetID > 0);

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
    if (!pDB->TableExists(_T("Questions")))
        CreateTable_Questions(pDB);

    //Get row from database table
    wxString sSQL = wxString::Format(
        _T("SELECT * FROM Questions WHERE (SpaceID = %d AND SetID = %d AND QuestionID = %d);"),
        nSpaceID, m_nSetID, m_nIndex);
    wxSQLite3ResultSet q = pDB->ExecuteQuery(sSQL.c_str());
    if (!q.NextRow())
    {
        //Didn't exits. Insert this question data
        wxSQLite3Statement stmt = pDB->PrepareStatement(
            _T("INSERT INTO Questions VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"));
        stmt.Bind(1, nSpaceID);
        stmt.Bind(2, (int)m_nSetID);
        stmt.Bind(3, (int)m_nIndex);
        stmt.Bind(4, (int)m_nParam0);
        stmt.Bind(5, (int)m_nParam1);
        stmt.Bind(6, (int)m_nParam2);
        stmt.Bind(7, (int)m_nParam3);
        stmt.Bind(8, (int)m_nParam4);
        stmt.Bind(9, (int)m_nGroup);
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
            _T("UPDATE Questions SET Param0 = ?, Param1 = ?, Param2 = ?, ")
            _T("Param3 = ?, Param4 = ?, Grp = ?, Asked = ?, Success = ?, ")
            _T("Repetitions = ?, LastAsked = ?, DaysRepIntv = ? ")
            _T("WHERE (SpaceID = ? AND SetID = ? AND QuestionID = ?);"));
        stmt.Bind(1, (int)m_nParam0);
        stmt.Bind(2, (int)m_nParam1);
        stmt.Bind(3, (int)m_nParam2);
        stmt.Bind(4, (int)m_nParam3);
        stmt.Bind(5, (int)m_nParam4);
        stmt.Bind(6, (int)m_nGroup);
        stmt.Bind(7, m_nAskedTotal);
        stmt.Bind(8, m_nSuccessTotal);
        stmt.Bind(9, m_nRepetitions);
        stmt.Bind(10, m_tsLastAsked.GetValue());
        stmt.Bind(11, m_tsDaysRepIntv.GetValue());
        stmt.Bind(12, nSpaceID);
        stmt.Bind(13, (int)m_nSetID);
        stmt.Bind(14, m_nIndex);
        stmt.ExecuteUpdate();
    }
}

//---------------------------------------------------------------------------------------
bool Question::LoadQuestions(wxSQLite3Database* pDB, long nSetID, ProblemSpace* pPS)
{
    //Load all questions for requested problem space and set, and add the question
    //to the problem space.
    //Returns true if data loaded

    try
    {
        //Get rows from database table
        long nSpaceID = pPS->GetSpaceID();
        wxString sSQL = wxString::Format(
            _T("SELECT * FROM Questions WHERE (SpaceID = %d AND SetID = %d);"),
            nSpaceID, nSetID);
        wxSQLite3ResultSet q = pDB->ExecuteQuery(sSQL.c_str());
        bool fThereIsData = false;
        while (q.NextRow())
        {
            long nQuestionID = (long)q.GetInt(_T("QuestionID"));
            long nParam0 = (long)q.GetInt(_T("Param0"));
            long nParam1 = (long)q.GetInt(_T("Param1"));
            long nParam2 = (long)q.GetInt(_T("Param2"));
            long nParam3 = (long)q.GetInt(_T("Param3"));
            long nParam4 = (long)q.GetInt(_T("Param4"));
            int nGroup = q.GetInt(_T("Grp"));
            int nAsked = q.GetInt(_T("Asked"));
            int nSuccess = q.GetInt(_T("Success"));
            int nRepetitions = q.GetInt(_T("Repetitions"));
            wxTimeSpan tsLastAsked = wxTimeSpan( q.GetInt64(_T("LastAsked")) );
            long nDaysRepIntv = wxTimeSpan( q.GetInt64(_T("DaysRepIntv")) ).GetDays();

            Question* pQ = pPS->AddQuestion(nParam0, nParam1, nParam2, nParam3, nParam4,
                                            nGroup, nAsked, nSuccess, nRepetitions,
                                            tsLastAsked, nDaysRepIntv);
            pQ->SetIndex(nQuestionID);

            fThereIsData = true;
        }
        return fThereIsData;    //Data loaded
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage(_T("[ProblemSpace::LoadSet] Error in DB. Error code: %d, Message: '%s'"),
                 e.GetErrorCode(), e.GetMessage().c_str() );
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
            m_nGroup++;
            m_nRepetitions = 0;
            if (m_nGroup == lmNUM_GROUPS)
                m_nGroup--;
        }
    }
    else
    {
        m_nGroup = 0;       //demote question
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
{
    m_tmCreation = wxDateTime::Now();
    m_tmLastUsed = wxDateTime::Now();
    m_sSpaceName = _T("");
    m_nSpaceID = 0;
    m_nRepetitions = 1;
    m_nMandatoryParams = 0;
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
    m_sets.clear();
    m_sSpaceName = _T("");
    m_nMandatoryParams = 0;
}

//---------------------------------------------------------------------------------------
bool ProblemSpace::LoadSet(wxString& sSetName)
{
    //load from DB all question for current space and set sSetName.
    //Returns false if error (data not found)
    //AWARE: External representation of wxTimeSpan will be two
    //       32 bits fields: Days+Seconds

    wxASSERT (sSetName != _T(""));

    //get LENMUS_NEW set ID and save data
    m_nSetID = GetSetID(m_nSpaceID, sSetName);
    m_sSetName = sSetName;

    //Check if this set is already loaded
    if (IsSetLoaded(m_nSetID))
        return true;            //already loaded. Return no error.

    wxDateTime tmLastUsed = wxDateTime::Now();

    //load data from SQLite3 database
    wxSQLite3Database* pDB = m_appScope.get_database();
    bool fLoadOK = Question::LoadQuestions(pDB, m_nSetID, this);
    if (fLoadOK)
        m_sets.push_back(m_nSetID);

    return fLoadOK;
}

//---------------------------------------------------------------------------------------
bool ProblemSpace::IsSetLoaded(long nSetID)
{
    //returns true if set is already loaded in this space

    std::list<long>::iterator it = std::find(m_sets.begin(), m_sets.end(), m_nSetID);
    return it != m_sets.end();
}

//---------------------------------------------------------------------------------------
void ProblemSpace::SaveAndClear()
{
    //save problem space to configuration file
    //AWARE: External representation of wxTimeSpan will be two 32 bits fields: Days+Seconds

    if (m_sSpaceName == _T(""))
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
        if (!pDB->TableExists(_T("Spaces")))
            CreateTable_Spaces(pDB);

        //save Space data
        int nKey;
        sSQL = wxString::Format(
            _T("SELECT * FROM Spaces WHERE (SpaceName = '%s' AND User = '%s');"),
            m_sSpaceName.c_str(), m_sUser.c_str() );

        wxSQLite3ResultSet q = pDB->ExecuteQuery(sSQL.c_str());
        if (q.NextRow())
        {
            //data found in table. Update data.
            nKey = q.GetInt(0);
            wxSQLite3Statement stmt = pDB->PrepareStatement(
                _T("UPDATE Spaces SET User = ?, Repetitions = ?, MandatoryParams = ?, ")
                _T("LastUsed = ?, Creation = ?, TotalRespTime = ?, TotalAsked = ? ")
                _T("WHERE (SpaceID = ?);"));
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
                pDB->PrepareStatement(_T("INSERT INTO Spaces VALUES (?, ?, ?, ?, ?, ?, ?, ?)"));
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
        wxLogMessage(_T("[ProblemSpace::SaveAndClear] Error in DB. Error code: %d, Message: '%s'"),
                    e.GetErrorCode(), e.GetMessage().c_str() );
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
    //Clear current data and prepares to load a LENMUS_NEW collection of questions. Loads Space from
    //DB if exists. Otherwise, creates it in DB

    ClearSpace();

    LoadSpace(sSpaceName, nRepetitionsThreshold, nNumMandatoryParams);
    m_sSetName = _T("");
    m_tmLastUsed = wxDateTime::Now();
}

//---------------------------------------------------------------------------------------
void ProblemSpace::LoadSpace(wxString& sSpaceName, int nRepetitionsThreshold,
                               int nNumMandatoryParams)
{
    //If exists, load space data
    wxString sUser = ::wxGetUserId();
    wxASSERT (sUser != _T("") && sUser.Len() < 40);
    wxASSERT (sSpaceName != _T("") && sSpaceName.Len() < 200);

    //load data from database
    wxSQLite3Database* pDB = m_appScope.get_database();
    try
    {
        wxString sSQL;

        //if Spaces table doesn't exist create it
        if (!pDB->TableExists(_T("Spaces")))
            CreateTable_Spaces(pDB);

        //Get data for problem space
        sSQL = wxString::Format(
            _T("SELECT * FROM Spaces WHERE (SpaceName = '%s' AND User = '%s');"),
            sSpaceName.c_str(), sUser.c_str() );

        wxSQLite3ResultSet q = pDB->ExecuteQuery(sSQL.c_str());
        if (q.NextRow())
        {
            //data found in table
            m_nSpaceID = q.GetInt(0);
            m_sSpaceName = sSpaceName;
            m_sUser = sUser;
            m_nRepetitions = q.GetInt(_T("Repetitions"));
            m_nMandatoryParams = q.GetInt(_T("MandatoryParams"));
            m_tmLastUsed = q.GetDateTime(_T("LastUsed"));
            m_tmCreation = q.GetDateTime(_T("Creation"));
            m_tsTotalRespTime = wxTimeSpan( q.GetInt64(_T("TotalRespTime")) );
            m_nTotalAsked = q.GetInt(_T("TotalAsked"));

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
                pDB->PrepareStatement(_T("INSERT INTO Spaces  (SpaceName, User, Repetitions, ")
                                        _T("MandatoryParams, LastUsed, Creation, TotalRespTime, ")
                                        _T("TotalAsked) VALUES (?, ?, ?, ?, ?, ?, ?, ?)"));

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
        wxLogMessage(_T("[ProblemSpace::LoadSpace] Error in DB. Error code: %d, Message: '%s'"),
                 e.GetErrorCode(), e.GetMessage().c_str() );
    }
}

//---------------------------------------------------------------------------------------
long ProblemSpace::GetSetID(long nSpaceID, wxString& sSetName)
{
    //Returns set ID. If set does not exist, create it in DB

    wxASSERT(nSpaceID > 0L);
    wxASSERT(sSetName != _T(""));
    wxASSERT(sSetName.Len() < 200);

    //load data from SQLite3 database
    wxSQLite3Database* pDB = m_appScope.get_database();
    try
    {
        wxString sSQL;

        //if Sets table doesn't exist create it
        if (!pDB->TableExists(_T("Sets")))
            CreateTable_Sets(pDB);

        //Get SetID for this set
        long nSetID;
        sSQL = wxString::Format(
            _T("SELECT * FROM Sets WHERE (SetName = '%s' AND SpaceID = %d);"),
            sSetName.c_str(), nSpaceID);

        wxSQLite3ResultSet q = pDB->ExecuteQuery(sSQL.c_str());
        if (q.NextRow())
        {
            //key found in table
            nSetID = q.GetInt(0);
            //wxLogMessage(_T("[ProblemSpace::GetSetID] SpaceID %d: SetName '%s' found in table. nSetID: %d"),
            //             nSpaceID, sSetName.c_str(), nSetID );
        }
        else
        {
            //the set was never stored. Do it now and get its ID
            sSQL = wxString::Format(
                _T("INSERT INTO Sets (SpaceID, SetName) VALUES (%d, '%s');"),
                nSpaceID, sSetName.c_str());
            pDB->ExecuteUpdate(sSQL.c_str());
            nSetID = pDB->GetLastRowId().ToLong();
            //wxLogMessage(_T("[ProblemSpace::GetSetID] SpaceID %d: SetName '%s' NOT found in table. Created. ID: %d"),
            //             nSpaceID, sSetName.c_str(), nSetID );
        }
        return nSetID;
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage(_T("[ProblemSpace::GetSetID] Error in DB. Error code: %d, Message: '%s'"),
                 e.GetErrorCode(), e.GetMessage().c_str() );
    }
    return 0;   //error. //TODO: Replace by trow ?
}

//---------------------------------------------------------------------------------------
void ProblemSpace::StartNewSet(wxString& sSetName)
{
    //Prepare to add LENMUS_NEW questions to LENMUS_NEW set

    wxASSERT(sSetName != _T(""));

    //Get ID and save data for current Set
    m_nSetID = GetSetID(m_nSpaceID, sSetName);
    m_sSetName = sSetName;
    m_nSetQIndex = 0;
    wxASSERT(!IsSetLoaded(m_nSetID));
}

//---------------------------------------------------------------------------------------
Question* ProblemSpace::AddQuestion(long nParam0, long nParam1,
                                    long nParam2, long nParam3, long nParam4,
                                    int nGroup, int nAskedTotal, int nSuccessTotal,
                                    int nRepetitions, wxTimeSpan tsLastAsked,
                                    long nDaysRepIntv)
{
    //Adds question to space, to current set. It does not save data as this will
    //be done when saving the space

    wxASSERT(m_nSetID > 0 && m_sSetName != _T(""));

    Question* pQ = LENMUS_NEW Question(m_appScope, m_nSpaceID, m_nSetID, nParam0, nParam1,
                                nParam2, nParam3, nParam4, nGroup, nAskedTotal,
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
    pQ->SetIndex( ++m_nSetQIndex );
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

ProblemManager::ProblemManager(ApplicationScope& appScope, ExerciseCtrol* pOwnerExercise)
    : m_ProblemSpace(appScope)
    , m_pOwnerExercise(pOwnerExercise)
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
bool ProblemManager::LoadSet(wxString& sSetName)
{
    //Reads all questions from requested set and adds them to current problem space.
    //Returns false space does not exist.

    return m_ProblemSpace.LoadSet(sSetName);
}

//---------------------------------------------------------------------------------------
void ProblemManager::AddQuestionToSet(long nParam0, long nParam1, long nParam2, long nParam3,
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
LeitnerManager::LeitnerManager(ApplicationScope& appScope, 
                               ExerciseCtrol* pOwnerExercise, bool fLearningMode)
    : ProblemManager(appScope, pOwnerExercise)
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
    UpdateProblemSpace();
}

//---------------------------------------------------------------------------------------
void LeitnerManager::UpdateProblemSpace()
{
    if (m_fLearningMode)
        UpdateProblemSpaceForLearning();
    else
        UpdateProblemSpaceForPractising();
}

//---------------------------------------------------------------------------------------
void LeitnerManager::UpdateProblemSpaceForLearning()
{
    //reset counters for statistics
    m_nUnlearned = 0;
    m_nToReview = 0;
    m_nTotal = 0;

    //clear groups
    for (int iG=0; iG < lmNUM_GROUPS; iG++)
        m_NumQuestions[iG] = 0;

    //Explore all questions, compute statistics and move to Set0 all those questions whose
    //sheduled time is <= Today
    m_set0.clear();
    int nMaxQuestion = m_ProblemSpace.GetSpaceSize();
    for (int iQ=0; iQ < nMaxQuestion; iQ++)
    {
        Question* pQ = m_ProblemSpace.GetQuestion(iQ);
        int nGroup = pQ->GetGroup();
        wxDateTime tsScheduled = m_ProblemSpace.GetCreationDate() + pQ->GetSheduledTimeSpan();
        if (tsScheduled <= wxDateTime::Today() || nGroup == 0)
        {
            //scheduled for today. Add to set
            m_set0.push_back(iQ);
            m_fThereWhereQuestions = true;

            //statistics
            if (nGroup == 0)
                m_nUnlearned += m_ProblemSpace.RepetitionsThreshold() - pQ->GetRepetitions();
            else
                m_nToReview += m_ProblemSpace.RepetitionsThreshold() - pQ->GetRepetitions();
        }

        //Create the groups
        int iG = pQ->GetGroup();
        wxASSERT(iG >=0 && iG < lmNUM_GROUPS);
        m_NumQuestions[iG]++;
    }
    m_nTotal = m_nUnlearned + m_nToReview;

    //Check if there are questions for today
    if (m_set0.size() == 0)
    {
        //No questions for today
        wxString sStartOfMsg = _T("");      //without this, compiler complains: cannot add two pointers
        if (m_fThereWhereQuestions)
            wxMessageBox(sStartOfMsg + _("No more scheduled work for today.") + _T(" ")
                + _("Exercise will be changed to 'Practise' mode."));
        else
            wxMessageBox(sStartOfMsg + _("No scheduled work for today.") + _T(" ")
                + _("Exercise will be changed to 'Practise' mode."));
        m_fLearningMode = false;     //change to practise mode
        m_pOwnerExercise->change_mode(k_practise_mode);
        //m_pOwnerExercise->change_generation_mode_label(k_practise_mode);
        //m_pOwnerExercise->change_counters_ctrol();
        UpdateProblemSpaceForPractising();
        return;
    }

    //Shuffle Set0 (random ordering).
    std::random_shuffle( m_set0.begin(), m_set0.end() );

    //Set iterator to questions
    m_it0 = m_set0.begin();
}

//---------------------------------------------------------------------------------------
void LeitnerManager::UpdateProblemSpaceForPractising()
{
    //reset counters for statistics
    m_nRight = 0;
    m_nWrong = 0;

    //clear groups
    for (int iG=0; iG < lmNUM_GROUPS; iG++)
        m_group[iG].clear();

    //Compute the groups
    int nMaxQuestion = m_ProblemSpace.GetSpaceSize();
    for (int iQ=0; iQ < nMaxQuestion; iQ++)
    {
        int iG = m_ProblemSpace.GetGroup(iQ);
        wxASSERT(iG >=0 && iG < lmNUM_GROUPS);
        m_group[iG].push_back(iQ);
    }

    //compute groups probability range

    //determine number of times each group was repeated and total number of repetitions
    //
    //  TGi = RTn/RTi
    //
    //          n
    //          --
    //  Total = > (Gi is empty ? 0 : TGi)
    //          --
    //          i=0
    //

    double rTotal = 0.0;
    double rTG[lmNUM_GROUPS];
    double rRTn = (double)GetRepetitionInterval(lmNUM_GROUPS-1).GetDays();
    for (int i=0; i < lmNUM_GROUPS; i++)
    {
        rTG[i] = rRTn / (double)GetRepetitionInterval(i).GetDays();
        rTotal += (m_group[i].empty() ? 0 : rTG[i]);
    }

    // And assign probability to each group as follows:
    //
    //      If group i is empty
    //          assing it a probability range [0.0, 0.0]
    //
    //      Else
    //          Probability group i: P(i) = TG(i)/Total
    //                               PFROM(i)=PTO(i-1)
    //                               PTO(i) = PFROM(i) + P(i)
    //
    double rLastRange = 0.0;
    for (int i=0; i < lmNUM_GROUPS; i++)
    {
        if (m_group[i].empty())
            m_range[i] = -1.0;
        else
        {
            m_range[i] = rLastRange + rTG[i] / rTotal;
            rLastRange = m_range[i];
            //wxLogMessage(_T("[LeitnerManager::UpdateProblemSpaceForPractising] m_range[%d] = %.4f"), i, m_range[i]);
        }
    }

    //fix any truncation error in last valid range
    for (int i=lmNUM_GROUPS-1; i >= 0; i--)
    {
        if (m_range[i] != -1.0)
        {
            m_range[i] = 1.0;
            break;
        }
    }
}

//---------------------------------------------------------------------------------------
int LeitnerManager::ChooseQuestion()
{
    //Method to choose a question. Returns question index or -1 if no more questions

    if (m_fLearningMode)
        return ChooseQuestionForLearning();
    else
        return ChooseQuestionForPractising();
}

//---------------------------------------------------------------------------------------
int LeitnerManager::ChooseQuestionForLearning()
{
    if (m_it0 == m_set0.end())
    {
        //end of set reached. Rebuild set
        UpdateProblemSpace();

        //If no more questions scheduled for today, previous invocation to UpdateProblemSpace()
        //will change the mode to 'Practise mode'. If this is the case, choose question for
        //practising
        if (!IsLearningMode())
            return ChooseQuestionForPractising();
    }

    wxASSERT(m_it0 != m_set0.end());
    int iQ = *m_it0;
    ++m_it0;
    return iQ;
}

//---------------------------------------------------------------------------------------
int LeitnerManager::ChooseQuestionForPractising()
{
    //Method to choose a question. Returns question index
    //The algorithm to select a question is as follows:
    // 1. Select at random a question group, with group probabilities defined table m_range[iG]
    // 2. Select at random a question from selected group

    //select group
    float rG = (float)RandomGenerator::random_number(0, 10000) / 10000.0f;
    int iG;
    for (iG=0; iG < lmNUM_GROUPS; iG++)
    {
        if (rG <= m_range[iG]) break;
    }
    wxASSERT(iG < lmNUM_GROUPS);

    //select question from group
    int nGroupSize = (int)m_group[iG].size();
    wxASSERT(nGroupSize > 0);
    int iQ = RandomGenerator::random_number(0, nGroupSize-1);

    //return index to selected question
    return m_group[iG].at(iQ);
}

//---------------------------------------------------------------------------------------
void LeitnerManager::UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse)
{
    //Method to account for the answer

    wxASSERT(iQ >= 0 && iQ < m_ProblemSpace.GetSpaceSize());

    if (m_fLearningMode)
        return UpdateQuestionForLearning(iQ, fSuccess, tsResponse);
    else
        return UpdateQuestionForPractising(iQ, fSuccess, tsResponse);
}

//---------------------------------------------------------------------------------------
void LeitnerManager::UpdateQuestionForLearning(int iQ, bool fSuccess, wxTimeSpan tsResponse)
{
    //update question data and promote/demote question
    Question* pQ = m_ProblemSpace.GetQuestion(iQ);
    int nOldGroup = pQ->GetGroup();
    pQ->UpdateAsked(&m_ProblemSpace);
    pQ->UpdateSuccess(&m_ProblemSpace, fSuccess);

    //schedule next repetition and update statistics
    if (fSuccess)
    {
        //Question answered right. If repetition threshold reached, schedule it for
        //repetition after some time
        if (pQ->GetRepetitions() >= m_ProblemSpace.RepetitionsThreshold())
        {
            wxTimeSpan tsDaysInvtal = GetRepetitionInterval(pQ->GetGroup());
            pQ->SetRepetitionInterval( wxDateTime::Today() + tsDaysInvtal - m_ProblemSpace.GetCreationDate() );
        }

        //statistics
        if (nOldGroup == 0)
            m_nUnlearned--;
        else
            m_nToReview--;
    }
    else
    {
        //Question answered wrong. Schedule it for inmmediate repetition.
        pQ->SetRepetitionInterval( wxDateTime::Today() - m_ProblemSpace.GetCreationDate() );

        //statistics
        if (nOldGroup > 0)
        {
            m_nUnlearned++;
            m_nToReview--;
        }
    }

    //update groups
    int nNewGroup = pQ->GetGroup();
    if (nOldGroup != nNewGroup)
    {
        m_NumQuestions[nOldGroup]--;
        m_NumQuestions[nNewGroup]++;
    }

    //update times
    m_ProblemSpace.AddTotalRespTime( tsResponse );    //total response time since start
    m_ProblemSpace.IncrementTotalAsked();             //total num questions asked since start
}

//---------------------------------------------------------------------------------------
void LeitnerManager::UpdateQuestionForPractising(int iQ, bool fSuccess, wxTimeSpan tsResponse)
{
    //in practise mode no performance data is updated/saved. Only update displayed statistics

    WXUNUSED(iQ);
    WXUNUSED(tsResponse);

    if (fSuccess)
        m_nRight++;
    else
        m_nWrong++;
}

//---------------------------------------------------------------------------------------
wxTimeSpan LeitnerManager::GetRepetitionInterval(int nGroup)
{
    //return repetion interval (days) for received group
    //  ------------------------------------------------------
    //          Repetition
    //  Group   interval       Learning level                  Days
    //  ------------------------------------------------------
    //      0    1 day          Poor: need more work               1    0 points
    //      1    4 days                                            4
    //      2    7 days                                            7
    //      3    12 days                                          12
    //      4    20 days                                          20
    //  ------------------------------------------------------
    //      5    1 month        Fair: need some repetitions       30    1 points
    //      6    2 months                                         60
    //      7    3 months                                         90
    //      8    5 months                                        150
    //  ------------------------------------------------------
    //      9    9 months       Good: known questions            270    2 points
    //      10  16 months                                        480
    //      11   2 years                                         720
    //      12   4 years                                        1440
    //      13   6 years                                        2160
    //      14  11 years                                        3960
    //      15  18 years                                        6120
    //  ------------------------------------------------------
    //
    // This table use a factor of 1.7 to increase subsequent intervals.


    static wxTimeSpan tsInterval[lmNUM_GROUPS] =
    {
        wxTimeSpan::Days(1),        //Group 0
        wxTimeSpan::Days(4),        //Group 1
        wxTimeSpan::Days(7),        //Group 2
        wxTimeSpan::Days(12),       //Group 3
        wxTimeSpan::Days(20),       //Group 4
        wxTimeSpan::Days(30),       //Group 5
        wxTimeSpan::Days(60),       //Group 6
        wxTimeSpan::Days(90),       //Group 7
        wxTimeSpan::Days(150),      //Group 8
        wxTimeSpan::Days(270),      //Group 9
        wxTimeSpan::Days(480),      //Group 10
        wxTimeSpan::Days(720),      //Group 11
        wxTimeSpan::Days(1440),     //Group 12
        wxTimeSpan::Days(2160),     //Group 13
        wxTimeSpan::Days(3960),     //Group 14
        wxTimeSpan::Days(6120),     //Group 15
    };

    if (nGroup >= lmNUM_GROUPS) nGroup = lmNUM_GROUPS-1;
    return tsInterval[nGroup];
}

//---------------------------------------------------------------------------------------
int LeitnerManager::GetNew()
{
    return m_nUnlearned;
}

//---------------------------------------------------------------------------------------
int LeitnerManager::GetExpired()
{
    return m_nToReview;
}

//---------------------------------------------------------------------------------------
int LeitnerManager::GetTotal()
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
float LeitnerManager::GetGlobalProgress()
{
    int nPoints = 0;
    int nTotal = 0;
    for (int iG=0; iG < lmNUM_GROUPS; iG++)
    {
        nPoints  += iG * m_NumQuestions[iG];
        nTotal += m_NumQuestions[iG];
    }
    if (nTotal == 0)
        return 0.0f;
    else
        return (float)(100 * nPoints) / (float)((lmNUM_GROUPS-1) * nTotal);
}

//---------------------------------------------------------------------------------------
float LeitnerManager::GetSessionProgress()
{
    if (m_nTotal == 0)
        return 0.0f;
    else
        return (float)(100 * (m_nTotal - m_nUnlearned - m_nToReview)) / (float)m_nTotal;
}

//---------------------------------------------------------------------------------------
const wxString LeitnerManager::GetProgressReport()
{
    //get average user response time
    wxString sAvrgRespTime = _T(" ");
    sAvrgRespTime += _("Unknown");
    int nAsked = m_ProblemSpace.GetTotalAsked();
    if (nAsked > 0)
    {
        double rMillisecs = m_ProblemSpace.GetTotalRespTime().GetMilliseconds().ToDouble() / (double)nAsked;
    }

    //Prepare message
    wxString m_sHeader = _T("<html><body>");
    wxString sContent = m_sHeader +
        _T("<center><h3>") + _("Session report") + _T("</h3></center><p>") +
        _("New questions:") + wxString::Format(_T(" %d"), m_nUnlearned) + _T("<br>") +
        _("Questions to review:") + wxString::Format(_T(" %d"), m_nToReview) + _T("<br>") +
        _("Average answer time:") + sAvrgRespTime + _T("<br>") +
       //_T("</p><center><h3>") + _("Progress report") + _T("</h3></center><p>") +
       // _("Program build date:") + _T(" ") __TDATE__ _T("<br>") +
       // _("Your computer information:") +
        _T("</p></body></html>");

    return sContent;
}

//---------------------------------------------------------------------------------------
wxTimeSpan LeitnerManager::GetEstimatedSessionTime()
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
        rMillisecs = 30000;          //assume 30 secs per question if no data available

    //return estimation
    return wxTimeSpan::Milliseconds( (wxLongLong)(double(m_nUnlearned + m_nToReview) * rMillisecs) );
}




//=======================================================================================
// QuizManager implementation
//=======================================================================================
QuizManager::QuizManager(ApplicationScope& appScope, ExerciseCtrol* pOwnerExercise)
    : ProblemManager(appScope, pOwnerExercise)
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
void QuizManager::UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse)
{
    WXUNUSED(iQ);
    WXUNUSED(tsResponse);

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
