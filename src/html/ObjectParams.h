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

#ifndef __LM_OBJECTPARAMS_H__        //to avoid nested includes
#define __LM_OBJECTPARAMS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ObjectParams.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/html/htmltag.h"
class lmEBookCtrolOptions;
class wxHtmlWinParser;

class lmEBookCtrolParams
{
public:
    lmEBookCtrolParams(const wxHtmlTag& tag, int nWidth, int nHeight, int nPercent);
    virtual ~lmEBookCtrolParams() {}

    virtual void AddParam(const wxHtmlTag& tag);
    virtual void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:
    void LogError(const wxString& sMsg);

    // html object window attributes
    int                m_nWidth;
    int                m_nHeight;
    int                m_nPercent;
    
    // other
    lmEBookCtrolOptions* m_pOptions;        // control options object
    wxString            m_sParamErrors;     // error string if param parsing error

};

class lmExerciseParams : public lmEBookCtrolParams
{
public:
    lmExerciseParams(const wxHtmlTag& tag, int nWidth, int nHeight, int nPercent);
    virtual ~lmExerciseParams() {}

    virtual void AddParam(const wxHtmlTag& tag);

protected:

};


#endif  // __LM_OBJECTPARAMS_H__

