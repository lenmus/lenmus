//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2020 LenMus project
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

#ifndef __LENMUS_MUSIC_READING_CONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_MUSIC_READING_CONSTRAINS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_generators.h"
#include "lenmus_constrains.h"
#include "lenmus_scores_constrains.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>



namespace lenmus
{

//---------------------------------------------------------------------------------------
// Options for TheoMusicReadingCtrol control
class MusicReadingConstrains : public ExerciseOptions
{
public:
    MusicReadingConstrains(wxString sSection, ApplicationScope& appScope);
    ~MusicReadingConstrains();

    void SetControlPlay(bool fValue, wxString sLabels = "") {
        fPlayCtrol = fValue;
        if (sLabels != "")
            set_labels(sLabels, &sPlayLabel, &sStopPlayLabel);
    }
    void SetControlSolfa(bool fValue, wxString sLabels = "") {
        fSolfaCtrol = fValue;
        if (sLabels != "")
            set_labels(sLabels, &sSolfaLabel, &sStopSolfaLabel);
    }

    inline ScoreConstrains* GetScoreConstrains() { return m_pScoreConstrains; }

    void save_settings() override;


    bool        fPlayCtrol;             //Instert "Play" link
    wxString    sPlayLabel;             //label for "Play" link
    wxString    sStopPlayLabel;         //label for "Stop playing" link

    bool        fSolfaCtrol;            //insert a "Sol-fa" link
    wxString    sSolfaLabel;            //label for "Sol-fa" link
    wxString    sStopSolfaLabel;        //label for "Stop sol-fa" link

    bool        fBorder;

private:
    void set_labels(wxString& sLabel, wxString* pStart, wxString* pStop);
    void load_settings() override;

    ScoreConstrains*  m_pScoreConstrains;

};


}   // namespace lenmus

#endif  // __LENMUS_MUSIC_READING_CONSTRAINS_H__
