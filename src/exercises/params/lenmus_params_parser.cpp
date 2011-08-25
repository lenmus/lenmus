//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

#include "lenmus_params_parser.h"

#include "lenmus_chords_constrains.h"   //lmEChordType
#include "lenmus_scales_constrains.h"  //lmEScaleType, Scale name conversion

//// for (compilers that support precompilation, includes <wx/wx.h>.
//#include <wx/wxprec.h>
//
//#include <wx/wx.h>
//
//#include "../score/Score.h"                 //common enum types
//#include "../auxmusic/Chord.h"              //chord name conversion


namespace lenmus
{


string ParseKeys(const string& sParamValue, wxString sFullParam, KeyConstrains* pKeys)
{
//    bool fError = false;
//
//    if (sParamValue == _T("all"))
//    {
//        // allow all key signatures
//        int i;
//        for (i=0; i <= k_max_key; i++) {
//            pKeys->SetValid((EKeySignature)i, true);
//        }
//    }
//
//    else if (sParamValue == _T("allMajor"))
//    {
//        // allow all major key signatures
//        int i;
//        for (i=lmMIN_MAJOR_KEY; i <= lmMAX_MAJOR_KEY; i++) {
//            pKeys->SetValid((EKeySignature)i, true);
//        }
//    }
//
//    else if (sParamValue == _T("allMinor"))
//    {
//        // allow all minor key signatures
//        int i;
//        for (i=lmMIN_MINOR_KEY; i <= lmMAX_MINOR_KEY; i++) {
//            pKeys->SetValid((EKeySignature)i, true);
//        }
//    }
//
//    else
//    {
//        //loop to get all keys
//        int iColon;
//        wxString sKey;
//        EKeySignature nKey;
//        while (sParamValue != _T("")) {
//            //get key
//            iColon = sParamValue.Find(_T(","));
//            if (iColon != -1) {
//                sKey = sParamValue.Left(iColon);
//                sParamValue = sParamValue.substr(iColon + 1);      //skip the colon
//            }
//            else {
//                sKey = sParamValue;
//                sParamValue = _T("");
//            }
//            nKey = LDPNameToKey(sKey);
//            if (nKey == (EKeySignature)-1) {
//                fError = true;
//                break;
//            }
//            pKeys->SetValid(nKey, true);
//        }
//    }
//
//    if (fError)
//        return wxString::Format(
//            _T("Invalid param value in:\n<param %s >\n")
//            _T("Invalid value = %s \n")
//            _T("Acceptable format: list of key signatures or keywords 'all', 'allMajor', 'allMinor' \n"),
//            sFullParam.c_str(), sParamValue.c_str() );
//    else
        return "";

}

string ParseChords(const string& sParamValue, wxString sFullParam, bool* pfValidChords)
{
    //chords      Keyword "all" or a list of allowed chords:
    //                m-minor, M-major, a-augmented, d-diminished, s-suspended
    //                T-triad, dom-dominant, hd-half diminished

    //                triads: mT, MT, aT, dT, s4, s2
    //                sevenths: m7, M7, a7, d7, mM7, aM7 dom7, hd7
    //                sixths: m6, M6, a6

//    bool fError = false;
//
//    if (sParamValue == _T("all")) {
//        // allow all chords
//        int i;
//        for (i=0; i <= ect_Max; i++) {
//            *(pfValidChords+i) = true;
//        }
//    }
//
//    else {
//        //loop to get allowed chords
//        int iColon;
//        wxString sChord;
//        lmEChordType nType;
//        while (sParamValue != _T("")) {
//            //get chord
//            iColon = sParamValue.Find(_T(","));
//            if (iColon != -1) {
//                sChord = sParamValue.Left(iColon);
//                sParamValue = sParamValue.substr(iColon + 1);      //skip the colon
//            }
//            else {
//                sChord = sParamValue;
//                sParamValue = _T("");
//            }
//            nType = lmChordShortNameToType(sChord);
//            if (nType == (lmEChordType)-1) {
//                fError = true;
//                break;
//            }
//            *(pfValidChords + (int)nType) = true;
//        }
//    }
//
//    if (fError)
//        return wxString::Format(
//            _T("Invalid param value in:\n<param %s >\n")
//            _T("Invalid value = %s \n")
//            _T("Acceptable format: Keyword 'all' or a list of allowed chords.\n"),
//            sFullParam.c_str(), sParamValue.c_str() );
//    else
        return "";

}

string ParseScales(const string& sParamValue, wxString sFullParam, bool* pfValidScales)
{
    //scales      Keyword "all" or a list of allowed scales:
    //              major: MN (natural), MH (harmonic), M3 (type III), MM (mixolydian)
    //              minor: mN (natural), mM (melodic), mD (dorian), mH (harmonic)
    //              medieval modes: Do (Dorian), Ph (Phrygian), Ly (Lydian),
    //                              Mx (Mixolydian), Ae (Aeolian), Io (Ionian),
    //                              Lo (Locrian)
    //              other: Pm (Pentatonic minor), PM (Pentatonic Major), Bl (Blues)
    //              non-tonal: WT (Whole Tones), Ch (Chromatic)
    //
    //
    //            Default: "MN, mN, mH, mM"
//
//    bool fError = false;
//
//    if (sParamValue == _T("all"))
//    {
//        // allow all scales
//        for (int i=0; i <= est_Max; i++)
//            *(pfValidScales+i) = true;
//    }
//    else
//    {
//        //disable all scales
//        for (int i=0; i <= est_Max; i++)
//            *(pfValidScales+i) = false;
//
//        //loop to get allowed chords
//        while (sParamValue != _T(""))
//        {
//            //get scale
//            wxString sScale;
//            int iColon = sParamValue.Find(_T(","));
//            if (iColon != -1)
//            {
//                sScale = sParamValue.Left(iColon);
//                sParamValue = sParamValue.substr(iColon + 1);      //skip the colon
//            }
//            else
//            {
//                sScale = sParamValue;
//                sParamValue = _T("");
//            }
//            lmEScaleType nType = lmScaleShortNameToType(sScale);
//            if (nType == (lmEScaleType)-1)
//            {
//                fError = true;
//                break;
//            }
//            *(pfValidScales + (int)nType) = true;
//        }
//    }
//
//    if (fError)
//        return wxString::Format(
//            _T("Invalid param value in:\n<param %s >\n")
//            _T("Invalid value = %s \n")
//            _T("Acceptable format: Keyword 'all' or a list of allowed scales.\n"),
//            sFullParam.c_str(), sParamValue.c_str() );
//    else
        return "";
}

string ParseClef(const string& sParamValue, EClefExercise* pClef)
{
    // clef       'G | F4 | F3 | C4 | C3 | C2 | C1'

//    EClefExercise nClef = LDPNameToClef(sParamValue);
//    if (nClef != -1)
//    {
//        *pClef = nClef;
//        return wxEmptyString;
//    }
//    else
//        return wxString::Format(
//            _T("Invalid param value in:\n<param %s >\n")
//            _T("Invalid value = %s \n")
//            _T("Acceptable values: G | F4 | F3 | C4 | C3 | C2 | C1"),
//            sFullParam.c_str(), sParamValue.c_str() );
        return "";
}


}  //namespace lenmus
