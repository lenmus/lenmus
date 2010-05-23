//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
#pragma implementation "UrlAuxCtrol.h"
#endif

// For compilers that support precompilation, includes <wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "UrlAuxCtrol.h"
#include "../../html/HtmlWindow.h"
#include "../../app/ArtProvider.h"


//colors
#include "../../globals/Colors.h"
extern lmColors* g_pColors;

wxImage lmColourizeImage(wxImage& imgSrc, double hueChange, double satChange,
                         double litChange, bool fColourize);

//-------------------------------------------------------------------------------------
// Implementation of lmUrlAuxCtrol

DEFINE_EVENT_TYPE(lmEVT_URL_CLICK)


BEGIN_EVENT_TABLE(lmUrlAuxCtrol, wxPanel)
    EVT_LEFT_DOWN    (lmUrlAuxCtrol::OnClick)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmUrlAuxCtrol, wxPanel)


lmUrlAuxCtrol::lmUrlAuxCtrol(wxWindow* parent, wxWindowID id, double rScale,
                             const wxString& sNormalLabel,
                             const wxString& sBitmap,
                             const wxString& sAltLabel,
                             const wxString& sAltBitmap,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
    : wxPanel(parent, id, pos, size, style)
    , m_sNormalLabel(sNormalLabel)
    , m_sBitmap(sBitmap)
    , m_sAltLabel(sAltLabel)
    , m_sAltBitmap(sAltBitmap)
    , m_pBitmap((wxStaticBitmap*)NULL)
    , m_fNormal(true)
{
    //load bitmaps
    if (m_sBitmap != wxEmptyString)
    {
        m_oEnaBitmap = wxArtProvider::GetBitmap(m_sBitmap, wxART_TOOLBAR, wxSize(16,16));
        wxImage imgDis = m_oEnaBitmap.ConvertToImage();
        imgDis = lmColourizeImage(imgDis, 34.0, 9.0, 70.0, true);
        m_oDisBitmap = wxBitmap(imgDis);
    }
    if (m_sAltBitmap != wxEmptyString)
    {
        m_oAltEnaBitmap = wxArtProvider::GetBitmap(m_sAltBitmap, wxART_TOOLBAR, wxSize(16,16));
        wxImage imgDis = m_oAltEnaBitmap.ConvertToImage();
        imgDis = lmColourizeImage(imgDis, 34.0, 9.0, 70.0, true);
        m_oAltDisBitmap = wxBitmap(imgDis);
        //m_oAltDisBitmap = wxBitmap(imgDis.ConvertToGreyscale(0.299, 0.587, 0.114));
    }

    //create control
    CreateCtrol();

    //font and other initializations
    wxFont font = parent->GetFont();
    font.SetUnderlined(true);
    double rCurSize = (double)font.GetPointSize();
    font.SetPointSize( (int)(rCurSize * rScale) );
    m_pAnchor->SetFont(font);
    m_pAnchor->SetForegroundColour(g_pColors->HtmlLinks());
    SetCursor(wxCURSOR_HAND);

    //connect click events
    m_pAnchor->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(lmUrlAuxCtrol::OnClick),
                       NULL, this);
    if (m_pBitmap)
        m_pBitmap->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(lmUrlAuxCtrol::OnClick),
                        NULL, this);

    SetBackgroundColour(*wxWHITE);
}

void lmUrlAuxCtrol::CreateCtrol()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxHORIZONTAL );

    if (m_sBitmap != wxEmptyString)
    {
	    m_pBitmap = new wxStaticBitmap( this, wxID_ANY, m_oEnaBitmap,
                                wxDefaultPosition, wxSize( 16,16 ), 0 );
	    pMainSizer->Add( m_pBitmap, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );
    }

	m_pAnchor = new wxStaticText( this, wxID_ANY, m_sNormalLabel, wxDefaultPosition, wxDefaultSize, 0 );
	m_pAnchor->Wrap( -1 );
	pMainSizer->Add( m_pAnchor, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
	pMainSizer->Fit( this );
}

