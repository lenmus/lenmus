//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Generators.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <wx/longlong.h>

#include <wx/arrstr.h>      //AWARE: Required by wxsqlite3. In Linux GCC complains about wxArrayString not defined in wxsqlite3.h
#include "wx/wxsqlite3.h"

#include <algorithm>
#include <numeric>
#include <vector>
#include <iterator>


#include "Generators.h"
#include "ExerciseCtrol.h"
#include "../score/KeySignature.h"


extern wxSQLite3Database* g_pDB;    //the database
extern wxConfigBase *g_pPrefs;      // the config object
extern lmDPitch GetFirstLineDPitch(lmEClefType nClef);

//---------------------------------------------------------------------------------------------------
// lmRandomGenerator implementation
//      Random generators for various elementes: clefs, notes, keys, tiem signatures, etc.
//      Generation methods accept as parameter a constrain object of proper class
//---------------------------------------------------------------------------------------------------

lmRandomGenerator::lmRandomGenerator()
{

}

int lmRandomGenerator::RandomNumber(int nMin, int nMax)
{
    // Generates a random number in the closed interval [nMin, nMax].

    // note that rand() returns an int in the range 0 to RAND_MAX (= 0x7fff)
    int nRange = nMax - nMin + 1;
    int nRnd = rand() % nRange;            // 0..nRange-1 = 0..(nMax-nMin+1)-1 = 0..(nMax-nMin)
    return nRnd + nMin;                    // nMin ... (nMax-nMin)+nMin = nMin...nMax
}

bool lmRandomGenerator::FlipCoin()
{
    return ((rand() & 0x01) == 0x01);     //true in odd number, false if even
}

lmEClefType lmRandomGenerator::GenerateClef(lmClefConstrain* pValidClefs)
{
    // Generates a random clef, choosen to satisfy the received constraints

    int nWatchDog = 0;
    int nClef = RandomNumber(lmMIN_CLEF, lmMAX_CLEF);
    while (!pValidClefs->IsValid((lmEClefType)nClef)) {
        nClef = RandomNumber(lmMIN_CLEF, lmMAX_CLEF);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmRandomGenerator::GenerateClef."));
            return lmMIN_CLEF;
        }
    }
    return (lmEClefType)nClef;
}

lmEKeySignatures lmRandomGenerator::GenerateKey(lmKeyConstrains* pValidKeys)
{
    // Generates a random key signature, choosen to satisfy the received constraints

    int nWatchDog = 0;
    int nKey = RandomNumber(lmMIN_KEY, lmMAX_KEY);
    while (!pValidKeys->IsValid((lmEKeySignatures)nKey)) {
        nKey = RandomNumber(lmMIN_KEY, lmMAX_KEY);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmRandomGenerator::GenerateKey."));
            return lmMIN_KEY;
        }
    }
    return (lmEKeySignatures)nKey;
}

lmEKeySignatures lmRandomGenerator::RandomKeySignature()
{
    return (lmEKeySignatures)RandomNumber(lmMIN_KEY, lmMAX_KEY);
}

lmETimeSignature lmRandomGenerator::GenerateTimeSign(lmTimeSignConstrains* pValidTimeSignatures)
{
    // Generates a random time signature, choosen to satisfy the received constraints

    int nWatchDog = 0;
    int nKey = RandomNumber(lmMIN_TIME_SIGN, lmMAX_TIME_SIGN);
    while (!pValidTimeSignatures->IsValid((lmETimeSignature)nKey)) {
        nKey = RandomNumber(lmMIN_TIME_SIGN, lmMAX_TIME_SIGN);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmRandomGenerator::GenerateTime."));
            return lmMIN_TIME_SIGN;
        }
    }
    return (lmETimeSignature)nKey;
}

lmETimeSignature lmRandomGenerator::RandomTimeSignature()
{
    return (lmETimeSignature)RandomNumber(lmMIN_TIME_SIGN, lmMAX_TIME_SIGN);
}

