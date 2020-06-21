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

#ifndef __LENMUS_SCORE_CORRECTOR_H__        //to avoid nested includes
#define __LENMUS_SCORE_CORRECTOR_H__

//lomse
#include <lomse_internal_model.h>
using namespace lomse;

//other
#include <vector>
using namespace std;

namespace lenmus
{

//---------------------------------------------------------------------------------------
class Matrix
{
private:
    int m_rows, m_cols;
    int* m_data;

public:
    Matrix()
        : m_rows(0)
        , m_cols(0)
        , m_data(nullptr)
    {
    }

    Matrix(int rows, int cols)
        : m_rows(rows)
        , m_cols(cols)
    {
        m_data = new int[rows*cols];
    }

    ~Matrix()
    {
        delete[] m_data;
    }

    void allocate(int rows, int cols)
    {
        m_rows = rows;
        m_cols = cols;
        m_data = new int[rows*cols];
    }

    inline int& operator()(int iRow, int iCol) { return m_data[m_cols*iRow+iCol]; }
    inline int operator()(int iRow, int iCol) const { return m_data[m_cols*iRow+iCol]; }
    inline int* get_row(int i) { return &m_data[m_cols*i]; }

};

//---------------------------------------------------------------------------------------
// Compares two monophonic scores and marks the differences
class ScoreComparer
{
public:
    ScoreComparer();
    virtual ~ScoreComparer();

    bool are_equal(ImoScore* pA, ImoScore* pB);

    //access to comparison info
    inline int get_number_of_differences() { return m_D; }
    void mark_scores();

protected:
    void free_memory();
    void initialize();
    int encode(ImoScore* pScore, int** X);
    bool encodings_are_equal();
    void allocate_v_matrix();
    void find_lcs();
    void find_optimal_path();
    void compute_differences();
    void set_full_differences();

    int encode_staffobj(ImoStaffObj* pSO);
    void mark_score(ImoScore* pScore, int** X);

protected:
    //scores to compare
    ImoScore* m_pScoreA;
    ImoScore* m_pScoreB;

    //encoded scores
    int* m_A;
    int* m_B;
    int m_N;
    int m_M;
    int m_MAX;

    //vector with all V vectors
    Matrix m_v;
    int m_vSize;    //size of V vector

    //results
    int m_D;        //lenght of SES
    int m_pathSize;
    int* m_xPath;   //to hold solution
    int* m_yPath;

};


}   //namespace lenmus

#endif  // __LENMUS_SCORE_CORRECTOR_H__
