//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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

#ifndef __LENMUS_MSG_BOX_H__        //to avoid nested includes
#define __LENMUS_MSG_BOX_H__

//wxWidgets headers
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
class MsgBoxBase : public wxDialog
{
public:
    virtual ~MsgBoxBase();

    //event handlers
    void OnRespButton(wxCommandEvent& event);


protected:
    MsgBoxBase(const wxString& sMessage, const wxString& sTitle);
    void CreateControls();
    void FinishLayout();
    void AddButton(const wxString& sLabel, const wxString& sDescr);

    enum {
        lmMAX_BUTTONS = 10,     //max number of buttons allowed
    };

	wxBoxSizer*         m_pMainSizer;
	wxStaticBitmap*     m_pBitmap;
	wxStaticText*       m_pMessage;
	const wxString&     m_sMessage;
	wxBoxSizer*         m_pButtonsSizer;
    int                 m_nNumButtons;
    wxButton*           m_pButton[lmMAX_BUTTONS];
    wxStaticText*       m_pText[lmMAX_BUTTONS];
    int                 m_nMaxButtonWidth;

    wxDECLARE_EVENT_TABLE();
};

//---------------------------------------------------------------------------------------
class ErrorBox : public MsgBoxBase
{
public:
    ErrorBox(const wxString& sMessage, const wxString& sButtonText);
    ~ErrorBox() {}

protected:

};

//class lmInfoBox : public MsgBoxBase
//{
//public:
//    lmInfoBox();
//    ~lmInfoBox();
//
//protected:
//
//};
//
//class lmYesNoBox : public MsgBoxBase
//{
//public:
//    lmYesNoBox();
//    ~lmYesNoBox();
//
//protected:
//
//};

//---------------------------------------------------------------------------------------
class QuestionBox : public MsgBoxBase
{
public:
    QuestionBox(const wxString& sMessage, int nNumButtons, ...);
    ~QuestionBox() {}

protected:

};


}   // namespace lenmus

#endif    // __LENMUS_MSG_BOX_H__
