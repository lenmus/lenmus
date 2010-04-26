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

#ifndef __LM_LANGCHOICEDLG_H__
#define __LM_LANGCHOICEDLG_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "LangChoiceDlg.cpp"
#endif


class lmLangChoiceDlg: public wxDialog
{
public:
    lmLangChoiceDlg(wxWindow* parent, wxWindowID id, const wxString& title);
    wxString GetLang() { return m_sLang; }

private:
    void OnOk(wxCommandEvent& event);
       
    wxChoice*       m_pChoice;
    wxString        m_sLang;
    int             m_nNumLangs;
    wxArrayString   m_cLangCodes;
    wxArrayString   m_cLangNames;
     
    DECLARE_EVENT_TABLE()
};

#endif // __LM_LANGCHOICEDLG_H__
