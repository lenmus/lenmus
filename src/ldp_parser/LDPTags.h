// RCS-ID: $Id$
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
/*! @file LDPTags.h
    @brief Header file with LDP language tags
    @ingroup ldp_parser
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __LDPTAGS_H__        //to avoid nested includes
#define __LDPTAGS_H__

#include "wx/hashmap.h"

enum lmELdpTags {
	LDP_TAG_SCORE = 0,
	LDP_TAG_LANGUAGE,
	LDP_TAG_PART,
	LDP_TAG_NUMINSTR,
	LDP_TAG_INSTRUMENT,
	LDP_TAG_MEASURE,
	LDP_TAG_BARLINE,
    // barline types
	LDP_TAG_BARLINE_SIMPLE,
	LDP_TAG_BARLINE_DOUBLE,
	LDP_TAG_BARLINE_START,
	LDP_TAG_BARLINE_END,
	LDP_TAG_BARLINE_START_REPETITION,
	LDP_TAG_BARLINE_END_REPETITION,
	LDP_TAG_BARLINE_DOUBLE_REPETITION,

	LDP_TAG_CLEF,
	LDP_TAG_KEY,
	LDP_TAG_TIME,
	LDP_TAG_NOTE,
	LDP_TAG_REST,
	// note/rests types
    LDP_TAG_DOUBLE_WHOLE,
    LDP_TAG_WHOLE,
    LDP_TAG_HALF,
    LDP_TAG_QUARTER,
    LDP_TAG_EIGHTH,
    LDP_TAG_16TH,
    LDP_TAG_32ND,
    LDP_TAG_64TH,
    LDP_TAG_128TH,
    LDP_TAG_256TH,

    // the last item, to know the number of tags in tags set
    LDP_TAGS_TABLE_SIZE
};


// declare a hash map with string keys and string values
WX_DECLARE_STRING_HASH_MAP( wxString, lmTagsTable );

class lmLdpTagsTable
{
public:
	~lmLdpTagsTable();

	static lmLdpTagsTable* GetInstance();
    static void DeleteInstance();

	void LoadTags(wxString sLanguage);

	const wxString& TagName(lmELdpTags nTag) { return m_sTags2[nTag]; }
	const wxString& TagName(wxString sTag) { return m_Tags[sTag]; }

private:
	lmLdpTagsTable() {}
	void Initialize();

	static lmLdpTagsTable*	m_pInstance;	//the only instance of this class

	wxString m_sTags2[LDP_TAGS_TABLE_SIZE];
	lmTagsTable m_Tags;


};

#endif    // __LDPTAGS_H__

