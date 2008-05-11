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

class lmBox;
class lmColStaffObjs;
class lmVStaffCursor;
class lmInstrGroup;


class lmInstrument : public lmScoreObj
{
public:
    //ctors and dtor
    lmInstrument(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
                 wxString sName, wxString sAbbrev);
    lmInstrument(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
                 lmScoreText* pName, lmScoreText* pAbbrev);
    ~lmInstrument();

	//---- virtual methods of base class -------------------------

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);
	inline lmEScoreObjType GetScoreObjType() { return lmSOT_Instrument; }


	//---- specific methods of this class ------------------------

	inline lmVStaff* GetVStaff() { return m_pVStaff; }

    //Returns the number of staves that this lmInstrument has (1..n)
    inline int GetNumStaves() { return m_pVStaff->GetNumStaves(); }

    //layout
    void SetIndentFirst(lmLocation* pPos) { SetIndent(&m_uIndentFirst, pPos); }
    void SetIndentOther(lmLocation* pPos) { SetIndent(&m_uIndentOther, pPos); }
    lmLUnits GetIndentFirst() { return m_uIndentFirst; }
    lmLUnits GetIndentOther() { return m_uIndentOther; }

    void MeasureNames(lmPaper* pPaper);
    void AddNameAndBracket(lmBox* pBSystem, lmBox* pBSliceInstr, lmPaper* pPaper, int nSystem);

	wxString GetInstrName();

	//cursor methods
	inline lmVStaffCursor* GetCursor() { return m_pVStaff->GetCursor(); }
    inline void ResetCursor() { m_pVStaff->ResetCursor(); }
	inline lmVStaffCursor* AttachCursor(lmScoreCursor* pSCursor)
                                { return m_pVStaff->GetCursor()->AttachCursor(pSCursor); }
	inline void DetachCursor() { m_pVStaff->GetCursor()->DetachCursor(); }

    //group related methods
    void OnRemovedFromGroup(lmInstrGroup* pGroup);
    void OnIncludedInGroup(lmInstrGroup* pGroup);
    bool IsLastOfGroup();
    bool IsFirstOfGroup();

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
    void Create(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
                lmScoreText* pName, lmScoreText* pAbbrev);
    void SetIndent(lmLUnits* pIndent, lmLocation* pPos);
	void AddNameAbbrevShape(lmBox* pBox, lmPaper* pPaper, lmScoreText* pName);
    bool RenderBraket();

    lmScore*        m_pScore;           //score to whith this instrument belongs
	lmVStaff*		m_pVStaff;			//VStaff for this instrument
    int             m_nMidiInstr;       //num. of MIDI instrument no use for this lmInstrument
    int             m_nMidiChannel;     //MIDI channel to use

    lmLUnits        m_uIndentFirst;     //indentation for first system
    lmLUnits        m_uIndentOther;     //indentation for other systems
    lmScoreText*    m_pName;            //instrument name
    lmScoreText*    m_pAbbreviation;    //instrument abbreviated name

    lmEBracketSymbol    m_nBracket;     //in case in has more that one staff
    lmInstrGroup*       m_pGroup;       //NULL if instrument not in group
    lmLUnits            m_uBracketWidth;    //to render the bracket
    lmLUnits            m_uBracketGap;      //to render the bracket



    // variables related to MusicXML import/export
    wxString    m_xmlId;            // part id
};



#endif    // __LM_INSTRUMENT_H__
