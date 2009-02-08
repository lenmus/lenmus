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

#include "Generators.h"
#include "../score/KeySignature.h"


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
// lmProblemSpace implementation
//-------------------------------------------------------------------------------------------------

lmProblemSpace::lmProblemSpace()
{
}

lmProblemSpace::~lmProblemSpace()
{
    Clear();
}

void lmProblemSpace::Clear()
{
    //delete all questions
    std::vector<lmQuestion*>::iterator it;
    for (it= m_questions.begin(); it != m_questions.end(); ++it)
        delete *it;
    m_questions.clear();
}

int lmProblemSpace::AddQuestion(int nGroup, int nAskedGlobal, int nSuccessGlobal)
{
    //return assigned index: 0..n

    wxASSERT(nGroup >=0 && nGroup < 3);
    wxASSERT(nAskedGlobal >= 0);
    wxASSERT(nSuccessGlobal >= 0 && nSuccessGlobal <= nAskedGlobal);

    lmQuestion* pQ = new lmQuestion;
    pQ->nGroup = nGroup;
    pQ->nAskedSession = 0;
    pQ->nSuccessSession = 0;
    pQ->nAskedGlobal = nAskedGlobal;
    pQ->nSuccessGlobal = nSuccessGlobal;
    m_questions.push_back(pQ);
    pQ->nIndex = (int)m_questions.size() - 1;

    return pQ->nIndex;
}

bool lmProblemSpace::Load(wxString& sKeyPrefix)
{
    //load problem space from configuration file. Returns false if error (data not found)

    Clear();
    wxString sKey = sKeyPrefix + _T("/NumQuestions");
    long nNumQuestions = g_pPrefs->Read(sKey, 0L);
    if (nNumQuestions <= 0) return false;           //no data found

    for (int iQ=0; iQ < nNumQuestions; ++iQ)
    {
        wxString sKeyQ = wxString::Format(_T("%s/Q%d/"), sKeyPrefix.c_str(), iQ);
        int nGroup = (int)g_pPrefs->Read(sKeyQ + _T("Grp"), 0L);
        int nAsked = (int)g_pPrefs->Read(sKeyQ + _T("Asked"), 0L);
        int nSuccess = (int)g_pPrefs->Read(sKeyQ + _T("Success"), 0L);
        AddQuestion(nGroup, nAsked, nSuccess);
    }

    return true;        //no error
}

void lmProblemSpace::Save(wxString& sKeyPrefix)
{
    //save problem space to configuration file

    long nNumQuestions = (long)m_questions.size();
    wxString sKey = sKeyPrefix + _T("/NumQuestions");
    g_pPrefs->Write(sKey, nNumQuestions);

    std::vector<lmQuestion*>::iterator it;
    int iQ;
    for (iQ=0, it= m_questions.begin(); it != m_questions.end(); ++it, ++iQ)
    {
        wxString sKeyQ = wxString::Format(_T("%s/Q%d/"), sKeyPrefix.c_str(), iQ);
        g_pPrefs->Write(sKeyQ + _T("Grp"), (long)((*it)->nGroup));
        g_pPrefs->Write(sKeyQ + _T("Asked"), (long)((*it)->nAskedGlobal));
        g_pPrefs->Write(sKeyQ + _T("Success"), (long)((*it)->nSuccessGlobal));
    }
}

