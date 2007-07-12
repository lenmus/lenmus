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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Cadence.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Cadence.h"
#include "Conversion.h"
#include "../ldp_parser/AuxString.h"
#include "../exercises/Generators.h"
#include "../score/KeySignature.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;


// an entry for the table to convert harmonic function to chord intervals
typedef struct lmFunctionDataStruct {
    wxString    sFunction;      // harmonic function 
    wxString    sChordMajor;    // nil if not usable
    wxString    sChordMinor1;   // alternative 1 or nil if not usable in minor mode
    wxString    sChordMinor2;   // alternative 2 or nil if no alternative
} lmFunctionData;


// Conversion table: harmonic function to chord intervals
static const lmFunctionData m_aFunctionData[] = {
    //                                    minor key      minor key
    //Function        Major key           option 1       option 2
    //----------    ------------------  -------------   --------------
    {_T("I"),       _T("M3,p5"),        _T("m3,p5"),    _T("nil") },    // 
    {_T("II"),      _T("m3,p5"),        _T("m3,d5"),    _T("m3,p5") },  //
    {_T("IIm"),     _T("?"),            _T("?"),        _T("nil") },    // 
    {_T("III"),     _T("M3,p5"),        _T("M3,p5"),    _T("M3,a5") },  //
    {_T("IV"),      _T("M3,p5"),        _T("m3,p5"),    _T("M3,p5") },  //
    {_T("IVm"),     _T("m3,p5"),        _T("?"),        _T("nil") },    //
    {_T("V"),       _T("M3,p5"),        _T("M3,p5"),    _T("nil") },    //
    {_T("V7"),      _T("M3,p5,m7"),     _T("M3,p5,m7"), _T("nil") },    //
    {_T("Vaug5"),   _T("M3,a5"),        _T("nil"),      _T("nil") },    //
    {_T("Vdim5"),   _T("M3,d5"),        _T("M3,d5"),    _T("nil") },    // 
    {_T("VI"),      _T("m3,p5"),        _T("M3,p5"),    _T("m3,d5") },  //
    {_T("VIaum"),   _T("?"),            _T("?"),        _T("nil") },    // 
    {_T("IVm6"),    _T("nil"),          _T("M3,M6"),    _T("nil") },    //    
    {_T("VII"),     _T("m3,d5"),        _T("#,m3,d5"),  _T("nil") },    //
    {_T("IIb/6"),   _T("m3,m6"),        _T("m3,m6"),    _T("nil") },    //
};


static wxString m_sCadenceName[lm_eCadMaxCadence];
static bool m_fStringsInitialized = false;


// AWARE: Array indexes are in correspondence with enum lmECadenceType
// AWARE: Change constant lmCHORDS_IN_CADENCE to increment number of chords
static const wxString aFunction[lm_eCadMaxCadence][lmCHORDS_IN_CADENCE] = {
    // Perfect authentic cadences
    { _T("V"),      _T("I") },      //Perfect_V_I
    { _T("V7"),     _T("I") },      //Perfect_V7_I
    { _T("I/64"),   _T("V") },      //Perfect_Ic64_V
    { _T("Va5"),    _T("I") },      //Perfect_Va5_I
    { _T("Vd5"),    _T("I") },      //Perfect_Vd5_I
    // Plagal cadences
    { _T("IV"),     _T("I") },      //Plagal_IV_I
    { _T("IVm"),    _T("I") },      //Plagal_iv_I
    { _T("II"),     _T("I") },      //Plagal_II_I
    { _T("IIm"),    _T("I") },      //Plagal_ii_I
    { _T("VI"),     _T("I") },      //Plagal_VI_I
    // Imperfect authentic cadences
	{ _T("V/6"),    _T("I") },      //Imperfect_V_I
    // Deceptive cadences
    { _T("V"),      _T("IV") },     //Deceptive_V_IV
    { _T("V"),      _T("IVm") },    //Deceptive_V_iv
    { _T("V"),      _T("VI") },     //Deceptive_V_VI
    { _T("V"),      _T("VIm") },    //Deceptive_V_vi
    { _T("V"),      _T("IIm") },    //Deceptive_V_ii
    { _T("V"),      _T("III") },    //Deceptive_V_III
    { _T("V"),      _T("VII") },    //Deceptive_V_VII
    // Half cadences
    { _T("IIm/6"),  _T("V") },      //Half_iic6_V
    { _T("IV"),     _T("V") },      //Half_IV_V
    { _T("I"),      _T("V") },      //Half_I_V
    { _T("IV6"),    _T("V") },      //Half_IV6_V
    { _T("II"),     _T("V") },      //Half_II_V
    { _T("IId/6"),  _T("V") },      //Half_IIdimc6_V
    { _T("IIb/6"),  _T("V") },      //Half_VdeVdim5c64_V
};



//-------------------------------------------------------------------------------------
// Implementation of lmCadence class


lmCadence::lmCadence()
{
    m_fCreated = false;
    m_nNumChords = 0;
}

