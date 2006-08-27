// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/bookctrl.h"

#include "wxMidi.h"

#ifndef _DEBUG
//For release versions (ANSI and Unicode) there is a compilation/linking error somewhere
//either in wxWidgets or in wxMidi as these two errors are generated:
//  wxmidi11u.lib(wxMidi.obj) : error LNK2019: símbolo externo "void __cdecl wxAssert(int,char const *,int,char const *,char const *)" (?wxAssert@@YAXHPBDH00@Z) sin resolver al que se hace referencia en la función "void __cdecl wxPostEvent(class wxEvtHandler *,class wxEvent &)" (?wxPostEvent@@YAXPAVwxEvtHandler@@AAVwxEvent@@@Z)
//  wxmidi11u.lib(wxMidiDatabase.obj) : error LNK2001: símbolo externo "void __cdecl wxAssert(int,char const *,int,char const *,char const *)" (?wxAssert@@YAXHPBDH00@Z) sin resolver
//As I can not avoid the error, these next definitions are a bypass:
    #ifdef _UNICODE
        extern void __cdecl wxAssert(int n, unsigned short const* s, int m, unsigned short const* s2, unsigned short const* s3);
        void __cdecl wxAssert(int n, unsigned short const* s, int m, unsigned short const* s2, unsigned short const* s3) {}
    #else
        extern void __cdecl wxAssert(int n, char const* s, int m, char const* s2, char const* s3);
        void __cdecl wxAssert(int n, char const * s, int m, char const* s2, char const* s3) {}
    #endif

#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

class MyPanel: public wxPanel
{
public:
    MyPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~MyPanel();

	//buttons
	void OnButtonOpenDevice(wxCommandEvent &event);
	void OnButtonNoteOn(wxCommandEvent &event);
	void OnButtonNoteOff(wxCommandEvent &event);
	void OnButtonChord(wxCommandEvent &event);
	void OnButtonPlayScale(wxCommandEvent &event);
	void OnButtonSysEx(wxCommandEvent &event);
	void OnButtonStartReceiving(wxCommandEvent &event);
	void OnButtonStopReceiving(wxCommandEvent &event);
	void OnButtonCrash(wxCommandEvent &event);
	void OnButtonLoopBack(wxCommandEvent &event);
	void OnButtonStartListening(wxCommandEvent &event);
	void OnButtonStopListening(wxCommandEvent &event);

	//combos
	void OnComboSections(wxCommandEvent &event);
	void OnComboInstruments(wxCommandEvent &event);

	//other events
	void OnSize( wxSizeEvent& WXUNUSED(event) );
	void OnPageChanging( wxBookCtrlEvent &event );
	void OnPollingEvent(wxCommandEvent &event);
	void OnInternalIdle();
	void OnMidiReceive(wxCommandEvent &event);

private:
	void DoProgramChange();
	void DoReceiveMessage();
	void CloseDevices();
	void CreatePollingEvent();
	void StartReceiving();

		// member variables

	//controls on the frame
    wxBookCtrl*		m_book;
    wxTextCtrl*		m_text;

	//controls on panel 1
    wxComboBox*		m_pOutCombo;
    wxComboBox*		m_pInCombo;
	wxButton*		m_btOpenDevice;

	//controls on panel 2
	wxComboBox*		m_pSectCombo;		//combo for section names
	wxComboBox*		m_pInstrCombo;		//combo for instrument names
	wxButton*		m_btNoteOn;
	wxButton*		m_btNoteOff;
	wxButton*		m_btPlayChord;
	wxButton*		m_btPlayScale;

	//controls on panel 3
	wxButton*		m_btSysEx;
	wxButton*		m_btLoopBack;

	//controls on panel 4
	wxButton*		m_btStartRec;
	wxButton*		m_btStopRec;

	//controls on panel 5
	wxButton*		m_btCrash;

	//controls on panel 6
	wxButton*		m_btStartLis;
	wxButton*		m_btStopLis;

