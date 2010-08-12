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

#ifndef __LM_GRAPHICMANAGER_H__        //to avoid nested includes
#define __LM_GRAPHICMANAGER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "GraphicManager.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <list>

#include "../score/defs.h"
#include "GMObject.h"		//for using lmGMSelection

class lmScore;
class lmPaper;
class lmBoxScore;


//Class lmGraphicManager stores and manages all score renderization issues
class lmGraphicManager
{
public:
    lmGraphicManager();
    ~lmGraphicManager();

    //renderization
    bool PrepareToRender(lmScore* pScore, lmLUnits paperWidth, lmLUnits paperHeight, 
                         double rScale, lmPaper* pPaper, int nOptions=0);
    wxBitmap* RenderScore(int nPage, int nOptions=0,
                          wxWindow* pRenderWindow = (wxWindow*)NULL,
                          wxPoint vOffset = wxPoint(0, 0));      //render page 1..n

    //bitmaps export
    void ExportAsImage(wxString& sFilename, wxString& sExt, int nImgType);
    void BitmapsToFile(wxString& sFilename, wxString& sExt, int nImgType);

    // highlighting
    void PrepareForHighlight();

    //access
    int GetNumPages();
    lmBoxScore* GetBoxScore() const { return m_pBoxScore; }
    lmGMObject* FindShapeAtPagePos(int nNumPage, lmUPoint uPos, bool fSelectable);
    lmBox* FindBoxAtPagePos(int nNumPage, lmUPoint uPos);

    //selection management
    //lmGMSelection* CreateSelection(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
    //                               lmLUnits uYMin, lmLUnits uYMax);
    //lmGMSelection* AddToSelection(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
    //                              lmLUnits uYMin, lmLUnits uYMax);
    //inline lmGMSelection* GetSelection() { return &m_Selection; }

    int GetNumObjectsSelected();
    void ClearSelection();
    void NewSelection(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
                      lmLUnits uYMin, lmLUnits uYMax);
    void NewSelection(lmGMObject* pGMO);



private:
    void Layout();
    void DeleteBitmaps();
    wxBitmap* GetPageBitmap(int nPage);
    wxBitmap* NewBitmap(int nPage);
    void AddBitmap(int nPage, wxBitmap* pBitmap);


    lmScore*        m_pScore;           //score to be rendered

	//TODO: lmPaper contains the DC. Therefore it must not be stored and reused.
	//Methods needing an lmPaper must receive it as parameter
    lmPaper*        m_pPaper;           //paper to use

    long            m_nLastScoreID;     // the ID of the last rendered score
    double          m_rScale;           // drawing scale
    lmPixels        m_xPageSize, m_yPageSize;        // size of page in pixels

    lmBoxScore*     m_pBoxScore;        //the main container

    bool            m_fReLayout;        //force to re-layout the score


    //offscreen bitmaps management
	typedef struct lmBitmapPage_Struct {
		wxBitmap*		pBitmap;        //ptr. to page bitmap
		int				nPage;			//num page for this bitmap
	} lmBitmapPage;

    std::list<lmBitmapPage*>    m_Bitmaps;      //offsceen bitmaps
    lmPixels                    m_xBitmapSize, m_yBitmapSize;    // size of bitmaps in pixels

    lmGMSelection   m_Selection;            //selected objects

};


//Global functions related to rendering scores
extern wxBitmap GenerateBitmapForKeyCtrol(wxString& sKeyName, lmEKeySignatures nKey);
extern wxBitmap GenerateBitmapForClefCtrol(wxString& sClefName, lmEClefType nClef);
extern wxBitmap GenerateBitmapForBarlineCtrol(wxString& sName, lmEBarline nBarlineType);
extern wxBitmap lmGenerateBitmap(lmScore* pScore, wxSize size, double rScale=1.0);




namespace lenmus
{

//-----------------------------------------------------------------------------
//forward declarations
class GraphicInterface;

//-----------------------------------------------------------------------------
// RenderizationFormat
//      a container for bitmap requirements: size, scale, etc.

class RenderizationFormat
{
protected:
    double      m_rScale;                   // drawing scale (1.0 = 100%)
    lmPixels    m_xPageSize, m_yPageSize;   // size of page in pixels

public:
    RenderizationFormat(double rScale, lmPixels xPageSize, lmPixels yPageSize)
        : m_rScale(rScale), m_xPageSize(xPageSize), m_yPageSize(yPageSize) {} 

    //getters & setters
    inline double get_scale() { return m_rScale; }
    inline lmPixels get_page_width() { return m_xPageSize; }
    inline lmPixels get_page_height() { return m_yPageSize; }
    inline void set_scale(double rScale) { m_rScale = rScale; }
    inline void set_page_width(lmPixels xPageSize) { m_xPageSize = xPageSize; }
    inline void set_page_height(lmPixels yPageSize) { m_yPageSize = yPageSize; }

};

//-----------------------------------------------------------------------------
class GraphicModel
{
protected:
    lmGraphicManager&   m_graphMngr;

public:
    GraphicModel(lmGraphicManager& graphMngr) : m_graphMngr(graphMngr) {}
    ~GraphicModel() {}

