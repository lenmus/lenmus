// RCS-ID: $Id: NoteRest.cpp,v 1.6 2006/02/23 19:23:54 cecilios Exp $
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
/*! @file NoteRest.cpp
    @brief Implementation file for class lmNoteRest
    @ingroup score_kernel
*/
//--------------------------------------------------------------------------------------------------
/*! @class lmNoteRest
    @ingroup score_kernel
    @brief Object lmNoteRest represents a note or a silence

    Si consideramos los silencios como un tipo de figura y los acordes como una generalizaci�n de
    una nota, obtenemos una entidad conceptual m�s general que engloba a todos ellos. Como en la
    notaci�n m�sical tradicional no existe este concepto, le llamo "FigSil" (Figura/Silencio):
        - Un silencio es un lmNoteRest sin notas
        - Una nota es un lmNoteRest con s�lo una nota
        - Un acorde es un lmNoteRest con varias notas

    Por c�mo ha ido evolucionando el programa, no se ha definido un objeto abstracto (FigSil) y
    tres objetos concretos (Nota, Acorde y Silencio) sino que el objeto lmNoteRest modela todo.

    Se han definido los m�todos .EsSilencio y .InChord para identificar, respectivamente, si
    una lmNoteRest modeliza un silencio y si una lmNoteRest es una nota que forma parte de un acorde. Si
    el resultado de ambas es False, indica que es una nota simple.

    Existe un objeto auxiliar (CAcorde) cuya funci�n es englobar algunas funcionalidaes propias
    de los acordes. Pero el acorde no es m�s que un conjunto de CNotas, identificadas como
    que forman un acorde.

*/
//THINK:
//   Grupos y acordes son entidades que agrupan varias notas. En algunos casos no interesan
//   las propiedades de cada nota sino las del conjunto. Por ejemplo, la duraci�n de un
//   acorde es la de una de sus notas. �Metodos globales que contemplen esta problem�tica?
//
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
#include "wx/debug.h"
#include "../ldp_parser/AuxString.h"
#include <math.h>

//implementation of the NoteRests List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(NoteRestsList);



//====================================================================================================
//Constructors and destructors
//====================================================================================================
lmNoteRest::lmNoteRest(lmVStaff* pVStaff, bool IsRest, ENoteType nNoteType, float rDuration,
                   bool fDotted, bool fDoubleDotted, wxInt32 nStaff) :
    lmCompositeObj(eTPO_NoteRest, pVStaff, nStaff, true, sbDRAGGABLE)
{
    // initialize all atributes
    m_fCalderon = false;
    m_nNoteType = nNoteType;
    m_fDotted = fDotted;
    m_fDoubleDotted = fDoubleDotted;
    m_nPentagrama = nStaff;
    m_fIsRest = IsRest;

    m_pNotations = (AuxObjsList*)NULL;
    m_pLyrics = (AuxObjsList*)NULL;

    m_rDuration = rDuration;
    m_pTupletBracket = (lmTupletBracket*)NULL;
    
    // default beaming information: not beamed
    m_fBeamed = false;
    m_pBeam = (lmBeam*)NULL;
   
}

lmNoteRest::~lmNoteRest()
{
    //Remove the associated AuxObjs
    if (m_pNotations) {
        m_pNotations->DeleteContents(true);
        m_pNotations->Clear();
        delete m_pNotations;
        m_pNotations = (AuxObjsList*)NULL;
    }
    if (m_pLyrics) {
        m_pLyrics->DeleteContents(true);
        m_pLyrics->Clear();
        delete m_pLyrics;
        m_pLyrics = (AuxObjsList*)NULL;
    }

    //remove the note/rest from the lmTupletBracket and if lmTupletBracket is empty delete it
    if (m_pTupletBracket) {
        m_pTupletBracket->Remove(this);
        if (m_pTupletBracket->NumNotes() == 0) {
            delete m_pTupletBracket;
            m_pTupletBracket = (lmTupletBracket*)NULL;
        }
    }

}

