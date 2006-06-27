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
/*! @file Instrument.h
    @brief Header file for class lmInstrument
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __INSTRUMENT_H__        //to avoid nested includes
#define __INSTRUMENT_H__



class lmInstrument
{
public:
    //ctor and dtor
    lmInstrument(lmScore* pScore, wxInt32 nNumStaves, wxInt32 nMIDIChannel, wxInt32 nMIDIInstr);
    ~lmInstrument();

    lmVStaff* AddVStaff();
    //returns lmVStaff number nStaff (1..n)
    lmVStaff* GetVStaff(wxInt32 nStaff);

    //Returns the number of VStaves that this lmInstrument has (1..n)
    inline wxInt32 GetNumStaves() {return (wxInt32)m_cStaves.GetCount(); }

    //Debug methods
    wxString Dump();
    wxString SourceLDP();
    wxString SourceXML();

    //MIDI configuration
    int GetMIDIChannel() { return m_nMidiChannel; }
    int GetMIDIInstrument() { return m_nMidiInstr; }

    // methods related to MusicXML import/export
    void XML_SetId(const wxString sId) { m_xmlId = sId; }
    const wxString XML_GetId() { return m_xmlId; }

private:
    lmScore        *m_pScore;            //score to whith this instrument belongs
    VStavesList    m_cStaves;            //wxList of VStaves that this instrument has
    wxString    m_sNombre;            //instrument name
    wxString    m_sShortName;        //name abreviation to use
    wxInt32        m_nMidiInstr;        //num. of MIDI instrument no use for this lmInstrument
    wxInt32        m_nMidiChannel;        //MIDI channel to use

    // variables related to MusicXML import/export
    wxString    m_xmlId;            // part id
};

// declare a list of Instruments class
#include "wx/list.h"
WX_DECLARE_LIST(lmInstrument, InstrumentsList);



#endif    // __INSTRUMENT_H__
