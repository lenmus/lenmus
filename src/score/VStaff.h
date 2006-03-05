// RCS-ID: $Id: VStaff.h,v 1.4 2006/02/23 19:25:14 cecilios Exp $
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

    lmStaff*    AddStaff(wxInt32 nNumLines=5, lmMicrons nMicrons=1800);
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
                      ETuplas nTupla,
                      bool fBeamed = false, lmTBeamInfo BeamInfo[] = NULL);

    lmNote*    AddNote(bool fAbsolutePitch,
                    wxString sStep, wxString sOctave, wxString sAlter,
                    EAccidentals nAccidentals,
                    ENoteType nNoteType, float rDuration,
                    bool fDotted, bool fDoubleDotted,
                    wxInt32 nStaff,
                    ETuplas nTupla = eTP_NoTupla,
                    bool fBeamed = false, lmTBeamInfo BeamInfo[] = NULL,
                    bool fInChord = false,
                    bool fTie = false,
                    EStemType nStem = eDefaultStem);

    lmBarline* AddBarline(ETipoBarra nType = etbBarraNormal, bool fVisible = true);

    lmWordsDirection* AddWordsDirection(wxString sText, wxString sLanguage,
                        RXMLPositionData oPos, RFontData oFontData);

    void ShiftTime(float rTimeShift);

    wxInt32 GetNumMeasures();

    // rendering methods
    void Draw(lmPaper* pPaper);
    void DrawStaffLines(bool fMeasuring,
                            lmPaper* pPaper,
                            lmMicrons dyInicial,
                            lmMicrons dyEntrePentagramas,
                            lmMicrons xFrom = 0,
                            lmMicrons xTo = 0);
    void DrawProlog(bool fMeasuring, bool fDrawTimekey, lmPaper* pPaper);
    void NewLine(lmPaper* pPaper);
    lmMicrons GetVStaffHeight();
    void SetUpFonts(lmPaper* pPaper);


    lmScoreObj* FindSelectableObject(wxPoint& pt);
    lmNote* FindPossibleStartOfTie(lmPitch nMidiPitch, int nStep);

    lmMicrons TenthsToLogical(lmTenths nTenths, wxInt32 nStaff);
    lmStaff* GetStaff(wxInt32 nStaff);
    lmMicrons GetStaffOffset(wxInt32 nStaff);
    lmMicrons GetXStartOfStaff() { return m_xLeft; }
    lmMicrons GetYTop();
    lmMicrons GetYBottom();


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

    //m_oUltTonalidad As CPOTonalidad
    //m_oUltMetrica As CPOMetrica
    //m_oIniMetrica As CPOMetrica       //métrica con la que empieza el pentagrama

    lmColStaffObjs    m_cStaffObjs;        //collection of StaffObjs that form this lmVStaff

    ////variables que almacenan la situación temporal a medida que se analiza el archivo con la
    ////partitura y se añaden notas y pentobjs. Una vez cargada la partitura dejan de tener sentido.
    ////Todas, salvo m_anContexto, vuelven a necesitarse en distintos momentos:
    ////   - al dibujar la partitura, para ir llevando la situación hasta el punto por donde va dibujando
    //m_anContexto(0 To 6) As Long    //alteraciones actuales
    //m_oCurClave() As CPOClave         //clave actual en cada pentagrama
    //m_oCurTonalidad As CPOTonalidad
    //m_oCurMetrica As CPOMetrica

    // staves
    StaffList           m_cStaves;      //list of Staves (lmStaff objects) that form this lmVStaff
    wxStaffListNode*    m_pStaffNode;   //for navigation

    //to store positions and sizes of Staves
    lmMicrons    m_xLeft;           //X coord.: start of staff lines
    lmMicrons    m_yLinTop;         //Y coord. of first line (line 5, first staff)
    lmMicrons    m_yLinBottom;      //Y coord. of last line (line 1, last staff)
    lmMicrons    m_dxLin;           //length of lines

    lmMicrons    m_leftMargin;      // lmVStaff margins (logical units)
    lmMicrons    m_topMargin;
    lmMicrons    m_rightMargin;
    lmMicrons    m_bottomMargin;

    ////variables temporales que almacenan valores mientras se dibuja la partitura
    //m_xInicioCompas As Long     //coordenada x de inicio del compas en proceso


    ////variables para dibujo
    //m_oPapel As CPapel
    //m_rEspacioPost As Single        //espaciado para las notas en partituras sin justificar.
 //                                       //Se fija en EstablecerPapel


};

// declare a list of staves class
#include "wx/list.h"
WX_DECLARE_LIST(lmVStaff, VStavesList);


#endif    // __VSTAFFT_H__
