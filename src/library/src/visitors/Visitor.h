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

#ifndef __LM_VISITOR_H__
#define __LM_VISITOR_H__

namespace lenmus 
{

/*!
 \brief The root base class for all visitors
*/
class BaseVisitor
{
public:
	virtual ~BaseVisitor() {}
};

/*!
 \brief Base class for visitors
*/
template<class T>
class Visitor : public BaseVisitor
{
public:
	virtual ~Visitor() {}
	virtual void VisitStart(T& element) {};
	virtual void VisitEnd(T& element) {};
};

/*!
 \brief Base class for objects accepting visitors
*/
class Visitable
{
public:
	virtual ~Visitable() {}
	virtual void accept_in(BaseVisitor& v) {}
	virtual void accept_out(BaseVisitor& v) {}
};

} //namespace lenmus


#endif      //__LM_VISITOR_H__
