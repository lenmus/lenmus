//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
//-------------------------------------------------------------------------------------

#ifndef __LM_TESTRUNNER_H__        //to avoid nested includes
#define __LM_TESTRUNNER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TestRunner.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <iostream>

#include <UnitTest++.h>
#include <TestReporter.h>

//a reporter for using a file stream
namespace UnitTest
{

    class lmTestReporter : public TestReporter
    {
    public:
        lmTestReporter(std::ofstream& outstream);
    private:
        virtual void ReportTestStart(TestDetails const& test);
        virtual void ReportFailure(TestDetails const& test, char const* failure);
        virtual void ReportTestFinish(TestDetails const& test, float secondsElapsed);
        virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);

        std::ofstream&   m_outstream;
    };

}

#ifdef __WXDEBUG__

// Debug version: full class, operative
//-------------------------------------------------------------------------------

class lmTestRunner
{
public:
    lmTestRunner(wxWindow* parent);

    void RunTests();

private:
    wxWindow*   m_pParent;

};


#else

//Release version: empty public members, no private members
//-------------------------------------------------------------------------------

class lmTestRunner
{
public:
    lmTestRunner(wxWindow* parent) {}

    void RunTests() {}

};
#endif  //__WXDEBUG__


#endif  // __LM_TESTRUNNER_H__
