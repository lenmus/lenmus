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

#ifndef __LM_COUNTERSCTROL_H__        //to avoid nested includes
#define __LM_COUNTERSCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "CountersAuxCtrol.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


class lmProblemManager;
class lmLeitnerManager;
class lmQuizManager;


//----------------------------------------------------------------------------
// lmCountersAuxCtrol: an abstract control to embed in html exercises to 
// statistics about user performance in doing the exercise
//----------------------------------------------------------------------------
class lmCountersAuxCtrol : public wxPanel     
{

public:

    // constructor and destructor    
    lmCountersAuxCtrol(wxWindow* parent, wxWindowID id, double rScale,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize);

    virtual ~lmCountersAuxCtrol();

    virtual void UpdateDisplay(bool fSuccess)=0;
    virtual void OnNewQuestion() {}

    //other
    void RightWrongSound(bool fSuccess);


protected:
    double              m_rScale;           //current scaling factor


};

//----------------------------------------------------------------------------
// lmQuizAuxCtrol: a control to embed in html exercises to display number
// on right and wrong student answers, in a session, and the total score
// (percentage)
//----------------------------------------------------------------------------
class lmQuizAuxCtrol : public lmCountersAuxCtrol    
{
public:

    // constructor and destructor    
    lmQuizAuxCtrol(wxWindow* parent, wxWindowID id, double rScale,
                   lmQuizManager* pProblemMngr, 
                   const wxPoint& pos = wxDefaultPosition);

    ~lmQuizAuxCtrol();

    //base class virtual methods implementation
    void UpdateDisplay(bool fSuccess);

    //overrides
    void OnNewQuestion();

    // event handlers
    void OnResetCounters(wxCommandEvent& WXUNUSED(event));

    // settings
    void ResetCounters();
    void NextTeam();

protected:
    void UpdateDisplays(int nTeam);
    void CreateCountersGroup(int nTeam, wxBoxSizer* pMainSizer, bool fTeam);

    lmQuizManager*   m_pProblemMngr;

    //displays
    wxStaticText*   m_pRightCounter[2];
    wxStaticText*   m_pWrongCounter[2];
    wxStaticText*   m_pTotalCounter[2];

    //labels
    wxStaticText*   m_pTeamTxt;
    
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// lmLeitnerAuxCtrol: a control to embed in html exercises to display statistics
// on user performance in learning the subject. It uses the Leitner system of
// spaced repetitions
//----------------------------------------------------------------------------
class lmLeitnerAuxCtrol : public lmCountersAuxCtrol    
{

public:

    // constructor and destructor    
    lmLeitnerAuxCtrol(wxWindow* parent, wxWindowID id, double rScale,
                      lmLeitnerManager* pProblemMngr, 
                      const wxPoint& pos = wxDefaultPosition);

    ~lmLeitnerAuxCtrol();

    //event handlers
    void OnExplainProgress(wxCommandEvent& WXUNUSED(event));

    //base class virtual methods implementation
    void UpdateDisplay(bool fSuccess);


protected:
    void CreateControls();

    lmLeitnerManager*       m_pProblemMngr;

	wxStaticText* m_pTxtPoor;
	wxStaticText* m_pTxtFair;
	wxStaticText* m_pTxtGood;
	wxStaticText* m_pTxtAchieved;
	wxStaticText* m_pLblProgress;
	wxStaticText* m_pTxtProgress;


    DECLARE_EVENT_TABLE()
};



#endif  // __LM_COUNTERSCTROL_H__

