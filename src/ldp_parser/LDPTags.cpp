//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

/*! @class lmLDPTags
    @ingroup ldp_parser
    @brief lmLDPTags This class is a singleton to contain the LDP tags table.

	Tag translations are going to be stored in a file named 'LDP_Tags.txt' in
	current language directory.
	On object initialization the Spanish tags set will be loaded.

	When parsing a score, if the score has a 'lang=xx' tag, then the tags set
	for that language will be loaded.

	The 'score' and 'language' tags are the only non-translatable, according to
	LDP standard v4.0

*/


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "LDPTags.h"
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

const wxString& lmLdpTagsTable::TagName(wxString sTag, wxString sContext)
{
    //returns the language dependent tag for the given internal tag sTag
    if (sContext == _T(""))
        return m_Tags[0][sTag];
    else {
        int iT = m_Contexts[sContext];
        wxASSERT( iT <= lmMAX_TAG_CONTEXTS );
        return m_Tags[iT][sTag];
    }
}

wxString lmLdpTagsTable::GetInternalTag(wxString sTag, wxString sContext)
{
    //returns the internal tag for the language dependent tag sTag.
    //If tag sTag does not exits returns _T("")
    //This method is just the opposite of method TagName()

    int iT = 0;
    if (sContext != _T("")) {
        iT = m_Contexts[sContext];
        wxASSERT( iT <= lmMAX_TAG_CONTEXTS );
    }

    // iterate over all the key signature tags to search for the given one
    lmTagsTable::iterator iter;
    for(iter = m_Tags[iT].begin(); iter != m_Tags[iT].end(); ++iter)
    {
        if (sTag == iter->second) return iter->first;
    }
    return wxEmptyString;

}

