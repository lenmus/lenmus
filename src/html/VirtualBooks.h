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
/*! @file VirtualBooks.h
    @brief Header file for for virtual books object
    @ingroup html_management
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __VIRTUALBOOKS_H__        //to avoid nested includes
#define __VIRTUALBOOKS_H__

#include "wx/wfstream.h"
#include "wx/mstream.h"

#include "TextBookController.h"

extern void LoadVirtualBooks(lmTextBookController* pBookController);


class lmVirtualBooks : public wxFileSystemHandler
{
public:
    lmVirtualBooks();
    ~lmVirtualBooks();

    wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
    bool CanOpen(const wxString& location);

private:
    void LoadIntroBook();


};


#endif    // __VIRTUALBOOKS_H__
