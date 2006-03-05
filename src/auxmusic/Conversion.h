// RCS-ID: $Id: Conversion.h,v 1.5 2006/02/23 19:18:28 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file Conversion.h
    @brief Header file for gloabl static functions for conversions
    @ingroup auxmusic
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __CONVERSION_H__        //to avoid nested includes
#define __CONVERSION_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class lmConverter
{
public:
    lmConverter() {}
    ~lmConverter() {}
    
    //MIDI pitch
    static lmPitch  PitchToMidiPitch(lmPitch nPitch);
    static lmPitch  MidiPitchToPitch(lmPitch nMidiPitch);
    static wxString MidiPitchToLDPName(lmPitch nMidiPitch);
    static wxString PitchToLDPName(lmPitch nPitch);
    static bool     IsNaturalNote(lmPitch ntMidi, EKeySignatures nTonalidad);

    // pitch name
    wxString        GetEnglishNoteName(lmPitch nPitch);
    wxString        GetNoteName(lmPitch nPitch);




private:

};

#endif  // __CONVERSION_H__
