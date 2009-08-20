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
#pragma implementation "FiguredBass.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif


#include "Score.h"
#include "FiguredBass.h"
#include "VStaff.h"
#include "Staff.h"
#include "../ldp_parser/LDPParser.h"

typedef struct
{
    int             nBitmapID;
    wxString        sFiguredBass;
}
lmCommonFBData;

static const lmCommonFBData m_CommonFB[] = 
{
    { 0,    _T("#") },
    { 1,    _T("b") },
    { 2,    _T("=") },
    { 3,    _T("2") },        //6 4 2
    { 4,    _T("#2") },       //6 4 #2. Raise 2 by a 1/2 step (not necessarily a sharp)
    { 5,    _T("b2") },       //6 4 b2. Lower 2 by a 1/2 step (not necessarily a flat)
    { 6,    _T("=2") },       //6 4 =2. Make 2 natural regardless of key signature
    { 7,    _T("2+") },       //6 4 #2. (Usu. dim. 7th chord) Raise 2 by 1/2 step
    { 8,    _T("2 3") },      //7 4 2 / 8 5 3. (bass moves, upper voices hold)
    { 9,    _T("3") },        //8 5 3
    { 10,   _T("4") },        //8 5 4. (Usu. 4 3)
    //{ 11,   _T("4 3") },      //8 5 4 / 8 5 3
    { 12,   _T("4 2") },      //6 4 2
    { 13,   _T("4+ 2") },     //6 #4 2. Raise 4 1/2 step
    { 14,   _T("4 3") },      //6 4 3
    { 15,   _T("5") },        //8 5 3
    { 16,   _T("5 #") },
    { 17,   _T("5 b") },
    { 18,   _T("5+") },       //8 #5 3. Raise 5 1/2 step
    { 19,   _T("5/") },       //6 5 3. indicates dim. 5th
    { 20,   _T("5 3") },      //8 5 3
    { 21,   _T("5 4") },      //8 5 4. (Usu. 4 3)
    { 22,   _T("6") },        //(8) 6 3
    { 23,   _T("6 #") },
    { 24,   _T("6 b") },
    { 25,   _T("6\\") },      //(8) #6 3. (Raise 6 by 1/2 step)
    { 26,   _T("6 3") },      //(8) 6 3
    { 27,   _T("6 #3") },
    { 28,   _T("6 b3") },
    { 29,   _T("6 4") },      //8 6 4
    { 30,   _T("6 4 2") },    //6 4 2
    { 31,   _T("6 4 3") },    //6 4 3
    { 32,   _T("6 5") },      //6 3 5
    { 33,   _T("6 5 3") },    //6 5 3
    { 34,   _T("7") },        //7 3 5
    { 35,   _T("7 4 2") },    //7 4 2 / 8 3 (1)
    { 36,   _T("8") },        //Play the bass line alone in octaves
    { 37,   _T("9") },        //9 5 3. (Usu. 9 8)
    { 38,   _T("10") },       //parallel 10ths
};

const int lmFB_NUM_COMMON = sizeof(m_CommonFB) / sizeof(lmCommonFBData);


//--------------------------------------------------------------------------------------
/// Class lmFBQuickPanel: panel for quick figured bass selection
//--------------------------------------------------------------------------------------

#include "properties/DlgProperties.h"
#include "../app/toolbox/ToolGroup.h"
#include "../widgets/Button.h"

class lmScoreObj;
class lmController;
class lmScore;


enum {
    lmID_QUICK_BT = 2600,
};


//--------------------------------------------------------------------------------------------
// Helper class lmGrpCommonFB: Buttons group for common figured bass values
//--------------------------------------------------------------------------------------------

class lmGrpCommonFB : public lmToolButtonsGroup
{
public:
    lmGrpCommonFB(wxPanel* pParent, wxBoxSizer* pMainSizer, int nNumButtons);
    ~lmGrpCommonFB() {}

    //implement virtual methods
    inline lmEToolGroupID GetToolGroupID() { return (lmEToolGroupID)-1; }
    void CreateControls(wxBoxSizer* pMainSizer);

};

