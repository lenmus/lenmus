// RCS-ID: $Id: AuxString.cpp,v 1.10 2006/02/28 17:39:56 cecilios Exp $
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
/*! @file AuxString.cpp
    @brief Implementation file for auxiliary global functions to verify / convert strings
    @ingroup ldp_parser
*/
#ifdef __GNUG__
// #pragma implementation
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/score.h"
#include "AuxString.h"
#include "../auxmusic/Conversion.h"

//----------------------------------------------------------------------------------------
/*! @page note_names        lmNote names and pitch values

@verbatim
 nPitchMIDI:    chromatic pitch: number representing the note in a chromatic scale. It is
                the same as the number used in MIDI for representing pitch.
 nPitch:        diatonic pitch: number representing the note in a diatonic scale
 nOctave:        octave number (as MIDI). The lowest scale (-1) is not yet used

 a pitch value of 0 (zero) represents a rest.

                                AltDiatonica                
  name      Octave      PitchMIDI    Pitch      Observations        
  ----      ------      ---------    -------    -----------------------------------
  rest      any         0            0          Any rest
  (c-1)    -1           1            
  c0        0           12           1          Do2 de la subcontraoctava (16,35 Hz)
  d0        0           14           2            
  e0        0           16           3            
  f0        0           17           4            
  g0        0           19           5            
  a0        0           21           6            
  b0        0           23           7            
  c1        1           24           8          Do1 de la contraoctava
  c2        2           36           15         Do de la gran octava
  c3        3           48           22         do de la pequeña octava
  c4        4           60           29         do1 de la octava primera (la1 = 440Hz)
  d4        4           62           30            
  e4        4           64           31            
  f4        4           65           32            
  ...
  c5        5           72           36         do2 de la octava segunda
  c6        6           84           43         do3 de la octava tercera
  c7        7           96           50         do4 de la octava cuarta
  c8        8           108          57         do5 de la octava quinta (4.186 Hz)
  etc.

  Accidentals are represented by minus (-) and plus (+) signs before the note name
       -  : flat
       +  : sharp
       -- : double flat
       ++ : double sharp
       =  : natural
       =- : natural-flat
       =+ : natural-sharp
  i.e.:  ++c3, =+c3, +c3, =c3, -c3, --c3, =-c3

  Pitch name can also be a number, representing MIDI pitch. i.e.: (n 29 n) = (n c4 n)
@endverbatim
*/
//-------------------------------------------------------------------------------------------*/

/*! @brief    Convert LDP pitch name to pitch and accidentals

    Convert LDP pitch name to pitch and accidentals.
    @return    Returns true if error (sPitch is not a valid pitch name)
            Otherwise, stores the corresponding data into the parameters pPitch, and 
            pAccidentals.

    @param[in]  sPitch    The LDP string with the pitch to convert. Pitch is represented 
                as the combination of the chromatic alteration, the step in the
                diatonic scale, and the octave (i.e. "+c4").
    @param[out] pPitch    Diatonic MIDI pitch.
    @param[out] pAccidentals parameter represents chromatic accidentals (does not 
                include key signature accidentals)
*/
bool PitchNameToData(wxString sPitch, int* pPitch, EAccidentals* pAccidentals)
{

    //It is assumed that sPitch is Trimed (no spaces before or after real data) and lower case
    bool fError = false;
    
    //if sPitch is a number it is interpreted as a MIDI pitch
    if (sPitch.IsNumber()) {
        long nAux = 0;
        fError = !sPitch.ToLong(&nAux);   
        wxASSERT(!fError);
        *pPitch = (int)nAux;
        *pAccidentals = eNoAccidentals; 
        /*! @todo
            analizar la nota MIDI y determinar las
            alteraciones en función de la armadura
        */
        return false;
    }
    
    //sPitch is alfanumeric: must be letter followed by a number (i.e.: "c4" )
    wxString sAlter;
    
    //split the string: accidentals and name
    switch (sPitch.Len()) {
        case 2: 
            sAlter = _T("");
            break;
        case 3:
            sAlter = sPitch.Mid(0, 1);
            sPitch = sPitch.Mid(1, 2);
            break;
        case 4:
            sAlter = sPitch.Mid(0, 2);
            sPitch = sPitch.Mid(2, 2);
            break;
        default:
            return true;   //error
    }

    wxString sStep = sPitch.Left(1);
    wxString sOctave = sPitch.Mid(1, 1);
    fError = StringToPitch(sStep, sOctave, pPitch);
    if (fError) return true;
    
    //analyse accidentals
    if (sAlter.IsEmpty()) {
        *pAccidentals = eNoAccidentals;
    } else if (sAlter.StartsWith( _T("+") )) {
        if (sAlter.StartsWith( _T("++") )) {
            *pAccidentals = eDoubleSharp;
        } else {
            *pAccidentals = eSharp;
        }
    } else if (sAlter.StartsWith( _T("-") )) {
        if (sAlter.StartsWith( _T("--") )) {
            *pAccidentals = eFlatFlat;
        } else {
            *pAccidentals = eFlat;
        }
    } else if (sAlter.StartsWith( _T("=+") )) {
        *pAccidentals = eNaturalSharp;
    } else if (sAlter.StartsWith( _T("=-") )) {
        *pAccidentals = eNaturalFlat;
    } else if (sAlter.StartsWith( _T("=") )) {
        *pAccidentals = eNatural;
    } else {
        return true;  //error
    }
    
    return false;  //no error

}

