//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2012 Cecilio Salmeron. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this 
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#include "lomse_basic_model_browser.h"


namespace lomse
{
        

//---------------------------------------------------------------------------------------
// interface for the Hierarchical Visitor: 
class ImVisitor
{
public:
    ImVisitor() {}
    virtual ~ImVisitor() {}

    virtual bool visitEnter( Composite node )=0;  // going into a branch
    virtual bool visitLeave( Composite node )=0;   // coming out
    virtual bool visit( Leaf node )=0;
}


//---------------------------------------------------------------------------------------
// To make life a little easier let's define a default visitor (null object)
class NullImVisitor : public ImVisitor
{
public:
    NullImVisitor() : ImVisitor() {}
    ~NullImVisitor() {}

    bool visitEnter( Composite node ) { return true; }
	bool visitLeave( Composite node ) { return true; }
	bool visit( Leaf node ) { return true; }
}


//---------------------------------------------------------------------------------------
// Now we just need to create the Composite structure. For details on this
// see CompositePattern. The only variation is the accept methods which both 
// need to return a bool. These members should be implemented as follows: 

	bool Composite.accept( ImVisitor v )
	{
	    if ( v.visitEnter( this ) )  // enter this node?
	    {
		    Iterator at = m_children.iterator();
		    while ( at.hasNext() )
		    if ( ! ((Component)at.next()).accept( v ) )
			    break;
	    }

	    return v.visitLeave( this );
	}


//---------------------------------------------------------------------------------------
// And the leaf implementation: 

	bool accept( ImVisitor visitor )
	{
	    return visitor.visit( this );
	}



//-------------------------------------------------------------------------------------
// ImBrowser implementation
//-------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Base class for visitors
template<class T>
class TreeBrowser 
{
public:
    TreeBrowser(T* tree) {}

    //tree is a pointer to root node. Must derive from NodeInTree<T>
    virtual void browse(T* tree)
    {
        enter(t);
        typename SimpleTree<T>::literator it;
        for (it = t.lbegin(); it != t.lend(); ++it)
            browse(**it);
        leave(t);
    }
        Visitor<ImoParagraph>* p = dynamic_cast<Visitor<ImoParagraph>*>(&v);
        if (p)
            p->start_visit(this);

        TreeNode<ImoObj>::children_iterator it;
        for (it = this->begin(); it != this->end(); ++it)
        {
            (*it)->accept_visitor(v);
        }

        if (p)
            p->end_visit(this);

};
class ImBrowser
{


ImBrowser::ImBrowser(ImObjVisitor& visitor)
    : m_visitor(visitor)
{
}

ImBrowser::~ImBrowser()
{
}

void ImBrowser::browse(ImoObj* pImo)
{
//    if (pImo->IsDocumnet())
//        browse_document(pImo, m_visitor);
//    else if (pImo->IsScore
    ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pImo );
    enter(pDoc);
//    for each content element
//        if <text>
//            browse_text();
//        else
//            browse_score();
    leave(pDoc);
}

void ImBrowser::browse_score(ImoScore* pScore)
{
    enter(pScore);
//    for each option
//        trverse option
//    for each attachment
//        traverse  attachment
//    for each instrument
//        traverse instrument
    leave(pScore);
}

void ImBrowser::browse_instrument()
{
//    for each staffobj
//        traverse staffobj
}


}  //namespace lomse
