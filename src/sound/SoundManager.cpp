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
#pragma implementation "SoundManager.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "SoundManager.h"

//use of Metonome
#include "../sound/Metronome.h"

//access to Midi configuration
#include "../sound/MidiManager.h"

//access to MainFrame to get metronome settings
#include "../app/MainFrame.h"
extern lmMainFrame *g_pMainFrame;


//-----------------------------------------------------------------------------------------
//! @class lmSoundEvent
//! @brief Auxiliary class to represent an event of the events table
//-----------------------------------------------------------------------------------------

lmSoundEvent::lmSoundEvent(float rTime, ESoundEventType nEventType, int nChannel,
                           lmPitch nMidiPitch, int nVolume, int nStep, lmStaffObj* pStaffObj,
                           int nMeasure)
{
    DeltaTime = (long)(rTime + 0.5);
    EventType = nEventType;
    Channel = nChannel;
    NotePitch = nMidiPitch;
    NoteStep = nStep;
    Volume = nVolume;
    pSO = pStaffObj;
    Measure = nMeasure;

}



//-----------------------------------------------------------------------------------------
//!   @class lmSoundManager
//    @brief Manager for the events table
//
//    There are three tables to maintain:
//    - m_aEvents: contains the MIDI events.
//    - m_aMeasures (wxArrayInt):
//        It contains the index over m_aEvents for the first event of each measure.
//    - m_aStartTime (wxArrayLong): Also, each item corresponds to a measure and
//        contains the start time for that measure.
//
//    @attention
//    Measures are numbered 1..n (musicians usual way) not 0..n-1. But tables
//    go 0..n+1 :
//      - Item 0 corresponds to control events before the start of the first
//        measure.
//      - Item n+1 corresponds to control events after the final bar, normally only
//        the EndOfTable control event.
//      - Items 1..n corresponds to the real measures 1..n.
//    In the events table m_aEvents, all events not in a real measure (measures 1..n) are
//    market as belonging to measure 0.
//
//    The three tables must be synchronized but are populated at different times and by
//    different methods:
//    - First, StoreMeasureStartTime() is invoked at the start of each measure.
//    - Then, StoreEvent() is invoked many times to store the events of that measure.
//    This process is repeated for every lmVStaff and lmInstrument and all tables are merged.
//    Finally, table m_aMeasures is computed.
//
//
//-----------------------------------------------------------------------------------------

lmSoundManager::lmSoundManager()
{
    m_pThread = (lmSoundManagerThread*) NULL;
}


void lmSoundManager::Initialize(int nPartes, int nTiempoIni, int nDurCompas, int nNumCompases)
{
    //
    // This method MUST BE invoked before using the table. Can be invoked later, at any time,
    // to reuse the object
    //

    //delete events in table
    for (int i = m_aEvents.GetCount(); i > 0; i--) {
        delete m_aEvents.Item(i-1);
        m_aEvents.RemoveAt(i-1);
    }

    //delete measures table
    m_aMeasures.Clear();

    //store information about the score
    m_nTiempoIni = nTiempoIni;
    m_nPartesCompas = nPartes;
    m_nDuracionCompas = nDurCompas;
    m_nNumCompases = nNumCompases;

}


void lmSoundManager::DeleteEventsTable()
{
    //delete events in table
    for (int i = m_aEvents.GetCount(); i > 0; i--) {
        delete m_aEvents.Item(i-1);
        m_aEvents.RemoveAt(i-1);
    }

}

lmSoundManager::~lmSoundManager()
{
    //@attention Sound manager destructor MUST NOT delete the events table, as the table
    //could be appended to other table.
    //So the table MUST BE explicitly deleted by calling DeleteEventsTable() when required

    //if the play thread exists, delete it
    if (m_pThread) {
        m_pThread->Delete();
        m_pThread->Wait();
        delete m_pThread;
        m_pThread = (lmSoundManagerThread*)NULL;
    }


}

void lmSoundManager::StoreEvent(float rTime, ESoundEventType nEventType, int nChannel,
                           lmPitch nMidiPitch, int nVolume, int nStep, lmStaffObj* pSO, int nMeasure)
{
    //create the event and add it to the table
    lmSoundEvent* pEvent = new lmSoundEvent(rTime, nEventType, nChannel, nMidiPitch,
                                            nVolume, nStep, pSO, nMeasure);
    m_aEvents.Add(pEvent);        //add event to table

}

