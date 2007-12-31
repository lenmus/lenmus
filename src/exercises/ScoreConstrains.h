//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_SCORECONSTRAINS_H__        //to avoid nested includes
#define __LM_SCORECONSTRAINS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ScoreConstrains.cpp"
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
#include "Constrains.h"


// ----------------------------------------------------------------------------------
/*! @page FragmentAndSegments
    @verbatim

    Fragment and Segments tables
    ----------------------------


    a) Score composition
    --------------------

    Score composition is based on rhythmic patterns called 'fragments'.

    A fragment is formed by concatenating notes and rests elements, for example:
    "(n * c)(n * c)(n * n)(n * c)(n * n)(n * s g+)(n * s)(n * c g-)"

    The elements are separated by commas (,) to form 'segments' (a segment is
    a group of elements that must go together and accupies one or more full
    beats), i.e:
    "(n * c),(n * c)(n * n)(n * c),(n * n),(n * s g+)(n * s)(n * c g-)"

    In certain cases, it is necesary to specify that a barline must be in a
    specific point. This will be specified by putting a bar (|) instead of a
    comma (,), i.e.:
    "(n * c),(n * c)(n * n)(n * c)|(n * n),(n * s g+)(n * s)(n * c g-)"

    Fragments must always start aligned to beat. Therefore it is necessary
    to include any requied rest at the begining. Previous example must be
    finally written as:
    "(s c)(n * c),(n * c)(n * n)(n * c)|(n * n),(n * s g+)(n * s)(n * c g-)"

    Rests at the begining must occupy less than a beat. That is, no empty beats
    are allowed at the begining.


    b) The fragment and segments tables
    -----------------------------------

    A ScoreConstrains object contains a FragmentsTable with all usable fragments.

    Each entry in the fragments table contains all data associated to a fragment:
    usable time signatures, time to align fragment to barline border (tam), and
    a SegmentsTable with all segments composing the fragment.

    Each entry in the segments table (lmSegmentEntry) contains the segment pattern
    and some segment values, such as segment's duration (ts) and time to align
    segment to beat border (tab).

    @endverbatim

*/
// ----------------------------------------------------------------------------------




//-------------------------------------------------------------------------------------------
// Segments table
//-------------------------------------------------------------------------------------------

// Definition of an entry of the segments table
class lmSegmentEntry
{
public:
    // constructor and destructor
    lmSegmentEntry(wxString sSegment, float rTimeAlignBeat, float rSegmentDuration)
    {
            m_sSegment = sSegment;
            m_rTimeAlignBeat = rTimeAlignBeat;
            m_rSegmentDuration = rSegmentDuration;
        }

    ~lmSegmentEntry() {}

    //accesors
    wxString GetSource() { return m_sSegment; }
    float GetTimeAlignBeat() { return m_rTimeAlignBeat; }
    float GetSegmentDuration() { return m_rSegmentDuration; }

    //member variables (one entry of the table)
    wxString    m_sSegment;                 //the pattern for this segment
    float       m_rTimeAlignBeat;           //time to align segment to beat border (tam)
    float       m_rSegmentDuration;         //duration of segment, excluding tam (ts)

};

// Finally this defines the type ArrayOfSegments as an array of lmSegmentEntry pointers
WX_DEFINE_ARRAY(lmSegmentEntry*, ArrayOfSegments);



//-------------------------------------------------------------------------------------------
// Fragments table
//-------------------------------------------------------------------------------------------

// Definition of an entry of the fragments table (corresponds to a fragment)
class lmFragmentEntry
{
public:
    // constructor and destructor
    lmFragmentEntry(lmTimeSignConstrains* pValidTimeSigns, float rTimeBarlineAling=0.0)
        {
            m_pValidTimeSigns = pValidTimeSigns;
            m_rTimeBarlineAling = rTimeBarlineAling;
        }

    ~lmFragmentEntry()
        {
            delete m_pValidTimeSigns;
            int i;
            for (i=0; i < (int)m_oSegments.Count(); i++) {
                delete m_oSegments[i];
            }
        }

    void AddSegment(lmSegmentEntry* pSegment) { m_oSegments.Add(pSegment); }
    void SetTimeToAlignToBarline(float rTime) { m_rTimeBarlineAling = rTime; }
    ArrayOfSegments* GetSegments() { return &m_oSegments; }


