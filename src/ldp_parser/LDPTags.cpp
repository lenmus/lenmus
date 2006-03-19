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
/*! @file LDPTags.cpp
    @brief Implementation file for class lmLDPTags
    @ingroup ldp_parser
*/
/*! @class lmLDPTags
    @ingroup ldp_parser
    @brief lmLDPTags This class is a singleton to contain the LDP tags table.

	Tag translations are going to be stored in a file named 'LDP_Tags.txt' in
	current language directory.
	On object initialization the Spanish tags set will be loaded.

	When parsing a score, if the score has a 'lang=xx' tag, then the tags set
	for that language will be loaded.

	The 'Score' and 'Language' tags are the only non-translatable, according to
	LDP standard v4.0

*/


#ifdef __GNUG__
// #pragma implementation
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "LDPTags.h"

// ----------------------------------------------------------------------------
// implementation
// ----------------------------------------------------------------------------

//initialize the only instance
lmLdpTagsTable* lmLdpTagsTable::m_pInstance = (lmLdpTagsTable*)NULL;

//destructor
lmLdpTagsTable::~lmLdpTagsTable()
{
}

lmLdpTagsTable* lmLdpTagsTable::GetInstance()
{
	if (!m_pInstance) {
		m_pInstance = new lmLdpTagsTable();
		m_pInstance->Initialize();
	}
	return m_pInstance;
}

void lmLdpTagsTable::DeleteInstance()
{
	if (m_pInstance) {
		delete m_pInstance;
	    m_pInstance = (lmLdpTagsTable*)NULL;
	}
}

//initialization
void lmLdpTagsTable::Initialize()
{

    // initialize table with default Spanish values
    m_sTags2[LDP_TAG_SCORE] = _T("Score");
    m_sTags2[LDP_TAG_LANGUAGE] = _T("Language");
	m_sTags2[LDP_TAG_PART] = _T("Parte");
	m_sTags2[LDP_TAG_NUMINSTR] = _T("NumInstr");
	m_sTags2[LDP_TAG_INSTRUMENT] = _T("Instrumento");
	m_sTags2[LDP_TAG_MEASURE] = _T("c");
	m_sTags2[LDP_TAG_BARLINE] = _T("Barra");
	m_sTags2[LDP_TAG_BARLINE_SIMPLE] = _T("Simple");
	m_sTags2[LDP_TAG_BARLINE_DOUBLE] = _T("Doble");
	m_sTags2[LDP_TAG_BARLINE_START] = _T("Inicio");
	m_sTags2[LDP_TAG_BARLINE_END] = _T("Fin");
	m_sTags2[LDP_TAG_BARLINE_START_REPETITION] = _T("InicioRepeticion");
	m_sTags2[LDP_TAG_BARLINE_END_REPETITION] = _T("FinRepeticion");
	m_sTags2[LDP_TAG_BARLINE_DOUBLE_REPETITION] = _T("DobleRepeticion");

	m_sTags2[LDP_TAG_CLEF] = _T("Clave");
	m_sTags2[LDP_TAG_KEY] = _T("Tonalidad");
	m_sTags2[LDP_TAG_TIME] = _T("Metrica");
	m_sTags2[LDP_TAG_NOTE] = _T("n");
	m_sTags2[LDP_TAG_REST] = _T("r");

	// note/rests types
    m_sTags2[LDP_TAG_DOUBLE_WHOLE] = _T("l");
    m_sTags2[LDP_TAG_WHOLE] = _T("r");
    m_sTags2[LDP_TAG_HALF] = _T("b");
    m_sTags2[LDP_TAG_QUARTER] = _T("n");
    m_sTags2[LDP_TAG_EIGHTH] = _T("c");
    m_sTags2[LDP_TAG_16TH] = _T("s");
    m_sTags2[LDP_TAG_32ND] = _T("f");
    m_sTags2[LDP_TAG_64TH] = _T("m");
    m_sTags2[LDP_TAG_128TH] = _T("g");
    m_sTags2[LDP_TAG_256TH] = _T("p");


    m_Tags[_T("Instrument")] = _T("Instrumento");


}

//load tags set for given language
void lmLdpTagsTable::LoadTags(wxString sLanguage)
{
    // initialize table with default Spanish values
    m_sTags2[LDP_TAG_SCORE] = _T("Score");
    m_sTags2[LDP_TAG_LANGUAGE] = _T("Language");
}
