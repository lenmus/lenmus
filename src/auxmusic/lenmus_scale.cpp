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

//lenmus
#include "lenmus_scale.h"

#include "lenmus_interval.h"
#include "lenmus_generators.h"


namespace lenmus
{

//---------------------------------------------------------------------------------------
// Define a static table with information about scales
// AWARE: Array indexes are in correspondence with enum EScaleType.
//        Intervals are defined as increment from previous note.

struct ScaleInfo
{
    int nNumNotes;
    FIntval nInterval[k_notes_in_scale];
};

#define k_interval_1s  FIntval(-9898)      //to signal end of table

static const ScaleInfo tData[est_Max] =
{
        //Major scales
    { 8, { k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_M2,k_interval_m2} },    //Major natural
    { 8, { k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_m2,k_interval_a2,k_interval_m2} },    //Major TypeII
    { 8, { k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2} },    //Major TypeIII
    { 8, { k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2} },    //Major TypeIV
        // Minor scales
    { 8, { k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2} },    //Minor Natural,
    { 8, { k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2} },    //Minor Dorian,
    { 8, { k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_m2,k_interval_a2,k_interval_m2} },    //Minor Harmonic,
    { 8, { k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_M2,k_interval_M2,k_interval_m2} },    //Minor Melodic,
        // Gregorian modes
    { 8, { k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_M2,k_interval_m2} },    //Gregorian Ionian (major natural),
    { 8, { k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2} },    //Gregorian Dorian,
    { 8, { k_interval_m2,k_interval_M2,k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2} },    //Gregorian Phrygian,
    { 8, { k_interval_M2,k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_m2} },    //Gregorian Lydian,
    { 8, { k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2} },    //Gregorian Mixolydian,
    { 8, { k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2} },    //Gregorian Aeolian (minor natural),
    { 8, { k_interval_m2,k_interval_M2,k_interval_M2,k_interval_m2,k_interval_M2,k_interval_M2,k_interval_M2} },    //Gregorian Locrian,
        // Other scales
    { 6, { k_interval_m3,k_interval_M2,k_interval_M2,k_interval_m3,k_interval_M2} },                //Pentatonic Minor,
    { 6, { k_interval_M2,k_interval_M2,k_interval_m3,k_interval_M2,k_interval_m3} },                //Pentatonic Major,
    { 7, { k_interval_m3,k_interval_M2,k_interval_m2,k_interval_m2,k_interval_m3,k_interval_M2} },          //Blues,
        // Non-tonal scales
    { 7, { k_interval_M2,k_interval_M2,k_interval_M2,k_interval_M2,k_interval_M2,k_interval_M2} },          //WholeTones,
    {13, { k_interval_1s,k_interval_1s,k_interval_1s,k_interval_1s,k_interval_1s,k_interval_1s,k_interval_1s,k_interval_1s,k_interval_1s,k_interval_1s,k_interval_1s,k_interval_1s} },       //Chromatic,
};


//=======================================================================================
// Implementation of Scale class
//=======================================================================================
Scale::Scale(FPitch fpRootNote, EScaleType nScaleType, EKeySignature nKey)
{
    m_nType = nScaleType;
    m_nKey = nKey;
    m_fpNote[0] = fpRootNote;

    //for non-tonal scales, key signature is not used
    bool fUseSharps = RandomGenerator::flip_coin();

    //get notes that form the scale
    int nNumNotes = get_num_notes();
    if (m_nType == est_Chromatic)
    {
        for (int i=1; i < nNumNotes; i++)
        {
            m_fpNote[i] = m_fpNote[i-1];
            m_fpNote[i].add_semitone(fUseSharps);
        }
    }
    else if (m_nType == est_WholeTones)
    {
        for (int i=1; i < nNumNotes; i++)
        {
            m_fpNote[i] = m_fpNote[i-1];
            m_fpNote[i].add_semitone(fUseSharps);
            m_fpNote[i].add_semitone(fUseSharps);
        }
    }
    else
    {
        for (int i=1; i < nNumNotes; i++)
        {
            FIntval fi = tData[m_nType].nInterval[i-1];
            m_fpNote[i] = m_fpNote[i-1] + fi;
        }
    }
}

//---------------------------------------------------------------------------------------
Scale::~Scale()
{
}

//---------------------------------------------------------------------------------------
int Scale::get_num_notes()
{
    return tData[m_nType].nNumNotes;
}

//---------------------------------------------------------------------------------------
string Scale::rel_ldp_name_for_note(int i)
{
    // Returns relative LDP pattern for note i (0 .. m_nNumNotes-1)
    wxASSERT( i < get_num_notes());
    return m_fpNote[i].to_rel_ldp_name(m_nKey);
}

//---------------------------------------------------------------------------------------
string Scale::abs_ldp_name_for_note(int i)
{
    // Returns absolute LDP pattern for note i (0 .. m_nNumNotes-1)
    wxASSERT( i < get_num_notes());
    return m_fpNote[i].to_abs_ldp_name();
}

//---------------------------------------------------------------------------------------
wxString Scale::type_to_name(EScaleType nType)
{
    wxASSERT(nType < est_Max);

    static wxString m_sScaleName[est_Max];
    static wxString m_language = "??";

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (m_language != ApplicationScope::get_language())
    {
        // Major scales
        m_sScaleName[est_MajorNatural] = _("Major natural");
        m_sScaleName[est_MajorTypeII] = _("Major type II");
        m_sScaleName[est_MajorTypeIII] = _("Major type III");
        m_sScaleName[est_MajorTypeIV] = _("Major type IV");

        // Minor scales
        m_sScaleName[est_MinorNatural] = _("Minor natural");
        m_sScaleName[est_MinorDorian] = _("Minor Dorian");
        m_sScaleName[est_MinorHarmonic] = _("Minor Harmonic");
        m_sScaleName[est_MinorMelodic] = _("Minor Melodic");

        // Gregorian modes
        m_sScaleName[est_GreekIonian] = _("Ionian");
        m_sScaleName[est_GreekDorian] = _("Dorian");
        m_sScaleName[est_GreekPhrygian] = _("Phrygian");
        m_sScaleName[est_GreekLydian] = _("Lydian");
        m_sScaleName[est_GreekMixolydian] = _("Mixolydian");
        m_sScaleName[est_GreekAeolian] = _("Aeolian");
        m_sScaleName[est_GreekLocrian] = _("Locrian");

        // Other scales
        m_sScaleName[est_PentatonicMinor] = _("Pentatonic minor");
        m_sScaleName[est_PentatonicMajor] = _("Pentatonic major");
        m_sScaleName[est_Blues] = _("Blues");
        m_sScaleName[est_WholeTones] = _("Whole tones");
        m_sScaleName[est_Chromatic] = _("Chromatic");

        m_language = ApplicationScope::get_language();
    }

    return m_sScaleName[nType];
}

//---------------------------------------------------------------------------------------
bool Scale::is_major(EScaleType nType)
{
    return (nType <= est_LastMajor);
}

//---------------------------------------------------------------------------------------
bool Scale::is_minor(EScaleType nType)
{
    return (nType > est_LastMajor && nType <= est_LastMinor);
}

//---------------------------------------------------------------------------------------
bool Scale::is_gregorian(EScaleType nType)
{
    return (nType > est_LastMinor && nType <= est_LastGreek);
}

//---------------------------------------------------------------------------------------
EScaleType Scale::short_name_to_type(const wxString& sName)
{
    // returns -1 if error
    //
    //  major: MN (natural), MH (harmonic), M3 (type III), MM (mixolydian)
    //  minor: mN (natural), mM (melodic), mD (dorian), mH (harmonic)
    //  medieval modes: Do (Dorian), Ph (Phrygian), Ly (Lydian),
    //                  Mx (Mixolydian), Ae (Aeolian), Io (Ionian),
    //                  Lo (Locrian)
    //  other: Pm (Pentatonic minor), PM (Pentatonic Major), Bl (Blues)
    //  non-tonal: WT (Whole Tones), Ch (Chromatic)


    // Major scales
    if      (sName == "MN") return est_MajorNatural;
    else if (sName == "MH") return est_MajorTypeII;
    else if (sName == "M3") return est_MajorTypeIII;
    else if (sName == "MM") return est_MajorTypeIV;

    // Minor scales
    else if (sName == "mN") return est_MinorNatural;
    else if (sName == "mD") return est_MinorDorian;
    else if (sName == "mH") return est_MinorHarmonic;
    else if (sName == "mM") return est_MinorMelodic;

    // Gregorian modes
    else if (sName == "Io") return est_GreekIonian;
    else if (sName == "Do") return est_GreekDorian;
    else if (sName == "Ph") return est_GreekPhrygian;
    else if (sName == "Ly") return est_GreekLydian;
    else if (sName == "Mx") return est_GreekMixolydian;
    else if (sName == "Ae") return est_GreekAeolian;
    else if (sName == "Lo") return est_GreekLocrian;

    // Other scales
    else if (sName == "Pm") return est_PentatonicMinor;
    else if (sName == "PM") return est_PentatonicMajor;
    else if (sName == "Bl") return est_Blues;

    //non-tonal scales
    else if (sName == "WT") return est_WholeTones;
    else if (sName == "Ch") return est_Chromatic;

    return (EScaleType)-1;  //error
}


}   //namespace lenmus
