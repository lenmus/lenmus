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

#ifndef __LENMUS_GENERATORS_H__        //to avoid nested includes
#define __LENMUS_GENERATORS_H__


//lenmus
#include "lenmus_generators.h"
#include "lenmus_constrains.h"

//lomse
#include "lomse_internal_model.h"
#include "lomse_pitch.h"
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/datetime.h>

//#include <vector>
//#include <list>



namespace lenmus
{

class RandomGenerator
{
public:
    RandomGenerator();
    ~RandomGenerator() {}

    //random numbers
    static int RandomNumber(int nMin, int nMax);
    static bool FlipCoin();

    // clefs
    static EClefExercise GenerateClef(ClefConstrains* pValidClefs);

    // key signature
    static EKeySignature GenerateKey(KeyConstrains* pValidKeys);
    static EKeySignature RandomKeySignature();

    //time signature
    static ETimeSignature GenerateTimeSign(TimeSignConstrains* pValidTimeSignatures);
    static ETimeSignature RandomTimeSignature();

    //notes
    static DiatonicPitch GenerateRandomDPitch(int nMinLine, int nRange,
                                              bool fRests, EClefExercise nClef);
    static wxString GenerateRandomRootNote(EClefExercise nClef, EKeySignature nKey,
                                           bool fAllowAccidentals);

};


//-------------------------------------------------------------------------------------------------
// Leitner learning method. The idea is to generate questions not at random but giving priority
// according to individual user needs. The method gradually adapt questions priorities to user
// needs based on success/failures to previous questions, to generate an optimal sequence
// of question repetitions, tailored to user profile.
//-------------------------------------------------------------------------------------------------

class ProblemSpace;
class ExerciseCtrol;

#define lmNUM_GROUPS    16        //number of groups to classify questions


class Question
{
public:
    Question(long nSpaceID, long nSetID, long nParam0 = 0L, long nParam1 = 0L,
               long nParam2 = 0L, long nParam3 = 0L, long nParam4 = 0L,
               int nGroup = 0, int nAskedTotal = 0, int nSuccessTotal = 0,
               int nRepetitions = 0,
               wxTimeSpan tsLastAsked = wxTimeSpan::Days(-36500),      //never asked (100 years)
               long nDaysRepIntv = (wxTimeSpan::Day()).GetDays()       //1 day
              );
    ~Question();

    inline void SetIndex(int nIndex) { m_nIndex = nIndex; }
    inline int GetIndex() { return m_nIndex; }
    inline int GetGroup() { return m_nGroup; }
    inline int GetRepetitions() { return m_nRepetitions; }
    long GetParam(int nNumParam);
    void SaveQuestion(int nSpaceID);
    static bool LoadQuestions(long nSetID, ProblemSpace* pPS);

    void UpdateAsked(ProblemSpace* pPS);
    void UpdateSuccess(ProblemSpace* pPS, bool fSuccess);

    inline wxTimeSpan GetSheduledTimeSpan() { return m_tsLastAsked + m_tsDaysRepIntv; }
    inline void SetRepetitionInterval(wxTimeSpan ts) { m_tsDaysRepIntv = ts; }


protected:
    int         m_nIndex;           //index (0..n) assigned to this question in the problem space
    long        m_nSpaceID;
    long        m_nSetID;
    long        m_nParam0;
    long        m_nParam1;
    long        m_nParam2;
    long        m_nParam3;
    long        m_nParam4;
    int         m_nGroup;           //0..n
    int         m_nRepetitions;     //num times answered right without promotion
    int         m_nAskedTotal;      //num times this question has been asked
    int         m_nSuccessTotal;    //num times this question has been answered right
    wxTimeSpan  m_tsLastAsked;      //last time this question was asked. Referred to problem space
                                    //  creation date (tmCreation). If never asked will be -1 day.
    wxTimeSpan  m_tsDaysRepIntv;    //repetition interval (days). Default 1 day
};


// ProblemSpace: The set of questions for an exercise
class ProblemSpace
{
public:
    ProblemSpace();
    ~ProblemSpace();

    //creation / save
    bool LoadSet(wxString& sSetName);
    void SaveAndClear();
    void NewSpace(wxString& sSpaceName, int nRepetitionsThreshold, int nNumMandatoryParams);
    void StartNewSet(wxString& sSetName);
    inline void EndOfNewSet() { m_sets.push_back(m_nSetID); }

    //accessors
    inline int GetSpaceSize() { return m_questions.size(); }
    inline int GetGroup(int iQ) { return m_questions[iQ]->GetGroup(); }
    inline int RepetitionsThreshold() { return m_nRepetitions; }
    inline wxDateTime GetCreationDate() { return m_tmCreation; }
    inline wxTimeSpan GetTotalRespTime() { return m_tsTotalRespTime; }
    inline int GetTotalAsked() { return m_nTotalAsked; }
    Question* GetQuestion(int iQ);
    long GetQuestionParam(int iQ, int nNumParam);

