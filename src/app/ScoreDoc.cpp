//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
#include "ScoreView.h"
#include "TheApp.h"                     //to access the main frame.
#include "MainFrame.h"                  
#include "../ldp_parser/LDPParser.h"
#include "../xml_parser/MusicXMLParser.h"
#include "../widgets/MsgBox.h"



//implementation of lmScore Document
IMPLEMENT_DYNAMIC_CLASS(lmDocument, wxDocument)

lmDocument::lmDocument()
    : m_pEditMode((lmEditorMode*)NULL)
    , m_pScore((lmScore*) NULL)
    , m_fIsBeingEdited(false)
{
}

lmDocument::~lmDocument()
{
    delete m_pScore;

    if (m_pEditMode)
        delete m_pEditMode;
}

bool lmDocument::OnCreate(const wxString& WXUNUSED(path), long flags)
{
    //invoked from lmDocManager to do View creation. Returns true if no error
    //Overrided here to deal with View/Controller creation issues

    wxView* pView = new lmScoreView();
    pView->SetDocument(this);
    if (!pView->OnCreate(this, flags))
    {
        delete pView;
        return false;
    }
    return true;
}

bool lmDocument::OnOpenDocument(const wxString& filename)
{
    //Invoked from lmDocManager when creating a new document.
    //This method assings contents to the created document by opening a LDP file.
    //Parameter filename is the full path to LDP file to open.

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

    wxFileName oFN(filename);
    m_pScore->SetScoreName(oFN.GetFullName());
    SetFilename(filename, true);
    SetDocumentSaved(true);
    Modify(false);
    UpdateAllViews();
    return true;
}

