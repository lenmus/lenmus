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

#ifndef __LM_INJECTORS_H__
#define __LM_INJECTORS_H__

#include <iostream>
#include "lenmus_factory.h"

using namespace std;

namespace lenmus
{

//forward declarations
class LdpParser;
class Analyser;
class ModelBuilder;
class LdpCompiler;
class Document;
class LdpFactory;


//-----------------------------------------------------------------------------------
class LibraryScope
{
public:
    LibraryScope(ostream& reporter=cout)
        : m_reporter(reporter)
        , m_pLdpFactory(NULL)       //lazzy instantiation. Singleton scope.
    {
    }

    ~LibraryScope()
    {
        if (m_pLdpFactory)
            delete m_pLdpFactory;
    }

    ostream& default_reporter() { return m_reporter; }
    LdpFactory* ldp_factory()   //Singleton scope
    {
        if (!m_pLdpFactory)
            m_pLdpFactory = new LdpFactory();
        return m_pLdpFactory;
    }

protected:
    ostream& m_reporter;
    LdpFactory* m_pLdpFactory;

};

//-----------------------------------------------------------------------------------
class DocumentScope
{
public:
    DocumentScope(ostream& reporter=cout) : m_reporter(reporter) {}
    ~DocumentScope() {}

    ostream& default_reporter() { return m_reporter; }

protected:
    ostream& m_reporter;

};

//-----------------------------------------------------------------------------------
class Injector
{
public:
    Injector() {}
    ~Injector() {}

    static LdpParser* inject_LdpParser(LibraryScope& libraryScope,
                                       DocumentScope& documentScope);
    static Analyser* inject_Analyser(LibraryScope& libraryScope,
                                     DocumentScope& documentScope);
    static ModelBuilder* inject_ModelBuilder(DocumentScope& documentScope);
    static LdpCompiler* inject_LdpCompiler(LibraryScope& libraryScope,
                                           DocumentScope& documentScope);
    static Document* inject_Document(LibraryScope& libraryScope);

};



}   //namespace lenmus

#endif      //__LM_INJECTORS_H__
