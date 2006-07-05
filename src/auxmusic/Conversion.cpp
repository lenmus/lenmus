// RCS-ID: $Id: Conversion.cpp,v 1.13 2006/03/03 14:59:45 cecilios Exp $
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
/*! @file Conversion.cpp
    @brief Implementation file for gloabl static functions for conversions
    @ingroup auxmusic
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Conversion.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/score.h"
#include "Conversion.h"

static wxString sEnglishNoteName[7] = { 
            _T("c"),  _T("d"), _T("e"), _T("f"), _T("g"), _T("a"), _T("b") };

static wxString sNoteName[7];


lmConverter::lmConverter()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    sNoteName[0] = _("c");
    sNoteName[1] = _("d");
    sNoteName[2] = _("e");
    sNoteName[3] = _("f");
    sNoteName[4] = _("g");
    sNoteName[5] = _("a");
    sNoteName[6] = _("b");

}
            
lmPitch lmConverter::PitchToMidiPitch(lmPitch nPitch)
{
    int nOctave = (nPitch - 1) / 7;
    int nRemainder = nPitch % 7;
    
    int nMidiPitch = nOctave * 12;
    switch (nRemainder) {
        case 0:  //si
            nMidiPitch = nMidiPitch + 11;   break;
        case 1:  //do
            //do nothing. Value is OK
            break;
        case 2:  //re
            nMidiPitch += 2;   break;
        case 3:  //mi
            nMidiPitch += 4;   break;
        case 4:  //fa
            nMidiPitch += 5;   break;
        case 5:  //sol
            nMidiPitch += 7;   break;
        case 6:  //la
            nMidiPitch += 9;   break;
    }
    nMidiPitch += 12;
    
    return nMidiPitch;
    
}

//! Returns true if the Midi note corresponds to natural note of the key signature scale
bool lmConverter::IsNaturalNote(lmPitch ntMidi, EKeySignatures nKey)
{
    
    //Prepare string with "1" in natural tones of the scale
    wxString sScale;
    switch (nKey) {
        case earmDo:
        case earmLam:
            //           C D EF G A B
            sScale = _T("101011010101");   break;

        //Sostenidos ---------------------------------------
        case earmSol:
        case earmMim:
            //           C D EF G A B
            sScale = _T("101010110101");   break;
        case earmRe:
        case earmSim:
            //           C D EF G A B
            sScale = _T("011010110101");   break;
        case earmLa:
        case earmFasm:
            //           C D EF G A B
            sScale = _T("011010101101");   break;
        case earmMi:
        case earmDosm:
            //           C D EF G A B
            sScale = _T("010110101101");   break;
        case earmSi:
        case earmSolsm:
            //           C D EF G A B
            sScale = _T("010110101011");   break;
        case earmFas:
        case earmResm:
            //           C D EF G A B
            sScale = _T("010101101011");   break;
        case earmDos:
        case earmLasm:
            //           C D EF G A B
            sScale = _T("110101101010");   break;

        //Bemoles -------------------------------------------
        case earmFa:
        case earmRem:
            //           C D EF G A B
            sScale = _T("101011010110");   break;
        case earmSib:
        case earmSolm:
            //           C D EF G A B
            sScale = _T("101101010110");   break;
        case earmMib:
        case earmDom:
            //           C D EF G A B
            sScale = _T("101101011010");   break;
        case earmLab:
        case earmFam:
            //           C D EF G A B
            sScale = _T("110101011010");   break;
        case earmReb:
        case earmSibm:
            //           C D EF G A B
            sScale = _T("110101101010");   break;
        case earmSolb:
        case earmMibm:
            //           C D EF G A B
            sScale = _T("010101101011");   break;
        case earmDob:
        case earmLabm:
            //           C D EF G A B
            sScale = _T("010110101011");   break;
        default:
            wxASSERT(false);
    }
    
    int nRemainder = ntMidi % 12;      //nRemainder goes from 0 (Do) to 11 (Si)
    return (sScale.Mid(nRemainder, 1) == _T("1"));

}



//void GetTipoNotaFromLetra(ByVal sLetra As String) As ENoteType
//    switch (UCase$(sLetra)
//        case "D"
//            GetTipoNotaFromLetra = eLong
//        case "R"
//            GetTipoNotaFromLetra = eWhole
//        case "B"
//            GetTipoNotaFromLetra = eHalf
//        case "N"
//            GetTipoNotaFromLetra = eQuarter
//        case "C"
//            GetTipoNotaFromLetra = eEighth
//        case "S"
//            GetTipoNotaFromLetra = e16th
//        case "F"
//            GetTipoNotaFromLetra = e32th
//        case "M"
//            GetTipoNotaFromLetra = e64th
//        case "G"
//            GetTipoNotaFromLetra = e128th
//        case "P"
//            GetTipoNotaFromLetra = e256th
//        default:
//            wxLogMessage(_T("[GetTipoNotaFromLetra]: Letra (" & sLetra & ") desconocida. Se devuelve negra"
//            GetTipoNotaFromLetra = eQuarter
//    }
//            
//}
//void MetricaQueDura(nDur As Long) As ETimeSignature
//    switch (nDur
//        //métricas binarias
//        case eWholeDuration
//            MetricaQueDura = emtr44
//        case eHalfDuration
//            MetricaQueDura = emtr24
//        case eQuarterDuration
//            MetricaQueDura = emtr28
//            
//        //métricas ternarias
//        case eHalfDottedDuration
//            MetricaQueDura = emtr68
//        case eQuarterDottedDuration
//            MetricaQueDura = emtr38
//            
//        //casos no contemplados
//        default:
//            Debug.Assert false  //duración no contemplada
//            MetricaQueDura = emtr24
//    }
//    
//}
//
//void EsTonalidadMayor(nTonalidad As EKeySignatures) As Boolean
//    EsTonalidadMayor = (nTonalidad < earmLam)
//    
//}
//
//void GetAlturaRel(nPitch As Long, nClave As EClefType) As Long
//    switch (nClave
//        case eclvSol
//            GetAlturaRel = nPitch - 29
//        case eclvFa4
//            GetAlturaRel = nPitch - 17
//        case eclvFa3
//            GetAlturaRel = nPitch - 19
//        case eclvDo1
//            GetAlturaRel = nPitch - 27
//        case eclvDo2
//            GetAlturaRel = nPitch - 25
//        case eclvDo3
//            GetAlturaRel = nPitch - 23
//        case eclvDo4
//            GetAlturaRel = nPitch - 21
//        default:   //suponer clave de Sol
//            GetAlturaRel = nPitch - 29
//    }
//    
//}
//
//
//Private Sub CrearPartiturasPatrones()
//
//    //Tipos patrones: dibuja los patrones
//    Dim i As Long
//    Dim nTipo As Long       //1 - Métrica binaria, 2 - Métrica ternaria
//    Dim sPatron As String
//    Dim ntN(1 To 16) As Long
//    
//    //todas las notas serán La4 (AltDiatonica = 34)
//    for (i = 1 To 16
//        ntN(i) = 34
//    }   // i
//    
//    //Crea las partituras
//    Dim oStaff As CStaff
//    for (nTipo = 1 To 2
//        for (i = 1 To nNumPatrones(nTipo)
//            //particulariza el patrón
//            sPatron = "(C " & SrcParticularizarPatron(sPatrones(nTipo, i), ntN) & _
//                "(Barra Final NoVisible) )"
//        
//            //crea la partitura
//            Set oPartituraPatron(nTipo, i) = New CScore
//            oPartituraPatron(nTipo, i).ScoreConstructor
//            Set oStaff = oPartituraPatron(nTipo, i).Pentagrama(1)
//            InicializarParseLinea
//            With oStaff
//                .AddClave eclvSol, 1, NO_VISIBLE
//                .AddArmadura earmDo, NO_VISIBLE
//                .CrearCompas sPatron
//            End With
//            
//        }   // i
//    }   // nTipo
//
//End Sub
//
//Public Sub CalcularDuracionPatrones()
//    Dim nTipo As Long, i As Long
//    
//    for (nTipo = 1 To 2
//        for (i = 1 To nNumPatrones(nTipo)
//            nDuracionPatron(nTipo, i) = SrcDuracionPatron(sPatrones(nTipo, i))
//        }   // i
//    }   // nTipo
//    
//
//End Sub
//
//void NormalizarPatron(ByVal sPatron As String) As String
//    //Devuelve el patron normalizado:
//    //   - en minúsculas
//    //   - sin blancos superfluos
//    
//    Dim sOut As String, i As Long, fBlancoPendiente As Boolean, cChar As String
//    Dim sTab As String, sCr As String, sLf As String
//    Dim fParentesisPrevio As Boolean
//    
//    sPatron = LCase$(sPatron)
//    sOut = sEmpty
//    fBlancoPendiente = false
//    fParentesisPrevio = false
//    for (i = 1 To Len(sPatron)
//        cChar = Mid$(sPatron, i, 1)
//        switch (cChar
//            case " "
//                if (Not fParentesisPrevio Then fBlancoPendiente = true
//                
//            case "(", ")"
//                fBlancoPendiente = false    //ignorar blancos previos
//                fParentesisPrevio = true    //para ignorar blancos posteriores
//                sOut = sOut & cChar
//                
//            default:
//                //otros caracteres. Llevar a salida
//                if (fBlancoPendiente Then sOut = sOut & " "
//                sOut = sOut & cChar
//                fBlancoPendiente = false
//                fParentesisPrevio = false
//                
//        }
//    }   // i
//    
//    NormalizarPatron = sOut
//            
//}
//
//void GetNombreMetrica(nMetrica As ETimeSignature) As String
//    Static fNombresCargados As Boolean
//    Static sNombre(1 To MAX_METRICAS) As String
//    
//    if (Not fNombresCargados Then
//        sNombre(emtr24) = "2 4"
//        sNombre(emtr34) = "3 4"
//        sNombre(emtr44) = "4 4"
//        sNombre(emtr28) = "2 8"
//        sNombre(emtr38) = "3 8"
//        sNombre(emtr22) = "2 2"
//        sNombre(emtr32) = "3 2"
//        sNombre(emtr68) = "6 8"
//        sNombre(emtr98) = "9 8"
//        sNombre(emtr128) = "12 8"
//    }
//    
//    GetNombreMetrica = sNombre(nMetrica)
//    
//}
//
//void NombreLDP_Clave(nClave As EClefType) As String
//    Static fNombresCargados As Boolean
//    Static sNombre(1 To MAX_CLAVES) As String
//    
//    if (Not fNombresCargados Then
//        sNombre(1) = "Sol"
//        sNombre(2) = "Fa4"
//        sNombre(3) = "Fa3"
//        sNombre(4) = "Do1"
//        sNombre(5) = "Do2"
//        sNombre(6) = "Do3"
//        sNombre(7) = "Do4"
//        sNombre(8) = "Sin clave"
//    }
//    
//    NombreLDP_Clave = sNombre(nClave)
//    
//}
//
//void EsArmaduraSostenidos(nTonalidad As EKeySignatures) As Boolean
//    
//    switch (nTonalidad
//        //case earmDo, earmLam:
//            //sin sostenidos ni bemoles. Sale por el default:
//        case earmSol, earmMim, earmRe, earmSim, earmLa, earmFasm, earmMi, earmDosm, _
//                earmSi, earmSolsm, earmFas, earmResm, earmDos, earmLasm:
//            EsArmaduraSostenidos = true
//        default:
//            EsArmaduraSostenidos = false
//    }
//
//}
//
//void GetNombreLng_Tonalidad(nTonalidad As EKeySignatures) As String
//        
//    switch (nTonalidad
//        case earmDo
//            GetNombreLng_Tonalidad = sGbDoMayor
//        case earmSol
//            GetNombreLng_Tonalidad = sGbSolMayor
//        case earmRe
//            GetNombreLng_Tonalidad = sGbReMayor
//        case earmLa
//            GetNombreLng_Tonalidad = sGbLaMayor
//        case earmMi
//            GetNombreLng_Tonalidad = sGbMiMayor
//        case earmSi
//            GetNombreLng_Tonalidad = sGbSiMayor
//        case earmFas
//            GetNombreLng_Tonalidad = sGbFasMayor
//        case earmDos
//            GetNombreLng_Tonalidad = sGbDosMayor
//        case earmDob
//            GetNombreLng_Tonalidad = sGbDobMayor
//        case earmSolb
//            GetNombreLng_Tonalidad = sGbSolbMayor
//        case earmReb
//            GetNombreLng_Tonalidad = sGbRebMayor
//        case earmLab
//            GetNombreLng_Tonalidad = sGbLabMayor
//        case earmMib
//            GetNombreLng_Tonalidad = sGbMibMayor
//        case earmSib
//            GetNombreLng_Tonalidad = sGbSibMayor
//        case earmFa
//            GetNombreLng_Tonalidad = sGbFaMayor
//
//        //Tonalidades relativas de las anteriores
//        case earmLam
//            GetNombreLng_Tonalidad = sGbLaMenor
//        case earmMim
//            GetNombreLng_Tonalidad = sGbMiMenor
//        case earmSim
//            GetNombreLng_Tonalidad = sGbSiMenor
//        case earmFasm
//            GetNombreLng_Tonalidad = sGbFasMenor
//        case earmDosm
//            GetNombreLng_Tonalidad = sGbDosMenor
//        case earmSolsm
//            GetNombreLng_Tonalidad = sGbSolsMenor
//        case earmResm
//            GetNombreLng_Tonalidad = sGbResMenor
//        case earmLasm
//            GetNombreLng_Tonalidad = sGbLasMenor
//        case earmLabm
//            GetNombreLng_Tonalidad = sGbLabMenor
//        case earmMibm
//            GetNombreLng_Tonalidad = sGbMibMenor
//        case earmSibm
//            GetNombreLng_Tonalidad = sGbSibMenor
//        case earmFam
//            GetNombreLng_Tonalidad = sGbFaMenor
//        case earmDom
//            GetNombreLng_Tonalidad = sGbDoMenor
//        case earmSolm
//            GetNombreLng_Tonalidad = sGbSolMenor
//        case earmRem
//            GetNombreLng_Tonalidad = sGbReMenor
//        default:
//            Debug.Assert false
//    }
//    
//}
//
//void GetNombreLDP_Tonalidad(nTonalidad As EKeySignatures) As String
//    //devuelve el nombre de la tonalidad, según lenguaje externo LDP
//        
//    Static fNombresCargados As Boolean
//    Static sNombreLDP(1 To lmMAX_KEY) As String
//
//    if (Not fNombresCargados Then
//        sNombreLDP(earmDo) = "Do"
//        sNombreLDP(earmSol) = "Sol"
//        sNombreLDP(earmRe) = "Re"
//        sNombreLDP(earmLa) = "La"
//        sNombreLDP(earmMi) = "Mi"
//        sNombreLDP(earmSi) = "Si"
//        sNombreLDP(earmFas) = "Fa#"
//        sNombreLDP(earmDos) = "Do#"
//        sNombreLDP(earmDob) = "Dob"
//        sNombreLDP(earmSolb) = "Solb"
//        sNombreLDP(earmReb) = "Reb"
//        sNombreLDP(earmLab) = "Lab"
//        sNombreLDP(earmMib) = "Mib"
//        sNombreLDP(earmSib) = "Sib"
//        sNombreLDP(earmFa) = "Fa"
//        // Tonalidades relativas de las anteriores
//        sNombreLDP(earmLam) = "La"
//        sNombreLDP(earmMim) = "Mi"
//        sNombreLDP(earmSim) = "Si"
//        sNombreLDP(earmFasm) = "Fa#"
//        sNombreLDP(earmDosm) = "Do#"
//        sNombreLDP(earmSolsm) = "Sol#"
//        sNombreLDP(earmResm) = "Re#"
//        sNombreLDP(earmLasm) = "La#"
//        sNombreLDP(earmLabm) = "Lab"
//        sNombreLDP(earmMibm) = "Mib"
//        sNombreLDP(earmSibm) = "Sib"
//        sNombreLDP(earmFam) = "Fa"
//        sNombreLDP(earmDom) = "Do"
//        sNombreLDP(earmSolm) = "Sol"
//        sNombreLDP(earmRem) = "Re"
//        fNombresCargados = true
//    }
//    
//    GetNombreLDP_Tonalidad = sNombreLDP(nTonalidad)
//    
//}
//
//Public Sub CargarCboNotas(cboX As ComboBox, Optional nAltDiat As Long = -1)
//    Dim i As Long
//    With cboX
//        .Clear
//        for (i = 1 To 57
//            .AddItem GetNotaOctavaLng(i)
//        }   // i
//        .ListIndex = nAltDiat
//    End With
//    
//End Sub
//
////devuelve cero si letra desconocida. Puede venir con puntillos
//void GetDuracionFromLetra(ByRef sLetra As String) As ENoteDuration
//
//    Dim nDur As Long
//    
//    switch (UCase$(Mid$(sLetra, 1, 1))
//        case "D"
//            nDur = eLongDuration
//        case "R"
//            nDur = eWholeDuration
//        case "B"
//            nDur = eHalfDuration
//        case "N"
//            nDur = eQuarterDuration
//        case "C"
//            nDur = eEighthDuration
//        case "S"
//            nDur = e16thDuration
//        case "F"
//            nDur = e32thDuration
//        case "M"
//            nDur = e64thDuration
//        case "G"
//            nDur = e128thDuration
//        case "P"
//            nDur = e256thDuration
//        default:
//            GetDuracionFromLetra = 0
//            Exit Function
//    }
//    
//    Dim i As Long
//    i = Len(sLetra)
//    if (i > 1 Then
//        if (i = 2 Then
//            if (Mid$(sLetra, 2, 1) = "." Then
//                nDur = (nDur * 3) / 2       // x1.5
//            Else
//                GetDuracionFromLetra = 0
//                Exit Function
//            }
//        ElseIf i = 3 Then
//            if (Mid$(sLetra, 2, 2) = ".." Then
//                nDur = (nDur * 7) / 4       // x1.75
//            Else
//                GetDuracionFromLetra = 0
//                Exit Function
//            }
//        Else
//            GetDuracionFromLetra = 0
//            Exit Function
//        }
//    }
//            
//    GetDuracionFromLetra = nDur
//
//}

/*! Returns the diatonic pitch that corresponds to the received MIDI pitch. It is assumed
    C major key signature.
*/
lmPitch lmConverter::MidiPitchToPitch(lmPitch nMidiPitch)
{
    /*! @aware: To make this algorithm indepedent from the choosen pitch scale, 
        we will use constant lmC4PITCH to determine the right value.
    */
    int nOctave = (nMidiPitch - 12) / 12;
    int nRemainder = nMidiPitch % 12;
    int nPitch = nOctave * 7 + 1;      // C note 
    int nShift = lmC4PITCH - 29;       // if nOctave is 4, nPicht will be 29
    int nScale[] = {0,0,1,1,2,3,3,4,4,5,5,6};
    return nPitch + nShift + nScale[nRemainder];
    
}

