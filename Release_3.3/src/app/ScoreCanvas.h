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
/*! @file ScoreCanvas.h
    @brief Header file for class lmScoreCanvas
    @ingroup app_gui
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef _SCORECANVAS_H        //to avoid nested includes
#define _SCORECANVAS_H

#if wxUSE_GENERIC_DRAGIMAGE
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#else
#include "wx/dragimag.h"
#endif

#include "wx/docview.h"
#include "global.h"
class lmScoreHighlightEvent;
#include "../sound/SoundEvents.h"


class lmScoreView;

class lmScoreCanvas : public wxWindow    
{
public:

    // constructors and destructor    
    lmScoreCanvas(lmScoreView *v, wxWindow *parent, const wxPoint& pos,
                  const wxSize& size, long style, wxColor colorBg);
    ~lmScoreCanvas();

    // event handlers
    //virtual void OnDraw(wxDC& dc);
    void OnPaint(wxPaintEvent &WXUNUSED(event));
    void OnMouseEvent(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnVisualHighlight(lmScoreHighlightEvent& event);


private:

    // Tile the bitmap
    bool TileBitmap(const wxRect& rect, wxDC& dc, wxBitmap& bitmap);


private:
    lmScoreView*    m_pView;        // owner view
    wxWindow*       m_pOwner;       // parent window
    wxColour        m_colorBg;      // colour for background

    DECLARE_EVENT_TABLE()
};

// Dragging states
#define DRAG_NONE     0
#define DRAG_START    1
#define DRAG_DRAGGING 2



#endif  // _SCORECANVAS_H
