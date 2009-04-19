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
#pragma implementation "Text.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "Text.h"
#include "UndoRedo.h"
#include "properties/DlgProperties.h"
#include "properties/TextProperties.h"
#include "../graphic/GMObject.h"
#include "../graphic/ShapeText.h"
#include "../graphic/ShapeLine.h"
#include "../ldp_parser/AuxString.h"


//Aux. function to convert font pointsize to lmLUnits
lmLUnits PointsToLUnits(int nPoints)
{
    //One point equals 1/72 of an inch
    //One inch equals 2.54 cm = 25.4 mm
    //then 1 pt = 25.4/72 mm
    return lmToLogicalUnits(nPoints * 25.4 / 72.0, lmMILLIMETERS);
}

//Inverse function: convert font points to lmLUnits
int LUnitsToPoints(lmLUnits uUnits)
{
    //One point equals 1/72 of an inch
    //One inch equals 2.54 cm = 25.4 mm
    //then 1 pt = 25.4/72 mm
    //and 1mm = 72/25.4 pt
    return (int)(0.5 + lmLogicalToUserUnits(uUnits * 72.0 / 25.4, lmMILLIMETERS) );
}



//==========================================================================================
// lmBasicText implementation
//==========================================================================================

lmBasicText::lmBasicText(wxString& sText, lmLocation& tPos, lmTextStyle* pStyle,
                         const wxString& sLanguage)
    : m_sText(sText)
    , m_pStyle(pStyle)
    , m_sLanguage(sLanguage)
    , m_tTextPos(tPos)
{
}

lmBasicText::~lmBasicText()
{
}



//==========================================================================================
// lmScoreText implementation
//==========================================================================================

lmScoreText::lmScoreText(wxString& sTitle, lmEHAlign nHAlign, lmTextStyle* pStyle)
    : lmAuxObj(lmDRAGGABLE),
      lmBasicText(sTitle, g_tDefaultPos, pStyle)
{
    m_nBlockAlign = lmBLOCK_ALIGN_NONE;
    m_nHAlign = nHAlign;
    m_nVAlign = lmVALIGN_DEFAULT;
}

lmUPoint lmScoreText::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	return uPos;
}

wxFont* lmScoreText::GetSuitableFont(lmPaper* pPaper)
{
    //wxLogMessage(_T("[lmScoreText::GetSuitableFont]: size=%d, name=%s"),
	//             m_nFontSize, m_sFontName );

    int nWeight = m_pStyle->tFont.nFontWeight;
    int nStyle = m_pStyle->tFont.nFontStyle;
    wxFont* pFont = pPaper->GetFont((int)PointsToLUnits(m_pStyle->tFont.nFontSize),
                                    m_pStyle->tFont.sFontName, wxDEFAULT, nStyle,
                                    nWeight, false);

    if (!pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmScoreText::GetSuitableFont"), wxOK);
        ::wxExit();
    }
	return pFont;
}

void lmScoreText::OnProperties(lmController* pController, lmGMObject* pGMO)
{
	wxASSERT(pGMO);

    lmDlgProperties dlg(pController);
    dlg.AddPanel( new lmTextProperties(dlg.GetNotebook(), this),
                  _("Text"));
    dlg.Layout();

    dlg.ShowModal();
}

//void lmScoreText::EditText(lmScore* pScore)
//{
//    lmDlgProperties dlg((lmController*)NULL);
//    dlg.AddPanel( new lmTextProperties(dlg.GetNotebook(), this), _("Text"));
//    dlg.Layout();
//
//    dlg.ShowModal();
//}

void lmScoreText::OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName)
{
	//invoked to add specific panels to the dialog

	if (sTabName == wxEmptyString)
        pDlg->AddPanel( new lmTextProperties(pDlg->GetNotebook(), this), _("Text"));
    else
        pDlg->AddPanel( new lmTextProperties(pDlg->GetNotebook(), this), sTabName);
}

void lmScoreText::Cmd_ChangeText(lmUndoItem* pUndoItem, wxString& sText, lmEHAlign nAlign,
                                 lmLocation tPos, lmTextStyle* pTS)
{
    m_sText = sText;
    m_nHAlign = nAlign;
    m_tTextPos = tPos;
    m_pStyle = pTS;
}

