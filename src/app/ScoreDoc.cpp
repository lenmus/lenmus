//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ScoreDoc.h"
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
#include "wx/filename.h"

#include "ScoreDoc.h"
//#include "ScoreView.h"
#include "TheApp.h"                     //to access the main frame. Used in OnNewScoreWithWizard()
#include "MainFrame.h"                  //to get the score created with the ScoreWizard
#include "../ldp_parser/LDPParser.h"
#include "../xml_parser/MusicXMLParser.h"



//implementation of lmScore Document
IMPLEMENT_DYNAMIC_CLASS(lmScoreDocument, wxDocument)

lmScoreDocument::lmScoreDocument()
{
    // default values
    m_pScore = (lmScore*) NULL;

}

lmScoreDocument::~lmScoreDocument()
{
    delete m_pScore;

}

bool lmScoreDocument::OnNewDocument()
{
    // The default implementation calls OnSaveModified and DeleteContents, makes a default
    // title for the document, and notifies the views that the filename (in fact, the title)
    // has changed.


    if (!OnSaveModified())
        return false;

    if (OnCloseDocument() == false) return false;
    DeleteContents();
    Modify(false);
    SetDocumentSaved(false);

    // create an empty score
    m_pScore = new lmScore();
    m_pScore->AddInstrument(0,0,_T(""));			//MIDI channel 0, MIDI instr 0
    //lmInstrument* pInstr = m_pScore->AddInstrument(0,0,_T(""));			//MIDI channel 0, MIDI instr 0
    //lmVStaff *pVStaff = pInstr->GetVStaff();
	//pVStaff->AddBarline(lm_eBarlineEOS, true);

    //In scores created in the score editor, we should render a full page, 
    //with empty staves. To this end, we need to change some options default value
    m_pScore->SetOption(_T("Score.FillPageWithEmptyStaves"), true);
    m_pScore->SetOption(_T("StaffLines.StopAtFinalBarline"), false);

    //Assign the score a default name
    wxString name;
    GetDocumentManager()->MakeDefaultName(name);
    SetTitle(name);
    SetFilename(name, true);

    return true;
}



bool lmScoreDocument::OnOpenDocument(const wxString& filename)
{
    //OnOpenDocument() is invoked in three cases:
    // - Normal invocation from DocManager for opening an LDP document
    // - Special invocation from MainFrame:
    //  * For importing MusicXML files. In this case parameter filename will start
    //      with "\\<<IMPORT>>//" follewed by the filename to open
    //  * For displaying a new score created with the Score Wizard. In this case parameter
    //      filename will start with "\\<<NEW_WIZARD>>//"


    //import a MusicXML score
    if (filename.StartsWith( _T("\\<<IMPORT>>//") ))
    {
        wxString sPath = filename.substr(15);
        size_t nSize = sPath.length() - 4;
        //wxLogMessage(_T("[lmScoreDocument::OnOpenDocument]Importing <%s>"), sPath.Left(nSize).c_str());
        return OnImportDocument(sPath.Left(nSize) );
    }

    //Open a score created with the score wizard
    if (filename.StartsWith( _T("\\<<NEW_WIZARD>>//") ))
    {
        //wxLogMessage(_T("[lmScoreDocument::OnOpenDocument] New score with wizard"));
        return OnNewScoreWithWizard();
    }

    //Normal case. Open a score from LDP file
    lmLDPParser parser;
    m_pScore = parser.ParseFile(filename);
    if (!m_pScore) return false;

    wxFileName oFN(filename);
    m_pScore->SetScoreName(oFN.GetFullName());
    SetFilename(filename, true);
    SetDocumentSaved(true);
    Modify(false);
    UpdateAllViews();
    return true;
}

bool lmScoreDocument::OnImportDocument(const wxString& filename)
{
    lmMusicXMLParser parser;
    m_pScore = parser.ParseMusicXMLFile(filename);
    if (!m_pScore) return false;

    wxFileName oFN(filename);
    m_pScore->SetScoreName(oFN.GetFullName());
    SetFilename(filename, true);
    Modify(false);
    UpdateAllViews();
    return true;
}

bool lmScoreDocument::OnNewScoreWithWizard()
{
    m_pScore = GetMainFrame()->GetWizardScore();       
    if (!m_pScore) return false;

    //Assign the score a default name
    wxString name;
    GetDocumentManager()->MakeDefaultName(name);
    SetTitle(name);
    SetFilename(name, true);

    Modify(false);
    UpdateAllViews();
    return true;
}

void lmScoreDocument::UpdateAllViews(wxView* sender, wxObject* hint)
{
    //Updates all views. If sender is non-NULL, does not update this view.
    //hint represents optional information to allow a view to optimize its update.

	m_pScore->SetModified(true);
	wxDocument::UpdateAllViews(sender, hint);
}

void lmScoreDocument::UpdateAllViews(bool fScoreModified, lmUpdateHint* pHints)
{
	m_pScore->SetModified(fScoreModified);
	wxDocument::UpdateAllViews((wxView*)NULL, pHints);
}


wxOutputStream& lmScoreDocument::SaveObject(wxOutputStream& stream)
{
	wxDocument::SaveObject(stream);

	wxTextOutputStream oTextStream(stream);
	oTextStream << m_pScore->SourceLDP();

	return stream;
}
