//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2013 LenMus project
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
// Compares two monophonic scores and marks the differences
class ScoreComparer
{
public:
    ScoreComparer();
    virtual ~ScoreComparer() {}

protected:
    void encode(ImoScore* pScore, vector<int>& X);

    int encode_staffobj(ImoStaffObj* pSO);

protected:
    //encoded scores
    vector<int> m_A;
    vector<int> m_B;

    //matrix with the possible transitions from one string to the other one



};


}   //namespace lenmus

#endif  // __LENMUS_SCORE_CORRECTOR_H__
