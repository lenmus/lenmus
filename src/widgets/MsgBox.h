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

#ifndef __LM_MSGBOX_H__        //to avoid nested includes
#define __LM_MSGBOX_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "MsgBox.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


class lmMsgBoxBase : public wxDialog
{
public:
    virtual ~lmMsgBoxBase();

    //event handlers
    void OnRespButton(wxCommandEvent& event);


protected:
    lmMsgBoxBase(const wxString& sMessage, const wxString& sTitle);
    void CreateControls();
    void AddButton(const wxString& sLabel, const wxString& sDescr);

	wxStaticBitmap*     m_pBitmap;
	wxStaticText*       m_pMessage;
	const wxString&     m_sMessage;
	wxBoxSizer*         m_pButtonsSizer;
    int                 m_nNumButtons;

    DECLARE_EVENT_TABLE()
};

class lmErrorBox : public lmMsgBoxBase
{
public:
    lmErrorBox(const wxString& sMessage, const wxString& sButtonText);
    ~lmErrorBox() {}

protected:

};

//class lmInfoBox : public lmMsgBoxBase
//{
//public:
//    lmInfoBox();
//    ~lmInfoBox();
//
//protected:
//
//};
//
//class lmYesNoBox : public lmMsgBoxBase
//{
//public:
//    lmYesNoBox();
//    ~lmYesNoBox();
//
//protected:
//
//};

class lmQuestionBox : public lmMsgBoxBase
{
public:
    lmQuestionBox(const wxString& sMessage, int nNumButtons, ...);
    ~lmQuestionBox() {}

protected:

};



#endif    // __LM_MSGBOX_H__