    //access
    inline int get_num_pages() { return m_graphMngr.GetNumPages(); }
    lmBoxScore* get_box_score() const { return m_graphMngr.GetBoxScore(); }
    lmGMObject* find_shape_at_page_pos(int nNumPage, lmUPoint uPos, bool fSelectable)
    {
        return m_graphMngr.FindShapeAtPagePos(nNumPage, uPos, fSelectable);
    }

    lmBox* find_box_at_page_pos(int nNumPage, lmUPoint uPos)
    {
        return m_graphMngr.FindBoxAtPagePos(nNumPage, uPos);
    }

};

//-----------------------------------------------------------------------------
class RenderizationServer
{
protected:
    lmGraphicManager&   m_graphMngr;

public:
    RenderizationServer(lmGraphicManager& graphMngr) : m_graphMngr(graphMngr) {}
    ~RenderizationServer() {}

    //renderization
    bool prepare_to_render(lmScore* pScore, lmLUnits paperWidth, lmLUnits paperHeight, 
                         double rScale, lmPaper* pPaper, int nOptions=0)
    {
        return m_graphMngr.PrepareToRender(pScore, paperWidth, paperHeight, 
                                            rScale, pPaper, nOptions);
    }

    wxBitmap* render_score(int nPage, int nOptions=0,
                          wxWindow* pRenderWindow = (wxWindow*)NULL,
                          wxPoint vOffset = wxPoint(0, 0))
    {
        //render page 1..n
        return m_graphMngr.RenderScore(nPage, nOptions, pRenderWindow, vOffset);
    }

    // highlighting
    void prepare_for_highlight() 
    {
        m_graphMngr.PrepareForHighlight();
    }

    //bitmaps export
    void export_as_image(wxString& sFilename, wxString& sExt, int nImgType)
    {
        m_graphMngr.ExportAsImage(sFilename, sExt, nImgType);
    }

    void bitmaps_to_file(wxString& sFilename, wxString& sExt, int nImgType)
    {
        m_graphMngr.BitmapsToFile(sFilename, sExt, nImgType);
    }

};

//-----------------------------------------------------------------------------
class GraphicInterface
{
private:
    lmGraphicManager        m_graphMngr;
    GraphicModel            m_model;
    RenderizationServer     m_server;

public:
    GraphicInterface() : m_model(m_graphMngr), m_server(m_graphMngr) {}
    ~GraphicInterface() {}

    //RenderizationServer -----------------------------------------
    bool prepare_to_render(lmScore* pScore, lmLUnits paperWidth, lmLUnits paperHeight, 
                         double rScale, lmPaper* pPaper, int nOptions=0)
    {
        return m_server.prepare_to_render(pScore, paperWidth, paperHeight, 
                                            rScale, pPaper, nOptions);
    }

    wxBitmap* render_score(int nPage, int nOptions=0,
                          wxWindow* pRenderWindow = (wxWindow*)NULL,
                          wxPoint vOffset = wxPoint(0, 0))
    {
        //render page 1..n
        return m_server.render_score(nPage, nOptions, pRenderWindow, vOffset);
    }

    // highlighting
    void prepare_for_highlight() 
    {
        m_server.prepare_for_highlight();
    }

    //bitmaps export
    void export_as_image(wxString& sFilename, wxString& sExt, int nImgType)
    {
        m_server.export_as_image(sFilename, sExt, nImgType);
    }

    void bitmaps_to_file(wxString& sFilename, wxString& sExt, int nImgType)
    {
        m_server.bitmaps_to_file(sFilename, sExt, nImgType);
    }

    //GraphicModel ---------------------------------------------------------------
    inline int get_num_pages() { return m_model.get_num_pages(); }
    lmBoxScore* get_box_score() const { return m_model.get_box_score(); }
    lmGMObject* find_shape_at_page_pos(int nNumPage, lmUPoint uPos, bool fSelectable)
    {
        return m_model.find_shape_at_page_pos(nNumPage, uPos, fSelectable);
    }

    lmBox* find_box_at_page_pos(int nNumPage, lmUPoint uPos)
    {
        return m_model.find_box_at_page_pos(nNumPage, uPos);
    }

    //selection management
    int get_num_objects_selected() { return m_graphMngr.GetNumObjectsSelected(); }
    void clear_selection() { m_graphMngr.ClearSelection(); }
    void new_selection(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
                      lmLUnits uYMin, lmLUnits uYMax)
    {
        m_graphMngr.NewSelection(nNumPage, uXMin, uXMax, uYMin, uYMax);
    }

    void new_selection(lmGMObject* pGMO)
    {
        m_graphMngr.NewSelection(pGMO);
    }

};

}   //lenmus namespace

#endif  // __LM_GRAPHICMANAGER_H__

