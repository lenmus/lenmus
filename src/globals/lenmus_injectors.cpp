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

//lenmus
#include "lenmus_injectors.h"
#include "lenmus_standard_header.h"

#include "lenmus_paths.h"
#include "lenmus_midi_server.h"
#include "lenmus_logger.h"
#include "lenmus_colors.h"
#include "lenmus_status_reporter.h"
#include "lenmus_string.h"
#include "lenmus_version.h"
#include "lenmus_wave_player.h"

//wxWidgets and others
#include <wx/arrstr.h>          //AWARE: Required by wxsqlite3. In Linux GCC complains
                                //       about wxArrayString not defined in wxsqlite3.h
#include <wx/wxsqlite3.h>       //to initialize wxSQLite3 DB


// to save configuration information into a file
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/filename.h>

#include <sstream>

using namespace std;

namespace lenmus
{

//=======================================================================================
// ApplicationScope implementation
//=======================================================================================
ApplicationScope::ApplicationScope(ostream& reporter)
    : m_reporter(reporter)
    , m_pPaths(NULL)            //lazzy instantiation. Singleton scope.
    , m_pPrefs(NULL)            //lazzy instantiation. Singleton scope.
    , m_pMidi(NULL)             //lazzy instantiation. Singleton scope.
    , m_pPlayer(NULL)           //lazzy instantiation. Singleton scope.
    , m_pLomseScope(NULL)
    , m_pLogger(NULL)
    , m_pColors(NULL)
    , m_pMetronome(NULL)
    , m_pStatus( LENMUS_NEW DefaultStatusReporter() )
    , m_pDB(NULL)
    , m_pProxySettings(NULL)
    , m_pWavePlayer(NULL)
    , m_pEditGui(NULL)
    , m_sAppName(_T(LENMUS_APP_NAME))
    , m_sVendorName(_T(LENMUS_VENDOR_NAME))
    , m_fAnswerSoundsEnabled(true)
    , m_fAutoNewProblem(true)
#if (LENMUS_DEBUG_BUILD == 1)
    , m_fReleaseBehaviour(false)
#else
    , m_fReleaseBehaviour(true)
#endif
    , m_fShowDebugLinks(false)
{
    set_version_string();
    initialize_lomse();
}

//---------------------------------------------------------------------------------------
ApplicationScope::~ApplicationScope()
{
    delete m_pPaths;
    delete m_pPlayer;
    delete m_pMidi;     //*AFTER* ScorePlayer, as player can be in use.
    delete m_pLogger;
    delete m_pColors;
    delete m_pStatus;
    delete m_pProxySettings;
    delete m_pWavePlayer;

    //database
    if (m_pDB)
    {
        m_pDB->Close();
        delete m_pDB;
        wxSQLite3Database::ShutdownSQLite();
    }

    //LAST ONE TO DELETE as any previous object could need it to save data
    delete m_pPrefs;
}

//---------------------------------------------------------------------------------------
void ApplicationScope::set_version_string()
{
    //examples: "5.0.a0", "5.0.b2", "5.1", "5.1.2"

    int major = LENMUS_VERSION_MAJOR;
    int minor = LENMUS_VERSION_MINOR;
    int patch = LENMUS_VERSION_PATCH;
    wxString type = _T(LENMUS_VERSION_TYPE);
    if (type.empty() || type == _T(" "))
    {
        if (patch == 0)
            m_sVersionString = wxString::Format(_T("%d.%d"), major, minor);
        else
            m_sVersionString = wxString::Format(_T("%d.%d.%d"), major, minor, patch);
    }
    else
    {
        m_sVersionString = wxString::Format(_T("%d.%d.%s%d"), major, minor,
                                            type.c_str(), patch);
    }
}

//---------------------------------------------------------------------------------------
wxString ApplicationScope::get_app_full_name()
{
    //i.e. "Lenmus Phonascus v5.0 alpha 0"

    wxString name = get_app_name();
    name += _T(" v");

    int major = LENMUS_VERSION_MAJOR;
    int minor = LENMUS_VERSION_MINOR;
    int patch = LENMUS_VERSION_PATCH;
    wxString type = _T(LENMUS_VERSION_TYPE);
    if (type.empty() || type == _T(" "))
    {
        name += get_version_string();
    }
    else
    {
        if (type == _T("a"))
            name += wxString::Format(_T("%d.%d alpha %d"), major, minor, patch);
        else if (type == _T("b"))
            name += wxString::Format(_T("%d.%d beta %d"), major, minor, patch);
        else
            name += get_version_string();
    }
    return name;
}

//---------------------------------------------------------------------------------------
Paths* ApplicationScope::get_paths()
{
    if (!m_pPaths)
        m_pPaths = LENMUS_NEW Paths( wxGetCwd(), *this );
    return m_pPaths;
}

//---------------------------------------------------------------------------------------
Colors* ApplicationScope::get_colors()
{
    if (!m_pColors)
        m_pColors = LENMUS_NEW Colors(*this);
    return m_pColors;
}

//---------------------------------------------------------------------------------------
WavePlayer* ApplicationScope::get_wave_player()
{
    if (!m_pWavePlayer)
        m_pWavePlayer = LENMUS_NEW WavePlayer(*this);
    return m_pWavePlayer;
}

//---------------------------------------------------------------------------------------
void ApplicationScope::set_status_reporter(StatusReporter* reporter)
{
    delete m_pStatus;
    if (reporter)
        m_pStatus = reporter;
    else
        m_pStatus = LENMUS_NEW DefaultStatusReporter();
}

//---------------------------------------------------------------------------------------
void ApplicationScope::initialize_lomse()
{
    // Lomse knows nothing about windows. It renders everything on bitmaps and the
    // user application uses them. For instance, to display it on a wxWindos.
    // Lomse supports a lot of bitmap formats and pixel formats. Therefore, before
    // using the Lomse library you MUST specify which bitmap formap to use.
    //
    // I will use a wxImage as the rendering  buffer. wxImage is platform independent
    // and its buffer is an array of characters in RGBRGBRGB... format,  in the
    // top-to-bottom, left-to-right order. That is, the first RGB triplet corresponds
    // to the first pixel of the first row; the second RGB triplet, to the second
    // pixel of the first row, and so on until the end of the first row,
    // with second row following after it and so on.
    // Therefore, the pixel format is RGB 24 bits.
    //
    // Let's define the requiered information:

        //the pixel format
        int pixel_format = k_pix_format_rgb24;  //RGB 24bits

        //the desired resolution. For Linux and Windows use 96 pixels per inch
        int resolution = 96;    //96 ppi

        //Normal y axis direction is 0 coordinate at top and increase downwards. You
        //must specify if you would like just the opposite behaviour. For Windows and
        //Linux the default behaviour is the right behaviour.
        bool reverse_y_axis = false;

    //redirect cout to my own stream, to intercept and display error msgs.
    m_cout_buffer = cout.rdbuf();
    cout.rdbuf (m_lomseReporter.rdbuf());

    //Now, initialize the library with these values
    m_lomse.init_library(pixel_format, resolution, reverse_y_axis, m_lomseReporter);
}

//---------------------------------------------------------------------------------------
void ApplicationScope::inform_lomse_about_fonts_path()
{
    Paths* pPaths = get_paths();
    wxString sPath = pPaths->GetFontsPath();
    m_lomse.set_default_fonts_path( to_std_string(sPath) );
}

//---------------------------------------------------------------------------------------
MidiServer* ApplicationScope::get_midi_server()
{
    if (!m_pMidi)
        m_pMidi = LENMUS_NEW MidiServer(*this);
    return m_pMidi;
}

//---------------------------------------------------------------------------------------
ScorePlayer* ApplicationScope::get_score_player()
{
    if (!m_pPlayer)
    {
        MidiServer* pMidi = get_midi_server();
        m_pPlayer = m_lomse.create_score_player(pMidi);
        m_pPlayer->post_highlight_events(true);
    }
    return m_pPlayer;
}

//---------------------------------------------------------------------------------------
wxConfigBase* ApplicationScope::get_preferences()
{
    if (!m_pPrefs)
        create_preferences_object();
    return m_pPrefs;
}

//---------------------------------------------------------------------------------------
void ApplicationScope::set_bin_folder(const wxString& sBinPath)
{
    delete m_pPaths;
    m_pPaths = LENMUS_NEW Paths(sBinPath, *this);
}

//---------------------------------------------------------------------------------------
void ApplicationScope::create_preferences_object()
{
    Paths* pPaths = get_paths();
    wxString path = pPaths->GetConfigPath();
    wxFileName filename(path, _T("lenmus"), _T("ini") );

    delete m_pPrefs;
    wxConfigBase::Set((wxConfigBase*) NULL);
    wxFileConfig* pConfig =
        LENMUS_NEW wxFileConfig(_T("lenmus"), _T("LenMus"), filename.GetFullPath(),
                         _T("lenmus"), wxCONFIG_USE_LOCAL_FILE );
    wxConfigBase::Set(pConfig);
    m_pPrefs = wxConfigBase::Get();

    //force writing back the default values just in case they're not present
    pConfig->SetRecordDefaults();
}

//---------------------------------------------------------------------------------------
void ApplicationScope::create_logger()
{
    m_pLogger = LENMUS_NEW Logger();

	// For debugging: send log messages to a file
    wxString sUserId = ::wxGetUserId();
    wxString sLogFile = get_paths()->GetLogPath() + sUserId + _T("_Debug_log.txt");
	wxLog *logger = LENMUS_NEW wxLogStderr( wxFopen(sLogFile.c_str(), _T("w")) );
    LENMUS_NEW wxLogChain(logger);

    // open data log file and re-direct all loging there
    sLogFile = get_paths()->GetLogPath() + sUserId + _T("_DataError_log.txt");
    m_pLogger->SetDataErrorTarget(sLogFile);
}

//---------------------------------------------------------------------------------------
void ApplicationScope::open_database()
{
    //initialize DataBase support and open database
    try
    {
        m_pDB = LENMUS_NEW wxSQLite3Database();
        Paths* pPaths = get_paths();
        wxString path = pPaths->GetConfigPath();
        wxFileName oDBFile(path, _T("lenmus"), _T("db") );
        wxLogMessage(_T("[ApplicationScope::open_database] SQLite3 Version: %s. DB file: '%s'"),
                     m_pDB->GetVersion().c_str(), oDBFile.GetFullPath().c_str() );
        m_pDB->Open(oDBFile.GetFullPath());
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage(_T("Error code: %d, Message: '%s'"),
                    e.GetErrorCode(), e.GetMessage().c_str() );
    }
}

//---------------------------------------------------------------------------------------
ProxySettings* ApplicationScope::get_proxy_settings()
{
    if (!m_pProxySettings)
    {
        m_pProxySettings = LENMUS_NEW ProxySettings();
        wxConfigBase* pPrefs = get_preferences();

        bool fUseProxy;
        pPrefs->Read(_T("/Internet/UseProxy"), &fUseProxy, false);
        m_pProxySettings->fUseProxy = fUseProxy;

        m_pProxySettings->sProxyHostname = pPrefs->Read(_T("/Internet/Hostname"), _T(""));

        long nPort = 0;
        wxString sPort = pPrefs->Read(_T("/Internet/PortNumber"), _T(""));
        if (sPort.IsNumber())
            sPort.ToLong(&nPort);
        m_pProxySettings->nProxyPort = nPort;

        bool fAuthentication;
        pPrefs->Read(_T("/Internet/ProxyAuthentication"), &fAuthentication, false);
        m_pProxySettings->fRequiresAuth = fAuthentication;

        m_pProxySettings->sProxyUsername = pPrefs->Read(_T("/Internet/Username"), _T(""));
        m_pProxySettings->sProxyPassword = pPrefs->Read(_T("/Internet/Password"), _T(""));
    }
    return m_pProxySettings;
}


}  //namespace lenmus