bool StringToPitch(wxString sStep, wxString sOctave, int* pPitch)
{
    /*
     sStep is a one char string: "a" to "g", in lower case
     sOctave is a one char string: "0" to "9"

     returns true if error (sStep is not "a"-"g" or sOctave not "0"-"9")
     Otherwise, stores the corresponding data into the parameters nPitch.

    */

    //analyze the letter and store it as diatonic pitch step
    *pPitch = LetterToStep(sStep) + 1;
    if (*pPitch == 0) return true;   //error
    
    //combine octave with pitch step
    if (! sOctave.IsNumber()) {
        return true;   //error
    }

    long nOctave;
    bool fError = !sOctave.ToLong(&nOctave);
    wxASSERT(!fError);
    *pPitch = *pPitch + 7 * nOctave;

    return (nOctave < 0 || nOctave > 9);        //true if error

}

//---------------------------------------------------------------------------------------------
// Receives a string (sNoteType) with the LDP letter for the type of note and, optionally,
// dots "."
// Set up variables nNoteType and flags fDotted and fDoubleDotted.
//
//  USA          UK                     ESP              LDP     NoteType
//  -----------  --------------------   -------------    ---     ---------
//  Long         Breve                  cuadrada         d       eLong = 0
//  Whole        Semibreve              redonda          r       eWhole = 1
//  half         minim                  blanca           b       eHalf = 2
//  quarter      crochet                negra            n       eQuarter = 3
//  eighth       quaver                 corchea          c       eEighth = 4
//  sixteenth    semiquaver             semicorchea      s       e16th = 5
//  32nd         demisemiquaver         fusa             f       e32th = 6
//  64th         hemidemisemiquaver     semifusa         m       e64th = 7
//  128th                               garrapatea       g       e128th = 8
//  256th                               semigarrapatea   p       e256th = 9
//
// Returns true if error in parsing
//---------------------------------------------------------------------------------------------
bool NoteTypeToData(wxString sNoteType, ENoteType* pnNoteType, bool* pfDotted, bool* pfDoubleDotted)
{
    sNoteType.Trim(false);        //remove spaces from left
    sNoteType.Trim(true);        //and from the right
    
    switch (sNoteType.GetChar(0)) {
        case _T('d'):
            *pnNoteType = eLong;
            break;
        case _T('r'):
            *pnNoteType = eWhole;
            break;
        case _T('b'):
            *pnNoteType = eHalf;
            break;
        case _T('n'):
            *pnNoteType = eQuarter;
            break;
        case _T('c'):
            *pnNoteType = eEighth;
            break;
        case _T('s'):
            *pnNoteType = e16th;
            break;
        case _T('f'):
            *pnNoteType = e32th;
            break;
        case _T('m'):
            *pnNoteType = e64th;
            break;
        case _T('g'):
            *pnNoteType = e128th;
            break;
        case _T('p'):
            *pnNoteType = e256th;
            break;
        default:
            return true;    //error
    }
    
    //analyze dots
    *pfDotted = false;
    *pfDoubleDotted = false;
    if (sNoteType.Len() > 1) {
        sNoteType = sNoteType.Mid(1);
        if (sNoteType.StartsWith( _T("..") )) {
            *pfDoubleDotted = true;
        } else if (sNoteType.StartsWith( _T(".") )) {
            *pfDotted = true;
        } else {
            return true;    //error
        }
    }
            
    return false;   //no error
    
}

