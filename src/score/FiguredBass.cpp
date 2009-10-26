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
#include "../auxmusic/Chord.h"

typedef struct
{
    int             nBitmapID;
    wxString        sFiguredBass;
}
lmCommonFBData;

static const lmCommonFBData m_CommonFB[] = 
{
    { 0,    _T("#") },        //5 #3
    { 1,    _T("b") },        //5 b3
    { 2,    _T("=") },        //5 =3
    { 3,    _T("2") },        //6 4 2
    { 4,    _T("#2") },        //6 4 #2
    { 5,    _T("b2") },        //6 4 b2
    { 6,    _T("=2") },        //6 4 =2
    { 7,    _T("2+") },        //6 4 #2
    { 8,    _T("2 3") },        //7 4 2
    { 9,    _T("3") },        //3                 //better "5"
    { 10,   _T("4") },        //5 4
    //{ 11,   _T("4 3") },        //8 5 4 / 8 5 3
    { 12,   _T("4 2") },        //6 4 2
    { 13,   _T("4+ 2") },       //6 #4 2
    { 14,   _T("4 3") },        //6 4 3
    { 15,   _T("5") },        //5 3
    { 16,   _T("5 #") },        //5 #3
    { 17,   _T("5 b") },        //5 b3
    { 18,   _T("5+") },        //#5 3
    { 19,   _T("5/") },        //6 5 3
    { 20,   _T("5 3") },        //5 3
    { 21,   _T("5 4") },        //5 4
    { 22,   _T("6") },        //6 3
    { 23,   _T("6 #") },        //6 #3
    { 24,   _T("6 b") },        //6 b3
    { 25,   _T("6\\") },        //#6 3
    { 26,   _T("6 3") },        //6 3
    { 27,   _T("6 #3") },        //6 #3
    { 28,   _T("6 b3") },        //6 b3
    { 29,   _T("6 4") },        //6 4
    { 30,   _T("6 4 2") },        //6 4 2
    { 31,   _T("6 4 3") },        //6 4 3
    { 32,   _T("6 5") },        //6 3 5
    { 33,   _T("6 5 3") },        //6 5 3
    { 34,   _T("7") },        //7 5 3
    { 35,   _T("7 4 2") },        //7 4 2
    { 36,   _T("8") },        //8 5 3     //?
    { 37,   _T("9") },        //9 5 3
    { 38,   _T("10") },        //10 5 3    //?
};

const int lmFB_NUM_COMMON = sizeof(m_CommonFB) / sizeof(lmCommonFBData);

typedef struct
{
    wxString        sFigBass;
    wxString        sSimpler;
}
lmSimplerFBData;

static const lmSimplerFBData m_SimplerFB[] =
{
    { _T("5 3"),    _T("5") },
    { _T("#5 3"),   _T("5+") },
    { _T("5 #3"),   _T("#") },
    { _T("5 b3"),   _T("b") },
    { _T("5 =3"),   _T("=") },
    { _T("6 4 2"),  _T("2") },
    { _T("6 4 #2"), _T("#2") },
    { _T("6 4 b2"), _T("b2") },
    { _T("6 4 =2"), _T("=2") },
    { _T("6 4 #2"), _T("2+") },
    { _T("7 4 2"),  _T("2 3") },
    { _T("5 4"),    _T("4") },
    { _T("6 4 2"),  _T("4 2") },
    { _T("6 #4 2"), _T("4+ 2") },
    { _T("6 4 3"),  _T("4 3") },
    { _T("6 5 3"),  _T("5/") },
    { _T("6 3"),    _T("6") },
    { _T("6 #3"),   _T("6 #") },
    { _T("6 b3"),   _T("6 b") },
    { _T("#6 3"),   _T("#6") },
    { _T("=6 3"),   _T("=6") },
    { _T("b6 3"),   _T("b6") },
    { _T("6 3 5"),  _T("6 5") },
    { _T("7 5 3"),  _T("7") },
    { _T("b7 5 3"), _T("7/") },
    { _T("8 5 3"),  _T("8") },  
    { _T("9 5 3"),  _T("9") },
    { _T("10 5 3"), _T("10") },
};

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
            m_pFB->SetDataFromString(sFigBass);
        }
    }
}



