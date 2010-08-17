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

#ifndef __LM_NOTEREST_H__        //to avoid nested includes
#define __LM_NOTEREST_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "NoteRest.cpp"
#endif

class lmVStaff;

#include "NotesRelationship.h"

#include "lenmus_internal_model.h"
using namespace lenmus;


#define lmDEFINE_REST        true
#define lmDEFINE_NOTE        false


class lmLyric;

class lmNoteRest: public lmStaffObj
{
public:
    //ctors and dtor
    virtual ~lmNoteRest();

    //implementation of virtual methods of base class lmStaffObj
    virtual bool IsComposite() { return true; }

    //layout
    bool IsAligned() { return true; }

	//methods related to voice
	inline void SetVoice(int nVoice) { m_nVoice = nVoice; }

    // methods related to note/rest positioning information
    lmLUnits GetStaffOffset() const;

    //methods related to associated AuxObjs management
    lmFermata* AddFermata(const lmEPlacement nPlacement, long nID);
    void AddLyric(lmLyric* pLyric);

    //methods related to duration
    inline float GetDuration() const { return m_rDuration; }
    inline float GetTimePosIncrement() { return m_rDuration; }
    inline void SetDuration(float rNewDuration) { m_rDuration = rNewDuration; }
    wxString GetLDPNoteType();

    //methods related to dots
    inline int GetNumDots() { return m_nNumDots; }
    void ChangeDots(int nDots);

    // methods related to beams
    void CreateBeam(bool fBeamed, BeamInfo* pBeamInfo);
	inline void OnIncludedInBeam(lmBeam* pBeam) { m_pBeam = pBeam; }
	//inline void OnRemovedFromBeam() { m_pBeam = (lmBeam*)NULL; }
    inline bool IsBeamed() const { return m_pBeam != (lmBeam*)NULL; }
    inline lmEBeamType GetBeamType(int level) { return m_BeamInfo[level].get_type(); }
    inline void SetBeamType(int level, lmEBeamType type) { m_BeamInfo[level].set_type(type); }
    inline void SetBeamInfo(int level, BeamInfo& BeamInfo)
                    { 
                        m_BeamInfo[level].set_type( BeamInfo.get_type() );
                        m_BeamInfo[level].set_repeat( BeamInfo.get_repeat() );
                    }

	inline lmBeam* GetBeam() { return m_pBeam; }
	inline BeamInfo* GetBeamInfo() { return &m_BeamInfo[0]; }
    lmBeam* IncludeOnBeam(lmEBeamType nBeamType, lmBeam* pBeam=(lmBeam*)NULL);


    //methods related to tuplets
    inline bool IsInTuplet() { return m_pTuplet != (lmTupletBracket*)NULL; }
    inline lmTupletBracket* GetTuplet() { return m_pTuplet; }
	inline void OnIncludedInTuplet(lmTupletBracket* pTuplet) { m_pTuplet = pTuplet; }
	//inline void OnRemovedFromTuplet() { m_pTuplet = (lmTupletBracket*)NULL; }

    //methods related to sound
    void AddMidiEvents(lmSoundManager* pSM, float rMeasureStartTime, int nChannel,
                       int nMeasure);

    //accessors
    inline lmENoteType GetNoteType() const { return m_nNoteType; }
	inline int GetVoice() { return m_nVoice; }

	//source & debug
    virtual wxString Dump();
    virtual wxString SourceLDP(int nIndent, bool fUndoData);
    virtual wxString SourceXML(int nIndent);

	//relationships
	void OnIncludedInRelationship(void* pRel, lmERelationshipClass nRelClass);
	void OnRemovedFromRelationship(void* pRel, lmERelationshipClass nRelClass);
	void OnIncludedInRelationship(lmRelObj* pRel);
	void OnRemovedFromRelationship(lmRelObj* pRel);



protected:
    lmNoteRest(lmVStaff* pVStaff, long nID, bool IsRest, lmENoteType nNoteType,
               float rDuration, int nNumDots, int nStaff, int nVoice, bool fVisible);

    lmLUnits AddDotShape(lmCompositeShape* pCS, lmPaper* pPaper, lmLUnits xPos, lmLUnits yPos, 
                         wxColour colorC);

        
        //
        // member variables
        //

    //bool		m_fIsRest;          //This lmNoteRest is a rest
    lmENoteType	m_nNoteType;        //type of note / rest
	int			m_nVoice;			//voice: 1..lmMAX_VOICE
    
    //duration and time modifiers
    float       m_rDuration;            //duration as defined in MusicXML: duration/divisions
    int         m_nNumDots;             //number of dots: 0..n (3 max?)

    // beaming information: only valid if m_pBeam != NULL
    lmBeam*     m_pBeam;                //if not NULL the note/rest is in this beam
    BeamInfo m_BeamInfo[6];          //beam mode for each level

    //tuplet related variables
    lmTupletBracket*    m_pTuplet;    //ptr to lmTupletBracket if this note/rest is part of a tuplet

    //AuxObjs associated to this note
    AuxObjsList*    m_pNotations;     //list of Notations
    AuxObjsList*    m_pLyrics;        //list of Lyrics

};


// global functions related to notes/rests
extern float NoteTypeToDuration(lmENoteType nNoteType, bool fDotted, bool fDoubleDotted);
extern float NoteTypeToDuration(lmENoteType nNoteType, int nDots);
extern lmEGlyphIndex lmGetGlyphForNoteRest(lmENoteType nNoteType, bool fForNote,
                                           bool fStemDown = false);



#endif    // __LM_NOTEREST_H__
