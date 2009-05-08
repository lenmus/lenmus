//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
IMPLEMENT_DYNAMIC_CLASS(lmDocument, wxDocument)

lmDocument::lmDocument()
    : m_pEditMode((lmEditorMode*)NULL)
    , m_pScore((lmScore*) NULL)
{
}

lmDocument::~lmDocument()
{
    delete m_pScore;
}

bool lmDocument::OnNewDocument()
{
    //TODO: Remove this method. It is never used. For now keep it as reference just in case
    //it could be necessary when widening the lmDocument model

    //// The default implementation calls OnSaveModified and DeleteContents, makes a default
    //// title for the document, and notifies the views that the filename (in fact, the title)
    //// has changed.


    //if (!OnSaveModified())
    //    return false;

    //if (OnCloseDocument() == false) return false;
    //DeleteContents();
    //Modify(false);
    //SetDocumentSaved(false);

    //// create an empty score
    //m_pScore = new lmScore();
    //m_pScore->AddInstrument(0,0,_T(""));			//MIDI channel 0, MIDI instr 0

    ////In scores created in the score editor, we should render a full page,
    ////with empty staves. To this end, we need to change some options default value
    //m_pScore->SetOption(_T("Score.FillPageWithEmptyStaves"), true);
    //m_pScore->SetOption(_T("StaffLines.StopAtFinalBarline"), false);

    ////Assign the score a default name
    //wxString name;
    //GetDocumentManager()->MakeDefaultName(name);
    //SetTitle(name);
    //SetFilename(name, true);

    return true;
}

bool lmDocument::OnOpenDocument(const wxString& filename)
{
    //OnOpenDocument() is invoked in three cases:
    // - Normal invocation from DocManager for opening an LDP document
    // - Special invocation from MainFrame:
    //  * For importing MusicXML files. In this case parameter filename will start
    //    with "\\<<IMPORT>>//" follewed by the filename to open
    //  * For displaying an already existing score created in the program (i.e.
    //    exercises, score wizard). Filename will start with "\\<<LOAD>>//"


    //import a MusicXML score
    if (filename.StartsWith( _T("\\<<IMPORT>>//") ))
    {
        wxString sPath = filename.substr(15);
        size_t nSize = sPath.length() - 4;
        //wxLogMessage(_T("[lmDocument::OnOpenDocument]Importing <%s>"), sPath.Left(nSize).c_str());
        return OnImportDocument(sPath.Left(nSize) );
    }

    //Open an already created score
    if (filename.StartsWith( _T("\\<<LOAD>>//") ))
    {
        wxString sID = filename.Mid(12, 6);
        long nID = 0;
        sID.ToLong(&nID);
        //wxLogMessage(_T("[lmDocument::OnOpenDocument] New score with wizard"));
        return OnDisplayCreatedScore( (int)nID );
    }

    //Normal case. Open a score from LDP file
    lmLDPParser parser;
    m_pScore = parser.ParseFile(filename);
    if (!m_pScore)
    {
        //return false;
        //BUG_BYPASS: if open file fails the program crashes. The process of closing the
        //view is not correctly implemented in lmScoreView. This requires detailed 
        //investigation. Meanwhile, instead of returning 'false', lets create an empty
        //score.
        m_pScore = new lmScore();
        m_pScore->AddInstrument(0,0,_T(""));			//MIDI channel 0, MIDI instr 0
        m_pScore->SetOption(_T("Score.FillPageWithEmptyStaves"), true);
        m_pScore->SetOption(_T("StaffLines.StopAtFinalBarline"), false);
    }

    GetMainFrame()->AddFileToHistory(filename);
    wxFileName oFN(filename);
    m_pScore->SetScoreName(oFN.GetFullName());
    //wxLogMessage(_T("[lmDocument::OnOpenDocument] Dump of score: ---------------------------"));
    //wxLogMessage( m_pScore->Dump() );
    SetFilename(filename, true);
    SetDocumentSaved(true);
    Modify(false);
    UpdateAllViews();
    return true;
}

bool lmDocument::OnImportDocument(const wxString& filename)
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

bool lmDocument::OnDisplayCreatedScore(int nID)
{
    //get the score to display
    m_pScore = GetMainFrame()->GetScoreToEdit(nID);
    if (!m_pScore) return false;

    //Assign the window a default name if no name assigned to the score;
    //otherwise assign it the name of the score
    wxString name = m_pScore->GetScoreName();
    if (name == _T(""))
    {
        GetDocumentManager()->MakeDefaultName(name);
    }
    SetTitle(name);
    SetFilename(name, true);

    Modify(false);
    UpdateAllViews();
    return true;
}

void lmDocument::UpdateAllViews(wxView* sender, wxObject* hint)
{
    //Updates all views. If sender is non-NULL, does not update this view.
    //hint represents optional information to allow a view to optimize its update.

	m_pScore->SetModified(true);
	wxDocument::UpdateAllViews(sender, hint);
}

void lmDocument::UpdateAllViews(bool fScoreModified, lmUpdateHint* pHints)
{
	m_pScore->SetModified(fScoreModified);
	wxDocument::UpdateAllViews((wxView*)NULL, pHints);
}

#if wxUSE_STD_IOSTREAM
//For Linux I can not manage to use wxStreams. Therefore, I include both alternatives
wxSTD ostream& lmDocument::SaveObject(wxSTD ostream& stream)
{
	wxDocument::SaveObject(stream);

    //TODO: Recode next sentences using std streams
//	wxTextOutputStream oTextStream(stream);
//	oTextStream << m_pScore->SourceLDP();

	return stream;
}
#else
wxOutputStream& lmDocument::SaveObject(wxOutputStream& stream)
{
	wxDocument::SaveObject(stream);

	wxTextOutputStream oTextStream(stream);
	oTextStream << m_pScore->SourceLDP();

	return stream;
}
#endif
