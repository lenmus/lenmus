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

/// @file Tree.h
/// @brief Generic classes to build and traverse trees
///

#ifndef __LM_TREE_H__
#define __LM_TREE_H__

#include <iostream>
#include <stack>
#include <vector>
#include <iterator>
#include <stdexcept>
#include "SmartPointer.h"
#include "../visitors/Visitor.h"

namespace lenmus
{


template <typename T>
class LM_EXPORT Tree : virtual public RefCounted
{
public:
    typedef SmartPtr<T> SpNodeInTree;

protected:
	SpNodeInTree m_root;

public:
    Tree() {}
    Tree(const SmartPtr<T>& node) { m_root = node; }

    void set_root(const SmartPtr<T>& node) { m_root = node; }
    SpNodeInTree get_root() { return m_root; }

    class depth_first_iterator
    {
        protected:
            SpNodeInTree m_currentNode;

        public:
            depth_first_iterator() {}
	        depth_first_iterator(const SpNodeInTree& node) { m_currentNode = node; }
            virtual ~depth_first_iterator() {}

	        SpNodeInTree operator *() const { return m_currentNode; }
		    bool operator ==(const depth_first_iterator& it) const {
			    return m_currentNode.get_pointer() == it.m_currentNode.get_pointer();
		    }
		    bool operator !=(const depth_first_iterator& it) const {
                return m_currentNode.get_pointer() != it.m_currentNode.get_pointer();
            }

            depth_first_iterator& operator ++()
            {
                if (m_currentNode == NULL)
                    return *this;   //we are at end

	            if (m_currentNode->get_first_child() != NULL)
                {
		            m_currentNode = m_currentNode->get_first_child();
	            }
	            else
                {
                    //proceed with next sibling
                    if (m_currentNode->get_next_sibling())
                        m_currentNode = m_currentNode->get_next_sibling();
                    else
                    {
                        //go back and proceed with next sibling
                        SpNodeInTree& node = m_currentNode;
                        while(node && node->get_next_sibling() == NULL)
                        {
                            node = m_currentNode->get_parent();
                        }
                        m_currentNode = (node ? node->get_next_sibling() : node);
                    }
                }
	            return *this;
            }

            depth_first_iterator& operator --()
            {
                if (m_currentNode == NULL)
                    return *this;   //we are at end

	            if (m_currentNode->get_prev_sibling() != NULL)
                {
		            m_currentNode = m_currentNode->get_prev_sibling();
	            }
	            else
                {
                    //proceed with parent
                    m_currentNode->get_parent();
                }
	            return *this;
            }

    };

    typedef depth_first_iterator iterator;

    depth_first_iterator begin() { return depth_first_iterator(m_root); }
    depth_first_iterator end() { return depth_first_iterator(); }

    /// Erase a node. Removes from the tree the node and all its descendants.
    /// Returns iterator pointing to next node after the erased one, where 'next'
    /// must be interpreted according traversal method ordering rules
    iterator erase(iterator position);

    iterator replace_node(iterator position, const SmartPtr<T>& newNode);

};


/// A node in the tree. It is a base abstract class from which any tree node must derive.
/// It adds the links to place the node in the tree and provides iterators for traversing
/// the tree.
template<class T>
class NodeInTree : public Tree<T>
{
public:
    typedef SmartPtr<T> SpNodeInTree;

protected:
    SpNodeInTree m_parent;
	SpNodeInTree m_firstChild;
    SpNodeInTree m_lastChild;
	SpNodeInTree m_prevSibling;
    SpNodeInTree m_nextSibling;

    NodeInTree() {};

public:
    //getters
    virtual SpNodeInTree get_parent() { return m_parent; }
    virtual SpNodeInTree get_first_child() { return m_firstChild; }
    virtual SpNodeInTree get_last_child() { return m_lastChild; }
    virtual SpNodeInTree get_prev_sibling() { return m_prevSibling; }
    virtual SpNodeInTree get_next_sibling() { return m_nextSibling; }