void lmUrlAuxCtrol::OnClick(wxMouseEvent& event)
{
    //wxLogMessage(_T("[lmUrlAuxCtrol::OnClick]"));
    wxCommandEvent eventCustom(lmEVT_URL_CLICK);
    eventCustom.SetId(this->GetId());
    GetParent()->GetEventHandler()->ProcessEvent( eventCustom );
}

void lmUrlAuxCtrol::SetNormalLabel(bool fNormal)
{
    //change anchor string
    m_fNormal = fNormal;
    m_pAnchor->SetLabel(fNormal ? m_sNormalLabel : m_sAltLabel);
    m_pAnchor->Refresh();

    //change bitmap
    if (m_pBitmap)
    {
        bool fEnable = this->IsEnabled();
        if (fNormal)
            m_pBitmap->SetBitmap( (fEnable ? m_oEnaBitmap : m_oDisBitmap));
        else
            m_pBitmap->SetBitmap( (fEnable ? m_oAltEnaBitmap : m_oAltDisBitmap));
	    this->Layout();
    }
}

bool lmUrlAuxCtrol::Enable(bool fEnable)
{
    //change bitmap
    if (m_pBitmap)
    {
        if (m_fNormal)
            m_pBitmap->SetBitmap( (fEnable ? m_oEnaBitmap : m_oDisBitmap));
        else
            m_pBitmap->SetBitmap( (fEnable ? m_oAltEnaBitmap : m_oAltDisBitmap));
	    this->Layout();
    }

    return wxPanel::Enable(fEnable);
}

void lmUrlAuxCtrol::ChangeNormalLabel(const wxString& sLabel)
{
    m_sNormalLabel = sLabel;
    SetNormalLabel(true);
}


// Stores HSL values
struct HSLValue
{
    double hue, saturation, lightness;
};

// Convert RGB to HSL (wxWidgets handles the RGB <-> HSV conversions)
// (based on code at http://www.easyrgb.com/math.html)
HSLValue RGBtoHSL(const wxImage::RGBValue &rgbc) {
        HSLValue hsl;

        double rgb[3];
        rgb[0] = (double)rgbc.red / 255.0;
        rgb[1] = (double)rgbc.green / 255.0;
        rgb[2] = (double)rgbc.blue / 255.0;

        double min = wxMin(rgb[0], wxMin(rgb[1], rgb[2]));
        double max = wxMax(rgb[0], wxMax(rgb[1], rgb[2]));
        double delta = max - min;

        hsl.lightness = (max + min) / 2.0;
        if(delta == 0) { // Grey
                hsl.hue = -1.0;
                hsl.saturation = 0;
                return hsl;
        }

        if(hsl.lightness <= 0.5) hsl.saturation = delta / (max + min);
        else                    hsl.saturation = delta / (2.0 - max - min);

        double dr = (((max - rgb[0]) / 6.0) + (delta / 2.0)) / delta;
        double dg = (((max - rgb[1]) / 6.0) + (delta / 2.0)) / delta;
        double db = (((max - rgb[2]) / 6.0) + (delta / 2.0)) / delta;

        if     (rgb[0] == max) hsl.hue = db - dg;
        else if(rgb[1] == max) hsl.hue = (1.0/3.0) + dr - db;
        else if(rgb[2] == max) hsl.hue = (2.0/3.0) + dg - dr;

        if(hsl.hue < 0.0) hsl.hue += 1.0;
        if(hsl.hue > 1.0) hsl.hue -= 1.0;

        return hsl;
}


// Helper function
double Hue2RGB(double v1, double v2, double H) {
        if(H < 0.0) H += 1.0;
        if(H > 1.0) H -= 1.0;
        if((6.0*H) < 1.0) return (v1 + (v2 - v1) * 6.0 * H);
        if((2.0*H) < 1.0) return v2;
        if((3.0*H) < 2.0) return (v1 + (v2 - v1) * ((2.0/3.0) - H) * 6.0);
        return v1;
}

