//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifdef __GNUG__
#pragma implementation "KeySignature.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "Score.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;



static wxString m_sKeySignatureName[30];
static wxString m_sLDPKeyName[30] = {
        _T("C"), _T("G"), _T("D"), _T("A"), _T("E"), _T("B"),
        _T("F+"), _T("C+"), _T("C-"), _T("G-"), _T("D-"), _T("A-"),
        _T("E-"), _T("B-"), _T("F"), _T("a"), _T("e"), _T("b"),
        _T("f+"), _T("c+"), _T("g+"), _T("d+"), _T("a+"), _T("a-"),
        _T("e-"), _T("b-"), _T("f"), _T("c"), _T("g"), _T("d") };


//-------------------------------------------------------------------------------------------------
// lmKeySignature object implementation
//-------------------------------------------------------------------------------------------------

//
//constructors and destructor
//

//constructor for traditional key signatures
lmKeySignature::lmKeySignature(int nFifths, bool fMajor, lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_KeySignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    m_sKeySignatureName[0] = _("C Major");
    m_sKeySignatureName[1] = _("G Major");
    m_sKeySignatureName[2] = _("D Major");
    m_sKeySignatureName[3] = _("A Major");
    m_sKeySignatureName[4] = _("E Major");
    m_sKeySignatureName[5] = _("B Major");
    m_sKeySignatureName[6] = _("F # Major");
    m_sKeySignatureName[7] = _("C # Major");
    m_sKeySignatureName[8] = _("C b Major");
    m_sKeySignatureName[9] = _("G b Major");
    m_sKeySignatureName[10] = _("D b Major");
    m_sKeySignatureName[11] = _("A b Major");
    m_sKeySignatureName[12] = _("E b Major");
    m_sKeySignatureName[13] = _("B b Major");
    m_sKeySignatureName[14] = _("F Major");
    m_sKeySignatureName[15] = _("A minor");
    m_sKeySignatureName[16] = _("E minor");
    m_sKeySignatureName[17] = _("B minor");
    m_sKeySignatureName[18] = _("F # minor");
    m_sKeySignatureName[19] = _("C # minor");
    m_sKeySignatureName[20] = _("G # minor");
    m_sKeySignatureName[21] = _("D # minor");
    m_sKeySignatureName[22] = _("A # minor");
    m_sKeySignatureName[23] = _("A b minor");
    m_sKeySignatureName[24] = _("E b minor");
    m_sKeySignatureName[25] = _("B b minor");
    m_sKeySignatureName[26] = _("F minor");
    m_sKeySignatureName[27] = _("C minor");
    m_sKeySignatureName[28] = _("G minor");
    m_sKeySignatureName[29] = _("D minor");

    m_fHidden = false;
    m_fTraditional = true;
    m_nFifths = nFifths;
    m_fMajor = fMajor;

    SetKeySignatureType();

    g_pLogger->LogTrace(_T("lmKeySignature"),
        _T("[lmKeySignature::lmKeySignature] m_nFifths=%d, m_fMajor=%s, nKey=%d"),
            m_nFifths, (m_fMajor ? _T("yes") : _T("no")), m_nKeySignature );

}

wxString lmKeySignature::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tKey Sign. fifths: %d Key=%s %s\tTimePos=%.2f\n"),
        m_nId, m_nFifths, m_sLDPKeyName[m_nKeySignature], (m_fMajor ? _T("major") : _T("minor")), m_rTimePos );
    return sDump;
}

wxString lmKeySignature::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(key ");
    sSource += m_sLDPKeyName[m_nKeySignature];

    //visible?
    if (!m_fVisible) { sSource += _T(" noVisible"); }

    //location
    sSource += SourceLDP_Location(m_uPaperPos);

	//attached AuxObjs
	sSource += lmStaffObj::SourceLDP(nIndent+1);

    sSource += _T(")\n");
    return sSource;

}

wxString lmKeySignature::SourceXML(int nIndent)
{
    //TODO
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("TODO: lmKeySignature::SourceXML\n");

	return sSource;
}

