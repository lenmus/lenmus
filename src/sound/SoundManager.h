//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#ifndef __LM_SOUNDMANAGER_H__        //to avoid nested includes
#define __LM_SOUNDMANAGER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SoundManager.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <vector>

#include "../score/Score.h"


enum ESoundEventType
{
    // AWARE Event type value is used to sort the events table. 
    // MUST keep order for priority
    eSET_ProgInstr = 1,        //program a new instrument
    eSET_NoteOFF,              //sound off
    eSET_VisualOFF,            //remove visual highlight. No effect on sound
    eSET_RhythmChange,          //change in rithm (time signature)
    eSET_NoteON,               //sound on
    eSET_VisualON,             //add visual highlight. No effect on sound
    eSET_MarcaEnFRitmos,
    eSET_EndOfScore,        //end of table
};

//auxiliary class lmSoundEvent describes a sound event
class lmSoundEvent
{
public:
    lmSoundEvent(float rTime, ESoundEventType nEventType, int nChannel,
                 lmMPitch nMidiPitch, int nVolume, int nStep, lmStaffObj* pSO,
                 int nMeasure);
    ~lmSoundEvent() {}


    long                DeltaTime;        //Relative to metronome speed
    ESoundEventType     EventType;
    int                 Channel;
    union {
        int             NotePitch;          //MIDI pitch
        int             lmInstrument;       //MIDI instrument for eSET_ProgInstr events
        int             NumBeats;          //for RhythmChange events
    };
    int                 Volume;
    union {
        int             NoteStep;       //lmNote step 0..6 : 0-Do, 1-Re, ... 6-Si
        int             BeatDuration;   //for RhythmChange events. In LDP duration units
    };
    lmStaffObj*         pSO;            //staffobj who originated the event (for visual highlight)
    int                 Measure;        //measure number containing this staffobj

};

//lmSoundEvents will be maintained in a dynamic array. Let's declare it
#include "wx/dynarray.h"
WX_DEFINE_ARRAY(lmSoundEvent*, ArraySoundEventPtrs);


class lmSoundManager;        //forward declaration

// Play method will be implemented inside a thread, so that normal prgram execution does not
//get suspended while playing
class lmSoundManagerThread : public wxThread
{
public:
    lmSoundManagerThread(lmSoundManager* pSM, 
                       int nEvStart, 
                       int nEvEnd,
                       lmEPlayMode nPlayMode,
                       bool fVisualTracking,
                       bool fCountOff,
                       long nMM,
                       wxWindow* pWindow );

    ~lmSoundManagerThread();

    // thread execution starts here
    void* Entry();

    // called when the thread exits
    void OnExit() {}

private:

    lmSoundManager* m_pSM;    //parent containing MIDI events table

    //parameters for DoPlaySegment
    int         m_nEvStart;
    int         m_nEvEnd;
    lmEPlayMode   m_nPlayMode;
    bool        m_fVisualTracking;
    bool        m_fCountOff;
    long        m_nMM;
    wxWindow*   m_pWindow;        // window to receive SCORE_HIGHLIGHT events

};


//Class lmSoundManager stores and manages all sound events related to a score
class lmSoundManager
{
public:
    lmSoundManager(lmScore* pScore);
    ~lmSoundManager();

    // sound events table
    void StoreEvent(float rTime, ESoundEventType nEventType, int nChannel,
                    lmMPitch nMidiPitch, int nVolume, int nStep, lmStaffObj* pSO, int nMeasure);
    int GetNumEvents() { return (int)m_aEvents.GetCount(); }
    lmSoundEvent* GetEvent(int i);
    void CloseTable();
    void DeleteEventsTable();
    void Initialize(int nPartes, int nTiempoIni, int nDurCompas, int nNumMeasures);
    void Append(lmSoundManager* pSndMgr);

    //debug
    wxString DumpMidiEvents();

    // playing
    void Play(bool fVisualTracking = lmNO_VISUAL_TRACKING, 
              bool fCountOff = lmNO_COUNTOFF,
              lmEPlayMode nPlayMode = ePM_NormalInstrument,
              long nMM = 0,
              wxWindow* pWindow = (wxWindow*)NULL );

    void PlayMeasure(int nMeasure,
                     bool fVisualTracking = lmNO_VISUAL_TRACKING, 
                     lmEPlayMode nPlayMode = ePM_NormalInstrument,
                     long nMM = 0,
                     wxWindow* pWindow = (wxWindow*)NULL );

    void PlayFromMeasure(int nMeasure,
						bool fVisualTracking = lmNO_VISUAL_TRACKING, 
                        bool fCountOff = lmNO_COUNTOFF,
						lmEPlayMode nPlayMode = ePM_NormalInstrument,
						long nMM = 0,
						wxWindow* pWindow = (wxWindow*)NULL );
    void Stop();
    void Pause();
    void WaitForTermination();

    //only to be used by lmSoundManagerThread
    void DoPlaySegment(int nEvStart, int nEvEnd,
                     lmEPlayMode nPlayMode,
                     bool fVisualTracking,
                     bool fCountOff,
                     long nMM,
                     wxWindow* pWindow );

    inline void EndOfThread() { m_pThread = (lmSoundManagerThread*)NULL; }

    inline bool IsPlaying() { return m_fPlaying; }

private:
    void SortByTime();
    void PlaySegment(int nEvStart, int nEvEnd,
                     lmEPlayMode nPlayMode,
                     bool fVisualTracking,
                     bool fCountOff,
                     long nMM,
                     wxWindow* pWindow );

    //debug
    wxString DumpEventsTable();
    wxString DumpMeasuresTable();


        //member variables

    lmScore*                m_pScore;       //score to play
    lmSoundManagerThread*   m_pThread;      // play execution thread
    wxWindow*               m_pWindow;      // window to receive SCORE_HIGHLIGHT events
    bool                    m_fPaused;      // execution is paused
    bool                    m_fPlaying;     // playing (control in DoPlaySegment loop)


    //sound events table
    ArraySoundEventPtrs        m_aEvents;        //the events table
    int                        m_iEV;            //index to first free entry
    int                     m_nNumMeasures;     //higest measure number in events table

    //measures table 
    std::vector<int>        m_aMeasures;        //index on m_aEvents for the first event of each measure

};

#endif  // __LM_SOUNDMANAGER_H__

