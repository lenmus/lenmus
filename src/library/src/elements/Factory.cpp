//--------------------------------------------------------------------------------------
//  LenMus Library
//  Copyright (c) 2010 LenMus project
//
//  This program is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along
//  with this library; if not, see <http://www.gnu.org/licenses/> or write to the
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
//  MA  02111-1307,  USA.
//
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//  Credits:
//      This file is based on the "Factory.cpp" file from the MusicXML Library
//      v.2.00, distributed under LGPL 2.1 or greater. Copyright (c) 2006 Grame,
//      Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France, 
//      research@grame.fr.
//
//-------------------------------------------------------------------------------------

#ifdef VC6
# pragma warning (disable : 4786)
#endif

#include <sstream>
#include <iostream>
#include <exception>
#include "Elements.h"
#include "Factory.h"

using namespace std;

namespace lenmus
{


template<int type>
class LdpElementFunctor : public Functor<SpLdpElement> 
{
	public:
		SpLdpElement operator ()() {  return LdpObject<type>::NewLdpObject(); }
};


Factory::Factory()
{
    //Register all ldp elements

    //simple, generic elements
    m_TypeToName[k_label] = _T("label");
    m_TypeToName[k_number] = _T("number");
    m_TypeToName[k_string] = _T("string");

    //composite elements
    m_TypeToName[k_abbrev] = _T("abbrev");
    m_TypeToName[k_above] = _T("above");
    m_TypeToName[k_barline] = _T("barline");
    m_TypeToName[k_below] = _T("below");
    m_TypeToName[k_bold] = _T("bold");
    m_TypeToName[k_bold_italic] = _T("bold-italic");
    m_TypeToName[k_brace] = _T("brace");
    m_TypeToName[k_bracket] = _T("bracket");
    m_TypeToName[k_center] = _T("center");
    m_TypeToName[k_chord] = _T("chord");
    m_TypeToName[k_clef] = _T("clef");
    m_TypeToName[k_color] = _T("color");            
    m_TypeToName[k_defineStyle] = _T("defineStyle");
    m_TypeToName[k_down] = _T("down");
    m_TypeToName[k_duration] = _T("duration");
    m_TypeToName[k_dx] = _T("dx");
    m_TypeToName[k_dy] = _T("dy");
    m_TypeToName[k_end] = _T("end");
    m_TypeToName[k_fermata] = _T("fermata");
    m_TypeToName[k_font] = _T("font");
    m_TypeToName[k_goBack] = _T("goBack");
    m_TypeToName[k_goFwd] = _T("goFwd");
    m_TypeToName[k_graphic] = _T("graphic");
    m_TypeToName[k_group] = _T("group");
    m_TypeToName[k_hasWidth] = _T("hasWidth");
    m_TypeToName[k_infoMIDI] = _T("infoMIDI");
    m_TypeToName[k_instrName] = _T("instrName");
    m_TypeToName[k_instrument] = _T("instrument");
    m_TypeToName[k_italic] = _T("italic");
    m_TypeToName[k_joinBarlines] = _T("joinBarlines");
    m_TypeToName[k_key] = _T("key");
    m_TypeToName[k_landscape] = _T("landscape");
    m_TypeToName[k_language] = _T("language");
    m_TypeToName[k_left] = _T("left");
    m_TypeToName[k_line] = _T("line");
    m_TypeToName[k_metronome] = _T("metronome");
    m_TypeToName[k_musicData] = _T("musicData");
    m_TypeToName[k_name] = _T("name");
    m_TypeToName[k_newSystem] = _T("newSystem");
    m_TypeToName[k_no] = _T("no");
    m_TypeToName[k_normal] = _T("normal");
    m_TypeToName[k_noVisible] = _T("noVisible");
    m_TypeToName[k_opt] = _T("opt");
    m_TypeToName[k_pageLayout] = _T("pageLayout");
    m_TypeToName[k_pageMargins] = _T("pageMargins");
    m_TypeToName[k_pageSize] = _T("pageSize");
    m_TypeToName[k_parentheses] = _T("parentheses");
    m_TypeToName[k_pitch] = _T("pitch");
    m_TypeToName[k_portrait] = _T("portrait");
    m_TypeToName[k_right] = _T("right");
    m_TypeToName[k_score] = _T("score");
    m_TypeToName[k_spacer] = _T("spacer");
    m_TypeToName[k_split] = _T("split");
    m_TypeToName[k_start] = _T("start");
    m_TypeToName[k_staves] = _T("staves");
    m_TypeToName[k_stem] = _T("stem");
    m_TypeToName[k_style] = _T("style");
    m_TypeToName[k_symbol] = _T("symbol");
    m_TypeToName[k_style] = _T("style");
    m_TypeToName[k_systemLayout] = _T("systemLayout");
    m_TypeToName[k_systemMargins] = _T("systemMargins");
    m_TypeToName[k_text] = _T("text");
    m_TypeToName[k_time] = _T("time");
    m_TypeToName[k_title] = _T("title");
    m_TypeToName[k_vers] = _T("vers");
    m_TypeToName[k_up] = _T("up");
    m_TypeToName[k_yes] = _T("yes");

    m_TypeToName[k_g] = _T("g");   //beamed group 
    m_TypeToName[k_l] = _T("l");   //tie 
    m_TypeToName[k_note] = _T("n");   //note
    m_TypeToName[k_p] = _T("p");   //staff number  
    m_TypeToName[k_r] = _T("r");   //rest 
    m_TypeToName[k_t] = _T("t");   //tuplet 
    m_TypeToName[k_v] = _T("v");   //voice


    //Register all types
    m_NameToFunctor[_T("label")] = new LdpElementFunctor<k_label>;
    m_NameToFunctor[_T("number")] = new LdpElementFunctor<k_number>;
    m_NameToFunctor[_T("string")] = new LdpElementFunctor<k_string>;

    m_NameToFunctor[_T("abbrev")] = new LdpElementFunctor<k_abbrev>;
    m_NameToFunctor[_T("above")] = new LdpElementFunctor<k_above>;
    m_NameToFunctor[_T("barline")] = new LdpElementFunctor<k_barline>;
    m_NameToFunctor[_T("below")] = new LdpElementFunctor<k_below>;
    m_NameToFunctor[_T("bold")] = new LdpElementFunctor<k_bold>;
    m_NameToFunctor[_T("bold_italic")] = new LdpElementFunctor<k_bold_italic>;
    m_NameToFunctor[_T("brace")] = new LdpElementFunctor<k_brace>;
    m_NameToFunctor[_T("bracket")] = new LdpElementFunctor<k_bracket>;
    m_NameToFunctor[_T("center")] = new LdpElementFunctor<k_center>;
    m_NameToFunctor[_T("chord")] = new LdpElementFunctor<k_chord>;
    m_NameToFunctor[_T("clef")] = new LdpElementFunctor<k_clef>;
    m_NameToFunctor[_T("color")] = new LdpElementFunctor<k_color>;            
    m_NameToFunctor[_T("defineStyle")] = new LdpElementFunctor<k_defineStyle>;
    m_NameToFunctor[_T("down")] = new LdpElementFunctor<k_down>;
    m_NameToFunctor[_T("duration")] = new LdpElementFunctor<k_duration>;
    m_NameToFunctor[_T("dx")] = new LdpElementFunctor<k_dx>;
    m_NameToFunctor[_T("dy")] = new LdpElementFunctor<k_dy>;
    m_NameToFunctor[_T("end")] = new LdpElementFunctor<k_end>;
    m_NameToFunctor[_T("fermata")] = new LdpElementFunctor<k_fermata>;
    m_NameToFunctor[_T("font")] = new LdpElementFunctor<k_font>;
    m_NameToFunctor[_T("goBack")] = new LdpElementFunctor<k_goBack>;
    m_NameToFunctor[_T("goFwd")] = new LdpElementFunctor<k_goFwd>;
    m_NameToFunctor[_T("graphic")] = new LdpElementFunctor<k_graphic>;
    m_NameToFunctor[_T("group")] = new LdpElementFunctor<k_group>;
    m_NameToFunctor[_T("hasWidth")] = new LdpElementFunctor<k_hasWidth>;
    m_NameToFunctor[_T("infoMIDI")] = new LdpElementFunctor<k_infoMIDI>;
    m_NameToFunctor[_T("instrName")] = new LdpElementFunctor<k_instrName>;
    m_NameToFunctor[_T("instrument")] = new LdpElementFunctor<k_instrument>;
    m_NameToFunctor[_T("italic")] = new LdpElementFunctor<k_italic>;
    m_NameToFunctor[_T("joinBarlines")] = new LdpElementFunctor<k_joinBarlines>;
    m_NameToFunctor[_T("key")] = new LdpElementFunctor<k_key>;
    m_NameToFunctor[_T("landscape")] = new LdpElementFunctor<k_landscape>;
    m_NameToFunctor[_T("language")] = new LdpElementFunctor<k_language>;
    m_NameToFunctor[_T("line")] = new LdpElementFunctor<k_line>;
    m_NameToFunctor[_T("left")] = new LdpElementFunctor<k_left>;
    m_NameToFunctor[_T("metronome")] = new LdpElementFunctor<k_metronome>;
    m_NameToFunctor[_T("musicData")] = new LdpElementFunctor<k_musicData>;
    m_NameToFunctor[_T("name")] = new LdpElementFunctor<k_name>;
    m_NameToFunctor[_T("newSystem")] = new LdpElementFunctor<k_newSystem>;
    m_NameToFunctor[_T("no")] = new LdpElementFunctor<k_no>;
    m_NameToFunctor[_T("normal")] = new LdpElementFunctor<k_normal>;
    m_NameToFunctor[_T("noVisible")] = new LdpElementFunctor<k_noVisible>;
    m_NameToFunctor[_T("opt")] = new LdpElementFunctor<k_opt>;
    m_NameToFunctor[_T("pageLayout")] = new LdpElementFunctor<k_pageLayout>;
    m_NameToFunctor[_T("pageMargins")] = new LdpElementFunctor<k_pageMargins>;
    m_NameToFunctor[_T("pageSize")] = new LdpElementFunctor<k_pageSize>;
    m_NameToFunctor[_T("parentheses")] = new LdpElementFunctor<k_parentheses>;
    m_NameToFunctor[_T("pitch")] = new LdpElementFunctor<k_pitch>;
    m_NameToFunctor[_T("portrait")] = new LdpElementFunctor<k_portrait>;
    m_NameToFunctor[_T("right")] = new LdpElementFunctor<k_right>;
    m_NameToFunctor[_T("score")] = new LdpElementFunctor<k_score>;
    m_NameToFunctor[_T("spacer")] = new LdpElementFunctor<k_spacer>;
    m_NameToFunctor[_T("split")] = new LdpElementFunctor<k_split>;
    m_NameToFunctor[_T("start")] = new LdpElementFunctor<k_start>;
    m_NameToFunctor[_T("staves")] = new LdpElementFunctor<k_staves>;
    m_NameToFunctor[_T("stem")] = new LdpElementFunctor<k_stem>;
    m_NameToFunctor[_T("style")] = new LdpElementFunctor<k_style>;
    m_NameToFunctor[_T("symbol")] = new LdpElementFunctor<k_symbol>;
    m_NameToFunctor[_T("systemLayout")] = new LdpElementFunctor<k_systemLayout>;
    m_NameToFunctor[_T("systemMargins")] = new LdpElementFunctor<k_systemMargins>;
    m_NameToFunctor[_T("text")] = new LdpElementFunctor<k_text>;
    m_NameToFunctor[_T("time")] = new LdpElementFunctor<k_time>;
    m_NameToFunctor[_T("title")] = new LdpElementFunctor<k_title>;
    m_NameToFunctor[_T("vers")] = new LdpElementFunctor<k_vers>;
    m_NameToFunctor[_T("up")] = new LdpElementFunctor<k_up>;
    m_NameToFunctor[_T("yes")] = new LdpElementFunctor<k_yes>;

    m_NameToFunctor[_T("g")] = new LdpElementFunctor<k_g>;   //beamed group 
    m_NameToFunctor[_T("l")] = new LdpElementFunctor<k_l>;   //tie 
    m_NameToFunctor[_T("n")] = new LdpElementFunctor<k_note>;   //note
    m_NameToFunctor[_T("p")] = new LdpElementFunctor<k_p>;   //staff number  
    m_NameToFunctor[_T("r")] = new LdpElementFunctor<k_r>;   //rest 
    m_NameToFunctor[_T("t")] = new LdpElementFunctor<k_t>;   //tuplet 
    m_NameToFunctor[_T("v")] = new LdpElementFunctor<k_v>;   //voice
}

SpLdpElement Factory::create(const string_type& name) const
{
	map<string_type, Functor<SpLdpElement>*>::const_iterator it 
        = m_NameToFunctor.find(name);
	if (it != m_NameToFunctor.end())
    {
		Functor<SpLdpElement>* f= it->second;
		if (f)
        {
			SpLdpElement element = (*f)();
			element->set_name(name);
			return element;
		}
	}
    std::tstringstream err;
    err << _T("Factory::create() called with unknown element \"")
        << name << _T("\"") << endl;
    cout << err.str();
    throw std::runtime_error( err.str() );
	return 0;
}

SpLdpElement Factory::create(int type) const
{
	map<int, string_type>::const_iterator it = m_TypeToName.find( type );
	if (it != m_TypeToName.end())
		return create(it->second);

    std::tstringstream err;
    err << _T("Factory::create called with unknown type \"")
        << type << _T("\"") << endl;
    throw std::runtime_error( err.str() );
	return 0;
}

}   //namespace lenmus 
