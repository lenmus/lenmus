//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

#ifndef __LENMUS_ART_PROVIDER_H__        //to avoid nested includes
#define __LENMUS_ART_PROVIDER_H__

#include <wx/wxprec.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/bitmap.h>
#include <wx/image.h>

#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"


namespace lenmus
{

//forward declarations

//---------------------------------------------------------------------------------------
// resources are identified by an wxArtId. This is just a string.
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


//---------------------------------------------------------------------------------------
class ArtProvider : public wxArtProvider
{
protected:
    ApplicationScope& m_appScope;

public:
    ArtProvider(ApplicationScope& appScope);

protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size);
    wxFileName get_filepath(const wxArtID& id, const wxArtClient& client,
                            const wxSize& size);
    wxImage get_image(const wxArtID& id, const wxArtClient& client,
                      const wxSize& size);

};


#endif    // __LENMUS_ART_PROVIDER_H__


}   //namespace lenmus
