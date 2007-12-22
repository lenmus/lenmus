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

#ifndef __LM_VSTAFF_H__        //to avoid nested includes
#define __LM_VSTAFF_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "VStaff.cpp"
#endif

#include "StaffObjIterator.h"

class StaffList;
class lmSpacer;
class lmMetronomeMark;
class lmBoxSliceVStaff;



class lmVStaff : public lmScoreObj
{
public:
    lmVStaff(lmScore* pScore, lmInstrument* pInstr, bool fOverlayered);
    ~lmVStaff();

	//---- virtual methods of base class -------------------------

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);


	//---- specific methods of this class ------------------------

	//Adding StaffObs (at the end)
    lmStaff*    AddStaff(int nNumLines=5, lmLUnits nMicrons=0);
    lmClef*     AddClef(lmEClefType nClefType, int nStaff = 1, bool fVisible = true);

    lmTimeSignature* AddTimeSignature(int nBeats, int nBeatType,
                        bool fVisible = true);    //for type eTS_Normal
    lmTimeSignature* AddTimeSignature(lmETimeSignature nTimeSign,
                        bool fVisible = true);    //for type eTS_Normal
    lmTimeSignature* AddTimeSignature(lmETimeSignatureType nType,
                        bool fVisible = true);    //for types eTS_Common, eTS_Cut and eTS_SenzaMisura
    lmTimeSignature* AddTimeSignature(int nSingleNumber,
                        bool fVisible = true);    //for type eTS_SingleNumber
    lmTimeSignature* AddTimeSignature(int nNumBeats, int nBeats[], int nBeatType,
                        bool fVisible = true);    //for type eTS_Composite
    lmTimeSignature* AddTimeSignature(int nNumFractions, int nBeats[], int nBeatType[],
                        bool fVisible = true);    //for type eTS_Multiple

    lmKeySignature* AddKeySignature(int nFifths, bool fMajor, bool fVisible = true);
    lmKeySignature* AddKeySignature(lmEKeySignatures nKeySignature, bool fVisible = true);

    lmRest*     AddRest(lmENoteType nNoteType, float rDuration,
                      bool fDotted, bool fDoubleDotted,
                      int nStaff, bool fVisible = true,
                      bool fBeamed = false, lmTBeamInfo BeamInfo[] = NULL);

    lmNote*     AddNote(lmEPitchType nPitchType,
                    wxString sStep, wxString sOctave, wxString sAlter,
                    lmEAccidentals nAccidentals,
                    lmENoteType nNoteType, float rDuration,
                    bool fDotted, bool fDoubleDotted,
                    int nStaff, bool fVisible = true,
                    bool fBeamed = false, lmTBeamInfo BeamInfo[] = NULL,
                    bool fInChord = false,
                    bool fTie = false,
                    lmEStemType nStem = lmSTEM_DEFAULT);

    lmBarline*  AddBarline(lmEBarline nType = etb_SimpleBarline, bool fVisible = true);

    lmMetronomeMark* AddMetronomeMark(int nTicksPerMinute,
                            bool fParentheses = false, bool fVisible = true);
    lmMetronomeMark* AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                            lmENoteType nRightNoteType, int nRightDots,
                            bool fParentheses = false, bool fVisible = true);
    lmMetronomeMark* AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                            int nTicksPerMinute,
                            bool fParentheses = false, bool fVisible = true);

    lmSOControl* AddNewSystem();

    lmSpacer* AddSpacer(lmTenths nWidth);

    lmStaffObj* AddAnchorObj();

    lmStaffObj* AddText(wxString sText, lmEAlignment nAlign,
                        lmLocation* pPos, lmFontInfo oFontData, bool fHasWidth);

	//inserting StaffObs
    lmBarline* InsertBarline(lmStaffObj* pCursorSO, lmEBarline nType = etb_SimpleBarline);
	lmClef* InsertClef(lmStaffObj* pCursorSO, lmEClefType nClefType);
	lmNote* InsertNote(lmStaffObj* pCursorSO, lmEPitchType nPitchType, wxString sStep,
					   wxString sOctave, lmENoteType nNoteType, float rDuration);




    void ShiftTime(float rTimeShift);

    int GetNumMeasures();

    // rendering methods
    lmLUnits LayoutStaffLines(lmBox* pBox, lmLUnits xFrom, lmLUnits xTo, lmLUnits yPos);
    void DrawProlog(bool fMeasuring, int nMeasure, bool fDrawTimekey, lmPaper* pPaper);
	void AddPrologShapes(lmBoxSliceVStaff* pBSV, int nMeasure, bool fDrawTimekey, lmPaper* pPaper);
    void NewLine(lmPaper* pPaper);
    lmLUnits GetVStaffHeight();
    void SetUpFonts(lmPaper* pPaper);
    bool IsOverlayered() { return m_fOverlayered; }
    lmLUnits GetStaffLineThick(int nStaff);
    inline bool HideStaffLines() { return GetOptionBool(_T("StaffLines.Hide")); }

    lmNote* FindPossibleStartOfTie(lmAPitch anPitch);

	//units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths, int nStaff);
	lmTenths LogicalToTenths(lmLUnits uUnits, int nStaff);

    lmStaff* GetStaff(int nStaff);
    lmLUnits GetStaffOffset(int nStaff);
    lmLUnits GetYTop();
    lmLUnits GetYBottom();


    //Debug methods
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);

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

    //renderization related methods
    bool GetXPosFinalBarline(lmLUnits* pPos);
    void SetSpaceBeforeClef(lmLUnits nSpace) { m_nSpaceBeforeClef = nSpace; }
    lmLUnits GetSpaceBeforeClef() { return m_nSpaceBeforeClef; }

    //miscellaneous
    inline bool IsGlobalStaff() const { return (m_pInstrument == (lmInstrument*)NULL); }


private:
    void SetFont(lmStaff* pStaff, lmPaper* pPaper);
    void ResetContexts();

    //common code for all time signatures types
    lmTimeSignature* AddTimeSignature(lmTimeSignature* pTS);




        // member variables

    lmScore*        m_pScore;           //lmScore to which this lmVStaff belongs
    lmInstrument*   m_pInstrument;      //lmInstrument to which this lmVStaff belongs
    bool            m_fOverlayered;     //this VStaff is overlayered on previous one

    lmColStaffObjs    m_cStaffObjs;        //collection of StaffObjs that form this lmVStaff

    // staves
    StaffList           m_cStaves;      //list of Staves (lmStaff objects) that form this lmVStaff
    wxStaffListNode*    m_pStaffNode;   //for navigation

    //to draw barlines, from first staff to last staff
    lmLUnits    m_yLinTop;         //Y coord. of first line (line 5, first staff)
    lmLUnits    m_yLinBottom;      //Y coord. of last line (line 1, last staff)

    lmLUnits    m_leftMargin;      // lmVStaff margins (logical units)
    lmLUnits    m_topMargin;
    lmLUnits    m_rightMargin;
    lmLUnits    m_bottomMargin;
    lmLUnits    m_nHeight;          //TopMargin + Staves height + BottomMargin

    //for drawing prolog
    lmLUnits    m_nSpaceBeforeClef;

};



#endif    // __LM_VSTAFF_H__
