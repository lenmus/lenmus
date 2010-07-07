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

#ifndef __LM_OBSERVABLE_H__
#define __LM_OBSERVABLE_H__

#include <list>

using namespace std;

namespace lenmus 
{

//forward declarations
class Observable;


//-------------------------------------------------------------------------------------
class Observer
{
public:
	virtual void handle_event(Observable* ref) = 0;
};
 

//-------------------------------------------------------------------------------------
class Observable
{
protected:
    std::list<Observer*>    m_observers;

public:
    Observable() {}
	virtual ~Observable() {}

	void add_observer(Observer* ref) {
		m_observers.push_back(ref);
	}
 
	void remove_observer(Observer* ref) {
		m_observers.remove(ref);
	}
 
protected:
	void notify_observers() {
        std::list<Observer*>::iterator it;
		for(it = m_observers.begin(); it != m_observers.end(); ++it)
            (*it)->handle_event(this);
	}

};

 
}   //namespace lenmus

#endif      //__LM_OBSERVABLE_H__