    //member variables (one entry of the table)
    float                   m_rTimeBarlineAling;    //time to align fragment to barline or 0 if no alignment required
    lmTimeSignConstrains*   m_pValidTimeSigns;      //valid time signatures to use with this fragment
    ArrayOfSegments         m_oSegments;            //table of segments composing the fragment


};

// this defines the type ArrayOfFragments as an array of lmFragmentEntry pointers
WX_DEFINE_ARRAY(lmFragmentEntry*, ArrayOfFragments);

//Finally let's define the class that implements the fragments' table and the algoritms
// to load, save and deal with it
class lmFragmentsTable
{
public:
    lmFragmentsTable();
    ~lmFragmentsTable();

    void AddEntry(lmTimeSignConstrains* pValidTimeSigns, wxString sPattern);

    // methods for accesing entries
    int SelectFragments(lmETimeSignature nTimeSign);
    void ChooseRandom();
    lmSegmentEntry* GetNextSegment();

private:
    // methods for pattern analysis
    int SplitPattern(wxString sSource);
    int SplitFragment(wxString sSource);
    wxString GetFirstSegmentDuracion(wxString sSegment,
                                float* pSegmentDuration, float* pTimeAlignBeat);
    float GetPatternDuracion(wxString sPattern, lmTimeSignConstrains* pValidTimeSigns);



    // the table
    ArrayOfFragments    m_aFragment;    //array of fragment entries

    // Selection set
    wxArrayInt          m_aSelectionSet;    // indexes to the entries that form the selection set
    int                 m_nSelItem;         // choosen item
    ArrayOfSegments*    m_pSegments;        // the segments of the choosen fragment
    int                 m_nNextSegment;     // index over *m_pSegments pointing to next segment to return


};



//-------------------------------------------------------------------------------------------
// The ScoreConstrains object
//-------------------------------------------------------------------------------------------


#define lmNO_DEFAULTS   false

/*! @class lmScoreConstrains
    @brief Options for lmTheoMusicReadingCtrol control
*/
class lmScoreConstrains
{
public:
    lmScoreConstrains();
    ~lmScoreConstrains() {}

    bool IsValidClef(lmEClefType nClef) { return m_oClefs.IsValid(nClef); }
    void SetClef(lmEClefType nClef, bool fValid) { m_oClefs.SetValid(nClef, fValid); }
    void SetMaxNote(lmEClefType nClef, wxString sNote) { m_oClefs.SetUpperPitch(nClef, sNote); }
    void SetMinNote(lmEClefType nClef, wxString sNote) { m_oClefs.SetLowerPitch(nClef, sNote); }
    lmClefConstrain* GetClefConstrains() { return &m_oClefs; }

    lmKeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }
    lmTimeSignConstrains* GetTimeSignConstrains() { return &m_oValidTimeSign; }

    void SetMaxInterval(int nValue) { m_nMaxInterval = nValue; }
    int GetMaxInterval() { return m_nMaxInterval; }

    void SetMetronomeMM(long nValue) { m_nMM = nValue; }
    long GetMetronomeMM() { return m_nMM; }

    void LoadSettings();
    void SaveSettings();
    wxString Verify();



    // Fragments table
    int SelectFragments(lmETimeSignature nTimeSign) {
                return(m_aFragmentsTable.SelectFragments(nTimeSign));
            }
    void ChooseRandomFragment() { return m_aFragmentsTable.ChooseRandom(); }
    lmSegmentEntry* GetNextSegment() { return m_aFragmentsTable.GetNextSegment(); }
    void AddFragment(lmTimeSignConstrains* pValidTimeSigns, wxString sPattern) {
                m_aFragmentsTable.AddEntry(pValidTimeSigns, sPattern);
            }
    void SetSection(wxString sSection) { m_sSection = sSection; }



private:

    wxString    m_sSection;         // section name to save the constraints
    int         m_nMaxInterval;     // max interval in two consecutive notes
    long        m_nMM;              // metronome setting

    lmKeyConstrains         m_oValidKeys;           //allowed key signatures
    lmClefConstrain         m_oClefs;               //allowed clefs and scopes
    lmTimeSignConstrains    m_oValidTimeSign;       //allowed time signatures
    lmFragmentsTable        m_aFragmentsTable;      //allowed fragments
};


#endif  // __LM_SCORECONSTRAINS_H__
