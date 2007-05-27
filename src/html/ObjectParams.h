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

#ifdef __GNUG__
#pragma interface "ObjectParams.cpp"
#endif

#ifndef __OBJECTPARAMS_H__        //to avoid nested includes
#define __OBJECTPARAMS_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/html/htmltag.h"
//#include "wx/html/winpars.h"

class wxHtmlWinParser;

class lmObjectParams
{
public:
    lmObjectParams(const wxHtmlTag& tag, int nWidth, int nHeight, int nPercent);
    virtual ~lmObjectParams() {}

    virtual void AddParam(const wxHtmlTag& tag)=0;
    virtual void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:
    void LogError(const wxString& sMsg);

    // html object window attributes
    int                m_nWidth;
    int                m_nHeight;
    int                m_nPercent;
};


#endif  // __OBJECTPARAMS_H__