// Convert HSL to RGB (wxWidgets handles the HSV <-> RGB conversions)
// (based on code at http://www.easyrgb.com/math.html)
wxImage::RGBValue HSLtoRGB(const HSLValue &hsl) {
        wxImage::RGBValue rgb;

        if(hsl.saturation == 0) {
                rgb.red = (int)(hsl.lightness * 255.0);
                rgb.green = rgb.red;
                rgb.blue = rgb.red;
                return rgb;
        }

        double v2;
        if(hsl.lightness < 0.5) v2 = hsl.lightness * (1.0 + hsl.saturation);
        else                    v2 = (hsl.lightness + hsl.saturation) - (hsl.saturation * hsl.lightness);

        double v1 = 2.0 * hsl.lightness - v2;

        rgb.red =   (int)(255.0 * Hue2RGB(v1, v2, hsl.hue + (1.0/3.0)) + 0.5);
        rgb.green = (int)(255.0 * Hue2RGB(v1, v2, hsl.hue) + 0.5);
        rgb.blue =  (int)(255.0 * Hue2RGB(v1, v2, hsl.hue - (1.0/3.0)) + 0.5);

        return rgb;
}

void lmModulateColour(double hueChange, double satChange, double litChange,
                 wxImage::RGBValue& rgb, bool fColourize = false, bool useHSL = true)
{
    //Modulates the hue, saturation and lightness of a color
    //Colorize =       true            false
    //hueChange =    0 .. 360       -180 .. 180
    //satChange =    0 .. 100       -100 .. 100
    //litChange = -100 .. 100       -100 .. 100
    //If fColourize == true, original hue is replaced by new one
    //(based on code from Graphic Gems)
    double h, s, l;

    if(useHSL)
    {
        // Convert to HSL
        HSLValue hsl = RGBtoHSL(rgb);
        h = hsl.hue;
        s = hsl.saturation;
        l = hsl.lightness;
    }
    else
    {
        // Convert to HSV
        wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
        h = hsv.hue;
        s = hsv.saturation;
        l = hsv.value;
    }

    l += litChange/100.0;
    if(l < 0.0) l = 0.0;
    else if(l > 1.0) l = 1.0;

    s += satChange/100.0;
    if(s < 0.0) s = 0.0;
    else if(s > 1.0) s = 1.0;

    if (fColourize)
    {
        h = hueChange/100.0;
    }
    else
    {
        if(h != -1.0)
        {
            h += hueChange/100.0;
            if(h < 0.0) h += 1.0;
            else if(h > 1.0) h -= 1.0;
        }
    }

    // Convert back to RGB
    if(useHSL)
    {
        HSLValue hsl;
        hsl.hue = h; hsl.lightness = l; hsl.saturation = s;
        rgb = HSLtoRGB(hsl);
    }
    else
    {
        wxImage::HSVValue hsv;
        hsv.hue = h; hsv.value = l; hsv.saturation = s;
        rgb = wxImage::HSVtoRGB(hsv);
    }
}

