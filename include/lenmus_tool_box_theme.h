//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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

#ifndef __LM_COLORSCHEME_H__
#define __LM_COLORSCHEME_H__

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/colour.h>
    #include <wx/pen.h>
    #include <wx/brush.h>
#endif


using namespace std;


namespace lenmus
{


enum lmEColours
{
    //ToolGroup
    lmCOLOUR_GROUP_BORDER_ACTIVE = 0,
    lmCOLOUR_GROUP_BORDER_INACTIVE,
    lmCOLOUR_GROUP_TITLE_ACTIVE,
    lmCOLOUR_GROUP_TITLE_INACTIVE,
    lmCOLOUR_GROUP_BACKGROUND_SELECTED,
    lmCOLOUR_GROUP_BACKGROUND_NORMAL,
};

enum lmEPens
{
    //ToolGroup
    lmPEN_GROUP_BORDER_ACTIVE = 0,
    lmPEN_GROUP_BORDER_INACTIVE,
    lmPEN_GROUP_TITLE_ACTIVE,
    lmPEN_GROUP_TITLE_INACTIVE,
    lmPEN_GROUP_BACKGROUND_SELECTED,
    lmPEN_GROUP_BACKGROUND_NORMAL,
};

enum lmEBrushes
{
    //ToolGroup
    lmBRUSH_GROUP_BORDER_ACTIVE = 0,
    lmBRUSH_GROUP_BORDER_INACTIVE,
    lmBRUSH_GROUP_TITLE_ACTIVE,
    lmBRUSH_GROUP_TITLE_INACTIVE,
    lmBRUSH_GROUP_BACKGROUND_SELECTED,
    lmBRUSH_GROUP_BACKGROUND_NORMAL,
};


class ToolboxTheme
{
public:
	ToolboxTheme(wxColour Normal);
	ToolboxTheme();
	~ToolboxTheme() {}

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

    wxColour GetColour(lmEColours iColor);
    wxPen& GetPen(lmEPens iPen);
    wxBrush& GetBrush(lmEBrushes iBrush);

    //Utilities
    static wxColour ChangeLuminance(wxColour C, float luminance);
	static wxColour ChangeSaturation(wxColour C, float saturation);
	static wxColour ChangeValue(wxColour C, float value);
	static void RGB_To_HSL(wxColour C, float& H, float& S, float& L);
	static wxColour HSL_To_RGB(float H, float S, float L);
	static void RGB_To_HSV(wxColour C, float& H, float& S, float& V);
	static wxColour HSV_To_RGB(float H, float S, float V);
    static wxColour DarkenColour(const wxColour& c, float rPercentage);
    static wxColour LightenColour(const wxColour& c, float rPercentage);

private:

	//the colors that form the scheme
	wxColour			m_prettyDark;
	wxColour			m_dark;
	wxColour			m_lightDark;
	wxColour			m_normal;
	wxColour			m_lightBright;
	wxColour			m_bright;
	wxColour			m_prettyBright;

    //colours
    wxPen       m_GroupBorderActivePen;
    wxPen       m_GroupBorderInactivePen;
    wxColour    m_GroupTitleActive;
    wxColour    m_GroupTitleInactive;
    wxBrush     m_GroupBackgroundSelectedBrush;
    wxBrush     m_GroupBackgroundNormalBrush;
    //wxFont m_caption_font;
    //int m_border_size;
};



}   // namespace lenmus

#endif	// __LM_COLORSCHEME_H__

