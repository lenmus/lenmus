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
#pragma implementation "ShapeBracket.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "ShapeBracket.h"
#include "../score/Score.h"
#include "agg_basics.h"


lmVertex m_BracketVertices[] = {
    {   0.0, 2051.0, agg::path_cmd_move_to },
    {   0.0, 2036.0, agg::path_cmd_line_to },
    {  84.0, 1975.0, agg::path_cmd_curve3 },      //ctrol
    { 131.0, 1896.0, agg::path_cmd_curve3 },      //on-curve
    { 183.0, 1808.0, agg::path_cmd_curve3 },      //ctrol
    { 183.0, 1703.0, agg::path_cmd_curve3 },      //on-curve
    { 183.0, 1623.0, agg::path_cmd_curve3 },      //ctrol
    { 143.0, 1454.0, agg::path_cmd_curve3 },      //on-curve
    {  75.0, 1164.0, agg::path_cmd_curve3 },      //ctrol
    {  69.0, 1137.0, agg::path_cmd_curve3 },      //on-curve
    {  28.0,  936.0, agg::path_cmd_curve3 },      //ctrol
    {  28.0,  794.0, agg::path_cmd_curve3 },      //on-curve
    {  28.0,  614.0, agg::path_cmd_curve3 },      //ctrol
    {  91.0,  425.0, agg::path_cmd_curve3 },      //on-curve
    { 163.0,  210.0, agg::path_cmd_curve3 },      //ctrol
    { 311.0,    0.0, agg::path_cmd_curve3 },      //on-curve
    { 311.0,   15.0, agg::path_cmd_line_to },
    { 247.0,  133.0, agg::path_cmd_curve3 },      //ctrol
    { 216.0,  204.0, agg::path_cmd_curve3 },      //on-curve
    { 172.0,  300.0, agg::path_cmd_curve3 },      //ctrol
    { 151.0,  396.0, agg::path_cmd_curve3 },      //on-curve
    { 129.0,  489.0, agg::path_cmd_curve3 },      //ctrol
    { 129.0,  596.0, agg::path_cmd_curve3 },      //on-curve
    { 129.0,  724.0, agg::path_cmd_curve3 },      //ctrol
    { 175.0,  910.0, agg::path_cmd_curve3 },      //on-curve
    { 219.0, 1066.0, agg::path_cmd_curve3 },      //ctrol
    { 263.0, 1229.0, agg::path_cmd_curve3 },      //on-curve
    { 307.0, 1389.0, agg::path_cmd_curve3 },      //ctrol
    { 307.0, 1488.0, agg::path_cmd_curve3 },      //on-curve
    { 307.0, 1697.0, agg::path_cmd_curve3 },      //ctrol
    { 237.0, 1820.0, agg::path_cmd_curve3 },      //on-curve
    { 173.0, 1932.0, agg::path_cmd_curve3 },      //ctrol
    {  11.0, 2043.0, agg::path_cmd_curve3 },
    { 176.0, 2165.0, agg::path_cmd_curve3 },      //ctrol
    { 237.0, 2274.0, agg::path_cmd_curve3 },      //on-curve
    { 307.0, 2398.0, agg::path_cmd_curve3 },      //ctrol
    { 307.0, 2609.0, agg::path_cmd_curve3 },      //on-curve
    { 307.0, 2703.0, agg::path_cmd_curve3 },      //ctrol
    { 263.0, 2865.0, agg::path_cmd_curve3 },      //on-curve
    { 218.0, 3030.0, agg::path_cmd_curve3 },      //ctrol
    { 175.0, 3185.0, agg::path_cmd_curve3 },      //on-curve
    { 129.0, 3371.0, agg::path_cmd_curve3 },      //ctrol
    { 129.0, 3501.0, agg::path_cmd_curve3 },      //on-curve
    { 129.0, 3658.0, agg::path_cmd_curve3 },      //ctrol
    { 177.0, 3802.0, agg::path_cmd_curve3 },      //on-curve
    { 216.0, 3919.0, agg::path_cmd_curve3 },      //ctrol
    { 307.0, 4078.0, agg::path_cmd_curve3 },      //on-curve
    { 307.0, 4094.0, agg::path_cmd_line_to },
    { 162.0, 3894.0, agg::path_cmd_curve3 },      //ctrol
    {  90.0, 3673.0, agg::path_cmd_curve3 },      //on-curve
    {  28.0, 3483.0, agg::path_cmd_curve3 },      //ctrol
    {  28.0, 3301.0, agg::path_cmd_curve3 },      //on-curve
    {  28.0, 3156.0, agg::path_cmd_curve3 },      //ctrol
    {  68.0, 2960.0, agg::path_cmd_curve3 },      //on-curve
    {  75.0, 2927.0, agg::path_cmd_curve3 },      //ctrol
    { 142.0, 2643.0, agg::path_cmd_curve3 },      //on-curve
    { 183.0, 2470.0, agg::path_cmd_curve3 },      //ctrol
    { 183.0, 2394.0, agg::path_cmd_curve3 },      //on-curve
    { 183.0, 2287.0, agg::path_cmd_curve3 },      //ctrol
    { 132.0, 2199.0, agg::path_cmd_curve3 },      //on-curve
    {  89.0, 2124.0, agg::path_cmd_curve3 },      //ctrol
    {   0.0, 2051.0, agg::path_cmd_curve3 },      //on_curve
    {   0.0,    0.0, agg::path_cmd_end_poly | agg::path_flags_close | agg::path_flags_ccw }, //close polygon
    {   0.0,    0.0, agg::path_cmd_stop }
};

