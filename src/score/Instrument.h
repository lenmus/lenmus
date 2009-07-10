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

#ifndef __LM_INSTRUMENT_H__        //to avoid nested includes
#define __LM_INSTRUMENT_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Instrument.cpp"
#endif

class lmBox;
class lmColStaffObjs;
class lmVStaffCursor;
class lmInstrGroup;
class lmVStaff;
class lmScoreCursor;
class lmDlgProperties;


class lmInstrument : public lmScoreObj
{
public:
    //ctors and dtor
    lmInstrument(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
                 wxString sName, wxString sAbbrev);
    lmInstrument(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
                 lmInstrNameAbbrev* pName, lmInstrNameAbbrev* pAbbrev);
    ~lmInstrument();

	//---- virtual methods of base class -------------------------

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);
	inline lmEScoreObjType GetScoreObjType() { return lmSOT_Instrument; }
    inline lmScore* GetScore() { return m_pScore; }


	//---- specific methods of this class ------------------------

	inline lmVStaff* GetVStaff() { return m_pVStaff; }

    //Returns the number of staves that this lmInstrument has (1..n)
    int GetNumStaves();

    //layout
    inline void SetIndentFirst(lmLocation* pPos) { SetIndent(&m_uIndentFirst, pPos); }
    inline void SetIndentOther(lmLocation* pPos) { SetIndent(&m_uIndentOther, pPos); }
    inline lmLUnits GetIndentFirst() { return m_uIndentFirst; }
    inline lmLUnits GetIndentOther() { return m_uIndentOther; }

    void MeasureNames(lmPaper* pPaper);
    void AddNameAndBracket(lmBox* pBSystem, lmBox* pBSliceInstr, lmPaper* pPaper, int nSystem);

    //properties
	const wxString& GetInstrName();
    bool IsFirstOfSystem();

	//cursor methods
	lmVStaffCursor* GetVCursor();
    void ResetCursor();
    void AttachCursor(lmVStaffCursor* pVCursor);

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
    inline int GetMIDIChannel() { return m_nMidiChannel; }
    inline int GetMIDIInstrument() { return m_nMidiInstr; }
    inline void SetMIDIChannel(int nMidiChannel) { m_nMidiChannel = nMidiChannel; }
    inline void SetMIDIInstrument(int nMidiInstr) { m_nMidiInstr = nMidiInstr; }

    // methods related to MusicXML import/export
    inline void XML_SetId(const wxString sId) { m_xmlId = sId; }
    inline const wxString XML_GetId() { return m_xmlId; }

	//interactive edition
	void OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName = wxEmptyString);
    void OnPropertiesChanged();


private:
    void Create(lmScore* pScore, int nMIDIChannel, int nMIDIInstr,
                lmInstrNameAbbrev* pName, lmInstrNameAbbrev* pAbbrev);
    void AddName(wxString& sName);
    void AddAbbreviation(wxString& sAbbrev);
    void SetIndent(lmLUnits* pIndent, lmLocation* pPos);
	void AddNameAbbrevShape(lmBox* pBox, lmPaper* pPaper, lmInstrNameAbbrev* pName);
    bool RenderBraket();

    lmScore*            m_pScore;           //score to whith this instrument belongs
	lmVStaff*		    m_pVStaff;			//VStaff for this instrument
    int                 m_nMidiInstr;       //num. of MIDI instrument no use for this lmInstrument
    int                 m_nMidiChannel;     //MIDI channel to use

    lmLUnits            m_uIndentFirst;     //indentation for first system
    lmLUnits            m_uIndentOther;     //indentation for other systems
    lmInstrNameAbbrev*  m_pName;            //instrument name
    lmInstrNameAbbrev*  m_pAbbreviation;    //instrument abbreviated name

    lmEBracketSymbol    m_nBracket;         //in case in has more that one staff
    lmInstrGroup*       m_pGroup;           //NULL if instrument not in group
    lmLUnits            m_uBracketWidth;    //to render the bracket
    lmLUnits            m_uBracketGap;      //to render the bracket

    // variables related to MusicXML import/export
    wxString            m_xmlId;            // part id
};



#endif    // __LM_INSTRUMENT_H__
