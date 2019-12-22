//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
#include "lenmus_interval.h"
#include "lenmus_generators.h"

//other
#include <vector>
using namespace std;


namespace lenmus
{

static wxString m_sIntervalName[16];
static wxString m_language = "??";

//---------------------------------------------------------------------------------------
void initialize_strings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

    m_sIntervalName[0] = "";
    m_sIntervalName[1] = _("Unison");
    m_sIntervalName[2] = _("2nd");
    m_sIntervalName[3] = _("3rd");
    m_sIntervalName[4] = _("4th");
    m_sIntervalName[5] = _("5th");
    m_sIntervalName[6] = _("6th");
    m_sIntervalName[7] = _("7th");
    m_sIntervalName[8] = _("octave");
    m_sIntervalName[9] = _("9th");
    m_sIntervalName[10] = _("10th");
    m_sIntervalName[11] = _("11th");
    m_sIntervalName[12] = _("12th");
    m_sIntervalName[13] = _("13th");
    m_sIntervalName[14] = _("14th");
    m_sIntervalName[15] = _("Two octaves");

    m_language = ApplicationScope::get_language();
}

//---------------------------------------------------------------------------------------
wxString get_FIntval_name(FIntval intv)
{
    //AWARE: This method is restricted to two octaves

    int interval = int(intv);
    wxASSERT(interval < 81);      // 80 = two octaves
    if (m_language != ApplicationScope::get_language())
        initialize_strings();

    int nNumIntv = intv.get_number();   // 0..15

    wxString sName = m_sIntervalName[nNumIntv];
    if (interval == 0)
        sName = _("Unison");
    else if (interval == 1)
        sName = _("Chromatic semitone");
    else if (interval == 2)
        sName = _("Chromatic tone");
    else
    {
        switch (intv.get_type())
        {
            case k_diminished:        sName += _(" diminished");          break;
            case k_minor:             sName += _(" minor");               break;
            case k_major:             sName += _(" mayor");               break;
            case k_augmented:         sName += _(" augmented");           break;
            case k_perfect:           sName += _(" perfect");             break;
            case k_double_augmented:  sName += _(" double augmented");    break;
            case k_double_diminished: sName += _(" double diminished");   break;
            default:
                wxASSERT(false);
        }
    }
    return sName;
}


