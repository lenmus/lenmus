//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "ColorScheme.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/colour.h"
#include "wx/settings.h"

#include "ColorScheme.h"


lmColorScheme::lmColorScheme(wxColour Normal)
{
	SetBaseColor(Normal);
}

lmColorScheme::lmColorScheme()
{
	SetBaseColor( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );
}

void lmColorScheme::SetBaseColor(wxColour color)
{
#if 0	//1 to use saturation/value (HSV model), 0 to use luminance (HSL model)
	float h, s, v;
	RGB_To_HSV(color, h, s, v);

	m_prettyDark = HSV_To_RGB(h, s, v * 0.25f);
	m_dark = HSV_To_RGB(h, s, v * 0.5f);
	m_lightDark = HSV_To_RGB(h, s, v * 0.75f);
	m_normal = color;		//saturation = 1
	m_lightBright = HSV_To_RGB(h, s * 0.75f, v);
	m_bright = HSV_To_RGB(h, s * 0.5f, v);
	m_prettyBright = HSV_To_RGB(h, s * 0.25f, v);
#else
	float h, s, l;
	RGB_To_HSL(color, h, s, l);

	float Lup = (1.0f - l)/4.0f;

	m_prettyDark = HSL_To_RGB(h, s, l * 0.25f);
	m_dark = HSL_To_RGB(h, s, l * 0.5f);
	m_lightDark = HSL_To_RGB(h, s, l * 0.75f);
	m_normal = color;
	m_lightBright = HSL_To_RGB(h, s, l+Lup);
	m_bright = HSL_To_RGB(h, s, l+Lup+Lup);
	m_prettyBright = HSL_To_RGB(h, s, l+Lup+Lup+Lup);
#endif
}

wxColour lmColorScheme::ChangeLuminance(wxColour C, float luminance)
{
	//Returns the received color, but with its luminance changed

	float h, s, l;
	RGB_To_HSL(C, h, s, l);
	l = luminance;
	return HSL_To_RGB(h, s, l);
}

wxColour lmColorScheme::ChangeValue(wxColour C, float value)
{
	//Returns the received color, but with its value changed

	float h, s, v;
	RGB_To_HSV(C, h, s, v);
	v = value;
	return HSV_To_RGB(h, s, v);
}

wxColour lmColorScheme::ChangeSaturation(wxColour C, float saturation)
{
	//Returns the received color, but with its saturation changed

	float h, s, v;
	RGB_To_HSV(C, h, s, v);
	s = saturation;
	return HSV_To_RGB(h, s, v);
}

void lmColorScheme::RGB_To_HSL(wxColour C, float& H, float& S, float& L)
{
	// Conversion from RBG to HSL.
	// H in the range [0, 360), indicating the angle, in degrees of the hue,
	// S and L in the range [0, 1]. 
	// See: http://en.wikipedia.org/wiki/HSL_and_HSV

	float r = (float)C.Red()/255.0f;
	float g = (float)C.Green()/255.0f;
	float b = (float)C.Blue()/255.0f;

	float max = wxMax(r, wxMax(g, b));
	float min = wxMin(r, wxMin(g, b));

	// L computation
	L = (max + min) / 2.0f;

	//S computation
	if (L > 0.5f)
		S = (max-min) / (2.0f * (1.0f - L));
	else if (L > 0.000001f)
		S = (max-min) / (2.0f * L);
	else
		S = 0.0f;

	// H computation
	if (max == min)
		H = 0.0f;
	else if (max == r)
	{
		if (g >= b)
			H = 60.0f * ((g-b)/(max-min));
		else
			H = 60.0f * ((g-b)/(max-min)) + 360.0f;
	}
	else if (max == g)
		H = 60.0f * ((b-r)/(max-min)) + 120.0f;
	else
		H = 60.0f * ((r-g)/(max-min)) + 240.0f;
}

