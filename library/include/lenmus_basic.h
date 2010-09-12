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

namespace lenmus
{

#ifndef max
    #define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
    #define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

//internationalization
//#include <libintl.h>
#define TRT(String) (String)      //gettext(String)
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

// class FloatPoint
class FloatPoint
{
public:
    // members are public to simplify.
    float x, y;

    // constructors
    FloatPoint() : x(0.0f), y(0.0f) { }
    FloatPoint(float rx, float ry) : x(rx), y(ry) { }

    // no copy ctor or assignment operator - defaults are ok

    bool operator==(const FloatPoint& pt) const { return x == pt.x && y == pt.y; }
    bool operator!=(const FloatPoint& pt) const { return x != pt.x || y != pt.y; }

    FloatPoint operator+(const FloatPoint& pt) const { return FloatPoint(x + pt.x, y + pt.y); }
    FloatPoint operator-(const FloatPoint& pt) const { return FloatPoint(x - pt.x, y - pt.y); }

    FloatPoint& operator+=(const FloatPoint& pt) { x += pt.x; y += pt.y; return *this; }
    FloatPoint& operator-=(const FloatPoint& pt) { x -= pt.x; y -= pt.y; return *this; }
};

//specific types
typedef float Tenths;       //relative unit: one tenth of staff interline space
typedef FloatPoint TPoint;  //point, in Tenths
typedef unsigned short int16u;

//------------------------------------------------------------------------------
// color type: rgba 16bits.
// I will copy this definition from agg::rgba16. The idea is to
// replace my definition by agg::rgba16 when the graphic layer is developed.
// Remember: alpha 0 means transparent and 255 means opaque
//------------------------------------------------------------------------------

struct rgba16
{
    int16u r;
    int16u g;
    int16u b;
    int16u a;

    enum base_scale_e
    {
        base_shift = 16,
        base_scale = 1 << base_shift,
        base_mask  = base_scale - 1
    };

    rgba16() : r(0), g(0), b(0), a(base_mask) {}

    rgba16(unsigned r_, unsigned g_, unsigned b_, unsigned a_= base_mask) :
        r(int16u(r_)),
        g(int16u(g_)),
        b(int16u(b_)),
        a(int16u(a_)) {}

    rgba16(const rgba16& c, unsigned a_) :
        r(c.r), g(c.g), b(c.b), a(int16u(a_)) {}

    bool operator != (rgba16 color)
    {
        return r != color.r || g != color.g || b != color.b || a != color.a ;
    }

    void clear()
    {
        r = g = b = a = 0;
    }

    const rgba16& transparent()
    {
        a = 0;
        return *this;
    }

    static rgba16 no_color() { return rgba16(0,0,0,0); }



};


}   //namespace lenmus

#endif	// __LML_BASIC_H__

