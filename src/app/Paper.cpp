// RCS-ID: $Id: Paper.cpp,v 1.4 2006/02/23 19:17:12 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file Paper.cpp
    @brief Implementation file for class lmPaper
    @ingroup app_gui
*/
/*! @class lmPaper
    @ingroup app_gui
    @brief The generic canvas on which the score is rendered.
    
    A 'lmPaper' is the generic canvas on which the score is rendered. It might be a display
    device,    a printer, etc. Basically it is a DC on wich to write and draw and some methods
    to deal with unit conversion.
    
    The lmPaper responsibilities are:
    - it manages the offscreen bitmaps that receives all drawing operations
    - it is a container for the DC object on which to write.
    - it is responsible for all scale and unit conversion methods
    - it is responsible for informing the staff objects about page margins, spacings, 
        layout, etc.

    For rendering a score the steps to follow are:
    1. The view must call Prepare(). Prepare is responsible for calling
        StartDoc(), NewPage() and then score->Draw()
    2. The draw process starts with a new page prepared and must call NewPage()
        for advancing the paper.
    3. When finishing, must not invoke NewPage().
    4. Then Prepare must call EndDoc() 

    ==> StartDoc() and EndDoc() are not public

*/
#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "scoreview.h"
#include "../score/Score.h"
#include "Page.h"
#include "FontManager.h"
#include "Paper.h"


// global data structures for printing. Defined in TheApp.cpp
#include "wx/cmndata.h"
extern wxPrintData* g_pPrintData;
extern wxPageSetupData* g_pPaperSetupData;

// Definition of the BitmapList class
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(BitmapList);


lmPaper::lmPaper()
{
    m_xCursor = 0;
    m_yCursor = 0;
    m_pDC = (wxDC *) NULL;

    // bitmaps
    m_xPageSize = 0;
    m_yPageSize = 0;
    m_rScale = 0;
    m_numPages = 0;
    m_fRedraw = true;



}

lmPaper::~lmPaper()
{
    DeleteBitmaps();
    //m_cBitmaps.DeleteContents(true);    // so that Clear() willdelete all bitmaps
    //m_cBitmaps.Clear();                    // now, delete all elements
}

// returns paper size in logical units
wxSize& lmPaper::GetPaperSize()
{
    return m_Page.PageSize();
    //wxSize sz = g_pPaperSetupData->GetPaperSize();
    //return wxSize(10*sz.GetWidth(), 10*sz.GetHeight());
}


void lmPaper::Prepare(lmScore* pScore, lmMicrons paperWidth, lmMicrons paperHeight, double rScale)
{
    // If the score has changed or the scale has changed or paper size has changed 
    // we need to recreate the bitmaps
    bool fDrawScore = m_fRedraw;
    m_fRedraw = false;
    if (m_pScore != pScore || m_rScale != rScale || fDrawScore) {

        fDrawScore = true;

        //store new values
        m_pScore = pScore;
        m_rScale = rScale;
        m_xPageSize = paperWidth;
        m_yPageSize = paperHeight;

        // if size has changed delete the allocated bitmaps
        if (paperWidth != m_xBitmapSize || paperHeight != m_yBitmapSize) {
            DeleteBitmaps();
        }
    }

    //repaint score if necesary
    if (fDrawScore) {

        // Allocate a DC in memory for the offscreen bitmap
        wxMemoryDC memDC;
        SetDC(&memDC);

        m_numPages = 0;        // first page

        // Prepare the first page
        NewPage();

        // Ask the score to draw itself onto this lmPaper
        m_pScore->Draw(this);

        // @attention method lmScore.Draw() will call NewPage() when need a new page. This will
        // select a new bitmap into de memDC.

        ////DEBUG: draw red lines on last page to show cursor position
        //lmMicrons yCur = GetCursorY();
        //lmMicrons xCur = GetCursorX();
        //m_pDC->SetPen(*wxRED_PEN);
        //m_pDC->DrawLine(0, yCur, GetPaperSize().GetWidth(), yCur);
        //m_pDC->DrawLine(xCur, 0, xCur, GetPaperSize().GetHeight());
        ////End DEBUG --------------------------------------------

        // deselect last page bitmap
        if (m_pDC->IsKindOf(CLASSINFO(wxMemoryDC))) {
            ((wxMemoryDC*)m_pDC)->SelectObject(wxNullBitmap);
        }
        SetDC( (wxDC*)NULL );
    }

}

