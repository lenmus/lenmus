//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "EarTunningCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "EarTunningCtrol.h"
#include "Constrains.h"
#include "Generators.h"



//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"


//------------------------------------------------------------------------------------
// Implementation of lmEarTunningCtrol: a CompareMidiCtrol
//------------------------------------------------------------------------------------



//Layout definitions
const int BUTTONS_DISTANCE    = 5;        //pixels

//IDs for controls
enum {
    ID_BUTTON = 3010,
};


IMPLEMENT_CLASS(lmEarTunningCtrol, lmCompareMidiCtrol)

BEGIN_EVENT_TABLE(lmEarTunningCtrol, lmCompareMidiCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+2, wxEVT_COMMAND_BUTTON_CLICKED, lmEarTunningCtrol::OnRespButton)
END_EVENT_TABLE()

lmEarTunningCtrol::lmEarTunningCtrol(wxWindow* parent, wxWindowID id, 
                           lmExerciseOptions* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmCompareMidiCtrol(parent, id, pConstrains, wxSize(400, 150), pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;

    //ask parent class to create the controls
    CreateControls();
}

lmEarTunningCtrol::~lmEarTunningCtrol()
{
}


wxString lmEarTunningCtrol::SetNewProblem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore - The score with the problem to propose
    //  m_pSolutionScore - The score with the solution or NULL if it is the
    //              same score than the problem score.
    //  m_sAnswer - the message to present when displaying the solution
    //  m_nRespIndex - the number of the button for the right answer
    //  m_nPlayMM - the speed to play the score
    //
    //It must return the message to display to introduce the problem.

    m_mpPitch[0] = 60;
    m_mpPitch[1] = 61;

    //compute the right answer
    m_sAnswer = _T("Second pitch is higher:\n   First: MIDI 50\n   Second: MIDI 60");
    m_nRespIndex = 2;

    //return message to display to introduce the problem
    return _T("");
    
}

wxDialog* lmEarTunningCtrol::GetSettingsDlg()
{
    //return new lmDlgCfgEarIntervals(this, m_pConstrains, true);    // true -> enable First note equal checkbox
    return (wxDialog*)NULL;
}

