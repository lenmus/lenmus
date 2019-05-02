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

#ifndef __LENMUS_TEST_RUNNER_CLASS_H__        //to avoid nested includes
#define __LENMUS_TEST_RUNNER_CLASS_H__

#include <iostream>

#include <UnitTest++.h>
#include <TestReporter.h>



class wxWindow;



//a reporter for using a file stream
namespace UnitTest
{

    class MyTestReporter : public TestReporter
    {
    public:
        MyTestReporter(std::ofstream& outstream);
    private:
        virtual void ReportTestStart(TestDetails const& test);
        virtual void ReportFailure(TestDetails const& test, char const* failure);
        virtual void ReportTestFinish(TestDetails const& test, float secondsElapsed);
        virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);

        std::ofstream&   m_outstream;
    };

}

namespace lenmus
{

//---------------------------------------------------------------------------------------
class MyTestRunner
{

private:
    wxWindow*   m_pParent;
    bool        m_fUseCout;

public:
    MyTestRunner(wxWindow* parent, bool fUseCout=false);

    void RunTests(bool fVerbose=false);
};


}   //namespace lenmus

#endif  // __LENMUS_TEST_RUNNER_CLASS_H__
