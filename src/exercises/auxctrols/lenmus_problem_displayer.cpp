//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2013 LenMus project
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
#include "lenmus_problem_displayer.h"

#include "lenmus_constrains.h"
#include "lenmus_exercise_ctrol.h"
#include "lenmus_dlg_debug.h"
#include "lenmus_document_canvas.h"

//lomse
#include <lomse_internal_model.h>
#include <lomse_midi_table.h>
#include <lomse_ldp_exporter.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/window.h>

namespace lenmus
{

//=======================================================================================
// ProblemDisplayer implementation
//=======================================================================================
ProblemDisplayer::ProblemDisplayer(DocumentWindow* pCanvas, ImoContent* pWrapper,
                                   Document* pDoc, LUnits minHeight, ImoStyle* pStyle)
    : m_pCanvas(pCanvas)
    , m_pWrapper(pWrapper)
    , m_pDoc(pDoc)
    , m_pDisplayedScore(NULL)
    , m_pScoreToPlay(NULL)
    , m_pProblemPara(NULL)
    , m_pSolutionPara(NULL)
{
    //receives a content wrapper to place problem/solution on it

    ImoStyle* displayStyle = pStyle;
    if (!pStyle)
    {
        displayStyle = m_pDoc->create_private_style("Default style");
        displayStyle->border_width(10.0f);
        displayStyle->padding_top(0.0f);
        displayStyle->padding_bottom(0.0f);
        displayStyle->padding_left(400.0f);  //4 millimeters
        displayStyle->margin_bottom( displayStyle->em_to_LUnits(1.0f) );  //1em
        displayStyle->min_height(minHeight);
    }
    pWrapper->set_style(displayStyle);
}

//---------------------------------------------------------------------------------------
ProblemDisplayer::~ProblemDisplayer()
{
//    do_stop_sounds();     //stop any possible score being played
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::set_score(ImoScore* pScore)
{
    remove_current_score();

    if (pScore)
    {
        m_pDisplayedScore = pScore;
        m_pScoreToPlay = pScore;
        m_pWrapper->append_child_imo(pScore);
        pScore->set_style( m_pDoc->get_default_style() );
        m_pDoc->set_dirty();
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::remove_current_score()
{
    if (m_pDisplayedScore)
    {
        m_pWrapper->remove_child_imo(m_pDisplayedScore);
        delete m_pDisplayedScore;
        m_pDoc->set_dirty();
        m_pDisplayedScore = NULL;
        m_pScoreToPlay = NULL;
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::set_problem_text(const string& msg)
{
    remove_problem_text();

    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    m_pProblemPara = m_pWrapper->add_paragraph(pDefStyle);
    m_pProblemPara->add_text_item(msg, pDefStyle);
    m_pDoc->set_dirty();
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::remove_problem_text()
{
    if (m_pProblemPara)
    {
        m_pWrapper->remove_item(m_pProblemPara);
        m_pDoc->set_dirty();
        delete m_pProblemPara;
        m_pProblemPara = NULL;
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::set_solution_text(const string& msg)
{
    remove_solution_text();

    ImoStyle* pDefStyle = m_pDoc->get_default_style();
    m_pSolutionPara = m_pWrapper->add_paragraph(pDefStyle);
    m_pSolutionPara->add_text_item(msg, pDefStyle);
    m_pDoc->set_dirty();
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::remove_solution_text()
{
    if (m_pSolutionPara)
    {
        m_pWrapper->remove_item(m_pSolutionPara);
        m_pDoc->set_dirty();
        delete m_pSolutionPara;
        m_pSolutionPara = NULL;
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::clear()
{
    remove_current_score();
    remove_problem_text();
    remove_solution_text();
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::debug_show_source_score()
{
    if (m_pDisplayedScore)
    {
        wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
        LdpExporter exporter;
        string source = exporter.get_source(m_pDisplayedScore);
        DlgDebug dlg(pParent, _T("Generated source code"), to_wx_string(source));
        dlg.ShowModal();
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::debug_show_midi_events()
{
    if (m_pDisplayedScore)
    {
        SoundEventsTable* pTable = m_pDisplayedScore->get_midi_table();
        wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
        DlgDebug dlg(pParent, _T("MIDI events table"), to_wx_string(pTable->dump_midi_events()) );
        dlg.ShowModal();
    }
}


}   //namespace lenmus
