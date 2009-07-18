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
#pragma implementation "TextProperties.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/richtext/richtextctrl.h>

#include "TextProperties.h"
#include "DlgTextStyles.h"
#include "../Text.h"
#include "../../app/ArtProvider.h"
#include "../../app/ScoreCanvas.h"
#include "../../app/ScoreDoc.h"         // lmDELAY_REPAINT


//--------------------------------------------------------------------------------------
/// Implementation of lmTextProperties
//--------------------------------------------------------------------------------------

enum {
    lmEDIT_CUT = 2100,
    lmEDIT_COPY,
    lmEDIT_PASTE,
    lmEDIT_LEFT,
    lmEDIT_CENTER,
    lmEDIT_RIGHT,
    lmEDIT_STYLE,
    lmEDIT_CHANGE_STYLE,
};


BEGIN_EVENT_TABLE(lmTextProperties, lmPropertiesPage)
    EVT_BUTTON  (lmEDIT_CUT, lmTextProperties::OnCut)
    EVT_BUTTON  (lmEDIT_COPY, lmTextProperties::OnCopy)
    EVT_BUTTON  (lmEDIT_PASTE, lmTextProperties::OnPaste)
    EVT_BUTTON  (lmEDIT_LEFT, lmTextProperties::OnLeft)
    EVT_BUTTON  (lmEDIT_CENTER, lmTextProperties::OnCenter)
    EVT_BUTTON  (lmEDIT_RIGHT, lmTextProperties::OnRight)
    EVT_CHOICE  (lmEDIT_STYLE, lmTextProperties::OnStyle)
    EVT_BUTTON  (lmEDIT_CHANGE_STYLE, lmTextProperties::OnEditStyles)
END_EVENT_TABLE()