lmGrpCommonFB::lmGrpCommonFB(wxPanel* pParent, wxBoxSizer* pMainSizer, int nNumButtons)
        : lmToolButtonsGroup(pParent, nNumButtons, lmTBG_ALLOW_NONE, pMainSizer,
                             lmID_QUICK_BT, lmTOOL_NONE)
{
    CreateControls(pMainSizer);
}

void lmGrpCommonFB::CreateControls(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxBoxSizer* pCtrolsSizer = CreateGroup(pMainSizer, _("Frequent symbols"));

    wxBoxSizer* pButtonsSizer;
    wxSize btSize(32, 42);
	for (int iB=0; iB < m_nNumButtons; iB++)
	{
		if (iB % 14 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		//wxString sBtName = _T("figured_bass_3");
        wxString sBtName = wxString::Format(_T("figured_bass_%1d"), m_CommonFB[iB].nBitmapID);
		wxString sToolTip = wxString::Format(_("Figured bass '%s'"), m_CommonFB[iB].sFiguredBass);
		m_pButton[iB] = new lmCheckButton(this, lmID_QUICK_BT+iB, wxBitmap(32, 42));
        m_pButton[iB]->SetBitmapUp(sBtName, _T(""), btSize);
        m_pButton[iB]->SetBitmapDown(sBtName, _T("button_selected_flat"), btSize);
        m_pButton[iB]->SetBitmapOver(sBtName, _T("button_over_flat"), btSize);
		m_pButton[iB]->SetToolTip(sToolTip);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 0) );
	}
	this->Layout();

	SelectButton(4);	//select ?
}



//--------------------------------------------------------------------------------------------
// Helper class lmFBQuickPanel: panel for properties
//--------------------------------------------------------------------------------------------

class lmFBQuickPanel : public lmPropertiesPage
{
public:
	lmFBQuickPanel(wxWindow* parent, lmFiguredBass* pBL);
	~lmFBQuickPanel();

    //implementation of pure virtual methods in base class
    void OnAcceptChanges(lmController* pController);

    // event handlers

protected:
    void CreateControls();

    //controls
	wxBitmapButton*     m_pBtQuick[25];
	wxRadioBox*         m_pRadEnclose;
	
    //other variables
    lmFiguredBass*      m_pFB;
    lmGrpCommonFB*      m_pGrpCommon;


    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------------
/// Implementation of lmFBQuickPanel
//--------------------------------------------------------------------------------------

#include "../graphic/GraphicManager.h"	//to use GenerateBitmapForBarlineCtrol()
#include "../app/ScoreCanvas.h"			//lmConroller
BEGIN_EVENT_TABLE(lmFBQuickPanel, lmPropertiesPage)

END_EVENT_TABLE()


//AWARE: pScore is needed as parameter in the constructor for those cases in
//wich the object is being created and is not yet included in the score. In this
//cases method GetScore() will fail, so we can not use it in the implementation
//of this class
lmFBQuickPanel::lmFBQuickPanel(wxWindow* parent, lmFiguredBass* pFB)
    : lmPropertiesPage(parent)
    , m_pFB(pFB)
{
    CreateControls();
}

void lmFBQuickPanel::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pGrpCommon = new lmGrpCommonFB(this, pMainSizer, lmFB_NUM_COMMON);

	wxString m_pRadEncloseChoices[] = { _("None"), _("Parenthesis"), _("Squared bracktes") };
	int m_pRadEncloseNChoices = sizeof( m_pRadEncloseChoices ) / sizeof( wxString );
	m_pRadEnclose = new wxRadioBox( this, wxID_ANY, _("Enclose in"), wxDefaultPosition, wxDefaultSize, m_pRadEncloseNChoices, m_pRadEncloseChoices, 3, wxRA_SPECIFY_COLS );
	m_pRadEnclose->SetSelection( 0 );
	pMainSizer->Add( m_pRadEnclose, 0, wxALL|wxEXPAND, 5 );
	
	SetSizer( pMainSizer );
	Layout();
}

lmFBQuickPanel::~lmFBQuickPanel()
{
}