void lmScoreText::UndoCmd_ChangeText(lmUndoItem* pUndoItem, wxString& sText,
                                     lmEHAlign nAlign, lmLocation tPos,
                                     lmTextStyle* pTS)
{
    Cmd_ChangeText(pUndoItem, sText, nAlign, tPos, pTS);
}



//==========================================================================================
// lmTextItem implementation
//==========================================================================================

lmTextItem::lmTextItem(wxString& sTitle, lmEHAlign nHAlign, lmTextStyle* pStyle)
    : lmScoreText(sTitle, nHAlign, pStyle)
{
    m_nBlockAlign = lmBLOCK_ALIGN_NONE;
    m_nVAlign = lmVALIGN_DEFAULT;
}

lmShape* lmTextItem::CreateShape(lmPaper* pPaper, lmUPoint uPos)
{
    // Creates the shape and returns it

    lmShapeText* pGMObj =
        new lmShapeText(this, m_sText, GetSuitableFont(pPaper), pPaper,
                        uPos, _T("ScoreText"), lmDRAGGABLE, m_pStyle->nColor);

    StoreShape(pGMObj);
    return pGMObj;
}

lmLUnits lmTextItem::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model.

	WXUNUSED(colorC);

    //create the shape object
    lmShape* pShape = CreateShape(pPaper, uPos);

    //According to LDP specifications [LDP manual, 3.6. The Text element] default text
    //anchor pos is at bottom left corner. But text shape anchor pos is top left corner.
    //Therefore it is necessary to shift the shape upwards by text height.
    lmLUnits uHeight = pShape->GetHeight();
    pShape->Shift(0.0f, -uHeight);

    //add shape to graphic model
	pBox->AddShape(pShape);

	// set total width
	return pShape->GetWidth();
}

wxString lmTextItem::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tText '%s'"), m_nId, m_sText.Left(15).c_str() );

    sDump += lmAuxObj::Dump();
    sDump += _T("\n");
    return sDump;
}

wxString lmTextItem::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(text");

    //text goes after main tag
    sSource += _T(" \"");
    sSource += m_sText;
    sSource += _T("\"");

    //alignment
    if (m_nHAlign == lmHALIGN_CENTER)
        sSource += _T(" center");
    else if (m_nHAlign == lmHALIGN_LEFT)
        sSource += _T(" left");
    else
        sSource += _T(" right");

    //style info
    sSource += _T(" (style \"");
    sSource += m_pStyle->sName;
    sSource += _T("\")");

	//base class info
    sSource += lmAuxObj::SourceLDP(nIndent);

    //close element
    sSource += _T(")\n");

    return sSource;
}

wxString lmTextItem::SourceXML(int nIndent)
{
    //TODO
    wxString sSource = _T("TODO: lmTextItem XML Source code generation methods");
    return sSource;

////    <direction placement="above">
////      <direction-type>
////        <words xml:lang="la" relative-y="5" relative-x="-5">Angelus
//// dicit:</words>
////      </direction-type>
////    </direction>
//
//    sFuente = "<direction placement=""?"">" & sCrLf & _
//                "  <direction-type>" & sCrLf & _
//                "    <words xml:lang=""??"" relative-y=""??"" relative-x=""??"">" & _
//                    m_sTexto & "</words>" & sCrLf & _
//                "  </direction-type>" & sCrLf & _
//                "<direction>"
//
}



//==========================================================================================
// lmInstrNameAbbrev implementation
//==========================================================================================

wxString lmInstrNameAbbrev::SourceLDP(wxString sTag)
{
    wxString sSource = _T("(");
    sSource += sTag;

    //text goes after main tag
    sSource += _T(" \"");
    sSource += m_sText;
    sSource += _T("\"");

    //style info
    sSource += _T(" (style \"");
    sSource += m_pStyle->sName;
    sSource += _T("\")");

	//base class info
    sSource += lmAuxObj::SourceLDP(0);

    //close element
    sSource += _T(")");

    return sSource;
}



//==========================================================================================
// lmScoreTitle implementation
//==========================================================================================

lmScoreTitle::lmScoreTitle(wxString& sTitle, lmEBlockAlign nBlockAlign, lmEHAlign nHAlign,
                         lmEVAlign nVAlign, lmTextStyle* pStyle)
    : lmScoreText(sTitle, nHAlign, pStyle)
{
    m_nBlockAlign = nBlockAlign;
    m_nVAlign = nVAlign;
}