    //setters
    virtual void set_parent(const SpNodeInTree& parent) { m_parent = parent; }
    virtual void set_first_child(const SpNodeInTree& firstChild) { m_firstChild = firstChild; }
    virtual void set_last_child(const SpNodeInTree& lastChild) { m_lastChild = lastChild; }
    virtual void set_prev_sibling(const SpNodeInTree& prevSibling) { m_prevSibling = prevSibling; }
    virtual void set_next_sibling(const SpNodeInTree& nextSibling) { m_nextSibling = nextSibling; }

    /// returns 'true' if this node is terminal (doesn't have children)
	virtual bool is_terminal() const { return m_firstChild==NULL; }

	/// returns true if this node is the root, that is, if there is no parent node
	virtual bool is_root() const { return m_parent == NULL; }

    //methods related to children
	virtual void append_child(const SpNodeInTree& child);
    virtual int get_num_children();
    virtual SpNodeInTree get_child(int i);


    class children_iterator
    {
        protected:
            SpNodeInTree m_currentNode;

        public:
            children_iterator() {};
            children_iterator(const SpNodeInTree& n) { m_currentNode = n; }
            children_iterator(T* pNode) { m_currentNode = SmartPtr<T>(pNode); }
            children_iterator(NodeInTree<T>* n) { m_currentNode =  dynamic_cast<T*>(n); }
            virtual ~children_iterator() {}

	        SpNodeInTree operator *() const { return m_currentNode; }

	        children_iterator& operator ++() {
                //cout << "before: it=" << m_currentNode.get_pointer() << endl;
                if (m_currentNode.get_pointer())
                {
                    //cout << "sibling=" << m_currentNode->get_next_sibling() << endl;
                    m_currentNode = m_currentNode->get_next_sibling();
                }
                //cout << "after: it=" << m_currentNode.get_pointer() << endl;
                return *this;
            }

	        children_iterator& operator --() {
                //cout << "before: it=" << m_currentNode.get_pointer() << endl;
                if (m_currentNode.get_pointer())
                {
                    //cout << "sibling=" << m_currentNode->get_next_sibling() << endl;
                    m_currentNode = m_currentNode->get_prev_sibling();
                }
                //cout << "after: it=" << m_currentNode.get_pointer() << endl;
                return *this;
            }

		    bool operator ==(const children_iterator& it) const {
			    return m_currentNode.get_pointer() == it.m_currentNode.get_pointer();
		    }
		    bool operator !=(const children_iterator& it) const {
                return m_currentNode.get_pointer() != it.m_currentNode.get_pointer();
            }

    };

