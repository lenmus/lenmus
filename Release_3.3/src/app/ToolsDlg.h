// RCS-ID: $Id: ToolsDlg.h,v 1.3 2006/02/23 19:17:49 cecilios Exp $
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
/*! @file ToolsDlg.h
    @brief Header file for class lmToolsDlg
    @ingroup app_gui
*/
#ifndef __TOOLSDLG_H__        //to avoid nested includes
#define __TOOLSDLG_H__

#include <wx/dialog.h>

class wxBoxSizer;
class wxStaticBitmap;
class wxBitmap;

//class lmToolsDlg:public wxDialog {
//   DECLARE_DYNAMIC_CLASS(lmToolsDlg)
//
// public:
//   lmToolsDlg(wxWindow * parent);
//   virtual ~ lmToolsDlg();
//
//   void OnOK(wxCommandEvent & event);
//
//   wxBoxSizer *topsizer;
//   wxStaticBitmap *icon;
//   wxBitmap *logo;
//
//    DECLARE_EVENT_TABLE()
//};

class lmToolsDlg: public wxFrame
{
public:
    wxPanel *panel;
    lmToolsDlg(wxFrame *frame, wxChar *title, int x, int y );
};

#endif    // __TOOLSDLG_H__
