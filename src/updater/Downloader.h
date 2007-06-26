//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street,
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//    This file is based in code taken from package 'WebDownloader'
//    by Francesco Montorsi:
//          Name:        download.h
//          Purpose:     lmDownloadThread, wxSizeCacherThread
//          Author:      Francesco Montorsi
//          Created:     2005/07/14
//          RCS-ID:      $Id: download.h,v 1.22 2005/10/20 16:06:01 frm Exp $
//          Copyright:   (c) 2005 Francesco Montorsi
//          Licence:     wxWidgets licence
//-------------------------------------------------------------------------------------

#ifndef __DOWNLOADER_H__
#define __DOWNLOADER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Downloader.cpp"
#endif

//headers
#include <wx/url.h>
#include <wx/datetime.h>


//possible values  for the m_nStatus variable
enum mThreadWorkStatus {
	lmTWS_IDDLE = 0,
	lmTWS_DOWNLOADING,
};



////! The thread helper which downloads the webupdate script and/or packages.
////! This class is generic and can be used to download anything you need
////! from any URL.
////! It already provides some useful functions like #GetDownloadSpeed() and
////! #GetRemainingTime() which helps you to create dialogs with progress bars
////! and/or time indicators.

class lmDownloadThread : public wxThread
{
public:
	lmDownloadThread(wxEvtHandler *dlg = NULL);
	~lmDownloadThread();

    // thread execution starts here
    void* Entry();

    void SetURL(wxString sUrl) { m_sUrl = sUrl; }

        // current status

	//! Returns TRUE if this thread is downloading a file.
	bool IsDownloading() const
		{ return (IsRunning() && m_nStatus == lmTWS_DOWNLOADING); }

	//! Returns TRUE if this thread is running but it's not downloading anything.
	bool IsWaiting() const
		{ return (IsRunning() && m_nStatus == lmTWS_IDDLE); }

//        // miscellaneous

	void StartDownload();

//	//! Aborts the current download.
//	void AbortDownload() {
//		wxASSERT(IsDownloading() || IsComputingMD5());
//		wxMutexLocker lock(m_mStatus);
//		m_nStatus = lmTWS_IDDLE;
//	}
//
//        // to avoid accessing methods (these vars are only read by this thread;
//        // they are never written and so they are not protected with mutexes).
//
//	//! The wxEvtHandler which will receive our wxDT_NOTIFICATION events.
//	wxEvtHandler *m_pHandler;


//	//! The name of the file where the downloaded file will be saved.
//	wxString m_strOutput;
//
//    // information for error messages
//	wxString    m_strResName;       //name of the resource being downloaded
//	wxString    m_strID;            //ID of the resource being downloaded
//
//
//protected:
//
//	wxDateTime      m_dtStart;          //time when the download started.
//	unsigned long   m_nCurrentSize;     //bytes currently downloaded
//	unsigned long   m_nFinalSize;       //final size of the download
//	bool            m_bSuccess;         //state of last download
//	int             m_nFileCount;   	//number of files downloaded
//	bool            m_bReady;

    //variables protected by mutexes

	mThreadWorkStatus   m_nStatus;  //current working status (iddle, downloading, ...)
	wxMutex             m_mStatus;  //mutex over previous variable

//	int         m_nCurrentIndex;    //current element being downloaded
//	wxMutex     m_mIndex;           //mutex over previous variable

private:
    wxString        m_sUrl;     //URI of the resource to download

};

#endif // __DOWNLOADER_H__

