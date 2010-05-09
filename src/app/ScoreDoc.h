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

#ifndef __LM_SCORECDOC_H__
#define __LM_SCORECDOC_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ScoreDoc.cpp"
#endif

#include <wx/docview.h>
#include <wx/cmdproc.h>
#include "../score/Score.h"

#if lmUSE_LIBRARY
    using namespace lenmus;
    #include "lenmus_document.h"
#endif

class lmEditorMode;

//------------------------------------------------------------------------------------
// lmUpdateHint helper class: data to optimize window re-paintings
//------------------------------------------------------------------------------------

//options to control renderization
enum
{
    lmHINT_NO_LAYOUT                = 0x0001,   //No relayout. Just re-draw the shapes
    lmHINT_FORCE_RELAYOUT           = 0x0002,   //force an score re-layout
    lmHINT_NO_BITMAPS               = 0x0004,   //do not re-use offscreen bitmaps
    lmHINT_NO_LAYOUT_ON_PAPER_RESIZE = 0x0008,  //do not re-layout if paper re-size
    lmHINT_NEW_SCORE                = 0x0010,   //score replaced in document. Update all
};

class lmUpdateHint: public wxObject
{
public:
    lmUpdateHint(int nOptions=0x0000) { m_nOptions = nOptions; }
    ~lmUpdateHint() {}

    inline int Options() { return m_nOptions; }

protected:
    int         m_nOptions;

};


//------------------------------------------------------------------------------------
// lmDocument
//------------------------------------------------------------------------------------

class lmDocument: public wxDocument
{
  DECLARE_DYNAMIC_CLASS(lmDocument)

public:
#if lmUSE_LIBRARY
    lmDocument(Document* pDoc);
#endif
    lmDocument();
    ~lmDocument();

	//overrides for virtual methods in wxDocument
    bool OnCreate(const wxString& path, long flags);
	void UpdateAllViews(wxView* sender=(wxView*)NULL, wxObject* hint=(wxObject*)NULL);
    bool OnOpenDocument(const wxString& filename);
#if wxUSE_STD_IOSTREAM
    wxSTD ostream& SaveObject(wxSTD ostream& stream);
#else
    wxOutputStream& SaveObject(wxOutputStream& stream);
#endif

    //specific methods

    inline bool IsBeingEdited() { return m_fIsBeingEdited; }
    bool OnImportDocument(const wxString& filename);
    bool OnNewDocumentWithContent(lmScore* pScore);
    void ReplaceScore(lmScore* pScore, bool fUpdateViews = true);
    lmScore* GetScore();

#if lmUSE_LIBRARY
    inline Document* get_document() { return m_pDoc; }
#endif

    //Edit mode
    void OnCustomizeController(lmEditorMode* pMode);
    inline lmEditorMode* GetEditMode() { return m_pEditMode; }

private:
    wxString GetFilenameToSaveUnitTest();

    // data to store
    lmScore*            m_pScore;		//the score this document represents
    lmEditorMode*       m_pEditMode;
    bool                m_fIsBeingEdited;   //the document is being edited

#if lmUSE_LIBRARY
    Document*           m_pDoc;
#endif
};


#endif    // __LM_SCORECDOC_H__
