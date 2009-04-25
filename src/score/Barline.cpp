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
#pragma implementation "Barline.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"

#else
//for lmBarline
#include <wx/image.h>

//for lmBarline properties
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bmpcbox.h>
#include <wx/sizer.h>
#include <wx/panel.h>

#endif


#include "Score.h"
#include "VStaff.h"
#include "../graphic/ShapeBarline.h"


//--------------------------------------------------------------------------------------
/// Class lmBarlineProperties
//--------------------------------------------------------------------------------------

#include "defs.h"
#include "properties/DlgProperties.h"

class lmScoreObj;
class lmController;
class lmScore;

class lmBarlineProperties : public lmPropertiesPage
{
public:
	lmBarlineProperties(wxWindow* parent, lmBarline* pBL);
	~lmBarlineProperties();

    //implementation of pure virtual methods in base class
    void OnAcceptChanges(lmController* pController);

    // event handlers

protected:
    void CreateControls();

    //controls
	wxStaticText*		m_pTxtBarline;
	wxBitmapComboBox*	m_pBarlinesList;

    //other variables
    lmBarline*			m_pBL;


    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------------
/// Implementation of lmBarlineProperties
//--------------------------------------------------------------------------------------

#include "../graphic/GraphicManager.h"	//to use GenerateBitmapForBarlineCtrol()
#include "../app/ScoreCanvas.h"			//lmConroller

enum {
    lmID_BARLINE = 2600,
};

static wxString m_sBarlineName[lm_eMaxBarline];

BEGIN_EVENT_TABLE(lmBarlineProperties, lmPropertiesPage)

END_EVENT_TABLE()

static lmBarlinesDBEntry tBarlinesDB[lm_eMaxBarline+1];

//AWARE: pScore is needed as parameter in the constructor for those cases in
//wich the text is being created and is not yet included in the score. In this
//cases method GetScore() will fail, so we can not use it in the implementation
//of this class
lmBarlineProperties::lmBarlineProperties(wxWindow* parent, lmBarline* pBL)
    : lmPropertiesPage(parent)
{
    m_pBL = pBL;

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

    CreateControls();
	LoadBarlinesBitmapComboBox(m_pBarlinesList, tBarlinesDB);
	SelectBarlineBitmapComboBox(m_pBarlinesList, m_pBL->GetBarlineType() );
}

void lmBarlineProperties::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	m_pTxtBarline = new wxStaticText( this, wxID_ANY, wxT("Barline type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtBarline->Wrap( -1 );
	m_pTxtBarline->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Tahoma") ) );

	pMainSizer->Add( m_pTxtBarline, 0, wxALL, 5 );

	wxArrayString m_pBarlinesListChoices;
    m_pBarlinesList = new wxBitmapComboBox();
    m_pBarlinesList->Create(this, lmID_BARLINE, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
							0, NULL, wxCB_READONLY);
	pMainSizer->Add( m_pBarlinesList, 0, wxALL, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

lmBarlineProperties::~lmBarlineProperties()
{
}

void lmBarlineProperties::OnAcceptChanges(lmController* pController)
{
	int iB = m_pBarlinesList->GetSelection();
    lmEBarline nType = tBarlinesDB[iB].nBarlineType;
	if (nType == m_pBL->GetBarlineType())
		return;		//nothing to change

    if (pController)
    {
        //Editing and existing object. Do changes by issuing edit commands
        pController->ChangeBarline(m_pBL, nType, m_pBL->IsVisible());
    }
  //  else
  //  {
  //      //Direct creation. Modify text object directly
  //      m_pParentText->SetText( m_pTxtCtrl->GetValue() );
  //      m_pParentText->SetStyle(pStyle);
		//m_pParentText->SetAlignment(m_nHAlign);
  //  }
}



//-------------------------------------------------------------------------------------------------
// lmBarline object implementation
//-------------------------------------------------------------------------------------------------
static lmLUnits m_uThinLineWidth;
static lmLUnits m_uThickLineWidth;
static lmLUnits m_uSpacing;         // between lines and lines-dots
static lmLUnits m_uRadius;            // for dots

//
//constructors and destructor
//

lmBarline::lmBarline(lmEBarline nBarlineType, lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_Barline, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nBarlineType = nBarlineType;
    SetLayer(lm_eLayerBarlines);

    m_uThinLineWidth = m_pVStaff->TenthsToLogical(1.5, 1);  // thin line width
    m_uThickLineWidth = m_pVStaff->TenthsToLogical(6, 1);   // thick line width
    m_uSpacing = m_pVStaff->TenthsToLogical(4, 1);          // space between lines: 4 tenths
    m_uRadius = m_pVStaff->TenthsToLogical(2, 1);           // dots radius: 2 tenths
}

lmBarline::~lmBarline()
{
}

wxString lmBarline::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tBarline %s\tTimePos=%.2f"),
        m_nId, GetBarlineLDPNameFromType(m_nBarlineType).c_str(), m_rTimePos );
    sDump += lmStaffObj::Dump();
    sDump += _T("\n");
    return sDump;
}

wxString lmBarline::SourceLDP(int nIndent)
{
    wxString sSource = _T("");

    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(barline ");
    sSource += GetBarlineLDPNameFromType(m_nBarlineType);

	//base class
	sSource += lmStaffObj::SourceLDP(nIndent);

    //close element
    sSource += _T(")\n");
    return sSource;
}

wxString lmBarline::SourceXML(int nIndent)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("<barline>\n");
	nIndent++;

	//barline style
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	sSource += _T("<bar-style>");
    switch(m_nBarlineType)
    {
        case lm_eBarlineEndRepetition:
            sSource += _T("endRepetition");		//TODO
			break;
        case lm_eBarlineStartRepetition:
            sSource += _T("startRepetition");	//TODO
			break;
        case lm_eBarlineEnd:
            sSource += _T("light-heavy");
			break;
        case lm_eBarlineDouble:
            sSource += _T("light-light");
			break;
        case lm_eBarlineSimple:
            sSource += _T("regular");
			break;
        case lm_eBarlineStart:
            sSource += _T("heavy-light");
			break;
        case lm_eBarlineDoubleRepetition:
            sSource += _T("doubleRepetition");	//TODO
			break;
        default:
            wxASSERT(false);
    }
	sSource += _T("</bar-style>\n");

	//close barline
	nIndent--;
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("</barline>\n");

    return sSource;
}

lmLUnits lmBarline::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model.
    // Paper cursor must be used as the base for positioning.

    lmLUnits uyTop = m_pVStaff->GetYTop();
    lmLUnits uyBottom = m_pVStaff->GetYBottom();

    if (false && lmPRESERVE_SHAPES && !IsDirty())
    {
        //Not dirty: just add existing shapes to the Box
        lmShape* pOldShape = this->GetShape();
        pBox->AddShape(pOldShape, GetLayer());
        pOldShape->SetColour(*wxCYAN);//colorC);       //change its colour to new desired colour

        //reset position
        //TODO. Meanwhile do not execute this code:    if (false && ...
    }
    else
    {
	    lmEBarline nType = m_nBarlineType;

        //create the shape

        lmShapeBarline* pShape =
            new lmShapeBarline(this, nType, uPos.x, uyTop, uyBottom, m_uThinLineWidth,
                            m_uThickLineWidth, m_uSpacing, m_uRadius, colorC);
	    pBox->AddShape(pShape, GetLayer());
        StoreShape(pShape);
    }
    return GetShape()->GetBounds().GetWidth();

}

void lmBarline::OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName)
{
	//invoked to add specific panels to the dialog

    WXUNUSED(sTabName)

	pDlg->AddPanel( new lmBarlineProperties(pDlg->GetNotebook(), this),
				_("Barline"));

	//change dialog title
	pDlg->SetTitle(_("Barline properties"));
}