    children_iterator begin_children() { return children_iterator(m_firstChild); }
    children_iterator end_children() { return children_iterator(); }

};


template <class T>
void NodeInTree<T>::append_child(const SpNodeInTree& child)
{
    SpNodeInTree oldLastChild = m_lastChild;

    //links in child
    child->set_parent( dynamic_cast<T*>(this));
    child->set_prev_sibling( m_lastChild );
    child->set_next_sibling( NULL );

    //links in parent
    if (!m_firstChild)
        m_firstChild = child;
    m_lastChild = child;

    //links in old last child
    if (oldLastChild)
    {
        assert( oldLastChild->get_next_sibling() == NULL );
        oldLastChild->set_next_sibling( child );
    }

    //cout << "Append child ----------------------------------" << endl;
    //cout << "first child: " << m_firstChild << ", last child: " << m_lastChild << endl;
    //cout << "prev sibling: " << m_prevSibling << ", next sibling: " << m_nextSibling << endl;

    //cout << "Added child -----------------------------------" << endl;
    //cout << "first child: " << child->get_first_child() << ", last child: " << child->get_last_child() << endl;
    //cout << "prev sibling: " << child->get_prev_sibling() << ", next sibling: " << child->get_next_sibling() << endl;
    //cout << "-----------------------------------------------" << endl;

    //system("pause");

}

template <class T>
int NodeInTree<T>::get_num_children()
{
    NodeInTree<T>::children_iterator it;
	int numChildren = 0;
    for (it=this->begin_children(); it != this->end_children(); ++it)
    {
        //cout << "it=" << (*it).get_pointer() << endl;
        //cout << "this.end_children=" << *(this->end_children()) << endl;
        numChildren++;
    }
    return numChildren;
}

template <class T>
SmartPtr<T> NodeInTree<T>::get_child(int i)
{
    // i = 1..n
    children_iterator it(this);
    int numChild = 1;
    for (it=this->begin_children(); it != this->end_children() && numChild < i; ++it, ++numChild);
    if (it != this->end_children() && i == numChild)
    {
        //return *it;
        return (*it).get_pointer();
    }
    else
        throw std::runtime_error( "[NodeInTree<T>::get_child]. Num child greater than available children" );
}

template <class T>
typename Tree<T>::depth_first_iterator Tree<T>::erase(depth_first_iterator position)
{
    SpNodeInTree nodeToErase = *position;

    //links in previous sibling
    if (nodeToErase->get_prev_sibling())
    {
        SpNodeInTree prevSibling = nodeToErase->get_prev_sibling();
        prevSibling->set_next_sibling( nodeToErase->get_next_sibling() );
    }

    //links in next sibling
    if (nodeToErase->get_next_sibling())
    {
        SpNodeInTree nextSibling = nodeToErase->get_next_sibling();
        nextSibling->set_prev_sibling( nodeToErase->get_prev_sibling() );
    }

    //links in parent
    if (!nodeToErase->is_root())
    {
        SpNodeInTree parent = nodeToErase->get_parent();
        if (parent->get_first_child() == nodeToErase)
            parent->set_first_child( nodeToErase->get_next_sibling() );
        if (parent->get_last_child() == nodeToErase)
            parent->set_last_child( nodeToErase->get_prev_sibling() );
    }

    //determine next node after deleted one
    if (nodeToErase->get_next_sibling())
        return nodeToErase->get_next_sibling();
    else
    {
        //go up and take next sibling
        if (!nodeToErase->is_root())
            return nodeToErase->get_parent()->get_next_sibling();
        else
        {
            set_root(NULL);
            return end();
        }
    }
}

/// Replaces iterator pointed node (and all its sub-tree) by the received node (and
/// its sub-tree). Return iterator pointing to new node
template <class T>
typename Tree<T>::depth_first_iterator Tree<T>::replace_node(depth_first_iterator position, const SpNodeInTree& newNode)
{
    SpNodeInTree nodeToReplace = *position;

    //links in new node
    newNode->set_parent( nodeToReplace->get_parent() );
    newNode->set_prev_sibling( nodeToReplace->get_prev_sibling() );
    newNode->set_next_sibling( nodeToReplace->get_next_sibling() );

    //links in previous sibling
    if (nodeToReplace->get_prev_sibling())
    {
        SpNodeInTree prevSibling = nodeToReplace->get_prev_sibling();
        prevSibling->set_next_sibling( newNode );
    }

    //links in next sibling
    if (nodeToReplace->get_next_sibling())
    {
        SpNodeInTree nextSibling = nodeToReplace->get_next_sibling();
        nextSibling->set_prev_sibling( newNode );
    }

    //links in parent
    if (!nodeToReplace->is_root())
    {
        SpNodeInTree parent = nodeToReplace->get_parent();
        if (parent->get_first_child() == nodeToReplace)
            parent->set_first_child( newNode );
        if (parent->get_last_child() == nodeToReplace)
            parent->set_last_child( newNode );
    }
    else
        set_root(newNode);

    return newNode;
}





}   //namespace lenmus

#endif      //__LM_TREE_H__