lmShape* lmScoreTitle::CreateShape(lmPaper* pPaper, lmUPoint uPos)
{
    // Creates the shape and returns it

    lmShapeTitle* pGMObj
        = new lmShapeTitle(this, m_sText, GetSuitableFont(pPaper), pPaper,
                               m_nBlockAlign, m_nHAlign, m_nVAlign,
                               uPos.x, uPos.y, 0.0f, 0.0f, m_pStyle->nColor);

    StoreShape(pGMObj);
    return pGMObj;
}

lmLUnits lmScoreTitle::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model.

	WXUNUSED(colorC);

    //create the shape object
    lmShape* pShape = CreateShape(pPaper, uPos);

    //add shape to graphic model
	pBox->AddShape(pShape);

	// set total width
	return pShape->GetWidth();
}

wxString lmScoreTitle::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tTitle '%s'"), m_nId, m_sText.Left(15).c_str() );

    sDump += lmAuxObj::Dump();
    sDump += _T("\n");
    return sDump;
}

wxString lmScoreTitle::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(title");

    //alignment
    if (m_nHAlign == lmHALIGN_CENTER)
        sSource += _T(" center");
    else if (m_nHAlign == lmHALIGN_LEFT)
        sSource += _T(" left");
    else
        sSource += _T(" right");

    //text goes after alignment
    sSource += _T(" \"");
    sSource += m_sText;
    sSource += _T("\"");

    //style info
    sSource += _T(" (style \"");
    sSource += m_pStyle->sName;
    sSource += _T("\")");

	//base class info
    sSource += lmAuxObj::SourceLDP(nIndent);

    //close element
    sSource += _T(")\n");
    return sSource;
}

wxString lmScoreTitle::SourceXML(int nIndent)
{
    //TODO
    wxString sSource = _T("TODO: lmScoreTitle XML Source code generation methods");
    return sSource;
}



//==========================================================================================
// lmBaseText implementation: Single piece of text plus its style
//==========================================================================================

lmBaseText::lmBaseText(const wxString& sText, lmTextStyle* pStyle)
    : m_sText(sText)
    , m_pStyle(pStyle)
{
    wxASSERT(sText != _T("") && pStyle); 
}

wxFont* lmBaseText::GetSuitableFont(lmPaper* pPaper)
{
    //wxLogMessage(_T("[lmBaseText::GetSuitableFont]: size=%d, name=%s"),
	//             m_nFontSize, m_sFontName );

    int nWeight = m_pStyle->tFont.nFontWeight;
    int nStyle = m_pStyle->tFont.nFontStyle;
    wxFont* pFont = pPaper->GetFont((int)PointsToLUnits(m_pStyle->tFont.nFontSize),
                                    m_pStyle->tFont.sFontName, wxDEFAULT, nStyle,
                                    nWeight, false);

    if (!pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmBaseText::GetSuitableFont"), wxOK);
        ::wxExit();
    }
	return pFont;
}

wxString lmBaseText::SourceLDP(int nIndent)
{
    WXUNUSED(nIndent);

    wxString sSource = _T("\"");
    sSource += m_sText;
    sSource += _T("\"");

    //style info
    sSource += _T(" (style \"");
    sSource += m_pStyle->sName;
    sSource += _T("\")");

    return sSource;
}

wxString lmBaseText::SourceXML(int nIndent)
{
    //TODO
    return wxEmptyString;
}

wxString lmBaseText::Dump()
{
    //TODO
    return wxEmptyString;
}



//==========================================================================================
// lmScoreBlock implementation: An AuxObj modelling an abstract block: box (a
//      rectangle) plus alligment attributes
//==========================================================================================

