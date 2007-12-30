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

//----------------------------------------------------------------------------------------------------
/*! @class lmColStaffObjs
    @ingroup score_kernel
    @brief A Collection of StaffObjs

 Esta clase encapsula las estructuras de datos y sus métodos de mantenimiento y acceso, para soportar
 la representación interna de una colección de StaffObjs (una CStaff, con sus compases y StaffObjs).

 Inicialmente se modelizaba dentro de CStaff. El problema era que a medida que se complicaba el modelo
 de objetos se veian afectadas muchas funciones de CStaff. Para independizar
 a las funciones de la implementación concreta de la secuencia de StaffObjs se ha creado esta
 clase. Así, podrá cambiarse en el futuro su implenetación (por ejemplo, para mejorar rendimiento)
 sin que se vea afectado el resto de clases.

 Lo único que hay que respetar son las siguientes restricciones:
 - Un compas termina siempre con un lmStaffObj de tipo Barra.
 - Cualquiera que sea el tipo de recorrido elegido, respeta que los StaffObjs de un compas no se
   recuperan antes que los del siguiente compas. Además, respeta que la barra de fin de compas sea
   el último elemento que se recupera de un compas.

 Para facilitar las futuras modificaciones en las estructuras de datos que implementan esta
 clase, cuando se crea un iterador se especifica el tipo de recorrido necesario. La implementación
 debe siempre respetar esos criterios. Los códigos y sus significados son:

 - eTR_AsStored:
       Se recorren por el orden en que se encuentran definidos en las estructuras de datos.
       No tiene que respetar ninguna restricción, sino que debe proporcionar los StaffObjs
       en el orden en que realmente se encuentren almacenados.
       Esta ordenación se utiliza para depuración, en el método Dump.
       También se usa cuando hay que acceder a todos los StaffObjs de la CStaff y el orden
       de acceso es totalmente indiferente, ya que siempre será el acceso más rápido. Por
       ejemplo en CStaff.EstablecerPapel

 - eTR_ByTime:
       The StaffObjs are recovered ordered by increasing time pos

 - eTR_OptimizeAccess:
       the recovery order inside a measure is not important. So use the ordering that results in
       the fastest access time

 - Cualquiera que sea el tipo de recorrido elegido, respeta que los StaffObjs de un compas no se
   recuperan antes que los del siguiente compas. Además, respeta que la barra de fin de compas sea
   el último elemento que se recupera de un compas.

*/
//----------------------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ColStaffObjs.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <algorithm>

#include "Score.h"
#include "wx/debug.h"
#include "StaffObjIterator.h"

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;


//====================================================================================================
//Constructors and destructors
//====================================================================================================
lmColStaffObjs::lmColStaffObjs()
{
    m_fStartMeasure = true;
    m_rTime = 0;                   //reset time counters
    m_rMaxTime = 0;

}

lmColStaffObjs::~lmColStaffObjs()
{
	lmItCSO itSO;
	for (itSO = m_cStaffobjs.begin(); itSO != m_cStaffobjs.end(); itSO++)
	{
		delete *itSO;
	}
    m_cStaffobjs.clear();

	//delete measures data
	lmItMeasure itM;
	for (itM = m_aMeasureData.begin(); itM != m_aMeasureData.end(); itM++)
	{
		delete *itM;
    }
	m_aMeasureData.clear();

	m_aMeasures.clear();

}


//====================================================================================================
//Methods to add, insert and delete StaffObjs
//====================================================================================================


