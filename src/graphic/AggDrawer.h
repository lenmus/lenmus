//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_AGGDRAWER_H__        //to avoid nested includes
#define __LM_AGGDRAWER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "AggDrawer.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/defs.h"
#include "Drawer.h"

#include "wx/image.h"


#include "agg_rendering_buffer.h"       //to deal with bytes buffer
#include "agg_pixfmt_rgb.h"             //to use buffer as pixels (to know about the pixel format)
#include "agg_rasterizer_scanline_aa.h" //to use rasterized scanline
#include "agg_scanline_u.h"
#include "agg_renderer_scanline.h"
#include "agg_renderer_primitives.h"    //for rendering aliassed lines
#include "agg_renderer_markers.h"       //for rendering markers
#include "agg_scanline_bin.h"
#include "agg_font_freetype.h"          //font renderization using FreeType


//basic types
typedef agg::rgba8      lmColor_rgba8;      //rgba, 8 bits per channel (four bytes)
typedef agg::rgba       lmColor_rgba;       //internal rgba, double value per channel

#define pix_format                                      agg::pix_format_rgb24
typedef agg::pixfmt_rgb24                               lmPixelsBuffer;
typedef agg::renderer_base<lmPixelsBuffer>              base_ren_type;
typedef agg::renderer_scanline_aa_solid<base_ren_type>  lmRendererSolidType;

typedef agg::font_engine_freetype_int32             font_engine_type;
typedef agg::font_cache_manager<font_engine_type>   font_manager_type;

typedef agg::font_cache_manager<font_engine_type>::gray8_adaptor_type   lmGray8AdaptorType;
typedef agg::font_cache_manager<font_engine_type>::gray8_scanline_type  lmGary8ScanlineType;

//-----------------------------------------------------------------------------------------
// Helper class to manage music font
// This class is a singleton
//-----------------------------------------------------------------------------------------

class lmMusicFontManager
{
public:
    ~lmMusicFontManager();

    static void DeleteInstance();
    static lmMusicFontManager* GetInstance();

    bool LoadFont(wxString& sFontName);
    inline bool IsFontValid() { return m_fValidFont; }
    inline void SetScale(double rScale) { m_rScale = rScale; }

    inline const agg::glyph_cache* GetGlyphCache(unsigned int nChar) { return m_fman.glyph(nChar); }
    inline void AddKerning(double* x, double* y) { m_fman.add_kerning(x, y); }
    inline void InitAdaptors(const agg::glyph_cache* glyph, double x, double y)
                    { m_fman.init_embedded_adaptors(glyph, x, y); }
    inline lmGray8AdaptorType& GetGray8AdaptorType() { return m_fman.gray8_adaptor(); }
    inline lmGary8ScanlineType& GetGray8ScanlineType() { return m_fman.gray8_scanline(); }


    inline double GetFontHeight() { return m_rFontHeight; }
    inline double GetAscender() { return m_feng.ascender(); }
    inline double GetDescender() { return m_feng.descender(); }

    void SetFontSize(double rPoints);
    void SetFontHeight(double rPoints);
    void SetFontWidth(double rPoints);

protected:
    lmMusicFontManager();

private:

    static lmMusicFontManager*  m_pInstance;    //the only instance of this class

    font_engine_type            m_feng;
    font_manager_type           m_fman;

    bool m_fFlip_y;

    //font related variables
    double  m_rScale;           //view scale, to compute font size for glyphs
    double  m_rFontHeight;
    double  m_rFontWidth;
    bool    m_fHinting;
    bool    m_fValidFont;       //there is a font loaded

};



//-----------------------------------------------------------------------------------------
// AggDrawer is an anti-aliased drawer using the AGG library (Anti-Grain Geometry)
//-----------------------------------------------------------------------------------------

class lmAggDrawer : public lmDrawer
{
public:
    //Constructor, allocating a new bitmap as rendering buffer
    lmAggDrawer(int widthPixels, int heightPixels, double rScale = 1.0, int nStride=0);

	//Constructor, allocating a copy of the received bitmap as rendering buffer
	lmAggDrawer(wxBitmap* pBitmap, double rScale, int stride=0);

    ~lmAggDrawer();

    bool IsDirectDrawer() { return false; }

    //access to image
    wxImage& GetImageBuffer() { return m_buffer; }

