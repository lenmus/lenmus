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
/*! @file BoxPage.h
    @brief Header file for class lmBoxPage
    @ingroup graphic_management
*/
#ifndef __BOXPAGE_H__        //to avoid nested includes
#define __BOXPAGE_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/score.h"

/*
    Class lmBoxPage contains one or more instances of class lmBoxSystem. An instance
    of class lmBoxPage knows about its height and width and also about the size of
    its margins. Some graphical elements of the score, like for instance the visible
    title and composer of a piece are also controlled by class lmBoxPage. Class lmBoxPage
    contains one or more instances of class lmBoxSystem.
*/
class lmBoxPage
{
public:
    lmBoxPage();
    ~lmBoxPage();
    void SetFirstSystem(int nSystem) { m_nFirstSystem = nSystem; }
    void SetLastSystem(int nSystem) { m_nLastSystem = nSystem; }
    int GetFirstSystem() { return m_nFirstSystem; }
    int GetLastSystem() { return m_nLastSystem; }

private:
    int     m_nFirstSystem;
    int     m_nLastSystem;

};



#endif  // __BOXPAGE_H__