lmDPitch lmRandomGenerator::GenerateRandomDPitch(int nMinLine, int nRange, bool fRests,
                                     lmEClefType nClef)
{
    // Generates a random pitch in the range nMinLine to nMinLine+nRange-1,
    // both included.
    // If fRest==true also pitch = 0 (rest) can be generated.

    int nPitch;

    if (fRests)
    {
        //also generate rests
        nPitch = RandomNumber(0, nRange);
        nPitch = (nPitch == nRange ? 0 : nPitch + nMinLine);
    }
    else
    {
        //do not generate rests
        nPitch = RandomNumber(0, nRange-1) + nMinLine;
    }

    //correct note pitch to suit key signature base line
    nPitch += GetFirstLineDPitch(nClef) - 2;

    return nPitch;
}

wxString lmRandomGenerator::GenerateRandomRootNote(lmEClefType nClef,
                                                   lmEKeySignatures nKey,
                                                   bool fAllowAccidentals)
{
    // Get the index (0..6, 0=Do, 1=Re, 3=Mi, ... , 6=Si) to the root note for
    // the Key signature. For example, if nKeySignature is La sharp minor it returns
    // index = 5 (La)
    int nRoot = GetRootNoteIndex(nKey);
    wxString sNotes = _T("cdefgab");

    // Get the accidentals implied by the key signature.
    // Each element of the array refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
    // and its value can be one of: 0=no accidental, -1 = a flat, 1 = a sharp
    int nAccidentals[7];
    ComputeAccidentals(nKey, nAccidentals);

    wxString sAcc[5] = { _T("--"), _T("-"), _T(""), _T("+"), _T("x") };
    wxString sRootNote = sAcc[nAccidentals[nRoot]+ 2].c_str() +
                         sNotes.substr(nRoot, 1) +
                         (nRoot > 4 ? _T("3") : _T("4"));

    return sRootNote;
}




//-------------------------------------------------------------------------------------------------
// helper functions
// wxTimeSpan::GetDays() returns an int. This limits precission to 89.78 years. It should return
// a long. Therefore I will define some helper functions to fix this and to simplify usage
//-------------------------------------------------------------------------------------------------
long GetDays(wxTimeSpan& ts)
{
    wxLongLong nDays = ts.GetSeconds() / (60 * 60 * 24);
    return nDays.ToLong();
}

long GetAdditionalSeconds(wxTimeSpan& ts, long nDays)
{
    //Returns seconds after removing days
    wxLongLong nSeconds = (ts - wxTimeSpan::Days(nDays)).GetSeconds();
    return nSeconds.ToLong();
}



//-------------------------------------------------------------------------------------------------
//helper functions to centralize DB operations
//-------------------------------------------------------------------------------------------------

void lmCreateTable_QuestionsData()
{
    //Create QuestionsData table

    wxLogMessage(_T("Creating QuestionsData table"));
    g_pDB->ExecuteUpdate(_T("CREATE TABLE QuestionsData (")
                            _T("SpaceKey INTEGER")
                            _T(", Qi INTEGER")
                            _T(", Grp INTEGER")
                            _T(", Asked INTEGER")
                            _T(", Success INTEGER")
                            _T(", Repetitions INTEGER")
                            _T(", LastAsked INTEGER")       //TimeSpan
                            _T(", DaysRepIntv INTEGER")     //TimeSpan
                            _T(");"));
}

void lmCreateTable_ProblemSpaceKeys()
{
    //Create ProblemSpaceKeys table

    wxLogMessage(_T("Creating ProblemSpaceKeys table"));
    g_pDB->ExecuteUpdate(_T("CREATE TABLE ProblemSpaceKeys (")
                            _T("SpaceKey INTEGER PRIMARY KEY AUTOINCREMENT")
                            _T(", SpaceName char(200)  );"));
}

void lmCreateTable_ProblemSpaceData()
{
    //Create ProblemSpaceData table

    wxLogMessage(_T("Creating ProblemSpaceData table"));
    g_pDB->ExecuteUpdate(_T("CREATE TABLE ProblemSpaceData (")
                            _T("SpaceKey INTEGER")
                            _T(", NumQuestions INTEGER")
                            _T(", Repetitions INTEGER")
                            _T(", LastUsed INTEGER")        //DateTime
                            _T(", Creation INTEGER")        //DateTime
                            _T(", TotalRespTime INTEGER")     //TimeSpan
                            _T(", TotalAsked INTEGER")
                            _T(");"));
}

