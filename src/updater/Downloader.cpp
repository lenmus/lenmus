//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed pInput the hope that it will be useful, but WITHOUT ANY 
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
//    This file is based pInput code taken from package 'WebDownloader'
//    by Francesco Montorsi:
//          Name:        download.cpp
//          Purpose:     lmDownloadThread, wxSizeCacherThread
//          Author:      Francesco Montorsi
//          Created:     2005/06/23
//          RCS-ID:      $Id: download.cpp,v 1.40 2005/11/12 14:00:24 frm Exp $
//          Copyright:   (c) 2005 Francesco Montorsi
//          Licence:     wxWidgets licence
//-------------------------------------------------------------------------------------
/*! @file Downloader.h
    @brief Header file for class lmDownloadThread
    @ingroup updates_management
*/



// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// includes
#ifndef WX_PRECOMP
#include "wx/log.h"
#endif

#include <wx/wfstream.h>
#include <wx/filename.h>

#include "Downloader.h"


//// wxWidgets RTTI
//DEFINE_EVENT_TYPE(wxEVT_COMMAND_DOWNLOAD_COMPLETE);
//DEFINE_EVENT_TYPE(wxEVT_COMMAND_CACHESIZE_COMPLETE);
//
//
//
//
////wxFileName wxGetFileNameFromURI(const wxString &uri)
////{
////    // remove the file: prefix
////    wxString path(uri);
////    path.Remove(0, wxString(wxT("file:")).Len());
////
////    // now just build a wxfilename
////    return wxFileName(path);            // URIs always use the '/' directory separator
////}
////
////wxString wxMakeFileURI(const wxFileName &fn)
////{
////    wxString path = fn.GetFullPath();
////
////    // pInput case we are using win32 paths with backslashes...
////    // (this must be done also under Unix since even there we could
////    //  be forced to handle win32 paths)
////    path.Replace(wxT("\\"), wxT("/"));
////
////    // now use wxURI as filter
////    return wxURI(wxT("file:") + path).BuildURI();
////}
//
//wxInputStream *lmGetInputStreamFromURI(const wxString& sUri)
//{
//    wxInputStream* pInput;
//
//    if (sUri.StartsWith(_T("file:"))
//    {
//        // we can handle file:// protocols ourselves
//        wxLogMessage(wxT("lmGetInputStreamFromURI - using wxFileInputStream"));
//        wxURI u(sUri);
//        pInput = new wxFileInputStream(u.GetPath());
//    }
//    else {
//        // we won't directly use wxURL because it must be alive together with
//        // the wxInputStream it generates... lmURLInputStream solves this problem
//        wxLogMessage(wxT("lmGetInputStreamFromURI - using wxURL"));
//        pInput = new lmURLInputStream(sUri);
//    }
//
//    return pInput;
//}

//unsigned long wxGetSizeOfURI(const wxString &uri)
//{
//    wxLogDebug(wxT("wxGetSizeOfURI - getting size of [") + uri + wxT("]"));
//    wxInputStream *is = lmGetInputStreamFromURI(uri);
//    if (is == NULL) {
//        wxLogDebug(wxT("wxGetSizeOfURI - aborting; invalid URL !"));
//        return 0;
//    }
//
//    if (!is->IsOk()) {
//        wxLogDebug(wxT("wxGetSizeOfURI - aborting; invalid URL !"));
//        delete is;          // be sure to avoid leaks
//        return 0;
//    }
//
//    // intercept the 302 HTTP "return code"
//#if 0
//    wxProtocol &p = u.GetProtocol();
//    wxHTTP *http = wxDynamicCast(&p, wxHTTP);
//    if (http != NULL && http->GetResponse() == 302) {
//        wxLogMessage(wxT("wxGetSizeOfURI - can't get the size of the resource located at [") +
//            uri + wxT("] because the request has been redirected... update your URL"));
//        return 0;
//    }
//#endif
//
//    unsigned long sz = (unsigned long)is->GetSize();
//    delete is;
//
//    // see wxHTTP::GetInputStream docs
//    if (sz == 0xffffffff)
//        sz = 0;
//    return sz;
//}
//



