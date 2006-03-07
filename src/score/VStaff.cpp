// RCS-ID: $Id: VStaff.cpp,v 1.5 2006/03/03 15:01:11 cecilios Exp $
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
/*! @file VStaff.cpp
    @brief Implementation file for class lmVStaff
    @ingroup score_kernel
*/
//--------------------------------------------------------------------------------------------------
/*! @class lmVStaff
    @ingroup score_kernel
    @brief A generalization of the 'satff' concept.

    The object lmVStaff (Virtual staff) is a generalization of the concept 'staff': it is a staff
    with an great number of lines and spaces (almost infinite) so to represent all posible pitches,
    but whose visual renderization can be controlled so
    that only the specified (needed) lines are rendered i.e: the standard five-lines staff, the piano
    grand staff, the single line staff used for rithm or unpitched music, or any other design you
    would like.

    An lmVStaff can contain many 'staffs' (groups of lines rendered toghether), depending on the layout
    choosen. For example:
    - the standard five-lines staff is an lmVStaff with one five-lines staff.
    - the grand staff is an lmVStaff with two five-lines staves.
    - the single line unpitched staff is an VSatff with one sigle line staff.

    The concept of 'staff' is not modelled into the program, its is just a term to be used only 
    in documentation, algoritms explanations, etc.

    An lmVStaff is composed by objects such as notes, rests, barlines, keys, and all other
    musical signs traditionally used for writing music.
    All this objects than can appear in a staff will be modeled by an abstract class 'lmStaffObj'.
    Therefore an lmVStaff is, roughfly, a collection of StaffObjs and some attributes.

    Bars are not modelled by objects. They are just the collection of lmStaffObj found between
    two lmStaffObj of type 'barline' (and between the start of the score and the first barline).
    Nevertheless, tha concept of bar is very important at least in two situations:
        1. In interpretation (paying the score). For example:  play from bar #7
        2. When rendering the score, as all the objects in a bar must be rendered toghedtr in the same
        paper line.
    Due to this, althoug the bar is not modelled as an object, there exits methods in the VirtualStaff
    object to deal with bars.

    La definición de compas que se ha adoptado (el conjunto de los staffobj que haya
    entre dos staffobj de tipo "barra de compas") implica que todos los elementos de una
    partitura, menos la barra final de cierre, pertenecen a algún compas. Así pues la
    definición adoptada engloba también a la de que "una parte es un conjunto de compases" pero
    es más general.

    Nota: como el objeto CStaff proviene de la evolución, por generalización, del objeto CStaff
    pueden quedar, en comentarios y nombres, referencias a Pentagrama en vez de Parte
*/
//-------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Modelo
//   - Todos los StaffObjs que componen el pentagrama están en la colección m_cStaffObjs, ordenados por
//   orden de creación
//   - Cada staffobj:
//       - tiene un ID único, lo que permite identificarlo y comparar si dos referencias
//           se refieren al mismo staffobj
//       - mantiene el núm del compas al que pertenece. Si se inserta o se borra un compas
//           hay que renumerar
//   - La colección m_cPoInicioCompas contiene un puntero al primer staffobj de cada compas
//
//   - THINK: Dado un Pentobj debería ser inmediato localizarle dentro de la colección. Para ello
//   en cada staffobj debería guardarse su índice en la colección. Ello obliga a realmacenar índices
//   tras una inserción o un borrado. La alternativa actual es recorrer secuencialmente la
//   colección. Ver método CIterador.PosicionarEnItem
//--------------------------------------------------------------------------------------------------

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "../app/global.h"

//implementation of the staves List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(VStavesList);


//constructor
lmVStaff::lmVStaff(lmScore* pScore, lmInstrument* pInstr)
{
    //pScore is the lmScore to which this vstaff belongs.
    //Initially the lmVStaff will have only one standard five-lines staff. This can be
    //later modified invoking the 'AddStaff' method (REVIEW)
       
    m_pScore = pScore;
    m_pInstrument = pInstr;

    // default lmVStaff margins (logical units = tenths of mm)
    m_leftMargin = 0;
    m_topMargin = 0;
    m_rightMargin = 0;
    m_bottomMargin = 100;    // 1 cm

    //create one standard staff (five lines, 7.2 mm height)
    lmStaff* pStaff = new lmStaff(pScore);
    m_cStaves.Append(pStaff);
    
    g_pLastNoteRest = (lmNoteRest*)NULL;

}

lmVStaff::~lmVStaff()
{
    m_cStaves.DeleteContents(true);
    m_cStaves.Clear();
}

lmStaff* lmVStaff::AddStaff(wxInt32 nNumLines, lmMicrons nMicrons)
{
    lmStaff* pStaff = new lmStaff(m_pScore, nNumLines, nMicrons);
    m_cStaves.Append(pStaff);
    
    return pStaff;

}

lmStaff* lmVStaff::GetStaff(wxInt32 nStaff)
{
    //iterate over the collection of Staves (lmStaff Objects) to locate staff nStaff
    wxInt32 iS;
    StaffList::Node* pNode = m_cStaves.GetFirst();
    for (iS=1 ; iS != nStaff && pNode; iS++ ) {
        pNode = pNode->GetNext();    //get next lmStaff
    }
    wxASSERT(pNode);
    return (lmStaff *)pNode->GetData();

}

lmStaff* lmVStaff::GetFirstStaff()
{
    m_pStaffNode = m_cStaves.GetFirst();
    return (m_pStaffNode ? (lmStaff *)m_pStaffNode->GetData() : (lmStaff *)m_pStaffNode);
}

lmStaff* lmVStaff::GetNextStaff()
{
    wxASSERT(m_pStaffNode);
    m_pStaffNode = m_pStaffNode->GetNext();
    return (m_pStaffNode ? (lmStaff *)m_pStaffNode->GetData() : (lmStaff *)m_pStaffNode);
}

