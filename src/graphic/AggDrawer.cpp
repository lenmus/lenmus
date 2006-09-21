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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Drawer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "AggDrawer.h"

// Coordinates
//
//    All lmDrawer methods expect coordinates in world units (microns). But all
//    AGG methods expect coordinates in device units (pixels).
//    Anti-aliased methods expects either doubles or integers in format (24,8).
//    But renderer_marker expects no decimals, so values must be multiplied by 256.
//    So lmAggDrawer methods must do the units conversion.
//

// I use an wxImage as buffer. wxImage is platform independent
// and its buffer is an array of characters
// in RGBRGBRGB... format in the top-to-bottom, left-to-right order, that 
// is the first RGB triplet corresponds to the pixel first pixel of the first
// row, the second one --- to the second pixel of the first row and so on until 
// the end of the first row, with second row following after it and so on.
#define BYTES_PP 3      // Bytes per pixel


lmAggDrawer::lmAggDrawer(wxDC* pDC, int widthPixels, int heightPixels, int stride) :
    lmDrawer(pDC)
    //, m_feng()
    //, m_fman(m_feng)
{
    //wxLogMessage(_T("[lmAggDrawer::lmAggDrawer]"));
    wxASSERT(widthPixels != 0 && heightPixels != 0);
    Initialize();

    // allocate a rendering buffer
    if (stride==0) 
        m_nStride = widthPixels * BYTES_PP;
    else
        m_nStride = stride * BYTES_PP;

    m_nBufWidth = widthPixels;
    m_nBufHeight = heightPixels;
    m_buffer = wxImage(widthPixels, heightPixels);
    m_pdata = m_buffer.GetData();
    m_oRenderingBuffer.attach(m_pdata, m_nBufWidth, m_nBufHeight, m_nStride);

    //To treat rendering buffer as pixels
    m_pPixelsBuffer = new lmPixelsBuffer( m_oRenderingBuffer );

    //the base renderer
    m_pRenBase = new base_ren_type( *m_pPixelsBuffer );

    m_pRenSolid = new lmRendererSolidType(*m_pRenBase);
    m_pRenBase->clear(agg::rgba8(255,255,255));

    ////font settings
    //m_feng.gamma(agg::gamma_none());

    //the renderer_marker, for aliased drawing.
    //Renderer_marker is a specialization of renderer_primitives so it
    //includes all its methods.
    m_pRenMarker = new lmRendererMarkerType(*m_pRenBase);

    // initialize colours
    m_pRenMarker->line_color( m_colorF );

}

lmAggDrawer::~lmAggDrawer()
{
    //wxLogMessage(_T("[lmAggDrawer::~lmAggDrawer]"));
    delete m_pDummyBitmap;
    delete m_pRenMarker;
    delete m_pRenSolid;
    delete m_pPixelsBuffer;
    delete m_pRenBase;
}

void lmAggDrawer::Initialize()
{
    //m_nCursorX = 0;
    //m_nCursorY = 0;

    ////font
    //m_fValidFont = false;
    //m_rFontHeight = 14;
    //m_rFontWidth = 14;
    //m_fHinting = true;
    //m_fKerning = true;

    //allocate something to paint on it
    m_pDummyBitmap = new wxBitmap(1, 1);
    ((wxMemoryDC*)m_pDC)->SelectObject(*m_pDummyBitmap);


    //compute world to device conversion factors
    m_xDevicePixelsPerLU = (double)m_pDC->LogicalToDeviceXRel(100000) / 100000.0;
    m_yDevicePixelsPerLU = (double)m_pDC->LogicalToDeviceYRel(100000) / 100000.0;

    //wxLogMessage(_T("[lmAggDrawer::Initialize] m_xDevicePixelsPerLU=%f, m_yDevicePixelsPerLU=%f"),
    //    m_xDevicePixelsPerLU, m_yDevicePixelsPerLU);

    // set default colours
    m_colorF = agg::rgba8(0,0,0);
    m_colorB = agg::rgba8(255, 255, 255);

    wxBrush brush(wxColour(0,0,0), wxSOLID);
    SetBrush(brush);
    m_pDC->SetBackgroundMode(wxTRANSPARENT);
    m_pDC->SetBackground(*wxWHITE_BRUSH);
    m_pDC->SetTextBackground(wxColour(0,0,0));

}

