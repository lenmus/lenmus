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

#ifndef _OBJOPTIONS_H__        //to avoid nested includes
#define _OBJOPTIONS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ObjOptions.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/variant.h"
#include "wx/dynarray.h"
#include "wx/hashmap.h"

#define lmObjOptionsItem    wxVariant       //to simplify future replacement, if necessary

// declare a hash map for options names, with string keys and int values
WX_DECLARE_STRING_HASH_MAP(int, lmObjOptionsTable);

//define an array of ints
WX_DEFINE_ARRAY_INT(int, ArrayOfInts);

// declare an array of ptrs to wxVariants
WX_DEFINE_ARRAY(lmObjOptionsItem*, lmArrayOfObjOptionsValues);

enum lmObjOptionsDataType
{
    lmE_String = 0,
    lmE_Bool,
    lmE_Long,
    lmE_Double,
};

class lmScoreObj;

class lmObjOptions
{
public:
    lmObjOptions();
    ~lmObjOptions();

    //Set up a value
    void SetOption(wxString sName, long nLongValue);
    void SetOption(wxString sName, wxString sStringValue);
    void SetOption(wxString sName, double nDoubleValue);
    void SetOption(wxString sName, bool fBoolValue);

    //Look for the value of an option. A method for each supported data type.
    //Recursive search throug the ObjOptions chain
    long GetOptionLong(wxString sOptName);
    double GetOptionDouble(wxString sOptName);
    bool GetOptionBool(wxString sOptName);
    wxString GetOptionString(wxString sOptName); 

private:
    
    lmScoreObj*                   m_pOwner;      //the owner of this context
    
    //options are kept as pairs property/value
    lmObjOptionsTable           m_aItems;   //list of properties with indexes to type and value
    ArrayOfInts                 m_aType;    //data type for each option. Index in m_aItems
    lmArrayOfObjOptionsValues   m_aValue;   //value for each option.  Index in m_aItems

};



#endif    // _OBJOPTIONS_H__

