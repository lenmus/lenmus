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

#ifndef __LM_COLORSCHEME_H__
#define __LM_COLORSCHEME_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ColorScheme.cpp"
#endif


class lmColorScheme
{
public:
	lmColorScheme(wxColour Normal);
	lmColorScheme();
	~lmColorScheme() {}

	//construction
	void SetBaseColor(wxColour normalColor);

	//access
	inline wxColour PrettyDark() { return m_prettyDark; }
	inline wxColour Dark() { return m_dark; }
	inline wxColour LightDark() { return m_lightDark; }
	inline wxColour Normal() { return m_normal; }
	inline wxColour LightBright() { return m_lightBright; }
	inline wxColour Bright() { return m_bright; }
	inline wxColour PrettyBright() { return m_prettyBright; }

private:
	wxColour ChangeLuminance(wxColour C, float luminance);
	wxColour ChangeSaturation(wxColour C, float saturation);
	wxColour ChangeValue(wxColour C, float value);
	void RGB_To_HSL(wxColour C, float& H, float& S, float& L);
	wxColour HSL_To_RGB(float H, float S, float L);
	void RGB_To_HSV(wxColour C, float& H, float& S, float& V);
	wxColour HSV_To_RGB(float H, float S, float V);

	//the colors that form the scheme
	wxColour			m_prettyDark;
	wxColour			m_dark;
	wxColour			m_lightDark;
	wxColour			m_normal;
	wxColour			m_lightBright;
	wxColour			m_bright;
	wxColour			m_prettyBright;

};

#endif	// __LM_COLORSCHEME_H__

