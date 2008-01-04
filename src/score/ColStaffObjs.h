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

#ifndef __LM_COLSTAFFOBJS_H__        //to avoid nested includes
#define __LM_COLSTAFFOBJS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ColStaffObjs.cpp"
#endif

#include <vector>
#include <list>

class lmStaffObj;
class lmStaffObjIterator;

// To simplify future modifications of this class (i.e changing the data structures to implement it)
// when an iterator is requested we will force to specify the intended usage so that 
// this class can optimize it. The defined codes are:
//
// enum ETraversingOrder
//	eTR_ByTime:
//		The StaffObjs must be recovered ordered by increasing time position.
//
//	eTR_OptimizeAccess:
//		The recovery order inside a bar is not important. So use the ordering that results in
//		the fastest access time
//
//		Ordering is internal to bars and never afect to the barline, who is always the last item
//		recovered on each bar. That is, in the previous ordering methods, the following
//		restrictions always applies:
//		1. StaffObjs in a bar are always traversed before than those of the next bar.
//		2. StaffBojs of type lmBarline are the last item traversed in each bar.
//
//	eTR_AsStored:
//		Items are recovered in the order in which they are stored in the internal
//		data structures used to implement the collection. This ordering method MUST ONLY
//		BE USED for debugging purposes.

enum ETraversingOrder
{
    eTR_AsStored = 1,        //se recorren por orden de almacenamiento, sin restricciones.
    eTR_ByTime,                //se recorren por marca de tiempo
    eTR_OptimizeAccess        //se recorren en el orden en que resulte más rápido
};


#define lmItMeasure		std::list<lmMeasureData*>::iterator 
#define lmItCSO			std::list<lmStaffObj*>::iterator


typedef struct lmMeasureData_Struct {
	lmItCSO		itStartSO;		//first StaffObj in the measure
	lmItCSO		itEndSO;		//last StaffObj, normally a barline
	int			nNumMeasure;	//measure number: 1..n
} lmMeasureData;



class lmColStaffObjs
{
public:
    lmColStaffObjs();
    ~lmColStaffObjs();

    void Store(lmStaffObj* pSO);
	void Insert(lmStaffObj* pNewSO, lmStaffObj* pBeforeSO);
	void Delete(lmStaffObj* pCursorSO);

    int GetNumMeasures();
    void ShiftTime(float rTimeShift);

    //iterator related methods
    lmStaffObjIterator* CreateIterator(ETraversingOrder nOrder);
	lmStaffObjIterator* CreateIteratorTo(lmStaffObj* pSO);
	lmItCSO GetIteratorTo(lmStaffObj* pSO);
    int GetNumStaffObjs();
    lmItCSO GetFirstInMeasure(int nBar);
    lmItCSO GetFirst();
    lmItCSO GetLast();
    bool EndOfList(lmItCSO pNode);
    bool StartOfList(lmItCSO pNode);

	//debug
	wxString DumpStaffObjs();
	wxString DumpMeasuresData();
	wxString DumpMeasures();


private:
	void AssignTime(lmStaffObj* pSO, float* pTime, float* pMaxTime);
	void RepositionObjects(lmItCSO pItem, float* pTime, float* pMaxTime);
	void AddMeasure(lmItCSO itStartSO, lmItCSO itEndSO);
	void InsertMeasure(lmItMeasure itMBefore, lmItCSO itStartSO, lmItCSO itEndSO);


    std::list<lmStaffObj*>		m_cStaffobjs;		//list of StaffObjs in this collection
    std::list<lmMeasureData*>	m_aMeasureData;		//info about measures
    std::vector<lmItMeasure>	m_aMeasures;		//iterator pointing to m_aMeasureData item

	//info only for Store method (info for last measure)
    bool	m_fStartMeasure;	//start a new measure. To manage the creation of measures
    float	m_rTime[lmMAX_VOICE];	//time from start of the measure. One counter per voice
										//Time is measured in 256th notes: 1-256th, ..., 256-whole, 512-double whole
										//Float for grater precision (triplets problem, see comment at CPONota.ExactDuration)
    float	m_rMaxTime[lmMAX_VOICE]; //aqui se guarda el máximo tiempo alcanzado en el compas en curso. Sirve para evitar que una
										//orden <avanzar> sobrepase este valor. Además, este será el tiempo que corresponde a la barra
										//de fin de compas.

};

#endif    // __LM_COLSTAFFOBJS_H__