bool lmCadence::Create(wxString sRootNote, lmECadenceType nCadenceType,
                       EKeySignatures nKey)
{
    // return true if cadence created


    //save parameters
    m_nType = nCadenceType;
    m_nKey = nKey;

    //get chords
    int iC;
    for (iC=0; iC < lmCHORDS_IN_CADENCE; iC++)
    {
        // get the function
        wxString sFunct = aFunction[nCadenceType][iC];
        
        // if no function, exit loop. No more chords in cadence
        if (sFunct == _T("")) {
            if (iC > 1) break;      //no error
            //table maintenace error
            wxLogMessage(_T("[lmCadence::Create] No harmonic function!. nCadenceType=%d, iC=%d"),
                    nCadenceType, iC);
            return false;
        }

        //get the chord intervals
        wxString sIntervals = SelectChord(aFunction[nCadenceType][iC], nKey);
        if (sIntervals == _T("")) {
            //error: no chord for choosen function
            wxLogMessage(_T("[lmCadence::Create] No chord found for nCadenceType=%d, nKey=%d, iC=%d"),
                    nCadenceType, nKey, iC);
            return false;
        }

        //Prepare the chord
        m_aChord[iC].Create(sIntervals, sRootNote);
        m_nNumChords++;
    }

    m_fCreated = true;
    return true;            
}

lmCadence::~lmCadence()
{
}

lmChordManager* lmCadence::GetChord(int iC)
{
    // returns a pointer to chord iC (0..n-1).
    // if iC is out of range returns NULL pointer

    if (iC >=0 && iC < m_nNumChords)
        return &m_aChord[iC];
    else
        return (lmChordManager*) NULL;
}


wxString lmCadence::SelectChord(wxString sFunction, EKeySignatures nKey)
{
    // return the intervals that form the chord, or empty string if errors


    // look for function
    int iF, iMax = sizeof(m_aFunctionData)/sizeof(lmFunctionData);
    for (iF=0; iF < iMax; iF++) {
        if (m_aFunctionData[iF].sFunction == sFunction) break;
    }
    if (iF == iMax) {
        // table maintenance error
        wxLogMessage(_T("[]"), sFunction.c_str(), nKey);
        return _T("");      // not valid chord
    }

    if (IsMajor(nKey))
    {
        // major key: return chord for major key
        wxString sChord = m_aFunctionData[iF].sChordMajor;
        if (sChord == _T("nil"))
            return _T("");        // not valid chord
        else
            return sChord;
    }
    else
    {
        // minor key: return one of the chords for minor key
        wxString sChord1 = m_aFunctionData[iF].sChordMinor1;
        wxString sChord2 = m_aFunctionData[iF].sChordMinor2;
        if (sChord1 == _T("nil"))
            return _T("");        // not valid chord
        else if (sChord2 == _T("nil"))
            return sChord1;
        else {
            lmRandomGenerator oRnd;
            if (oRnd.FlipCoin())
                return sChord1;
            else
                return sChord2;
       }
    }
}


//----------------------------------------------------------------------------------------
//global functions
//----------------------------------------------------------------------------------------

//wxString ScaleTypeToName(lmECadenceType nType)
//{
//    wxASSERT(nType < est_Max);
//
//    //language dependent strings. Can not be statically initiallized because
//    //then they do not get translated
//    if (!m_fStringsInitialized)
//    {
//        // Major scales
//        m_sCadenceName[est_MajorNatural] = _("Major natural");
//        m_sCadenceName[est_MajorTypeII] = _("Major type II");
//        m_sCadenceName[est_MajorTypeIII] = _("Major type III");
//        m_sCadenceName[est_MajorTypeIV] = _("Major type IV");
//
//        // Minor scales
//        m_sCadenceName[est_MinorNatural] = _("Minor natural");
//        m_sCadenceName[est_MinorDorian] = _("Minor Dorian");
//        m_sCadenceName[est_MinorHarmonic] = _("Minor Harmonic");
//        m_sCadenceName[est_MinorMelodic] = _("Minor Melodic");
//
//        // Greek scales
//        m_sCadenceName[est_GreekIonian] = _("Greek Ionian");
//        m_sCadenceName[est_GreekDorian] = _("Greek Dorian");
//        m_sCadenceName[est_GreekPhrygian] = _("Greek Phrygian");
//        m_sCadenceName[est_GreekLydian] = _("Greek Lydian");
//        m_sCadenceName[est_GreekMixolydian] = _("Greek Mixolydian");
//        m_sCadenceName[est_GreekAeolian] = _("Greek Aeolian");
//        m_sCadenceName[est_GreekLocrian] = _("Greek Locrian");
//
//        // Other scales
//        m_sCadenceName[est_PentatonicMinor] = _("Pentatonic minor");
//        m_sCadenceName[est_PentatonicMajor] = _("Pentatonic major");
//        m_sCadenceName[est_Blues] = _("Blues");
//        m_sCadenceName[est_WholeTones] = _("Whole tones");
//        m_sCadenceName[est_Chromatic] = _("Chromatic");
//
//        m_fStringsInitialized = true;
//    }
//
//    return m_sCadenceName[nType];
//
//}
//
//int NumNotesInScale(lmECadenceType nType)
//{
//    wxASSERT(nType < est_Max);
//    return 1 + ((tData[nType].sIntervals).Length() / 3);
//}
