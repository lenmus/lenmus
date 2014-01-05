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

#ifndef __LENMUS_IMAGES_CREATOR_H__        //to avoid nested includes
#define __LENMUS_IMAGES_CREATOR_H__


//wxWidgets
#include "wx/wxprec.h"
#include <wx/image.h>

//lomse
#include <lomse_doorway.h>
#include <lomse_document.h>
#include <lomse_graphic_view.h>
#include <lomse_interactor.h>
#include <lomse_presenter.h>
#include <lomse_events.h>
using namespace lomse;


namespace lenmus
{

//---------------------------------------------------------------------------------------
class ImagesCreator
{
protected:
    LomseDoorway&   m_lomse;        //the Lomse library doorway
    Presenter*      m_pPresenter;
    Document*       m_pDoc;

    //the Lomse View renders its content on a bitmap. To manage it, Lomse
    //associates the bitmap to a RenderingBuffer object.
    //It is your responsibility to render the bitmap on a window.
    //Here you define the rendering buffer and its associated bitmap to be
    //used by the previously defined View.
    RenderingBuffer     m_rbuf_window;
    wxImage*            m_buffer;		//the image to serve as buffer
	unsigned char*      m_pdata;		//ptr to the bitmap
    int                 m_nBufWidth, m_nBufHeight;	//size of the bitmap

public:
    ImagesCreator(LomseDoorway& lomse);
    virtual ~ImagesCreator();

    void create_image(const string& src, int format, wxImage* buffer, double scale=1.0);
    void create_image_for_document(wxImage* buffer, double scale=1.0);
    Document* get_empty_document();

protected:
    void attach_rendering_buffer(wxImage* buffer);

};


}   //namespace lenmus

#endif //__LENMUS_IMAGES_CREATOR_H__
