//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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
//---------------------------------------------------------------------------------------

#ifndef __LM_URLAUXCTROL_H__        //to avoid nested includes
#define __LM_URLAUXCTROL_H__

//lenmus
#include "lenmus_standard_header.h"

//wxWidgest
#include <wx/wxprec.h>
#include <wx/wx.h>

//lomse
#include <lomse_internal_model.h>
#include <lomse_document.h>
#include <lomse_events.h>
using namespace lomse;



namespace lenmus
{

#define lmNO_BITMAP  wxEmptyString

//---------------------------------------------------------------------------------------
class UrlAuxCtrol
{
protected:
	ImoParagraph*   m_pPara;
	Document*       m_pDoc;
	ImoLink*        m_pLink;

    string          m_sNormalLabel;     //Label displayed normally (i.e. "play")
    string          m_sAltLabel;        //Alternative label (i.e. "stop playing")
    wxString        m_sBitmap;          //bitmap file name
    wxString        m_sAltBitmap;       //bitmap to use with alternative label. Empty to use normal bitmap
//	wxStaticBitmap* m_pBitmap;
    wxBitmap        m_oEnaBitmap;
    wxBitmap        m_oDisBitmap;
    wxBitmap        m_oAltEnaBitmap;
    wxBitmap        m_oAltDisBitmap;
    bool            m_fNormal;          //true when normal label

public:
    UrlAuxCtrol(EventHandler* parentCtrol, int eventType, ImoParagraph* pPara,
                Document* pDoc, const string& url, const string& sNormalLabel,
                LUnits linkWidth = -1.0f,
                const wxString& sBitmap = wxEmptyString, const string& sAltLabel = "",
                const wxString& sAltBitmap = wxEmptyString);
    ~UrlAuxCtrol() {}

    void enable(bool fEnable = true);
    void set_normal_label(bool fNormal = true);
    inline void set_alternative_label() { set_normal_label(false); }
    void replace_normal_label(const wxString& sLabel);

};


}   //namespace lenmus

#endif  // __LM_URLAUXCTROL_H__
