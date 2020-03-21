//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2020 LenMus project
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
#include "lenmus_colors.h"
#include "lenmus_status_reporter.h"
#include "lenmus_string.h"
#include "lenmus_version.h"
#include "lenmus_wave_player.h"
#include "lenmus_tool_page.h"       //KeyTranslator
#include "lenmus_help_system.h"
#include "lenmus_actions.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;

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

wxString ApplicationScope::m_language;

//=======================================================================================
// ApplicationScope implementation
//=======================================================================================
ApplicationScope::ApplicationScope(ostream& reporter)
    : m_reporter(reporter)
    , m_pPaths(nullptr)            //lazzy instantiation. Singleton scope.
    , m_pPrefs(nullptr)            //lazzy instantiation. Singleton scope.
    , m_pMidi(nullptr)             //lazzy instantiation. Singleton scope.
    , m_pPlayer(nullptr)           //lazzy instantiation. Singleton scope.
    , m_pLomseScope(nullptr)
    , m_pColors(nullptr)
    , m_pMetronome(nullptr)
    , m_pStatus( LENMUS_NEW DefaultStatusReporter() )
    , m_pDB(nullptr)
    , m_pProxySettings(nullptr)
    , m_pWavePlayer(nullptr)
    , m_pEditGui(nullptr)
    , m_pKeyTranslator(nullptr)
    , m_pHelp(nullptr)
    , m_sAppName(LENMUS_APP_NAME)
    , m_sVendorName(LENMUS_VENDOR_NAME)
    , m_fAnswerSoundsEnabled(true)
    , m_fAutoNewProblem(true)
#if (LENMUS_DEBUG_BUILD == 1)
    , m_fReleaseBehaviour(false)
#else
    , m_fReleaseBehaviour(true)
#endif
    , m_fShowDebugLinks(false)
    , m_fExperimentalFeatures(false)
{
    initialize_lomse();
    m_language = "";
}

