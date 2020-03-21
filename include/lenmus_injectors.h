//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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

#ifndef __LENMUS_INJECTORS_H__
#define __LENMUS_INJECTORS_H__

// define this to 1 to use external help controller. 0 for wxHtmlHelpController (htb format)
#define LENMUS_USE_EXT_HELP 1


//lenmus
#include "lenmus_standard_header.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_score_player.h>
#include <lomse_metronome.h>
using namespace lomse;

//wxWidgets
//#define system ::system         //bypass for bug in wxcrtbase.h: "reference to 'system' is ambiguous"
//#include <wx/wxprec.h>
#include <wx/string.h>
#include <wx/config.h>
#if LENMUS_USE_EXT_HELP
#include "wx/generic/helpext.h"
#else
#include <wx/html/helpctrl.h>
#endif
#undef system                   //bypass for bug in wxcrtbase.h: "reference to 'system' is ambiguous"

#include <iostream>
#include <sstream>
using namespace std;

class wxSQLite3Database;

namespace lenmus
{

//forward declarations
class Paths;
class MidiServer;
//class Logger;
class Colors;
class StatusReporter;
class WavePlayer;
class EditInterface;
class KeyTranslator;
class HelpSystem;
class KeyTranslator;

//---------------------------------------------------------------------------------------
struct ProxySettings
{
    ProxySettings()
        : sProxyHostname("")
        , sProxyUsername("")
        , sProxyPassword("")
        , fUseProxy(false)
        , fRequiresAuth(false)
        , nProxyPort(0)
    {
    }

	wxString sProxyHostname;
	wxString sProxyUsername;
	wxString sProxyPassword;
	bool fUseProxy;
    bool fRequiresAuth;
	int nProxyPort;
};


//---------------------------------------------------------------------------------------
class ApplicationScope
{
protected:
    ostream& m_reporter;
    LomseDoorway m_lomse;
    Paths* m_pPaths;
    wxConfigBase* m_pPrefs;
    MidiServer* m_pMidi;
    ScorePlayer* m_pPlayer;
    LibraryScope* m_pLomseScope;
    Colors* m_pColors;
    Metronome* m_pMetronome;
    StatusReporter* m_pStatus;
    wxSQLite3Database* m_pDB;
    ProxySettings* m_pProxySettings;
    WavePlayer* m_pWavePlayer;
    EditInterface* m_pEditGui;
    KeyTranslator* m_pKeyTranslator;
    HelpSystem* m_pHelp;

    wxString m_sAppName;
    wxString m_sVendorName;
    wxString m_sBinPath;

    ostringstream   m_lomseReporter;    //to have access to error messages
    streambuf*      m_cout_buffer;      //to restore cout

    //some global options
    bool m_fAnswerSoundsEnabled;
    bool m_fAutoNewProblem;
    bool m_fReleaseBehaviour;
    bool m_fShowDebugLinks;
    bool m_fExperimentalFeatures;
    int m_trackingMode;
    LUnits m_tempoLineWidth;

    static wxString m_language;

public:
    ApplicationScope(ostream& reporter=cout);
    ~ApplicationScope();


    //settings
    void set_bin_folder(const wxString& sBinPath);
    void create_preferences_object();
    void create_logger();
    void open_database();
    void set_status_reporter(StatusReporter* reporter);
    void inform_lomse_about_fonts_path();
    inline void set_metronome(Metronome* pMtr) { m_pMetronome = pMtr; }
    inline void set_edit_gui(EditInterface* pGui) { m_pEditGui = pGui; }
    void initialize_help(wxWindow* pParent);
    void on_language_changed(wxString lang);

    //access to global objects/variables
    Paths* get_paths();
    wxConfigBase* get_preferences();
    MidiServer* get_midi_server();
    ScorePlayer* get_score_player();
    Colors* get_colors();
    inline StatusReporter* get_status_reporter() { return m_pStatus; }
    inline wxSQLite3Database* get_database() { return m_pDB; }
    ProxySettings* get_proxy_settings();
    inline Metronome* get_metronome() { return m_pMetronome; }
    WavePlayer* get_wave_player();
    inline EditInterface* get_edit_gui() { return m_pEditGui; }
    KeyTranslator* get_key_translator();
    HelpSystem* get_help_controller();

//    inline ostream& default_reporter() { return m_reporter; }
    inline LomseDoorway& get_lomse() { return m_lomse; }
    inline ostringstream& get_lomse_reporter() { return m_lomseReporter; }

    //app info
    inline wxString& get_app_name() { return m_sAppName; }
    inline wxString& get_vendor_name() { return m_sVendorName; }
    static int get_version_major();
    static int get_version_minor();
    static wxString get_version_patch();
    wxString get_version_string();
    wxString get_long_version_string();
    wxString get_app_full_name();
    static wxString get_language();

    //global options
    inline bool are_answer_sounds_enabled() { return m_fAnswerSoundsEnabled; }
    inline void enable_answer_sounds(bool value) { m_fAnswerSoundsEnabled = value; }
    inline bool is_auto_new_problem_enabled() { return m_fAutoNewProblem; }
    inline void enable_auto_new_problem(bool value) { m_fAutoNewProblem = value; }
    inline void force_release_behaviour(bool value) { m_fReleaseBehaviour = value; }
    inline bool is_release_behaviour() { return m_fReleaseBehaviour; }
    inline void set_show_debug_links(bool value) { m_fShowDebugLinks = value; }
    inline bool show_debug_links() { return m_fShowDebugLinks; }
    inline void enable_experimental_features(bool value) { m_fExperimentalFeatures = value; }
	inline bool are_experimental_features_enabled() { return m_fExperimentalFeatures; }
    inline void set_visual_tracking_mode(int value) { m_trackingMode = value; }
    inline int get_visual_tracking_mode() { return m_trackingMode; }
    inline void set_tempo_line_width(LUnits value) { m_tempoLineWidth = value; }
    inline LUnits get_tempo_line_width() { return m_tempoLineWidth; }

    //global optionf for debug
    //inline void set_justify_systems(bool value) { m_sAppName = value; }

protected:
    void initialize_lomse();

};

//---------------------------------------------------------------------------------------
class Injector
{
public:
    Injector() {}
    ~Injector() {}
};


}   //namespace lenmus

#endif      //__LENMUS_INJECTORS_H__
