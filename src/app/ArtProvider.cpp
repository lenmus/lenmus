//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ArtProvider.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/filename.h"

#include "ArtProvider.h"

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;

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

/* TODO: Can this be removed? */
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
"  XXX           ",
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


//
// To simplify maintenace, all icons of this application are defined in this class
//

// resources are identified by an wxArtId. This is just a string.
wxBitmap lmArtProvider::CreateBitmap(const wxArtID& id,
                                     const wxArtClient& client,
                                     const wxSize& size)
{
    wxBitmap oBitmap;
    wxImage image;
    wxString sPath = g_pPaths->GetImagePath();
    wxString sFile;

    //set size. 16x16 is the default
    wxString sSize = _T("_16.png");
    if (size.GetHeight() == 24) {
        sSize = _T("_24.png");
    }
    else if (size.GetHeight() == 32) {
        sSize = _T("_32.png");
    }
    else if (size.GetHeight() == 22) {
        sSize = _T("_22.png");
    }
    else if (size.GetHeight() == 48) {
        sSize = _T("_48.png");
    }

    //icon for text book controller
    if ( client == wxART_HELP_BROWSER ) {
        if ( id == wxART_HELP ) {
            return wxBitmap(null_xpm);
        }
    }

    //TextBookController buttons
    if ( id == wxART_HELP_BOOK ) {
        sFile = _T("app_book");
    }
    else if ( id == wxART_HELP_FOLDER ) {
        sFile = _T("app_book");
    }
    else if ( id == wxART_HELP_PAGE ) {
        return wxNullBitmap;
    }
    else if ( id == wxART_ADD_BOOKMARK ) {
        return wxNullBitmap;
    }
    else if ( id == wxART_DEL_BOOKMARK ) {
        return wxNullBitmap;
    }
    else if ( id == wxART_HELP_SIDE_PANEL ) {
        sFile = _T("tool_index_panel");
    }
    else if ( id == wxART_GO_BACK ) {
        sFile = _T("tool_previous");
    }
    else if ( id == wxART_GO_FORWARD ) {
        sFile = _T("tool_next");
    }
    else if ( id == wxART_GO_TO_PARENT ) {
        return wxNullBitmap;
    }
    else if ( id == wxART_GO_UP ) {
        sFile = _T("tool_page_previous");
    }
    else if ( id == wxART_GO_DOWN ) {
        sFile = _T("tool_page_next");
    }
    else if ( id == wxART_FILE_OPEN ) {
        return wxNullBitmap;
    }
    else if ( id == wxART_PRINT ) {
        sFile = _T("tool_print");
    }
    else if ( id == wxART_HELP_SETTINGS ) {
        sFile = _T("tool_font_size");
    }

    //MainFrame toolbar
    else if (id.Left(5) == _T("tool_")) {
        sFile = id;
    }
    else if (id.Left(4) == _T("msg_")) {
        sFile = id;
    }
    else if (id.Left(4) == _T("opt_")) {
        sFile = id;
    }
    else if (id.Left(7) == _T("button_")) {
        sFile = id;
    }
    else if (id.Left(8) == _T("welcome_")) {
        sFile = id;
        sSize = _T(".png");
    }

    //miscelaneous
    else if (id == _T("backgrnd")) {
        sFile = _T("backgrnd");
		sSize = _T(".png");
    }
    else if (id == _T("app_icon")) {
        sFile = _T("app_icon");
		sSize = _T(".png");
    }
    else if (id == _T("app_splash")) {
        sFile = _T("splash");
		sSize = _T(".png");
    }
    else if (id == _T("banner_updater")) {
        sFile = _T("UpdaterBanner");
		sSize = _T(".png");
    }
    else if (id == _T("logo50x67")) {
        sFile = _T("logo50x67");
		sSize = _T(".png");
    }
    else if (id == _T("preview")) {
        sFile = _T("preview");
		sSize = _T(".png");
    }

    // other IDs
    else {
        sFile = id;
    }

    wxFileName oFilename = wxFileName(sPath, sFile + sSize, wxPATH_NATIVE);
    if (image.LoadFile(oFilename.GetFullPath(), wxBITMAP_TYPE_PNG)) {
        oBitmap = wxBitmap(image);
        return oBitmap;
    }
    else {
        /*TODO if file not found we need to return something. Otherwise, for tool bars
            and other objects a crash will be produced
        */
        wxLogMessage(_T("[lmArtProvider::CreateBitmap] File %s not found. Error icon returned"),
                        oFilename.GetFullPath().c_str() );
        oBitmap = wxBitmap(error_16_xpm);
        return oBitmap;
    }

}