void lmColStaffObjs::Store(lmStaffObj* pSO)
{
    //Stores the received StaffObj in this collection, inserting it at the end, before EOS
    //object 
    //Controls division into measures and assign time position to the lmStaffObj

    //assign time to the StaffObj and increment time counters
	AssignTime(pSO, &m_rTime, &m_rMaxTime);
    
    //store the lmStaffObj in the collection. StaffObjs are stored by arrival order
	lmItCSO pItem;
	//if (pSO->GetClass() == eSFOT_Barline && ((lmBarline*)pSO)->GetBarlineType() == lm_eBarlineEOS)
 //   {
 //       //it is the EOS object. Add it and finish
        m_cStaffobjs.push_back(pSO);    //insert at the end
		pItem = --m_cStaffobjs.end();     //point to inserted object
	//}
	//else 
	//{
	//	//insert before the EOS object
	//	pItem = --m_cStaffobjs.end();     //point to EOS object
	//	wxASSERT ((*pItem)->GetClass() == eSFOT_Barline && ((lmBarline*)(*pItem))->GetBarlineType() == lm_eBarlineEOS );
	//	m_cStaffobjs.insert(pItem, pSO);    //insert before item pointed by pItem

	//	//update EOS time
	//	(*pItem)->SetTimePos(pSO->GetTimePos());

	//	pItem--;    //point to inserted object
	//}

    //if this lmStaffObj is the first one of a measure, store a pointer to the node in the
    //measures table
    if (m_fStartMeasure) 
    {
        m_fStartMeasure = false;
		AddMeasure(pItem, pItem);
    }
    
    //store, inside the lmStaffObj, a ptr to the measure data
	pSO->SetItMeasure( m_aMeasures[m_aMeasures.size() - 1] );

    //Finally, if this lmStaffObj is a barline, signal that a new measure must be started
    //for the next lmStaffObj, reset time counters, and update measure data
    if (pSO->GetClass() == eSFOT_Barline)
    {
        m_fStartMeasure = true;		//a new measure must be started with next StaffObj

		lmItMeasure itM = pSO->GetItMeasure();		//update measure data
		(*itM)->itEndSO = pItem;

        m_rTime = 0;				//reset time counters
        m_rMaxTime = 0;
    }
    
}

//void lmColStaffObjs::Store(lmStaffObj* pSO)
//{
//    //Stores the received StaffObj in this collection, inserting it at the end, before EOS
//    //objcet 
//    //Controls division into measures and assign time position to the lmStaffObj
//
//    //assign time to the StaffObj and increment time counters
//	AssignTime(pSO, &m_rTime, &m_rMaxTime);
//    
//    //store the lmStaffObj in the collection. StaffObjs are stored by arrival order
//	lmItCSO pItem;
//	if (pSO->GetClass() == eSFOT_Barline && ((lmBarline*)pSO)->GetBarlineType() == lm_eBarlineEOS)
//    {
//        //it is the EOS object
//        m_cStaffobjs.push_back(pSO);    //insert at the end
//		pItem = --m_cStaffobjs.end();     //point to EOS object
//	}
//	else 
//	{
//		//insert before the EOS object
//		pItem = --m_cStaffobjs.end();     //point to EOS object
//		wxASSERT ((*pItem)->GetClass() == eSFOT_Barline && ((lmBarline*)(*pItem))->GetBarlineType() == lm_eBarlineEOS );
//		Insert(pSO, (*pItem));
//		return;
//	}
//
//    //if this lmStaffObj is the first one of a measure, store a pointer to the node in the
//    //measures table
//    if (m_fStartMeasure) 
//    {
//        m_fStartMeasure = false;
//		AddMeasure(pItem, pItem);
//    }
//    
//    //store, inside the lmStaffObj, a ptr to the measure data
//	pSO->SetItMeasure( m_aMeasures[m_aMeasures.size() - 1] );
//
//    //Finally, if this lmStaffObj is a barline, signal that a new measure must be started
//    //for the next lmStaffObj, reset time counters, and update measure data
//    if (pSO->GetClass() == eSFOT_Barline)
//    {
//        m_fStartMeasure = true;		//a new measure must be started with next StaffObj
//
//		lmItMeasure itM = pSO->GetItMeasure();		//update measure data
//		(*itM)->itEndSO = pItem;
//
//        m_rTime = 0;				//reset time counters
//        m_rMaxTime = 0;
//    }
//    
//}


