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

#include <sstream>
#include <algorithm>

#include "lomse_injectors.h"
#include "lomse_document.h"
#include "lomse_user_command.h"
#include "lomse_model_builder.h"

using namespace std;

namespace lomse
{

////------------------------------------------------------------------
//// UserCommandExecuter
////------------------------------------------------------------------
//
//UserCommandExecuter::UserCommandExecuter(Document* pDoc)
//    : m_pDoc(pDoc)
//    , m_docCommandExecuter(pDoc)
//{
//    DocumentScope* pDocScope = m_pDoc->get_scope();
//    m_pModelBuilder = Injector::inject_ModelBuilder(*pDocScope);
//}
//
//UserCommandExecuter::UserCommandExecuter(Document* pDoc, ModelBuilder* pBuilder)
//    : m_pDoc(pDoc)
//    , m_docCommandExecuter(pDoc)
//    , m_pModelBuilder(pBuilder)
//{
//    //dependency injection constructor, only for unit testing
//}
//
//UserCommandExecuter::~UserCommandExecuter()
//{
//    delete m_pModelBuilder;
//}
//
//void UserCommandExecuter::execute(UserCommand& cmd)
//{
//    UserCommandData* data
//      = new UserCommandData(cmd.get_name(),
//                            m_docCommandExecuter.is_document_modified(),
//                            static_cast<int>(m_docCommandExecuter.undo_stack_size()) );
//    m_stack.push(data);
//    cmd.do_actions(&m_docCommandExecuter);
//    data->set_end_pos( static_cast<int>(m_docCommandExecuter.undo_stack_size()) );
//    update_model();
//}
//
//void UserCommandExecuter::undo()
//{
//    UserCommandData* data = m_stack.pop();
//    for (int i=0; i < data->get_num_actions(); ++i)
//      m_docCommandExecuter.undo();
//    update_model();
//}
//
//void UserCommandExecuter::redo()
//{
//    UserCommandData* data = m_stack.undo_pop();
//    for (int i=0; i < data->get_num_actions(); ++i)
//      m_docCommandExecuter.redo();
//    update_model();
//}
//
//void UserCommandExecuter::update_model()
//{
//    //m_pModelBuilder->update_model(m_pDoc->get_tree());
//    //m_pDoc->notify_that_document_has_been_modified();
//}


}  //namespace lomse