lmUPoint lmKeySignature::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	//TODO
	return uPos;
}

lmLUnits lmKeySignature::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model. 
    // Paper cursor must be used as the base for positioning.


    //get the position on which the time signature must be drawn
    lmLUnits uxLeft = uPos.x;
	lmLUnits uyTop = uPos.y + m_pVStaff->GetStaffOffset(m_nStaffNum);

    //Key signature is common to all lmVStaff staves of the instrument, but the lmStaffObj
    //representing it is only present in the first staff. Therefore, for renderization, it
    //is necessary to repeat the shape in each staff of the instrument
    //So in the following loop we add the key signature shape for each VStaff of the
    //instrument
    lmCompositeShape* pShape;
    lmStaff* pStaff = m_pVStaff->GetFirstStaff();
    for (int nStaff=1; pStaff; pStaff = m_pVStaff->GetNextStaff(), nStaff++)
    {
        //get current clef
        lmClef* pClef = pStaff->GetCurrentClef();
        lmEClefType nClef = pClef->GetClefType();

        // Add the shape for key signature
        pShape = CreateShape(pBox, pPaper, lmUPoint(uxLeft, uyTop), nClef, colorC, pStaff);

        //compute vertical displacement for next staff
        uyTop += pStaff->GetHeight();
        uyTop += pStaff->GetAfterSpace();
    }

	// set total width (incremented in one line for after space)
	return pShape->GetWidth() + m_pVStaff->TenthsToLogical(10, m_nStaffNum);

}