	//data
	wxFrame*			m_pFrame;		//ptr to parent frame			
	wxMidiSystem*		m_pMidi;		//ptr to MIDI package
    wxMidiOutDevice*	m_pOutDev;		//ptr to current MIDI output device
    wxMidiInDevice*		m_pInDev;		//ptr to current MIDI input device
	long				m_latency;		//delay to use
	bool				m_fReceiveEnabled;		//MIDI in enabled
	bool				m_fDoCrash;				//force a program crash
	bool				m_fCreatePollingEvent;		//create a new Midi event

    DECLARE_EVENT_TABLE()
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    MENU_Quit = 1,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    MENU_About = wxID_ABOUT,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(MENU_Quit,  MyFrame::OnQuit)
    EVT_MENU(MENU_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("wxMidi test sample application"),
                                 wxPoint(50, 50), wxSize(600, 400));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, -1, title, pos, size, style)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(MENU_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(MENU_About, _T("&About...\tF1"), _T("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

	new MyPanel( this, 10, 10, 600, 400 );


#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxMidi sample!"));
#endif // wxUSE_STATUSBAR
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);	// TRUE is to force the frame to close
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the wxMidi sample.\n")
                _T("Welcome to wxMidi %s"), wxMIDI_VERSION);

    wxMessageBox(msg, _T("About wxMidi sample"), wxOK | wxICON_INFORMATION, this);
}


//----------------------------------------------------------------------
// MyPanel
//----------------------------------------------------------------------

enum {
	ID_MY_PANEL = 100,
	ID_BOOK,
	ID_COMBO_OUTDEV,
	ID_OPEN_DEVICE,
	ID_NOTE_ON,
	ID_NOTE_OFF,
	ID_PLAY_CHORD,
	ID_PLAY_SCALE,
	ID_COMBO_SECTIONS,
	ID_COMBO_INSTRUMENTS,
	ID_SEND_SYSEX,
	ID_COMBO_INDEV,
	ID_START_RECEIVING,
	ID_STOP_RECEIVING,
	ID_CRASH,
	ID_START_LISTENING,
	ID_STOP_LISTENING,
	ID_LOOP_BACK
};

//Define a new event to poll MIDI input
DECLARE_EVENT_TYPE(wxEVT_POLLING_EVENT, -1)

DEFINE_EVENT_TYPE(wxEVT_POLLING_EVENT)



BEGIN_EVENT_TABLE(MyPanel, wxPanel)
	EVT_SIZE      (MyPanel::OnSize)

	EVT_BOOKCTRL_PAGE_CHANGING(ID_BOOK, MyPanel::OnPageChanging)

	EVT_BUTTON    (ID_OPEN_DEVICE, MyPanel::OnButtonOpenDevice)
	EVT_BUTTON    (ID_NOTE_ON, MyPanel::OnButtonNoteOn)
	EVT_BUTTON    (ID_NOTE_OFF, MyPanel::OnButtonNoteOff)
	EVT_BUTTON    (ID_PLAY_CHORD, MyPanel::OnButtonChord)
	EVT_BUTTON    (ID_PLAY_SCALE, MyPanel::OnButtonPlayScale)

	EVT_BUTTON    (ID_SEND_SYSEX, MyPanel::OnButtonSysEx)
	EVT_BUTTON    (ID_LOOP_BACK, MyPanel::OnButtonLoopBack)

	EVT_BUTTON    (ID_START_RECEIVING, MyPanel::OnButtonStartReceiving)
	EVT_BUTTON    (ID_STOP_RECEIVING, MyPanel::OnButtonStopReceiving)

	EVT_BUTTON    (ID_CRASH, MyPanel::OnButtonCrash)

	EVT_BUTTON    (ID_START_LISTENING, MyPanel::OnButtonStartListening)
	EVT_BUTTON    (ID_STOP_LISTENING, MyPanel::OnButtonStopListening)

	EVT_COMBOBOX  (ID_COMBO_SECTIONS, MyPanel::OnComboSections)
	EVT_COMBOBOX  (ID_COMBO_INSTRUMENTS, MyPanel::OnComboInstruments)

	EVT_COMMAND	  (ID_MY_PANEL, wxEVT_POLLING_EVENT, MyPanel::OnPollingEvent)
	EVT_COMMAND	  (wxID_ANY, wxEVT_MIDI_INPUT, MyPanel::OnMidiReceive)

