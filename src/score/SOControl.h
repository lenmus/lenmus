// RCS-ID: $Id: SOControl.h,v 1.3 2006/02/23 19:24:42 cecilios Exp $
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
/*! @file SOControl.h
    @brief Header file for class lmSOControl
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __SOCONTROL_H__        //to avoid nested includes
#define __SOCONTROL_H__

class lmSOControl:  public lmSimpleObj
{
public:
    //constructor and destructor
    lmSOControl(lmVStaff* pVStaff, float rTimeShift);
    ~lmSOControl() {}

    //implementation of virtual methods defined in abstract base class lmStaffObj
    void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC) {};
    wxBitmap* GetBitmap(double rScale) { return (wxBitmap*)NULL; };


    //    debugging
    wxString Dump();
    wxString SourceLDP();
    wxString SourceXML();


private:
    float            m_rTimeShift;        //the time shift (positive or negative) applied

};

#endif    // __SOCONTROL_H__
