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

//#ifndef __LENMUS_SCALESMANAGER_H__        //to avoid nested includes
//#define __LENMUS_SCALESMANAGER_H__
//
//// For compilers that support precompilation, includes <wx/wx.h>.
//#include <wx/wxprec.h>
//#include <wx/wx.h>
//
//#include "../score/Score.h"
//#include "lenmus_interval.h"
//#include "lenmus_scales_constrains.h"
//#include "lenmus_conversion.h"
//
//
//namespace lenmus
//{
//
////forward declarations
////declare global functions defined in this module
//extern wxString ScaleTypeToName(lmEScaleType nType);
//extern int NumNotesInScale(lmEScaleType nType);
//extern bool IsScaleMajor(lmEScaleType nType);
//extern bool IsScaleMinor(lmEScaleType nType);
//extern bool IsScaleGregorian(lmEScaleType nType);
//#define IsTonalScale(nScaleType)  ((nScaleType < est_StartNonTonal))
//
////a scale is a sequence of up 13 notes (12 chromatic notes plus repetition of first one).
////Change this for more notes in a scale
//#define lmNOTES_IN_SCALE  13
//
//class lmScalesManager
//{
//public:
//    //build a scale from root note and type
//    lmScalesManager(wxString sRootNote, lmEScaleType nScaleType,
//                   EKeySignature nKey = k_key_C);
//    //destructor
//    ~lmScalesManager();
//
//    lmEScaleType GetScaleType() { return m_nType; }
//    wxString GetName() { return ScaleTypeToName( m_nType ); }
//    int GetNumNotes();
//    wxString GetPattern(int i);
//    wxString GetAbsPattern(int i);
//    inline lmFPitch GetNote(int i) { return m_fpNote[i]; }
//
//
//private:
//
//    //member variables
//
//    lmEScaleType      m_nType;
//    EKeySignature  m_nKey;
//    lmFPitch        m_fpNote[lmNOTES_IN_SCALE];     //the scale
//
//};
//
//
//}   //namespace lenmus
//
//#endif  // __LENMUS_SCALESMANAGER_H__
