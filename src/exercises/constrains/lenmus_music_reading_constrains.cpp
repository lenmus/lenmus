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
#include "lenmus_music_reading_constrains.h"

#include "lenmus_generators.h"
#include "lenmus_constrains.h"

//wxWidgets
#include <wx/wxprec.h>

namespace lenmus
{

//=======================================================================================
// implementation of MusicReadingConstrains
//=======================================================================================
MusicReadingConstrains::MusicReadingConstrains(wxString sSection,
                                               ApplicationScope& appScope)
    : ExerciseOptions(sSection, appScope)
    , fPlayCtrol(false)
    , sPlayLabel(_("Play"))
    , sStopPlayLabel(_("Stop"))
    , fSolfaCtrol(false)
    , sSolfaLabel(_("Read"))
    , sStopSolfaLabel(_("Stop"))
    , fBorder(false)
    , m_pScoreConstrains(LENMUS_NEW ScoreConstrains(appScope))
{
}

//---------------------------------------------------------------------------------------
MusicReadingConstrains::~MusicReadingConstrains()
{
    delete m_pScoreConstrains;
}

//---------------------------------------------------------------------------------------
void MusicReadingConstrains::set_labels(wxString& sLabel, wxString* pStart,
                                        wxString* pStop)
{
    int i = sLabel.Find("|");
    if (i != -1)
    {
        if (i > 1)
            *pStart = sLabel.substr(0, i-1);
        if (i < (int)sLabel.length()-1)
            *pStop = sLabel.substr(i+1);
    }
    else
         *pStart = sLabel;
}

//---------------------------------------------------------------------------------------
void MusicReadingConstrains::save_settings()
{
    if (m_sSection == "")
        return;

    m_pScoreConstrains->save_settings();
}

//---------------------------------------------------------------------------------------
void MusicReadingConstrains::load_settings()
{
    if (m_sSection == "")
        return;

    m_pScoreConstrains->load_settings();
}



}   // namespace lenmus
