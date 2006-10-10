/////////////////////////////////////////////////////////////////////////////
// Name:        hyperlinkctrl.cpp
// Purpose:     wxHyperlinkCtrl
// Author:      Angelo Mandato
// Created:     2005/08/11
// RCS-ID:      $Id: hyperlinkctrl.cpp,v 1.1 2005/08/12 04:12:49 amandato Exp $
// Copyright:   (c) 2001-2005 Angelo Mandato
// Licence:     wxWidgets licence
// Version:		1.4
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "hyperlinkctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// includes
#ifndef WX_PRECOMP
	// here goes the #include <wx/abc.h> directives for those
	// files which are not included by wxprec.h
  #include <wx/wx.h>
#endif

#include <wx/msgdlg.h>
#include <wx/mimetype.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/clipbrd.h>

#include <wx/event.h>
#include <wx/tooltip.h>

#include <wx/hyperlinkctrl.h>

#ifdef __WIN32__
	#include <windows.h>
	#include <wx/msw/registry.h>
#endif



DEFINE_EVENT_TYPE(wxEVT_COMMAND_LINK_CLICKED)
#ifndef HYPERLINKCTRL_COMMANDEVENT
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LINK_MCLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LINK_RCLICKED)
#endif

BEGIN_EVENT_TABLE(wxHyperlinkCtrl, wxStaticText)
	EVT_MOUSE_EVENTS( wxHyperlinkCtrl::OnMouseEvent)
	EVT_MOTION(wxHyperlinkCtrl::OnMouseEvent)
  EVT_MENU(HYPERLINKS_POPUP_COPY, wxHyperlinkCtrl::OnPopUpCopy )
END_EVENT_TABLE()

//! wxHyperlinkCtrl constructor
wxHyperlinkCtrl::wxHyperlinkCtrl(wxWindow *parent, wxWindowID id, const wxString &label,
               const wxPoint &pos, const wxSize &size, int style, const wxString& name, const wxString& szURL )
			   : wxStaticText(parent, id, label, pos, size, style|wxPOPUP_WINDOW, name)
{
	if( szURL.IsEmpty() )
		m_szURL = label;
	else
		m_szURL = szURL;

	// Set Tooltip
	SetToolTip( m_szURL );

	// Set default properties
	ReportErrors(); // default: true
	SetUnderlines(); // default: true, true, true
	SetColours(); // default: blue, violet, blue
	SetVisited(); // default: false
	EnableRollover();// default: false
	SetBold(); // default: false
	SetLinkCursor(); // default: wxCURSOR_HAND

//**Added By Mark McManus
    AutoBrowse(); // default true
    DoPopup(); // default true
//**Mark McManus

  OpenInSameWindow(); // default false
	// Set control properties and refresh
	UpdateLink(true);
}

//! Goto the specified URL.  This function may be called statically.  bSameWindowIfPossible only works with Internet Explorer.
bool wxHyperlinkCtrl::GotoURL( const wxString &szUrl, const wxString &szBrowser, const bool bReportErrors, const bool bSameWinIfPossible )
{
  wxLogNull logOff;

	// Use the browser specified
	if(!szBrowser.IsEmpty() )
	{
		wxString szCmd = szBrowser;
		szCmd += wxEmptyString;
		szCmd += szUrl; // URLs are encoded so no need to put them in quotes

		if( wxExecute( szCmd, FALSE) != 0 )
			return true;
		else
			DisplayError(wxT("Unable to launch specified browser."), bReportErrors);

    return false;
	}

#ifdef __WIN32__

  if( bSameWinIfPossible )
  {
	  HINSTANCE result = ShellExecute(NULL, _T("open"), szUrl.c_str(), NULL,NULL, SW_SHOW);

	  if( (unsigned int)result > HINSTANCE_ERROR )
		  return true;

	  // Hack for Firefox, it returns FNF, do not know why
	  if( (unsigned int)result == SE_ERR_FNF )
		  return true;
  }

	wxRegKey Regkey( wxT("HKEY_CLASSES_ROOT\\http\\shell\\open\\command") );
	Regkey.Open();
	wxString szOpenWith = Regkey;

	wxString szCmd = wxT("");
	if( szOpenWith.Find(wxT("%1")) != -1 )
	{
		szCmd = szOpenWith;
		szCmd.Replace( wxT("%1"), wxT("\"") + szUrl + wxT("\"") );
	}
	else
	{
		// first strip the \" character from the front and parse out the string...
		szOpenWith.Trim(false);

		if( szOpenWith.GetChar(0) == wxT('\"') )
		{
			szOpenWith.Remove(0, 1);
			int nEnd = szOpenWith.Find(wxT("\""));
			if( nEnd > 0 )
				szOpenWith.Remove(nEnd);
		}
		else
		{
			int nEnd = szOpenWith.Find(wxT(" "));
			if( nEnd > 0 )
				szOpenWith.Remove(nEnd);
		}
		
		szCmd.Printf(wxT("\"%s\" \"%s\""), szOpenWith.c_str(), szUrl.c_str() );
	}

	if( wxExecute( szCmd, FALSE) != 0 )
		return true;
	else
	{
		DisplayError(wxT("Unable to launch default browser."), bReportErrors);
		return false;
	}
	
#endif
	// Untested code follows
	wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension( wxT("html") );
	if ( !ft )
	{
		DisplayError(wxT("Impossible to determine the file type for extension html"), bReportErrors);
		return false;
	}

	wxString cmd;
	bool bResult = ft->GetOpenCommand(&cmd, wxFileType::MessageParameters( szUrl, wxT("") ));
	delete ft;

	if (!bResult)
	{
		DisplayError(wxT("Unable to retrieve command information"), bReportErrors);
		return false;
	}

	bResult = (wxExecute( cmd, FALSE) != 0);
  if( !bResult )
    DisplayError(wxT("Unable to launch browser."), bReportErrors);

	return bResult;
}

