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
/*! @file VStaff.h
    @brief Header file for class lmVStaff
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __VSTAFFT_H__        //to avoid nested includes
#define __VSTAFFT_H__

#include "StaffObjIterator.h"
class StaffList;



class lmVStaff
{
public:
    //ctor and dtor
    lmVStaff(lmScore* pScore, lmInstrument* pInstr);
    ~lmVStaff();

    lmStaff*    AddStaff(wxInt32 nNumLines=5, lmLUnits nMicrons=0);
    lmClef*    AddClef(EClefType nClefType, wxInt32 nStaff = 1, bool fVisible = true);

    lmTimeSignature* AddTimeSignature(int nBeats, int nBeatType,
                        bool fVisible = true);    //for type eTS_Normal
    lmTimeSignature* AddTimeSignature(ETimeSignature nTimeSign,
                        bool fVisible = true);    //for type eTS_Normal
    lmTimeSignature* AddTimeSignature(ETimeSignatureType nType,
                        bool fVisible = true);    //for types eTS_Common, eTS_Cut and eTS_SenzaMisura
    lmTimeSignature* AddTimeSignature(int nSingleNumber,
                        bool fVisible = true);    //for type eTS_SingleNumber
    lmTimeSignature* AddTimeSignature(int nNumBeats, int nBeats[], int nBeatType,
                        bool fVisible = true);    //for type eTS_Composite
    lmTimeSignature* AddTimeSignature(int nNumFractions, int nBeats[], int nBeatType[],
                        bool fVisible = true);    //for type eTS_Multiple

    lmKeySignature* AddKeySignature(int nFifths, bool fMajor);
    lmKeySignature* AddKeySignature(EKeySignatures nKeySignature);

    lmRest*    AddRest(ENoteType nNoteType, float rDuration,
                      bool fDotted, bool fDoubleDotted,
                      wxInt32 nStaff,
                      bool fBeamed = false, lmTBeamInfo BeamInfo[] = NULL);

    lmNote*    AddNote(bool fAbsolutePitch,
                    wxString sStep, wxString sOctave, wxString sAlter,
                    EAccidentals nAccidentals,
                    ENoteType nNoteType, float rDuration,
                    bool fDotted, bool fDoubleDotted,
                    wxInt32 nStaff,
                    bool fBeamed = false, lmTBeamInfo BeamInfo[] = NULL,
                    bool fInChord = false,
                    bool fTie = false,
                    EStemType nStem = eDefaultStem);

    lmBarline* AddBarline(EBarline nType = etb_SimpleBarline, bool fVisible = true);

    lmWordsDirection* AddWordsDirection(wxString sText, wxString sLanguage,
                        RXMLPositionData oPos, RFontData oFontData);

    void ShiftTime(float rTimeShift);

    wxInt32 GetNumMeasures();

    // rendering methods
    void DrawStaffLines(bool fMeasuring,
                        lmPaper* pPaper,
                        lmLUnits dyInicial,
                        lmLUnits dyEntrePentagramas,
                        lmLUnits xFrom = 0,
                        lmLUnits xTo = 0);
    void DrawProlog(bool fMeasuring, bool fDrawTimekey, lmPaper* pPaper);
    void NewLine(lmPaper* pPaper);
    lmLUnits GetVStaffHeight();
    void SetUpFonts(lmPaper* pPaper);


    lmScoreObj* FindSelectableObject(wxPoint& pt);
    lmNote* FindPossibleStartOfTie(lmPitch nMidiPitch, int nStep);

    lmLUnits TenthsToLogical(lmTenths nTenths, wxInt32 nStaff);
    lmStaff* GetStaff(wxInt32 nStaff);
    lmLUnits GetStaffOffset(wxInt32 nStaff);
    lmLUnits GetXStartOfStaff() { return m_xLeft; }
    lmLUnits GetYTop();
    lmLUnits GetYBottom();


    //Debug methods
    wxString Dump();
    wxString SourceLDP();
    wxString SourceXML();

    // restricted methods
    lmStaffObjIterator* CreateIterator(ETraversingOrder nOrder);    //for lmFormatter objects

    //for navigation along staves
    int GetNumStaves() { return (int)m_cStaves.GetCount(); }
    lmStaff* GetFirstStaff();
    lmStaff* GetNextStaff();
    lmStaff* GetLastStaff();

    //context management
    void UpdateContext(lmNote* pStartNote, int nStaff, int nStep,
                       int nNewAccidentals, lmContext* pCurrentContext);

    //sound related methods
    lmSoundManager* ComputeMidiEvents(int nChannel);



private:
    void SetFont(lmStaff* pStaff, lmPaper* pPaper);
    void ResetContexts();

    //common code for all time signatures types
    lmTimeSignature* lmVStaff::AddTimeSignature(lmTimeSignature* pTS);




        // member variables

    lmScore        *m_pScore;           //lmScore to which this lmVStaff belongs
    lmInstrument   *m_pInstrument;      //lmInstrument to which this lmVStaff belongs

    lmColStaffObjs    m_cStaffObjs;        //collection of StaffObjs that form this lmVStaff

    // staves
    StaffList           m_cStaves;      //list of Staves (lmStaff objects) that form this lmVStaff
    wxStaffListNode*    m_pStaffNode;   //for navigation

    //to store positions and sizes of Staves
    lmLUnits    m_xLeft;           //X coord.: start of staff lines
    lmLUnits    m_yLinTop;         //Y coord. of first line (line 5, first staff)
    lmLUnits    m_yLinBottom;      //Y coord. of last line (line 1, last staff)
    lmLUnits    m_dxLin;           //length of lines

    lmLUnits    m_leftMargin;      // lmVStaff margins (logical units)
    lmLUnits    m_topMargin;
    lmLUnits    m_rightMargin;
    lmLUnits    m_bottomMargin;
    lmLUnits    m_nHeight;          //TopMargin + Staves height + BottomMargin

};

// declare a list of staves class
#include "wx/list.h"
WX_DECLARE_LIST(lmVStaff, VStavesList);


#endif    // __VSTAFFT_H__
