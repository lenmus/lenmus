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

#ifndef __LML_BASIC_H__
#define __LML_BASIC_H__

#ifndef max
    #define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
    #define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

//internationalization
//#include <libintl.h>
#define _(String) (String)      //gettext(String)
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)


#endif	// __LML_BASIC_H__