END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

MyPanel::MyPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, ID_MY_PANEL, wxPoint(x, y), wxSize(w, h) )
{

	//initialize member variables
	m_pFrame = frame;
    m_pOutDev = (wxMidiOutDevice*)NULL;
    m_pInDev = (wxMidiInDevice*)NULL;
	m_pMidi = wxMidiSystem::GetInstance();

	m_fCreatePollingEvent = false;
	m_fReceiveEnabled = false;
	m_fDoCrash = false;

	////frame layout
    m_text = new wxTextCtrl(this, wxID_ANY, _T(""),
                            wxPoint(0, 250), wxSize(100, 50), wxTE_MULTILINE);

    m_book = new wxBookCtrl(this, ID_BOOK);


	//
	// Panel 1: MIDI devices selection panel ------------------------------------------
	//

    wxPanel *panel = new wxPanel(m_book);

	wxBoxSizer * pMainSizer = new wxBoxSizer(wxVERTICAL);
	panel->SetSizer(pMainSizer);		// set autolayout based on sizers
	panel->SetAutoLayout(true);


	//Devices combos + Open devices button
	pMainSizer->Add(new wxStaticText(panel, -1, _T("Output devices available:")),
					0, wxALL, 10 );

	m_pOutCombo = new wxComboBox( panel, ID_COMBO_OUTDEV, _T("This"),
                              wxPoint(20,25), wxSize(270, wxDefaultCoord),
                              0, NULL,
                              wxCB_DROPDOWN | wxCB_READONLY | wxPROCESS_ENTER);
	pMainSizer->Add(m_pOutCombo, 0, wxALL, 10 );

	pMainSizer->Add(new wxStaticText(panel, -1, _T("Input devices available:")),
					0, wxALL, 10 );

	m_pInCombo = new wxComboBox( panel, ID_COMBO_INDEV, _T("This"),
                              wxPoint(20,25), wxSize(270, wxDefaultCoord),
                              0, NULL,
                              wxCB_DROPDOWN | wxCB_READONLY | wxPROCESS_ENTER);
	pMainSizer->Add(m_pInCombo, 0, wxALL, 10 );

    m_btOpenDevice = new wxButton(panel, ID_OPEN_DEVICE, _T("Open devices"), 
								  wxDefaultPosition, wxSize(140,25) );
    pMainSizer->Add(m_btOpenDevice,0, wxALL, 10 );

    m_book->AddPage(panel, _("Devices"), true);		//true -> select this page



	//
	// Panel 2: MIDI out test panel ----------------------------------------------------
	//

    panel = new wxPanel(m_book);
	wxBoxSizer * pPanel2Sizer = new wxBoxSizer(wxVERTICAL);
	panel->SetSizer(pPanel2Sizer);
	panel->SetAutoLayout(true);

	//sections and instruments combos + Program change button
	wxBoxSizer* pInstrSizer = new wxBoxSizer(wxHORIZONTAL);
	pPanel2Sizer->Add(pInstrSizer, 0, wxALL, 10 );

	m_pSectCombo = new wxComboBox( panel, ID_COMBO_SECTIONS, _T("This"),
                              wxPoint(20,25), wxSize(270, wxDefaultCoord),
                              0, NULL,
                              wxCB_DROPDOWN | wxCB_READONLY | wxPROCESS_ENTER);
	pInstrSizer->Add(m_pSectCombo, 0, wxALL, 10 );

	wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
	pMidiGM->PopulateWithSections(m_pSectCombo);
	m_pInstrCombo = new wxComboBox( panel, ID_COMBO_INSTRUMENTS, _T("This"),
                              wxPoint(20,25), wxSize(270, wxDefaultCoord),
                              0, NULL,
                              wxCB_DROPDOWN | wxCB_READONLY | wxPROCESS_ENTER);
	pInstrSizer->Add(m_pInstrCombo, 0, wxALL, 10 );
	pMidiGM->PopulateWithInstruments(m_pInstrCombo, 0, 0);


	//sizer for the buttons
	wxBoxSizer* pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
	pPanel2Sizer->Add(pButtonsSizer, 0, wxALL, 10);

	m_btNoteOn = new wxButton(panel, ID_NOTE_ON, _T("Note On"),
									wxDefaultPosition, wxSize(100,25) );
    pButtonsSizer->Add(m_btNoteOn,0, wxALL, 10 );

	m_btNoteOff = new wxButton(panel, ID_NOTE_OFF, _T("Note Off"),
									wxDefaultPosition, wxSize(100,25) );
    pButtonsSizer->Add(m_btNoteOff,0, wxALL, 10 );

	m_btPlayChord = new wxButton(panel, ID_PLAY_CHORD, _T("Play chord"),
									wxDefaultPosition, wxSize(100,25) );
    pButtonsSizer->Add(m_btPlayChord,0, wxALL, 10 );

	m_btPlayScale = new wxButton(panel, ID_PLAY_SCALE, _T("Play scale"),
									wxDefaultPosition, wxSize(100,25) );
    pButtonsSizer->Add(m_btPlayScale,0, wxALL, 10 );


    m_book->AddPage(panel, _("MIDI output"), false);


	//
	// Panel 3: Send SysEx test panel ----------------------------------------------------
	//

    panel = new wxPanel(m_book);
	wxBoxSizer * pPanel3Sizer = new wxBoxSizer(wxVERTICAL);
	panel->SetSizer(pPanel3Sizer);
	panel->SetAutoLayout(true);


	pPanel3Sizer->Add(
		new wxStaticText(panel, wxID_ANY,
			_T("Sorry, SysEx messages are equipment-specific commands and data for each\n")
			_T("MIDI maker. You have to change the pre-programed test message to suit\n")
			_T("your specific MIDI equipment. In this example, the sysex message is\n")
			_T("a command for a Casio Privia PX-100 keyboard, to set up reverberation\n")
			_T("to value 03") ),
		1, wxEXPAND , 30 );

	wxBoxSizer* pSizer3 = new wxBoxSizer(wxHORIZONTAL);
	pPanel3Sizer->Add(pSizer3, 0, wxALL, 10);

	m_btSysEx = new wxButton(panel, ID_SEND_SYSEX, _T("Send SysEx"),
									wxPoint(30,130), wxSize(140,25) );
    pSizer3->Add(m_btSysEx, 0, wxALL, 10 );

    pSizer3->Add(
		new wxButton(panel, ID_STOP_RECEIVING, _T("Stop receiving"),
				wxPoint(200,130), wxSize(140,25) ),
		0, wxALL, 10 );

	//m_btLoopBack = new wxButton(panel, ID_LOOP_BACK, _T("Loop back test"),
	//								wxPoint(30,180), wxSize(140,25) );
 //   pPanel3Sizer->Add(m_btLoopBack, 0, wxALL, 10 );
	
    m_book->AddPage(panel, _("MIDI SysEx"), false);



	//
	// Panel 4: MIDI input test panel (polling) ----------------------------------------
	//

    panel = new wxPanel(m_book);
	wxBoxSizer* pPanel4Sizer = new wxBoxSizer(wxVERTICAL);
	panel->SetSizer(pPanel4Sizer);		// set autolayout based on sizers
	panel->SetAutoLayout(true);

	m_btStartRec = new wxButton(panel, ID_START_RECEIVING, _T("Start receiving"),
									wxPoint(30,30), wxSize(140,25) );
    pPanel4Sizer->Add(m_btStartRec, 0, wxALL, 10 );

	m_btStopRec = new wxButton(panel, ID_STOP_RECEIVING, _T("Stop receiving"),
									wxPoint(30,30), wxSize(140,25) );
    pPanel4Sizer->Add(m_btStopRec, 0, wxALL, 10 );

    m_book->AddPage(panel, _("input (polling)"), false);



	//
	// Panel 5: Crash test panel ----------------------------------------------------
	//

    panel = new wxPanel(m_book);
	wxBoxSizer* pPanel5Sizer = new wxBoxSizer(wxVERTICAL);
	panel->SetSizer(pPanel5Sizer);		// set autolayout based on sizers
	panel->SetAutoLayout(true);

	m_btCrash = new wxButton(panel, ID_CRASH, _T("Crash program"),
									wxPoint(30,30), wxSize(140,25) );
    pPanel4Sizer->Add(m_btCrash, 0, wxALL, 10 );

    m_book->AddPage(panel, _("Crash test"), false);


	//
	// Panel 6: MIDI input test panel (Events) ------------------------------------------
	//

    panel = new wxPanel(m_book);
	wxBoxSizer* pPanel6Sizer = new wxBoxSizer(wxVERTICAL);
	panel->SetSizer(pPanel6Sizer);		// set autolayout based on sizers
	panel->SetAutoLayout(true);

	m_btStartLis = new wxButton(panel, ID_START_LISTENING, _T("Start receiving"),
									wxPoint(30,30), wxSize(140,25) );
    pPanel6Sizer->Add(m_btStartLis, 0, wxALL, 10 );

	m_btStopLis = new wxButton(panel, ID_STOP_LISTENING, _T("Stop receiving"),
									wxPoint(30,30), wxSize(140,25) );
    pPanel6Sizer->Add(m_btStopLis, 0, wxALL, 10 );

    m_book->AddPage(panel, _("input (events)"), false);




	// populate combo box with available Midi devices
	
	int nNumDevices = m_pMidi->CountDevices();
    wxString sMsg;                                                             
	sMsg.Printf(_T("There are %d MIDI devices available\n"), nNumDevices);
	m_text->SetValue(sMsg);

	// available input and output devices
	int nItem, nInput=0, nOutput=0;
	for (int i = 0; i < nNumDevices; i++) {
        wxMidiOutDevice* pMidiDev = new wxMidiOutDevice(i);
        if (pMidiDev->IsOutputPort()) {
			nOutput++;
			sMsg.Printf(_T("%s [%s]"),
						pMidiDev->DeviceName(),
						pMidiDev->InterfaceUsed() );
			nItem = m_pOutCombo->Append(sMsg);
			m_pOutCombo->SetClientData(nItem, (void *)i);
        }
		if (pMidiDev->IsInputPort()) {
			nInput++;
			sMsg.Printf(_T("%s [%s]"),
						pMidiDev->DeviceName(),
						pMidiDev->InterfaceUsed() );
			nItem = m_pInCombo->Append(sMsg);
			m_pInCombo->SetClientData(nItem, (void *)i);
        }
		delete pMidiDev;
    }
	if (nOutput > 0) m_pOutCombo->SetSelection(0);
	if (nInput > 0) m_pInCombo->SetSelection(0);

	//disable all controls and  buttons but "Open Device"
	//until a Midi device is selected
	m_btOpenDevice->Enable(true);
	m_btNoteOn->Enable(false);
	m_btNoteOff->Enable(false);
	m_btPlayChord->Enable(false);
	m_pSectCombo->Enable(false);
	m_pInstrCombo->Enable(false);

}

