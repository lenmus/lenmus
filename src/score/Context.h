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

#ifndef __LM_CONTEXT_H__        //to avoid nested includes
#define __LM_CONTEXT_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Context.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class lmKeySignature;
class lmClef;
class lmTimeSignature;


class lmContext
{
public:
    lmContext(lmClef* pClef, lmKeySignature* pKey, lmTimeSignature* pTime,
              bool fClefInherited, bool fKeyInherited, bool fTimeInherited);
    lmContext(lmContext* pContext);
    ~lmContext() {}

    void SetAccidental(int i, int nValue) { m_nAccidentals[i] = nValue; }
    int GetAccidentals(int i) { return m_nAccidentals[i]; }
    void CopyAccidentals(lmContext* pContext);

    inline lmClef* GetClef() const { return m_pClef; }
    inline lmKeySignature* GetKey() const { return m_pKey; }
    inline lmTimeSignature* GetTime() const { return m_pTime; }
    lmEClefType GetClefType() const;

	//navigation and list management
	inline lmContext* GetPrev() const { return m_pPrev; }
	inline lmContext* GetNext() const { return m_pNext; }
	inline void SetPrev(lmContext* pPrev) { m_pPrev = pPrev; }
	inline void SetNext(lmContext* pNext) { m_pNext = pNext; }

    //Updating values
    void PropagateValueWhileInherited(lmStaffObj* pSO);
    void PropagateNewWhileInherited(lmStaffObj* pNewSO);
    inline bool IsModified() const { return m_fModified; }
    inline void SetModified(bool fValue) { m_fModified = fValue; }


    //debug
//#if defined(__WXDEBUG__)
    inline int GetContextId() { return m_nId; }
//#endif
    wxString DumpContext(int nIndent = 0);



private:
    void InitializeAccidentals();
    void SetKey(lmKeySignature* pKey);

        // member variables

    //information about current clef, key and time signature
    lmClef*             m_pClef;
    lmKeySignature*     m_pKey;
    lmTimeSignature*    m_pTime;
    bool                m_fClefInherited;
    bool                m_fKeyInherited;
    bool                m_fTimeInherited;

    //the context has been modified. This flag is used to propagate actions
    bool                m_fModified;

    //the next array keeps information about the accidentals applicable to each
    //note. Each element refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
    int     m_nAccidentals[7];

	//Contexts are organized as a double linked list. Here are the links
	lmContext*		m_pNext;		//pointer to next context 
	lmContext*		m_pPrev;		//pointer to previous context 

    //#if defined(__WXDEBUG__)
    int             m_nId;
    //#endif

};



// this defines the type ArrayOfContexts as an array of lmContext pointers
WX_DEFINE_ARRAY(lmContext*, ArrayOfContexts);


#endif  // __LM_CONTEXT_H__
