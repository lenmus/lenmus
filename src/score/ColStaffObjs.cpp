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
// lmVoiceLine implementation
//====================================================================================================

lmVoiceLine::lmVoiceLine(lmVStaff* pOwner)
{
    m_pOwner = pOwner;

	//initializations
    m_fStartMeasure = true;
    m_rTime = 0.0f;                   
    m_rMaxTime = 0.0f;
}

lmVoiceLine::~lmVoiceLine()
{
	//AWARE: For now it is not necessary to delete StaffObjs as they are deleted 
	//when deleting m_cStaffobjs

	//lmItCSO itSO;
	//for (itSO = m_StaffObjs.begin(); itSO != m_StaffObjs.end(); itSO++)
	//{
	//	delete *itSO;
	//}
    m_StaffObjs.clear();

}

void lmVoiceLine::AddToVoice(lmStaffObj* pSO)
{
	//assign timepos
	AssignTime(pSO, &m_rTime, &m_rMaxTime);

	//insert after object pointed by cursor
	m_StaffObjs.push_back(pSO);
}

void lmVoiceLine::AssignTime(lmStaffObj* pSO, float* pTime, float* pMaxTime)
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

wxString lmVoiceLine::Dump()
{
    wxString sDump = _T("\nVoice StaffObjs:\n");
	lmItCSO itSO;
	for (itSO = m_StaffObjs.begin(); itSO != m_StaffObjs.end(); itSO++)
	{
		sDump += (*itSO)->Dump();
	}
	return sDump;
}




//====================================================================================================
// lmSegmentData implementation
//====================================================================================================

lmSegmentData::lmSegmentData()
{
	for (int iS=0; iS < lmMAX_STAFF; iS++)
		pContext[iS] = (lmContext*)NULL;
}

//====================================================================================================
// lmSegmentsTable implementation
//====================================================================================================

lmSegmentsTable::lmSegmentsTable(lmVStaff* pOwner)
{
    m_pOwner = pOwner;
}

lmSegmentsTable::~lmSegmentsTable()
{
	//delete measures data
	lmItMeasure itM;
	for (itM = m_aSegmentsData.begin(); itM != m_aSegmentsData.end(); itM++)
	{
		delete *itM;
    }
	m_aSegmentsData.clear();

	m_aMeasures.clear();
}

void lmSegmentsTable::AddSegment(lmItCSO itStartSO, lmItCSO itEndSO)
{
	//Adds a new segment afater last segment

	//create the segment data item
	lmSegmentData* pData = new lmSegmentData;
	pData->itStartSO = itStartSO;
	pData->itEndSO = itEndSO;
	pData->nNumMeasure = (int)m_aMeasures.size() + 1;
	m_aSegmentsData.push_back(pData);

	//update measures table
	m_aMeasures.push_back( --m_aSegmentsData.end() );

    //set contexts. As we are adding at the back it is faster to take the last context.
    //TODO: For this, it is necessary to have access to the owner VStaff
    //for (int nStaff=1; nStaff < m_nNumStaves; nStaff++)
    //{
    //    lmItMeasure itMeasure = m_aMeasures.back();
    //    lmStaff* pStaff = pVStaff->GetStaff(nStaff);
    //    (*itMeasure)->pContext[nStaff-1] = pStaff->GetLastContext();
    //}
    UpdateContexts(pData->nNumMeasure);

	wxASSERT(pData->nNumMeasure == (int)m_aMeasures.size());
}

void lmSegmentsTable::InsertSegment(lmItMeasure itMBefore, lmItCSO itStartSO,
								   lmItCSO itEndSO)
{
	//Inserts a new measure before measure pointed by itMBefore. Iterators
	//itStartSO and itEndSO must point to start and end StaffObjs for this
	//new created measure

	//create the measure data item
	int nNumMeasure = (*itMBefore)->nNumMeasure;
	lmSegmentData* pData = new lmSegmentData;
	pData->itStartSO = itStartSO;
	pData->itEndSO = itEndSO;
	pData->nNumMeasure = nNumMeasure;
	m_aSegmentsData.insert(itMBefore, pData);

	//update measures table:
	//1) make room for a new measure by inserting a new element at the end
	m_aMeasures.push_back( m_aSegmentsData.end() );
	//2) move data from element i to element i+1
	for (int i=(int)m_aMeasures.size()-2; i >= nNumMeasure-1; i--)
		m_aMeasures[i+1] = m_aMeasures[i];
	//3)update the inserted element
	m_aMeasures[nNumMeasure-1] = --itMBefore;		//new inserted item

	//update measure numbers in m_aSegmentsData
	for (lmItMeasure itM = ++itMBefore; itM != m_aSegmentsData.end(); itM++)
	{
		(*itM)->nNumMeasure++;
    }

    //set contexts 
    UpdateContexts(nNumMeasure);

}

