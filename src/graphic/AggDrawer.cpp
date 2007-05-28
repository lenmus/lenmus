//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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
#pragma implementation "AggDrawer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "AggDrawer.h"
#include "agg_ellipse.h"


// lmDrawer interface must be designed so that all methods can be easily implemented
// with wxDC methods as all printing will be done without anti-aliasing, by direct usage
// of printer wxDC (through lmDirecDrawer object). Also dragging
// and play highlight is done with wxDC methods
//
// In wxWidgets:
// - the pen is for drawing outlines (outline of rectangles, ellipses, etc.)
// - the brush is for filling in areas (background of rectangles, ellipses, etc.)
// - text appearance is controlled neither by pen nor by brush but by methods
//      SetTextForeground() and SetTextBackground()
//
// Drawing methods
// - SolidX : Solid renderization, anti-aliased
// - OutlinedX: Outline renderization, anti-aliased
// - SketchX: Aliased renderization. Will use current settings for outline and
//      fill colors.
//
// Two versions: with pen/brush parameters and without these parameters. Versions
// with pen/brush parameters don't change pen/brush settings
//
// Coordinates
//
// All lmDrawer methods expect coordinates in world units (lmLUnits). But all
// AGG methods expect coordinates in device units (pixels). So all lmAggDrawer
// methods must convert the received coordinates.
// Anti-aliased methods expects either doubles or integers in format (24,8).
// But renderer_marker expects no decimals, so values must be multiplied by 256.
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
    //wxLogMessage(_T("[lmAggDrawer::lmAggDrawer] pixels=(%d, %d)"), widthPixels, heightPixels);
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
    m_pRenMarker->line_color( m_textColorF );

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
    m_textColorF = agg::rgba8(0,0,0);
    m_textColorB = agg::rgba8(255, 255, 255);
    m_lineColor = agg::rgba8(0,0,0);
    m_fillColor = agg::rgba8(0,0,0);

    m_pDC->SetBackgroundMode(wxTRANSPARENT);
    m_pDC->SetBackground(*wxWHITE_BRUSH);
    m_pDC->SetTextBackground(wxColour(0,0,0));

}

//draw shapes
void lmAggDrawer::SketchLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
                             wxColour color)
{
    m_pRenMarker->line_color( lmToRGBA8(color) );
    // renderer_marker expects no decimals, so values must be multiplied by 256.
    m_pRenMarker->line(WorldToDeviceX(x1)*256, WorldToDeviceY(y1)*256,
            WorldToDeviceX(x2)*256, WorldToDeviceY(y2)*256, true);  //true means 'include last point'

}

void lmAggDrawer::SketchRectangle(lmUPoint point, wxSize size, wxColour color)
{
    // renderer_marker expects no decimals, so values must be multiplied by 256.
    double x = WorldToDeviceX(point.x) * 256.0;
    double y = WorldToDeviceY(point.y) * 256.0;
    double w = WorldToDeviceX(size.GetWidth()) * 256.0;
    double h = WorldToDeviceY(size.GetHeight()) * 256.0;

    m_pRenMarker->line_color( lmToRGBA8(color) );
    // in last parameter, true means 'include last point'
    m_pRenMarker->line(x, y, x+w, y, false);
    m_pRenMarker->line(x+w, y, x+w, y+h, false);
    m_pRenMarker->line(x+w, y+h, x, y+h, false);
    m_pRenMarker->line(x, y+h, x, y, false);

}

//void lmAggDrawer::SolidRectangle(wxCoord left, wxCoord top, wxCoord width, wxCoord height)
//{
//    double x = WorldToDeviceX(left);
//    double y = WorldToDeviceY(top);
//    double w = WorldToDeviceX(width);
//    double h = WorldToDeviceY(height);
//
//    m_ras.reset();
//    m_ras.move_to_d(x, y);
//    m_ras.line_to_d(x, y + h);
//    m_ras.line_to_d(x + w, y + h);
//    m_ras.line_to_d(x + w, y);
//    m_ras.line_to_d(x, y);
//    agg::render_scanlines_aa_solid(m_ras, m_sl, *m_pRenBase, m_textColorF);
//
//}