float m_rxMaxBracket = 311.0f;
float m_ryMaxBracket = 4094.0f;
const int m_nNumVerticesBracket = sizeof(m_BracketVertices)/sizeof(lmVertex);


lmVertex m_BraceVertices[] = {
    {   0.0, 327.0, agg::path_cmd_move_to },
    //{ 187.0, 305.0, agg::path_cmd_curve4 },
    //{ 496.0, 235.0, agg::path_cmd_curve4 },
    //{ 561.0, 196.0, agg::path_cmd_curve4 }, //on
    { 273.0, 307.0, agg::path_cmd_curve3 },
    { 561.0, 196.0, agg::path_cmd_curve3 }, //on
    { 641.0, 151.0, agg::path_cmd_curve3 },
    { 696.0, 102.0, agg::path_cmd_curve3 },    //on-curve
    { 739.0,  64.0, agg::path_cmd_curve3 },
    { 791.0,   0.0, agg::path_cmd_curve3 },    //on-curve
    { 836.0,  16.0, agg::path_cmd_line_to },
    { 804.0,  83.0, agg::path_cmd_curve3 },
    { 760.0, 139.0, agg::path_cmd_curve3 },    //on-curve
    { 724.0, 185.0, agg::path_cmd_curve3 },
    { 655.0, 254.0, agg::path_cmd_curve3 },    //on-curve
    { 601.0, 306.0, agg::path_cmd_curve3 },
    { 551.0, 342.0, agg::path_cmd_curve3 },    //on-curve
    { 510.0, 372.0, agg::path_cmd_curve3 },
    { 443.0, 408.0, agg::path_cmd_curve3 },    //on-curve
    { 404.0, 430.0, agg::path_cmd_curve3 },
    { 307.0, 479.0, agg::path_cmd_curve3 },    //on-curve
};

