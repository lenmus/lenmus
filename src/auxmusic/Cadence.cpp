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
// AWARE: The maximum number of notes in a chord is defined in 'ChordManager.h', constant
//        lmNOTES_IN_CHORD. Currently its value is 6. Change this value if you need more
//        notes.

static const lmFunctionData m_aFunctionData[] = {
    //                                    minor key      minor key
    //Function        Major key           option 1       option 2
    //-----------    ------------------  -------------   --------------
    {_T("I"),       _T("M3,p5"),        _T("m3,p5"),    _T("nil") },    // 
    {_T("II"),      _T("m3,p5"),        _T("m3,d5"),    _T("m3,p5") },  //
    {_T("IIm"),     _T("?"),            _T("?"),        _T("nil") },    // 
    {_T("III"),     _T("M3,p5"),        _T("M3,p5"),    _T("M3,a5") },  //
    {_T("IV"),      _T("M3,p5"),        _T("m3,p5"),    _T("M3,p5") },  //
    {_T("IVm"),     _T("m3,p5"),        _T("?"),        _T("nil") },    //
    {_T("V"),       _T("M3,p5"),        _T("M3,p5"),    _T("nil") },    //
    {_T("V7"),      _T("M3,p5,m7"),     _T("M3,p5,m7"), _T("nil") },    //
    {_T("Va5"),     _T("M3,a5"),        _T("nil"),      _T("nil") },    //
    {_T("Vd5"),     _T("M3,d5"),        _T("M3,d5"),    _T("nil") },    // 
    {_T("VI"),      _T("m3,p5"),        _T("M3,p5"),    _T("m3,d5") },  //
    {_T("VIaum"),   _T("?"),            _T("?"),        _T("nil") },    // 
    {_T("IVm6"),    _T("nil"),          _T("M3,M6"),    _T("nil") },    //    
    {_T("VII"),     _T("m3,d5"),        _T("#,m3,d5"),  _T("nil") },    //
    {_T("IIb/6"),   _T("m3,m6"),        _T("m3,m6"),    _T("nil") },    //
};


static wxString m_sCadenceName[lm_eCadMaxCadence+1];
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

