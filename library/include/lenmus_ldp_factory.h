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
//      This file is based on the "LdpFactory.h" file from the MusicXML Library
//      v.2.00, distributed under LGPL 2.1 or greater. Copyright (c) 2006 Grame,
//      Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France, 
//      research@grame.fr.
//
//-------------------------------------------------------------------------------------

#ifndef __LML_LDP_FACTORY_H__
#define __LML_LDP_FACTORY_H__

#include <string>
#include <map>

#include "lenmus_build_options.h"
#include "lenmus_functor.h"
#include "lenmus_ldp_elements.h"

namespace lenmus 
{

class LdpElement;
class LdpFunctor;

// A factory to create Ldp elements. Should have library scope
class LM_EXPORT LdpFactory
{
protected:
    //the functors to create each specific ldp object
	std::map<std::string, LdpFunctor*> m_NameToFunctor;
    //association element-type <-> element-name
	std::map<ELdpElement, std::string>	m_TypeToName;

public:
    LdpFactory();
	virtual ~LdpFactory();

    //Creates an element from its name
	LdpElement* create(const std::string& name, int numLine=0) const;	
    //Creates an element from its type
	LdpElement* create(ELdpElement type, int numLine=0) const;

    //Get the name from element type
    const std::string& get_name(ELdpElement type) const;

    //utility methods
    LdpElement* new_element(ELdpElement type, LdpElement* value, int numLine=0)
    {
	    LdpElement* elm = create(type);
	    elm->append_child(value);
	    return elm;
    }

    LdpElement* new_value(ELdpElement type, const std::string& value, int numLine=0)
    {
	    LdpElement* elm = create(type, numLine);
        elm->set_simple();
	    elm->set_value(value);
	    return elm;
    }

    LdpElement* new_label(const std::string& value, int numLine=0) {
        return new_value(k_label, value, numLine);
    }

    LdpElement* new_string(const std::string& value, int numLine=0) {
        return new_value(k_string, value, numLine);
    }

    LdpElement* new_number(const std::string& value, int numLine=0) {
        return new_value(k_number, value, numLine);
    }

};

}   //namespace lenmus 

#endif      //__LML_LDP_FACTORY_H__