//-------------------------------------------------------------------------------------------------
// global functions related to barlines
//-------------------------------------------------------------------------------------------------

void LoadBarlinesBitmapComboBox(wxBitmapComboBox* pCtrol, lmBarlinesDBEntry tBarlines[])
{
    pCtrol->Clear();
    int i=0;
	while ((int)tBarlines[i].nBarlineType != -1)
    {
        pCtrol->Append(wxEmptyString,
                       GenerateBitmapForBarlineCtrol(tBarlines[i].sBarlineName,
                                                     tBarlines[i].nBarlineType ),
					   (void*)(&tBarlines[i]) );
		i++;
    }
	if (i > 0)
		pCtrol->SetSelection(0);
}

void SelectBarlineBitmapComboBox(wxBitmapComboBox* pCtrol, lmEBarline nType)
{
	//select received barline type in the barlines combo box
	int nMax = pCtrol->GetCount();
	for (int iB=0; iB < nMax; iB++)
	{
		if (nType == ((lmBarlinesDBEntry*)pCtrol->GetClientData(iB))->nBarlineType)
		{
			//wxLogMessage(_T("[SelectBarlineBitmapComboBox] nType=%d, iB=%d, nMax=%d"), nType, iB, nMax);
			pCtrol->SetSelection(iB);
			return;
		}
	}
}

wxString GetBarlineLDPNameFromType(lmEBarline nBarlineType)
{
    switch(nBarlineType)
    {
        case lm_eBarlineEndRepetition:
            return _T("endRepetition");
        case lm_eBarlineStartRepetition:
            return _T("startRepetition");
        case lm_eBarlineEnd:
            return _T("end");
        case lm_eBarlineDouble:
            return _T("double");
        case lm_eBarlineSimple:
            return _T("simple");
        case lm_eBarlineStart:
            return _T("start");
        case lm_eBarlineDoubleRepetition:
            return _T("doubleRepetition");
        default:
            wxASSERT(false);
            return _T("");        //let's keep the compiler happy
    }

}

const wxString& GetBarlineName(lmEBarline nBarlineType)
{
    static bool fStringsLoaded = false;

    if (!fStringsLoaded)
    {
        //language dependent strings. Can not be statically initiallized because
        //then they do not get translated
        m_sBarlineName[lm_eBarlineSimple] = _("Simple barline");
        m_sBarlineName[lm_eBarlineDouble]= _("Double barline");
        m_sBarlineName[lm_eBarlineEnd] = _("Final barline");
        m_sBarlineName[lm_eBarlineStartRepetition] = _("Start repetition");
        m_sBarlineName[lm_eBarlineEndRepetition] = _("End repetition");
        m_sBarlineName[lm_eBarlineStart] = _("Start barline");
        m_sBarlineName[lm_eBarlineDoubleRepetition] = _("Double repetition");
        fStringsLoaded = true;
    }

    return m_sBarlineName[nBarlineType];
}