//-----------------------------------------------------------------------------------------
// lmFiguredBassData implementation
//-----------------------------------------------------------------------------------------

lmFiguredBassData::lmFiguredBassData()
    : m_sError(_T(""))
{
    Initialize();
}

void lmFiguredBassData::Initialize()
{
    //initialize interval info
    for (int i=0; i <= lmFB_MAX_INTV; i++)
    {
        m_tFBInfo[i].nQuality = lm_eIM_NotPresent;
        m_tFBInfo[i].nAspect = lm_eIA_Normal;
        m_tFBInfo[i].sSource = _T("");
        m_tFBInfo[i].sPrefix = _T("");
        m_tFBInfo[i].sSuffix = _T("");
        m_tFBInfo[i].sOver = _T("");
        m_tFBInfo[i].fSounds = false;
    }
}

void lmFiguredBassData::CopyDataFrom(lmFiguredBassData* pFBData)
{
    for (int i=0; i <= lmFB_MAX_INTV; i++)
    {
        m_tFBInfo[i].nQuality = pFBData->m_tFBInfo[i].nQuality;
        m_tFBInfo[i].nAspect = pFBData->m_tFBInfo[i].nAspect;
        m_tFBInfo[i].sSource = pFBData->m_tFBInfo[i].sSource;
        m_tFBInfo[i].sPrefix = pFBData->m_tFBInfo[i].sPrefix;
        m_tFBInfo[i].sSuffix = pFBData->m_tFBInfo[i].sSuffix;
        m_tFBInfo[i].sOver = pFBData->m_tFBInfo[i].sOver;
        m_tFBInfo[i].fSounds = pFBData->m_tFBInfo[i].fSounds;
    }
}

lmFiguredBassData::lmFiguredBassData(lmChord* pChord, lmEKeySignatures nKey)
    : m_sError(_T(""))
{
    //Constructor from a lmChord and a key signature.
    //Useful to know the figured bass that encodes the chord. Key signature is
    //necessary to know if any chord note has accidentals.
    //
    //Steps:
    //1. Normalize chord and determine chord intervals
    //2. Encode intervals as number + accidentals
    //3. Create figured bass string
    //4. Build lmFiguredBassData from this figured bass string
    //5. Remove implicit intervals


        //1. Normalize chord and determine chord intervals

    //normalize chord
    lmChord oChord = *pChord;       //copy chord to not alter received chord
    oChord.Normalize();

    //get intervals present in the chord
    int nNumIntvals = oChord.GetNumIntervals();
    wxString sIntvals[lmINTERVALS_IN_CHORD];
    int nIntvalNum[lmINTERVALS_IN_CHORD];
    if (nNumIntvals > 0)
    {
        for (int i=1; i <= nNumIntvals; i++)
        {
            lmFIntval fi = oChord.GetInterval(i);
            sIntvals[i-1]= FIntval_GetIntvCode( fi );
            nIntvalNum[i-1] = FIntval_GetNumber(fi);
            wxASSERT(nIntvalNum[i-1] <= lmFB_MAX_INTV);
        }
    }
    else
        //No chord. Only root note.
        wxASSERT(false);


        //2. encode intervals as number + accidentals

    //get accidentals for desired key signature
    int nAccidentals[7];
    ::lmComputeAccidentals(nKey, nAccidentals);

    //compare note accidentals with key signature accidentals
    int nDiffAcc[lmNOTES_IN_CHORD];
    for (int i=0; i < oChord.GetNumNotes(); i++)
        nDiffAcc[i] = oChord.GetAccidentals(i) - nAccidentals[ oChord.GetStep(i) ];


        //3,4. create figured bass string and store info in this figured bass object
        //     member variables

    //initialize interval info
    Initialize();

    //store info about existing chord intervals
    for (int iIntv=oChord.GetNumIntervals()-1; iIntv >= 0; iIntv--)
    {
        int i = nIntvalNum[iIntv];
        wxASSERT(i <= lmFB_MAX_INTV);
        bool fIgnore = false;
        int nAcc = nDiffAcc[iIntv+1] - nDiffAcc[0];
        if (nAcc == -1)
        {
            m_tFBInfo[i].sSource += _T("b");
            m_tFBInfo[i].nQuality = lm_eIM_LowerHalf;
        }
        else if (nAcc == 1)
        {
            m_tFBInfo[i].sSource += _T("#");
            m_tFBInfo[i].nQuality = lm_eIM_RaiseHalf;
        }
        else if (nAcc == 0)
        {
            m_tFBInfo[i].sSource = _T("");
            m_tFBInfo[i].nQuality = lm_eIM_AsImplied;
        }
        else
        {
            wxLogMessage(_T("[lmFiguredBassData::lmFiguredBassData] No provision for this case."));
            fIgnore = true;
        }

        if (fIgnore)
        {
            //Not expected case. Ignore this interval to avoid errors/crashes
            m_tFBInfo[i].nQuality = lm_eIM_NotPresent;
            m_tFBInfo[i].sSource = _T("");
            m_tFBInfo[i].fSounds = false;
        }
        else
        {
            m_tFBInfo[i].sSource += wxString::Format(_T("%d"), nIntvalNum[iIntv] );
            m_tFBInfo[i].fSounds = true;
        }
    }


        //5. look for a simpler formulation

    wxString sFigBass = this->GetFiguredBassString();
    for (int i=0; i < sizeof(m_SimplerFB)/sizeof(lmSimplerFBData); i++)
    {
        if (m_SimplerFB[i].sFigBass == sFigBass)
        {
            this->SetDataFromString( (wxString)m_SimplerFB[i].sSimpler );
            break;
        }
    }
}