void lmAggDrawer::SolidCircle(lmLUnits xCenter, lmLUnits yCenter, lmLUnits radius)
{
    double x = WorldToDeviceX(xCenter);
    double y = WorldToDeviceY(yCenter);
    double r = WorldToDeviceX(radius);

    agg::ellipse e1;
    m_ras.reset();
    e1.init(x, y, r, r, 0);
    m_ras.add_path(e1);
    agg::render_scanlines_aa_solid(m_ras, m_sl, *m_pRenBase, m_fillColor);
}

void lmAggDrawer::SolidPolygon(int n, lmUPoint points[], wxColor color)
{
    //wxLogMessage(_T("[lmAggDrawer::SolidPolygon]"));
    m_ras.reset();
    m_ras.move_to_d( WorldToDeviceX(points[0].x), WorldToDeviceY(points[0].y) );
    int i;
    for (i=1; i < n; i++) {
        m_ras.line_to_d( WorldToDeviceX(points[i].x), WorldToDeviceY(points[i].y) );
    }
    agg::render_scanlines_aa_solid(m_ras, m_sl, *m_pRenBase, lmToRGBA8(color));

}


//brushes, colors, fonts, ...


wxColour lmAggDrawer::GetFillColor()
{
    return lmToWxColor(m_fillColor);
}

void lmAggDrawer::SetFillColor(wxColour color)
{
    m_fillColor = lmToRGBA8(color);
}

wxColour lmAggDrawer::GetLineColor()
{
    return lmToWxColor(m_lineColor);
}

void lmAggDrawer::SetLineColor(wxColour color)
{
    m_lineColor = lmToRGBA8(color);
}

void lmAggDrawer::SetLineWidth(lmLUnits uWidth)
{
    m_uLineWidth = uWidth;
}

void lmAggDrawer::SetPen(wxColour color, lmLUnits uWidth)
{
    m_lineColor = lmToRGBA8(color);
    m_uLineWidth = uWidth;
}

void lmAggDrawer::SetFont(wxFont& font)
{
    //wxLogMessage(_T("[lmAggDrawer::SetFont]"));
    m_pDC->SetFont(font);
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

    // intersect bitmap rectangle with main bitmap rectangle
    int xC = wxMax(xD, 0);
    int yC = wxMax(yD, 0);
    int xBotC = wxMax( wxMin(xD + wD, m_nBufWidth), 0);
    int yBotC = wxMax( wxMin(yD + hD, m_nBufHeight), 0);
    int wC = xBotC - xC;
    int hC = yBotC - yC;

    //if intersection is null, terminate. Text is clipped
    if (wC <= 0 || hC <= 0) return;

    //int xS = (xD > 0 ? 0 : -xD);
    int yS = (yD > 0 ? 0 : -yD);

    //if text bitmap is fully over main bitmap, just get subimage
    //else get intersection subimage and copy it to greater text bitmap

    wxImage subimage;
    if (xD >= 0 && yD >=0 && wD <= m_nBufWidth && hD <= m_nBufHeight)
    {
        //bitmap is fully over main bitmap, just get subimage
        subimage = m_buffer.GetSubImage(wxRect(xC, yC, wC, hC));
    }
    else
    {
        //get intersection subimage and copy it to greater text bitmap
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

    //create the bitmap from the initialized subimage
    wxBitmap bitmap(subimage);
    wxMemoryDC* pDC = (wxMemoryDC*)m_pDC;
    pDC->SelectObject(bitmap);

    // draw text onto this bitmap
    m_pDC->DrawText(text, 0, 0);
    pDC->SelectObject(*m_pDummyBitmap);

    // Convert bitmap to image and copy it back on main buffer
    wxImage image = bitmap.ConvertToImage();
    unsigned char* pData = image.GetData();
    int iY;
    int nLength = wC * BYTES_PP;       // line length
    for (iY=0; iY < hC; iY++) {
        memcpy(m_pdata + m_nBufWidth*(yC+iY)*BYTES_PP + xC*BYTES_PP,
               pData+(iY+yS)*nLength,
               nLength);
    }

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


lmColor_rgba8 lmAggDrawer::lmToRGBA8(wxColour color)
{
    return agg::rgba8(color.Red(), color.Green(), color.Blue());
}

wxColour lmAggDrawer::lmToWxColor(lmColor_rgba8 color)
{
    return wxColor(color.r, color.g, color.b);
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

*/