void lmSegmentsTable::RemoveSegment(lmItMeasure itMNext, int nMeasure)
{
	// 6. From current MeasureData (NextSO) to end of MeasureData collection
	//    MeasureData[i].numMeasure --
	lmItMeasure itM = itMNext; itM++;
	while (itM != m_aSegmentsData.end())
	{
		(*itM)->nNumMeasure--;
		itM++;
	}

	// 7. Delete MeasureData[NextSO]
	m_aSegmentsData.erase( itMNext );

	// 8. From Measure nMeasure+1 to end of vector aMeasures - 1:
	//	  element[i] = element[i+1]
	for (int i=nMeasure; i < (int)m_aMeasures.size()-1; i++)
		m_aMeasures[i] = m_aMeasures[i+1];

	// 9. Remove last element of aMeasures
	m_aMeasures.pop_back();
}

lmItCSO lmSegmentsTable::GetFirstStaffObjInMeasure(int nMeasure)
{
    //returns the first lmStaffObj in measure number nMeasure (1..n)

    wxASSERT(nMeasure > 0 && nMeasure <= (int)m_aMeasures.size());
    lmItMeasure pMeasure = m_aMeasures[nMeasure-1];
	return (*pMeasure)->itStartSO;
}


lmItCSO lmSegmentsTable::GetLastStaffObjInMeasure(int nMeasure)
{
    //returns the last lmStaffObj in measure number nMeasure (1..n)

    wxASSERT(nMeasure > 0 && nMeasure <= (int)m_aMeasures.size());
    lmItMeasure pMeasure = m_aMeasures[nMeasure-1];
	return (*pMeasure)->itEndSO;
}


