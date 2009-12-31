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
};



IMPLEMENT_DYNAMIC_CLASS(lmToolPageBarlines, lmToolPage)


lmToolPageBarlines::lmToolPageBarlines()
{
}

lmToolPageBarlines::lmToolPageBarlines(wxWindow* parent)
{
    Create(parent);
}

void lmToolPageBarlines::Create(wxWindow* parent)
{
    //base class
    lmToolPage::CreatePage(parent, lmPAGE_BARLINES);

    //initialize data
    m_sPageToolTip = _("Edit tools for barlines and rehearsal marks");
    m_sPageBitmapName = _T("tool_barlines");

    //create groups
    CreateGroups();
}

lmToolPageBarlines::~lmToolPageBarlines()
{
}

void lmToolPageBarlines::CreateGroups()
{
    //Create the groups for this page

    wxBoxSizer *pMainSizer = GetMainSizer();

    m_pGrpBarlines = new lmGrpBarlines(this, pMainSizer, lmMM_DATA_ENTRY);
    AddGroup(m_pGrpBarlines);

	CreateLayout();

    //initialize info about selected group/tool
    m_nCurGroupID = lmGRP_BarlineType;
    m_nCurToolID = m_pGrpBarlines->GetCurrentToolID();

    m_fGroupsCreated = true;
}

wxString lmToolPageBarlines::GetToolShortDescription()
{
    //returns a short description of the selected tool. This description is used to
    //be displayed in the status bar

    return _("Add barline");
}



//--------------------------------------------------------------------------------
// lmGrpBarlines implementation
//--------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGrpBarlines, lmToolGroup)
    EVT_COMBOBOX    (lmID_BARLINES_LIST, lmGrpBarlines::OnBarlinesList)
END_EVENT_TABLE()

static lmBarlinesDBEntry m_tBarlinesDB[lm_eMaxBarline+1];


lmGrpBarlines::lmGrpBarlines(lmToolPage* pParent, wxBoxSizer* pMainSizer,
                             int nValidMouseModes)
        : lmToolGroup(pParent, lm_eGT_ToolSelector, pParent->GetColors(),
                      nValidMouseModes)
{
    //To avoid having to translate again barline names, we are going to load them
    //by using global function GetBarlineName()
    int i;
    for (i = 0; i < lm_eMaxBarline; i++)
    {
        m_tBarlinesDB[i].nBarlineType = (lmEBarline)i;
        m_tBarlinesDB[i].sBarlineName = GetBarlineName((lmEBarline)i);
    }
    //End of table item
    m_tBarlinesDB[i].nBarlineType = (lmEBarline)-1;
    m_tBarlinesDB[i].sBarlineName = _T("");
}

void lmGrpBarlines::CreateGroupControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    SetGroupTitle(_("Barline"));
    wxBoxSizer* pCtrolsSizer = CreateGroupSizer(pMainSizer);

    //bitmap combo box to select the clef
    m_pBarlinesList = new wxBitmapComboBox();
    m_pBarlinesList->Create(this, lmID_BARLINES_LIST, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
                       0, NULL, wxCB_READONLY);

	pCtrolsSizer->Add( m_pBarlinesList, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	this->Layout();

    //initializations
	LoadBarlinesBitmapComboBox(m_pBarlinesList, m_tBarlinesDB);
	SelectBarlineBitmapComboBox(m_pBarlinesList, lm_eBarlineSimple);
}

void lmGrpBarlines::OnBarlinesList(wxCommandEvent& event)
{
    //Notify owner page about the tool change
    WXUNUSED(event);

    ((lmToolPage*)m_pParent)->OnToolChanged(GetToolGroupID(), GetCurrentToolID());
}

lmEBarline lmGrpBarlines::GetSelectedBarline()
{
	int iB = m_pBarlinesList->GetSelection();
    return m_tBarlinesDB[iB].nBarlineType;
}

lmEToolID lmGrpBarlines::GetCurrentToolID()
{
    return (lmEToolID)m_pBarlinesList->GetSelection();
}

//void lmGrpBarlines::OnAddBarline(wxCommandEvent& event)
//{
//    //insert selected barline
//	WXUNUSED(event);
//	int iB = m_pBarlinesList->GetSelection();
//    lmController* pSC = GetMainFrame()->GetActiveController();
//    if (pSC)
//    {
//        pSC->InsertBarline(m_tBarlinesDB[iB].nBarlineType);
//
//        //return focus to active view
//        GetMainFrame()->SetFocusOnActiveView();
//    }
//}
