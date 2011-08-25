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

//#ifndef __LENMUS_THOEKEYSIGNCONSTRAINS_H__        //to avoid nested includes
//#define __LENMUS_THOEKEYSIGNCONSTRAINS_H__
//
//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma interface "TheoKeySignConstrains.cpp"
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
//#include "../score/Score.h"
//#include "lenmus_constrains.h"


namespace lenmus
{

//enum EProblemTheoKeySig
//{
//    eIdentifyKeySignature = 0,
//    eWriteKeySignature,
//    eBothKeySignProblems
//};
//
//enum EScaleMode
//{
//    eMajorMode = 0,
//    eMinorMode,
//    eMayorAndMinorModes
//};
//
//
///*! @class TheoKeySignConstrains
//    @brief Options for TheoKeySignCtrol control
//*/
//class TheoKeySignConstrains : public ExerciseOptions
//{
//public:
//    TheoKeySignConstrains(wxString sSection, ApplicationScope& appScope);
//    ~TheoKeySignConstrains() {}
//
//    EProblemTheoKeySig GetProblemType() { return m_nProblemType; }
//    void SetProblemType(EProblemTheoKeySig nType) { m_nProblemType = nType; }
//
//    int GetMaxAccidentals() { return m_nMaxAccidentals; }
//    void SetMaxAccidentals(int nNum) { m_nMaxAccidentals = nNum; }
//
//    EScaleMode GetScaleMode() { return m_nMode; }
//    void SetScaleMode(EScaleMode nMode) { m_nMode = nMode; }
//
//    bool IsValidClef(EClefExercise nClef) { return m_oClefs.IsValid(nClef); }
//    void SetClef(EClefExercise nClef, bool fValid) { m_oClefs.SetValid(nClef, fValid); }
//    ImoClefConstrain* GetClefConstrains() { return &m_oClefs; }
//
//    KeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }
//
//
//private:
//    EProblemTheoKeySig      m_nProblemType;
//    int                     m_nMaxAccidentals;
//    EScaleMode              m_nMode;
//    KeyConstrains         m_oValidKeys;           //allowed key signatures
//    ImoClefConstrain         m_oClefs;               //allowed clefs
//
//};


}   // namespace lenmus

//#endif  // __LENMUS_THOEKEYSIGNCONSTRAINS_H__
