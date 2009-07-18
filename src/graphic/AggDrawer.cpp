//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
#pragma implementation "AggDrawer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "AggDrawer.h"
#include "GMObject.h"
#include "agg_basics.h"
#include "agg_conv_curve.h"
#include "agg_conv_stroke.h"
#include "agg_conv_marker.h"
#include "agg_conv_concat.h"
#include "agg_path_storage.h"
#include "agg_vcgen_markers_term.h"
#include "agg_ellipse.h"

//-----------------------------------------------------------------------------------------
// lmMusicFontManager implementation
//-----------------------------------------------------------------------------------------

lmMusicFontManager* lmMusicFontManager::m_pInstance = (lmMusicFontManager*)NULL;

lmMusicFontManager::lmMusicFontManager()
    : m_feng()
    , m_fman(m_feng)
{
    //font defaults (FreeType)
    m_fValidFont = false;
    m_rFontHeight = 14;
    m_rFontWidth = 14;
    m_fHinting = false;     //hinting is pantented by Apple. Can not be used without license
    //AWARE:
    //Apple Computer, Inc., owns three patents that are related to the
    //hinting process of glyph outlines within TrueType fonts. Hinting (also named
    //grid-fitting) is used to enhance the quality of glyphs at small bitmap sizes.
    //Therefore, you can not use hinting unless you are authorized (you purchased
    //a license from Apple, or because you are in a country where the patents do
    //not apply, etc.). Nevertheless lenmus font doesn't include hinting information
    //and, so, previous flag value doesn't matter. But its value is important
    //if I finally use FreeType for all fonts.


    m_fFlip_y = true;       //TODO: Apparently the behaviour is reversed !!! Investigate this
    m_rScale = 1.0;

    //font settings
    m_feng.gamma(agg::gamma_none());

    //load music font
    wxString sMusicFont = _T("lmbasic2.ttf");
    LoadFont(sMusicFont);
}

lmMusicFontManager::~lmMusicFontManager()
{
}

lmMusicFontManager* lmMusicFontManager::GetInstance()
{
    if (!m_pInstance)
    {
        m_pInstance = new lmMusicFontManager();
    }
    return m_pInstance;
}

void lmMusicFontManager::DeleteInstance()
{
    if (m_pInstance)
        delete m_pInstance;
}

