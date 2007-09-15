//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifndef __LENMUS_MUSICREADINGCONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_MUSICREADINGCONSTRAINS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "MusicReadingConstrains.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Generators.h"
#include "Constrains.h"
#include "ScoreConstrains.h"

// Options for lmTheoMusicReadingCtrol control
class lmMusicReadingConstrains : public lmExerciseOptions
{
public:
    lmMusicReadingConstrains(wxString sSection);
    ~lmMusicReadingConstrains();

    void SetControlPlay(bool fValue, wxString sLabels = _T(""))
        {
            fPlayCtrol = fValue;
            if (sLabels != _T(""))
                SetLabels(sLabels, &sPlayLabel, &sStopPlayLabel);
        }
    void SetControlSolfa(bool fValue, wxString sLabels = _T(""))
        {
            fSolfaCtrol = fValue;
            if (sLabels != _T(""))
                SetLabels(sLabels, &sSolfaLabel, &sStopSolfaLabel);
        }
    void SetControlSettings(bool fValue, wxString sKey =_T(""))
        {
            fSettingsLink = fValue;
            sSettingsKey = sKey;
        }


    inline lmScoreConstrains* GetScoreConstrains() { return m_pScoreConstrains; }
    inline void SetScoreConstrains(lmScoreConstrains* pConstrains) {
                    m_pScoreConstrains = pConstrains;
    }


    bool        fPlayCtrol;             //Instert "Play" link
    wxString    sPlayLabel;             //label for "Play" link
    wxString    sStopPlayLabel;         //label for "Stop playing" link

    bool        fSolfaCtrol;            //insert a "Sol-fa" link
    wxString    sSolfaLabel;            //label for "Sol-fa" link
    wxString    sStopSolfaLabel;        //label for "Stop sol-fa" link

    bool        fBorder;

    bool        fSettingsLink;          // insert the settings link
    wxString    sSettingsKey;           // key for saving the user settings

private:
    void SetLabels(wxString& sLabel, wxString* pStart, wxString* pStop);

    lmScoreConstrains*  m_pScoreConstrains;

};


#endif  // __LENMUS_MUSICREADINGCONSTRAINS_H__
