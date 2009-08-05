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

#ifndef __LM_FIGUREDBASS__H_        //to avoid nested includes
#define __LM_FIGUREDBASS__H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "FiguredBass.cpp"
#endif

class lmFiguredBass : public lmStaffObj
{
public:
    lmFiguredBass(lmVStaff* pStaff, long nID, wxString& sFiguredBass);
    ~lmFiguredBass() {}

    // properties
    inline float GetTimePosIncrement() { return 0; }
	inline wxString GetName() const { return _T("figured bass"); }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //    debugging
    wxString Dump();
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

    //specific methods of this object


private:

    enum lmEIntervalModifier
    {
        lm_eIM_NotPresent = 0,  //the interval is not present, that is, it doesn't sound
        lm_eIM_AsImplied,       //the interval is as implied by key signature
        lm_eIM_RaiseHalf,       //the interval is raised by half step, relative to the key signature
        lm_eIM_LowerHalf,       //the interval is lowered by half step, relative to the key signature
        lm_eIM_Natural,         //the interval is natural, regardless of the key signature
        lm_eIM_Diminished,      //the interval is diminished, regardless of the key signature
    };

    lmEIntervalModifier     m_Intervals[12];        //i=0..14 --> intervals 2nd..13th. 0&1 not used
    bool                    m_fStartOfLine;         //start of line (hold chord)
    bool                    m_fEndOfLine;           //change of chord

};



#endif    // __LM_FIGUREDBASS__H_

