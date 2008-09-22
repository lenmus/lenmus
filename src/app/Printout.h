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

#ifndef __LM_PRINTOUT_H        //to avoid nested includes
#define __LM_PRINTOUT_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Printout.cpp"
#endif

class lmScoreView;

class lmPrintout: public wxPrintout
{
public:
    lmPrintout(lmScoreView* pView, wxString title = _T("My printout"));
    bool OnPrintPage(int page);
    bool HasPage(int page);
    bool OnBeginDocument(int startPage, int endPage);
    void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

protected:
    lmScoreView*    m_pView;        // the view to print/preview
    int            m_nMinPage;        // page range: 1..n
    int            m_nMaxPage;

};




#endif    // __LM_PRINTOUT_H
