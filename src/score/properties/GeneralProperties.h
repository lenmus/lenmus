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

#ifndef __LM_GENERALPROPERTIES_H__        //to avoid nested includes
#define __LM_GENERALPROPERTIES_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "GeneralProperties.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"

#else
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>

#endif

#include "DlgProperties.h"
#include "../../score/defs.h"


//--------------------------------------------------------------------------------------
/// Class lmGeneralProperties
//--------------------------------------------------------------------------------------

class lmScoreObj;
class lmController;
class lmScore;

class lmGeneralProperties : public lmPropertiesPage 
{
public:
	lmGeneralProperties(lmDlgProperties* parent, lmScoreObj* pSCO, lmScore* pScore);
	~lmGeneralProperties();

    //implementation of pure virtual methods in base class
    void OnAcceptChanges(lmController* pController, bool fCurrentPage);

    // event handlers


protected:
    void CreateControls();

    //controls
	wxStaticText*		m_pTxtTodo;

    //other variables
    lmScoreObj*			m_pSCO;
    lmScore*            m_pScore;


    DECLARE_EVENT_TABLE()
};


#endif //__LM_GENERALPROPERTIES_H__