void lmColStaffObjs::Insert(lmStaffObj* pNewSO, lmStaffObj* pBeforeSO)
{
    //Stores the new StaffObj (pNewSO) in this collection, inserting it BEFORE the 
	//StaffObj pBeforeSO. Insertion DOES NOT move objects to new mesures, so this
	//operation could create irregular measures

	//locate insertion point
	lmItMeasure itBeforeM = pBeforeSO->GetItMeasure();
	lmMeasureData* pMData = *itBeforeM;
	lmItCSO itFromCSO = pMData->itStartSO;
	lmItCSO itToCSO = pMData->itEndSO; itToCSO++;
	lmItCSO itBeforeCSO = std::find(itFromCSO, itToCSO, pBeforeSO);

	//initialize time counters:
	float rTime = pBeforeSO->GetTimePos();
	lmItCSO itEndCSO = pMData->itEndSO;
	lmStaffObj* pEndSO = *itEndCSO;
	float rMaxTime = (*(pMData->itEndSO))->GetTimePos();
	int nNumMeasure = pMData->nNumMeasure;
	
    //assign the time position of StattObj pointed by itBeforeCSO
    //assign time to the StaffObj and increment time counters
	AssignTime(pNewSO, &rTime, &rMaxTime);
    
    //insert the lmStaffObj in the collection, before the specified object
    m_cStaffobjs.insert(itBeforeCSO, pNewSO);	//insert before item pointed by itBeforeCSO
    lmItCSO itNewCSO = itBeforeCSO;				//iterator to point to the new inserted object
	itNewCSO--;								

	//if this StaffObj is a barline it is necessary to update the end of current measure
	//and to create a new one before this one
    if (pNewSO->GetClass() == eSFOT_Barline)
    {
		InsertMeasure(itBeforeM, pMData->itStartSO, itNewCSO);
		pMData->itStartSO = itBeforeCSO;
    }
	else
	{
		//if the insertion point StaffObj is the first one of a measure, we have to update
		//the pointer in the measuresData table, unless the new inserted one is a barline
		lmItCSO itOldStartCSO = pMData->itStartSO;
		if (*itOldStartCSO == pBeforeSO) 
			pMData->itStartSO = itNewCSO;
	}
    
    //store, inside the lmStaffObj, a ptr to the measure data
	pNewSO->SetItMeasure( m_aMeasures[nNumMeasure-1] );

	//Debug: force a dump of StaffObjs collection and measures tables
	lmItMeasure itMNew = pNewSO->GetItMeasure();
	lmMeasureData* pMDataNew = *itMNew;
	itFromCSO = pMDataNew->itStartSO;
	itToCSO = pMDataNew->itEndSO;
	nNumMeasure = pMDataNew->nNumMeasure;
	wxLogMessage(_T("[lmColStaffObjs::Insert] (Before) Inserted pSO ID=%d, Measure %d, Start ID=%d, End ID=%d"),
		pNewSO->GetID(), nNumMeasure, (*itFromCSO)->GetID(),  (*itToCSO)->GetID() );

	RepositionObjects(itBeforeCSO, &rTime, &rMaxTime);

	//Debug: force a dump of StaffObjs collection and measures tables
	itMNew = pNewSO->GetItMeasure();
	pMDataNew = *itMNew;
	itFromCSO = pMDataNew->itStartSO;
	itToCSO = pMDataNew->itEndSO;
	nNumMeasure = pMDataNew->nNumMeasure;
	wxLogMessage(_T("[lmColStaffObjs::Insert] (After) Inserted pSO ID=%d, Measure %d, Start ID=%d, End ID=%d"),
		pNewSO->GetID(), nNumMeasure, (*itFromCSO)->GetID(),  (*itToCSO)->GetID() );
    #if defined(__WXDEBUG__)
    wxString sDump = _T("");
	sDump += DumpStaffObjs();
	sDump += DumpMeasuresData();
	sDump += DumpMeasures();
    g_pLogger->LogTrace(_T("lmColStaffObjs::Insert"), sDump );
    #endif

}

