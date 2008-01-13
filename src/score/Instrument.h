//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_INSTRUMENT_H__        //to avoid nested includes
#define __LM_INSTRUMENT_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Instrument.cpp"
#endif

#include <vector>

class lmBox;

class lmInstrument : public lmScoreObj
{
public:
    //ctors and dtor
    lmInstrument(lmScore* pScore, int nNumStaves, int nMIDIChannel, int nMIDIInstr,
                 wxString sName, wxString sAbbrev);
    lmInstrument(lmScore* pScore, int nNumStaves, int nMIDIChannel, int nMIDIInstr,
                 lmScoreText* pName, lmScoreText* pAbbrev);
    ~lmInstrument();

	//---- virtual methods of base class -------------------------

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);
	inline lmEScoreObjType GetScoreObjType() { return lmSOT_Instrument; }


	//---- specific methods of this class ------------------------

    lmVStaff* AddVStaff(bool fOverlayered);
    //returns lmVStaff number nStaff (1..n)
    lmVStaff* GetVStaff(int nStaff);

    //Returns the number of VStaves that this lmInstrument has (1..n)
    inline int GetNumStaves() {return (int)m_cStaves.size(); }

    //layout
    void SetIndentFirst(lmLocation* pPos) { SetIndent(&m_nIndentFirst, pPos); }
    void SetIndentOther(lmLocation* pPos) { SetIndent(&m_nIndentOther, pPos); }
    lmLUnits GetIndentFirst() { return m_nIndentFirst; }
    lmLUnits GetIndentOther() { return m_nIndentOther; }

    void MeasureNames(lmPaper* pPaper);
	void AddNameShape(lmBox* pBox, lmPaper* pPaper);
	void AddAbbreviationShape(lmBox* pBox, lmPaper* pPaper);

	wxString GetInstrName();


    //Debug methods
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);

    //MIDI configuration
    int GetMIDIChannel() { return m_nMidiChannel; }
    int GetMIDIInstrument() { return m_nMidiInstr; }

    // methods related to MusicXML import/export
    void XML_SetId(const wxString sId) { m_xmlId = sId; }
    const wxString XML_GetId() { return m_xmlId; }

private:
    void Create(lmScore* pScore, int nNumStaves, int nMIDIChannel, int nMIDIInstr,
                lmScoreText* pName, lmScoreText* pAbbrev);
    void SetIndent(lmLUnits* pIndent, lmLocation* pPos);

    lmScore*        m_pScore;           //score to whith this instrument belongs
	std::vector<lmVStaff*>	m_cStaves;	//list of VStaffs that this instrument has
    int             m_nMidiInstr;       //num. of MIDI instrument no use for this lmInstrument
    int             m_nMidiChannel;     //MIDI channel to use

    lmLUnits        m_nIndentFirst;     //indentation for first system
    lmLUnits        m_nIndentOther;     //indentation for other systems
    lmScoreText*    m_pName;            //instrument name
    lmScoreText*    m_pAbbreviation;    //abreviated name to use


    // variables related to MusicXML import/export
    wxString    m_xmlId;            // part id
};



#endif    // __LM_INSTRUMENT_H__