void lmNoteRest::CreateBeam(bool fBeamed, lmTBeamInfo BeamInfo[])
{
    // Set up beaming information

    m_fBeamed = fBeamed;
    if (!m_fBeamed) {
        m_pBeam = (lmBeam*)NULL;
    }
    else {
        for (int i=0; i < 6; i++) {
            m_BeamInfo[i] = BeamInfo[i];
        }
        if (m_BeamInfo[0].Type == eBeamBegin) {
            m_pBeam = new lmBeam(g_pLastNoteRest);
            g_pCurBeam = m_pBeam;
            m_pBeam->Include(this);
        } else {
            m_pBeam = g_pCurBeam;
            if (!m_pBeam) {
                //! @todo Show message. Error: �se pide finalizar un grupo que no ha sido abierto!
                fBeamed = false;
            } else {
                if (IsRest())
                    m_pBeam->Include(this);
                else {
                    if (!IsInChord()) m_pBeam->Include(this);
                }
                if (m_BeamInfo[0].Type == eBeamEnd) {
                        //m_pBeam->ComputeStems();
                    //@attention with this note/rest the beaming ends. But it si not yet posible to
                    //compute beaming information as ther could remain notes to add in
                    //chord to this note. Due to this, the computation of stems has
                    //been delayed to the measuremen phase of the first note of the beam.
                    g_pCurBeam = (lmBeam*)NULL;        // no beam open
                }
            }
        }
    }
}

//====================================================================================================
// methods related to associated AuxObjs management
//====================================================================================================
void lmNoteRest::AddFermata(bool fOverNote)
{
    if (!m_pNotations) m_pNotations = new AuxObjsList();

    lmFermata* pFermata = new lmFermata(this, fOverNote);
    m_pNotations->Append(pFermata);

}

void lmNoteRest::AddLyric(lmLyric* pLyric)
{
    if (!m_pLyrics) m_pLyrics = new AuxObjsList();

    pLyric->SetOwner(this);
    m_pLyrics->Append(pLyric);

}
//====================================================================================================
// lmNote values parsing methods
//====================================================================================================


//---------------------------------------------------------------------------------------------
// Receives a string (sDato) with the letter for the type of note and, optionally, dots "."
// Set up variables m_nNoteType and flags m_fDotted and m_fDoubleDotted.
//
//                cuadrada    d   0
//                redonda     r   1
//   half        blanca      b   2
//   quarter    negra       n   3
//   eighth        corchea     c   4
//   sixteenth    semicorchea s   5
//                fusa        f   6
//                semifusa    m   7
//
// Returns true if error in parsing
//---------------------------------------------------------------------------------------------
bool lmNoteRest::ParseTipoNota(wxString& sDato)
{
    //normalize
    sDato.Trim(false);        //remove spaces from left
    sDato.Trim(true);        //and from the right
    sDato.MakeLower();        
    
    int nNoteType = LDPNoteTypeToEnumNoteType(sDato);
    if (nNoteType == -1) return true;    //error
    m_nNoteType = (ENoteType)nNoteType;

    //analyze dots
    if (sDato.Len() > 1) {
        sDato = sDato.Mid(1);
        if (sDato.StartsWith( _T("..") )) {
            m_fDoubleDotted = true;
        } else if (sDato.StartsWith( _T(".") )) {
            m_fDotted = true;
        } else {
            return true;    //error
        }
    }
            
    return false;   //no error
    
}

//void lmNoteRest::Let IPentObj_Left(nLeft As Long)
//    Dim nDsplz As Long
//    nDsplz = nLeft - m_rLeft
//    m_rLeft = nLeft
//    m_xPos = m_xPos + nDsplz
//    m_xStem = m_xStem + nDsplz
//    m_rRight = m_rRight + nDsplz
//    
//}
//
//void lmNoteRest::Get IPentObj_Anchor() As Single
//    IPentObj_Anchor = m_xAnchor - m_rLeft
//}
//
////=========================================================================================
////Uso exclusivo por CBeam para transferencia de datos y dibujo de barras de corchete
////=========================================================================================
//
//Friend void lmNoteRest::Let PosSilencio(nPos As Long)
//    //Llamada s�lo desde CBeam para alinear los silencios dentro de un grupo
//    m_yPosSilencio = nPos
//}
//
//void lmNoteRest::Get Duracion() As Long
//    //Esta funci�n no tiene en cuenta el efecto de agrupaci�n en tupla
//    Duracion = 2 ^ (9 - m_nNoteType)
//    if (m_fDotted) { Duracion = (Duracion * 3) \ 2       //x 1.5
//    if (m_fDoubleDotted) { Duracion = (Duracion * 7) \ 4  //x 1.75
//
//}

