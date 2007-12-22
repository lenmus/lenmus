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

#ifndef __PRINTOUT_H        //to avoid nested includes
#define __PRINTOUT_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Printout.cpp"
#endif

class lmScoreView;

class lmPrintout: public wxPrintout
{
public:
    lmPrintout(lmScoreView* pView, wxChar *title = _T("My printout"));
    bool OnPrintPage(int page);
    bool HasPage(int page);
    bool OnBeginDocument(int startPage, int endPage);
    void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

protected:
    lmScoreView*    m_pView;        // the view to print/preview
    int            m_nMinPage;        // page range: 1..n
    int            m_nMaxPage;

};




#endif    // __PRINTOUT_H
