//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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

#ifndef __LENMUS_GENERATORS_H__        //to avoid nested includes
#define __LENMUS_GENERATORS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_constrains.h"
#include "lenmus_injectors.h"

//lomse
#include "lomse_internal_model.h"
#include "lomse_pitch.h"
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/datetime.h>


namespace lenmus
{

class RandomGenerator
{
public:
    RandomGenerator() {}
    ~RandomGenerator() {}

    //random numbers
    static int random_number(int nMin, int nMax);
    static bool flip_coin();
    static void shuffle(int num, int* pIdx);

    // clefs
    static EClef generate_clef(ClefConstrains* pValidClefs);

    // key signature
    static EKeySignature generate_key(KeyConstrains* pValidKeys);
    static EKeySignature random_key_signature();

    //time signature
    static ETimeSignature GenerateTimeSign(TimeSignConstrains* pValidTimeSignatures);
    static ETimeSignature RandomTimeSignature();

    //notes
    static DiatonicPitch GenerateRandomDiatonicPitch(int nMinLine, int nRange,
                                              bool fRests, EClef nClef);
    static FPitch get_best_root_note(EClef nClef, EKeySignature nKey);

};


//=======================================================================================
// Leitner learning method.
//    The idea is to generate questions not at random but giving priority according to
//    individual user needs. The method gradually adapts questions priorities to user
//    needs based on success/failures to previous questions, to generate an optimal
//    sequence of question repetitions, tailored to user profile.
//=======================================================================================

class ProblemSpace;

#define k_num_boxes    16        //number of boxes to classify questions




//---------------------------------------------------------------------------------------
// Question: A question for an exercise
class Question
{
public:
    Question(ApplicationScope& appScope, long nSpaceID, long nDeckID,
             long nParam0 = 0L, long nParam1 = 0L,
             long nParam2 = 0L, long nParam3 = 0L, long nParam4 = 0L,
             int nBox = 0, int nAskedTotal = 0, int nSuccessTotal = 0,
             int nRepetitions = 0,
             wxTimeSpan tsLastAsked = wxTimeSpan::Days(-36500),      //never asked (100 years)
             long nDaysRepIntv = (wxTimeSpan::Day()).GetDays()       //1 day
    );
    ~Question();

    inline void SetIndex(int nIndex) { m_nIndex = nIndex; }
    inline int GetIndex() { return m_nIndex; }
    inline int get_box_index() { return m_nBox; }
    inline int GetRepetitions() { return m_nRepetitions; }
    long GetParam(int nNumParam);
    void SaveQuestion(int nSpaceID);
    static bool LoadQuestions(wxSQLite3Database* pDB, long nDeckID, ProblemSpace* pPS);

    void UpdateAsked(ProblemSpace* pPS);
    void UpdateSuccess(ProblemSpace* pPS, bool fSuccess);

    inline wxTimeSpan GetSheduledTimeSpan() { return m_tsLastAsked + m_tsDaysRepIntv; }
    inline void SetRepetitionInterval(wxTimeSpan ts) { m_tsDaysRepIntv = ts; }


protected:
    ApplicationScope& m_appScope;
    int         m_nIndex;           //index (0..n) assigned to this question in the problem space
    long        m_nSpaceID;
    long        m_nDeckID;
    long        m_nParam0;
    long        m_nParam1;
    long        m_nParam2;
    long        m_nParam3;
    long        m_nParam4;
    int         m_nBox;           //0..n
    int         m_nRepetitions;     //num times answered right without promotion
    int         m_nAskedTotal;      //num times this question has been asked
    int         m_nSuccessTotal;    //num times this question has been answered right
    wxTimeSpan  m_tsLastAsked;      //last time this question was asked. Referred to problem space
                                    //  creation date (tmCreation). If never asked will be -1 day.
    wxTimeSpan  m_tsDaysRepIntv;    //repetition interval (days). Default 1 day
};


//---------------------------------------------------------------------------------------
// ProblemSpace: The set of questions for an exercise
class ProblemSpace
{
public:
    ProblemSpace(ApplicationScope& appScope);
    ~ProblemSpace();

    //creation / save
    bool load_deck(wxString& sDeckName);
    void SaveAndClear();
    void NewSpace(wxString& sSpaceName, int nRepetitionsThreshold, int nNumMandatoryParams);
    void start_new_deck(wxString& sDeckName);
    inline void end_of_new_deck() { m_decks.push_back(m_nDeckID); }

    //accessors
    inline int GetSpaceSize() { return m_questions.size(); }
    inline int get_box_index(int iQ) { return m_questions[iQ]->get_box_index(); }
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
                            int nBox = 0, int nAskedTotal = 0, int nSuccessTotal = 0,
                            int nRepetitions = 0,
                            wxTimeSpan tsLastAsked = wxTimeSpan::Days(-36500),      //never asked (100 years)
                            long nDaysRepIntv = (wxTimeSpan::Day()).GetDays()       //1 day
                           );

    //info
    inline bool IsEmpty() { return m_questions.size() == 0; }
    inline long GetSpaceID() { return m_nSpaceID; }
    bool IsQuestionParamMandatory(int nNumParam);


private:
    long get_deck_id(long nSpaceID, wxString& sDeckName);
    bool is_deck_loaded(long nDeckID);
    void LoadSpace(wxString& sSpaceName, int nRepetitionsThreshold, int nNumMandatoryParams);


