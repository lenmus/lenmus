//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_ARTPROVIDER_H__        //to avoid nested includes
#define __LM_ARTPROVIDER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ArtProvider.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/artprov.h"

// resources are identified by an wxArtId. This is just a string. To assign LenMus
// identifiers and not to collide with wxWidgets identifiers, mine will be
// just a number prefixed with characters "lm"

enum {                // ids for bitmaps, icons, etc.
    Tool_new = 0,
    Tool_open,
    Tool_save,
    Tool_copy,
    Tool_cut,
    Tool_paste,
    Tool_print,
    Tool_help,
    Tool_open_book,
    Tool_metronome,
    Tool_Max    //It must be the last one and it is used to compute size
};


class lmArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size);

};


#endif    // __LM_ARTPROVIDER_H__
