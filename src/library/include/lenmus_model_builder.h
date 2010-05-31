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

#ifndef __LM_MODEL_BUILDER_H__
#define __LM_MODEL_BUILDER_H__

#include <vector>
#include "lenmus_document.h"

using namespace std;

namespace lenmus
{


//-------------------------------------------------------------------------------------
// ModelBuilder. Implements the final step of LDP compiler: code generation.
// Traverses the parse tree and creates the internal model (core tables)
//-------------------------------------------------------------------------------------

class ModelBuilder
{
protected:
    ostream&    m_reporter;
    LdpTree*    m_pTree;
    //std::vector<CoreTableEntry*> m_table;

public:
    ModelBuilder(LdpTree* tree, ostream& reporter);
    ~ModelBuilder();

    void build_model();

protected:

};


}   //namespace lenmus

#endif      //__LM_MODEL_BUILDER_H__
