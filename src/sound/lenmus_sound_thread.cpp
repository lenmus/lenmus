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

#include "lenmus_sound_thread.h"

#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
ScorePlayerBase::ScorePlayerBase(ImoScore* pScore)
    : m_numMeasures(0)
    , m_pScore(pScore)
    , m_pThread((SoundThread*) NULL)
    , m_fPlaying(false)
{
}

//---------------------------------------------------------------------------------------
ScorePlayerBase::~ScorePlayerBase()
{
    //if the play thread exists, delete it
    if (m_pThread)
    {
        m_pThread->Delete();
        m_pThread = (SoundThread*)NULL;
    }
}

void ScorePlayerBase::Play(bool fVisualTracking, bool fCountOff,
                        lmEPlayMode nPlayMode, long nMM, wxWindow* pWindow)
{
	//play all the score

    int nEvStart = m_measures[1];     //get first event for firts measure
    int nEvEnd = m_events.size() - 1;

    PlaySegment(nEvStart, nEvEnd, nPlayMode, fVisualTracking,
                fCountOff, nMM, pWindow);
}

void ScorePlayerBase::PlayMeasure(int nMeasure, bool fVisualTracking,
                        lmEPlayMode nPlayMode, long nMM, wxWindow* pWindow)
{
    // Play back measure n (n = 1 ... num_measures)

    //remember:
    //   real measures 1..n correspond to table items 1..n
    //   items 0 and n+1 are fictitius measures for pre and post control events
    int nEvStart = m_measures[nMeasure];
    int nEvEnd = m_measures[nMeasure + 1] - 1;

    PlaySegment(nEvStart, nEvEnd, nPlayMode, fVisualTracking,
                lmNO_COUNTOFF, nMM, pWindow);
}

void ScorePlayerBase::PlayFromMeasure(int nMeasure, bool fVisualTracking, bool fCountOff,
                                     lmEPlayMode nPlayMode, long nMM, wxWindow* pWindow)
{
    // Play back from measure n (n = 1 ... num_measures) to end

    //remember:
    //   real measures 1..n correspond to table items 1..n
    //   items 0 and n+1 are fictitius measures for pre and post control events
    int nEvStart = m_measures[nMeasure];
    while (nEvStart == -1 && nMeasure < m_numMeasures)
    {
        //Current measure is empty. Start in next one
        nEvStart = m_measures[++nMeasure];
    }

    if (nEvStart == -1)
        return;     //all measures are empty after selected one!


    int nEvEnd = m_events.size() - 1;

    PlaySegment(nEvStart, nEvEnd, nPlayMode, fVisualTracking,
                fCountOff, nMM, pWindow);
}


void ScorePlayerBase::PlaySegment(int nEvStart, int nEvEnd,
                               lmEPlayMode nPlayMode,
                               bool fVisualTracking,
                               bool fCountOff,
                               long nMM,
                               wxWindow* pWindow )
{
    // Replay all events in table, from nEvStart to nEvEnd, both included.
    // fCountOff - marcar con el metrónomo un compas completo antes de comenzar la
    //       ejecución. Para que este flag actúe requiere que el lmMetronome esté activo

    if (m_pThread) {
        // A thread exits. If it is paused resume it
        if (m_pThread->IsPaused()) {
            m_pThread->Resume();
            return;
        }
        else {
            // It must be an old thread. Delete it
            m_pThread->Delete();
            m_pThread = (SoundThread*)NULL;
        }
    }

    //Create a new thread. The thread object is created in the suspended state
    m_pThread = new SoundThread(this,
                        nEvStart, nEvEnd, nPlayMode, fVisualTracking,
                        fCountOff, nMM, pWindow);

    if ( m_pThread->Create() != wxTHREAD_NO_ERROR ) {
        //TODO proper error handling
        wxMessageBox(_("Can't create a thread!"));

        m_pThread->Delete();    //to free the memory occupied by the thread object
        m_pThread = (SoundThread*) NULL;
        return;
    }
    m_pThread->SetPriority(WXTHREAD_MAX_PRIORITY);

    //Start the thread execution. This will cause that thread method Enter() is invoked
    //and it will do the job to play the segment.
    if (m_pThread->Run() != wxTHREAD_NO_ERROR ) {
        //TODO proper error handling
        wxMessageBox(_("Can't start the thread!"));

        m_pThread->Delete();    //to free the memory occupied by the thread object
        m_pThread = (SoundThread*) NULL;
        return;
   }
}