MyPanel::~MyPanel()
{
	CloseDevices();
	if (m_pMidi) delete m_pMidi;
}

void MyPanel::OnPageChanging( wxBookCtrlEvent &event )
{
	//Prevent page change if no MIDI device is selected
    int nNumPage = event.GetOldSelection();
	if (nNumPage != -1) {
		if (!m_pOutDev)
		{
			event.Veto();
			wxMessageBox(_("You can not change to another page until a MIDI Device is susscessfully opened"));
			return;
		}
	}
	event.Allow();

}


void MyPanel::OnSize( wxSizeEvent& WXUNUSED(event) )
{
    int x = 0;
    int y = 0;
    GetClientSize( &x, &y );

    if (m_book) m_book->SetSize( 2, 2, x-4, y*2/3-4 );
    if (m_text) m_text->SetSize( 2, y*2/3+2, x-4, y/3-4 );
}


void MyPanel::OnButtonOpenDevice( wxCommandEvent &event )
{

	CloseDevices();

	//get number of Midi device to use
	int nIndex = m_pOutCombo->GetSelection();
	int nMidiDev = (int) m_pOutCombo->GetClientData(nIndex);
    m_pOutDev = new wxMidiOutDevice(nMidiDev);

    // open output device
	wxString sMsg;
	long m_lantency = 0;
	wxMidiError nErr = m_pOutDev->Open(m_lantency);
	if (nErr) {
		sMsg.Printf(_T("Error %d in Open: %s \n"),
					nErr, m_pMidi->GetErrorText(nErr));
	}
	else {
	    sMsg.Printf(_T("Output device '%s' sucessfully opened.\n"),
			m_pOutDev->DeviceName());
	}
	m_text->AppendText(sMsg);

	//Open input device
	nIndex = m_pInCombo->GetSelection();
	if (nIndex != -1) {
		nMidiDev = (int) m_pInCombo->GetClientData(nIndex);
		m_pInDev = new wxMidiInDevice(nMidiDev);
		nErr = m_pInDev->Open();
		if (nErr)
				sMsg.Printf(_T("Error %d in Open: %s \n"),
							nErr, m_pMidi->GetErrorText(nErr));
		else {
			sMsg.Printf(_T("Input device %s sucessfully opened.\n"),
				m_pInDev->DeviceName());
		}
	}
	else {
	    sMsg = _T("No input device available.\n");
	}
	m_text->AppendText(sMsg);


	//enable all buttons
	m_btOpenDevice->Enable(true);
	m_btNoteOn->Enable(true);
	m_btNoteOff->Enable(false);
	m_btPlayChord->Enable(true);
	m_pSectCombo->Enable(true);
	m_pInstrCombo->Enable(true);

}