lmEIntervalQuality lmFiguredBassData::GetQuality(int nIntv)
{
    //return the interval quality for interval number nIntv (2..13)

    wxASSERT(nIntv >= lmFB_MIN_INTV && nIntv <= lmFB_MAX_INTV);
    return m_tFBInfo[nIntv].nQuality;
}

bool lmFiguredBassData::IntervalSounds(int nIntv)
{
    //return true if interval number nIntv (2..13) must be present in chord

    wxASSERT(nIntv >= lmFB_MIN_INTV && nIntv <= lmFB_MAX_INTV);
    return m_tFBInfo[nIntv].fSounds;
}

wxString lmFiguredBassData::GetFiguredBassString()
{
    //add source string
    wxString sFigBass = _T("");
    for (int i=lmFB_MAX_INTV; i >= lmFB_MIN_INTV; i--)
    {
        if (m_tFBInfo[i].nQuality != lm_eIM_NotPresent
            && m_tFBInfo[i].nAspect != lm_eIA_Understood)
        {
            if (sFigBass != _T(""))
                sFigBass += _T(" ");
            sFigBass += m_tFBInfo[i].sSource;
        }
    }
    return sFigBass;
}

bool lmFiguredBassData::IsEquivalent(lmFiguredBassData* pFBD)
{
    //Compares this figured bass with the received one. Returns true if both are
    //equivalent, that is, if both encode the same chord

    bool fOK = true;
    for (int i=lmFB_MAX_INTV; i >= lmFB_MIN_INTV && fOK; i--)
    {
        if (IntervalSounds(i) == pFBD->IntervalSounds(i))
        {
            if (GetQuality(i) == lm_eIM_NotPresent)
                fOK &= (pFBD->GetQuality(i) == lm_eIM_NotPresent
                        || pFBD->GetQuality(i) == lm_eIM_AsImplied);
            else if (pFBD->GetQuality(i) == lm_eIM_NotPresent)
                fOK &= (GetQuality(i) == lm_eIM_NotPresent
                        || GetQuality(i) == lm_eIM_AsImplied);
            else
                fOK &= (GetQuality(i) == pFBD->GetQuality(i));
        }
        else
            return false;
    }
    return fOK;
}

lmFiguredBassData::lmFiguredBassData(wxString& sData)
{
    SetDataFromString(sData);
}

