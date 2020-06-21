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

#include <UnitTest++.h>
#include <sstream>
#include "lenmus_config.h"

//classes related to these tests
#include "lenmus_injectors.h"
#include "lenmus_paths.h"
#include "lenmus_string.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


class AppTestFixture
{
public:
    wxConfigBase* m_pSavePrefs;
    ApplicationScope m_appScope;
    std::string m_scores_path;

    AppTestFixture()     //SetUp fixture
        //: m_pSavePrefs( wxConfigBase::Get() )
        : m_appScope(cout)
    {

        m_scores_path = LENMUS_TEST_SCORES_PATH;
    }

    ~AppTestFixture()    //TearDown fixture
    {
        //wxConfigBase::Set(m_pSavePrefs);
    }
};

SUITE(AppTest)
{

    TEST_FIXTURE(AppTestFixture, AppScope_AppNames)
    {
        CHECK( m_appScope.get_app_name() == "Lenmus Phonascus" );
        CHECK( m_appScope.get_vendor_name() == "Lenmus" );
    }

    //TEST_FIXTURE(AppTestFixture, AppScope_GetPathDefault)
    //{
    //    #if (LENMUS_PLATFORM_WIN32 == 1)
    //        m_appScope.set_bin_folder(_T("c:\\lenmus\\bin\\"));
    //        Paths* pPaths = m_appScope.get_paths();
    //        CHECK( pPaths != nullptr );

    //        CHECK( pPaths->GetSrcRootPath() == _T("c:\\lenmus\\") );
    //        CHECK( pPaths->GetBinPath() == _T("c:\\lenmus\\bin\\") );
    //        CHECK( pPaths->GetXrcPath() == _T("c:\\lenmus\\xrc\\") );
    //        CHECK( pPaths->GetTemporaryPath() == _T("c:\\lenmus\    emp\\") );
    //        CHECK( pPaths->GetImagePath() == _T("c:\\lenmus\\res\\icons\\") );
    //        CHECK( pPaths->GetCursorsPath() == _T("c:\\lenmus\\res\\cursors\\") );
    //        CHECK( pPaths->GetSoundsPath() == _T("c:\\lenmus\\res\\sounds\\") );
    //        CHECK( pPaths->GetLocaleRootPath() == _T("c:\\lenmus\\locale\\") );
    //        CHECK( pPaths->GetScoresPath() == _T("c:\\lenmus\\scores\\") );
    //        CHECK( pPaths->GetTestScoresPath() == _T("c:\\lenmus\    est-scores\\") );
    //        CHECK( pPaths->GetSamplesPath() == _T("c:\\lenmus\\scores\\samples\\") );
    //        CHECK( pPaths->GetTemplatesPath() == _T("c:\\lenmus\    emplates\\") );
    //        #if (LENMUS_DEBUG_BUILD == 1)
    //            CHECK( pPaths->GetConfigPath() == _T("c:\\lenmus\\z_bin\\") );
    //        #else
    //            CHECK( pPaths->GetConfigPath() == _T("c:\\lenmus\\bin\\") );
    //        #endif
    //        CHECK( pPaths->GetLogPath() == _T("c:\\lenmus\\logs\\") );
    //        CHECK( pPaths->GetFontsPath() == _T("c:\\lenmus\\res\\fonts\\") );

    //    #else if (LENMUS_PLATFORM_UNIX == 1)
    //        m_appScope.set_bin_folder("/home/x/lenmus/");
    //        Paths* pPaths = m_appScope.get_paths();
    //        CHECK( pPaths != nullptr );

    //        CHECK( pPaths->GetSrcRootPath() == "../../../lenmus/" );
    //        CHECK( pPaths->GetBinPath() == "/home/x/lenmus/" );
    //        CHECK( pPaths->GetXrcPath() == "../../../lenmus/xrc/" );
    //        CHECK( pPaths->GetTemporaryPath() == "../../../lenmus/temp/" );
    //        CHECK( pPaths->GetImagePath() == "../../../lenmus/res/icons/" );
    //        CHECK( pPaths->GetCursorsPath() == "../../../lenmus/res/cursors/" );
    //        CHECK( pPaths->GetSoundsPath() == "../../../lenmus/res/sounds/" );
    //        CHECK( pPaths->GetLocaleRootPath() == "../../../lenmus/locale/" );
    //        CHECK( pPaths->GetScoresPath() == "../../../lenmus/scores/" );
    //        CHECK( pPaths->GetTestScoresPath() == "../../../lenmus/test-scores/" );
    //        CHECK( pPaths->GetSamplesPath() == "../../../lenmus/scores/samples/" );
    //        CHECK( pPaths->GetTemplatesPath() == "../../../lenmus/templates/" );
    //        CHECK( pPaths->GetConfigPath() == "../../../lenmus/" );
    //        CHECK( pPaths->GetLogPath() == "../../../lenmus/logs/" );
    //        CHECK( pPaths->GetFontsPath() == "../../../lenmus/res/fonts/" );
    //    #endif
    //    cout << "GetSrcRootPath='" << to_std_string(pPaths->GetSrcRootPath()) << "'" << endl;
    //    cout << "GetBinPath='" << to_std_string(pPaths->GetBinPath()) << "'" << endl;
    //    cout << "GetXrcPath='" << to_std_string(pPaths->GetXrcPath()) << "'" << endl;
    //    cout << "GetTemporaryPath='" << to_std_string(pPaths->GetTemporaryPath()) << "'" << endl;
    //    cout << "GetImagePath='" << to_std_string(pPaths->GetImagePath()) << "'" << endl;
    //    cout << "GetCursorsPath='" << to_std_string(pPaths->GetCursorsPath()) << "'" << endl;
    //    cout << "GetSoundsPath='" << to_std_string(pPaths->GetSoundsPath()) << "'" << endl;
    //    cout << "GetLocaleRootPath='" << to_std_string(pPaths->GetLocaleRootPath()) << "'" << endl;
    //    cout << "GetScoresPath='" << to_std_string(pPaths->GetScoresPath()) << "'" << endl;
    //    cout << "GetTestScoresPath='" << to_std_string(pPaths->GetTestScoresPath()) << "'" << endl;
    //    cout << "GetSamplesPath='" << to_std_string(pPaths->GetSamplesPath()) << "'" << endl;
    //    cout << "GetTemplatesPath='" << to_std_string(pPaths->GetTemplatesPath()) << "'" << endl;
    //    cout << "GetConfigPath='" << to_std_string(pPaths->GetConfigPath()) << "'" << endl;
    //    cout << "GetLogPath='" << to_std_string(pPaths->GetLogPath()) << "'" << endl;
    //    cout << "GetFontsPath='" << to_std_string(pPaths->GetFontsPath()) << "'" << endl;
    //}

    TEST_FIXTURE(AppTestFixture, AppScope_GetMidiServer)
    {
        MidiServer* pMidi = m_appScope.get_midi_server();
        CHECK( pMidi != nullptr );
    }

};

