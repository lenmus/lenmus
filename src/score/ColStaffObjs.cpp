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
//Funciones de creación, inserción y borrado
//====================================================================================================


//void lmColStaffObjs::Store(lmStaffObj* pSO)
//{
//    //Stores the received StaffObj in this collection, inserting it at the end, before EOS
//    //objcet 
//    //Controls division into measures and assign time position to the lmStaffObj
//
//    //assign time position
//    pSO->SetTimePos(m_rTime);
//    
//    //increment time counters
//    if (pSO->GetClass() == eSFOT_NoteRest) {
//        //only NoteRests increment time counters
//        lmNoteRest* pNR = (lmNoteRest*)pSO;
//        if (pNR->IsRest()) {
//            m_rTime += pNR->GetDuration();
//        } else {
//            //It is a note. Verify if it is part of a chord
//            lmNote* pNote = (lmNote*)pSO;
//            if (!pNote->IsInChord()) {
//                //AWARE time counter is going to be incremented also for chord base notes.
//                //As we are in the process of building the score object from the source code,
//                //chord base notes have not yet being marked as "InChord". This is because
//                //the chord is created when the next note is processed.
//                m_rTime += pNote->GetDuration();
//            } else {
//                //for the same reason, this point is reached only by notes that are in a chord
//                //(but not the base note).
//                //Assing to this note the time position of the note base. Do not increment
//                //time counter as it has been already incremented when storing the base note.
//                lmNote* pNoteBase = (pNote->GetChord())->GetBaseNote();
//                pSO->SetTimePos(pNoteBase->GetTimePos());
//            }
//        }
//        
//    } else {
//        //it is not a lmNoteRest. Increment time counter in the amount coded in the lmStaffObj
//        m_rTime += pSO->GetTimePosIncrement();
//    }
//    
//    //Time counter update. Update now the maximun value reached.
//    m_rMaxTime = wxMax(m_rMaxTime, m_rTime);
//    
//    //if (the lmStaffObj being stored is a barline it is necessary to ensure that
//    //its time position match up the measure duration, as there could have been back and forward
//    //displacements.
//    if (pSO->GetClass() == eSFOT_Barline) {
//        //the object is a barline. Assing to it the maximum time position
//        pSO->SetTimePos(m_rMaxTime);
//    }
//    
//    //store the lmStaffObj in the collection. StaffObjs are stored by arrival order
//    if (pSO->GetClass() == eSFOT_Control && ((lmSOControl*)pSO)->GetCtrolType() == lmEND_OF_STAFF)
//    {
//        //it is the EOS object. Add it and finish
//        m_cStaffobjs.push_back(pSO);    //insert at the end
//        return;
//    }
//
//    //insert before the EOS object
//    lmItCSO pItem = --m_cStaffobjs.end();     //point to EOS object
//    wxASSERT ((*pItem)->GetClass() == eSFOT_Control && ((lmSOControl*)(*pItem))->GetCtrolType() == lmEND_OF_STAFF);
//    m_cStaffobjs.insert(pItem, pSO);    //insert before item pointed by pItem
//    pItem--;    //point to inserted object
//
//    //if this lmStaffObj is the first one of a measure, store a pointer to the node in the
//    //measures table
//    if (m_fStartMeasure) 
//    {
//        m_fStartMeasure = false;
//        m_aMeasures.push_back(pItem);
//    }
//    
//    //store, inside the lmStaffObj, a ptr to the collection
//	pSO->SetItMeasure(pItem);
//
//    //Finally, if this lmStaffObj is a barline, signal that a new measure must be started
//    //for the next lmStaffObj and reset time counters
//    if (pSO->GetClass() == eSFOT_Barline)
//    {
//        m_fStartMeasure = true;
//        m_rTime = 0;            //reset time counters
//        m_rMaxTime = 0;
//    }
//    
//}