void MyPanel::DoProgramChange()
{
	//Change Midi instrument to the one selected in combo Instruments
	int nInstr = m_pInstrCombo->GetSelection();
	int nSect = m_pSectCombo->GetSelection();
	wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
	int nProgram = pMidiGM->GetInstrFromSection(nSect, nInstr);
	int nChannel = 0;
	wxString sMsg;
    wxMidiError nErr = m_pOutDev->ProgramChange(nChannel, nProgram);
	if (nErr) {
		sMsg = wxString::Format(
			_T("Error %d in ProgramChange\n"), nErr);
	}
	else {
		sMsg = wxString::Format(
			_T("ProgramChange OK: %s\n"), pMidiGM->GetInstrumentName(nProgram) );
	}
	m_text->AppendText(sMsg);

}
void MyPanel::OnButtonNoteOn(wxCommandEvent &event)
{
	wxString sMsg;
	wxMidiShortMessage msg(0x90, 60, 127);
	wxMidiError nErr = m_pOutDev->Write(&msg);
	// alternative:
    // wxMidiError nErr = m_pOutDev->NoteOn(0, 60, 127);
	if (nErr) {
		sMsg.Printf(_T("Error %d in NoteOn: %s \n"),
					nErr, m_pMidi->GetErrorText(nErr));
	}
	else {
	    sMsg = _T("NoteOn OK\n");
	}
	m_text->AppendText(sMsg);

	//disable all buttons but Note off
	m_btOpenDevice->Enable(false);
	m_btNoteOn->Enable(false);
	m_btNoteOff->Enable(true);
	m_btPlayChord->Enable(false);

}
void MyPanel::OnButtonNoteOff(wxCommandEvent &event)
{
	wxString sMsg;
    wxMidiError nErr = m_pOutDev->NoteOff(0, 60, 127);
	if (nErr) {
		sMsg.Printf(_T("Error %d in NoteOff: %s \n"),
					nErr, m_pMidi->GetErrorText(nErr));
	}
	else {
	    sMsg = _T("NoteOff OK\n");
	}
	m_text->AppendText(sMsg);

	//enable/disable buttons
	m_btOpenDevice->Enable(true);
	m_btNoteOn->Enable(true);
	m_btNoteOff->Enable(false);
	m_btPlayChord->Enable(true);

}