//-------------------------------------------------------------------------------------------------
// lmQuestion implementation
//-------------------------------------------------------------------------------------------------

lmQuestion::lmQuestion(int nGroup, int nAskedTotal, int nSuccessTotal, int nRepetitions,
               wxTimeSpan tsLastAsked, long nDaysRepIntv)
{
    wxASSERT(nGroup >=0 && nGroup < lmNUM_GROUPS);
    wxASSERT(nAskedTotal >= 0);
    wxASSERT(nSuccessTotal >= 0 && nSuccessTotal <= nAskedTotal);

    m_nGroup = nGroup;
    m_nAskedTotal = nAskedTotal;
    m_nSuccessTotal = nSuccessTotal;
    m_nRepetitions = nRepetitions;
    m_tsLastAsked = tsLastAsked;
    m_tsDaysRepIntv = wxTimeSpan::Days(nDaysRepIntv);
    m_nIndex = -1;      //not yet assigned
}

lmQuestion::~lmQuestion()
{
}

void lmQuestion::SaveQuestion(int nProblemSpaceKey)
{
    //Save data to DB

    //Create ProblemSpaceData table if it does not exist
    if (!g_pDB->TableExists(_T("QuestionsData")))
        lmCreateTable_QuestionsData();

    //Get row from database table
    wxString sSQL = wxString::Format(
        _T("SELECT * FROM QuestionsData WHERE (SpaceKey = %d AND Qi = %d);"),
        nProblemSpaceKey, m_nIndex);
    wxSQLite3ResultSet q = g_pDB->ExecuteQuery(sSQL.c_str());
    if (!q.NextRow())
    {
        //Didn't exits. Insert this question data
        wxSQLite3Statement stmt = g_pDB->PrepareStatement(
            _T("INSERT INTO QuestionsData VALUES (?, ?, ?, ?, ?, ?, ?, ?);"));
        stmt.Bind(1, nProblemSpaceKey);
        stmt.Bind(2, m_nIndex);
        stmt.Bind(3, m_nGroup);
        stmt.Bind(4, m_nAskedTotal);
        stmt.Bind(5, m_nSuccessTotal);
        stmt.Bind(6, m_nRepetitions);
        stmt.Bind(7, m_tsLastAsked.GetValue());
        stmt.Bind(8, m_tsDaysRepIntv.GetValue());
        stmt.ExecuteUpdate();
    }
    else
    {
        //Update saved data
        wxSQLite3Statement stmt = g_pDB->PrepareStatement(
            _T("UPDATE QuestionsData SET Grp = ?, Asked = ?, Success = ?, ")
            _T("Repetitions = ?, LastAsked = ?, DaysRepIntv = ? ")
            _T("WHERE (SpaceKey = ? AND Qi = ?);"));
        stmt.Bind(1, m_nGroup);
        stmt.Bind(2, m_nAskedTotal);
        stmt.Bind(3, m_nSuccessTotal);
        stmt.Bind(4, m_nRepetitions);
        stmt.Bind(5, m_tsLastAsked.GetValue());
        stmt.Bind(6, m_tsDaysRepIntv.GetValue());
        stmt.Bind(7, nProblemSpaceKey);
        stmt.Bind(8, m_nIndex);
        stmt.ExecuteUpdate();
    }
}

void lmQuestion::LoadQuestion(int nProblemSpaceKey, int iQ, lmProblemSpace* pPS)
{
    try
    {
        //Get row from database table
        wxString sSQL = wxString::Format(
            _T("SELECT * FROM QuestionsData WHERE (SpaceKey = %d AND Qi = %d);"),
            nProblemSpaceKey, iQ);
        wxSQLite3ResultSet q = g_pDB->ExecuteQuery(sSQL.c_str());
        if (!q.NextRow())
        {
            wxLogMessage(_T("[lmQuestion::LoadQuestion] Question not in DB!"));
            wxASSERT(false);
        }

        //data found in table
        int nGroup = q.GetInt(_T("Grp"));
        int nAsked = q.GetInt(_T("Asked"));
        int nSuccess = q.GetInt(_T("Success"));
        int nRepetitions = q.GetInt(_T("Repetitions"));
        wxTimeSpan tsLastAsked = wxTimeSpan( q.GetInt64(_T("LastAsked")) );
        long nDaysRepIntv = wxTimeSpan( q.GetInt64(_T("DaysRepIntv")) ).GetDays();

        pPS->AddQuestion(nGroup, nAsked, nSuccess, nRepetitions, tsLastAsked, nDaysRepIntv);
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage(_T("[lmProblemSpace::Load] Error in DB. Error code: %d, Message: '%s'"),
                 e.GetErrorCode(), e.GetMessage().c_str() );
    }
}