void lmProblemSpace::UpdateQuestion(int iNdx, bool fSuccess)
{
    //Method to account for the answer. The algorithm is a direct
    //implementation of Leitner system description:
    //
    //  "If you look at a Group 0 card and get the correct answer, you "promote" it to Group 1. 
    //   A correct answer with a Group 1 card "promotes" that card to Group 2. If you make a mistake
    //   with a Group 1 or Group 2 card, it gets "demoted" to Group 0, which forces you to study 
    //   that card more often.".
    //

    wxASSERT(iNdx >= 0 && iNdx < (int)m_questions.size());
    lmQuestion* pQ = m_questions[iNdx];

    ++(pQ->nAskedSession);
    ++(pQ->nAskedGlobal);
    if (fSuccess)
    {
        ++(pQ->nSuccessSession);
        ++(pQ->nSuccessGlobal);
    }

    //change question group
    int nNewGroup = pQ->nGroup;
    if (fSuccess)
        nNewGroup++;        // Promote question
    else
        nNewGroup = 0;     // Move question to Group 0 ('Poor' group)

    if (nNewGroup > 2)
        nNewGroup = 2;     // last group

    pQ->nGroup = nNewGroup;
}





//-------------------------------------------------------------------------------------------------
// lmProblemManager implementation
//-------------------------------------------------------------------------------------------------

lmProblemManager::lmProblemManager()
{
    m_pProblemSpace = (lmProblemSpace*)NULL;
}

lmProblemManager::lmProblemManager(lmProblemSpace* pProblemSpace)
{
    SetProblemSpace(pProblemSpace);
}

lmProblemManager::~lmProblemManager()
{
}

void lmProblemManager::SetProblemSpace(lmProblemSpace* pProblemSpace)
{
    //save data
    m_pProblemSpace = pProblemSpace;
}



//-------------------------------------------------------------------------------------------------
// lmLeitnerManager implementation
//-------------------------------------------------------------------------------------------------

lmLeitnerManager::lmLeitnerManager()
    : lmProblemManager()
{
}

lmLeitnerManager::lmLeitnerManager(lmProblemSpace* pProblemSpace)
    : lmProblemManager(pProblemSpace)
{
}

lmLeitnerManager::~lmLeitnerManager()
{
}

void lmLeitnerManager::SetProblemSpace(lmProblemSpace* pProblemSpace)
{
    //save data
    m_pProblemSpace = pProblemSpace;

    //clear group vectors
    for (int iG=0; iG < 3; iG++)
        m_group[iG].clear();

    //Compute groups
    int nMaxQuestion = m_pProblemSpace->GetSpaceSize();
    for (int iQ=0; iQ < nMaxQuestion; iQ++)
    {
        int iG = m_pProblemSpace->GetGroup(iQ);
        wxASSERT(iG >=0 && iG < 3);
        m_group[iG].push_back(iQ);
    }

    //compute group probability range
    ComputeProbabilities();
}

void lmLeitnerManager::ComputeProbabilities()
{
    //compute group probability range
    int nGroupSize[3];
    int nEmptyGroups = 0;
    for (int iG=0; iG < 3; iG++)
    {
        nGroupSize[iG] = (int)m_group[iG].size();
        if (nGroupSize[iG] == 0)
        {
            m_range[iG] = -1.0f;
            nEmptyGroups++;
        }
        else
            m_range[iG] = 0.0f;
    }

    if (nEmptyGroups == 0)
    {
        // All three groups have questions
        // Probability group 0 = 300/460 = 0.65  ==> Range [0.00, 0.65]
        // Probability group 1 = 100/460 = 0.22  ==> Range [0.65, 0.87]
        // Probability group 2 =  60/460 = 0.13  ==> Range [0.87, 1.00]
        m_range[0] = 0.65f;
        m_range[1] = 0.87f;
        m_range[2] = 1.0f;
    }
    else if (nEmptyGroups == 1)
    {
        if (m_range[0] == -1.0f)
        {
            // Group 0 empty:
            // Probability group 1 = 100/160 = 0.63  ==> Range [0.00, 0.63]
            // Probability group 2 =  60/160 = 0.37  ==> Range [0.63, 1.00]
            m_range[1] = 0.63f;
            m_range[2] = 1.0f;
        }
        else if (m_range[1] == -1.0f)
        {
            // Group 1 empty:
            // Probability group 0 = 300/360 = 0.83  ==> Range [0.00, 0.83]
            // Probability group 2 =  60/360 = 0.17  ==> Range [0.83, 1.00]
            m_range[0] = 0.83f;
            m_range[2] = 1.0f;
        }
        else
        {
            // Group 2 empty:
            // Probability group 0 = 300/400 = 0.75  ==> Range [0.00, 0.75]
            // Probability group 1 = 100/400 = 0.25  ==> Range [0.75, 1.00]
            wxASSERT(m_range[2] == -1.0f);
            m_range[0] = 0.75f;
            m_range[1] = 1.0f;
        }
    }
    else if (nEmptyGroups == 2)
    {
        // Two groups empty:
        // Probability of remaining group = 1.0  ==> Range [0.0, 1.0]
        for (int iG=0; iG < 3; iG++)
        {
            if (m_range[iG] == 0.0f)
                m_range[iG] = 1.0f;
        }
    }
    else
    {
        //Problem space empty!!
        wxASSERT(false);
    }
}