void MyPanel::OnButtonChord(wxCommandEvent &event)
{
	//PlayChord. Let's test Write with several events
    int chord[] = { 60, 67, 76, 83, 90 };
	#define CHORD_SIZE 5

	wxMidiShortMessage* pMidiMsg;
	for (int i = 0; i < CHORD_SIZE; i++) {
        pMidiMsg = new wxMidiShortMessage(0x90, chord[i], 100);
		m_pOutDev->Write(pMidiMsg);
		delete pMidiMsg;
    }

	//Stop the chord after 1 second.
	//Use AllSoundsOff to test it instead of stopping note by note
	::wxMilliSleep(1000);
    m_pOutDev->AllSoundsOff();    

}

void MyPanel::OnButtonPlayScale(wxCommandEvent &event)
{
	//Play a scale 

	int scale[] = { 60, 62, 64, 65, 67, 69, 71, 72, 72, 71, 69, 67, 65, 64, 62, 60 };
	#define SCALE_SIZE 16
	int channel = 0, volume = 127;

	for (int i = 0; i < SCALE_SIZE; i++) {
		m_pOutDev->NoteOn(channel, scale[i], volume);
		::wxMilliSleep(200);	// wait 200ms
    	m_pOutDev->NoteOff(channel, scale[i], volume);    
	}

}

