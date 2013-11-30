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

#ifndef __LOMSE__BASIC_MODEL_BROWSER_H__        //to avoid nested includes
#define __LOMSE__BASIC_MODEL_BROWSER_H__

#include "lomse_visitor.h"
#include "lomse_internal_model.h"
#include "lomse_basic_model_browser.h"


using namespace std;

namespace lomse
{

//forward declarations
class BasicModel;


class ImObjVisitor : public Visitor<ImoObj>
{
public:
    ImObjVisitor() : Visitor<ImoObj>() {}
	virtual ~ImObjVisitor() {}

	virtual void start_visit(ImoObj* pImo) {}
	virtual void end_visit(ImoObj* pImo) {}
};


// BasicModelBrowser: A container for the objects composing the basic model
//----------------------------------------------------------------------------------
class BasicModelBrowser
{
protected:
    ImObjVisitor& m_visitor;

public:
    BasicModelBrowser(ImObjVisitor& visitor);
    ~BasicModelBrowser();

    void browse(ImoObj* pImo);

protected:
    inline void enter(ImoObj* pImo) { pImo->accept_in(m_visitor); }
    inline void leave(ImoObj* pImo) { pImo->accept_out(m_visitor); }

    void browse_score(ImoScore* pScore);
    void browse_instrument();

};


}   //namespace lomse

#endif    // __LOMSE__BASIC_MODEL_BROWSER_H__