int lmLeitnerManager::ChooseQuestion()
{
    //Method to choose a question. Returns question index
    //The algorithm to select a question is as follows:
    // 1. Select at random a question group, with group probabilities defined table m_range[iG]
    // 2. Select at random a question from selected group

    wxASSERT(m_pProblemSpace);

    //select group
    float rG = (float)lmRandomGenerator::RandomNumber(0, 10000) / 10000.0f;
    int iG;
    for (iG=0; iG < 3; iG++)
    {
        if (rG <= m_range[iG]) break;
    }
    wxASSERT(iG < 3);

    //select question from group
    int nGroupSize = (int)m_group[iG].size();
    wxASSERT(nGroupSize > 0);
    int iQ = lmRandomGenerator::RandomNumber(0, nGroupSize-1); 

    //return index to selected question
    return m_group[iG].at(iQ);
}

void lmLeitnerManager::UpdateQuestion(int iQ, bool fSuccess)
{
    //Method to account for the answer

    wxASSERT(m_pProblemSpace);
    int nOldGroup = m_pProblemSpace->GetGroup(iQ);
    m_pProblemSpace->UpdateQuestion(iQ, fSuccess);
    int nNewGroup = m_pProblemSpace->GetGroup(iQ);

    //update group vectors
    if (nOldGroup != nNewGroup)
    {
        std::vector<int>::iterator it;
        it = find(m_group[nOldGroup].begin(), m_group[nOldGroup].end(), iQ);
        m_group[nOldGroup].erase(it);
        m_group[nNewGroup].push_back(iQ);
        wxLogMessage(_T("[lmLeitnerManager::UpdateQuestion] question %d moved from group %d (size=%d) to %d (size=%d)"),
                     iQ, nOldGroup, m_group[nOldGroup].size(), nNewGroup, 
                     m_group[nNewGroup].size() );
        //update groups probability range
        ComputeProbabilities();
    }
}

void lmLeitnerManager::Statistics()
{
    //get information about student performance

    wxASSERT(m_pProblemSpace);

    wxLogMessage(_T("Statistics"));
    wxLogMessage(_T("-----------------------------------------"));
    wxLogMessage(_T("Group 0 (bad): %d, range=%.2f"), m_group[0].size(), m_range[0]);
    wxLogMessage(_T("Group 1 (med): %d, range=%.2f"), m_group[1].size(), m_range[1]);
    wxLogMessage(_T("Group 2 (good): %d, range=%.2f"), m_group[2].size(), m_range[2]);
}

float lmLeitnerManager::GetPoor()
{
    int nTotal = m_group[0].size() + m_group[1].size() + m_group[2].size();
    return 100.0f * ((float)m_group[0].size() / (float)nTotal);
}

float lmLeitnerManager::GetFair()
{
    int nTotal = m_group[0].size() + m_group[1].size() + m_group[2].size();
    return 100.0f * ((float)m_group[1].size() / (float)nTotal);
}