//=======================================================================================
// Interval implementation
//=======================================================================================
Interval::Interval(bool fDiatonic, DiatonicPitch dpMin, DiatonicPitch dpMax,
                   bool fAllowedIntervals[], bool fAscending,
                   EKeySignature nKey, FPitch fpStartNote)
    : m_nKey(nKey)
{
    //Constructor to build a random interval, for aural training exercises,
    //satisfying the received constrains
//    LogMessage("[Interval::Interval] --------------------------------------");
//    LogMessage("[Interval::Interval] fDiatonic=%d, dpMin=%d, dpMax=%d, fpStartNote=%d, fAscending=%d",
//               (fDiatonic ? 1 : 0), int(dpMin), int(dpMax), int(fpStartNote),
//               (fAscending ? 1 : 0) );

    //posible valid intervals for aural training
    static FIntval m_interval[25] = {
        k_interval_p1,  k_interval_m2,  k_interval_M2,  k_interval_m3,  k_interval_M3,
        k_interval_p4,  k_interval_a4,  k_interval_p5,  k_interval_m6,  k_interval_M6,
        k_interval_m7,  k_interval_M7,  k_interval_p8,  k_interval_m9,  k_interval_M9,
        k_interval_m10, k_interval_M10, k_interval_p11, k_interval_a11, k_interval_p12,
        k_interval_m13, k_interval_M13, k_interval_m14, k_interval_M14, k_interval_p15
    };

    //compute max allowed interval for the specified notes range
    FPitch fpMin = dpMin.to_FPitch(nKey);
    FPitch fpMax = dpMax.to_FPitch(nKey);
    FIntval maxIntv = FIntval(fpMax - fpMin);
    if (maxIntv > k_interval_p15)
        maxIntv = k_interval_p15;

    //extract and count allowed intervals
    int nAllowedIntv[25];
    int nNumIntv = 0;
    for (int i=0; i < 25; i++)
    {
        if (fAllowedIntervals[i])
        {
            nAllowedIntv[nNumIntv] = i;
//            LogMessage("[Interval::Interval] nAllowedIntv[%d]=%d", nNumIntv, i);
            nNumIntv++;
        }
    }
//    LogMessage("[Interval::Interval] nNumIntv=%d", nNumIntv);

    //select an interval at random. This is the first thing to do in order that all
    //intervals have the same probability. Other algorithms that I have tried
    //don't work because give more probability to some intervals
    vector<FPitch> validNotes;
    FIntval selIntv;
    while(nNumIntv != 0)
    {
        //select interval
        int iSel = RandomGenerator::random_number(0, nNumIntv - 1);
        selIntv = m_interval[ nAllowedIntv[iSel] ];
//        LogMessage("[Interval::Interval] iSel=%d, selIntv=%d", iSel, selIntv);

        //determine max minimum note for the selected interval
        FPitch fpTop = fpMax - selIntv;
        DiatonicPitch dpTop(fpTop.step(), fpTop.octave());

        //determine valid notes to start the choosen interval
        DiatonicPitch dpCur = dpMin;
        do
        {
            FPitch fpFirst = dpCur.to_FPitch(nKey);
            FPitch fpSecond = fpFirst + selIntv;
//            LogMessage("[Interval::Interval] fpFirst=%d, fpSecond=%d",
//                       int(fpFirst), int(fpSecond));

            //if no accidentals allowed, filter out this note if requires accidentals
            if (fDiatonic)
            {
                if (fpSecond.is_natural_note_for(nKey))
                    validNotes.push_back(fpFirst);
            }
            else if (fpStartNote == k_undefined_fpitch || fpStartNote == fpFirst)
                validNotes.push_back(fpFirst);

            //try next note
            dpCur = dpCur + 1;

        } while (dpCur <= dpTop);

        //if the interval can be built, exit loop
//        LogMessage("[Interval::Interval] validNotes.size()=%d", validNotes.size());
        if (validNotes.size() > 0)
            break;

        //the interval can not be generated because there are no valid
        //notes to build it. Discard this interval and choose anoher one.
//        LogMessage("[Interval::Interval] Cannot generate, nNumIntv=%d, iSel=%d", nNumIntv, iSel);
        --nNumIntv;
        if (iSel < nNumIntv)
        {
            for (int i=iSel; i < nNumIntv; i++)
                nAllowedIntv[i] = nAllowedIntv[i + 1];
        }
    }

    //The loop has been exited because two possible reasons:
    //1. number of valid notes > 0 (The interval can be built)
    //2. nNumIntv==0 (No valid interval can be built)
    if (nNumIntv == 0)
    {
//        LogMessage("[Interval::Interval] Cannot generate interval");
        wxMessageBox(_("It is not possible to generate an interval satisfying the constraints imposed by the chosen settings."));
        m_fp[0] = FPitch(k_step_C, k_octave_4, 0);
        m_fp[1] = FPitch(k_step_G, k_octave_4, 0);
        return;
    }

    //choose interval notes
    if (fpStartNote == k_undefined_fpitch)
    {
        //not required to start with fpStartNote. Choose at random
        int iN = RandomGenerator::random_number(0, static_cast<int>(validNotes.size()) - 1);
        if (fAscending)
            m_fp[0] = validNotes[iN];
        else
            m_fp[0] = validNotes[iN] + selIntv;
    }
    else
    {
        //force first note to be fpStartNote
        m_fp[0] = fpStartNote;
    }

    //determine second note
    if (fAscending)
        m_fp[1] = m_fp[0] + selIntv;
    else
        m_fp[1] = m_fp[0] - selIntv;

//    LogMessage("[Interval::Interval] int: fp0=%d, fp1=%d", int(m_fp[0]), int(m_fp[1]));
}

//---------------------------------------------------------------------------------------
int Interval::get_num_semitones()
{
    return get_interval().get_num_semitones();
}

//---------------------------------------------------------------------------------------
wxString Interval::get_interval_name()
{
    wxString name = get_FIntval_name(get_interval());
    if (get_interval() != k_interval_p1)
    {
        name += ", ";
        name += (m_fp[1] > m_fp[0] ? _("ascending") : _("descending") );
    }
    return name;
}


}   //namespace lenmus
