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
//      This file is based on the "Factory.h" file from the MusicXML Library
//      v.2.00, distributed under LGPL 2.1 or greater. Copyright (c) 2006 Grame,
//      Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France, 
//      research@grame.fr.
//
//-------------------------------------------------------------------------------------

#ifndef __LM_ELEMENTS_FACTORY_H__
#define __LM_ELEMENTS_FACTORY_H__

#include <string>
#include <map>

#include "lenmus_build_options.h"
#include "lenmus_functor.h"
#include "lenmus_elements.h"

namespace lenmus 
{

class LdpElement;
class LdpFunctor;

/*!
 \brief A factory to create Ldp objects. It is a singleton
*/
class LM_EXPORT Factory
{
protected:
    //! the functors to create each specific ldp object
	std::map<std::string, LdpFunctor*> m_NameToFunctor;
    //! association element-type <-> element-name
	std::map<ELdpElements, std::string>	m_TypeToName;

public:
	virtual ~Factory();
    static Factory* instance();

    //! Creates an element from its name
	LdpElement* create(const std::string& name, int numLine=0) const;	
    //! Creates an element from its type
	LdpElement* create(ELdpElements type, int numLine=0) const;

    //! Get the name from element type
    const std::string& get_name(ELdpElements type) const;

protected:
    Factory();

private:
    static Factory* m_pInstance;
};

static LdpElement* new_element(ELdpElements type, LdpElement* value, int numLine=0)
{
	LdpElement* elm = Factory::instance()->create(type);
	elm->append_child(value);
	return elm;
}

static LdpElement* new_value(ELdpElements type, const std::string& value, int numLine=0)
{
	LdpElement* elm = Factory::instance()->create(type, numLine);
    elm->set_simple();
	elm->set_value(value);
	return elm;
}

static LdpElement* new_label(const std::string& value, int numLine=0)
{
    return new_value(k_label, value, numLine);
}

static LdpElement* new_string(const std::string& value, int numLine=0)
{
    return new_value(k_string, value, numLine);
}

static LdpElement* new_number(const std::string& value, int numLine=0)
{
    return new_value(k_number, value, numLine);
}



}   //namespace lenmus 

#endif      //__LM_ELEMENTS_FACTORY_H__
