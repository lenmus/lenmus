//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

#include "lenmus_url_aux_ctrol.h"
#include "lenmus_art_provider.h"
#include "lenmus_colors.h"

//wxWidgets
#include <wx/wxprec.h>


//TODO 5.0 commented out
//wxImage lmColourizeImage(wxImage& imgSrc, double hueChange, double satChange,
//                         double litChange, bool fColourize);


namespace lenmus
{

// forward declarations
class ImoBoxContainer;
class DocumentCanvas;


//-------------------------------------------------------------------------------------
// Implementation of UrlAuxCtrol

//---------------------------------------------------------------------------------------
UrlAuxCtrol::UrlAuxCtrol(EventHandler* parentCtrol, int eventType, ImoParagraph* pPara,
                         Document* pDoc, const string& url, const string& sNormalLabel,
                         LUnits linkWidth, const wxString& sBitmap,
                         const string& sAltLabel, const wxString& sAltBitmap)
    : m_pPara(pPara)
    , m_pDoc(pDoc)
    , m_sNormalLabel(sNormalLabel)
    , m_sAltLabel(sAltLabel)
    , m_sBitmap(sBitmap)
    , m_sAltBitmap(sAltBitmap)
//    , m_pBitmap((wxStaticBitmap*)NULL)
    , m_fNormal(true)
{
//TODO 5.0 commented out
//    //load bitmaps
//    if (m_sBitmap != wxEmptyString)
//    {
//        m_oEnaBitmap = wxArtProvider::GetBitmap(m_sBitmap, wxART_TOOLBAR, wxSize(16,16));
//        wxImage imgDis = m_oEnaBitmap.ConvertToImage();
//        imgDis = lmColourizeImage(imgDis, 34.0, 9.0, 70.0, true);
//        m_oDisBitmap = wxBitmap(imgDis);
//    }
//    if (m_sAltBitmap != wxEmptyString)
//    {
//        m_oAltEnaBitmap = wxArtProvider::GetBitmap(m_sAltBitmap, wxART_TOOLBAR, wxSize(16,16));
//        wxImage imgDis = m_oAltEnaBitmap.ConvertToImage();
//        imgDis = lmColourizeImage(imgDis, 34.0, 9.0, 70.0, true);
//        m_oAltDisBitmap = wxBitmap(imgDis);
//        //m_oAltDisBitmap = wxBitmap(imgDis.ConvertToGreyscale(0.299, 0.587, 0.114));
//    }

    //create style
    ImoStyle* pLinkStyle = m_pDoc->create_private_style("Default style");
    pLinkStyle->color( Color(0,0,255) );
    pLinkStyle->text_decoration(ImoTextStyle::k_decoration_underline);
//    SetCursor(wxCURSOR_HAND);

    if (linkWidth > 0.0f)
    {
        ImoStyle* pDefStyle = m_pDoc->find_style("Default style");
        ImoInlineWrapper* pBox = pPara->add_inline_box(linkWidth, pDefStyle);
        m_pLink = pBox->add_link(url, pLinkStyle);
    }
    else
        m_pLink = m_pPara->add_link(url, pLinkStyle);

    m_pLink->add_text_item(sNormalLabel, pLinkStyle);
    m_pLink->add_event_handler(eventType, parentCtrol);
}

//---------------------------------------------------------------------------------------
void UrlAuxCtrol::set_normal_label(bool fNormal)
{
//TODO 5.0 commented out
//    //change anchor string
//    m_fNormal = fNormal;
//    m_pLink->SetLabel(fNormal ? m_sNormalLabel : m_sAltLabel);
//    m_pLink->Refresh();
//
//    //change bitmap
//    if (m_pBitmap)
//    {
//        bool fEnable = this->IsEnabled();
//        if (fNormal)
//            m_pBitmap->SetBitmap( (fEnable ? m_oEnaBitmap : m_oDisBitmap));
//        else
//            m_pBitmap->SetBitmap( (fEnable ? m_oAltEnaBitmap : m_oAltDisBitmap));
//	    this->Layout();
//    }
}

//---------------------------------------------------------------------------------------
void UrlAuxCtrol::enable(bool fEnable)
{
//TODO 5.0 commented out
//    //change bitmap
//    if (m_pBitmap)
//    {
//        if (m_fNormal)
//            m_pBitmap->SetBitmap( (fEnable ? m_oEnaBitmap : m_oDisBitmap));
//        else
//            m_pBitmap->SetBitmap( (fEnable ? m_oAltEnaBitmap : m_oAltDisBitmap));
//	    this->Layout();
//    }
//
//    return wxPanel::Enable(fEnable);
}

//---------------------------------------------------------------------------------------
void UrlAuxCtrol::replace_normal_label(const wxString& sLabel)
{
//TODO 5.0 commented out
//    m_sNormalLabel = sLabel;
//    set_normal_label(true);
}


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//TODO 5.0 commented out

//// Stores HSL values
//struct HSLValue
//{
//    double hue, saturation, lightness;
//};
//
//// Convert RGB to HSL (wxWidgets handles the RGB <-> HSV conversions)
//// (based on code at http://www.easyrgb.com/math.html)
//HSLValue RGBtoHSL(const wxImage::RGBValue &rgbc) {
//        HSLValue hsl;
//
//        double rgb[3];
//        rgb[0] = (double)rgbc.red / 255.0;
//        rgb[1] = (double)rgbc.green / 255.0;
//        rgb[2] = (double)rgbc.blue / 255.0;
//
//        double min = wxMin(rgb[0], wxMin(rgb[1], rgb[2]));
//        double max = wxMax(rgb[0], wxMax(rgb[1], rgb[2]));
//        double delta = max - min;
//
//        hsl.lightness = (max + min) / 2.0;
//        if(delta == 0) { // Grey
//                hsl.hue = -1.0;
//                hsl.saturation = 0;
//                return hsl;
//        }
//
//        if(hsl.lightness <= 0.5) hsl.saturation = delta / (max + min);
//        else                    hsl.saturation = delta / (2.0 - max - min);
//
//        double dr = (((max - rgb[0]) / 6.0) + (delta / 2.0)) / delta;
//        double dg = (((max - rgb[1]) / 6.0) + (delta / 2.0)) / delta;
//        double db = (((max - rgb[2]) / 6.0) + (delta / 2.0)) / delta;
//
//        if     (rgb[0] == max) hsl.hue = db - dg;
//        else if(rgb[1] == max) hsl.hue = (1.0/3.0) + dr - db;
//        else if(rgb[2] == max) hsl.hue = (2.0/3.0) + dg - dr;
//
//        if(hsl.hue < 0.0) hsl.hue += 1.0;
//        if(hsl.hue > 1.0) hsl.hue -= 1.0;
//
//        return hsl;
//}
//
//
//// Helper function
//double Hue2RGB(double v1, double v2, double H) {
//        if(H < 0.0) H += 1.0;
//        if(H > 1.0) H -= 1.0;
//        if((6.0*H) < 1.0) return (v1 + (v2 - v1) * 6.0 * H);
//        if((2.0*H) < 1.0) return v2;
//        if((3.0*H) < 2.0) return (v1 + (v2 - v1) * ((2.0/3.0) - H) * 6.0);
//        return v1;
//}
//
//// Convert HSL to RGB (wxWidgets handles the HSV <-> RGB conversions)
//// (based on code at http://www.easyrgb.com/math.html)
//wxImage::RGBValue HSLtoRGB(const HSLValue &hsl) {
//        wxImage::RGBValue rgb;
//
//        if(hsl.saturation == 0) {
//                rgb.red = (int)(hsl.lightness * 255.0);
//                rgb.green = rgb.red;
//                rgb.blue = rgb.red;
//                return rgb;
//        }
//
//        double v2;
//        if(hsl.lightness < 0.5) v2 = hsl.lightness * (1.0 + hsl.saturation);
//        else                    v2 = (hsl.lightness + hsl.saturation) - (hsl.saturation * hsl.lightness);
//
//        double v1 = 2.0 * hsl.lightness - v2;
//
//        rgb.red =   (int)(255.0 * Hue2RGB(v1, v2, hsl.hue + (1.0/3.0)) + 0.5);
//        rgb.green = (int)(255.0 * Hue2RGB(v1, v2, hsl.hue) + 0.5);
//        rgb.blue =  (int)(255.0 * Hue2RGB(v1, v2, hsl.hue - (1.0/3.0)) + 0.5);
//
//        return rgb;
//}
//
//void lmModulateColour(double hueChange, double satChange, double litChange,
//                 wxImage::RGBValue& rgb, bool fColourize = false, bool useHSL = true)
//{
//    //Modulates the hue, saturation and lightness of a color
//    //Colorize =       true            false
//    //hueChange =    0 .. 360       -180 .. 180
//    //satChange =    0 .. 100       -100 .. 100
//    //litChange = -100 .. 100       -100 .. 100
//    //If fColourize == true, original hue is replaced by new one
//    //(based on code from Graphic Gems)
//    double h, s, l;
//
//    if(useHSL)
//    {
//        // Convert to HSL
//        HSLValue hsl = RGBtoHSL(rgb);
//        h = hsl.hue;
//        s = hsl.saturation;
//        l = hsl.lightness;
//    }
//    else
//    {
//        // Convert to HSV
//        wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
//        h = hsv.hue;
//        s = hsv.saturation;
//        l = hsv.value;
//    }
//
//    l += litChange/100.0;
//    if(l < 0.0) l = 0.0;
//    else if(l > 1.0) l = 1.0;
//
//    s += satChange/100.0;
//    if(s < 0.0) s = 0.0;
//    else if(s > 1.0) s = 1.0;
//
//    if (fColourize)
//    {
//        h = hueChange/100.0;
//    }
//    else
//    {
//        if(h != -1.0)
//        {
//            h += hueChange/100.0;
//            if(h < 0.0) h += 1.0;
//            else if(h > 1.0) h -= 1.0;
//        }
//    }
//
//    // Convert back to RGB
//    if(useHSL)
//    {
//        HSLValue hsl;
//        hsl.hue = h; hsl.lightness = l; hsl.saturation = s;
//        rgb = HSLtoRGB(hsl);
//    }
//    else
//    {
//        wxImage::HSVValue hsv;
//        hsv.hue = h; hsv.value = l; hsv.saturation = s;
//        rgb = wxImage::HSVtoRGB(hsv);
//    }
//}
//
//void lmColourize(double newHue, double newSat, double litChange,
//                 wxImage::RGBValue& rgb, bool useHSL = true)
//{
//    //Changes the hue, saturation and lightness of a color. This is
//    //similar to Photoshop Hue/Saturation adjustement layer with
//    //colourize=yes
//    //
//    //newHue =       0 .. 360
//    //neweSat =      0 .. 100
//    //litChange = -100 .. 100
//
//    double h, s, l;
//
//    if(useHSL)
//    {
//        // Convert to HSL
//        HSLValue hsl = RGBtoHSL(rgb);
//        h = hsl.hue;
//        s = hsl.saturation;
//        l = hsl.lightness;
//    }
//    else
//    {
//        // Convert to HSV
//        wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
//        h = hsv.hue;
//        s = hsv.saturation;
//        l = hsv.value;
//    }
//
//    if (litChange > 0.0)
//        l = litChange/100.0 * (1.0 - l) + l;
//    else if (litChange < 0.0)
//        l *= (1.0 + litChange/100.0);
//    // else
//        // litChange==0 --> No change
//
//    if(l < 0.0) l = 0.0;
//    else if(l > 1.0) l = 1.0;
//
//    s = newSat/100.0;
//    if(s < 0.0) s = 0.0;
//    else if(s > 1.0) s = 1.0;
//
//    h = newHue/360.0;
//    if(h < 0.0) h = 0.0;
//    else if(h > 1.0) h = 1.0;
//
//    // Convert back to RGB
//    if(useHSL)
//    {
//        HSLValue hsl;
//        hsl.hue = h; hsl.lightness = l; hsl.saturation = s;
//        rgb = HSLtoRGB(hsl);
//    }
//    else
//    {
//        wxImage::HSVValue hsv;
//        hsv.hue = h; hsv.value = l; hsv.saturation = s;
//        rgb = wxImage::HSVtoRGB(hsv);
//    }
//}
//
//wxImage lmColourizeImage(wxImage& imgSrc, double hueChange, double satChange,
//                         double litChange, bool fColourize)
//{
//    //Changes hue of each pixel of the image, and adjust saturation and lightness
//    //as desired.
//    //newHue is a doblue in the range 0.0..1.0 where 0.0 is 0 degrees and
//    //1.0 is 360 degrees
//
//    wxImage imgDest;
//
//    wxCHECK_MSG( imgSrc.Ok(), imgDest, "invalid imgDest" );
//
//    imgDest.Create(imgSrc.GetWidth(), imgSrc.GetHeight(), false);
//
//    unsigned char *dest = imgDest.GetData();
//
//    wxCHECK_MSG( dest, imgDest, "unable to create imgDest" );
//
//    unsigned char *src = imgSrc.GetData();
//    bool hasMask = imgSrc.HasMask();
//    unsigned char maskRed = imgSrc.GetMaskRed();
//    unsigned char maskGreen = imgSrc.GetMaskGreen();
//    unsigned char maskBlue = imgSrc.GetMaskBlue();
//
//    if ( hasMask )
//        imgDest.SetMaskColour(maskRed, maskGreen, maskBlue);
//
//    wxImage::HSVValue hsv;
//    wxImage::RGBValue rgb;
//    const long size = imgSrc.GetWidth() * imgSrc.GetHeight();
//    for ( long i = 0; i < size; i++, src += 3, dest += 3 )
//    {
//        // don't modify the mask
//        if ( hasMask && src[0] == maskRed && src[1] == maskGreen && src[2] == maskBlue )
//        {
//            memcpy(dest, src, 3);
//        }
//        else
//        {
//#if 0
//            // modify imgDest
//            rgb.red = src[0];
//            rgb.green = src[1];
//            rgb.blue = src[2];
//            hsv = wxImage::RGBtoHSV(rgb);
//
//            hsv.hue = newHue;
//            //if (hsv.hue > 1.0)
//            //    hsv.hue = hsv.hue - 1.0;
//            //else if (hsv.hue < 0.0)
//            //    hsv.hue = hsv.hue + 1.0;
//
//            rgb = wxImage::HSVtoRGB(hsv);
//            dest[0] = rgb.red;
//            dest[1] = rgb.green;
//            dest[2] = rgb.blue;
//#endif
//            rgb.red = src[0];
//            rgb.green = src[1];
//            rgb.blue = src[2];
//            //lmColourChange(hueChange, satChange, litChange, rgb, fColourize, true);
//            lmColourize(hueChange, satChange, litChange, rgb, true);
//            dest[0] = rgb.red;
//            dest[1] = rgb.green;
//            dest[2] = rgb.blue;
//        }
//    }
//
//    // copy the alpha channel, if any
//    if (imgSrc.HasAlpha())
//    {
//        const size_t alphaSize = imgSrc.GetWidth() * imgSrc.GetHeight();
//        unsigned char *alpha = (unsigned char*)malloc(alphaSize);
//        memcpy(alpha, imgSrc.GetAlpha(), alphaSize);
//        imgDest.InitAlpha();
//        imgDest.SetAlpha(alpha);
//    }
//
//    return imgDest;
//}


}   //namespace lenmus