//! Captures mouse events for Cursor, Link colors and Underlines
void wxHyperlinkCtrl::OnMouseEvent(wxMouseEvent& event)
{
	if ( !event.Moving() )
  {
    if ( event.Entering() )
		{
			SetCursor( m_crHand ); //wxCursor( wxCURSOR_HAND ) );

			if( m_bEnableRollover )
			{
				SetForegroundColour(m_crLinkRolloverColor);
				wxFont fontTemp = GetFont();
				fontTemp.SetUnderlined( m_bRolloverUnderline );
				if( m_bBold )
					fontTemp.SetWeight(wxBOLD);

				SetFont( fontTemp );
				Refresh(true);
			}
		}
    else if ( event.Leaving() )
		{
			SetCursor( wxNullCursor );
			if( m_bEnableRollover )
				UpdateLink(true);
		}
    else if( !event.Moving() )
    {
      if ( event.LeftUp() ) 
      {
				if( m_bAutoBrowse )
        {
					GotoURL( m_szURL, m_szBrowserPath, m_bReportErrors, m_bSameWinIfPossible );
        }
        else
        {
#ifdef HYPERLINKCTRL_COMMANDEVENT
          wxCommandEvent eventOut(wxEVT_COMMAND_LINK_CLICKED, GetId());
          eventOut.SetEventObject(this);
          GetParent()->ProcessEvent( eventOut );
#else
          wxPoint posOnParent = GetPosition();
          posOnParent.x += event.GetPosition().x;
          posOnParent.y += event.GetPosition().y;

				  wxHyperlinkEvent eventOut(wxEVT_COMMAND_LINK_CLICKED, GetId());
				  eventOut.SetEventObject(this);
				  eventOut.SetPosition( posOnParent );
          GetParent()->ProcessEvent( eventOut );
#endif
        }

				SetVisited( true );
      }
      else if ( event.RightUp() )
      {
				//**Modified By Mark McManus, enhanced by Angelo Mandato
        if( m_bDoPopup )
        {
					wxMenu *menuPopUp = new wxMenu(wxT(""), wxMENU_TEAROFF);
					menuPopUp->Append(HYPERLINKS_POPUP_COPY, wxT("Copy"));
					PopupMenu( menuPopUp, wxPoint( event.m_x, event.m_y ) );
					delete menuPopUp; // ADDED 06/30/2004 (prevents memory leaks)
        }
				else
				{
#ifndef HYPERLINKCTRL_COMMANDEVENT
          wxPoint posOnParent = GetPosition();
          posOnParent.x += event.GetPosition().x;
          posOnParent.y += event.GetPosition().y;

				  wxHyperlinkEvent eventOut(wxEVT_COMMAND_LINK_RCLICKED, GetId());
				  eventOut.SetEventObject(this);
				  eventOut.SetPosition( posOnParent );
          GetParent()->ProcessEvent( eventOut );
#endif
				}
        //**Mark McManus
      }
      else if( event.MiddleUp() )
      {
#ifndef HYPERLINKCTRL_COMMANDEVENT
        wxPoint posOnParent = GetPosition();
        posOnParent.x += event.GetPosition().x;
        posOnParent.y += event.GetPosition().y;

				wxHyperlinkEvent eventOut(wxEVT_COMMAND_LINK_MCLICKED, GetId());
				eventOut.SetEventObject(this);
				eventOut.SetPosition( posOnParent );
        GetParent()->ProcessEvent( eventOut );
#endif
      }
    }
  }

	event.Skip();
}

//! Menu pop up copy to clipboard event
void wxHyperlinkCtrl::OnPopUpCopy( wxCommandEvent &event )
{
  wxUnusedVar(event);
  wxTheClipboard->UsePrimarySelection();

  if (!wxTheClipboard->Open())
    return;

  wxTextDataObject *data = new wxTextDataObject( m_szURL );
  wxTheClipboard->SetData( data );
  wxTheClipboard->Close();
}

//! Updates the links colors and underline properties
void wxHyperlinkCtrl::UpdateLink(const bool bRefresh)
{
	wxFont fontTemp = GetFont();

	if( m_bVisited )
	{
		SetForegroundColour( m_crVisitedColour );
		fontTemp.SetUnderlined( m_bVisitedUnderline );
	}
	else
	{
		SetForegroundColour( m_crLinkColour );
		fontTemp.SetUnderlined( m_bLinkUnderline );
	}

	if( m_bBold )
		fontTemp.SetWeight(wxBOLD);

	SetFont( fontTemp );
	Refresh( bRefresh );
}

#ifdef __WIN95__
long wxHyperlinkCtrl::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
	if (nMsg == WM_NCHITTEST)
		return (long)HTCLIENT;

	return wxStaticText::MSWWindowProc( nMsg, wParam, lParam );
}
#endif

void wxHyperlinkCtrl::DisplayError( const wxString &szError, const bool bReportErrors )
{
	if( bReportErrors )
		wxMessageBox( szError, wxT("Hyperlink Error"), wxOK | wxCENTRE | wxICON_ERROR  );
}