void lmPaper::RestartPageCursors()
{
    m_xCursor = GetPageLeftMargin();
    m_yCursor = GetPageTopMargin();

}

void lmPaper::NewLine(lmMicrons nSpace)
{
    m_yCursor += nSpace;
    m_xCursor = GetPageLeftMargin();
    
}



void lmPaper::NewPage()
{
    wxASSERT(m_pDC);
    RestartPageCursors();

    if (m_pDC->IsKindOf(CLASSINFO(wxMemoryDC))) {
        // get the bitmap for first page
        wxBitmap* pBitmap = GetPageBitmap(m_numPages);
        wxASSERT(pBitmap);
        wxASSERT(pBitmap->Ok());
        m_numPages++;

        ((wxMemoryDC *)m_pDC)->SelectObject(*pBitmap);
    }

    m_pDC->Clear();
    m_pDC->SetMapMode(lmDC_MODE);
    m_pDC->SetUserScale( m_rScale, m_rScale );

    ////DEBUG: draw green lines to show initial cursor position
    //if (m_numPages==1) {
    //    lmMicrons yCur = GetCursorY();
    //    lmMicrons xCur = GetCursorX();
    //    m_pDC->SetPen(*wxGREEN_PEN);
    //    m_pDC->DrawLine(0, yCur, GetPaperSize().GetWidth(), yCur);
    //    m_pDC->DrawLine(xCur, 0, xCur, GetPaperSize().GetHeight());

    //    m_pDC->DrawLine(0, GetPaperSize().GetHeight()-100, 
    //                    GetPaperSize().GetWidth(), 
    //                    GetPaperSize().GetHeight()-100);
    //}
    ////End DEBUG --------------------------------------------

}

lmMicrons lmPaper::GetRightMarginXPos()
{ 
    return GetPaperSize().GetWidth() - GetPageRightMargin();
}

lmMicrons lmPaper::GetLeftMarginXPos()
{
    return GetPageLeftMargin();
}

// Get the bitmap for page nPage. If no bitmap is allocated, do it.
// nPage = 0 .. n-1
wxBitmap* lmPaper::GetPageBitmap(wxInt32 nPage)
{
    wxASSERT(nPage >= 0);
    wxASSERT(nPage <= m_numPages);    // m_numPages could be not yet incremented

    wxBitmap* pBitmap;
    wxInt32 nNumBitmaps = m_cBitmaps.GetCount();
    if (nNumBitmaps > 0 && nNumBitmaps > nPage ) {
        // bitmap already exits. Get it.
        wxBitmapListNode* pNode = m_cBitmaps.Item(nPage);
        wxASSERT(pNode);
        pBitmap = pNode->GetData();

    } else {
        //No bitmap allocated. Create one
        pBitmap = new wxBitmap(m_xPageSize, m_yPageSize);
        if (!pBitmap || !pBitmap->Ok()) {
            if (pBitmap) {
                delete pBitmap;
                pBitmap = (wxBitmap *) NULL;
            }
            wxLogMessage(_T("lmPaper::GetPageBitmap: Bitmap size (%d, %d) pixels."), m_xPageSize, m_yPageSize);
            wxMessageBox(_("Sorry, not enough memory to create a pBitmap to display the page."),
                _T("lmPaper.GetPageBitmap"), wxOK);
            ::wxExit();
        }
        // add the new bitmap to the list and store its size
        m_cBitmaps.Append(pBitmap);
        m_xBitmapSize = m_xPageSize;
        m_yBitmapSize = m_yPageSize;

    }

    return pBitmap;
}

wxFont* lmPaper::GetFont(int nPointSize, wxString sFontName, 
                       int nFamily, int nStyle, int nWeight, bool fUnderline)
{
    return m_fontManager.GetFont(nPointSize, sFontName, nFamily, nStyle, nWeight, fUnderline);
}


