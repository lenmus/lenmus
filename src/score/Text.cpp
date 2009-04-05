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
#include "../graphic/GMObject.h"
#include "../graphic/ShapeText.h"
#include "properties/DlgProperties.h"
#include "properties/TextProperties.h"


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

lmScoreBlock::lmScoreBlock(lmTenths tWidth, lmTenths tHeight, lmTPoint tPos,
                           lmEBlockAlign nBlockAlign,
                           lmEHAlign nHAlign, lmEVAlign nVAlign)
    : lmAuxObj(lmDRAGGABLE)
    , m_tWidth(tWidth)
    , m_tHeight(tHeight)
    , m_tPos(tPos)
    , m_nBlockAlign(nBlockAlign)
    , m_nHAlign(nHAlign)
    , m_nVAlign(nVAlign)
{
}



//==========================================================================================
// lmScoreTextParagraph implementation: box + alignment + collection of lmBaseText
//==========================================================================================

lmScoreTextParagraph::lmScoreTextParagraph()
    : lmScoreBlock()
{
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
    sSource += _T("(paragraph");

    //alignment
    if (m_nHAlign == lmHALIGN_CENTER)
        sSource += _T(" center");
    else if (m_nHAlign == lmHALIGN_LEFT)
        sSource += _T(" left");
    else
        sSource += _T(" right");

    ++nIndent;
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

    ////compute position
    lmLUnits uxStart = m_pParent->TenthsToLogical(m_tPos.x) + pPaper->GetCursorX();
    lmLUnits uyStart = m_pParent->TenthsToLogical(m_tPos.y) + pPaper->GetCursorY();
    //lmLUnits uxEnd = uxStart + m_pParent->TenthsToLogical(m_tWidth) + pPaper->GetCursorX();
    //lmLUnits uyEnd = uyStart + m_pParent->TenthsToLogical(m_tHeight) + pPaper->GetCursorY();
    //lmLUnits uWidth = m_pParent->TenthsToLogical(m_tWidth);
    //lmLUnits uBoundsExtraWidth = m_pParent->TenthsToLogical(2);  //TODO user option?

    //create the shape
    std::list<lmBaseText*>::iterator it;
    //for (it = m_texts.begin(); it != m_texts.end(); ++it)
    it = m_texts.begin();
    wxFont* pFont = (*it)->GetSuitableFont(pPaper);

    lmShape* pShape = new lmShapeTextbox(this, (*it)->GetText(), pFont, pPaper,
                                         m_nBlockAlign, m_nHAlign, m_nVAlign,
                                         uxStart, uyStart, 0.0f, 0.0f,
                                         (*it)->GetColour());

	pBox->AddShape(pShape);
    StoreShape(pShape);
    return pShape->GetBounds().GetWidth();
}

lmUPoint lmScoreTextParagraph::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
    //TODO
    return uOrg;
}

void lmScoreTextParagraph::MoveObjectPoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts, bool fAddShifts)
{
    //TODO
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

