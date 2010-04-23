//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_KEYSIGNATURE_H__        //to avoid nested includes
#define __LM_KEYSIGNATURE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "KeySignature.cpp"
#endif

#include <vector>

class lmGMObject;

//------------------------------------------------------------------------------------------------
// lmKeySignature object
//------------------------------------------------------------------------------------------------

class lmKeySignature: public lmStaffObj
{
public:
    //constructors and destructor

        //constructor for traditional key signatures
    lmKeySignature(int nFifths, bool fMajor, lmVStaff* pVStaff, long nID, bool fVisible=true);
    ~lmKeySignature();

	wxString GetName() const { return _T("key signature"); }

    //other methods
    lmEKeySignatures GetKeyType() { return m_nKeySignature; }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
    void StoreOriginAndShiftShapes(lmLUnits uxShift, int nShapeIdx = 0);

    //debugging
    wxString Dump();

    //source code generation
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

    //renderization
    lmShape* CreateShape(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, lmEClefType nClef,
                         lmStaff* pStaff, wxColour colorC = *wxBLACK);

    //methods for hiding the key in prologs
    void Hide(bool fHide) { m_fHidden = fHide; }

	//context management
    inline void SetContext(int nStaff, lmContext* pContext)
                                { wxASSERT(nStaff > 0); m_pContext[nStaff-1] = pContext; }
    inline lmContext* GetContext(int nStaff) 
                            { wxASSERT(nStaff > 0); return m_pContext[nStaff-1]; }
    void RemoveCreatedContexts();


private:
    lmShape* AddAccidental(bool fSharp, lmPaper* pPaper, lmUPoint uPos,
					       wxColour colorC, lmStaff* pStaff);

    void SetKeySignatureType();

    // member variables
    bool				m_fHidden;          //to hide it in system prolog
    bool				m_fMajor;
    bool				m_fTraditional;     //it's a traditional signature. Encoded by the
											//	redundant enumeration and fifths/mode pair
    lmEKeySignatures	m_nKeySignature;
    int					m_nFifths;
    lmContext*          m_pContext[lmMAX_STAFF];    //ptr to current context for each staff
    lmShape*	        m_pShapes[lmMAX_STAFF];		//a shape for each staff

};

//
// global functions related to KeySignatures
//
extern void lmComputeAccidentals(lmEKeySignatures nKeySignature, int nAccidentals[]);
extern int lmGetRootNoteStep(lmEKeySignatures nKeySignature);
extern bool lmIsMajorKey(lmEKeySignatures nKeySignature);
extern bool lmIsMinorKey(lmEKeySignatures nKeySignature);
extern const wxString& lmGetKeySignatureName(lmEKeySignatures nKeySignature);
extern int KeySignatureToNumFifths(lmEKeySignatures nKeySignature);
extern lmEKeySignatures lmGetRelativeMinorKey(lmEKeySignatures nMajorKey);
extern lmEKeySignatures lmGetRelativeMajorKey(lmEKeySignatures nMinorKey);
extern wxString GetKeyLDPNameFromType(lmEKeySignatures nKeySignature);

#endif    // __LM_KEYSIGNATURE_H__

