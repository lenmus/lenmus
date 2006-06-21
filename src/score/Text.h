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
/*! @file Text.h
    @brief Header file for classes lmBasicText and lmText
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __SBTEXT__H__        //to avoid nested includes
#define __SBTEXT__H__


class lmBasicText
{
public:
    lmBasicText(wxString sText, wxString sLanguage,
                   RXMLPositionData oPos, RFontData oFontData);

    ~lmBasicText() {}

    void SetText(wxString sText) { m_sText = sText; }
    void SetLanguage(wxString sLanguage) { m_sLanguage = sLanguage; }


protected:
    wxString    m_sText;
    wxString    m_sLanguage;

    // position
    lmTenths    m_xDef, m_yDef;
    lmTenths    m_xRel, m_yRel;
    bool        m_fOverrideDefaultX;
    bool        m_fOverrideDefaultY;

    // font
    wxString    m_sFontName;
    int         m_nFontSize;
    bool        m_fBold;
    bool        m_fItalic;

};


class lmText :  public lmSimpleObj
{
public:
    lmText(lmScore* pScore, wxString sText,
           lmLUnits xPos=0, lmLUnits yPos=0, bool fXAbs=false, bool fYAbs=false, 
           wxString sFontName=_T("Arial"), int nFontSize=12, 
           bool fBold=false, bool fItalic=false);

    lmText(lmScore* pScore, wxString sTitle, lmEAlignment nAlign,
           lmLUnits xPos, lmLUnits yPos, 
           wxString sFontName, int nFontSize, lmETextStyle nStyle);

    ~lmText() {}

    // properties related to the clasification of this lmStaffObj
    //EPositioningType GetPositioningType() { return ePos_Predefined; }
    float GetTimePosIncrement() { return 0; }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC);
    wxBitmap* GetBitmap(double rScale);
    void SetFont(lmPaper* pPaper);

    //    debugging
    wxString Dump();
    wxString SourceLDP();
    wxString SourceXML();

    //specific method of this object
    void SetText(wxString text) { m_sText = text; }
    lmEAlignment GetAlignment() { return m_nAlignment; }


private:
    wxString        m_sText;
    lmScore*        m_pScore;
    lmLUnits        m_xPos;
    lmLUnits        m_yPos;
    bool            m_fXAbs;
    bool            m_fYAbs;
    wxString        m_sFontName;
    int             m_nFontSize;
    bool            m_fBold;
    bool            m_fItalic;
    lmEAlignment    m_nAlignment;

};

//global functions defined in this module
extern int PointsToLUnits(lmLUnits nPoints);

#endif    // __SBTEXT__H__