//---------------------------------------------------------------------------------------
ApplicationScope::~ApplicationScope()
{
    //restore cout buffer
    cout.rdbuf(m_cout_buffer);

    delete m_pPlayer;
    delete m_pMidi;     //*AFTER* ScorePlayer, as player can be in use.
    delete m_pColors;
    delete m_pStatus;
    delete m_pProxySettings;
    delete m_pWavePlayer;
    delete m_pKeyTranslator;
    delete m_pHelp;
    delete m_pPaths;    //*AFTER* all others, to allow its use in
                        //m_pMidi::save_user_preferences()
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
void ApplicationScope::on_language_changed(wxString lang)
{
    delete m_pHelp;
    m_pHelp = nullptr;
    m_language = lang;
}

//---------------------------------------------------------------------------------------
int ApplicationScope::get_version_major() { return LENMUS_VERSION_MAJOR; }

//---------------------------------------------------------------------------------------
int ApplicationScope::get_version_minor() { return LENMUS_VERSION_MINOR; }

//---------------------------------------------------------------------------------------
wxString ApplicationScope::get_version_patch() { return wxString( LENMUS_VERSION_PATCH ); }

//---------------------------------------------------------------------------------------
wxString ApplicationScope::get_version_string()
{
    //i.e.: "5.1.2"

    return wxString( LENMUS_VERSION );
}

//---------------------------------------------------------------------------------------
wxString ApplicationScope::get_app_full_name()
{
    //i.e.: "Lenmus Phonascus v5.2.1"

    wxString name = get_app_name();
    name += " v";
    name += get_version_string();
    return name;
}

//---------------------------------------------------------------------------------------
wxString ApplicationScope::get_long_version_string()
{
    //i.e.: "5.2.1+a1b2c3d-dirty"

    return wxString( LENMUS_VERSION_LONG );
}

//---------------------------------------------------------------------------------------
wxString ApplicationScope::get_language()
{
    return m_language;
}

//---------------------------------------------------------------------------------------
Paths* ApplicationScope::get_paths()
{
    return m_pPaths;
}

//---------------------------------------------------------------------------------------
void ApplicationScope::set_bin_folder(const wxString& sBinPath)
{
    delete m_pPaths;
    m_sBinPath = sBinPath;
    m_pPaths = LENMUS_NEW Paths(sBinPath, *this);
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
    LOMSE_LOG_INFO("Lomse fonts path set to = %s", sPath.ToStdString().c_str() );
}

//---------------------------------------------------------------------------------------
MidiServer* ApplicationScope::get_midi_server()
{
    if (!m_pMidi)
        m_pMidi = LENMUS_NEW MidiServer(*this);
    return m_pMidi;
}

//---------------------------------------------------------------------------------------
KeyTranslator* ApplicationScope::get_key_translator()
{
    if (!m_pKeyTranslator)
        m_pKeyTranslator = LENMUS_NEW KeyTranslator(*this);
    return m_pKeyTranslator;
}

//---------------------------------------------------------------------------------------
ScorePlayer* ApplicationScope::get_score_player()
{
    if (!m_pPlayer)
    {
        MidiServer* pMidi = get_midi_server();
        m_pPlayer = m_lomse.create_score_player(pMidi->get_current_synth());
        m_pPlayer->post_tracking_events(true);
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
void ApplicationScope::create_preferences_object()
{
    Paths* pPaths = get_paths();
    wxString path = pPaths->GetConfigPath();
    wxFileName filename(path, "lenmus", "ini" );

    delete m_pPrefs;
    wxConfigBase::Set((wxConfigBase*) nullptr);
    wxFileConfig* pConfig =
        LENMUS_NEW wxFileConfig("lenmus", "LenMus", filename.GetFullPath(),
                         "lenmus", wxCONFIG_USE_LOCAL_FILE );
    wxConfigBase::Set(pConfig);
    m_pPrefs = wxConfigBase::Get();

    //force writing back the default values just in case they're not present
    pConfig->SetRecordDefaults();
}

//---------------------------------------------------------------------------------------
void ApplicationScope::create_logger()
{
    //AWARE:
    // - Macro LOMSE_LOG_DEBUG will only work if option LOMSE_ENABLE_DEBUG_LOGS is
    //   set in lomse_config.h.
    // - Macro LOMSE_LOG_INFO always work.
    // - Both write logs to lomse-log.txt

    logger.set_logging_mode(Logger::k_normal_mode); //k_normal_mode k_debug_mode k_trace_mode
    logger.set_logging_areas(Logger::k_score_player);   //k_events); //k_layout); //k_all  k_mvc | );

	// For debugging: send wxWidgets log messages to a file
    wxString sUserId = ::wxGetUserId();
    wxString sLogFile = get_paths()->GetLogPath() + sUserId + "-lenmus-log.txt";
	wxLog *lmLogger = LENMUS_NEW wxLogStderr( wxFopen(sLogFile.wx_str(), "w") );
	wxLog::SetActiveTarget(lmLogger);
	wxLogMessage("[ApplicationScope::create_logger] INFO: Log messages derived to file.");
	//test----------
	const string file(__FILE__);
	int line(__LINE__);
	const string prettyFunction(__PRETTY_FUNCTION__);
	wxLogMessage("%s, %d, %s INFO: Log messages derived to file.", file, line, prettyFunction);
	//--------------

    LOMSE_LOG_INFO("lenmus log file = [%s]", sLogFile.ToStdString().c_str() );
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
        wxFileName oDBFile(path, "lenmus", "db" );
        LOMSE_LOG_INFO("SQLite3 Version: %s. DB file: '%s'",
                       m_pDB->GetVersion().ToStdString().c_str(),
                       oDBFile.GetFullPath().ToStdString().c_str() );
        m_pDB->Open(oDBFile.GetFullPath());
    }
    catch (wxSQLite3Exception& e)
    {
       LOMSE_LOG_ERROR("Error code: %d, Message: '%s'",
                       e.GetErrorCode(), e.GetMessage().ToStdString().c_str() );
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
        pPrefs->Read("/Internet/UseProxy", &fUseProxy, false);
        m_pProxySettings->fUseProxy = fUseProxy;

        m_pProxySettings->sProxyHostname = pPrefs->Read("/Internet/Hostname", "");

        long nPort = 0;
        wxString sPort = pPrefs->Read("/Internet/PortNumber", "");
        if (sPort.IsNumber())
            sPort.ToLong(&nPort);
        m_pProxySettings->nProxyPort = nPort;

        bool fAuthentication;
        pPrefs->Read("/Internet/ProxyAuthentication", &fAuthentication, false);
        m_pProxySettings->fRequiresAuth = fAuthentication;

        m_pProxySettings->sProxyUsername = pPrefs->Read("/Internet/Username", "");
        m_pProxySettings->sProxyPassword = pPrefs->Read("/Internet/Password", "");
    }
    return m_pProxySettings;
}

//---------------------------------------------------------------------------------------
HelpSystem* ApplicationScope::get_help_controller()
{
    return m_pHelp;
}

//---------------------------------------------------------------------------------------
void ApplicationScope::initialize_help(wxWindow* pParent)
{
    //TODO: Un-comment when the Help system is again needed
//    m_pHelp = LENMUS_NEW HelpSystem(pParent, *this);
//    m_pHelp->initialize();
}


}  //namespace lenmus
