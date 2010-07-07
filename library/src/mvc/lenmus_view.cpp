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

#include "lenmus_document.h"
#include "lenmus_view.h"

using namespace std;

namespace lenmus
{

//-------------------------------------------------------------------------------------
// View implementation
//-------------------------------------------------------------------------------------

View::View(Document* pDoc)
    : m_pDoc(pDoc)
{
} 

View::~View()
{
}



//-------------------------------------------------------------------------------------
// EditView implementation
//-------------------------------------------------------------------------------------

EditView::EditView(Document* pDoc)
    : View(pDoc)
    , m_cursor(pDoc)
{
} 

EditView::~EditView()
{
}

void EditView::on_document_reloaded()
{
    DocCursor cursor(m_pDoc);
    m_cursor = cursor;
}

//Document::iterator EditView::get_cursor_position()
//{
//    return m_cursor.get_iterator();
//}


}  //namespace lenmus
