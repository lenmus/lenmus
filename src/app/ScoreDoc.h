//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#include "wx/docview.h"
#include "wx/cmdproc.h"
#include "../score/Score.h"

//options for method UpdateAllViews
#define lmSCORE_MODIFIED        true
#define lmSCORE_NOT_MODIFIED    false

//------------------------------------------------------------------------------------
// lmUpdateHint helper class: data to optimize window re-paintings 
//------------------------------------------------------------------------------------

//options to control renderization
enum
{
    lmREDRAW                        = 0x0001,   //just re-draw the affected pages
    lmFORCE_RELAYOUT                = 0x0002,   //force an score re-layout
    lmNO_BITMAPS                    = 0x0004,   //do not re-use offscreen bitmaps
    lmNO_RELAYOUT_ON_PAPER_RESIZE   = 0x0008,   //do not re-layout if paper re-size
    lmDELAY_RELAYOUT                = 0x0010,   //delay relayout to next repaint
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
// lmScoreDocument
//------------------------------------------------------------------------------------

class lmScoreDocument: public wxDocument
{
  DECLARE_DYNAMIC_CLASS(lmScoreDocument)

public:
    lmScoreDocument();
    ~lmScoreDocument();

	//overrides for virtual methods in wxDocument
	void UpdateAllViews(wxView* sender=(wxView*)NULL, wxObject* hint=(wxObject*)NULL);
    bool OnOpenDocument(const wxString& filename);
    bool OnNewDocument();
	wxOutputStream& SaveObject(wxOutputStream& stream);

    //specific methods

    void UpdateAllViews(bool fScoreModified, lmUpdateHint* pHints);
    bool OnImportDocument(const wxString& filename);
    bool OnNewScoreWithWizard();
    inline lmScore* GetScore() {return m_pScore; };

private:
    // data to store
    lmScore*            m_pScore;		//the score this document represents

};


#endif    // __LM_SCORECDOC_H__