////================================================================================================
//// Funciones para manejar acordes
////================================================================================================
//void lmNoteRest::Get DuracionEnAcorde() As Long
//    //Si la nota no pertenece a un acorde devuelve la duraci�n de la nota.
//    //Si pertenece a un acorde devuelve la duraci�n del acorde si esta es la nota de comienzo
//    //o cero si esta es una de las restantes del acorde.
//    //Este comportamiento permite llamar a Duraci�nAcorde en vez de a Duraci�n y conseguir
//    //un tratamiento uniforme
//    if (this.InChord) {
//        if (m_fNotaBase) {
//            DuracionEnAcorde = m_oAcorde.Duracion
//        } else {
//            DuracionEnAcorde = 0
//        }
//    } else {
//        //nota suelta no en acorde
//        DuracionEnAcorde = this.Duracion
//    }
//
//}
//
////================================================================================================
//// Funciones para manejar grupos. Empaquetan el acceso al beam asociado
////================================================================================================
//
//void lmNoteRest::Get DuracionAgrupada() As Long
//    //Devuelve la duraci�n total del grupo que comienza en esta nota, o la de esta nota si
//    //no es comienzo de grupo
//    if (m_nBeamGrouping = etaInicioGrupo) {
//        DuracionAgrupada = m_oBeam.Duracion
//    } else {
//        DuracionAgrupada = this.DuracionEnAcorde
//    }
//
//}
//
//Function NumNotes() As Long
//    //Devuelve el n�mero de NoteRests que componen el grupo que empieza en esta nota o cero si
//    //esta nota no es comienzo de grupo
//    if (m_nBeamGrouping = etaInicioGrupo) {
//        NumNotes = m_oBeam.NumNotes
//    } else {
//        NumNotes = 0
//    }
//
//}
//
////================================================================================================
//
//Friend void lmNoteRest::Get notaMidi() As Integer
//    Dim nResto As Long
//    nResto = m_nPitch Mod 7
//    notaMidi = PitchToMidi(m_nPitch)
//
//    //modifica la altura de la nota en funci�n del contexto
//    if (nResto = 0) {  //nota Si
//        nResto = 6
//    } else {
//        nResto = nResto - 1
//    }
//    notaMidi = notaMidi + m_anContexto(nResto)
//   
//    //modifica la nota en funci�n de las alteraciones propias
//    if (m_nAlteraciones = eDoubleSharp) {
//        if (m_anContexto(nResto) = 1) {
//            notaMidi = notaMidi + 1
//        } else {if (m_anContexto(nResto) = 0) {
//            notaMidi = notaMidi + 2
//        } else {
//            notaMidi = notaMidi - m_anContexto(nResto) + 2
//        }
//    } else {if (m_nAlteraciones = eSharp Or m_nAlteraciones = eNaturalSharp) {
//        notaMidi = notaMidi - m_anContexto(nResto) + 1
//    } else {if (m_nAlteraciones = eNatural) {
//        if (m_anContexto(nResto) > 0) {
//                notaMidi = notaMidi - 1
//        } else {if (m_anContexto(nResto) < 0) {
//                notaMidi = notaMidi + 1
//        }
//    } else {if (m_nAlteraciones = eFlatFlat) {
//        if (m_anContexto(nResto) = -1) {
//            notaMidi = notaMidi - 1
//        } else {if (m_anContexto(nResto) = 0) {
//            notaMidi = notaMidi - 2
//        } else {
//            notaMidi = notaMidi - m_anContexto(nResto) - 2
//        }
//    } else {if (m_nAlteraciones = eFlat Or m_nAlteraciones = eNaturalFlat) {
//        notaMidi = notaMidi - m_anContexto(nResto) - 1
//    }
//    
//}

void lmNoteRest::AddMidiEvents(lmSoundManager* pSM, float rMeasureStartTime, int nChannel,
                             int nMeasure)
{
    /*
    Coumpute MIDI events for this lmNoteRest and add them to the lmSoundManager object received
    as parameter.
    */

    //Generate Note ON event
    float rTime = m_rTimePos + rMeasureStartTime;
    if (IsRest()) {
        //Generate only event for visual highlight
        pSM->StoreEvent( rTime, eSET_VisualON, nChannel, 0, 0, this, nMeasure);
    }
    else {
        //It is a note. Generate Note On event
        lmNote* pN = (lmNote*)this;
        if (!pN->IsTiedToPrev()) {
            //It is not tied to the previous one. Generate NoteOn event to start the sound and
            //highlight the note
            pSM->StoreEvent(rTime, eSET_NoteON, nChannel, pN->GetMidiPitch(), pN->GetStep(),
                            this, nMeasure);
        }
        else {
            //This note is tied to the previous one. Generate only a VisualOn event as the
            //sound is already started by the previous note.
            pSM->StoreEvent(rTime, eSET_VisualON, nChannel, pN->GetMidiPitch(), pN->GetStep(),
                            this, nMeasure);
        }
    }
    
    //generate NoteOff event
    rTime += GetDuration();
    if (IsRest()) {
        //Is a rest. Genera only a VisualOff event
        pSM->StoreEvent(rTime, eSET_VisualOFF, nChannel, 0, 0, this, nMeasure);
    }
    else {
        //It is a note
        lmNote* pN = (lmNote*)this;
        if (!pN->IsTiedToNext()) {
            //It is not tied to next note. Generate NoteOff event to stop the sound and
            //un-highlight the note
            pSM->StoreEvent(rTime, eSET_NoteOFF, nChannel, pN->GetMidiPitch(), pN->GetStep(),
                            this, nMeasure);
        }
        else {
            //This note is tied to the next one. Generate only a VisualOff event so that
            //the note will be un-highlighted but the sound will not be stopped.
            pSM->StoreEvent(rTime, eSET_VisualOFF, nChannel, pN->GetMidiPitch(), pN->GetStep(),
                            this, nMeasure);
        }
    }
    
}