void lmQuestion::UpdateAsked(lmProblemSpace* pPS)
{
    m_nAskedTotal++;
    m_tsLastAsked = wxDateTime::Now() - pPS->CreationDate();
}

void lmQuestion::UpdateSuccess(lmProblemSpace* pPS, bool fSuccess)
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




//-------------------------------------------------------------------------------------------------
// lmProblemSpace implementation
//-------------------------------------------------------------------------------------------------

lmProblemSpace::lmProblemSpace()
{
    m_tmCreation = wxDateTime::Now();
    m_tmLastUsed = wxDateTime::Now();
    m_sKey = _T("");
    m_nRepetitions = 1;
}

lmProblemSpace::~lmProblemSpace()
{
    ClearQuestions();
}

void lmProblemSpace::ClearQuestions()
{
    //delete all questions
    std::vector<lmQuestion*>::iterator it;
    for (it= m_questions.begin(); it != m_questions.end(); ++it)
        delete *it;
    m_questions.clear();
}

int lmProblemSpace::AddQuestion(int nGroup, int nAskedTotal, int nSuccessTotal,
                                int nRepetitions, wxTimeSpan tsLastAsked,
                                long nDaysRepIntv)
{
    //return assigned index: 0..n

    lmQuestion* pQ = new lmQuestion(nGroup, nAskedTotal, nSuccessTotal, nRepetitions,
                                    tsLastAsked, nDaysRepIntv);
    m_questions.push_back(pQ);
    pQ->SetIndex( (int)m_questions.size() - 1 );

    return pQ->GetIndex();
}

bool lmProblemSpace::Load(wxString& sKey)
{
    //load problem space from configuration file. Returns false if error (data not found)
    //AWARE: External representation of wxTimeSpan will be two 32 bits fields: Days+Seconds

    wxASSERT (sKey != _T(""));

    if (m_sKey == sKey && GetSpaceSize() > 0) return true;        //already loaded

    m_sKey = sKey;
    ClearQuestions();
    m_tmLastUsed = wxDateTime::Now();

    //load data from SQLite3 database
    try
    {
        wxString sSQL;

        //Check if ProblemSpaceKeys table exists
        if (!g_pDB->TableExists(_T("ProblemSpaceKeys")))
            return false;           //no data found

        //Get key for this problem space
        int nKey;
        sSQL = wxString::Format(
            _T("SELECT * FROM ProblemSpaceKeys WHERE (SpaceName = '%s');"), m_sKey.c_str());
        wxASSERT(m_sKey.Len() < 200);

        wxSQLite3ResultSet q = g_pDB->ExecuteQuery(sSQL.c_str());
        if (q.NextRow())
        {
            //key found in table
            nKey = q.GetInt(0);
            wxLogMessage(_T("Key '%s' found in table. Key: %d"), m_sKey.c_str(), nKey );
        }
        else
        {
            //the problem space name was never stored.
            return false;           //no data found
        }


        //Get ProblemSpace global data
        if (!g_pDB->TableExists(_T("ProblemSpaceData")))
            return false;           //no data found

        int nNumQuestions;
        sSQL = wxString::Format(
            _T("SELECT * FROM ProblemSpaceData WHERE (SpaceKey = %d);"), nKey);
        wxLogMessage(sSQL.c_str());
        q = g_pDB->ExecuteQuery(sSQL.c_str());
        if (q.NextRow())
        {
            wxLogMessage(_T("data found in table"));
            //data found in table
            m_sKey = sKey;
            nNumQuestions = q.GetInt(_T("NumQuestions"));
            m_nRepetitions = q.GetInt(_T("Repetitions"));
            m_tmLastUsed = q.GetDateTime(_T("LastUsed"));
            m_tmCreation = q.GetDateTime(_T("Creation"));
            m_tsTotalRespTime = wxTimeSpan( q.GetInt64(_T("TotalRespTime")) );
            m_nTotalAsked = q.GetInt(_T("TotalAsked"));
        }
        else
            return false;           //no data found

        //load questions
        for (int iQ=0; iQ < nNumQuestions; ++iQ)
        {
            lmQuestion::LoadQuestion(nKey, iQ, this);
        }

        return true;        //no error
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage(_T("[lmProblemSpace::Load] Error in DB. Error code: %d, Message: '%s'"),
                 e.GetErrorCode(), e.GetMessage().c_str() );
    }
    return false;        //error
}