    // Aliased shapes, even when anti-alising is supported.
    void SketchLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2, wxColour color,
					int style = wxSOLID);
    void SketchRectangle(lmUPoint uPoint, lmUSize uSize, wxColour color);

    //solid shapes, anti-aliased when supported.
    void SolidPolygon(int n, lmUPoint uPoints[], wxColor color);
    void SolidCircle(lmLUnits ux, lmLUnits uy, lmLUnits uRadius);
    void SolidShape(lmShape* pShape, wxColor color);

    //brushes, colors, fonts, ...
    void SetFont(wxFont& font);
    void SetLogicalFunction(int function);

    //general operations
    void Clear();

    wxColour GetFillColor();
    void SetFillColor(wxColour color);
    wxColour GetLineColor();
    void SetLineColor(wxColour color);
    void SetLineWidth(lmLUnits uWidth);
    void SetPen(wxColour color, lmLUnits uWidth);

    //text
    void DrawText(const wxString& text, lmLUnits x, lmLUnits y);
    void SetTextForeground(const wxColour& colour);
    void SetTextBackground(const wxColour& colour);
    void GetTextExtent(const wxString& string, lmLUnits* w, lmLUnits* h);

    //text (FreeType with AGG rederization)
    bool FtLoadFont(wxString& sFontName);
    int FtDrawChar(unsigned int nChar);
    int FtDrawText(wxString& sText);
    int FtDrawText(unsigned int* pText, size_t nLength);

    inline void FtSetFontSize(double rPoints) { return m_pMFM->SetFontSize(rPoints); }
    inline void FtSetFontHeight(double rPoints) { return m_pMFM->SetFontHeight(rPoints); }
    inline void FtSetFontWidth(double rPoints) { return m_pMFM->SetFontWidth(rPoints); }

    void FtSetTextPosition(lmLUnits uxPos, lmLUnits uyPos);
    void FtSetTextPositionPixels(lmPixels vxPos, lmPixels vyPos);
    void FtGetTextExtent(const wxString& sText, lmLUnits* pWidth, lmLUnits* pHeight,
                         lmLUnits* pDescender = NULL, lmLUnits* pAscender = NULL);
    lmURect FtGetGlyphBounds(unsigned int nGlyph);
    wxRect FtGetGlyphBoundsInPixels(unsigned int nGlyph);

    // units conversion
    lmLUnits DeviceToLogicalX(lmPixels x);
    lmLUnits DeviceToLogicalY(lmPixels y);
    lmPixels LogicalToDeviceX(lmLUnits x);
    lmPixels LogicalToDeviceY(lmLUnits y);

    //conversion
    inline double WorldToDeviceX(lmLUnits x) const { return m_xDevicePixelsPerLU * (double)x; }
    inline double WorldToDeviceY(lmLUnits y) const { return m_yDevicePixelsPerLU * (double)y; }
    inline lmLUnits DeviceToWorldX(double x) const { return (lmLUnits)(x / m_xDevicePixelsPerLU); }
    inline lmLUnits DeviceToWorldY(double y) const { return (lmLUnits)(y / m_yDevicePixelsPerLU); }
    lmColor_rgba8 lmToRGBA8(wxColour color);
    wxColour lmToWxColor(lmColor_rgba8 color);

private:
    void Initialize(double rScale);
	void Create(int stride);


        // member variables

    //the rendering buffer as bytes
    wxImage                     m_buffer;               //the image to serve as buffer
    unsigned char*              m_pdata;                //ptr to the real bytes buffer
    agg::rendering_buffer       m_oRenderingBuffer;     //the agg rendering buffer
    int                         m_nBufWidth, m_nBufHeight;      //size of the bitmap
    int                         m_nStride;               //the row step

    //the rendering buffer as pixels
    lmPixelsBuffer*     m_pPixelsBuffer;

    //the base renderer
    base_ren_type*      m_pRenBase;

    //anti-aliased rendered
    lmRendererSolidType*     m_pRenSolid;


    agg::scanline_u8                m_sl;
    agg::rasterizer_scanline_aa<>   m_ras;

    //the renderer_marker, for aliased drawing of shapes.
    typedef agg::renderer_markers<base_ren_type> lmRendererMarkerType;
    lmRendererMarkerType*       m_pRenMarker;

    // scaling factors between device pixels and logical units
    double        m_xDevicePixelsPerLU;
    double        m_yDevicePixelsPerLU;


    //current brush/pen/color settings
    lmColor_rgba8   m_textColorF;       //text foreground color;
    lmColor_rgba8   m_textColorB;       //text background color;
    lmColor_rgba8   m_lineColor;        //pen color
    lmColor_rgba8   m_fillColor;        //brush color


    //the dummy DC used for units conversion
    wxMemoryDC      m_DummyDC;
    wxBitmap*       m_pDummyBitmap;

    //FreeType fonts: font engine and font cache management
    lmMusicFontManager*     m_pMFM;

    //font_engine_type    m_feng;
    //font_manager_type   m_fman;

    //bool m_fFlip_y;

    ////font related variables
    //double  m_rScale;           //view scale, to compute font size for glyphs
    //double  m_rFontHeight;
    //double  m_rFontWidth;
    //bool    m_fHinting;
    bool    m_fKerning;
    //bool    m_fValidFont;       //there is a font loaded

    //current buffer font position (pixels, with decimals)
    double     m_vCursorX;
    double     m_vCursorY;
};


#endif  // __LM_AGGDRAWER_H__

