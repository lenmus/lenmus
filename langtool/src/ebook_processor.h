//--------------------------------------------------------------------------------------
//    LenMus project: free software for music theory and language
//    Copyright (c) 2002-2010 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program. If not, see <http://www.gnu.org/licenses/>. 
//
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
#ifdef __GNUG__
#pragma interface ebook_processor.cpp
#endif

#ifndef __EBOOK_PROCESSOR_H__        //to avoid nested includes
#define __EBOOK_PROCESSOR_H__

#include <vector>

#include "wx/wfstream.h"

class wxXml2Node;
class wxXml2Document;
class wxZipOutputStream;
class wxTextOutputStream;
class wxFFileOutputStream;
class wxArrayString;
class lmContentStorage;
class lmElement;

enum lmEbookProcessingOptions {      //processing options
    lmLANG_FILE = 1,      //generate Lang file
};

enum lmEbookProcessorDebugOptions {
    eLogTree = 1,
    eDumpTree = 2,
};

//struct to store replacements
typedef struct lmReplacement_Data
{
    wxString sTag;
    wxString sOpen;
    wxString sClose;
}
lmReplacement;


//-----------------------------------------------------------------------------
// lmElement declaration
// Information about an output tag pair (html)
//-----------------------------------------------------------------------------

class lmElement
{
public:
    lmElement(lmContentStorage* pParent, int nStartOpen, int nStartClose, int nNameLenght,
              bool fIsSingle=false, int nLevel=0);
    lmElement(lmContentStorage* pParent, lmElement* pE, int nPos, int nLevel=0);
    ~lmElement();

    void AddClose(int nEndOpen, int nEndClose);

    inline int StartOpen() { return m_nStartOpen; }
    inline int StartClose() { return m_nStartClose; }
    inline bool IsSingle() { return m_fIsSingle; }
    int EndOpen();
    int EndClose();
    inline bool IsClosed() { return m_fIsClosed; }
    inline int GetLevel() { return m_nLevel; }
    inline bool IsPlaceholder() { return m_fIsPlaceholder; }
    inline void MarkAsPlaceholder(bool fValue) { m_fIsPlaceholder = fValue; }
    inline bool IsEmpty() { return m_nStartClose == m_nStartOpen; }

    //tags and content
    wxString GetTagName();
    inline int NameLength() { return m_nNameLength; }
    inline int ContentLenght() { return m_nEndOpen - m_nStartClose - 1; }
    wxString GetOpenTag();
    wxString GetCloseTag();
    wxString GetFullContent();
    lmElement* FindOpen(const wxString& sTag);
    lmElement* FindClose(const wxString& sTag);
    inline int GetNumChildren() { return (int)m_elements.size(); }
    inline lmElement* GetChild(int iE) { return m_elements[iE]; }

    //saved data
    inline const wxString& GetOpenReplacement() { return m_sOpenReplacement; }
    inline const wxString& GetCloseReplacement() { return m_sCloseReplacement; }
    inline void SetOpenReplacement(const wxString& sReplacement) { m_sOpenReplacement = sReplacement; }
    inline void SetCloseReplacement(const wxString& sReplacement) { m_sCloseReplacement = sReplacement; }

    //inner elements
    inline int GetNumElements() { return (int)m_elements.size(); }
    inline lmElement* GetElement(int i) { return m_elements[i]; }

    //parsing
    void AddOpenTag(lmElement* pE, int nLevel);
    lmElement* AddCloseTag(int nStart, int nEnd, int nLevel);
    void AddSingleTag(lmElement* pE, int nLevel);

    //translation
    void ToPo(lmContentStorage* pMsg);
    void FromPo(lmContentStorage* pMsg, lmContentStorage* pResult);
    void OptimizePoMsge();
    void ClosePlaceholder(int nTag, lmContentStorage* pCS);
    void OpenPlaceholder(int nTag, lmContentStorage* pCS);
    void SinglePlaceholder(int nTag, lmContentStorage* pCS);

    void Dump();

private:
    void ClearElements();


    lmContentStorage*   m_pParent;

    int         m_nLevel;           //0..n
    int         m_nStartOpen;       //index to "<" 
    int         m_nStartClose;      //index to ">"
    int         m_nNameLength;      //tag name size  
    int         m_nEndOpen;         //index to "<" 
    int         m_nEndClose;        //index to ">"
    bool        m_fIsSingle;        //is a single tag "<xxx />"
    bool        m_fIsClosed;

    //Original tag content, to restore placeholders.
    bool        m_fIsPlaceholder;
    wxString    m_sOpenReplacement;    
    wxString    m_sCloseReplacement;

