//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

//#ifndef __LENMUS_MUSICREADINGCONSTRAINS_H__        //to avoid nested includes
//#define __LENMUS_MUSICREADINGCONSTRAINS_H__
//
//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma interface "MusicReadingConstrains.cpp"
//#endif
//
//// For compilers that support precompilation, includes <wx/wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#ifndef WX_PRECOMP
//#include <wx/wx.h>
//#endif
//
//#include "lenmus_generators.h"
//#include "lenmus_constrains.h"
//#include "ScoreConstrains.h"


namespace lenmus
{

//// Options for TheoMusicReadingCtrol control
//class MusicReadingConstrains : public ExerciseOptions
//{
//public:
//    MusicReadingConstrains(wxString sSection, ApplicationScope& appScope);
//    ~MusicReadingConstrains();
//
//    void SetControlPlay(bool fValue, wxString sLabels = _T(""))
//        {
//            fPlayCtrol = fValue;
//            if (sLabels != _T(""))
//                set_labels(sLabels, &sPlayLabel, &sStopPlayLabel);
//        }
//    void SetControlSolfa(bool fValue, wxString sLabels = _T(""))
//        {
//            fSolfaCtrol = fValue;
//            if (sLabels != _T(""))
//                set_labels(sLabels, &sSolfaLabel, &sStopSolfaLabel);
//        }
//    void SetControlSettings(bool fValue, wxString sKey =_T(""))
//        {
//            fSettingsLink = fValue;
//            sSettingsKey = sKey;
//        }
//
//
//    inline ImoScoreConstrains* GetScoreConstrains() { return m_pScoreConstrains; }
//    inline void SetScoreConstrains(ImoScoreConstrains* pConstrains) {
//                    m_pScoreConstrains = pConstrains;
//    }
//
//
//    bool        fPlayCtrol;             //Instert "Play" link
//    wxString    sPlayLabel;             //label for "Play" link
//    wxString    sStopPlayLabel;         //label for "Stop playing" link
//
//    bool        fSolfaCtrol;            //insert a "Sol-fa" link
//    wxString    sSolfaLabel;            //label for "Sol-fa" link
//    wxString    sStopSolfaLabel;        //label for "Stop sol-fa" link
//
//    bool        fBorder;
//
//    bool        fSettingsLink;          // insert the settings link
//    wxString    sSettingsKey;           // key for saving the user settings
//
//private:
//    void set_labels(wxString& sLabel, wxString* pStart, wxString* pStop);
//
//    ImoScoreConstrains*  m_pScoreConstrains;
//
//};


}   // namespace lenmus

//#endif  // __LENMUS_MUSICREADINGCONSTRAINS_H__
