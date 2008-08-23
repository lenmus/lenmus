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
#pragma implementation "TextProperties.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "TextProperties.h"
#include "../Text.h"
#include "../../app/ArtProvider.h"
#include "../../app/ScoreCanvas.h"


//--------------------------------------------------------------------------------------
/// Implementation of lmTextProperties
//--------------------------------------------------------------------------------------

const int lmEDIT_CUT = wxNewId();
const int lmEDIT_COPY = wxNewId();
const int lmEDIT_PASTE = wxNewId();
const int lmEDIT_BOLD = wxNewId();
const int lmEDIT_ITALIC = wxNewId();
const int lmEDIT_COLOR = wxNewId();
const int lmEDIT_STYLE = wxNewId();


BEGIN_EVENT_TABLE(lmTextProperties, lmPropertiesPage)
    EVT_BUTTON  (lmEDIT_BOLD, lmTextProperties::OnBold)
    EVT_BUTTON  (lmEDIT_ITALIC, lmTextProperties::OnItalic)
    EVT_BUTTON  (lmEDIT_CUT, lmTextProperties::OnCut)
    EVT_BUTTON  (lmEDIT_COPY, lmTextProperties::OnCopy)
    EVT_BUTTON  (lmEDIT_PASTE, lmTextProperties::OnPaste)
    EVT_COLOURPICKER_CHANGED(lmEDIT_COLOR, lmTextProperties::OnColor)
    EVT_CHOICE  (lmEDIT_STYLE, lmTextProperties::OnStyle)
END_EVENT_TABLE()