float lmLeitnerManager::GetGood()
{
    int nTotal = m_group[0].size() + m_group[1].size() + m_group[2].size();
    return 100.0f * ((float)m_group[2].size() / (float)nTotal);
}

float lmLeitnerManager::GetAchieved()
{
    return 15.2f;
}

float lmLeitnerManager::GetProgress()
{
    return 15.2f;
}



//-------------------------------------------------------------------------------------------------
// lmPractiseManager implementation
//-------------------------------------------------------------------------------------------------

lmPractiseManager::lmPractiseManager()
    : lmProblemManager()
{
}

lmPractiseManager::lmPractiseManager(lmProblemSpace* pProblemSpace)
    : lmProblemManager(pProblemSpace)
{
}

lmPractiseManager::~lmPractiseManager()
{
}

void lmPractiseManager::SetProblemSpace(lmProblemSpace* pProblemSpace)
{
    //save data
    m_pProblemSpace = pProblemSpace;

    //clear group vectors
    for (int iG=0; iG < 3; iG++)
        m_group[iG].clear();

    //Compute groups
    int nMaxQuestion = m_pProblemSpace->GetSpaceSize();
    for (int iQ=0; iQ < nMaxQuestion; iQ++)
    {
        int iG = m_pProblemSpace->GetGroup(iQ);
        wxASSERT(iG >=0 && iG < 3);
        m_group[iG].push_back(iQ);
    }

    //compute group probability range
    ComputeProbabilities();
}

void lmPractiseManager::ComputeProbabilities()
{
    //compute group probability range
    int nGroupSize[3];
    int nEmptyGroups = 0;
    for (int iG=0; iG < 3; iG++)
    {
        nGroupSize[iG] = (int)m_group[iG].size();
        if (nGroupSize[iG] == 0)
        {
            m_range[iG] = -1.0f;
            nEmptyGroups++;
        }
        else
            m_range[iG] = 0.0f;
    }

    if (nEmptyGroups == 0)
    {
        // All three groups have questions
        // Probability group 0 = 300/460 = 0.65  ==> Range [0.00, 0.65]
        // Probability group 1 = 100/460 = 0.22  ==> Range [0.65, 0.87]
        // Probability group 2 =  60/460 = 0.13  ==> Range [0.87, 1.00]
        m_range[0] = 0.65f;
        m_range[1] = 0.87f;
        m_range[2] = 1.0f;
    }
    else if (nEmptyGroups == 1)
    {
        if (m_range[0] == -1.0f)
        {
            // Group 0 empty:
            // Probability group 1 = 100/160 = 0.63  ==> Range [0.00, 0.63]
            // Probability group 2 =  60/160 = 0.37  ==> Range [0.63, 1.00]
            m_range[1] = 0.63f;
            m_range[2] = 1.0f;
        }
        else if (m_range[1] == -1.0f)
        {
            // Group 1 empty:
            // Probability group 0 = 300/360 = 0.83  ==> Range [0.00, 0.83]
            // Probability group 2 =  60/360 = 0.17  ==> Range [0.83, 1.00]
            m_range[0] = 0.83f;
            m_range[2] = 1.0f;
        }
        else
        {
            // Group 2 empty:
            // Probability group 0 = 300/400 = 0.75  ==> Range [0.00, 0.75]
            // Probability group 1 = 100/400 = 0.25  ==> Range [0.75, 1.00]
            wxASSERT(m_range[2] == -1.0f);
            m_range[0] = 0.75f;
            m_range[1] = 1.0f;
        }
    }
    else if (nEmptyGroups == 2)
    {
        // Two groups empty:
        // Probability of remaining group = 1.0  ==> Range [0.0, 1.0]
        for (int iG=0; iG < 3; iG++)
        {
            if (m_range[iG] == 0.0f)
                m_range[iG] = 1.0f;
        }
    }
    else
    {
        //Problem space empty!!
        wxASSERT(false);
    }
}

