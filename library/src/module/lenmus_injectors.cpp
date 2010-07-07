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

#include "lenmus_injectors.h"
#include "lenmus_parser.h"
#include "lenmus_analyser.h"
#include "lenmus_model_builder.h"
#include "lenmus_compiler.h"
#include "lenmus_document.h"

using namespace std;

namespace lenmus
{

LdpParser* Injector::inject_LdpParser(LibraryScope& libraryScope,
                                      DocumentScope& documentScope)
{
    return new LdpParser(documentScope.default_reporter(),
                         libraryScope.ldp_factory());
}

Analyser* Injector::inject_Analyser(LibraryScope& libraryScope,
                                    DocumentScope& documentScope)
{
    return new Analyser(documentScope.default_reporter(), libraryScope.ldp_factory());
}

ModelBuilder* Injector::inject_ModelBuilder(DocumentScope& documentScope)
{
    return new ModelBuilder(documentScope.default_reporter());
}

LdpCompiler* Injector::inject_LdpCompiler(LibraryScope& libraryScope,
                                          DocumentScope& documentScope)
{
    return new LdpCompiler(inject_LdpParser(libraryScope, documentScope),
                           inject_Analyser(libraryScope, documentScope),
                           inject_ModelBuilder(documentScope));
}

Document* Injector::inject_Document(LibraryScope& libraryScope)
{
    DocumentScope documentScope( libraryScope.default_reporter() );
    return new Document(inject_LdpCompiler(libraryScope, documentScope));
}


}  //namespace lenmus
