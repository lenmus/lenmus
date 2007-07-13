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

#ifndef __SCALESMANAGER_H__        //to avoid nested includes
#define __SCALESMANAGER_H__

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
extern wxString ScaleTypeToName(EScaleType nType);
extern int NumNotesInScale(EScaleType nType);

//a scale is a sequence of up 13 notes (12 chromatic notes plus repetition of first one).
//Change this for more notes in a scale
#define lmNOTES_IN_SCALE  13

class lmScalesManager
{
public:
    //build a scale from root note and type
    lmScalesManager(wxString sRootNote, EScaleType nScaleType,
                   EKeySignatures nKey = earmDo);
    //destructor
    ~lmScalesManager();

    EScaleType GetScaleType() { return m_nType; }
    wxString GetName() { return ScaleTypeToName( m_nType ); }
    int GetNumNotes();
    wxString GetPattern(int i);


private:
//    int GetMidiNote(int nMidiRoot, wxString sInterval);

    //member variables

    EScaleType      m_nType;
    EKeySignatures  m_nKey;
    lmNoteBits      m_tBits[lmNOTES_IN_SCALE];

};

#endif  // __SCALESMANAGER_H__

