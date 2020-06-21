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
    , m_pProblemScore(nullptr)
    , m_pProblemPara(nullptr)
    , m_pSolutionPara(nullptr)
    , m_pSolutionScore(nullptr)
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
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::set_problem_score(ImoScore* pScore)
{
    remove_problem_score();

    if (pScore)
    {
        m_pProblemScore = pScore;
        m_pWrapper->append_child_imo(pScore);
        pScore->set_style( m_pDoc->get_default_style() );
        m_pDoc->set_dirty();
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::set_solution_score(ImoScore* pScore)
{
    remove_solution_score();

    if (pScore)
    {
        m_pSolutionScore = pScore;
        m_pWrapper->append_child_imo(pScore);
        pScore->set_style( m_pDoc->get_default_style() );
        m_pDoc->set_dirty();
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::remove_problem_score()
{
    if (m_pProblemScore)
    {
        m_pWrapper->remove_child_imo(m_pProblemScore);
        delete m_pProblemScore;
        m_pDoc->set_dirty();
        m_pProblemScore = nullptr;
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::remove_solution_score()
{
    if (m_pSolutionScore)
    {
        m_pWrapper->remove_child_imo(m_pSolutionScore);
        delete m_pSolutionScore;
        m_pDoc->set_dirty();
        m_pSolutionScore = nullptr;
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::set_problem_text(const string& msg, ImoStyle* pStyle)
{
    remove_problem_text();

    if (pStyle == nullptr)
        pStyle = m_pDoc->get_default_style();
    m_pProblemPara = m_pWrapper->add_paragraph(pStyle);
    m_pProblemPara->add_text_item(msg, pStyle);
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
        m_pProblemPara = nullptr;
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::set_solution_text(const string& msg, ImoStyle* pStyle)
{
    remove_solution_text();

    if (pStyle == nullptr)
        pStyle = m_pDoc->get_default_style();
    m_pSolutionPara = m_pWrapper->add_paragraph(pStyle);
    m_pSolutionPara->add_text_item(msg, pStyle);
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
        m_pSolutionPara = nullptr;
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::clear()
{
    remove_problem_score();
    remove_solution_score();
    remove_problem_text();
    remove_solution_text();
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::debug_show_source_score()
{
    if (m_pProblemScore)
    {
        wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
        LdpExporter exporter;
        string source = exporter.get_source(m_pProblemScore);
        DlgDebug dlg(pParent, "Generated source code", to_wx_string(source));
        dlg.ShowModal();
    }
}

//---------------------------------------------------------------------------------------
void ProblemDisplayer::debug_show_midi_events()
{
    if (m_pProblemScore)
    {
        SoundEventsTable* pTable = m_pProblemScore->get_midi_table();
        wxWindow* pParent = dynamic_cast<wxWindow*>(m_pCanvas);
        DlgDebug dlg(pParent, "MIDI events table", to_wx_string(pTable->dump_midi_events()) );
        dlg.ShowModal();
    }
}


}   //namespace lenmus
