//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "ebook_processor.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/txtstrm.h"
#include "wx/zipstrm.h"
#include "wx/textfile.h"
#include "wx/arrstr.h"

#include "ebook_processor.h"
#include "wx/xml2.h"            // include libxml2 wrapper definitions
#include "wx/dtd.h"				// include libxml2 wrapper definitions
#include "Paths.h"

//to avoid errors by returning local temp. pointer
const wxString lmEmptyString = _T("");

#define ltNO_INDENT false

enum lmEOptions {
    eNOTHING = 0,
    eTOC = 1,           //write tag content to TOC
    eHTML = 2,          //write tag content to HTML file
    eTRANSLATE = 4,     //get translation before using tag content
};


// Constant strings that need translation
static const wxString m_sPhonascus =
    _T("the teacher of music");
static const wxString m_sCoverPage =
    _T("Cover page");
static const wxString m_sFooter1 =
    _T("Send your comments and suggestions to the LenMus team (www.lenmus.org)");

static const wxString m_sTranslators =
    _T("Translated to #REPLACE BY YOUR LANGUAGE NAME# by #REPLACE BY YOUR NAME#.");

#if 0        //1=use GNU Free Doc. LIcense, 0=use CC-BY-SA
static const wxString m_sFooter2 =
    _T("Licensed under the terms of the GNU Free Documentation License v1.3");
static const wxString m_sFooter3 =
    _T("");

#else
static const wxString m_sFooter2 =
    _T("Licensed under the Creative Commons Attribution/Share-Alike License;");
static const wxString m_sFooter3 =
    _T("additional terms may apply. See cover page of this eBook for exceptions and details.");

#endif

//footers for "myMusicTheory" style
static const wxString m_sMMT_Footer2 = 
    _T("Copyright © 2007-2012 myMusicTheory & LenMus project. All rights reserved.");


//strings used in credits replacements
static const wxString m_sReferences = _T("References");
static const wxString m_sLessonsBased = _T("This lesson is based on materials from:");


//=======================================================================================
// lmTags implementation
//=======================================================================================

//---------------------------------------------------------------------------------------
//Tags used as params in exercises, containing non-translatable information
const wxString lmTags::m_aExerciseParamTags[] =
{
    _T("accidentals"),
    _T("cadences"),
    _T("cadence_buttons"),
    _T("chords"),
    _T("clef"),
    _T("control_go_back"),
    _T("control_settings"),
    _T("control_solfa"),
    _T("control_play"),
    _T("fragment"),
    _T("inversions"),
    _T("key"),           //keys
    _T("keys"),
    _T("max_accidentals"),
    _T("max_interval"),
    _T("metronome"),
    _T("mode"),
    _T("problem_level"),
    _T("problem_type"),
    _T("play_key"),
    _T("play_mode"),
    _T("scales"),
    _T("show_key"),
    _T("time"),
    _T("top_margin"),
};

//---------------------------------------------------------------------------------------
//Tags used as params in scores containing non-translatable information
const wxString lmTags::m_aScoreParamTags[] =
{
    _T("control_measures"), //obsolete 5.0
    _T("control_play"),
    _T("control_solfa"),
    _T("metronome"),
    _T("music_border"),     //obsolete 5.0
    _T("score_type"),       //obsolete 5.0
    _T("top_margin"),       //obsolete 5.0
};

//---------------------------------------------------------------------------------------
//Tags not needing any processing. Just replace tag be the corresponding
//open/close out tag.
const wxString lmTags::m_aJustReplaceTags[] =
{
    _T("content"),
    _T("copyright"),
    _T("copyrightsymbol"),
    _T("itemizedlist"),
    _T("orderedlist"),
    _T("subscript"),
    _T("superscript"),
    _T("sbr"),
    _T("simplelist"),
};

//---------------------------------------------------------------------------------------
//Tags that delimits a translatable message (paragraph, sentence, etc.)
const wxString lmTags::m_aPoMsgTags[] =
{
    _T("abstract"),
    _T("copyright"),
    _T("creditsitem"),
    //_T("legalnotice"),
    _T("listitem"),
    _T("para"),
    _T("td"),
    _T("title"),
};