bool lmDocument::OnImportDocument(const wxString& filename)
{
    //Invoked from lmDocManager when creating a new document.
    //This method assings contents to the created document by importing a MusicXML file.
    //Parameter filename is the full path to MusicXML file to open.

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

bool lmDocument::OnNewDocumentWithContent(lmScore* pScore)
{
    //Invoked from lmDocManager when creating a new document.
    //This method assings contents to the created document by using the received score.

    //save the score to load in the document
    m_pScore = pScore;
    wxASSERT(pScore);

    //Assign the window a default name if no name assigned to the score;
    //otherwise assign it the name of the score
    wxString name = m_pScore->GetScoreName();
    if (name == _T(""))
        name = GetDocumentManager()->MakeNewDocumentName();
    SetTitle(name);
    SetFilename(name, true);

    Modify(true);           //as it is new, to ask for saving it
    UpdateAllViews();
    return true;
}

void lmDocument::ReplaceScore(lmScore* pScore, bool fUpdateViews)
{
    //this method is intended for undo/redo based on saving full scores.
    //It receives the score that has to replace current one. Current score
    //must be deleted and all view updated.

    wxASSERT(pScore);

    //copy information from current score, before deleting it
    pScore->SetScoreName( m_pScore->GetScoreName() );

    //delete current score and update with new one
    delete m_pScore;
    m_pScore = pScore;

    //update all view
    Modify(false);
    if (fUpdateViews)
        UpdateAllViews((wxView*)NULL, new lmUpdateHint(lmHINT_NEW_SCORE));
    else
    {
        m_fIsBeingEdited = true;

        ////Invalidate saved data about the score
        //wxList& aViews = this->GetViews();
        //wxList::compatibility_iterator node = aViews.GetFirst();
        //while (node)
        //{
        //    wxView* pView = (wxView*)node->GetData();
        //    if (pView->IsKindOf(CLASSINFO(lmScoreView)))
        //        ((lmScoreView*)pView)->DeleteCaret();
        //    node = node->GetNext();
        //}
    }
}

void lmDocument::OnCustomizeController(lmEditorMode* pMode)
{
    wxASSERT(m_pScore);

    if (!pMode && !m_pScore->GetCreationMode().empty())
    {
        wxString sQuestion = _("This score was created by an exercise.");
        sQuestion += _T("\n\n");
        sQuestion += _("Would you like to continue in exercise mode?");

        lmQuestionBox oQB(sQuestion, 2,     //msge, num buttons,
            _("Exercise mode"), _("Continue in exercise mode. The toolbox will include the additional exercise tools required by the exercise"),
            _("Normal mode"), _("Continue in normal mode. No special tools will be included.")
        );
        int nAnswer = oQB.ShowModal();

		if (nAnswer == 0)   //'Exercise mode' button
        {
            pMode = new lmEditorMode( m_pScore->GetCreationMode(), m_pScore->GetCreationVers() );
        }
    }

    //force ToolBox creation if doesn't exist yet
    ((lmMainFrame*)GetMainFrame())->ShowToolBox(true);
	lmToolBox* pToolBox = ((lmMainFrame*)GetMainFrame())->GetActiveToolBox();
	wxASSERT(pToolBox);

    //sets the edit mode and creates the score processor, if necessary
    m_pEditMode = pMode;
    if (pMode)
    {
        pMode->CreateScoreProcessor();
        m_pScore->SetCreationMode(pMode->GetModeName(), pMode->GetModeVers());
    }

    //customize ToolBox
    if (pMode)
        pMode->CustomizeToolBoxPages(pToolBox);
    else
        pToolBox->SetDefaultConfiguration();

    ((lmMainFrame*)GetMainFrame())->SetFocusOnActiveView();
}

void lmDocument::UpdateAllViews(wxView* sender, wxObject* hint)
{
    //Updates all views. If sender is non-NULL, does not update that view.
    //hint represents optional information to allow a view to optimize its update.

	m_pScore->SetModified( IsModified() );
    m_fIsBeingEdited = false;
	wxDocument::UpdateAllViews(sender, hint);
}

#if wxUSE_STD_IOSTREAM
//For Linux I can not manage to use wxStreams. Therefore, I include both alternatives
wxSTD ostream& lmDocument::SaveObject(wxSTD ostream& stream)
{
	wxDocument::SaveObject(stream);
    stream << std::string( (m_pScore->SourceLDP(false)).mb_str(*wxConvCurrent) );
    return stream;
}
#else
wxOutputStream& lmDocument::SaveObject(wxOutputStream& stream)
{
	wxDocument::SaveObject(stream);

	wxTextOutputStream oTextStream(stream);
	oTextStream << m_pScore->SourceLDP(false);      //false: do not export undo data

	return stream;
}
#endif

bool lmDocument::SaveAsUnitTest()
{
    wxString sFileName = GetFilenameToSaveUnitTest();
    if (sFileName.IsEmpty())
        return false;

    SetFilename(sFileName);
    SetTitle(wxFileNameFromPath(sFileName));

    // Notify the views that the filename has changed
    wxList::compatibility_iterator node = m_documentViews.GetFirst();
    while (node)
    {
        wxView *view = (wxView *)node->GetData();
        view->OnChangeFilename();
        node = node->GetNext();
    }

    //save the score
    wxString msgTitle;
    if (!wxTheApp->GetAppName().empty())
        msgTitle = wxTheApp->GetAppName();
    else
        msgTitle = wxString(_("File error"));

#if wxUSE_STD_IOSTREAM
    wxSTD ofstream store(sFileName.mb_str(), wxSTD ios::binary);
    if (store.fail() || store.bad())
#else
    wxFileOutputStream store(sFileName);
    if (store.GetLastError() != wxSTREAM_NO_ERROR)
#endif
    {
        (void)wxMessageBox(_("Sorry, could not open this file for saving."), msgTitle, wxOK | wxICON_EXCLAMATION,
                           GetDocumentWindow());
        // Saving error
        return false;
    }

    //save the score
    if (!SaveObject(store))
    {
        (void)wxMessageBox(_("Sorry, could not save this file."), msgTitle, wxOK | wxICON_EXCLAMATION,
                           GetDocumentWindow());
        // Saving error
        return false;
    }

    //save positioning data, for tests validation
    //TODO

    return true;
}

wxString lmDocument::GetFilenameToSaveUnitTest()
{
    wxDocTemplate *docTemplate = GetDocumentTemplate();
    if (!docTemplate)
        return wxEmptyString;

    wxString defaultDir = docTemplate->GetDirectory();
    if (defaultDir.IsEmpty())
        defaultDir = wxPathOnly(GetFilename());

    wxString tmp = wxFileSelector(_("Save as"),
            defaultDir,
            wxFileNameFromPath(GetFilename()),
            _T("lmt"),
            _T("*.lmt"),
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
            GetDocumentWindow());

    if (tmp.empty())
        return wxEmptyString;

    wxString fileName(tmp);
    wxString path, name, ext;
    wxSplitPath(fileName, & path, & name, & ext);

    if (ext.empty())
        fileName += _T(".lmt");

    return fileName;
}
