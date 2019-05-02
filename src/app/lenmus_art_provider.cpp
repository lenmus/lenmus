//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

#include "lenmus_art_provider.h"

#include "lenmus_injectors.h"
#include "lenmus_paths.h"
#include "lenmus_string.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

// images to use when error in opening a PNG file
static const char *error_16_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 2 1",
"X c #000000",
"  c #FFFFFF",
/* pixels */
"XXXXXXXXXXXXXXXX",
"XX            XX",
"X X          X X",
"X  X        X  X",
"X   X      X   X",
"X    X    X    X",
"X     X  X     X",
"X      XX      X",
"X      XX      X",
"X     X  X     X",
"X    X    X    X",
"X   X      X   X",
"X  X        X  X",
"X X          X X",
"XX            XX",
"XXXXXXXXXXXXXXXX"
};

static const char *null_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 4 1",
"  c None",
". c Black",
"X c #808080",
"o c #000080",
/* pixels */
"                ",
"                ",
"  XXX           ",
"                ",
"  XXX           ",
"                ",
"  XXXXXXX       ",
"                ",
"  XXX           ",
"                ",
"  XXX           ",
"                ",
"  XXX           ",
"                ",
"                ",
"                "
};


//---------------------------------------------------------------------------------------
ArtProvider::ArtProvider(ApplicationScope& appScope)
    : wxArtProvider()
    , m_appScope(appScope)
{
}

//---------------------------------------------------------------------------------------
// resources are identified by an wxArtId. It is just a string.
wxFileName ArtProvider::get_filepath(const wxArtID& id, const wxArtClient& client,
                                     const wxSize& size)
{
    Paths* pPaths = m_appScope.get_paths();
    wxString sPath = pPaths->GetImagePath();
    wxString sFile;

    //set size. 16x16 is the default
    wxString sSize = "_16.png";
    if (size.GetHeight() == size.GetWidth())
    {
        if (size.GetHeight() == 24) {
            sSize = "_24.png";
        }
        else if (size.GetHeight() == 32) {
            sSize = "_32.png";
        }
        else if (size.GetHeight() == 22) {
            sSize = "_22.png";
        }
        else if (size.GetHeight() == 48) {
            sSize = "_48.png";
        }
    }
    else
        sSize = wxString::Format("_%dx%d.png", size.GetWidth(), size.GetHeight());

    //icon for text book controller
    if ( client == wxART_HELP_BROWSER ) {
        if ( id == wxART_HELP ) {
            return wxFileName("null");
        }
    }

    //TextBookController buttons
    if ( id == wxART_ADD_BOOKMARK ) {
        sFile = "tool_bookmark_add";
    }
    else if ( id == wxART_DEL_BOOKMARK ) {
        sFile = "tool_bookmark_remove";
    }
    else if ( id == wxART_ERROR ) {
        sFile = "msg_error";
    }
    else if ( id == wxART_FILE_OPEN ) {
        return wxFileName("null");
    }
    else if ( id == wxART_GO_BACK ) {
        sFile = "tool_previous";
    }
    else if ( id == wxART_GO_FORWARD ) {
        sFile = "tool_next";
    }
    else if ( id == wxART_GO_TO_PARENT ) {
        return wxFileName("null");          //<---
    }
    else if ( id == wxART_GO_UP ) {
        sFile = "tool_page_previous";
    }
    else if ( id == wxART_GO_DOWN ) {
        sFile = "tool_page_next";
    }
    else if ( id == wxART_HELP_BOOK ) {
        sFile = "app_book";
    }
    else if ( id == wxART_HELP_FOLDER ) {
        sFile = "app_book";
    }
    else if ( id == wxART_HELP_PAGE ) {
        return wxFileName("null");
    }
    else if ( id == wxART_HELP_SETTINGS ) {
        sFile = "tool_font_size";
    }
    else if ( id == wxART_HELP_SIDE_PANEL ) {
        sFile = "tool_index_panel";
    }
    else if ( id == wxART_INFORMATION ) {
        sFile = "msg_info";
    }
    else if ( id == wxART_PRINT ) {
        sFile = "tool_print";
    }
    else if ( id == wxART_WARNING ) {
        sFile = "msg_info";
    }

    //MainFrame toolbar
    else if (id.Left(5) == "tool_") {
        sFile = id;
    }
    else if (id.Left(4) == "msg_") {
        sFile = id;
    }
    else if (id.Left(4) == "opt_") {
        sFile = id;
    }
    else if (id.Left(7) == "button_") {
        sFile = id;
    }
    else if (id.Left(8) == "welcome_") {
        sFile = id;
        sSize = ".png";
    }

    //miscelaneous
    else if (id == "backgrnd") {
        sFile = "backgrnd";
		sSize = ".png";
    }
    else if (id == "app_icon") {
        sFile = "app_icon";
		sSize = ".png";
    }
    else if (id == "app_splash") {
        sFile = "splash";
		sSize = ".png";
    }
    else if (id == "banner_updater") {
        sFile = "UpdaterBanner";
		sSize = ".png";
    }
    else if (id == "logo50x67") {
        sFile = "logo50x67";
		sSize = ".png";
    }
    else if (id == "preview") {
        sFile = "preview";
		sSize = ".png";
    }
    else if (id == "right_answers") {
        sFile = "right_answers";
		sSize = "_24.png";
    }
    else if (id == "wrong_answers") {
        sFile = "wrong_answers";
		sSize = "_24.png";
    }
    else if (id == "total_marks") {
        sFile = "total_marks";
		sSize = "_24.png";
    }

    // other IDs
    else
        sFile = id;

    return wxFileName(sPath, sFile + sSize, wxPATH_NATIVE);
}

//---------------------------------------------------------------------------------------
wxBitmap ArtProvider::CreateBitmap(const wxArtID& id,
                                   const wxArtClient& client,
                                   const wxSize& size)
{
    wxImage image = get_image(id, client, size);
    return wxBitmap(image);
}

//---------------------------------------------------------------------------------------
wxImage ArtProvider::get_image(const wxArtID& id, const wxArtClient& client,
                               const wxSize& size)
{
    wxFileName oFilename = get_filepath(id, client, size);
    //LOMSE_LOG_INFO("Art: Filepath='%s'",
    //               to_std_string(oFilename.GetFullPath()).c_str() );

    if (oFilename.GetFullPath() == "null")
    {
        wxBitmap oBitmap(null_xpm);
        return oBitmap.ConvertToImage();
    }

    wxImage image;
    if (image.LoadFile(oFilename.GetFullPath(), wxBITMAP_TYPE_PNG))
        return image;
    else
    {
        // if file not found we need to return something. Otherwise, for tool bars
        // and other objects a crash will be produced
        LOMSE_LOG_ERROR("File %s not found. Error icon returned",
                        to_std_string(oFilename.GetFullPath()).c_str() );
        wxBitmap oBitmap(error_16_xpm);
        return oBitmap.ConvertToImage();
    }
}



}   //namespace lenmus
