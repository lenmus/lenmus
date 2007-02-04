//--------------------------------------------------------------------------------------
//    LenMus project: free software for music theory and language
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
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __EBOOK_PROCESSOR_H__        //to avoid nested includes
#define __EBOOK_PROCESSOR_H__

#include "wx/wfstream.h"

class wxXml2Node;
class wxXml2Document;
class wxZipOutputStream;
class wxTextOutputStream;
class wxFFileOutputStream;
class wxArrayString;

enum {      //processing options
    lmLANG_FILE = 1,      //generate Lang file
};

enum lmEbookProcessorDebugOptions {
    eLogTree = 1,
    eDumpTree = 2,
};



class lmEbookProcessor
{
public:
    lmEbookProcessor(int nDbgOptions=0, wxTextCtrl* pUserLog=(wxTextCtrl*)NULL);
    ~lmEbookProcessor();

    bool GenerateLMB(wxString sFilename, wxString sLangCode, wxString sCharCode,
                     int nOptions=0);
    bool CreatePoFile(wxString sFilename, wxString& sCharSet, wxString& sLangName,
                      wxString& sLangCode, wxString& sFolder);

    static wxString GetLibxml2Version();

private:

    enum EParentType
    {
        lmPARENT_BOOKINFO = 0,
        lmPARENT_CHAPTER,
        lmPARENT_SECTION,
        lmPARENT_THEME,
        lmPARENT_PART,
    };
  
    // Tags' processors
    bool AbstractTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool AuthorTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool BookTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool BookinfoTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool ChapterTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool ContentTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool CopyrightTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool EmphasisTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool HolderTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool ItemizedlistTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool LeafletTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool LeafletcontentTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool LegalnoticeTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool LinkTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool ListitemTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool OrderedlistTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool ParaTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool PartTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool ScoreTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool SectionTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool SimplelistTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool ThemeTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool TitleTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool TitleabbrevTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool TocimageTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool UlinkTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool YearTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);

    bool ExerciseTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);
    bool ExerciseParamTag(const wxXml2Node& oNode, bool fTranslate);
    bool ExerciseMusicTag(const wxXml2Node& oNode);

    // auxiliary
    void IncrementTitleCounters();
    void DecrementTitleCounters();
    wxString GetTitleCounters();
    wxString GetParentNumber();
    void CreateBookCover();

    // Parsing methods
    bool ProcessChildAndSiblings(const wxXml2Node& oNode, int nWriteOptions=0,
                                 wxString* pText = (wxString*)NULL);
    bool ProcessChildren(const wxXml2Node& oNode, int nOptions=0,
                         wxString* pText = (wxString*)NULL);
    bool ProcessTag(const wxXml2Node& oNode, int nOptions=0, wxString* pText=NULL);

    // File generation methods
        // TOC
    bool StartTocFile(wxString sFilename);
    void TerminateTocFile();
    void WriteToToc(wxString sText, bool fIndent=true);
        // HTML
    bool StartHtmlFile(wxString sFilename, wxString sId);
    void TerminateHtmlFile();
    void TerminateLeafletFile();
    void WriteToHtml(wxString sText);
    void CloseHtmlFile();
    void CreatePageHeaders(wxString sBookTitle, wxString sHeaderTitle,
                           wxString sTitleNum);
    void CreateLeafletHeaders(wxString sBookTitle, wxString sHeaderTitle,
                           wxString sTitleNum);
        // Lang (.cpp)
    bool StartLangFile(wxString sFilename);
    void WriteToLang(wxString sText);
        // LMB
    bool StartLmbFile(wxString sFilename, wxString sLangCode, wxString sCharCode);
    void TerminateLmbFile();
    void CopyToLmb(wxString sFilename);


    // Debug and methods to inform user
    void DumpXMLTree(const wxXml2Node& oNode);
    void DumpNodeAndSiblings(const wxXml2Node& oNode, wxString& sTree, int n);
    void DumpNode(const wxXml2Node& oNode, wxString& sTree, int n);
    void LogMessage(const wxChar* szFormat, ...);
    void LogError(const wxChar* szFormat, ...);



    // member variables

    //files
    wxString        m_sFilename;            // full path & name of xml file being processed
    wxFile*         m_pTocFile;
    wxFile*         m_pHtmlFile;
    wxFile*         m_pLangFile;

    // variables for toc processing
    int             m_nTocIndentLevel;      // to indent output
    bool            m_fTitleToToc;          // write title to toc
    bool            m_fThemeInToc;          // the theme is in TOC
    wxString        m_sTocFilename;

    // variables for html processing
    wxString        m_sCharCode;            // charset code (i.e. 'utf-8', 'iso-8859-9') to use
    int             m_nHtmlIndentLevel;     // to indent output
    wxString        m_sHtmlPagename;        
    int             m_nHeaderLevel;
#define             lmMAX_TITLE_LEVEL  8
    int             m_nNumTitle[lmMAX_TITLE_LEVEL];     // 8 levels for numbering titles
    int             m_nTitleLevel;          // current level
    wxString        m_sChapterTitle;
    wxString        m_sChapterNum;
    wxString        m_sBookTitle;
    EParentType     m_nParentType;           // to know the owner of the title tag
    wxString        m_sChapterTitleAbbrev;
    wxString        m_sBookTitleAbbrev;
    wxString        m_sThemeTitleAbbrev;

    // variables for idx processing

    // variables for Lang files processing
    bool            m_fOnlyLangFile;          //only generate the Lang file

    // variables for LMB (zip file) generation
    bool                    m_fGenerateLmb;
    wxTextOutputStream*     m_pLmbFile;
    wxZipOutputStream*      m_pZipFile;
    wxFFileOutputStream*    m_pZipOutFile;
    wxArrayString           m_aFilesToPack;

    // leaflet
    bool            m_fIsLeaflet;

    // bookinfo data
    bool            m_fProcessingBookinfo;
    wxString        m_sBookId;
    wxString        m_sBookAbstract;
    wxString        m_sCopyrightYear;
    wxString        m_sCopyrightHolder;
    wxString        m_sLegalNotice;
    wxString        m_sAuthorName;

    // variables for processing external entities
    bool            m_fExtEntity;           //waiting to include an external entity
    wxString        m_sExtEntityName;       //external entity to include

    //debugging and trace
    bool            m_fLogTree;
    bool            m_fDump;
    wxTextCtrl*     m_pLog;

};


#endif    // __EBOOK_PROCESSOR_H__