int LetterToStep(wxString sStep)
{
    //analyze the letter and return it as diatonic note number
    wxChar cStep = sStep.GetChar(0);
    switch (cStep) {
        case _T('c'):
            return 0;
        case _T('d'):
            return 1;
        case _T('e'):
            return 2;
        case _T('f'):
            return 3;
        case _T('g'):
            return 4;
        case _T('a'):
            return 5;
        case _T('b'):
            return 6;
        default:
            return 0;   //error
    }

}

//! Receives an pattern (a group of elements) and returns its duration
float SrcGetPatternDuracion(wxString sPattern)
{
    /*
        split the segment into elements, coumpute the duration of
        each element and return the total duration of the segment
    */
    int iEnd;
    wxString sElement;
    float rPatternDuration=0.0;

    wxString sSource = sPattern;
    while (sSource != _T("") )
    {
        //extract the element and remove it from source
        iEnd = SrcSplitPattern(sSource) + 1;
        sElement = sSource.Mid(0, iEnd);
        sSource = sSource.Mid(iEnd);

        //compute element's duration
        rPatternDuration += SrcGetElementDuracion(sElement);
    }
    return rPatternDuration;

}

//! Receives an element (note or rest) and returns its duration
float SrcGetElementDuracion(wxString sElement)
{
    //element is either (n ppp ddd ...) or (s ddd ...). We need the ddd

    int iStart, i;
    wxString sAux;
    wxString sElementType = sElement.Mid(1, 1);
    if (sElementType == _T("n") ) {
        //skip note pitch
        sAux = sElement.Mid(3);
    } else if (sElementType == _T("s") ) {
        sAux = sElement;
    }

    // find NoteType
    iStart = sAux.Find(_T(" ")) + 1;

    // fin dots
    i = iStart + 1;
    while (sAux.Mid(i, 1) == _T(".") ) i++;

    //extract NoteType and dots
    wxString sNoteType = sAux.Mid(iStart, i - iStart);
    
    // compute duration
    return LDPNoteTypeToDuration(sNoteType);

}

/*! returns true if received element is a rest
    sElement must be normalized (lower case, no extra spaces)
*/
bool SrcIsRest(wxString sElement)
{
    return (sElement.Mid(1, 1) == _T("s") );
}

/*! Receives an string formed by concatenated elements, for example:
    "(n * n)(n * s g+)(n * s)(n * c g-)"
    sSource must be normalized (lower case, no extra spaces)
    @return the index to the end (closing parenthesis) of first element
*/
int SrcSplitPattern(wxString sSource)
{                    
    int i;                  //index to character being explored
    int iMax;               //sSource length
    int nAPar;              //open parenthesis counter

    iMax = sSource.Length();
    wxASSERT(iMax > 0);                         //sSource must not be empty
    wxASSERT(sSource.Mid(0, 1) == _T("(") );    //must start with parenthesis
    
    nAPar = 1;       //let//s count first parenthesis
    //look for the matching closing parenthesis
    bool fFound = false;
    for (i=1; i < iMax; i++) {
        if (sSource.Mid(i, 1) == _T("(") ) {
            nAPar++;
        } else if (sSource.Mid(i, 1) == _T(")") ) {
            nAPar--;
            if (nAPar == 0) {
                //matching parenthesis found. Exit loop
                fFound = true;
                break;
            }
        }
    }
    wxASSERT(fFound);
    return i;
        
}



//----------------------------------------------------------------------------------------
// LDP
//----------------------------------------------------------------------------------------