//draw shapes
void lmAggDrawer::DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{ 
    //wxLogMessage(_T("[lmAggDrawer::DrawLine]"));
    // renderer_marker expects no decimals, so values must be multiplied by 256.
    m_pRenMarker->line(WorldToDeviceX(x1)*256, WorldToDeviceY(y1)*256,
            WorldToDeviceX(x2)*256, WorldToDeviceY(y2)*256, true);

}

void lmAggDrawer::DrawRectangle(wxPoint point, wxSize size)
{ 
    DrawRectangle(point.x, point.y, size.GetWidth(), size.GetHeight());
}

void lmAggDrawer::DrawRectangle(wxCoord left, wxCoord top, wxCoord width, wxCoord height)
{ 
    //wxLogMessage(_T("[lmAggDrawer::DrawRectangle]"));
    double x = WorldToDeviceX(left);
    double y = WorldToDeviceY(top);
    double w = WorldToDeviceX(width);
    double h = WorldToDeviceY(height);

    m_ras.reset(); 
    m_ras.move_to_d(x, y);
    m_ras.line_to_d(x, y + h);
    m_ras.line_to_d(x + w, y + h);
    m_ras.line_to_d(x + w, y);
    m_ras.line_to_d(x, y);
    agg::render_scanlines_aa_solid(m_ras, m_sl, *m_pRenBase, m_colorF);

    //wxLogMessage(_T("[lmAggDrawer::DrawRectangle] DrawRectangle(%.2f, %.2f, %.2f, %.2f)"), x, y, w, h);
}

void lmAggDrawer::DrawCircle(wxCoord x, wxCoord y, wxCoord radius)
{ 
    //! @todo
    //wxLogMessage(_T("[lmAggDrawer::DrawCircle]"));
    m_pDC->DrawCircle(x, y, radius); 
}

void lmAggDrawer::DrawCircle(const wxPoint& pt, wxCoord radius) 
{ 
    //! @todo
    //wxLogMessage(_T("[lmAggDrawer::DrawCircle]"));
    m_pDC->DrawCircle(pt, radius); 
}

void lmAggDrawer::DrawPolygon(int n, wxPoint points[]) 
{ 
    //wxLogMessage(_T("[lmAggDrawer::DrawPolygon]"));
    m_ras.reset(); 
    m_ras.move_to_d( WorldToDeviceX(points[0].x), WorldToDeviceY(points[0].y) );
    int i;
    for (i=1; i < n; i++) {
        m_ras.line_to_d( WorldToDeviceX(points[i].x), WorldToDeviceY(points[i].y) );
    }
    agg::render_scanlines_aa_solid(m_ras, m_sl, *m_pRenBase, m_colorF);

}

//brushes, colors, fonts, ...
void lmAggDrawer::SetBrush(wxBrush brush) 
{ 
    //! @todo
    //wxLogMessage(_T("[lmAggDrawer::SetBrush]"));
    m_pDC->SetBrush(brush); 
}

void lmAggDrawer::SetFont(wxFont& font) 
{
    //! @todo
    //wxLogMessage(_T("[lmAggDrawer::SetFont]"));
    m_pDC->SetFont(font); 
}

void lmAggDrawer::SetPen(wxPen& pen) 
{ 
    //wxLogMessage(_T("[lmAggDrawer::SetPen]"));
    m_pDC->SetPen(pen); 
}

const wxPen& lmAggDrawer::GetPen() const 
{ 
    //! @todo
    //wxLogMessage(_T("[lmAggDrawer::GetPen]"));
    return m_pDC->GetPen(); 
}

void lmAggDrawer::SetLogicalFunction(int function) 
{ 
    //! @todo
    //wxLogMessage(_T("[lmAggDrawer::SetLogicalFunction]"));
    m_pDC->SetLogicalFunction(function); 
}