/*! Returns the LDP diatonic pitch that corresponds to the received MIDI pitch. It is assumed
    C major key signature.
*/
wxString lmConverter::MidiPitchToLDPName(lmPitch nMidiPitch)
{
    int nOctave = (nMidiPitch - 12) / 12;
    int nRemainder = nMidiPitch % 12;
    wxString sNote[] = {_T("c"), _T("+c"), _T("d"), _T("+d"), _T("e"), _T("f"), _T("+f"),
                        _T("g"), _T("+g"), _T("a"), _T("+a"), _T("b") };
    return sNote[nRemainder] + wxString::Format(_T("%d"), nOctave);
    
}

/*! Returns the LDP note name that corresponds to the received pitch. For exaplample,
    pitch 29 will return "C4". 
*/
wxString lmConverter::PitchToLDPName(lmPitch nPitch)
{
    int nOctave = (nPitch - 1) / 7;
    int nStep = (nPitch -1) % 7;
    wxString sNote[] = {_T("c"), _T("d"), _T("e"), _T("f"), _T("g"), _T("a"), _T("b") };
    return sNote[nStep] + wxString::Format(_T("%d"), nOctave);
    
}


////Devuelve el patrón (el nombre de nota, la octava y las alteraciones que hubiera) que
////correspondan a la nota Midi según la tonalidad de la escala.
////Devuelve modifica la variable ntDiat para dejar la nota diatónica
////Por ejemplo, Si4b (midi 70) en tonalidad de Fa mayor se devuelve Si (patron="B4", ntDiat=35 Si)
////pero en tonalidad de La mayor devuelve La# (patron="A4+", ntDiat=34 La)
//void NotaMidiToPatron(nAltMidi As Integer, nTonalidad As EKeySignatures, _
//            ByRef ntDiat As Long) As String
//    Dim nRemainder As Long, nOctava As Long, i As Long
//    
//    nOctava = (nAltMidi - 12) \ 12
//    nRemainder = nAltMidi % 12
//    
//    Dim sPatron As String, sDesplz As String
//    switch (nTonalidad
//        case earmDo, earmLam:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  "
//            sPatron = "C  C+ D  D+ E  F  F+ G  G+ A  A+ B  "
//            sDesplz = "0  0  1  1  2  3  3  4  4  5  5  6  "
//        //Sostenidos ---------------------------------------
//        case earmSol, earmMim:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  " #F
//            sPatron = "c  c+ d  d+ e  e+ f  g  g+ a  a+ b  "
//            sDesplz = "0  0  1  1  2  2  3  4  4  5  5  6  "
//        case earmRe, earmSim:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  " #FC
//            sPatron = "B+ C  D  D+ E  E+ F  G  G+ A  A+ B  "
//            sDesplz = "-1 0  1  1  2  2  3  4  4  5  5  6  "
//        case earmLa, earmFasm:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  " #FCG
//            sPatron = "B+ C  D  D+ E  E+ F  F++G  A  A+ B  "
//            sDesplz = "-1 0  1  1  2  2  3  3  4  5  5  6  "
//        case earmMi, earmDosm:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGD
//            sPatron = "B+ C  C++D  E  E+ F  F++G  A  A+ B  "
//            sDesplz = "-1 0  0  1  2  2  3  3  4  5  5  6  "
//        case earmSi, earmSolsm:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGDA
//            sPatron = "B+ C  C++D  E  E+ F  F++G  G++A  B  "
//            sDesplz = "-1 0  0  1  2  2  3  3  4  4  5  6  "
//        case earmFas, earmResm:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGDAE
//            sPatron = "B+ C  C++D  D++E  F  F++G  G++A  B  "
//            sDesplz = "-1 0  0  1  1  2  3  3  4  4  5  6  "
//        case earmDos, earmLasm:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  " #FCGDAEB
//            sPatron = "B  C  C++D  D++E  F  F++G  G++A  A++"
//            sDesplz = "-1 0  0  1  1  2  3  3  4  4  5  5  "
//        //Bemoles -------------------------------------------
//        case earmFa, earmRem:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  "
//            sPatron = "C  C+ D  D+ E  F  F+ G  G+ A  B  B= "
//            sDesplz = "0  0  1  1  2  3  3  4  4  5  6  6  "
//        case earmSib, earmSolm:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  "
//            sPatron = "C  C+ D  E  E= F  F+ G  G+ A  B  B= "
//            sDesplz = "0  0  1  2  2  3  3  4  4  5  6  6  "
//        case earmMib, earmDom:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  "
//            sPatron = "C  C+ D  E  E= F  F+ G  A  A= B  B= "
//            sDesplz = "0  0  1  2  2  3  3  4  5  5  6  6  "
//        case earmLab, earmFam:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  "
//            sPatron = "C  D  D= E  E= F  F+ G  A  A= B  B= "
//            sDesplz = "0  1  1  2  2  3  3  4  5  5  6  6  "
//        case earmReb, earmSibm:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  "
//            sPatron = "C  D  D= E  E= F  G  G= A  A= B  B= "
//            sDesplz = "0  1  1  2  2  3  4  4  5  5  6  6  "
//        case earmSolb, earmMibm:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  "
//            sPatron = "C= D  D= E  E= F  G  G= A  A= B  C  "
//            sDesplz = "0  1  1  2  2  3  4  4  5  5  6  7  "
//        case earmDob, earmLabm:
//            //         "C  C# D  D# E  F  F# G  G# A  A# B  "
//            sPatron = "C= D  D= E  F  F= G  G= A  A= B  C  "
//            sDesplz = "0  1  1  2  2  3  4  4  5  5  6  7  "
//        default:
//            wxLogMessage(_T("[IsNaturalNote]: Tonalidad (" & nTonalidad & ") no contemplada."
//    }
//    
//    Dim nDesplz As Long
//    i = 3 * nRemainder + 1
//    nDesplz = CLng(Mid$(sDesplz, i, 2))
//    ntDiat = nOctava * 7 + 1 + nDesplz
//    
//    if (nDesplz = -1 Then
//        nOctava = nOctava - 1
//    ElseIf nDesplz = 7 Then
//        nOctava = nOctava + 1
//    }
//    NotaMidiToPatron = RTrim$(Mid$(sPatron, i + 1, 2)) & Mid$(sPatron, i, 1) & nOctava
//    
//}
//
//void NotaLDPToAltDiatonica(sNombre As String) As Long
//    Dim nAltura As Long, nAlteraciones As EAccidentals
//    ParseAltura sNombre, nAltura, nAlteraciones
//    NotaLDPToAltDiatonica = nAltura
//}