bool LDPDataToPitch(wxString sPitch, EAccidentals* pAccidentals,
                    wxString* sStep, wxString* sOctave)
{
    /*
    Analyzes string sPitch (LDP format) and extracts its parts (Step, octave and 
    accidentals) and stores them in the corresponding parameters.
    Returns true if error (sPitch is not a valid pitch name)

    In LDP pitch is represented as a combination of the step of the diatonic scale, the 
    chromatic alteration, and the octave.
      - The nAccidentals parameter represents chromatic alteration (does not include tonal
        key alterations)
      - The octave element is represented by the numbers 0 to 9, where 4 indicates
        the octave started by middle C.
    */

    //It is assumed that sPitch is Trimed (no spaces before or after real data) and lower case
    bool fError = false;
    
    //if sPitch is a number it is interpreted as a MIDI pitch
    if (sPitch.IsNumber()) {
        long nAux = 0;
        fError = !sPitch.ToLong(&nAux);   
        wxASSERT(!fError);
        lmConverter oConverter;
        sPitch = oConverter.MidiPitchToLDPName((lmPitch) nAux);
        nAux = sPitch.Length();
        if (nAux == 2) {
            *sStep = sPitch.Mid(0, 1);
            *sOctave =  sPitch.Mid(1);
            *pAccidentals = eNoAccidentals;
        }
        else {
            *sStep =  sPitch.Mid(1, 1);
            *sOctave =  sPitch.Mid(2);
            *pAccidentals = eSharp;
        }
        return false;
    }
    
    //sPitch is alfanumeric: must be letter followed by a number (i.e.: "c4" )
    wxString sAlter;
    
    //split the string: accidentals and name
    switch (sPitch.Len()) {
        case 2: 
            sAlter = _T("");
            break;
        case 3:
            sAlter = sPitch.Mid(0, 1);
            sPitch = sPitch.Mid(1, 2);
            break;
        case 4:
            sAlter = sPitch.Mid(0, 2);
            sPitch = sPitch.Mid(2, 2);
            break;
        default:
            return true;   //error
    }

    *sStep = sPitch.Left(1);
    *sOctave = sPitch.Mid(1, 1);
   
    //analyse accidentals
    if (sAlter.IsEmpty()) {
        *pAccidentals = eNoAccidentals;
    } else if (sAlter.StartsWith( _T("+") )) {
        if (sAlter.StartsWith( _T("++") )) {
            *pAccidentals = eDoubleSharp;
        } else {
            *pAccidentals = eSharp;
        }
    } else if (sAlter.StartsWith( _T("-") )) {
        if (sAlter.StartsWith( _T("--") )) {
            *pAccidentals = eFlatFlat;
        } else {
            *pAccidentals = eFlat;
        }
    } else if (sAlter.StartsWith( _T("=+") )) {
        *pAccidentals = eNaturalSharp;
    } else if (sAlter.StartsWith( _T("=-") )) {
        *pAccidentals = eNaturalFlat;
    } else if (sAlter.StartsWith( _T("=") )) {
        *pAccidentals = eNatural;
    } else {
        return true;  //error
    }
    
    return false;  //no error

}

/// Returns -1 if error
EClefType LDPNameToClef(wxString sClefName)
{
    EClefType nClef = eclvSol;
    if (sClefName == _T("Do1")) {
        return eclvDo1;
    } else if (sClefName == _T("Do2")) {
        return eclvDo2;
    } else if (sClefName == _T("Do3")) {
        return eclvDo3;
    } else if (sClefName == _T("Do4")) {
        return eclvDo4;
    } else if (sClefName == _T("Sol")) {
        return eclvSol;
    } else if (sClefName == _T("Fa3")) {
        return eclvFa3;
    } else if (sClefName == _T("Fa4")) {
        return eclvFa4;
    } else if (sClefName == _T("SinClave")) {
        return eclvSinClave;
    } else {
        return (EClefType)-1;
    }
}

