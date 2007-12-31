//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LM_WAVEMANAGER_H__        //to avoid nested includes
#define __LM_WAVEMANAGER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "WaveManager.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/sound.h"


//This class is a singleton
class lmWaveManager
{
public:
    ~lmWaveManager();
    static lmWaveManager* GetInstance();
    static void Destroy();

    //produce predefined sounds
    void WrongAnswerSound();
    void RightAnswerSound();

    //set up configuration

protected:
    lmWaveManager();

private:
    //default values and user preferences
    void LoadUserPreferences();
    void SaveUserPreferences();

        //
        // member variables
        //

    static lmWaveManager*    m_pInstance;    //the only instance of this class

    wxSound     m_oWrong;       //sound for wrong answers
    wxSound     m_oRight;       //sound for right answers

};


#endif    // __LM_WAVEMANAGER_H__