lmStaff* lmVStaff::GetLastStaff()
{
    wxASSERT(m_pStaffNode);
    m_pStaffNode = m_cStaves.GetLast();
    return (m_pStaffNode ? (lmStaff *)m_pStaffNode->GetData() : (lmStaff *)m_pStaffNode);
}

lmMicrons lmVStaff::TenthsToLogical(lmTenths nTenths, wxInt32 nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->TenthsToLogical(nTenths);

}

void lmVStaff::UpdateContext(lmNote* pStartNote, int nStaff, int nStep,
                           int nNewAccidentals, lmContext* pCurrentContext)
{
    /*
    Note pStartNote (whose diatonic name is nStep) has accidentals that must be propagated to
    the context and to the following notes until the end of the measure or until a new accidental
    for the same step is found
    */


    //create a new context by updating current one
    lmStaff* pStaff = GetStaff(nStaff);
    lmContext* pNewContext = pStaff->NewContext(pCurrentContext, nNewAccidentals, nStep);    

    /*! @todo
    For now, as we are not yet dealing with edition, it is not possible to
    insert notes in a score. Therefore, there are no notes after the one being
    processed (pStartNote). So the treatment to propagate accidentals until the
    start of the next measure is not yet implemented.
    */

    /*
    //propagate new context
    //define a forward iterator
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmNoteRest* pNR = (lmNoteRest*)NULL;
    lmNote* pNote = (lmNote*)NULL;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    pIter->MoveToObject(pStartNote);
    wxASSERT(pIter);
    pIter->MoveNext();

    while(!pIter->EndOfList())
    {
        pSO = pIter->GetCurrent();
        switch (pSO->GetType()) {
            case eTPO_NoteRest:
                if (pSO->GetStaffNum() == nStaff) {
                    pNR = (lmNoteRest*)pSO;
                    if (!pNR->IsRest()) {
                        pNote = (lmNote*)pSO;
                        if (pNote->UpdateContext(nStep, nNewAccidentals, pNewContext)) {
                            //if returns true, no context modification was necessary.
                            //So is not necessary to continue until the end of the measure
                            delete pIter;
                            return;
                        }
                    }
                }
                break;

            case eTPO_Barline:
                //End of measure reached. End uptade process
                    delete pIter;
                    return;
                break;

            default:
                ;
        }
        pIter->MoveNext();
    }
    delete pIter;
    return;
    */

}


//---------------------------------------------------------------------------------------
// Methods for adding StaffObjs 
//---------------------------------------------------------------------------------------

////añade un staffobj de tipo lmClef al final de la colección.
////Si con este staffobj se inicia un compas, incrementa la cuenta de compases y guarda el índice al
////staffobj con el que comienza (este que se añade)
lmClef* lmVStaff::AddClef(EClefType nClefType, wxInt32 nStaff, bool fVisible)
{    
    wxASSERT_MSG(nStaff <= GetNumStaves(), _T(""));
    
    lmClef *pClef = new lmClef(nClefType, this, nStaff, fVisible);
    lmStaff* pStaff = GetStaff(nStaff);
    pStaff->NewContext(pClef);
    m_cStaffObjs.Store(pClef);
    return pClef;

}

// returns a pointer to the lmNote object just created
lmNote* lmVStaff::AddNote(bool fAbsolutePitch,
                    wxString sStep, wxString sOctave, wxString sAlter,
                    EAccidentals nAccidentals,
                    ENoteType nNoteType, float rDuration,
                    bool fDotted, bool fDoubleDotted,
                    wxInt32 nStaff,
                    ETuplas nTupla,
                    bool fBeamed, lmTBeamInfo BeamInfo[],
                    bool fInChord,
                    bool fTie,
                    EStemType nStem)
{

    wxASSERT(nStaff <= GetNumStaves() );
    
    lmStaff* pStaff = GetStaff(nStaff);
    lmContext* pContext = pStaff->GetLastContext();

    lmNote* pNt = new lmNote(this, fAbsolutePitch,
                        sStep, sOctave, sAlter, nAccidentals,
                        nNoteType, rDuration, fDotted, fDoubleDotted, nStaff, pContext, 
                        nTupla, fBeamed, BeamInfo, fInChord, fTie, nStem);

    m_cStaffObjs.Store(pNt);
    return pNt;

}

// returns a pointer to the lmRest object just created
lmRest* lmVStaff::AddRest(ENoteType nNoteType, float rDuration,
                      bool fDotted, bool fDoubleDotted,
                      wxInt32 nStaff,
                      ETuplas nTupla,
                      bool fBeamed, lmTBeamInfo BeamInfo[])
{
    wxASSERT(nStaff <= GetNumStaves() );
    
    lmStaff* pStaff = GetStaff(nStaff);
    lmContext* pContext = pStaff->GetLastContext();

    lmRest* pR = new lmRest(this, nNoteType, rDuration, fDotted, fDoubleDotted, nStaff,
                        pContext, nTupla, fBeamed, BeamInfo);

    m_cStaffObjs.Store(pR);
    return pR;
    
}

lmWordsDirection* lmVStaff::AddWordsDirection(wxString sText, wxString sLanguage,
                            RXMLPositionData oPos, RFontData oFontData)
{
    lmWordsDirection* pWD = new lmWordsDirection(this, sText, sLanguage,
                        oPos, oFontData);

    m_cStaffObjs.Store(pWD);
    return pWD;

}