void lmColStaffObjs::Delete(lmStaffObj* pCursorSO)
{
	//Delete the StaffObj pointed by pCursorSO

	//get some data
	EStaffObjType nDeletedObjType = pCursorSO->GetClass();
	lmItMeasure itMCursor = pCursorSO->GetItMeasure();
	lmMeasureData* pMDataCursor = *itMCursor;
	lmItCSO itFromCSO = pMDataCursor->itStartSO;
	lmItCSO itToCSO = pMDataCursor->itEndSO;
	lmStaffObj* pBarlineSO = *itToCSO;			//barline for current measure
	itToCSO++;
	lmItCSO itCursorCSO = std::find(itFromCSO, itToCSO, pCursorSO);
	lmItCSO itNextCSO = itCursorCSO; itNextCSO++;
	lmStaffObj* pNextSO = *itNextCSO;
	lmItMeasure itMNext = pNextSO->GetItMeasure();
	int nMeasure = pMDataCursor->nNumMeasure;

	//the EOS Barline can not be deleted
	if (nDeletedObjType == eSFOT_Barline 
		&& ((lmBarline*)pCursorSO)->GetBarlineType() == lm_eBarlineEOS) return;

	// 2. initialize time counters:
	//		current time = time assigned to CursorSO
	//		max.time = time assigned to barline of current measure
	float rTime = pCursorSO->GetTimePos();
	float rMaxTime = pBarlineSO->GetTimePos();
	
	// 3. if object to delete is the first one of measure update MeasureData
	if (*(pMDataCursor->itStartSO) == pCursorSO)
		pMDataCursor->itStartSO = itNextCSO;

	// 4. Remove the StaffObj CursorSO
	m_cStaffobjs.erase(itCursorCSO);

	// 5. if CursorSO is a barline we have to update the end of current measureData
	if (nDeletedObjType == eSFOT_Barline)
		pMDataCursor->itEndSO = (*itMNext)->itEndSO;

	// 4. Update time & MeasureData ptr of all following StaffObjs (from NextSO to 
	//	  first barline):
	// 4.1 Asign: Cur_StaffObj = NextSO
	// 4.2 While not end of collection
	while (itNextCSO != m_cStaffobjs.end())
	{
		// 4.2.1. Assign time to it
		AssignTime(*itNextCSO, &rTime, &rMaxTime);

		// 4.2.2. Update StaffObj information: MeasureData = MeasureData[CursorSO]
		(*itNextCSO)->SetItMeasure( itMCursor );

		// 4.2.3 if Cur_StaffObj is a barline break loop
		//		 else take next StaffObj
		if ((*itNextCSO)->GetClass() == eSFOT_Barline) break;
		itNextCSO++;
	}

	// If CursorSO is a barline we have to update measures:
	if (nDeletedObjType == eSFOT_Barline)
	{
		// 6. From current MeasureData (NextSO) to end of MeasureData collection
		//    MeasureData[i].numMeasure --
		lmItMeasure itM = itMNext; itM++;
		while (itM != m_aMeasureData.end())
		{
			(*itM)->nNumMeasure--;
			itM++;
		}

		// 7. Delete MeasureData[NextSO]
		m_aMeasureData.erase( itMNext );

		// 8. From Measure nMeasure+1 to end of vector aMeasures - 1:
		//	  element[i] = element[i+1]
		for (int i=nMeasure; i < (int)m_aMeasures.size()-1; i++)
			m_aMeasures[i] = m_aMeasures[i+1];

		// 9. Remove last element of aMeasures
		m_aMeasures.pop_back();

	}
    #if defined(__WXDEBUG__)
    wxString sDump = _T("");
	sDump += DumpStaffObjs();
	sDump += DumpMeasuresData();
	sDump += DumpMeasures();
    g_pLogger->LogTrace(_T("lmColStaffObjs::Delete"), sDump );
    #endif
}

