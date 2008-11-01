//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "DlgEditText.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "DlgEditText.h"
#include "../Text.h"
#include "../../app/ArtProvider.h"
#include "../../app/ScoreCanvas.h"


//--------------------------------------------------------------------------------------
/// Implementation of lmDlgEditText
//--------------------------------------------------------------------------------------

#define lmEDIT_CUT 1000
#define lmEDIT_COPY 1001
#define lmEDIT_PASTE 1002
#define lmEDIT_BOLD 1003
#define lmEDIT_ITALIC 1004
#define lmEDIT_UNDERLINED 1005
#define lmEDIT_FONT_NAME 1006
#define lmEDIT_FONT_SIZE 1007


BEGIN_EVENT_TABLE(lmDlgEditText, wxDialog)
    EVT_MENU(lmEDIT_BOLD, lmDlgEditText::OnBold)
    EVT_MENU(lmEDIT_ITALIC, lmDlgEditText::OnItalic)
    EVT_MENU(lmEDIT_UNDERLINED, lmDlgEditText::OnUnderline)

    EVT_BUTTON(wxID_OK, lmDlgEditText::OnAccept)
    EVT_BUTTON(wxID_CANCEL, lmDlgEditText::OnCancel)
END_EVENT_TABLE()


lmDlgEditText::lmDlgEditText(lmController* pController, lmScoreText* pParentText)
    : wxDialog(pController, wxID_ANY, _("Edit text"), wxDefaultPosition, wxSize(400, 250),
               wxDEFAULT_DIALOG_STYLE )
{
    m_pController = pController;
    m_pParentText = pParentText;
    CreateControls();

    //load bitmaps for tools

    //load current text
    m_pTxtCtrl->SetValue(m_pParentText->GetText());

    //show dlg
	this->Layout();
	this->Centre( wxBOTH );
}

