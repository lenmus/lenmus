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

#ifdef __GNUG__
#pragma implementation "ParamsParser.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ParamsParser.h"
#include "../score/Score.h"                 //common enum types
#include "../exercises/ChordConstrains.h"   //EChordType
#include "../auxmusic/ChordManager.h"       //chord name conversion
#include "../ldp_parser/AuxString.h"        //LDPInternalNameToKey


wxString ParseKeys(wxString sParamValue, wxString sFullParam, lmKeyConstrains* pKeys)
{
    bool fError = false;

    if (sParamValue == _T("all")) {
        // allow all key signatures
        int i;
        for (i=0; i <= lmMAX_KEY; i++) {
            pKeys->SetValid((EKeySignatures)i, true);
        }
    }

    else {
        //loop to get all keys
        int iColon;
        wxString sKey;
        EKeySignatures nKey;
        while (sParamValue != _T("")) {
            //get key
            iColon = sParamValue.Find(_T(","));
            if (iColon != -1) {
                sKey = sParamValue.Left(iColon);
                sParamValue = sParamValue.substr(iColon + 1);      //skip the colon
            }
            else {
                sKey = sParamValue;
                sParamValue = _T("");
            }
            nKey = LDPInternalNameToKey(sKey);
            if (nKey == (EKeySignatures)-1) {
                fError = true;
                break;
            }
            pKeys->SetValid(nKey, true);
        }
    }

    if (fError)
        return wxString::Format( wxGetTranslation(
            _T("Invalid param value in:\n<param %s >\n")
            _T("Invalid value = %s \n")
            _T("Acceptable format: list of key signatures or keyword 'all' \n")),
            sFullParam.c_str(), sParamValue.c_str() );
    else
        return wxEmptyString;

}

wxString ParseChords(wxString sParamValue, wxString sFullParam, bool* pfValidChords)
{
    //chords      Keyword "all" or a list of allowed chords:
    //                m-minor, M-major, a-augmented, d-diminished, s-suspended
    //                T-triad, dom-dominant, hd-half diminished

    //                triads: mT, MT, aT, dT, s4, s2
    //                sevenths: m7, M7, a7, d7, mM7, aM7 dom7, hd7
    //                sixths: m6, M6, a6

    bool fError = false;

    if (sParamValue == _T("all")) {
        // allow all chords
        int i;
        for (i=0; i <= ect_Max; i++) {
            *(pfValidChords+i) = true;
        }
    }

    else {
        //loop to get allowed chords
        int iColon;
        wxString sChord;
        EChordType nType;
        while (sParamValue != _T("")) {
            //get chord
            iColon = sParamValue.Find(_T(","));
            if (iColon != -1) {
                sChord = sParamValue.Left(iColon);
                sParamValue = sParamValue.substr(iColon + 1);      //skip the colon
            }
            else {
                sChord = sParamValue;
                sParamValue = _T("");
            }
            nType = ChordShortNameToType(sChord);
            if (nType == (EChordType)-1) {
                fError = true;
                break;
            }
            *(pfValidChords + (int)nType) = true;
        }
    }

    if (fError)
        return wxString::Format( wxGetTranslation(
            _T("Invalid param value in:\n<param %s >\n")
            _T("Invalid value = %s \n")
            _T("Acceptable format: Keyword 'all' or a list of allowed chords.\n")),
            sFullParam.c_str(), sParamValue.c_str() );
    else
        return wxEmptyString;

}

