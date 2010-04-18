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

#ifndef __LM_PROCESSOR_H__        //to avoid nested includes
#define __LM_PROCESSOR_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Processor.cpp"
#endif

#include <list>

class wxPanel;
class wxBoxSizer;

class lmScore;
class lmStaffObj;
class lmAuxObj;
class lmUrlAuxCtrol;
class lmScoreCommand;

#include "../auxmusic/Chord.h"
#include "../auxmusic/Harmony.h"


//--------------------------------------------------------------------------
// lmScoreProcessor: An abstract class to create score processors
//--------------------------------------------------------------------------
class lmScoreProcessor : public wxEvtHandler
{
	DECLARE_ABSTRACT_CLASS(lmScoreProcessor)

public:
    virtual ~lmScoreProcessor();

    //this class methods
    void DoProcess();
    virtual bool SetTools() = 0;
    virtual void* GetProcessOptions() { return (void*)NULL; }

protected:
    lmScoreProcessor();

    //the processor function
    friend class lmCmdScoreProcessor;
    virtual bool ProcessScore(lmScore* pScore, void* pOpt)=0;

    //tools panel related
    bool CreateToolsPanel(wxString sTitle, wxString sDoLink = wxEmptyString);
    inline wxBoxSizer* GetMainSizer() { return m_pMainSizer; }
    void AddDoLink(wxBoxSizer* pSizer, wxString sDoLink);
    void RealizePanel();


private:
    wxPanel*            m_pToolsPanel;
	wxBoxSizer*         m_pMainSizer;
    lmUrlAuxCtrol*      m_pDoLink;
};

//lmProcessorMngr: Helper class for score processors creation and destruction
//management. It is a singleton.
//Its behaviour is similar to a table of smart pointers with reference counting
//--------------------------------------------------------------------------------
class lmProcessorMngr
{
public:
    ~lmProcessorMngr();

    static lmProcessorMngr* GetInstance();
    static void DeleteInstance();
    lmScoreProcessor* CreateScoreProcessor(wxClassInfo* pScoreProcInfo);
    void IncrementReference(lmScoreProcessor* pProc);
    void DeleteScoreProcessor(lmScoreProcessor* pProc);


protected:
    lmProcessorMngr();

    static lmProcessorMngr*  m_pInstance;    //the only instance of this class

    std::map<lmScoreProcessor*, long>     m_ActiveProcs;
};


//----------------------------------------------------------------------------------------------
// lmTestProcessor: A processor to do tests and prepare examples without affecting main code
//----------------------------------------------------------------------------------------------
#ifdef _LM_DEBUG_

class lmNote;

class lmTestProcessor : public lmScoreProcessor
{
	DECLARE_DYNAMIC_CLASS(lmTestProcessor)

public:
    bool SetTools();

protected:
    //Only Processor Manager can create and destroy instances
    friend class lmProcessorMngr;
    lmTestProcessor();
    ~lmTestProcessor();

    //implementation of virtual methods
    friend class lmCmdScoreProcessor;
    bool ProcessScore(lmScore* pScore, void* pOpt);

    //other
    void DrawArrow(lmNote* pNote1, lmNote* pNote2, wxColour color);

};
#endif

//----------------------------------------------------------------------------------------------
// lmHarmonyProcessor: A processor to check an score for harmony 'errors'
//----------------------------------------------------------------------------------------------
class lmHarmonyProcessor : public lmScoreProcessor
{
	DECLARE_DYNAMIC_CLASS(lmHarmonyProcessor)

public:
    //implementation of virtual pure methods
    bool SetTools();

#ifdef _LM_DEBUG_
    void UnitTests();
    void  TestDisplay(lmScore* pScore, lmStaffObj* cpSO, wxColour colour);
#endif

protected:
    //Only Processor Manager can create and destroy instances
    friend class lmProcessorMngr;
    lmHarmonyProcessor();
    ~lmHarmonyProcessor();

    //implementation of virtual methods
    friend class lmCmdScoreProcessor;
    bool ProcessScore(lmScore* pScore, void* pOpt);

    //other

    wxSize* pBoxSize;
    lmFontInfo tFont;
    ChordInfoBox* pInfoBox;
    wxSize* pErrorBoxSize;
    wxSize* pBigErrorBoxSize;
    ChordInfoBox* pChordErrorBox;
    ChordInfoBox* pBigChordErrorBox;

};

#endif    // __LM_PROCESSOR_H__