bool lmMusicFontManager::LoadFont(wxString& sFontName)
{
    //load the requested font. It must be in /bin folder
    //Returns true if any error

    //wxString sFullFileName = GetFontFullFileName(sFontName);

    wxMBConvUTF8 oConv;
    wxCharBuffer str = sFontName.mb_str(oConv);

    agg::glyph_rendering gren = agg::glyph_ren_agg_gray8;
    if(! m_feng.load_font(str.data(), 0, gren))
    {
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

    //wxLogMessage(_T("[lmMusicFontManager::LoadFont] font asc: %.2lf, desc:%.2lf\n"),
    //             m_feng.ascender(), m_feng.descender() );

    m_fValidFont = true;
    return false;
}

void lmMusicFontManager::SetFontSize(double rPoints)
{
    m_rFontHeight = m_rFontWidth = rPoints * m_rScale;
    m_feng.height(m_rFontHeight);
    m_feng.width(m_rFontWidth);
}

void lmMusicFontManager::SetFontHeight(double rPoints)
{
    m_rFontHeight = rPoints, m_feng.height(rPoints);
}

void lmMusicFontManager::SetFontWidth(double rPoints)
{
    m_rFontWidth = rPoints, m_feng.width(rPoints);
}



//-----------------------------------------------------------------------------------------
// agg basic vertices sources
//-----------------------------------------------------------------------------------------

struct lmAggLineVS
{
    double x1, y1, x2, y2;
    int f;

    lmAggLineVS(double x1_, double y1_, double x2_, double y2_) :
        x1(x1_), y1(y1_), x2(x2_), y2(y2_), f(0) {}

    void rewind(unsigned) { f = 0; }
    unsigned vertex(double* x, double* y)
    {
        if(f == 0) { ++f; *x = x1; *y = y1; return agg::path_cmd_move_to; }
        if(f == 1) { ++f; *x = x2; *y = y2; return agg::path_cmd_line_to; }
        return agg::path_cmd_stop;
    }
};


//-----------------------------------------------------------------------------------------
lmLineHeadVS::lmLineHeadVS() :
    m_head_d1(1.0),
    m_head_d2(1.0),
    m_head_d3(1.0),
    m_head_d4(0.0),
    m_tail_d1(1.0),
    m_tail_d2(1.0),
    m_tail_d3(1.0),
    m_tail_d4(0.0),
    m_head_type(type_none),
    m_tail_type(type_none),
    m_curr_id(0),
    m_curr_coord(0)
{
}

void lmLineHeadVS::rewind(unsigned path_id)
{
    m_status = stop;
    m_curr_id = path_id;
    m_curr_coord = 0;

    if(path_id == 0)
    {
        switch(m_head_type)
        {
        case type_none:
            m_cmd[0] = agg::path_cmd_stop;
            return;

        case type_arrowhead_diamond:
            m_status = arrow;
            m_coord[0]  = -m_head_d1;            m_coord[1]  = 0.0;
            m_coord[2]  = m_head_d2 + m_head_d4; m_coord[3]  = -m_head_d3;
            m_coord[4]  = m_head_d2;             m_coord[5]  = 0.0;
            m_coord[6]  = m_head_d2 + m_head_d4; m_coord[7]  = m_head_d3;

            m_cmd[0] = agg::path_cmd_move_to;
            m_cmd[1] = agg::path_cmd_line_to;
            m_cmd[2] = agg::path_cmd_line_to;
            m_cmd[3] = agg::path_cmd_line_to;
            m_cmd[4] = agg::path_cmd_end_poly | agg::path_flags_close | agg::path_flags_ccw;
            m_cmd[5] = agg::path_cmd_stop;
            return;

        case type_circle:
            m_radius = m_head_d1;
            m_status = circle_start;
            return;
        }
        return;
    }

    if(path_id == 1)
    {
        switch(m_tail_type)
        {
        case type_none:
            m_cmd[0] = agg::path_cmd_stop;
            return;

        case type_arrowhead_diamond:
			m_status = arrow;
            m_coord[0]  = -m_tail_d1;            m_coord[1]  = 0.0;
            m_coord[2]  = m_tail_d2 + m_tail_d4; m_coord[3]  = -m_tail_d3;
            m_coord[4]  = m_tail_d2;             m_coord[5]  = 0.0;
            m_coord[6]  = m_tail_d2 + m_tail_d4; m_coord[7]  = m_tail_d3;

            m_cmd[0] = agg::path_cmd_move_to;
            m_cmd[1] = agg::path_cmd_line_to;
            m_cmd[2] = agg::path_cmd_line_to;
            m_cmd[3] = agg::path_cmd_line_to;
            m_cmd[4] = agg::path_cmd_end_poly | agg::path_flags_close | agg::path_flags_ccw;
            m_cmd[5] = agg::path_cmd_stop;
			return;

        case type_arrowtail:
			m_status = arrow;
			m_coord[0]  =  m_tail_d1;             m_coord[1]  =  0.0;
			m_coord[2]  =  m_tail_d1 - m_tail_d4; m_coord[3]  =  m_tail_d3;
			m_coord[4]  = -m_tail_d2 - m_tail_d4; m_coord[5]  =  m_tail_d3;
			m_coord[6]  = -m_tail_d2;             m_coord[7]  =  0.0;
			m_coord[8]  = -m_tail_d2 - m_tail_d4; m_coord[9]  = -m_tail_d3;
			m_coord[10] =  m_tail_d1 - m_tail_d4; m_coord[11] = -m_tail_d3;

			m_cmd[0] = agg::path_cmd_move_to;
			m_cmd[1] = agg::path_cmd_line_to;
			m_cmd[2] = agg::path_cmd_line_to;
			m_cmd[3] = agg::path_cmd_line_to;
			m_cmd[4] = agg::path_cmd_line_to;
			m_cmd[5] = agg::path_cmd_line_to;
			m_cmd[7] = agg::path_cmd_end_poly | agg::path_flags_close | agg::path_flags_ccw;
			m_cmd[6] = agg::path_cmd_stop;
			return;

        case type_circle:
            m_radius = m_tail_d1;
            m_status = circle_start;
            return;
        }
        return;
    }
}

unsigned lmLineHeadVS::vertex(double* x, double* y)
{
    unsigned cmd = agg::path_cmd_stop;
    switch(m_status)
    {
    case circle_start:
        m_circle.init(0.0, 0.0, m_radius, m_radius);
        m_circle.rewind(0);
        m_status = circle_point;

    case circle_point:
        cmd = m_circle.vertex(x, y);
        if(agg::is_stop(cmd)) m_status = stop;
        else return cmd;

    case arrow:
        if(m_curr_id < 2)
        {
            unsigned curr_idx = m_curr_coord * 2;
            *x = m_coord[curr_idx];
            *y = m_coord[curr_idx + 1];
            return m_cmd[m_curr_coord++];
        }
        return agg::path_cmd_stop;

    case stop:
    	return agg::path_cmd_stop;
    }

    return cmd;
}



//-----------------------------------------------------------------------------------------------
// lmAggLineHeadConv is a conversion pipeline to add a stroke and a line head/tail. Internally it
// has three converters:
//      stroke_type [of type agg::conv_stroke] converts the path to the required stroke
//      marker_type [of type agg::conv_marker, lmLineHeadVS>] adds an arrow head to the line.
//      concat_type [of type agg::conv_concat] concats both converters
//-----------------------------------------------------------------------------------------------
template<class Source> struct lmAggLineHeadConv
{
    typedef agg::conv_stroke<Source, agg::vcgen_markers_term>                   stroke_type;
    typedef agg::conv_marker<typename stroke_type::marker_type, lmLineHeadVS>   marker_type;
    typedef agg::conv_concat<stroke_type, marker_type>                          concat_type;

    stroke_type    s;
    lmLineHeadVS   ah;
    marker_type    m;
    concat_type    c;

    lmAggLineHeadConv(Source& src, double w, lmELineCap nStartCap, lmELineCap nEndCap) :
        s(src),
        ah(),
        m(s.markers(), ah),
        c(s, m)
    {
        s.width(w);

        double k = ::pow(w, 0.7);
        switch(nStartCap)
        {
            case lm_eLineCap_None:
                break;

            case lm_eLineCap_Arrowhead:
                ah.head_arrowhead(0.0, 8*k, 3*k, 2*k);
                break;

            case lm_eLineCap_Diamond:
                ah.head_arrowhead(6*k, 6*k, 6*k, -6*k);
                break;

            case lm_eLineCap_Circle:
                ah.head_circle(4.0*k);
                break;
        }

        switch(nEndCap)
        {
            case lm_eLineCap_None:
                break;

            case lm_eLineCap_Arrowhead:
                ah.tail_arrowhead(0.0, 8*k, 3*k, 2*k);
                break;

            case lm_eLineCap_Arrowtail:
                ah.tail_arrowtail(0, 2.5*k, 3*k, 5*k);
                break;

            case lm_eLineCap_Diamond:
                ah.tail_arrowhead(6*k, 6*k, 6*k, -6*k);
                break;

            case lm_eLineCap_Circle:
                ah.tail_circle(4.0*k);
                break;
        }
        //s.shorten(w * 2.0);       //reduce el tamaño de la linea, recortando el final
    }

    void rewind(unsigned path_id) { c.rewind(path_id); }
    unsigned vertex(double* x, double* y) { return c.vertex(x, y); }
};




//-----------------------------------------------------------------------------------------
// lmAggDrawer implementation
//-----------------------------------------------------------------------------------------


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


lmAggDrawer::lmAggDrawer(int widthPixels, int heightPixels, double rScale, int nStride)
	: lmDrawer((wxDC*)NULL)
{
    // Constructor, allocating a new bitmap of requested size as rendering buffer

    wxASSERT(widthPixels != 0 && heightPixels != 0);
    Initialize(rScale);

    // allocate a new rendering buffer
    m_nBufWidth = widthPixels;
    m_nBufHeight = heightPixels;
    m_buffer = wxImage(widthPixels, heightPixels);

	Create(nStride);
}

lmAggDrawer::lmAggDrawer(wxBitmap* pBitmap, double rScale, int stride)
	: lmDrawer((wxDC*)NULL)
{
	//Constructor, allocating a copy of the received bitmap as rendering buffer

    Initialize(rScale);

    // allocate a copy of the received bitmap as rendering buffer
	m_nBufWidth = pBitmap->GetWidth();
	m_nBufHeight = pBitmap->GetHeight();
    m_buffer = pBitmap->ConvertToImage();

	Create(stride);
}

void lmAggDrawer::Create(int stride)
{
    // allocate a rendering buffer and initialize

    if (stride==0)
        m_nStride = m_nBufWidth * BYTES_PP;
    else
        m_nStride = stride * BYTES_PP;

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
    m_DummyDC.SelectObject(wxNullBitmap);
    delete m_pDummyBitmap;

    delete m_pRenMarker;
    delete m_pRenSolid;
    delete m_pPixelsBuffer;
    delete m_pRenBase;
}

void lmAggDrawer::Initialize(double rScale)
{
    m_vCursorX = 0;
    m_vCursorY = 0;

    //get instance of music font manager
    m_pMFM = lmMusicFontManager::GetInstance();
    m_pMFM->SetScale(rScale);

    ////font defaults (FreeType)
    //m_fValidFont = false;
    //m_rFontHeight = 14;
    //m_rFontWidth = 14;
    m_fKerning = true;
    //m_fHinting = false;     //hinting is pantented by Apple. Can not be used without license
    ////AWARE:
    ////Apple Computer, Inc., owns three patents that are related to the
    ////hinting process of glyph outlines within TrueType fonts. Hinting (also named
    ////grid-fitting) is used to enhance the quality of glyphs at small bitmap sizes.
    ////Therefore, you can not use hinting unless you are authorized (you purchased
    ////a license from Apple, or because you are in a country where the patents do
    ////not apply, etc.). Nevertheless lenmus font doesn't include hinting information
    ////and, so, previous flag value doesn't matter. But its value is important
    ////if I finally use FreeType for all fonts.


    //m_fFlip_y = true;       //TODO: Apparently the behaviour is reversed !!! Investigate this

    //allocate a memory DC with a small bitmap for units conversion
    m_pDummyBitmap = new wxBitmap(1, 1);
    m_DummyDC.SelectObject(*m_pDummyBitmap);
    m_DummyDC.SetMapMode(lmDC_MODE);
    m_DummyDC.SetUserScale(rScale, rScale );
    m_pDC = &m_DummyDC;

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

void lmAggDrawer::Clear()
{
    //Clears the rendering buffer using the current background brush.

    m_pRenBase->clear(m_textColorB);
}

//draw shapes
void lmAggDrawer::SketchLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
                             wxColour color, int style)
{
	//TODO: take style into account

    m_pRenMarker->line_color( lmToRGBA8(color) );
    // renderer_marker expects no decimals, so values must be multiplied by 256.
    m_pRenMarker->line((int)(WorldToDeviceX(x1)*256), int(WorldToDeviceY(y1)*256),
            (int)(WorldToDeviceX(x2)*256), (int)(WorldToDeviceY(y2)*256), true);  //true means 'include last point'

}

void lmAggDrawer::SketchRectangle(lmUPoint uPoint, lmUSize uSize, wxColour color)
{
    // renderer_marker expects no decimals, so values must be multiplied by 256.
    double x = WorldToDeviceX(uPoint.x) * 256.0;
    double y = WorldToDeviceY(uPoint.y) * 256.0;
    double w = WorldToDeviceX((int)(uSize.GetWidth() * 256.0));
    double h = WorldToDeviceY((int)(uSize.GetHeight() * 256.0));

    m_pRenMarker->line_color( lmToRGBA8(color) );
    // in last parameter, true means 'include last point'
    m_pRenMarker->line((int)x, (int)y, (int)(x+w), (int)y, false);
    m_pRenMarker->line((int)(x+w), (int)y, (int)(x+w), (int)(y+h), false);
    m_pRenMarker->line((int)(x+w), (int)(y+h), (int)x, (int)(y+h), false);
    m_pRenMarker->line((int)x, (int)(y+h), (int)x, (int)y, false);

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
    m_ras.reset();
    m_ras.move_to_d( WorldToDeviceX(points[0].x), WorldToDeviceY(points[0].y) );
    int i;
    for (i=1; i < n; i++) {
        m_ras.line_to_d( WorldToDeviceX(points[i].x), WorldToDeviceY(points[i].y) );
    }
    agg::render_scanlines_aa_solid(m_ras, m_sl, *m_pRenBase, lmToRGBA8(color));
}

void lmAggDrawer::SolidShape(lmShape* pShape, wxColor color)
{
    //get vertices from shape
    lmLUnits ux;
    lmLUnits uy;
    unsigned cmd;

    agg::path_storage path;
    agg::conv_curve<agg::path_storage> curve(path);

    path.remove_all();
    unsigned int nPathId = path.start_new_path();

    pShape->RewindVertices(0);      //path_id = 0
    while(!agg::is_stop(cmd = pShape->GetVertex(&ux, &uy)))
    {
        double x1 = WorldToDeviceX(ux);
        double y1 = WorldToDeviceY(uy);
        switch(cmd)
        {
            case agg::path_cmd_move_to:
                path.move_to(x1, y1);
                break;

            case agg::path_cmd_line_to:
                path.line_to(x1, y1);
                break;

            case agg::path_cmd_curve3:
            {
                cmd = pShape->GetVertex(&ux, &uy);
                double x2 = WorldToDeviceX(ux);
                double y2 = WorldToDeviceY(uy);
                path.curve3(x1, y1, x2, y2);
                break;
            }

            default:
                path.end_poly(cmd);
        }
    }

    double x;
    double y;
    m_ras.reset();
    path.rewind(nPathId);
    while(!agg::is_stop(cmd = curve.vertex(&x, &y)))
    {
        m_ras.add_vertex(x, y, cmd);
    }

    agg::render_scanlines_aa_solid(m_ras, m_sl, *m_pRenBase, lmToRGBA8(color));
}

void lmAggDrawer::DecoratedLine(lmUPoint& start, lmUPoint& end, lmLUnits width,
                                lmELineCap nStartCap, lmELineCap nEndCap, wxColor color)
{
    //antialiased line with stroke converter + lmLineHeadVS marker

    m_ras.reset();
    lmAggLineVS line( WorldToDeviceX(start.x), WorldToDeviceY(start.y),
                      WorldToDeviceX(end.x), WorldToDeviceY(end.y) );

    lmAggLineHeadConv<lmAggLineVS> stroke(line, WorldToDeviceY(width), nStartCap, nEndCap);
    m_ras.add_path(stroke);
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
    m_pDC->SetFont(font);
}

void lmAggDrawer::SetLogicalFunction(int function)
{
    m_pDC->SetLogicalFunction(function);
}


//------------------------------------------------------------------------------------------
//text. Platform native renderization
//------------------------------------------------------------------------------------------

void lmAggDrawer::DrawText(const wxString& text, lmLUnits x, lmLUnits y)
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
    wxCoord wD = m_pDC->LogicalToDeviceXRel((int)wL) + 2,
            hD = m_pDC->LogicalToDeviceYRel((int)hL) + 2;

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
    m_pDC->SetTextForeground(colour);       //for texts
    m_textColorF = lmToRGBA8(colour);       //for music font
    m_textColorF.r = colour.Red();
    m_textColorF.g = colour.Green();
    m_textColorF.b = colour.Blue();
    m_textColorF.a = colour.Alpha();
}

