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

#ifndef __COLSTAFFOBJS_H__        //to avoid nested includes
#define __COLSTAFFOBJS_H__

#ifdef __GNUG__
#pragma interface "ColStaffObjs.cpp"
#endif

#include "Score.h"

// to manage bars let's define an array to contain pointers to the StaffObjsList nodes
// where each measure start
#include "wx/dynarray.h"
WX_DEFINE_ARRAY(wxStaffObjsListNode*, wxArrayNodes);



class lmColStaffObjs
{
public:
    lmColStaffObjs();
    ~lmColStaffObjs();

    void Store(lmStaffObj *pSO);
    int GetNumMeasures();
    void ShiftTime(float rTimeShift);

    //iterator related methods
    lmStaffObjIterator* CreateIterator(ETraversingOrder nOrder);
    int GetNumStaffObjs();
    wxStaffObjsListNode* GetFirstInMeasure(int nBar);
    wxStaffObjsListNode* GetFirst();
    wxStaffObjsListNode* GetLast();
    //lmStaffObj* GetNext();
    //StaffObjsList::Node *m_pNode;    //last returned node

private:

    StaffObjsList    m_cStaffobjs;        //list of StaffObjs that belongs to this collection
    wxArrayNodes    m_aStartMeasure;    //Node (lmStaffObj list) in which each measure starts
    bool            m_fStartMeasure;    //start a new measure. To manage the creation of measures

    //time from start of the measure
    //Time is measured in semigarrapateas: 1-semigarrapatea, ..., 256-redonda, 512-cuadrada
    //Float for grater precision (triplets problem, see comment at CPONota.ExactDuration)
    float            m_rTimePos;

    //aqui se guarda el máximo tiempo alcanzado en el compas en curso. Sirve para evitar que una
    //orden <avanzar> sobrepase este valor. Además, este será el tiempo que corresponde a la barra
    //de fin de compas.
    float            m_rMaxTimePos;

};

#endif    // __COLSTAFFOBJS_H__