//text
void lmAggDrawer::DrawText(const wxString& text, wxCoord x, wxCoord y) 
{
    //wxLogMessage(_T("[lmAggDrawer::DrawText]"));
#if 0
        //
        // Experimental code to use native text renderization
        // Version 1. Render on a new bitmap and blend it with main bitmap
        //

    // Get size of text, in logical units
    lmLUnits wL, hL;
    m_pDC->GetTextExtent(text, &wL, &hL);

    // convert size to pixels. As GetTextExtent has not enough precision
    // I will add a couple of pixels for security
    wxCoord wD = m_pDC->LogicalToDeviceXRel(wL) + 2,
            hD = m_pDC->LogicalToDeviceYRel(hL) + 2;

    // allocate the bitmap
    wxBitmap bitmap((int)wD, (int)hD);
    wxMemoryDC* pDC = (wxMemoryDC*)m_pDC;
    pDC->SelectObject(bitmap);

    // draw onto the bitmap
    //m_pDC->SetBackground(*wxWHITE_BRUSH);
    //m_pDC->SetBackgroundMode(wxTRANSPARENT);
    //m_pDC->SetTextForeground(*wxBLACK);
    m_pDC->Clear();
    m_pDC->DrawText(text, 0, 0);
    pDC->SelectObject(wxNullBitmap);

    // Convert to image and make it masked
    wxImage image = bitmap.ConvertToImage();
    static int hh=0;
    if (hh++==50) {
        image.SaveFile(_T("C:\\usr\\agg_image.bmp"), wxBITMAP_TYPE_BMP);
        int iX, iY;
        for (iX=0; iX < wD; iX++) {
            for(iY=0; iY < hD; iY++) {
                unsigned char red = image.GetRed(iX, iY);
                unsigned char green = image.GetGreen(iX,iY);
                unsigned char blue = image.GetBlue(iX,iY);
                //wxLogMessage(_T("color r=%d, g=%d, b=%d"), red, green, blue);
            }
            //wxLogMessage(_T("New row"));
        }
    }
    //image.SetMaskColour(255, 255, 255);

    // get the image buffer and blend it with this Drawer main buffer
    agg::rendering_buffer oBuffer;     //the agg buffer to be blended
    unsigned char* pData = image.GetData();
    oBuffer.attach(pData, wD, hD, 0);
    lmPixelsBuffer* pPixels = new lmPixelsBuffer( oBuffer );    //the bitmap buffer as pixels

    lmColor_rgba8 colorWhite = agg::rgba8(255, 255, 255);
    lmColor_rgba8 color;
    int xD = (int)floor(WorldToDeviceX(x) + 0.5);
    int yD = (int)floor(WorldToDeviceY(y) + 0.5);
    int iX, iY;
    for (iX=0; iX < wD; iX++) {
        for(iY=0; iY < hD; iY++) {
            unsigned char red = image.GetRed(iX, iY);
            unsigned char green = image.GetGreen(iX,iY);
            unsigned char blue = image.GetBlue(iX,iY);
            color = agg::rgba8((unsigned int)red, (unsigned int)blue, (unsigned int)green);  //pPixels->pixel(iX, iY);
            if (!(red == 255 && blue == 255 && green == 255)) {
                m_pPixelsBuffer->copy_pixel(iX+xD, iY+yD, color);
            }
        }
    }

    delete pPixels;
#endif



#if 1
        //
        // Experimental code to use native text renderization. 
        // Version 2. Render on a copy of main bitmap
        //

    // Get size of text, in logical units
    lmLUnits wL, hL;
    GetTextExtent(text, &wL, &hL);

    // convert size to pixels. As GetTextExtent has not enough precision
    // I will add a couple of pixels for security
    wxCoord wD = m_pDC->LogicalToDeviceXRel(wL) + 2,
            hD = m_pDC->LogicalToDeviceYRel(hL) + 2;

    // get a bitmap initialized with main bitmap
    int xD = (int)floor(WorldToDeviceX(x) + 0.5);
    int yD = (int)floor(WorldToDeviceY(y) + 0.5);

    //if clipped
    wxImage subimage;
    int xC = (xD > 0 ? xD : 0);
    int yC = (yD > 0 ? yD : 0);
    int wC = (xD > 0 ? wD : xD+wD);
    int hC = (yD > 0 ? hD : yD+hD);
    int yS = (yD > 0 ? 0 : -yD);
    int xS = (xD > 0 ? 0 : -xD);

    if (xD < 1 || yD < 1) {
        subimage.Create(wD, hD);
        unsigned char* pData = subimage.GetData();
        int iY;
        int nLength = wC * BYTES_PP;       // line length
        for (iY=0; iY < hC; iY++) {
            memcpy(pData+(iY+yS)*nLength, 
                   m_pdata + m_nBufWidth*(yC+iY)*BYTES_PP + xC*BYTES_PP,
                   nLength);
        }
    }
    else {
        subimage = m_buffer.GetSubImage(wxRect(xC, yC, wC, hC));
    }
    wxBitmap bitmap(subimage);
    wxMemoryDC* pDC = (wxMemoryDC*)m_pDC;
    pDC->SelectObject(bitmap);

    // draw onto the bitmap
    m_pDC->DrawText(text, 0, 0);
    pDC->SelectObject(*m_pDummyBitmap);

    // Convert bitmap to image and copy it on main buffer
    wxImage image = bitmap.ConvertToImage();
    unsigned char* pData = image.GetData();
    int iY;
    int nLength = wC * BYTES_PP;       // line length
    for (iY=0; iY < hC; iY++) {
        memcpy(m_pdata + m_nBufWidth*(yC+iY)*BYTES_PP + xC*BYTES_PP,
               pData+(iY+yS)*nLength,
               nLength);
    }

#endif


}