void lmAggDrawer::SetTextBackground(const wxColour& colour)
{
    m_pDC->SetTextBackground(colour);       //for texts
    m_textColorB = lmToRGBA8(colour);       //for music font
}

void lmAggDrawer::GetTextExtent(const wxString& string, lmLUnits* w, lmLUnits* h)
{
    wxCoord width, height;
    m_pDC->GetTextExtent(string, &width, &height);
    *w = (lmLUnits)width;
    *h = (lmLUnits)height;
}


//------------------------------------------------------------------------------------------
//text. FreeType with AGG rederization
//------------------------------------------------------------------------------------------

bool lmAggDrawer::FtLoadFont(wxString& sFontName)
{
    //load the requested font. It must be in /bin folder
    //Returns true if any error

    wxASSERT(false);    //Not allowed
    return m_pMFM->LoadFont(sFontName);
}

int lmAggDrawer::FtDrawText(wxString& sText)
{
    //render text (FreeType) at current position, using current settings for font.
    //Returns the number of chars drawn

    if (!m_pMFM->IsFontValid()) return 0;

    //convert text to utf-32
    size_t nLength = sText.Length();
    wxMBConvUTF32 oConv32;
    wxCharBuffer s32Text = sText.mb_str(oConv32);

    return FtDrawText((unsigned int*)s32Text.data(), nLength);
}