void lmProblemSpace::NewSpace(int nNumQuestions, int nRepetitions, wxString& sKey)
{
    //Create a new empty problem space, that is, a problem space that has no data saved in DB.
    //Previous problem space has been already saved if necessary, so here we have nothing to do
    //about saving current data.

    ClearQuestions();

    m_sKey = sKey;
    m_tmCreation = wxDateTime::Now();
    m_tmLastUsed = wxDateTime::Now();
    m_nRepetitions = nRepetitions;
    m_tsTotalRespTime = wxTimeSpan::Seconds(0);
    m_nTotalAsked = 0;

    for (int i= 0; i < nNumQuestions; i++)
    {
        AddQuestion(0, 0, 0, 0);
    }
}

void lmProblemSpace::SaveAndClear()
{
    //save problem space to configuration file
    //AWARE: External representation of wxTimeSpan will be two 32 bits fields: Days+Seconds

    if (m_sKey == _T(""))
    {
        ClearQuestions();
        return;
    }

    //save data to database
    try
    {
        wxString sSQL;

        g_pDB->Begin();

        //Create ProblemSpaceKeys table if it does not exist
        if (!g_pDB->TableExists(_T("ProblemSpaceKeys")))
            lmCreateTable_ProblemSpaceKeys();

        //Get key for this problem space
        int nKey;
        sSQL = wxString::Format(
            _T("SELECT * FROM ProblemSpaceKeys WHERE (SpaceName = '%s');"), m_sKey.c_str());
        wxASSERT(m_sKey.Len() < 200);

        wxSQLite3ResultSet q = g_pDB->ExecuteQuery(sSQL.c_str());
        if (q.NextRow())
        {
            //key found in table
            nKey = q.GetInt(0);
            wxLogMessage(_T("Key '%s' found in table. Key: %d"), m_sKey.c_str(), nKey );
        }
        else
        {
            //the problem space name was never stored. Do it now and get its key
            sSQL = wxString::Format(
                _T("INSERT INTO ProblemSpaceKeys (SpaceName) VALUES ('%s');"), m_sKey.c_str());
            g_pDB->ExecuteUpdate(sSQL.c_str());
            nKey = g_pDB->GetLastRowId().ToLong();
            wxLogMessage(_T("Key '%s' NOT found in table. Key: %d"), m_sKey.c_str(), nKey );
        }


        //Create ProblemSpaceData table if it does not exist
        if (!g_pDB->TableExists(_T("ProblemSpaceData")))
            lmCreateTable_ProblemSpaceData();

        //save ProblemSpace global data
        sSQL = wxString::Format(
            _T("SELECT * FROM ProblemSpaceData WHERE (SpaceKey = %d);"), nKey);
        q = g_pDB->ExecuteQuery(sSQL.c_str());
        if (q.NextRow())
        {
            //data found in table. Update data.
            wxSQLite3Statement stmt = g_pDB->PrepareStatement(
                _T("UPDATE ProblemSpaceData SET NumQuestions = ?, Repetitions = ?, ")
                _T("LastUsed = ?, Creation = ?, TotalRespTime = ?, TotalAsked = ? ")
                _T("WHERE (SpaceKey = ?);"));
            stmt.Bind(1, (int)m_questions.size());
            stmt.Bind(2, (int)m_nRepetitions);
            stmt.BindDateTime(3, m_tmLastUsed);
            stmt.BindDateTime(4, m_tmCreation);
            stmt.Bind(5, m_tsTotalRespTime.GetValue());
            stmt.Bind(6, (int)m_nTotalAsked);
            stmt.Bind(7, nKey);
            stmt.ExecuteUpdate();
        }
        else
        {
            //new problem space. Insert data
            wxSQLite3Statement stmt =
                g_pDB->PrepareStatement(_T("INSERT INTO ProblemSpaceData VALUES (?, ?, ?, ?, ?, ?, ?)"));
            stmt.Bind(1, nKey);
            stmt.Bind(2, (int)m_questions.size());
            stmt.Bind(3, (int)m_nRepetitions);
            stmt.BindDateTime(4, m_tmLastUsed);
            stmt.BindDateTime(5, m_tmCreation);
            stmt.Bind(6, m_tsTotalRespTime.GetValue());
            stmt.Bind(7, (int)m_nTotalAsked);
            stmt.ExecuteUpdate();
        }

        //save and delete questions
        std::vector<lmQuestion*>::iterator it;
        for (it= m_questions.begin(); it != m_questions.end(); ++it)
        {
            (*it)->SaveQuestion(nKey);
            delete *it;
        }
        m_questions.clear();

        //clear other data
        m_sKey = _T("");

        g_pDB->Commit();
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage(_T("[lmProblemSpace::SaveAndClear] Error in DB. Error code: %d, Message: '%s'"),
                    e.GetErrorCode(), e.GetMessage().c_str() );
    }
}