void lmSoundManager::StoreMeasureStartTime(int nMeasure, float rTime)
{
    //wxMessageBox( wxString::Format( _T("measure=%d, count=%d"), nMeasure, m_aStartTime.GetCount() ));
    if (m_aStartTime.GetCount() == 0) {
        //add start time for fictitius control measure 0
       m_aStartTime.Add(0L);
    }
    wxASSERT(nMeasure == (int)m_aStartTime.GetCount());        //remember: nMeasure is 1 based
    m_aStartTime.Add((long)(rTime + 0.5));
}

void lmSoundManager::Append(lmSoundManager* pSndMgr)
{
    //
    // Add to this object tables the entries from the tables received
    //

        //
        // Merge m_aEvents table
        //

    //THINK: Next commented code doesn't work. Alloc doesn't reallocate existing items; it
    //just allocates empty space; existing items are just destroyed!
    int nNewRows = pSndMgr->GetNumEvents();
    //int nCurSize = m_aEvents.GetCount();
    //m_aEvents.Alloc((size_t)(nCurSize + nNewRows));        //allocate more space if needed

    //loop to copy entries
    for (int i=0; i < nNewRows; i++) {
        m_aEvents.Add( pSndMgr->GetEvent(i) );
    }

        //
        // Merge m_aStartTime table. All measures should be equal in the different
        // instruments and VSTaffs, so the merge process is just to verify tables
        //

    //tables must have equal sizes unless one of them is empty
    int nNewTableSize = pSndMgr->GetNumMeasures();
    int nCurTableSize = m_aStartTime.GetCount();
    if (nCurTableSize == 0)
    {
        //current table is empty. Copy the appended table
        for (int i=0; i < nNewTableSize; i++) {
            m_aStartTime.Add( pSndMgr->GetStartTime(i) );
        }
    }
    else if (nNewTableSize != nCurTableSize) {
        wxMessageBox( wxString::Format( _T("[lmSoundManager::Append] StartTime tables have different sizes (%d, %d)"),
            nNewTableSize, nCurTableSize ));
        wxASSERT(false);
    }
    else
    {
        //verify times
        for (int i=0; i < nCurTableSize; i++) {
            if (m_aStartTime[i] != pSndMgr->GetStartTime(i)) {
                wxMessageBox( wxString::Format( _T("[lmSoundManager::Append] Measure %d has different duration: old %d, new %d"),
                    i, m_aStartTime[i], pSndMgr->GetStartTime(i) ));
                wxASSERT(false);
            }
        }
    }

}

lmSoundEvent* lmSoundManager::GetEvent(int i)
{
    wxASSERT(i >= 0 && i < (int)m_aEvents.GetCount());
    return m_aEvents.Item(i);
}

void lmSoundManager::CloseTable()
{
    //sort table by time
    SortByTime();

    //Add an EndOfScore event
    StoreEvent( (float)(m_aEvents.Item(m_aEvents.GetCount()-1)->DeltaTime), eSET_EndOfScore,
            0, 0, 0, 0, (lmStaffObj*)NULL, 0);

    //Create the table of measures
    int nM = -1;
    for (int i=0; i < (int)m_aEvents.GetCount(); i++) {
        if (m_aEvents.Item(i)->Measure != nM) {
            m_aMeasures.Add(i);
            nM++;
            wxASSERT(nM == m_aEvents.Item(i)->Measure || 0 == m_aEvents.Item(i)->Measure);
        }
    }

}

