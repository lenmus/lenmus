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

//=======================================================================================
// Implementation of FIntval
//=======================================================================================

static wxString m_sIntervalName[16];
static bool m_fStringsInitialized = false;

//interval names. The index in this array is the FIntval value
wxString m_sFIntvalCode[41] = {
    _T("p1"),  _T("a1"),  _T("da1"), _T("dd2"), _T("d2"), _T("m2"),  _T("M2"),  _T("a2"),
    _T("da2"), _T("dd3"), _T("d3"),  _T("m3"),  _T("M3"), _T("a3"),  _T("da3"), _T("dd4"),
    _T("d4"),  _T("p4"),  _T("a4"),  _T("da4"), _T("--"), _T("dd5"), _T("d5"),  _T("p5"),
    _T("a5"),  _T("da5"), _T("dd6"), _T("d6"),  _T("m6"), _T("M6"),  _T("a6"),  _T("da6"),
    _T("dd7"), _T("d7"),  _T("m7"),  _T("M7"),  _T("a7"), _T("da7"), _T("dd8"), _T("d8"),
    _T("p8")
};

// an entry for the table of intervals data
typedef struct {
    EIntervalType   nType;
    int             nNumIntv;
    int             nNumSemitones;
} IntervalData;

static const IntervalData m_aIntvData[40] =
{
    {/*  0 - lm_p1  */  k_perfect,            1,  0 },
    {/*  1 - lm_a1  */  k_augmented,          1,  1 },
    {/*  2 - lm_da1 */  k_double_augmented,   1,  2 },
    {/*  3 - lm_dd2 */  k_double_diminished,  2, -1 },
    {/*  4 - lm_d2  */  k_diminished,         2,  0 },
    {/*  5 - lm_m2  */  k_minor,              2,  1 },
    {/*  6 - lm_M2  */  k_major,              2,  2 },
    {/*  7 - lm_a2  */  k_augmented,          2,  3 },
    {/*  8 - lm_da2 */  k_double_augmented,   2,  4 },
    {/*  9 - lm_dd3 */  k_double_diminished,  3,  1 },
    {/* 10 - lm_d3  */  k_diminished,         3,  2 },
    {/* 11 - lm_m3  */  k_minor,              3,  3 },
    {/* 12 - lm_M3  */  k_major,              3,  4 },
    {/* 13 - lm_a3  */  k_augmented,          3,  5 },
    {/* 14 - lm_da3 */  k_double_augmented,   3,  6 },
    {/* 15 - lm_dd4 */  k_double_diminished,  4,  3 },
    {/* 16 - lm_d4  */  k_diminished,         4,  4 },
    {/* 17 - lm_p4  */  k_perfect,            4,  5 },
    {/* 18 - lm_a4  */  k_augmented,          4,  6 },
    {/* 19 - lm_da4 */  k_double_augmented,   4,  7 },
    {/*empty*/          (EIntervalType)0,     0,  0 },
    {/* 21 - lm_dd5 */  k_double_diminished,  5,  5 },
    {/* 22 - lm_d5  */  k_diminished,         5,  6 },
    {/* 23 - lm_p5  */  k_perfect,            5,  7 },
    {/* 24 - lm_a5  */  k_augmented,          5,  8 },
    {/* 25 - lm_da5 */  k_double_augmented,   5,  9 },
    {/* 26 - lm_dd6 */  k_double_diminished,  6,  6 },
    {/* 27 - lm_d6  */  k_diminished,         6,  7 },
    {/* 28 - lm_m6  */  k_minor,              6,  8 },
    {/* 29 - lm_M6  */  k_major,              6,  9 },
    {/* 30 - lm_a6  */  k_augmented,          6, 10 },
    {/* 31 - lm_da6 */  k_double_augmented,   6, 10 },
    {/* 32 - lm_dd7 */  k_double_diminished,  7,  8 },
    {/* 33 - lm_d7  */  k_diminished,         7,  9 },
    {/* 34 - lm_m7  */  k_minor,              7, 10 },
    {/* 35 - lm_M7  */  k_major,              7, 11 },
    {/* 36 - lm_a7  */  k_augmented,          7, 12 },
    {/* 37 - lm_da7 */  k_double_augmented,   7, 13 },
    {/* 38 - lm_dd8 */  k_double_diminished,  8, 10 },
    {/* 39 - lm_d8  */  k_diminished,         8, 11 },
};