void lmColStaffObjs::Store(lmStaffObj* pSO)
{
    //Stores the received StaffObj in this collection, inserting it at the end, before EOS
    //objcet 
    //Controls division into measures and assign time position to the lmStaffObj

    //assign time to the StaffObj and increment time counters
	AssignTime(pSO, &m_rTime, &m_rMaxTime);
    
    //store the lmStaffObj in the collection. StaffObjs are stored by arrival order
    if (pSO->GetClass() == eSFOT_Control && ((lmSOControl*)pSO)->GetCtrolType() == lmEND_OF_STAFF)
    {
        //it is the EOS object. Add it and finish
        m_cStaffobjs.push_back(pSO);    //insert at the end
        return;
    }

    //insert before the EOS object
    lmItCSO pItem = --m_cStaffobjs.end();     //point to EOS object
    wxASSERT ((*pItem)->GetClass() == eSFOT_Control && ((lmSOControl*)(*pItem))->GetCtrolType() == lmEND_OF_STAFF);
    m_cStaffobjs.insert(pItem, pSO);    //insert before item pointed by pItem
    pItem--;    //point to inserted object

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


void lmColStaffObjs::Insert(lmStaffObj* pNewSO, lmStaffObj* pBeforeSO)
{
    //Stores the new StaffObj (pNewSO) in this collection, inserting it BEFORE the 
	//StaffObj pBeforeSO. Insertion DOES NOT move objects to new mesures, so this
	//operation could create irregular measures

	//locate insertion point
	lmItMeasure itM = pBeforeSO->GetItMeasure();
	lmItCSO itCSO = std::find((*itM)->itStartSO, ++((*itM)->itEndSO), pBeforeSO);

	//initialize time counters:
	float rTime = (*itCSO)->GetTimePos();
	float rMaxTime = (*((*itM)->itEndSO))->GetTimePos();
	int nNumMeasure = (*itM)->nNumMeasure;
	
    //assign the time position of StattObj pointed by itCSO
    //assign time to the StaffObj and increment time counters
	AssignTime(pNewSO, &rTime, &rMaxTime);
    
    //store the lmStaffObj in the collection
    if (pNewSO->GetClass() == eSFOT_Control && ((lmSOControl*)pNewSO)->GetCtrolType() == lmEND_OF_STAFF)
    {
        //it is the EOS object. Add it and finish
        m_cStaffobjs.push_back(pNewSO);    //insert at the end
        return;
    }

    //insert before the specified object
    m_cStaffobjs.insert(itCSO, pNewSO);		//insert before item pointed by itCSO
    lmItCSO itNewCSO = itCSO;				//iterator to point to the new inserted object
	itNewCSO--;								

	//if this StaffObj is a barline it is necessary to update the end of current measure
	//and to create a new one
    if (pNewSO->GetClass() == eSFOT_Barline)
    {
		InsertMeasure(itM, (*itM)->itStartSO, itNewCSO);
		(*itM)->itStartSO = itCSO;
    }

    //if this lmStaffObj is the first one of a measure, store a pointer to the node in the
    //measures table
  //  if (m_fStartMeasure) 
  //  {
  //      m_fStartMeasure = false;

		//lmMeasureData* pData = new lmMeasureData;
		//pData->itStartSO = pItem;
		//pData->itEndSO = pItem;
		//pData->nNumMeasure = ++m_nNumMeasure;
		//m_aMeasureData.push_back(pData);

		//m_aMeasures.push_back( --m_aMeasureData.end() );
		//wxASSERT(m_nNumMeasure == (int)m_aMeasures.size());
  //  }
    
    //store, inside the lmStaffObj, a ptr to the measure data
	pNewSO->SetItMeasure( m_aMeasures[m_aMeasures.size()-1] );

    ////Finally, if this lmStaffObj is a barline, signal that a new measure must be started
    ////for the next lmStaffObj and reset time counters
    //if (pNewSO->GetClass() == eSFOT_Barline)
    //{
    //    m_fStartMeasure = true;
    //    rTime = 0;            //reset time counters
    //    rMaxTime = 0;
    //}

	RepositionObjects(itCSO, &rTime, &rMaxTime);
	DumpStaffObjs();
	DumpMeasuresData();
	DumpMeasures();
    
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
	//Inserts a new measure before measure pointed by itMBefore

	//create the measure data item
	int nNumMeasure = (*itMBefore)->nNumMeasure;
	lmMeasureData* pData = new lmMeasureData;
	pData->itStartSO = itStartSO;
	pData->itEndSO = itEndSO;
	pData->nNumMeasure = nNumMeasure;
	m_aMeasureData.insert(itMBefore, pData);

	//update measures table
	m_aMeasures.push_back( m_aMeasureData.end() );
	for (int i=nNumMeasure-1; i < (int)m_aMeasures.size()-1; i++)
		m_aMeasures[i+1] = m_aMeasures[i];
	m_aMeasures[nNumMeasure-1] = --itMBefore;		//new inserted item

	//update measure numbers in m_aMeasureData
	for (lmItMeasure itM = itMBefore; itM != m_aMeasureData.end(); itM++)
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



//====================================================================================================
//Debug methods
//====================================================================================================

void lmColStaffObjs::DumpStaffObjs()
{
	lmItCSO itSO;
	for (itSO = m_cStaffobjs.begin(); itSO != m_cStaffobjs.end(); itSO++)
	{
		wxLogMessage((*itSO)->Dump());
	}
}

void lmColStaffObjs::DumpMeasuresData()
{
	lmItMeasure itM;
	for (itM = m_aMeasureData.begin(); itM != m_aMeasureData.end(); itM++)
	{
		wxLogMessage(_T("MesureData: measure %d"), (*itM)->nNumMeasure);
    }
}

void lmColStaffObjs::DumpMeasures()
{
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