//-------------------------------------------------------------------------------------
// Implementation of lmDownloadThread
//-------------------------------------------------------------------------------------

//// this macro avoids the repetion of a lot of code
//#define lmABORT_DOWNLOAD(msg) {                                      \
//            wxLogMessage(wxT("lmDownloadThread::Entry - ") +             \
//                wxString(msg) + wxT(" - DOWNLOAD ABORTED !!!"));        \
//            m_bSuccess = false;                                         \
//            m_mStatus.Lock();                                           \
//            m_nStatus = lmTWS_IDDLE;                                  \
//            m_mStatus.Unlock();                                         \
//            wxPostEvent(m_pHandler, updatevent);                        \
//            continue;                                                   \
//    }
//

#define lmABORT_DOWNLOAD(msg) {                                      \
            wxLogMessage(_T("[lmDownloadThread::Entry] - ") +             \
                wxString(msg) + wxT(" - DOWNLOAD ABORTED !!!"));        \
            m_nStatus = lmTWS_IDDLE;                                  \
            continue;                                                   \
    }

lmDownloadThread::lmDownloadThread(wxEvtHandler* dlg) : wxThread(wxTHREAD_JOINABLE)
{
//    m_pHandler = dlg;
//    m_bSuccess = false; 
//    m_dtStart = wxDateTime::UNow(); 
//    m_nFileCount=0; 
    m_nStatus = lmTWS_IDDLE; 
//    m_nFinalSize = 0; 
//    m_nCurrentSize = 0; 
//    m_nCurrentIndex = 0; 
//    m_bReady = false;
}

lmDownloadThread::~lmDownloadThread()
{
}

void *lmDownloadThread::Entry()
{
//    /*
//    Downloads the file and then sends the wxDT_NOTIFICATION event
//    to the m_pHandler event handler.
//    Also sets the m_bSuccess flag before exiting.
//    */
//
//    // we'll use wxPostEvent to post this event since this is the
//    // only thread-safe way to post events !
//    wxCommandEvent updatevent(wxEVT_COMMAND_DOWNLOAD_COMPLETE);

    // begin loop
    while (!TestDestroy()) {

        if (m_nStatus == lmTWS_IDDLE) {
            //m_bReady = true;
            wxThread::Sleep(100);
            continue;
        }

        // when status is changed to DOWNLOAD (in method 'StartDownload' execution
        // continues here

//        // reset our variables
//        m_nFinalSize = 0;
//        m_nCurrentSize = 0;
//
//        // we are starting the download of a file; update our datetime field
//        m_dtStart = wxDateTime::UNow();

        wxLogMessage(_T("[lmDownloadThread::Entry] Downloading ") + m_sUrl);

        // ensure we can build a wxURL from the given URL
        wxURL oURL(m_sUrl);
        if (oURL.GetError() != wxURL_NOERR) {
            lmABORT_DOWNLOAD(wxT("Not valid URL: ")
                wxT("url is [") + m_sUrl + wxT("]"));
        }
        oURL.GetProtocol().SetTimeout(30);              // 30 sec
        wxInputStream* pInput = oURL.GetInputStream();


        // check input stream
        if (!pInput) {
            // something is wrong with the input URL...
            lmABORT_DOWNLOAD(wxT("Something is wrong with the input URL; ")
                wxT("url is [") + m_sUrl + wxT("]"));
        }
        if (!pInput->IsOk()) {
            delete pInput;
            lmABORT_DOWNLOAD(wxT("Cannot init the INPUT stream"));
        }

        //start download
//        wxFileOutputStream pOut(m_strOutput);
//        if (!pOut.IsOk()) {
//            delete pInput;
//            lmABORT_DOWNLOAD(wxT("Cannot open/init the OUPUT stream [")
//                                + m_strOutput + wxT("]"));
//        }
//        m_nFinalSize = pInput->GetSize();
//
//        // see wxHTTP docs
//        if (m_nFinalSize == 0xffffffff)
//            m_nFinalSize = 0;
//
//        // write the downloaded stuff pInput the output file
//        // without using the
//        //      pOut.Write(*pInput);
//        // command; that would be easier but would not allow
//        // the program to stop this thread while downloading
//        // the file since the TestDestroy() function would not
//        // be called pInput that way...
//        char buf[wxDT_BUF_TEMP_SIZE];
//        while (!TestDestroy() && m_nStatus == lmTWS_DOWNLOADING) {
//            size_t bytes_read = pInput->Read(buf, WXSIZEOF(buf)).LastRead();
//            if ( !bytes_read )
//                break;
//
//            if ( pOut.Write(buf, bytes_read).LastWrite() != bytes_read )
//                break;
//
//            // update our downloaded bytes var
//            m_nCurrentSize = pOut.GetSize();
//
//        }

        // we don't need the input stream anymore
        delete pInput;

//        // if m_nFinalSize is set to zero, then we cannot trust it;
//        // we must consider the size of the remote file as unavailable
//        // since the wxHTTP protocol does not allow us to get it...
//        if (!pOut.IsOk() || pOut.GetSize() == 0 ||
//            (pOut.GetSize() != m_nFinalSize && m_nFinalSize != 0))
//            lmABORT_DOWNLOAD(wxT("Output FILE stream size is wrong"));
//
//        wxLogMessage(_T("[lmDownloadThread::Entry] Download completed download of %lu bytes"),
//                        m_nCurrentSize);
//
//        // we have successfully download the file
//        m_bSuccess = true;


        wxLogMessage(_T("[lmDownloadThread::Entry] Download completed."));
        {
            // go pInput wait mode
            //wxMutexLocker locker(m_mStatus);
            m_nStatus = lmTWS_IDDLE;
        }

//        wxPostEvent(m_pHandler, updatevent);
//        m_nFileCount++;
//
//        // we reset our variables here because there is a delay between the
//        // lmDownloadThread::StartDownload() calls and the execution of the
//        // first statements of this thread...
//        m_nCurrentSize = 0;
//        m_nFinalSize = 0;
    }

    return (void*)false;

}

