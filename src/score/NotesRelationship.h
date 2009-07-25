//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
//-------------------------------------------------------------------------------------

#ifndef __LM_NOTESRELATIONSHIP_H__        //to avoid nested includes
#define __LM_NOTESRELATIONSHIP_H__

//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma interface ".cpp"
//#endif

#include <list>
#include <algorithm>

class lmNoteRest;
class lmNote;

//instead of using RTTI I define a class type value
enum lmERelationshipClass
{
	lm_eBeamClass = 0,
	lm_eChordClass,
	lm_eTupletClass,
    lm_eTieClass,
};

template <class T>      // T is either lmNote or lmNoteRest
class lmRelationship
{
public:
	virtual ~lmRelationship() {}

    virtual void Include(T* pNR, int nIndex = -1)=0;
    virtual void Remove(T* pNR)=0;
    virtual T* GetStartNoteRest()=0;
    virtual T* GetEndNoteRest()=0;
	inline lmERelationshipClass GetClass() { return m_nClass; }
	virtual void OnRelationshipModified()=0;

protected:
	lmRelationship(lmERelationshipClass nClass) : m_nClass(nClass) {}

	lmERelationshipClass	m_nClass;			//relationship class

};


//template <class T>      // T is either lmNote or lmNoteRest
//class lmBinaryRelationship : public lmRelationship<T>
//{
//public:
//	virtual ~lmBinaryRelationship();
//
//	//implementation of lmRelationship virtual methods
//    virtual void Remove(T* pNR);
//    inline T* GetStartNoteRest() { return m_pStartNR; }
//    inline T* GetEndNoteRest() { return m_pEndNR; }
//    virtual void OnRelationshipModified() {};
//
//
//protected:
//    lmBinaryRelationship(lmERelationshipClass nClass, T* pStartNR, T* pEndNR);
//
//    T*		m_pStartNR;     //notes/rests related by this object
//    T*		m_pEndNR;
//};


template <class T>      // T is either lmNote or lmNoteRest
class lmMultipleRelationship : public lmRelationship<T>
{
public:
    virtual ~lmMultipleRelationship();

	//implementation of lmRelationship virtual methods
    virtual void Include(T* pNR, int nIndex = -1);
    virtual void Remove(T* pNR);
    inline int NumNotes() { return (int)m_Notes.size(); }
    inline T* GetStartNoteRest() { return m_Notes.front(); }
    inline T* GetEndNoteRest() { return m_Notes.back(); }
    virtual void OnRelationshipModified() {};

        //specific methods

    virtual int GetNoteIndex(T* pNR);

    //access to notes/rests
    T* GetFirstNoteRest();
    T* GetNextNoteRest();
    typename std::list<T*>& GetListOfNoteRests() { return m_Notes; }

	wxString Dump();


protected:
    lmMultipleRelationship(lmERelationshipClass nClass);

    std::list<T*>   m_Notes;        //list of note/rests that form the relation
    typename std::list<T*>::iterator m_it;   //for methods GetFirstNoteRest() and GetNextNoteRest()

};





//--------------------------------------------------------------------------------------------
// lmRelationship implementation
//--------------------------------------------------------------------------------------------



////--------------------------------------------------------------------------------------------
//// lmBinaryRelationship implementation
////--------------------------------------------------------------------------------------------
//
//template <class T>
//lmBinaryRelationship<T>::lmBinaryRelationship(lmERelationshipClass nClass,
//                                              T* pStartNR, T* pEndNR)
//	: lmRelationship<T>(nClass)
//    , m_pStartNR(pStartNR)
//    , m_pEndNR(pEndNR)
//{
//}
//
//template <class T>
//lmBinaryRelationship<T>::~lmBinaryRelationship()
//{
//    //AWARE: notes must not be deleted as they are part of a lmScore
//    //and will be deleted there.
//
//	//inform the notes
//    if (m_pStartNR)
//        m_pStartNR->OnRemovedFromRelationship(this, lmRelationship<T>::GetClass());
//
//    if (m_pEndNR)
//        m_pEndNR->OnRemovedFromRelationship(this, lmRelationship<T>::GetClass());
//}
//
//template <class T>
//void lmBinaryRelationship<T>::Remove(T* pNR)
//{
//    //remove note/rest.
//	//AWARE: This method is always invoked by a NoteRest. Therefore it will
//	//not inform back the NoteRest, as this is unnecessary and causes problems when
//	//deleting the relationship object
//
//
//    if (m_pStartNR == pNR)
//    {
//        m_pStartNR = (lmNote*)NULL;
//        //m_pEndNR->RemoveTie(this);
//        //m_pEndNR = (lmNote*)NULL;
//    }
//    else if (m_pEndNR == pNR)
//    {
//        m_pEndNR = (lmNote*)NULL;
//        //m_pStartNR->RemoveTie(this);
//        //m_pStartNR = (lmNote*)NULL;
//    }
//}




