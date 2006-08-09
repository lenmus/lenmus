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
/*! @file Constrains.h
    @brief Header file for Constrain derived classes
    @ingroup generators
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __CONSTRAINS_H__        //to avoid nested includes
#define __CONSTRAINS_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/score.h"


enum EIntervalName              // name of the intervals considered in exercises
{
    ein_1 = 0,      // unison       // must start with 0. Used as index for arrays
    ein_2min,
    ein_2maj,
    ein_3min,
    ein_3maj,
    ein_4,
    ein_4aug,
    ein_5,
    ein_6min,
    ein_6maj,
    ein_7min,
    ein_7maj,
    ein_8,
    ein_9min,
    ein_9maj,
    ein_10min,
    ein_10maj,
    ein_11,
    ein_11aug,
    ein_12,
    ein_13min,
    ein_13maj,
    ein_14min,
    ein_14maj,
    ein_2oct,
    ein_Max_Item        //Must be the last one. Just to know how many items
};

#define lmNUM_INTVALS  ein_Max_Item     //num intervals considered in constrains

//----------------------------------------------------------------------------------------

class lmClefConstrain
{
public:
    lmClefConstrain();
    ~lmClefConstrain() {}
    bool IsValid(EClefType nClef) { return m_fValidClefs[nClef-lmMIN_CLEF]; }
    void SetValid(EClefType nClef, bool fValid) { m_fValidClefs[nClef-lmMIN_CLEF] = fValid; }

    //pitch scope
    wxString GetLowerPitch(EClefType nClef)  { return m_aLowerPitch[nClef-lmMIN_CLEF]; }
    wxString GetUpperPitch(EClefType nClef)  { return m_aUpperPitch[nClef-lmMIN_CLEF]; }
    void SetLowerPitch(EClefType nClef, wxString sPitch)  {
                m_aLowerPitch[nClef-lmMIN_CLEF] = sPitch;
            }
    void SetUpperPitch(EClefType nClef, wxString sPitch)  {
                m_aUpperPitch[nClef-lmMIN_CLEF] = sPitch;
            }
 

private:
    bool m_fValidClefs[lmMAX_CLEF - lmMIN_CLEF + 1];
    wxString m_aLowerPitch[lmMAX_CLEF - lmMIN_CLEF + 1];
    wxString m_aUpperPitch[lmMAX_CLEF - lmMIN_CLEF + 1];
};

//----------------------------------------------------------------------------------------

class lmKeyConstrains
{
public:
    lmKeyConstrains();
    ~lmKeyConstrains() {}
    bool IsValid(EKeySignatures nKey) { return m_fValidKeys[nKey-lmMIN_KEY]; }
    void SetValid(EKeySignatures nKey, bool fValid) { m_fValidKeys[nKey-lmMIN_KEY] = fValid; }

private:
    bool m_fValidKeys[lmMAX_KEY - lmMIN_KEY + 1];
};

//----------------------------------------------------------------------------------------

class lmTimeSignConstrains
{
public:
    lmTimeSignConstrains();
    ~lmTimeSignConstrains() {}
    bool IsValid(ETimeSignature nTime) { return m_fValidTimes[nTime-lmMIN_TIME_SIGN]; }
    void SetValid(ETimeSignature nTime, bool fValid) {
            m_fValidTimes[nTime-lmMIN_TIME_SIGN] = fValid; 
        }
    bool SetConstrains(wxString sTimeSigns);

private:
    bool m_fValidTimes[lmMAX_TIME_SIGN - lmMIN_TIME_SIGN + 1];
};

// lmTheoIntervalsConstrains -------------------------------------------------------------

enum EProblemTheoIntervals
{
    ePT_DeduceInterval = 0,         //WARNING: the enum values are used as indexes
    ePT_BuildInterval,              // in DlgCfgTheoIntervals. Do not alter
    ePT_Both                        // neither order nor values. Must start in 0
};


class lmTheoIntervalsConstrains //: public ProblemConstrains
{
public:
    lmTheoIntervalsConstrains();
    ~lmTheoIntervalsConstrains() {}

    bool IsValidClef(EClefType nClef) { return m_oClefs.IsValid(nClef); }
    void SetClef(EClefType nClef, bool fValid) { m_oClefs.SetValid(nClef, fValid); }

    EProblemTheoIntervals GetProblemType() { return m_nProblemType; }
    void SetProblemType(EProblemTheoIntervals nType) { m_nProblemType = nType; }

    bool GetAccidentals() { return m_fAccidentals; }
    void SetAccidentals(bool fValue) { m_fAccidentals = fValue; }

    bool GetDoubleAccidentals() { return m_fDoubleAccidentals; }
    void SetDoubleAccidentals(bool fValue) { m_fDoubleAccidentals = fValue; }

    lmClefConstrain* GetClefConstrains() { return &m_oClefs; }

    void SaveSettings();


private:
    void LoadSettings();

    lmClefConstrain         m_oClefs;
    EProblemTheoIntervals   m_nProblemType;
    bool                    m_fAccidentals;             //allow accidentals
    bool                    m_fDoubleAccidentals;       //allow double accidentals

};


/*! @class lmScoreCtrolOptions
    @brief Options for lmScoreCtrol control
*/
class lmScoreCtrolOptions
{
public:
    lmScoreCtrolOptions() {
            //default values
            fPlayCtrol = false;
            fSolfaCtrol = false;
            fMeasuresCtrol = false;
            fBorder = false;
            fMusicBorder = false;
            sPlayLabel = _("Play");
            sStopPlayLabel = _("Stop");
            sSolfaLabel = _("Read");
            sStopSolfaLabel = _("Stop");
            sMeasuresLabel = _("Measure %d");
            sStopMeasureLabel = _("Stop %d");
        }

