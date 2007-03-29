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

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "wx/debug.h"


//====================================================================================================
//Constructors and destructors
//====================================================================================================
lmColStaffObjs::lmColStaffObjs()
{
    m_fStartMeasure = true;
    m_rTimePos = 0;                   //reset time counters
    m_rMaxTimePos = 0;
}

lmColStaffObjs::~lmColStaffObjs()
{
    //wxLogMessage(wxString::Format(
    //    _T("[lmColStaffObjs::~lmColStaffObjs] Deleting lmColStaffObjs: num objs = %d"), GetNumStaffObjs() ) );
    m_cStaffobjs.DeleteContents(true);
    m_cStaffobjs.Clear();

    m_aStartMeasure.Clear();
    //wxLogMessage(wxString::Format(
    //    _T("[lmColStaffObjs::~lmColStaffObjs] After deleting lmColStaffObjs: num objs = %d"), GetNumStaffObjs() ) );

}


//====================================================================================================
//Funciones de creación, inserción y borrado
//====================================================================================================


void lmColStaffObjs::Store(lmStaffObj *pSO)
{
    /*
    Stores a lmStaffObj in this StaffObjs collection.
    Controls division into bars and assign time position to the lmStaffObj
    
    Input parameters:
       pSO: pointer to the lmStaffObj to store

    */

    //store time position
    pSO->SetTimePos(m_rTimePos);
    
    //increment time counters
    if (pSO->GetClass() == eSFOT_NoteRest) {
        //only NoteRests increment time counters
        lmNoteRest* pNR = (lmNoteRest*)pSO;
        if (pNR->IsRest()) {
            m_rTimePos += pNR->GetDuration();
        } else {
            //It is a note. Verify if it is part of a chord
            lmNote* pNote = (lmNote*)pSO;
            if (!pNote->IsInChord()) {
                //@attention time counter is going to be incremented also for chord base notes,
                //what is right.
                //As we are in the process of building the score object from the source code,
                //chord base notes have not yet being marked as "InChord". This is because
                //the chord is created when the next note is processed.
                m_rTimePos += pNote->GetDuration();
            } else {
                //for (the same reason, this point is reached only by notes that are in a chord
                //(but not the base note).
                //Assing to this note the time position of the note base. Do not increment
                //time counter as it has been already incremented when storing the base note.
                lmNote* pNoteBase = (pNote->GetChord())->GetBaseNote();
                pSO->SetTimePos(pNoteBase->GetTimePos());
            }
        }
        
    } else {
        //it is not a lmNoteRest. Increment time counter in the amount coded in the lmStaffObj
        m_rTimePos += pSO->GetTimePosIncrement();
    }
    
    //Time counter update. Update now the maximun value reached.
    m_rMaxTimePos = wxMax(m_rMaxTimePos, m_rTimePos);
    
    //if (the lmStaffObj being stored is a barline it is necessary to ensure that
    //its time position match up the measure duration, as there could have been back and forward
    //displacements.
    if (pSO->GetClass() == eSFOT_Barline) {
        //the object is a barline. Assing to it the maximum time position
        pSO->SetTimePos(m_rMaxTimePos);
    }
    
    //store the lmStaffObj in the collection. StaffObjs are stored by arrival order
    wxStaffObjsListNode* pNode = m_cStaffobjs.Append(pSO);
    
    //if this lmStaffObj is the first one of a measure, store a pointer to the node in the
    //measures table
    if (m_fStartMeasure) 
    {
        m_fStartMeasure = false;
        m_aStartMeasure.Add(pNode);
    }
    
    //store, inside the lmStaffObj, the measure number in which the lmStaffObj is included
    pSO->SetNumMeasure((int)m_aStartMeasure.GetCount() );

    //Finally, if this lmStaffObj is a barline, signal that a new measure must be started
    //for the next lmStaffObj and reset time counters
    if (pSO->GetClass() == eSFOT_Barline)
    {
        m_fStartMeasure = true;
        m_rTimePos = 0;            //reset time counters
        m_rMaxTimePos = 0;
    }
    
}

////====================================================================================================
////Methods for accesing specific items and information about the collection
////====================================================================================================
//
//lmStaffObj* lmColStaffObjs::GetFirst()
//{
//    m_pNode = m_cStaffobjs.GetFirst();
//    return (m_pNode ? (lmStaffObj *)m_pNode->GetData() : (lmStaffObj *)m_pNode);
//}
//
//lmStaffObj* lmColStaffObjs::GetNext()
//{
//    m_pNode = m_pNode->GetNext();
//    return (m_pNode ? (lmStaffObj *)m_pNode->GetData() : (lmStaffObj *)m_pNode);
//}
//
//
int lmColStaffObjs::GetNumStaffObjs()
{
    //returns the number of StaffObjs in the collection
    return m_cStaffobjs.GetCount();
}

int lmColStaffObjs::GetNumMeasures()
{
    //returns the number of bars in the collection
    return (int)m_aStartMeasure.GetCount();
    
}

wxStaffObjsListNode* lmColStaffObjs::GetFirstInMeasure(int nMeasure)
{
    //returns the first lmStaffObj in measure number nMeasure (1..n)
    //This is not a walking method but an access method as:
    //   - the collection is not accessed
    //   - It does not either instantiate an iterator nor return any index to instantiate an
    //   iterator
    //---------------------------------------------------------------------------------------
    wxASSERT(nMeasure > 0 && nMeasure <= (int)m_aStartMeasure.GetCount());
    wxStaffObjsListNode* pNode = m_aStartMeasure.Item(nMeasure-1);
    wxASSERT(pNode);
    return pNode;
    
}

wxStaffObjsListNode* lmColStaffObjs::GetFirst()
{
    return m_cStaffobjs.GetFirst();
}

wxStaffObjsListNode* lmColStaffObjs::GetLast()
{
    return m_cStaffobjs.GetLast();
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
    /*
    Shifts time counters, forward or backwards
    */

    //update time counters and check that shift is inside current measure
    m_rTimePos += rTimeShift;
    if (m_rTimePos < 0) { m_rTimePos = 0; }      //can not jump back before the start of current measure
    //! @todo Check that does not go to the next measure
    //! @todo Display error message if jump out of current measure boundaries
//    if (m_rTimePos > m_rMaxTimePos) { m_rTimePos = m_rMaxTimePos   //can not jump out of this bar

}
