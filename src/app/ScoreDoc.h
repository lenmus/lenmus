//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street,
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __SCOREDOC_H__
#define __SCOREDOC_H__

#ifdef __GNUG__
#pragma interface "ScoreDoc.cpp"
#endif

#include "wx/docview.h"
#include "wx/cmdproc.h"
#include "../score/Score.h"


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

    bool OnImportDocument(const wxString& filename);
    inline lmScore* GetScore() {return m_pScore; };

    // to manage selected objects
    lmScoreObj* FindSelectableObject(lmUPoint& pt);

private:
    // data to store
    lmScore*            m_pScore;        // the score this document represents
    wxSize            m_paperSize;    // intended paper size for which this score is designed (tenths of mm)

};


#endif    // __SCOREDOC_H__