void lmDownloadThread::StartDownload()
{
    /*  Starts a new download.
        This function must be called only when this thread is iddle
    */

	//wxASSERT(m_bReady && !IsDownloading() && !IsComputingMD5());
	wxASSERT(m_nStatus == lmTWS_IDDLE);
	//wxMutexLocker lock(m_mStatus);
	m_nStatus = lmTWS_DOWNLOADING;		
}

//wxString lmDownloadThread::GetDownloadSpeed() const
//{
//    wxASSERT(IsDownloading());
//    wxLongLong msec = GetElapsedMSec();
//    if (msec <= 0)
//        return wxT("0 KB/s");       // avoid division by zero
//
//    wxLongLong nBytesPerMilliSec = wxLongLong(GetCurrDownloadedBytes()) / msec;
//
//    // we don't like bytes per millisecond as measure unit !
//    long nKBPerSec = (nBytesPerMilliSec * 1000/1024).ToLong();          // our conversion factor
//    return wxString::Format(wxT("%li KB/s"), nKBPerSec);
//}
//
//wxString lmDownloadThread::GetRemainingTime() const
//{
//    wxASSERT(IsDownloading());
//    wxLongLong sec = GetElapsedMSec()/1000;
//    if (sec <= 0)
//        return wxT("not available");        // avoid division by zero
//
//    // remaining time is the number of bytes we still need to download
//    // divided by our download speed...
//    wxLongLong nBytesPerSec = wxLongLong(GetCurrDownloadedBytes()) / sec;
//    if (nBytesPerSec <= 0)
//        return wxT("not available");        // avoid division by zero
//
//    long remsec = (wxLongLong(m_nFinalSize-GetCurrDownloadedBytes())/nBytesPerSec).ToLong();
//    if (remsec < 0)
//        return wxT("not available");
//
//    if (remsec < 60)
//        return wxString::Format(wxT("%li sec"), remsec);
//    else if (remsec < 60*60)
//        return wxString::Format(wxT("%li min, %li sec"), remsec/60, remsec%60);
//    else if (remsec < 60*60*24)
//        return wxString::Format(wxT("%li hours, %li min, %li sec"),
//                    remsec/3600, (remsec/60)%60, (remsec/3600)%60);
//    else
//        return wxT("not available");
//}
//
//
//