// nPage = 0 ... n-1
wxBitmap* lmPaper::GetOffscreenBitmap(wxInt32 nPage)
{
    wxASSERT(nPage >=0 && nPage < m_numPages);
    wxASSERT(m_cBitmaps.GetCount());
    wxASSERT(m_cBitmaps.GetCount() > (size_t)nPage );

    wxBitmapListNode* pNode = m_cBitmaps.Item(nPage);
    wxASSERT(pNode);
    return pNode->GetData();
    
}

void lmPaper::DeleteBitmaps()
{
    wxBitmapListNode* pNode = m_cBitmaps.GetFirst();
    while (pNode) {
        wxBitmap* pBitmap = pNode->GetData();
        delete pBitmap;
        delete pNode;
        pNode = m_cBitmaps.GetFirst();
    }

}

/*
//Variables a nivel de pentagrama (CStaff)
//Private m_dyAntesPentagrama As Single
//Private m_dyTrasPentagrama As Single
//Private m_dyEntrePentagramas As Single

//variables a nivel de partitura
//Private m_dyEntreSistemas As Single

//Estos márgenes pueden tener valores negativos para, por ejemplo, poder solapar cosas.

//-------------------------------------------------------------------------------------------------------------------


Private m_oPapel As Object     //PictureBox o Print. Representa el papel donde se dibuja
Private m_nId As Long           //id de este papel. Para identificación única

//nuevas variables para manejar dibujo mediante fonts
Private m_nPointSize As Long
Private m_nGrosorLineas As Long
Private m_nColor As Long            //color en que se dibuja
Private nResolucion As Long // = 96
Const nUnitsPerEM As Long = 2048

//valores fijos que se establecen en inicialización
Private m_rEscala As Single
Private m_rdyLinea As Single

//valores derivados de los anteriores
Private m_yCursor As Single         //posición del do central (va variando a medida que se van pintando pentagramas)
Private m_ndyLines As Single     //distancia entre líneas (fijo = m_rdyLinea * m_rEscala)

//variables para ir eliminando y sustituyendo por las nuevas
Private dxRightMarging As Single

//nuevas variables para racionalizar todo

//- Valores relativos, que van multiplicados por rEscala
//Todos los valores estan en pixels
Private m_nMargenDrcho As Long
Private m_nLeftMarging As Long
Private m_nMargenSup As Long
Private m_nMargenInf As Long

//- Valores absolutos, en pixels
Private m_nLeft As Long
Private m_nTop As Long

//dibujo de literales asociados a signos de repetición (Fine, al Segno, ...)
Private m_sFontSigno As String


//- Acceso y establecimiento de valores

wxInt32 lmScoreCanvas::Get ID() As Long
    ID = m_nId
}

wxInt32 lmScoreCanvas::Let Ancho(nAncho As Long)
    dxRightMarging = m_oPapel.ScaleWidth - nAncho
    m_nMargenDrcho = nAncho
    
}

wxInt32 lmScoreCanvas::Get Ancho() As Long
    Ancho = m_oPapel.ScaleWidth - dxRightMarging
    
}

wxInt32 lmScoreCanvas::Let color(nColor As Long)
    m_nColor = nColor
    m_oPapel.ForeColor = nColor
}

wxInt32 lmScoreCanvas::Let MargenIzqdo(nAncho As Long)
    m_nLeftMarging = nAncho
    
}

wxInt32 lmScoreCanvas::Get MargenIzqdo() As Long
    MargenIzqdo = m_nLeftMarging
    
}

wxInt32 lmScoreCanvas::Let MargenDrcho(nAncho As Long)
    m_nMargenDrcho = nAncho
    
}

wxInt32 lmScoreCanvas::Get MargenDrcho() As Long
    MargenDrcho = m_nMargenDrcho
    
}

wxInt32 lmScoreCanvas::Let MargenSuperior(nEspacio As Long)
    m_nMargenSup = nEspacio
    RecalcularValores
    
}



//-Aqui empieza lo viejo

wxInt32 lmScoreCanvas::Let Escala(ByVal rValor As Single)
    Dim i As Long, rZoom As Single
    
    //ajusta la escala a un valor que permita que el espaciado entre líneas sea pixels exactos
    // El diseño del font está realizado para que la distancia entre líneas del pentagrama sean
    // 512 FUnits. Esto da una resolución óptima en pantallas con resolucion VGA (96 pixels por pulgada)
    // ya que permite dibujar las líneas del pentagrama sobre pixels exactos, según las siguientes
    // relaciones:
    //       dyLineas = distancia entre líneas (pixels)
    //       Font size = 3 * dyLineas   (points)
    //       Escala = 100 * dyLineas / 8     (%)
    //
    // Dado el factor de zoom (en porcentaje, por ej. zoom=250,0%) los datos pueden calcularse como:
    //       i = Redondear((zoom*8) / 100)
    //       dyLineas = i
    //       FontSize = 3*i
    //
    Dim rDPIx As Single, rDPIy As Single, rFx As Single, rFy As Single
    rDPIx = Screen.TwipsPerPixelX
    rDPIy = Screen.TwipsPerPixelY
    rFx = 15# / rDPIx
    rFy = 15# / rDPIy
    
    rZoom = rValor * 100#
    i = Round((rZoom * 8#) / 100#)
    
    m_nPointSize = 3 * i
    sDbgFonts = sDbgFonts & "FontSize = " & m_nPointSize & sCrLf

    //actualiza rValor para dejar la escala realmente aplicada
    rValor = CSng(i) / 8#

    m_ndyLines = i * rFy
    sDbgFonts = sDbgFonts & "Escala = " & rValor & ", dyLinea = " & m_ndyLines & sCrLf

    //Calcula, en pixels, el grosor de las líneas (es de 51 FUnits, la décima parte de la distancia entre líneas)
    m_nGrosorLineas = FUnitsToTwips(51) * nResolucion \ 1440
    if m_nGrosorLineas < 1 { m_nGrosorLineas = 1
    sDbgFonts = sDbgFonts & "nGrosorLineas = " & m_nGrosorLineas & sCrLf
    sDbgFonts = sDbgFonts & "Twips por pixel X = " & Screen.TwipsPerPixelX & sCrLf & _
        "Twips por pixel Y = " & Screen.TwipsPerPixelY & sCrLf

    m_rEscala = rValor
    xUnits = m_nGrosorLineas
    EscalarValores
    
}

void lmScoreCanvas::PonerEnLienzo(picLienzo As Object, nTop As Long, nLeft As Long)
    //PonerEnLinezo es la primera función a llamar antes de poder utilizar un objeto CPapel.
    //Aquí se establecen las características del lienzoi a utilizar:
    //   - Márgenes del lienzo (nTop y nLeft)
    //   - Objeto que actuará como lienzo (picLienzo)
    //   - Factores de conversión DotsPerInch (rDPIx. rDPIy)
    
    Dim rDPIx As Single, rDPIy As Single    //Factores de conversión DotsPerInch
    Dim rFx As Single, rFy As Single
    rDPIx = Screen.TwipsPerPixelX
    rDPIy = Screen.TwipsPerPixelY
    rFx = 15# / rDPIx
    rFy = 15# / rDPIy
    
    //Definir el lienzo y el origen del papel respecto del lienzo
    m_nLeft = nLeft
    m_nTop = nTop
    Set m_oPapel = picLienzo
    m_oPapel.ScaleMode = vbPixels

    m_nId = AsignarPapelId          //obtener identificación única
    
    //Establecer dimensiones de referencia y escala
    nResolucion = 1440 / rDPIx
    m_nPointSize = 24
    m_rdyLinea = FUnitsToTwips(512) / rDPIy    // 8# * rFy
//    m_ryDo = 5 * m_rdyLinea
    Escala = 1#
    
    //establecer márgenes
    m_nMargenSup = 60
    m_nMargenDrcho = 60
    m_nLeftMarging = 10
    
    
    //limpia el papel
    LimpiarPapel
    m_oPapel.ScaleMode = vbPixels    //no se porqué. Pero si no se hace, falla.
    
    //establece la posición actual
    m_xCursor = m_nLeftMarging * m_rEscala
    
    //calcular la posición base del pentagrama
    m_yCursor = m_nTop + m_nMargenSup + 5 * m_rdyLinea * m_rEscala
    
    //font para signos de repetion
    m_sFontSigno = "Arial"      // "Georgia" es muy bueno para mf, ff, p ...

}

Private Sub EscalarValores()
    MPapel.dyLinea = m_ndyLines
    
    //Inicializa valores derivados
    dyNota = (m_ndyLines / 2)
    dyStem = 4# * m_ndyLines
    
    //inicializa variables
    dxRightMarging = 10# * m_rEscala
    
    With m_oPapel
        .FontName = "LeMus Notas"
        .Font.Size = m_nPointSize   //24 * m_rEscala
        .Font.Bold = False
        .Font.Italic = False
    }

}

Private Sub RecalcularValores()
    EscalarValores
    m_xCursor = m_nLeftMarging * m_rEscala     //reestablecer la posición inicial
    m_yCursor = m_nMargenSup + 5 * m_rdyLinea * m_rEscala   //calcular la posición base del pentagrama
//    MsgBox "Nuevo=" & m_yCursor

}

void lmScoreCanvas::IniciarHoja(Optional nNumPag As Long = 0)
    if TypeOf m_oPapel Is Printer {
       if nNumPag > 1 {
          m_oPapel.NewPage
       }
    } else {
       LimpiarPapel
       m_oPapel.ScaleMode = vbPixels    //no se porqué. Pero si no se hace, falla.
    }
    
    m_xCursor = m_nLeftMarging * m_rEscala
    m_nColor = colorNegro
    
    //calcular la posición base del pentagrama
    m_yCursor = m_nTop + m_nMargenSup + 5 * m_rdyLinea * m_rEscala
    
}

void lmScoreCanvas::PintarLinea(fMeasuring As Boolean, _
        xIni As Single, yIni As Single, xFin As Single, yFin As Single, _
        Optional nGrosor As Long = 1, Optional fGrosorIzqdaArriba As Boolean = False)
    //Dibuja una línea vertical u horizontal. El grosor de la línea siempre se dibuja
    //hacia abajo (línea horizontal) o hacia la derecha (línea vertical), salvo que el
    //flag fGrosorIzqdaArriba sea True
        
    if fMeasuring { Exit Sub
    
    Dim iMax As Long
    
    iMax = nGrosor * m_nGrosorLineas - 1
    if fGrosorIzqdaArriba { iMax = -iMax
    if xIni = xFin {     //linea vertical
        m_oPapel.Line (xIni, yIni)-(xFin + iMax, yFin), , BF
    } else {if yIni = yFin {     //línea horizontal
        m_oPapel.Line (xIni, yIni)-(xFin, yFin + iMax), , BF
    }
    
}

Public Function PintarCorchete(fMeasuring As Boolean, nTipoNota As ENoteType, _
        fStemAbajo As Boolean, nxLeft As Long, nyTop As Long) As Single

    Dim sGlyph As String
    Select case nTipoNota
        case eEighth
            sGlyph = IIf(fStemAbajo, CHAR_CORCHETE_DEBAJO_1, CHAR_CORCHETE_ENCIMA_1)
        case e16th
            sGlyph = IIf(fStemAbajo, CHAR_CORCHETE_DEBAJO_2, CHAR_CORCHETE_ENCIMA_2)
        case e32th
            sGlyph = IIf(fStemAbajo, CHAR_CORCHETE_DEBAJO_3, CHAR_CORCHETE_ENCIMA_3)
        case e64th
            sGlyph = IIf(fStemAbajo, CHAR_CORCHETE_DEBAJO_4, CHAR_CORCHETE_ENCIMA_4)
        case e256th
            sGlyph = IIf(fStemAbajo, CHAR_CORCHETE_DEBAJO_5, CHAR_CORCHETE_ENCIMA_5)
        case e128th
            sGlyph = IIf(fStemAbajo, CHAR_CORCHETE_DEBAJO_6, CHAR_CORCHETE_ENCIMA_6)
        case } else {
            MsgBox "[PintarCorchete] Error: tipo de nota (" & nTipoNota & ") no contemplado"
            sGlyph = IIf(fStemAbajo, CHAR_CORCHETE_DEBAJO_1, CHAR_CORCHETE_ENCIMA_1)
    }
     
    if Not fMeasuring {
        if fStemAbajo {
            Escribir sGlyph, nxLeft, nyTop - 5.5 * m_ndyLines
        } else {
            Escribir sGlyph, nxLeft, nyTop - 5.5 * m_ndyLines
        }
    }
    PintarCorchete = m_oPapel.TextWidth(sGlyph)
        
End Function

void lmScoreCanvas::PintarCorcheteGrupo(xIni As Single, yIni As Single, _
            xFin As Single, yFin As Single, fStemsAbajo As Boolean)
            
    Dim i As Long, yInicio As Single, yFinal As Single, iMax As Long, rIncr As Single
    
    yInicio = Round(yIni)
    yFinal = Round(yFin)
    rIncr = IIf(fStemsAbajo, -1#, 1#)
    iMax = 3 * m_nGrosorLineas - 1
    for (i = 0 To iMax
        m_oPapel.Line (xIni, yInicio)-(xFin, yFinal)
        yInicio = yInicio + rIncr
        yFinal = yFinal + rIncr
    Next i
    
}

void lmScoreCanvas::PintarMetronomo(fMeasuring As Boolean, nTipoNotaIni As EMetronomo, _
        nTipoNotaFin As EMetronomo, nVelocidad As Long)
        
    if Not fMeasuring {
        Dim sGlifos As String
        Select case nTipoNotaIni
            case eMtr_Negra
                sGlifos = "_["
            case eMtr_NegraPuntillo
                sGlifos = "`["
            case } else {
                MsgBox "[PintarMetronomo] Tipo de nota (" & nTipoNotaIni & ") no contemplado"
        }
        if nVelocidad = 0 {
            Select case nTipoNotaFin
                case eMtr_Negra
                    sGlifos = sGlifos & "_"
                case eMtr_NegraPuntillo
                    sGlifos = sGlifos & "`"
                case } else {
                    MsgBox "[PintarMetronomo] Tipo de nota (" & nTipoNotaFin & ") no contemplado"
            }
            Escribir sGlifos, m_xCursor - 31# * xUnits, m_yCursor - 13 * m_ndyLines
        } else {
            //falta: escritura de velocidad
        }
    }
    
}

void lmScoreCanvas::PintarRespiracion(fMeasuring As Boolean)

    //Al llegar aqui está posicionado justo tras la nota
    Dim sGlyph As String
    sGlyph = CHAR_RESPIRACION
    m_xCursor = m_xCursor + DrawGlyphs(fMeasuring, sGlyph, CLng(m_xCursor + m_ndyLines), CLng(m_yCursor - 11.5 * m_ndyLines))
    
}





void lmScoreCanvas::Avanzar(Optional rEspacio As Single = 0)
    if rEspacio = 0 {
        m_xCursor = m_xCursor + 1.5 * m_ndyLines
    } else {
        m_xCursor = m_xCursor + rEspacio
    }
    
}

void lmScoreCanvas::EscribirTitulo(sTexto As String)
    With m_oPapel
        .FontName = "Times New Roman"
        .Font.Size = 14
        .Font.Bold = True
        .Font.Italic = True
        .CurrentX = m_nLeftMarging * m_rEscala
        .CurrentY = m_nTop
        m_oPapel.Print sTexto
        
        .FontName = "LeMus Notas"
        .Font.Size = m_nPointSize   //24
        .Font.Bold = False
        .Font.Italic = False
    }
}

void lmScoreCanvas::EscribirTexto(sTexto As String, nxLeft As Long, nyTop As Long, _
        sFontName As String, nFontSize As Long, fBold As Boolean, fItalic As Boolean)
        
    With m_oPapel
        .FontName = sFontName
        .Font.Size = nFontSize
        .Font.Bold = fBold
        .Font.Italic = fItalic
        .CurrentX = nxLeft
        .CurrentY = nyTop - 12
        m_oPapel.Print sTexto
        
        .FontName = "LeMus Notas"
        .Font.Size = m_nPointSize
        .Font.Bold = False
        .Font.Italic = False
    }
}

void lmScoreCanvas::DibujarRepeticion(fMeasuring As Boolean, nNum As Long, nxLeft As Long, nyTop As Long)
    if fMeasuring { Exit Sub
    
    Dim rX As Single, rY As Single
    rX = CSng(nxLeft)
    rY = CSng(nyTop)
    
    //línea horizontal
    Me.PintarLinea fMeasuring, rX, rY, rX + 80#, rY
    //linea vertical
    Me.PintarLinea fMeasuring, rX, rY, rX, rY + 10#
    //Número de la repeticion
    Me.EscribirTexto nNum & ".", nxLeft + m_ndyLines / 3, nyTop + 1.5 * m_ndyLines, "Arial", m_nPointSize / 3, False, False
    
}

void lmScoreCanvas::DibujarSigno(fMeasuring As Boolean, nSigno As EDirectivasRepeticion, nxLeft As Long, nyTop As Long)
    
    if fMeasuring { Exit Sub
    
    Dim sGlyph As String, nX As Long, nY As Long
    
    nX = nxLeft
    nY = nyTop
    Select case nSigno
        case eDR_DaCapo
            Me.EscribirTexto "Da Capo", nxLeft, nyTop, m_sFontSigno, m_nPointSize / 2, False, True
            Exit Sub
        case eDR_DC
            sGlyph = CHAR_DACAPO
            nY = nY - 47
        case eDR_Fine
            Me.EscribirTexto "Fine", nxLeft, nyTop, m_sFontSigno, m_nPointSize / 2, False, True
            Exit Sub
        case eDR_Segno
            sGlyph = CHAR_SEGNO
            nY = nY - 47
        case eDR_Coda
            sGlyph = CHAR_CODA
            nY = nY - 47
        case eDR_DalSegno
            Me.EscribirTexto "Dal Segno", nxLeft, nyTop, m_sFontSigno, m_nPointSize / 2, False, True
            Exit Sub
        case eDR_DS
            sGlyph = CHAR_DALSEGNO
            nY = nY - 47
        case eDR_AlCoda
            Me.EscribirTexto " al Coda", nxLeft, nyTop, m_sFontSigno, m_nPointSize / 2, False, True
            Exit Sub
        case eDR_AlFine
            Me.EscribirTexto "D.S. al Fine", nxLeft, nyTop, m_sFontSigno, m_nPointSize / 2, False, True
            Exit Sub
        case } else {
            Debug.Assert False
    }
            Escribir sGlyph, nX, nY
    
}

//devuelve el valor en unidades del lienzo (i.e. pixels)
Public Function EscalarDecimas(nDecimas As Long) As Single
    EscalarDecimas = (CSng(nDecimas) * m_ndyLines) / 10#
End Function

void lmScoreCanvas::FinalizarDocumento()
    if TypeOf m_oPapel Is Printer {
        m_oPapel.EndDoc
    } else {
        //Guarda la imagen en m_picPapel
        //Set m_picPapel = m_oPapel.Picture
//        With m_oPapel.Picture
//            BitBlt m_picPapel.Handle, 0, 0, .Width, .Height, .Handle, 0, 0, vbSrcCopy
//        }
    }
}

Private Sub Escribir(sTexto As String, nxPos As Long, nyPos As Long)
    With m_oPapel
        .CurrentX = nxPos
        .CurrentY = nyPos
        m_oPapel.Print sTexto
    }
    
}

//==========================================================================================

Private Function FUnitsToTwips(nFUnits As Long) As Long
    //pixels por em (ppem) = Redondear(PointSize * Resolucion /  72)
    //pixels = Redondear((FUnits / Units_per_em) * ppem)
    //Twips = pixels * 1440 / Resolucion

    Dim ppem  As Long, nPixels As Long
    
    ppem = Round((m_nPointSize * nResolucion) / 72, 0)
    nPixels = Round((nFUnits * ppem) / nUnitsPerEM, 0)
    FUnitsToTwips = nPixels * 1440 \ nResolucion
    
End Function

*/