void lmAggDrawer::SetTextForeground(const wxColour& colour) 
{
    //! @todo
    //wxLogMessage(_T("[lmAggDrawer::SetTextForeground]"));
    m_pDC->SetTextForeground(colour); 
}

void lmAggDrawer::SetTextBackground(const wxColour& colour) 
{
    //! @todo
    //wxLogMessage(_T("[lmAggDrawer::SetTextBackground]"));
    m_pDC->SetTextBackground(colour); 
}

void lmAggDrawer::GetTextExtent(const wxString& string, lmLUnits* w, lmLUnits* h) 
{ 
    //! @todo
    //wxLogMessage(_T("[lmAggDrawer::GetTextExtent]"));
    wxCoord width, height;
    m_pDC->GetTextExtent(string, &width, &height);
    *w = (lmLUnits)width;
    *h = (lmLUnits)height;
}


// units conversion
lmLUnits lmAggDrawer::DeviceToLogicalX(lmPixels x) 
{
    return m_pDC->DeviceToLogicalXRel(x); 
}

lmLUnits lmAggDrawer::DeviceToLogicalY(lmPixels y) 
{ 
    return m_pDC->DeviceToLogicalYRel(y); 
}

lmPixels lmAggDrawer::LogicalToDeviceX(lmLUnits x) 
{ 
    return m_pDC->LogicalToDeviceXRel(x); 
}

lmPixels lmAggDrawer::LogicalToDeviceY(lmLUnits y) 
{ 
    return m_pDC->LogicalToDeviceYRel(y); 
}


