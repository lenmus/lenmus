//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

#ifndef __LENMUS_STATUS_REPORTER_H__        //to avoid nested includes
#define __LENMUS_STATUS_REPORTER_H__

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lenmus
#include <lomse_basic.h>
using namespace lomse;


namespace lenmus
{

//---------------------------------------------------------------------------------------
//StatusReporter: interface for displying/logging status information
class StatusReporter
{
public:
    StatusReporter() {}
    virtual ~StatusReporter() {}

    //updating fields
    virtual void report_status(const wxString& sText) = 0;
    virtual void report_mouse_data(int nPage, TimeUnits rTime, int nMeasure, UPoint uPos) = 0;
    virtual void report_caret_data(int nPage, TimeUnits rTime, int nMeasure) = 0;
    virtual void report_caret_time(const string& timecode) = 0;
};

//---------------------------------------------------------------------------------------
//DefaultStatusReporter: a nullptr reporter that just discards received data
class DefaultStatusReporter : public StatusReporter
{
public:
    DefaultStatusReporter() : StatusReporter() {}
    virtual ~DefaultStatusReporter() {}

    //updating fields
    void report_status(const wxString& WXUNUSED(sText)) {}
    void report_mouse_data(int WXUNUSED(nPage), TimeUnits WXUNUSED(rTime),
                           int WXUNUSED(nMeasure), UPoint WXUNUSED(uPos)) {}
    void report_caret_data(int WXUNUSED(nPage), TimeUnits WXUNUSED(rTime),
                           int WXUNUSED(nMeasure)) {}
    void report_caret_time(const string& WXUNUSED(timecode)) {}
};


}   //namespace lenmus

#endif    // __LENMUS_STATUS_REPORTER_H__
