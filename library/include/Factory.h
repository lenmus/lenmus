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

#include "BuildOptions.h"
#include "Functor.h"
#include "Singleton.h"

namespace lenmus 
{

/*!
 \brief A factory to create Ldp objects
*/
class LM_EXPORT Factory : public Singleton<Factory>
{
protected:
    //! the functors to create each specific ldp object
	std::map<std::string, Functor<SpLdpElement>*> m_NameToFunctor;
    //! association element-type <-> element-name
	std::map<ELdpElements, std::string>	m_TypeToName;

public:
    Factory();
	virtual ~Factory() {}

    //! Creates an element from its name
	SpLdpElement create(const std::string& name, int numLine=0) const;	
    //! Creates an element from its type
	SpLdpElement create(ELdpElements type, int numLine=0) const;

    //! Get the name from element type
    const std::string& get_name(ELdpElements type) const;

};

static SpLdpElement new_element(ELdpElements type, SpLdpElement value)
{
	SpLdpElement elm = Factory::instance().create(type);
	elm->append_child(value);
	return elm;
}

static SpLdpElement new_value(ELdpElements type, const std::string& value)
{
	SpLdpElement elm = Factory::instance().create(type);
    elm->set_simple();
	elm->set_value(value);
	return elm;
}

static SpLdpElement new_label(const std::string& value)
{
    return new_value(k_label, value);
}

static SpLdpElement new_string(const std::string& value)
{
    return new_value(k_string, value);
}

static SpLdpElement new_number(const std::string& value)
{
    return new_value(k_number, value);
}



}   //namespace lenmus 

#endif      //__LM_ELEMENTS_FACTORY_H__
