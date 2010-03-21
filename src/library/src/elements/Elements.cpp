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
//-------------------------------------------------------------------------------------

#include <sstream>
#include "Elements.h"

using namespace std;

namespace lenmus
{

LdpElement::LdpElement()
    : m_fSimple(false)
    , m_name(_T(""))
    , m_type(k_undefined)
{
}

SpLdpElement LdpElement::create() 
{ 
    LdpElement* pElm = new LdpElement();
    assert(pElm != 0);
    return pElm; 
}

void LdpElement::accept_in(BaseVisitor& v)
{
	Visitor<SpLdpElement>* p = dynamic_cast<Visitor<SpLdpElement>*>(&v);
	if (p)
    {
		SpLdpElement element = this;
		p->VisitStart(element);
	}
}

void LdpElement::accept_out(BaseVisitor& v)
{
	Visitor<SpLdpElement>* p = dynamic_cast<Visitor<SpLdpElement>*>(&v);
	if (p)
    {
		SpLdpElement element = this;
		p->VisitEnd(element);
	}
}

bool LdpElement::operator ==(LdpElement& element)
{
	if (get_type() != element.get_type())
        return false;
	if (get_name() != element.get_name())
        return false;
    if (get_num_parameters() != element.get_num_parameters())
        return false;
    for (int i=0; i < get_num_parameters(); i++)
    {
        LdpElement* parmThis = get_parameter(i);
        LdpElement* parmElm = element.get_parameter(i);
	    if (parmThis->to_string() != parmElm->to_string())
            return false;
    }
	return true;
}

string_type LdpElement::get_ldp_value()
{
    if (m_type == k_string)
    {
        tstringstream s;
        s << _T("\"") << m_value << _T("\"");
        return s.str();
    }
    else
        return m_value;
}

string_type LdpElement::to_string()
{
	tstringstream s;
    if (is_simple())
	    s << get_ldp_value();
    else
    {
	    s << _T("(") << m_name;
        if (has_children())
        {
            SimpleTree<LdpElement>::literator it;
            for (it = lbegin(); it != lend(); ++it)
	            s << _T(" ") << (*it)->to_string();
        }
        else
            s << _T(" ") << get_ldp_value();

        s << _T(")");
    }
    return s.str();
}
    
int LdpElement::get_num_parameters() 
{ 
    return static_cast<int>(elements().size()); 
}

LdpElement* LdpElement::get_parameter(int i)
{
    assert( i >= 0 && i < get_num_parameters());
    return elements().at(i);
}



}   //namespace lenmus