lmVertex m_BraceVertices2[] = {
    { 307.0,  685.0, agg::path_cmd_line_to },
    { 404.0,  734.0, agg::path_cmd_curve3 },
    { 443.0,  756.0, agg::path_cmd_curve3 },    //on-curve
    { 510.0,  792.0, agg::path_cmd_curve3 },
    { 551.0,  822.0, agg::path_cmd_curve3 },    //on-curve
    { 601.0,  858.0, agg::path_cmd_curve3 },
    { 655.0,  910.0, agg::path_cmd_curve3 },    //on-curve
    { 724.0,  979.0, agg::path_cmd_curve3 },
    { 760.0, 1025.0, agg::path_cmd_curve3 },    //on-curve
    { 804.0, 1081.0, agg::path_cmd_curve3 },
    { 836.0, 1148.0, agg::path_cmd_curve3 },    //on-curve
    { 791.0, 1164.0, agg::path_cmd_line_to },
    { 739.0, 1100.0, agg::path_cmd_curve3 },
    { 696.0, 1062.0, agg::path_cmd_curve3 },    //on-curve
    { 641.0, 1102.0, agg::path_cmd_curve3 },
    { 561.0,  968.0, agg::path_cmd_curve3 },    //on-curve
    { 273.0,  857.0, agg::path_cmd_curve3 },
    {   0.0,  885.0, agg::path_cmd_curve3 },    //on-curve
    {   0.0,    0.0, agg::path_cmd_end_poly | agg::path_flags_close | agg::path_flags_ccw }, //close polygon
    {   0.0,    0.0, agg::path_cmd_stop }
};


float m_dxBrace = 479.0f;
float m_dyBrace = 606.0f;
float m_dxBraceBar = 307.0f;
float m_dyStartBrace = 279.0f;
const int m_nNumVerticesBrace = sizeof(m_BraceVertices)/sizeof(lmVertex);
const int m_nNumVerticesBrace2 = sizeof(m_BraceVertices2)/sizeof(lmVertex);


//-------------------------------------------------------------------------------------
// Implementation of lmShapeBracketBrace


lmShapeBracketBrace::lmShapeBracketBrace(lmInstrument* pInstr, lmEBracketSymbol nSymbol,
                                         wxColour color, wxString sName)
	: lmSimpleShape(eGMO_ShapeBracket, pInstr, 0, sName)
{
    m_nSymbol = nSymbol;
	m_color = color;
}

lmShapeBracketBrace::~lmShapeBracketBrace()
{
}

wxString lmShapeBracketBrace::Dump(int nIndent)
{
	//TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("Idx: %d %s: "),
		m_nOwnerIdx, m_sGMOName.c_str() );
    sDump += DumpBounds();
    sDump += _T("\n");

    //base class
    sDump += lmShape::Dump(nIndent);

	return sDump;
}

void lmShapeBracketBrace::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uxLeft += xIncr;
	m_uyTop += yIncr;
    m_uxRight += xIncr;
	m_uyBottom += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);
    SetAffineTransform();

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

void lmShapeBracketBrace::Render(lmPaper* pPaper, wxColour color)
{
    //update selection rectangle
    m_uSelRect = GetBounds();

    m_nContour = 0;
    m_nCurVertex = 0;
    pPaper->SolidShape(this, color);

    lmSimpleShape::Render(pPaper, color);
}


//-------------------------------------------------------------------------------------
// Implementation of lmShapeBracket


lmShapeBracket::lmShapeBracket(lmInstrument* pInstr, lmLUnits xLeft, lmLUnits yTop,
                               lmLUnits xRight, lmLUnits yBottom, wxColour color)
	: lmShapeBracketBrace(pInstr, lm_eBracket, color, _T("bracket"))
{
    m_nType = eGMO_ShapeBracket;

    m_uxLeft = xLeft;
    m_uyTop = yTop;
    m_uxRight = xRight;
    m_uyBottom = yBottom;

	//set bounds
	SetXLeft(xLeft);
	SetYTop(yTop);
	SetXRight(xRight);
	SetYBottom(yBottom);

    //set scaling and translation
    SetAffineTransform();
}

lmShapeBracket::~lmShapeBracket()
{
}

