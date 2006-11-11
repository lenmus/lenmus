// RCS-ID: $Id: HelpController.h,v 1.2 2006/02/23 19:21:06 cecilios Exp $
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
/*! @file HelpController.h
    @brief Header file for class lmHelpController
    @ingroup html_management
*/
#ifndef __HELPCONTROLLER_H__
#define __HELPCONTROLLER_H__

#include "wx/defs.h"

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "HelpController.h"
#endif

#include "wx/html/helpctrl.h"

class lmHelpController : public wxHtmlHelpController
{
public:
    lmHelpController(int style = wxHF_DEFAULT_STYLE) : wxHtmlHelpController(style) {}
    void OnQuit();

};

#endif // __HELPCONTROLLER_H__