void ScorePlayerBase::Stop()
{
    if (!m_pThread) return;

    m_pThread->Delete();    //request the tread to terminate
    m_pThread = (SoundThread*)NULL;
}

void ScorePlayerBase::Pause()
{
    if (!m_pThread) return;

    m_pThread->Pause();
}

void ScorePlayerBase::WaitForTermination()
{
    // Waits until the end of the score playback

    if (!m_pThread) return;

    m_pThread->Delete();
    m_pThread = (SoundThread*)NULL;
}


//---------------------------------------------------------------------------------------
// Methods to be executed in the thread
//---------------------------------------------------------------------------------------

void ScorePlayerBase::DoPlaySegment(int nEvStart, int nEvEnd,
                               lmEPlayMode nPlayMode,
                               bool fVisualTracking,
                               bool fCountOff,
                               long nMM,
                               wxWindow* pWindow )
{
    // This is the real method doing the work. It will execute in the SoundThread

    // if MIDI not operative terminate
    if (!g_pMidi || !g_pMidiOut) {
        // inform that the play has ended
        if (pWindow) {
            lmEndOfPlayEvent event(0);
            ::wxPostEvent( pWindow, event );
        }
        return;
    }

    //TODO All issues related to sol-fa voice

    wxASSERT(nEvStart >= 0 && nEvEnd < (int)m_events.size() );
    if (m_events.size() == 0) return;                  //tabla empty

    #define lmQUARTER_DURATION  64        //duration (LDP units) of a quarter note (to convert to milliseconds)
    #define lmSOLFA_NOTE        60        //pitch for sight reading with percussion sound
    int nPercussionChannel = g_pMidi->MtrChannel();        //channel to use for percussion

    //OK. We start playing. 
    m_fPlaying = true;

    //prepare metronome settings
    lmMetronome* pMtr = g_pMainFrame->GetMetronome();
    bool fPlayWithMetronome = pMtr->IsRunning();
    bool fMetronomeEnabled = pMtr->IsEnabled();
    pMtr->Enable(false);    //mute sound

    // Ask score window to get ready for visual highlight
    bool fVisualHighlight = fVisualTracking && pWindow;
    if (fVisualHighlight && pWindow) {
        lmScoreHighlightEvent event(m_pScore->GetScoreID(), (lmStaffObj*)NULL, ePrepareForHighlight);
        ::wxPostEvent( pWindow, event );
    }

    //Prepare instrument for metronome. Instruments for music voices 
    //are prepared by events of type ProgInstr
    g_pMidiOut->ProgramChange(g_pMidi->MtrChannel(), g_pMidi->MtrInstr());

    //declaration of some time related variables.
    //DeltaTime variables refer to relative time (LDP time)
    //TU = LenMus Time Units. One quarte note = 64TU.
    //Time variables refer to absolute time (milisecs), that is, DeltaTime (TU)
    //converted to real time
    long nEvTime;           //time for next event
    long nMtrEvDeltaTime;   //time for next metronome click

    //default beat and metronome information. It is going to be properly set
    //when a SoundEvent::k_RhythmChange event is found (a time signature object). So these
    //default settings will be used when no time signature in the score.
    long nMtrPulseDuration = lmQUARTER_DURATION;                     //a beat duration, in TU
    long nMtrIntvalOff = wxMin(7, nMtrPulseDuration / 4);            //click sound duration, in TU
    long nMtrIntvalNextClick = nMtrPulseDuration - nMtrIntvalOff;    //interval from click off to next click
    long nMeasureDuration = nMtrPulseDuration * 4;                   //in TU. Assume 4/4 time signature
    long nMtrNumPulses = 4;                                          //assume 4/4 time signature

    //Execute control events that take place before the segment to play, so that
    //instruments and tempo are properly programmed. Continue in the loop while
    //we find control events in segment to play.
    int i = 0;
    bool fContinue = true;
    while (fContinue)
    {
        if (m_events[i]->EventType == SoundEvent::k_prog_instr)
        {
            //change program
            switch (nPlayMode) {
                case ePM_NormalInstrument:
                    g_pMidi->VoiceChange(m_events[i]->Channel, m_events[i]->lmInstrument);
                    break;
                case ePM_RhythmInstrument:
                    g_pMidi->VoiceChange(m_events[i]->Channel, 57);        //57 = Trumpet
                    break;
                case ePM_RhythmPercussion:
                    g_pMidi->VoiceChange(m_events[i]->Channel, 66);        //66 = High Timbale
                    break;
                case ePM_RhythmHumanVoice:
                    //do nothing. Wave sound will be used
                    break;
                default:
                    wxASSERT(false);
            }
        }
        else if (m_events[i]->EventType == SoundEvent::k_rhythm_change)
        {
            //set up new beat and metronome information
            nMeasureDuration = m_events[i]->BeatDuration * m_events[i]->NumBeats;
            nMtrNumPulses = m_events[i]->NumPulses;
            nMtrPulseDuration = nMeasureDuration / nMtrNumPulses;       //a pulse duration, in TU
            nMtrIntvalOff = wxMin(7, nMtrPulseDuration / 4);            //click sound duration (interval to click off), in TU
            nMtrIntvalNextClick = nMtrPulseDuration - nMtrIntvalOff;    //interval from click off to next click, in TU
        }
        else
        {
            // it is not a control event. Continue in the loop only
            // if we have not reached the start of the segment to play
            fContinue = (i < nEvStart);
        }
        if (fContinue) i++;
    }
    //Here i points to the first event of desired measure that is not a control event.

   // metronome interval duration, in milliseconds
   long nMtrClickIntval = (nMM == 0 ? pMtr->GetInterval() : 60000/nMM);

   //Therefore, conversion factor to transform event delta time (TU) into event real
   //time (ms) will be  r = nMtrClickIntval (ms) / nMtrPulseDuration (TU)

    //Define and initialize time counter. If playback starts not at the begining but 
	//in another measure, advance time counter to that measure
    long nTime = 0;
	if (nEvStart > 1) {
		nTime = (pMtr->GetInterval() * m_events[nEvStart]->DeltaTime) / nMtrPulseDuration;
	}


    //first note could be a syncopated note or an off-beat note. In these cases metronome
	//will start before the first note
    nMtrEvDeltaTime = (m_events[i]->DeltaTime / nMtrPulseDuration) * nMtrPulseDuration;

    //generate count off metronome clicks for a full measure
    if (fCountOff)
    {
        for (int i=nMtrNumPulses; i > 1; --i)
        {
            //generate click
            g_pMidiOut->NoteOn(g_pMidi->MtrChannel(), g_pMidi->MtrTone2(), 127);
            wxThread::Sleep((unsigned long)(nMtrClickIntval/2));
            g_pMidiOut->NoteOff(g_pMidi->MtrChannel(), g_pMidi->MtrTone2(), 127);
            wxThread::Sleep((unsigned long)(nMtrClickIntval/2));
        }
        //generate final click
        g_pMidiOut->NoteOn(g_pMidi->MtrChannel(), g_pMidi->MtrTone1(), 127);
        wxThread::Sleep((unsigned long)(nMtrClickIntval/2));
        g_pMidiOut->NoteOff(g_pMidi->MtrChannel(), g_pMidi->MtrTone1(), 127);
        wxThread::Sleep((unsigned long)(nMtrClickIntval/2));
    }

    //loop to execute score events
    bool fFirstBeatInMeasure = true;    //first beat of a measure
    bool fMtrOn = false;                //if true, next metronome event is start
                                        //   metronome click, else stop metronome click
    do
    {
        //if metronome has been just activated compute next metronome event
        if (!fPlayWithMetronome && pMtr->IsRunning()) {
            nMtrEvDeltaTime = ((m_events[i]->DeltaTime / nMtrPulseDuration) + 1) * nMtrPulseDuration;
            fMtrOn = false;
        }
        fPlayWithMetronome = pMtr->IsRunning();

        //Verify if next event should be a metronome click
        if (fPlayWithMetronome && nMtrEvDeltaTime <= m_events[i]->DeltaTime)
        {
            //Next event shoul be a metronome click or the click off event for the previous metronome click
            nEvTime = (nMtrClickIntval * nMtrEvDeltaTime) / nMtrPulseDuration;
            if (nTime < nEvTime) {
                //::wxMilliSleep(nEvTime - nTime);
                wxThread::Sleep((unsigned long)(nEvTime - nTime));
            }

            if (fMtrOn) {
                //the event is the click off for the previous metronome click
                if (fFirstBeatInMeasure) {
                    g_pMidiOut->NoteOff(g_pMidi->MtrChannel(), g_pMidi->MtrTone1(), 127);
                } else {
                    g_pMidiOut->NoteOff(g_pMidi->MtrChannel(), g_pMidi->MtrTone2(), 127);
                }
                //TODO
                //FMain.picMtrLEDOff.Visible = true;
                //FMain.picMtrLEDRojoOn.Visible = false;
                fMtrOn = false;
                nMtrEvDeltaTime += nMtrIntvalNextClick;
            }
            else {
                //the event is a metronome click
                fFirstBeatInMeasure = (nMtrEvDeltaTime % nMeasureDuration == 0);
                if (fFirstBeatInMeasure) {
                    g_pMidiOut->NoteOn(g_pMidi->MtrChannel(), g_pMidi->MtrTone1(), 127);
                } else {
                    g_pMidiOut->NoteOn(g_pMidi->MtrChannel(), g_pMidi->MtrTone2(), 127);
                }
                //TODO
                //FMain.picMtrLEDOff.Visible = false;
                //FMain.picMtrLEDRojoOn.Visible = true;
                fMtrOn = true;
                nMtrEvDeltaTime += nMtrIntvalOff;

            }
            nTime = nEvTime;

        }
        else
        {
            //next even comes from the table. Usually it will be a note on/off
            nEvTime = (nMtrClickIntval * m_events[i]->DeltaTime) / nMtrPulseDuration;   //lmQUARTER_DURATION;
            if (nTime < nEvTime) {
                //::wxMilliSleep((unsigned long)(nEvTime - nTime));
                wxThread::Sleep((unsigned long)(nEvTime - nTime));
            }

            if (m_events[i]->EventType == SoundEvent::k_NoteON)
            {
                //start of note
                switch(nPlayMode)
                {
                    case ePM_NormalInstrument:
                        g_pMidiOut->NoteOn(m_events[i]->Channel, m_events[i]->NotePitch,
                                          m_events[i]->Volume);
                        break;
                    case ePM_RhythmInstrument:
                        g_pMidiOut->NoteOn(m_events[i]->Channel, lmSOLFA_NOTE,
                                          m_events[i]->Volume);
                        break;
                    case ePM_RhythmPercussion:
                        g_pMidiOut->NoteOn(nPercussionChannel, lmSOLFA_NOTE,
                                          m_events[i]->Volume);
                        break;
                    case ePM_RhythmHumanVoice:
                        //WaveOn .NoteStep, m_events[i]->Volume);
                        break;
                    default:
                        wxASSERT(false);
                }

                if (fVisualHighlight && pWindow) {
                    lmScoreHighlightEvent event(m_pScore->GetScoreID(), m_events[i]->pSO, eVisualOn);
                    ::wxPostEvent( pWindow, event );
                }

            }
            else if (m_events[i]->EventType == SoundEvent::k_note_off)
            {
                //finalización de nota
                switch(nPlayMode)
                {
                    case ePM_NormalInstrument:
                        g_pMidiOut->NoteOff(m_events[i]->Channel, m_events[i]->NotePitch, 127);
                    case ePM_RhythmInstrument:
                        g_pMidiOut->NoteOff(m_events[i]->Channel, lmSOLFA_NOTE, 127);
                    case ePM_RhythmPercussion:
                        g_pMidiOut->NoteOff(nPercussionChannel, lmSOLFA_NOTE, 127);
                    case ePM_RhythmHumanVoice:
                        //WaveOff
                        break;
                    default:
                        wxASSERT(false);
                }

                if (fVisualHighlight && pWindow) {
                    lmScoreHighlightEvent event(m_pScore->GetScoreID(), m_events[i]->pSO, eVisualOff);
                    ::wxPostEvent( pWindow, event );
                }
            }
//                else if (m_events[i]->EventType == SoundEvent::k_MarcaEnFRitmos) {
//                    //Marcaje en FRitmos
//                    FRitmos.AvanzarMarca
//
//                }
            else if (m_events[i]->EventType == SoundEvent::k_visual_on)
            {
                //set visual highlight
                if (fVisualHighlight && pWindow) {
                    lmScoreHighlightEvent event(m_pScore->GetScoreID(), m_events[i]->pSO, eVisualOn);
                    ::wxPostEvent( pWindow, event );
                }
            }
            else if (m_events[i]->EventType == SoundEvent::k_visual_off)
            {
                //remove visual highlight
                if (fVisualHighlight && pWindow) {
                    lmScoreHighlightEvent event(m_pScore->GetScoreID(), m_events[i]->pSO, eVisualOff);
                    ::wxPostEvent( pWindow, event );
                }
            }
            else if (m_events[i]->EventType == SoundEvent::k_end_of_score)
            {
                //end of table. Do nothing
            }
            else if (m_events[i]->EventType == SoundEvent::k_rhythm_change)
            {
                //set up new beat and metronome information
                nMeasureDuration = m_events[i]->BeatDuration * m_events[i]->NumBeats;
                nMtrNumPulses = m_events[i]->NumPulses;
                nMtrPulseDuration = nMeasureDuration / nMtrNumPulses;        //a pulse duration
                nMtrIntvalOff = wxMin(7, nMtrPulseDuration / 4);            //click duration (interval to click off)
                nMtrIntvalNextClick = nMtrPulseDuration - nMtrIntvalOff;    //interval from click off to next click
            }
            else if (m_events[i]->EventType == SoundEvent::k_prog_instr)
            {
                //change program
                switch (nPlayMode) {
                    case ePM_NormalInstrument:
                        g_pMidi->VoiceChange(m_events[i]->Channel, m_events[i]->NotePitch);
                        break;
                    case ePM_RhythmInstrument:
                        g_pMidi->VoiceChange(m_events[i]->Channel, 57);        //57 = Trumpet
                        break;
                    case ePM_RhythmPercussion:
                        g_pMidi->VoiceChange(m_events[i]->Channel, 66);        //66 = High Timbale
                        break;
                    case ePM_RhythmHumanVoice:
                        //do nothing. Wave sound will be used
                        break;
                    default:
                        wxASSERT(false);
                }
            }
            else
            {
                //program error. Unknown event type
                //wxASSERT(false);        //TODO remove comment
            }

            nTime = max(nTime, nEvTime);        //to avoid going backwards when no metronome
                                                //before start and progInstr events
            i++;
        }

        if (!m_pThread || m_pThread->TestDestroy()) {
            //Stop playing requested. Exit the loop
            break;
        }

    } while (i <= nEvEnd);

    //restore main metronome
    pMtr->Enable(fMetronomeEnabled);

    //ensure that all visual highlight is removed in case the loop was exited because
    //stop playing was requested
    if (fVisualHighlight && pWindow) {
        lmScoreHighlightEvent event(m_pScore->GetScoreID(), (lmStaffObj*)NULL, eRemoveAllHighlight);
        ::wxPostEvent( pWindow, event );
    }

    //ensure that all sounds are off and that metronome LED is switched off
    g_pMidiOut->AllSoundsOff();
    //TODO    metronome LED
    //FMain.picMtrLEDOff.Visible = true;
    //FMain.picMtrLEDRojoOn.Visible = false;

    //inform that the play has ended
    if (pWindow) {
        lmEndOfPlayEvent event(0);
        ::wxPostEvent( pWindow, event );
    }

    m_fPlaying = false;
}

//================================================================================
// Class SoundThread implementation
//================================================================================

SoundThread::SoundThread(ScorePlayerBase* pSM, int nEvStart, int nEvEnd,
                         lmEPlayMode nPlayMode, bool fVisualTracking,
                         bool fCountOff, long nMM, wxWindow* pWindow)
    : wxThread(wxTHREAD_DETACHED)
{
    m_pSM = pSM;
    m_nEvStart = nEvStart;
    m_nEvEnd = nEvEnd;
    m_nPlayMode = nPlayMode;
    m_fVisualTracking = fVisualTracking;
    m_fCountOff = fCountOff;
    m_nMM = nMM;
    m_pWindow = pWindow;
}

SoundThread::~SoundThread()
{
    m_pSM->EndOfThread();
}

void* SoundThread::Entry()
{

    //ask Sound Manager to play
    //AWARE the checking to see if the thread was asked to exit is done in DoPlaySegment
    m_pSM->DoPlaySegment(m_nEvStart, m_nEvEnd, m_nPlayMode, m_fVisualTracking,
                m_fCountOff, m_nMM, m_pWindow);

    return NULL;
}


}   //namespace lenmus