    //operations
    void ClearSpace();
    inline void AddTotalRespTime(wxTimeSpan tsResponse) { m_tsTotalRespTime += tsResponse; }
    inline void IncrementTotalAsked() { m_nTotalAsked++; }
    void AddNewQuestion(long nParam0 = 0L, long nParam1 = 0L, long nParam2 = 0L,
                        long nParam3 = 0L, long nParam4 = 0L);
    Question* AddQuestion(long nParam0 = 0L, long nParam1 = 0L, long nParam2 = 0L,
                            long nParam3 = 0L, long nParam4 = 0L,
                            int nGroup = 0, int nAskedTotal = 0, int nSuccessTotal = 0,
                            int nRepetitions = 0,
                            wxTimeSpan tsLastAsked = wxTimeSpan::Days(-36500),      //never asked (100 years)
                            long nDaysRepIntv = (wxTimeSpan::Day()).GetDays()       //1 day
                           );

    //info
    inline bool IsEmpty() { return m_questions.size() == 0; }
    inline long GetSpaceID() { return m_nSpaceID; }
    bool IsQuestionParamMandatory(int nNumParam);


private:
    long GetSetID(long nSpaceID, wxString& sSetName);
    bool IsSetLoaded(long nSetID);
    void LoadSpace(wxString& sSpaceName, int nRepetitionsThreshold, int nNumMandatoryParams);


    std::vector<Question*>    m_questions;
    std::list<long>             m_sets;         //setIDs of loaded sets

    //information to save
    wxString        m_sSpaceName;       //name for this problem space
    wxString        m_sUser;            //user name
    long            m_nSpaceID;         //ID for this space
    wxString        m_sSetName;         //name for current set (last loaded set)
    long            m_nSetID;           //ID for current set
    long            m_nSetQIndex;       //index for question to add to current set
    wxDateTime      m_tmCreation;       //creation date
    wxDateTime      m_tmLastUsed;       //last date when this problem space was used
    int             m_nRepetitions;     //num of repetitions to promote a question
    int             m_nMandatoryParams;  //number of mandatory params that questions have
    wxTimeSpan      m_tsTotalRespTime;  //total time for answering questions
    int             m_nTotalAsked;      //total num questions asked

};



// Problem manager. Abstract class from which all problem managers must derive
// Load/Saves/Updates the problem space. Keep statistics about right/wrong answers
class ProblemManager
{
public:
    ProblemManager(ExerciseCtrol* pOwnerExercise);
    virtual ~ProblemManager();

    virtual void OnProblemSpaceChanged()=0;
    void SaveProblemSpace();
    inline int GetSpaceSize() { return m_ProblemSpace.GetSpaceSize(); }

    //Method to choose a question. Returns question index
    virtual int ChooseQuestion()=0;

    //Method to account for the answer
    virtual void UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse)=0;

    //new methods to integrate sets
    inline void NewSpace(wxString& sSpaceName, int nRepetitionsThreshold, int nNumMandatoryParams)
                    { m_ProblemSpace.NewSpace(sSpaceName, nRepetitionsThreshold, nNumMandatoryParams); }

    virtual bool LoadSet(wxString& sSetName);
    inline void StartNewSet(wxString& sSetName) { m_ProblemSpace.StartNewSet(sSetName); }
    inline void EndOfNewSet() { m_ProblemSpace.EndOfNewSet(); }
    virtual void AddQuestionToSet(long nParam0=0L, long nParam1=0L, long nParam2=0L,
                                  long nParam3=0L, long nParam4=0L);
    virtual bool IsQuestionParamMandatory(int nNumParam);
    virtual long GetQuestionParam(int iQ, int nNumParam);

protected:
    ProblemSpace          m_ProblemSpace;
    ExerciseCtrol*        m_pOwnerExercise;
};

// Quiz manager. A problem manager that generates questions at random.
class QuizManager : public ProblemManager
{
public:
    QuizManager(ExerciseCtrol* pOwnerExercise);
    ~QuizManager();

    //implementation of virtual methods
    int ChooseQuestion();
    void UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse);
    void OnProblemSpaceChanged() { ResetCounters(); }

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

// Leitner manager. A problem manager that chooses questions based on the Leitner system, that
// is, it adapts questions priorities to user needs based on success/failures
class LeitnerManager : public ProblemManager
{
public:
    LeitnerManager(ExerciseCtrol* pOwnerExercise, bool fLearningMode);
    ~LeitnerManager();

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
    wxTimeSpan GetEstimatedSessionTime();
    const wxString GetProgressReport();

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


}   //namespace lenmus

#endif  // __LENMUS_GENERATORS_H__
