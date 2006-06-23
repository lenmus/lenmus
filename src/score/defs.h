// RCS-ID: $Id: defs.h,v 1.3 2006/02/23 19:22:56 cecilios Exp $
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
/*! @file defs.h
    @brief Common definitions for all score kernel
    @ingroup score_kernel
*/

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __DEFS_H__        //to avoid nested includes
#define __DEFS_H__

//// Types used for portability

//units
#define lmLUnits    int        // lmPaper logical units. 32 bits int
#define lmTenths    int        // Staff relative units.  32 bits int
#define lmPixels    int        // Device units (usually pixels).  32 bits int

//pitch
#define lmPitch        int        // Midi pitch or diatonic pitch

//// Other definitions

// Pitch conversion
#define lmC0PITCH         1        // Pitch assigned to C0
#define lmC1PITCH         8        // Pitch assigned to C1
#define lmC2PITCH        15        // Pitch assigned to C2
#define lmC3PITCH        22        // Pitch assigned to C3
#define lmC4PITCH        29        // Pitch assigned to C4
#define lmC5PITCH        36        // Pitch assigned to C5
#define lmC6PITCH        43        // Pitch assigned to C6
#define lmC7PITCH        50        // Pitch assigned to C7
#define lmC8PITCH        57        // Pitch assigned to C8


// DC user mode
#define lmDC_MODE    wxMM_LOMETRIC    // mode for DC (logical unit: one tenth of mm)
#define lmSCALE        1.0            // scaling factor so that logical units become microns

enum lmEUnits {
    lmMICRONS = 0,
    lmMILLIMETERS,
    lmCENTIMETERS,
    lmINCHES,
    lmTENTHS
};

// defined in TheApp.cpp
extern int lmToLogicalUnits(int nValue, lmEUnits nUnits);
extern int lmToLogicalUnits(double rValue, lmEUnits nUnits);


#endif    // __DEFS_H__ 