void lmDlgEditText::CreateControls()
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	m_pToolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL );
    wxSize nSize(16, 16);
    m_pToolBar->SetToolBitmapSize(nSize);
    m_pToolBar->AddTool(lmEDIT_COPY, _T("Copy"),
            wxArtProvider::GetBitmap(_T("tool_copy"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_copy_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Copy"));
    m_pToolBar->AddTool(lmEDIT_CUT, _T("Cut"),
            wxArtProvider::GetBitmap(_T("tool_cut"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_cut_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Cut"));
    m_pToolBar->AddTool(lmEDIT_PASTE, _T("Paste"),
            wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_paste_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Paste"));
	m_pToolBar->AddSeparator();

    m_pToolBar->AddTool(lmEDIT_BOLD, _T("Paste"),
            wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_paste_dis"), wxART_TOOLBAR, nSize),
            wxITEM_CHECK, _("Paste"));
    m_pToolBar->AddTool(lmEDIT_ITALIC, _T("Paste"),
            wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_paste_dis"), wxART_TOOLBAR, nSize),
            wxITEM_CHECK, _("Paste"));
    m_pToolBar->AddTool(lmEDIT_UNDERLINED, _T("Paste"),
            wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_paste_dis"), wxART_TOOLBAR, nSize),
            wxITEM_CHECK, _("Paste"));
	m_pToolBar->AddSeparator();

	m_pColourPicker = new wxColourPickerCtrl( m_pToolBar, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_pColourPicker->SetToolTip( _("Text color selector") );

	m_pToolBar->AddControl( m_pColourPicker );
	m_pToolBar->AddSeparator();
	wxArrayString m_pCboFontNameChoices;
	m_pCboFontName = new wxChoice( m_pToolBar, lmEDIT_FONT_NAME, wxDefaultPosition, wxSize( 135,-1 ), m_pCboFontNameChoices, 0 );
	m_pCboFontName->SetSelection( 0 );
	m_pToolBar->AddControl( m_pCboFontName );
	wxArrayString m_pCboFontSizeChoices;
	m_pCboFontSize = new wxChoice( m_pToolBar, lmEDIT_FONT_SIZE, wxDefaultPosition, wxSize( 45,-1 ), m_pCboFontSizeChoices, 0 );
	m_pCboFontSize->SetSelection( 0 );
	m_pToolBar->AddControl( m_pCboFontSize );

	m_pToolBar->Realize();

	pMainSizer->Add( m_pToolBar, 0, wxEXPAND, 5 );

	m_pTxtCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add( m_pTxtCtrl, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* pButtonsSizer;
	pButtonsSizer = new wxBoxSizer( wxHORIZONTAL );


	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pBtAccept = new wxButton( this, wxID_OK, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtAccept, 0, wxALL, 5 );


	pButtonsSizer->Add( 0, 0, 2, wxEXPAND, 5 );

	m_pBtCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtCancel, 0, wxALL, 5 );


	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	pMainSizer->Add( pButtonsSizer, 0, wxEXPAND, 5 );

	this->SetSizer( pMainSizer );
}

lmDlgEditText::~lmDlgEditText()
{
}

void lmDlgEditText::OnBold(wxCommandEvent& event)
{
    wxFont font = m_pTxtCtrl->GetFont();

    font.SetWeight(event.IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    DoChangeFont(font);
}

void lmDlgEditText::OnItalic(wxCommandEvent& event)
{
    wxFont font = m_pTxtCtrl->GetFont();

    font.SetStyle(event.IsChecked() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    DoChangeFont(font);
}

void lmDlgEditText::OnUnderline(wxCommandEvent& event)
{
    wxFont font = m_pTxtCtrl->GetFont();

    font.SetUnderlined(event.IsChecked());
    DoChangeFont(font);
}

void lmDlgEditText::DoChangeFont(const wxFont& font, const wxColour& color)
{
    m_pTxtCtrl->SetFont(font);
    if ( color.Ok() )
        m_pTxtCtrl->SetForegroundColour(color);
    Refresh();
}

//void lmDlgEditText::DoResizeFont(int diff)
//{
//    wxFont font = m_pTxtCtrl->GetFont();
//
//    font.SetPointSize(font.GetPointSize() + diff);
//    DoChangeFont(font);
//}


void lmDlgEditText::OnAccept(wxCommandEvent& WXUNUSED(event))
{
    //save values
    if (m_pParentText->GetText() != m_pTxtCtrl->GetValue())
        m_pController->ChangeText(m_pParentText,
                                  m_pTxtCtrl->GetValue(),
                                  m_pParentText->GetAlignment(),
                                  m_pParentText->GetLocation(),
                                  m_pParentText->GetStyle(),
                                  m_pParentText->GetColour() );

    EndModal(wxID_OK);
}

void lmDlgEditText::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    EndDialog(wxID_CANCEL);
}

//-------


//bool lmDlgEditText::DoEnumerateFamilies(bool fixedWidthOnly,
//                                  wxFontEncoding encoding,
//                                  bool silent)
//{
//    MyFontEnumerator fontEnumerator;
//
//    fontEnumerator.EnumerateFacenames(encoding, fixedWidthOnly);
//
//    if ( fontEnumerator.GotAny() )
//    {
//        int nFacenames = fontEnumerator.GetFacenames().GetCount();
//        if ( !silent )
//        {
//            wxLogStatus(this, _T("Found %d %sfonts"),
//                        nFacenames, fixedWidthOnly ? _T("fixed width ") : _T(""));
//        }
//
//        wxString facename;
//
//        if ( silent )
//        {
//            // choose the first
//            facename = fontEnumerator.GetFacenames().Item(0);
//        }
//        else
//        {
//            // let the user choose
//            wxString *facenames = new wxString[nFacenames];
//            int n;
//            for ( n = 0; n < nFacenames; n++ )
//                facenames[n] = fontEnumerator.GetFacenames().Item(n);
//
//            n = wxGetSingleChoiceIndex
//                (
//                    _T("Choose a facename"),
//                    SAMPLE_TITLE,
//                    nFacenames,
//                    facenames,
//                    this
//                );
//
//            if ( n != -1 )
//                facename = facenames[n];
//
//            delete [] facenames;
//        }
//
//        if ( !facename.empty() )
//        {
//            wxFont font(wxNORMAL_FONT->GetPointSize(),
//                        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
//                        wxFONTWEIGHT_NORMAL, false, facename, encoding);
//
//            DoChangeFont(font);
//        }
//
//        return true;
//    }
//    else if ( !silent )
//    {
//        wxLogWarning(_T("No such fonts found."));
//    }
//
//    return false;
//}
//
//void lmDlgEditText::OnEnumerateFamiliesForEncoding(wxCommandEvent& WXUNUSED(event))
//{
//    wxFontEncoding enc = GetEncodingFromUser();
//    if ( enc != wxFONTENCODING_SYSTEM )
//    {
//        DoEnumerateFamilies(false, enc);
//    }
//}
//
//void lmDlgEditText::OnSetNativeDesc(wxCommandEvent& WXUNUSED(event))
//{
//    wxString fontInfo = wxGetTextFromUser
//                        (
//                            _T("Enter native font string"),
//                            _T("Input font description"),
//                            m_canvas->GetFont().GetNativeFontInfoDesc(),
//                            this
//                        );
//    if ( fontInfo.empty() )
//        return;     // user clicked "Cancel" - do nothing
//
//    wxFont font;
//    font.SetNativeFontInfo(fontInfo);
//    if ( !font.Ok() )
//    {
//        wxLogError(_T("Font info string \"%s\" is invalid."),
//                   fontInfo.c_str());
//        return;
//    }
//
//    DoChangeFont(font);
//}
//
//void lmDlgEditText::OnSetFaceName(wxCommandEvent& WXUNUSED(event))
//{
//    wxString facename = GetCanvas()->GetFont().GetFaceName();
//    wxString newFaceName = wxGetTextFromUser(
//            _T("Here you can edit current font face name."),
//            _T("Input font facename"), facename,
//            this);
//    if (newFaceName.IsEmpty())
//        return;     // user clicked "Cancel" - do nothing
//
//    wxFont font(GetCanvas()->GetFont());
//    if (font.SetFaceName(newFaceName))      // change facename only
//    {
//        wxASSERT_MSG(font.Ok(), _T("The font should now be valid"));
//        DoChangeFont(font);
//    }
//    else
//    {
//        wxASSERT_MSG(!font.Ok(), _T("The font should now be invalid"));
//        wxMessageBox(_T("There is no font with such face name..."),
//                     _T("Invalid face name"), wxID_OK|wxICON_ERROR, this);
//    }
//}
//
//void lmDlgEditText::OnSetNativeUserDesc(wxCommandEvent& WXUNUSED(event))
//{
//    wxString fontdesc = GetCanvas()->GetFont().GetNativeFontInfoUserDesc();
//    wxString fontUserInfo = wxGetTextFromUser(
//            _T("Here you can edit current font description"),
//            _T("Input font description"), fontdesc,
//            this);
//    if (fontUserInfo.IsEmpty())
//        return;     // user clicked "Cancel" - do nothing
//
//    wxFont font;
//    if (font.SetNativeFontInfoUserDesc(fontUserInfo))
//    {
//        wxASSERT_MSG(font.Ok(), _T("The font should now be valid"));
//        DoChangeFont(font);
//    }
//    else
//    {
//        wxASSERT_MSG(!font.Ok(), _T("The font should now be invalid"));
//        wxMessageBox(_T("Error trying to create a font with such description..."));
//    }
//}
//
//void lmDlgEditText::OnSetEncoding(wxCommandEvent& WXUNUSED(event))
//{
//    wxFontEncoding enc = GetEncodingFromUser();
//    if ( enc == wxFONTENCODING_SYSTEM )
//        return;
//
//    wxFont font = m_canvas->GetFont();
//    font.SetEncoding(enc);
//    DoChangeFont(font);
//}
//
//wxFontEncoding lmDlgEditText::GetEncodingFromUser()
//{
//    wxArrayString names;
//    wxArrayInt encodings;
//
//    const size_t count = wxFontMapper::GetSupportedEncodingsCount();
//    names.reserve(count);
//    encodings.reserve(count);
//
//    for ( size_t n = 0; n < count; n++ )
//    {
//        wxFontEncoding enc = wxFontMapper::GetEncoding(n);
//        encodings.push_back(enc);
//        names.push_back(wxFontMapper::GetEncodingName(enc));
//    }
//
//    int i = wxGetSingleChoiceIndex
//            (
//                _T("Choose the encoding"),
//                SAMPLE_TITLE,
//                names,
//                this
//            );
//
//    return i == -1 ? wxFONTENCODING_SYSTEM : (wxFontEncoding)encodings[i];
//}
//void lmDlgEditText::OnwxPointerFont(wxCommandEvent& event)
//{
//    wxFont font;
//
//    switch ( event.GetId() )
//    {
//        case Font_wxNORMAL_FONT:
//            font = *wxNORMAL_FONT;
//            break;
//
//        case Font_wxSMALL_FONT:
//            font = *wxSMALL_FONT;
//            break;
//
//        case Font_wxITALIC_FONT:
//            font = *wxITALIC_FONT;
//            break;
//
//        case Font_wxSWISS_FONT:
//            font = *wxSWISS_FONT;
//            break;
//
//        default:
//            wxFAIL_MSG( _T("unknown standard font") );
//            return;
//    }
//
//    DoChangeFont(font);
//}
//
//void lmDlgEditText::OnSelectFont(wxCommandEvent& WXUNUSED(event))
//{
//    wxFontData data;
//    data.SetInitialFont(m_canvas->GetFont());
//    data.SetColour(m_canvas->GetColour());
//
//    wxFontDialog dialog(this, data);
//    if ( dialog.ShowModal() == wxID_OK )
//    {
//        wxFontData retData = dialog.GetFontData();
//        wxFont font = retData.GetChosenFont();
//        wxColour colour = retData.GetColour();
//
//        DoChangeFont(font, colour);
//    }
//}
//
//void lmDlgEditText::OnTestTextValue(wxCommandEvent& WXUNUSED(event))
//{
//    wxString value = m_pTxtCtrl->GetValue();
//    m_pTxtCtrl->SetValue(value);
//    if ( m_pTxtCtrl->GetValue() != value )
//    {
//        wxLogError(_T("Text value changed after getting and setting it"));
//    }
//}
//
//void lmDlgEditText::OnViewMsg(wxCommandEvent& WXUNUSED(event))
//{
//#if wxUSE_FILEDLG
//    // first, choose the file
//    static wxString s_dir, s_file;
//    wxFileDialog dialog(this, _T("Open an email message file"),
//                        s_dir, s_file);
//    if ( dialog.ShowModal() != wxID_OK )
//        return;
//
//    // save for the next time
//    s_dir = dialog.GetDirectory();
//    s_file = dialog.GetFilename();
//
//    wxString filename = dialog.GetPath();
//
//    // load it and search for Content-Type header
//    wxTextFile file(filename);
//    if ( !file.Open() )
//        return;
//
//    wxString charset;
//
//    static const wxChar *prefix = _T("Content-Type: text/plain; charset=");
//    const size_t len = wxStrlen(prefix);
//
//    size_t n, count = file.GetLineCount();
//    for ( n = 0; n < count; n++ )
//    {
//        wxString line = file[n];
//
//        if ( !line )
//        {
//            // if it is an email message, headers are over, no need to parse
//            // all the file
//            break;
//        }
//
//        if ( line.Left(len) == prefix )
//        {
//            // found!
//            const wxChar *pc = line.c_str() + len;
//            if ( *pc == _T('"') )
//                pc++;
//
//            while ( *pc && *pc != _T('"') )
//            {
//                charset += *pc++;
//            }
//
//            break;
//        }
//    }
//
//    if ( !charset )
//    {
//        wxLogError(_T("The file '%s' doesn't contain charset information."),
//                   filename.c_str());
//
//        return;
//    }
//
//    // ok, now get the corresponding encoding
//    wxFontEncoding fontenc = wxFontMapper::Get()->CharsetToEncoding(charset);
//    if ( fontenc == wxFONTENCODING_SYSTEM )
//    {
//        wxLogError(_T("Charset '%s' is unsupported."), charset.c_str());
//        return;
//    }
//
//    m_pTxtCtrl->LoadFile(filename);
//
//    if ( fontenc == wxFONTENCODING_UTF8 ||
//            !wxFontMapper::Get()->IsEncodingAvailable(fontenc) )
//    {
//        // try to find some similar encoding:
//        wxFontEncoding encAlt;
//        if ( wxFontMapper::Get()->GetAltForEncoding(fontenc, &encAlt) )
//        {
//            wxEncodingConverter conv;
//
//            if (conv.Init(fontenc, encAlt))
//            {
//                fontenc = encAlt;
//                m_pTxtCtrl -> SetValue(conv.Convert(m_pTxtCtrl -> GetValue()));
//            }
//            else
//            {
//                wxLogWarning(_T("Cannot convert from '%s' to '%s'."),
//                             wxFontMapper::GetEncodingDescription(fontenc).c_str(),
//                             wxFontMapper::GetEncodingDescription(encAlt).c_str());
//            }
//        }
//        else
//            wxLogWarning(_T("No fonts for encoding '%s' on this system."),
//                         wxFontMapper::GetEncodingDescription(fontenc).c_str());
//    }
//
//    // and now create the correct font
//    if ( !DoEnumerateFamilies(false, fontenc, true /* silent */) )
//    {
//        wxFont font(wxNORMAL_FONT->GetPointSize(),
//                    wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
//                    wxFONTWEIGHT_NORMAL, false /* !underlined */,
//                    wxEmptyString /* facename */, fontenc);
//        if ( font.Ok() )
//        {
//            DoChangeFont(font);
//        }
//        else
//        {
//            wxLogWarning(_T("No fonts for encoding '%s' on this system."),
//                         wxFontMapper::GetEncodingDescription(fontenc).c_str());
//        }
//    }
//#endif // wxUSE_FILEDLG
//}

