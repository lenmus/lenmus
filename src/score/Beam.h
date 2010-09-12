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

#ifndef __LM_BEAM_H__        //to avoid nested includes
#define __LM_BEAM_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Beam.cpp"
#endif

#include <list>

#include "lenmus_internal_model.h"
using namespace lenmus;


class lmShapeBeam;
class lmShapeStem;
class lmShapeNote;


//========================================================================================
//helper class to keep info about a beam. For building a beam
//========================================================================================
class lmBeamInfo
{
public:
    lmBeamInfo() : fEndOfBeam(false)
                 , nBeamID(lmNEW_ID)
                 , nBeamNum(0L)
                 , pNR((lmNoteRest*)NULL)
            {
                for (int i=0; i < 6; i++)
                    nBeamType[i] = ImoBeam::k_none;
            }
    ~lmBeamInfo() {}

    bool            fEndOfBeam;
    long            nBeamID;
    long            nBeamNum;
    lmEBeamType     nBeamType[6];
    lmNoteRest*     pNR;
};



//========================================================================================

class lmBeam : public lmMultiRelObj
{
public:
    lmBeam(lmNote* pNote, long nID=lmNEW_ID);
    ~lmBeam();

	//implementation of lmAuxObj virtual methods
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);

	//implementation of lmRelObj/lmMultiRelObj virtual methods
	void OnRelationshipModified();

    //source code related methods. Implementation of virtual pure in lmRelObj
    wxString SourceLDP_First(int nIndent, bool fUndoData, lmNoteRest* pNR);
    wxString SourceLDP_Middle(int nIndent, bool fUndoData, lmNoteRest* pNR);
    wxString SourceLDP_Last(int nIndent, bool fUndoData, lmNoteRest* pNR);
    wxString SourceXML_First(int nIndent, lmNoteRest* pNR);
    wxString SourceXML_Middle(int nIndent, lmNoteRest* pNR);
    wxString SourceXML_Last(int nIndent, lmNoteRest* pNR);

    //implementation of lmAuxObj virtual methods

	//specific methods
    void CreateShape();
	void AddNoteAndStem(lmShapeStem* pStem, lmShapeNote* pNote, ImoBeamInfo* pBeamInfo);
    void AddRestShape(lmShape* pRestShape);
    void AutoSetUp();
    void NeedsSetUp(bool fValue) { m_fNeedsSetUp = fValue; }
    bool NeedsSetUp() { return m_fNeedsSetUp; }

private:
    int GetBeamingLevel(lmNote* pNote);
    wxString SourceLDP(int nIndent, bool fUndoData, lmNoteRest* pNR);
    wxString SourceXML(int nIndent, lmNoteRest* pNR);

        //member variables

    bool            m_fStemsDown;
    bool            m_fNeedsSetUp;
	lmShapeBeam*	m_pBeamShape;

    //beam information to be transferred to each beamed note
    int            m_nPosForRests;        //relative position for rests

};

//global functions related to beams
extern wxString& GetLDPBeamNameFromType(lmEBeamType nType);
extern wxString& GetXMLBeamNameFromType(lmEBeamType nType);


#endif    // __LM_BEAM_H__