wxString lmConverter::GetNoteName(lmPitch nPitch)
{
    //returns the name of the note translated to current language
    int iNote = (nPitch - 1) % 7;
    return sNoteName[iNote];

}

//void GetNotaOctavaLng(nPitch As Long) As String
//    Dim nOctava As Long
//    nOctava = (nPitch - 1) \ 7
//    GetNotaOctavaLng = GetNombreLng_Nota(nPitch) & CStr(nOctava)
//
//}
//
//void GetNombreLatino(nPitch As Long) As String
//    Dim nOctava As Long, iNota As Long
//    Static sNombreLatino(0 To 6) As String
//    Static fNombresCargados As Boolean
//    
//    nOctava = (nPitch - 1) \ 7
//    iNota = (nPitch - 1) % 7
//    
//    if (Not fNombresCargados Then
//        sNombreLatino(0) = "Do"
//        sNombreLatino(1) = "Re"
//        sNombreLatino(2) = "Mi"
//        sNombreLatino(3) = "Fa"
//        sNombreLatino(4) = "Sol"
//        sNombreLatino(5) = "La"
//        sNombreLatino(6) = "Si"
//        
//        fNombresCargados = true
//        
//    }
//    
//    GetNombreLatino = sNombreLatino(iNota) & CStr(nOctava)
//
//}

