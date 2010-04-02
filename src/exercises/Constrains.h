//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
//-------------------------------------------------------------------------------------

#ifndef __LM_CONSTRAINS_H__        //to avoid nested includes
#define __LM_CONSTRAINS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Constrains.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/Score.h"


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

#define lmNUM_INTVALS  ein_Max_Item     //num intervals considered in constraints

//problem generation & evaluation modes
enum
{
    lm_eLearningMode = 0,
    lm_ePractiseMode,
    lm_eExamMode,
    lm_eQuizMode,
    //
    lm_eNumGenerationModes
};

extern const wxString& lmGetGenerationModeName(long nMode);
 

class lmClefConstrain
{
public:
    lmClefConstrain();
    ~lmClefConstrain() {}
    bool IsValid(lmEClefType nClef) { return m_fValidClefs[nClef-lmMIN_CLEF]; }
    void SetValid(lmEClefType nClef, bool fValid) { m_fValidClefs[nClef-lmMIN_CLEF] = fValid; }

    //pitch scope
    wxString GetLowerPitch(lmEClefType nClef)  { return m_aLowerPitch[nClef-lmMIN_CLEF]; }
    wxString GetUpperPitch(lmEClefType nClef)  { return m_aUpperPitch[nClef-lmMIN_CLEF]; }
    void SetLowerPitch(lmEClefType nClef, wxString sPitch)  {
                m_aLowerPitch[nClef-lmMIN_CLEF] = sPitch;
            }
    void SetUpperPitch(lmEClefType nClef, wxString sPitch)  {
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
    bool IsValid(lmEKeySignatures nKey) { return m_fValidKeys[nKey-lmMIN_KEY]; }
    void SetValid(lmEKeySignatures nKey, bool fValid) { m_fValidKeys[nKey-lmMIN_KEY] = fValid; }
    lmEKeySignatures GetRandomKeySignature();

private:
    bool m_fValidKeys[lmMAX_KEY - lmMIN_KEY + 1];
};

//----------------------------------------------------------------------------------------

class lmTimeSignConstrains
{
public:
    lmTimeSignConstrains();
    ~lmTimeSignConstrains() {}
    bool IsValid(lmETimeSignature nTime) { return m_fValidTimes[nTime-lmMIN_TIME_SIGN]; }
    void SetValid(lmETimeSignature nTime, bool fValid) {
            m_fValidTimes[nTime-lmMIN_TIME_SIGN] = fValid;
        }
    bool SetConstrains(wxString sTimeSigns);

private:
    bool m_fValidTimes[lmMAX_TIME_SIGN - lmMIN_TIME_SIGN + 1];
};


//--------------------------------------------------------------------------------
// Abstract class to create constrains/options objects for lmEBookCtrolOptions
//--------------------------------------------------------------------------------
class lmEBookCtrolOptions
{
public:
    lmEBookCtrolOptions(wxString sSection);
    virtual ~lmEBookCtrolOptions() {}

    virtual void SaveSettings() {}

    void SetSettingsLink(bool fValue) { m_fSettingsLink = fValue; }
    bool IncludeSettingsLink() { return m_fSettingsLink; }
    void SetSection(wxString sSection) {
                m_sSection = sSection;
                LoadSettings();
            }
    inline wxString& GetSection() { return m_sSection; }

    void SetGoBackLink(wxString sURL) { m_sGoBackURL = sURL; }
    bool IncludeGoBackLink() { return m_sGoBackURL != _T(""); }
    wxString GetGoBackURL() { return m_sGoBackURL; }

    void SetPlayLink(bool fValue) { m_fPlayLink = fValue; }
    bool IncludePlayLink() { return m_fPlayLink; }



protected:
    virtual void LoadSettings() {}

    wxString    m_sSection;         //section name to save the constraints
    wxString    m_sGoBackURL;       //URL for "Go back" link of empty string if no link
    bool        m_fPlayLink;        //In theory mode the score could be not playable
    bool        m_fSettingsLink;    //include 'settings' link


};


class lmExerciseOptions : public lmEBookCtrolOptions
{
public:
    lmExerciseOptions(wxString sSection);
    virtual ~lmExerciseOptions() {}

    inline void SetTheoryMode(bool fValue) { m_fTheoryMode = fValue; }
    inline bool IsTheoryMode() { return m_fTheoryMode; }

    inline void SetButtonsEnabledAfterSolution(bool fValue) {
            m_fButtonsEnabledAfterSolution = fValue;
    }
    inline bool ButtonsEnabledAfterSolution() { return m_fButtonsEnabledAfterSolution; }

    inline void SetSolutionLink(bool fValue) { m_fSolutionLink = fValue; }
    inline bool IncludeSolutionLink() { return m_fSolutionLink; }

    inline void SetUsingCounters(bool fValue) { m_fUseCounters = fValue; }
    inline bool IsUsingCounters() { return m_fUseCounters; }

    inline void SetGenerationMode(long nMode) { m_nGenerationMode = nMode; }
    inline long GetGenerationMode() { return m_nGenerationMode; }
    inline bool IsGenerationModeSupported(long nMode) { return m_fSupportedMode[nMode]; }
    inline void SetGenerationModeSupported(long nMode, bool fValue) {
                                            m_fSupportedMode[nMode] = fValue; }


protected:

    //The Ctrol could be used both for ear training exercises and for theory exercises.
    //Following variables are used for configuration
    bool        m_fTheoryMode;

    //options
    bool        m_fButtonsEnabledAfterSolution;
    bool        m_fUseCounters;     //option to not use counters
    bool        m_fSolutionLink;    //include 'show solution' link
    long        m_nGenerationMode;  //problem generation & evaluation mode 
    bool        m_fSupportedMode[lm_eNumGenerationModes];
};



// Options for lmScoreCtrol control
class lmScoreCtrolOptions : public lmEBookCtrolOptions
{
public:
    lmScoreCtrolOptions(wxString sSection);
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

    void SetMetronomeMM(long nValue) { m_nMM = nValue; }
    long GetMetronomeMM() { return m_nMM; }


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

    double      rTopMargin;             //top margin for score, in millimeters

    double      rScale;

private:
    void SetLabels(wxString& sLabel, wxString* pStart, wxString* pStop);

    long        m_nMM;                  // metronome setting

};




#endif  // __LM_CONSTRAINS_H__

