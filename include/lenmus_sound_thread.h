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

#ifndef __LENMUS_SOUND_THREAD_H__        //to avoid nested includes
#define __LENMUS_SOUND_THREAD_H__

#include <wx/wxprec.h>
#include <wx/wx.h>

#include <vector>

namespace lenmus
{

//---------------------------------------------------------------------------------------
// Play method will be implemented inside a thread, so that normal prgram execution does not
//get suspended while playing
class SoundThread : public wxThread
{
protected:

    ScorePlayerBase* m_pSM;    //parent containing MIDI events table

    //parameters for DoPlaySegment
    int         m_nEvStart;
    int         m_nEvEnd;
    lmEPlayMode   m_nPlayMode;
    bool        m_fVisualTracking;
    bool        m_fCountOff;
    long        m_nMM;
    wxWindow*   m_pWindow;        // window to receive SCORE_HIGHLIGHT events

public:
    SoundThread(ScorePlayerBase* pSM, 
                       int nEvStart, 
                       int nEvEnd,
                       lmEPlayMode nPlayMode,
                       bool fVisualTracking,
                       bool fCountOff,
                       long nMM,
                       wxWindow* pWindow );

    virtual ~SoundThread();

    // thread execution starts here
    void* Entry();

    // called when the thread exits
    void OnExit() {}

};

//---------------------------------------------------------------------------------------
// ScorePlayerBase 
class ScorePlayerBase
{
protected:
    ImoScore*                m_pScore;       //score to play
    SoundThread*   m_pThread;      // play execution thread
    wxWindow*               m_pWindow;      // window to receive SCORE_HIGHLIGHT events
    bool                    m_fPaused;      // execution is paused
    bool                    m_fPlaying;     // playing (control in DoPlaySegment loop)

public:
    ScorePlayerBase(ImoScore* pScore);
    ~ScorePlayerBase();


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

    //only to be used by SoundThread
    void DoPlaySegment(int nEvStart, int nEvEnd,
                     lmEPlayMode nPlayMode,
                     bool fVisualTracking,
                     bool fCountOff,
                     long nMM,
                     wxWindow* pWindow );

    inline void EndOfThread() { m_pThread = (SoundThread*)NULL; }

    inline bool IsPlaying() { return m_fPlaying; }

private:
    void PlaySegment(int nEvStart, int nEvEnd,
                     lmEPlayMode nPlayMode,
                     bool fVisualTracking,
                     bool fCountOff,
                     long nMM,
                     wxWindow* pWindow );

};


}   //namespace lenmus

#endif  // __LENMUS_SOUND_THREAD_H__