lmTextProperties::lmTextProperties(wxWindow* parent, lmScoreText* pParentText)
    : lmPropertiesPage(parent)
{
    //m_pController = pController;
    m_pParentText = pParentText;
    CreateControls();

    //load current values
    m_pTxtCtrl->SetValue(m_pParentText->GetText());
    m_pColourPicker->SetColour( m_pParentText->GetColour() );
    //m_pBtBold->Press(m_pParentText->IsBold());

    wxFont font = m_pTxtCtrl->GetFont();
    font.SetWeight(m_pParentText->IsBold() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    font.SetStyle(m_pParentText->IsItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    font.SetPointSize(m_pParentText->GetFontSize());
    font.SetFaceName(m_pParentText->GetFontName());
    DoChangeFont(font, m_pParentText->GetColour());

}

void lmTextProperties::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* pToolbarSizer;
	pToolbarSizer = new wxBoxSizer( wxHORIZONTAL );
	
    wxSize btSize(16, 16);
    this->SetBackgroundColour(*wxWHITE);
	m_pBtCut = new lmBitmapButton(this, lmEDIT_CUT, wxBitmap(24, 24));
    m_pBtCut->SetBitmapUp(_T("tool_cut"), _T(""), btSize);
    m_pBtCut->SetBitmapDown(_T("tool_cut"), _T("button_selected_flat"), btSize);
    m_pBtCut->SetBitmapOver(_T("tool_cut"), _T("button_over_flat"), btSize);
    m_pBtCut->SetBitmapDisabled(_T("tool_cut_dis"), _T(""), btSize);
    m_pBtCut->SetToolTip(_("Cut selected text"));
	pToolbarSizer->Add( m_pBtCut, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pBtCopy = new wxBitmapButton( this, lmEDIT_COPY,
                            wxArtProvider::GetBitmap(_T("tool_copy"), wxART_TOOLBAR, btSize),
                            wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	pToolbarSizer->Add( m_pBtCopy, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pBtPaste = new wxBitmapButton( this, lmEDIT_PASTE,
                            wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, btSize),
                            wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	pToolbarSizer->Add( m_pBtPaste, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pBtBold = new lmCheckButton(this, lmEDIT_BOLD, wxBitmap(24, 24));
    m_pBtBold->SetBitmapUp(_T("tool_paste"), _T(""), btSize);
    m_pBtBold->SetBitmapDown(_T("tool_paste"), _T("button_selected_flat"), btSize);
    m_pBtBold->SetBitmapOver(_T("tool_paste_dis"), _T("button_over_flat"), btSize);
    m_pBtBold->SetToolTip(_("Add/remove bold style"));
	pToolbarSizer->Add( m_pBtBold, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pBtItalic = new wxBitmapButton( this, lmEDIT_ITALIC, wxNullBitmap, wxDefaultPosition, wxSize( 16,16 ), wxBU_AUTODRAW );
	pToolbarSizer->Add( m_pBtItalic, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pColourPicker = new wxColourPickerCtrl( this, lmEDIT_COLOR, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_pColourPicker->SetToolTip( _("Text color selector") );
	
	pToolbarSizer->Add( m_pColourPicker, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_pCboTextStyleChoices;
	m_pCboTextStyle = new wxChoice( this, lmEDIT_STYLE, wxDefaultPosition, wxSize( 250,-1 ), m_pCboTextStyleChoices, 0 );
	m_pCboTextStyle->SetSelection( 0 );
	pToolbarSizer->Add( m_pCboTextStyle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	pMainSizer->Add( pToolbarSizer, 0, wxEXPAND, 5 );
	
	m_pTxtCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtCtrl->SetMinSize( wxSize( 450,150 ) );
	
	pMainSizer->Add( m_pTxtCtrl, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( pMainSizer );
	this->Layout();
	pMainSizer->Fit( this );
}

lmTextProperties::~lmTextProperties()
{
}

void lmTextProperties::OnBold(wxCommandEvent& event)
{
    wxFont font = m_pTxtCtrl->GetFont();

    font.SetWeight(event.IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    DoChangeFont(font);
}

void lmTextProperties::OnItalic(wxCommandEvent& event)
{
    wxFont font = m_pTxtCtrl->GetFont();

    font.SetStyle(event.IsChecked() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    DoChangeFont(font);
}

void lmTextProperties::OnCut(wxCommandEvent& event)
{
    //wxFont font = m_pTxtCtrl->GetFont();

    //font.SetStyle(event.IsChecked() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    //DoChangeFont(font);
}

void lmTextProperties::OnCopy(wxCommandEvent& event)
{
    //wxFont font = m_pTxtCtrl->GetFont();

    //font.SetStyle(event.IsChecked() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    //DoChangeFont(font);
}

void lmTextProperties::OnPaste(wxCommandEvent& event)
{
    //wxFont font = m_pTxtCtrl->GetFont();

    //font.SetStyle(event.IsChecked() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    //DoChangeFont(font);
}

void lmTextProperties::OnColor(wxColourPickerEvent& event)
{
    m_pTxtCtrl->SetForegroundColour( event.GetColour() );
    Refresh();
}

void lmTextProperties::OnStyle(wxCommandEvent& event)
{
    //wxFont font = m_pTxtCtrl->GetFont();

    //font.SetStyle(event.IsChecked() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    //DoChangeFont(font);
}

void lmTextProperties::DoChangeFont(const wxFont& font, const wxColour& color)
{
    m_pTxtCtrl->SetFont(font);
    if ( color.Ok() )
        m_pTxtCtrl->SetForegroundColour(color);
    Refresh();
}

void lmTextProperties::OnAcceptChanges(lmController* pController)
{
    wxFont font = m_pTxtCtrl->GetFont();

    lmFontInfo tFont = m_pParentText->GetFontInfo();
    if (font.GetWeight() == wxFONTWEIGHT_BOLD) 
        if (font.GetStyle() == wxFONTSTYLE_ITALIC)
            tFont.nStyle = lmTEXT_ITALIC_BOLD;
        else
            tFont.nStyle = lmTEXT_BOLD;
    else
        if (font.GetStyle() == wxFONTSTYLE_ITALIC)
            tFont.nStyle = lmTEXT_ITALIC;
        else
            tFont.nStyle = lmTEXT_NORMAL;

    pController->ChangeText(m_pParentText,
                            m_pTxtCtrl->GetValue(),
                            m_pParentText->GetAlignment(),
                            m_pParentText->GetLocation(),
                            tFont,
                            m_pColourPicker->GetColour() );
}

//void lmTextProperties::DoResizeFont(int diff)
//{
//    wxFont font = m_pTxtCtrl->GetFont();
//
//    font.SetPointSize(font.GetPointSize() + diff);
//    DoChangeFont(font);
//}


//-------


//bool lmTextProperties::DoEnumerateFamilies(bool fixedWidthOnly,
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
//void lmTextProperties::OnEnumerateFamiliesForEncoding(wxCommandEvent& WXUNUSED(event))
//{
//    wxFontEncoding enc = GetEncodingFromUser();
//    if ( enc != wxFONTENCODING_SYSTEM )
//    {
//        DoEnumerateFamilies(false, enc);
//    }
//}
//
//void lmTextProperties::OnSetNativeDesc(wxCommandEvent& WXUNUSED(event))
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
//void lmTextProperties::OnSetFaceName(wxCommandEvent& WXUNUSED(event))
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
//void lmTextProperties::OnSetNativeUserDesc(wxCommandEvent& WXUNUSED(event))
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
//void lmTextProperties::OnSetEncoding(wxCommandEvent& WXUNUSED(event))
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
//wxFontEncoding lmTextProperties::GetEncodingFromUser()
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
//void lmTextProperties::OnwxPointerFont(wxCommandEvent& event)
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
//void lmTextProperties::OnSelectFont(wxCommandEvent& WXUNUSED(event))
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
//void lmTextProperties::OnTestTextValue(wxCommandEvent& WXUNUSED(event))
//{
//    wxString value = m_pTxtCtrl->GetValue();
//    m_pTxtCtrl->SetValue(value);
//    if ( m_pTxtCtrl->GetValue() != value )
//    {
//        wxLogError(_T("Text value changed after getting and setting it"));
//    }
//}
//
//void lmTextProperties::OnViewMsg(wxCommandEvent& WXUNUSED(event))
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