wxString lmNoteRest::GetLDPNoteType()
{
    switch(m_nNoteType) {
        case eLong:
            return _T("D");
        case eWhole:
            return _T("R");
        case eHalf:
            return _T("B");
        case eQuarter:
            return _T("N");
        case eEighth:
            return _T("C");
        case e16th:
            return _T("S");
        case e32th:
            return _T("F");
        case e64th:
            return _T("M");
        case e128th:
            return _T("G");
        case e256th:
            return _T("P");
        default:
            wxASSERT(false);
            return _T("");        //compiler happy
    }

}


//void lmNoteRest::Redibujar(nColor As Long)
//    m_oPapel.color = nColor
//    m_oPapel.xPos = m_xPos
//    m_oPapel.yBase = m_yPos
//    Dibujar false
//    m_oPapel.color = colorNegro
//    
//}
//
//void lmNoteRest::Dibujar(fModoMedida As Boolean)
//    //Esta funci�n es llamada s�lo desde lmNoteRest.IPentObj_Dibujate y desde lmNoteRest.Redibujar
//    
//    //guarda los datos para "Redibujar"
//    m_xPos = m_oPapel.xPos
//    m_yPos = m_oPapel.yBase
//    
//    if (m_nPitch != 0) {
//        DibujarNota fModoMedida
//    } else {
//        DibujarSilencio fModoMedida
//    }
//    
//    //si es la �ltima nota de un acorde y son notas con plica dibuja la plica del acorde
//    if (this.InChord) {
//        if (m_oAcorde.EsUltimaNota(this) And m_nNoteType >= eHalf) {
//            m_oAcorde.DibujarStem fModoMedida, m_oPapel
//        }
//    }
//
//    //Si es la �ltima nota de un grupo dibuja los corchetes del grupo
//    if (m_nBeamGrouping = etaFinGrupo) { m_oBeam.DibujarCorchetes fModoMedida, m_oPapel, m_fStemDown
//    
//}
//
//

/*! Receives a string (sNoteType) with the LDP letter for the type of note and, optionally,
    dots ".". It is assumed the source is normalized (no spaces, lower case)
    @returns the enum value that corresponds to this note type, or -1 if error
*/
int LDPNoteTypeToEnumNoteType(wxString sNoteType)
{
    wxChar cNoteType = sNoteType.GetChar(0);
    switch (cNoteType) {
        case _T('d'):
            return  eLong;
        case _T('r'):
            return  eWhole;
        case _T('b'):
            return  eHalf;
        case _T('n'):
            return  eQuarter;
        case _T('c'):
            return  eEighth;
        case _T('s'):
            return  e16th;
        case _T('f'):
            return  e32th;
        case _T('m'):
            return  e64th;
        case _T('g'):
            return  e128th;
        case _T('p'):
            return  e256th;
        default:
            return  -1;     //error
    }
}

/*! Receives a string (sNoteType) with the LDP letter for the type of note and, optionally,
    dots ".". It is assumed the source is normalized (no spaces, lower case)
    @returns the duration or -1.0 if error
*/
float LDPNoteTypeToDuration(wxString sNoteType)
{
    int nNoteType = LDPNoteTypeToEnumNoteType(sNoteType);
    if (nNoteType == -1) return -1.0;    //error

    //analyze dots
    bool fDotted=false, fDoubleDotted=false;
    if (sNoteType.Len() > 1) {
        if (sNoteType.Mid(1) == _T("..") ) {
            fDoubleDotted = true;
        } else if (sNoteType.Mid(1) ==  _T(".") ) {
            fDotted = true;
        } else {
            return -1.0;    //error
        }
    }

    return NoteTypeToDuration((ENoteType)nNoteType, fDotted, fDoubleDotted);
}

float NoteTypeToDuration(ENoteType nNoteType, bool fDotted, bool fDoubleDotted)
{
    //compute duration without modifiers
    float rDuration = pow(2, (9 - nNoteType));
    
   //take dots into account
    if (fDotted) { rDuration *= 1.5; }
    if (fDoubleDotted) { rDuration *= 1.75; }

    return rDuration;

}
