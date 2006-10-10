/////////////////////////////////////////////////////////////////////////////
// Name:        hyperlinkctrl.h
// Purpose:     Hyperlink control for wxWidgets
//				Features include the ability to perform your own Left, Middle, and Right 
//				click events to perform your own custom event handling and ability to open
//				link in a new or current browser window.
// Author:      Angelo Mandato
// Created:     2005/08/10
// RCS-ID:      $Id: hyperlinkctrl.h,v 1.2 2005/08/12 03:58:52 amandato Exp $
// Copyright:   (c) 2005 Angelo Mandato (http://www.spaceblue.com)
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

/*////////////////////////////////////////////////
	/// Contributors ///
	Name:			Mark McManus
	Email:			mmcmanus@scientificmetrics.com
	Notes:			Contributed code for the EVT_COMMAND_LINK_CLICKED event
					which is fired when AutoBrowse(false) is set and added
					DoPopup(false) to supress pop up menu to copy web link.
					Thanks Mark!
	
*/////////////////////////////////////////////////

#ifndef _WX_HYPERLINKCTRL_H_
#define _WX_HYPERLINKCTRL_H_

// optimization for GCC
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "hyperlinkctrl.h"
#endif

// Define below if you simply want to use wxCommandEvents for single click
// click events on your links
//#define HYPERLINKCTRL_COMMANDEVENT

#include <wx/wx.h>
#include "wx/httpenginedef.h"

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_HTTPENGINE, wxEVT_COMMAND_LINK_CLICKED, 7771)
#ifndef HYPERLINKCTRL_COMMANDEVENT
  DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_HTTPENGINE, wxEVT_COMMAND_LINK_MCLICKED, 7772)
  DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_HTTPENGINE, wxEVT_COMMAND_LINK_RCLICKED, 7773)
#endif
END_DECLARE_EVENT_TYPES()


#ifdef HYPERLINKCTRL_COMMANDEVENT

#define EVT_LINK(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_COMMAND_LINK_CLICKED, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
        (wxObject *) NULL \
    ),

#else

class WXDLLIMPEXP_HTTPENGINE wxHyperlinkEvent : public wxCommandEvent
{
public:
  wxHyperlinkEvent( WXTYPE eventType = wxEVT_COMMAND_LINK_RCLICKED, int id = 0 ) : wxCommandEvent( eventType, id ) { };

	virtual wxEvent *Clone() const { return new wxHyperlinkEvent(*this);  };

	void SetPosition( const wxPoint &pos ) { m_pos = pos; };
	wxPoint GetPosition(void) { return m_pos; };
private:
	wxPoint	m_pos;
};

typedef void (wxEvtHandler::*wxHyperlinkEventFunction)(wxHyperlinkEvent);

// Event types for wxHyperlinkCtrl:
#define EVT_LINK(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_COMMAND_LINK_CLICKED, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxHyperlinkEventFunction)&fn, \
        (wxObject *) NULL \
    ),
#define EVT_LINK_MCLICKED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_COMMAND_LINK_MCLICKED, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxHyperlinkEventFunction)&fn, \
        (wxObject *) NULL \
    ),
#define EVT_LINK_RCLICKED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_COMMAND_LINK_RCLICKED, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxHyperlinkEventFunction)&fn, \
        (wxObject *) NULL \
    ),

#endif

enum
{
	HYPERLINKS_POPUP_COPY = 1000,
};

// wxHyperlinkCtrl class
class WXDLLIMPEXP_HTTPENGINE wxHyperlinkCtrl : public wxStaticText
{
public:

#ifdef __WIN95__
	long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif

	// Constructor
	wxHyperlinkCtrl( wxWindow *parent, wxWindowID id, const wxString &label,
			const wxPoint &pos, const wxSize &size, int style = 0, const wxString& name = wxT("staticText"), const wxString& szURL = wxEmptyString);

