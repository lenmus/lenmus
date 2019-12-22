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

#include "lenmus_wave_player.h"

#include "lenmus_paths.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/filename.h>


namespace lenmus
{

//=======================================================================================
// WavePlayer implementation
//=======================================================================================
WavePlayer::WavePlayer(ApplicationScope& appScope)
    : m_appScope(appScope)
{
    //load sounds from files
    Paths* pPaths = m_appScope.get_paths();
    wxString sPath = pPaths->GetSoundsPath();

    wxFileName oFileWrong = wxFileName(sPath, "wrong", "wav", wxPATH_NATIVE);
    m_oWrong.Create(oFileWrong.GetFullPath());

    wxFileName oFileRight = wxFileName(sPath, "right", "wav", wxPATH_NATIVE);
    m_oRight.Create(oFileRight.GetFullPath());

}

//---------------------------------------------------------------------------------------
void WavePlayer::play_wrong_answer_sound()
{
    if (m_oWrong.IsOk())
        m_oWrong.Play(wxSOUND_SYNC);
}

//---------------------------------------------------------------------------------------
void WavePlayer::play_right_answer_sound()
{
    if (m_oRight.IsOk())
        m_oRight.Play(wxSOUND_SYNC);
}


}  //namespace lenmus
