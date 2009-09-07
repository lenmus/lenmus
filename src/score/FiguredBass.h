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

class lmDlgProperties;
class lmChord;

//interval quality: how the interval is interpreted
enum lmEIntervalQuality
{
    lm_eIM_NotPresent = 0,  //the interval is not present, that is, it doesn't sound
    lm_eIM_AsImplied,       //the interval is as implied by key signature
    lm_eIM_RaiseHalf,       //the interval is raised by half step, relative to the key signature
    lm_eIM_LowerHalf,       //the interval is lowered by half step, relative to the key signature
    lm_eIM_Natural,         //the interval is natural, regardless of the key signature
    lm_eIM_Diminished,      //the interval is diminished, regardless of the key signature
};

//interval aspect: how the interval is drawn
enum lmEIntervalAspect
{
    lm_eIA_Normal = 0,      //no modifiers. Draw number as it is stored
    lm_eIA_Parenthesis,     //draw interval number enclosed in parenthesis
    lm_eIA_Understood,      //interval number is understood. Do not draw it
};

//info required by lmFiguredBass constructor to define one interval
typedef struct lmFiguredBassInfo_Data
{
    lmEIntervalQuality      nQuality;       //quality modifier
    lmEIntervalAspect       nAspect;        //aspect modifier
    wxString                sSource;        //source string, without parenthesis
    wxString                sPrefix;        //string with all prefix chars.
    wxString                sSuffix;        //string with all suffix chars.
    wxString                sOver;          //string with all overlayed chars
    bool                    fSounds;        //the interval exists and must sound
}
lmFiguredBassInfo;

#define lmFB_MIN_INTV   2
#define lmFB_MAX_INTV   13


// lmFiguredBassData: helper class with information about a figured bass annotation
//-------------------------------------------------------------------------------------------

class lmFiguredBassData
{
public:
    lmFiguredBassData();
    lmFiguredBassData(lmChord* pChord, lmEKeySignatures nKey);
    lmFiguredBassData(wxString& sData);

    void SetIntervalsInfo(lmFiguredBassInfo* pFBInfo);
    void GetIntervalsInfo(lmFiguredBassInfo* pFBInfo);

    //general information
    wxString GetFiguredBassString();
    inline wxString& GetError() { return m_sError; }
    bool IsEquivalent(lmFiguredBassData* pFBD);

    //information about intervals
    lmEIntervalQuality GetQuality(int nIntv);
    inline lmEIntervalAspect GetAspect(int nIntv) { return m_tFBInfo[nIntv].nAspect; }
    inline wxString& GetSource(int nIntv) { return m_tFBInfo[nIntv].sSource; }
    inline wxString& GetPrefix(int nIntv) { return m_tFBInfo[nIntv].sPrefix; }
    inline wxString& GetSuffix(int nIntv) { return m_tFBInfo[nIntv].sSuffix; }
    inline wxString& GetOver(int nIntv) { return m_tFBInfo[nIntv].sOver; }
    bool IntervalSounds(int nIntv);

    //modify data
    inline void SetQuality(int nIntv, lmEIntervalQuality nQuality) 
                    { m_tFBInfo[nIntv].nQuality = nQuality; }
    inline void SetAspect(int nIntv, lmEIntervalAspect nAspect)
                    { m_tFBInfo[nIntv].nAspect = nAspect; }
    inline void SetSource(int nIntv, wxString sSource)
                    { m_tFBInfo[nIntv].sSource = sSource; }
    inline void SetPrefix(int nIntv, wxString sPrefix)
                    { m_tFBInfo[nIntv].sPrefix = sPrefix; }
    inline void SetSuffix(int nIntv, wxString sSuffix)
                    { m_tFBInfo[nIntv].sSuffix = sSuffix; }
    inline void SetOver(int nIntv, wxString sOver)
                    { m_tFBInfo[nIntv].sOver = sOver; }
    inline void SetSounds(int nIntv, bool fValue)
                    { m_tFBInfo[nIntv].fSounds = fValue; }


protected:
    void Initialize();

        //member variables

    wxString            m_sError;       //error msg for constructor from string

public:
    lmFiguredBassInfo   m_tFBInfo[lmFB_MAX_INTV+1]; //i=0..13 --> intervals 2nd..13th. 0&1 not used


};



// lmFiguredBass: an StaffObj to draw a figured bass annotation
//-------------------------------------------------------------------------------------------

class lmFiguredBass : public lmStaffObj, public lmFiguredBassData
{
public:
    lmFiguredBass(lmVStaff* pVStaff, long nID, lmFiguredBassInfo* pFBInfo);
    lmFiguredBass(lmVStaff* pVStaff, long nID, lmChord* pChord, lmEKeySignatures nKey);
    ~lmFiguredBass() {}

    inline lmFiguredBassData* GetFiguredBassData() { return (lmFiguredBassData*)this; }
    void SetFiguredBassData(lmFiguredBassData* pFBData);

    // properties
    inline float GetTimePosIncrement() { return 0; }
	inline wxString GetName() const { return _T("figured bass"); }

    //layout
    bool IsAligned() { return true; }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);
	lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //    debugging
    wxString Dump();
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

	//edit properties
	void OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName = wxEmptyString);


private:
    lmLUnits AddSymbol(lmCompositeShape* pShape, lmPaper* pPaper, wxChar ch, wxFont* pFont,
                       lmUPoint uPos, wxColour colorC);

    bool                m_fStartOfLine;         //start of line (hold chord)
    bool                m_fEndOfLine;           //change of chord
    bool                m_fParenthesis;         //enclose all figured bass in parenthesis

};


//-----------------------------------------------------------------------------------
// global methods related to figured bass
//-----------------------------------------------------------------------------------

#ifdef __WXDEBUG__

    //Access to common figured bass data table
    extern void lmGetFiguredBassInfo(int iString, lmFiguredBassInfo* pFBI);
    extern int lmGetFiguredBassInfoSize();
    extern const wxString& lmGetFiguredBassString(int nString);

    //Unit tests
    extern bool lmFiguredBassUnitTests();

#endif

#endif    // __LM_FIGUREDBASS__H_