//---------------------------------------------------------------------------------------
bool lmTags::is_just_replace_tag(const wxString& sTag)
{
    for(int i=0; i < int(sizeof(m_aJustReplaceTags)/sizeof(wxString)); ++i)
    {
        if (m_aJustReplaceTags[i] == sTag)
            return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool lmTags::is_exercise_param_tag(const wxString& sTag)
{
    for(int i=0; i < int(sizeof(m_aExerciseParamTags)/sizeof(wxString)); ++i)
    {
        if (m_aExerciseParamTags[i] == sTag)
            return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool lmTags::is_score_param_tag(const wxString& sTag)
{
    for(int i=0; i < int(sizeof(m_aScoreParamTags)/sizeof(wxString)); ++i)
    {
        if (m_aScoreParamTags[i] == sTag)
            return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool lmTags::is_po_msg_delimiter_tag(const wxString& sTag)
{
    for(int i=0; i < int(sizeof(m_aPoMsgTags)/sizeof(wxString)); ++i)
    {
        if (m_aPoMsgTags[i] == sTag)
            return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool lmTags::is_placeholder_tag(const wxString& sName)
{
    //returns true if the tag has to be replaced by a 'placeholder' tag, that is,
    //if the tag is not generic (has parameters).

    return (   sName == _T("object")
            || sName == _T("a")
            || sName == _T("img")
            || sName == _T("font")
            || sName == _T("div")
            || sName == _T("param")
        );
}

//---------------------------------------------------------------------------------------
bool lmTags::is_supress_tag(const wxString& sName)
{
    //returns true if the tag content has to be removed from output and just write
    //a placeholder.

    return (sName == _T("object"));
}

//---------------------------------------------------------------------------------------
void lmTags::add_replacement(const wxString& tag, const wxString& open,
                             const wxString& close)
{
    m_Replacements.push_back( lmReplacement(tag, open, close) );
}

//---------------------------------------------------------------------------------------
const lmReplacement& lmTags::get_replacement(const wxString& tag)
{
    for(int i=0; i < int(m_Replacements.size()); ++i)
    {
        if (m_Replacements[i].sTag == tag)
            return m_Replacements[i];
    }
    return m_Replacements[0];
}



//-----------------------------------------------------------------------------
// lmElement implementation
//-----------------------------------------------------------------------------

lmElement::lmElement(lmContentStorage* pParent, int nStartOpen, int nStartClose,
                     int nNameLenght, bool fIsSingle, int nLevel)
    : m_pParent(pParent)
    , m_nLevel(nLevel)
    , m_nStartOpen(nStartOpen)
    , m_nStartClose(nStartClose)
    , m_nNameLength(nNameLenght)
    , m_nEndOpen(0)
    , m_nEndClose(0)
    , m_fIsSingle(fIsSingle)
    , m_fIsClosed(fIsSingle)
    , m_fIsPlaceholder(false)
    , m_pCurElement((lmElement*)NULL)
{
}

//---------------------------------------------------------------------------------------
lmElement::lmElement(lmContentStorage* pParent, lmElement* pE, int nPos, int nLevel)
    : m_pParent(pParent)
{
    //Clone with update positions

    m_nLevel = pE->GetLevel() + nLevel;
    m_nStartOpen = pE->StartOpen() + nPos;
    m_nStartClose = pE->StartClose() + nPos;
    m_nNameLength = pE->NameLength();
    m_fIsSingle = pE->IsSingle();
    m_nEndOpen = pE->EndOpen() + nPos;
    m_nEndClose = pE->EndClose() + nPos;
    m_fIsClosed = pE->IsClosed();
    m_fIsPlaceholder = pE->IsPlaceholder();
}

//---------------------------------------------------------------------------------------
lmElement::~lmElement()
{
    ClearElements();
}

//---------------------------------------------------------------------------------------
void lmElement::AddClose(int nEndOpen, int nEndClose)
{
    m_nEndOpen = nEndOpen;
    m_nEndClose = nEndClose;
    m_fIsClosed = true;
}

//---------------------------------------------------------------------------------------
void lmElement::Dump()
{
    if (!IsEmpty())
    {
        if (m_fIsSingle)
            wxLogMessage(wxString::Format(_T("Tag='%s', level=%d, single (%d, %d), IsPH=%s, Replz='%s'"),
                         m_pParent->GetContent().Mid(m_nStartOpen+1, m_nNameLength).c_str(),
                         m_nLevel,
                         m_nStartOpen, m_nStartClose,
                         (IsPlaceholder() ? _T("true") : _T("false")),
                         m_sOpenReplacement.c_str()
                        ));
        else
        {
            wxLogMessage(wxString::Format(_T("Tag='%s', level=%d, open (%d, %d), close(%d, %d), IsPH=%s, OpenReplz='%s', CloseReplz='%s'"),
                         m_pParent->GetContent().Mid(m_nStartOpen+1, m_nNameLength).c_str(),
                         m_nLevel,
                         m_nStartOpen, m_nStartClose, m_nEndOpen, m_nEndClose,
                         (IsPlaceholder() ? _T("true") : _T("false")),
                         m_sOpenReplacement.c_str(), m_sCloseReplacement.c_str()
                        ));
        }
    }

    if (m_elements.size() > 0)
    {
        wxLogMessage(wxString::Format(_T("Num. elements = %d"), m_elements.size()));

        std::vector<lmElement*>::iterator it;
        for (it = m_elements.begin(); it != m_elements.end(); ++it)
            (*it)->Dump();
    }
}

//---------------------------------------------------------------------------------------
void lmElement::AddOpenTag(lmElement* pE, int nLevel)
{
    if (nLevel > 0)
    {
        m_pCurElement->AddOpenTag(pE, --nLevel);
    }
    else
    {
        m_elements.push_back(pE);
        m_pCurElement = pE;
    }
}

//---------------------------------------------------------------------------------------
lmElement* lmElement::AddCloseTag(int nStart, int nEnd, int nLevel)
{
    //returns pointer to element that is closed by this action

    if (nLevel > 0)
    {
        return m_pCurElement->AddCloseTag(nStart, nEnd, --nLevel);
    }
    else
    {
        //find element and close it
        std::vector<lmElement*>::reverse_iterator it;
        for (it = m_elements.rbegin(); it != m_elements.rend(); ++it)
        {
            if (!(*it)->IsClosed())
            {
                (*it)->AddClose(nStart, nEnd);
                return *it;
            }
        }
        wxLogMessage(_T("ERROR: AddCloseTag. No open element to close!"));
        wxASSERT(false);
        return NULL;        //compiler happy
    }
}

//---------------------------------------------------------------------------------------
void lmElement::AddSingleTag(lmElement* pE, int nLevel)
{
    if (nLevel > 0)
        m_pCurElement->AddSingleTag(pE, --nLevel);
    else
        m_elements.push_back(pE);
}

//---------------------------------------------------------------------------------------
void lmElement::ClearElements()
{
    std::vector<lmElement*>::iterator it;
    for (it = m_elements.begin(); it != m_elements.end(); ++it)
        delete *it;
    m_elements.clear();
}

//---------------------------------------------------------------------------------------
wxString lmElement::GetTagName()
{
    return m_pParent->GetContent().Mid(m_nStartOpen+1, m_nNameLength);
}

//---------------------------------------------------------------------------------------
void lmElement::ClosePlaceholder(int nTag, lmContentStorage* pCS)
{
    //get tag name info
    wxString sName = GetTagName();

    //if (lmTags::is_placeholder_tag(sName))
    //    pCS->Add(wxString::Format(_T("</%s-%d>"), sName, nTag));
    //else
    //    pCS->Add(wxString::Format(_T("</%s>"), sName));

    if (lmTags::is_placeholder_tag(sName) || lmTags::is_supress_tag(sName))
        pCS->AddElementClose(wxString::Format(_T("</%s-%d>"), sName.c_str(), nTag), GetCloseTag());
    else
        pCS->AddElementClose(wxString::Format(_T("</%s>"), sName.c_str()));
}

//---------------------------------------------------------------------------------------
void lmElement::OpenPlaceholder(int nTag, lmContentStorage* pCS)
{
    //get tag name info
    wxString sName = GetTagName();

    if (lmTags::is_placeholder_tag(sName) || lmTags::is_supress_tag(sName))
        pCS->AddElementOpen(wxString::Format(_T("<%s-%d>"), sName.c_str(), nTag), GetOpenTag());
    else
        pCS->AddElementOpen(wxString::Format(_T("<%s>"), sName.c_str()));
}

//---------------------------------------------------------------------------------------
void lmElement::SinglePlaceholder(int nTag, lmContentStorage* pCS)
{
    //get tag name info
    wxString sName = GetTagName();

    //AWARE: There is a bug in wxHtml and it needs a space for tag <br/> --> <br />. Otherwise
    //it doesn't work. Therefore I add spaces to close single tags
    if (lmTags::is_supress_tag(sName))
        pCS->AddElementSingle(wxString::Format(_T("<%s-%d />"), sName.c_str(), nTag), GetFullContent() );
    else if (lmTags::is_placeholder_tag(sName))
        pCS->AddElementSingle(wxString::Format(_T("<%s-%d />"), sName.c_str(), nTag), GetOpenTag() );
    else
        pCS->AddElementSingle(wxString::Format(_T("<%s />"), sName.c_str()));
}

//---------------------------------------------------------------------------------------
void lmElement::ToPo(lmContentStorage* pMsg)
{
    //filter content
    const int nMAX_LEVEL = 30;
    int nNumPH[nMAX_LEVEL];     //placeholder number for each tag level
    m_pParent->ResetNumPH();    //reset placeholder number

    if (m_elements.size() == 0)
    {
        //just copy content
        if (IsEmpty())
            pMsg->Add(m_pParent->GetContent());
        else
            pMsg->Add(m_pParent->GetContent().Mid(m_nStartClose+1, ContentLenght()));
    }
    else
    {
        int iCur = (IsEmpty() ? 0 : StartClose() + 1);
        std::vector<lmElement*>::iterator it;
        for (it = m_elements.begin(); it != m_elements.end(); ++it)
        {
            int iStart = (*it)->StartOpen();
            int iEnd = (*it)->StartClose();
            int nLevel = (*it)->GetLevel();

            //add content until start of tag
            if (iStart > iCur)
                pMsg->Add( m_pParent->GetContent().Mid(iCur, iStart-iCur) );

            //replace tag
            if ( (*it)->IsSingle())
            {
                (*it)->SinglePlaceholder(m_pParent->GetNextNumPH(), pMsg);
            }
            else if (lmTags::is_supress_tag((*it)->GetTagName()) )
            {
                (*it)->SinglePlaceholder(m_pParent->GetNextNumPH(), pMsg);
                iEnd = (*it)->EndClose();
            }
            else
            {
                //open tag
                nNumPH[nLevel] = m_pParent->GetNextNumPH();
                (*it)->OpenPlaceholder(nNumPH[nLevel], pMsg);

                //add tag content
                (*it)->ToPo(pMsg);

                //close tag
                iStart = (*it)->EndOpen();
                iEnd = (*it)->EndClose();
                (*it)->ClosePlaceholder(nNumPH[nLevel], pMsg);
            }
            iCur = iEnd+1;
        }

        //add content from last element to end of this element content
        int iEnd = (IsEmpty() ? m_pParent->GetContent().length() - 1 :
                                (IsSingle() ? 0 : EndOpen() - 1) );
        if (iCur <= iEnd)
            pMsg->Add( m_pParent->GetContent().Mid(iCur, iEnd - iCur + 1) );
    }
}

//---------------------------------------------------------------------------------------
void lmElement::FromPo(lmContentStorage* pPoMsg, lmContentStorage* pResult)
{
    //restore placeholders content
    //pPoMsg:   the PO msg (already parsed, with replacements)
    //pResult:  the CS to place the result
    //the owner CS: the translation (parsed, but no replacements / PH marks)

    if (m_elements.size() == 0)
    {
        //just copy content
        if (IsEmpty())
            pResult->Add(m_pParent->GetContent());
        else
            pResult->Add(m_pParent->GetContent().Mid(m_nStartClose+1, ContentLenght()));
    }
    else
    {
        int iCur = (IsEmpty() ? 0 : StartClose() + 1);
        std::vector<lmElement*>::iterator it;
        for (it = m_elements.begin(); it != m_elements.end(); ++it)
        {
            int iStart = (*it)->StartOpen();

            //add content until start of tag
            if (iStart > iCur)
                pResult->Add( m_pParent->GetContent().Mid(iCur, iStart-iCur) );

            //restore tag/placeholder
            wxString sTagName = (*it)->GetTagName();
            if (sTagName.Find(_T('-')) >= 0)
            {
                //restore original tag content
                pResult->Add( pPoMsg->FindOpenReplacement((*it)->GetOpenTag()) );
                iCur = (*it)->StartClose() + 1;

                if (!(*it)->IsSingle())
                {
                    //add tag content
                    (*it)->FromPo(pPoMsg, pResult);

                    //close tag
                    pResult->Add( pPoMsg->FindCloseReplacement((*it)->GetCloseTag()) );
                    iCur = (*it)->EndClose() + 1;
                }
            }
            else
            {
                //Copy tag
                pResult->Add( (*it)->GetOpenTag() );
                iCur = (*it)->StartClose() +1;

                if (!(*it)->IsSingle())
                {
                    //add tag content
                    (*it)->FromPo(pPoMsg, pResult);

                    //close tag
                    pResult->Add( (*it)->GetCloseTag() );
                    iCur = (*it)->EndClose() + 1;
                }
            }
        }

        //add content from last element to end of buffer
        int iEnd = (IsEmpty() ? m_pParent->GetContent().length() - 1 :
                                (IsSingle() ? 0 : EndOpen() - 1) );
        if (iCur <= iEnd)
            pResult->Add( m_pParent->GetContent().Mid(iCur, iEnd - iCur + 1) );
    }
}

//---------------------------------------------------------------------------------------
int lmElement::EndOpen()
{
    return (IsEmpty() ? m_pParent->GetContent().length()-1 : m_nEndOpen);
}

//---------------------------------------------------------------------------------------
int lmElement::EndClose()
{
    return (IsEmpty() ? m_pParent->GetContent().length()-1 : m_nEndClose);
}

//---------------------------------------------------------------------------------------
wxString lmElement::GetOpenTag()
{
    if (IsEmpty())
        return wxEmptyString;
    return m_pParent->GetContent().Mid(m_nStartOpen, m_nStartClose-m_nStartOpen+1);
}

//---------------------------------------------------------------------------------------
wxString lmElement::GetCloseTag()
{
    if (IsEmpty() || IsSingle())
        return wxEmptyString;
    return m_pParent->GetContent().Mid(m_nEndOpen, m_nEndClose-m_nEndOpen+1);
}

//---------------------------------------------------------------------------------------
wxString lmElement::GetFullContent()
{
    if (IsEmpty())
        return wxEmptyString;
    return m_pParent->GetContent().Mid(m_nStartOpen, m_nEndClose-m_nStartOpen+1);
}

//---------------------------------------------------------------------------------------
lmElement* lmElement::FindOpen(const wxString& sTag)
{
    if (GetOpenTag() == sTag)
        return this;

    if (m_elements.size() == 0)
        return (lmElement*)NULL;

    std::vector<lmElement*>::iterator it;
    for (it = m_elements.begin(); it != m_elements.end(); ++it)
    {
        lmElement* pE = (*it)->FindOpen(sTag);
        if (pE)
            return pE;
    }
    return (lmElement*)NULL;
}

//---------------------------------------------------------------------------------------
lmElement* lmElement::FindClose(const wxString& sTag)
{
    if (GetCloseTag() == sTag)
        return this;

    if (m_elements.size() == 0)
        return (lmElement*)NULL;

    std::vector<lmElement*>::iterator it;
    for (it = m_elements.begin(); it != m_elements.end(); ++it)
    {
        lmElement* pE = (*it)->FindClose(sTag);
        if (pE)
            return pE;
    }
    return (lmElement*)NULL;
}

//---------------------------------------------------------------------------------------
void lmElement::OptimizePoMsge()
{
    //This element is embracing the real content and is going to be supressed from
    //po message

    //remove spaces, new lines and single tags from start and end of content
    int nEnd = (IsSingle() ? StartClose() : EndOpen() - 1);
    int nStart = StartClose() + 1;
    if (IsEmpty())
    {
        nEnd++;
        nStart--;
    }
    int iE = m_pParent->RemoveSpacesNewLines(nStart, nEnd, this);

    //if no children elements, or no more elements to process, done.
    if (m_elements.size() == 0 || iE >= (int)m_elements.size())
        return;

    //check if first child is embracing remaining content and, if so, recurse
    lmElement* pE = m_elements[iE];
    int iStart = m_pParent->GetStartOpt();
    int iEnd = m_pParent->GetEndOpt();
    if (pE->IsSingle())
    {
        if (pE->StartOpen() == iStart && pE->StartClose() == iEnd)
            pE->OptimizePoMsge();
    }
    else if (pE->StartOpen() == iStart && pE->EndClose() == iEnd)
        pE->OptimizePoMsge();
}



//---------------------------------------------------------------------------------------
// lmContentStorage implementation
//---------------------------------------------------------------------------------------

lmContentStorage::lmContentStorage()
    : m_sBuffer(_T(""))
    , m_nMaxLevel(0)
    , m_pOwnerElement((lmElement*)NULL)
    , m_nStartSpaces(0)
    , m_nFinalSpaces(0)
    , m_nCurLevel(0)
    , m_fTranslated(false)
{
    CreateOwnerElement();
}

//---------------------------------------------------------------------------------------
lmContentStorage::~lmContentStorage()
{
    if (m_pOwnerElement)
        delete m_pOwnerElement;
}

//---------------------------------------------------------------------------------------
void lmContentStorage::Add(wxChar c)
{
    m_sBuffer.Append(c);
}

//---------------------------------------------------------------------------------------
void lmContentStorage::Add(const wxString& str)
{
    m_sBuffer.Append(str);
}

//---------------------------------------------------------------------------------------
void lmContentStorage::Add(const wxString* str)
{
    if(str) Add(*str);
}

//---------------------------------------------------------------------------------------
void lmContentStorage::Add(const wxChar* str)
{
    while(*str)
    {
        Add(*str++);
    }
}

//---------------------------------------------------------------------------------------
void lmContentStorage::AddNewLine()
{
    Add(_T('\n'));
}

//---------------------------------------------------------------------------------------
void lmContentStorage::Add(lmContentStorage* pCS)
{
    Add(pCS->GetContent());
    m_fTranslated = pCS->IsTranslated();
}

//---------------------------------------------------------------------------------------
void lmContentStorage::Clear()
{
    m_sBuffer = _T("");
    if (m_pOwnerElement)
    {
        delete m_pOwnerElement;
        m_pOwnerElement = (lmElement*)NULL;
    }
}

//---------------------------------------------------------------------------------------
void lmContentStorage::Dump()
{
    wxLogMessage(m_sBuffer);
    if (IsParsed())
        m_pOwnerElement->Dump();
}

//-- translation ------------------------------------------------------------

//---------------------------------------------------------------------------------------
wxString lmContentStorage::GeneratePoMessage(lmContentStorage* pResult)
{
    //pResult: the CS to place the generated Po message
    ParseElements();

    //update m_iStartOpt and m_iEndOpt indexes to mark parts to supress
    m_pOwnerElement->OptimizePoMsge();

    //copy optimized content to a new CS, to replace tags by placeholders
    if (m_iStartOpt <= m_iEndOpt)
    {
        lmContentStorage csPo;
        csPo.Add( m_sBuffer.Mid(m_iStartOpt, m_iEndOpt - m_iStartOpt + 1) );
        csPo.ReplaceTagsByPlaceholders(pResult);

        return pResult->GetContent();
    }
    else
        return wxEmptyString;
}

//---------------------------------------------------------------------------------------
void lmContentStorage::ReplaceTagsByPlaceholders(lmContentStorage* pResult)
{
    //pResult: the CS to place the generated Po message
    //this CS: the original content, without initial & final spaces, NLs and embracing tags

    ParseElements();
    ResetNumPH();
    m_pOwnerElement->ToPo(pResult);
}

//---------------------------------------------------------------------------------------
void lmContentStorage::GenerateTranslation(lmContentStorage* pPoMsg,
                                            const wxString& sTrans)
{
    //this CS:  the original content (already parsed, supress indexes updated)
    //pPoMsg:   the result of transforming this CS to generate the PO msg (is parsed).
    //sTrans:   the translation of pPoMsg

    //parse translation
    lmContentStorage csTrans;
    csTrans.Add( sTrans );
    csTrans.ParseElements();

    //restore supressed content (tags, spaces, NLs) at beginning
    lmContentStorage csResult;
    if (m_iStartOpt > 0)
        csResult.Add( m_sBuffer.Mid(0, m_iStartOpt) );

    //replace placeholders by original tags
    csTrans.RestorePlaceholders(pPoMsg, &csResult);

    //restore supressed content (tags, spaces, NLs) at end
    m_iEndOpt++;        //point to first char excluded
    if (m_iEndOpt > m_iStartOpt && m_iEndOpt < (int)m_sBuffer.length()-1)
        csResult.Add( m_sBuffer.Mid(m_iEndOpt) );

    //wxLogMessage(_T("Original: ------------------------------------------------------------"));
    //wxLogMessage(GetContent());
    //wxLogMessage(_T("PoMsg: ---------------------------------------------------------------"));
    //wxLogMessage(pPoMsg->GetContent());
    //wxLogMessage(_T("Trans: ---------------------------------------------------------------"));
    //wxLogMessage(csResult.GetContent());

    //transfer result to this CS
    Clear();
    m_sBuffer = csResult.GetContent();
    m_fTranslated = true;
}

//---------------------------------------------------------------------------------------
void lmContentStorage::RestorePlaceholders(lmContentStorage* pPoMsg,
                                           lmContentStorage* pResult)
{
    //pOrig:    original text, parsed and with updated supress indexes
    //pPoMsg:   the result of transforming pOrig to generate the PO msg (is parsed).
    //this CS:  the translation of pPoMsg (already parsed)
    //pResult:  the CS to place the result

    wxASSERT(IsParsed());

    //restore placeholders
    m_pOwnerElement->FromPo(pPoMsg, pResult);
}

//---------------------------------------------------------------------------------------
void lmContentStorage::CreateOwnerElement()
{

    if (m_pOwnerElement)
        delete m_pOwnerElement;

    //create empty element enclosing all buffer: the owner element
    m_pOwnerElement = new lmElement(this, 0, 0, 0, false, 0);
}

//--- parsing -----------------------------------------------------------

//---------------------------------------------------------------------------------------
void lmContentStorage::ParseElements()
{
    CreateOwnerElement();

    //analyze buffer
    int nTagLevel = 0;
    int nLength = m_sBuffer.length();
    bool fNoChar = true;        //to count initial spaces
    int iStart = 0;
    bool fInTag = false;
    bool fClose = false;
    for(int i = 0; i < nLength; ++i)
    {
        if (m_sBuffer[i] == _T('<'))
        {
            iStart = i;
            fInTag = true;
            fClose = (i < (nLength-1) && m_sBuffer[i+1] == _T('/'));
            if (fClose) i++;    //skip '/'
        }
        else if (m_sBuffer[i] == _T('>'))
        {
            if (fInTag)
            {
                if (fClose)
                {
                    CloseElement(--nTagLevel, iStart, i);
                }
                else
                {
                    OpenElement(nTagLevel++, iStart, i);
                    m_nMaxLevel = wxMax(m_nMaxLevel, nTagLevel);
                }
                fInTag = false;
            }
        }
        else if (m_sBuffer[i] == _T('/'))
        {
            if (fInTag)
            {
                if (i < (nLength-1) && m_sBuffer[i+1] == _T('>'))
                {
                    i++;
                    SingleElement(nTagLevel, iStart, i);
                    fInTag = false;
                }
            }
        }
        else if (fNoChar)
        {
            if (m_sBuffer[i] == _T(' '))
                ++m_nStartSpaces;
            else
                fNoChar = false;
        }
    }

    //count final spaces
    if (fNoChar)
        return;

    for(int i = nLength-1; i >= 0; --i)
    {
        if (m_sBuffer[i] == _T(' '))
            ++m_nFinalSpaces;
        else
            break;
    }

    //Dump();
}

//---------------------------------------------------------------------------------------
void lmContentStorage::OpenElement(int nLevel, int nStart, int nEnd)
{
    wxASSERT(IsParsed());

    //get tag name info
    wxString sTag = m_sBuffer.Mid(nStart, nEnd - nStart + 1);
    int nNameLength = sTag.Find(_T(' '));
    if (nNameLength == wxNOT_FOUND)
        nNameLength = nEnd - nStart - 1;
    else
        nNameLength -= 1;

    //create element
    lmElement* pE = new lmElement(this, nStart, nEnd, nNameLength, false, nLevel);
    m_pOwnerElement->AddOpenTag(pE, nLevel);
}

//---------------------------------------------------------------------------------------
void lmContentStorage::CloseElement(int nLevel, int nStart, int nEnd)
{
    wxASSERT(IsParsed());
    m_pOwnerElement->AddCloseTag(nStart, nEnd, nLevel);
}

//---------------------------------------------------------------------------------------
void lmContentStorage::SingleElement(int nLevel, int nStart, int nEnd)
{
    wxASSERT(IsParsed());

    //get tag name info
    wxString sTag = m_sBuffer.Mid(nStart, nEnd - nStart + 1);
    int nNameLength = sTag.Find(_T(' '));
    if (nNameLength == wxNOT_FOUND)
        nNameLength = nEnd - nStart - 2;
    else
        nNameLength -= 1;

    //create element
    lmElement* pE = new lmElement(this, nStart, nEnd, nNameLength, true, nLevel);
    m_pOwnerElement->AddSingleTag(pE, nLevel);
}

//---------------------------------------------------------------------------------------
void lmContentStorage::AddElementOpen(const wxString& sTag, const wxString& sReplacement)
{
    //Adds a tag to open an element. If a replacement is specified, the element is
    //marked as 'placeholder' and its replacement is saved.

    if(!IsParsed())
        CreateOwnerElement();

    //get tag info
    int nStart = 0;
    int nEnd = sTag.length() - 1;
    int nNameLength = sTag.Find(_T(' '));
    if (nNameLength == wxNOT_FOUND)
        nNameLength = nEnd - nStart - 2;
    else
        nNameLength -= 1;

    //add tag to buffer
    int nPos = m_sBuffer.length();
    Add(sTag);

    //create element
    int nLevel = m_nCurLevel++;
    lmElement* pE = new lmElement(this, nStart+nPos, nEnd+nPos, nNameLength, false, nLevel);
    m_pOwnerElement->AddOpenTag(pE, nLevel);
    if (!sReplacement.IsEmpty())
    {
        pE->MarkAsPlaceholder(true);
        pE->SetOpenReplacement(sReplacement);
    }
}

//---------------------------------------------------------------------------------------
void lmContentStorage::AddElementClose(const wxString& sTag, const wxString& sReplacement)
{
    //Adds a tag to close an element. A replacement must be specified iff
    //the element was open as 'placeholder'.

    wxASSERT(IsParsed());

    //get tag info
    int nStart = 0;
    int nEnd = sTag.length() - 1;
    int nNameLength = sTag.Find(_T(' '));
    if (nNameLength == wxNOT_FOUND)
        nNameLength = nEnd - nStart - 3;
    else
        nNameLength -= 2;

    //add tag to buffer
    int nPos = m_sBuffer.length();
    Add(sTag);

    //close the element
    int nLevel = --m_nCurLevel;
    lmElement* pE = m_pOwnerElement->AddCloseTag(nStart+nPos, nEnd+nPos, nLevel);
    if (!sReplacement.IsEmpty())
    {
        wxASSERT(pE->IsPlaceholder());
        pE->SetCloseReplacement(sReplacement);
    }
    else
    {
        wxASSERT(!pE->IsPlaceholder());
    }
}

//---------------------------------------------------------------------------------------
void lmContentStorage::AddElementSingle(const wxString& sTag, const wxString& sReplacement)
{
    //Adds a tag to create a single element. If a replacement is specified, the element
    //is marked as 'placeholder' and its replacement is saved.

    if(!IsParsed())
        CreateOwnerElement();

    //get tag info
    int nStart = 0;
    int nEnd = sTag.length() - 1;
    int nNameLength = sTag.Find(_T(' '));
    if (nNameLength == wxNOT_FOUND)
        nNameLength = nEnd - nStart - 3;
    else
        nNameLength -= 1;

    //add tag to buffer
    int nPos = m_sBuffer.length();
    Add(sTag);

    //create element
    int nLevel = m_nCurLevel;
    lmElement* pE = new lmElement(this, nStart+nPos, nEnd+nPos, nNameLength, true, nLevel);
    m_pOwnerElement->AddSingleTag(pE, nLevel);
    if (!sReplacement.IsEmpty())
    {
        pE->MarkAsPlaceholder(true);
        pE->SetOpenReplacement(sReplacement);
    }
}

//---------------------------------------------------------------------------------------
const wxString& lmContentStorage::FindOpenReplacement(const wxString& sTag)
{
    //find a single/open tag whose content is sTag and returns its replacement

    lmElement* pE = m_pOwnerElement->FindOpen(sTag);
    if (pE)
        return pE->GetOpenReplacement();
    else
        return lmEmptyString;
}

//---------------------------------------------------------------------------------------
const wxString& lmContentStorage::FindCloseReplacement(const wxString& sTag)
{
    //find a close tag whose content is sTag and returns its replacement

    lmElement* pE = m_pOwnerElement->FindClose(sTag);
    if (pE)
        return pE->GetCloseReplacement();
    else
        return lmEmptyString;
}

//---------------------------------------------------------------------------------------
int lmContentStorage::RemoveSpacesNewLines(int iStart, int iEnd, lmElement* pE)
{
    //this CS contains the parsed po msg.
    //Receives indexes to first and last content chars to analyze, as well as
    //a pointer to the owner element of that content
    //
    //Updates m_iStartOpt and m_iEndOpt to remove spaces, new line chars and
    //single tags. And return index to first child element not supressed.

    //update indexes
    m_iStartOpt = iStart;
    m_iEndOpt = iEnd;

    //loop to remove chars ' ' & '\n' from start and remove single tags after them.
    //loop while tag removed.
    //As we are removing single elements from beginnig, all them are child of
    //the content owner element (otherwise we should find a non-single element
    //embracing them).
    int iElement = 0;
    while(true)
    {
        //remove chars ' ' & '\n' from start
        while (m_iStartOpt < m_iEndOpt &&
            (m_sBuffer[m_iStartOpt] == _T(' ') || m_sBuffer[m_iStartOpt] == _T('\n')) )
        {
            m_iStartOpt++;
        }
        //check for a possible single tag after the removed content, and remove
        //it if found
        if (pE->GetNumChildren() > iElement)
        {
            lmElement* pChild = pE->GetChild(iElement);
            if (!pChild->IsSingle() || pChild->StartOpen() != m_iStartOpt)
                break;  //no tag removed
            m_iStartOpt = pChild->StartClose() + 1;
            iElement++;
        }
        else
            break;  //no tag removed
    }


    //loop to remove chars ' ' & '\n' from final and remove single tags before them.
    //loop while tag removed.
    //As we are removing single elements from final, all them must be child of
    //the content owner element (otherwise we should find a non-single element
    //embracing them).
    int iE = pE->GetNumChildren() - 1;
    while(true)
    {
        //remove final chars ' ' & '\n'
        while (m_iStartOpt < m_iEndOpt &&
            (m_sBuffer[m_iEndOpt] == _T(' ') || m_sBuffer[m_iEndOpt] == _T('\n')) )
        {
            m_iEndOpt--;
        }
        //check for a possible single tag before the removed content, and remove
        //it if found
        if (iE >= 0)
        {
            lmElement* pChild = pE->GetChild(iE);
            if (!pChild->IsSingle() || pChild->StartClose() != m_iEndOpt)
                break;  //no tag removed
            m_iEndOpt = pChild->StartOpen() - 1;
            iE--;
        }
        else
            break;  //no tag removed
    }

    return iElement;
}



//=======================================================================================
// lmEbookProcessor
//=======================================================================================

//---------------------------------------------------------------------------------------
lmEbookProcessor::lmEbookProcessor(int nDbgOptions, wxTextCtrl* pUserLog)
    : m_pTocFile((wxFile*) NULL)
    , m_pHtmlFile((wxFile*) NULL)
    , m_pLangFile((wxFile*)NULL)
    , m_pLmbFile((wxTextOutputStream*)NULL)
    , m_pZipFile((wxZipOutputStream*)NULL)
    , m_nPlaceHolder(0)
    , m_pLog(pUserLog)
    , m_fInScoreTag(false)
{
    //options. TODO: dialog to change options
    m_fGenerateLmb = true;

    //debug options
    m_fLogTree = (nDbgOptions & eLogTree) != 0;
    m_fDump = (nDbgOptions & eDumpTree) != 0;

    load_tags();
}

//---------------------------------------------------------------------------------------
lmEbookProcessor::~lmEbookProcessor()
{
    if (m_pTocFile) delete m_pTocFile;
    if (m_pHtmlFile) delete m_pHtmlFile;
    if (m_pLangFile) delete m_pLangFile;
    if (m_pLmbFile) delete m_pLmbFile;
    if (m_pZipFile) delete m_pZipFile;

}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::load_tags()
{
    //HTML replacements
    m_htmlTags.add_replacement( _T(""),             _T(""),         _T("") );     //empty lmElement, for errors
    m_htmlTags.add_replacement( _T("content"),      _T(""),         _T("") );
    m_htmlTags.add_replacement( _T("copyright"),    _T(""),         _T("") );
    m_htmlTags.add_replacement( _T("itemizedlist"), _T("<ul>\n"),   _T("</ul>\n") );
    m_htmlTags.add_replacement( _T("orderedlist"),  _T("<ol>\n"),   _T("</ol>\n") );
    m_htmlTags.add_replacement( _T("sbr"),          _T("<br />\n"), _T("") );      //wxHtml needs a space after 'br'
    m_htmlTags.add_replacement( _T("subscript"),    _T(" <sub>"),   _T("</sub>") );
    m_htmlTags.add_replacement( _T("superscript"),  _T(" <sup>"),   _T("</sup>") );
    m_htmlTags.add_replacement( _T("simplelist"),   _T("<ul>\n"),   _T("</ul>\n") );

    //LDP replacements
    m_ldpTags.add_replacement( _T(""),             _T(""),         _T("") );     //empty lmElement, for errors
    m_ldpTags.add_replacement( _T("content"),      _T(""),         _T("") );
    m_ldpTags.add_replacement( _T("copyright"),    _T(""),         _T("") );
    m_ldpTags.add_replacement( _T("itemizedlist"), _T("<ul>\n"),   _T("</ul>\n") );
    m_ldpTags.add_replacement( _T("orderedlist"),  _T("<ol>\n"),   _T("</ol>\n") );
    m_ldpTags.add_replacement( _T("sbr"),          _T("\n"), _T("") );
    m_ldpTags.add_replacement( _T("subscript"),    _T(" "),   _T(" ") );
    m_ldpTags.add_replacement( _T("superscript"),  _T(" "),   _T(" ") );
    m_ldpTags.add_replacement( _T("simplelist"),   _T("<ul>\n"),   _T("</ul>\n") );
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::GenerateLMB(wxString sFilename, wxString sLangCode,
                                   wxString sCharCode, int nOptions)
{
    // returns false if error
    // PO and lang.cpp are created in sDestName (langtool\locale\)
    // book.lmb is created in sDestName (lenmus\locale\xx\books\)
    // book.toc is temporarily created in sDestName (lenmus\locale\xx\books\)

    //decide tags to use
    if (nOptions & k_generate_html)
    {
        m_format = k_format_html;
        m_tags = &m_htmlTags;
    }
    else
    {
        m_format = k_format_ldp;
        m_tags = &m_ldpTags;
    }

    // Prepare for a new XML file processing
    m_fProcessingBookinfo = false;
    m_fOnlyLangFile = nOptions & k_generate_lang_file;
    m_fGenerateLmb = !m_fOnlyLangFile;
    m_sFilename = sFilename;
    m_sCharCode = sCharCode;
    m_sLangCode = sLangCode;

    //add layout files
    m_aFilesToPack.Empty();


    // load the XML file as tree of nodes
    LogMessage(_T("Loading file %s"), sFilename.c_str());
    wxXml2Document oDoc;
    wxString sError;
    if (!oDoc.Load(sFilename, &sError)) {
        LogMessage(_T("*** Error parsing file %s\nError:%s"), sFilename.c_str(), sError.c_str());
        return false;
    }
    wxXml2Node oRoot = oDoc.GetRoot();

    if (m_fDump) DumpXMLTree(oRoot);        //for debugging

    m_fExtEntity = false;                   //not waiting for an external entity
    m_sExtEntityName = wxEmptyString;       //so, no name

    // Prepare Lang file
    if (m_fOnlyLangFile)
 {
        if (!StartLangFile( sFilename )) {
            LogMessage(_T("*** Error: Lang file '%s' can not be created."), sFilename.c_str());
            oRoot.DestroyIfUnlinked();
            oDoc.DestroyIfUnlinked();
            return false;        //error
        }
    }

    // Prepare the TOC file
    if (!StartTocFile( sFilename ))
    {
        LogMessage(_T("*** Error: toc file '%s' can not be created."), sFilename.c_str());
        oRoot.DestroyIfUnlinked();
        oDoc.DestroyIfUnlinked();
        return false;        //error
    }

    // prepare de LMB file
    if (m_fGenerateLmb)
    {
        if (!StartLmbFile(sFilename, sLangCode, m_sCharCode))
        {
            LogMessage(_T("*** Error: LMB file '%s' can not be created."), sFilename.c_str());
            oRoot.DestroyIfUnlinked();
            oDoc.DestroyIfUnlinked();
            return false;        //error
        }
    }

    //Process the document. Root node must be <book> or <article>
    bool fError = false;
    if (oRoot.GetName() == _T("book") || oRoot.GetName() == _T("article"))
        fError = BookArticleTag(oRoot, oRoot.GetName());
    else
    {
        LogMessage(
            _T("*** Error. First tag is neither <book> nor <article> but <%s>"),
            oRoot.GetName().c_str() );
        oRoot.DestroyIfUnlinked();
        oDoc.DestroyIfUnlinked();
        return false;
    }

    // Close files
    TerminateTocFile();
    TerminateLmbFile();

    oRoot.DestroyIfUnlinked();
    oDoc.DestroyIfUnlinked();

    if (fError)
        LogMessage(_T("There are errors in compilation."));
    else
        LogMessage(_T("Compilation done successfully."));

    return !fError;

}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::GetTagContent(const wxXml2Node& oNode, lmContentStorage* pResult,
                                     bool fInTextContainerTag)
{
    lmContentStorage csContent;
    bool fError = ProcessChildAndSiblings(oNode, &csContent, fInTextContainerTag);

    if (!pResult) return false;   //content is useless!

    //deal with translation
    if (is_po_msg_delimiter_tag(oNode.GetName()))
    {
        wxLogMessage(_T("Tag: %s ------------------------------------------------------------"),
            oNode.GetName().c_str() );
        wxLogMessage(csContent.GetContent());

        TranslateContent(&csContent);

        wxLogMessage(_T("Translation:"));
        wxLogMessage(csContent.GetContent());

        wxString trans = csContent.GetContent();

        if (fInTextContainerTag && m_format == k_format_ldp && m_fTextTagOpen)
            pResult->Add(_T("(txt (style \"eBook_normal\") \""));
        pResult->Add(trans);
    }
    else
    {
        //add content
        pResult->Add(&csContent);
    }

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ProcessChildAndSiblings(const wxXml2Node& oNode,
                                               lmContentStorage* pResult,
                                               bool fInTextContainerTag)
{
    m_fExtEntity = false;                   //not waiting for an external entity
    m_sExtEntityName = wxEmptyString;       //so, no name

    wxXml2Node oCurr(oNode.GetFirstChild());
    bool fError = false;
    while (oCurr != wxXml2EmptyNode) {
        fError |= ProcessChildren(oCurr, pResult, fInTextContainerTag);
        oCurr = oCurr.GetNext();
    }
    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ProcessChildren(const wxXml2Node& oNode,
                                       lmContentStorage* pResult,
                                       bool fInTextContainerTag)
{
    bool fError = false;

    if (oNode == wxXml2EmptyNode) return false;
    if (m_fLogTree) wxLogMessage(_T("[ProcessChildren] nodeType=%d. Name='%s', content='%s'"),
                                 oNode.GetType(), oNode.GetName().c_str(), oNode.GetContent().c_str() );

    if (oNode.GetType() == wxXML_TEXT_NODE)
    {
        // it is a text node: write its contents to output
        wxString sContent = oNode.GetContent();
        if (sContent != wxEmptyString && sContent.Last() == wxT('\n')) sContent.RemoveLast();
        if (sContent != wxEmptyString && sContent.GetChar(0) == wxT('\n')) sContent.Remove(0, 1);

        // libxml2 creates text nodes associated to all elements, even
        // when no content is present in the xml file. In these cases
        // the text node contains just a simple '\n'. Next code lines
        // are for filtering ou these lines
        wxString tmp = sContent.Trim();
        sContent.Replace(_T("\n"), _T(" "));
        while (sContent.Replace(_T("  "), _T(" ")) > 0);    //more than one consecutive space

        //replace common entities
        //sContent.Replace(_T("\""), _T("&quot;"), true);
        //sContent.Replace(_T("<"), _T("&lt;"), true);
        //sContent.Replace(_T(">"), _T("&gt;"), true);

        //sContent.Replace(_T("í"), _T("&iacute;"), true);
        //sContent.Replace(_T("ó"), _T("&oacute;"), true);
        //sContent.Replace(_T("ú"), _T("&uacute;"), true);
        //sContent.Replace(_T("ñ"), _T("&ntilde;"), true);
        //sContent.Replace(_T("Ñ"), _T("&Ntilde;"), true);

        if (!tmp.IsEmpty())
        {
            if (fInTextContainerTag && m_format == k_format_ldp)
            {
                //text nodes requires special processing if inside certain
                //tags (<para>, <listitem> ) as in this cases it is necessary
                //to open the <text> ldp tag if not yet created
                if (!m_fTextTagOpen && pResult != NULL)
                {
                    //pResult->Add( _T("(txt (style \"eBook_normal\") \"") );
                    m_fTextTagOpen = true;
                }

            }

            //add children content to main content
            if (pResult) pResult->Add(sContent);
        }

    }
    else if (oNode.GetType() == wxXML_ELEMENT_NODE)
    {
        // it is an lmElement. Process it (recursive, as ProcessTags call ProcessChildAndSiblings)

        //if (fInTextContainerTag && m_format == k_format_ldp)
        //{
        //    //text nodes requires special processing if inside certain
        //    //tags (<para>, <listitem> ) as in this cases it is necessary
        //    //to open the <text> ldp tag if not yet created
        //    if (m_fTextTagOpen && pResult != NULL)
        //    {
        //        //pResult->Add( _T(" \")") );      //close text tag
        //        m_fTextTagOpen = false;
        //    }
        //}
        fError |= ProcessTag(oNode, pResult);
    }
    else if (oNode.GetType() == wxXML_ENTITY_DECL)
    {
	    // A DTD node which declares an entity.
	    // This value is used to identify a node as a wxXml2EntityDecl.
	    // Looks like:     <!ENTITY myentity "entity's replacement">
        wxXml2EntityDecl* pNode = (wxXml2EntityDecl*)&oNode;
        //wxLogMessage(_T("[ProcessChildren] External entity (17). Name='%s', SystemID='%s'"),
        //             oNode.GetName(), pNode->GetSystemID() );

        if (m_fExtEntity &&  m_sExtEntityName == oNode.GetName())
        {
            // Insert here the referenced xml tree

            // prepare full URL
            // TODO: For now, assume that external entities are in the same folder than
            //       main xml file. Need to improve this.
            //wxString sOldFilename = m_sFilename;
            wxFileName oFN(m_sFilename);
            oFN.SetFullName(pNode->GetSystemID());
            //m_sFilename = oFN.GetFullPath();

            wxXml2Document oDoc;
            wxString sError;
            LogMessage(_T("Processing %s."), oFN.GetFullName().c_str());
            if (!oDoc.Load(oFN.GetFullPath(), &sError)) {
                LogMessage(_T("*** Error parsing file %s\nError:%s"), oFN.GetFullPath().c_str(), sError.c_str());
                return true;    //error
            }
            //Verify type of document. Must be <book>
            wxXml2Node oRoot = oDoc.GetRoot();
            ProcessChildren(oRoot, pResult, false);
            //m_sFilename = sOldFilename;

            //done
            m_fExtEntity = false;
            m_sExtEntityName = wxEmptyString;
        }
    }
    else if (oNode.GetType() == wxXML_ENTITY_REF_NODE) {
	    //! Like a text node, but this node contains only an "entity". Entities
	    //! are strings like: &amp; or &quot; or &lt; ....
        // It must have an wxXML_ENTITY_DECL child. Process it.

        // Save the name of the node
        m_fExtEntity = true;
        m_sExtEntityName = oNode.GetName();

        // and process children (must be the external entities definitions table)
        // the first one is the desired one. Ignore the remaining.
        wxXml2Node oChild(oNode.GetFirstChild());
        while (oChild != wxXml2EmptyNode) {
            ProcessChildren(oChild, pResult, false);
            oChild = oChild.GetNext();
        }
    }
    else {
        // ignore it
        //wxLogMessage(_T("[ProcessChildren] NodeType=%d, Name='%s'"),
        //                oNode.GetType(), oNode.GetName() );
    }

    return fError;

}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ProcessVariableAssignTag(const wxXml2Node& oNode,
                                                wxString* pVar)
{
    lmContentStorage cs;
    bool fError = GetTagContent(oNode, &cs);
    *pVar = cs.GetContent();
    return fError;
}


//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ProcessJustReplaceTag(const wxString& sTag,
                                             const wxXml2Node& oNode,
                                             lmContentStorage* pResult)
{
    //Get replacements
    const lmReplacement& tReplz = GetReplacement(sTag);

    // openning tag
    AddReplacement(tReplz.sOpen, pResult);

    //process tag's children and write content to html
    bool fError = GetTagContent(oNode, pResult);

    // closing tag
    AddReplacement(tReplz.sClose, pResult);

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ProcessTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    if (oNode == wxXml2EmptyNode) return false;
    //wxLogMessage(_T("[ProcessTag] NodeType=%d, Name='%s'"),
    //                oNode.GetType(), oNode.GetName() );

    if (oNode.GetType() != wxXML_ELEMENT_NODE) return false;

    bool fError = false;
    wxString sElement = oNode.GetName();
    //wxString spaces(wxT(' '), m_xx);
    //wxLogMessage(spaces + _T("[ProcessTag] - lmElement [%s]"), sElement.c_str());

    //Just replace tags
    if (is_just_replace_tag(sElement)) {
        fError = ProcessJustReplaceTag(sElement, oNode, pResult);
    }

    //variable assignemt tags
    else if (sElement == _T("abstract")) {
        fError = ProcessVariableAssignTag(oNode, &m_sBookAbstract);
    }
    else if (sElement == _T("author")) {
        fError = ProcessVariableAssignTag(oNode, &m_sAuthorName);
    }
    else if (sElement == _T("holder")) {
        fError = ProcessVariableAssignTag(oNode, &m_sCopyrightHolder);
    }
    else if (sElement == _T("legalnotice")) {
        fError = ProcessVariableAssignTag(oNode, &m_sLegalNotice);
    }
    else if (sElement == _T("year")) {
        fError = ProcessVariableAssignTag(oNode, &m_sCopyrightYear);
    }

    //processing tags
    else if (sElement == _T("bookinfo")) {
        LogMessage(_T("*** Warning: Tag 'bookinfo' obsolete. Replace by 'info'"));
        fError = InfoTag(oNode, pResult);
    }
    else if (sElement == _T("chapter")) {
        fError = ChapterTag(oNode, pResult);
    }
    else if (sElement == _T("col")) {
        fError = ColTag(oNode, pResult);
    }
    else if (sElement == _T("credits")) {
        fError = CreditsTag(oNode, pResult);
    }
    else if (sElement == _T("creditsitem")) {
        fError = CreditsitemTag(oNode, pResult);
    }
    else if (sElement == _T("emphasis")) {
        fError = EmphasisTag(oNode, pResult);
    }
    else if (sElement == _T("exercise")) {
        fError = ExerciseTag(oNode, pResult);
    }
    else if (sElement == _T("imagedata")) {
        fError = ImagedataTag(oNode, pResult);
    }
    else if (sElement == _T("info")) {
        fError = InfoTag(oNode, pResult);
    }
    else if (sElement == _T("link")) {
        fError = LinkTag(oNode, pResult);
    }
    else if (sElement == _T("listitem")) {
        fError = ListitemTag(oNode, pResult);
    }
    else if (sElement == _T("para")) {
        fError = ParaTag(oNode, pResult);
    }
    else if (sElement == _T("part")) {
        fError = PartTag(oNode, pResult);
    }
    else if (sElement == _T("score")) {
        fError = ScoreTag(oNode, pResult);
    }
    else if (sElement == _T("section")) {
        fError = SectionTag(oNode, pResult);
    }
    else if (sElement == _T("style")) {
        fError = StyleTag(oNode, pResult);
    }
    else if (sElement == _T("table")) {
        fError = TableTag(oNode, pResult);
    }
    else if (sElement == _T("tbody")) {
        fError = TbodyTag(oNode, pResult);
    }
    else if (sElement == _T("td")) {
        fError = TdTag(oNode, pResult);
    }
    else if (sElement == _T("theme")) {
        fError = ThemeTag(oNode, pResult);
    }
    else if (sElement == _T("title")) {
        fError = TitleTag(oNode, pResult);
    }
    else if (sElement == _T("titleabbrev")) {
        fError = TitleabbrevTag(oNode, pResult);
    }
    else if (sElement == _T("tr")) {
        fError = TrTag(oNode, pResult);
    }
    else if (sElement == _T("translationcredits")) {
        fError = TranslationcreditsTag(oNode, pResult);
    }
    else if (sElement == _T("tocimage")) {
        fError = TocimageTag(oNode, pResult);
    }
    else if (sElement == _T("ulink")) {
        fError = UlinkTag(oNode, pResult);
    }

    // exercises related param tags
    else if (m_fInScoreTag && is_score_param_tag(sElement))
    {
        fError = (m_format == k_format_html ? ExerciseParamTag(oNode, pResult)
                                            : ScoreParamTag(oNode, pResult) );
    }
    else if (!m_fInScoreTag && is_exercise_param_tag(sElement))
    {
        fError = ExerciseParamTag(oNode, pResult);
    }

    // special parameters for translation
    else if (sElement == _T("music")) {
        fError = ExerciseMusicTag(oNode, pResult);
    }

    //unrecognized tag
    else  {
        LogMessage(_T("*** Error: Found tag <%s>. No treatment defined."), sElement.c_str());
        fError = true;
    }

    return fError;
}

//---------------------------------------------------------------------------------------
wxString lmEbookProcessor::GetLibxml2Version()
{
    return wxXml2::GetLibxml2Version();
}


//------------------------------------------------------------------------------------
// Tags' processors
// Processing model:
//  - Each tag processor receives Options. These specify where the output must go.
//      Options can be superseded when so specified in the options themselves
//  - Only input inside a <theme> tags go to html output. So HTML output is enabled
//      in <theme> tag and disabled when closing it
//------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------
bool lmEbookProcessor::BookArticleTag(const wxXml2Node& oNode, const wxString& sTagName)
{
    // receives and processes a <book> node and its children.
    // return true if error

    m_fIsArticle = (sTagName == _T("article"));
    m_sStyle = oNode.GetPropVal(_T("style"), _T("lenmus-book"));
    m_fTextTagOpen = false;

    //images to pack
    if (m_fIsArticle)
    {
        //single page eBook
        m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("ebook_banner_left1.png"));
        m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("ebook_line_orange.png"));
        m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("leaflet_banner_right_") +
            m_sLangCode + _T(".png"));
    }
    else
    {
        //full eBook with cover page.
        if (m_sStyle == _T("lenmus-book"))
        {
            //LenMus style
            m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("ebook_banner_left1.png"));
            m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("ebook_banner_right2.png"));
            m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("ebook_line_orange.png"));
            m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("navbg7.jpg"));
            m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("emusicbook_title.png"));
        }
        else
        {
            //myMusicTheory style
            //m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("ebook_banner_left1.png"));
            //m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("ebook_banner_right2.png"));
            //m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("ebook_line_orange.png"));
            m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("myMusicTheory_126x65.jpg"));
            m_aFilesToPack.Add( g_pPaths->GetLayoutPath() + _T("emusicbook_title_small.png"));
        }
    }

    //get book id and add it to the pages table. This id will be used for the
    //book cover page
    m_sBookId = oNode.GetPropVal(_T("id"), _T(""));
    if (m_sBookId == _T("")) {
        wxLogMessage(_T("Node <book> has no id property"));
    }

    // reset titles numbering counters
    m_nTitleLevel = -1;
    for (int i=0; i < lmMAX_TITLE_LEVEL; i++)
        m_nNumTitle[i] = 0;

    m_sBookTitleAbbrev = wxEmptyString;
    m_sBookTitle = wxEmptyString;

    //process tag's children
    bool fError = GetTagContent(oNode, NULL);

    return fError;

}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::InfoTag(const wxXml2Node& oNode,
                               lmContentStorage* WXUNUSED(pResult))
{
    // receives and processes a <info> node and its children.
    // return true if error

    // convert tag: output to open output tag
    // -- no output implied by this tag --

    // tag processing implications
    m_nHeaderLevel = 0;
    m_fProcessingBookinfo = true;
    m_fTitleToToc = true;
    m_nParentType = lmPARENT_BOOKINFO;

    //process tag's children
    bool fError = GetTagContent(oNode, NULL);

    //end tag: processing implications
    m_fProcessingBookinfo = false;

    // create the cover page
    if (!m_fIsArticle)
        CreateBookCover();

    return fError;

}

// Normal, common tags

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ChapterTag(const wxXml2Node& oNode,
                                  lmContentStorage* pResult)
{
    // convert tag: output to open output tag
    wxString sId = oNode.GetPropVal(_T("id"), _T(""));
    if (sId != _T(""))
        WriteToToc(_T("<entry id=\"") + sId + _T("\">\n"));
    else
        WriteToToc(_T("<entry>\n"));
    m_nTocIndentLevel++;
    IncrementTitleCounters();

    // tag processing implications
    m_fTitleToToc = true;
    m_nParentType = lmPARENT_CHAPTER;
    m_sChapterTitleAbbrev = wxEmptyString;
    m_sChapterTitle = wxEmptyString;

    //process tag's children
    bool fError = GetTagContent(oNode, pResult);

    //convert tag: output to close html tags
    m_nTocIndentLevel--;
    WriteToToc(_T("</entry>\n"));

    DecrementTitleCounters();
    m_sChapterTitleAbbrev = wxEmptyString;
    m_sChapterTitle = wxEmptyString;

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ColTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // get attributes
    wxString sAlign = oNode.GetPropVal(_T("align"), _T("left"));
    wxString sValign = oNode.GetPropVal(_T("valign"), _T("middle"));
    wxString sWidth = oNode.GetPropVal(_T("width"), _T("2000"));
    static int numStyle = 0;

    //create style
    wxString styleName = wxString::Format(_T("\"table-col-%d\""), ++numStyle);
    m_sStyles += _T("   (defineStyle ");
    m_sStyles += styleName;
    m_sStyles += _T(" (table-col-width ");
    m_sStyles += sWidth;
    m_sStyles += _T(")) \n");

    //open output tag
    if (m_format == k_format_html)
    {
    }
    else
    {
        wxString out = _T("(tableColumn (style ");
        out += styleName;
        out += _T("))\n"),
        pResult->Add(out);
    }

    return false;   //no error
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::CreditsTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    //open output tag
    if (m_format == k_format_html)
    {
        pResult->Add(_T("<br /><br /><br /><br /><br /><b>"));
        pResult->Add(::wxGetTranslation(m_sReferences));
        pResult->Add(_T("</b><font size='-1'><br />\n<p>"));
        pResult->Add(::wxGetTranslation(m_sLessonsBased));
        pResult->Add(_T("</p>\n<ul>\n"));
    }
    else
    {
        pResult->Add( _T("(heading 2 (style \"eBook_heading_2\")(txt \""));
        pResult->Add(::wxGetTranslation(m_sReferences));
        pResult->Add(_T("\"))\n(para (style \"eBook_references_text\")(txt \""));
        pResult->Add(::wxGetTranslation(m_sLessonsBased));
        pResult->Add(_T("\"))\n(itemizedlist \n"));
    }

    //process tag's children
    bool fError = GetTagContent(oNode, pResult);

    //close html tags
    if (m_format == k_format_html)
        pResult->Add(_T("</font></ul>"));
    else
        pResult->Add(_T(")\n"));

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::CreditsitemTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // openning tag
    if (m_format == k_format_html)
        pResult->Add(_T("<li>"));
    else
        pResult->Add(_T("   (listitem (style \"eBook_references_text\")"));

    //process tag's children and deal with translation
    m_fTextTagOpen = false;
    bool fError = GetTagContent(oNode, pResult, true /* in text container tag */);

    //output to close tags
    if (m_format == k_format_html)
        pResult->Add(_T("</li>"));
    else
        pResult->Add( m_fTextTagOpen ? _T("\") )") : _T(" )") );
    pResult->AddNewLine();
    m_fTextTagOpen = false;

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::EmphasisTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // openning tag
    wxString sLink;
    //if (m_format == k_format_html)
        pResult->Add(_T(" <b>"));
    //else
    //    pResult->Add(_T(" (txt (style \"eBook_normal_emphasis\") \""));

    //process tag's children and write note content to html
    bool fError = GetTagContent(oNode, pResult);

    // closing tag
    //if (m_format == k_format_html)
        pResult->Add(_T("</b>"));
    //else
    //    pResult->Add(_T("\")"));

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ExerciseTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // get attributes
    wxString sType = oNode.GetPropVal(_T("type"), _T(""));
    wxString sWidth = oNode.GetPropVal(_T("width"), _T(""));
    wxString sHeight = oNode.GetPropVal(_T("height"), _T(""));
    wxString sBorder = oNode.GetPropVal(_T("border"), _T(""));

    //open output tag
    wxString sOpen;
    if (m_format == k_format_html)
    {
        sOpen = _T("<object type=\"Application/LenMus\" classid=\"") + sType +
            _T("\" width=\"") + sWidth +
            _T("\" height=\"") + sHeight +
            _T("\" border=\"") + sBorder +
            _T("\">\n");
    }
    else
    {
        sOpen = _T("(dynamic (classid ") + sType + _T(")\n");
        //all height parameters are in millimeters (no decimal point). Convert to LU
        if (!sHeight.empty())
        {
            long height = 0L;
            sHeight.ToLong(&height);
            //height *= 100L;
            sOpen += wxString::Format(_T("   (param height \"%d\")\n"), height);
        }
    }
    pResult->Add(sOpen);

    //process tag's children
    bool fError = GetTagContent(oNode, pResult);

    //close html tags
    if (m_format == k_format_html)
    {
        pResult->Add(_T("</object>\n"));
    }
    else
    {
        pResult->Add(_T(")\n"));
    }

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ExerciseParamTag(const wxXml2Node& oNode,
                                        lmContentStorage* pResult)
{
    // convert tag
    wxString sOpen = _T("<param name=\"") + oNode.GetName() + _T("\" value=\"");
    if (m_format == k_format_html)
        sOpen = _T("<param name=\"") + oNode.GetName() + _T("\" value=\"");
    else
    {
        wxString name = oNode.GetName();
        if (name == _T("music_border")
            || name == _T("score_type")
            //|| name == _T("control_play")
           )
        {
            return false;   //no error
        }
        sOpen = _T("   (param ") + oNode.GetName() + _T(" \"");
    }

    // complete link address
    if (oNode.GetName() == _T("control_go_back"))
    {
        wxFileName oFN(m_sFilename);
        sOpen += oFN.GetName() + _T("_");
    }
    pResult->Add(sOpen);

    //params have no more xml content, just the value. Output it
    bool fError = GetTagContent(oNode, pResult);

    // complete link address
    wxString sClose = _T("");
    if (m_format == k_format_html)
    {
        if (oNode.GetName() == _T("control_go_back")) {
            sClose += _T(".htm");
        }
        sClose += _T("\" />\n");
    }
    else
    {
        if (oNode.GetName() == _T("control_go_back")) {
            sClose += _T(".lms");
        }
        sClose += _T("\")\n");
    }
    pResult->Add(sClose);

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ExerciseMusicTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    if (m_format == k_format_html)
    {
        // convert tag
        pResult->Add(_T("<param name=\"") + oNode.GetName() + _T("\" value=\""));
    }

    //params have no more xml content, just the value. Get it
    lmContentStorage csMusic;
    bool fError = GetTagContent(oNode, &csMusic);

    //locate all translatable strings (start with underscore)
    wxString sNoTrans, sTrans;
    int iStart, iEnd, nQuoteLength;

    wxString sMusic = csMusic.GetContent();
    //iStart = sMusic.Find(_T("_&quot;"));
    //nQuoteLength = 7;   //length of string "_&quot;"
    iStart = sMusic.Find(_T("_\""));
    nQuoteLength = 2;   //length of string "_\""
    if (iStart == wxNOT_FOUND) {
        iStart = sMusic.Find(_T("_''"));
        nQuoteLength = 3;   //length of string "_''"
    }

    while (sMusic.Length() > 0 && iStart != wxNOT_FOUND)
    {
        if (iStart != wxNOT_FOUND) {
            //start of translatable string
            sNoTrans = sMusic.Left(iStart);
            pResult->Add( sNoTrans );

            sMusic = sMusic.Mid(iStart+nQuoteLength);

            // find end of string
            //iEnd = sMusic.Find(_T("&quot;"));
            //nQuoteLength = 6;   //length of string "&quot;"
            iEnd = sMusic.Find(_T("\""));
            nQuoteLength = 1;   //length of string "\""
            if (iEnd == wxNOT_FOUND) {
                iEnd = sMusic.Find(_T("''"));
                nQuoteLength = 2;       //length of string "''"
            }

            //end of translatable string
            sTrans = sMusic.Left(iEnd);
            //TODO: filter out strings containing only numbers, spaces and symbols but
            //      no characters
            if (m_fOnlyLangFile)
            {
                WriteToLang(sTrans);
                //pResult->Add(sTrans);
            }
            else
                sTrans = ::wxGetTranslation(sTrans);

            pResult->Add( _T("''") + sTrans + _T("''") );

            //remaining
            sMusic = sMusic.Mid(iEnd+nQuoteLength);
            iStart = sMusic.Find(_T("_&quot;"));
            nQuoteLength = 7;   //length of string "&quot;"
            if (iStart == wxNOT_FOUND) {
                iStart = sMusic.Find(_T("_''"));
                nQuoteLength = 3;   //length of string "''"
            }
        }
    }

    if (m_format == k_format_html)
        pResult->Add(sMusic + _T("\" />\n"));
    else
        pResult->Add(sMusic + _T("\n"));

    if (m_format == k_format_ldp)
    {
        //add scorePlayer with saved options
        if (m_sScorePlayer != _T(""))
            pResult->Add( m_sScorePlayer + _T(")\n") );
    }

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ImagedataTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    //<imagedata fileref='notesymbol_parts.png' />

    // get attributes
    wxString sFileref = oNode.GetPropVal(_T("fileref"), _T(""));
    wxString sAlign = oNode.GetPropVal(_T("align"), _T(""));
    wxString sValign = oNode.GetPropVal(_T("valign"), _T(""));
    wxString sTranslate = oNode.GetPropVal(_T("translate"), _T(""));
    bool fTranslate = (sTranslate == _T("yes"));
    if (sFileref == _T("")) {
        LogError(_T("Node <image> has no 'fileref' property"));
        return true;    //error
    }

    // convert tag
    //if (m_format == k_format_html)
    //{
        //output as html tag
        wxString sOut = _T("<img src=\"") + sFileref + _T("\"");
        if (sAlign != _T(""))
            sOut += _T(" align=\"") + sAlign + _T("\"");
        if (sValign != _T(""))
            sOut += _T(" valign=\"") + sValign + _T("\"");
        sOut += _T(" />");
        pResult->Add(sOut);
//    }
//    else
//    {
//        //output as LDP code
//        wxString style = _T("eBook_img");
//        if (sAlign != _T(""))
//            style += _T("_") + sAlign;
//        if (sValign != _T(""))
//            style += _T("_") + sValign;
////        if (!style_exists(style))
////            create_img_style(style, sAlign, sValign);
//        //wxString sOut = _T("(image (style \"") + style +
//        //                _T("\")(file \"") + sFileref + _T("\"))");
//        wxString sOut = _T("<image (style \"") + style +
//                        _T("\")(file \"") + sFileref + _T("\") />");
//        pResult->Add(sOut);
//    }

    //add to list of files to pack in lmb file
    wxFileName oFN(m_sFilename);
    oFN.AppendDir(_T("figures"));
    if (fTranslate)
        oFN.AppendDir(m_sLangCode);
    oFN.SetFullName( sFileref );
    m_aFilesToPack.Add(oFN.GetFullPath());

    return false;   //no error
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::LinkTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // openning tag

    // get its 'linkend' property and find the associated page
    wxFileName oFN( m_sFilename );
    wxString sId = oNode.GetPropVal(_T("linkend"), _T(""));
    if (sId != _T(""))
    {
        wxString sName = oFN.GetName() + _T("_") + sId;
        oFN.SetName(sName);
        oFN.SetExt( m_format == k_format_html ? _T("htm") : _T("lms") );
    }
    //if (m_format == k_format_html)
    //{
        if (sId != _T(""))
            pResult->Add(_T(" <a href=\"#LenMusPage/") + oFN.GetFullName() + _T("\">"));
        else
            pResult->Add(_T(" <a href=\"#\">"));
    //}
    //else
    //{
    //    if (sId != _T(""))
    //        pResult->Add(_T(" (link (style \"eBook_normal_link\")(url \"#LenMusPage/") + oFN.GetFullName() +
    //                     _T("\")(txt \""));
    //    else
    //        pResult->Add(_T(" (link (style \"eBook_normal_link\")(url \"#\")(txt \""));
    //}

    //process tag's children and write note content to html
    bool fError = GetTagContent(oNode, pResult);

    // closing tag
    //if (m_format == k_format_html)
        pResult->Add(_T("</a>"));
    //else
    //    pResult->Add(_T("\"))"));

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ListitemTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // openning tag
    //if (m_format == k_format_html)
        pResult->Add(_T("<li>"));
    //else
    //    pResult->Add(_T("   (listitem (style \"eBook_listitem\")"));

    //process tag's children and deal with translation
    m_fTextTagOpen = false;
    bool fError = GetTagContent(oNode, pResult, true /* in text container tag */);

    //output to close tags
    //if (m_format == k_format_html)
        pResult->Add(_T("</li>"));
    //else
    //    pResult->Add( m_fTextTagOpen ? _T("\") )") : _T(" )") );
    pResult->AddNewLine();
    m_fTextTagOpen = false;

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ParaTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // get attributes
    wxString sRole = oNode.GetPropVal(_T("role"), _T(""));

    // openning tag
    if (m_format == k_format_html)
    {
        if (sRole == _T("center"))
            pResult->Add(_T("<center>"));
        pResult->Add(_T("<p>"));
    }
    else
    {
        pResult->Add(_T("(para (style \"eBook_para\")"));
    }

    //process tag's children and deal with translation
    m_fTextTagOpen = false;
    bool fError = GetTagContent(oNode, pResult, true /* in text container tag */);

    //output to close html tags
    if (m_format == k_format_html)
    {
        pResult->Add(_T("</p>"));
        if (sRole == _T("center"))
            pResult->Add(_T("</center>"));
        pResult->AddNewLine();
    }
    else
    {
        pResult->Add( m_fTextTagOpen ? _T("\") )") : _T(" )") );
        pResult->AddNewLine();
    }
    m_fTextTagOpen = false;

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::PartTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // openning tag
    if (m_fThemeInToc) IncrementTitleCounters();
    // HTML:
    wxString sId = oNode.GetPropVal(_T("id"), _T(""));
    if (m_format == k_format_html)
    {
        if (sId != _T(""))
            pResult->Add(_T("<div id=\"") + sId + _T("\">\n"));
        else
            pResult->Add(_T("<div>\n"));
    }

    // TOC:
    // no toc output

    // tag processing implications
    m_fTitleToToc = false;
    m_nParentType = lmPARENT_PART;
    m_nHeaderLevel++;

    //process tag's children
    bool fError = GetTagContent(oNode, pResult);

    //closing tag:
    // HTML:
    if (m_format == k_format_html)
        pResult->Add(_T("</div>\n"));

    // TOC:
    // no toc content
    // processing implications:
    m_nHeaderLevel--;

    if (m_fThemeInToc) DecrementTitleCounters();
    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ScoreParamTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    //this method is only invoked in k_format_ldp mode

    wxString name = oNode.GetName();
    bool fError = false;

    if (name == _T("control_play"))
    {
        //fix ordering: <metronome> before <control_play>
        if (m_sScorePlayer == _T(""))
        {
            m_sScorePlayer = _T("(scorePlayer (playLabel \"");
            m_sScorePlayer += ::wxGetTranslation(_T("Play"));
            m_sScorePlayer += _T("\")(stopLabel \"");
            m_sScorePlayer += ::wxGetTranslation(_T("Stop playing"));
            m_sScorePlayer += _T("\")");
        }
    }
    else if (name == _T("metronome"))
    {
        //fix ordering: <metronome> before <control_play>
        if (m_sScorePlayer == _T(""))
            m_sScorePlayer = _T("(scorePlayer ");

        //metronome param has no more xml content, just the value. Get it
        wxString mm;
        fError = ProcessVariableAssignTag(oNode, &mm);
        if (!fError)
            m_sScorePlayer += _T("(mm ") + mm + _T(")");
    }

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ScoreTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    m_fInScoreTag = true;

    // get attributes
    wxString sWidth = oNode.GetPropVal(_T("width"), _T(""));
    wxString sHeight = oNode.GetPropVal(_T("height"), _T(""));
    wxString sBorder = oNode.GetPropVal(_T("border"), _T(""));

    // convert tag: output to open output tag
    if (m_format == k_format_html)
    {
        pResult->Add(_T("<object type=\"Application/LenMus\" classid=\"Score")
            _T("\" width=\"") + sWidth +
            _T("\" height=\"") + sHeight +
            _T("\" border=\"") + sBorder +
            _T("\">\n"));
    }
    else
    {
        m_sScorePlayer = _T("");
    }

    //process tag's children
    bool fError = GetTagContent(oNode, pResult);

    //convert tag: output to close html tags
    if (m_format == k_format_html)
        pResult->Add(_T("</object>\n"));

    m_fInScoreTag = false;
    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::SectionTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // openning tag
    wxString sId = oNode.GetPropVal(_T("id"), _T(""));
    if (sId != _T(""))
        WriteToToc(_T("<entry id=\"") + sId + _T("\">\n"));
    else
        WriteToToc(_T("<entry>\n"));
    m_nTocIndentLevel++;
    IncrementTitleCounters();

    // tag processing implications
    m_fTitleToToc = true;
    m_nParentType = lmPARENT_SECTION;
    m_sChapterTitleAbbrev = wxEmptyString;
    m_sChapterTitle = wxEmptyString;

    //process tag's children
    bool fError = GetTagContent(oNode, pResult);

    //convert tag: output to close tags
    m_nTocIndentLevel--;
    WriteToToc(_T("</entry>\n"));

    DecrementTitleCounters();

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::StyleTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // get name
    wxString styleName = oNode.GetPropVal(_T("name"), _T(""));

    //process tag's children
    lmContentStorage csStyle;
    bool fError = GetTagContent(oNode, &csStyle);

    if (fError || styleName == _T(""))
        return true;

    //create style
    wxString style = _T("   (defineStyle \"");
    style += styleName;
    style += _T("\" ");
    style += csStyle.GetContent();
    style += _T(")\n");
    m_sStyles += style;

    return false;   //no error
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::TableTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // get attributes
    wxString sRole = oNode.GetPropVal(_T("role"), _T(""));

    // openning tag
    if (m_format == k_format_html)
    {
        if (sRole == _T("center"))
            pResult->Add(_T("<center>"));
        pResult->Add(_T("<table border='4'>"));
    }
    else
    {
        pResult->Add(_T("(table (style \"eBook_table\")\n"));
    }

    // tag processing implications

    //process tag's children and write note content to html
    bool fError = GetTagContent(oNode, pResult);

    // closing tag
    if (m_format == k_format_html)
    {
        pResult->Add(_T("</table>"));
        if (sRole == _T("center"))
            pResult->Add(_T("</center>"));
    }
    else
    {
        pResult->Add(_T(")"));
    }
    pResult->AddNewLine();

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::TbodyTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // openning tag
    if (m_format == k_format_html)
        pResult->Add(_T("<tbody>\n"));
    else
        pResult->Add(_T("(tableBody \n"));

    // tag processing implications

    //process tag's children and write note content to html
    m_fTextTagOpen = false;
    bool fError = GetTagContent(oNode, pResult);    //, true /* in text container tag */);

    // closing tag
    if (m_format == k_format_html)
        pResult->Add(_T("</tbody>\n"));
    else
        pResult->Add( _T(" )\n") );
    m_fTextTagOpen = false;

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::TdTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // get attributes
    wxString sAlign = oNode.GetPropVal(_T("align"), _T("left"));
    wxString sValign = oNode.GetPropVal(_T("valign"), _T("middle"));

    // openning tag
    if (m_format == k_format_html)
    {
        wxString sOpen = _T("<td align='");
        sOpen += sAlign;
        sOpen += _T("' valign='");
        sOpen += sValign;
        sOpen += _T("'>");
        pResult->Add(sOpen);
    }
    else
    {
        pResult->Add(_T("(tableCell (style \"eBook_table_cell\")\n"));
    }

    // tag processing implications

    //process tag's children and write note content to html
    m_fTextTagOpen = false;
    bool fError = GetTagContent(oNode, pResult, true /* in text container tag */);

    // closing tag
    if (m_format == k_format_html)
        pResult->Add(_T("</td>\n"));
    else
        pResult->Add( m_fTextTagOpen ? _T("\") )\n") : _T(" )\n") );
    m_fTextTagOpen = false;

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::TrTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // openning tag
    if (m_format == k_format_html)
        pResult->Add(_T("<tr>\n"));
    else
        pResult->Add(_T("(tableRow \n"));

    // tag processing implications

    //process tag's children and write note content to html
    bool fError = GetTagContent(oNode, pResult);

    // closing tag
    if (m_format == k_format_html)
        pResult->Add(_T("</tr>\n"));
    else
        pResult->Add(_T(")\n"));

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::ThemeTag(const wxXml2Node& oNode, lmContentStorage* WXUNUSED(pResult))
{
    // get its 'id' and 'header' properties
    wxString sId = oNode.GetPropVal(_T("id"), _T(""));
    wxString sHeader = oNode.GetPropVal(_T("header"), _T(""));
    wxString sToToc = oNode.GetPropVal(_T("toc"), _T("yes"));
    m_fThemeInToc = (m_fIsArticle ? false : (sToToc == _T("yes")) );

    // openning tag
    m_sThemeTitleAbbrev = wxEmptyString;
    if (m_fThemeInToc) IncrementTitleCounters();
    // HTML:
    wxFileName oHTM( m_sFilename );
    wxString sName = oHTM.GetName() + _T("_") + sId;
    oHTM.SetName(sName);
    oHTM.SetExt( m_format == k_format_html ? _T("htm") : _T("lms") );
    m_sHtmlPagename = oHTM.GetFullName();

    lmContentStorage csHtmlContent;
    // TOC
    if (m_fIsArticle)
        WriteToToc(_T("<coverpage>") + m_sHtmlPagename + _T("</coverpage>\n"));
    else
    {
        if (m_fThemeInToc)
        {
            if (sId != _T(""))
                WriteToToc(_T("<entry id=\"") + sId + _T("\">\n"));
            else
                WriteToToc(_T("<entry>\n"));
            m_nTocIndentLevel++;
        }
    }

    // tag processing implications
    m_nHeaderLevel = 1;
    m_fTitleToToc = !m_fIsArticle;
    m_nParentType = lmPARENT_THEME;
    m_sStyles = _T("");

    //process tag's children
    bool fError = GetTagContent(oNode, &csHtmlContent);

    //closing tag:
    // HTML:
    //terminate_content_file();    // Close previous html page
    create_content_file(m_sHtmlPagename, &csHtmlContent);

    // TOC
    if (m_fThemeInToc)
    {
        //WriteToToc(_T("<page>") + m_sHtmlPagename + _T("</page>\n"));
        m_nTocIndentLevel--;
        WriteToToc(_T("</entry>\n"));
        DecrementTitleCounters();
    }


    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::TitleTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // openning tag
    wxString sTitleNum = wxEmptyString;
    bool fTitleToToc = ((m_nParentType == lmPARENT_BOOKINFO) && m_fTitleToToc) ||
                       ((m_nParentType == lmPARENT_CHAPTER) && m_fTitleToToc) ||
                       ((m_nParentType == lmPARENT_SECTION) && m_fTitleToToc) ||
                       ((m_nParentType == lmPARENT_THEME) && m_fThemeInToc);

    if (fTitleToToc)
    {
        sTitleNum = GetTitleCounters();
        if (m_nParentType != lmPARENT_BOOKINFO)
        {
            WriteToToc(_T("<page>") + m_sHtmlPagename + _T("</page>\n"));
            WriteToToc(_T("<titlenum>") + sTitleNum + _T("</titlenum>\n"));
        }
        WriteToToc(_T("<title>"));
    }

    //process tag's children and write title content to toc
    lmContentStorage csTitle;
    bool fError = GetTagContent(oNode, &csTitle);
    wxString sTitle = csTitle.GetContent();

    //save the title
    if (m_nParentType == lmPARENT_BOOKINFO) {
        m_sBookTitle = sTitle;
    }
    else if (m_nParentType == lmPARENT_CHAPTER) {
        m_sChapterTitle = sTitle;
        m_sChapterNum = sTitleNum;
    }
    else if (m_nParentType == lmPARENT_THEME)
    {
        //determine which title to use for page headers
        if (m_sChapterTitleAbbrev != wxEmptyString)
            m_sHeaderTitle = m_sChapterTitleAbbrev;
        else if (m_sChapterTitle != wxEmptyString)
            m_sHeaderTitle = m_sChapterTitle;
        else if (m_sThemeTitleAbbrev != wxEmptyString)
            m_sHeaderTitle = m_sThemeTitleAbbrev;
        else
            m_sHeaderTitle = sTitle;

        // replace %d by parent number or preceed header by parent number
        m_sHeaderParentNum = GetParentNumber();
        if (m_sHeaderTitle.Find(_T("%d")) != wxNOT_FOUND)
            m_sHeaderTitle.Replace(_T("%d"), m_sHeaderParentNum);
        else
            m_sHeaderTitle = m_sHeaderParentNum + _T(". ") + m_sHeaderTitle;

        if (m_sHeaderParentNum.IsEmpty())
            m_sHeaderParentNum = sTitleNum;
    }

    //send titles to output
    if (m_nParentType == lmPARENT_THEME || m_nParentType == lmPARENT_PART)
    {
        if (m_format == k_format_html)
        {
            if (m_nHeaderLevel > 0)
            {
                if (m_sStyle == _T("lenmus-book"))
                {
                    pResult->Add( wxString::Format(_T("<br /><br /><h%d>"), m_nHeaderLevel));
                    pResult->Add( sTitleNum );
                    pResult->Add( sTitle );
                    pResult->Add( wxString::Format(_T("</h%d>\n"), m_nHeaderLevel));
                }
                else
                {
                    pResult->Add( wxString::Format(_T("<br /><br /><h%d><font color='#be041a'>"), m_nHeaderLevel));
                    pResult->Add( sTitleNum );
                    pResult->Add( sTitle );
                    pResult->Add( wxString::Format(_T("</font></h%d>\n"), m_nHeaderLevel));
                }
            }
        }
        else
        {
            if (m_nHeaderLevel > 0)
            {
                pResult->Add( wxString::Format(_T("(heading %d (style \"eBook_heading_%d\")(txt \""),
                                    m_nHeaderLevel, m_nHeaderLevel));
                pResult->Add( sTitleNum );
                pResult->Add( sTitle );
                pResult->Add( _T("\"))\n") );
            }
        }
    }

    //TOC:
    if (fTitleToToc)
        WriteToToc(sTitle + _T("</title>\n"), ltNO_INDENT );

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::TitleabbrevTag(const wxXml2Node& oNode,
                                      lmContentStorage* WXUNUSED(pResult))
{
    //process tag's children. Do not write content
    lmContentStorage csTitle;
    bool fError = GetTagContent(oNode, &csTitle);
    wxString sTitle = csTitle.GetContent();

    //save the title
    if (m_nParentType == lmPARENT_BOOKINFO) {
        m_sBookTitleAbbrev = sTitle;
    }
    else if (m_nParentType == lmPARENT_CHAPTER) {
        m_sChapterTitleAbbrev = sTitle;
    }
    else if (m_nParentType == lmPARENT_THEME) {
        m_sThemeTitleAbbrev = sTitle;
    }
    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::TocimageTag(const wxXml2Node& oNode,
                                   lmContentStorage* pResult)
{
    //process tag's children. Do not write content
    lmContentStorage csImageName;
    bool fError = GetTagContent(oNode, &csImageName);
    wxString sImageName = csImageName.GetContent();
    //pResult->Add(sImageName);

    //add to list of files to pack in lmb file
    m_aFilesToPack.Add(sImageName);

    // End of tag processing implications
    //TOC:
    wxFileName oFN(sImageName);
    WriteToToc(_T("<image>") + oFN.GetFullName() + _T("</image>\n"));

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::UlinkTag(const wxXml2Node& oNode, lmContentStorage* pResult)
{
    // openning tag

    // get its 'url' property and find the associated page
    wxString sUrl = oNode.GetPropVal(_T("url"), _T(""));
    wxString sLink;
    //if (m_format == k_format_html)
    //{
        if (sUrl != _T(""))
            sLink = _T(" <a href=\"") + sUrl + _T("\">");
        else
            sLink = _T(" <a href=\"#\">");
    //}
    //else
    //{
    //    if (sUrl != _T(""))
    //        pResult->Add(_T(" (link (style \"eBook_normal_link\")(url \"") + sUrl +
    //                     _T("\")(txt \"") );
    //    else
    //        pResult->Add(_T(" (link (style \"eBook_normal_link\")(url \"#\")(txt \""));
    //}

    pResult->Add(sLink);

    //process tag's children and write note content to html
    bool fError = GetTagContent(oNode, pResult);

    // closing tag
    //if (m_format == k_format_html)
        pResult->Add(_T("</a>"));
    //else
    //    pResult->Add(_T("\"))"));

    return fError;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::TranslationcreditsTag(const wxXml2Node& WXUNUSED(oNode),
                                             lmContentStorage* pResult)
{
    if (m_sLangCode != _T("en"))
    {
        if (m_format == k_format_html)
        {
            pResult->Add(_T("<li>"));
            pResult->Add( ::wxGetTranslation(m_sTranslators) );
            pResult->Add(_T("</li>"));
        }
        else
        {
            pResult->Add(_T("   (listitem (style \"eBook_listitem\")"));
            pResult->Add(_T("(txt (style \"eBook_normal\") \""));
            pResult->Add( ::wxGetTranslation(m_sTranslators) );
            pResult->Add(_T("\") )"));
        }
        pResult->AddNewLine();
    }

    return false;
}



//
// Auxiliary
//

//---------------------------------------------------------------------------------------
wxString lmEbookProcessor::GetTitleCounters()
{
    // Returns current theme number. For example, if current
    // theme is '2.4.7' this method returns '2.4.7. '

    wxString sTitleNum = wxEmptyString;
    for (int i=0; i <= m_nTitleLevel; i++) {
        sTitleNum += wxString::Format(_T("%d."), m_nNumTitle[i] );
    }
    if (sTitleNum != wxEmptyString) sTitleNum += _T(" ");

    return  sTitleNum;

}

//---------------------------------------------------------------------------------------
wxString lmEbookProcessor::GetParentNumber()
{
    // Returns current theme number minus one level. For example, if current
    // theme is '2.4.7' this method returns '2.4'

    wxString sTitleNum = wxEmptyString;
    if (m_nTitleLevel >= 0) {
        sTitleNum = wxString::Format(_T("%d"), m_nNumTitle[0] );
        for (int i=1; i < m_nTitleLevel; i++) {
            sTitleNum += wxString::Format(_T(".%d"), m_nNumTitle[i] );
        }
    }
    else {
        // title level is < 0. This is because it is a theme not in toc.
        // lets return previous theme number
        sTitleNum = wxString::Format(_T("%d"), m_nNumTitle[0] );
    }

    return  sTitleNum;

}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::IncrementTitleCounters()
{
    m_nTitleLevel++;
    if ( m_nTitleLevel >= lmMAX_TITLE_LEVEL) {
        wxLogMessage(_T("Too many nested levels for sections/themes/parts"));
        m_nTitleLevel--;
    }
    m_nNumTitle[m_nTitleLevel]++;

    for (int i=m_nTitleLevel+1; i < lmMAX_TITLE_LEVEL; i++) {
        m_nNumTitle[i] = 0;
    }
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::DecrementTitleCounters()
{
    m_nTitleLevel--;
    if (m_nTitleLevel < 0) return;

}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::CreateBookCover()
{
    wxFileName outfile( m_sFilename );
    wxString sName = outfile.GetName() + _T("_cover");
    outfile.SetName(sName);
    outfile.SetExt( m_format == k_format_html ? _T("htm") : _T("lms") );
    start_content_file(outfile.GetFullName());

    //Generate header
    wxString sNil = _T("");
    wxString sOut = sNil;
    if (m_format == k_format_html)
    {
        sOut +=
        _T("<html>\n<head>\n")
        _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") + m_sCharCode +
        _T("\">\n")
        _T("<title>") + m_sBookTitle + _T("</title>\n")
        _T("</head>\n\n");

        write_to_content_file(sOut);
    }
    else
    {
        write_ldp_headers();
    }


    //initializations
    m_nOutIndentLevel = 1;

    if (m_format == k_generate_html)
        add_html_layout();
    else
        add_ldp_layout();

    close_content_file();

    //write it to toc
    WriteToToc(_T("<coverpage>") + outfile.GetFullName() + _T("</coverpage>\n"));
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::add_html_layout()
{
    if (m_sStyle == _T("lenmus-book"))
    {
        //LenMus eBook style
        write_to_content_file(
            _T("<body bgcolor='#808080'>\n")
            _T("<table width='720' bgcolor='#ffffff' cellpadding='0' cellspacing='0'>\n")
            _T("<tr><td bgcolor='#ffffff'>\n")
            _T("\n")
            _T("<!-- banner -->\n")
            _T("<table width='100%' cellpadding='0' cellspacing='0'>\n")
            _T("<tr><td bgcolor='#7f8adc' width='23%' valign='top'><img src='navbg7.jpg'></td>\n")
	        _T("    <td bgcolor='#ffffff' width='6%'>&nbsp;</td>\n")
            _T("<td valign='top' bgcolor='#ffffff'>\n")
            _T("\n")
            _T("<!-- content -->\n")
            _T("<table width='100%' cellpadding='0' cellspacing='0' border='0'>\n")
            _T("<tr><td colspan='2' valign='top' align='right'><img src='emusicbook_title.png'></td></tr>\n")
            _T("<tr><td>\n")
	        _T("    <br /><br /><br /><br />\n")
	        _T("    <br /><br /><br /><br />\n")
            _T("</td><td width='30'>&nbsp;</td></tr>\n")
            _T("<tr><td><h1>") + m_sBookTitle + _T("</h1></td><td>&nbsp;</td></tr>\n")
            _T("<tr><td><h3>") + m_sAuthorName + _T("</h3></td><td>&nbsp;</td></tr>\n")
            _T("<tr><td>\n") + m_sBookAbstract +
	            _T("</td><td>&nbsp;</td></tr>\n")
            _T("\n")
            _T("<tr><td>&nbsp;</td><td>&nbsp;</td></tr>\n")
            _T("<tr><td>\n")
            _T("    <font size='-1' face='Arial'>\n")
	        _T("    <p><b>Copyright © ") + m_sCopyrightYear + _T(" ") + m_sCopyrightHolder +
                _T("</b></p>\n")
            _T("\n<p>") + m_sLegalNotice +
            _T("</p></font>\n")
            _T("</td><td>&nbsp;</td></tr>\n")
            _T("</table>\n")
            _T("</td></tr>\n")
            _T("</table>\n")
            _T("</td></tr></table>\n")
            _T("</body>\n")
            _T("</html>\n") );
    }
    else if (m_sStyle == _T("myMusicTheory"))
    {
        write_to_content_file(
            _T("<body bgcolor='#808080'>\n")
            _T("<table width='720' bgcolor='#ffffff' cellpadding='0' cellspacing='0'>\n")
            _T("<tr><td bgcolor='#ffffff'>\n")
            _T("\n")
            _T("<!-- banner -->\n")
            _T("<table width='100%' cellpadding='0' cellspacing='0'>\n")
            _T("<tr><td bgcolor='#eeeeff' width='23%' valign='top'><img src='emusicbook_title_small.png'></td>\n")
	        _T("    <td bgcolor='#ffffff' width='6%'>&nbsp;</td>\n")
            _T("<td valign='top' bgcolor='#ffffff'>\n")
            _T("\n")
            _T("<!-- content -->\n")
            _T("<table width='100%' cellpadding='0' cellspacing='0' border='0'>\n")
            _T("<tr><td colspan='2' valign='top' align='right'>&nbsp;</td></tr>\n")
            _T("<tr><td colspan='2' valign='top' align='right'><img src='myMusicTheory_126x65.jpg'>&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>\n")
            _T("<tr><td>\n")
	        _T("    <br /><br /><br /><br />\n")
	        _T("    <br /><br /><br /><br />\n")
            _T("</td><td width='30'>&nbsp;</td></tr>\n")
            _T("<tr><td><h1><font color='#be041a'>") + m_sBookTitle + _T("</font></h1></td><td>&nbsp;</td></tr>\n")
            _T("<tr><td><h3>") + m_sAuthorName + _T("</h3></td><td>&nbsp;</td></tr>\n")
            _T("<tr><td>\n") + m_sBookAbstract +
	            _T("</td><td>&nbsp;</td></tr>\n")
            _T("\n")
            _T("<tr><td>&nbsp;</td><td>&nbsp;</td></tr>\n")
            _T("<tr><td>\n")
            _T("    <font size='-1' face='Arial'>\n")
	        _T("    <p><b>Copyright © ") + m_sCopyrightYear + _T(" ") + m_sCopyrightHolder +
                _T("</b></p>\n")
            _T("\n<p>") + m_sLegalNotice +
            _T("</p></font>\n")
            _T("<p>&nbsp;</p><p>&nbsp;</p><p>&nbsp;</p><p>&nbsp;</p><p>&nbsp;</p>")
            _T("</td><td>&nbsp;</td></tr>\n")
            _T("</table>\n")
            _T("</td></tr>\n")
            _T("</table>\n")
            _T("</td></tr></table>\n")
            _T("</body>\n")
            _T("</html>\n") );
    }
    else
        LogMessage(_T("Invalid style '%s'. Html headers not created!"), m_sStyle.c_str());

}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::add_ldp_layout()
{
    write_to_content_file(
        _T("(heading 1 (style \"eBook_heading_1\")(txt \"") + m_sBookTitle + _T("\"))\n")
        _T("(heading 3 (style \"eBook_heading_3\")(txt \"") + m_sAuthorName + _T("\"))\n")
        _T("(para (style \"eBook_para\")(txt \"") + m_sBookAbstract + _T("\"))\n")
        _T("\n")
	    _T("(para (style \"eBook_copyright\") (txt \"Copyright © ") + m_sCopyrightYear +
            _T(" ") + m_sCopyrightHolder +
            _T("\"))\n")
        + m_sLegalNotice + _T("))\n")
    );
}

//---------------------------------------------------------------------------------------
// File managing
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Toc file
//---------------------------------------------------------------------------------------

bool lmEbookProcessor::StartTocFile(wxString sFilename)
{
    // returns false if error

    wxFileName oTOC( sFilename );
    oTOC.SetExt(_T("toc"));
    m_sTocFilename = oTOC.GetFullName();
    m_pTocFile = new wxFile(m_sTocFilename, wxFile::write);
    if (!m_pTocFile->IsOpened()) {
        LogMessage(_T("*** Error: File %s can not be created"), oTOC.GetFullName().c_str());
        return false;        //error
    }

    //initializations
    m_nTocIndentLevel = 0;

    //Generate header
    wxString sNil = _T("");
    wxString sHeader = sNil +
        _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
        _T("<lmBookTOC>\n");

    WriteToToc(sHeader);
    m_nTocIndentLevel++;

    return true;

}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::TerminateTocFile()
{
    if (!m_pTocFile) return;

    WriteToToc(_T("</lmBookTOC>\n"), false);
    m_pTocFile->Close();

    delete m_pTocFile;
    m_pTocFile = (wxFile*) NULL;
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::WriteToToc(wxString sText, bool fIndent)
{
    if (!m_pTocFile) return;

    if (fIndent) {
        wxString sIndent;
        sIndent.Append(_T(' '), 3 * m_nTocIndentLevel);
        m_pTocFile->Write(sIndent + sText);
        //wxLogMessage(sIndent + sText);
    }
    else {
        m_pTocFile->Write(sText);
        //wxLogMessage(sText);
    }
}

//---------------------------------------------------------------------------------------
// Html file managing
//---------------------------------------------------------------------------------------

bool lmEbookProcessor::create_content_file(wxString sFilename, lmContentStorage* pContent)
{
    //returns true if created ok

    bool fOK = start_content_file(sFilename);
    if (!fOK) return fOK;

    //create page headers
    if (m_fIsArticle)
        CreateArticleHeaders(m_sBookTitle, m_sHeaderTitle, m_sHeaderParentNum);
    else
        CreatePageHeaders(m_sBookTitle, m_sHeaderTitle, m_sHeaderParentNum);

    if (!pContent->IsEmpty())
        write_to_content_file(pContent->GetContent());

    //terminate html file
    if (m_fIsArticle)
        TerminateArticleFile();
    else
        terminate_content_file();

    return true;
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::start_content_file(const wxString& sFilename)
{
    //returns true if created ok

//    wxLogMessage(_T("start_content_file '%s'"), sFilename.c_str());
    if (m_fGenerateLmb)
        m_pZipFile->PutNextEntry( sFilename );
    else
    {
        m_pHtmlFile = new wxFile(sFilename, wxFile::write);
        if (!m_pHtmlFile->IsOpened()) {
            LogMessage(_T("*** Error: File %s can not be created"), sFilename.c_str());
            return false;        //error
        }
    }

    return true;
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::terminate_content_file()
{
    if (!((m_fGenerateLmb && m_pLmbFile) || m_pHtmlFile)) return;

    if (m_format == k_format_html)
        terminate_html_layout();
    else
        terminate_ldp_layout();

    close_content_file();
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::terminate_html_layout()
{
    if (m_sStyle == _T("lenmus-book"))
    {
        //LenMus eBook style
        write_to_content_file(
            _T("</td><td bgcolor='#ffffff' width='20'></td></tr></table>\n")
            _T("\n")
            _T("<br /><br /><br /><br />\n")
            _T("<br /><br /><br /><br />\n")
            _T("<br /><br /><br /><br />\n")
            _T("<br /><br /><br /><br />\n")
            _T("<br /><br /><br /><br />\n")
            _T("<table width='100%' cellpadding='0' cellspacing='0'>\n")
            _T("<tr><td bgcolor='#ff8800'><img src='ebook_line_orange.png'></td></tr>\n")
            _T("<tr><td bgcolor='#7f8adc' align='center'>\n")
            _T("    <font size='-1' color='#ffffff'><br /><br />\n"));
        write_to_content_file( ::wxGetTranslation(m_sFooter1) );
        write_to_content_file( _T("<br />\n") );
        write_to_content_file( ::wxGetTranslation(m_sFooter2) );
        if (m_sFooter3 != _T(""))
        {
            write_to_content_file( _T("<br />\n") );
            write_to_content_file( ::wxGetTranslation(m_sFooter3) );
        }
        write_to_content_file( _T("<br />\n")
	        _T("    </font>\n")
            _T("</td></tr>\n")
            _T("</table>\n")
            _T("\n")
            _T("</td></tr></table></center>\n")
            _T("\n")
            _T("</body>\n")
            _T("</html>\n") );
    }
    else if (m_sStyle == _T("myMusicTheory"))
    {
        //myMusictTheory style
        write_to_content_file(
            _T("</td><td bgcolor='#ffffff' width='20'></td></tr></table>\n")
            _T("\n")
            _T("<br /><br /><br /><br />\n")
            _T("<br /><br /><br /><br />\n")
            _T("<br /><br /><br /><br />\n")
            _T("<br /><br /><br /><br />\n")
            _T("<br /><br /><br /><br />\n")
            _T("<table width='100%' cellpadding='0' cellspacing='0'>\n")
            //_T("<tr><td bgcolor='#ff8800'><img src='ebook_line_orange.png'></td></tr>\n")
            _T("<tr><td bgcolor='#f9e7e7' align='center'>\n")
            _T("    <font size='-1' color='#000000'><br /><br />\n"));
        write_to_content_file( ::wxGetTranslation(m_sFooter1) );
        write_to_content_file( _T("<br />\n") );
        write_to_content_file( ::wxGetTranslation(m_sMMT_Footer2) );
        write_to_content_file( _T("<br />\n")
	        _T("    </font>\n")
            _T("</td></tr>\n")
            _T("</table>\n")
            _T("\n")
            _T("</td></tr></table></center>\n")
            _T("\n")
            _T("</body>\n")
            _T("</html>\n") );
    }
    else
        LogMessage(_T("Invalid style '%s'. Html headers not created!"), m_sStyle.c_str());
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::terminate_ldp_layout()
{
    write_to_content_file(
        _T("\n))\n")
    );
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::TerminateArticleFile()
{
    if (!((m_fGenerateLmb && m_pLmbFile) || m_pHtmlFile)) return;

    // Write footer
    write_to_content_file(
        _T("</td><td bgcolor='#ffffff' width='10'></td></tr></table>\n")
        _T("\n")
        _T("<br /><br /><br /><br />\n")
        _T("<br /><br /><br /><br />\n")
        _T("<br /><br /><br /><br />\n")
        _T("<br /><br /><br /><br />\n")
        _T("<br /><br /><br /><br />\n")
        _T("<table width='100%' cellpadding='0' cellspacing='0'>\n")
        _T("<tr><td bgcolor='#ff8800'><img src='ebook_line_orange.png'></td></tr>\n")
        _T("<tr><td bgcolor='#7f8adc' align='center'>\n")
        _T("    <font size='-1' color='#ffffff'><br /><br />\n"));
    write_to_content_file( ::wxGetTranslation(m_sFooter1) );
    write_to_content_file( _T("<br />\n") );
    write_to_content_file( ::wxGetTranslation(m_sFooter2) );
    if (m_sFooter3 != _T(""))
    {
        write_to_content_file( _T("<br />\n") );
        write_to_content_file( ::wxGetTranslation(m_sFooter3) );
    }
    write_to_content_file( _T("<br />\n")
	    _T("    </font>\n")
        _T("</td></tr>\n")
        _T("</table>\n")
        _T("\n")
        _T("</td></tr></table></center>\n")
        _T("\n")
        _T("</body>\n")
        _T("</html>\n") );

    close_content_file();
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::close_content_file()
{
    if (!((m_fGenerateLmb && m_pLmbFile) || m_pHtmlFile))
        return;

    if (m_pHtmlFile)
    {
        m_pHtmlFile->Close();
        delete m_pHtmlFile;
        m_pHtmlFile = (wxFile*) NULL;
    }
    else
    {
        m_pZipFile->CloseEntry();
    }
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::write_to_content_file(const wxString& sText)
{
    if (!((m_fGenerateLmb && m_pLmbFile) || m_pHtmlFile)) return;
    if (sText == wxEmptyString) return;

    if (m_fGenerateLmb)
    {
        //wxLogMessage(_T("write_to_content_file: (original) ------------------------------------------"));
        //wxLogMessage(sText);

        wxString trans = sText;
        if (trans.StartsWith(_T(" <a href=")))
        {
            trans.Replace(_T("<a href="),  _T("(link (style \"eBook_normal_link\")(url "), false);
            if (trans.EndsWith(_T("</a>")))
            {
                trans.RemoveLast();
                trans.RemoveLast();
                trans.RemoveLast();
                trans.RemoveLast();
                trans.Append(_T("\"))"));
            }
        }

        trans.Replace(_T("<b>"), _T("\") (txt (style \"eBook_normal_emphasis\") \""));
        trans.Replace(_T("</b>"), _T("\")(txt (style \"eBook_normal\") \""));
        trans.Replace(_T("</a>"), _T("\"))(txt (style \"eBook_normal\") \""));
        trans.Replace(_T("<a href="), _T("\") (link (style \"eBook_normal_link\")(url "));

        trans.Replace(_T("<li> \")"), _T("   (listitem (style \"eBook_listitem\")"));
        trans.Replace(_T("<li>(txt"), _T("   (listitem (style \"eBook_listitem\")(txt"));
        trans.Replace(_T("<li>"), _T("   (listitem (style \"eBook_listitem\")(txt \""));

        trans.Replace(_T("\") \"</li>"), _T("\") )"));
        trans.Replace(_T(")</li>"), _T(") )"));
        trans.Replace(_T("</ul></li>"), _T(") )"));
        trans.Replace(_T("</li>"), _T("\") )"));

        trans.Replace(_T(")\n<ul>"), _T(")\n(itemizedlist"));
        trans.Replace(_T("\n<ul>"), _T("\")\n(itemizedlist"));
        trans.Replace(_T(")\n<ol>"), _T(")\n(orderedlist"));
        trans.Replace(_T("\n<ol>"), _T("\")\n(orderedlist"));

        trans.Replace(_T("</ul>"), _T(")"));
        trans.Replace(_T("</ol>"), _T(")"));

        trans.Replace(_T("(tableCell (style \"eBook_table_cell\")\n \")"),
                      _T("(tableCell (style \"eBook_table_cell\")\n") );
        trans.Replace(_T("(txt (style \"eBook_normal\") \" )"), _T(")") );

        trans.Replace(_T("<img src="), _T("(image (style \"eBook_img\")(file ") );
        trans.Replace(_T(" />"), _T("))") );

        trans.Replace(_T(">"), _T(")(txt \""));
        trans.Replace(_T("(txt (style \"eBook_normal\") )"), _T(" )") );

        //specific hacks
        trans.Replace(_T("(image (style \"eBook_img\")(file \"tool_metronome_16.png\"))"),
                      _T("\") (image (style \"eBook_img\")(file \"tool_metronome_16.png\")) (txt \"") );

        //wxLogMessage(_T("write_to_content_file: (processed) ------------------------------------------"));
        //wxLogMessage(trans);

        m_pLmbFile->WriteString( trans );
    }
    else {
        m_pHtmlFile->Write(sText);
    }
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::CreatePageHeaders(wxString sBookTitle, wxString sHeaderTitle,
                                         wxString sTitleNum)
{
    if (!((m_fGenerateLmb && m_pLmbFile) || m_pHtmlFile)) return;

    if (m_format == k_format_html)
        write_html_headers(sBookTitle, sHeaderTitle, sTitleNum);
    else
        write_ldp_headers();
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::write_html_headers(wxString sBookTitle, wxString sHeaderTitle,
                                          wxString sTitleNum)
{
    write_to_content_file(
        _T("<html>\n<head>\n")
        _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") );
    write_to_content_file(m_sCharCode + _T("\">\n<title>") );
    write_to_content_file( ::wxGetTranslation(sBookTitle) );
    write_to_content_file(_T(": "));
    write_to_content_file( ::wxGetTranslation(sHeaderTitle) );
    write_to_content_file(_T("</title>\n"));
    write_to_content_file(_T("</head>\n\n"));

    m_nOutIndentLevel = 1;     //indent

    if (m_sStyle == _T("lenmus-book"))
    {
        //LenMus eBook style
        write_to_content_file(
            _T("<body bgcolor='#808080'>\n")
            _T("\n")
            _T("<center>\n")
            _T("<table width='720' bgcolor='#ffffff' cellpadding='0' cellspacing='0'>\n")
            _T("<tr><td bgcolor='#ffffff'>\n")
            _T("\n")
            _T("<table width='100%' cellpadding='0' cellspacing='0'>\n")
            _T("<tr><td bgcolor='#7f8adc' align='left'>\n")
            _T("	<font size='-1' color='#ffffff'>&nbsp;&nbsp;") );
        write_to_content_file( ::wxGetTranslation(sBookTitle) );
        write_to_content_file(
            _T("</font></td></tr>\n")
            _T("<tr><td bgcolor='#7f8adc' align='right'><br />\n")
            _T("	<b><font size='+4' color='#ffffff'>") );
        write_to_content_file( ::wxGetTranslation(sHeaderTitle) );
        write_to_content_file(
            _T("&nbsp;</font></b><br /></td></tr>\n")
            _T("<tr><td bgcolor='#ff8800'><img src='ebook_line_orange.png'></td></tr>\n")
            _T("</table>\n")
            _T("\n")
            _T("<br />\n")
            _T("<table width='100%' cellpadding='0' cellspacing='0'>\n")
            _T("<tr><td bgcolor='#ffffff' width='30'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>\n")
            _T("<td>\n") );
    }
    else if (m_sStyle == _T("myMusicTheory"))
    {
        //myMusictTheory style
        write_to_content_file(
            _T("<body bgcolor='#808080'>\n")
            _T("\n")
            _T("<center>\n")
            _T("<table width='720' bgcolor='#ffffff' cellpadding='0' cellspacing='0'>\n")
            _T("<tr><td bgcolor='#ffffff'>\n")
            _T("\n")
            _T("<table width='100%' cellpadding='0' cellspacing='0'>\n")
            _T("<tr><td bgcolor='#eeeeff' align='left'>\n")
            _T("	<font size='-1' color='#000000'>&nbsp;&nbsp;") );
        write_to_content_file( ::wxGetTranslation(sBookTitle) );
        write_to_content_file(
            _T("</font></td></tr>\n")
            _T("<tr><td bgcolor='#eeeeff' align='right'><br />\n")
            _T("	<b><font size='+4' color='#000000'>") );
        write_to_content_file( ::wxGetTranslation(sHeaderTitle) );
        write_to_content_file(
            _T("&nbsp;</font></b><br /></td></tr>\n")
            //_T("<tr><td bgcolor='#ff8800'><img src='ebook_line_orange.png'></td></tr>\n")
            _T("</table>\n")
            _T("\n")
            _T("<br />\n")
            _T("<table width='100%' cellpadding='0' cellspacing='0'>\n")
            _T("<tr><td bgcolor='#ffffff' width='30'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>\n")
            _T("<td>\n") );
    }
    else
        LogMessage(_T("Invalid style '%s'. Html headers not created!"), m_sStyle.c_str());
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::write_ldp_headers()
{
    write_to_content_file(
        _T("(lenmusdoc (vers 0.0)\n")
        _T("(styles \n")
        _T("   (defineStyle \"eBook_content\" (font \"Liberation serif\" 12pt bold)(color #000000)) \n")
        _T("   (defineStyle \"eBook_heading_1\" \n")
        _T("      (font-name \"Liberation serif\")\n")
        _T("      (font-size 16pt)\n")
        _T("      (font-style normal)\n")
        _T("      (font-weight bold)\n")
        _T("      (color #000000)\n")
        _T("      (margin-top 0)\n")
        _T("      (margin-bottom 300)\n")
        _T("   )\n")
        _T("   (defineStyle \"eBook_heading_2\" \n")
        _T("      (font-name \"Liberation serif\")\n")
        _T("      (font-size 14pt)\n")
        _T("      (font-style normal)\n")
        _T("      (font-weight bold)\n")
        _T("      (color #000000)\n")
        _T("      (margin-top 400)\n")      //4mm
        _T("      (margin-bottom 200)\n")
        _T("   )\n")
        _T("   (defineStyle \"eBook_heading_3\" \n")
        _T("      (font-name \"Liberation serif\")\n")
        _T("      (font-size 14pt)\n")
        _T("      (font-style normal)\n")
        _T("      (font-weight bold)\n")
        _T("      (color #000000)\n")
        _T("      (margin-top 400)\n")      //4mm
        _T("      (margin-bottom 200)\n")
        _T("   )\n")
        _T("   (defineStyle \"eBook_heading_4\" \n")
        _T("      (font-name \"Liberation serif\")\n")
        _T("      (font-size 14pt)\n")
        _T("      (font-style normal)\n")
        _T("      (font-weight bold)\n")
        _T("      (color #000000)\n")
        _T("      (margin-top 400)\n")      //4mm
        _T("      (margin-bottom 200)\n")
        _T("   )\n")
        _T("   (defineStyle \"eBook_para\" (margin-bottom 420)) \n")        //4.2 mm
        _T("   (defineStyle \"eBook_listitem\" (margin-bottom 0)) \n")
        _T("   (defineStyle \"eBook_normal\" (font \"Liberation serif\" 12pt normal)(color #000000)) \n")
        _T("   (defineStyle \"eBook_normal_emphasis\" (font \"Liberation serif\" 12pt bold)(color #000000)) \n")
        _T("   (defineStyle \"eBook_table\" \n")
        _T("      (font-name \"Liberation serif\")\n")
        _T("      (font-size 12pt)\n")
        _T("      (font-style normal)\n")
        _T("      (font-weight normal)\n")
        _T("      (color #000000)\n")
        _T("      (margin-bottom 500)\n")      //5mm
        _T("   )\n")
        _T("   (defineStyle \"eBook_table_cell\" \n")
        _T("      (border-width 20.0f)\n")
        _T("      (font-name \"Liberation serif\")\n")
        _T("      (font-size 12pt)\n")
        _T("      (font-style normal)\n")
        _T("      (color #000000)\n")
        _T("   )\n")
        _T("   (defineStyle \"eBook_normal_link\" \n")
        _T("      (font-name \"Liberation serif\")\n")
        _T("      (font-size 12pt)\n")
        _T("      (font-style normal)\n")
        _T("      (color #0000ff)\n")
        _T("      (text-decoration  underline)\n")
        _T("   )\n")
        _T("   (defineStyle \"eBook_normal_italics\" (font \"Liberation serif\" 12pt italic)(color #000000)) \n")
        _T("   (defineStyle \"eBook_references_text\" (font \"Liberation serif\" 12pt normal)(color #000000)) \n")
        _T("   (defineStyle \"eBook_copyright\" (font \"Liberation serif\" 12pt bold)(color #000000)) \n")
        _T("   (defineStyle \"eBook_legal_notice\" (font \"Liberation serif\" 12pt bold)(color #000000)) \n")
        _T("   (defineStyle \"eBook_img\" (font \"Liberation serif\" 12pt bold)(color #000000)) \n")
    );

    if (m_sStyles != _T(""))
        write_to_content_file(m_sStyles);

    write_to_content_file(
        _T(")\n\n")
        _T("(content \n")
    );
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::CreateArticleHeaders(wxString sBookTitle, wxString sHeaderTitle,
                                         wxString sTitleNum)
{
    if (!((m_fGenerateLmb && m_pLmbFile) || m_pHtmlFile)) return;

    //create page headers
    write_to_content_file(
        _T("<html>\n<head>\n")
        _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=") );
    write_to_content_file(m_sCharCode + _T("\">\n<title>") );
    write_to_content_file( ::wxGetTranslation(sBookTitle) );
    write_to_content_file(_T(": "));
    write_to_content_file( ::wxGetTranslation(m_sCoverPage) );
    write_to_content_file(_T("</title>\n</head>\n\n"));

    m_nOutIndentLevel = 1;     //indent

    write_to_content_file(
        _T("<body bgcolor='#808080'>\n")
        _T("\n")
        _T("<center>\n")
        _T("<table width='720' bgcolor='#ffffff' cellpadding='0' cellspacing='0'>\n")
        _T("<tr><td bgcolor='#ffffff'>\n")
        _T("\n")
        _T("<!-- banner -->\n")
        _T("<table width='100%' cellpadding='0' cellspacing='0'>\n")
        _T("<tr><td width='42%' bgcolor='#7f8adc'><img src='ebook_banner_left1.png'></td>\n")
        _T("<td bgcolor='#7f8adc'>&nbsp;</td>\n")
        _T("<td width='28%' bgcolor='#7f8adc' align='right'><img src='leaflet_banner_right_") );
        write_to_content_file(m_sLangCode);
        write_to_content_file(
        _T(".png'></td>\n")
        _T("<td width='20' bgcolor='#7f8adc' rowspan='2'>&nbsp;</td>\n")
        _T("</tr>\n")
        _T("<tr><td bgcolor='#ff8800' colspan='4'>.</td></tr>\n")
        _T("</table>\n\n")
        _T("\n")
        _T("<br />\n")
        _T("<table width='720' cellpadding='0' cellspacing='0'>\n")
        _T("<tr><td width='14' bgcolor='#ffffff'></td>\n")
        _T("<td>\n") );
}

//---------------------------------------------------------------------------------------
// Lmb file management
//---------------------------------------------------------------------------------------


bool lmEbookProcessor::StartLmbFile(wxString sFilename, wxString sLangCode,
                                    wxString sCharCode)
{
    // returns true if success

    wxFileName oFNP( sFilename );
    wxFileName oFDest( g_pPaths->GetLenMusLocalePath() );
    oFDest.AppendDir(sLangCode);
    oFDest.AppendDir(_T("books"));
    oFDest.SetName( oFNP.GetName() );
    oFDest.SetExt(_T("lmb"));
    m_pZipOutFile = new wxFFileOutputStream( oFDest.GetFullPath(), _T("w+b"));
    m_pZipFile = new wxZipOutputStream(*m_pZipOutFile);
    m_pLmbFile = new wxTextOutputStream(*m_pZipFile, wxEOL_NATIVE, wxCSConv(sCharCode));

    return true;
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::TerminateLmbFile()
{
    if (!(m_fGenerateLmb && m_pLmbFile)) return;

    // copy toc file
    CopyToLmb( m_sTocFilename );
    if (!::wxRemoveFile(m_sTocFilename))
    {
        LogMessage(_T("*** Error: File %s could not be deleted"), m_sTocFilename.c_str());
    }

    //copy other files (i.e.: images)
    wxFileName oRoot(m_sFilename);      //to get the root path
    for (int i=0; i < (int)m_aFilesToPack.GetCount(); i++)
    {
        wxFileName oFN( m_aFilesToPack.Item(i) );
        if (!oFN.IsAbsolute()) {
            oFN.SetPath(oRoot.GetPath());
        }
        CopyToLmb( oFN.GetFullPath() );
    }

    //terminate
    delete m_pZipFile;
    m_pLmbFile = (wxTextOutputStream*)NULL;
    m_pZipFile = (wxZipOutputStream*)NULL;
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::CopyToLmb(wxString sFilename)
{
    wxFFileInputStream inFile( sFilename, _T("rb") );
    if (!inFile.IsOk())
    {
        LogMessage(_T("*** Error: File %s can not be merged into LMB"), sFilename.c_str());
        return;
    }
    wxFileName oFN(sFilename);
    m_pZipFile->PutNextEntry( oFN.GetFullName() );
    m_pZipFile->Write( inFile );
    m_pZipFile->CloseEntry();
}


//---------------------------------------------------------------------------------------
// Lang (.cpp) file management
//---------------------------------------------------------------------------------------

bool lmEbookProcessor::StartLangFile(wxString sFilename)
{
    // returns true if success

    wxFileName oFNP( sFilename );
    wxFileName oFDest( g_pPaths->GetLocalePath() );
    oFDest.AppendDir(_T("src"));
    ::wxSetWorkingDirectory(  oFDest.GetPath() );
    oFDest.AppendDir( oFNP.GetName() );
    //if dir does not exist, create it
    //::wxMkDir( oFNP.GetName() );
    //bypass for bug in unicode build (GTK) for wxMkdir
    oFNP.Mkdir(0777);

    oFDest.SetName( oFNP.GetName() );
    oFDest.SetExt(_T("cpp"));
    LogMessage(_T("Creating file '%s'"), oFDest.GetFullPath().c_str());
    m_pLangFile = new wxFile(oFDest.GetFullPath(), wxFile::write);
    if (!m_pLangFile->IsOpened()) {
        LogMessage(_T("*** Error: File %s can not be created"), oFDest.GetFullPath().c_str());
        m_pLangFile = (wxFile*)NULL;
        return false;        //error
    }

    m_pLangFile->Write(_T("wxString sTxt;\n"));

    // add texts included by Langtool (footers, headers, etc.)
    WriteToLang( m_sFooter1 );
    WriteToLang( m_sFooter2 );
    if (!m_sFooter3.IsEmpty())
        WriteToLang( m_sFooter3 );
    WriteToLang( m_sMMT_Footer2 );
    WriteToLang( m_sPhonascus );
    WriteToLang( m_sCoverPage );
    WriteToLang( m_sTranslators );
    WriteToLang( m_sReferences );
    WriteToLang( m_sLessonsBased );

    return true;
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::WriteToLang(const wxString& sText)
{
    //add text to Lang file

    if (!m_pLangFile || sText == _T("")) return;

    //change /n by //n
    wxString sContent = sText;
    sContent.Replace(_T("\n"), _T("\\n"));
    m_pLangFile->Write(_T("sTxt = _(\""));
    m_pLangFile->Write(sContent + _T("\");\n"), wxConvLibc );
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::CreatePoFile(wxString sFilename,
                                    wxString& WXUNUSED(sCharSet),
                                    wxString& sLangName,
                                    wxString& WXUNUSED(sLangCode),
                                    wxString& sFolder)
{
    // returns true if success

    wxFile oFile(sFilename, wxFile::write);
    if (!m_pLangFile->IsOpened())
    {
        LogMessage(_T("*** Error: File %s can not be created"), sFilename.c_str());
        m_pLangFile = (wxFile*)NULL;
        return false;        //error
    }

    //Generate Po header
    wxString sNil = _T("");
    wxString sHeader = sNil +
        _T("msgid \"\"\n")
        _T("msgstr \"\"\n")
        _T("\"Project-Id-Version: LenMus 3.4\\n\"\n")
        _T("\"POT-Creation-Date: \\n\"\n")
        _T("\"PO-Revision-Date: 2006-08-25 12:19+0100\\n\"\n")
        _T("\"Last-Translator: \\n\"\n")
        _T("\"Language-Team:  <cecilios@gmail.com>\\n\"\n")
        _T("\"MIME-Version: 1.0\\n\"\n")
        _T("\"Content-Type: text/plain; charset=utf-8\\n\"\n")
        _T("\"Content-Transfer-Encoding: 8bit\\n\"\n")
        _T("\"X-Poedit-Language: ") + sLangName + _T("\\n\"\n")
        _T("\"X-Poedit-SourceCharset: utf-8\\n\"\n")
        _T("\"X-Poedit-Basepath: c:\\usr\\desarrollo_wx\\lenmus\\langtool\\locale\\src\\n\"\n")
        _T("\"X-Poedit-SearchPath-0: ") + sFolder + _T("\\n\"\n\n");


    oFile.Write(sHeader);
    oFile.Close();
    return true;
}


//---------------------------------------------------------------------------------------
// Debug methods
//---------------------------------------------------------------------------------------

void lmEbookProcessor::DumpXMLTree(const wxXml2Node& oNode)
{
    wxString sTree;
    sTree.Alloc(1024);
    int nIndent = 3;

    // get a string with the tree structure...
    DumpNodeAndSiblings(oNode, sTree, nIndent);
    wxLogMessage(sTree);
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::DumpNodeAndSiblings(const wxXml2Node& oNode, wxString& sTree, int n)
{
    wxXml2Node oCurr(oNode);

    do {
        DumpNode(oCurr, sTree, n);
        oCurr = oCurr.GetNext();
    } while (oCurr != wxXml2EmptyNode);
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::DumpNode(const wxXml2Node& oNode, wxString& sTree, int n)
{
#define STEP            4

    if (oNode == wxXml2EmptyNode) return;
    wxString toadd, spaces(wxT(' '), n);

    // concatenate the name of this node
    toadd = oNode.GetName();

    // if this is a text node, then add also the contents...
    if (oNode.GetType() == wxXML_TEXT_NODE ||
        oNode.GetType() == wxXML_COMMENT_NODE ||
        oNode.GetType() == wxXML_CDATA_SECTION_NODE) {

        wxString content = oNode.GetContent();
        if (content != wxEmptyString && content.Last() == wxT('\n')) content.RemoveLast();
        if (content != wxEmptyString && content.GetChar(0) == wxT('\n')) content.Remove(0, 1);

        // a little exception: libxml2 when loading a document creates a
        // lot of text nodes containing just a simple \n;
        // in this cases, just show "[null]"
        wxString tmp = content.Trim();
        if (tmp.IsEmpty())
            toadd += wxT(" node: [null]");
        else
            toadd += wxT(" node: ") + content;


    } else {        // if it's not a text node, then add the properties...

        wxXml2Property prop(oNode.GetProperties());
        while (prop != wxXml2EmptyProperty) {
            toadd += wxT(" ") + prop.GetName() + wxT("=");
            toadd += prop.GetValue();
            prop = prop.GetNext();
        }
    }

    sTree += spaces;

#define SHOW_ANNOYING_NEWLINES
#ifdef SHOW_ANNOYING_NEWLINES

    // text nodes with newlines and/or spaces will be shown as [null]
    sTree += toadd;
#else

    // text nodes with newlines won't be shown at all
    if (toadd != wxT("textnode: [null]")) sTree += toadd;
#endif

    // go one line down
    sTree += wxT("\n");

    // do we must add the close tag ?
    bool bClose = FALSE;

    // and then, a step more indented, its children
    wxXml2Node child(oNode.GetFirstChild());
    while (child != wxXml2EmptyNode) {

        DumpNode(child, sTree, n+STEP);
        child = child.GetNext();

        // add a close tag because at least one child is present...
        bClose = TRUE;
    }

    if (bClose) sTree += wxString(wxT(' '), n) + wxT("/") + oNode.GetName() + wxT("\n");
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::LogMessage(const wxChar* szFormat, ...)
{
    if (!m_pLog) return;

    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg = wxString::FormatV(szFormat, argptr) + _T("\n");
    m_pLog->AppendText(sMsg);
    va_end(argptr);
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::LogError(const wxChar* szFormat, ...)
{
    if (!m_pLog) return;

    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg = _T("*** Error ***: ");
    sMsg += wxString::FormatV(szFormat, argptr) + _T("\n");
    m_pLog->AppendText(sMsg);
    va_end(argptr);
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::is_just_replace_tag(const wxString& sTag)
{
    return m_tags->is_just_replace_tag(sTag);
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::is_exercise_param_tag(const wxString& sTag)
{
    return m_tags->is_exercise_param_tag(sTag);
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::is_score_param_tag(const wxString& sTag)
{
    return m_tags->is_score_param_tag(sTag);
}

//---------------------------------------------------------------------------------------
bool lmEbookProcessor::is_po_msg_delimiter_tag(const wxString& sTag)
{
    return m_tags->is_po_msg_delimiter_tag(sTag);
}

//---------------------------------------------------------------------------------------
const lmReplacement& lmEbookProcessor::GetReplacement(const wxString& sTag)
{
    return m_tags->get_replacement(sTag);
}

//---------------------------------------------------------------------------------------
void lmEbookProcessor::AddReplacement(const wxString& sOuttag, lmContentStorage* pResult)
{
    if (sOuttag.IsEmpty()) return;

    if (pResult)
        pResult->Add(sOuttag);
}

//---------------------------------------------------------------------------------------
// translation
//---------------------------------------------------------------------------------------

void lmEbookProcessor::TranslateContent(lmContentStorage* pContent)
{
    if (pContent && !pContent->IsTranslated())
    {
        lmContentStorage csPoMsg;
        wxString sMsg = pContent->GeneratePoMessage(&csPoMsg);
        wxLogMessage(_T("Po Msg:"));
        wxLogMessage(sMsg);
        if (sMsg.StartsWith(_T("<a-1>Exercise ")) )    //<a-1>Exercise 1</a-1> -
        {
            int iEnd = sMsg.Find(_T("</a-1>"));
            wxString sMsg1 = sMsg.Mid(5, iEnd-5);
            wxString sMsg2 = sMsg.Mid(iEnd+6);
            //wxLogMessage(_T("sMsg1=[%s]"), sMsg1);
            //wxLogMessage(_T("sMsg2=[%s]"), sMsg2);

            //replace content or write to lang file
            if (m_fOnlyLangFile)
            {
                WriteToLang(sMsg1);
                WriteToLang(sMsg2);
                //mark as 'translated'
                pContent->SetTranslated(true);
                //wxLogMessage(_T("Po Msg:"));
                //wxLogMessage(sMsg);
            }
            else
            {
                //get translation
                wxString sTrans = _T("<a-1>");
                sTrans += ::wxGetTranslation(sMsg1);
                sTrans += _T("</a-1>");
                sTrans += ::wxGetTranslation(sMsg2);

                //do inverse processing to reconstruct original message
                pContent->GenerateTranslation(&csPoMsg, sTrans);
            }
        }

        else if (sMsg.StartsWith(_T("Scales\n<ul>\n<li> <a-1>")) )
        {
            int iEnd = sMsg.Find(_T("</a-1>"));
            wxString sMsg1 = sMsg.Mid(5, iEnd-5);
            wxString sMsg2 = sMsg.Mid(iEnd+6);
            if (m_fOnlyLangFile)
            {
                WriteToLang(_T("Scales"));
                WriteToLang(_T("Semitones. The chromatic scale"));
                WriteToLang(_T("Diatonic scales. Tonal center"));
                WriteToLang(_T("Tones.Major scales"));
                WriteToLang(_T("Minor natural scales"));
                WriteToLang(_T("Harmonic and melodic minor scales"));
                WriteToLang(_T("Exercises"));
                pContent->SetTranslated(true);
            }
            else
            {
                wxString sTrans = ::wxGetTranslation(_T("Scales"));
                sTrans += _T("\n<ul>\n<li> <a-1>");
                sTrans += ::wxGetTranslation(_T("Semitones. The chromatic scale"));
                sTrans += _T("</a-1></li>\n<li> <a-1>");
                sTrans += ::wxGetTranslation(_T("Diatonic scales. Tonal center"));
                sTrans += _T("</a-1></li>\n<li> <a-1>");
                sTrans += ::wxGetTranslation(_T("Tones.Major scales"));
                sTrans += _T("</a-1></li>\n<li> <a-1>");
                sTrans += ::wxGetTranslation(_T("Minor natural scales"));
                sTrans += _T("</a-1></li>\n<li> <a-1>");
                sTrans += ::wxGetTranslation(_T("Harmonic and melodic minor scales"));
                sTrans += _T("</a-1></li>\n<li> <a-1>");
                sTrans += ::wxGetTranslation(_T("Exercises"));
                sTrans += _T("</a-1></li>\n</ul>");

                pContent->GenerateTranslation(&csPoMsg, sTrans);
            }
        }

        else if (sMsg.StartsWith(_T("Intervals\n<ul>\n<li> <a-1>What")) )
        {
            int iEnd = sMsg.Find(_T("</a-1>"));
            wxString sMsg1 = sMsg.Mid(5, iEnd-5);
            wxString sMsg2 = sMsg.Mid(iEnd+6);
            if (m_fOnlyLangFile)
            {
                WriteToLang(_T("Intervals"));
                WriteToLang(_T("What is an interval?"));
                pContent->SetTranslated(true);
            }
            else
            {
                wxString sTrans = ::wxGetTranslation(_T("Intervals"));
                sTrans += _T("\n<ul>\n<li> <a-1>");
                sTrans += ::wxGetTranslation(_T("What is an interval?"));
                sTrans += _T("</a-1></li>\n</ul>");

                pContent->GenerateTranslation(&csPoMsg, sTrans);
            }
        }

        else if (sMsg.StartsWith(_T("Cadences\n<ul>\n<li> <a-1>")) )
        {
            int iEnd = sMsg.Find(_T("</a-1>"));
            wxString sMsg1 = sMsg.Mid(5, iEnd-5);
            wxString sMsg2 = sMsg.Mid(iEnd+6);
            if (m_fOnlyLangFile)
            {
                WriteToLang(_T("Cadences"));
                WriteToLang(_T("Terminal cadences"));
                WriteToLang(_T("Transient cadences"));
                WriteToLang(_T("Exercises on cadences"));
                pContent->SetTranslated(true);
            }
            else
            {
                wxString sTrans = ::wxGetTranslation(_T("Cadences"));
                sTrans += _T("\n<ul>\n<li> <a-1>");
                sTrans += ::wxGetTranslation(_T("Cadences"));
                sTrans += _T("</a-1></li>\n<li> <a-1>");
                sTrans += ::wxGetTranslation(_T("Terminal cadences"));
                sTrans += _T("</a-1></li>\n<li> <a-1>");
                sTrans += ::wxGetTranslation(_T("Transient cadences"));
                sTrans += _T("</a-1></li>\n<li> <a-1>");
                sTrans += ::wxGetTranslation(_T("Exercises on cadences"));
                sTrans += _T("</a-1></li>\n</ul>");

                pContent->GenerateTranslation(&csPoMsg, sTrans);
            }
        }

        else
        {
            //remove common entities
            //sMsg.Replace(_T("&quot;"), _T("'"), true);
            //sMsg.Replace(_T("&lt;"), _T("<"), true);
            //sMsg.Replace(_T("&gt;"), _T(">"), true);

            //replace content or write to lang file
            if (m_fOnlyLangFile)
            {
                WriteToLang(sMsg);
                //mark as 'translated'
                pContent->SetTranslated(true);
                //wxLogMessage(_T("Po Msg:"));
                //wxLogMessage(sMsg);
            }
            else
            {
                //get translation
                wxString sTrans = ::wxGetTranslation(sMsg);

                //restore commomn entities
                //sTrans.Replace(_T("\\\""), _T("&quot;"), true);
                //sTrans.Replace(_T("<"), _T("&lt;"), true);
                //sTrans.Replace(_T(">"), _T("&gt;"), true);

                //do inverse processing to reconstruct original message
                pContent->GenerateTranslation(&csPoMsg, sTrans);
            }
        }
    }
}

