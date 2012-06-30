//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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

#include "lenmus_test_runner.h"


#include <iostream>
#include <fstream>
#include <streambuf>

using std::ofstream;
using std::ostream;
using std::streambuf;
using std::cout;
using namespace std;

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/ffile.h>

#include "lenmus_dlg_debug.h"

#include <UnitTest++.h>


namespace lenmus
{

//=======================================================================================
// MyTestRunner implementation
//
// MyTestRunner is the component which orchestrates the execution of tests and provides
// the outcome to the user.
//=======================================================================================

MyTestRunner::MyTestRunner(wxWindow* parent)
    : m_pParent(parent)
{
}

void MyTestRunner::RunTests()
{
    //main method to orchestrate the execution of tests and provide the
    //outcome to the user.

    //prepare file to save unit tests results
    ofstream outdata;
    outdata.open("UnitTests-results.txt");
    if( !outdata )
        wxLogMessage(_T("Error: 'UnitTests-results.txt' could not be opened" ));

    //redirect cout to my stream
    streambuf* cout_buffer = cout.rdbuf();
    cout.rdbuf (outdata.rdbuf());

    //headers: running date and time
    outdata << "Lenmus tests runner. "
            << std::string((wxDateTime::Now()).Format(_T("%Y/%m/%d %H:%M:%S\n\n")).mb_str(*wxConvCurrent) );

    //Run the tests
    using namespace UnitTest;
    MyTestReporter reporter(outdata);
    TestRunner runner(reporter);
	runner.RunTestsIf(Test::GetTestList(), NULL, True(), 0);

    outdata.flush();

    //restore old cout buffer
    cout.rdbuf (cout_buffer);

    //show results
    wxString sFileContent;
    wxFFile file(_T("UnitTests-results.txt"));
    if ( file.IsOpened() && file.ReadAll(&sFileContent) )
    {
        DlgDebug dlg(m_pParent, _T("Unit tests results"), sFileContent, false);     //false: no 'Save' button
        dlg.ShowModal();
    }
}

}  //namespace lenmus


//=======================================================================================
//a reporter for using a file stream
namespace UnitTest {

MyTestReporter::MyTestReporter(std::ofstream& outstream)
    : TestReporter()
    , m_outstream(outstream)
{
}

void MyTestReporter::ReportFailure(TestDetails const& details, char const* failure)
{
#if defined(__APPLE__) || defined(__GNUG__)
    //errorFormat = "%s:%d: error: Failure in %s: %s\n";
    m_outstream << details.filename << ":" << details.lineNumber <<
        ": error: Failure in " << details.testName << ": " << failure << "\n";
#else
    //errorFormat = "%s(%d): error: Failure in %s: %s\n";
    m_outstream << details.filename << "(" << details.lineNumber <<
        "): error: Failure in " << details.testName << ": " << failure << "\n";
#endif
}

void MyTestReporter::ReportTestStart(TestDetails const& /*test*/)
{
}

void MyTestReporter::ReportTestFinish(TestDetails const& /*test*/, float)
{
}

void MyTestReporter::ReportSummary(int const totalTestCount, int const failedTestCount,
                                 int const failureCount, float secondsElapsed)
{
    if (failureCount > 0)
        m_outstream << "FAILURE: " << failedTestCount << " out of " << totalTestCount
            << " tests failed (" << failureCount << " failures).\n";
    else
        m_outstream << "Success: " << totalTestCount << " tests passed.\n";

    m_outstream << "Test time: " << secondsElapsed << " seconds.\n";
}

}  //namespace UnitTest

