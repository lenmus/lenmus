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

/*  class lmFontManager
    @brief Takes care of all font allocation and management duties for a view.

    The fontsize to use to draw any musical symbol depends on two factors: stafflines
    spacing and rendering scale.

    Allocating the font in the lmStaff object would lead to many allocations of the same
    font. Therefore it's been decided that the staff object will have a pointer to the
    font, and all fonts will be uniquely allocated in the lmFontManager object.

    To my knowledge a score will have, at most, two staff sizes: one smaller for melody
    and a bigger one for piano. So let's assume that five sizes per score is a reasonable
    high limit. With this asumption, we are going to use a dynamic array structure for
    keeping fonts.


*/

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "FontManager.h"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "FontManager.h"

// Definition of the FontsList class
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(FontsList);



lmFontManager::lmFontManager()
{
}

lmFontManager::~lmFontManager()
{
    m_cFonts.DeleteContents(true);        // so that Clear() will delete all fonts
    m_cFonts.Clear();                    // now, delete all elements

}

wxFont* lmFontManager::GetFont(int nPointSize, wxString sFontName,
                               int nFamily, int nStyle, int nWeight, bool fUnderline)
{
    /*
    The font is determined by the following parameters:

    - nPointSize:    size in points
    - nFamily:    Supported families are: wxDEFAULT, wxDECORATIVE, wxROMAN, wxSCRIPT, wxSWISS,
                wxMODERN. wxMODERN is a fixed pitch font; the others are either fixed or variable pitch.
    - nStyle:    The value can be wxNORMAL, wxSLANT or wxITALIC.
    - nWeight:    The value can be wxNORMAL, wxLIGHT or wxBOLD.
    - fUnderline:     The value can be true or false.
    - sFontName:    An string specifying the actual typeface to be used.

    */

    wxFont* pFont;
    int nNumFonts = m_cFonts.GetCount();
    if (nNumFonts > 0) {
        //iterate over the collection of fonts
        wxFontsListNode* pNode = m_cFonts.GetFirst();
        pFont = (pNode ? (wxFont *)pNode->GetData() : (wxFont *)pNode);
        for ( ; pNode; ) {
            pFont = (wxFont *)pNode->GetData();
            if ((pFont->GetPointSize() == nPointSize) &&
                (pFont->GetFaceName() == sFontName) &&
                ((nFamily == wxDEFAULT) || (pFont->GetFamily() == nFamily)) &&
                (pFont->GetStyle() == nStyle) &&
                (pFont->GetWeight() == nWeight) &&
                (pFont->GetUnderlined() == fUnderline))  return pFont;

            //get next font
            pNode = pNode->GetNext();
        }
    }

    // Font does not exist. Allocate it.
    //wxLogMessage(_T("[lmFontManager::GetFont]: Allocating new font. size=%d, name=%s"),
    //        nPointSize, sFontName);

    pFont = new wxFont(nPointSize, nFamily, nStyle, nWeight, fUnderline,
                        sFontName, wxFONTENCODING_DEFAULT);

    if (!pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmFontManager.GetFont"), wxOK);
        wxLogMessage(wxString::Format(
            _T("Error allocating font: PointSize=%d, FontName=%s, Family=%d, Style=%d, Weight=%d, Underline=%s"),
            nPointSize, sFontName.c_str(), nFamily, nStyle, nWeight, (fUnderline ? _T("yes") : _T("no")) ));
        ::wxExit();
    }

    // add the new font to the list and return it
    m_cFonts.Append(pFont);
    return pFont;

}

/*
// Load font data from the .ini file. All files are expected to be found in path /bin,
//    that is, in the same folder than this exe.
//
void lmFontManager::LoadFile(wxString sFileName)
{
    wxTextFile oFile;       //the file to process
    wxString sPath = g_pPaths->GetDataPath();
    wxFileName oFileName(sPath, sFileName, _T("ini"), wxPATH_NATIVE);

    //loop to read and analyze lines
    if (!oFile.Open(oFileName.GetFullPath()) ) {
        wxLogError(wxString::Format( _("Error opening file <%s>"), oFileName.GetFullPath() ));
        return;
    }

    wxString sLine;
    int nPreviousLesson = 0, nCurLesson;
    int nNumLine = 1;
    for (sLine = oFile.GetFirstLine(); !oFile.Eof(); sLine = oFile.GetNextLine() ) {
        //process current line
        if (sLine.Trim() != _T("") && sLine.substr(0, 2) != _T("//")) {
            nCurLesson = AnalyzeGlyphLine(nNumLine, sLine);
            if (nPreviousLesson > nCurLesson) {
                wxMessageBox(wxString::Format(
                    _("File '%s' is not ordered. File ignored."),
                    oFileName.GetFullPath() ));
            }
            nPreviousLesson = nCurLesson;
            nNumLine++;
        }
    }
    //process the last line
    if (sLine.Trim(false) != _T("") && sLine.substr(0, 2) != _T("//")) {
        AnalyzeLine(iFile, nNumLine, sLine);
    }

    //close the file
    oFile.Close();

}

// Analyzes a line and loads it in the corresponding table.
//
//    @returns The level and lesson numbers combined (100*level+lesson) for controlling
//             that the files are ordered
//
int lmFontManager::AnalyzeGlyphLine(int nLine, wxString sLine)
{
    wxASSERT(nLine > 0);

    long nLevelLesson = 0;

    int iSemicolon;
    wxString sData;
    bool fOK;

    //i.e.: <1;04;Sol;c4;c5;0>

    //get level
    iSemicolon = sLine.Find(_T(";"));
    sData = sLine.Left(iSemicolon);
    long nLevel;
    fOK = sData.ToLong(&nLevel);
    wxASSERT(fOK);
    sLine = sLine.substr(iSemicolon + 1);      //skip the semicolon

    //get lesson
    iSemicolon = sLine.Find(_T(";"));
    sData = sLine.Left(iSemicolon);
    long nLesson;
    fOK = sData.ToLong(&nLesson);
    wxASSERT(fOK);
    sLine = sLine.substr(iSemicolon + 1);      //skip the semicolon

    //get clef
    iSemicolon = sLine.Find(_T(";"));
    wxString sClef = sLine.Left(iSemicolon);
    lmEClefType nClef = LDPNameToClef(sClef);
    wxASSERT(nClef != (lmEClefType)-1);

    //get lower scope
    iSemicolon = sLine.Find(_T(";"));
    wxString sLowerScope = sLine.Left(iSemicolon);

    //get upper scope
    iSemicolon = sLine.Find(_T(";"));
    wxString sUpperScope = sLine.Left(iSemicolon);

    //get flag
    bool fOnlyThisLesson = (sLine.substr(iSemicolon + 1, 1) == _T("1"));

    // build the entry
    m_pLessonsClefs->AddEntry((int)nLevel, (int)nLesson, fOnlyThisLesson, nClef,
                                sLowerScope, sUpperScope);

}

*/
