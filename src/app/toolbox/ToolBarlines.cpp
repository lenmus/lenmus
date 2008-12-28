//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "ToolBarlines.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#else
#include <wx/xrc/xmlres.h>
#include <wx/bmpcbox.h>
#include <wx/statline.h>
#include <wx/clrpicker.h>
#endif

#include "ToolsBox.h"
#include "ToolBarlines.h"
#include "ToolGroup.h"
#include "../ArtProvider.h"         //to use ArtProvider for managing icons
#include "../TheApp.h"              //to use GetMainFrame()
#include "../MainFrame.h"           //to get active lmScoreCanvas
#include "../ScoreCanvas.h"         //to send commands
#include "../../widgets/Button.h"
#include "../../score/defs.h"
#include "../../score/Barline.h"
#include "../../graphic/GraphicManager.h"   //to use GenerateBitmapForKeyCtrol()



#define lmSPACING 5

//event IDs
enum {
    lmID_BARLINES_LIST = 2600,
    lmID_BARLINE_ADD,
};



lmToolPageBarlines::lmToolPageBarlines(wxWindow* parent)
	: lmToolPage(parent)
{
	wxBoxSizer* pMainSizer = GetMainSizer();

    //create groups
    m_pGrpBarlines = new lmGrpBarlines(this, pMainSizer);

	CreateLayout();
}


lmToolPageBarlines::~lmToolPageBarlines()
{
    delete m_pGrpBarlines;
}

lmToolGroup* lmToolPageBarlines::GetToolGroup(lmEToolGroupID nGroupID)
{
    switch(nGroupID)
    {
        case lmGRP_BarlineType:    return m_pGrpBarlines;
        default:
            wxASSERT(false);
    }
    return (lmToolGroup*)NULL;      //compiler happy
}



//--------------------------------------------------------------------------------
// lmGrpBarlines implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGrpBarlines, lmToolGroup)
    EVT_BUTTON      (lmID_BARLINE_ADD, lmGrpBarlines::OnAddBarline)
END_EVENT_TABLE()

static lmBarlinesDBEntry tBarlinesDB[lm_eMaxBarline+1];


lmGrpBarlines::lmGrpBarlines(lmToolPage* pParent, wxBoxSizer* pMainSizer)
        : lmToolGroup(pParent)
{
    //To avoid having to translate again barline names, we are going to load them
    //by using global function GetBarlineName()
    int i;
    for (i = 0; i < lm_eMaxBarline; i++)
    {
        tBarlinesDB[i].nBarlineType = (lmEBarline)i;
        tBarlinesDB[i].sBarlineName = GetBarlineName((lmEBarline)i);
    }
    //End of table item
    tBarlinesDB[i].nBarlineType = (lmEBarline)-1;
    tBarlinesDB[i].sBarlineName = _T("");

    CreateControls(pMainSizer);
	LoadBarlinesBitmapComboBox(m_pBarlinesList, tBarlinesDB);
	SelectBarlineBitmapComboBox(m_pBarlinesList, lm_eBarlineSimple);
}

void lmGrpBarlines::CreateControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxBoxSizer* pCtrolsSizer = CreateGroup(pMainSizer, _("Add barline"));

    //bitmap combo box to select the clef
    m_pBarlinesList = new wxBitmapComboBox();
    m_pBarlinesList->Create(this, lmID_BARLINES_LIST, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
                       0, NULL, wxCB_READONLY);

	pCtrolsSizer->Add( m_pBarlinesList, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    //button to add the clef
    m_pBtAddBarline = new wxButton(this, lmID_BARLINE_ADD, _("Add barline"), wxDefaultPosition, wxDefaultSize, 0 );
	pCtrolsSizer->Add( m_pBtAddBarline, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	this->Layout();
}

void lmGrpBarlines::OnAddBarline(wxCommandEvent& event)
{
    //insert selected barline
	WXUNUSED(event);
	int iB = m_pBarlinesList->GetSelection();
    lmController* pSC = GetMainFrame()->GetActiveController();
    if (pSC)
    {
        pSC->InsertBarline(tBarlinesDB[iB].nBarlineType);

        //return focus to active view
        GetMainFrame()->SetFocusOnActiveView();
    }
}
