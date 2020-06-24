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

#include "lenmus_score_corrector.h"

//lomse
#include <lomse_staffobjs_table.h>
#include <lomse_im_note.h>
using namespace lomse;


namespace lenmus
{

//=======================================================================================
// ScoreComparer implementation
//=======================================================================================
ScoreComparer::ScoreComparer()
    : m_A(nullptr)
    , m_B(nullptr)
    , m_D(0)
    , m_xPath(nullptr)
    , m_yPath(nullptr)
{
}

//---------------------------------------------------------------------------------------
void ScoreComparer::initialize()
{
    free_memory();

    m_A = nullptr;
    m_B = nullptr;
    m_xPath = nullptr;
    m_yPath = nullptr;
    m_D = 0;
}

//---------------------------------------------------------------------------------------
ScoreComparer::~ScoreComparer()
{
    free_memory();
}

//---------------------------------------------------------------------------------------
void ScoreComparer::free_memory()
{
    delete [] m_A;
    delete [] m_B;
    delete [] m_xPath;
    delete [] m_yPath;
}

//---------------------------------------------------------------------------------------
bool ScoreComparer::are_equal(ImoScore* pA, ImoScore* pB)
{
    //top level method: compares two scores and returns true if both are equal.
    //In case of differences the LCD (longest Common Subsequence) and the
    //SES (Shortest Edit Script) are computed.

    initialize();

    m_pScoreA = pA;
    m_pScoreB = pB;

    m_N = encode(pA, &m_A);
    m_M = encode(pB, &m_B);

    if (encodings_are_equal())
        return true;    //both scores are equal

    //scores are different. Find LCS and SES
    allocate_v_matrix();
    find_lcs();
    if (m_D > 0)
    {
        find_optimal_path();
        compute_differences();
    }
    else
    {
        //nothing in common between both scores
        set_full_differences();
    }

    return false;
}

//---------------------------------------------------------------------------------------
int ScoreComparer::encode(ImoScore* pScore, int** X)
{
    //encode staff objects as integers, to simplify and speed up comparison

    ColStaffObjs* pCol = pScore->get_staffobjs_table();
    int size = pCol->num_entries();
    *X = new int[size];
    ColStaffObjsIterator it;
    int i=0;
    for (it = pCol->begin(); it != pCol->end(); ++it, ++i)
    {
        ImoStaffObj* pSO = static_cast<ImoStaffObj*>( (*it)->imo_object() );
        *(*X+i) = encode_staffobj(pSO);
    }
    return size;
}

//---------------------------------------------------------------------------------------
int ScoreComparer::encode_staffobj(ImoStaffObj* pSO)
{
    int value = 0;

    //encode staffobj type
    if (pSO->is_note_rest())
    {
        if (pSO->is_note())
            value += 200;

        //encode note/rest type
        ImoNoteRest* pNR = static_cast<ImoNoteRest*>(pSO);
        switch(pNR->get_note_type())
        {
            case k_longa:   value += 0;     break;
            case k_breve:   value += 10;    break;
            case k_whole:   value += 20;    break;
            case k_half:    value += 30;    break;
            case k_quarter: value += 40;    break;
            case k_eighth:  value += 50;    break;
            case k_16th:    value += 60;    break;
            case k_32nd:    value += 70;    break;
            case k_64th:    value += 80;    break;
            case k_128th:   value += 90;    break;
            case k_256th:   value += 100;   break;
            default:                        break;
        }

        //encode dots
        value += pNR->get_dots();
    }

    else if (pSO->is_barline())
        value += 1000;

    else
        value = 2000;

    return value;
}

//---------------------------------------------------------------------------------------
bool ScoreComparer::encodings_are_equal()
{
    if (m_N != m_M)
        return false;

    for(int i=0; i < m_N; ++i)
    {
        if (*(m_A+i) != *(m_B+i))
            return false;
    }
    return true;
}

//---------------------------------------------------------------------------------------
void ScoreComparer::allocate_v_matrix()
{
    m_MAX = m_N + m_M;
    m_vSize = 2 * m_MAX + 1;

    m_v.allocate(m_MAX+1, m_vSize);
}

//---------------------------------------------------------------------------------------
void ScoreComparer::find_lcs()
{
    //find the LCS (Longest Common Subsequence). Return length of SES or -1
    //This method implements the Greedy LCS/SES algorithm, as described in Myers, Fig.2,
    //for finding all D-paths. From this information, LCS and SES can be easily deduced.

    // MAX = N+M
    // D limits: [0, MAX]
    // V[k] limits: [− MAX, MAX]
    // for a given D, k is the range [-D, D]
    // * To record an endpoint (x,y) in diagonal k it suffices to retain just x
    //   because y is known to be x − k. Consequently, V is a vector of integers
    //   where V[k] contains the x value of the endpoint of a furthest
    //   reaching path in diagonal k.
    // * There is an V vector for each D value
    // * Matrix m_v[D][k] contains the V vectors for each D value.

    //1. prepare the V vector and initialize it to zeros
    int* V = m_v.get_row(0);
    for (int i=0; i < m_vSize; ++i)
        *(V+i) = 0;

    //2. main loop. Each pass finds the paths for a D value.
    for (int D = 0 ; D <= m_MAX ; D++ )
    {
        if (D > 0)
        {
            //start a new V. Initialize it with a copy of previous one
            int* Z = m_v.get_row(D);
            for (int i = 0; i < m_vSize; ++i)
                *(Z+i) = *(V+i);
            V = Z;
        }

        for (int k = -D ; k <= D ; k += 2 )
        {
            //4. move down or right?
            bool down = ( k == -D ||  (k != D && *(V + k-1+m_MAX) < *(V + k+1+m_MAX)));

            int kPrev = down ? k + 1 : k - 1;

            //provisional end point, if no diagonal
            int xEnd = down ? *(V + kPrev+m_MAX) : *(V + kPrev+m_MAX) + 1;
            int yEnd = xEnd - k;

            //9. check if can follow diagonal and, if yes, update end point
            while (xEnd < m_N && yEnd < m_M && *(m_A + xEnd) == *(m_B + yEnd))
            {
                xEnd++; yEnd++;
            }

            //10. save end point
            *(V + k+m_MAX) = xEnd;

            //11. check if end point reached
            if (xEnd >= m_N && yEnd >= m_M)
            {
                //12. solution found. Length of an SES is D
                m_D = D;
                return;
            }
        }
    }
    //14. Length of an SES is greater than MAX.
    m_D = -1;
    return;
}

//---------------------------------------------------------------------------------------
void ScoreComparer::find_optimal_path()
{
    //compute an optimal path from (0,0) to V[d][k]

    m_xPath = new int[m_D+1];
    m_yPath = new int[m_D+1];

    //begin at final point and move backwards to (0,0)
    int x = m_N;
    int y = m_M;

    int d;
    for (d = m_D; x > 0 || y > 0; --d)
    {
        *(m_xPath + d) = x;
        *(m_yPath + d) = y;

        int* V = m_v.get_row(d);
        int k = x - y;

        //up or left?
        bool up = (k == -d || (k != d && V[k - 1+m_MAX] < V[k + 1+m_MAX]));

        //next start point
        int kNext = up ? k + 1 : k - 1;
        x = V[kNext + m_MAX];
        y = x - kNext;
    }

    if (d >= 0)
    {
        *(m_xPath) = x;
        *(m_yPath) = y;
    }
}

//---------------------------------------------------------------------------------------
void ScoreComparer::compute_differences()
{
    //from optimal path, deletions and insertions are deduced.
    //Vectors m_A and m_B are reused to store the marks, as follows:
    //  m_A[i] == 0 if element i is in LCS
    //         == 1 if element i must be deleted
    //  m_B[i] == 0 if element i is in LCS
    //         == 1 if element i must be inserted

    int iA = 0;
    int iB = 0;
    int i = 0;
    int xPrev = 0;
    int yPrev = 0;

    if (m_xPath[0] == 0)
    {
        xPrev = m_xPath[0];
        yPrev = m_yPath[0];
        i = 1;
    }

    for (; i <= m_D; ++i)
    {
        int xShift = m_xPath[i] - xPrev;
        int yShift = m_yPath[i] - yPrev;
        if (yShift > xShift)
        {
            //vertical shift greater: insertion
            m_B[iB++] = 1;
        }
        else if (yShift < xShift)
        {
            //horizontal shift greater: deletion
            m_A[iA++] = 1;
        }
        else
        {
            //diagonal move.
            m_B[iB++] = 0;
            m_A[iA++] = 0;
        }

        //additional diagonal steps
        int d = max(xShift, yShift) - 1;
        while (d > 0)
        {
            m_B[iB++] = 0;
            m_A[iA++] = 0;
            --d;
        }

        xPrev = m_xPath[i];
        yPrev = m_yPath[i];
    }

    //remaining elements are in LCS
    while (iB < m_M)
        m_B[iB++] = 0;

    while (iA < m_N)
        m_A[iA++] = 0;
}

//---------------------------------------------------------------------------------------
void ScoreComparer::set_full_differences()
{
    for (int i = 0; i < m_M; ++i)
        m_B[i] = 1;

    for (int i = 0; i < m_N; ++i)
        m_A[i] = 1;
}

//---------------------------------------------------------------------------------------
void ScoreComparer::mark_scores()
{
    mark_score(m_pScoreA, &m_A);
    mark_score(m_pScoreB, &m_B);
}

//---------------------------------------------------------------------------------------
void ScoreComparer::mark_score(ImoScore* pScore, int** X)
{
    //add color to inserted/deleted staffobjs

    ColStaffObjs* pCol = pScore->get_staffobjs_table();
    ColStaffObjsIterator it;
    int i=0;
    for (it = pCol->begin(); it != pCol->end(); ++it, ++i)
    {
        ImoStaffObj* pSO = static_cast<ImoStaffObj*>( (*it)->imo_object() );
        if (*(*X+i) == 1)
            pSO->set_color( Color(255,0,0) );
    }
    pScore->set_dirty(true);

}


}   //namespace lenmus
