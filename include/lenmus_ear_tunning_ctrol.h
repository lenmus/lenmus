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

#ifndef __LM_EARTUNNINGCTROL_H__        //to avoid nested includes
#define __LM_EARTUNNINGCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "EarTunningCtrol.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//#include "EarTunningConstrains.h"
#include "ExerciseCtrol.h"
#include "../score/Score.h"

//--------------------------------------------------------------------------------
// class for the exercise of un-tunned pitches
//--------------------------------------------------------------------------------

class lmEarTunningCtrol : public lmCompareMidiCtrol    
{
   DECLARE_DYNAMIC_CLASS(lmEarTunningCtrol)

public:

    // constructor and destructor    
    lmEarTunningCtrol(wxWindow* parent, wxWindowID id,
               lmExerciseOptions* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmEarTunningCtrol();

protected:
    //implementation of virtual methods
    wxString SetNewProblem();
    wxDialog* GetSettingsDlg();

private:

        // member variables


    DECLARE_EVENT_TABLE()
};



#endif  // __LM_EARTUNNINGCTROL_H__