EKeySignatures LDPNameToKey(wxString sKeyName)
{
    static wxString m_sLDPKeyNames[lmMAX_KEY - lmMIN_KEY + 1];
    static bool m_fLDPNamesLoaded = false;

    if (!m_fLDPNamesLoaded) {
        //major key signatures
        m_sLDPKeyNames[earmDo] = _T("Do");
        m_sLDPKeyNames[earmSol] = _T("Sol");
        m_sLDPKeyNames[earmRe] = _T("Re");
        m_sLDPKeyNames[earmLa] = _T("La");
        m_sLDPKeyNames[earmMi] = _T("Mi");
        m_sLDPKeyNames[earmSi] = _T("Si");
        m_sLDPKeyNames[earmFas] = _T("Fa#");
        m_sLDPKeyNames[earmDos] = _T("Do#");
        m_sLDPKeyNames[earmDob] = _T("Dob");
        m_sLDPKeyNames[earmSolb] = _T("Solb");
        m_sLDPKeyNames[earmReb] = _T("Reb");
        m_sLDPKeyNames[earmLab] = _T("Lab");
        m_sLDPKeyNames[earmMib] = _T("Mib");
        m_sLDPKeyNames[earmSib] = _T("Sib");
        m_sLDPKeyNames[earmFa] = _T("Fa");
        // minor key signatures
        m_sLDPKeyNames[earmLam] = _T("Lam");
        m_sLDPKeyNames[earmMim] = _T("Mim");
        m_sLDPKeyNames[earmSim] = _T("Sim");
        m_sLDPKeyNames[earmFasm] = _T("Fa#m");
        m_sLDPKeyNames[earmDosm] = _T("Do#m");
        m_sLDPKeyNames[earmSolsm] = _T("Sol#m");
        m_sLDPKeyNames[earmResm] = _T("Re#m");
        m_sLDPKeyNames[earmLasm] = _T("La#m");
        m_sLDPKeyNames[earmLabm] = _T("Labm");
        m_sLDPKeyNames[earmMibm] = _T("Mibm");
        m_sLDPKeyNames[earmSibm] = _T("Sibm");
        m_sLDPKeyNames[earmFam] = _T("Fam");
        m_sLDPKeyNames[earmDom] = _T("Dom");
        m_sLDPKeyNames[earmSolm] = _T("Solm");
        m_sLDPKeyNames[earmRem] = _T("Rem");
        m_fLDPNamesLoaded = true;
    }
    
    int i;
    for (i = lmMIN_KEY; i <= lmMAX_KEY; i++) {
        if (m_sLDPKeyNames[i-lmMIN_KEY] == sKeyName) return (EKeySignatures)i;
    }
    return (EKeySignatures)-1;
    
}



//----------------------------------------------------------------------------------------
// MusicXML
//----------------------------------------------------------------------------------------

bool XmlDataToClef(wxString sClefLine, EClefType* pClef)
{
    if (sClefLine == _T("C1")) {
        *pClef = eclvDo1;
    } else if (sClefLine == _T("C2")) {
        *pClef = eclvDo2;
    } else if (sClefLine == _T("C3")) {
        *pClef = eclvDo3;
    } else if (sClefLine == _T("C4")) {
        *pClef = eclvDo4;
    } else if (sClefLine == _T("G2")) {
        *pClef = eclvSol;
    } else if (sClefLine == _T("F3")) {
        *pClef = eclvFa3;
    } else if (sClefLine == _T("F4")) {
        *pClef = eclvFa4;
    } else if (sClefLine == _T("SINCLAVE")) {
        *pClef = eclvSinClave;
    } else {
        return true;    //error
    }

    return false;

}

bool XmlDataToBarStyle(wxString sBarStyle, ETipoBarra* pType)
{
    if (sBarStyle == _T("FINREPETICION")) {
        *pType = etbBarraFinRepeticion;
    } else if (sBarStyle == _T("INICIOREPETICION")) {
        *pType = etbBarraInicioRepeticion;
    } else if (sBarStyle == _T("light-heavy")) {
        *pType = etbBarraFinal;
    } else if (sBarStyle == _T("light-light")) {
        *pType = etbBarraDoble;
    } else if (sBarStyle == _T("regular")) {
        *pType = etbBarraNormal;
    } else if (sBarStyle == _T("heavy-light")) {
        *pType = etbBarraInicial;
    } else if (sBarStyle == _T("DOBLEREPETICION")) {
        *pType = etbDobleRepeticion;
    } else {
        //! @todo Add styles dotted, heavy, heavy-heavy, none
        //! @todo Remove styles FINREPETICION, INICIOREPETICION, DOBLEREPETICION
        return true;    //error
    }

    return false;

}

void LoadCboBoxWithNoteNames(wxComboBox* pCboBox, lmPitch nSelNote)
{
    int i;
    pCboBox->Clear();
    for (i=1; i < 60; i++) {
        pCboBox->Append( GetNoteNamePhysicists((lmPitch) i) );
    }
    pCboBox->SetValue( GetNoteNamePhysicists(nSelNote) );
    
}

void LoadCboBoxWithNoteNames(wxComboBox* pCboBox, wxString sNoteName)
{
    int i;
    pCboBox->Clear();
    for (i=1; i < 60; i++) {
        pCboBox->Append( GetNoteNamePhysicists((lmPitch) i) );
    }
    pCboBox->SetValue( sNoteName );
    
}