wxString lmSoundManager::DumpMidiEvents()
{
    wxString sMsg = wxEmptyString;

    if (m_aEvents.GetCount() == 0) {
        sMsg = _T("There are no MIDI events");
    }
    else {
        //headers
        sMsg = _T("Num.\tTime\tChannel\tMeas.\tEvent\t\tPitch\tStep\tVolume\n");

        lmSoundEvent* pSE;
        for(int i=0; i < (int)m_aEvents.GetCount(); i++) {
            //division line every four entries
            if (i % 4 == 0) {
                sMsg += _T("-------------------------------------------------------------\n");
            }

            //list current entry
            pSE = m_aEvents.Item(i);
            sMsg += wxString::Format( _T("%4d:\t%d\t%d\t%d\t"),
                        i, pSE->DeltaTime, pSE->Channel, pSE->Measure);
            switch (pSE->EventType)
            {
                case eSET_NoteON:
                    sMsg += _T("ON        ");
                    break;
                case eSET_NoteOFF:
                    sMsg += _T("OFF       ");
                    break;
                case eSET_VisualON:
                    sMsg += _T("VISUAL ON ");
                    break;
                case eSET_VisualOFF:
                    sMsg += _T("VISUAL OFF");
                    break;
                case eSET_EndOfScore:
                    sMsg += _T("END TABLE ");
                    break;
                case eSET_MarcaEnFRitmos:
                    sMsg += _T("CTROL     ");
                    break;
                case eSET_RhythmChange:
                    sMsg += _T("RITHM CHG ");
                    break;
                case eSET_ProgInstr:
                    sMsg += _T("PRG INSTR ");
                    break;
                default:
                    sMsg += wxString::Format(_T("?? %d"), pSE->EventType);
            }
            sMsg += wxString::Format(_T("\t%d\t%d\t%d\n"),
                        pSE->NotePitch, pSE->NoteStep, pSE->Volume);
        }

        // measures start time table and first event for each measure
        sMsg += wxString::Format( _T("\n\nMeasures start times and first event (%d measures)\n\n"),
                    m_aStartTime.GetCount() );
        sMsg += _T("Num.\tTime\tEvent\n");
        for(int i=0; i < (int)m_aStartTime.GetCount(); i++) {
            //division line every four entries
            if (i % 4 == 0) {
                sMsg += _T("-------------------------------------------------------------\n");
            }
            sMsg += wxString::Format( _T("%4d:\t%d\t%d\n"),
                            i, m_aStartTime[i], m_aMeasures[i]);
       }

    }

    return sMsg;

}

void lmSoundManager::SortByTime()
{
    //
    // Sort events by time, measure and event type. Uses the bubble sort algorithm
    //

    int j, k;
    bool fChanges;
    int nNumElements = m_aEvents.GetCount();
    lmSoundEvent* pEvAux;

    for (int i = 0; i < nNumElements; i++)
    {
        fChanges = false;
        j = nNumElements - 1;

        while ( j != i )
        {
            k = j - 1;
            if ((m_aEvents[j]->DeltaTime < m_aEvents[k]->DeltaTime) ||
                ((m_aEvents[j]->DeltaTime == m_aEvents[k]->DeltaTime) &&
                 ((m_aEvents[j]->Measure < m_aEvents[k]->Measure) ||
                  (m_aEvents[j]->EventType < m_aEvents[k]->EventType))))
            {
                //interchange elements
                pEvAux = m_aEvents[j];
                m_aEvents[j] = m_aEvents[k];
                m_aEvents[k] = pEvAux;
                fChanges = true;
            }
            j = k;
        }

        //If there were no changes in this loop step it implies that the table is ordered;
        //in this case exit loop to save time
        if (!fChanges) break;
    }

}

void lmSoundManager::Play(bool fVisualTracking, bool fMarcarCompasPrevio,
                        EPlayMode nPlayMode, long nMM, wxWindow* pWindow)
{
    int nEvStart = m_aMeasures.Item(1);     //get first event for firts measure
    PlaySegment(nEvStart, m_aEvents.GetCount() - 1, nPlayMode, fVisualTracking,
                fMarcarCompasPrevio, nMM, pWindow);
}

void lmSoundManager::PlayMeasure(int nMeasure, bool fVisualTracking,
                        EPlayMode nPlayMode, long nMM, wxWindow* pWindow)
{
    //
    // Play back measure n (n = 1 ... num_measures)
    //

    int nEvStart, nEvEnd;

    //remember:
    //   real measures 1..n correspond to table items 1..n
    //   items 0 and n+1 are fictitius measures for pre and post control events
    nEvStart = m_aMeasures.Item(nMeasure);
    nEvEnd = m_aMeasures.Item(nMeasure + 1) - 1;

    PlaySegment(nEvStart, nEvEnd, nPlayMode, fVisualTracking,
                NO_MARCAR_COMPAS_PREVIO, nMM, pWindow);
}