lmCompositeShape* lmKeySignature::CreateShape(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos,
					              lmEClefType nClef, wxColour colorC, lmStaff* pStaff)
{
    // This method is also used when rendering the prolog

    //create the container shape object
    lmCompositeShape* pShape = new lmCompositeShape(this, _T("Key signature"), lmDRAGGABLE);
	pBox->AddShape(pShape);
    m_pShape = pShape;

    lmLUnits uSharpPos[8];      //sharps positions, in order of sharps appearance
    lmLUnits uFlatPos[8];       //flats positions, in order of flats appearance
    lmLUnits uOneLine;          //space, in logical units, for half line

    uOneLine = pStaff->TenthsToLogical(10.0);
    lmEKeySignatures nKeySignature = m_nKeySignature;

    //Compute position of sharps and flats. Depends on the clef
	lmLUnits yPos = uPos.y;
    switch(nClef) {
        case lmE_Sol:
            uSharpPos[1] = yPos - 5 * uOneLine;         //line 5 (Fa)
            uSharpPos[2] = yPos - 3.5 * uOneLine;       //space between lines 3 y 4 (Do)
            uSharpPos[3] = yPos - 5.5 * uOneLine;       //space above line 5 (Sol)
            uSharpPos[4] = yPos - 4 * uOneLine;         //line 4 (Re)
            uSharpPos[5] = yPos - 2.5 * uOneLine;       //space between lines 2 y 3 (La)
            uSharpPos[6] = yPos - 4.5 * uOneLine;       //space between lines 4 y 5 (Mi)
            uSharpPos[7] = yPos - 3 * uOneLine;         //line 3 (Si)

            uFlatPos[1] = yPos - 3 * uOneLine;          //line 3 (Si)
            uFlatPos[2] = yPos - 4.5 * uOneLine;        //space between lines 4 y 5 (Mi)
            uFlatPos[3] = yPos - 2.5 * uOneLine;        //space between lines 2 y 3 (La)
            uFlatPos[4] = yPos - 4 * uOneLine;          //line 4 (Re)
            uFlatPos[5] = yPos - 2 * uOneLine;          //line 2 (Sol)
            uFlatPos[6] = yPos - 3.5 * uOneLine;        //space between lines 3 y 4 (Do)
            uFlatPos[7] = yPos - 1.5 * uOneLine;        //space between lines 1 y 2 (Fa)
            break;

        case lmE_Fa4:
            uSharpPos[1] = yPos - 4 * uOneLine;         //line 4 (Fa)
            uSharpPos[2] = yPos - 2.5 * uOneLine;       //space between lines 2 y 3 (Do)
            uSharpPos[3] = yPos - 4.5 * uOneLine;       //space between lines 4 y 5 (Sol)
            uSharpPos[4] = yPos - 3 * uOneLine;         //line 3 (Re)
            uSharpPos[5] = yPos - 1.5 * uOneLine;       //line 5 (La)
            uSharpPos[6] = yPos - 3.5 * uOneLine;       //space between lines 3 y 4 (Mi)
            uSharpPos[7] = yPos - 2 * uOneLine;         //space aboveline 5 (Si)

            uFlatPos[1] = yPos - 2 * uOneLine;          //line 2 (Si)
            uFlatPos[2] = yPos - 3.5 * uOneLine;        //space between lines 3 y 4 (Mi)
            uFlatPos[3] = yPos - 1.5 * uOneLine;        //space between lines 1 y 2 (La)
            uFlatPos[4] = yPos - 3 * uOneLine;          //line 3 (Re)
            uFlatPos[5] = yPos - uOneLine;              //line 1 (Sol)
            uFlatPos[6] = yPos - 2.5 * uOneLine;        //space between lines 2 y 3 (Do)
            uFlatPos[7] = yPos - 4 * uOneLine;          //linea 4 (Fa)
            break;

        case lmE_Fa3:
            wxASSERT(false);        //TODO Clef Fa3
            break;

        case lmE_Do1:
            uSharpPos[1] = yPos - 2.5 * uOneLine;       //space between lines 2 y 3 (Fa)
            uSharpPos[2] = yPos - uOneLine;             //line 1 (Do)
            uSharpPos[3] = yPos - 3 * uOneLine;         //line 3 (Sol)
            uSharpPos[4] = yPos - 1.5 * uOneLine;       //space between lines 1 y 2 (Re)
            uSharpPos[5] = yPos - 3.5 * uOneLine;       //space between lines 3 y 4 (La)
            uSharpPos[6] = yPos - 2 * uOneLine;         //line 2 (Mi)
            uSharpPos[7] = yPos - 4 * uOneLine;         //linea 4 (Si)

            uFlatPos[1] = yPos - 4 * uOneLine;          //line 4 (Si)
            uFlatPos[2] = yPos - 2 * uOneLine;          //line 2 (Mi)
            uFlatPos[3] = yPos - 3.5 * uOneLine;        //space between lines 3 y 4 (La)
            uFlatPos[4] = yPos - 1.5 * uOneLine;        //space between lines 1 y 2 (Re)
            uFlatPos[5] = yPos - 3 * uOneLine;          //line 3 (Sol)
            uFlatPos[6] = yPos - uOneLine;              //line 1 (Do)
            uFlatPos[7] = yPos - 2.5 * uOneLine;        //space between lines 2 y 3 (Fa)
            break;

        case lmE_Do2:
            wxASSERT(false);        //TODO Clef Do2
            break;
        case lmE_Do3:
            wxASSERT(false);        //TODO Clef Do3
            break;
        case lmE_Do4:
            wxASSERT(false);        //TODO Clef Do4
            break;
        case lmE_Percussion:
            nKeySignature = earmDo;    //force not to draw any accidentals
            break;
        default:
            wxASSERT(false);
    }

    // Check if it is necessary to draw sharps or flats, and how many.
    int nNumAccidentals = KeySignatureToNumFifths(nKeySignature);
    bool fDrawSharps = (nNumAccidentals > 0);    //true if sharps, false if flats

    g_pLogger->LogTrace(_T("lmKeySignature"),
        _T("[lmKeySignature::DrawAt] nNumAccidentals=%d, fDrawSharps=%s, m_nFifths=%d, m_fMajor=%s, nKey=%d"),
            nNumAccidentals, (fDrawSharps ? _T("yes") : _T("no")),
            m_nFifths, (m_fMajor ? _T("yes") : _T("no")), nKeySignature );

    //add shapes for the required flats / sharps
    nNumAccidentals = abs(nNumAccidentals);
    lmLUnits nWidth = 0;
    for (int i=1; i <= nNumAccidentals; i++)
    {
        lmLUnits yPos = (fDrawSharps ? uSharpPos[i] : uFlatPos[i]);
        lmShape* pSA = AddAccidental(fDrawSharps, pPaper, lmUPoint(uPos.x+nWidth, yPos),
                                     colorC, pStaff);
        pShape->Add(pSA);
        nWidth += pSA->GetWidth();
    }

    return pShape;
}


