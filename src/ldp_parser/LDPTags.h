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
#pragma interface "LDPTags.cpp"
#endif

#ifndef __LDPTAGS_H__        //to avoid nested includes
#define __LDPTAGS_H__

#include "wx/hashmap.h"


// declare a hash map for tags, with string keys and string values
WX_DECLARE_STRING_HASH_MAP( wxString, lmTagsTable );

// declare a hash map for contexts, with string keys and int values
WX_DECLARE_STRING_HASH_MAP( int, lmContextsTable );

#define lmMAX_TAG_CONTEXTS     7

class lmLdpTagsTable
{
public:
	~lmLdpTagsTable();

	static lmLdpTagsTable* GetInstance();
    static void DeleteInstance();

	void LoadTags(wxString sLanguage, wxString sCharset);

    const wxString& TagName(wxString sTag, wxString sContext=_T(""));
    wxString GetInternalTag(wxString sTag, wxString sContext=_T(""));

private:
	lmLdpTagsTable() {}

	static lmLdpTagsTable*	m_pInstance;	//the only instance of this class

	lmTagsTable         m_Tags[lmMAX_TAG_CONTEXTS+1];
    lmContextsTable     m_Contexts;

};

#endif    // __LDPTAGS_H__

