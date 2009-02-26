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
#else
    #include <wx/datetime.h>
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

class lmProblemSpace;
class lmExerciseCtrol;

#define lmNUM_GROUPS    16        //number of groups to classify questions


class lmQuestion
{
public:
    lmQuestion(int nGroup, int nAskedTotal, int nSuccessTotal, int nRepetitions,
               wxTimeSpan tsLastAsked = wxTimeSpan::Days(-1),          //never asked
               long nDaysRepIntv = (wxTimeSpan::Day()).GetDays()       //1 day
               );
    ~lmQuestion();

    inline void SetIndex(int nIndex) { m_nIndex = nIndex; }
    inline int GetIndex() { return m_nIndex; }
    inline int GetGroup() { return m_nGroup; }
    inline int GetRepetitions() { return m_nRepetitions; }
    void SaveQuestion(int nProblemSpaceKey);
    static void LoadQuestion(int nProblemSpaceKey, int iQ, lmProblemSpace* pPS);

    void UpdateAsked(lmProblemSpace* pPS);
    void UpdateSuccess(lmProblemSpace* pPS, bool fSuccess);

    inline wxTimeSpan GetSheduledTimeSpan() { return m_tsLastAsked + m_tsDaysRepIntv; }
    inline void SetRepetitionInterval(wxTimeSpan ts) { m_tsDaysRepIntv = ts; }




protected:
    int         m_nIndex;           //index (0..n) assigned to this question in the problem space
    int         m_nGroup;           //0..n
    int         m_nRepetitions;     //num times answered right without promotion
    int         m_nAskedTotal;      //num times this question has been asked
    int         m_nSuccessTotal;    //num times this question has been answered right
    wxTimeSpan  m_tsLastAsked;      //last time this question was asked. Referred to problem space 
                                    //  creation date (tmCreation). If never asked will be -1 day.
    wxTimeSpan  m_tsDaysRepIntv;    //repetition interval (days). Default 1 day

};


// lmProblemSpace: The set of questions for an exercise
class lmProblemSpace
{
public:
    lmProblemSpace();
    ~lmProblemSpace();

    //creation / save
    bool Load(wxString& sKey);
    void SaveAndClear();
    void NewSpace(int nNumQuestions, int nRepetitions, wxString& sKey);

    int AddQuestion(int nGroup, int nAskedTotal, int nSuccessTotal, int nRepetitions,
                    wxTimeSpan tsLastAsked = wxTimeSpan::Days(-36500),      //never asked (100 years)
                    long nDaysRepIntv = (wxTimeSpan::Day()).GetDays()       //1 day
                    );

    //accesors
    inline int GetSpaceSize() { return m_questions.size(); }
    inline int GetGroup(int iQ) { return m_questions[iQ]->GetGroup(); }
    inline int RepetitionsThreshold() { return m_nRepetitions; }
    lmQuestion* GetQuestion(int iQ);
    inline wxDateTime CreationDate() { return m_tmCreation; }
    inline wxTimeSpan GetTotalRespTime() { return m_tsTotalRespTime; }
    inline int GetTotalAsked() { return m_nTotalAsked; }

    //operations
    void ClearQuestions();
    inline void AddTotalRespTime(wxTimeSpan tsResponse) { m_tsTotalRespTime += tsResponse; }
    inline void IncrementTotalAsked() { m_nTotalAsked++; }

    //info
    inline bool IsEmpty() { return m_questions.size() == 0; }


private:

    std::vector<lmQuestion*>     m_questions;

    //information to save
    wxString        m_sKey;             //name for this problem space
    wxDateTime      m_tmCreation;       //creation date 
    wxDateTime      m_tmLastUsed;       //last date when this problem space was used
    int             m_nRepetitions;     //num of repetitions to promote a question
    wxTimeSpan      m_tsTotalRespTime;  //total time for answering questions
    int             m_nTotalAsked;      //total num questions asked

};


