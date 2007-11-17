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

#ifndef __DIRECTION_H__        //to avoid nested includes
#define __DIRECTION_H__

#ifdef __GNUG__
#pragma interface "Direction.cpp"
#endif

class lmWordsDirection : public lmStaffObj, public lmBasicText
{
public:
    lmWordsDirection(lmVStaff* pStaff, wxString sText, lmEAlignment nAlign,
                   lmLocation* pPos, lmFontInfo oFontData, bool fHasWidth);
    ~lmWordsDirection() {}

    // properties related to the clasification of this lmStaffObj
    float GetTimePosIncrement() { return 0; }

    //implementation of virtual methods defined in abstract base class lmStaffObj
    void LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC);
    wxBitmap* GetBitmap(double rScale);
    void SetFont(lmPaper* pPaper);

    //    debugging
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML();

    //specific methods of this object


private:
    bool            m_fHasWidth;
    lmEAlignment    m_nAlign;

};

#endif    // __DIRECTION_H__

