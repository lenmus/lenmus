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

//lenmus
#include "lenmus_images_creator.h"

//wxWidgets
#include "wx/wxprec.h"
#include <wx/image.h>

//other
#include <iostream>

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


//=======================================================================================
// ImagesCreator implementation
//=======================================================================================
ImagesCreator::ImagesCreator(LomseDoorway& lomse)
	: m_lomse(lomse)
    , m_pPresenter(nullptr)
    , m_pDoc(nullptr)
	, m_buffer(nullptr)
	, m_pdata(nullptr)
    , m_nBufWidth(0)
    , m_nBufHeight(0)
{

}

//---------------------------------------------------------------------------------------
ImagesCreator::~ImagesCreator()
{
    delete m_pPresenter;
}

//---------------------------------------------------------------------------------------
void ImagesCreator::attach_rendering_buffer(wxImage* buffer)
{
    m_buffer = buffer;
    m_nBufWidth = buffer->GetWidth();
    m_nBufHeight = buffer->GetHeight();

    //get pointer to wxImage internal bitmap
    m_pdata = m_buffer->GetData();

    //Attach this bitmap to Lomse rendering buffer
    #define BYTES_PER_PIXEL 3   //wxImage  has RGB, 24 bits format
    int stride = m_nBufWidth * BYTES_PER_PIXEL;     //number of bytes per row
    m_rbuf_window.attach(m_pdata, m_nBufWidth, m_nBufHeight, stride);
}

//---------------------------------------------------------------------------------------
void ImagesCreator::create_image(const string& src, int format,
                                 wxImage* buffer, double scale)
{
    delete m_pPresenter;
    m_pPresenter = m_lomse.new_document(k_view_simple, src, format);
    create_image_for_document(buffer, scale);
}

//---------------------------------------------------------------------------------------
void ImagesCreator::create_image_for_document(wxImage* buffer, double scale)
{
    if (SpInteractor spInteractor = m_pPresenter->get_interactor(0).lock())
    {
        attach_rendering_buffer(buffer);
        spInteractor->set_rendering_buffer(&m_rbuf_window);
        spInteractor->set_scale(scale, 0, 0, k_no_redraw);
        spInteractor->set_operating_mode(Interactor::k_mode_read_only);
//        spInteractor->hide_caret();
        spInteractor->redraw_bitmap();
    }
}

//---------------------------------------------------------------------------------------
Document* ImagesCreator::get_empty_document()
{
    delete m_pPresenter;
    m_pPresenter = m_lomse.new_document(k_view_simple);
    m_pDoc = m_pPresenter->get_document_raw_ptr();
    return m_pDoc;
}


}   //namespace lenmus
