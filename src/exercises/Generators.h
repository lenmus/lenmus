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

#ifndef __LM_GENERATORS_H__        //to avoid nested includes
#define __LM_GENERATORS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Generators.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Constrains.h"

class lmRandomGenerator
{
public:
    lmRandomGenerator();
    ~lmRandomGenerator() {}

    //random numbers
    static int RandomNumber(int nMin, int nMax);
    static bool FlipCoin();

    // clefs
    static lmEClefType GenerateClef(lmClefConstrain* pValidClefs);

    // key signature
    static lmEKeySignatures GenerateKey(lmKeyConstrains* pValidKeys);
    static lmEKeySignatures RandomKeySignature();

    //time signature
    static lmETimeSignature GenerateTimeSign(lmTimeSignConstrains* pValidTimeSignatures);
    static lmETimeSignature RandomTimeSignature();

    //notes
    static lmDPitch GenerateRandomDPitch(int nMinLine, int nRange, bool fRests, lmEClefType nClef);
    static wxString GenerateRandomRootNote(lmEClefType nClef, lmEKeySignatures nKey, bool fAllowAccidentals);

};


//-------------------------------------------------------------------------------------------------
// Leitner learning method. The idea is to generate questions not at random but giving priority
// according to individual user needs. The method gradually adapt questions priorities to user
// needs based on success/failures to previous questions, to generate an optimal sequence 
// of question repetitions, tailored to user profile.
//-------------------------------------------------------------------------------------------------

class lmProblemSpace
{
public:
    lmProblemSpace();
    ~lmProblemSpace();

    //creation / save
    bool Load(wxString& sKey);
    void Save(wxString& sKey);
    int AddQuestion(int nGroup, int nAskedGlobal, int nSuccessGlobal);

    //accesors
    inline int GetSpaceSize() { return m_questions.size(); }
    inline int GetGroup(int idx) { return m_questions[idx]->nGroup; }

    //operations
    void UpdateQuestion(int iNdx, bool fSuccess);
    void Clear();

    //info
    inline bool IsEmpty() { return m_questions.size() == 0; }


private:
    typedef struct {
        int     nIndex;     // 0..n
        int     nGroup;     // 0,1,2
        int     nAskedSession;
        int     nSuccessSession;
        int     nAskedGlobal;
        int     nSuccessGlobal;
    } lmQuestion;

    std::vector<lmQuestion*>     m_questions;

};


// Problem manager. Abstract class from which all problem managers must derive
class lmProblemManager
{
public:
    lmProblemManager(lmProblemSpace* pProblemSpace);
    lmProblemManager();
    virtual ~lmProblemManager();

    virtual void SetProblemSpace(lmProblemSpace* pProblemSpace);

    //Method to choose a question. Returns question index
    virtual int ChooseQuestion()=0;

    //Method to account for the answer
    virtual void UpdateQuestion(int iQ, bool fSuccess)=0;

    //Method to get information about student performance
    virtual void Statistics() {}

protected:
    lmProblemSpace*     m_pProblemSpace;
};

// Quiz manager. Generates questions at random.
class lmQuizManager : public lmProblemManager
{
public:
    lmQuizManager(lmProblemSpace* pProblemSpace);
    lmQuizManager();
    ~lmQuizManager();

    //implementation of virtual methods
    int ChooseQuestion();
    void UpdateQuestion(int iQ, bool fSuccess);

    //specific for this class
    void ResetCounters();
    inline void SetNumTeams(int nNum) { m_nMaxTeam = nNum; };
    inline int GetNumTeams() { return m_nMaxTeam; }
    inline int GetRight(int nTeam) { return m_nRight[nTeam]; }
    inline int GetWrong(int nTeam) { return m_nWrong[nTeam]; }
    inline int GetCurrentTeam() { return m_nCurrentTeam; }
    void NextTeam();


private:
    //counters for right and wrong answers
    int         m_nRight[2];
    int         m_nWrong[2];

    //teams
    int         m_nMaxTeam;             //num of teams (1..2)
    int         m_nCurrentTeam;         //team currently playing (0..1)
    bool        m_fStart;               //to ensure that first time we start with first team
};

// Leitner manager. Adapt questions priorities to user needs based on success/failures
class lmLeitnerManager : public lmProblemManager
{
public:
    lmLeitnerManager(lmProblemSpace* pProblemSpace);
    lmLeitnerManager();
    ~lmLeitnerManager();

    //implementation of virtual methods
    int ChooseQuestion();
    void UpdateQuestion(int iQ, bool fSuccess);

    //overrides
    void SetProblemSpace(lmProblemSpace* pProblemSpace);
    void Statistics();

    //specific for this class
    float GetPoor();
    float GetFair();
    float GetGood();
    float GetAchieved();
    float GetProgress();

private:
    void ComputeProbabilities();

    std::vector<int>    m_group[3];
    float               m_range[3];     //probability range for each group. -1.0 means 'do not use'

};


// Practise mode manager. As Leitner manager, it also adapts questions priorities
// to user needs based on success/failures. But it doesn't schedule repetitions and
// doesn't save performance data
class lmPractiseManager : public lmProblemManager
{
public:
    lmPractiseManager(lmProblemSpace* pProblemSpace);
    lmPractiseManager();
    ~lmPractiseManager();

    //implementation of virtual methods
    int ChooseQuestion();
    void UpdateQuestion(int iQ, bool fSuccess);

    //overrides
    void SetProblemSpace(lmProblemSpace* pProblemSpace);
    void Statistics();

    //specific for this class
    float GetPoor();
    float GetFair();
    float GetGood();
    float GetAchieved();
    float GetProgress();

private:
    void ComputeProbabilities();

    std::vector<int>    m_group[3];
    float               m_range[3];     //probability range for each group. -1.0 means 'do not use'

};

#endif  // __LM_GENERATORS_H__