void lmColStaffObjs::AssignTime(lmStaffObj* pSO, float* pTime, float* pMaxTime)
{
	// Assigns a time to the StaffObj and update time counters
	
	//A1. Assign StaffObj a time:

	//If it is a note in chord and not base note assign it the time assigned to 
	//base note
	bool fPartOfChord = false;		//to avoid having to check this many times
    if (pSO->GetClass() == eSFOT_NoteRest && !(((lmNoteRest*)pSO)->IsRest())
       && ((lmNote*)pSO)->IsInChord() && !((lmNote*)pSO)->IsBaseOfChord() )
	{
        lmNote* pNoteBase = ((lmNote*)pSO)->GetChord()->GetBaseNote();
        pSO->SetTimePos(pNoteBase->GetTimePos());
		fPartOfChord = true;
	}

	//Else if it is a barline assing it the maximum time position. This is done
	//because current time counter could not be at the end, due to a backup tag.
	else if (pSO->GetClass() == eSFOT_Barline) {
        pSO->SetTimePos(*pMaxTime);
	}

	//Else, assign it current time value
	else
		pSO->SetTimePos(*pTime);


	//A2. Increment current time counter
	//If note in chord and not base note do not increment counter
	//Else increment counter by StaffObj duration
    if (pSO->GetClass() == eSFOT_NoteRest)
	{
		if (!fPartOfChord)
			*pTime += ((lmNoteRest*)pSO)->GetDuration();
	}
	else
		*pTime += pSO->GetTimePosIncrement();


	//A3. Update max time counter
	*pMaxTime = wxMax(*pMaxTime, *pTime);

}

void lmColStaffObjs::RepositionObjects(lmItCSO itCSO, float* pTime, float* pMaxTime)
{
	//B6. Update timePos of all following StaffObjs (from pItem to first barline):

	while (itCSO != m_cStaffobjs.end())
	{
		AssignTime(*itCSO, pTime, pMaxTime);
		if ((*itCSO)->GetClass() == eSFOT_Barline) break;
		itCSO++;
	}
}

void lmColStaffObjs::InsertMeasure(lmItMeasure itMBefore, lmItCSO itStartSO,
								   lmItCSO itEndSO)
{
	//Inserts a new measure before measure pointed by itMBefore. Iterators
	//itStartSO and itEndSO must point to start and end StaffObjs for this
	//new created measure

	//create the measure data item
	int nNumMeasure = (*itMBefore)->nNumMeasure;
	lmMeasureData* pData = new lmMeasureData;
	pData->itStartSO = itStartSO;
	pData->itEndSO = itEndSO;
	pData->nNumMeasure = nNumMeasure;
	m_aMeasureData.insert(itMBefore, pData);

	//update measures table:
	//1) make room for a new measure by inserting a new element at the end
	m_aMeasures.push_back( m_aMeasureData.end() );
	//2) move data from element i to element i+1
	for (int i=(int)m_aMeasures.size()-2; i >= nNumMeasure-1; i--)
		m_aMeasures[i+1] = m_aMeasures[i];
	//3)update the inserted element
	m_aMeasures[nNumMeasure-1] = --itMBefore;		//new inserted item

	//update measure numbers in m_aMeasureData
	for (lmItMeasure itM = ++itMBefore; itM != m_aMeasureData.end(); itM++)
	{
		(*itM)->nNumMeasure++;
    }

}

void lmColStaffObjs::AddMeasure(lmItCSO itStartSO, lmItCSO itEndSO)
{
	//Adds a new measure afater last measure

	//create the measure data item
	lmMeasureData* pData = new lmMeasureData;
	pData->itStartSO = itStartSO;
	pData->itEndSO = itEndSO;
	pData->nNumMeasure = (int)m_aMeasures.size() + 1;
	m_aMeasureData.push_back(pData);

	//update measures table
	m_aMeasures.push_back( --m_aMeasureData.end() );

	wxASSERT(pData->nNumMeasure == (int)m_aMeasures.size());
}


//=================================================================================
//Methods for accesing specific items and information about the collection
//=================================================================================

int lmColStaffObjs::GetNumStaffObjs()
{
    //returns the number of StaffObjs in the collection
    return (int)m_cStaffobjs.size();
}

int lmColStaffObjs::GetNumMeasures()
{
    //returns the number of measures in the collection
    return (int)m_aMeasures.size();
    
}

lmItCSO lmColStaffObjs::GetFirstInMeasure(int nMeasure)
{
    //returns the first lmStaffObj in measure number nMeasure (1..n)
    wxASSERT(nMeasure > 0 && nMeasure <= (int)m_aMeasures.size());
    lmItMeasure pMeasure = m_aMeasures[nMeasure-1];
	return (*pMeasure)->itStartSO;
    
}