lmShape* lmKeySignature::AddAccidental(bool fSharp, lmPaper* pPaper, lmUPoint uPos,
					                   wxColour colorC, lmStaff* pStaff)
{
    //create a shape for the accidental at position uxLeft, uyTop. 
    //Returns it

    int nGlyph;
    if (fSharp) 
        nGlyph = GLYPH_SHARP_ACCIDENTAL;
    else
        nGlyph = GLYPH_FLAT_ACCIDENTAL;

    lmLUnits yPos = uPos.y - pStaff->TenthsToLogical(aGlyphsInfo[nGlyph].GlyphOffset);
    return new lmShapeGlyph(this, nGlyph, GetSuitableFont(pPaper), pPaper,
							lmUPoint(uPos.x, yPos), _T("Accidental"));

}

lmLUnits lmKeySignature::AddShape(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, lmEClefType nClef,
                    int nStaff, wxColour colorC)
{
    // This method is, primarely, to be used when rendering the prolog
    // Returns the width of the draw


    // get the staff
    lmStaff* pStaff = m_pVStaff->GetStaff(nStaff);

    // create the shape for key signature
    lmShape* pShape = CreateShape(pBox, pPaper, uPos, nClef, colorC, pStaff);

    return pShape->GetWidth();
}

void lmKeySignature::SetKeySignatureType()
{
    if (m_fMajor) {
        switch (m_nFifths) {
            case 0:     m_nKeySignature = earmDo;    break;
            case 1:     m_nKeySignature = earmSol;   break;
            case 2:     m_nKeySignature = earmRe;    break;
            case 3:     m_nKeySignature = earmLa;    break;
            case 4:     m_nKeySignature = earmMi;    break;
            case 5:     m_nKeySignature = earmSi;    break;
            case 6:     m_nKeySignature = earmFas;   break;
            case 7:     m_nKeySignature = earmDos;   break;
            case -1:    m_nKeySignature = earmFa;    break;
            case -2:    m_nKeySignature = earmSib;   break;
            case -3:    m_nKeySignature = earmMib;   break;
            case -4:    m_nKeySignature = earmLab;   break;
            case -5:    m_nKeySignature = earmReb;   break;
            case -6:    m_nKeySignature = earmSolb;  break;
            case -7:    m_nKeySignature = earmDob;   break;
            default:
                wxASSERT(false);
        }
    }
    else {
        switch (m_nFifths) {
            case 0:     m_nKeySignature = earmLam;    break;
            case 1:     m_nKeySignature = earmMim;    break;
            case 2:     m_nKeySignature = earmSim;    break;
            case 3:     m_nKeySignature = earmFasm;   break;
            case 4:     m_nKeySignature = earmDosm;   break;
            case 5:     m_nKeySignature = earmSolsm;  break;
            case 6:     m_nKeySignature = earmResm;   break;
            case 7:     m_nKeySignature = earmLasm;   break;
            case -1:    m_nKeySignature = earmRem;    break;
            case -2:    m_nKeySignature = earmSolm;   break;
            case -3:    m_nKeySignature = earmDom;    break;
            case -4:    m_nKeySignature = earmFam;    break;
            case -5:    m_nKeySignature = earmSibm;   break;
            case -6:    m_nKeySignature = earmMibm;   break;
            case -7:    m_nKeySignature = earmLabm;   break;
            default:
                wxASSERT(false);
        }
    }

}


//---------------------------------------------------------------------------------------
// Global methods related to Key signatures
//---------------------------------------------------------------------------------------