void MyPanel::OnButtonSysEx(wxCommandEvent &event)
{
	//Sorry, SysEx messages are equipment-specific commands and data for each MIDI maker.
	//You have to change the following message to suit your specific MIDI equipment
	// In this exaple, the sysex message is a command for a Casio Privia PX-100
	// keyboard, to set up reverberation to value 03

	//test of sending a sysex message
	wxByte msg[] = {
		0xF0,				//start of long message
		0x7F, 0x7F, 0x04, 0x05, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x00,			//command reverb.
		0x03,				// reverberation value: 0x00 - 0x03
		0xF7				//end of message
	};

	wxMidiSysExMessage sysex_msg(msg);
    wxMidiError nErr = m_pOutDev->Write(&sysex_msg);

	wxString sMsg;
	if (nErr) {
		sMsg.Printf(_T("Error %d in WriteSysEx: %s \n"),
					nErr, m_pMidi->GetErrorText(nErr));
	}
	else {
	    sMsg = _T("WriteSysEx OK\n");
	}
	m_text->AppendText(sMsg);

	StartReceiving();

}

void MyPanel::OnComboSections(wxCommandEvent &event)
{
	// A new section selected. Reload Instruments combo with the instruments in the
	//selected section

	wxMidiDatabaseGM* pMidiGM = wxMidiDatabaseGM::GetInstance();
	int nSect = m_pSectCombo->GetSelection();
	pMidiGM->PopulateWithInstruments(m_pInstrCombo, nSect);
	DoProgramChange();

}

void MyPanel::OnComboInstruments(wxCommandEvent &event)
{
	// A new instrument selected. Change Midi program
	DoProgramChange();
}

void MyPanel::CloseDevices()
{
	//close devices and delete midi device objects
	if (m_pOutDev) {
		m_pOutDev->Close();
		delete m_pOutDev;
		m_pOutDev = (wxMidiOutDevice*)NULL;
	}
	if (m_pInDev) {
		m_pInDev->Close();
		delete m_pInDev;
		m_pInDev = (wxMidiInDevice*)NULL;
	}

}

void MyPanel::OnButtonStartListening(wxCommandEvent &event)
{
	if (!m_pInDev) {
		m_text->AppendText(_T("No input device. Listening not started.\n"));
		return;
	}

	//Filter out active sensing messages (0xFE) and clock messages (0xF8 only)
    m_pInDev->SetFilter(wxMIDI_FILT_ACTIVE | wxMIDI_FILT_CLOCK);

    // empty the buffer after setting filter, just in case anything got through
    m_pInDev->Flush();

	m_text->AppendText(_T("Start listening...\n"));
	wxMidiError nErr = m_pInDev->StartListening(this);
	if (nErr) {
		m_text->AppendText( wxString::Format(
					_T("Error %d in StartListening: %s \n"),
					nErr, m_pMidi->GetErrorText(nErr) ));
	}

}

void MyPanel::OnButtonStopListening(wxCommandEvent &event)
{
	if (!m_pInDev) {
		m_text->AppendText(_T("No input device. Listening was not started.\n"));
		return;
	}

	m_text->AppendText(_T("Stop listening.\n"));
	m_pInDev->StopListening();

}



void MyPanel::OnButtonStartReceiving(wxCommandEvent &event)
{
	StartReceiving();
}