lmItCSO lmColStaffObjs::GetFirst()
{
    return m_cStaffobjs.begin();
}

lmItCSO lmColStaffObjs::GetLast()
{
    return --m_cStaffobjs.end();
}

bool lmColStaffObjs::EndOfList(lmItCSO pNode)
{
    return pNode == m_cStaffobjs.end();
}

bool lmColStaffObjs::StartOfList(lmItCSO pNode)
{
    // returns true if cursor points to before first item
    return pNode == --m_cStaffobjs.begin();
}

lmItCSO lmColStaffObjs::GetIteratorTo(lmStaffObj* pSO)
{
	//returns an iterator pointing to StaffObj pSO

	lmItMeasure itM = pSO->GetItMeasure();
	lmItCSO itFromCSO = (*itM)->itStartSO;
	lmItCSO itToCSO = (*itM)->itEndSO; 	itToCSO++;
	return std::find(itFromCSO, itToCSO, pSO);
}

//====================================================================================================
//Debug methods
//====================================================================================================

wxString lmColStaffObjs::DumpStaffObjs()
{
    wxString sDump = _T("\nStaffObjs collection:\n");
	lmItCSO itSO;
	for (itSO = m_cStaffobjs.begin(); itSO != m_cStaffobjs.end(); itSO++)
	{
		sDump += (*itSO)->Dump();
	}
	return sDump;
}

wxString lmColStaffObjs::DumpMeasuresData()
{
    wxString sDump = _T("\nMeasures Data:\n");
	int i=1;
	lmItMeasure itM;
	for (itM = m_aMeasureData.begin(); itM != m_aMeasureData.end(); itM++, i++)
	{
		sDump += wxString::Format(_T("%d: numMeasure=%d\n"),
								  i, (*itM)->nNumMeasure );
        sDump += wxString::Format(_T("\tStarts with object Id %d\n"),
                                  (*((*itM)->itStartSO))->GetID() );
        sDump += wxString::Format(_T("\tEnds with object Id %d\n"),
                                  (*((*itM)->itEndSO))->GetID() );
    }
	return sDump;
}

wxString lmColStaffObjs::DumpMeasures()
{
	wxString sDump = _T("\nChecking Measures:\n");
	bool fOK = true;
	for (int i=0; i < (int)m_aMeasures.size(); i++)
	{
		lmItMeasure itM = m_aMeasures[i];
		int nNumM = (*itM)->nNumMeasure;
		if (nNumM != i+1)
		{
			sDump += wxString::Format(_T("\tError: Item %d points to measure %d\n"),
								      i, nNumM);
			fOK = false;
		}
		else
			sDump += wxString::Format(_T("\tItem %d points to measure %d\n"),
										i, nNumM);

	}

	if (fOK)
		sDump += _T("\tNo errors found.");

	return sDump;
}




//====================================================================================================
//Other methods
//====================================================================================================

lmStaffObjIterator* lmColStaffObjs::CreateIterator(ETraversingOrder nOrder)
{
    //creates and returns an iterator

    lmStaffObjIterator* pIter = new lmStaffObjIterator(nOrder, this);
    return pIter;
    
}

lmStaffObjIterator* lmColStaffObjs::CreateIteratorTo(lmStaffObj* pSO)
{
    //creates and returns an iterator

    lmStaffObjIterator* pIter = new lmStaffObjIterator(pSO, this);
    return pIter;
    
}

void lmColStaffObjs::ShiftTime(float rTimeShift)
{
    // Shifts time counters, forward or backwards
 
    //update time counters and check that shift is inside current measure
    m_rTime += rTimeShift;
    if (m_rTime < 0) { m_rTime = 0; }      //can not jump back before the start of current measure
    //TODO Check that does not go to the next measure
    //TODO Display error message if jump out of current measure boundaries
//    if (m_rTime > m_rMaxTime) { m_rTime = m_rMaxTime   //can not jump out of this bar

}