lmQuestion* lmProblemSpace::GetQuestion(int iQ)
{
    wxASSERT(iQ >= 0 && iQ < GetSpaceSize());
    return m_questions[iQ];
}





//-------------------------------------------------------------------------------------------------
// lmProblemManager implementation
//-------------------------------------------------------------------------------------------------

lmProblemManager::lmProblemManager(lmExerciseCtrol* pOwnerExercise)
{
    m_pProblemSpace = (lmProblemSpace*)NULL;
    m_pProblemSpace = new lmProblemSpace();
    m_pOwnerExercise = pOwnerExercise;
}

lmProblemManager::~lmProblemManager()
{
    m_pProblemSpace->SaveAndClear();
    delete m_pProblemSpace;
}

bool lmProblemManager::LoadProblemSpace(wxString& sKey)
{
    bool fLoaded = m_pProblemSpace->Load(sKey);
    if (fLoaded)
        OnProblemSpaceChanged();
    return fLoaded;
}

void lmProblemManager::SetNewSpace(int nNumQuestions, int nRepetitions, wxString sKey)
{
    m_pProblemSpace->NewSpace(nNumQuestions, nRepetitions, sKey);
    OnProblemSpaceChanged();
}

void lmProblemManager::SaveProblemSpace()
{
    m_pProblemSpace->SaveAndClear();
}



//-------------------------------------------------------------------------------------------------
// lmLeitnerManager implementation
//-------------------------------------------------------------------------------------------------

lmLeitnerManager::lmLeitnerManager(lmExerciseCtrol* pOwnerExercise, bool fLearningMode)
    : lmProblemManager(pOwnerExercise), m_fLearningMode(fLearningMode)
{
    //reset counters for statistics
    m_nUnlearned = 0;
    m_nToReview = 0;
    m_nTotal = 0;
    m_nRight = 0;
    m_nWrong = 0;

    m_fThereWhereQuestions = false;
}

lmLeitnerManager::~lmLeitnerManager()
{
}

void lmLeitnerManager::OnProblemSpaceChanged()
{
    UpdateProblemSpace();
}

void lmLeitnerManager::UpdateProblemSpace()
{
    if (m_fLearningMode)
        UpdateProblemSpaceForLearning();
    else
        UpdateProblemSpaceForPractising();
}

