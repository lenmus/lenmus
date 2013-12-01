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
{
}

//---------------------------------------------------------------------------------------
void ScoreComparer::encode(ImoScore* pScore, vector<int>& X)
{
    ColStaffObjs* pCol = pScore->get_staffobjs_table();
    ColStaffObjsIterator it;
    for (it = pCol->begin(); it != pCol->end(); ++it)
    {
        ImoStaffObj* pSO = static_cast<ImoStaffObj*>( (*it)->imo_object() );
        X.push_back( encode_staffobj(pSO) );
    }
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
            case k_32th:    value += 70;    break;
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


}   //namespace lenmus