/*

void lmAggDrawer::Clear()
{
    memset(m_pdata, 255, m_nBufWidth * m_nBufHeight * BYTES_PP);
}
 
unsigned int lmAggDrawer::DrawText(wxString& sText)
{
    //render text (FreeType) at current position, using current settings for font
    //returns the number of glyphs rendered

    if (!m_fValidFont) return 0;

    //convert text to utf-32
    size_t nLength = sText.Length();
    wxMBConvUTF32 oConv32;
    wxCharBuffer s32Text = sText.mb_str(oConv32);

    return DrawText((unsigned int*)s32Text.data(), nLength);
}

bool lmAggDrawer::LoadFont(wxString sFontName)
{
    //load the requested font. Returns true if any error

    wxMBConvUTF8 oConv;
    wxCharBuffer str = sFontName.mb_str(oConv);

    agg::glyph_rendering gren = agg::glyph_ren_agg_gray8;
    if(! m_feng.load_font(str.data(), 0, gren)) {
        m_fValidFont = false;
        return true;    //error
    }

    //set curren values for renderization
    m_feng.hinting(m_fHinting);
    m_feng.height(m_rFontHeight);
    m_feng.width(m_rFontWidth);
    m_feng.flip_y(m_fFlip_y);

    //un-comment this to rotate/skew/translate the text
    //agg::trans_affine mtx;
    //mtx *= agg::trans_affine_rotation(agg::deg2rad(-4.0));
    ////mtx *= agg::trans_affine_skewing(-0.4, 0);
    ////mtx *= agg::trans_affine_translation(1, 0);
    //m_feng.transform(mtx);

    m_fValidFont = true;
    return false;   
}

unsigned int lmAggDrawer::DrawText(unsigned int* pText, size_t nLength)
{
    //returns the number of glyphs rendered

    if (!m_fValidFont) return 0;

    unsigned num_glyphs = 0;

    double x  = (double)m_nCursorX;
    double y0 = m_rFontHeight + 10.0;
    double y  = (double)m_nCursorY;

    const unsigned int* p = pText;

    while(*p && nLength--)
    {
        const agg::glyph_cache* glyph = m_fman.glyph(*p);
        if(glyph)
        {
            if(m_fKerning)
            {
                m_fman.add_kerning(&x, &y);
            }

            if(x >= m_nBufWidth - m_rFontHeight)
            {
                x = 10.0;
                y0 += m_rFontHeight;
                //if(y0 <= 120) break;
                y = y0;
            }

            m_fman.init_embedded_adaptors(glyph, x, y);

            //render the glyph using method agg::glyph_ren_agg_gray8
            m_pRenSolid->color(agg::rgba8(0, 0, 0));
            agg::render_scanlines(m_fman.gray8_adaptor(), 
                                    m_fman.gray8_scanline(), 
                                    *m_pRenSolid);

            // increment pen position
            x += glyph->advance_x;
            y += glyph->advance_y;
            ++num_glyphs;
        }
        ++p;
    }
    return num_glyphs;

}

//--------------------------------------------------------------------------------------
// methods to draw aliased objects using renderer_primitives / renderer_marker
//--------------------------------------------------------------------------------------

void lmAggDrawer::DrawLine(int x1, int y1, int x2, int y2, bool fIncludeLast)
{
    m_pRenMarker->line(x1, y1, x2, y2, fIncludeLast);
}

void lmAggDrawer::MoveTo(int x, int y)
{
    m_pRenMarker->move_to(x, y);
}

void lmAggDrawer::LineTo(int x, int y, bool fIncludeLast)
{
    m_pRenMarker->line_to(x, y, fIncludeLast);
}

void lmAggDrawer::SetLineColor(const lmColor_rgba8& c)
{
    m_pRenMarker->line_color(c);
}

void lmAggDrawer::SetFillColor(const lmColor_rgba8& c)
{
    m_pRenMarker->fill_color(c);
}

const lmColor_rgba8& lmAggDrawer::GetFillColor()
{
    return m_pRenMarker->fill_color();
}

const lmColor_rgba8& lmAggDrawer::GetLineColor()
{
    return m_pRenMarker->line_color();
}

void lmAggDrawer::DrawEllipse(int x, int y, int rx, int ry)
{
    m_pRenMarker->ellipse(x, y, rx, ry);
}

//-----------------------------------------------------------------------------------
// render anti-aliased
//-----------------------------------------------------------------------------------

void lmAggDrawer::DrawLine_AA(double x1, double y1, double x2, double y2, double rWidth)
{
    m_ras.move_to_d(x1, y1);
    m_ras.line_to_d(x2, y2);
    m_ras.line_to_d(x2+rWidth, y2+rWidth);
    m_ras.line_to_d(x1+rWidth, y1+rWidth);
    m_ras.line_to_d(x1, y1);
}

*/