    lmElement*  m_pCurElement;      //current open element to receive inner elements
    std::vector<lmElement*> m_elements;
};


//-----------------------------------------------------------------------------
// lmContentStorage declaration
//-----------------------------------------------------------------------------

class lmContentStorage
{
public:
    lmContentStorage();
    ~lmContentStorage();

    //add content
    void Add(wxChar c);
    void Add(const wxString& str);
    void Add(const wxString* str);
    void Add(const wxChar* str);
    void Add(lmContentStorage* pCS);
    void AddNewLine();

    void Clear();

    //add parsed content
    void AddElementOpen(const wxString& sTag, const wxString& sReplacement = wxEmptyString);
    void AddElementClose(const wxString& sTag, const wxString& sReplacement = wxEmptyString);
    void AddElementSingle(const wxString& sTag, const wxString& sReplacement = wxEmptyString);

    //access
    inline const wxString& GetContent() { return m_sBuffer; }
    inline int GetContentLength() { return (int)m_sBuffer.length(); }
    inline wxChar GetChar(int iPos) { return m_sBuffer[iPos]; }
    inline int GetMaxLevel() { return m_nMaxLevel; }
    const wxString& FindOpenReplacement(const wxString& sTag);
    const wxString& FindCloseReplacement(const wxString& sTag);

    //info
    inline bool IsEmpty() { return m_sBuffer.IsEmpty(); }
    inline int GetStartSpaces() { return m_nStartSpaces; }
    inline int GetFinalSpaces() { return m_nFinalSpaces; }

    void Dump();

    //translation
    wxString GeneratePoMessage(lmContentStorage* pCS);
    void RestorePlaceholders(lmContentStorage* pPoMsg, lmContentStorage* pResult);
    void GenerateTranslation(lmContentStorage* pPoMsg, const wxString& sTrans);
    int RemoveSpacesNewLines(int iStart, int iEnd, lmElement* pE);
    void ReplaceTagsByPlaceholders(lmContentStorage* pResult);
    inline int GetStartOpt() { return m_iStartOpt; }
    inline int GetEndOpt() { return m_iEndOpt; }
    inline bool IsTranslated() { return m_fTranslated; }
    inline void SetTranslated(bool fValue) { m_fTranslated = fValue; }



    //parsing
    void ParseElements();
    inline bool IsParsed() { return m_pOwnerElement != (lmElement*)NULL; }

    //placeholders numbering
    inline void ResetNumPH() { m_nPlaceholderCounter = 0; }
    inline int GetNextNumPH() { return ++m_nPlaceholderCounter; }


private:
    void ClearElements();

    //parsing
    void CreateOwnerElement();
    void OpenElement(int nLevel, int nStart, int nEnd);
    void CloseElement(int nLevel, int nStart, int nEnd);
    void SingleElement(int nLevel, int nStart, int nEnd);

    wxString        m_sBuffer;
    int             m_nMaxLevel;
    lmElement*      m_pOwnerElement;
    int             m_nPlaceholderCounter;
    int             m_nStartSpaces;
    int             m_nFinalSpaces;
    int             m_nCurLevel;

    //translation
    bool            m_fTranslated;
    int             m_iStartOpt;
    int             m_iEndOpt;


};


//-----------------------------------------------------------------------------
// lmEbookProcessor declaration
//-----------------------------------------------------------------------------

class lmEbookProcessor
{
public:
    lmEbookProcessor(int nDbgOptions=0, wxTextCtrl* pUserLog=(wxTextCtrl*)NULL);
    ~lmEbookProcessor();

    bool GenerateLMB(wxString sFilename, wxString sLangCode, wxString sCharCode,
                     int nOptions=0);
    bool CreatePoFile(wxString sFilename, wxString& WXUNUSED(sCharSet),
                      wxString& sLangName, wxString& WXUNUSED(sLangCode),
                      wxString& sFolder);

    static wxString GetLibxml2Version();

private:

    enum lmEParentType
    {
        lmPARENT_BOOKINFO = 0,
        lmPARENT_CHAPTER,
        lmPARENT_SECTION,
        lmPARENT_THEME,
        lmPARENT_PART,
    };
  