lmScoreBlock::lmScoreBlock(lmTenths ntWidth, lmTenths ntHeight, lmTPoint ntPos,
                           lmEBlockAlign nBlockAlign,
                           lmEHAlign nHAlign, lmEVAlign nVAlign)
    : lmAuxObj(lmDRAGGABLE)
    , m_ntWidth(ntWidth)
    , m_ntHeight(ntHeight)
    , m_ntPos(ntPos)
    , m_nBlockAlign(nBlockAlign)
    , m_nHAlign(nHAlign)
    , m_nVAlign(nVAlign)
    //anchor line
    , m_fHasAnchorLine(false)
    , m_nAnchorLineStyle(lm_eLine_None)
    , m_nAnchorLineEndStyle(lm_eEndLine_None)
    , m_nAnchorLineColor(*wxBLACK)
    , m_ntAnchorLineWidth(1.0f)
    //default block looking 
    , m_nBgColor(*wxWHITE)
    , m_nBorderColor(*wxBLACK)
    , m_ntBorderWidth(1.0f)
    , m_nBorderStyle(lm_eLine_None)
    //shapes
    , m_pShapeRectangle((lmShapeRectangle*)NULL)
    , m_pShapeLine((lmShapeLine*)NULL)
{
}

void lmScoreBlock::AddAnchorLine(lmLocation tPoint, lmTenths ntWidth, lmELineStyle nStyle,
                                 lmELineEndStyle nEndStyle, wxColour nColor)
{
    m_fHasAnchorLine = true;
    m_ntAnchorPoint = lmTPoint(tPoint.x, tPoint.y);
    m_nAnchorLineStyle = nStyle;
    m_nAnchorLineColor = nColor;
    m_ntAnchorLineWidth = ntWidth;
    m_nAnchorLineEndStyle = nEndStyle;
}

void lmScoreBlock::MoveObjectPoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts,
                                    bool fAddShifts)
{
    //This method is only used during interactive edition.
    //It receives a vector with the shifts for object points and a flag to signal
    //whether to add or to substract shifts.

    if (nShapeIdx == lmIDX_RECTANGLE)
    {
        //wxLogMessage(_T("[lmScoreBlock::MoveObjectPoints] Before. Rectangle: Left-top=(%.2f, %.2f), right-bottom=(%.2f, %.2f), join=(%.2f, %.2f)"),
        //    m_ntPos.x, m_ntPos.y, m_ntPos.x + m_ntWidth, m_ntPos.y + m_ntHeight,
        //    m_ntAnchorJoinPoint.x, m_ntAnchorJoinPoint.y );

        //moved points belongs to the rectangle
        //receives 4 points: top-left, top-right, bottom-right and bottom-left
        wxASSERT(nNumPoints == 4);

        lmUPoint uTopLeftShift = *pShifts;
        lmUPoint uBottomRightShift = *(pShifts+2);
        lmLUnits uWidthShift = uBottomRightShift.x - uTopLeftShift.x;
        lmLUnits uHeightShift = uBottomRightShift.y - uTopLeftShift.y;
        if (fAddShifts)
        {
            m_ntPos.x += m_pParent->LogicalToTenths(uTopLeftShift.x);
            m_ntPos.y += m_pParent->LogicalToTenths(uTopLeftShift.y);
            m_ntWidth += m_pParent->LogicalToTenths(uWidthShift);
            m_ntHeight += m_pParent->LogicalToTenths(uHeightShift);
        }
        else
        {
            m_ntPos.x -= m_pParent->LogicalToTenths(uTopLeftShift.x);
            m_ntPos.y -= m_pParent->LogicalToTenths(uTopLeftShift.y);
            m_ntWidth -= m_pParent->LogicalToTenths(uWidthShift);
            m_ntHeight -= m_pParent->LogicalToTenths(uHeightShift);
        }

        //inform the rectangle shape
        m_pShapeRectangle->MovePoints(nNumPoints, nShapeIdx, pShifts, fAddShifts);

        //If there is an anchor line, move also line point attached to rectangle
        if (m_fHasAnchorLine)
            MoveJoinPoint();
    }
    else
    {
        //moved points belong to the anchor line
        //receives 2 points: start, end.
        //As end point is fixed only start point can be moved
        wxASSERT(nShapeIdx == lmIDX_ANCHORLINE);
        wxASSERT(nNumPoints == 2);
        wxASSERT(m_fHasAnchorLine);

        if (fAddShifts)
        {
            m_ntAnchorPoint.x += m_pParent->LogicalToTenths((*pShifts).x);
            m_ntAnchorPoint.y += m_pParent->LogicalToTenths((*pShifts).y);
        }
        else
        {
            m_ntAnchorPoint.x -= m_pParent->LogicalToTenths((*pShifts).x);
            m_ntAnchorPoint.y -= m_pParent->LogicalToTenths((*(pShifts)).y);
        }

        //inform the shape
        wxASSERT(m_pShapeLine);
        m_pShapeLine->MovePoints(nNumPoints, nShapeIdx, pShifts, fAddShifts);
        MoveJoinPoint();
    }
}

