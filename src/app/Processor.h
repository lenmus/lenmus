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

//--------------------------------------------------------------------------
// lmScoreProcessor: An abstract class to create score processors
//--------------------------------------------------------------------------
class lmScoreProcessor : public wxEvtHandler
{
public:
    lmScoreProcessor();
    virtual ~lmScoreProcessor();


    virtual bool ProcessScore(lmScore* pScore) = 0;
    virtual bool UndoChanges(lmScore* pScore) = 0;
    virtual bool SetTools() = 0;


protected:
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


//--------------------------------------------------------------------------
// A processor to check an score for harmony 'errors' and add markup to 
// show them
//--------------------------------------------------------------------------
class lmHarmonyProcessor : public lmScoreProcessor
{
public:
    lmHarmonyProcessor();
    ~lmHarmonyProcessor();

    //implementation of virtual methods
    bool ProcessScore(lmScore* pScore);
    bool UndoChanges(lmScore* pScore);
    bool SetTools();

protected:

    //Error markup: the marked staffobj and its markup attachment
    typedef std::pair<lmStaffObj*, lmAuxObj*> lmMarkup;

    //list of added markup objects
    std::list<lmMarkup*> m_markup;   

};

#endif    // __LM_PROCESSOR_H__