////fMarcarUnCompasPrevio - marcar con el metrónomo un compas completo antes de comenzar la
////       ejecución. Para que este flag actúe requiere que el lmMetronome esté activo
void lmSoundManager::PlaySegment(int nEvStart, int nEvEnd,
                               EPlayMode nPlayMode,
                               bool fVisualTracking,
                               bool fMarcarUnCompasPrevio,
                               long nMM,
                               wxWindow* pWindow )
{
    //
    // Replay all events in table, from nEvStart to nEvEnd, both included.
    //

    if (m_pThread) {
        // A thread exits. If it is paused resume it
        if (m_pThread->IsPaused()) {
            m_pThread->Resume();
            return;
        }
        else {
            // It must be an old thread. Delete it
            m_pThread->Delete();
            m_pThread->Wait();
            delete m_pThread;
            m_pThread = (lmSoundManagerThread*)NULL;
        }
    }

    //Create a new thread. The thread object is created in the suspended state
    m_pThread = new lmSoundManagerThread(this,
                        nEvStart, nEvEnd, nPlayMode, fVisualTracking,
                        fMarcarUnCompasPrevio, nMM, pWindow);

    if ( m_pThread->Create() != wxTHREAD_NO_ERROR ) {
        //! @todo proper error handling
        wxMessageBox(_("Can't create a thread!"));

        m_pThread->Delete();    //to free the memory occupied by the thread object
        delete m_pThread;
        m_pThread = (lmSoundManagerThread*) NULL;
        return;
    }
    m_pThread->SetPriority(WXTHREAD_MAX_PRIORITY);

    //Start the thread execution. This will cause that thread method Enter() is invoked
    //and it will do the job to play the segment.
    if (m_pThread->Run() != wxTHREAD_NO_ERROR ) {
        //! @todo proper error handling
        wxMessageBox(_("Can't start the thread!"));

        m_pThread->Delete();    //to free the memory occupied by the thread object
        delete m_pThread;
        m_pThread = (lmSoundManagerThread*) NULL;
        return;
   }

}

void lmSoundManager::Stop()
{
    if (!m_pThread) return;

    m_pThread->Delete();    //request the tread to terminate
    m_pThread->Wait();
    delete m_pThread;
    m_pThread = (lmSoundManagerThread*)NULL;

}

void lmSoundManager::Pause()
{
    if (!m_pThread) return;

    m_pThread->Pause();

}

void lmSoundManager::WaitForTermination()
{
    //
    // Waits until the end of the score playback
    //

    if (!m_pThread) return;

    m_pThread->Wait();
    delete m_pThread;
    m_pThread = (lmSoundManagerThread*)NULL;

}


//----------------------------------------------------------------------------------------
// Methods to be executed in the thread
//----------------------------------------------------------------------------------------