wxString lmSegmentsTable::DumpSegmentsData()
{
    wxString sDump = _T("\nMeasures Data:\n");
	int i=1;
	lmItMeasure itM;
	for (itM = m_aSegmentsData.begin(); itM != m_aSegmentsData.end(); itM++, i++)
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

wxString lmSegmentsTable::DumpMeasures()
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

void lmSegmentsTable::SetContext(int nMeasure, int nStaff, lmContext* pContext) 
{ 
    //store the context applicable to measure nMeasure (1..n) and staff nStaff (1..m)

    wxASSERT(nMeasure > 0 && nMeasure <= (int)m_aMeasures.size());
    lmItMeasure itMeasure = m_aMeasures[nMeasure-1];
	(*itMeasure)->pContext[nStaff-1] = pContext; 
}

lmContext* lmSegmentsTable::GetContext(int nMeasure, int nStaff) 
{
    //return the context applicable to measure nMeasure (1..n) and staff nStaff (1..m)

    wxASSERT(nMeasure > 0 && nMeasure <= (int)m_aMeasures.size());
    lmItMeasure itMeasure = m_aMeasures[nMeasure-1];
	return (*itMeasure)->pContext[nStaff-1]; 
}

void lmSegmentsTable::UpdateContexts(int nNewMeasure)
{
    //When add/inserting a new entry in the measures table it is necessary to get current
    //contexts for that measure and to store them in the new measures table. This method
    //does it. nNewMeasure (1..n) is the number of the measure to update.

    wxASSERT(nNewMeasure > 0 && nNewMeasure <= (int)m_aMeasures.size());
    lmItMeasure itMeasure = m_aMeasures[nNewMeasure-1];

    //first measure never has contexts (as it is the first one, no StaffObjs yet)
    if (nNewMeasure == 1)
    {
        for (int iStaff=0; iStaff < m_pOwner->GetNumStaves(); iStaff++)
	        (*itMeasure)->pContext[iStaff] = (lmContext*)NULL;
        return;
    }


	//AWARE: Following code only executes if measure > 1


    //initialize contexts with contexts at start of previous segment
	lmSegmentData* pPrevSegment = *m_aMeasures[nNewMeasure-2];
    bool fStaffDone[lmMAX_STAFF];
    for (int iS=0; iS < m_pOwner->GetNumStaves(); iS++) 
	{
        fStaffDone[iS] = false;
		(*itMeasure)->pContext[iS] = pPrevSegment->GetContext(iS+1);
	}

    //get last StaffObj of previous measure and start iterator
    lmStaffObj* pLastSO = *(pPrevSegment->itEndSO);
    lmStaffObj* pStartSO = *(pPrevSegment->itStartSO);
	lmStaffObjIterator* pIter = m_pOwner->CreateIteratorTo(pLastSO);
    bool fDone = false;
    while(!fDone && !pIter->StartOfList())
	{
        lmContext* pCT = (lmContext*)NULL;
        lmStaffObj* pSO = pIter->GetCurrent();
        if (pSO->GetClass() == eSFOT_Clef)
        {
            int nStaff = pSO->GetStaffNum();
            if (!fStaffDone[nStaff]-1)
            {
                pCT = ((lmClef*)pSO)->GetContext();
	            (*itMeasure)->pContext[nStaff-1] = pCT;
                fStaffDone[nStaff-1] = true;
            }
        }

        if (!pCT)
        {
		    if (pSO->GetClass() == eSFOT_KeySignature
                || pSO->GetClass() == eSFOT_TimeSignature)
            {
                for (int nStaff=1; nStaff <= m_pOwner->GetNumStaves(); nStaff++)
                {
                    if (!fStaffDone[nStaff-1])
                    {
		                if (pSO->GetClass() == eSFOT_KeySignature)
                            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		                else // eSFOT_TimeSignature)
                            pCT = ((lmTimeSignature*)pSO)->GetContext(nStaff);
	                    (*itMeasure)->pContext[nStaff-1] = pCT;
                        fStaffDone[nStaff-1] = true;
                    }
                }
                break;  //fast finish
            }
        }

        //check if we can finish
		if (pSO == pStartSO)
			break;
		else 
		{
			fDone = true;
			for (int iS=0; iS < m_pOwner->GetNumStaves(); iS++) 
				fDone &= fStaffDone[iS];
		}

        pIter->MovePrev();
    }

    delete pIter;

}



//====================================================================================================
// lmColStaffObjs implementation
//====================================================================================================

lmColStaffObjs::lmColStaffObjs(lmVStaff* pOwner)
    : m_Segments(pOwner)
{
    m_pOwner = pOwner;
    m_fStartMeasure = true;
    m_rTime[0] = 0;                   //reset time counters
    m_rMaxTime[0] = 0;

    for(int i=0; i < lmMAX_STAFF; i++)
        m_nCurVoice[i] = 0;

	m_nNumVoices = 0;

	//AWARE: lmColStaffObjs constructor is invoked from lmVStaff constructor, before the
	//lmVStaff is ready. Therefore, you can not invoke lmVStaff methods from here
}

lmColStaffObjs::~lmColStaffObjs()
{
	lmItCSO itSO;
	for (itSO = m_cStaffobjs.begin(); itSO != m_cStaffobjs.end(); itSO++)
	{
		delete *itSO;
	}
    m_cStaffobjs.clear();

	//delete voices
    for(int iV=0; iV < m_nNumVoices; iV++)
		delete m_VoiceLine[iV];

}

void lmColStaffObjs::StartVoices()
{
    //start a voice for each staff
	m_nNumVoices = m_pOwner->GetNumStaves();
    for(int iS=0; iS < m_nNumVoices; iS++)
    {
        m_nCurVoice[iS] = iS+1;
		lmVoiceLine* pLine = new lmVoiceLine(m_pOwner);
		m_VoiceLine[iS] = pLine;
    }
}

int lmColStaffObjs::DetermineVoiceLine(lmStaffObj* pSO, int nStaff)
{
    //returns the voice line (1..n) for this StaffObj

	if (m_nNumVoices == 0)
		StartVoices();

	if (nStaff == 0)
	{
		if (pSO->GetClass() == eSFOT_NoteRest)
			return ((lmNoteRest*)pSO)->GetVoice();
		else
			return m_nCurVoice[ pSO->GetStaffNum() - 1 ];
	}
	else
		return m_nCurVoice[nStaff - 1];
}


//====================================================================================================
//Methods to add, insert and delete StaffObjs
//====================================================================================================


void lmColStaffObjs::Store(lmStaffObj* pSO)
{
    //Stores the received StaffObj in this collection, inserting it at the end, before EOS
    //object 
    //Controls division into measures and assign time position to the lmStaffObj

	//Determine voice
	int iVoice = DetermineVoiceLine(pSO) - 1;

	//assign time to the StaffObj and increment time counters
	m_VoiceLine[iVoice]->AddToVoice(pSO);
	AssignTime(pSO, &m_rTime[0], &m_rMaxTime[0]);
    
    //store the lmStaffObj in the collection. StaffObjs are stored by time position
	lmItCSO pItem;
    m_cStaffobjs.push_back(pSO);    //insert at the end
	pItem = --m_cStaffobjs.end();     //point to inserted object

    //if this lmStaffObj is the first one of a measure, create the entry in the
    //segments table
    if (m_fStartMeasure) 
    {
        m_fStartMeasure = false;
		//AddMeasure(pItem, pItem);
        m_Segments.AddSegment(pItem, pItem);
    }
    
    //store, inside the lmStaffObj, a ptr to the segment
	pSO->SetItMeasure( m_Segments.back() );

    //Finally, if this lmStaffObj is a barline, signal that a new measure must be started
    //for the next lmStaffObj, reset time counters, and update measure data
    if (pSO->GetClass() == eSFOT_Barline)
    {
        m_fStartMeasure = true;		//a new measure must be started with next StaffObj

		lmItMeasure itM = pSO->GetItMeasure();		//update measure data
		(*itM)->itEndSO = pItem;
		
		for(int i=0; i < 8; i++)
		{	//reset time counters
			m_rTime[i] = 0.0f;				
			m_rMaxTime[i] = 0.0f;
		}
    }
    
}

void lmColStaffObjs::Insert(lmStaffObj* pNewSO, lmStaffObj* pBeforeSO)
{
    //Stores the new StaffObj (pNewSO) in this collection, inserting it BEFORE the 
	//StaffObj pBeforeSO. Insertion DOES NOT move objects to new mesures, so this
	//operation could create irregular measures

	//locate insertion point
	lmItMeasure itBeforeM = pBeforeSO->GetItMeasure();
	lmSegmentData* pMData = *itBeforeM;
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
		//InsertMeasure(itBeforeM, pMData->itStartSO, itNewCSO);
		m_Segments.InsertSegment(itBeforeM, pMData->itStartSO, itNewCSO);
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
	pNewSO->SetItMeasure( m_Segments[nNumMeasure-1]);

	////Debug: force a dump of StaffObjs collection and measures tables
	//lmItMeasure itMNew = pNewSO->GetItMeasure();
	//lmSegmentData* pMDataNew = *itMNew;
	//itFromCSO = pMDataNew->itStartSO;
	//itToCSO = pMDataNew->itEndSO;
	//nNumMeasure = pMDataNew->nNumMeasure;
	//wxLogMessage(_T("[lmColStaffObjs::Insert] (Before) Inserted pSO ID=%d, Measure %d, Start ID=%d, End ID=%d"),
	//	pNewSO->GetID(), nNumMeasure, (*itFromCSO)->GetID(),  (*itToCSO)->GetID() );

	RepositionObjects(itBeforeCSO, &rTime, &rMaxTime);

	////Debug: force a dump of StaffObjs collection and measures tables
	//itMNew = pNewSO->GetItMeasure();
	//pMDataNew = *itMNew;
	//itFromCSO = pMDataNew->itStartSO;
	//itToCSO = pMDataNew->itEndSO;
	//nNumMeasure = pMDataNew->nNumMeasure;
	//wxLogMessage(_T("[lmColStaffObjs::Insert] (After) Inserted pSO ID=%d, Measure %d, Start ID=%d, End ID=%d"),
	//	pNewSO->GetID(), nNumMeasure, (*itFromCSO)->GetID(),  (*itToCSO)->GetID() );
 //   #if defined(__WXDEBUG__)
 //   wxString sDump = _T("");
	//sDump += DumpStaffObjs();
	//sDump += DumpSegmentsData();
	//sDump += DumpMeasures();
 //   g_pLogger->LogTrace(_T("lmColStaffObjs::Insert"), sDump );
 //   #endif

}

void lmColStaffObjs::Delete(lmStaffObj* pCursorSO)
{
	//Delete the StaffObj pointed by pCursorSO

	//get some data
	EStaffObjType nDeletedObjType = pCursorSO->GetClass();
	lmItMeasure itMCursor = pCursorSO->GetItMeasure();
	lmSegmentData* pMDataCursor = *itMCursor;
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
        m_Segments.RemoveSegment(itMNext, nMeasure);

 //   #if defined(__WXDEBUG__)
 //   wxString sDump = _T("");
	//sDump += DumpStaffObjs();
	//sDump += DumpSegmentsData();
	//sDump += DumpMeasures();
 //   g_pLogger->LogTrace(_T("lmColStaffObjs::Delete"), sDump );
 //   #endif
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
    return m_Segments.size();
}

lmItCSO lmColStaffObjs::GetFirstStaffObjInMeasure(int nMeasure)
{
    //returns the first lmStaffObj in measure number nMeasure (1..n)

    return m_Segments.GetFirstStaffObjInMeasure(nMeasure);
}


lmItCSO lmColStaffObjs::GetLastStaffObjInMeasure(int nMeasure)
{
    //returns the last lmStaffObj in measure number nMeasure (1..n)

    return m_Segments.GetLastStaffObjInMeasure(nMeasure);
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
	return DumpVoices();

 //   wxString sDump = _T("\nStaffObjs collection:\n");
	//lmItCSO itSO;
	//for (itSO = m_cStaffobjs.begin(); itSO != m_cStaffobjs.end(); itSO++)
	//{
	//	sDump += (*itSO)->Dump();
	//}
	//return sDump;
}

wxString lmColStaffObjs::DumpSegmentsData()
{
	return m_Segments.DumpSegmentsData();
}

wxString lmColStaffObjs::DumpMeasures()
{
	return m_Segments.DumpMeasures();
}

wxString lmColStaffObjs::DumpVoices()
{
	wxString sDump = wxString::Format(_T("\nThis instrument has %d voices:\n\n"), m_nNumVoices);
    for(int iV=0; iV < m_nNumVoices; iV++)
    {
		sDump += wxString::Format(_T("\nVoices %d:\n"), iV+1);
		sDump += m_VoiceLine[iV]->Dump();
	}
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
    m_rTime[0] += rTimeShift;
    if (m_rTime[0] < 0) { m_rTime[0] = 0; }      //can not jump back before the start of current measure
    //TODO Check that does not go to the next measure
    //TODO Display error message if jump out of current measure boundaries
//    if (m_rTime[0] > m_rMaxTime[0]) { m_rTime[0] = m_rMaxTime[0]   //can not jump out of this bar

}


//====================================================================================================
// lmColStaffObjs: contexts management
//====================================================================================================

void lmColStaffObjs::SetContext(int nMeasure, int nStaff, lmContext* pContext) 
{ 
    //store the context applicable to measure nMeasure (1..n) and staff nStaff (1..m)

    m_Segments.SetContext(nMeasure, nStaff, pContext);
}

lmContext* lmColStaffObjs::GetContext(int nMeasure, int nStaff) 
{
    //return the context applicable to measure nMeasure (1..n) and staff nStaff (1..m)

	return m_Segments.GetContext(nMeasure, nStaff);
}

lmContext* lmColStaffObjs::GetCurrentContext(lmStaffObj* pSO)
{
	// Returns the context that is applicable to the received StaffObj.
	// AWARE: This method does not return a context with updated accidentals; 
    // the returned context is valid only for clef, key signature and time signature.
    // To get applicable accidentals use NewUpdatedContext() instead.

    ////get start of segment context
    //pSO

    int nStaff = pSO->GetStaffNum();
    lmContext* pCT = (lmContext*)NULL;
	lmStaffObjIterator* pIter = CreateIteratorTo(pSO);
    while(!pIter->StartOfList())
	{
        pSO = pIter->GetCurrent();
		if (pSO->GetClass() == eSFOT_Clef && nStaff == pSO->GetStaffNum())
            pCT = ((lmClef*)pSO)->GetContext();
		else if (pSO->GetClass() == eSFOT_KeySignature)
            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		else if (pSO->GetClass() == eSFOT_TimeSignature)
            pCT = ((lmTimeSignature*)pSO)->GetContext(nStaff);

        if (pCT)
            break;
		else
		    pIter->MovePrev();
    }
    delete pIter;

	return pCT;
}

lmContext* lmColStaffObjs::NewUpdatedContext(lmStaffObj* pThisSO)
{
	//returns the applicable context for this StaffObj, updated with all
	//accidentals introduced by previous notes.
	//AWARE: context ownership is transferred to the caller.
	//       The returned context MUST BE deleted by the invoking method.

    int nStaff = pThisSO->GetStaffNum();
	lmSegmentData* pData = *(pThisSO->GetItMeasure());
    lmContext* pCT = pData->GetContext(nStaff);
	lmStaffObj* pEndSO =  *(pData->itStartSO);
	lmStaffObjIterator* pIter = CreateIteratorTo(pThisSO);
    while(!pIter->StartOfList())
	{
        lmStaffObj* pSO = pIter->GetCurrent();
		if (pSO->GetClass() == eSFOT_Clef && nStaff == pSO->GetStaffNum())
            pCT = ((lmClef*)pSO)->GetContext();
		else if (pSO->GetClass() == eSFOT_KeySignature)
            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		else if (pSO->GetClass() == eSFOT_TimeSignature)
            pCT = ((lmTimeSignature*)pSO)->GetContext(nStaff);

        if (pCT || pSO == pEndSO)
            break;
		else
		    pIter->MovePrev();
    }

	if (!pCT) {
		delete pIter;
		return pCT;
	}

	//Here pIter is pointing to the StaffObj pointing to the current context.
	//Now we have to go forward, updating accidentals until we reach pThisSO

	lmContext* pUpdated = new lmContext(pCT);
    while(true)
	{
        lmStaffObj* pSO = pIter->GetCurrent();
		if (pSO == pThisSO) break;

		if (pSO->GetClass() == eSFOT_NoteRest && !((lmNote*)pSO)->IsRest())
		{
			//Note found. Update context
			lmAPitch apPitch = ((lmNote*)pSO)->GetAPitch();
			pUpdated->SetAccidental(apPitch.Step(), apPitch.Accidentals());
		}
		pIter->MoveNext();
	}
    delete pIter;

	return pUpdated;
}

lmContext* lmColStaffObjs::NewUpdatedLastContext(int nStaff)
{
	//returns the last context for staff nStaff, updated with all
	//accidentals introduced by previous notes.
	//AWARE: context ownership is transferred to the caller.
	//       The returned context MUST BE deleted by the invoking method.

    lmContext* pCT = m_pOwner->GetLastContext(nStaff);
	lmSegmentData* pSegment = m_Segments.GetLastSegment();
	lmStaffObj* pEndSO = (pSegment ? *(pSegment->itStartSO) : (lmStaffObj*)NULL);
    lmStaffObjIterator* pIter = CreateIterator(eTR_ByTime);
    pIter->MoveLast();
    while(!pIter->StartOfList())
	{
        lmStaffObj* pSO = pIter->GetCurrent();
		if (pSO->GetClass() == eSFOT_Clef && nStaff == pSO->GetStaffNum())
            pCT = ((lmClef*)pSO)->GetContext();
		else if (pSO->GetClass() == eSFOT_KeySignature)
            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		else if (pSO->GetClass() == eSFOT_TimeSignature)
            pCT = ((lmTimeSignature*)pSO)->GetContext(nStaff);

        if (pCT || (pEndSO && pEndSO == pSO))
            break;
        else
		    pIter->MovePrev();
    }

	if (!pCT) {
		delete pIter;
		return pCT;
	}

	//Here pIter is pointing to the StaffObj pointing to the current context.
	//Now we have to go forward, updating accidentals until we reach end of collection

	lmContext* pUpdated = new lmContext(pCT);
    while(!pIter->EndOfList())
	{
        lmStaffObj* pSO = pIter->GetCurrent();
		if (pSO->GetClass() == eSFOT_NoteRest && !((lmNote*)pSO)->IsRest())
		{
			//Note found. Update context
			lmAPitch apPitch = ((lmNote*)pSO)->GetAPitch();
			pUpdated->SetAccidental(apPitch.Step(), apPitch.Accidentals());
		}
		pIter->MoveNext();
	}
    delete pIter;

	return pUpdated;
}


lmContext* lmColStaffObjs::GetLastContext(int nStaff)
{
	//returns the last context applicable to staff nStaff (1..n)
	wxASSERT(nStaff > 0);
	return m_pOwner->GetStaff(nStaff)->GetLastContext();
}

lmContext* lmColStaffObjs::GetStartOfSegmentContext(int nMeasure, int nStaff)
{
    return m_Segments.GetContext(nMeasure, nStaff);
}
