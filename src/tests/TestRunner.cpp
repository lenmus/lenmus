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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TestRunner.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "cppunit.h"
#include "TestRunner.h"

//emptry file unless debug version
#ifdef __WXDEBUG__

#include <iostream>
#include <fstream>

using CppUnit::Test;
using CppUnit::TestSuite;
using CppUnit::TestFactoryRegistry;
using CppUnit::TextUi::TestRunner;
using CppUnit::CompilerOutputter;

using std::string;
using std::vector;
using std::auto_ptr;
using std::cout;
using std::ofstream;
using std::ostream;

#include <wx/ffile.h>

#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>


#include "../app/DlgDebug.h"


//Old unit tests
extern bool lmChordUnitTests();
extern bool lmFiguredBassUnitTests();



//---------------------------------------------------------------------------------------------------
// lmTestRunner implementation
//
// lmTestRunner is the component which orchestrates the execution of tests and provides
// the outcome to the user.
//---------------------------------------------------------------------------------------------------

lmTestRunner::lmTestRunner(wxWindow* parent)
    : m_pParent(parent)
{
}

void lmTestRunner::List(Test *test, ostream& outstream, const string& parent /*=""*/) const
{
    TestSuite *suite = dynamic_cast<TestSuite*>(test);
    string name;

    if (suite) {
        // take the last component of the name and append to the parent
        name = test->getName();
        string::size_type i = name.find_last_of(".:");
        if (i != string::npos)
            name = name.substr(i + 1);
        name = parent + "." + name;

        // drop the 1st component from the display and indent
        if (parent != "") {
            string::size_type j = i = name.find('.', 1);
            while ((j = name.find('.', j + 1)) != string::npos)
                outstream << "  ";
            outstream << "  " << name.substr(i + 1) << "\n";
        }

        typedef vector<Test*> Tests;
        typedef Tests::const_iterator Iter;

        const Tests& tests = suite->getTests();

        for (Iter it = tests.begin(); it != tests.end(); ++it)
            List(*it, outstream, name);
    }
    else {  // if (m_longlist) {
        string::size_type i = 0;
        while ((i = parent.find('.', i + 1)) != string::npos)
            outstream << "  ";
        outstream << "  " << test->getName() << "\n";
    }
}

void lmTestRunner::RunTests()
{
    //main method to orchestrate the execution of tests and provide the 
    //outcome to the user.

    //prepare file to save unit tests results
    ofstream outdata;
    outdata.open("UnitTests-results.txt");
    if( !outdata )
        wxLogMessage(_T("Error: 'UnitTests-results.txt' could not be opened" ));

    //headers: running date and time
    outdata << "LenMus tests runner. "
            << std::string((wxDateTime::Now()).Format(_T("%Y/%m/%d %H:%M:%S\n")).mb_str(*wxConvCurrent) );

    //create the event manager and test controller
    CppUnit::TestResult controller;

    //add a listener that colllects test results
    CppUnit::TestResultCollector result;
    controller.addListener( &result );        

    //add a listener that print dots as tests run
    CppUnit::BriefTestProgressListener progress;
    controller.addListener( &progress );      

    //add the top suite to the test runner
    CppUnit::TestRunner runner;
    auto_ptr<Test> test( TestFactoryRegistry::getRegistry().makeTest() );
    TestSuite *suite = dynamic_cast<TestSuite*>(test.get());

    if (suite && suite->countTestCases() == 0)
            wxLogError(_T("No test suite"));
    else
    {
        List(test.get(), outdata);
        runner.addTest(test.release());
    }

    //Run the tests
    wxStopWatch oTimer;
    runner.run( controller );

    //Print results in a compiler compatible format.
    CppUnit::CompilerOutputter outputter( &result, outdata );
    outputter.write();                      

    //old tests not yet refactored
    outdata << std::endl << "Old tests follows ----------------------\n" << std::endl;

    if (!lmChordUnitTests())
        outdata << "Test failure in lmChordUnitTests\n";

    if (!lmFiguredBassUnitTests())
        outdata << "Test failure in lmFiguredBassUnitTests\n";

    //write elapsed time to do tests
    oTimer.Pause();
    outdata << "\nTests executed in " << oTimer.Time() << " ms";

    outdata.flush();

    //show results
    wxString sFileContent;
    wxFFile file(_T("UnitTests-results.txt"));
    if ( file.IsOpened() && file.ReadAll(&sFileContent) )
    {
        lmDlgDebug dlg(m_pParent, _T("Unit tests results"), sFileContent, false);     //false: no 'Save' button
        dlg.ShowModal();
    }

    //return result.wasSuccessful() ? 0 : 1;
}

#endif