int lmAggDrawer::FtDrawChar(unsigned int nChar)
{
    //render char (FreeType) at current position, using current settings for font
    //Returns 0 if error. 1 if ok

    return FtDrawText(&nChar, 1);
}

int lmAggDrawer::FtDrawText(unsigned int* pText, size_t nLength)
{
    //returns the number of chars drawn

    if (!m_pMFM->IsFontValid()) return 0;

    int num_glyphs = 0;

    double x  = m_vCursorX;
    double y  = m_vCursorY;

    const unsigned int* p = pText;

    while(*p && nLength--)
    {
        const agg::glyph_cache* glyph = m_pMFM->GetGlyphCache(*p);
        if(glyph)
        {
            if(m_fKerning)
            {
                m_pMFM->AddKerning(&x, &y);
            }

            m_pMFM->InitAdaptors(glyph, x, y);

            //render the glyph using method agg::glyph_ren_agg_gray8
            m_pRenSolid->color(m_textColorF);               //agg::rgba8(0, 0, 0));
            agg::render_scanlines(m_pMFM->GetGray8AdaptorType(),
                                  m_pMFM->GetGray8ScanlineType(),
                                  *m_pRenSolid);

            // increment pen position
            x += glyph->advance_x;
            ++num_glyphs;
        }
        ++p;
    }
    return num_glyphs;
}