wxColour lmColorScheme::HSL_To_RGB(float H, float S, float L)
{
	// Conversion from HSL to RGB.
	// H in the range [0, 360), indicating the angle, in degrees of the hue,
	// S and L in the range [0, 1]. 
	// See: http://en.wikipedia.org/wiki/HSL_and_HSV

	// compute q
	float q;
	if (L < 0.5f)
		q = L * (1 + S);
	else
		q = L + S - L * S;

	//compute p
	float p = 2 * L - q;

	//compute t components
	float r13 = 1.0f/3.0f;
	float r23 = 2.0f/3.0f;
	float r16 = 1.0f/6.0f;

	float Hk = H / 360.0f;
	float Tr = Hk + r13;
	float Tg = Hk;
	float Tb = Hk - r13;

	if (Tr < 0.0f)
		Tr += 1.0f;
	else if (Tr > 1.0f)
		Tr -= 1.0f;

	if (Tg < 0.0f)
		Tg += 1.0f;
	else if (Tg > 1.0f)
		Tg -= 1.0f;

	if (Tb < 0.0f)
		Tb += 1.0f;
	else if (Tb > 1.0f)
		Tb -= 1.0f;

	//compute RGB components

	//Red
	float r;
	if (Tr < r16)
		r = p + (q - p) * 6.0f * Tr;
	else if (Tr < 0.5f)
		r = q;
	else if (Tr < r23)
		r = p + (q - p) * 6.0f * (r23 - Tr);
	else
		r = p;

	//Green
	float g;
	if (Tg < r16)
		g = p + (q - p) * 6.0f * Tg;
	else if (Tg < 0.5f)
		g = q;
	else if (Tg < r23)
		g = p + (q - p) * 6.0f * (r23 - Tg);
	else
		g = p;

	//Blue
	float b;
	if (Tb < r16)
		b = p + (q - p) * 6.0f * Tb;
	else if (Tb < 0.5f)
		b = q;
	else if (Tb < r23)
		b = p + (q - p) * 6.0f * (r23 - Tb);
	else
		b = p;

	//prepare RBG color
	wxColour color((unsigned char)(r * 255.0f),
				   (unsigned char)(g * 255.0f),
				   (unsigned char)(b * 255.0f) );
	return color;
}

void lmColorScheme::RGB_To_HSV(wxColour C, float& H, float& S, float& V)
{
	// Conversion from RBG to HSV
	// H in the range [0, 360), indicating the angle, in degrees of the hue,
	// S and V in the range [0, 1]. 
	// See: http://en.wikipedia.org/wiki/HSL_and_HSV

	float r = (float)C.Red()/255.0f;
	float g = (float)C.Green()/255.0f;
	float b = (float)C.Blue()/255.0f;

	float max = wxMax(r, wxMax(g, b));
	float min = wxMin(r, wxMin(g, b));

	//H computation
	if (max == min)
		H = 0.0f;
	else if (max == r)
	{
		if (g >= b)
			H = 60.0f * ((g-b)/(max-min));
		else
			H = 60.0f * ((g-b)/(max-min)) + 360.0f;
	}
	else if (max == g)
		H = 60.0f * ((b-r)/(max-min)) + 120.0f;
	else
		H = 60.0f * ((r-g)/(max-min)) + 240.0f;

	// V computation
	V = max;

	//S computation
	if (max > 0.000001f)
		S = (max-min) / max;
	else
		S = 0.0f;

}

wxColour lmColorScheme::HSV_To_RGB(float H, float S, float V)
{
	// Conversion from HSV to RBG
	// H in the range [0, 360), indicating the angle, in degrees of the hue,
	// S and V in the range [0, 1]. 
	// See: http://en.wikipedia.org/wiki/HSL_and_HSV

	int Hi = (int)(H / 60.0f);
	float f = (H / 60.0f) - (float)Hi;
	Hi = Hi % 6;
	unsigned char p = (unsigned char)(255.0f * V * (1 - S));
	unsigned char q = (unsigned char)(255.0f * V * (1 - f * S));
	unsigned char t = (unsigned char)(255.0f * V * (1 - (1 - f) * S));
	unsigned char v = (unsigned char)(255.0f * V);

	wxColour color;
	switch(Hi)
	{
		case 0:
			color.Set(v, t, p);
			return color;
		case 1:
			color.Set(q, v, p);
			return color;
		case 2:
			color.Set(p, v, t);
			return color;
		case 3:
			color.Set(p, q, v);
			return color;
		case 4:
			color.Set(t, p, v);
			return color;
		case 5:
			color.Set(v, t, p);
			return color;
		default:
			color.Set(v, p, q);	//compiler happy
			return color;
	}
}