int lmPractiseManager::ChooseQuestion()
{
    //Method to choose a question. Returns question index
    //The algorithm to select a question is as follows:
    // 1. Select at random a question group, with group probabilities defined table m_range[iG]
    // 2. Select at random a question from selected group

    wxASSERT(m_pProblemSpace);

    //select group
    float rG = (float)lmRandomGenerator::RandomNumber(0, 10000) / 10000.0f;
    int iG;
    for (iG=0; iG < 3; iG++)
    {
        if (rG <= m_range[iG]) break;
    }
    wxASSERT(iG < 3);

    //select question from group
    int nGroupSize = (int)m_group[iG].size();
    wxASSERT(nGroupSize > 0);
    int iQ = lmRandomGenerator::RandomNumber(0, nGroupSize-1); 

    //return index to selected question
    return m_group[iG].at(iQ);
}

void lmPractiseManager::UpdateQuestion(int iQ, bool fSuccess)
{
    //Method to account for the answer

    wxASSERT(m_pProblemSpace);
    int nOldGroup = m_pProblemSpace->GetGroup(iQ);
    m_pProblemSpace->UpdateQuestion(iQ, fSuccess);
    int nNewGroup = m_pProblemSpace->GetGroup(iQ);

    //update group vectors
    if (nOldGroup != nNewGroup)
    {
        std::vector<int>::iterator it;
        it = find(m_group[nOldGroup].begin(), m_group[nOldGroup].end(), iQ);
        m_group[nOldGroup].erase(it);
        m_group[nNewGroup].push_back(iQ);
        wxLogMessage(_T("[lmPractiseManager::UpdateQuestion] question %d moved from group %d (size=%d) to %d (size=%d)"),
                     iQ, nOldGroup, m_group[nOldGroup].size(), nNewGroup, 
                     m_group[nNewGroup].size() );
        //update groups probability range
        ComputeProbabilities();
    }
}

void lmPractiseManager::Statistics()
{
    //get information about student performance

    wxASSERT(m_pProblemSpace);

    wxLogMessage(_T("Statistics"));
    wxLogMessage(_T("-----------------------------------------"));
    wxLogMessage(_T("Group 0 (bad): %d, range=%.2f"), m_group[0].size(), m_range[0]);
    wxLogMessage(_T("Group 1 (med): %d, range=%.2f"), m_group[1].size(), m_range[1]);
    wxLogMessage(_T("Group 2 (good): %d, range=%.2f"), m_group[2].size(), m_range[2]);
}

float lmPractiseManager::GetPoor()
{
    int nTotal = m_group[0].size() + m_group[1].size() + m_group[2].size();
    return 100.0f * ((float)m_group[0].size() / (float)nTotal);
}

float lmPractiseManager::GetFair()
{
    int nTotal = m_group[0].size() + m_group[1].size() + m_group[2].size();
    return 100.0f * ((float)m_group[1].size() / (float)nTotal);
}

float lmPractiseManager::GetGood()
{
    int nTotal = m_group[0].size() + m_group[1].size() + m_group[2].size();
    return 100.0f * ((float)m_group[2].size() / (float)nTotal);
}

float lmPractiseManager::GetAchieved()
{
    return 15.2f;
}

float lmPractiseManager::GetProgress()
{
    return 15.2f;
}



//-------------------------------------------------------------------------------------------------
// lmQuizManager implementation
//-------------------------------------------------------------------------------------------------

lmQuizManager::lmQuizManager()
    : lmProblemManager()
{
    //initializations
    m_nMaxTeam = 0;             
    m_nCurrentTeam = 0;
    ResetCounters();
}

lmQuizManager::lmQuizManager(lmProblemSpace* pProblemSpace)
    : lmProblemManager(pProblemSpace)
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

void lmQuizManager::UpdateQuestion(int iQ, bool fSuccess)
{
    WXUNUSED(iQ);
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