void lmAggDrawer::FtSetTextPosition(lmLUnits uxPos, lmLUnits uyPos)
{
    m_vCursorX = WorldToDeviceX(uxPos);
    m_vCursorY = WorldToDeviceY(uyPos);
}

void lmAggDrawer::FtSetTextPositionPixels(lmPixels vxPos, lmPixels vyPos)
{
    m_vCursorX = (double)vxPos;
    m_vCursorY = (double)vyPos;
}

wxRect lmAggDrawer::FtGetGlyphBoundsInPixels(unsigned int nGlyph)
{
    //returns glyph bounding box. In pixels

    wxASSERT(nGlyph);

    wxRect boxRect;
    if (!m_pMFM->IsFontValid()) return boxRect;

    const agg::glyph_cache* glyph = m_pMFM->GetGlyphCache(nGlyph);
    if(glyph)
    {
        //m_pMFM->InitAdaptors(glyph, x, y);
        agg::rect_i bbox = glyph->bounds;        //rect_i is a rectangle with integer values
        boxRect.x = bbox.x1;
        boxRect.y = bbox.y1;
        boxRect.width = bbox.x2-bbox.x1;
        boxRect.height = bbox.y2-bbox.y1;
    }
    return boxRect;
}

lmURect lmAggDrawer::FtGetGlyphBounds(unsigned int nGlyph)
{
    //returns glyph bounding box. In lmLUnits

    wxRect vBox = FtGetGlyphBoundsInPixels(nGlyph);
    return lmURect(DeviceToLogicalX(vBox.x), DeviceToLogicalY(vBox.y),
                   DeviceToLogicalX(vBox.width), DeviceToLogicalY(vBox.height) );
}