//---------------------------------------------------------------------------------------
void FIntval::initialize_strings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

    m_sIntervalName[0] = _T("");
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

    m_fStringsInitialized = true;
}

//---------------------------------------------------------------------------------------
FIntval::FIntval(const wxString& sName)
{
    // unison
    if (sName == _T("p1")) m_interval = lm_p1;
    else if (sName == _T("a1")) m_interval = lm_a1;
    // second
    else if (sName == _T("d2")) m_interval = lm_d2;
    else if (sName == _T("m2")) m_interval = lm_m2;
    else if (sName == _T("M2")) m_interval = lm_M2;
    else if (sName == _T("a2")) m_interval = lm_a2;
    // third
    else if (sName == _T("d3")) m_interval = lm_d3;
    else if (sName == _T("m3")) m_interval = lm_m3;
    else if (sName == _T("M3")) m_interval = lm_M3;
    else if (sName == _T("a3")) m_interval = lm_a3;
    // fourth
    else if (sName == _T("d4")) m_interval = lm_d4;
    else if (sName == _T("p4")) m_interval = lm_p4;
    else if (sName == _T("a4")) m_interval = lm_a4;
    // fifth
    else if (sName == _T("d5")) m_interval = lm_d5;
    else if (sName == _T("p5")) m_interval = lm_p5;
    else if (sName == _T("a5")) m_interval = lm_a5;
    //sixth
    else if (sName == _T("d6")) m_interval = lm_d6;
    else if (sName == _T("m6")) m_interval = lm_m6;
    else if (sName == _T("M6")) m_interval = lm_M6;
    else if (sName == _T("a6")) m_interval = lm_a6;
    // seventh
    else if (sName == _T("d7")) m_interval = lm_d7;
    else if (sName == _T("m7")) m_interval = lm_m7;
    else if (sName == _T("M7")) m_interval = lm_M7;
    else if (sName == _T("a7")) m_interval = lm_a7;
    // octave
    else if (sName == _T("d8")) m_interval = lm_d8;
    else if (sName == _T("p8")) m_interval = lm_p8;

    else
        m_interval = lmNULL_FIntval;
}

//---------------------------------------------------------------------------------------
FIntval::FIntval (int intv, EIntervalType type)
{
    int i;
    for (i=0; i < 40; i++)
    {
        if (m_aIntvData[i].nNumIntv == intv && m_aIntvData[i].nType == type)
            break;
    }
    if (i < 40)
        m_interval = i;
    else
        wxASSERT(false);
}

//---------------------------------------------------------------------------------------
int FIntval::get_number()
{
    //returns interval number: 1=unison, 2=2nd, ..., 8=8ve, 9=9th, ..., 15=2 octaves, ...

    int octave = (m_interval / 40) * 7;
    int num = m_interval % 40;     //num = 0..39

    return m_aIntvData[num].nNumIntv + octave;
}

//---------------------------------------------------------------------------------------
wxString FIntval::get_code()
{
    //returns interval code

    int nOctave = ((m_interval / 40) * 7);
    int num = m_interval % 40;     //num = 0..39
    nOctave += m_aIntvData[num].nNumIntv;

    wxString sCode = _T("");
    switch (m_aIntvData[num].nType)
    {
        case k_diminished:        return wxString::Format(_T("d%d"), nOctave);
        case k_minor:             return wxString::Format(_T("m%d"), nOctave);
        case k_major:             return wxString::Format(_T("M%d"), nOctave);
        case k_augmented:         return wxString::Format(_T("a%d"), nOctave);
        case k_perfect:           return wxString::Format(_T("p%d"), nOctave);
        case k_double_augmented:  return wxString::Format(_T("da%d"), nOctave);
        case k_double_diminished: return wxString::Format(_T("dd%d"), nOctave);
        default:
            wxASSERT(false);
    }
    return wxEmptyString;
}

