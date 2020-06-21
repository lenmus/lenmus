//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_DYNCONTROL_H__
#define __LENMUS_DYNCONTROL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//lomse
#include <lomse_internal_model.h>
#include <lomse_events.h>
#include <lomse_control.h>
using namespace lomse;

//other
#include <string>
using namespace std;

namespace lenmus
{

//forward declarations
class DynControl;
class DocumentWindow;


//---------------------------------------------------------------------------------------
// factory class to create DynControl objects
class DynControlFactory
{
public:
    DynControlFactory() {}
    virtual ~DynControlFactory() {}

    static DynControl* create_dyncontrol(ApplicationScope& appScope,
                                         const string& classid,
                                         DocumentWindow* pCanvas);

protected:
    static long new_id() {
        static long dynId = 0L;
        return ++dynId;
    }

};


//---------------------------------------------------------------------------------------
// Abstract class from which all dynamic content generators/controllers must derive
class DynControl
{
protected:
    long m_dynId;
    ApplicationScope& m_appScope;

public:
    virtual ~DynControl() {}

    virtual void generate_content(ADynamic dyn, ADocument doc) = 0;

    inline long get_dynid() { return m_dynId; }

protected:
    DynControl(long dynId, ApplicationScope& appScope)
        : m_dynId(dynId)
        , m_appScope(appScope)
    {}

};


//---------------------------------------------------------------------------------------
// A dummy control to use when dyncontrol factory fails
class DummyControl : public DynControl
{
protected:
    string m_classid;

public:
    DummyControl(long dynId, ApplicationScope& appScope, const string& classid);
    ~DummyControl() {};

    void generate_content(ADynamic dyn, ADocument doc);

};

}   //namespace lenmus

#endif      //__LENMUS_DYNCONTROL_H__