void lmFiguredBassData::SetDataFromString(wxString& sData)
{
    //Creates intervals data from string.
    //Sets m_sError with empty string if no error, or with error message 

    Initialize();

    //interval data being parsed
    const wxChar* pStart;             //pointer to first char of interval string
    wxString sPrefix, sSuffix, sOver;
    lmEIntervalQuality nQuality;
    bool fParenthesis;
    wxString sIntval;
    wxString sFingerPrint = _T("");     //explicit present intervals (i.e. "53", "642")

    //Finite automata to parse the string

    //posible automata states
    enum lmFBState
    {
        lmFB_START,
        lmFB_PFX01,
        lmFB_NUM01,
        lmFB_PAR01,
        lmFB_END01,
        lmFB_ERROR,
        lmFB_FINISH,
    };

    int nState = lmFB_START;            //automata current state
    const wxChar* pDataStart = sData.c_str();     //points to first char of string
    const wxChar* p = pDataStart;                 //p points to char being parsed
    bool fContinueParsing = true;
    while (fContinueParsing)
    {
        switch (nState)
        {
            //Starting a new interval
            case lmFB_START:

                //initialize interval data
                pStart = p;
                sPrefix = _T("");
                sSuffix = _T("");
                sOver = _T("");
                sIntval = _T("");
                nQuality = lm_eIM_AsImplied;
                fParenthesis = false;

                if (*p == _T('('))
                {
                    fParenthesis = true;
                    ++p;    //GetNextChar()
                }
                nState = lmFB_PFX01;
                break;

            //New interval, without parenthesis
            case lmFB_PFX01:
                if (lmIsNumber(*p))
                {
                    sIntval = *p;
                    ++p;    //GetNextChar()
                    nState = lmFB_NUM01;
                }
                else if (*p == _T('#') || *p == _T('+'))
                {
                    sPrefix = *p;
                    nQuality = lm_eIM_RaiseHalf;
                    ++p;    //GetNextChar()
                    nState = lmFB_NUM01;
                }
                else if (*p == _T('b') || *p == _T('-'))
                {
                    sPrefix = *p;
                    nQuality = lm_eIM_LowerHalf;
                    ++p;    //GetNextChar()
                    nState = lmFB_NUM01;
                }
                else if (*p == _T('='))
                {
                    sPrefix = *p;
                    nQuality = lm_eIM_Natural;
                    ++p;    //GetNextChar()
                    nState = lmFB_NUM01;
                }
                else
                    nState = lmFB_ERROR;
                break;

            //interval number
            case lmFB_NUM01:
                if (lmIsNumber(*p))
                {
                    sIntval += *p;
                    ++p;    //GetNextChar()
                }
                else if (*p == _T('#') || *p == _T('+'))
                {
                    sSuffix = *p;
                    nQuality = lm_eIM_RaiseHalf;
                    ++p;    //GetNextChar()
                    nState = lmFB_PAR01;
                }
                else if (*p == _T('b') || *p == _T('-'))
                {
                    sSuffix = *p;
                    nQuality = lm_eIM_LowerHalf;
                    ++p;    //GetNextChar()
                    nState = lmFB_PAR01;
                }
                else if (*p == _T('/'))
                {
                    sOver = *p;
                    nQuality = lm_eIM_RaiseHalf;
                    ++p;    //GetNextChar()
                    nState = lmFB_PAR01;
                }
                else if (*p == _T('\\'))
                {
                    sOver = *p;
                    nQuality = lm_eIM_LowerHalf;
                    ++p;    //GetNextChar()
                    nState = lmFB_PAR01;
                }
                else
                    nState = lmFB_PAR01;
                break;

            //close parenthesis
            case lmFB_PAR01:
                if (*p == _T(')'))
                {
                    nState = (fParenthesis ? lmFB_END01 : lmFB_ERROR);
                    ++p;    //GetNextChar()
                }
                else
                    nState = lmFB_END01;
                break;

            //one interval finished
            case lmFB_END01:

                //one interval
                if (*p == _T(' ') || *p == _T('\0'))
                {
                    //one interval completed. Get interval number. If number not present
                    //assume 3
                    long nIntv;
                    if (sIntval != _T(""))
                        sIntval.ToLong(&nIntv);
                    else
                    {
                        nIntv = 3L;
                        m_tFBInfo[nIntv].nAspect = lm_eIA_Understood;
                    }
                    //transfer data to total variables
                    m_tFBInfo[nIntv].nQuality = nQuality;
                    if (fParenthesis)
                        m_tFBInfo[nIntv].nAspect = lm_eIA_Parenthesis;
                    m_tFBInfo[nIntv].sSource = sData.Mid((size_t)(pStart-pDataStart), (size_t)(p-pStart) );
                    m_tFBInfo[nIntv].sPrefix = sPrefix;
                    m_tFBInfo[nIntv].sSuffix = sSuffix;
                    m_tFBInfo[nIntv].sOver = sOver;
                    m_tFBInfo[nIntv].fSounds = true;

                    //add interval to finger print
                    sFingerPrint += wxString::Format(_T("%d"), nIntv);

                    //continue with next interval or finish parser
                    if (*p == _T(' '))
                    {
                        nState = lmFB_START;
                        ++p;    //GetNextChar()
                    }
                    else
                        fContinueParsing = false;
                }
                else
                    nState = lmFB_ERROR;
                break;

            //Error state
            case lmFB_ERROR:
                m_sError = wxString::Format(_T("Invalid char %c (after %s) in figured bass string %s. Figured bass ignored"),
                    *p, sData.Left(size_t(p-pDataStart-1)).c_str(), sData.c_str() );
                fContinueParsing = false;
                break;

            default:
                wxASSERT(false);
                fContinueParsing = false;
        }
    }

    //determine implicit intervals that exists although not present in figured bass notation
    if (sFingerPrint == _T("3"))        //5 3
    {
        m_tFBInfo[5].fSounds = true;        //add 5th
    }
    else if (sFingerPrint == _T("4"))   //5 4
    {
        m_tFBInfo[5].fSounds = true;        //add 5th
    }
    else if (sFingerPrint == _T("5"))   //5 3
    {
        m_tFBInfo[3].fSounds = true;        //add 3rd
    }
    else if (sFingerPrint == _T("6"))   //6 3
    {
        m_tFBInfo[3].fSounds = true;        //add 3rd
    }
    else if (sFingerPrint == _T("2"))   //6 4 2
    {
        m_tFBInfo[6].fSounds = true;        //add 6th
        m_tFBInfo[4].fSounds = true;        //add 4th
    }
    else if (sFingerPrint == _T("42"))  //6 4 2
    {
        m_tFBInfo[6].fSounds = true;        //add 6th
        m_tFBInfo[4].fSounds = true;        //add 4th
    }
    else if (sFingerPrint == _T("43"))  //6 4 3
    {
        m_tFBInfo[6].fSounds = true;        //add 6th
    }
    else if (sFingerPrint == _T("65"))  //6 5 3
    {
        m_tFBInfo[3].fSounds = true;        //add 3rd
    }
    else if (sFingerPrint == _T("7"))  //7 5 3
    {
        m_tFBInfo[5].fSounds = true;        //add 5th
        m_tFBInfo[3].fSounds = true;        //add 3rd
    }
    else if (sFingerPrint == _T("9"))  //9 5 3
    {
        m_tFBInfo[5].fSounds = true;        //add 5th
        m_tFBInfo[3].fSounds = true;        //add 3rd
    }
    else if (sFingerPrint == _T("10"))  //10 5 3
    {
        m_tFBInfo[5].fSounds = true;        //add 5th
        m_tFBInfo[3].fSounds = true;        //add 3rd
    }
}