void MyPanel::OnInternalIdle()
{
	if (m_fCreatePollingEvent) {
		m_fCreatePollingEvent = false;
		CreatePollingEvent();
	}

}

void MyPanel::OnMidiReceive(wxCommandEvent &event)
{
	event.Skip(true);			//do not propagate this event
	m_text->AppendText(_T("Event wxEVT_MIDI_INPUT.  "));
	while (m_pInDev->Poll()) {
		DoReceiveMessage();
	}

}

void MyPanel::CreatePollingEvent()
{
	// if reception enabled create new polling Midi input event 

	if (m_fReceiveEnabled) {
		wxCommandEvent new_event( wxEVT_POLLING_EVENT, GetId() );
		new_event.SetEventObject( this );
		GetEventHandler()->AddPendingEvent( new_event );	// Add it to the queue
	}

}

void MyPanel::OnButtonCrash(wxCommandEvent &event)
{
	StartReceiving();
	m_fDoCrash = true;
}

void MyPanel::OnPollingEvent(wxCommandEvent &event)
{
	event.Skip(true);			//do not propagate this event

    // simulate crash if m_fDoCrash is true
    if (m_fDoCrash) {
		m_fDoCrash = false;
		// crash the program to test whether midi ports get closed
		int * tmp = NULL;
		*tmp = 5;
    }

	if (!m_pInDev) return;

    if (m_pInDev->Poll()) {
		DoReceiveMessage();
    }
	else {
		//no messages available
		// wait 100ms for low event creation rate when no data available
		::wxMilliSleep(100);
	}

	//continue polling the Midi input
	m_fCreatePollingEvent = true;		//when idle, create a polling Midi input event

}

void MyPanel::DoReceiveMessage()
{
	//Read a meessage waiting to be read

	wxString sMsg;
	wxMidiError nErr;
	wxMidiMessage* pMidiMsg = m_pInDev->Read(&nErr);
	if (nErr) {
		sMsg.Printf(_T("Error %d in Read: %s \n"),
					nErr, m_pMidi->GetErrorText(nErr));
	}
	else {
		if (pMidiMsg->GetType() == wxMIDI_SHORT_MSG) {
			wxMidiShortMessage* pMsg = (wxMidiShortMessage*) pMidiMsg;
			sMsg.Printf(_T("Received short message: time %ld:\t%02X %02X %02X\n"),
						pMsg->GetTimestamp(),
						pMsg->GetStatus(),
						pMsg->GetData1(),
						pMsg->GetData2() );
			delete pMsg;		// do not forget!!
		}
		else {
			wxMidiSysExMessage* pMsg = (wxMidiSysExMessage*) pMidiMsg;
			sMsg.Printf(_T("Received sysex message: time %ld:\t"),
						pMsg->GetTimestamp() );
			wxByte* pData = pMsg->GetMessage();
			for (int i=0; i < pMsg->Length(); i++, pData++) {
				if (i != 0 && i % 16 == 0) sMsg += _T("\n\t\t");
				sMsg += wxString::Format(_T("%02X "), *pData);
			}
			sMsg += _T("\n");
			delete pMsg;		// do not forget!!
		}
	}
	m_text->AppendText(sMsg);

}

void MyPanel::OnButtonStopReceiving(wxCommandEvent &event)
{
	m_fReceiveEnabled = false;
	m_text->AppendText(_T("Reception stopped.\n"));
}

void MyPanel::StartReceiving()
{
	if (!m_pInDev) {
		m_text->AppendText(_T("No input device. Reception not started.\n"));
		return;
	}

	m_text->AppendText(_T("Start receiving:\n"));
	m_fReceiveEnabled = true;

	//Filter out active sensing messages (0xFE) and clock messages (0xF8 only)
    m_pInDev->SetFilter(wxMIDI_FILT_ACTIVE | wxMIDI_FILT_CLOCK);

    // empty the buffer after setting filter, just in case anything got through
    m_pInDev->Flush();

	//CreatePollingEvent();
	m_fCreatePollingEvent = true;		//when idle, create a Midi event
}

void MyPanel::OnButtonLoopBack(wxCommandEvent &event)
{
}