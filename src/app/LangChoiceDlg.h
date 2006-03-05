// RCS-ID: $Id: LangChoiceDlg.h,v 1.3 2006/02/23 19:16:31 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file LangChoiceDlg.h
    @brief Header file for class lmLangChoiceDlg
    @ingroup app_gui
*/
#ifndef __LANGCHOICEDLG_H__
#define __LANGCHOICEDLG_H__

class lmLangChoiceDlg: public wxDialog
{
public:
    lmLangChoiceDlg(wxWindow* parent, wxWindowID id, const wxString& title);
    wxString GetLang() { return m_sLang; }

private:
    void OnOk(wxCommandEvent& event);
       
    wxChoice*        m_pChoice;
    wxString        m_sLang;
    int                m_nNumLangs;
    wxArrayString    m_cLangCodes;
    wxArrayString    m_cLangNames;
     
    DECLARE_EVENT_TABLE()
};

#endif // __LANGCHOICEDLG_H__