void lmLeitnerManager::UpdateProblemSpaceForLearning()
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
    int nMaxQuestion = m_pProblemSpace->GetSpaceSize();
    for (int iQ=0; iQ < nMaxQuestion; iQ++)
    {
        lmQuestion* pQ = m_pProblemSpace->GetQuestion(iQ);
        int nGroup = pQ->GetGroup();
        wxDateTime tsScheduled = m_pProblemSpace->CreationDate() + pQ->GetSheduledTimeSpan();
        if (tsScheduled <= wxDateTime::Today() || nGroup == 0)
        {
            //scheduled for today. Add to set
            m_set0.push_back(iQ);
            m_fThereWhereQuestions = true;

            //statistics
            if (nGroup == 0)
                m_nUnlearned += m_pProblemSpace->RepetitionsThreshold() - pQ->GetRepetitions();
            else
                m_nToReview += m_pProblemSpace->RepetitionsThreshold() - pQ->GetRepetitions();
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
        m_pOwnerExercise->ChangeGenerationModeLabel(lm_ePractiseMode);
        m_pOwnerExercise->ChangeCountersCtrol();
        UpdateProblemSpaceForPractising();
        //m_it0 = m_set0.begin();
        return;
    }

    //Shuffle Set0 (random ordering).
    std::random_shuffle( m_set0.begin(), m_set0.end() );

    //Set iterator to questions
    m_it0 = m_set0.begin();
}

