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

#ifndef __LML_BASIC_MODEL_H__        //to avoid nested includes
#define __LML_BASIC_MODEL_H__

#include <list>

using namespace std;

namespace lenmus
{

//forward declarations
class ImoObj;
class ImoBeam;
class ImoTuplet;
class ImoTie;


// BasicModel: A container for the objects composing the basic model
//----------------------------------------------------------------------------------
class BasicModel
{
protected:
    ImoObj* m_pRoot;
    std::list<ImoBeam*> m_beams;
    std::list<ImoTuplet*> m_tuplets;
    std::list<ImoTie*> m_ties;

public:
    BasicModel();
    ~BasicModel();

    //building the model
    inline void set_root(ImoObj* pRoot) { m_pRoot = pRoot; }
    inline void add_beam(ImoBeam* pBeam) { m_beams.push_back(pBeam); }
    inline void add_tuplet(ImoTuplet* pTuplet) { m_tuplets.push_back(pTuplet); }
    inline void add_tie(ImoTie* pTie) { m_ties.push_back(pTie); }

    //getters
    inline ImoObj* get_root() { return m_pRoot; }
    inline std::list<ImoBeam*>& get_beams() { return m_beams; }
    inline std::list<ImoTuplet*>& get_tuplets() { return m_tuplets; }
    inline std::list<ImoTie*>& get_ties() { return m_ties; }

protected:
    void delete_beams();
    void delete_tuplets();
    void delete_ties();

};


}   //namespace lenmus

#endif    // __LML_BASIC_MODEL_H__