lmTextProperties::lmTextProperties(wxWindow* parent, lmScoreText* pParentText)
    : lmPropertiesPage(parent)
{
	m_pScore = pParentText->GetScore();
    m_pParentText = pParentText;
    CreateControls();

    //load current values
    m_pTxtCtrl->SetValue(m_pParentText->GetText());

    //set font according to text style
    wxFont font = m_pTxtCtrl->GetFont();
    font.SetWeight(m_pParentText->IsBold() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    font.SetStyle(m_pParentText->IsItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    font.SetPointSize(m_pParentText->GetFontSize());
    font.SetFaceName(m_pParentText->GetFontName());
    DoChangeFont(font, m_pParentText->GetColour());

	m_nHAlign = m_pParentText->GetAlignment();
	DoChangeAlignment();

    //combo with styles
    lmTextStyle* pCurStyle = m_pParentText->GetStyle();
    int nSel = -1;
    int iStyle = 0;
    lmTextStyle* pStyle = m_pScore->GetFirstStyle();
    while (pStyle)
    {
        m_pCboTextStyle->Append( pStyle->sName );
        if (pCurStyle == pStyle)
		{
            nSel = iStyle;
			DoChangeStyle(pStyle);
		}
        iStyle++;
        pStyle = m_pScore->GetNextStyle();
    }
    m_pCboTextStyle->SetSelection(nSel);
}

void lmTextProperties::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* pToolbarSizer;
	pToolbarSizer = new wxBoxSizer( wxHORIZONTAL );

    wxSize btSize(16, 16);
	m_pBtCut = new wxBitmapButton( this, lmEDIT_CUT,
                            wxArtProvider::GetBitmap(_T("tool_cut"), wxART_TOOLBAR, btSize),
                            wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	pToolbarSizer->Add( m_pBtCut, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	m_pBtCopy = new wxBitmapButton( this, lmEDIT_COPY,
                            wxArtProvider::GetBitmap(_T("tool_copy"), wxART_TOOLBAR, btSize),
                            wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	pToolbarSizer->Add( m_pBtCopy, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	m_pBtPaste = new wxBitmapButton( this, lmEDIT_PASTE,
                            wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, btSize),
                            wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	pToolbarSizer->Add( m_pBtPaste, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );


	pToolbarSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pBtLeft = new wxBitmapButton( this, lmEDIT_LEFT,
                            wxArtProvider::GetBitmap(_T("tool_text_left"), wxART_TOOLBAR, btSize),
                            wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	pToolbarSizer->Add( m_pBtLeft, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	m_pBtCenter = new wxBitmapButton( this, lmEDIT_CENTER,
                            wxArtProvider::GetBitmap(_T("tool_text_center"), wxART_TOOLBAR, btSize),
                            wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	pToolbarSizer->Add( m_pBtCenter, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	m_pBtRight = new wxBitmapButton( this, lmEDIT_RIGHT,
                            wxArtProvider::GetBitmap(_T("tool_text_right"), wxART_TOOLBAR, btSize),
                            wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	pToolbarSizer->Add( m_pBtRight, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );


	pToolbarSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pTxtStyle = new wxStaticText( this, wxID_ANY, _("Text style:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtStyle->Wrap( -1 );
	pToolbarSizer->Add( m_pTxtStyle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_pCboTextStyleChoices;
	m_pCboTextStyle = new wxChoice( this, lmEDIT_STYLE, wxDefaultPosition, wxSize( 250,-1 ), m_pCboTextStyleChoices, 0 );
	m_pCboTextStyle->SetSelection( 0 );
	pToolbarSizer->Add( m_pCboTextStyle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_pBtnChangeStyles = new wxButton( this, lmEDIT_CHANGE_STYLE, _("Add/change styles"), wxDefaultPosition, wxDefaultSize, 0 );
	pToolbarSizer->Add( m_pBtnChangeStyles, 0, wxALL, 5 );

	pMainSizer->Add( pToolbarSizer, 0, wxEXPAND, 5 );

	//m_pTxtCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtCtrl = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString);
	m_pTxtCtrl->SetMinSize( wxSize( 550,150 ) );


	pMainSizer->Add( m_pTxtCtrl, 1, wxALL|wxEXPAND, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
	pMainSizer->Fit( this );
}

lmTextProperties::~lmTextProperties()
{
}

void lmTextProperties::OnCut(wxCommandEvent& event)
{
    m_pTxtCtrl->Cut();
}

void lmTextProperties::OnCopy(wxCommandEvent& event)
{
    m_pTxtCtrl->Copy();
}

void lmTextProperties::OnPaste(wxCommandEvent& event)
{
    m_pTxtCtrl->Paste();
}

void lmTextProperties::OnLeft(wxCommandEvent& event)
{
	m_nHAlign = lmHALIGN_LEFT;
	DoChangeAlignment();
}

void lmTextProperties::OnCenter(wxCommandEvent& event)
{
	m_nHAlign = lmHALIGN_CENTER;
	DoChangeAlignment();
}

void lmTextProperties::OnRight(wxCommandEvent& event)
{
	m_nHAlign = lmHALIGN_RIGHT;
	DoChangeAlignment();
}

void lmTextProperties::OnEditStyles(wxCommandEvent& event)
{
    //open styles dialog
    lmDlgTextStyles dlg(this->GetParent(), m_pScore);
    dlg.ShowModal();

    //apply changes
    lmTextStyle* pStyle = m_pScore->GetStyleInfo( m_pParentText->GetStyle()->sName );
	DoChangeStyle(pStyle);
}

void lmTextProperties::OnStyle(wxCommandEvent& event)
{
    //apply changes
    lmTextStyle* pStyle = m_pScore->GetStyleInfo( m_pCboTextStyle->GetStringSelection() );
	DoChangeStyle(pStyle);
}

void lmTextProperties::DoChangeStyle(lmTextStyle* pStyle)
{
    wxFont font = m_pTxtCtrl->GetFont();
    font.SetStyle(pStyle->tFont.nFontStyle);
    font.SetWeight(pStyle->tFont.nFontWeight);
    font.SetPointSize(pStyle->tFont.nFontSize);
    font.SetFaceName(pStyle->tFont.sFontName);
    DoChangeFont(font, pStyle->nColor);
}

void lmTextProperties::DoChangeFont(const wxFont& font, const wxColour& color)
{
    wxRichTextRange range = wxRichTextRange(0, m_pTxtCtrl->GetLastPosition()+1);
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_FONT | wxTEXT_ATTR_TEXT_COLOUR );
    attr.SetFont(font);
	attr.SetTextColour(color);
    if (attr.GetFont().Ok())
	{
        m_pTxtCtrl->SetStyle(range, attr);
        m_pTxtCtrl->SetBasicStyle(attr);
	}
}

void lmTextProperties::DoChangeAlignment()
{
	wxTextAttrAlignment nAlign;
	switch (m_nHAlign)
	{
		case lmHALIGN_LEFT:		nAlign = wxTEXT_ALIGNMENT_LEFT;		break;
		case lmHALIGN_CENTER:	nAlign = wxTEXT_ALIGNMENT_CENTER;	break;
		case lmHALIGN_RIGHT:	nAlign = wxTEXT_ALIGNMENT_RIGHT;	break;
		default:
			nAlign = wxTEXT_ALIGNMENT_DEFAULT;
	}
	m_pTxtCtrl->SelectAll();
	m_pTxtCtrl->ApplyAlignmentToSelection(nAlign);
	m_pTxtCtrl->SelectNone();
}

void lmTextProperties::OnAcceptChanges(lmController* pController)
{
    lmTextStyle* pStyle = m_pScore->GetStyleInfo( m_pCboTextStyle->GetStringSelection() );

    // remove all cr/lf from the new string
    wxString sNewText = m_pTxtCtrl->GetValue();
    wxString sResult;
    size_t len = sNewText.length();
    sResult.Alloc(len);
    for ( size_t n = 0; n < len; n++ )
    {
        if (sNewText[n] != _T('\x0d') && sNewText[n] != _T('\x0a'))
            sResult += sNewText[n];
    }

    //if text not changed and is empty or nothing else changed, return. Nothing to do
    if (sResult == m_pParentText->GetText()
        && (sResult == _T("")
            || (m_nHAlign == m_pParentText->GetAlignment()
                && pStyle == m_pParentText->GetStyle() )))
        return;

    //Proceed to do the changes
    if (pController)
    {
        //Editing an existing object. Do changes by issuing edit commands
        pController->ChangeText(m_pParentText,
                                sResult,
                                m_nHAlign,
                                m_pParentText->GetLocation(),
                                pStyle);
    }
    else
    {
        //Direct creation. Modify text object directly
        m_pParentText->SetText(sResult);
        m_pParentText->SetStyle(pStyle);
		m_pParentText->SetAlignment(m_nHAlign);
    }
}
