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

#ifndef __LM_BOXPAGE_H__        //to avoid nested includes
#define __LM_BOXPAGE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BoxPage.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <vector>
#include <list>

#include "../score/defs.h"
#include "GMObject.h"

class lmBoxScore;
class lmBoxSystem;
class lmBoxSlice;
class lmScore;
class lmPaper;
class lmShapeMargin;
class lmHandler;
class lmLayer;


class lmBoxPage : public lmBox
{
public:
    lmBoxPage(lmBoxScore* pParent, int nNumPage);
    ~lmBoxPage();

	//info
    inline int GetFirstSystem() const { return m_nFirstSystem; }
    inline int GetLastSystem() const { return m_nLastSystem; }
	int GetSystemNumber(lmBoxSystem* pSystem);

	//access to objects
	lmBoxSystem* GetSystem(int nSystem);		//nSystem = 1..n

	//operations
    lmBoxSystem* AddSystem(int nSystem, lmLUnits uxPos, lmLUnits uyPos, bool fFirstOfPage);
    void Render(lmScore* pScore, lmPaper* pPaper);
    void RenderWithHandlers(lmPaper* pPaper);
    void DrawAllHandlers(lmPaper* pPaper);
    void OnNeedToDrawHandlers(lmGMObject* pGMO);

    //selection
    lmGMObject* FindShapeAtPos(lmUPoint& uPoint, bool fSelectable);


    //renderization related
    inline wxWindow* GetRenderWindow() { return m_pRenderWindow; }
    inline wxPoint& GetRenderWindowOffset() { return m_vOffset; }
    inline void SetRenderWindow(wxWindow* pWindow) { m_pRenderWindow = pWindow; }
    inline void SetRenderWindowOffset(wxPoint& vOffset) { m_vOffset = vOffset; }

    //implementation of virtual methods from base class
    wxString Dump(int nIndent);
	inline int GetPageNumber() const { return m_nNumPage; }

	//owners and related
	lmBoxSystem* GetOwnerSystem() { return (lmBoxSystem*)NULL; }
    inline lmBoxScore* GetBoxScore() const { return m_pBScore; }
    inline lmBoxScore* GetOwnerBoxScore() { return m_pBScore; }
    inline lmBoxPage* GetOwnerBoxPage() { return this; }

    //active handlers
	void AddActiveHandler(lmHandler* pHandler);

    //layers and shapes
    void AddShapeToLayer(lmShape* pShape, long nLayerID);
    void PopulateLayers();


private:
    void CreateLayers();

    // a lmBoxPage is, mainly, a collection of lmBoxSystems

    lmBoxScore*     m_pBScore;          //parent score box
    int             m_nNumPage;         //this page number (1..n)
    int             m_nFirstSystem;
    int             m_nLastSystem;
    lmUPoint        m_pageOrgL;         //page origin
    lmShapeMargin*  m_pMarginShapes[4];
    wxWindow*       m_pRenderWindow;    //the window on which the page is rendered
    wxPoint         m_vOffset;          //page offset to apply.

    //objects in this page to be rendered with handlers
    std::vector<lmGMObject*>    m_GMObjsWithHandlers;

	//list of active handlers contained within this page
    std::list<lmHandler*>       m_ActiveHandlers;  

    //layers and shapes
	std::list<lmLayer*>	    m_Layers;		//contained shapes, ordered by layer

};



#endif  // __LM_BOXPAGE_H__