void lmScoreBlock::OnParentComputedPositionShifted(lmLUnits uxShift, lmLUnits uyShift)
{
    //This method is invoked only from lmScoreObj::StoreOriginAndShiftShapes() to inform
    //about a change in the computed final position for parent ScoreObj. Therefore, it
    //is necessary to update this AuxObj reference pos and both shapes.

	m_uComputedPos.x += uxShift;
	m_uComputedPos.y += uyShift;

    //shift shapes
    if (m_pShapeRectangle)
    {
        m_pShapeRectangle->Shift(uxShift, uyShift);
        m_pShapeRectangle->ApplyUserShift( this->GetUserShift() );
    }
	if (m_pShapeLine)
    {
        m_pShapeLine->Shift(uxShift, uyShift);
        m_pShapeLine->ApplyUserShift( this->GetUserShift() );
    }
}

void lmScoreBlock::OnParentMoved(lmLUnits uxShift, lmLUnits uyShift)
{
	//TODO: specific flag to decouple from parent staffObj, so the user can
	//control if the attached AuxObj will be moved with the parent or not

	if (m_pShapeRectangle)
    {
        lmUPoint uUserShift = GetUserShift(lmIDX_RECTANGLE);
	    uUserShift.x += uxShift;
	    uUserShift.y += uyShift;
        this->SaveUserLocation(uUserShift.x, uUserShift.y, 0);
        m_pShapeRectangle->ApplyUserShift(uUserShift);
    }
	if (m_pShapeLine)
    {
        lmUPoint uUserShift = GetUserShift(lmIDX_ANCHORLINE);
	    uUserShift.x += uxShift;
	    uUserShift.y += uyShift;
        this->SaveUserLocation(uUserShift.x, uUserShift.y, 0);
        m_pShapeLine->ApplyUserShift(uUserShift);
    }
}

void lmScoreBlock::MoveJoinPoint()
{
    //The rectangle or the anchor line start point has been moved. This method is then invoked
    //to recompute the new join point between line and rectangle

    wxASSERT(m_pShapeLine);
    lmTPoint ntNewJoinPoint;
    if (ComputeAnchorJoinPoint(&ntNewJoinPoint))
    {
        //anchor line is hidden. Move join point to top-left
        ntNewJoinPoint = m_ntPos;
        m_pShapeLine->SetVisible(false);
    }
    else
        m_pShapeLine->SetVisible(true);

    lmUPoint uLineShifs[2];
    uLineShifs[0] = lmUPoint(0.0f, 0.0f);
    uLineShifs[1].x = m_pParent->TenthsToLogical(ntNewJoinPoint.x - m_ntAnchorJoinPoint.x);
    uLineShifs[1].y = m_pParent->TenthsToLogical(ntNewJoinPoint.y - m_ntAnchorJoinPoint.y);
    m_ntAnchorJoinPoint = ntNewJoinPoint;
    m_pShapeLine->MovePoints(2, lmIDX_ANCHORLINE, &uLineShifs[0], true);    //true -> add shifts
}