    ApplicationScope&       m_appScope;
    std::vector<Question*>  m_questions;
    std::list<long>         m_decks;         //DecksIDs of loaded decks

    //information to save
    wxString        m_sSpaceName;       //name for this problem space
    wxString        m_sUser;            //user name
    long            m_nSpaceID;         //ID for this space
    wxString        m_sDeckName;        //name for current set (last loaded set)
    long            m_nDeckID;          //ID for current set
    long            m_nQIndexForDeck;   //index for question to add to current deck
    wxDateTime      m_tmCreation;       //creation date
    wxDateTime      m_tmLastUsed;       //last date when this problem space was used
    int             m_nRepetitions;     //num of repetitions to promote a question
    int             m_nMandatoryParams;  //number of mandatory params that questions have
    wxTimeSpan      m_tsTotalRespTime;  //total time for answering questions
    int             m_nTotalAsked;      //total num questions asked

};


//---------------------------------------------------------------------------------------
// Problem manager. Abstract class from which all problem managers must derive
// Load/Saves/Updates the problem space. Keep statistics about right/wrong answers
class ProblemManager
{
public:
    ProblemManager(ApplicationScope& appScope);
    virtual ~ProblemManager();

    virtual void OnProblemSpaceChanged()=0;
    void save_problem_space();
    inline int GetSpaceSize() { return m_ProblemSpace.GetSpaceSize(); }

    //Method to choose a question. Returns question index
    virtual bool more_questions()=0;
    virtual int ChooseQuestion()=0;

    //Method to account for the answer
    virtual void UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse)=0;

    //new methods to integrate sets
    inline void NewSpace(wxString& sSpaceName, int nRepetitionsThreshold, int nNumMandatoryParams)
                    { m_ProblemSpace.NewSpace(sSpaceName, nRepetitionsThreshold, nNumMandatoryParams); }

    virtual bool load_deck(wxString& sDeckName);
    inline void start_new_deck(wxString& sDeckName) { m_ProblemSpace.start_new_deck(sDeckName); }
    inline void end_of_new_deck() { m_ProblemSpace.end_of_new_deck(); }
    virtual void add_question_to_deck(long nParam0=0L, long nParam1=0L, long nParam2=0L,
                                      long nParam3=0L, long nParam4=0L);
    virtual bool IsQuestionParamMandatory(int nNumParam);
    virtual long GetQuestionParam(int iQ, int nNumParam);

protected:
    ProblemSpace          m_ProblemSpace;
};

//---------------------------------------------------------------------------------------
// Quiz manager. A problem manager that generates questions at random.
class QuizManager : public ProblemManager
{
public:
    QuizManager(ApplicationScope& appScope);
    ~QuizManager();

    //implementation of virtual methods
    bool more_questions() { return true; }
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

//---------------------------------------------------------------------------------------
// Leitner manager. A problem manager that chooses questions based on the Leitner system, that
// is, it adapts questions priorities to user needs based on success/failures
class LeitnerManager : public ProblemManager
{
public:
    LeitnerManager(ApplicationScope& appScope, bool fLearningMode);
    ~LeitnerManager();

    //implementation of virtual methods
    bool more_questions();
    int ChooseQuestion();
    void UpdateQuestion(int iQ, bool fSuccess, wxTimeSpan tsResponse);
    void OnProblemSpaceChanged();

    //specific for this class
    wxTimeSpan get_repetition_interval(int nBox);
    inline bool IsLearningMode() { return m_fLearningMode; }
    void change_to_practise_mode();

    //statistics for learning mode
    int get_new();
    int get_expired();
    int get_total();
    float get_global_progress();
    float get_session_progress();
    float get_short_term_progress();
    float get_medium_term_progress();
    float get_long_term_progress();
    wxTimeSpan get_estimated_session_time();
    const wxString get_progress_report();
    void compute_achievement_indicators();

    //statistics for practise mode
    inline int GetRight() { return m_nRight; }
    inline int GetWrong() { return m_nWrong; }
    void ResetPractiseCounters();

private:
    void update_problem_space();
    void update_problem_space_for_learning();
    void update_problem_space_for_practising();
    int choose_question_for_learning();
    int choose_question_for_practising();
    void update_question_for_learning(int iQ, bool fSuccess, wxTimeSpan tsResponse);
    void update_question_for_practising(int iQ, bool fSuccess, wxTimeSpan tsResponse);
    void remove_current_question();

    bool m_fLearningMode;           //true: learning mode, false: practise mode
    bool m_fThereWhereQuestions;    //to control change to Practise mode

    //for learning mode
    std::vector<int> m_set0;            //questions scheduled for today
    int m_iQ;                           //index pointing to currently asked question or -1 if none
    std::vector<int>::iterator m_it0;   //iterator pointing to next question to ask
    static double m_w[k_num_boxes];     //weighting factors

    //for practise mode
    std::vector<int> m_box[k_num_boxes];    //questions, by box
    double m_range[k_num_boxes];            //probability range for each box. -1.0 means 'do not use'

    //statistics for learning mode
    int     m_nUnlearned;
    int     m_nToReview;
    int     m_nTotal;
    int     m_NumQuestions[k_num_boxes];    //num questions per box
    double  m_short, m_medium, m_long;      //achievement indicators

    //statistics for practise mode
    int     m_nRight;
    int     m_nWrong;

};


}   //namespace lenmus

#endif  // __LENMUS_GENERATORS_H__