//-----------------------------------------------------------------------------------------
// lmFiguredBass implementation
//-----------------------------------------------------------------------------------------

lmFiguredBass::lmFiguredBass(lmVStaff* pVStaff, long nID, lmFiguredBassData* pFBData)
    : lmStaffObj(pVStaff, nID, lm_eSO_FiguredBass, pVStaff, 1, lmVISIBLE, lmDRAGGABLE)
    , lmFiguredBassData()
    , m_fStartOfLine(false)
    , m_fEndOfLine(false)
    , m_fParenthesis(false)
{
    //Constructor from lmFiguredBassData

    SetLayer(lm_eLayerNotes);
    CopyDataFrom(pFBData);
}

lmFiguredBass::lmFiguredBass(lmVStaff* pVStaff, long nID, lmChord* pChord,
                             lmEKeySignatures nKey)
    : lmStaffObj(pVStaff, nID, lm_eSO_FiguredBass, pVStaff, 1, lmVISIBLE, lmDRAGGABLE)
    , lmFiguredBassData(pChord, nKey)
    , m_fStartOfLine(false)
    , m_fEndOfLine(false)
    , m_fParenthesis(false)
{
    //Constructor from a lmChord and a key signature.

    SetLayer(lm_eLayerNotes);
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
    sSource += GetFiguredBassString();
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

#ifdef __WXDEBUG__
//--------------------------------------------------------------------------------
//Methods for debugging
//--------------------------------------------------------------------------------

bool lmFiguredBassUnitTests()
{
    //Unit test for lmFiguredBass contructor from lmChord
    //returns true if test passed correctly

    typedef struct lmTestDataStruct
    {
        lmEKeySignatures    nKey;       //key signature
        wxString            sFigBass;   //test result, to validate test
        int                 nNumNotes;  //num notes in chord
        wxString            sNotes[6];  //notes for chord
    }
    lmTestData;

    static lmTestData tTestChords[] =
    {
        //          Test       num.
        // nKey     result    notes   nChord notes, up to 6. LDP encoding
        { earmDo, _T("5"),      6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        { earmDo, _T("6"),      5,  {_T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        { earmRe, _T("6 4"),    4,  {_T("a2"), _T("+f3"), _T("d4"), _T("+f4") }},
        { earmDo, _T("7"),      5,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("b4") }},
        { earmDo, _T("7/"),     4,  {_T("c3"), _T("-b3"), _T("e4"), _T("g4") }},
        { earmDo, _T("4"),      4,  {_T("e3"), _T("-b3"), _T("g4"), _T("c5") }},
        { earmDo, _T("9"),      5,  {_T("a2"), _T("d3"), _T("d4"), _T("+f4"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
        //{ earmDo, _T("(5)"),    6,  {_T("c3"), _T("e3"), _T("e4"), _T("g4"), _T("c5"), _T("e5") }},
    };

    wxLogMessage(_T("UnitTests: Figured bass contructor from lmChord"));
    wxLogMessage(_T("==============================================="));
    lmLDPParser parserLDP(_T("en"), _T("utf-8"));
    lmScore* pScore = new lmScore();
    lmInstrument* pInstr = pScore->AddInstrument(0,0, _T(""));
    lmVStaff* pVStaff = pInstr->GetVStaff();
    pVStaff->AddClef( lmE_Sol );
    pVStaff->AddKeySignature( earmDo );
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );

    int nNumTestCases = sizeof(tTestChords) / sizeof(lmTestData);
    bool fTestOK = true;
    for (int i=0; i < nNumTestCases; i++)
    {
        lmChord oChord(tTestChords[i].nNumNotes, &(tTestChords[i].sNotes[0]), tTestChords[i].nKey);
        lmFiguredBass* pFB = new lmFiguredBass(pVStaff, lmNEW_ID, &oChord, tTestChords[i].nKey);
        bool fOK = (tTestChords[i].sFigBass == pFB->GetFiguredBassString());
        fTestOK &= fOK;
        if (!fOK)
            wxLogMessage(_T("%s. Case %d, expecting'%s' but result is '%s'"),
                (fOK ? _T("OK") : _T("FAIL")),
                i, tTestChords[i].sFigBass.c_str(),
                pFB->GetFiguredBassString().c_str() );

        delete pFB;
    }
    delete pScore;

    return fTestOK;        
}

#endif      //Debug global methods