void ComputeAccidentals(lmEKeySignatures nKeySignature, int nAccidentals[])
{
    // Given a key signature (nKeySignature) this function fills the array
    // nAccidentals with the accidentals implied by the key signature.
    // Each element of the array refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
    // and its value can be one of:
    //     0  = no accidental
    //    -1  = a flat
    //     1  = a sharp

    // initialize array: no accidentals
    for (int i=0; i < 7; i++) {
        nAccidentals[i] = 0;
    }

    // accidentals implied by the key signature
    switch (nKeySignature) {
        case earmDo:
        case earmLam:
            //no accidentals
            break;

        //Sharps ---------------------------------------
        case earmSol:
        case earmMim:
            nAccidentals[3] = 1;     //Fa #
            break;
        case earmRe:
        case earmSim:
            nAccidentals[3] = 1;     //Fa #
            nAccidentals[0] = 1;     //Do #
            break;
        case earmLa:
        case earmFasm:
            nAccidentals[3] = 1;     //Fa #
            nAccidentals[0] = 1;     //Do #
            nAccidentals[4] = 1;     //Sol #
             break;
       case earmMi:
        case earmDosm:
            nAccidentals[3] = 1;     //Fa #
            nAccidentals[0] = 1;     //Do #
            nAccidentals[4] = 1;     //Sol #
            nAccidentals[1] = 1;     //Re #
            break;
        case earmSi:
        case earmSolsm:
            nAccidentals[3] = 1;     //Fa #
            nAccidentals[0] = 1;     //Do #
            nAccidentals[4] = 1;     //Sol #
            nAccidentals[1] = 1;     //Re #
            nAccidentals[5] = 1;     //La #
            break;
        case earmFas:
        case earmResm:
            nAccidentals[3] = 1;     //Fa #
            nAccidentals[0] = 1;     //Do #
            nAccidentals[4] = 1;     //Sol #
            nAccidentals[1] = 1;     //Re #
            nAccidentals[5] = 1;     //La #
            nAccidentals[2] = 1;     //Mi #
            break;
        case earmDos:
        case earmLasm:
            nAccidentals[3] = 1;     //Fa #
            nAccidentals[0] = 1;     //Do #
            nAccidentals[4] = 1;     //Sol #
            nAccidentals[1] = 1;     //Re #
            nAccidentals[5] = 1;     //La #
            nAccidentals[2] = 1;     //Mi #
            nAccidentals[6] = 1;     //Si #
             break;

        //Flats -------------------------------------------
        case earmFa:
        case earmRem:
            nAccidentals[6] = -1;         //Si b
            break;
        case earmSib:
        case earmSolm:
            nAccidentals[6] = -1;         //Si b
            nAccidentals[2] = -1;         //Mi b
             break;
       case earmMib:
        case earmDom:
            nAccidentals[6] = -1;         //Si b
            nAccidentals[2] = -1;         //Mi b
            nAccidentals[5] = -1;         //La b
            break;
        case earmLab:
        case earmFam:
            nAccidentals[6] = -1;         //Si b
            nAccidentals[2] = -1;         //Mi b
            nAccidentals[5] = -1;         //La b
            nAccidentals[1] = -1;         //Re b
             break;
        case earmReb:
        case earmSibm:
            nAccidentals[6] = -1;         //Si b
            nAccidentals[2] = -1;         //Mi b
            nAccidentals[5] = -1;         //La b
            nAccidentals[1] = -1;         //Re b
            nAccidentals[4] = -1;         //Sol b
            break;
        case earmSolb:
        case earmMibm:
            nAccidentals[6] = -1;         //Si b
            nAccidentals[2] = -1;         //Mi b
            nAccidentals[5] = -1;         //La b
            nAccidentals[1] = -1;         //Re b
            nAccidentals[4] = -1;         //Sol b
            nAccidentals[0] = -1;         //Do b
            break;
        case earmDob:
        case earmLabm:
            nAccidentals[6] = -1;         //Si b
            nAccidentals[2] = -1;         //Mi b
            nAccidentals[5] = -1;         //La b
            nAccidentals[1] = -1;         //Re b
            nAccidentals[4] = -1;         //Sol b
            nAccidentals[0] = -1;         //Do b
            nAccidentals[3] = -1;         //Fa b
            break;
        default:
            wxASSERT(false);
    }

}

