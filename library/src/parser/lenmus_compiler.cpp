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

#include "lenmus_compiler.h"
#include "lenmus_parser.h"
#include "lenmus_analyser.h"
#include "lenmus_model_builder.h"
#include "lenmus_injectors.h"

using namespace std;

namespace lenmus
{

//------------------------------------------------------------------
// LdpCompiler implementation
//------------------------------------------------------------------

LdpCompiler::LdpCompiler(LdpParser* p, Analyser* a, ModelBuilder* mb) 
    : m_pParser(p)
    , m_pAnalyser(a)
    , m_pModelBuilder(mb)
{
}

LdpCompiler::LdpCompiler(LibraryScope& libraryScope, DocumentScope& documentScope) 
    : m_pParser( Injector::inject_LdpParser(libraryScope, documentScope) )
    , m_pAnalyser( Injector::inject_Analyser(libraryScope, documentScope) )
    , m_pModelBuilder( Injector::inject_ModelBuilder(documentScope) )
{
}

LdpCompiler::~LdpCompiler()
{
    delete m_pParser;
    delete m_pAnalyser;
    delete m_pModelBuilder;
}

LdpTree* LdpCompiler::compile_file(const std::string& filename)
{
    LdpTree* pParseTree = m_pParser->parse_file(filename);
    return compile(pParseTree);
}

LdpTree* LdpCompiler::compile_string(const std::string& source)
{
    LdpTree* pParseTree = m_pParser->parse_text(source);
    return compile(pParseTree);
}

LdpTree* LdpCompiler::create_empty()
{
    LdpTree* pParseTree = parse_empty_doc();
    return compile(pParseTree);
}

LdpTree* LdpCompiler::create_with_empty_score()
{
    LdpTree* pParseTree = m_pParser->parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6))))");
    return compile(pParseTree);
}

LdpTree* LdpCompiler::compile(LdpTree* pParseTree)
{
    LdpTree* pFinalTree;
    if (pParseTree->get_root()->is_type(k_score))
    {
        pFinalTree = wrap_score_in_lenmusdoc(pParseTree);
        delete pParseTree;
    }
    else
        pFinalTree = pParseTree;

    m_pAnalyser->analyse_tree(pFinalTree);
    m_pModelBuilder->build_model(pFinalTree);
    return pFinalTree;
}

LdpTree* LdpCompiler::wrap_score_in_lenmusdoc(LdpTree* pParseTree)
{
    LdpTree* pFinalTree = parse_empty_doc();
    LdpTree::depth_first_iterator it = pFinalTree->begin();
    while (it != pFinalTree->end() && !(*it)->is_type(k_content))
        ++it;
    long nContentID = (*it)->get_id();
    long nScoreNewID = ++nContentID;
    fix_score_ids(pParseTree, nScoreNewID);
    (*it)->append_child(pParseTree->get_root());
    return pFinalTree;
}

void LdpCompiler::fix_score_ids(LdpTree* pTree, long nNewFirstID)
{
    LdpTree::depth_first_iterator it = pTree->begin();
    for (it = pTree->begin(); it != pTree->end(); ++it)
    {
        if (!(*it)->is_simple())
            (*it)->set_id( (*it)->get_id() + nNewFirstID );
    }
}

LdpTree* LdpCompiler::parse_empty_doc()
{
    return m_pParser->parse_text("(lenmusdoc (vers 0.0) (content ))");
}

int LdpCompiler::get_num_errors()
{
    return m_pParser->get_num_errors();
}


}  //namespace lenmus