//for types eTS_Common, eTS_Cut and eTS_SenzaMisura
lmTimeSignature* lmVStaff::AddTimeSignature(ETimeSignatureType nType, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_SingleNumber
lmTimeSignature* lmVStaff::AddTimeSignature(int nSingleNumber, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nSingleNumber, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Composite
lmTimeSignature* lmVStaff::AddTimeSignature(int nNumBeats, int nBeats[], int nBeatType,
                                        bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nNumBeats, nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Multiple
lmTimeSignature* lmVStaff::AddTimeSignature(int nNumFractions, int nBeats[], int nBeatType[],
                                        bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nNumFractions, nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Normal
lmTimeSignature* lmVStaff::AddTimeSignature(int nBeats, int nBeatType, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

lmTimeSignature* lmVStaff::AddTimeSignature(ETimeSignature nTimeSign, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nTimeSign, this, fVisible);
    return AddTimeSignature(pTS);
}


//common code for all time signatures types
lmTimeSignature* lmVStaff::AddTimeSignature(lmTimeSignature* pTS)
{
    m_cStaffObjs.Store(pTS);

    //Store current time signature
    //As time signatures are common to all staves we store it in all staves
    wxStaffListNode* pNode = m_cStaves.GetFirst();
    lmStaff* pStaff;
    for (; pNode; pNode = pNode->GetNext() ) {
        pStaff = (lmStaff *)pNode->GetData();
        pStaff->NewContext(pTS);
    }

    return pTS;
}

lmKeySignature* lmVStaff::AddKeySignature(int nFifths, bool fMajor)
{
    lmKeySignature* pKS = new lmKeySignature(nFifths, fMajor, this);
    m_cStaffObjs.Store(pKS);

    //Store current key signature
    //As key signatures are common to all staves we store it in all staves
    wxStaffListNode* pNode = m_cStaves.GetFirst();
    lmStaff* pStaff;
    for (; pNode; pNode = pNode->GetNext() ) {
        pStaff = (lmStaff *)pNode->GetData();
        pStaff->NewContext(pKS);
    }

    return pKS;
}

lmKeySignature* lmVStaff::AddKeySignature(EKeySignatures nKeySignature)
{
    int nFifths = KeySignatureToNumFifths(nKeySignature);
    bool fMajor = IsMajor(nKeySignature);
    return AddKeySignature(nFifths, fMajor);
}

wxInt32 lmVStaff::GetNumMeasures()
{
    return m_cStaffObjs.GetNumMeasures();
}

void lmVStaff::Draw(lmPaper* pPaper)
{
    // setup the fonts with the right size
    SetUpFonts(pPaper);

    //iterate over the collection to draw the StaffObjs
    lmStaffObj* pSO;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    while(!pIter->EndOfList()) {
        pSO = pIter->GetCurrent();
        pSO->Draw(DO_MEASURE, pPaper);
        pSO->Draw(DO_DRAW, pPaper);
        pIter->MoveNext();
    }
    delete pIter;

}

void lmVStaff::DrawStaffLines(bool fMeasuring,
                            lmPaper* pPaper,
                            lmMicrons dyInicial,
                            lmMicrons dyEntrePentagramas,
                            lmMicrons xFrom,
                            lmMicrons xTo)
{
    /*
        Draw all staff lines of this lmVStaff and store their sizes and positions
     parameters:
       pCanvas                pointer to Project to use
       fMeasuring            Draw in meassuring mode            
       dyInicial               espaciado antes del 1er pentagrama
       dyEntrePentagramas      espacio entre pentagramas
    
     results:
         if DO_DRAW mode (fMeasuring == false) store values in following variables:
            m_xLeft: posición X de comienzo de las líneas
            m_yLinTop: posición Y de comienzo de la primera línea que se dibuja
            m_yLinBottom: posición Y de comienzo de la última línea que se dibuja
            m_dxLin: largo de las líneas
    */

    if (fMeasuring) { return; }

    wxDC* pDC = pPaper->GetDC();

    lmMicrons xRight, yCur;

    //DEBUG: draw top border of lmVStaff region
    m_xLeft = pPaper->GetLeftMarginXPos();
    xRight = pPaper->GetRightMarginXPos();
    yCur = pPaper->GetCursorY();
    pDC->SetPen(*wxRED_PEN);
    pDC->DrawLine(m_xLeft, yCur-1, xRight, yCur-1);
    //-----------------------------------------


    //Set left position and lenght of lines, and save them
    m_xLeft = ((xFrom==0) ? pPaper->GetLeftMarginXPos() + m_leftMargin : xFrom);
    xRight = ((xTo == 0) ? pPaper->GetRightMarginXPos() - m_rightMargin : xTo);
    m_dxLin = xRight - m_xLeft;            //largo de las líneas
   
    yCur = pPaper->GetCursorY() + m_topMargin;
    m_yLinTop = yCur;              //save y coord. for first line start point

    //DEBUG: draw top border of first lmStaff region
    pDC->SetPen(*wxCYAN_PEN);
    pDC->DrawLine(m_xLeft, yCur-1, xRight, yCur-1);
    //-----------------------------------------

    //??    pDC->DrawWidth = m_nGrosorLineas;
    pDC->SetPen(*wxBLACK_PEN);

    //iterate over the collection of Staves (lmStaff Objects)
    StaffList::Node* pNode = m_cStaves.GetFirst();
    lmStaff* pStaff = (pNode ? (lmStaff *)pNode->GetData() : (lmStaff *)pNode);
    for ( ; pStaff; ) {
        //draw one staff
        for (wxInt32 iL = 1; iL <= pStaff->GetNumLines(); iL++ ) {
            pDC->DrawLine (m_xLeft, yCur, xRight, yCur);
            m_yLinBottom = yCur;                        //save line position
            yCur = yCur + pStaff->GetLineSpacing();     //+ m_nGrosorLineas - 1
        }
        yCur = yCur - pStaff->GetLineSpacing() + pStaff->GetAfterSpace();

        //get next lmStaff
        pNode = pNode->GetNext();
        pStaff = (pNode ? (lmStaff *)pNode->GetData() : (lmStaff *)pNode);
    }
//??    pDC->DrawWidth = 1;

}

void lmVStaff::SetUpFonts(lmPaper* pPaper)
{
    //iterate over the collection of Staves (lmStaff Objects) to set up the fonts
    // to use on that staff
    StaffList::Node* pNode = m_cStaves.GetFirst();
    for ( ; pNode; ) {
        SetFont((lmStaff *)pNode->GetData(), pPaper);
        pNode = pNode->GetNext();
    }
}

void lmVStaff::SetFont(lmStaff* pStaff, lmPaper* pPaper)
{
    // Font "LeMus Notas" has been designed to draw on a staff whose interline
    // space is of 512 FUnits. This gives an optimal rendering on VGA displays (96 pixels per inch)
    // as staff lines are drawn on exact pixels, according to the following relationships:
    //       Let dyLines be the distance between lines (pixels), then
    //       Font size = 3 * dyLines   (points)
    //       Scale = 100 * dyLines / 8     (%)
    //
    // Given a zooming factor (as a percentage, i.e. zoom=250.0%) fontsize can be computed as
    //       i = Round((zoom*8) / 100)
    //       dyLines = i        (pixels)
    //       FontSize = 3*i        (points)
    //
    // As all scaling takes place in the DC it is not necessary to allocate fonts of
    // different size as all scaling takes place in the DC. Then:
    //       Let dyLines be the distance between lines (logical units), then
    //       Font size = 3 * dyLines   (logical points)

    lmMicrons dyLinesL = pStaff->GetLineSpacing();

    // the font for drawing is scaled by the DC.
    pStaff->SetFontDraw( pPaper->GetFont(3 * dyLinesL) );        //logical points

    //// the font for dragging is not scaled by the DC as all dragging operations takes
    //// place dealing with device units
    //wxInt32 dyLinesD = pPaper->LogicalToDeviceY(100 * dyLinesL);
    //pStaff->SetFontDrag( pPaper->GetFont((3 * dyLinesD) / 100) );

    ////Calcula, en pixels, el grosor de las líneas (es de 51 FUnits, la décima parte de la distancia entre líneas)
    //m_nGrosorLineas = FUnitsToTwips(51) * nResolucion \ 1440
    //if (m_nGrosorLineas < 1) m_nGrosorLineas = 1;
    //sDbgFonts = sDbgFonts & "nGrosorLineas = " & m_nGrosorLineas & sCrLf
    //xUnits = m_nGrosorLineas


}



//=========================================================================================
// Methods for finding StaffObjs
//=========================================================================================

lmScoreObj* lmVStaff::FindSelectableObject(wxPoint& pt)
{
    lmStaffObj* pSO;
    lmScoreObj* pChildSO;
    lmCompositeObj* pCO;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_OptimizeAccess);
    //iterate over the collection of StaffObjs to look for a suitable lmStaffObj
    while(!pIter->EndOfList()) {
        pSO = pIter->GetCurrent();
        if (pSO->IsComposite()) {
            pCO = (lmCompositeObj*)pSO;
            pChildSO = pCO->FindSelectableObject(pt);
            if (pChildSO) {
                delete pIter;
                return pChildSO;
            }
        }
        else {
            if (pSO->IsAtPoint(pt)) {
                delete pIter;
                return pSO;
            }
        }
        pIter->MoveNext();
    }
    delete pIter;
    return (lmScoreObj*) NULL;

}

lmMicrons lmVStaff::GetStaffOffset(wxInt32 nStaff)
{
    //returns the Y offset to staff nStaff (1..n)
    wxASSERT(nStaff <= GetNumStaves() );
    lmMicrons yOffset = m_topMargin;

    // iterate over the collection of Staves (lmStaff Objects) to add up the
    // height and after space of all previous staves to the requested one
    lmStaff* pStaff;
    StaffList::Node* pNode = m_cStaves.GetFirst();
    for (wxInt32 iS=1 ; iS < nStaff && pNode; iS++) {
        pStaff = (lmStaff *)pNode->GetData();
        yOffset += pStaff->GetHeight();
        yOffset += pStaff->GetAfterSpace();
        pNode = pNode->GetNext();
    }
    return yOffset;

}

wxString lmVStaff::Dump()
{
    wxString sDump = _T("");

    //iterate over the collection to dump the StaffObjs
    lmStaffObj* pSO;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_AsStored);
    while(!pIter->EndOfList())
    {
        pSO = pIter->GetCurrent();
        sDump += pSO->Dump();
        pIter->MoveNext();
    }
    delete pIter;
    return sDump;
 
}

wxString lmVStaff::SourceLDP()
{
    wxString sSource;
    wxStaffObjsListNode* pNode;

    //iterate over the collection of StaffObjs
    int iC = 1;            // bar number
    lmStaffObj* pSO;
    lmStaffObjIterator* pIT = m_cStaffObjs.CreateIterator(eTR_AsStored);  //THINK: Should be eTR_ByTime?
    while(!pIT->EndOfList())
    {
        pSO = pIT->GetCurrent();
        //verify if this lmStaffObj is the start of a new bar
        wxASSERT(GetNumMeasures() > 0);
        if (iC <= GetNumMeasures())
        {
            pNode = m_cStaffObjs.GetFirstInMeasure(iC);
            wxASSERT(pNode);
            if (pSO->GetID() == ((lmStaffObj*)pNode->GetData())->GetID() )
            {
                //if it is not the first bar close the previous one
                if (iC != 1) { sSource += _T("      )\n"); }
                //start a new bar
                sSource += wxString::Format(_T("         (C %d\n"), iC);
                iC++;
            }
        }

        sSource += pSO->SourceLDP();
        pIT->MoveNext();
    }
    delete pIT;

    sSource += _T("         )\n");        // end of last bar
    return sSource;

//    Do While oIT.QuedanItems
//        Set oPo = oIT.GetItem
//        //determina si empieza compas
//        if (iC <= this.NumCompases {
//            if (oPo.ID = m_cStaffObjs.GetFirstInMeasure(iC).ID {
//                //si no es el primer compas cierra el precedente
//                if (iC != 1 { sFuente = sFuente & "      )" & sCrLf
//                //inicia nuevo compas
//                sFuente = sFuente & "         (C " & iC & sCrLf
//                iC = iC + 1
//            }
//        }
//        //obtiene el fuente del staffobj
//        sFuente = sFuente & oPo.Fuente & sCrLf
//        //avanza al siguiente staffobj
//        oIT.AdvanceCursor
//    Loop
//    //cierra último compas
//    sFuente = sFuente & "         )" & sCrLf
//    Fuente = sFuente
    
}

wxString lmVStaff::SourceXML()
{
    wxString sSource = _T("TODO: lmVStaff XML Source code generation methods");

//    Dim oPo As IPentObj, iC As Long, sFuente As String
//    Dim oIT As CIterador
//    
//    Set oIT = m_cStaffObjs.CreateIterator(eTR_AsStored)
//    iC = 1
//    Do While oIT.QuedanItems
//        Set oPo = oIT.GetItem
//        //determina si empieza compas
//        if (iC <= this.NumCompases {
//            if (oPo.ID = m_cStaffObjs.GetFirstInMeasure(iC).ID {
//                //si no es el primer compas cierra el precedente
//                if (iC != 1 { sFuente = sFuente & "    </measure>" & sCrLf
//                //inicia nuevo compas
//                sFuente = sFuente & "    <measure number=" & iC & """>" & sCrLf
//                iC = iC + 1
//            }
//        }
//        //obtiene el fuente del staffobj
//        sFuente = sFuente & oPo.FuenteXML & sCrLf
//        //avanza al siguiente staffobj
//        oIT.AdvanceCursor
//    Loop
//    //cierra último compas
//    sFuente = sFuente & "    </measure>" & sCrLf
//    FuenteXML = sFuente

    return sSource;
    
}

//void lmVStaff::Get GetXStartOfStaff() As Long
//    GetXStartOfStaff = m_xLeft
//}
//
//void lmVStaff::Get GetXInicioCompas() As Long
//    GetXInicioCompas = m_xInicioCompas
//}

// the next two methods are mainly used for drawing the barlines. For that purpose it is necessary
// to know the y coordinate of the top most upper line of first staff and the bottom most lower
// line of the last staff.

lmMicrons lmVStaff::GetYTop()
{
    return m_yLinTop;
}

lmMicrons lmVStaff::GetYBottom()
{
    return m_yLinBottom;
}

//void lmVStaff::Get Largo() As Long
//    Largo = m_dxLin
//}

lmMicrons lmVStaff::GetVStaffHeight()
{
    //! @todo all
    //devuelve la altura (incluido el espacio de separación posterior) del conjunto de pentagramas
    //que componen esta parte. Está expresado en décimas de línea
    
    //Dim iP As Long, rAlto As Single
    //rAlto = m_topMargin
    //iP = 1
    //for (iP = 1 To m_nNumStaves
    //    rAlto = rAlto + CSng((m_nNumLineas(iP) - 1) * 10) + pStaff->GetAfterSpace()
    //}  // iP
    //
    //return rAlto - pStaff->GetAfterSpace() + m_bottomMargin
    return 20000;    // 2cm
    
}

//
//void lmVStaff::Get Contexto(iNota As Long) As Long
//    Contexto = m_anContexto(iNota)
//}
//
//void lmVStaff::Get GetNumLineas(iPent As Long) As Long
//    wxASSERT(iPent <= m_nNumStaves
//    GetNumLineas = m_nNumLineas(iPent)
//}
//
//void lmVStaff::Get EspaciadoEntrePentagramas() As Single
//    EspaciadoEntrePentagramas = pStaff->GetAfterSpace()
//}
//
//void lmVStaff::Let EspaciadoEntrePentagramas(rDecimas As Single)
//    pStaff->GetAfterSpace() = rDecimas
//}
//
//void lmVStaff::Get EspaciadoAntesParte() As Single
//    EspaciadoAntesParte = m_topMargin
//}
//
//void lmVStaff::Let EspaciadoAntesParte(rDecimas As Single)
//    m_topMargin = rDecimas
//}
//
//void lmVStaff::Get GetCurClave(iPent As Long) As EClefType
//    wxASSERT(iPent <= m_nNumStaves
//    GetCurClave = m_oCurClave(iPent).Valor
//}
//
//void lmVStaff::Get GetNumPentagramas() As Long
//    GetNumPentagramas = m_nNumStaves
//}
//
//void lmVStaff::AddDirectivaRepeticion(nTipo As EDirectivasRepeticion, nNum As Long, _
//        nX As Long, nY As Long, fXAbs As Boolean, fYAbs As Boolean)
//        
//    //construye el staffobj de tipo "repeticion"
//    Dim oRepe As CPORepeticion
//    Set oRepe = new CPORepeticion
//    oRepe.ConstructorRepeticion this, nTipo, nNum, nX, nY, fXAbs, fYAbs
//    m_cStaffObjs.Almacenar oRepe
//
//End Sub
//
//void lmVStaff::AddDirectivaTexto(sTexto As String, _
//        nX As Long, nY As Long, fXAbs As Boolean, fYAbs As Boolean, _
//        Optional sFontName As String = "Arial", _
//        Optional nFontSize As Long = 10, _
//        Optional fBold As Boolean = False, _
//        Optional fItalic As Boolean = False)
//        
//    //construye el staffobj de tipo "texto"
//    Dim oTxt As CPOTexto
//    Set oTxt = new CPOTexto
//    oTxt.ConstructorTexto this, sTexto, nX, nY, fXAbs, fYAbs, sFontName, nFontSize, fBold, fItalic
//    m_cStaffObjs.Almacenar oTxt
//
//End Sub
//
//void lmVStaff::AddIndicacionMetronomo(nTipoNotaIni As EMetronomo, _
//        nTipoNotaFin As EMetronomo, nVelocidad As Long, _
//        Optional fVisible As Boolean = True)
//        
//    Dim oIndicacion As CPOIndicacion
//    Set oIndicacion = new CPOIndicacion
//    oIndicacion.ConstructorIndMetronomo nTipoNotaIni, nTipoNotaFin, nVelocidad, this, fVisible
//    m_cStaffObjs.Almacenar oIndicacion
//
//End Sub
//
//void lmVStaff::Get MetricaInicial() As ETimeSignature
//    MetricaInicial = m_oIniMetrica.Valor
//}
//
lmBarline* lmVStaff::AddBarline(ETipoBarra nType, bool fVisible)
{
    lmBarline* pBarline = new lmBarline(nType, this, fVisible);
    m_cStaffObjs.Store(pBarline);
    
    //Reset contexts for the new measure that starts
    ResetContexts();

    return pBarline;
    
}

void lmVStaff::ResetContexts()
{
    /*
    Verify if current context is just the key signature accidentals. If not,
    create a new context.
    This method is invoked after a barline to reset the context if it was modified 
    by accidentals in notes
    */

    // iterate over the collection of Staves
    lmStaff* pStaff;
    lmContext* pOldContext;
    lmContext* pNewContext;
    wxStaffListNode* pNode = m_cStaves.GetFirst();
    for (; pNode; pNode = pNode->GetNext() ) {
        pStaff = (lmStaff *)pNode->GetData();

         pOldContext = pStaff->GetLastContext();
        pNewContext = new lmContext(pOldContext->GetClef(),
                                    pOldContext->GeyKey(),
                                    pOldContext->GetTime() );
        bool fEqual = true;
        for (int i=0; i < 7; i++) {
            if (pOldContext->GetAccidentals(i) != pNewContext->GetAccidentals(i)) {
                fEqual = false;
                break;
            }
        }
        if (fEqual) {
            //current context is just the key signature accidentals. Continue using it.
            delete pNewContext;
        }
        else {
            //current contex has additional accidentals. Use the new clean one
            pStaff->NewContext(pNewContext);
        }
    }

}

//void lmVStaff::AddGrafObj(nTipo As EGrafObjs, _
//                Optional fVisible As Boolean = True, _
//                Optional nParm1 As Long)
//    
//    Dim oGrafObj As CPOGrafObj
//    Set oGrafObj = new CPOGrafObj
//    oGrafObj.ConstructorGrafObj nTipo, this, fVisible, nParm1
//    m_cStaffObjs.Almacenar oGrafObj
//
//End Sub
//
//void lmVStaff::AddEspacio(Optional nEspacio As Long = 8)
//    AddGrafObj eGO_Espacio, True, nEspacio
//End Sub
//
//void lmVStaff::Get NumParte() As Long
//    //Devuelve el número que hace esta parte (1..n) dentro de los de su instrumento
//    NumParte = m_nStaff
//}
//
//Function GetXPosBarraFinal() As Long
//    //devuelve la posición de la barra del último compas
//    Dim oIT As CIterador, oPo As IPentObj
//    Set oIT = m_cStaffObjs.CreateIterator(eTR_AsStored)
//    oIT.AvanzarAlUltimo
//    Do While oIT.QuedanItems
//        Set oPo = oIT.GetItem
//        if (oPo.Tipo = eTPO_Barline { Exit Do
//        oIT.RetrocederCursor
//    Loop
//    wxASSERT(oPo.Tipo = eTPO_Barline
//    
//    GetXPosBarraFinal = oPo.Right
//    
//}

void lmVStaff::NewLine(lmPaper* pPaper)
{        
    //move x cursor to the left and advance y cursor the space 
    //height of all stafves of this lmVStaff
    pPaper->NewLine(GetVStaffHeight());
    
}

void lmVStaff::DrawProlog(bool fMeasuring, bool fDrawTimekey, lmPaper* pPaper)
{
    /*
    The prolog (clef and key signature) must be rendered on each system,
    but the matching StaffObjs only exist in the first system. Therefore, in the 
    normal staffobj rendering process, the prolog would be rendered only in 
    the first system.
    So, for the other systems it is necessary to force the rendering
    of the prolog because there are no StaffObjs representing it.
    This method does it.
    */

    wxDC* pDC = pPaper->GetDC();
    lmMicrons nPrologWidth = 0;

    wxASSERT(pDC);
    lmClef* pClef = (lmClef*)NULL;
    EClefType nClef = eclvUndefined;
    lmKeySignature* pKey = (lmKeySignature*)NULL;
    //ETimeSignature nTimeKey;

    //@attention when this method is invoked the paper position must be at the left marging,
    //at the start of a new system.
    lmMicrons xStartPos = pPaper->GetCursorX();         //Save x to align all clefs
    lmMicrons yStartPos = pPaper->GetCursorY();
    
    //iterate over the collection of lmStaff objects to draw current cleft and key signature

    wxStaffListNode* pNode = m_cStaves.GetFirst();
    lmStaff* pStaff = (lmStaff*)NULL;
    lmMicrons yOffset = 0;
    lmMicrons xPos=0;
    lmMicrons nWidth=0;

    for (int nStaff=1; pNode; pNode = pNode->GetNext(), nStaff++)
    {
        pStaff = (lmStaff *)pNode->GetData();
        xPos = xStartPos;

        //render clef
        pClef = pStaff->GetCurrentClef();
        if (pClef) {
            nClef = pClef->GetType();
            wxPoint pos = wxPoint(xPos, yStartPos+yOffset);        //absolute position
            nWidth = pClef->DrawAt(fMeasuring, pDC, pos);
            xPos += nWidth;
        }
        
        //render key signature
        pKey = pStaff->GetCurrentKey();
        if (pKey) {
            wxASSERT(nClef != eclvUndefined);
            wxPoint pos = wxPoint(xPos, yStartPos+yOffset);        //absolute position
            nWidth = pKey->DrawAt(fMeasuring, pDC, pos, nClef, nStaff);
            xPos += nWidth;
//        
//        //if requested (flag fDrawTimekey), render time key (only on first staff)
//        if (fDrawTimekey And iStf = 1 {
//            if (Not m_oCurMetrica Is Nothing {
//                nTimeKey = m_oCurMetrica.Valor
//                pPaper->PintarMetrica fMeasuring, nTimeKey, yDesplz
//            }
//        }
        }

        //compute prolog width
        nPrologWidth = wxMax(nPrologWidth, xPos - xStartPos);
        
        //compute vertical displacement for next staff
        yOffset += pStaff->GetHeight();
        yOffset += pStaff->GetAfterSpace();
            
    }

    // update paper cursor position
    pPaper->SetCursorX(xStartPos + nPrologWidth);
    
}

lmSoundManager* lmVStaff::ComputeMidiEvents(int nChannel)
{
    /*
    nChannel is the MIDI channel to use for all events of this lmVStaff.
    Returns the lmSoundManager object. It is not retained by the lmVStaff, so it is caller
    responsibility to delete it when no longer needed.
    */

    //! @todo review this commented code
//    Dim nMetrica As ETimeSignature, nDurCompas As Long, nTiempoIni As Long
//    
//    nMetrica = this.MetricaInicial
//    nDurCompas = GetDuracionMetrica(nMetrica)
//    nTiempoIni = nDurCompas - this.DuracionCompas(1)
//    
//    wxASSERT(nTiempoIni >= 0        //El compas tiene más notas de las que caben
//    
//    //Si el primer compas no es anacrusa, retrasa todo un compas para que
//    //se marque un compas completo antes de comenzar
//    if (nTiempoIni = 0 { nTiempoIni = nDurCompas
    
    //Create lmSoundManager and initialize MIDI events table
    lmSoundManager* pSM = new lmSoundManager();
    //! @todo review next line
//    pSM->Inicializar GetStaffsCompas(nMetrica), nTiempoIni, nDurCompas, this.NumCompases

    //iterate over the collection to create the MIDI events
    float rMeasureStartTime = 0;
    int nMeasure = 1;        //to count measures (1 based, normal musicians way)
    pSM->StoreMeasureStartTime(nMeasure, rMeasureStartTime);

    //iterate over the collection to create the MIDI events
    lmStaffObj* pSO;
    lmNoteRest* pNR;
    lmTimeSignature* pTS;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    while(!pIter->EndOfList()) {
        pSO = pIter->GetCurrent();
        if (pSO->GetType() == eTPO_NoteRest) {
            pNR = (lmNoteRest*)pSO;
            pNR->AddMidiEvents(pSM, rMeasureStartTime, nChannel, nMeasure);
        }
        else if (pSO->GetType() == eTPO_Barline) {
            rMeasureStartTime += pSO->GetTimePos();        //add measure duration
            nMeasure++;
            pSM->StoreMeasureStartTime(nMeasure, rMeasureStartTime);
            //! @todo If time signature changes add a RithmChange event
        }
        else if (pSO->GetType() == eTPO_TimeSignature) {
            //add a RithmChange event to set up tempo (num beats, duration of a beat)
            pTS = (lmTimeSignature*)pSO;
            pTS->AddMidiEvent(pSM, rMeasureStartTime, nMeasure);
        }
        pIter->MoveNext();
    }
    delete pIter;

    return pSM;
    
}

float lmVStaff::GetTotalDuration()
{
    //iterate over the stafobjs collection to accumulate measures duration
    lmStaffObj* pSO;
    lmNoteRest* pNR;
    lmTimeSignature* pTS;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    float rTime = 0.;
    while(!pIter->EndOfList()) {
        pSO = pIter->GetCurrent();
        if (pSO->GetType() == eTPO_Barline) {
            rTime += pSO->GetTimePos();        //add measure duration
        }
        pIter->MoveNext();
    }
    delete pIter;
    return rTime;
    
}

lmNote* lmVStaff::FindPossibleStartOfTie(lmPitch nMidiPitch, int nStep)
{
    /*
    This method is invoked from lmNote constructor to find if the note being created 
    (the "target note") is tied to a previous one ("the candidate" one). 
    This method explores backwards to try to find a note that can be tied with the received 
    as parameter (the "target note").

    Algorithm:
    Find the first previous note of the same pitch and step, in this measure or 
    in the previous one

    */

    //define a backwards iterator
    bool fInPrevMeasure = false;
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmNoteRest* pNR = (lmNoteRest*)NULL;
    lmNote* pNote = (lmNote*)NULL;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_AsStored);
    pIter->MoveLast();
    while(!pIter->EndOfList())
    {
        pSO = pIter->GetCurrent();
        switch (pSO->GetType()) {
            case eTPO_NoteRest:
                pNR = (lmNoteRest*)pSO;
                if (!pNR->IsRest()) {
                    pNote = (lmNote*)pSO;
                    if (pNote->CanBeTied(nMidiPitch, nStep)) {
                        delete pIter;
                        return pNote;    // candidate found
                    }
                }
                break;

            case eTPO_Barline:
                if (fInPrevMeasure) {
                    delete pIter;
                    return (lmNote*)NULL;        // no suitable note found
                }
                fInPrevMeasure = true;
                break;

            default:
                ;
        }
        pIter->MovePrev();
    }
    delete pIter;
    return (lmNote*)NULL;        //no suitable note found

}


//Function RepositionBar(iBar As Long, nShift As Long, nBarLeft As Long, _
//            nNewBarWidth As Long) As Long
//    //Shift the position of all StaffObjs in bar number iBar the amount given by parameter nShift.
//    //In addition, the position of the barline at the end of this bar is also shifted so that
//    //the new width on the bar becames nNewBarWidth. The shift amount applied to the barline
//    //is retuned as the result of this function.
//    //
//    //Parameters:
//    //iBar - the number of this bar (absolute, that is, from the begining of the score)
//    //nShift - the shift amount to apply to all StaffObjs of bar number iBar
//    //nBarLeft - the new left position for the start of this bar
//    //nNewBarWidth - the new width that this bar will have.
//    
//    
//    wxASSERT(iBar <= this.NumCompases
//    
//    if (nNewBarWidth = 0 { Exit Function
//
//    Dim i As Long, nBarlineShift As Long
//    Dim oPo As IPentObj
//    
//    //get staffobj on which the bar starts
//    Dim oIT As CIterador
//    Set oIT = m_cStaffObjs.CreateIterator(eTR_OptimizeAccess)
//    oIT.AdvanceToMeasure iBar
//    Do While oIT.QuedanItems
//        Set oPo = oIT.GetItem
//        
//        //if this staffobj is the barline, shift it and exit
//        if (oPo.Tipo = eTPO_Barline {
//            nBarlineShift = (nBarLeft + nNewBarWidth) - oPo.Left
//            oPo.Left = nBarLeft + nNewBarWidth - (oPo.Right - oPo.Left - 1)
//            Exit Do
//        }
//        
//        //otherwise shift the staffobj
//        oPo.Left = oPo.Left + nShift
//            
//        oIT.AdvanceCursor
//    Loop
//    RepositionBar = nBarlineShift
//    
//}
//
////Desplaza la barra de fin de compas para que quede en la posición nLeft.
////Devuelve el desplazamiento aplicado a la barra
//Function SetAnchoCompas(iCompas As Long, nLeft As Long) As Long
//    
//    wxASSERT(iCompas <= this.NumCompases
//
//    Dim i As Long
//    Dim oPo As IPentObj
//    
//    //localizar el fin del compas
//    Dim oIT As CIterador
//    Set oIT = m_cStaffObjs.CreateIterator(eTR_OptimizeAccess)
//    oIT.AdvanceToMeasure iCompas
//    Do While oIT.QuedanItems
//        Set oPo = oIT.GetItem
//        if (oPo.Tipo = eTPO_Barline { Exit Do
//        oIT.AdvanceCursor
//    Loop
//    
//    i = nLeft - oPo.Left    //desplazamiento a aplicar
//    oPo.Left = nLeft - (oPo.Right - oPo.Left - 1)
//    SetAnchoCompas = i
//
//}
//
////Devuelve la duración del compas
////Al llevarse ahora una marca de tiempo, la duración del compas viene dada por la marca de
////tiempo de la barra de fin de compas, lo que simplifica el tratamiento
//Function DuracionCompas(iCompas As Long) As Long
//    wxASSERT(iCompas <= this.NumCompases
//    
//    //Algoritmo: Localizar la barra de fin del compas y devolver su marca de tiempo
//
//    Dim oPo As IPentObj
//    
//    //bucle hasta el fin del compas
//    Dim oIT As CIterador
//    Set oIT = m_cStaffObjs.CreateIterator(eTR_OptimizeAccess)
//    oIT.AdvanceToMeasure iCompas
//    Do While oIT.QuedanItems
//        Set oPo = oIT.GetItem
//        if (oPo.Tipo = eTPO_Barline { Exit Do
//        oIT.AdvanceCursor
//    Loop
//    
//    wxASSERT(oPo.Tipo = eTPO_Barline      //no pueden existir compases que no acaben en barra
//    DuracionCompas = oPo.TimePos
//
//}

void lmVStaff::ShiftTime(float rTimeShift)
{
    /*
    Shifts the time counter and inserts a control lmStaffObj to signal the shift event
    This is necessary for rendering (to start a new thread) and for exporting the score:
     - in LDP, to create an element AVANCE/RETROCESO
     - in MusicXML: to create an element FORWARD/BACKWARD

    */

    //shift time counters
    m_cStaffObjs.ShiftTime(rTimeShift);

    //Insert a control object to signal the shift event so that we can start a 
    //new thread at rendering
    lmSOControl* pControl = new lmSOControl(this, rTimeShift);
    m_cStaffObjs.Store(pControl);
    
}

//=========================================================================================
// Friend methods to be used only by lmFormatter objects
//=========================================================================================

//allow IViewer to navigate through the StaffObjs collection
lmStaffObjIterator* lmVStaff::CreateIterator(ETraversingOrder nOrder)
{
    return m_cStaffObjs.CreateIterator(nOrder);
}

////el posicionamiento relativo de objetos requiere conocer la
////posición de inicio del compas. Para ello, las funciones de dibujo lo guardan
//// aqui, de forma que el método GetXInicioCompas pueda devolver este valor
//Friend void lmVStaff::Let SetXInicioCompas(xPos As Long)
//    m_xInicioCompas = xPos
//}
//
//// End of friend methods only for IViewer =================================================
//
//
////==============================================================================================
//// Funciones que añaden StaffObjs al final del pentagrama a partir de un fuente.
////==============================================================================================
//
//
////sElemento es un elemento <Compas> :
////v1.0  <Compas> ::= ("C" [<Num>] {<Figura> | <Grupo>}* )
//void lmVStaff::CrearCompas(ByVal sElemento As String)
//
//    Dim oRaiz As CNodo
//    Set oRaiz = AnalizarFuente(sElemento)
//    AnalizarCompas oRaiz, this
//        
//End Sub

//Devuelve una referencia al objeto CPONota creado.
//lmNoteRest* lmVStaff::AddNoteRestFromSource(wxString sText)
//{
//    lmLDPParser parserLDP;
//    lmLDPNode* pNode = parserLDP.ParseText(sText);
//
//    if (pNode->GetName() == _T("N")) {
//        pNR = parserLDP.AnalyzeNote(pNode, this);
//    } else {
//        pNR = parserLDP.AnalyzeRest(pNode, this);
//    }
//
//    return pNR;
//        
//}