bool lmCadence::Create(lmECadenceType nCadenceType, EKeySignatures nKey)
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

        //Get root note for this key signature and clef
        wxString sRootNote = GetRootNote(sFunct, nKey, eclvSol);
        g_pLogger->LogTrace(_T("lmCadence"),
                _T("[lmCadence::Create] sFunc='%s', nKey=%d, sRootNote='%s'"),
				sFunct.c_str(), nKey, sRootNote.c_str());
				
       //Prepare the chord
        m_aChord[iC].Create(sRootNote, sIntervals, nKey);
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

    //TODO: how to return inversions?

    //Strip out inversions
    wxString sFunc;
    int nInversion;
    int iSlash = sFunction.find(_T('/'));
    if (iSlash != (int)wxStringBase::npos) {
        sFunc = sFunction.substr(0, iSlash);
        wxString sInv = sFunction.substr(iSlash+1);
        if (sInv==_T("6"))
            nInversion = 1;
        else if (sInv==_T("64"))
            nInversion = 2;
        else {
            wxLogMessage(_T("[lmCadence::SelectChord] Conversion table maintenance error. Unknown inversion code '%s'"), sInv.c_str());
            nInversion = 0;
        }
    }
    else {
        sFunc = sFunction;
        nInversion = 0;
    }

    // look for function
    int iF, iMax = sizeof(m_aFunctionData)/sizeof(lmFunctionData);
    for (iF=0; iF < iMax; iF++) {
        if (m_aFunctionData[iF].sFunction == sFunc) break;
    }
    if (iF == iMax) {
        // table maintenance error
        wxLogMessage(_T("[lmCadence::SelectChord] Conversion table maintenance error. Function '%s' not found. Key=%d"),
            sFunction.c_str(), nKey);
        return _T("");      // not valid chord
    }

    if (IsMajor(nKey))
    {
        // major key: return chord for major key
        wxString sChord = m_aFunctionData[iF].sChordMajor;
        if (sChord == _T("nil"))
            return _T("");        // not valid chord
        if (sChord == _T("?")) {
            wxLogMessage(_T("[lmCadence::SelectChord] Conversion table maintenance error. Undefined chord. Function '%s', Key=%d"),
                sFunction.c_str(), nKey);
            return _T("");        // not valid chord
        }
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

wxString lmCadence::GetRootNote(wxString sFunct, EKeySignatures nKey, EClefType nClef)
{
    // TODO: Take clef into account

    //Get root note for this key signature
    int nRoot = GetRootNoteIndex(nKey);    //index (0..6, 0=Do, 1=Re, 3=Mi, ... , 6=Si) to root note

    // add function grade
    size_t nSize = sFunct.length();
    if (nSize > 2) {
        if (sFunct.compare(0, 3, _T("VII")) == 0)
            nRoot += 6;
        else if (sFunct.compare(0, 3, _T("III")) == 0)
            nRoot += 2;
        else if (sFunct.compare(0, 2, _T("VI")) == 0)
            nRoot += 5;
        else if (sFunct.compare(0, 2, _T("IV")) == 0)
            nRoot += 3;
        else if (sFunct.compare(0, 2, _T("II")) == 0)
            nRoot += 1;
        else if (sFunct.compare(0, 1, _T("V")) == 0)
            nRoot += 4;
    }
    else if (nSize > 1) {
        if (sFunct.compare(0, 2, _T("VI")) == 0)
            nRoot += 5;
        else if (sFunct.compare(0, 2, _T("IV")) == 0)
            nRoot += 3;
        else if (sFunct.compare(0, 2, _T("II")) == 0)
            nRoot += 1;
        else if (sFunct.compare(0, 1, _T("V")) == 0)
            nRoot += 4;
    }
    else if (sFunct.compare(0, 1, _T("V")) == 0)
        nRoot += 4;

    nRoot = nRoot % 7;

    // convert to note name in octave 4
    wxString sNotes = _T("cdefgab");
    wxString sRootNote = sNotes.substr(nRoot,1) + _T("4");
    return sRootNote;

}

wxString lmCadence::GetName()
{
    //get the functions
    int iC;
	wxString sName = CadenceTypeToName(m_nType) + _T(" : ");
    for (iC=0; iC < lmCHORDS_IN_CADENCE; iC++)
    {
        wxString sFunct = aFunction[m_nType][iC];
        if (sFunct == _T("")) break;
		if (iC != 0) sName += _T(" -> ");
		sName += sFunct;
	}
	return sName;

}

//----------------------------------------------------------------------------------------
//global functions
//----------------------------------------------------------------------------------------

wxString CadenceTypeToName(lmECadenceType nType)
{
    wxASSERT(nType <= lm_eCadMaxCadence);

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fStringsInitialized)
    {
	    // Terminal cadences
		    // Perfect authentic cadences
        m_sCadenceName[lm_eCadPerfect_V_I] = _("Perfect");
        m_sCadenceName[lm_eCadPerfect_V7_I] = _("Perfect");
        m_sCadenceName[lm_eCadPerfect_Ic64_V] = _("Perfect");
        m_sCadenceName[lm_eCadPerfect_Va5_I] = _("Perfect");
        m_sCadenceName[lm_eCadPerfect_Vd5_I] = _("Perfect");
			// Plagal cadences
        m_sCadenceName[lm_eCadPlagal_IV_I] = _("Plagal");
        m_sCadenceName[lm_eCadPlagal_iv_I] = _("Plagal");
        m_sCadenceName[lm_eCadPlagal_II_I] = _("Plagal");
        m_sCadenceName[lm_eCadPlagal_ii_I] = _("Plagal");
        m_sCadenceName[lm_eCadPlagal_VI_I] = _("Plagal");

		// Transient cadences
			// Imperfect authentic cadences
	    m_sCadenceName[lm_eCadImperfect_V_I] = _("Imperfect authentic");
			// Deceptive cadences
        m_sCadenceName[lm_eCadDeceptive_V_IV] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_iv] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_VI] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_vi] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_ii] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_III] = _("Deceptive");
        m_sCadenceName[lm_eCadDeceptive_V_VII] = _("Deceptive");
			// Half cadences
        m_sCadenceName[lm_eCadHalf_iic6_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_IV_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_I_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_IV6_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_II_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_IIdimc6_V] = _("Half cadence");
        m_sCadenceName[lm_eCadHalf_VdeVdim5c64_V] = _("Half cadence");
        m_sCadenceName[lm_eCadLastHalf] = _("Half cadence");

        m_fStringsInitialized = true;
    }

    return m_sCadenceName[nType];

}

//int NumNotesInScale(lmECadenceType nType)
//{
//    wxASSERT(nType < est_Max);
//    return 1 + ((tData[nType].sIntervals).Length() / 3);
//}