void lmFBQuickPanel::OnAcceptChanges(lmController* pController)
{
    int iB = m_pGrpCommon->GetSelectedButton();
    wxString sFigBass;
    if (iB == -1)
        return;     //user didn't selected any common value
    else
    {
        //User has selected a quick button
        sFigBass = m_CommonFB[iB].sFiguredBass;

        if (pController)
        {
            //Editing and existing object. Do changes by issuing edit commands
            pController->ChangeFiguredBass(m_pFB, sFigBass);
        }
        else
        {
            //Direct creation. Modify object directly
            lmFiguredBassInfo tFBInfo[lmFB_MAX_INTV+1];
            lmLDPParser::ValidateFiguredBassString(sFigBass, &tFBInfo[0]);
            m_pFB->SetIntervalsInfo(&tFBInfo[0]);
        }
    }
}



//-----------------------------------------------------------------------------------------
// lmFiguredBass implementation
//-----------------------------------------------------------------------------------------

lmFiguredBass::lmFiguredBass(lmVStaff* pVStaff, long nID, lmFiguredBassInfo* pFBInfo)
    : lmStaffObj(pVStaff, nID, lm_eSO_FiguredBass, pVStaff, 1, lmVISIBLE, lmDRAGGABLE)
    , m_fStartOfLine(false)
    , m_fEndOfLine(false)
    , m_fParenthesis(false)
{
    SetLayer(lm_eLayerNotes);
    SetIntervalsInfo(pFBInfo);
}

void lmFiguredBass::SetIntervalsInfo(lmFiguredBassInfo* pFBInfo)
{
    //copy received data
    for (int i=0; i <= lmFB_MAX_INTV; i++)
        m_tFBInfo[i] = *(pFBInfo + i);
}

void lmFiguredBass::GetIntervalsInfo(lmFiguredBassInfo* pFBInfo)
{
    //copy internal data
    for (int i=0; i <= lmFB_MAX_INTV; i++)
         *(pFBInfo + i) = m_tFBInfo[i];
}

lmUPoint lmFiguredBass::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	return uPos;
}

