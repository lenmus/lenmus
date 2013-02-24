//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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
//---------------------------------------------------------------------------------------

#include "lenmus_utilities.h"
#include "lenmus_string.h"



namespace lenmus
{


////----------------------------------------------------------------------------------------
///*! @page note_names        ImoNote names and pitch values
//
//@verbatim
// nPitchMIDI:    chromatic pitch: number representing the note in a chromatic scale. It is
//                the same as the number used in MIDI for representing pitch.
// nPitch:        diatonic pitch: number representing the note in a diatonic scale
// nOctave:        octave number (as MIDI). The lowest scale (-1) is not yet used
//
// a pitch value of 0 (zero) represents a rest.
//
//                                AltDiatonica
//  name      Octave      PitchMIDI    Pitch      Observations
//  ----      ------      ---------    -------    -----------------------------------
//  rest      any         0            0          Any rest
//  (c-1)    -1           1
//  c0        0           12           1          Do2 de la subcontraoctava (16,35 Hz)
//  d0        0           14           2
//  e0        0           16           3
//  f0        0           17           4
//  g0        0           19           5
//  a0        0           21           6
//  b0        0           23           7
//  c1        1           24           8          Do1 de la contraoctava
//  c2        2           36           15         Do de la gran octava
//  c3        3           48           22         do de la pequeña octava
//  c4        4           60           29         do1 de la octava primera (la1 = 440Hz)
//  d4        4           62           30
//  e4        4           64           31
//  f4        4           65           32
//  ...
//  c5        5           72           36         do2 de la octava segunda
//  c6        6           84           43         do3 de la octava tercera
//  c7        7           96           50         do4 de la octava cuarta
//  c8        8           108          57         do5 de la octava quinta (4.186 Hz)
//  etc.
//
//  Accidentals are represented by minus (-) and plus (+) signs before the note name
//       -  : flat
//       +  : sharp
//       -- : double flat
//       ++ : sharp-sharp (two sharps)
//       =  : natural
//       =- : natural-flat
//       =+ : natural-sharp
//       x  : double sharp
//  i.e.:  ++c3, =+c3, +c3, =c3, -c3, --c3, =-c3, xc3
//
//  Pitch name can also be a number, representing MIDI pitch. i.e.: (n 60 n) = (n c4 n)
//@endverbatim
//*/
////-------------------------------------------------------------------------------------------*/
//
///*! @brief    Convert LDP pitch name to pitch and accidentals
//
//    Convert LDP pitch name to pitch and accidentals.
//    @return    Returns true if error (sPitch is not a valid pitch name)
//            Otherwise, stores the corresponding data into the parameters pPitch, and
//            pAccidentals.
//
//    @param[in]  sPitch    The LDP string with the pitch to convert. Pitch is represented
//                as the combination of the chromatic alteration, the step in the
//                diatonic scale, and the octave (i.e. "+c4").
//    @param[out] pPitch    Diatonic MIDI pitch.
//    @param[out] pAccidentals parameter represents chromatic accidentals (does not
//                include key signature accidentals)
//*/
//bool PitchNameToData(wxString sPitch, int* pPitch, lmEAccidentals* pAccidentals)
//{
//
//    //It is assumed that sPitch is Trimed (no spaces before or after real data) and lower case
//    bool fError = false;
//
//    //if sPitch is a number it is interpreted as a MIDI pitch
//    if (sPitch.IsNumber()) {
//        long nAux = 0;
//        fError = !sPitch.ToLong(&nAux);
//        wxASSERT(!fError);
//        *pPitch = (int)nAux;
//        *pAccidentals = lm_eNoAccidentals;
//        //TODO: analizar la nota MIDI y determinar las
//        //      alteraciones en función de la armadura
//        return false;
//    }
//
//    //sPitch is alfanumeric: must be letter followed by a number (i.e.: "c4" )
//    wxString sAlter;
//
//    //split the string: accidentals and name
//    switch (sPitch.length()) {
//        case 2:
//            sAlter = _T("");
//            break;
//        case 3:
//            sAlter = sPitch.substr(0, 1);
//            sPitch = sPitch.substr(1, 2);
//            break;
//        case 4:
//            sAlter = sPitch.substr(0, 2);
//            sPitch = sPitch.substr(2, 2);
//            break;
//        default:
//            return true;   //error
//    }
//
//    wxString sStep = sPitch.Left(1);
//    wxString sOctave = sPitch.substr(1, 1);
//    fError = StringToPitch(sStep, sOctave, pPitch);
//    if (fError) return true;
//
//    //analyse accidentals
//    if (sAlter.IsEmpty()) {
//        *pAccidentals = lm_eNoAccidentals;
//    } else if (sAlter.StartsWith( _T("+") )) {
//        if (sAlter.StartsWith( _T("++") )) {
//            *pAccidentals = lm_eSharpSharp;
//        } else {
//            *pAccidentals = lm_eSharp;
//        }
//    } else if (sAlter.StartsWith( _T("-") )) {
//        if (sAlter.StartsWith( _T("--") )) {
//            *pAccidentals = lm_eFlatFlat;
//        } else {
//            *pAccidentals = lm_eFlat;
//        }
//    } else if (sAlter.StartsWith( _T("=+") )) {
//        *pAccidentals = lm_eNaturalSharp;
//    } else if (sAlter.StartsWith( _T("=-") )) {
//        *pAccidentals = lm_eNaturalFlat;
//    } else if (sAlter.StartsWith( _T("=") )) {
//        *pAccidentals = lm_eNatural;
//    } else if (sAlter.StartsWith( _T("x") )) {
//        *pAccidentals = lm_eDoubleSharp;
//    } else {
//        return true;  //error
//    }
//
//    return false;  //no error
//
//}
//
//bool StringToPitch(wxString sStep, wxString sOctave, int* pPitch)
//{
//    /*
//     sStep is a one char string: "a" to "g", in lower case
//     sOctave is a one char string: "0" to "9"
//
//     returns true if error (sStep is not "a"-"g" or sOctave not "0"-"9")
//     Otherwise, stores the corresponding data into the parameters nPitch.
//
//    */
//
//    //analyze the letter and store it as diatonic pitch step
//    *pPitch = LetterToStep(sStep) + 1;
//    if (*pPitch == 0) return true;   //error
//
//    //combine octave with pitch step
//    if (! sOctave.IsNumber()) {
//        return true;   //error
//    }
//
//    long nOctave;
//    bool fError = !sOctave.ToLong(&nOctave);
//    wxASSERT(!fError);
//    *pPitch = *pPitch + 7 * nOctave;
//
//    return (nOctave < 0 || nOctave > 9);        //true if error
//
//}
//
//int LetterToStep(wxString sStep)
//{
//    //analyze the letter and return it as diatonic note number
//    return LetterToStep( sStep.GetChar(0) );
//}
//
//int LetterToStep(wxChar cStep)
//{
//    //analyze the letter and return it as diatonic note number
//    switch (cStep) {
//        case _T('c'):
//            return 0;
//        case _T('d'):
//            return 1;
//        case _T('e'):
//            return 2;
//        case _T('f'):
//            return 3;
//        case _T('g'):
//            return 4;
//        case _T('a'):
//            return 5;
//        case _T('b'):
//            return 6;
//        default:
//            return 0;   //error
//    }
//
//}

//---------------------------------------------------------------------------------------
bool ldp_pattern_is_rest(const wxString& sElement)
{
    // returns true if received element is a rest.
    // sElement must be normalized (lower case, no extra spaces)

    return (sElement.substr(1, 1) == _T("r") );
}

//---------------------------------------------------------------------------------------
int split_ldp_pattern(const wxString& sSource)
{
    // Receives an string formed by concatenated elements, for example:
    // "(n * q)(n * s g+)(n * s)(n * e g-)"
    // sSource must be normalized (lower case, no extra spaces)
    // Return the index to the end (closing parenthesis) of first element

    int i;                  //index to character being explored
    int iMax;               //sSource length
    int nAPar;              //open parenthesis counter

    iMax = sSource.length();
    wxASSERT(iMax > 0);                         //sSource must not be empty
    wxASSERT(sSource.substr(0, 1) == _T("(") );    //must start with parenthesis

    nAPar = 1;       //let//s count first parenthesis
    //look for the matching closing parenthesis
    bool fFound = false;
    for (i=1; i < iMax; i++) {
        if (sSource.substr(i, 1) == _T("(") ) {
            nAPar++;
        } else if (sSource.substr(i, 1) == _T(")") ) {
            nAPar--;
            if (nAPar == 0) {
                //matching parenthesis found. Exit loop
                fFound = true;
                break;
            }
        }
    }
    if (fFound)
        return i;

    wxASSERT(fFound);
    return i;
}



////----------------------------------------------------------------------------------------
//// LDP
////----------------------------------------------------------------------------------------
//
//
//bool LDPDataToPitch(wxString sPitch, lmEAccidentals* pAccidentals,
//                    wxString* sStep, wxString* sOctave)
//{
//    /*
//    Analyzes string sPitch (LDP format) and extracts its parts (Step, octave and
//    accidentals) and stores them in the corresponding parameters.
//    Returns true if error (sPitch is not a valid pitch name)
//
//    In LDP pitch is represented as a combination of the step of the diatonic scale, the
//    chromatic alteration, and the octave.
//      - The nAccidentals parameter represents chromatic alteration (does not include tonal
//        key alterations)
//      - The octave element is represented by the numbers 0 to 9, where 4 indicates
//        the octave started by middle C.
//    */
//
//    //It is assumed that sPitch is Trimed (no spaces before or after real data) and lower case
//    bool fError = false;
//
//    //if sPitch is a number it is interpreted as a MIDI pitch
//    if (sPitch.IsNumber()) {
//        long nAux = 0;
//        fError = !sPitch.ToLong(&nAux);
//        wxASSERT(!fError);
//        sPitch = MPitch_ToLDPName((lmMPitch) nAux);
//        nAux = sPitch.length();
//        if (nAux == 2) {
//            *sStep = sPitch.substr(0, 1);
//            *sOctave =  sPitch.substr(1);
//            *pAccidentals = lm_eNoAccidentals;
//        }
//        else {
//            *sStep =  sPitch.substr(1, 1);
//            *sOctave =  sPitch.substr(2);
//            *pAccidentals = lm_eSharp;
//        }
//        return false;
//    }
//
//    //sPitch is alfanumeric: must be letter followed by a number (i.e.: "c4" )
//    wxString sAlter;
//
//    //split the string: accidentals and name
//    switch (sPitch.length()) {
//        case 2:
//            sAlter = _T("");
//            break;
//        case 3:
//            sAlter = sPitch.substr(0, 1);
//            sPitch = sPitch.substr(1, 2);
//            break;
//        case 4:
//            sAlter = sPitch.substr(0, 2);
//            sPitch = sPitch.substr(2, 2);
//            break;
//        default:
//            return true;   //error
//    }
//
//    *sStep = sPitch.Left(1);
//    *sOctave = sPitch.substr(1, 1);
//
//    //analyse accidentals
//    if (sAlter.IsEmpty()) {
//        *pAccidentals = lm_eNoAccidentals;
//    } else if (sAlter.StartsWith( _T("+") )) {
//        if (sAlter.StartsWith( _T("++") )) {
//            *pAccidentals = lm_eSharpSharp;
//        } else {
//            *pAccidentals = lm_eSharp;
//        }
//    } else if (sAlter.StartsWith( _T("-") )) {
//        if (sAlter.StartsWith( _T("--") )) {
//            *pAccidentals = lm_eFlatFlat;
//        } else {
//            *pAccidentals = lm_eFlat;
//        }
//    } else if (sAlter.StartsWith( _T("=+") )) {
//        *pAccidentals = lm_eNaturalSharp;
//    } else if (sAlter.StartsWith( _T("=-") )) {
//        *pAccidentals = lm_eNaturalFlat;
//    } else if (sAlter.StartsWith( _T("=") )) {
//        *pAccidentals = lm_eNatural;
//    } else if (sAlter.StartsWith( _T("x") )) {
//        *pAccidentals = lm_eDoubleSharp;
//    } else {
//        return true;  //error
//    }
//
//    return false;  //no error
//
//}
//
//FPitch lmLDPDataToFPitch(wxString& sPitch)
//{
//    //returns the FPitch encoded in string sPitch (LDP format).
//    //sPitch is must be 'trimed' (no spaces before or after real data) and in lower case
//    //If any error, returns 0.
//
//    //split the string: accidentals and name
//    wxString sAlter;
//    switch (sPitch.length()) {
//        case 2:
//            sAlter = _T("");
//            break;
//        case 3:
//            sAlter = sPitch.substr(0, 1);
//            sPitch = sPitch.substr(1, 2);
//            break;
//        case 4:
//            sAlter = sPitch.substr(0, 2);
//            sPitch = sPitch.substr(2, 2);
//            break;
//        default:
//            return 0;   //error
//    }
//
//    //get step
//    wxString sStep = sPitch.Left(1);
//    static wxString sSteps = _T("abcdefg");
//    int nStep = LetterToStep(sStep);
//
//    //get octave
//    long nOctave;
//    wxString sOctave = sPitch.substr(1, 1);
//    if (!sOctave.ToLong(&nOctave))
//        return 0;       //error
//
//    //analyse accidentals
//    int nAcc = 0;
//    if (sAlter.IsEmpty())
//        nAcc = 0;
//    else if (sAlter.StartsWith( _T("+") ))
//        nAcc = (sAlter.StartsWith( _T("++") ) ? 2 : 1);
//    else if (sAlter.StartsWith( _T("-") ))
//        nAcc = (sAlter.StartsWith(_T("--")) ? -2 : -1);
//    else if (sAlter.StartsWith( _T("=+") ))
//        nAcc = 1;
//    else if (sAlter.StartsWith( _T("=-") ))
//        nAcc = -1;
//    else if (sAlter.StartsWith( _T("=") ))
//        nAcc = 0;
//    else if (sAlter.StartsWith( _T("x") ))
//        nAcc = 2;
//    else
//        return 0;  //error
//
//    //convert to FPitch
//    return FPitch((int)nStep, (int)nOctave, nAcc);  //no error
//}
//
//
//
////----------------------------------------------------------------------------------------
//// MusicXML
////----------------------------------------------------------------------------------------
//
//bool XmlDataToClef(wxString sClefLine, lmEClefType* pClef)
//{
//    if (sClefLine == _T("C1")) {
//        *pClef = lmE_Do1;
//    } else if (sClefLine == _T("C2")) {
//        *pClef = lmE_Do2;
//    } else if (sClefLine == _T("C3")) {
//        *pClef = lmE_Do3;
//    } else if (sClefLine == _T("C4")) {
//        *pClef = lmE_Do4;
//    } else if (sClefLine == _T("G2")) {
//        *pClef = lmE_G;
//    } else if (sClefLine == _T("F3")) {
//        *pClef = lmE_Fa3;
//    } else if (sClefLine == _T("F4")) {
//        *pClef = lmE_Fa4;
//    } else if (sClefLine == _T("SINCLAVE")) {
//        *pClef = lmE_Percussion;
//    } else {
//        return true;    //error
//    }
//
//    return false;
//
//}
//
//bool XmlDataToBarStyle(wxString sBarStyle, lmEBarline* pType)
//{
//    if (sBarStyle == _T("FINREPETICION")) {
//        *pType = lm_eBarlineEndRepetition;
//    } else if (sBarStyle == _T("INICIOREPETICION")) {
//        *pType = lm_eBarlineStartRepetition;
//    } else if (sBarStyle == _T("light-heavy")) {
//        *pType = lm_eBarlineEnd;
//    } else if (sBarStyle == _T("light-light")) {
//        *pType = lm_eBarlineDouble;
//    } else if (sBarStyle == _T("regular")) {
//        *pType = lm_eBarlineSimple;
//    } else if (sBarStyle == _T("heavy-light")) {
//        *pType = lm_eBarlineStart;
//    } else if (sBarStyle == _T("DOBLEREPETICION")) {
//        *pType = lm_eBarlineDoubleRepetition;
//    } else {
//        //TODO Add styles dotted, heavy, heavy-heavy, none
//        //TODO Remove styles FINREPETICION, INICIOREPETICION, DOBLEREPETICION
//        return true;    //error
//    }
//
//    return false;
//}

//---------------------------------------------------------------------------------------
void load_combobox_with_note_names(wxComboBox* pCboBox, DiatonicPitch nSelNote)
{
    pCboBox->Clear();
    for (int i=1; i < 60; i++)
    {
        DiatonicPitch dp(i);
        pCboBox->Append( to_wx_string(dp.get_english_note_name()) );
    }
    DiatonicPitch dp(nSelNote);
    pCboBox->SetValue( to_wx_string(dp.get_english_note_name()) );
}

//---------------------------------------------------------------------------------------
void load_combobox_with_note_names(wxComboBox* pCboBox, wxString sNoteName)
{
    pCboBox->Clear();
    for (int i=1; i < 60; i++)
    {
        DiatonicPitch dp(i);
        pCboBox->Append( to_wx_string(dp.get_english_note_name()) );
    }
    pCboBox->SetValue( sNoteName );
}

//void lmLoadChoiceWithNoteNames(wxChoice* pChoice, wxString sNoteName)
//{
//    int i;
//    pChoice->Clear();
//    for (i=1; i < 60; i++) {
//        pChoice->Append( GetNoteNamePhysicists((DiatonicPitch) i) );
//    }
//    pChoice->SetStringSelection(sNoteName);
//}
//
//wxString LineStyleToLDP(lmELineStyle nStyle)
//{
//    switch(nStyle)
//    {
//        case lm_eLine_None:
//            return _T("none");
//        case lm_eLine_Solid:
//            return _T("solid");
//        case lm_eLine_LongDash:
//            return _T("longDash");
//        case lm_eLine_ShortDash:
//            return _T("shortDash");
//        case lm_eLine_Dot:
//            return _T("dot");
//        case lm_eLine_DotDash:
//            return _T("dotDash");
//        default:
//            wxLogMessage(_T("[LineStyleToLDP] Error: invalid line style %d"), nStyle);
//            wxASSERT(false);
//            return _T("");      //compiler happy
//    }
//};
//
//wxString LineCapToLDP(lmELineCap nLineCap)
//{
//    switch(nLineCap)
//    {
//        case lm_eLineCap_None:
//            return _T("none");
//        case lm_eLineCap_Arrowhead:
//            return _T("arrowhead");
//        case lm_eLineCap_Arrowtail:
//            return _T("arrowtail");
//        case lm_eLineCap_Circle:
//            return _T("circle");
//        case lm_eLineCap_Square:
//            return _T("square");
//        case lm_eLineCap_Diamond:
//            return _T("diamond");
//        default:
//            wxLogMessage(_T("[LineCapToLDP] Error: invalid line cap %d"), nLineCap);
//            wxASSERT(false);
//            return _T("");      //compiler happy
//    }
//};
//
//wxString lmTPointToLDP(lmTPoint& tPoint, const wxString& sName,
//                       bool fEmptyIfZero)
//{
//    //if sName != empty generates "(<sName> dx:<x> dy:<y>)"
//    //else generates "dx:<x> dy:<y>".
//    //if fEmptyIfZero==true values 0.0 are not generated. For instance:
//    //  tPoint=(0.0, 3.7) will generate "dy:3.7"
//    //if both are 0 will return empty string
//
//	wxString sSource = _T("");
//
//    if (fEmptyIfZero && tPoint.x == 0.0f && tPoint.y == 0.0f)
//        return sSource;
//
//    //element name
//    if (sName != wxEmptyString)
//        sSource = _T("(") + sName;
//
//    //dx & dy values
//    if (!(fEmptyIfZero && tPoint.x == 0.0f))
//    {
//		sSource += _T(" dx:");
//        sSource += DoubleToStr((double)tPoint.x, 4);
//    }
//    if (!(fEmptyIfZero && tPoint.y == 0.0f))
//    {
//		sSource += _T(" dy:");
//        sSource += DoubleToStr((double)tPoint.y, 4);
//    }
//
//    //close element
//    if (sName != wxEmptyString)
//        sSource += _T(")");
//
//    return sSource;
//}
//
//wxString lmColorToLDP(const wxColour& nColor, bool fEmptyIfEqual,
//                      const wxColour& nRefColor)
//{
//    //Generates "(color #rrggbb)"
//    //if fEmptyIfEqual==true and nRefColor==nColor returns empty string
//
//    if (fEmptyIfEqual && nRefColor==nColor)
//        return wxEmptyString;
//
//    //element name
//	wxString sSource = _T("(color ");
//    sSource += nColor.GetAsString(wxC2S_HTML_SYNTAX);
//    sSource += _T(")");
//
//    return sSource;
//}
//
//wxString lmFloatToLDP(float rValue, const wxString& sName,
//                      bool fEmptyIfEqual, float rRefValue)
//{
//    //Generates "(<sName> <value>)"
//    //if fEmptyIfEqual==true and rRefValue==rValue returns empty string
//
//    if (fEmptyIfEqual && rRefValue==rValue)
//        return wxEmptyString;
//
//    //element name
//    wxString sSource = wxString::Format(_T("(%s "), sName.c_str());
//	sSource += DoubleToStr((double)rValue, 4);
//    sSource += _T(")");
//
//    return sSource;
//}
//

//---------------------------------------------------------------------------------------
const wxString& get_key_signature_name(EKeySignature type)
{
    static wxString m_name[k_num_keys];
    static bool fStringsLoaded = false;

    if (!fStringsLoaded)
    {
        //language dependent strings. Can not be statically initiallized because
        //then they do not get translated
        m_name[k_key_C] = _("C Major");
        m_name[k_key_G] = _("G Major");
        m_name[k_key_D] = _("D Major");
        m_name[k_key_A] = _("A Major");
        m_name[k_key_E] = _("E Major");
        m_name[k_key_B] = _("B Major");
        m_name[k_key_Fs] = _("F # Major");
        m_name[k_key_Cs] = _("C # Major");
        m_name[k_key_Cf] = _("C b Major");
        m_name[k_key_Gf] = _("G b Major");
        m_name[k_key_Df] = _("D b Major");
        m_name[k_key_Af] = _("A b Major");
        m_name[k_key_Ef] = _("E b Major");
        m_name[k_key_Bf] = _("B b Major");
        m_name[k_key_F] = _("F Major");

        m_name[k_key_a] = _("A minor");
        m_name[k_key_e] = _("E minor");
        m_name[k_key_b] = _("B minor");
        m_name[k_key_fs] = _("F # minor");
        m_name[k_key_cs] = _("C # minor");
        m_name[k_key_gs] = _("G # minor");
        m_name[k_key_ds] = _("D # minor");
        m_name[k_key_as] = _("A # minor");
        m_name[k_key_af] = _("A b minor");
        m_name[k_key_ef] = _("E b minor");
        m_name[k_key_bf] = _("B b minor");
        m_name[k_key_f] = _("F minor");
        m_name[k_key_c] = _("C minor");
        m_name[k_key_g] = _("G minor");
        m_name[k_key_d] = _("D minor");

        fStringsLoaded = true;
    }
    return m_name[type];
}




//=======================================================================================
// global functions related to rendering scores
//=======================================================================================

//wxBitmap GenerateBitmapForKeyCtrol(wxString& sKeyName, EKeySignature nKey)
//{
//    //create a score with an invisible G clef and the key signature
//    lmScore* pScore = new_score();
//    lmInstrument* pInstr = pScore->AddInstrument(0,0,_T(""));   //one vstaff, MIDI channel 0, MIDI instr 0
//    lmVStaff *pVStaff = pInstr->GetVStaff();
//    pScore->SetTopSystemDistance( pVStaff->TenthsToLogical(20, 1) );     // 2 lines
//    pVStaff->AddClef( lmE_Sol, 1, lmNO_VISIBLE );
//    lmStaffObj* pSO = (lmStaffObj*)pVStaff->AddKeySignature(nKey);
//
//    if (sKeyName != _T(""))
//    {
//        //define the font to use for text
//        lmFontInfo tFont = {_T("Tahoma"), 7, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL};
//        lmTextStyle* pStyle = pScore->GetStyleName(tFont);
//        lmTextItem* pText =
//            pVStaff->AddText(sKeyName, lmHALIGN_DEFAULT, pStyle, pSO, lmNEW_ID);
//	    pText->SetUserLocation(20, 70);    //lmTenths
//    }
//
//    pScore->SetPageTopMargin(0.0f);
//    pScore->SetPageLeftMargin( pVStaff->TenthsToLogical(15.0) );     //1.5 lines
//    pScore->SetPageRightMargin( pVStaff->TenthsToLogical(15.0) );    //1.5 lines
//
//    wxBitmap bmp = lmGenerateBitmap(pScore, wxSize(108, 64), 1.0);
//    delete pScore;
//    return bmp;
//}
//
////---------------------------------------------------------------------------------------
//wxBitmap GenerateBitmapForClefCtrol(wxString& sClefName, lmEClefType nClef)
//{
//    //create a score with a clef
//    lmScore* pScore = new_score();
//    lmInstrument* pInstr = pScore->AddInstrument(0,0,_T(""));   //one vstaff, MIDI channel 0, MIDI instr 0
//    lmVStaff *pVStaff = pInstr->GetVStaff();
//    pScore->SetTopSystemDistance( pVStaff->TenthsToLogical(20, 1) );     // 2 lines
//    lmStaffObj* pSO = (lmStaffObj*)pVStaff->AddClef( nClef );
//
//    if (sClefName != _T(""))
//    {
//        //define the font to use for text
//        lmFontInfo tFont = {_T("Tahoma"), 7, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL};
//        lmTextStyle* pStyle = pScore->GetStyleName(tFont);
//        lmTextItem* pText =
//            pVStaff->AddText(sClefName, lmHALIGN_DEFAULT, pStyle, pSO, lmNEW_ID);
//	    pText->SetUserLocation(-10, 85);    //lmTenths
//    }
//
//    pScore->SetPageTopMargin(0.0f);
//    pScore->SetPageLeftMargin( pVStaff->TenthsToLogical(15.0) );     //1.5 lines
//    pScore->SetPageRightMargin( pVStaff->TenthsToLogical(15.0) );    //1.5 lines
//
//    wxBitmap bmp = lmGenerateBitmap(pScore, wxSize(108, 64), 1.0);
//    delete pScore;
//    return bmp;
//}
//
////---------------------------------------------------------------------------------------
//wxBitmap GenerateBitmapForBarlineCtrol(wxString& sName, lmEBarline nBarlineType)
//{
//    //create a score with a barline and its name
//    lmScore* pScore = new_score();
//	pScore->SetOption(_T("Staff.DrawLeftBarline"), false);
//    lmInstrument* pInstr = pScore->AddInstrument(0,0,_T(""));   //one vstaff, MIDI channel 0, MIDI instr 0
//    lmVStaff *pVStaff = pInstr->GetVStaff();
//    pScore->SetTopSystemDistance( pVStaff->TenthsToLogical(20, 1) );     // 2 lines
//	lmStaffObj* pSO = (lmStaffObj*)pVStaff->AddSpacer(40);
//    pVStaff->AddBarline(nBarlineType);
//	pVStaff->AddSpacer(40);
//
//    if (sName != _T(""))
//    {
//        //define the font to use for text
//        lmFontInfo tFont = {_T("Tahoma"), 7, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL};
//        lmTextStyle* pStyle = pScore->GetStyleName(tFont);
//        lmTextItem* pText =
//            pVStaff->AddText(sName, lmHALIGN_DEFAULT, pStyle, pSO, lmNEW_ID);
//	    pText->SetUserLocation(-30, 75);    //lmTenths
//    }
//
//    pScore->SetPageTopMargin(0.0f);
//    pScore->SetPageLeftMargin( pVStaff->TenthsToLogical(15.0) );     //1.5 lines
//    pScore->SetPageRightMargin( pVStaff->TenthsToLogical(15.0) );    //1.5 lines
//
//    wxBitmap bmp = lmGenerateBitmap(pScore, wxSize(108, 64), 1.0);
//    delete pScore;
//    return bmp;
//}
//
////OBSOLETE:
////Obsolete code. Left here as an example on how to write text into a bitmap
////
////wxBitmap GenerateBitmap(lmScore* pScore, wxString& sName, wxSize size, wxSize shift)
////{
////    wxBitmap bitmap = lmGenerateBitmap(pScore, size, 1.0);
////    wxASSERT(bitmap.Ok());
////
////   //write text in black
////    wxMemoryDC dc;
////    dc.SelectObject(bitmap);
////    dc.SetMapMode(wxMM_TEXT);
////    dc.SetUserScale(1.0, 1.0);
////
////    int h, w;
////    dc.SetPen(*wxBLACK);
////    dc.SetFont(*wxNORMAL_FONT);
////    dc.GetTextExtent(sName, &w, &h);
////    dc.DrawText(sName, shift.x+(size.x-w)/2, shift.y+(size.y-h)/2 + h);
////
////    //clean up and return the bitmap
////    dc.SelectObject(wxNullBitmap);
////    return bitmap;
////}

////---------------------------------------------------------------------------------------
//wxBitmap render_on_bitmap(Document* pDoc, wxSize size, double scale)
//{
//    //allocate a bitmap of the specified size, then ask lomse to render the first
//    //document page on the bitmap, and return it.
//
//	wxBitmap bmp(size.x, size.y);

//    //allocate a memory dc
//    wxMemoryDC dc;
//
//    //fill bitmap in white
//    dc.SelectObject(bmp);
//    dc.SetBrush(*wxWHITE_BRUSH);
//	dc.SetBackground(*wxWHITE_BRUSH);
//	dc.Clear();
//
//    // prepare and do renderization
//    //BUG_BYPASS
//    {
//        int nWidth;
//        int nHeight;
//        ::wxDisplaySize(&nWidth, &nHeight);
//#ifdef _LM_LINUX_
//        rScale = rScale * lmSCALE * (1024.0 / (double)nWidth) * 0.8;
//#else
//        rScale = rScale * lmSCALE * (1024.0 / (double)nWidth);
//#endif
//    }
//    //rScale = rScale * lmSCALE;
//
//    //adjust score size to fit in bitmap
//    dc.SetMapMode(lmDC_MODE);
//    dc.SetUserScale( rScale, rScale );
//    lmLUnits xLU = (lmLUnits)dc.DeviceToLogicalXRel(size.x);
//    lmLUnits yLU = (lmLUnits)dc.DeviceToLogicalYRel(size.y);
//    lmUSize uScoreSize = pScore->GetPaperSize();
//    pScore->SetPageSize(xLU, yLU);
//
//    lmPaper oPaper;
//    lmGraphicManager oGraphMngr;
//    oGraphMngr.PrepareToRender(pScore, size.x, size.y, rScale, &oPaper,
//                                lmHINT_FORCE_RELAYOUT);
//    wxBitmap* pBitmap = oGraphMngr.RenderScore(1);
//
//    //restore score size
//    pScore->SetPageSize(uScoreSize.GetWidth(), uScoreSize.GetHeight());
//
//    //clean up and return new bitmap
//    dc.SelectObject(wxNullBitmap);
//    return *pBitmap;
//}



}  //namespace lenmus