void lmLeitnerManager::UpdateProblemSpaceForPractising()
{
    //reset counters for statistics
    m_nRight = 0;
    m_nWrong = 0;

    //clear groups
    for (int iG=0; iG < lmNUM_GROUPS; iG++)
        m_group[iG].clear();

    //Compute the groups
    int nMaxQuestion = m_pProblemSpace->GetSpaceSize();
    for (int iQ=0; iQ < nMaxQuestion; iQ++)
    {
        int iG = m_pProblemSpace->GetGroup(iQ);
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
            wxLogMessage(_T("[lmLeitnerManager::UpdateProblemSpaceForPractising] m_range[%d] = %.4f"), i, m_range[i]);
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

int lmLeitnerManager::ChooseQuestion()
{
    //Method to choose a question. Returns question index or -1 if no more questions

    if (m_fLearningMode)
        return ChooseQuestionForLearning();
    else
        return ChooseQuestionForPractising();
}

int lmLeitnerManager::ChooseQuestionForLearning()
{
    wxASSERT(m_pProblemSpace);

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
    //}
    //else
    //{
    //    //No more questions scheduled for today. Move to 'Practise mode'
    //    m_fLearningMode = false;     //change to practise mode
    //    m_pOwnerExercise->ChangeGenerationModeLabel(lm_ePractiseMode);
    //    m_pOwnerExercise->ChangeCountersCtrol();
    //    UpdateProblemSpace();
    //    return ChooseQuestionForPractising();
    //}
}

int lmLeitnerManager::ChooseQuestionForPractising()
{
    //Method to choose a question. Returns question index
    //The algorithm to select a question is as follows:
    // 1. Select at random a question group, with group probabilities defined table m_range[iG]
    // 2. Select at random a question from selected group

    wxASSERT(m_pProblemSpace);

    //select group
    float rG = (float)lmRandomGenerator::RandomNumber(0, 10000) / 10000.0f;
    int iG;
    for (iG=0; iG < lmNUM_GROUPS; iG++)
    {
        if (rG <= m_range[iG]) break;
    }
    wxASSERT(iG < lmNUM_GROUPS);

    //select question from group
    int nGroupSize = (int)m_group[iG].size();
    wxASSERT(nGroupSize > 0);
    int iQ = lmRandomGenerator::RandomNumber(0, nGroupSize-1);

    //return index to selected question
    return m_group[iG].at(iQ);
}

void lmLeitnerManager::UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse)
{
    //Method to account for the answer

    wxASSERT(m_pProblemSpace);
    wxASSERT(iQ >= 0 && iQ < m_pProblemSpace->GetSpaceSize());

    if (m_fLearningMode)
        return UpdateQuestionForLearning(iQ, fSuccess, tsResponse);
    else
        return UpdateQuestionForPractising(iQ, fSuccess, tsResponse);
}

void lmLeitnerManager::UpdateQuestionForLearning(int iQ, bool fSuccess, wxTimeSpan tsResponse)
{
    //update question data and promote/demote question
    lmQuestion* pQ = m_pProblemSpace->GetQuestion(iQ);
    int nOldGroup = pQ->GetGroup();
    pQ->UpdateAsked(m_pProblemSpace);
    pQ->UpdateSuccess(m_pProblemSpace, fSuccess);

    //schedule next repetition and update statistics
    if (fSuccess)
    {
        //Question answered right. If repetition threshold reached, schedule it for
        //repetition after some time
        if (pQ->GetRepetitions() >= m_pProblemSpace->RepetitionsThreshold())
        {
            wxTimeSpan tsDaysInvtal = GetRepetitionInterval(pQ->GetGroup());
            pQ->SetRepetitionInterval( wxDateTime::Today() + tsDaysInvtal - m_pProblemSpace->CreationDate() );
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
        pQ->SetRepetitionInterval( wxDateTime::Today() - m_pProblemSpace->CreationDate() );

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
    m_pProblemSpace->AddTotalRespTime( tsResponse );    //total response time since start
    m_pProblemSpace->IncrementTotalAsked();             //total num questions asked since start
}

void lmLeitnerManager::UpdateQuestionForPractising(int iQ, bool fSuccess, wxTimeSpan tsResponse)
{
    //in practise mode no performance data is updated/saved. Only update displayed statistics

    WXUNUSED(iQ);
    WXUNUSED(tsResponse);

    if (fSuccess)
        m_nRight++;
    else
        m_nWrong++;
}

wxTimeSpan lmLeitnerManager::GetRepetitionInterval(int nGroup)
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

int lmLeitnerManager::GetNew()
{
    return m_nUnlearned;
}

int lmLeitnerManager::GetExpired()
{
    return m_nToReview;
}

int lmLeitnerManager::GetTotal()
{
    return m_nTotal;
}

void lmLeitnerManager::ResetPractiseCounters()
{
    m_nRight = 0;
    m_nWrong = 0;
}

float lmLeitnerManager::GetGlobalProgress()
{
    int nPoints = 0;
    int nTotal = 0;
    for (int iG=0; iG < lmNUM_GROUPS; iG++)
    {
        nPoints  += iG * m_NumQuestions[iG];
        nTotal += m_NumQuestions[iG];
    }
    return (float)(100 * nPoints) / (float)((lmNUM_GROUPS-1) * nTotal);
}

float lmLeitnerManager::GetSessionProgress()
{
    return (float)(100 * (m_nTotal - m_nUnlearned - m_nToReview)) / (float)m_nTotal;
}

wxTimeSpan lmLeitnerManager::GetEST()
{
    //Return the estimated time span for answering all unknown + expired questions
    //After some testing using current session data for the estimation produces estimations that
    //vary greatly from one question to the next one. Therefore, for the estimation I will use
    //only historical data. It is more conservative (greater times) but times are more consistent.

    //get average user response time
    double rMillisecs;
    int nAsked = m_pProblemSpace->GetTotalAsked();
    if (nAsked > 0)
        rMillisecs = m_pProblemSpace->GetTotalRespTime().GetMilliseconds().ToDouble() / (double)nAsked;
    else
        rMillisecs = 30000;          //assume 30 secs per question if no data available

    //return estimation
    return wxTimeSpan::Milliseconds( (wxLongLong)(double(m_nUnlearned + m_nToReview) * rMillisecs) );
}




//-------------------------------------------------------------------------------------------------
// lmQuizManager implementation
//-------------------------------------------------------------------------------------------------

lmQuizManager::lmQuizManager(lmExerciseCtrol* pOwnerExercise)
    : lmProblemManager(pOwnerExercise)
{
    //initializations
    m_nMaxTeam = 0;
    m_nCurrentTeam = 0;
    ResetCounters();
}

lmQuizManager::~lmQuizManager()
{
}

int lmQuizManager::ChooseQuestion()
{
    //Method to choose a question. Returns question index
    //The algorithm to select a question is just to choose a question at random.

    wxASSERT(m_pProblemSpace);

    //select question at random.
    int nSize = m_pProblemSpace->GetSpaceSize();
    wxASSERT(nSize > 0);
    return lmRandomGenerator::RandomNumber(0, nSize-1);
}

void lmQuizManager::UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse)
{
    WXUNUSED(iQ);
    WXUNUSED(tsResponse);

    if (fSuccess)
        m_nRight[m_nCurrentTeam]++;
    else
        m_nWrong[m_nCurrentTeam]++;
}

void lmQuizManager::ResetCounters()
{
    for (int i=0; i < 2; i++)
    {
        m_nRight[i] = 0;
        m_nWrong[i] = 0;
    }
    m_fStart = true;
}

void lmQuizManager::NextTeam()
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