lmLUnits lmFiguredBass::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    //TODO: This code draws the figured bass after last staff, with a separation from it
    // of GetStaffDistance(). We need to define a reference position for figyred bass and
    // to take its existance into account for margins, spacings, etc.

    //get the position on which the figured bass must be drawn
    lmLUnits uxStartPos = uPos.x;
	lmLUnits uyPosTop = uPos.y + m_pVStaff->GetStaffOffset(m_nStaffNum)
						- m_pVStaff->TenthsToLogical(20, m_nStaffNum);

    //Move after last staff
    //At this point uyPosTop is correctly positioned for first staff, so no need to
    //add StaffDistance for first staff
    lmStaff* pStaff = m_pVStaff->GetFirstStaff();
    for (int nStaff=1; pStaff; pStaff = m_pVStaff->GetNextStaff(), nStaff++)
    {
        //add top margin if not first staff
        if (nStaff > 1)
            uyPosTop += pStaff->GetStaffDistance();

        //move to next staff
        uyPosTop += pStaff->GetHeight();
    }

	//create the shape object
    int nShapeIdx = 0;
    lmCompositeShape* pShape = new lmCompositeShape(this, nShapeIdx, colorC, _T("Figured bass"),
                                                    lmDRAGGABLE);
	//loop to create glyphs for each interval
    lmLUnits uIntervalsSpacing = 1.4f * m_pVStaff->TenthsToLogical(aGlyphsInfo[GLYPH_FIGURED_BASS_7].thHeight, m_nStaffNum );
  	wxFont* pFont = GetSuitableFont(pPaper);
    lmCompositeShape* pIntvShape[lmFB_MAX_INTV+1];
    lmLUnits uShifts[lmFB_MAX_INTV+1];
    for (int i=lmFB_MAX_INTV; i > 1; i--)
    {
        if (m_tFBInfo[i].nQuality == lm_eIM_NotPresent)
        {
            pIntvShape[i] = (lmCompositeShape*)NULL;
            uShifts[i] = 0.0f;
        }
        else
        {
            lmLUnits uxPos = uxStartPos;
            pIntvShape[i] = new lmCompositeShape(this, nShapeIdx, colorC, _T("Fig. bass intv."),
                                                 lmDRAGGABLE);

            //add parenthesis
            if (m_tFBInfo[i].nAspect == lm_eIA_Parenthesis)
            {
                uxPos += AddSymbol(pIntvShape[i], pPaper, _T('('), pFont,
                                   lmUPoint(uxPos, uyPosTop), colorC);
            }
            //add prefix
            if (m_tFBInfo[i].sPrefix != _T(""))
            {
                //let's save position just in case intval.number is understood
                uShifts[i] = uxPos - uxStartPos;    //horizontal shift to digit

                const wxChar* p = m_tFBInfo[i].sPrefix.c_str();
                while(*p)
                {
                    uxPos += AddSymbol(pIntvShape[i], pPaper, *p, pFont,
                                       lmUPoint(uxPos, uyPosTop), colorC);
                    p++;
                }
            }
            //add interval
            if (m_tFBInfo[i].nAspect != lm_eIA_Understood)
            {
                uShifts[i] = uxPos - uxStartPos;    //horizontal shift to digit

                int nGlyph;
                if (m_tFBInfo[i].sOver == _T("/"))
		            nGlyph = GLYPH_FIGURED_BASS_7_STRIKED;  //0 + (int)i;
                else
		            nGlyph = GLYPH_FIGURED_BASS_0 + (int)i;

		        lmLUnits uyPos = uyPosTop 
						        + m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].GlyphOffset, m_nStaffNum );
                lmShape* pDigit =
                    new lmShapeGlyph(this, -1, nGlyph, pPaper, lmUPoint(uxPos, uyPos), 
									 _T("intval number"), lmNO_DRAGGABLE, colorC);
                pIntvShape[i]->Add(pDigit);
		        uxPos += pDigit->GetBounds().GetWidth();
            }
            //add suffix
            if (m_tFBInfo[i].sSuffix != _T(""))
            {
                const wxChar* p = m_tFBInfo[i].sSuffix.c_str();
                while(*p)
                {
                    uxPos += AddSymbol(pIntvShape[i], pPaper, *p, pFont,
                                       lmUPoint(uxPos, uyPosTop), colorC);
                    p++;
                }
            }
            //add parenthesis
            if (m_tFBInfo[i].nAspect == lm_eIA_Parenthesis)
            {
                uxPos += AddSymbol(pIntvShape[i], pPaper, _T(')'), pFont,
                                   lmUPoint(uxPos, uyPosTop), colorC);
            }

            //increment y position for next interval
            uyPosTop += uIntervalsSpacing;
        }
    }

    //aling all numbers and add all shapes to main one
    lmLUnits uxShift = 0.0f;
    for (int i=lmFB_MAX_INTV; i > 1; i--)
        uxShift = wxMax(uxShift, uShifts[i]);

    for (int i=lmFB_MAX_INTV; i > 1; i--)
    {
        if (pIntvShape[i])
        {
            pIntvShape[i]->Shift(uxShift - uShifts[i], 0.0f);
            pShape->Add(pIntvShape[i]);
        }
    }

	pBox->AddShape(pShape, GetLayer());
    StoreShape(pShape);

	return pShape->GetWidth();
}

lmLUnits lmFiguredBass::AddSymbol(lmCompositeShape* pShape, lmPaper* pPaper, wxChar ch,
								  wxFont* pFont, lmUPoint uPos, wxColour colorC)
{
    // returns the width of the symbol shape

    lmEGlyphIndex nGlyph;
    switch(ch)
    {
        case ('#'):  nGlyph = GLYPH_FIGURED_BASS_SHARP;     break;
        case ('b'):  nGlyph = GLYPH_FIGURED_BASS_FLAT;      break;
        case ('='):  nGlyph = GLYPH_FIGURED_BASS_NATURAL;   break;
        case ('+'):  nGlyph = GLYPH_FIGURED_BASS_PLUS;      break;
        case ('-'):  nGlyph = GLYPH_FIGURED_BASS_MINUS;     break;
        //case ('/'):  nGlyph = GLYPH_FIGURED_BASS_SHARP;     break;
        //case ('\\'):  nGlyph = GLYPH_FIGURED_BASS_SHARP;     break;
        case ('('):  nGlyph = GLYPH_FIGURED_BASS_OPEN_PARENTHESIS;  break;
        case (')'):  nGlyph = GLYPH_FIGURED_BASS_CLOSE_PARENTHESIS; break;
    }

	//create the shape
    lmShapeGlyph* pSG = 
		new lmShapeGlyph(this, -1, nGlyph, pPaper, uPos, _T("symbol"),
				         lmDRAGGABLE, colorC);

	//add the shape to the composite parent shape
	pShape->Add(pSG);

	return pSG->GetWidth();
}

