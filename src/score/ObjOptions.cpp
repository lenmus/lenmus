//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
#pragma implementation "ObjOptions.h"
#endif

#include "StaffObj.h"
#include "ObjOptions.h"


//-------------------------------------------------------------------------------------
// lmObjOptions implementation
//-------------------------------------------------------------------------------------
lmObjOptions::lmObjOptions()
{
    
}

lmObjOptions::~lmObjOptions()
{
    //delete values
    for(long i = m_aValue.GetCount(); i > 0; i--) {
        delete m_aValue.Item(i-1);
        m_aValue.RemoveAt(i-1);
    }
    
}

//Set up a value
void lmObjOptions::SetOption(wxString sName, long nLongValue)
{
    lmObjOptionsTable::iterator it = m_aItems.find(sName);
    if (it == m_aItems.end()) {
        //Not found. Add it
        lmObjOptionsItem* pItem = new lmObjOptionsItem(nLongValue);
        int iV = m_aType.GetCount();        //index to value
        m_aType.Add(lmE_Long);
        m_aValue.Add(pItem);                //store the value
        m_aItems[sName] = iV;               //store index
    }
    else {
        //Found. Replace its value
        int iV = it->second;                //index to value
        wxASSERT(m_aType.Item(iV) == lmE_Long);
        lmObjOptionsItem& oItem = *(m_aValue.Item(iV));    //get the item
        oItem = nLongValue;                 //replace the value
    }
}

void lmObjOptions::SetOption(wxString sName, wxString sStringValue)
{
    lmObjOptionsTable::iterator it = m_aItems.find(sName);
    if (it == m_aItems.end()) {
        //Not found. Add it
        lmObjOptionsItem* pItem = new lmObjOptionsItem(sStringValue);
        int iV = m_aType.GetCount();        //index to value
        m_aType.Add(lmE_String);
        m_aValue.Add(pItem);                //store the value
        m_aItems[sName] = iV;               //store index
    }
    else {
        //Found. Replace its value
        int iV = it->second;                //index to value
        wxASSERT(m_aType.Item(iV) == lmE_String);
        lmObjOptionsItem& oItem = *(m_aValue.Item(iV));    //get the item
        oItem = sStringValue;                 //replace the value
    }
}

void lmObjOptions::SetOption(wxString sName, double nDoubleValue)
{
    lmObjOptionsTable::iterator it = m_aItems.find(sName);
    if (it == m_aItems.end()) {
        //Not found. Add it
        lmObjOptionsItem* pItem = new lmObjOptionsItem(nDoubleValue);
        int iV = m_aType.GetCount();        //index to value
        m_aType.Add(lmE_Double);
        m_aValue.Add(pItem);                //store the value
        m_aItems[sName] = iV;               //store index
    }
    else {
        //Found. Replace its value
        int iV = it->second;                //index to value
        wxASSERT(m_aType.Item(iV) == lmE_Double);
        lmObjOptionsItem& oItem = *(m_aValue.Item(iV));    //get the item
        oItem = nDoubleValue;                 //replace the value
    }
}

void lmObjOptions::SetOption(wxString sName, bool fBoolValue)
{
    lmObjOptionsTable::iterator it = m_aItems.find(sName);
    if (it == m_aItems.end()) {
        //Not found. Add it
        lmObjOptionsItem* pItem = new lmObjOptionsItem(fBoolValue);
        int iV = m_aType.GetCount();        //index to value
        m_aType.Add(lmE_Bool);
        m_aValue.Add(pItem);                //store the value
        m_aItems[sName] = iV;               //store index
    }
    else {
        //Found. Replace its value
        int iV = it->second;                //index to value
        wxASSERT(m_aType.Item(iV) == lmE_Bool);
        lmObjOptionsItem& oItem = *(m_aValue.Item(iV));    //get the item
        oItem = fBoolValue;                 //replace the value
    }
}


//Look for the value of an option. A method for each supported data type.
//Recursive search through the ObjOptions chain
long lmObjOptions::GetOptionLong(wxString sOptName)
{
    lmObjOptionsTable::iterator it = m_aItems.find(sOptName);
    if (it == m_aItems.end()) {
        //Not found. Look up in parent's options
        lmObjOptions* pObjOpt = m_pOwner->GetObjOptions();
        if (!pObjOpt) {
            wxMessageBox(_T("Fatal program error: ObjOption '%' does not exist."), sOptName);
            wxASSERT(false);
        }
        return pObjOpt->GetOptionLong(sOptName);
    }
    else {
        //Found
        int iV = it->second;        //get index to value
        wxASSERT(m_aType.Item(iV) == lmE_Long);
        return m_aValue.Item(iV)->GetLong();
    }
    
}

double lmObjOptions::GetOptionDouble(wxString sOptName)
{
    lmObjOptionsTable::iterator it = m_aItems.find(sOptName);
    if (it == m_aItems.end()) {
        //Not found. Look up in parent's options
        lmObjOptions* pObjOpt = m_pOwner->GetObjOptions();
        if (!pObjOpt) {
            wxMessageBox(_T("Fatal program error: ObjOption '%' does not exist."), sOptName);
            wxASSERT(false);
        }
        return pObjOpt->GetOptionDouble(sOptName);
    }
    else {
        //Found
        int iV = it->second;        //get index to value
        wxASSERT(m_aType.Item(iV) == lmE_Double);
        return m_aValue.Item(iV)->GetDouble();
    }
    
}

bool lmObjOptions::GetOptionBool(wxString sOptName)
{
    lmObjOptionsTable::iterator it = m_aItems.find(sOptName);
    if (it == m_aItems.end()) {
        //Not found. Look up in parent's options
        lmObjOptions* pObjOpt = m_pOwner->GetObjOptions();
        if (!pObjOpt) {
            wxMessageBox(_T("Fatal program error: ObjOption '%' does not exist."), sOptName);
            wxASSERT(false);
        }
        return pObjOpt->GetOptionBool(sOptName);
    }
    else {
        //Found
        int iV = it->second;        //get index to value
        wxASSERT(m_aType.Item(iV) == lmE_Bool);
        return m_aValue.Item(iV)->GetBool();
    }
    
}

wxString lmObjOptions::GetOptionString(wxString sOptName)
{
    lmObjOptionsTable::iterator it = m_aItems.find(sOptName);
    if (it == m_aItems.end()) {
        //Not found. Look up in parent's options
        lmObjOptions* pObjOpt = m_pOwner->GetObjOptions();
        if (!pObjOpt) {
            wxMessageBox(_T("Fatal program error: ObjOption '%' does not exist."), sOptName);
            wxASSERT(false);
        }
        return pObjOpt->GetOptionString(sOptName);
    }
    else {
        //Found
        int iV = it->second;        //get index to value
        wxASSERT(m_aType.Item(iV) == lmE_String);
        return m_aValue.Item(iV)->GetString();
    }
    
}