void lmAggDrawer::FtGetTextExtent(const wxString& sText,
                                         lmLUnits* pWidth, lmLUnits* pHeight,
                                         lmLUnits* pDescender, lmLUnits* pAscender)
{
    //Gets the dimensions of the string using the currently selected font.
    //Parameters:
    //  sText is the text string to measure,
    //  descent is the dimension from the baseline of the font to the bottom of
    //          the descender,
    //  externalLeading is any extra vertical space added to the font by the
    //          font designer (usually is zero).
    //
    //The text extent is returned in w and h pointers.
    //
    //The currently selected font is used to compute dimensions.
    //Note that this function only works with single-line strings.

    //convert text to utf-32
    size_t nLength = sText.Length();
    wxMBConvUTF32 oConv32;
    wxCharBuffer s32Text = sText.mb_str(oConv32);

    double x  = 0.0;
    double y  = m_pMFM->GetFontHeight();
    const unsigned int* p = (unsigned int*)s32Text.data();

    while(*p && nLength--)
    {
        const agg::glyph_cache* glyph = m_pMFM->GetGlyphCache(*p);
        if(glyph)
        {
            if(m_fKerning)
                m_pMFM->AddKerning(&x, &y);

            x += glyph->advance_x;
        }
        ++p;
    }

    //return results
    *pWidth = DeviceToWorldX(x);
    *pHeight = DeviceToWorldY(y);

    if (pAscender)
        *pAscender = DeviceToWorldY(m_pMFM->GetAscender());

    if (pDescender)
        *pDescender = DeviceToWorldY(m_pMFM->GetDescender());
}


//------------------------------------------------------------------------------------------
// units conversion
//------------------------------------------------------------------------------------------

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
    return m_pDC->LogicalToDeviceXRel((int)x);
}

lmPixels lmAggDrawer::LogicalToDeviceY(lmLUnits y)
{
    return m_pDC->LogicalToDeviceYRel((int)y);
}


lmColor_rgba8 lmAggDrawer::lmToRGBA8(wxColour color)
{
    return agg::rgba8(color.Red(), color.Green(), color.Blue(), color.Alpha());
}

wxColour lmAggDrawer::lmToWxColor(lmColor_rgba8 color)
{
    return wxColor(color.r, color.g, color.b, color.a);
}

