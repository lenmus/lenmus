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

#include "../auxmusic/ChordManager.h"
#include "../auxmusic/Harmony.h"

//--------------------------------------------------------------------------
// lmScoreProcessor: An abstract class to create score processors
//--------------------------------------------------------------------------
class lmScoreProcessor : public wxEvtHandler
{
	DECLARE_ABSTRACT_CLASS(lmScoreProcessor)

public:
    virtual ~lmScoreProcessor();

    virtual bool ProcessScore(lmScore* pScore) = 0;
    virtual bool UndoChanges(lmScore* pScore) = 0;
    virtual bool SetTools() = 0;


protected:
    lmScoreProcessor();
    void DoProcess();
    void UndoProcess();

    //tools panel related
    bool CreateToolsPanel(wxString sTitle, wxString sDoLink = wxEmptyString,
                          wxString sUndoLink = wxEmptyString);
    inline wxBoxSizer* GetMainSizer() { return m_pMainSizer; }
    void AddStandardLinks(wxBoxSizer* pSizer, wxString sDoLink, wxString sUndoLink);
    void RealizePanel();


private:
    wxPanel*            m_pToolsPanel;
	wxBoxSizer*         m_pMainSizer;
    lmUrlAuxCtrol*      m_pDoLink;
    lmUrlAuxCtrol*      m_pUndoLink;
};


//----------------------------------------------------------------------------------------------
// lmTestProcessor: A processor to do tests and prepare examples without affecting main code
//----------------------------------------------------------------------------------------------
class lmNote;

class lmTestProcessor : public lmScoreProcessor
{
	DECLARE_DYNAMIC_CLASS(lmTestProcessor)

public:
    lmTestProcessor();
    ~lmTestProcessor();

    //implementation of virtual methods
    bool ProcessScore(lmScore* pScore);
    bool UndoChanges(lmScore* pScore);
    bool SetTools();

protected:
    void DrawArrow(lmNote* pNote1, lmNote* pNote2, wxColour color); 

};

//----------------------------------------------------------------------------------------------
// lmHarmonyProcessor: A processor to check an score for harmony 'errors' and add markup to 
// show them
//----------------------------------------------------------------------------------------------
class lmHarmonyProcessor : public lmScoreProcessor
{
	DECLARE_DYNAMIC_CLASS(lmHarmonyProcessor)

public:
    lmHarmonyProcessor();
    ~lmHarmonyProcessor();

    //implementation of virtual methods
    bool ProcessScore(lmScore* pScore);
    bool UndoChanges(lmScore* pScore);
    bool SetTools();

    //specific methods
    bool AnalyzeChordsLinks(lmChordDescriptor* pChordDescriptor, int nNCH);
#ifdef __WXDEBUG__
    void UnitTests();
    void  TestDisplay(lmScore* pScore, lmStaffObj* cpSO, wxColour colour);
#endif

protected:

    bool ProccessChord(lmScore* pScore, lmChordDescriptor* ptChordDescriptor
        , int* pNumChords, wxString &sStatusStr);
    void  DisplayChordInfo(lmScore* pScore, lmChordDescriptor*  pChordDsct, wxColour colour
                           , wxString &sText, bool reset=false);

    lmChordDescriptor tChordDescriptor[lmMAX_NUM_CHORDS];
    int nNumChords;

    lmActiveNotes ActiveNotesList;


    //Error markup: the marked staffobj and its markup attachment
    typedef std::pair<lmStaffObj*, lmAuxObj*> lmMarkup;

    //list of added markup objects
    std::list<lmMarkup*> m_markup;   

};

#endif    // __LM_PROCESSOR_H__