wxString lmConverter::GetEnglishNoteName(lmPitch nPitch)
{
    int iNota = (nPitch - 1) % 7;
    int nOctava = (nPitch - 1) / 7;
    
    return wxString::Format(_T("%s%d"), sEnglishNoteName[iNota], nOctava );

}

////==========================================================================================
//// Funciones a llevar a Corecsg
////==========================================================================================
//
//void LongAleatorio(nMin As Long, nMax As Long) As Long
//    Debug.Assert nMin <= nMax
//    LongAleatorio = Int((nMax - nMin + 1) * Rnd + nMin)
//    
//}
//
//void MinLong(a As Long, b As Long) As Long
//    if (a < b Then
//        MinLong = a
//    Else
//        MinLong = b
//    }
//    
//}
//
//void MaxLong(a As Long, b As Long) As Long
//    if (a > b Then
//        MaxLong = a
//    Else
//        MaxLong = b
//    }
//    
//}
//
//void MinSingle(a As Single, b As Single) As Single
//    if (a < b Then
//        MinSingle = a
//    Else
//        MinSingle = b
//    }
//
//}
//
//void MaxSingle(a As Single, b As Single) As Single
//    if (a > b Then
//        MaxSingle = a
//    Else
//        MaxSingle = b
//    }
//
//}
//
////GUtility
//Public Sub DoWaitEvents(msWait As Long)
//    Dim msEnd As Long
//    msEnd = GetTickCount + msWait
//    Do
//        DoEvents
//    Loop While GetTickCount < msEnd
//End Sub
//
