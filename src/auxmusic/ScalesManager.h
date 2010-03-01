//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#ifndef __LM_SCALESMANAGER_H__        //to avoid nested includes
#define __LM_SCALESMANAGER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ScalesManager.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/Score.h"
#include "Interval.h"
#include "../exercises/ScalesConstrains.h"
#include "Conversion.h"


//declare global functions defined in this module
extern wxString ScaleTypeToName(lmEScaleType nType);
extern int NumNotesInScale(lmEScaleType nType);
extern bool IsScaleMajor(lmEScaleType nType);
extern bool IsScaleMinor(lmEScaleType nType);
extern bool IsScaleGregorian(lmEScaleType nType);
#define IsTonalScale(nScaleType)  ((nScaleType < est_StartNonTonal))

//a scale is a sequence of up 13 notes (12 chromatic notes plus repetition of first one).
//Change this for more notes in a scale
#define lmNOTES_IN_SCALE  13

class lmScalesManager
{
public:
    //build a scale from root note and type
    lmScalesManager(wxString sRootNote, lmEScaleType nScaleType,
                   lmEKeySignatures nKey = earmDo);
    //destructor
    ~lmScalesManager();

    lmEScaleType GetScaleType() { return m_nType; }
    wxString GetName() { return ScaleTypeToName( m_nType ); }
    int GetNumNotes();
    wxString GetPattern(int i);



private:

    //member variables

    lmEScaleType      m_nType;
    lmEKeySignatures  m_nKey;
    lmFPitch        m_fpNote[lmNOTES_IN_SCALE];     //the scale

};

#endif  // __LM_SCALESMANAGER_H__

