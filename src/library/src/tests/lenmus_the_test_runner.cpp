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

#include <iostream>
#include <UnitTest++.h>

#if defined WIN32 || defined _WIN32
    //for detecting and isolating memory leaks with Visual C++
    #ifndef _DEBUG
        #define _DEBUG
    #endif
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

using namespace std;


int main()
{
    cout << "LenMus Library tests runner" << endl << endl;
    int nErrors = UnitTest::RunAllTests();

    #if defined WIN32 || defined _WIN32
        //system("pause");
        cin.get();
        _CrtDumpMemoryLeaks();

    #endif

    return nErrors;
}