//---------------------------------------------------------------------------------------
wxString FIntval::get_name()
{
    //AWARE: This method is restricted to two octaves

    wxASSERT(m_interval < 81);      // 80 = two octaves
    if (!m_fStringsInitialized)
        initialize_strings();

    int octave = (m_interval / 40) * 7;
    int num = m_interval % 40;     //num = 0..39
    int nNumIntv = m_aIntvData[num].nNumIntv + octave;      // 0..15

    wxString sName = m_sIntervalName[nNumIntv];
    if (m_interval == 0)
        sName = _("Unison");
    else if (m_interval == 1)
        sName = _("Chromatic semitone");
    else if (m_interval == 2)
        sName = _("Chromatic tone");
    else
    {
        switch (m_aIntvData[num].nType)
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

//---------------------------------------------------------------------------------------
EIntervalType FIntval::get_type()
{
    int num = m_interval % 40;   //num = 0..39
    return m_aIntvData[num].nType;
}

//---------------------------------------------------------------------------------------
int FIntval::get_num_semitones()
{
    int num = m_interval % 40;   //num = 0..39
    int octaves = (m_interval / 40) * 12;
    return m_aIntvData[num].nNumSemitones + octaves;
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
//    LogMessage(_T("[Interval::Interval] --------------------------------------"));
//    LogMessage(_T("[Interval::Interval] fDiatonic=%d, dpMin=%d, dpMax=%d, fpStartNote=%d, fAscending=%d"),
//               (fDiatonic ? 1 : 0), int(dpMin), int(dpMax), int(fpStartNote),
//               (fAscending ? 1 : 0) );

    //posible valid intervals for aural training
    static FIntval m_interval[25] = {
        lm_p1,  lm_m2,  lm_M2,  lm_m3,  lm_M3, lm_p4,  lm_a4,  lm_p5,  lm_m6,  lm_M6,
        lm_m7,  lm_M7,  lm_p8,  lm_m9,  lm_M9, lm_m10, lm_M10, lm_p11, lm_a11, lm_p12,
        lm_m13, lm_M13, lm_m14, lm_M14, lm_p15
    };

    //compute max allowed interval for the specified notes range
    FPitch fpMin = dpMin.to_FPitch(nKey);
    FPitch fpMax = dpMax.to_FPitch(nKey);
    FIntval maxIntv = FIntval(fpMax - fpMin);
    if (maxIntv > lm_p15)
        maxIntv = lm_p15;

    //extract and count allowed intervals
    int nAllowedIntv[25];
    int nNumIntv = 0;
    for (int i=0; i < 25; i++)
    {
        if (fAllowedIntervals[i])
        {
            nAllowedIntv[nNumIntv] = i;
//            LogMessage(_T("[Interval::Interval] nAllowedIntv[%d]=%d"), nNumIntv, i);
            nNumIntv++;
        }
    }
//    LogMessage(_T("[Interval::Interval] nNumIntv=%d"), nNumIntv);

    //select an interval at random. This is the first thing to do in order that all
    //intervals have the same probability. Other algorithms that I have tried
    //don't work because give more probability to some intervals
    vector<FPitch> validNotes;
    int selIntv;
    while(nNumIntv != 0)
    {
        //select interval
        int iSel = RandomGenerator::random_number(0, nNumIntv - 1);
        selIntv = (int)m_interval[ nAllowedIntv[iSel] ];
//        LogMessage(_T("[Interval::Interval] iSel=%d, selIntv=%d"), iSel, selIntv);

        //determine max minimum note for the selected interval
        FPitch fpTop = fpMax - selIntv;
        DiatonicPitch dpTop(fpTop.step(), fpTop.octave());

        //determine valid notes to start the choosen interval
        DiatonicPitch dpCur = dpMin;
        do
        {
            FPitch fpFirst = dpCur.to_FPitch(nKey);
            FPitch fpSecond = fpFirst + selIntv;
//            LogMessage(_T("[Interval::Interval] fpFirst=%d, fpSecond=%d"),
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
//        LogMessage(_T("[Interval::Interval] validNotes.size()=%d"), validNotes.size());
        if (validNotes.size() > 0)
            break;

        //the interval can not be generated because there are no valid
        //notes to build it. Discard this interval and choose anoher one.
//        LogMessage(_T("[Interval::Interval] Cannot generate, nNumIntv=%d, iSel=%d"), nNumIntv, iSel);
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
//        LogMessage(_T("[Interval::Interval] Cannot generate interval"));
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

//    LogMessage(_T("[Interval::Interval] int: fp0=%d, fp1=%d"), int(m_fp[0]), int(m_fp[1]));
}

//---------------------------------------------------------------------------------------
int Interval::get_num_semitones()
{
    return get_interval().get_num_semitones();
}

//---------------------------------------------------------------------------------------
wxString Interval::get_interval_name()
{
    wxString name = get_interval().get_name();
    if (get_interval() != lm_p1)
    {
        name += _T(", ");
        name += (m_fp[1] > m_fp[0] ? _("ascending") : _("descending") );
    }
    return name;
}



////-------------------------------------------------------------------------------------
//// Global methods
////-------------------------------------------------------------------------------------
//
//wxString ComputeInterval(wxString sRootNote, wxString sIntvCode,
//                         bool fAscending, EKeySignature nKey)
//{
//    //Root note elements. i.e.: '+d4' -> (1, 4, 1)
//    NoteBits tRoot;
//    if (lmConverter::NoteToBits(sRootNote, &tRoot)) {
//        wxLogMessage(_T("[ComputeInterval] Unexpected error in lmConverter::NoteToBits coversion. Note: '%s'"),
//                sRootNote.wx_str() );
//        wxASSERT(false);
//    }
//
//    NoteBits tNew;
//    ComputeInterval(&tRoot, sIntvCode, fAscending, &tNew);
//
//    return lmConverter::NoteBitsToName(tNew, nKey);
//
//}
//
////---------------------------------------------------------------------------------------
//void ComputeInterval(NoteBits* pRoot, wxString sIntvCode,
//                     bool fAscending, NoteBits* pNewNote)
//{
//    //interval elements. i.e.: '5a' -> (5, 8)
//    lmIntvBits tIntval;
//    if (IntervalCodeToBits(sIntvCode, &tIntval)) {
//        wxLogMessage(_T("[ComputeInterval] Unexpected error in IntervalCodeToBits coversion. Interval: '%s'"),
//                sIntvCode.wx_str() );
//        wxASSERT(false);
//    }
//
//    if (fAscending) {
//        // Compute ascending interval
//
//        // compute LENMUS_NEW step
//        int nNewStepFull = pRoot->nStep + tIntval.nNum - 1;
//        pNewNote->nStep = nNewStepFull % 7;
//
//        //compute octave increment
//        int nIncrOctave = (pNewNote->nStep == nNewStepFull ? 0 : (nNewStepFull - pNewNote->nStep)/7 );
//        pNewNote->nOctave = pRoot->nOctave + nIncrOctave;
//
//        //compute LENMUS_NEW step semitones
//        pNewNote->nStepSemitones = lmConverter::StepToSemitones(pNewNote->nStep);
//
//        //compute LENMUS_NEW accidentals
//        pNewNote->nAccidentals = (pRoot->nStepSemitones + tIntval.nSemitones -
//                            pNewNote->nStepSemitones) % 12 + pRoot->nAccidentals;
//        if (pNewNote->nAccidentals > 5) pNewNote->nAccidentals -= 12;
//    }
//    else {
//        //compute descending interval
//
//        // compute LENMUS_NEW step and octave decrement
//        int nNewStepFull = pRoot->nStep - (tIntval.nNum - 1);
//        pNewNote->nStep = nNewStepFull % 7;
//        int nIncrOctave = 0;    //assume no octave change
//        if (nNewStepFull < 0) {
//            if (pNewNote->nStep < 0) pNewNote->nStep += 7;
//            nIncrOctave = (nNewStepFull - pNewNote->nStep)/7 ;
//        }
//        pNewNote->nOctave = pRoot->nOctave + nIncrOctave;
//
//        //compute LENMUS_NEW step semitones
//        pNewNote->nStepSemitones = lmConverter::StepToSemitones(pNewNote->nStep);
//
//        //compute LENMUS_NEW accidentals
//        pNewNote->nAccidentals = (pRoot->nStepSemitones - tIntval.nSemitones -
//                            pNewNote->nStepSemitones) % 12 + pRoot->nAccidentals;
//        if (pNewNote->nAccidentals < -5) pNewNote->nAccidentals += 12;
//    }
//
//
//}
//
////---------------------------------------------------------------------------------------
//bool IntervalCodeToBits(wxString sIntvCode, lmIntvBits* pBits)
//{
//    //return true if error
//    //Restrictions: any interval , including greater that one octave, but limited to
//    // d, m , p , M and a. That is, it is not allowed double augmented, double diminished, etc.
//
//    //split interval number and type
//    int i = 0;
//    while (!(sIntvCode.Mid(i, 1)).IsNumber() ) {
//        i++;
//    }
//    wxString sChar = sIntvCode.substr(0, i);
//    wxString sNum = sIntvCode.substr(i);
//
//    long nNum;
//    sNum.ToLong(&nNum);
//    pBits->nNum = (int)nNum;
//
//    //reduce the interval
//    int nOctaves = (nNum - 1) / 7;
//    nNum = 1 + (nNum - 1) % 7;
//
//    // compute semitones implied by interval type
//    int nSemi;
//    if (nNum == 1)      nSemi = -1;
//    else if (nNum == 2) nSemi = 0;
//    else if (nNum == 3) nSemi = 2;
//    else if (nNum == 4) nSemi = 4;
//    else if (nNum == 5) nSemi = 6;
//    else if (nNum == 6) nSemi = 7;
//    else if (nNum == 7) nSemi = 9;
//    else
//    {
//        wxLogMessage(_T("[IntervalCodeToBits] Program error: Invalid interval '%s', nNum=%d"),
//                sIntvCode.wx_str(), nNum );
//        wxASSERT(false);    //impossible
//    }
//
//    if ( nNum == 1 || nNum == 4 || nNum == 5) {
//        if (sChar == _T("d"))       nSemi += 0;
//        else if (sChar == _T("p"))  nSemi += 1;
//        else if (sChar == _T("a"))  nSemi += 2;
//        else
//        {
//            wxLogMessage(_T("[IntervalCodeToBits] Program error: Invalid interval '%s', nNum=%d, sChar='%s'"),
//                    sIntvCode.wx_str(), nNum, sChar.wx_str() );
//            wxASSERT(false);    //impossible
//        }
//    }
//    else {  // 2, 3, 6, 7
//        if (sChar == _T("d"))       nSemi += 0;
//        else if (sChar == _T("m"))  nSemi += 1;
//        else if (sChar == _T("M"))  nSemi += 2;
//        else if (sChar == _T("a"))  nSemi += 3;
//        else
//        {
//            wxLogMessage(_T("[IntervalCodeToBits] Program error: Invalid interval '%s', nNum=%d, sChar='%s'"),
//                    sIntvCode.wx_str(), nNum, sChar.wx_str() );
//            wxASSERT(false);    //impossible
//        }
//    }
//    pBits->nSemitones = nSemi + 12 * nOctaves;
//
//    return false;
//
//}
//
////---------------------------------------------------------------------------------------
//wxString IntervalBitsToCode(lmIntvBits& tIntv)
//{
//    g_pLogger->LogTrace(_T("Interval"),
//            _T("IntervalBitsToCode: nNum=%d, nSemitones=%d\n"),
//            tIntv.nNum, tIntv.nSemitones );
//
//    wxString sNormal = _T("dmMa");
//    wxString sPerfect = _T("dpa");
//    int nSemitones = tIntv.nSemitones % 12;
//    int nNum = 1 + (tIntv.nNum - 1) % 7;
//    wxString sResp;
//    if (nNum == 1)  {
//        // unison, octave
//        // For unison nSemitones = -1, 0, 1
//        // For octave nSemitones = 11, 12, 13 (reduced in previous step to 11,0,1)
//        //    so we need to correct semitones
//        if (nSemitones == 11) nSemitones = -1;
//        sResp = sPerfect.substr(nSemitones + 1, 1);
//    }
//    else if (nNum == 2)
//        sResp = sNormal.substr(nSemitones, 1);
//    else if (nNum == 3)
//        sResp = sNormal.substr(nSemitones - 2, 1);
//    else if (nNum == 4)
//        sResp = sPerfect.substr(nSemitones - 4, 1);
//    else if (nNum == 5)
//        sResp = sPerfect.substr(nSemitones - 6, 1);
//    else if (nNum == 6)
//        sResp = sNormal.substr(nSemitones - 7, 1);
//    else if (nNum == 7)
//        sResp = sNormal.substr(nSemitones - 9, 1);
//    else
//        wxASSERT(false);    //impossible
//
//
//    sResp += wxString::Format(_T("%d"), tIntv.nNum);
//    return sResp;
//
//}


}   //namespace lenmus
