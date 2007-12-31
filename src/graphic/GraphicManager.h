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

#ifndef __LM_GRAPHICMANAGER_H__        //to avoid nested includes
#define __LM_GRAPHICMANAGER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "GraphicManager.cpp"
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

class lmScore;
class lmPaper;
class lmBoxScore;
class lmGMObject;

// offscreen bitmaps will be maintained in a list structure. Let's declare it
WX_DECLARE_LIST(wxBitmap, BitmapList);

// to use offscreen bitmaps or not
#define lmUSE_BITMAPS       true        //create a compatible bitmap and return it
#define lmNO_BITMAPS        false       //direct drawing on the DC

//Options for method Prepare
enum lmGraphMngrOptions
{
    lmFORCE_RELAYOUT                    = 0x0001,
    lmRELAYOUT_ON_PAPER_SIZE_CHANGE     = 0x0002,
};

//Class lmGraphicManager stores and manages all score renderization issues
class lmGraphicManager
{
public:
    lmGraphicManager(lmScore* pScore, lmPaper* pPaper);
    lmGraphicManager();
    ~lmGraphicManager();

    void Create(lmScore* pScore, lmPaper* pPaper);

    //renderization
    void Prepare(lmScore* pScore, lmLUnits paperWidth, lmLUnits paperHeight, 
                 double rScale, lmPaper* pPaper, int nOptions=0);

    void Layout();                          //measure phase
    wxBitmap* Render(bool fUseBitmaps, int nPage);      //render page 1..n

    //bitmaps export
    void ExportAsImage(wxString& sFilename, wxString& sExt, int nImgType);
    void BitmapsToFile(wxString& sFilename, wxString& sExt, int nImgType);

    // highlighting
    void PrepareForHighlight();

    //access
    int GetNumPages();
    lmBoxScore* GetBoxScore() const { return m_pBoxScore; }
    lmGMObject* FindGMObjectAtPagePosition(int nNumPage, lmUPoint uPos);



private:
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
    BitmapList      m_cBitmaps;         //offsceen bitmaps
    wxArrayInt      m_aBitmapPage;      //num page for this bitmap

    lmPixels        m_xBitmapSize, m_yBitmapSize;    // size of bitmaps in pixels

    // double buffering for score higlight
    int             m_nHighlightedPage;     // displayed page number (0 = none)
    wxBitmap*       m_pAuxBitmap;           // saved bitmap for displayed page
    bool            m_fHighlight;           // highlight in process

};

#endif  // __LM_GRAPHICMANAGER_H__

