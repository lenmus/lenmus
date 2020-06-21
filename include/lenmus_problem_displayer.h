//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

#ifndef __LENMUS_PROBLEM_DISPLAYER_H__
#define __LENMUS_PROBLEM_DISPLAYER_H__


//lenmus
#include "lenmus_standard_header.h"

////wxWidgets
//#include <wx/wxprec.h>
//#include <wx/wx.h>
//#include <wx/event.h>

//lomse
#include <lomse_internal_model.h>
#include <lomse_document.h>
using namespace lomse;


namespace lenmus
{

// forward declarations
class ExerciseOptions;
class ExerciseCtrol;
class DocumentWindow;


//---------------------------------------------------------------------------------------
// ProblemDisplayer: responsible for displaying a problem score and associated texts
class ProblemDisplayer
{
protected:
    DocumentWindow* m_pCanvas;
    ImoContent*     m_pWrapper;
    Document*       m_pDoc;
    ImoScore*       m_pProblemScore;
    ImoParagraph*   m_pProblemPara;
    ImoParagraph*   m_pSolutionPara;
    ImoScore*       m_pSolutionScore;

public:

    // constructor and destructor
    ProblemDisplayer(DocumentWindow* pCanvas, ImoContent* pWrapper, Document* pDoc,
                     LUnits minHeight, ImoStyle* pStyle=nullptr);
    ~ProblemDisplayer();

    void set_problem_score(ImoScore* pScore);
    void set_solution_score(ImoScore* pScore);
    void set_problem_text(const string& msg, ImoStyle* pStyle=nullptr);
    void set_solution_text(const string& msg, ImoStyle* pStyle=nullptr);
    void remove_problem_text();
    inline bool is_score_displayed() { return m_pProblemScore != nullptr; }
    void clear();
    inline bool is_displayed(ImoScore* pScore) { return pScore == m_pProblemScore
                                                        || pScore == m_pSolutionScore; }
    inline ImoScore* get_problem_score() { return m_pProblemScore; }

    //debug
    void debug_show_source_score();
    void debug_show_midi_events();

protected:
    void remove_problem_score();
    void remove_solution_score();
    void remove_solution_text();

};


}   // namespace lenmus

#endif  // __LENMUS_PROBLEM_DISPLAYER_H__