int GetRootNoteIndex(lmEKeySignatures nKeySignature)
{
    //returns the index (0..6, 0=Do, 1=Re, 3=Mi, ... , 6=Si) to the root note for
    //the Key signature. For example, if nKeySignature is La sharp minor it returns
    //index = 5 (La)

    //compute root note
    int nRootNote;
    switch(nKeySignature) {
        case earmDo:
        case earmDom:
        case earmDosm:
        case earmDos:
        case earmDob:
            nRootNote = 0;
            break;
        case earmRe:
        case earmReb:
        case earmResm:
        case earmRem:
            nRootNote = 1;
            break;
        case earmMi:
        case earmMim:
        case earmMib:
        case earmMibm:
            nRootNote = 2;
            break;
        case earmFa:
        case earmFasm:
        case earmFas:
        case earmFam:
            nRootNote = 3;
            break;
        case earmSol:
        case earmSolsm:
        case earmSolm:
        case earmSolb:
            nRootNote = 4;
            break;
        case earmLa:
        case earmLam:
        case earmLasm:
        case earmLab:
        case earmLabm:
            nRootNote = 5;
            break;
        case earmSim:
        case earmSi:
        case earmSib:
        case earmSibm:
            nRootNote = 6;
            break;
        default:
            wxASSERT(false);
    }

    return nRootNote;

}

bool IsMajor(lmEKeySignatures nKeySignature)
{
    return (nKeySignature < earmLam);
}

const wxString& GetKeySignatureName(lmEKeySignatures nKeySignature)
{
    return m_sKeySignatureName[nKeySignature - lmMIN_KEY];
}

int KeySignatureToNumFifths(lmEKeySignatures nKeySignature)
{
    // Retunrs the number of fifths that corresponds to the encoded key signature

    int nFifths = 0;        //num accidentals to draw (0..7)
    switch(nKeySignature) {
        case earmDo:
        case earmLam:
            nFifths = 0;
            break;

        //Sharps ---------------------------------------
        case earmSol:
        case earmMim:
            nFifths = 1;
            break;
        case earmRe:
        case earmSim:
            nFifths = 2;
            break;
        case earmLa:
        case earmFasm:
            nFifths = 3;
            break;
        case earmMi:
        case earmDosm:
            nFifths = 4;
            break;
        case earmSi:
        case earmSolsm:
            nFifths = 5;
            break;
        case earmFas:
        case earmResm:
            nFifths = 6;
            break;
        case earmDos:
        case earmLasm:
            nFifths = 7;
            break;

        //Flats -------------------------------------------
        case earmFa:
        case earmRem:
            nFifths = -1;
            break;
        case earmSib:
        case earmSolm:
            nFifths = -2;
            break;
        case earmMib:
        case earmDom:
            nFifths = -3;
            break;
        case earmLab:
        case earmFam:
            nFifths = -4;
            break;
        case earmReb:
        case earmSibm:
            nFifths = -5;
            break;
        case earmSolb:
        case earmMibm:
            nFifths = -6;
            break;
        case earmDob:
        case earmLabm:
            nFifths = -7;
            break;
        default:
            wxASSERT(false);
    }
    return nFifths;

}

lmEKeySignatures GetRelativeMinorKey(lmEKeySignatures nMajorKey)
{
    switch(nMajorKey) {
        case earmDo:
            return earmLam;
        case earmSol:
            return earmMim;
        case earmRe:
            return earmSim;
        case earmLa:
            return earmFasm;
        case earmMi:
            return earmDosm;
        case earmSi:
            return earmSolsm;
        case earmFas:
            return earmResm;
        case earmDos:
            return earmLasm;
        case earmFa:
            return earmRem;
        case earmSib:
            return earmSolm;
        case earmMib:
            return earmDom;
        case earmLab:
            return earmFam;
        case earmReb:
            return earmSibm;
        case earmSolb:
            return earmMibm;
        case earmDob:
            return earmLabm;
        default:
            wxASSERT(false);
            return earmDom;
    }

}

