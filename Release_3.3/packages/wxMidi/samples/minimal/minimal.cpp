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

#include "../../src/portmidi/pm_common/portmidi.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------


IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
#if defined(UNICODE) || defined(_UNICODE)
    wxFrame *frame = new wxFrame((wxFrame*) NULL, -1, _T("Test Unicode"));
#else
    wxFrame *frame = new wxFrame((wxFrame*) NULL, -1, _T("Test Multibyte"));
#endif
    frame->CreateStatusBar();
    frame->SetStatusText(_T("Test "));
    frame->Show(TRUE);
    SetTopWindow(frame);

    PmError ret = Pm_Initialize();
    if (ret == pmNoError)
    {
        wxLogMessage(_T("port midi initialised"));
    }
    else
    {
        wxLogMessage(_T("couldn't initialise portmidi"));
    }
    return true;
}

int MyApp::OnExit()
{
    PmError ret = Pm_Terminate(); 

    if (ret == pmNoError)
    {
        wxLogMessage(_T("port midi terminated successfully"));
    }
    else
    {
        wxLogMessage(_T("couldn't terminate port midi"));
    }   

    return 0;
}