wxString lmFiguredBass::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tFiguredBass\tTimePos=%.2f"),
        m_nId, m_rTimePos );
    sDump += lmStaffObj::Dump();
    sDump += _T("\n");
    return sDump;
}

wxString lmFiguredBass::SourceLDP(int nIndent, bool fUndoData)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    if (fUndoData)
        sSource += wxString::Format(_T("(figuredBass#%d "), GetID());
    else
        sSource += _T("(figuredBass ");

    //add source string
    sSource += _T("\"");
    bool fFirstIntval = true;
    for (int i=lmFB_MAX_INTV; i > 1; i--)
    {
        if (m_tFBInfo[i].nQuality != lm_eIM_NotPresent)
        {
            if (!fFirstIntval)
                sSource += _T(" ");
            if (m_tFBInfo[i].nAspect == lm_eIA_Parenthesis)
                sSource += _T("(");
            sSource += m_tFBInfo[i].sSource;
            if (m_tFBInfo[i].nAspect == lm_eIA_Parenthesis)
                sSource += _T(")");
            fFirstIntval = false;
        }
    }
    sSource += _T("\"");

	//base class
	wxString sBase = lmStaffObj::SourceLDP(nIndent, fUndoData);
    if (sBase != _T(""))
    {
        sSource += sBase;
        sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    }

    //close element
    sSource += _T(")\n");
	return sSource;
}

wxString lmFiguredBass::SourceXML(int nIndent)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("<figured-bass>\n");
	nIndent++;

    //intervals
    for (int i=lmFB_MAX_INTV; i > 1; i--)
    {
        if (m_tFBInfo[i].nQuality != lm_eIM_NotPresent)
        {
		    sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		    sSource += _T("<figure>\n");
		    nIndent++;

            //prefix
            if (m_tFBInfo[i].sPrefix != _T(""))
            {
	            sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
                sSource += _T("<prefix>");
                if (m_tFBInfo[i].nAspect == lm_eIA_Parenthesis)
                    sSource += _T("(");
                sSource += m_tFBInfo[i].sPrefix;
                sSource += _T("</prefix>\n");
            }

            //interval number
	        sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
            sSource += wxString::Format(_T("<figure-number>%d</figure-number>\n"), i);

            //suffix
	        sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
            sSource += _T("<suffix>");
            if (m_tFBInfo[i].sSuffix != _T(""))
                sSource += m_tFBInfo[i].sSuffix;
            if (m_tFBInfo[i].nAspect == lm_eIA_Parenthesis)
                sSource += _T(")");
            sSource += _T("</suffix>\n");

            //close <figure>
		    nIndent--;
		    sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		    sSource += _T("</figure>\n");
        }
    }

	//base class
	wxString sBase = lmStaffObj::SourceXML(nIndent);
    if (sBase != _T(""))
    {
        sSource += sBase;
        sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    }

    //close <figured-bass>
	nIndent--;
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	sSource += _T("</figured-bass>\n");

	return sSource;
}

void lmFiguredBass::OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName)
{
	//invoked to add specific panels to the dialog

    WXUNUSED(sTabName)

	pDlg->AddPanel( new lmFBQuickPanel(pDlg->GetNotebook(), this),
				_("Quick selection"));

	//change dialog title
	pDlg->SetTitle(_("Figured bass properties"));
}