//--------------------------------------------------------------------------------------------
// lmMultipleRelationship implementation
//--------------------------------------------------------------------------------------------

template <class T>
lmMultipleRelationship<T>::lmMultipleRelationship(lmERelationshipClass nClass)
	: lmRelationship<T>(nClass)
{
}

template <class T>
lmMultipleRelationship<T>::~lmMultipleRelationship()
{
    //AWARE: notes must not be deleted when deleting the list, as they are part of a lmScore
    //and will be deleted there.

	//the relationship is going to be removed. Release all notes
    typename std::list<T*>::iterator it;
    for(it=m_Notes.begin(); it != m_Notes.end(); ++it)
	{
        (*it)->OnRemovedFromRelationship(this, lmRelationship<T>::GetClass());
	}
    m_Notes.clear();
}

template <class T>
void lmMultipleRelationship<T>::Include(T* pNR, int nIndex)
{
    // Add a note to the relation. Index is the position that the added note/rest must occupy
	// (0..n). If -1, note/rest will be added at the end.

	//add the note/rest
	if (nIndex == -1 || nIndex == NumNotes())
		m_Notes.push_back(pNR);
	else
	{
		int iN;
		typename std::list<T*>::iterator it;
		for(iN=0, it=m_Notes.begin(); it != m_Notes.end(); ++it, iN++)
		{
			if (iN == nIndex)
			{
				//insert before current item
				m_Notes.insert(it, pNR);
				break;
			}
		}
	}
	//wxLogMessage(Dump());
	pNR->OnIncludedInRelationship(this, lmRelationship<T>::GetClass());
    OnRelationshipModified();
}

template <class T>
wxString lmMultipleRelationship<T>::Dump()
{
	wxString sDump = _T("");
	typename std::list<T*>::iterator it;
	for(it=m_Notes.begin(); it != m_Notes.end(); ++it)
	{
		sDump += wxString::Format(_T("Note id = %d\n"), (*it)->GetID());
	}
	return sDump;
}

template <class T>
int lmMultipleRelationship<T>::GetNoteIndex(T* pNR)
{
	//returns the position in the notes list (0..n)

	wxASSERT(NumNotes() > 0);

	int iN;
    typename std::list<T*>::iterator it;
    for(iN=0, it=m_Notes.begin(); it != m_Notes.end(); ++it, iN++)
	{
		if (pNR == *it) return iN;
	}
    wxASSERT(false);	//note not found
	return 0;			//compiler happy
}

template <class T>
void lmMultipleRelationship<T>::Remove(T* pNR)
{
    //remove note/rest.
	//AWARE: This method is always invoked by a NoteRest. Therefore it will
	//not inform back the NoteRest, as this is unnecessary and causes problems when
	//deleting the relationship object

    wxASSERT(NumNotes() > 0);

    typename std::list<T*>::iterator it;
    it = std::find(m_Notes.begin(), m_Notes.end(), pNR);
    m_Notes.erase(it);
    OnRelationshipModified();
	//pNR->OnRemovedFromRelationship(this, GetClass());
}

template <class T>
T* lmMultipleRelationship<T>::GetFirstNoteRest()
{
    m_it = m_Notes.begin();
    if (m_it == m_Notes.end())
        return (T*)NULL;
    else
        return *m_it;
}

template <class T>
T* lmMultipleRelationship<T>::GetNextNoteRest()
{
    //advance to next one
    ++m_it;
    if (m_it != m_Notes.end())
        return *m_it;

    //no more notes/rests
    return (T*)NULL;
}



#endif    // __LM_NOTESRELATIONSHIP_H__