void lmColourize(double newHue, double newSat, double litChange,
                 wxImage::RGBValue& rgb, bool useHSL = true)
{
    //Changes the hue, saturation and lightness of a color. This is
    //similar to Photoshop Hue/Saturation adjustement layer with
    //colourize=yes
    //
    //newHue =       0 .. 360
    //neweSat =      0 .. 100
    //litChange = -100 .. 100

    double h, s, l;

    if(useHSL)
    {
        // Convert to HSL
        HSLValue hsl = RGBtoHSL(rgb);
        h = hsl.hue;
        s = hsl.saturation;
        l = hsl.lightness;
    }
    else
    {
        // Convert to HSV
        wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
        h = hsv.hue;
        s = hsv.saturation;
        l = hsv.value;
    }

    if (litChange > 0.0)
        l = litChange/100.0 * (1.0 - l) + l;
    else if (litChange < 0.0)
        l *= (1.0 + litChange/100.0);
    // else
        // litChange==0 --> No change

    if(l < 0.0) l = 0.0;
    else if(l > 1.0) l = 1.0;

    s = newSat/100.0;
    if(s < 0.0) s = 0.0;
    else if(s > 1.0) s = 1.0;

    h = newHue/360.0;
    if(h < 0.0) h = 0.0;
    else if(h > 1.0) h = 1.0;

    // Convert back to RGB
    if(useHSL)
    {
        HSLValue hsl;
        hsl.hue = h; hsl.lightness = l; hsl.saturation = s;
        rgb = HSLtoRGB(hsl);
    }
    else
    {
        wxImage::HSVValue hsv;
        hsv.hue = h; hsv.value = l; hsv.saturation = s;
        rgb = wxImage::HSVtoRGB(hsv);
    }
}

wxImage lmColourizeImage(wxImage& imgSrc, double hueChange, double satChange,
                         double litChange, bool fColourize)
{
    //Changes hue of each pixel of the image, and adjust saturation and lightness
    //as desired.
    //newHue is a doblue in the range 0.0..1.0 where 0.0 is 0 degrees and
    //1.0 is 360 degrees

    wxImage imgDest;

    wxCHECK_MSG( imgSrc.Ok(), imgDest, wxT("invalid imgDest") );

    imgDest.Create(imgSrc.GetWidth(), imgSrc.GetHeight(), false);

    unsigned char *dest = imgDest.GetData();

    wxCHECK_MSG( dest, imgDest, wxT("unable to create imgDest") );

    unsigned char *src = imgSrc.GetData();
    bool hasMask = imgSrc.HasMask();
    unsigned char maskRed = imgSrc.GetMaskRed();
    unsigned char maskGreen = imgSrc.GetMaskGreen();
    unsigned char maskBlue = imgSrc.GetMaskBlue();

    if ( hasMask )
        imgDest.SetMaskColour(maskRed, maskGreen, maskBlue);

    wxImage::HSVValue hsv;
    wxImage::RGBValue rgb;
    const long size = imgSrc.GetWidth() * imgSrc.GetHeight();
    for ( long i = 0; i < size; i++, src += 3, dest += 3 )
    {
        // don't modify the mask
        if ( hasMask && src[0] == maskRed && src[1] == maskGreen && src[2] == maskBlue )
        {
            memcpy(dest, src, 3);
        }
        else
        {
#if 0
            // modify imgDest
            rgb.red = src[0];
            rgb.green = src[1];
            rgb.blue = src[2];
            hsv = wxImage::RGBtoHSV(rgb);

            hsv.hue = newHue;
            //if (hsv.hue > 1.0)
            //    hsv.hue = hsv.hue - 1.0;
            //else if (hsv.hue < 0.0)
            //    hsv.hue = hsv.hue + 1.0;

            rgb = wxImage::HSVtoRGB(hsv);
            dest[0] = rgb.red;
            dest[1] = rgb.green;
            dest[2] = rgb.blue;
#endif
            rgb.red = src[0];
            rgb.green = src[1];
            rgb.blue = src[2];
            //lmColourChange(hueChange, satChange, litChange, rgb, fColourize, true);
            lmColourize(hueChange, satChange, litChange, rgb, true);
            dest[0] = rgb.red;
            dest[1] = rgb.green;
            dest[2] = rgb.blue;
        }
    }

    // copy the alpha channel, if any
    if (imgSrc.HasAlpha())
    {
        const size_t alphaSize = imgSrc.GetWidth() * imgSrc.GetHeight();
        unsigned char *alpha = (unsigned char*)malloc(alphaSize);
        memcpy(alpha, imgSrc.GetAlpha(), alphaSize);
        imgDest.InitAlpha();
        imgDest.SetAlpha(alpha);
    }

    return imgDest;
}



