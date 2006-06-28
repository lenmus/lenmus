// RCS-ID: $Id: ScoreDoc.cpp,v 1.3 2006/02/23 19:17:49 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
//-------------------------------------------------------------------------------------
/*! @file ScoreDoc.cpp
    @brief Implementation file for class lmScoreDocument
    @ingroup app_gui
*/
#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/txtstrm.h"

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/filedlg.h"            // for File Selector Dialog
#include "ScoreDoc.h"
//#include "textView.h"
#include "scoreView.h"
#include "../ldp_parser/LDPParser.h"
#include "../xml_parser/XMLParser.h"



//implementation of lmScore Document
IMPLEMENT_DYNAMIC_CLASS(lmScoreDocument, wxDocument)

lmScoreDocument::lmScoreDocument()
{
    // default values
    m_paperSize.SetWidth(2100);        // DIN A4  21.0 x 29.7 cm
    m_paperSize.SetHeight(2970);
    m_pScore = (lmScore*) NULL;

}

lmScoreDocument::~lmScoreDocument()
{
    delete m_pScore;

}

lmScoreObj* lmScoreDocument::FindSelectableObject(wxPoint& pt)
{
    return m_pScore->FindSelectableObject(pt);
}

bool lmScoreDocument::OnNewDocument()
{
    // The default implementation calls OnSaveModified and DeleteContents, makes a default
    // title for the document, and notifies the views that the filename (in fact, the title)
    // has changed.


    if (!OnSaveModified())
        return false;

    if (OnCloseDocument()==false) return false;
    DeleteContents();
    Modify(false);
    SetDocumentSaved(false);

    // create an empty score
    m_pScore = new lmScore();
    m_pScore->AddInstrument(1,0,0,_T(""));        //one vstaff, MIDI channel 0, MIDI instr 0

    wxString name;
    GetDocumentManager()->MakeDefaultName(name);
    SetTitle(name);
    SetFilename(name, true);

    return true;
}



bool lmScoreDocument::OnOpenDocument(const wxString& filename)
{
    /*
    OpenDocument is called both for opening an LDP document and for importing MusicXML files.
    In this second case parameter filename will start with "\\<<IMPORT>>//" follewed by the
    filename to open
    */

    if (filename.StartsWith( _T("\\<<IMPORT>>//") ))
    {
        wxString sPath = filename.Mid(15);
        size_t nSize = sPath.Length() - 4;
        wxLogMessage(_T("Importing <%s>"), sPath.Left(nSize));
        return OnImportDocument(sPath.Left(nSize) );
    }

    lmLDPParser parser;
    m_pScore = parser.ParseFile(filename);
    if (!m_pScore) return false;

    SetFilename(filename, true);
    Modify(false);
    UpdateAllViews();
    return true;
}

bool lmScoreDocument::OnImportDocument(const wxString& filename)
{
    lmXMLParser parser;
    m_pScore = parser.ParseMusicXMLFile(filename);
    if (!m_pScore) return false;

    SetFilename(filename, true);
    Modify(false);
    UpdateAllViews();
    return true;
}