	// Link Colours properties
	void SetColours( const wxColour &link = wxColour( wxT("BLUE") ), const wxColour &visited  = wxColour(wxT("VIOLET")), const wxColour &rollover = wxColour( wxT("BLUE") ))
	{
		m_crLinkColour = link;
		m_crVisitedColour = visited;
		m_crLinkRolloverColor = rollover;
	};
	void GetColous( wxColour &link, wxColour &visited, wxColour &rollover )
	{
		link = m_crLinkColour;
		visited = m_crVisitedColour;
		rollover = m_crLinkRolloverColor;
	};

	// Underline properties
	void SetUnderlines( const bool link = true, const bool visited = true, const bool rollover = true )
	{
		m_bLinkUnderline = link;
		m_bRolloverUnderline = rollover;
		m_bVisitedUnderline = visited;
	};
	void GetUnderlines( bool link, bool visited, bool rollover )
	{
		link = m_bLinkUnderline;
		rollover = m_bRolloverUnderline;
		visited = m_bVisitedUnderline;
	};

	// Set Link Cursor properties
	void SetLinkCursor( const wxCursor &cur = wxCURSOR_HAND) { m_crHand = cur; };
	void GetLinkCursor( wxCursor &cur ) { cur = m_crHand; };

	// Visited properties
	void SetVisited( const bool bVisited = false ) { m_bVisited = bVisited; };
	bool GetVisited( void ) { return m_bVisited; }

	// Bold font properties
	void SetBold( const bool bBold = false) { m_bBold = bBold; };
	bool GetBold( void ) { return m_bBold; };

	// URL properties
	void SetURL( const wxString &szURL ) { m_szURL = szURL; }
	wxString GetURL( void ) { return m_szURL; }

  // Open in same window property
  void OpenInSameWindow( const bool bIfPossible = false ) { m_bSameWinIfPossible = bIfPossible; };

	// Broswer Path
	void SetBrowserPath( const wxString &browser ) { m_szBrowserPath = browser; };
	wxString GetBrowserPath( void ) { return m_szBrowserPath; };

  // Roll over properties
	void EnableRollover( const bool bEnable = false ) { m_bEnableRollover = bEnable; };

	// Report errors property
	void ReportErrors( const bool bReport = true ) { m_bReportErrors = bReport; };

//**Added By Mark McManus
  //AutoBrowse property
  void AutoBrowse( const bool bBrowse = true ){ m_bAutoBrowse = bBrowse; };

  //DoPopup property
  void DoPopup( const bool bPopup = true ) { m_bDoPopup = bPopup; };

//**Mark McManus

	// Goto URL
	static bool GotoURL( const wxString &szUrl, const wxString &szBrowser = wxEmptyString, const bool bReportErrors = false, const bool bSameWinIfPossible = false );

	// Refresh link properties
	void UpdateLink( const bool bRefresh = true );

private:
	// Copy URL event
	void OnPopUpCopy( wxCommandEvent &event );

	// Mouse event
	void OnMouseEvent( wxMouseEvent &event );

	// ErrorMmessage
	static void DisplayError( const wxString &szError, const bool bReportErrors );
	
	// The URL to goto.
	wxString m_szURL;

	// Browser Path
	wxString m_szBrowserPath;

	// Colours
	wxColour m_crLinkColour;
	wxColour m_crVisitedColour;
	wxColour m_crLinkRolloverColor;

	// Underlines
	bool m_bLinkUnderline;
	bool m_bRolloverUnderline;
	bool m_bVisitedUnderline;

	// Rollover enabled?
	bool m_bEnableRollover;

	// Visited link?
	bool m_bVisited;

	// Make link bold?
	bool m_bBold;

	// Specify a cursor
	wxCursor m_crHand;

	// Should Control report errors in dialog?
	bool m_bReportErrors;

//**Added By Mark McManus

  //Should Control browse to the URL (if false just fire wxEVT_COMMAND_LINK_CLICKED)
  bool m_bAutoBrowse;

  //Should the control show the Copy popup menu (if false just fire wxEVT_COMMAND_RLINK_CLICKED)
  bool m_bDoPopup;
//**Mark McManus

  bool m_bSameWinIfPossible;

  wxMenu *m_menuPopUp;

  DECLARE_EVENT_TABLE()
};

#endif
