// RCS-ID: $Id: SoundManager.h,v 1.4 2006/02/23 19:25:44 cecilios Exp $
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
/*! @file SoundManager.h
    @brief Header file for class lmSoundManager
    @ingroup sound_management
*/
#ifndef __SOUNDMANAGER_H__        //to avoid nested includes
#define __SOUNDMANAGER_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/score.h"


enum ESoundEventType
{
    // @attention Event type value is used to sort the events table. 
    // MUST keep order for priority
    eSET_ProgInstr = 1,        //program a new instrument
    eSET_NoteOFF,              //sound off
    eSET_VisualOFF,            //remove visual highlight. No effect on sound
    eSET_RithmChange,          //change in rithm (time signature)
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
                 lmPitch nMidiPitch, int nVolume, int nStep, lmStaffObj* pSO,
                 int nMeasure);
    ~lmSoundEvent() {}


    long                DeltaTime;        //Relative to metronome speed
    ESoundEventType     EventType;
    int                 Channel;
    union {
        int             NotePitch;          //MIDI pitch
        int             lmInstrument;       //MIDI instrument for eSET_ProgInstr events
        int             NumBeats;          //for RithmChange events
    };
    union {
        int             Volume;
        int             BeatDuration;   //for RithmChange events. In LDP duration units
    };
    int                 NoteStep;       //lmNote step 0..6 : 0-Do, 1-Re, ... 6-Si
    lmStaffObj*         pSO;            //staffobj who originated the event (for visual highlight)
    int                 Measure;        //measure number containing this staffobj

};

//sbSoundEvents will be maintained in a dynamic array. Let's declare it
#include "wx/dynarray.h"
WX_DEFINE_ARRAY(lmSoundEvent*, ArraySoundEventPtrs);
//WX_DEFINE_ARRAY_LONG(long, sbArrayOfLong);


class lmSoundManager;        //forward declaration

// Play method will be implemented inside a thread, so that normal prgram execution does not
//get suspended while playing
class lmSoundManagerThread : public wxThread
{
public:
    lmSoundManagerThread(lmSoundManager* pSM, 
                       int nEvStart, 
                       int nEvEnd,
                       EPlayMode nPlayMode,
                       bool fVisualTracking,
                       bool fMarcarUnCompasPrevio,
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
    EPlayMode   m_nPlayMode;
    bool        m_fVisualTracking;
    bool        m_fMarcarUnCompasPrevio;
    long        m_nMM;
    wxWindow*   m_pWindow;        // window to receive SCORE_HIGHLIGHT events

};


//Class lmSoundManager stores and manages all sound events related to a score
class lmSoundManager
{
public:
    lmSoundManager();
    ~lmSoundManager();

    // sound events table
    void StoreEvent(float rTime, ESoundEventType nEventType, int nChannel,
                    lmPitch nMidiPitch, int nStep, lmStaffObj* pSO, int nMeasure);
    int GetNumEvents() { return (int)m_aEvents.GetCount(); }
    lmSoundEvent* GetEvent(int i);
    void CloseTable();
    void DeleteEventsTable();
    void Initialize(int nPartes, int nTiempoIni, int nDurCompas, int nNumMeasures);
    void Append(lmSoundManager* pSndMgr);
    wxString DumpMidiEvents();

    // playing
    void Play(bool fVisualTracking = lmNO_VISUAL_TRACKING, 
              bool fMarcarCompasPrevio = NO_MARCAR_COMPAS_PREVIO,
              EPlayMode nPlayMode = ePM_NormalInstrument,
              long nMM = 0,
              wxWindow* pWindow = (wxWindow*)NULL );
    void PlayMeasure(int nMeasure,
                     bool fVisualTracking = lmNO_VISUAL_TRACKING, 
                     EPlayMode nPlayMode = ePM_NormalInstrument,
                     long nMM = 0,
                     wxWindow* pWindow = (wxWindow*)NULL );
    void Stop();
    void Pause();
    void WaitForTermination();

    // measures tables
    void StoreMeasureStartTime(int nMeasure, float rTime);
    int  GetNumMeasures() { return (int)m_aStartTime.GetCount(); }
    long GetStartTime(int nMeasure) { return m_aStartTime.Item(nMeasure); }

    //only to be used by lmSoundManagerThread
    void DoPlaySegment(int nEvStart, int nEvEnd,
                     EPlayMode nPlayMode,
                     bool fVisualTracking,
                     bool fMarcarUnCompasPrevio,
                     long nMM,
                     wxWindow* pWindow );


private:
    void SortByTime();
    void PlaySegment(int nEvStart, int nEvEnd,
                     EPlayMode nPlayMode,
                     bool fVisualTracking,
                     bool fMarcarUnCompasPrevio,
                     long nMM,
                     wxWindow* pWindow );




        //member variables

    lmSoundManagerThread*    m_pThread;        // play execution thread
    wxWindow*                m_pWindow;        // window to receive SCORE_HIGHLIGHT events
    bool                     m_fPaused;        // execution is paused


    //sound events table
    ArraySoundEventPtrs        m_aEvents;        //the events table
    int                        m_iEV;            //index to first free entry

    //measures tables 
    wxArrayInt        m_aMeasures;        //index on m_aEvents for the first event of each measure
    wxArrayLong       m_aStartTime;       //start time for each measure

    //information about the score
    int        m_nTiempoIni;            //tiempo en silencio al inicio de la partitura (anacrusa)
        //Si el primer compas no es anacrusa, m_nTiempoLibre se inicializa con la duraci�n del
        //compas, de forma que se retrase todo un compas y se fuerce as� a marcar un compas completo
        //antes de comenzar la melod�a
    int        m_nPartesCompas;        //n�mero de partes por compas (golpes de metr�nomo)
    int        m_nDuracionCompas;        //tiempo que dura un compas
    int        m_nNumCompases;            //num de compases reales de la partitura

};

#endif  // __SOUNDMANAGER_H__