bool lmScoreBlock::ComputeAnchorJoinPoint(lmTPoint* ptJoin)
{
    //Anchor line join point (the point at which the anchor line is attached to
    //the rectangle border) will be placed so that anchor line always passes
    //through the center of rectangle.
    //
    //This method computes the rectangle join point as follows:
    //  1. if anchor point is inside the rectangle, finish. Anchor point is hidden
    //  2. Compute center point of rectangle.
    //  3. Compute line anchor point to center point
    //  4. Compute intersection point to nearest vertical side
    //  5. if point is on side (line segment), finish. Anchor point is found
    //  6. Compute intersection point to nearest horizontal side
    //
    // Returns true if anchor point is hidden by rectangle. Otherwise content of
    // ptJoin is updated with found attachment point


    //wxLogMessage(_T("[lmScoreBlock::ComputeAnchorJoinPoint] Before. join=(%.2f, %.2f)"),
    //    m_ntAnchorJoinPoint.x, m_ntAnchorJoinPoint.y );

    //Rectangle bottom-right point
    lmTPoint ntRight(m_ntPos.x + m_ntWidth, m_ntPos.y + m_ntHeight);

    //1. if anchor point is inside the rectangle, finish. Anchor point is hidden
    if (m_ntAnchorPoint.x >= m_ntPos.x && m_ntAnchorPoint.x <= ntRight.x
        && m_ntAnchorPoint.y >= m_ntPos.y && m_ntAnchorPoint.y <= ntRight.y)
        return true;    //anchor point is inside the rectangle

    //2. Compute center point of rectangle
    lmTPoint ntCenter(m_ntPos.x + m_ntWidth/2.0f, m_ntPos.y + m_ntHeight/2.0f);
    
    //3. Compute slope of anchor line
    float m = (ntCenter.y - m_ntAnchorPoint.y) / (ntCenter.x - m_ntAnchorPoint.x);

    //4. Compute intersection point to nearest rectangle vertical side
    lmTPoint ntIntersect(m_ntPos.x, 0.0f);       //assume left side is nearer
    if (m_ntAnchorPoint.x > ntCenter.x)
        ntIntersect.x = ntRight.x;
    
    ntIntersect.y = m * (ntIntersect.x - m_ntAnchorPoint.x) + m_ntAnchorPoint.y;

    //5. if point is on side (line segment), finish. Anchor point is found
    if (ntIntersect.y >= m_ntPos.y && ntIntersect.y <= ntRight.y)
    {
        //found
        *ptJoin = ntIntersect;
        return false;       //intersection found
    }

    // 6. Compute intersection point to nearest horizontal side
    ntIntersect.y = m_ntPos.y;          //assume top side is nearer
    if (m_ntAnchorPoint.y > ntCenter.y)
        ntIntersect.y = ntRight.y;      //bottom side is nearer

    ntIntersect.x = m_ntAnchorPoint.x + (ntIntersect.y - m_ntAnchorPoint.y) / m;

    *ptJoin = ntIntersect;
    return false;       //intersection found
}


//==========================================================================================
// lmScoreTextParagraph implementation: box + alignment + collection of lmBaseText
//==========================================================================================

lmScoreTextParagraph::lmScoreTextParagraph(lmTenths ntWidth, lmTenths ntHeight,
                                           lmTPoint tPos)
    : lmScoreBlock(ntWidth, ntHeight, tPos)
{
    DefineAsMultiShaped();      //multi-shaped: box (#0) + anchor line (#1)
}

lmScoreTextParagraph::~lmScoreTextParagraph()
{
    //delete text units
    std::list<lmBaseText*>::iterator it;
    for (it = m_texts.begin(); it != m_texts.end(); ++it)
        delete *it;
    m_texts.clear();
}

void lmScoreTextParagraph::Defragment()
{
    //A text paragraph containing text with the same style contains just one lmBaseText object. 
    //When styling is applied to part of this object, the object is decomposed into separate 
    //objects, one lmBaseText for each different character style. This can lead to 
    //fragmentation after a lot of edit operations, potentially leading to several objects 
    //with the same style where just one would do. 
    //This Defragment ensures that the minimum number of lmBaseText objects is used.

    //TODO
}

void lmScoreTextParagraph::InsertTextUnit(lmBaseText* pText)
{
    //TODO: this code just adds the text at the end
    wxASSERT(pText);
    m_texts.push_back(pText);
    Defragment();
}