    ~lmScoreCtrolOptions() {}

    void SetControlPlay(bool fValue, wxString sLabels = _T(""))
        { 
            fPlayCtrol = fValue;
            if (sLabels != _T(""))
                SetLabels(sLabels, &sPlayLabel, &sStopPlayLabel);
        }
    void SetControlSolfa(bool fValue, wxString sLabels = _T(""))
        { 
            fSolfaCtrol = fValue;
            if (sLabels != _T(""))
                SetLabels(sLabels, &sSolfaLabel, &sStopSolfaLabel);
        }
    void SetControlMeasures(bool fValue, wxString sLabels = _T(""))
        { 
            fMeasuresCtrol = fValue;
            if (sLabels != _T(""))
                SetLabels(sLabels, &sMeasuresLabel, &sStopMeasureLabel);
        }

    
    bool        fPlayCtrol;             //Instert "Play" link
    wxString    sPlayLabel;             //label for "Play" link
    wxString    sStopPlayLabel;         //label for "Stop playing" link

    bool        fSolfaCtrol;            //insert a "Sol-fa" link
    wxString    sSolfaLabel;            //label for "Sol-fa" link
    wxString    sStopSolfaLabel;        //label for "Stop sol-fa" link

    bool        fMeasuresCtrol;         //insert "play-measure" links
    wxString    sMeasuresLabel;
    wxString    sStopMeasureLabel;

    bool        fBorder;                // border around control
    bool        fMusicBorder;           // border around music

private:
    void SetLabels(wxString& sLabel, wxString* pStart, wxString* pStop);

};


/*! @class lmMusicReadingCtrolOptions
    @brief Options for lmTheoMusicReadingCtrol control
*/
class lmMusicReadingCtrolOptions
{
public:
    lmMusicReadingCtrolOptions() {
            //default values
            fPlayCtrol = false;
            fSolfaCtrol = false;
            fBorder = false;
            fGoBackLink = false;
            fSettingsLink = false;
            sPlayLabel = _("Play");
            sStopPlayLabel = _("Stop");
            sSolfaLabel = _("Read");
            sStopSolfaLabel = _("Stop");
        }

    ~lmMusicReadingCtrolOptions() {}
    void SetControlPlay(bool fValue, wxString sLabels = _T(""))
        { 
            fPlayCtrol = fValue;
            if (sLabels != _T(""))
                SetLabels(sLabels, &sPlayLabel, &sStopPlayLabel);
        }
    void SetControlSolfa(bool fValue, wxString sLabels = _T(""))
        { 
            fSolfaCtrol = fValue;
            if (sLabels != _T(""))
                SetLabels(sLabels, &sSolfaLabel, &sStopSolfaLabel);
        }
    void SetControlSettings(bool fValue, wxString sKey =_T(""))
        {
            fSettingsLink = fValue;
            sSettingsKey = sKey;
        }

    void SetGoBackURL(wxString sURL) { fGoBackLink = true; sGoBackURL = sURL; }


    bool        fPlayCtrol;             //Instert "Play" link
    wxString    sPlayLabel;             //label for "Play" link
    wxString    sStopPlayLabel;         //label for "Stop playing" link

    bool        fSolfaCtrol;            //insert a "Sol-fa" link
    wxString    sSolfaLabel;            //label for "Sol-fa" link
    wxString    sStopSolfaLabel;        //label for "Stop sol-fa" link

    bool        fBorder;
    bool        fGoBackLink;            // insert a "Go back to theory" link
    wxString    sGoBackURL;             //URL for "Go back" link

    bool        fSettingsLink;          // insert the settings link
    wxString    sSettingsKey;           // key for saving the user settings

private:
    void SetLabels(wxString& sLabel, wxString* pStart, wxString* pStop);


};



// lmTheoScalesConstrains -------------------------------------------------------------

enum EProblemTheoScales
{
    ePTS_DeduceScale = 0,
    ePTS_BuildScale,
    ePTS_Both
};


class lmTheoScalesConstrains
{
public:
    lmTheoScalesConstrains() {
            m_fCtrolKeySignature = false;
            m_fDrawWithoutKeySignature = true;
            m_fMajor = false;
            m_fMinor = false;
            m_nProblemType = ePTS_Both;
        }
    ~lmTheoScalesConstrains() {}

    bool IsValidClef(EClefType nClef) { return m_oClefs.IsValid(nClef); }
    void SetClef(EClefType nClef, bool fValid) { m_oClefs.SetValid(nClef, fValid); }
    lmClefConstrain* GetClefConstrains() { return &m_oClefs; }

    EProblemTheoScales GetProblemType() { return m_nProblemType; }
    void SetProblemType(EProblemTheoScales nType) { m_nProblemType = nType; }

    void SetMajorType(bool fValue) { m_fMajor = fValue; }
    bool MajorType() { return m_fMajor; }
    void SetMinorType(bool fValue) { m_fMinor = fValue; }
    bool MinorType() { return m_fMinor; }
    void SetCtrolKeySignature(bool fValue) { m_fCtrolKeySignature = fValue; }
    bool CtrolKeySignature() { return m_fCtrolKeySignature; }
    bool DrawWithoutKeySignature() { return m_fDrawWithoutKeySignature; }

private:
    bool                m_fCtrolKeySignature;
    bool                m_fDrawWithoutKeySignature;
    bool                m_fMajor;
    bool                m_fMinor;
    lmClefConstrain     m_oClefs;
    EProblemTheoScales  m_nProblemType;

};

#endif  // __CONSTRAINS_H__