    // Tags' processors
    bool AbstractTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool AuthorTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool BookArticleTag(const wxXml2Node& oNode, const wxString& sTagName);
    bool InfoTag(const wxXml2Node& oNode, lmContentStorage* WXUNUSED(pResult));
    bool ChapterTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool HolderTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool ImagedataTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool LegalnoticeTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool LinkTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool ParaTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool PartTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool ScoreTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool SectionTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool SimplelistTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool TableTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool TdTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool ThemeTag(const wxXml2Node& oNode, lmContentStorage* WXUNUSED(pResult));
    bool TitleTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool TitleabbrevTag(const wxXml2Node& oNode, lmContentStorage* WXUNUSED(pResult));
    bool TocimageTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool TranslationcreditsTag(const wxXml2Node& WXUNUSED(oNode), lmContentStorage* pResult);
    bool UlinkTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool YearTag(const wxXml2Node& oNode, lmContentStorage* pResult);

    bool ExerciseTag(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool ExerciseParamTag(const wxXml2Node& oNode, lmContentStorage* pResult=NULL);
    bool ExerciseMusicTag(const wxXml2Node& oNode, lmContentStorage* pResult);

    bool ProcessJustReplaceTag(const wxString& sTag, const wxXml2Node& oNode,
                               lmContentStorage* pResult);
    bool ProcessVariableAssignTag(const wxXml2Node& oNode, wxString* pVar);


    // auxiliary
    void IncrementTitleCounters();
    void DecrementTitleCounters();
    wxString GetTitleCounters();
    wxString GetParentNumber();
    void CreateBookCover();
    void TranslateContent(lmContentStorage* pContent);
    void AddReplacement(const wxString& sOuttag, lmContentStorage* pResult);
    bool GetTagContent(const wxXml2Node& oNode, lmContentStorage* pResult);


    // Parsing methods
    bool ProcessChildAndSiblings(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool ProcessChildren(const wxXml2Node& oNode, lmContentStorage* pResult);
    bool ProcessTag(const wxXml2Node& oNode, lmContentStorage* pResult);

    //tags classification
    bool IsJustReplaceTag(const wxString& sTag);
    bool IsExerciseParamTag(const wxString& sTag);
    bool IsPoMsgDelimiterTag(const wxString& sTag);
    const lmReplacement& GetReplacement(const wxString& sTag);

    // File generation methods
        // TOC
    bool StartTocFile(wxString sFilename);
    void TerminateTocFile();
    void WriteToToc(wxString sText, bool fIndent=true);
        // HTML
    bool StartHtmlFile(const wxString& sFilename);
    void TerminateHtmlFile();
    void TerminateArticleFile();
    void WriteToHtml(const wxString& sText);
    void CloseHtmlFile();
    void CreatePageHeaders(wxString sBookTitle, wxString sHeaderTitle,
                           wxString sTitleNum);
    void CreateArticleHeaders(wxString sBookTitle, wxString sHeaderTitle,
                           wxString sTitleNum);
    bool CreateHtmlFile(wxString sFilename, lmContentStorage* pContent);

        // Lang (.cpp)
    bool StartLangFile(wxString sFilename);
    void WriteToLang(const wxString& sText);
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
    wxString        m_sCharCode;            //charset code (i.e. 'utf-8', 'iso-8859-9') to use
    wxString        m_sLangCode;            //language code to use (i.e. 'es', 'nl', 'gl_ES')
    int             m_nHtmlIndentLevel;     //to indent output
    wxString        m_sHtmlPagename;        
    int             m_nHeaderLevel;
#define             lmMAX_TITLE_LEVEL  8
    int             m_nNumTitle[lmMAX_TITLE_LEVEL];     // 8 levels for numbering titles
    int             m_nTitleLevel;          // current level
    wxString        m_sChapterTitle;
    wxString        m_sChapterNum;
    wxString        m_sBookTitle;
    lmEParentType     m_nParentType;           // to know the owner of the title tag
    wxString        m_sChapterTitleAbbrev;
    wxString        m_sBookTitleAbbrev;
    wxString        m_sThemeTitleAbbrev;
    wxString        m_sHeaderTitle;
    wxString        m_sHeaderParentNum;


    // variables for idx processing

    // variables for Lang files processing
    bool            m_fOnlyLangFile;          //only generate the Lang file

    // variables for LMB (zip file) generation
    bool                    m_fGenerateLmb;
    wxTextOutputStream*     m_pLmbFile;
    wxZipOutputStream*      m_pZipFile;
    wxFFileOutputStream*    m_pZipOutFile;
    wxArrayString           m_aFilesToPack;

    // article and book properties
    bool            m_fIsArticle;
    wxString        m_sStyle;

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

    // translation
    int             m_nPlaceHolder;

    //debugging and trace
    bool            m_fLogTree;
    bool            m_fDump;
    wxTextCtrl*     m_pLog;

};

#endif    // __EBOOK_PROCESSOR_H__
