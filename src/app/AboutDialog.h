// RCS-ID: $Id: AboutDialog.h,v 1.4 2006/02/28 17:39:55 cecilios Exp $
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
/*! @file AboutDialog.h
    @brief Header file for class lmAboutDialog
    @ingroup app_gui
*/
#ifndef __ABOUTDLGH__        //to avoid nested includes
#define __ABOUTDLGH__

#include <wx/dialog.h>

// Increment this every time you release a new version
#define LM_VERSION_STR _T("3.0 alpha 2")



class lmAboutDialog:public wxDialog {
    DECLARE_DYNAMIC_CLASS(lmAboutDialog)

public:
    lmAboutDialog(wxWindow * parent);
    ~lmAboutDialog();

    void OnOK(wxCommandEvent& WXUNUSED(event));

    DECLARE_EVENT_TABLE()
};

#endif    // __ABOUTDLGH__