void lmShapeBracket::SetAffineTransform()
{
    //scale and position vertices

    double rxScale((m_uxRight - m_uxLeft) / m_rxMaxBracket);
    double ryScale((m_uyBottom - m_uyTop) / m_ryMaxBracket);
    m_trans = agg::trans_affine(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    m_trans *= agg::trans_affine_scaling(rxScale, ryScale);
    m_trans *= agg::trans_affine_translation(m_uxLeft, m_uyTop);
}

unsigned lmShapeBracket::GetVertex(lmLUnits* pux, lmLUnits* puy)
{
	if(m_nCurVertex >= m_nNumVerticesBracket)
		return agg::path_cmd_stop;

	//scaling and translation
	double x = m_BracketVertices[m_nCurVertex].ux_coord;
	double y = m_BracketVertices[m_nCurVertex].uy_coord;
	m_trans.transform(&x, &y);

	//return values
	*pux = (lmLUnits)x;
	*puy = (lmLUnits)y;
	return m_BracketVertices[m_nCurVertex++].cmd;
}



//-------------------------------------------------------------------------------------
// Implementation of lmShapeBrace

lmShapeBrace::lmShapeBrace(lmInstrument* pInstr, lmLUnits xLeft, lmLUnits yTop,
                           lmLUnits xRight, lmLUnits yBottom,
                           lmLUnits dyHook, wxColour color)
	: lmShapeBracketBrace(pInstr, lm_eBrace, color, _T("brace"))
{
    m_nType = eGMO_ShapeBrace;

    m_uxLeft = xLeft;
    m_udyHook = dyHook;
    m_uyTop = yTop - m_udyHook;
    m_uxRight = xRight;
    m_uyBottom = yBottom + m_udyHook;
    m_rBraceBarHeight = (double)(yBottom - yTop);

	//set bounds
	SetXLeft(xLeft);
	SetYTop(yTop);
	SetXRight(xRight);
	SetYBottom(yBottom);

    //set scaling and translation
    SetAffineTransform();
}

lmShapeBrace::~lmShapeBrace()
{
}

void lmShapeBrace::SetAffineTransform()
{
    //scale and position vertices

    double rxScale((m_uxRight - m_uxLeft) / m_dxBraceBar);
    double ryScale(m_udyHook / m_dyBrace);
    m_trans = agg::trans_affine(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    m_trans *= agg::trans_affine_scaling(rxScale, ryScale);
    m_trans *= agg::trans_affine_translation(m_uxLeft, m_uyTop);
}

unsigned lmShapeBrace::GetVertex(lmLUnits* pux, lmLUnits* puy)
{
	switch(m_nContour)
	{
		case 0:		//top hook
		{
			//scaling and translation
			double x = m_BraceVertices[m_nCurVertex].ux_coord;
			double y = m_BraceVertices[m_nCurVertex].uy_coord;
			m_trans.transform(&x, &y);

			//return values
			*pux = (lmLUnits)x;
			*puy = (lmLUnits)y;
			unsigned cmd = m_BraceVertices[m_nCurVertex++].cmd;

			//change to rectangle contour?
			if(m_nCurVertex == m_nNumVerticesBrace)
            {
				m_nContour++;
                m_nCurVertex = 0;
            }

            return cmd;
		}
		case 1:		//bottom hook
		{
		    if(m_nCurVertex >= m_nNumVerticesBrace2)
			    return agg::path_cmd_stop;

			//scaling and translation
			double x = m_BraceVertices2[m_nCurVertex].ux_coord;
			double y = m_BraceVertices2[m_nCurVertex].uy_coord;
			m_trans.transform(&x, &y);

            y += m_rBraceBarHeight;

			//return values
			*pux = (lmLUnits)x;
			*puy = (lmLUnits)y;
			return m_BraceVertices2[m_nCurVertex++].cmd;
		}
		default:
			return agg::path_cmd_stop;
	}
}