//load tags set for given language
void lmLdpTagsTable::LoadTags(wxString sLanguage, wxString sCharset)
{
    //TODO For now charset is ignored

    //clear tables
    m_Contexts.clear();
    int i;
    for (i=0; i <= lmMAX_TAG_CONTEXTS; i++) {
        m_Tags[i].clear();
    }


    //TODO   For now I will load tags from program. This must be changed to load them
    //!         from file ''LDP_Tags.txt' in given language directory.

    //TODO   For now I will load Spanish tags as default. This must not be changed
    //!         while program generated tags are in Spanish.
    if (sLanguage == _T("en")) {
        m_Tags[0][_T("abbrev")] = _T("abbrev");
        m_Tags[0][_T("above")] = _T("above");
        m_Tags[0][_T("barline")] = _T("barline");
        m_Tags[0][_T("below")] = _T("below");
        m_Tags[0][_T("bold")] = _T("bold");
        m_Tags[0][_T("bold_italic")] = _T("bold-italic");
        m_Tags[0][_T("center")] = _T("center");
        m_Tags[0][_T("chord")] = _T("chord");
        m_Tags[0][_T("clef")] = _T("clef");
        m_Tags[0][_T("down")] = _T("down");
        m_Tags[0][_T("dx")] = _T("dx");
        m_Tags[0][_T("dy")] = _T("dy");
        m_Tags[0][_T("end")] = _T("end");
        m_Tags[0][_T("fermata")] = _T("fermata");
        m_Tags[0][_T("font")] = _T("font");
        m_Tags[0][_T("goBack")] = _T("goBack");
        m_Tags[0][_T("goFwd")] = _T("goFwd");
        m_Tags[0][_T("graphic")] = _T("graphic");
        m_Tags[0][_T("hasWidth")] = _T("hasWidth");
        m_Tags[0][_T("infoMIDI")] = _T("infoMIDI");
        m_Tags[0][_T("instrName")] = _T("instrName");
        m_Tags[0][_T("instrument")] = _T("instrument");
        m_Tags[0][_T("italic")] = _T("italic");
        m_Tags[0][_T("key")] = _T("key");
        m_Tags[0][_T("left")] = _T("left");
        m_Tags[0][_T("metronome")] = _T("metronome");
        m_Tags[0][_T("musicData")] = _T("musicData");
        m_Tags[0][_T("name")] = _T("name");
        m_Tags[0][_T("newSystem")] = _T("newSystem");
        m_Tags[0][_T("no")] = _T("no");
        m_Tags[0][_T("normal")] = _T("normal");
        m_Tags[0][_T("noVisible")] = _T("noVisible");
        m_Tags[0][_T("opt")] = _T("opt");
        m_Tags[0][_T("parentheses")] = _T("parentheses");
        m_Tags[0][_T("right")] = _T("right");
        m_Tags[0][_T("spacer")] = _T("spacer");
        m_Tags[0][_T("split")] = _T("split");
        m_Tags[0][_T("start")] = _T("start");
        m_Tags[0][_T("staves")] = _T("staves");
        m_Tags[0][_T("stem")] = _T("stem");
        m_Tags[0][_T("text")] = _T("text");
        m_Tags[0][_T("time")] = _T("time");
        m_Tags[0][_T("title")] = _T("title");
        m_Tags[0][_T("up")] = _T("up");
        m_Tags[0][_T("x")] = _T("x");
        m_Tags[0][_T("y")] = _T("y");
        m_Tags[0][_T("yes")] = _T("yes");

        //special context: note durations
        //letter 'x' can not be used as it is double sharp accidental
        m_Contexts[_T("NoteType")] = 1;
        m_Tags[1][_T("l")] = _T("l");
        m_Tags[1][_T("d")] = _T("d");
        m_Tags[1][_T("r")] = _T("w");
        m_Tags[1][_T("b")] = _T("h");
        m_Tags[1][_T("n")] = _T("q");
        m_Tags[1][_T("c")] = _T("e");
        m_Tags[1][_T("s")] = _T("s");
        m_Tags[1][_T("f")] = _T("t");
        m_Tags[1][_T("m")] = _T("i");
        m_Tags[1][_T("g")] = _T("o");
        m_Tags[1][_T("p")] = _T("f");

        //special context: barlines
        m_Contexts[_T("Barlines")] = 2;
        m_Tags[2][_T("simple")] = _T("simple");
        m_Tags[2][_T("double")] = _T("double");
        m_Tags[2][_T("end")] = _T("end");
        m_Tags[2][_T("start")] = _T("start");
        m_Tags[2][_T("startRepetition")] = _T("startRepetition");
        m_Tags[2][_T("endRepetition")] = _T("endRepetition");
        m_Tags[2][_T("doubleRepetition")] = _T("doubleRepetition");

        //special context: clefs
        m_Contexts[_T("Clefs")] = 3;
        m_Tags[3][_T("G")] = _T("G");
        m_Tags[3][_T("F")] = _T("F");
        m_Tags[3][_T("F3")] = _T("F3");
        m_Tags[3][_T("C1")] = _T("C1");
        m_Tags[3][_T("C2")] = _T("C2");
        m_Tags[3][_T("C3")] = _T("C3");
        m_Tags[3][_T("C4")] = _T("C4");
        m_Tags[3][_T("C5")] = _T("C5");
        m_Tags[3][_T("F5")] = _T("F5");
        m_Tags[3][_T("G1")] = _T("G1");
        m_Tags[3][_T("percussion")] = _T("percussion");
        m_Tags[3][_T("bass")] = _T("bass");
        m_Tags[3][_T("treble")] = _T("treble");
        m_Tags[3][_T("baritone")] = _T("baritone");
        m_Tags[3][_T("soprano")] = _T("soprano");
        m_Tags[3][_T("mezzosoprano")] = _T("mezzosoprano");
        m_Tags[3][_T("alto")] = _T("alto");
        m_Tags[3][_T("tenor")] = _T("tenor");
        m_Tags[3][_T("baritoneC")] = _T("baritoneC");
        m_Tags[3][_T("subbass")] = _T("subbass");
        m_Tags[3][_T("french")] = _T("french");

        //special context: key signatures
        m_Contexts[_T("Keys")] = 4;
        m_Tags[4][_T("Do")] = _T("C");
        m_Tags[4][_T("Sol")] = _T("G");
        m_Tags[4][_T("Re")] = _T("D");
        m_Tags[4][_T("La")] = _T("A");
        m_Tags[4][_T("Mi")] = _T("E");
        m_Tags[4][_T("Si")] = _T("B");
        m_Tags[4][_T("Fa+")] = _T("F+");
        m_Tags[4][_T("Do+")] = _T("C+");
        m_Tags[4][_T("Do-")] = _T("C-");
        m_Tags[4][_T("Sol-")] = _T("G-");
        m_Tags[4][_T("Re-")] = _T("D-");
        m_Tags[4][_T("La-")] = _T("A-");
        m_Tags[4][_T("Mi-")] = _T("E-");
        m_Tags[4][_T("Si-")] = _T("B-");
        m_Tags[4][_T("Fa")] = _T("F");
        m_Tags[4][_T("Lam")] = _T("a");
        m_Tags[4][_T("Mim")] = _T("e");
        m_Tags[4][_T("Sim")] = _T("b");
        m_Tags[4][_T("Fa+m")] = _T("f+");
        m_Tags[4][_T("Do+m")] = _T("c+");
        m_Tags[4][_T("Sol+m")] = _T("g+");
        m_Tags[4][_T("Re+m")] = _T("d+");
        m_Tags[4][_T("La+m")] = _T("a+");
        m_Tags[4][_T("La-m")] = _T("a-");
        m_Tags[4][_T("Mi-m")] = _T("e-");
        m_Tags[4][_T("Si-m")] = _T("b-");
        m_Tags[4][_T("Fam")] = _T("f");
        m_Tags[4][_T("Dom")] = _T("c");
        m_Tags[4][_T("Solm")] = _T("g");
        m_Tags[4][_T("Rem")] = _T("d");

        //special context: single char very frequent elements
        m_Contexts[_T("SingleChar")] = 5;
        m_Tags[5][_T("g")] = _T("g");   //beamed group 
        m_Tags[5][_T("l")] = _T("l");   //tie 
        m_Tags[5][_T("n")] = _T("n");   //note
        m_Tags[5][_T("p")] = _T("p");   //staff number  
        m_Tags[5][_T("r")] = _T("r");   //rest 
        m_Tags[5][_T("t")] = _T("t");   //tuplet 
        m_Tags[5][_T("v")] = _T("v");   //voice

        //context: tuplet options
        m_Contexts[_T("Tuplets")] = 6;
        m_Tags[6][_T("noBracket")] = _T("noBracket");
        m_Tags[6][_T("squaredBracket")] = _T("squaredBracket");
        m_Tags[6][_T("curvedBracket")] = _T("curvedBracket");
        m_Tags[6][_T("numNone")] = _T("numNone");
        m_Tags[6][_T("numActual")] = _T("numActual");
        m_Tags[6][_T("numBoth")] = _T("numBoth");

        //special context: graphic objects
        m_Contexts[_T("GraphObjs")] = 7;
        m_Tags[7][_T("line")] = _T("line");

    }
    else {
        // initialize table with default Spanish values

        //no context
        m_Tags[0][_T("abbrev")] = _T("abrev");
        m_Tags[0][_T("above")] = _T("encima");
        m_Tags[0][_T("barline")] = _T("barline");
        m_Tags[0][_T("below")] = _T("debajo");
        m_Tags[0][_T("bold")] = _T("negrita");
        m_Tags[0][_T("bold_italic")] = _T("negrita-cursiva");
        m_Tags[0][_T("center")] = _T("centrado");
        m_Tags[0][_T("chord")] = _T("acorde");
        m_Tags[0][_T("clef")] = _T("clave");            
        m_Tags[0][_T("down")] = _T("abajo");
        m_Tags[0][_T("dx")] = _T("dx");
        m_Tags[0][_T("dy")] = _T("dy");
        m_Tags[0][_T("end")] = _T("final");
        m_Tags[0][_T("fermata")] = _T("calderon");
        m_Tags[0][_T("font")] = _T("font");
        m_Tags[0][_T("goBack")] = _T("retroceder");
        m_Tags[0][_T("goFwd")] = _T("avanzar");
        m_Tags[0][_T("graphic")] = _T("grafico");
        m_Tags[0][_T("hasWidth")] = _T("tieneAnchura");
        m_Tags[0][_T("infoMIDI")] = _T("infoMIDI");
        m_Tags[0][_T("instrName")] = _T("nombreInstrumento");
        m_Tags[0][_T("instrument")] = _T("instrumento");
        m_Tags[0][_T("italic")] = _T("cursiva");
        m_Tags[0][_T("key")] = _T("tonalidad");
        m_Tags[0][_T("left")] = _T("izquierda");
        m_Tags[0][_T("metronome")] = _T("metronomo");
        m_Tags[0][_T("musicData")] = _T("datosMusica");
        m_Tags[0][_T("name")] = _T("nombre");
        m_Tags[0][_T("newSystem")] = _T("nuevoSistema");
        m_Tags[0][_T("no")] = _T("no");
        m_Tags[0][_T("normal")] = _T("normal");
        m_Tags[0][_T("noVisible")] = _T("noVisible");
        m_Tags[0][_T("opt")] = _T("opcion");
        m_Tags[0][_T("parentheses")] = _T("parentesis");
        m_Tags[0][_T("right")] = _T("derecha");
        m_Tags[0][_T("spacer")] = _T("espacio");
        m_Tags[0][_T("split")] = _T("partes");
        m_Tags[0][_T("start")] = _T("inicio");
        m_Tags[0][_T("staves")] = _T("numPentagramas");
        m_Tags[0][_T("stem")] = _T("plica");
        m_Tags[0][_T("text")] = _T("texto");
        m_Tags[0][_T("time")] = _T("metrica");
        m_Tags[0][_T("title")] = _T("titulo");
        m_Tags[0][_T("up")] = _T("arriba");
        m_Tags[0][_T("x")] = _T("x");
        m_Tags[0][_T("y")] = _T("y");
        m_Tags[0][_T("yes")] = _T("yes");

        //special context: note durations
        //letter 'x' can not be used as it is double sharp accidental
        m_Contexts[_T("NoteType")] = 1;
        m_Tags[1][_T("l")] = _T("l");
        m_Tags[1][_T("d")] = _T("d");
        m_Tags[1][_T("r")] = _T("r");
        m_Tags[1][_T("b")] = _T("b");
        m_Tags[1][_T("n")] = _T("n");
        m_Tags[1][_T("c")] = _T("c");
        m_Tags[1][_T("s")] = _T("s");
        m_Tags[1][_T("f")] = _T("f");
        m_Tags[1][_T("m")] = _T("m");
        m_Tags[1][_T("g")] = _T("g");
        m_Tags[1][_T("p")] = _T("p");

        //special context: barlines
        m_Contexts[_T("Barlines")] = 2;
        m_Tags[2][_T("simple")] = _T("Simple");
        m_Tags[2][_T("double")] = _T("Doble");
        m_Tags[2][_T("end")] = _T("Final");
        m_Tags[2][_T("start")] = _T("Inicial");
        m_Tags[2][_T("startRepetition")] = _T("InicioRepeticion");
        m_Tags[2][_T("endRepetition")] = _T("FinRepeticion");
        m_Tags[2][_T("doubleRepetition")] = _T("DobleRepeticion");

        //special context: clefs
        m_Contexts[_T("Clefs")] = 3;
        m_Tags[3][_T("G")] = _T("Sol");
        m_Tags[3][_T("F")] = _T("Fa4");
        m_Tags[3][_T("F3")] = _T("Fa3");
        m_Tags[3][_T("C1")] = _T("Do1");
        m_Tags[3][_T("C2")] = _T("Do2");
        m_Tags[3][_T("C3")] = _T("Do3");
        m_Tags[3][_T("C4")] = _T("Do4");
        m_Tags[3][_T("C5")] = _T("Do5");
        m_Tags[3][_T("F5")] = _T("Fa5");
        m_Tags[3][_T("G1")] = _T("Sol1");
        m_Tags[3][_T("percussion")] = _T("percusion");
        m_Tags[3][_T("bass")] = _T("Fa4");
        m_Tags[3][_T("treble")] = _T("Sol");
        m_Tags[3][_T("baritone")] = _T("Fa3");
        m_Tags[3][_T("soprano")] = _T("Do1");
        m_Tags[3][_T("mezzosoprano")] = _T("Do2");
        m_Tags[3][_T("alto")] = _T("Do3");
        m_Tags[3][_T("tenor")] = _T("Do4");
        m_Tags[3][_T("baritoneC")] = _T("Do5");
        m_Tags[3][_T("subbass")] = _T("Fa5");
        m_Tags[3][_T("french")] = _T("Sol1");

        //special context: key signatures
        m_Contexts[_T("Keys")] = 4;
        m_Tags[4][_T("Do")] = _T("Do");
        m_Tags[4][_T("Sol")] = _T("Sol");
        m_Tags[4][_T("Re")] = _T("Re");
        m_Tags[4][_T("La")] = _T("La");
        m_Tags[4][_T("Mi")] = _T("Mi");
        m_Tags[4][_T("Si")] = _T("Si");
        m_Tags[4][_T("Fa+")] = _T("Fa+");
        m_Tags[4][_T("Do+")] = _T("Do+");
        m_Tags[4][_T("Do-")] = _T("Do-");
        m_Tags[4][_T("Sol-")] = _T("Sol-");
        m_Tags[4][_T("Re-")] = _T("Re-");
        m_Tags[4][_T("La-")] = _T("La-");
        m_Tags[4][_T("Mi-")] = _T("Mi-");
        m_Tags[4][_T("Si-")] = _T("Si-");
        m_Tags[4][_T("Fa")] = _T("Fa");
        m_Tags[4][_T("Lam")] = _T("Lam");
        m_Tags[4][_T("Mim")] = _T("Mim");
        m_Tags[4][_T("Sim")] = _T("Sim");
        m_Tags[4][_T("Fa+m")] = _T("Fa+m");
        m_Tags[4][_T("Do+m")] = _T("Do+m");
        m_Tags[4][_T("Sol+m")] = _T("Sol+m");
        m_Tags[4][_T("Re+m")] = _T("Re+m");
        m_Tags[4][_T("La+m")] = _T("La+m");
        m_Tags[4][_T("La-m")] = _T("La-m");
        m_Tags[4][_T("Mi-m")] = _T("Mi-m");
        m_Tags[4][_T("Si-m")] = _T("Si-m");
        m_Tags[4][_T("Fam")] = _T("Fam");
        m_Tags[4][_T("Dom")] = _T("Dom");
        m_Tags[4][_T("Solm")] = _T("Solm");
        m_Tags[4][_T("Rem")] = _T("Rem");

        //special context: single char very frequent elements
        m_Contexts[_T("SingleChar")] = 5;
        m_Tags[5][_T("g")] = _T("g");   //beamed group 
        m_Tags[5][_T("l")] = _T("l");   //tie 
        m_Tags[5][_T("n")] = _T("n");   //note
        m_Tags[5][_T("p")] = _T("p");   //staff number  
        m_Tags[5][_T("r")] = _T("s");   //rest 
        m_Tags[5][_T("t")] = _T("t");   //tuplet 
        m_Tags[5][_T("v")] = _T("v");   //voice

        //context: tuplet options
        m_Contexts[_T("Tuplets")] = 6;
        m_Tags[6][_T("noBracket")] = _T("noBracket");
        m_Tags[6][_T("squaredBracket")] = _T("squaredBracket");
        m_Tags[6][_T("curvedBracket")] = _T("curvedBracket");
        m_Tags[6][_T("numNone")] = _T("numNone");
        m_Tags[6][_T("numActual")] = _T("numActual");
        m_Tags[6][_T("numBoth")] = _T("numBoth");

        //special context: graphic objects
        m_Contexts[_T("GraphObjs")] = 7;
        m_Tags[7][_T("line")] = _T("linea");

    }

}

