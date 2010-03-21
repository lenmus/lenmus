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

#ifndef __LM_SIMPLE_TREE_H__
#define __LM_SIMPLE_TREE_H__

#include <iostream>
#include <stack>
#include <vector>
#include <iterator>
#include "SmartPointer.h"
#include "../visitors/Visitor.h"

namespace lenmus 
{

template <typename T> 
class LM_EXPORT treeIterator : public std::iterator<std::input_iterator_tag, T>
{
	protected:
		typedef typename std::vector<T>::iterator nodes_iterator;
		typedef std::pair<nodes_iterator, T> state;

		std::stack<state>	fStack;
		T					fRootElement;
		nodes_iterator		fCurrentIterator;

	public:
		treeIterator() {}
		treeIterator(const T& t, bool end=false) {
			fRootElement = t;
			if (end) fCurrentIterator = t->elements().end();
			else forward_down (t);
		}
		treeIterator(const treeIterator& a)  { *this = a; }
		virtual ~treeIterator() {}
		
		T operator  *() const	{ return *fCurrentIterator; }
		T operator ->() const	{ return *fCurrentIterator; } 
		
        T getParent() const		{ return fStack.size() ? fStack.top().second : fRootElement; }
		
		virtual void forward_down(const T& t) {
			fCurrentIterator = t->elements().begin();
			if (fCurrentIterator != t->elements().end())
				fStack.push( make_pair(fCurrentIterator+1, t));
		}

		// current element is empty: go up to parent element and possibly down to neighbor element
		void forward_up() {
			while (fStack.size()) {
				state s = fStack.top();
				fStack.pop();

				fCurrentIterator = s.first;
				if (fCurrentIterator != s.second->elements().end()) {
					fStack.push( make_pair(fCurrentIterator+1, s.second));
					return;
				}
			}
		}
		
		// move the iterator forward
		void forward() {
			if ((*fCurrentIterator)->size()) forward_down(*fCurrentIterator);
			else forward_up();
		}
		treeIterator& operator ++()		{ forward(); return *this; }
		treeIterator& operator ++(int)	{ forward(); return *this; }

		treeIterator& erase() {
			T parent = getParent();
			fCurrentIterator = parent->elements().erase(fCurrentIterator);
			if (fStack.size()) fStack.pop();
			if (fCurrentIterator != parent->elements().end()) {
				fStack.push( make_pair(fCurrentIterator+1, parent));
			}
			else forward_up();
			return *this; 
		}

		treeIterator& insert(const T& value) {
			T parent = getParent();
			fCurrentIterator = parent->elements().insert(fCurrentIterator, value);
			if (fStack.size()) fStack.pop();
			fStack.push( make_pair(fCurrentIterator+1, parent));
			return *this;
		}

		bool operator ==(const treeIterator& i) const		{ 
			// we check that the iterators have the same parent (due to iterator compatibility issue with visual c++)
			return getParent() == i.getParent() ?  ( fCurrentIterator==i.fCurrentIterator ) : false;
		}
		bool operator !=(const treeIterator& i) const		{ return !(*this == i); }
};

/*!
 \brief a simple tree representation
*/
//------------------------------------------------------------------------------
template <typename T>
class LM_EXPORT SimpleTree : virtual public RefCounted
{
public:
	typedef SmartPtr<T>					treePtr;	///< the node sub elements type
	typedef std::vector<treePtr>		branchs;	///< the node sub elements container type
	typedef typename branchs::iterator	literator;	///< the current level iterator type
	typedef treeIterator<treePtr>		iterator;	///< the top -> bottom iterator type

	static treePtr new_tree() { SimpleTree<T>* o = new SimpleTree<T>; assert(o!=0); return o; }
	
	branchs& elements() { return fElements; }		
	const branchs& elements() const { return fElements; }		
	virtual void push(const treePtr& t)	{ fElements.push_back(t); }
	virtual int size() const { return (int)fElements.size(); }
	virtual bool empty() const { return fElements.size()==0; }

	iterator begin() { treePtr start=dynamic_cast<T*>(this); return iterator(start); }
	iterator end() { treePtr start=dynamic_cast<T*>(this); return iterator(start, true); }
	iterator erase(iterator i) { return i.erase(); }
	iterator insert(iterator before, const treePtr& value)	{ return before.insert(value); }
	
	literator lbegin() { return fElements.begin(); }
	literator lend() { return fElements.end(); }

protected:
    SimpleTree() {}
	virtual ~SimpleTree() {}

private:
	branchs	 fElements;
};


}   //namespace lenmus

#endif      //__LM_SIMPLE_TREE_H__