void lmSoundManager::DoPlaySegment(int nEvStart, int nEvEnd,
                               EPlayMode nPlayMode,
                               bool fVisualTracking,
                               bool fMarcarUnCompasPrevio,
                               long nMM,
                               wxWindow* pWindow )
{
    //
    // This is the real method doing the work. It will execute in the lmSoundManagerThread
    //

    // if MIDI not operative terminate
    if (!g_pMidi || !g_pMidiOut) {
        // inform that the play has ended
        if (pWindow) {
            lmEndOfPlayEvent event(0);
            ::wxPostEvent( pWindow, event );
        }
        return;
    }

    //! @todo All issues related to sol-fa voice

    wxASSERT(nEvStart >= 0 && nEvEnd < (int)m_aEvents.GetCount() );
    if (m_aEvents.GetCount() == 0) return;                  //tabla empty

    #define EIGHT_DURATION    64        //duration (LDP units) of an eight note (to convert to milliseconds)
    #define SOLFA_NOTE        60        //pitch for sight reading with percussion sound
    int nPercussionChannel = g_pMidi->MtrChannel();        //channel to use for percussion

    //prepare metronome settings
    lmMetronome* pMtr = g_pMainFrame->GetMetronome();
    long nMtrClickIntval = (nMM == 0 ? 0 : 60000/nMM);
    bool fPlayWithMetronome = pMtr->IsRunning();
    bool fMetronomeEnabled = pMtr->IsEnabled();
    pMtr->Enable(false);    //mute sound

    // Ask score window to get ready for visual highlight
    bool fVisualHighlight = fVisualTracking && pWindow;
    if (fVisualHighlight && pWindow) {
        lmScoreHighlightEvent event((lmStaffObj*)NULL, ePrepareForHighlight);
        ::wxPostEvent( pWindow, event );
    }

    //Prepare instrument for metronome. Instruments for music voices the instruments
    //are prepared by events ProgInstr
    g_pMidiOut->ProgramChange(g_pMidi->MtrChannel(), g_pMidi->MtrInstr());

    //declaration of some time related variables.
    //DeltaTime variables refer to relative time (LDP time)
    //Time variables refer to absolute time, that is, DeltaTime converted to real time
    long nEvTime;           //time for next event
    long nMtrEvDeltaTime;   //time for next metronome click

    //default beat and metronome information. It is going to be properly set
    //when a eSET_RhythmChange event is found (a time signature object). So these
    //default settings will be used when no time signature in the score.
    long nMtrBeatDuration = EIGHT_DURATION;                         //a beat duration
    long nMtrIntvalOff = wxMin(7, nMtrBeatDuration / 4);            //click duration (interval to click off)
    long nMtrIntvalNextClick = nMtrBeatDuration - nMtrIntvalOff;    //interval from click off to next click
    long nMeasureDuration = nMtrBeatDuration * 4;                   //assume 4/4 time signature

    //Execute control events that take place before the segment to play, so that
    //instruments and tempo are properly programmed. Continue in the loop while
    //we find control events in segment to play.
    int i = 0;
    bool fContinue = true;
    while (fContinue)
    {
        if (m_aEvents[i]->EventType == eSET_ProgInstr)
        {
            //change program
            switch (nPlayMode) {
                case ePM_NormalInstrument:
                    g_pMidi->VoiceChange(m_aEvents[i]->Channel, m_aEvents[i]->lmInstrument);
                    break;
                case ePM_RhythmInstrument:
                    g_pMidi->VoiceChange(m_aEvents[i]->Channel, 57);        //57 = Trumpet
                    break;
                case ePM_RhythmPercussion:
                    g_pMidi->VoiceChange(m_aEvents[i]->Channel, 66);        //66 = High Timbale
                    break;
                case ePM_RhythmHumanVoice:
                    //do nothing. Wave sound will be used
                    break;
                default:
                    wxASSERT(false);
            }
        }
        else if (m_aEvents[i]->EventType == eSET_RhythmChange)
        {
            //set up new beat and metronome information
            nMtrBeatDuration = m_aEvents[i]->BeatDuration;            //a beat duration
            nMeasureDuration = nMtrBeatDuration * m_aEvents[i]->NumBeats;
            nMtrIntvalOff = wxMin(7, nMtrBeatDuration / 4);            //click duration (interval to click off)
            nMtrIntvalNextClick = nMtrBeatDuration - nMtrIntvalOff;    //interval from click off to next click
        }
        else {
            // it is not a control event. Continue in the loop only
            // if we have not reached the start of the segment to play
            fContinue = (i < nEvStart);
        }
        if (fContinue) i++;
    }
    //Here i points to the first event of desired measure that is not a control event.

    //Define and initialize time counter
    long nTime = 0;


    //first note could be a sincopa o a contratiempo. In these cases metronome will
    //start before the first note
    nMtrEvDeltaTime = (m_aEvents[i]->DeltaTime / nMtrBeatDuration) * nMtrBeatDuration;

    /*! @todo
        Si el metrónomo no está activo o se solicita que no se marque un compás completo antes de empezar
        hay que avanzar el contador de tiempo hasta la primera nota
    */
//////    if (Not (fPlayWithMetronome && fMarcarUnCompasPrevio)) {
//////        if (m_nTiempoIni = nMeasureDuration) {
//////            nTime = (nSpeed * m_nTiempoIni) / EIGHT_DURATION;
//////        } else {
//////            nTime = (m_nTiempoIni Mod nMtrBeatDuration) * nMtrBeatDuration    //coge partes completas
//////            nTime = (nSpeed * nTime) / EIGHT_DURATION;
//////        }
//////        //localiza el primer evento de figsil (los eventos de control están en compas 0)
//////        for (i = nEvStart To nEvEnd
//////            if (m_aEvents[i]->Measure <> 0) { Exit For
//////        }   // i
//////        wxASSERT(i <= nEvEnd
//////        nMtrEvDeltaTime = (m_aEvents[i]->DeltaTime \ nMtrBeatDuration) * nMtrBeatDuration
//////    }

    //loop to execute events
    bool fFirstBeatInMeasure = true;    //first beat of a measure
    bool fMtrOn = false;
    do
    {
        //if metronome has been just activated compute next metronome event
        if (!fPlayWithMetronome && pMtr->IsRunning()) {
            nMtrEvDeltaTime = ((m_aEvents[i]->DeltaTime / nMtrBeatDuration) + 1) * nMtrBeatDuration;
            fMtrOn = false;
        }
        fPlayWithMetronome = pMtr->IsRunning();

        //Verify if next event should be a metronome click
        if (fPlayWithMetronome && nMtrEvDeltaTime <= m_aEvents[i]->DeltaTime)
        {
            //Next event shoul be a metronome click or the click off event for the previous metronome click
             if (nMM==0)
                nEvTime = (pMtr->GetInterval() * nMtrEvDeltaTime) / nMtrBeatDuration;   //EIGHT_DURATION;
            else
                nEvTime = (nMtrClickIntval * nMtrEvDeltaTime) / nMtrBeatDuration; //EIGHT_DURATION;
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
             if (nMM==0)
                nEvTime = (pMtr->GetInterval() * m_aEvents[i]->DeltaTime) / nMtrBeatDuration; //EIGHT_DURATION;
            else
                nEvTime = (nMtrClickIntval * m_aEvents[i]->DeltaTime) / nMtrBeatDuration;   //EIGHT_DURATION;
            if (nTime < nEvTime) {
                //::wxMilliSleep((unsigned long)(nEvTime - nTime));
                wxThread::Sleep((unsigned long)(nEvTime - nTime));
            }

            if (m_aEvents[i]->EventType == eSET_NoteON)
            {
                //start of note
                switch(nPlayMode)
                {
                    case ePM_NormalInstrument:
                        g_pMidiOut->NoteOn(m_aEvents[i]->Channel, m_aEvents[i]->NotePitch,
                                          m_aEvents[i]->Volume);
                        break;
                    case ePM_RhythmInstrument:
                        g_pMidiOut->NoteOn(m_aEvents[i]->Channel, SOLFA_NOTE,
                                          m_aEvents[i]->Volume);
                        break;
                    case ePM_RhythmPercussion:
                        g_pMidiOut->NoteOn(nPercussionChannel, SOLFA_NOTE,
                                          m_aEvents[i]->Volume);
                        break;
                    case ePM_RhythmHumanVoice:
                        //WaveOn .NoteStep, m_aEvents[i]->Volume);
                        break;
                    default:
                        wxASSERT(false);
                }

                if (fVisualHighlight && pWindow) {
                    lmScoreHighlightEvent event(m_aEvents[i]->pSO, eVisualOn);
                    ::wxPostEvent( pWindow, event );
                }

            }
            else if (m_aEvents[i]->EventType == eSET_NoteOFF)
            {
                //finalización de nota
                switch(nPlayMode)
                {
                    case ePM_NormalInstrument:
                        g_pMidiOut->NoteOff(m_aEvents[i]->Channel, m_aEvents[i]->NotePitch, 127);
                    case ePM_RhythmInstrument:
                        g_pMidiOut->NoteOff(m_aEvents[i]->Channel, SOLFA_NOTE, 127);
                    case ePM_RhythmPercussion:
                        g_pMidiOut->NoteOff(nPercussionChannel, SOLFA_NOTE, 127);
                    case ePM_RhythmHumanVoice:
                        //WaveOff
                        break;
                    default:
                        wxASSERT(false);
                }

                if (fVisualHighlight && pWindow) {
                    lmScoreHighlightEvent event(m_aEvents[i]->pSO, eVisualOff);
                    ::wxPostEvent( pWindow, event );
                }
            }
//                else if (m_aEvents[i]->EventType == eSET_MarcaEnFRitmos) {
//                    //Marcaje en FRitmos
//                    FRitmos.AvanzarMarca
//
//                }
            else if (m_aEvents[i]->EventType == eSET_VisualON)
            {
                //set visual highlight
                if (fVisualHighlight && pWindow) {
                    lmScoreHighlightEvent event(m_aEvents[i]->pSO, eVisualOn);
                    ::wxPostEvent( pWindow, event );
                }
            }
            else if (m_aEvents[i]->EventType == eSET_VisualOFF)
            {
                //remove visual highlight
                if (fVisualHighlight && pWindow) {
                    lmScoreHighlightEvent event(m_aEvents[i]->pSO, eVisualOff);
                    ::wxPostEvent( pWindow, event );
                }
            }
            else if (m_aEvents[i]->EventType == eSET_EndOfScore)
            {
                //end of table. Do nothing
            }
            else if (m_aEvents[i]->EventType == eSET_RhythmChange)
            {
                //set up new beat and metronome information
                nMtrBeatDuration = m_aEvents[i]->BeatDuration;            //a beat duration
                nMeasureDuration = nMtrBeatDuration * m_aEvents[i]->NumBeats;
                nMtrIntvalOff = wxMin(7, nMtrBeatDuration / 4);            //click duration (interval to click off)
                nMtrIntvalNextClick = nMtrBeatDuration - nMtrIntvalOff;    //interval from click off to next click
            }
            else if (m_aEvents[i]->EventType == eSET_ProgInstr)
            {
                //change program
                switch (nPlayMode) {
                    case ePM_NormalInstrument:
                        g_pMidi->VoiceChange(m_aEvents[i]->Channel, m_aEvents[i]->NotePitch);
                        break;
                    case ePM_RhythmInstrument:
                        g_pMidi->VoiceChange(m_aEvents[i]->Channel, 57);        //57 = Trumpet
                        break;
                    case ePM_RhythmPercussion:
                        g_pMidi->VoiceChange(m_aEvents[i]->Channel, 66);        //66 = High Timbale
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
                //wxASSERT(false);        //! @todo remove comment
            }

            nTime = wxMax(nTime, nEvTime);        //to avoid going backwards when no metronome
                                                //before start and progInstr events
            i++;
        }

        if (m_pThread->TestDestroy()) {
            //Stop playing requested. Exit the loop
            break;
        }

    } while (i <= nEvEnd);

    //restore main metronome
    pMtr->Enable(fMetronomeEnabled);

    //ensure that all visual highlight is removed in case the loop was exited because
    //stop playing was requested
    if (fVisualHighlight && pWindow) {
        lmScoreHighlightEvent event((lmStaffObj*)NULL, eRemoveAllHighlight);
        ::wxPostEvent( pWindow, event );
    }

    //ensure that all sounds are off and that metronome LED is switched off
    g_pMidiOut->AllSoundsOff();
    //! @todo    metronome LED
    //FMain.picMtrLEDOff.Visible = true;
    //FMain.picMtrLEDRojoOn.Visible = false;

    //inform that the play has ended
    if (pWindow) {
        lmEndOfPlayEvent event(0);
        ::wxPostEvent( pWindow, event );
    }

}

//================================================================================
// Class lmSoundManagerThread implementation
//================================================================================

lmSoundManagerThread::lmSoundManagerThread(lmSoundManager* pSM,
                                       int nEvStart,
                                       int nEvEnd,
                                       EPlayMode nPlayMode,
                                       bool fVisualTracking,
                                       bool fMarcarUnCompasPrevio,
                                       long nMM,
                                       wxWindow* pWindow )
    : wxThread(wxTHREAD_JOINABLE)
{
    m_pSM = pSM;
    m_nEvStart = nEvStart;
    m_nEvEnd = nEvEnd;
    m_nPlayMode = nPlayMode;
    m_fVisualTracking = fVisualTracking;
    m_fMarcarUnCompasPrevio = fMarcarUnCompasPrevio;
    m_nMM = nMM;
    m_pWindow = pWindow;
}

lmSoundManagerThread::~lmSoundManagerThread()
{
}

void* lmSoundManagerThread::Entry()
{

    //ask Sound Manager to play
    //@attention the checking to see if the thread was asked to exit is done in DoPlaySegment
    m_pSM->DoPlaySegment(m_nEvStart, m_nEvEnd, m_nPlayMode, m_fVisualTracking,
                m_fMarcarUnCompasPrevio, m_nMM, m_pWindow);

    return NULL;
}