wxString lmScoreTextParagraph::SourceLDP(int nIndent)
{
    //TODO
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(paragraph ");

    //location and alignment
    sSource += _T("dx:");
	sSource += DoubleToStr((double)m_ntPos.x, 4);
    sSource += _T(" dy:");
	sSource += DoubleToStr((double)m_ntPos.y, 4);
    if (m_nHAlign == lmHALIGN_CENTER)
        sSource += _T(" center");
    else if (m_nHAlign == lmHALIGN_LEFT)
        sSource += _T(" left");
    else
        sSource += _T(" right");
    sSource += _T("\n");

    //box
    ++nIndent;
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(size (width ");
	sSource += DoubleToStr((double)m_ntWidth, 4);
    sSource += _T(")(height ");
	sSource += DoubleToStr((double)m_ntHeight, 4);
    sSource += _T("))(color ");
    sSource += m_nBgColor.GetAsString(wxC2S_HTML_SYNTAX);
    sSource += _T(")\n");

    //border
    //<border> ::= (border <width><lineStyle><color>)
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(border (width ");
	sSource += DoubleToStr((double)m_ntBorderWidth, 4);
    sSource += _T(")(lineStyle ") + LineStyleToLDP(m_nBorderStyle);
    sSource += _T(")");
    sSource += _T("(color ");
    sSource += m_nBorderColor.GetAsString(wxC2S_HTML_SYNTAX);
    sSource += _T("))");

    //text
    std::list<lmBaseText*>::iterator it;
    for (it = m_texts.begin(); it != m_texts.end(); ++it)
    {
        sSource += _T("\n");
        sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
        sSource += _T("(text ");
        sSource += (*it)->SourceLDP(0);
        sSource += _T(")");
    }
    --nIndent;

    //anchor line
    if (m_fHasAnchorLine)
    {
        ++nIndent;
        std::list<lmBaseText*>::iterator it;
        for (it = m_texts.begin(); it != m_texts.end(); ++it)
        {
            sSource += _T("\n");
            sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
            sSource += _T("(anchorLine dx:");
		    sSource += DoubleToStr((double)m_ntAnchorPoint.x, 4);
            sSource += _T(" dy:");
		    sSource += DoubleToStr((double)m_ntAnchorPoint.y, 4);
            sSource += _T(" (lineStyle ") + LineStyleToLDP(m_nAnchorLineStyle);
            sSource += _T(")");
            sSource += _T("(color ");
            sSource += m_nAnchorLineColor.GetAsString(wxC2S_HTML_SYNTAX);
            sSource += _T(")");
            sSource += _T("(width ");
		    sSource += DoubleToStr((double)m_ntAnchorLineWidth, 4);
            sSource += _T(")");
            sSource += _T(")");
        }
        --nIndent;
    }

	//base class info
    sSource += _T("\n");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += lmAuxObj::SourceLDP(nIndent);

    //close element
    sSource += _T(")\n");
    return sSource;
}

wxString lmScoreTextParagraph::SourceXML(int nIndent)
{
    //TODO
    return wxEmptyString;
}

wxString lmScoreTextParagraph::Dump()
{
    //TODO
    return wxEmptyString;
}

lmLUnits lmScoreTextParagraph::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // Creates the shape and returns it

    WXUNUSED(colorC);

    //rectangle position
    lmLUnits uxStart = m_pParent->TenthsToLogical(m_ntPos.x) + pPaper->GetCursorX();
    lmLUnits uyStart = m_pParent->TenthsToLogical(m_ntPos.y) + pPaper->GetCursorY();
    lmLUnits uxEnd = uxStart + m_pParent->TenthsToLogical(m_ntWidth);
    lmLUnits uyEnd = uyStart + m_pParent->TenthsToLogical(m_ntHeight);
    lmLUnits uBorderWidth = m_pParent->TenthsToLogical(m_ntBorderWidth);

    //create the textbox shape
    std::list<lmBaseText*>::iterator it;
    //for (it = m_texts.begin(); it != m_texts.end(); ++it)
    it = m_texts.begin();
    wxFont* pFont = (*it)->GetSuitableFont(pPaper);

    //create the textbox shape (shape #0)
    m_pShapeRectangle = 
        new lmShapeTextbox(this, lmIDX_RECTANGLE, pPaper, (*it)->GetText(), pFont,
                           (*it)->GetColour(), m_nBlockAlign, m_nHAlign, m_nVAlign,
                           uxStart, uyStart, uxEnd, uyEnd,
                           m_nBgColor, uBorderWidth, m_nBorderColor);
	pBox->AddShape(m_pShapeRectangle);
    StoreShape(m_pShapeRectangle);

    //if it has anchor line create the line shape
    if (m_fHasAnchorLine)
    {
        //anchor line (shape #1)
        m_ntAnchorJoinPoint = m_ntPos;
        bool fHidden = ComputeAnchorJoinPoint(&m_ntAnchorJoinPoint);
        //wxLogMessage(_T("[lmScoreTextParagraph::LayoutObject] Join=(%.2f, %.2f)"),
        //    m_ntAnchorJoinPoint.x, m_ntAnchorJoinPoint.y );

        lmLUnits uxStartAnchor = m_pParent->TenthsToLogical(m_ntAnchorPoint.x) + pPaper->GetCursorX();
        lmLUnits uyStartAnchor = m_pParent->TenthsToLogical(m_ntAnchorPoint.y) + pPaper->GetCursorY();
        lmLUnits uxEndAnchor = m_pParent->TenthsToLogical(m_ntAnchorJoinPoint.x) + pPaper->GetCursorX();
        lmLUnits uyEndAnchor = m_pParent->TenthsToLogical(m_ntAnchorJoinPoint.y) + pPaper->GetCursorY();
        lmLUnits uAnchorLineWidth = m_pParent->TenthsToLogical(m_ntAnchorLineWidth);
        lmLUnits uExtraWidth = m_pParent->TenthsToLogical(4);   //TODO: User options?
        m_pShapeLine =
            new lmShapeLine(this, lmIDX_ANCHORLINE,
                            uxStartAnchor, uyStartAnchor, uxEndAnchor, uyEndAnchor, uAnchorLineWidth,
                            uExtraWidth, m_nAnchorLineStyle, m_nAnchorLineColor);
        m_pShapeLine->SetAsControlled(lmLINE_END);
        m_pShapeLine->SetLeftDraggable(false);
        m_pShapeLine->SetVisible(!fHidden);

	    pBox->AddShape(m_pShapeLine);
        StoreShape(m_pShapeLine);
    }
    return m_pShapeRectangle->GetBounds().GetWidth();
}



//==========================================================================================
// lmScoreTextBox implementation
//==========================================================================================

lmScoreTextBox::lmScoreTextBox()
    : lmScoreBlock()
{
}

lmScoreTextBox::~lmScoreTextBox()
{
    //delete text units
    std::list<lmScoreTextParagraph*>::iterator it;
    for (it = m_paragraphs.begin(); it != m_paragraphs.end(); ++it)
        delete *it;
    m_paragraphs.clear();
}

//lmShape* lmScoreTextBox::CreateShape(lmPaper* pPaper, lmUPoint uPos)
//{
//    // Creates the shape and returns it
//
//    lmShapeTitle* pGMObj
//        = new lmShapeTitle(this, m_sText, GetSuitableFont(pPaper), pPaper,
//                               m_nBlockAlign, m_nHAlign, m_nVAlign,
//                               uPos.x, uPos.y, 0.0f, 0.0f, m_pStyle->nColor);
//
//    StoreShape(pGMObj);
//    return pGMObj;
//}
//
//lmLUnits lmScoreTextBox::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
//{
//    // This method is invoked by the base class (lmStaffObj). It is responsible for
//    // creating the shape object and adding it to the graphical model.
//
//	WXUNUSED(colorC);
//
//    //create the shape object
//    lmShape* pShape = CreateShape(pPaper, uPos);
//
//    //add shape to graphic model
//	pBox->AddShape(pShape);
//
//	// set total width
//	return pShape->GetWidth();
//}
//
//wxString lmScoreTextBox::Dump()
//{
//    wxString sDump = wxString::Format(
//        _T("%d\tTitle '%s'"), m_nId, m_sText.Left(15).c_str() );
//
//    sDump += lmAuxObj::Dump();
//    sDump += _T("\n");
//    return sDump;
//}
//
//wxString lmScoreTextBox::SourceLDP(int nIndent)
//{
//    wxString sSource = _T("");
//    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
//    sSource += _T("(textbox");
//
//    //alignment
//    if (m_nHAlign == lmHALIGN_CENTER)
//        sSource += _T(" center");
//    else if (m_nHAlign == lmHALIGN_LEFT)
//        sSource += _T(" left");
//    else
//        sSource += _T(" right");
//
//    //text goes after alignment
//    sSource += _T(" \"");
//    sSource += m_sText;
//    sSource += _T("\"");
//
//    //style info
//    sSource += _T(" (style \"");
//    sSource += m_pStyle->sName;
//    sSource += _T("\")");
//
//	//base class info
//    sSource += lmAuxObj::SourceLDP(nIndent);
//
//    //close element
//    sSource += _T(")\n");
//    return sSource;
//}
//
//wxString lmScoreTextBox::SourceXML(int nIndent)
//{
//    //TODO
//    wxString sSource = _T("TODO: lmScoreTextBox XML Source code generation methods");
//    return sSource;
//}
//

