//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

#ifndef __LENMUS_SCORES_CONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_SCORES_CONSTRAINS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_constrains.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
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

    Each entry in the segments table (SegmentEntry) contains the segment pattern
    and some segment values, such as segment's duration (ts) and time to align
    segment to beat border (tab).

    @endverbatim

*/
//---------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------
// Segments table
//---------------------------------------------------------------------------------------

// Definition of an entry of the segments table
class SegmentEntry
{
public:
    SegmentEntry(wxString sSegment, TimeUnits rTimeAlignBeat, TimeUnits rSegmentDuration)
    {
        m_sSegment = sSegment;
        m_rTimeAlignBeat = rTimeAlignBeat;
        m_rSegmentDuration = rSegmentDuration;
    }
    ~SegmentEntry() {}

    //accesors
    inline wxString& GetSource() { return m_sSegment; }
    inline TimeUnits GetTimeAlignBeat() { return m_rTimeAlignBeat; }
    inline TimeUnits GetSegmentDuration() { return m_rSegmentDuration; }

    //member variables (one entry of the table)
    wxString    m_sSegment;                 //the pattern for this segment
    TimeUnits   m_rTimeAlignBeat;           //time to align segment to beat border (tam)
    TimeUnits   m_rSegmentDuration;         //duration of segment, excluding tam (ts)

};

// Finally this defines the type ArrayOfSegments as an array of SegmentEntry pointers
WX_DEFINE_ARRAY(SegmentEntry*, ArrayOfSegments);



//---------------------------------------------------------------------------------------
// Fragments table
//---------------------------------------------------------------------------------------

// Definition of an entry of the fragments table (corresponds to a fragment)
class lmFragmentEntry
{
public:
    // constructor and destructor
    lmFragmentEntry(TimeSignConstrains* pValidTimeSigns, TimeUnits rTimeBarlineAling=0.0)
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

    void AddSegment(SegmentEntry* pSegment) { m_oSegments.Add(pSegment); }
    void SetTimeToAlignToBarline(TimeUnits rTime) { m_rTimeBarlineAling = rTime; }
    ArrayOfSegments* GetSegments() { return &m_oSegments; }


    //member variables (one entry of the table)
    TimeUnits             m_rTimeBarlineAling;    //time to align fragment to barline or 0 if no alignment required
    TimeSignConstrains*   m_pValidTimeSigns;      //valid time signatures to use with this fragment
    ArrayOfSegments       m_oSegments;            //table of segments composing the fragment


};

// this defines the type ArrayOfFragments as an array of lmFragmentEntry pointers
WX_DEFINE_ARRAY(lmFragmentEntry*, ArrayOfFragments);

//---------------------------------------------------------------------------------------
//Finally let's define the class that implements the fragments' table and the algoritms
// to load, save and deal with it
class FragmentsTable
{
public:
    FragmentsTable(ApplicationScope& appScope);
    ~FragmentsTable();

    void AddEntry(TimeSignConstrains* pValidTimeSigns, wxString sPattern);

    // methods for accesing entries
    int SelectFragments(ETimeSignature nTimeSign);
    void ChooseRandom();
    SegmentEntry* GetNextSegment();

private:
    // methods for pattern analysis
    int SplitPattern(wxString sSource);
    int SplitFragment(wxString sSource);
    wxString GetFirstSegmentDuracion(wxString sSegment, TimeUnits* pSegmentDuration,
                                     TimeUnits* pTimeAlignBeat);
    TimeUnits GetPatternDuracion(wxString sPattern, TimeSignConstrains* pValidTimeSigns);



    // the table
    ApplicationScope&   m_appScope;
    ArrayOfFragments    m_aFragment;    //array of fragment entries

    // Selection set
    wxArrayInt          m_aSelectionSet;    // indexes to the entries that form the selection set
    int                 m_nSelItem;         // choosen item
    ArrayOfSegments*    m_pSegments;        // the segments of the choosen fragment
    int                 m_nNextSegment;     // index over *m_pSegments pointing to next segment to return


};



//---------------------------------------------------------------------------------------
// The ScoreConstrains object
//---------------------------------------------------------------------------------------


#define lmNO_DEFAULTS   false

class ScoreConstrains
{
public:
    ScoreConstrains(ApplicationScope& appScope);
    ~ScoreConstrains() {}

    bool IsValidClef(EClef nClef) { return m_oClefs.IsValid(nClef); }
    void SetClef(EClef nClef, bool fValid) { m_oClefs.SetValid(nClef, fValid); }
    void SetMaxNote(EClef nClef, wxString sNote) { m_oClefs.SetUpperPitch(nClef, sNote); }
    void SetMinNote(EClef nClef, wxString sNote) { m_oClefs.SetLowerPitch(nClef, sNote); }
    ClefConstrains* GetClefConstrains() { return &m_oClefs; }

    inline void set_key_signature(EKeySignature key, bool fValid) {
            m_oValidKeys.SetValid(key, fValid); }
    KeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }
    inline void set_time_signature(ETimeSignature time, bool fValid) {
            m_oValidTimeSign.SetValid(time, fValid); }
    TimeSignConstrains* GetTimeSignConstrains() { return &m_oValidTimeSign; }

    void SetMaxInterval(int nValue) { m_nMaxInterval = nValue; }
    int GetMaxInterval() { return m_nMaxInterval; }

    void SetMetronomeMM(long nValue) { m_nMM = nValue; }
    long GetMetronomeMM() { return m_nMM; }

    void load_settings();
    void save_settings();
    wxString Verify();

    // Fragments table
    inline int SelectFragments(ETimeSignature nTimeSign)
    {
        return(m_aFragmentsTable.SelectFragments(nTimeSign));
    }
    inline void AddFragment(TimeSignConstrains* pValidTimeSigns, wxString sPattern)
    {
        m_aFragmentsTable.AddEntry(pValidTimeSigns, sPattern);
    }
    inline void SetSection(wxString sSection) { m_sSection = sSection; }
    inline void ChooseRandomFragment() { return m_aFragmentsTable.ChooseRandom(); }
    inline SegmentEntry* GetNextSegment() { return m_aFragmentsTable.GetNextSegment(); }

private:
    ApplicationScope& m_appScope;
    wxString    m_sSection;         // section name to save the constraints
    int         m_nMaxInterval;     // max interval in two consecutive notes
    long        m_nMM;              // metronome setting

    KeyConstrains         m_oValidKeys;           //allowed key signatures
    ClefConstrains        m_oClefs;               //allowed clefs and scopes
    TimeSignConstrains    m_oValidTimeSign;       //allowed time signatures
    FragmentsTable        m_aFragmentsTable;      //allowed fragments
};


}   // namespace lenmus

#endif  // __LENMUS_SCORES_CONSTRAINS_H__
