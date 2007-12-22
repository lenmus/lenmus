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
//-------------------------------------------------------------------------------------

#ifndef __MUSICXMLPARSER_H__        //to avoid nested includes
#define __MUSICXMLPARSER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "MusicXMLParser.cpp"
#endif

#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include "wx/xml/xml.h"          // to use wxXmlDocument

#include "../score/Score.h"


//to give a name to ParseMusicXMLFile optional flags
#define sbDO_NOT_START_NEW_LOG    false
#define sbDO_NOT_SHOW_LOG_TO_USER    false

class lmMusicXMLParser
{
public:
    lmMusicXMLParser();
    ~lmMusicXMLParser();

    lmScore* ParseMusicXMLFile(const wxString& filename,
                             bool fNewLog = true,        //start a new data error log
                             bool fShowLog = true );    //show the error log after processing

private:
    void DumpXMLTree(wxXmlNode *pRoot);
    void ParseError(const wxChar* szFormat, ...);
    void TagError(const wxString sElement, const wxString sTagName, wxXmlNode* pElement = NULL);

    // auxiliary XML methods
    wxXmlNode* GetFirstChild(wxXmlNode* pNode);
    wxXmlNode* GetNextSibling(wxXmlNode* pNode);
    wxString GetAttribute(wxXmlNode* pNode, wxString sName, wxString sDefault = _T(""));
    bool GetYesNoAttribute(wxXmlNode* pNode, wxString sName, bool fDefault=true);
    wxString GetText(wxXmlNode* pElement);

    // methods to parse MusicXML elements

        // score.dtd
    void ParseScorePartwise(wxXmlNode* pNode, lmScore* pScore);
    void ParsePart(wxXmlNode* pNode, lmScore* pScore);
    void ParsePartList(wxXmlNode* pNode, lmScore* pScore);
    void ParseWork(wxXmlNode* pNode, lmScore* pScore);
    void ParseIdentification(wxXmlNode* pNode, lmScore* pScore);
    void ParseScorePart(wxXmlNode* pNode, lmScore* pScore);
    void ParseMeasure(wxXmlNode* pNode, lmVStaff* pVStaff);
    bool ParseMusicDataAttributes(wxXmlNode* pNode, lmVStaff* pVStaff);
    bool ParseMusicDataBarline(wxXmlNode* pNode, lmVStaff* pVStaff);
    bool ParseMusicDataDirection(wxXmlNode* pNode, lmVStaff* pVStaff);
    float ParseMusicDataBackupForward(wxXmlNode* pNode, lmVStaff* pVStaff);

        // note.dtd
    bool ParseMusicDataNote(wxXmlNode* pNode, lmVStaff* pVStaff);
    int ParseDuration(wxXmlNode* pElement);

        // attributes.dtd
    int ParseDivisions(wxXmlNode* pElement);

        // common.dtd
    void ParsePosition(wxXmlNode* pElement, lmLocation* pPos);
    void ParseFont(wxXmlNode* pElement, lmFontInfo* pFontData);
    bool ParsePlacement(wxXmlNode* pElement, bool fDefault=true);



    ////////////////////
    // member variables
    ////////////////////

    int        m_nCurrentDivisions;
    lmTupletBracket* m_pTupletBracket;

    //error management
    int        m_nErrors;        // number of parsing errors


};

#endif    // __MUSICXMLPARSER_H__
