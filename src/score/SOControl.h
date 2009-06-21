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

#ifndef __LM_SOCONTROL_H__        //to avoid nested includes
#define __LM_SOCONTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SOControl.cpp"
#endif

enum ESOCtrolType
{
    lmNEW_SYSTEM = 1,       // force a new system
};

class lmSOControl: public lmStaffObj
{
public:
    //constructors and destructor
    lmSOControl(ESOCtrolType nType, lmVStaff* pVStaff);                     //lmNEW_SYSTEM
    ~lmSOControl() {}

	wxString GetName() const { return _T("control object"); }


    //implementation of virtual methods defined in abstract base class lmStaffObj
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC);

    //SOControl specfic methods
    ESOCtrolType GetCtrolType() { return m_nCtrolType; }

    //    debugging
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);


private:
    ESOCtrolType    m_nCtrolType;

};

#endif    // __LM_SOCONTROL_H__