//
//// Questions manager. Abstract class from which all questions managers must derive
//// Chooses a question and takes note of right/wrong user answer
//class lmQuestionsManager
//{
//public:
//    lmQuestionsManager();
//    virtual ~lmQuestionsManager();
//
//    inline void UpdateProblemSpace(lmProblemSpace* pProblemSpace) { m_pProblemSpace = pProblemSpace; }
//
//    //Method to choose a question. Returns question index
//    virtual int ChooseQuestion()=0;
//
//    //Method to account for the answer
//    virtual void UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse)=0;
//
//protected:
//    lmProblemSpace*     m_pProblemSpace;
//
//};



// Problem manager. Abstract class from which all problem managers must derive
// Chooses a QuestionsMngr and a CountersAuxCtrol suitable for the exercise mode
// Load/Saves/Updates the problem space
class lmProblemManager
{
public:
    lmProblemManager(lmExerciseCtrol* pOwnerExercise);
    virtual ~lmProblemManager();

    virtual void OnProblemSpaceChanged()=0;
    bool LoadProblemSpace(wxString& sKey);
    void SaveProblemSpace();
    void SetNewSpace(int nNumQuestions, int nRepetitions, wxString sKey);
    inline int GetSpaceSize() { return m_pProblemSpace->GetSpaceSize(); }

    //Method to choose a question. Returns question index
    virtual int ChooseQuestion()=0;

    //Method to account for the answer
    virtual void UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse)=0;


protected:
    lmProblemSpace*         m_pProblemSpace;
    lmExerciseCtrol*        m_pOwnerExercise;
    //lmQuestionsManager*     m_pQuestionsMngr;
};

// Quiz manager. Generates questions at random.
class lmQuizManager : public lmProblemManager
{
public:
    lmQuizManager(lmExerciseCtrol* pOwnerExercise);
    ~lmQuizManager();

    //implementation of virtual methods
    int ChooseQuestion();
    void UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse);
    void OnProblemSpaceChanged() {}

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
    lmLeitnerManager(lmExerciseCtrol* pOwnerExercise, bool fLearningMode);
    ~lmLeitnerManager();

    //implementation of virtual methods
    int ChooseQuestion();
    void UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse);
    void OnProblemSpaceChanged();

    //specific for this class
    wxTimeSpan GetRepetitionInterval(int nGroup);
    inline bool IsLearningMode() { return m_fLearningMode; }

    //statistics for learning mode
    int GetNew();
    int GetExpired();
    int GetTotal();
    float GetGlobalProgress();
    float GetSessionProgress();
    wxTimeSpan GetEST();

    //statistics for practise mode
    inline int GetRight() { return m_nRight; }
    inline int GetWrong() { return m_nWrong; }
    void ResetPractiseCounters();


private:
    void UpdateProblemSpace();
    void UpdateProblemSpaceForLearning();
    void UpdateProblemSpaceForPractising();
    int ChooseQuestionForLearning();
    int ChooseQuestionForPractising();
    void UpdateQuestionForLearning(int iQ, bool fSuccess, wxTimeSpan tsResponse);
    void UpdateQuestionForPractising(int iQ, bool fSuccess, wxTimeSpan tsResponse);

    bool        m_fLearningMode;            //true: learning mode, false: practise mode
    bool        m_fThereWhereQuestions;     //to control change to Practise mode

    //for learning mode

    std::vector<int>                m_set0;                 //questions scheduled for today
    std::vector<int>::iterator      m_it0;                  //points to next question to ask

    //for practise mode

    std::vector<int>    m_group[lmNUM_GROUPS];  //questions, splitted by group
    double              m_range[lmNUM_GROUPS];  //probability range for each group. -1.0 means 'do not use'

    //statistics
    int     m_nUnlearned;
    int     m_nToReview;
    int     m_nTotal;
    int     m_NumQuestions[lmNUM_GROUPS];   //num questions per group

    //counters for right and wrong answers (practise mode)
    int         m_nRight;
    int         m_nWrong;

};



#endif  // __LM_GENERATORS_H__
