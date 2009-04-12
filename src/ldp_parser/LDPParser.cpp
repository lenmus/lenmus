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

// The parser for the LDP language
// "Parse" functions: work on source text
// "Analyze" functions: work on a tree of LMNodes

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "LDPParser.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <math.h>        // for function pow()
#include <algorithm>

#include "wx/textfile.h"
#include "wx/log.h"

#include "../score/Score.h"
#include "../score/VStaff.h"
#include "../score/MetronomeMark.h"
#include "../score/InstrGroup.h"
#include "../auxmusic/Conversion.h"
#include "../widgets/MsgBox.h"
#include "LDPParser.h"
#include "AuxString.h"


//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

#include "../app/Preferences.h"

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"


const wxString sEOF = _T("<<$$EOF$$>>");


//========================================================================================
//helper class to keep info about a tie
//========================================================================================
class lmTieInfo
{
public:
    lmTieInfo() {}
    ~lmTieInfo() {}

    bool        fStart;
    long        nTieId;
    lmNote*     pNote;
    lmTPoint    tBezier[4];
};



//========================================================================================
// lmLDPParser implementation
//========================================================================================

lmLDPParser::lmLDPParser()
{
    Create(_T("en"), _T("utf-8"));     //default tags in English
}

lmLDPParser::lmLDPParser(wxString sLanguage, wxString sCharset)
{
    Create(sLanguage, sCharset);
}

void lmLDPParser::Create(const wxString& sLanguage, const wxString& sCharset)
{
    m_pTokenizer = new lmLDPTokenBuilder(this);
    m_nNumLine = 0;
    m_pCurNode = (lmLDPNode*) NULL;
    m_fDebugMode = g_pLogger->IsAllowedTraceMask(_T("lmLDPParser"));
    m_pTuplet = (lmTupletBracket*)NULL;
    // default values for font and aligment for <title> elements
    //TODO user options instead of fixed values
    m_nTitleAlignment = lmHALIGN_CENTER;
    m_sTitleFontName = _T("Times New Roman");
    m_nTitleFontSize = 14;
    m_nTitleStyle = wxFONTSTYLE_NORMAL;
    m_nTitleWeight = wxFONTWEIGHT_BOLD;

    // default values for font and aligment for <text> elements
    //TODO user options instead of fixed values
    m_sTextFontName = _T("Times New Roman");
    m_nTextFontSize = 10;
    m_nTextStyle = wxFONTSTYLE_NORMAL;
    m_nTextWeight = wxFONTWEIGHT_NORMAL;

    // default values for notes/rests octave and duration
    m_sLastOctave = _T("4");        // octave 4
    m_sLastDuration = _T("q");      // quarter note

    // default values for tuplet options
    m_fShowTupletBracket = true;
    m_fShowNumber = true;
    m_fTupletAbove = true;

	//other initializations
	m_pLastNoteRest = (lmNoteRest*)NULL;
    m_pScore = (lmScore*) NULL;
}

lmLDPParser::~lmLDPParser()
{
    delete m_pTokenizer;

    Clear();
}

void lmLDPParser::Clear()
{
    //if (m_fDebugMode) {
    //    wxLogMessage(
    //        _T("**TRACE** Entering lmLDPParser destructor"));
    //}

    std::vector<lmLDPNode*>::iterator it;
    for(it=m_StackNodes.begin(); it != m_StackNodes.end(); ++it)
        delete *it;
    m_StackNodes.clear();

    std::list<lmTieInfo*>::iterator itT;
    for(itT=m_PendingTies.begin(); itT != m_PendingTies.end(); ++itT)
        delete *itT;
    m_PendingTies.clear();

    delete m_pCurNode;
    m_pCurNode = (lmLDPNode*) NULL;
}


lmScore* lmLDPParser::ParseFile(const wxString& filename, bool fErrorMsg)
{
    m_sFileName = filename;
    wxFileInputStream inFile(filename);
    if (!inFile.Ok())
    {
        if (fErrorMsg)
        {
            FileParsingError(wxString::Format( _T("Error opening file <%s>"),
                filename.c_str()));
        }
        return (lmScore*) NULL;
    }

    wxTextInputStream inTextFile(inFile);

    m_pTextFile = &inTextFile;
    m_pFile = &inFile;
    m_fFromString = false;        //a file is going to be parsed, not a string
    m_nErrors = 0;
    m_nWarnings = 0;
    g_pLogger->FlushDataErrorLog();
    lmLDPNode* pRoot = LexicalAnalysis();
    lmScore* pScore = (lmScore*) NULL;
	m_pLastNoteRest = (lmNoteRest*)NULL;

    //proceed to create the score
    if (pRoot)
        pScore = AnalyzeScore(pRoot);

    // report errors
    bool fShowLog = true;
    if (fShowLog && m_nErrors != 0) {
        g_pLogger->ShowDataErrors(_T("Warnings/errors while reading LenMus score."));
    }

    //if (pScore) pScore->Dump(_T("lenmus_score_dump.txt"));      //dbg
    return pScore;

}

lmLDPNode* lmLDPParser::ParseText(const wxString& sSource)
{
    //as error reporting is bad, I will verify that parenthesis are
    //matched. Otherwise, return error to save time and program failures
    if (!ParenthesisMatch(sSource)) return (lmLDPNode*)NULL;

    //now proceed
    Clear();                            //delete old allocated objects
    m_fFromString = true;                //parsing a string, not a file
    m_sFileName = _T("'No file'");
    m_fStartingTextAnalysis = true;        //signal the start of a new analysis
    m_sLastBuffer = sSource + sEOF;        //load string to parse into buffer

    return LexicalAnalysis();            // and proceed with the analysis
}

bool lmLDPParser::ParenthesisMatch(const wxString& sSource)
{
    int i = sSource.length();
    int nPar = 0;
    for(i=0; i < (int)sSource.length(); i++) {
        if (sSource.GetChar(i) == _T('('))
            nPar++;
        else if (sSource.GetChar(i) == _T(')'))
            nPar--;
    }
    return (nPar == 0);

}


void lmLDPParser::FileParsingError(const wxString& sMsg)
{
    wxMessageBox(sMsg, _T("Error"));
    //TODO: replace by lmErrorBox. But for this I need to implement auto-wrap
    //lmErrorBox oEB(sMsg, _(""));
    //oEB.ShowModal();
}

//------------------------------------------------------------------------------------------
// File management functions
//------------------------------------------------------------------------------------------

const wxString& lmLDPParser::GetNewBuffer()
{
    if (m_fFromString)
    {
        //parsing a string
        if (m_fStartingTextAnalysis)
        {
            // m_sLastBuffer is loaded with string to analyse. So return this buffer
            m_nNumLine = 1;
            m_fStartingTextAnalysis = false;
        }
        else
        {
            // the string is finished. End of analysis. Return EOF buffer
            m_sLastBuffer = sEOF;
        }
    }
    else
    {
        //parsing a file
        //m_sLastBuffer = _T("(n s)") + sEOF;
//        if (Not fRepetirLinea) {
            if (m_pFile->Eof()) {
                m_sLastBuffer = sEOF;
            } else {
                m_sLastBuffer = m_pTextFile->ReadLine();
                m_nNumLine++;
            }
//        }
    }
//    fRepetirLinea = false
    return m_sLastBuffer;
}

void lmLDPParser::ParseMsje(wxString sMsg)
{
    //GrabarError "Aviso - " & sMsg
    m_nWarnings++;

    //if (m_fDebugMode)
        wxLogMessage( _T("**WARNING** %s"), sMsg.c_str() );

}

void lmLDPParser::ParseError(EParsingStates nState, lmLDPToken* pTk)
{
    m_nErrors++;
    wxLogMessage(_T("** LDP ERROR **: Syntax error. State %d, TkType %s, tkValue <%s>"),
            nState, pTk->GetDescription().c_str(), pTk->GetValue().c_str() );

}

void lmLDPParser::AnalysisError(lmLDPNode* pNode, const wxChar* szFormat, ...)
{
    m_nErrors++;

    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg;
    if (pNode)
        sMsg = wxString::Format(_T("** LDP ERROR ** (line %d): "), pNode->GetNumLine());
    else
        sMsg = _T("** LDP ERROR **: ");

    sMsg += wxString::FormatV(szFormat, argptr);
    wxLogMessage(sMsg);
    if (m_fFromString)
        wxLogMessage(m_sLastBuffer);
    g_pLogger->LogDataError(sMsg);
    va_end(argptr);
}

lmLDPNode* lmLDPParser::LexicalAnalysis()
{
    /*
    This function analyzes string sSource and returns the root node of the analysis.
    The result of the analysis is a tree of nodes, each one representing an element. The
    root node is the element representing the element sSource.

    In this step a full lexical analysis is performed as well as a first syntactical
    analysis limited to verify that sSource has the structure of an element and its content
    is also elements (data between parenthesis). This element structure is then transformed
    into nodes and arranged into a tree.

    It is admisible that sSource has new line and carriage return characters so that this
    funtion can parse a whole file whose content is received into the string sSource.

    The analyzer is implemented with a main loop to keep trace of current automata state and
    as many functions as automata states, to perform the steps asociated to each state.

    */
////THINK: tal como está ahora, el análisis se hace en fases:
////1. Léxico (analysis sintáctico a nivel de tokens) + Analisis sintáctico limitado a
////   que todo el archivo tiene una estructura recursiva de elementos (datos
////   encerrados entre paréntesis). Transformación del texto fuente en una estructura de nodos
////2. Sintáctico y semántico: verificar la sintaxis de cada tipo de elemento y del conjunto.
////   Añadir objetos a CScore para representar los elementos analizados.
////
////El código de AnalizarArchivo hace la primera fase y la útima línea, la llamada a
////"AnalyzeScore", se encarga de la segunda fase.
//
////Dado que "AnalyzeScore" espera sólo
////una estructura de nodos y a su vez, las funciones llamadas por ella ("AnalyzeVStaff",
////"AnalyzeMeasure, "AnalyzeNote", etc) sólo esperan un arbol de nodos y un objeto en el que
////insertar el resultado, es perfectamente factible llamar a AnalizarArchivo (bueno a algo nuevo
////"ParseText" pero análogo a este código)
////para realizar el análisis de un texto plano representando, por ejemplo, un compas. Y llamar
////luego a "AnalyzeMeasure" pasándole el arbol de nodos creado y el pentagrama donde
////insertarlo.
//
//

    m_pTk = (lmLDPToken*) NULL;
    m_stackStates.Clear();
    m_StackNodes.clear();
    m_nErrors = 0;
    m_nWarnings = 0;

    m_nCurStaff = 1;
    m_nCurVoice = 1;


    m_pCurNode = new lmLDPNode(_T("Root Node"), 0);
    m_nLevel = 0;

    m_nState = A0_WaitingForStartOfElement;
    PushNode(A0_WaitingForStartOfElement);      //start the tree with the root node
    bool fExitLoop = false;
    while(!fExitLoop) {
        m_pTk = m_pTokenizer->ReadToken();        //m_pTk->ReadToken();

        // if debug mode write trace
        if (m_fDebugMode) {
            if (m_pTk->GetType() == tkEndOfElement) {
                wxLogMessage(
                    _T("**TRACE** State %d, TkType %s, tkValue <%s>, node <%s>"),
                    m_nState, m_pTk->GetDescription().c_str(),
                    m_pTk->GetValue().c_str(), m_pCurNode->GetName().c_str() );
            } else {
                wxLogMessage(
                    _T("**TRACE** State %d, TkType %s, tkValue <%s>"),
                    m_nState, m_pTk->GetDescription().c_str(),
                    m_pTk->GetValue().c_str() );
            }
        }

        switch (m_nState) {
            case A0_WaitingForStartOfElement:
                Do_WaitingForStartOfElement();
                break;
            case A1_WaitingForName:
                Do_WaitingForName();
                break;
            case A2_WaitingForParameter:
                //Do_WaitingForParameter();     //changed to allow for elements without parameters
                Do_ProcessingParameter();
                break;
            case A3_ProcessingParameter:
                Do_ProcessingParameter();
                break;
            case A4_Exit:
            case A5_ExitError:
                fExitLoop = true;
                break;
            default:
                ParseError(m_nState, m_pTk);
                fExitLoop = true;
        }
        if (m_pTk->GetType() == tkEndOfFile) fExitLoop = true;
    }

    // exit if error
    if (m_nState == A5_ExitError) {
        //TODO delete node objects. Clear() is not enough. Something else is needed.
        return (lmLDPNode*) NULL;
    }

    // at this point m_pCurNode is all the tree. Verify it.
    if (m_pCurNode->GetName() != _T("Root Node")) {
        AnalysisError(m_pCurNode, _T("Element RAIZ expected but found element %s. Analysis stopped."),
            m_pCurNode->GetName().c_str() );
        m_pCurNode->DumpNode();
        return (lmLDPNode*) NULL;
    }
    //m_pCurNode->DumpNode();
    return m_pCurNode->GetParameter(1);

}

void lmLDPParser::Do_WaitingForStartOfElement()
{
    switch (m_pTk->GetType()) {
        case tkStartOfElement:
            m_nState = A1_WaitingForName;
            break;
        case tkEndOfFile:
            m_nState = A4_Exit;
            break;
        default:
            ParseError(m_nState, m_pTk);
            m_nState = A0_WaitingForStartOfElement;
    }

}

void lmLDPParser::Do_WaitingForName()
{
    switch (m_pTk->GetType()) {
        case tkLabel:
            m_pCurNode = new lmLDPNode(m_pTk->GetValue(), m_nNumLine);
            if (m_fDebugMode) {
                wxLogMessage(
                    _T("**TRACE** State %d, TkType %s : creando nuevo nodo <%s>"),
                    m_nState, m_pTk->GetDescription().c_str(),
                    m_pCurNode->GetName().c_str() );
            }
            m_nState = A2_WaitingForParameter;
            break;
        default:
            ParseError(m_nState, m_pTk);
            if (m_pTk->GetType() == tkEndOfFile)
                m_nState = A4_Exit;
            else
                m_nState = A1_WaitingForName;
    }

}

void lmLDPParser::Do_WaitingForParameter()
{
    switch (m_pTk->GetType()) {
        case tkStartOfElement:
            PushNode(A3_ProcessingParameter);    // add current node (name of element or parameter) to the tree
            m_pTokenizer->RepeatToken();
            m_nState = A0_WaitingForStartOfElement;
            break;
        case tkLabel:
        case tkIntegerNumber:
        case tkRealNumber:
        case tkString:
            m_pCurNode->AddParameter(m_pTk->GetValue());
            m_nState = A3_ProcessingParameter;
            break;
        default:
            ParseError(m_nState, m_pTk);
            if (m_pTk->GetType() == tkEndOfFile)
                m_nState = A4_Exit;
            else
                m_nState = A2_WaitingForParameter;
    }

}

void lmLDPParser::Do_ProcessingParameter()
{
    switch (m_pTk->GetType()) {
        case tkLabel:
        case tkIntegerNumber:
        case tkRealNumber:
        case tkString:
            m_pCurNode->AddParameter(m_pTk->GetValue());
            m_nState = A3_ProcessingParameter;
            break;
        case tkStartOfElement:
            PushNode(A3_ProcessingParameter);    // add current node (name of element or parameter) to the tree
            m_pTokenizer->RepeatToken();
            m_nState = A0_WaitingForStartOfElement;
            break;
        case tkEndOfElement:
            {
            lmLDPNode* pParm = m_pCurNode;        //save ptr to node just created
            if (PopNode()) {                      //restore into m_pCurNode the previous node (the owner of this parameter)
                //error
                m_nState = A5_ExitError;
            }
            else
                m_pCurNode->AddNode(pParm);
            break;
            }
        default:
            ParseError(m_nState, m_pTk);
            if (m_pTk->GetType() == tkEndOfFile)
                m_nState = A4_Exit;
            else
                m_nState = A3_ProcessingParameter;
    }

}

void lmLDPParser::PushNode(EParsingStates nPopState)
{
    //nPopState is the state at which the automata will return when the Pop operation take place
    m_stackStates.Add(nPopState);
    m_StackNodes.push_back(m_pCurNode);    //Add(m_pCurNode);
    m_nLevel++;

    if (m_fDebugMode) {
        wxLogMessage( _T("**TRACE** PushNode - Stored values:  State %d, node <%s>. stack count=%d"),
            nPopState, m_pCurNode->GetName().c_str(),
            m_StackNodes.size() );
    }

}

//! returns true if error
bool lmLDPParser::PopNode()
{
   long i;
    m_nLevel--;
    i = m_stackStates.GetCount();
    if (i == 0) {
        //more closing parenthesis than parenthesis opened
        AnalysisError((lmLDPNode*)NULL,
                       _T("Syntax error: more closing parenthesis than parenthesis opened. Analysis stopped."));
        return true;    //error
    }
    i--;    // the array is 0 based
    EParsingStates curState = m_nState;
    m_nState = (EParsingStates) m_stackStates.Item(i);
    m_stackStates.RemoveAt(i);
    m_pCurNode = m_StackNodes[i];
    std::vector<lmLDPNode*>::iterator it =
		std::find(m_StackNodes.begin(), m_StackNodes.end(), m_pCurNode);
    m_StackNodes.erase(it);

    // if debug mode print message
    if (m_fDebugMode) {
        wxLogMessage( _T("**TRACE** PopNode - State %d, New Values: State %d, node <%s>"),
            curState, m_nState, m_pCurNode->GetName().c_str() );
    }

    return false;       //no error
}

lmScore* lmLDPParser::AnalyzeScore(lmLDPNode* pNode)
{
    //disable edition options that could interfere with direct score creation
    bool fAutoBeam = g_fAutoBeam;
    g_fAutoBeam = false;

    lmScore* pScore = (lmScore*) NULL;
    int i;

    if (!(pNode->GetName() == _T("score") || pNode->GetName() == _T("Score")) ) {
        AnalysisError(pNode, _T("Element 'score' expected but found element %s. Analysis stopped."),
            pNode->GetName().c_str() );
        return pScore;
    }

    //get parameters
    int nParms, iP;
    lmLDPNode* pX;
    nParms = pNode->GetNumParms();

    //parse version
    iP = 1;
    pX = pNode->GetParameter(iP);     //version
    if (!(pX->GetName() == _T("Vers") || pX->GetName() == _T("vers"))) {
        AnalysisError(pX, _T("Element 'vers' expected but found element %s. Analysis stopped."),
            pX->GetName().c_str() );
        return pScore;
    } else {
        m_sVersion = (pX->GetParameter(1))->GetName();
        i = m_sVersion.Find(_T('.'));
        long nVers, nRev;
       if (i != -1) {
            (m_sVersion.Mid(0, i)).ToLong(&nVers);
            (m_sVersion.Mid(i + 1)).ToLong(&nRev);
        } else {
            m_sVersion.ToLong(&nVers);
            nRev = 0;
        }
        m_nVersion = 100 * nVers + nRev;
        iP++;
    }

    switch (m_nVersion) {
        //case 102:
        //case 103:
        //    pScore = AnalyzeScoreV102(pNode);
        //    break;
        case 105:
        case 106:
            pScore = AnalyzeScoreV105(pNode);
            break;
        default:
            AnalysisError(pNode, _T("Error analysing LDP score: LDP version (%d) not supported. Analysis stopped."),
                m_nVersion );
            return pScore;
    }

    //restore edition options
    g_fAutoBeam = fAutoBeam;

     return pScore;
}

lmScore* lmLDPParser::AnalyzeScoreV105(lmLDPNode* pNode)
{
    //<score> = (score <vers> [<language>][<styles>][<pageLayout>][<titles>] <instrument>*)
    //<language> = (language LanguageCode Charset)

    lmLDPNode* pX;
    long i, iP;
    wxString sData;
    int nNumParms = pNode->GetNumParms();

    //parse element <language>
    wxString sLangCode = _T("en");
    wxString sCharset = _T("");
    iP = 2;      //first parameter is always the version and is already analyzed. So, skip it
    pX = pNode->GetParameter(iP);
    if (pX->GetName() == _T("language") ) {
        if (pX->GetNumParms() != 2) {
        }
        else {
            sLangCode = (pX->GetParameter(1))->GetName();
            sCharset = (pX->GetParameter(2))->GetName();
        }
        iP++;
    }

    // create the score
    m_pScore = new lmScore();

    //parse optional elements <defineStyle>
    pX = pNode->GetParameter(iP);
    while(pX->GetName() == _T("defineStyle") &&  iP <= nNumParms)
    {
        AnalyzeDefineStyle(pX, m_pScore);
        iP++;
        if (iP <= nNumParms) pX = pNode->GetParameter(iP);
    }

    //parse optional elements <titles>
    pX = pNode->GetParameter(iP);
    while(pX->GetName() == _T("title") &&  iP <= nNumParms)
    {
        AnalyzeTitle(pX, m_pScore);
        iP++;
        if (iP <= nNumParms) pX = pNode->GetParameter(iP);
    }

    //parse optional element <pageLayout>
    pX = pNode->GetParameter(iP);
    while(pX->GetName() == _T("pageLayout") &&  iP <= nNumParms)
    {
        AnalyzePageLayout(pX, m_pScore);
        iP++;
        if (iP <= nNumParms) pX = pNode->GetParameter(iP);
    }

    //parse optional elements <opt>
    pX = pNode->GetParameter(iP);
    while(pX->GetName() == _T("opt") &&  iP <= nNumParms)
    {
        AnalyzeOption(pX, m_pScore);
        iP++;
        if (iP <= nNumParms) pX = pNode->GetParameter(iP);
    }

    // loop to parse elements <instrument> and <group>
    i=0;
    while(iP <= nNumParms)
    {
        if ( pX->GetName() == _T("instrument") )
            AnalyzeInstrument105(pX, m_pScore, i++);
        else if ( pX->GetName() == _T("group") )
            i += AnalyzeGroup(pX, m_pScore, i);
        else
        {
            AnalysisError(pX, _T("Elements <instrument> or <group> expected but found element %s. Analysis stopped."),
                pX->GetName().c_str() );
            break;
        }
        iP++;
        if (iP <= nNumParms)
            pX = pNode->GetParameter(iP);
    }

    return m_pScore;
}

int lmLDPParser::AnalyzeGroup(lmLDPNode* pNode, lmScore* pScore, int nInstr)
{
    //Returns the number of instruments added to the score

    //<Group> = (group [<GrpName>] <GrpSymbol> [<JoinBarlines>] Instrument+ )
    //
    //<GrpName> = (name name-string [(abbrev abbreviation-string)])
    //<GrpSymbol> = (symbol {none | brace | bracket} )
    //<JoinBarlines> = (joinBarlines {yes | no} )

    lmLDPNode* pX;
    wxString sData;
    long iP;
    iP = 1;

    wxASSERT( pNode->GetName() == _T("group") );

    //default values for name
    //TODO user options instead of fixed values
    wxString sGrpName = _T("");           //no name for group
    wxString sNameStyle = _T("");
    lmEHAlign nNameAlign = lmHALIGN_LEFT;
    bool fNameHasWidth = false;
    lmFontInfo tNameFont = g_tInstrumentDefaultFont;
    lmLocation tNamePos = g_tDefaultPos;

    //default values for abbreviation
    //TODO user options instead of fixed values
    wxString sGrpAbbrev = _T("");         //no abreviated name for group
    wxString sAbbrevStyle = _T("");
    lmEHAlign nAbbrevAlign = lmHALIGN_LEFT;
    bool fAbbrevHasWidth = false;
    lmFontInfo tAbbrevFont = g_tInstrumentDefaultFont;
    lmLocation tAbbrevPos = g_tDefaultPos;

    //default values for other parameters
    bool fJoinBarlines = true;
    lmEBracketSymbol nGrpSymbol = lm_eBrace;

    //parse elements until <Instrument> tag found
    bool fInstrFound = false;
    for (; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);

        if (pX->GetName() == _T("instrument") )
        {
            fInstrFound = true;
            break;      //start of Instrument. Exit this loop
        }
        else if (pX->GetName() == _T("name") )
        {
            AnalyzeTextString(pX, &sGrpName, &sNameStyle, &nNameAlign, &tNamePos,
                              &tNameFont, &fNameHasWidth);
        }
        else if (pX->GetName() == _T("abbrev") )
        {
            AnalyzeTextString(pX, &sGrpAbbrev, &sAbbrevStyle, &nAbbrevAlign, &tAbbrevPos,
                              &tAbbrevFont, &fAbbrevHasWidth);
        }
        else if (pX->GetName() == _T("symbol") )
        {
            wxString sSymbol = (pX->GetParameter(1))->GetName();
            if (sSymbol == _T("brace") )
                nGrpSymbol = lm_eBrace;
            else if (sSymbol == _T("bracket") )
                nGrpSymbol = lm_eBracket;
            else
            {
                AnalysisError(pX, _T("Invalid group symbol '%s'. Brace assumed."), sSymbol.c_str());
            }
        }
        else if (pX->GetName() == _T("joinBarlines") )
        {
            fJoinBarlines = GetYesNoValue(pX, fJoinBarlines);
        }
        else
        {
            AnalysisError(pX, _T("[%s]: unknown element '%s' found. Element ignored."),
                _T("group"), pX->GetName().c_str() );
        }
    }

    //create the group relationship
    lmInstrGroup* pGroup = new lmInstrGroup(nGrpSymbol, fJoinBarlines);

    // loop to parse elements <instrument>
    while(iP <= pNode->GetNumParms())
    {
        pX = pNode->GetParameter(iP);
        if ( pX->GetName() == _T("instrument") )
        {
            AnalyzeInstrument105(pX, pScore, nInstr++, pGroup);
        }
        else
        {
            AnalysisError(pNode, _T("Elements <instrument> expected but found element %s. Analysis stopped."),
                pNode->GetName().c_str() );
            break;
        }
        iP++;
        if (iP <= pNode->GetNumParms())
            pX = pNode->GetParameter(iP);
    }

    return nInstr;
}

void lmLDPParser::AnalyzeInstrument105(lmLDPNode* pNode, lmScore* pScore, int nInstr,
                                       lmInstrGroup* pGroup)
{
    //<instrument> = (instrument [<InstrName>][<InfoMIDI>][<Staves>] <Voice>+ )

    //<InstrName> = (instrName name-string [abbreviation-string])
    //<InfoMIDI> = (infoMIDI num-instr [num-channel])
    //<Staves> = (staves {num | overlayered} )
    //<Voice> = (MusicData <music>+ )

    lmLDPNode* pX;
    wxString sData;
    long iP;
    iP = 1;

    wxASSERT( pNode->GetName() == _T("instrument") );

    //default values
	int nMIDIChannel = g_pMidi->DefaultVoiceChannel();
	int nMIDIInstr = g_pMidi->DefaultVoiceInstr();
    bool fMusicFound = false;               // <MusicData> tag found
    wxString sNumStaves = _T("1");          //one staff

    //default values for name
    //TODO user options instead of fixed values
    wxString sInstrName = _T("");           //no name for instrument
    wxString sInstrNameStyle = _T("");
    lmEHAlign nNameAlign = lmHALIGN_LEFT;
    bool fNameHasWidth = false;
    lmFontInfo tNameFont = g_tInstrumentDefaultFont;
    lmLocation tNamePos = g_tDefaultPos;

    //default values for abbreviation
    //TODO user options instead of fixed values
    wxString sInstrAbbrev = _T("");         //no abreviated name for instrument
    wxString sInstrAbbrevStyle = _T("");
    lmEHAlign nAbbrevAlign = lmHALIGN_LEFT;
    bool fAbbrevHasWidth = false;
    lmFontInfo tAbbrevFont = g_tInstrumentDefaultFont;
    lmLocation tAbbrevPos = g_tDefaultPos;

    // parse optional elements until <MusicData> tag found
    for (; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);

        if (pX->GetName() == _T("musicData") ) {
            fMusicFound = true;
            break;      //start of MusicData. Exit this loop
        }
        else if (pX->GetName() == _T("name") ) {
            AnalyzeTextString(pX, &sInstrName, &sInstrNameStyle, &nNameAlign, &tNamePos,
                              &tNameFont, &fNameHasWidth);
        }
        else if (pX->GetName() == _T("abbrev") )
		{
            AnalyzeTextString(pX, &sInstrAbbrev, &sInstrAbbrevStyle, &nAbbrevAlign,
                              &tAbbrevPos, &tAbbrevFont, &fAbbrevHasWidth);
        }
        else if (pX->GetName() == _T("infoMIDI") )
		{
			AnalyzeInfoMIDI(pX, &nMIDIChannel, &nMIDIInstr);
		}
        else if (pX->GetName() == _T("staves") )
		{
            pX = pX->GetParameter(1);
            if (pX->IsSimple()) {
                sNumStaves = pX->GetName();
                if (!sNumStaves.IsNumber()) {
                    AnalysisError(pX, _T("Number of staves expected but found '%s'. Element '%s' ignored."),
                        sNumStaves.c_str(), _T("staves") );
                    sNumStaves = _T("1");
                }
            }
            else {
                AnalysisError(pX, _T("Expected value for %s but found element '%s'. Ignored."),
                    _T("staves"), pX->GetName().c_str() );
           }
        }
        else {
            AnalysisError(pX, _T("[%s]: unknown element '%s' found. Element ignored."),
                _T("instrument"), pX->GetName().c_str() );
        }
    }

    //set number of staves (to be used by AnalyzeMusicData in order to add staves)
    sNumStaves.ToLong(&m_nNumStaves);

    //process firts voice
    if (!fMusicFound) {
        AnalysisError(pX, _T("Expected '%s' but found element %s. Analysis stopped."),
            _T("musicData"), pX->GetName().c_str() );
        return;
    }

    // create the instrument with one empty VStaff
    lmInstrNameAbbrev* pName = (lmInstrNameAbbrev*)NULL;
    lmInstrNameAbbrev* pAbbrev = (lmInstrNameAbbrev*)NULL;
    if (sInstrName != _T(""))
    {
        lmTextStyle* pTS;
        if (sInstrNameStyle != _T(""))
            pTS = pScore->GetStyleInfo(sInstrNameStyle);
        else
            pTS = pScore->GetStyleName(tNameFont);
        wxASSERT(pTS);
        pName = new lmInstrNameAbbrev(sInstrName, pTS);
        //convert position to LUnits. As the text is not yet owned we must use the score
	    if (tNamePos.xUnits == lmTENTHS)
        {
		    tNamePos.x = pScore->TenthsToLogical(tNamePos.x);
            tNamePos.xUnits = lmLUNITS;
        }
	    if (tNamePos.yUnits == lmTENTHS)
        {
		    tNamePos.y = pScore->TenthsToLogical(tNamePos.y);
            tNamePos.yUnits = lmLUNITS;
        }
        pName->SetUserLocation(tNamePos);
    }
    if (sInstrAbbrev != _T(""))
    {
        lmTextStyle* pTS;
        if (sInstrAbbrevStyle != _T(""))
            pTS = pScore->GetStyleInfo(sInstrAbbrevStyle);
        else
            pTS = pScore->GetStyleName(tAbbrevFont);
        wxASSERT(pTS);
        pAbbrev = new lmInstrNameAbbrev(sInstrAbbrev, pTS);
        //convert position to LUnits. As the text is not yet owned we must use the score
	    if (tAbbrevPos.xUnits == lmTENTHS)
        {
		    tAbbrevPos.x = pScore->TenthsToLogical(tAbbrevPos.x);
            tAbbrevPos.xUnits = lmLUNITS;
        }
	    if (tAbbrevPos.yUnits == lmTENTHS)
        {
		    tAbbrevPos.y = pScore->TenthsToLogical(tAbbrevPos.y);
            tAbbrevPos.yUnits = lmLUNITS;
        }
        pAbbrev->SetUserLocation(tAbbrevPos);
    }

    lmInstrument* pInstr = pScore->AddInstrument(nMIDIChannel, nMIDIInstr,
                                        pName, pAbbrev, pGroup);
    lmVStaff* pVStaff = pInstr->GetVStaff();

    // analyce first MusicData
    AnalyzeMusicData(pX, pVStaff);
    iP++;

	//CSG: 19/jan/2008. Support for more than one VStaff removed
    ////analyze other MusicData elements
    //for(; iP <= pNode->GetNumParms(); iP++) {
    //    pX = pNode->GetParameter(iP);
    //    if (pX->GetName() = _T("musicData") ) {
    //        pVStaff = pInstr->AddVStaff(true);      //true -> overlayered
    //        AnalyzeMusicData(pX, pVStaff);
    //    }
    //    else {
    //        AnalysisError(pX, _T("Expected '%s' but found element %s. Element ignored."),
    //            _T("musicData").c_str(), pX->GetName().c_str() );
    //    }
    //}

}

bool lmLDPParser::AnalyzeInfoMIDI(lmLDPNode* pNode, int* pChannel, int* pNumInstr)
{
	//analizes a <infoMIDI> element and updates variables pChannel and pNumInstr
	//returns true if error.
	//
    //		<InfoMIDI> = (infoMIDI num-instr num-channel)

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("infoMIDI") );

    //check that two numbers are specified
    if(pNode->GetNumParms() < 2) {
        AnalysisError(pNode, _T("Element 'infoMIDI' has less parameters than the minimum required. Ignored."));
        return true;
    }

    wxString sNum1 = (pNode->GetParameter(1))->GetName();
    wxString sNum2 = (pNode->GetParameter(2))->GetName();
    if (!sNum1.IsNumber() || !sNum2.IsNumber()) {
        AnalysisError(
            pNode,
            _T("Element 'infoMIDI': Two numbers expected but found '%s' and '%s'. Ignored."),
            sNum1.c_str(), sNum2.c_str() );
        return true;
    }

    long nAux;
    sNum1.ToLong(&nAux);
	*pNumInstr = int(nAux);
    sNum2.ToLong(&nAux);
	*pChannel = int(nAux);

    return false;
}

void lmLDPParser::AnalyzeMusicData(lmLDPNode* pNode, lmVStaff* pVStaff)
{

    wxASSERT(pNode->GetName() == _T("musicData"));

    long iP = 1;
    wxString sName;
    lmLDPNode* pX;

    //the VStaff only contains one staff. So if more thant one staves requested for
    //current instrument we have to add nNumStaves - 1
    if (m_nNumStaves > 1) {
        int i;
        for(i=1; i < m_nNumStaves; i++) {
            pVStaff->AddStaff(5);    //five lines staff, standard size
        }
    }

    //loop to analyze remaining elements: music
    for(; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);
        sName = pX->GetName();
        if (sName == _T("n")) {        // note
            AnalyzeNote(pX, pVStaff);
        } else if (sName == _T("r")) { // rest
            AnalyzeRest(pX, pVStaff);
        } else if (sName == _T("barline")) {
            AnalyzeBarline(pX, pVStaff);
        } else if (sName == _T("chord")) {
            AnalyzeChord(pX, pVStaff);
        } else if (sName == _T("clef")) {
            AnalyzeClef(pVStaff, pX);
        } else if (sName == _T("graphic")) {
            AnalyzeGraphicObj(pX, pVStaff);
        } else if (sName == _T("key")) {
            AnalyzeKeySignature(pX, pVStaff);
        } else if (sName == _T("metronome")) {
            AnalyzeMetronome(pX, pVStaff);
        } else if (sName == _T("newSystem")) {
            AnalyzeNewSystem(pX, pVStaff);
        } else if (sName == _T("spacer")) {
            AnalyzeSpacer(pX, pVStaff);
        } else if (sName == _T("text")) {
            AnalyzeText(pX, pVStaff);
        } else if (sName == _T("textbox")) {
            AnalyzeTextbox(pX, pVStaff);
        } else if (sName == _T("time")) {
            AnalyzeTimeSignature(pVStaff, pX);
        }
        // abbreviated barlines
        else if (sName == _T("|") ) {
            pVStaff->AddBarline(lm_eBarlineSimple, true);
			m_nCurVoice = 1;
        }
        else if (sName == _T("||") ) {
            pVStaff->AddBarline(lm_eBarlineDouble, true);
			m_nCurVoice = 1;
        }
        else if (sName == _T("|]") ) {
            pVStaff->AddBarline(lm_eBarlineEnd, true);
			m_nCurVoice = 1;
        }
        else if (sName == _T("[|") ) {
            pVStaff->AddBarline(lm_eBarlineStart, true);
			m_nCurVoice = 1;
        }
        else if (sName == _T(":|") ) {
            pVStaff->AddBarline(lm_eBarlineEndRepetition, true);
			m_nCurVoice = 1;
        }
        else if (sName == _T("|:") ) {
            pVStaff->AddBarline(lm_eBarlineStartRepetition, true);
			m_nCurVoice = 1;
        }
        else if (sName == _T("::") ) {
            pVStaff->AddBarline(lm_eBarlineDoubleRepetition, true);
			m_nCurVoice = 1;
        }
        // go forward and backward
        else if (sName == _T("goFwd")
                 || sName == _T("goBack") )
        {
            AnalyzeTimeShift(pX, pVStaff);
			if (sName == _T("goBack")) m_nCurVoice++;
		}
        //abbreviated syntax for notes and rests
        else if (sName.Left(1) == _T("n")) {
            AnalyzeNote(pX, pVStaff);
        } else if (sName.Left(1) == _T("r")) {
            AnalyzeRest(pX, pVStaff);
        }
        //error or non-supported elements
        else {
            AnalysisError(pX, _T("[AnalyzeMusicData]: Unknown or not allowed element '%s' found. Element ignored."),
                sName.c_str() );
        }
    }
	m_nCurVoice = 1;
}

void lmLDPParser::AnalyzeSplit(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //TODO   AnalyzeSplit code

}

void lmLDPParser::AnalyzeChord(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    // <chord> = (chord <Note>* )

    wxASSERT(pNode->GetName() == _T("chord"));

    //loop to analyze remaining elements: notes
    long iP;
    wxString sName;
    lmLDPNode* pX;

    for(iP=1; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);
        sName = pX->GetName();
        if (sName == _T("n")) {
            AnalyzeNote(pX, pVStaff, (iP != 1));     //first note is base of chord
        }
        else {
            AnalysisError(pX, _T("[AnalyzeChord]: Expecting notes found element '%s'. Element ignored."),
                sName.c_str() );
        }
    }

}

void lmLDPParser::AnalyzeTimeShift(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //If no error updates the VStaff with the time shift (with sign).
    //As in this method we ignore the time occupied by a measure, if it
    //is a move to start or end of measure, the returned value will be a very high
    //one (lmTIME_SHIFT_START_END) so that when checking measure limits, it gets truncated
    //to those limits and so achieve the desired result.

    // the time shift can be:
    // a) one of the tags 'start' and 'end'
    // b) a number: the amount of 256th notes to go forward or backwards
    // c) an algebraic expression formed by note names, i.e. "3*q+e" meaning three quarter
    //      notes plus an eighth one.

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("goFwd")
             || sElmName == _T("goBack") );

    bool fForward = (sElmName == _T("goFwd"));

    //check that there are parameters
    if(pNode->GetNumParms() < 1) {
        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Element ignored."),
            sElmName.c_str());
        return;
    }

    //get first parameter: time shift amount
    float rShift;
    wxString sValue = (pNode->GetParameter(1))->GetName();
    if (sValue == _T("start")) {
        if (!fForward)
            rShift = lmTIME_SHIFT_START_END;
        else {
            AnalysisError(pNode, _T("Element '%s' has an incoherent value: go forward to start?. Element ignored"),
                sElmName.c_str());
            return;
        }
    }
    else if (sValue == _T("end")) {
        if (fForward)
            rShift = lmTIME_SHIFT_START_END;
        else {
            AnalysisError(pNode, _T("Element '%s' has an incoherent value: go backwards to end?. Element ignored"),
                sElmName.c_str());
            return;
        }
    }
    else {
        //algebraic expression. Analyze it
        if (AnalyzeTimeExpression(sValue, pNode, &rShift)) return;      //error
    }

    //change sign for backwad movement
    if (!fForward) rShift = - rShift;

    //procced to do the time shift
    if (pVStaff->ShiftTime(rShift))
    {
        AnalysisError(pNode, pVStaff->GetErrorMessage() );
    }

}

lmTieInfo* lmLDPParser::AnalyzeTie(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    // <tie> = (tie num [start | stop] [<bezier>])

    //returns a ptr. to a new lmTieInfo struct or NULL if any important error.

    //check that there are parameters
    if (pNode->GetNumParms() < 2 || pNode->GetNumParms() > 3)
    {
        AnalysisError(pNode, _T("Element '%s' has less or more parameters than required. Tag ignored."),
            pNode->GetName().c_str() );
        return (lmTieInfo*)NULL;    //error
    }

    //create the TieInfo struct to save Tie data
    lmTieInfo* pTieInfo = new lmTieInfo;

    //initialize points
    for (int i=0; i < 4; i++)
        pTieInfo->tBezier[i] = lmTPoint(0.0f, 0.0f);

    //get tie number
    wxString sNum = pNode->GetParameter(1)->GetName();
    if (!sNum.IsNumber())
    {
        AnalysisError(pNode,
            _T("Element 'tie': Number expected but found '%s'. Tie ignored."), sNum.c_str() );
        delete pTieInfo;
        return (lmTieInfo*)NULL;    //error;
    }
    sNum.ToLong(&(pTieInfo->nTieId));

    //get tie type: start / end
    wxString sType = pNode->GetParameter(2)->GetName();
    if (!(sType == _T("start") || sType == _T("stop")) )
    {
        AnalysisError(pNode,
            _T("Element 'tie': Type must be 'start' or 'stop' but found '%s'. Tie ignored."), sType.c_str() );
        delete pTieInfo;
        return (lmTieInfo*)NULL;    //error;
    }
    pTieInfo->fStart = (sType == _T("start"));

    //get points values
    if (pNode->GetNumParms() == 3)
    {
        if (pNode->GetParameter(3)->GetName() != _T("bezier"))
            AnalysisError(pNode,
                _T("Element 'tie': Expected 'bezier' element but found '%s'. Parameter ignored."),
                pNode->GetParameter(3)->GetName().c_str() );
        else
            AnalyzeBezier(pNode->GetParameter(3), &(pTieInfo->tBezier[0]));
    }

    //end of analysis
    return pTieInfo;
}

bool lmLDPParser::AnalyzeBezierLocation(lmLDPNode* pNode, lmTPoint* pPoints)
{
    // <bezier-location> = { (start-x num) | (start-y num) | (end-x num) | (end-y num) |
    //                       (ctrol1-x num) | (ctrol1-y num) | (ctrol2-x num) | (ctrol2-y num) }
    // <num> = real number, in tenths

    //Returns true if error.  As result of the analysis, the corresponding value of array of
    //points pPoints is updated.

    //check that there is one parameter and only one
    if (pNode->GetNumParms()!= 1)
    {
        AnalysisError(pNode, _T("Element '%s' has less or more parameters than required. Tag ignored."),
            pNode->GetName().c_str() );
        return true;    //error
    }

    //get point name
    lmTenths* pTarget;
    wxString sName = pNode->GetName();
    if (sName == _T("start-x"))
        pTarget = &((*(pPoints+lmBEZIER_START)).x);
    else if (sName == _T("end-x"))
        pTarget = &((*(pPoints+lmBEZIER_END)).x);
    else if (sName == _T("ctrol1-x"))
        pTarget = &((*(pPoints+lmBEZIER_CTROL1)).x);
    else if (sName == _T("ctrol2-x"))
        pTarget = &((*(pPoints+lmBEZIER_CTROL2)).x);
    else if (sName == _T("start-y"))
        pTarget = &((*(pPoints+lmBEZIER_START)).y);
    else if (sName == _T("end-y"))
        pTarget = &((*(pPoints+lmBEZIER_END)).y);
    else if (sName == _T("ctrol1-y"))
        pTarget = &((*(pPoints+lmBEZIER_CTROL1)).y);
    else if (sName == _T("ctrol2-y"))
        pTarget = &((*(pPoints+lmBEZIER_CTROL2)).y);
    else
    {
        AnalysisError(pNode, _T("Element '%s' unknown. Ignored."), sName.c_str() );
        return true;    //error
    }

    //get point value
    wxString sValue = pNode->GetParameter(1)->GetName();
	double rNumberDouble;
	if (!StrToDouble(sValue, &rNumberDouble))
	{
        *pTarget = (float)rNumberDouble;
    }
    else
    {
        AnalysisError(pNode, _T("Element '%s': Invalid value '%s'. It must be a number with optional units. Zero assumed."),
            sName.c_str(), sValue.c_str() );
        return true;    //error
    }
    return false;       //no error
}

void lmLDPParser::AnalyzeBezier(lmLDPNode* pNode, lmTPoint* pPoints)
{
    // <bezier> = (bezier [bezier-location]* )

    //returns, in variable pPoints[4] the result of the analysis. If a point is not specified
    //value (0, 0) is assigned.

    //check that there are parameters
    if (pNode->GetNumParms() < 1 || pNode->GetNumParms() > 8)
    {
        AnalysisError(pNode, _T("Element '%s' has less or more parameters than required. Tag ignored."),
            pNode->GetName().c_str() );
        return;
    }

    //get points values
    for (int iP = 1; iP <= pNode->GetNumParms(); iP++)
    {
        AnalyzeBezierLocation(pNode->GetParameter(iP), pPoints);
    }
}

bool lmLDPParser::AnalyzeTimeExpression(const wxString& sData, lmLDPNode* pNode, float* pValue)
{
    // receives an algebraic expression, formed by aditions and substractions of
    // notes and numbers; parenthesis are not allowed. i.e: 3q+e+256 (three quarter
    // notes + one eighth note + one 256th note = 3*(256/4)+(256/8)+256 =
    // Analyzes this expresion and returns its numeric value

    float rTotal = 0.0;
    float rNum = 0.0;       //token (number) being formed
    wxString sChar;         //token (char) in process
    float rValue;           //value of the letter
    bool fAddUp = true;

    //loop. take a token, analyze it and if operator, perform operation
    int i = 1;
    bool fNumberReady = false;              //number ready to be processed
    for (i=0; i < (int)sData.length(); i++)
    {
        sChar = sData.GetChar(i);

        if (sChar.IsNumber()) {
            //Number: store in rNum until all digits read
            long nNumber;
            sChar.ToLong(&nNumber);
            rNum = 10.0 * rNum + (float)nNumber;
            fNumberReady = true;
        }
        else if (sChar == _T("+") || sChar == _T("-"))
        {
            //operator + or -. Do stored (fAddUp) operation and save this new one
            if (fNumberReady) {
                rTotal += (fAddUp ? rNum : - rNum);
                fNumberReady = false;
            }
            //save new operation
            fAddUp = (sChar == _T("+"));
        }
        else {
            //letter: compute its value. Multiply by rNum and do addition or substraction
            int nDots;
            lmENoteType nNoteType;
            if (AnalyzeNoteType(sChar, &nNoteType, &nDots)) {
                AnalysisError(pNode, _T("Time shift: Letter %s is not a valid note duration. Replaced by a quarter note"), sChar.c_str());
                rValue = (float)eQuarter;
            }
            else {
                rValue = NoteTypeToDuration(nNoteType, nDots);
            }

            if (!fNumberReady) rNum = 1;       //no había multiplicador
            rTotal += (fAddUp ? (rValue * rNum) : - (rValue * rNum));
            fNumberReady = false;

            //clear used data
            rNum = 0.0;
            fAddUp = true;
        }
    }

    if (fNumberReady) {
        rTotal += (fAddUp ? rNum : - rNum);
        fNumberReady = false;
    }

    *pValue = rTotal;

    return false;       //no error

}

void lmLDPParser::AddTie(lmNote* pNote, lmTieInfo* pTieInfo)
{
    //Received a note with a tie of type 'stop' and the lmTieInfo for the stop tie element.
    //This method must look for the matching start element and, if found, build the tie

    //look for the matching start element
    std::list<lmTieInfo*>::iterator itT;
    for(itT=m_PendingTies.begin(); itT != m_PendingTies.end(); ++itT)
    {
         if ((*itT)->nTieId == pTieInfo->nTieId)
             break;     //found
    }
    if (itT == m_PendingTies.end())
    {
        AnalysisError((lmLDPNode*)NULL, _T("No 'start' element for tie num. %d. Tie ignored."),
                      pTieInfo->nTieId );
        delete pTieInfo;
        return;
    }

    //element found. verify that it is of type 'start'
    if (!(*itT)->fStart)
    {
        AnalysisError((lmLDPNode*)NULL, _T("Duplicated 'stop' element for tie num. %d. Tie ignored."),
                      pTieInfo->nTieId );
        delete pTieInfo;
        delete *itT;
        m_PendingTies.erase(itT);
        return;
    }

    //verify that both notes have the same pitch
    if((*itT)->pNote->GetFPitch() != pNote->GetFPitch())
    {
        AnalysisError((lmLDPNode*)NULL, _T("Requesting to tie notes of different pitch. Tie %d ignored."),
                      pTieInfo->nTieId );
        delete pTieInfo;
        delete *itT;
        m_PendingTies.erase(itT);
        return;
    }

    //create the tie
    (*itT)->pNote->CreateTie(pNote, (*itT)->tBezier, pTieInfo->tBezier);

    //remove and delete consumed lmTieInfo elements
    delete pTieInfo;
    delete *itT;
    m_PendingTies.erase(itT);
}

lmNote* lmLDPParser::AnalyzeNote(lmLDPNode* pNode, lmVStaff* pVStaff, bool fChord)
{
    return (lmNote*)AnalyzeNoteRest(pNode, pVStaff, fChord);
}

lmRest* lmLDPParser::AnalyzeRest(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    return (lmRest*)AnalyzeNoteRest(pNode, pVStaff);
}

lmNoteRest* lmLDPParser::AnalyzeNoteRest(lmLDPNode* pNode, lmVStaff* pVStaff, bool fChord)
{
    //Notes and rests have a very similar structure so they share a lot of analysis code.
    //This method analyses the source of Notes and Rests and with its information builds
    //the corresponding lmNote or lmRest object and appends
    //it to the lmVStaff received as parameter.
    //Returns a pointer to the lmNoteRest created.

    // <Note> = (n <Pitch> <NoteType> [<NoteFlags>*] )
    // <NoteFlags> = {L | <RestFlags> }
    //
    // <Rest> = (s <NoteType> [<RestFlags>]*)
    // <RestFlags> = {C | AMR | G | P}


    wxString sElmName = pNode->GetName();       //for error messages
    wxASSERT(sElmName.Left(1) == _T("n") ||
             sElmName.Left(1) == _T("r") ||
             sElmName == _T("na") );

    bool fIsRest = (sElmName.Left(1) == _T("r"));   //analysing a rest

    lmEStemType nStem = lmSTEM_DEFAULT;
    bool fBeamed = false;
    bool fVisible = true;
    lmTBeamInfo BeamInfo[6];
    for (int i=0; i < 6; i++)
    {
        BeamInfo[i].Repeat = false;
        BeamInfo[i].Type = eBeamNone;
    }

    //tie
    lmTieInfo* pTieInfo = (lmTieInfo*)NULL;
    bool fTie = false;

    //Tuplet brakets
    bool fEndTuplet = false;
    //int nTupletNumber = 0;      // 0 = no tuplet
    int nActualNotes = 0;       // 0 = no tuplet
    int nNormalNotes = 0;

    //default values
    int nDots = 0;
    lmENoteType nNoteType = eQuarter;
    float rDuration = GetDefaultDuration(nNoteType, nDots, nActualNotes, nNormalNotes);
    wxString sStep = _T("c");
    wxString sOctave = _T("4");
    lmEAccidentals nAccidentals = lm_eNoAccidentals;
    lmEPitchType nPitchType = lm_ePitchRelative;

    bool fInChord = !fIsRest && ((sElmName == _T("na")) || fChord );
    long nParms = pNode->GetNumParms();

    //get parameters for pitch and duration
    int iP = 1;
    wxString sPitch = _T("");
    wxString sDuration = _T("");
    if (sElmName != _T("na") && sElmName.length() > 1)
    {
        //abbreviated notation. Split node name
        bool fPitchFound = false;
        bool fOctaveFound = false;
        int i;
        wxChar sChar;
        for (i=1; i < (int)sElmName.length(); i++)
        {
            sChar = sElmName.GetChar(i);
            if (sChar == _T('-') ||
                sChar == _T('+') ||
                sChar == _T('=') ||
                sChar == _T('x') )
            {
                //accidental
                sPitch += sChar;
            }
            else if ( (sElmName.Mid(i, 1)).IsNumber()) {
                //octave
                fOctaveFound = true;
                sPitch += sChar;
                i++;
                break;
            }
            else {
                if (fPitchFound) {
                    //octave not present. This is Duration
                    break;
                }
                else {
                    //note step name
                    sPitch += sChar;
                    fPitchFound = true;
                }
            }
        }

        //remaining string is Duration
        if (i >= (int)sElmName.length()) {
            //Duration not included. Inherit it
            sDuration = m_sLastDuration;
        }
        else
            sDuration = sElmName.substr(i);

        if (fIsRest)
        {
           // for rests, first parameter is duration
            sDuration = sPitch;
        }

        // inherit octave if not found
        if (!fOctaveFound) sPitch += m_sLastOctave;

        iP = 1;
    }

    else    //full notation. Get parameters
    {
        if (fIsRest) {
            if (nParms < 1) {
                AnalysisError(pNode, _T("Missing parameters in rest '%s'. Replaced by '(r n)'."),
                    pNode->ToString().c_str() );
				m_pLastNoteRest = pVStaff->AddRest(nNoteType, rDuration, nDots,
										m_nCurStaff, m_nCurVoice, fVisible);
                return m_pLastNoteRest;
            }
        }
        else
		{
            if (nParms < 2)
			{
                AnalysisError(pNode, _T("Missing parameters in note '%s'. Assumed (n c4 n)."),
                    pNode->ToString().c_str() );
                lmNote* pNt = pVStaff->AddNote(lm_ePitchRelative, 0, 4, 0, nAccidentals,
											   nNoteType, rDuration, nDots, m_nCurStaff,
											   m_nCurVoice, fVisible, fBeamed, BeamInfo,
											   (lmNote*)NULL, fTie, nStem);
				m_pLastNoteRest = pNt;
                return pNt;
            }
        }

        //get pitch and duration parameters
        if (fIsRest) {
           // for rests, first parameter is duration
            sDuration = (pNode->GetParameter(1))->GetName();
            sPitch = wxEmptyString;
            iP = 2;
        }
        else {
             //for notes: first one is pitch and accidentals, second one duration
            sPitch = (pNode->GetParameter(1))->GetName();
            sDuration = (pNode->GetParameter(2))->GetName();
            iP = 3;
        }
    }

    //for notes: analyse pitch and accidentals
    if (!fIsRest) {
        if (sPitch.IsNumber()) {
            //if sPitch is a number it represents a MIDI pitch in C major key signature.
            long nMidi = 0;
            sPitch.ToLong(&nMidi);
            sPitch = MPitch_ToLDPName((lmMPitch)nMidi);
        }
        if (sPitch == _T("*")) {
            nPitchType = lm_ePitchNotDefined;
        }
        else {
            if (LDPDataToPitch(sPitch, &nAccidentals, &sStep, &sOctave)) {
                AnalysisError(pNode, _T("Unknown note pitch '%s'. Assumed 'c4'."),
                    sPitch.c_str() );
            }
        }
    }

    //analyze duration and dots
    if (AnalyzeNoteType(sDuration, &nNoteType, &nDots)) {
        AnalysisError(pNode, _T("Unknown note/rest duration '%s'. A quarter note assumed."),
            sDuration.c_str() );
    }
    m_sLastDuration = sDuration;

    //analyze optional parameters
	lmLDPOptionalTags oOptTags(this);
	oOptTags.SetValid(lm_eTag_Visible, lm_eTag_Location_x, lm_eTag_Location_y,
						lm_eTag_StaffNum, -1);		//finish list with -1

	lmLocation tPos = g_tDefaultPos;
    int nStaff = m_nCurStaff;

	oOptTags.AnalyzeCommonOptions(pNode, iP, pVStaff, &fVisible, &nStaff, &tPos);
	m_nCurStaff = nStaff;

    //analyze remaining parameters: annotations
    bool fFermata = false;
    lmEPlacement nFermataPlacement = ep_Default;
	lmLocation tFermataPos = g_tDefaultPos;

    wxString sData;
    int iLevel, nLevel;
    lmLDPNode* pX = pNode->StartIterator(iP);
    while (pX)
    {
        if (pX->IsProcessed())
            ;   //ignore it
        else if (pX->IsSimple())
        {
            // Analysis of simple notations

            sData = pX->GetName();
            if (sData == _T("l") && !fIsRest)       //Tied to the next one
            {
                //AWARE: This simple notation is needed for exercise patterns. Can not be removed!
                fTie = true;
            }
            else if (sData.Left(1) == _T("g"))      //beamed group
            {
                if (sData.substr(1,1) == _T("+")) {       //Start of beamed group. Simple parameter
                    //compute beaming level dependig on note type
                    nLevel = GetBeamingLevel(nNoteType);
                    if (nLevel == -1) {
                        AnalysisError(pNode,
                            _T("Requesting beaming a note longer than eight. Beaming ignored."));
                    }
                    else {
                        // and the previous note must be beamed
                        if (m_pLastNoteRest && m_pLastNoteRest->IsBeamed() &&
                            m_pLastNoteRest->GetBeamType(0) != eBeamEnd) {
                            AnalysisError(pNode,
                                _T("Requesting to start a beamed group but there is already an open group. Beaming ignored."));
                        }
                        fBeamed = true;
                        for (iLevel=0; iLevel <= nLevel; iLevel++) {
                            BeamInfo[iLevel].Type = eBeamBegin;
                            //wxLogMessage(wxString::Format(
                            //    _T("[lmLDPParser::AnalyzeNote] BeamInfo[%d] = eBeamBegin"), iLevel));
                        }
                    }
                }

                else if (sData.substr(1,1) == _T("-")) {       //End of beamed group
                    //allow to close the beamed group
                    bool fCloseBeam = true;

                    //TODO   Beaming information only allowed in base note of chords
                    //!         This program should move this information to base note
                    //!         as this restriction is un-coherent with forcing the t- flag
                    //!         to be in the last note of the chord.
                    if (fInChord) {
                        AnalysisError(pNode,
                            _T("Request to end beaming a group in a note that is note the first one of a chord. Beaming ignored."));
                        fCloseBeam = false;
                    }

                    //There must exist a previous note/rest
                    if (!m_pLastNoteRest) {
                        AnalysisError(pNode,
                            _T("Request to end beaming a group but there is not a  previous note. Beaming ignored."));
                        fCloseBeam = false;
                    }
                    else {
                        // and the previous note must be beamed
                        if (!m_pLastNoteRest->IsBeamed() ||
                            m_pLastNoteRest->GetBeamType(0) == eBeamEnd) {
                            AnalysisError(pNode,
                                _T("Request to end beaming a group but previous note is not beamed. Beaming ignored."));
                            fCloseBeam = false;
                        }
                    }

                    //proceed to close all previous open levels
                    if (fCloseBeam) {
                        fBeamed = true;
                        int nCurLevel = GetBeamingLevel(nNoteType);
                        int nPrevLevel = GetBeamingLevel(m_pLastNoteRest->GetNoteType());

                        // close commom levels (as this must be done in each if/else branch it has
                        // been moved here to optimize. A commnet has been included there instead to
                        // facilitate the understanding of the algorithm)
                        for (iLevel=0; iLevel <= wxMin(nCurLevel, nPrevLevel); iLevel++) {
                            BeamInfo[iLevel].Type = eBeamEnd;
                            g_pLogger->LogTrace(_T("LDPParser_beams"),
                                _T("[lmLDPParser::AnalyzeNoteRest] BeamInfo[%d] = eBeamEnd"), iLevel);
                        }

                        // deal with differences between current note level and previous note level
                        if (nCurLevel > nPrevLevel) {
                            // current level is grater than previous one ==>
                            // close common levels (done) and put backward in current deeper levels
                            for (; iLevel <= nCurLevel; iLevel++) {
                                BeamInfo[iLevel].Type = eBeamBackward;
                                g_pLogger->LogTrace(_T("LDPParser_beams"),
                                    _T("[lmLDPParser::AnalyzeNoteRest] BeamInfo[%d] = eBeamBackward"), iLevel);
                            }
                        }
                        else if  (nCurLevel < nPrevLevel) {
                            // current level is lower than previous one:
                            // close common levels (done) and close deeper levels in previous note
                            for (; iLevel <= nPrevLevel; iLevel++) {
                                if (m_pLastNoteRest->GetBeamType(iLevel) == eBeamContinue) {
                                    m_pLastNoteRest->SetBeamType(iLevel, eBeamEnd);
                                    g_pLogger->LogTrace(_T("LDPParser_beams"),
                                        _T("[lmLDPParser::AnalyzeNoteRest] Changing previous BeamInfo[%d] = eBeamEnd"), iLevel);
                                }
                                else if (m_pLastNoteRest->GetBeamType(iLevel) == eBeamBegin) {
                                    m_pLastNoteRest->SetBeamType(iLevel, eBeamForward);
                                    g_pLogger->LogTrace(_T("LDPParser_beams"),
                                        _T("[lmLDPParser::AnalyzeNoteRest] Changing previous BeamInfo[%d] = eBeamForward"), iLevel);
                                }
                            }
                        }
                        else {
                            // current level is equal than previous one:
                            // close common levels (done)
                        }
                    }
                }

                else {
                    AnalysisError(pNode, _T("Error: notation '%s' unknown. It will be ignored."), sData.c_str() );
                }

            }

            else if (sData.Left(1) == _T("t"))
            {
                //start/end of tuplet. Simple parameter (tn / t-)

                lmTupletBracket* pTuplet;
                bool fOpenTuplet = (m_pTuplet ? false : true);
                if (!AnalyzeTuplet(pX, sElmName, fOpenTuplet, !fOpenTuplet,
                     &pTuplet, &nActualNotes, &nNormalNotes))
                {
                    if (pTuplet) {
                        // start of tuplet
                        m_pTuplet = pTuplet;
                    }
                    else {
                        //end of tuplet
                        fEndTuplet = true;
                    }
                }
            }

            else if (sData.Left(1) == _T("v"))      //voice
            {
				m_nCurVoice = AnalyzeVoiceNumber(sData, pNode);
			}
            else if (sData == _T("fermata"))        //fermata
			{
                fFermata = true;
            }
            else {
                AnalysisError(pNode, _T("Error: notation '%s' unknown. It will be ignored."), sData.c_str() );
            }

       }

       else     // Analysis of compound notations
       {
            sData = pX->GetName();
            if (sData == _T("g"))       //Start of group element
            {
                AnalysisError(pX, _T("Notation '%s' unknown or not implemented. Old (g + t3) syntax?"), sData.c_str());
            }
            else if (sData == _T("stem"))       //stem attributes
            {
                nStem = AnalyzeStem(pX, pVStaff);
            }
            else if (sData == _T("fermata"))        //fermata attributes
            {
                fFermata = true;
                nFermataPlacement = AnalyzeFermata(pX, pVStaff, &tFermataPos);
            }
            else if (sData == _T("t")) {       //start/end of tuplet. Simple parameter (tn / t-)
                lmTupletBracket* pTuplet;
                bool fOpenTuplet = (m_pTuplet ? false : true);
                if (!AnalyzeTuplet(pX, sElmName, fOpenTuplet, !fOpenTuplet,
                     &pTuplet, &nActualNotes, &nNormalNotes))
                {
                    if (pTuplet) {   // start of tuplet
                        m_pTuplet = pTuplet;
                    }
                    else {          //end of tuplet
                        fEndTuplet = true;
                    }
                }
            }
            else if (sData == _T("tie"))     //start/end of tie
            {
                if (fIsRest)
                    AnalysisError(pX, _T("Rests can not be tied. Tie ignored."), sData.c_str());
                else
                    pTieInfo = AnalyzeTie(pX, pVStaff);
            }
            else {
                AnalysisError(pX, _T("Notation '%s' unknown or not implemented."), sData.c_str());
            }

        }
        pX = pNode->GetNextParameter();
    }

    //force beaming for notes between eBeamBegin and eBeamEnd (only for single notes
    //and chord base notes, not for secondary notes of a chord)
    if (!fBeamed && !fInChord && nNoteType > eQuarter)
    {
        if (m_pLastNoteRest)
        {
            if (m_pLastNoteRest->IsBeamed())
            {
                //it can be the end of a group. Let's verify that at least a beam is open
                for (iLevel=0; iLevel <= 6; iLevel++)
                {
                    if ((m_pLastNoteRest->GetBeamType(iLevel) == eBeamBegin)
                         || (m_pLastNoteRest->GetBeamType(iLevel) == eBeamContinue))
                    {
                            fBeamed = true;
                            break;
                    }
                }

                if (fBeamed)
                {
                    int nCurLevel = GetBeamingLevel(nNoteType);
                    int nPrevLevel = GetBeamingLevel(m_pLastNoteRest->GetNoteType());

                    // continue common levels
                    for (iLevel=0; iLevel <= wxMin(nCurLevel, nPrevLevel); iLevel++)
                    {
                        BeamInfo[iLevel].Type = eBeamContinue;
                        g_pLogger->LogTrace(_T("LDPParser_beams"),
                            _T("[lmLDPParser::AnalyzeNoteRest] BeamInfo[%d] = eBeamContinue"), iLevel);
                    }

                    if (nCurLevel > nPrevLevel)
                    {
                        // current level is grater than previous one, start new beams
                        for (; iLevel <= nCurLevel; iLevel++) {
                            BeamInfo[iLevel].Type = eBeamBegin;
                            g_pLogger->LogTrace(_T("LDPParser_beams"),
                                _T("[lmLDPParser::AnalyzeNoteRest] BeamInfo[%d] = eBeamBegin"), iLevel);
                        }
                    }
                    else if  (nCurLevel < nPrevLevel)
                    {
                        // current level is lower than previous one
                        // close common levels (done) and close deeper levels in previous note
                        for (; iLevel <= nPrevLevel; iLevel++)
                        {
                            if (m_pLastNoteRest->GetBeamType(iLevel) == eBeamContinue)
                            {
                                m_pLastNoteRest->SetBeamType(iLevel, eBeamEnd);
                                g_pLogger->LogTrace(_T("LDPParser_beams"),
                                    _T("[lmLDPParser::AnalyzeNoteRest] Changing previous BeamInfo[%d] = eBeamEnd"), iLevel);
                            }
                            else if (m_pLastNoteRest->GetBeamType(iLevel) == eBeamBegin)
                            {
                                m_pLastNoteRest->SetBeamType(iLevel, eBeamForward);
                                g_pLogger->LogTrace(_T("LDPParser_beams"),
                                    _T("[lmLDPParser::AnalyzeNoteRest] Changing previous BeamInfo[%d] = eBeamFordward"), iLevel);
                            }
                        }
                    }
                }
            }
        }
    }

    //if not first note of tuple, tuple information is not present and need to be taken from
    //previous note
    if (m_pTuplet)
    {
        // a tuplet is open
        nActualNotes = m_pTuplet->GetActualNotes();
        nNormalNotes = m_pTuplet->GetNormalNotes();
    }

    // calculation of duration
    rDuration = GetDefaultDuration(nNoteType, nDots, nActualNotes, nNormalNotes);

    //Verify if note in chord has the same duration than base note
    if (fInChord && m_pLastNoteRest && m_pLastNoteRest->IsNote()
        && !IsEqualTime(m_pLastNoteRest->GetDuration(), rDuration) )
    {
        AnalysisError(pNode, _T("Error: note in chord has different duration than base note. Duration changed."));
		rDuration = m_pLastNoteRest->GetDuration();
        nNoteType = m_pLastNoteRest->GetNoteType();
        nDots = m_pLastNoteRest->GetNumDots();
    }

    //create the note/rest
    lmNoteRest* pNR;
    if (fIsRest)
	{
        pNR = pVStaff->AddRest(nNoteType, rDuration, nDots,
                               m_nCurStaff, m_nCurVoice, fVisible, fBeamed, BeamInfo);
		m_pLastNoteRest = pNR;
    }
    else
	{
        //TODO: Convert earlier to int
        int nStep = LetterToStep(sStep);
        long nAux;
        sOctave.ToLong(&nAux);
        int nOctave = (int)nAux;

        lmNote* pNt = pVStaff->AddNote(nPitchType,
                               nStep, nOctave, 0, nAccidentals,
                               nNoteType, rDuration, nDots, m_nCurStaff,
                               m_nCurVoice, fVisible, fBeamed, BeamInfo,
							   (fInChord ? (lmNote*)m_pLastNoteRest : (lmNote*)NULL),
							   fTie, nStem);
		if (!fInChord || pNt->IsBaseOfChord())
			m_pLastNoteRest = pNt;

		pNR = pNt;
        m_sLastOctave = sOctave;

        //add tie, if exists
        if (pTieInfo)
        {
            if (pTieInfo->fStart)
            {
                //start of tie. Save the information
                pTieInfo->pNote = pNt;
                m_PendingTies.push_back(pTieInfo);
            }
            else
            {
                //end of tie. Add it to the internal model
                AddTie(pNt, pTieInfo);
            }
        }
    }
	pNR->SetUserLocation(tPos);

    // Add notations
    if (m_pTuplet) {
        m_pTuplet->Include(pNR);             // add this note/rest to the tuplet

        if (fEndTuplet) {
            m_pTuplet = (lmTupletBracket*)NULL;
        }
    }

	if (fFermata) {
		lmFermata* pFermata = pNR->AddFermata(nFermataPlacement);
		pFermata->SetUserLocation(tFermataPos);
	}

    return pNR;

}

bool lmLDPParser::AnalyzeTuplet(lmLDPNode* pNode, const wxString& sParent,
                                bool fOpenAllowed, bool fCloseAllowed,
                                lmTupletBracket** pTuplet, int* pActual, int* pNormal)
{
    // sParent: name of parent element
    // Returns true if errors. The elemenent is ignored.
    // If no errors, updates pTuplet, pActual and pNormal with the result of parsing:
    //      - start of tuplet: pTuple points to new lmTupletBracket
    //      - end of tuplet: pTuplet is NULL


    // <Tuplet> = (t {- | + ActualNotes [NormalNotes][options] } )
    // Abbreviations:
    //      (t -)     --> t-
    //      (t + n)   --> tn
    //      (t + n m) --> tn/m

    bool fEndTuplet = false;
    int nActualNum, nNormalNum;

    bool fShowTupletBracket = m_fShowTupletBracket;
    bool fShowNumber = m_fShowNumber;
	lmEPlacement nTupletAbove = ep_Default;

    wxString sData = pNode->GetName();

    if (pNode->IsSimple()) {
        //start/end of tuplet. Simple parameter (t- | tn | tn/m )
        wxASSERT(sData.Left(1) == _T("t"));
        if (sData == _T("t-")) {
            //end of tuplet
            fEndTuplet = true;
        }
        else {
            //start of tuplet
            wxString sNumTuplet = sData.substr(1);
            int nSlash = sNumTuplet.Find(_T("/"));
            if (nSlash == 0) {
                //error: invalid element 't/num'
                AnalysisError(pNode, _T("[%s] Found unknown tag '%s'. Ignored."),
                    sParent.c_str(), sData.c_str());
                return true;
            }
            else if (nSlash == -1) {
                //abbreviated tuplet: 'tn'
                if (!sNumTuplet.IsNumber()) {
                    AnalysisError(pNode, _T("[%s] Found unknown tag '%s'. Ignored."),
                        sParent.c_str(), sData.c_str());
                    return true;
                }
                else {
                    long nNum;
                    sNumTuplet.ToLong(&nNum);
                    nActualNum = (int)nNum;
                    //implicit value for denominator
                    if (nActualNum == 2)
                        nNormalNum = 3;   //duplet
                    else if (nActualNum == 3)
                        nNormalNum = 2;   //triplet
                    else if (nActualNum == 4)
                        nNormalNum = 6;
                    else if (nActualNum == 5)
                        nNormalNum = 6;
                    else {
                        AnalysisError(pNode, _T("[%s] Found tag '%s' but no default value exists for NormalNotes. Ignored."),
                            sParent.c_str(), sData.c_str());
                        return true;
                    }
                }
            }
            else {
                //abbreviated tuplet: 'tn:m'. Split the two numbers
                wxString sActualNum = sNumTuplet.Left(nSlash);
                wxString sNormalNum = sNumTuplet.substr(nSlash+1);

                if (!sActualNum.IsNumber() || !sNormalNum.IsNumber() ) {
                    AnalysisError(pNode, _T("[%s] Found unknown tag '%s'. Ignored."),
                        sParent.c_str(), sData.c_str());
                    return true;
                }
                else {
                    long nNum;
                    sActualNum.ToLong(&nNum);
                    nActualNum = (int)nNum;
                    sNormalNum.ToLong(&nNum);
                    nNormalNum = (int)nNum;
                    if (nNormalNum < 1 || nActualNum < 1) {
                        AnalysisError(pNode, _T("[%s] Tag '%s'. Numbers must be greater than 0. Tag ignored."),
                            sParent.c_str(), sData.c_str());
                        return true;
                    }
                }
            }
        }
    }

    else {
        //compound element

        wxString sElmName = pNode->GetName();

        //check that at least one parameters (+, - sign) is specified
        if(pNode->GetNumParms() < 2) {
            AnalysisError(
                pNode,
                _T("Element '%s' has less parameters than the minimum required. Element ignored."),
                sElmName.c_str() );
            return true;
        }

        // get type: + or -
        wxString sType = (pNode->GetParameter(1))->GetName();
        if (sType ==_T("+") ) {             //start of tuplet
            fEndTuplet = false;
        } else if (sType ==_T("-") ) {      //end of tuplet
            fEndTuplet = true;
        } else {
            AnalysisError(pNode, _T("Element '%s': invalid type '%s'. It is neither '+' nor '-'. Tuplet ignored."),
                sElmName.c_str(), sType.c_str() );
            return true;    //error
        }

        // get actual notes number
        wxString sNumTuplet = (pNode->GetParameter(2))->GetName();
        if (!sNumTuplet.IsNumber()) {
            AnalysisError(pNode, _T("Element '%s': Expected number but found '%s'. Tuplet ignored."),
                sElmName.c_str(), sData.c_str());
            return true;
        }
        else {
            long nNum;
            sNumTuplet.ToLong(&nNum);
            nActualNum = (int)nNum;
            //implicit value for denominator
            if (nActualNum == 2)
                nNormalNum = 3;   //duplet
            else if (nActualNum == 3)
                nNormalNum = 2;   //triplet
            else if (nActualNum == 4)
                nNormalNum = 6;
            else if (nActualNum == 5)
                nNormalNum = 6;
            else
                nNormalNum = 0;  //required
        }

        // loop to parse remaining parameters: NormalNum and Options
        long iP = 3;
        wxString sData;
        for(; iP <= pNode->GetNumParms(); iP++) {
            sData = (pNode->GetParameter(iP))->GetName();
            if (fEndTuplet) {
                AnalysisError(pNode, _T("Element '%s': Found unknown data '%s'. Data ignored."),
                    sElmName.c_str(), sData.c_str());
            }
            else {
                //check if Normal notes number
                if (sData.IsNumber()) {
                    long nNum;
                    sData.ToLong(&nNum);
                    nNormalNum = (int)nNum;
                    if (nNormalNum < 1) {
                        AnalysisError(pNode, _T("Element '%s': Number for 'normal notes' must be greater than 0. Number ignored."),
                            sElmName.c_str(), sData.c_str());
                        return true;
                    }
                }
                else if (sData == _T("noBracket")) {
                    fShowTupletBracket = false;
                }
                else if (sData == _T("squaredBracket")) {
                    //TODO implement different kinds of brackets
                    fShowTupletBracket = true;
                }
                else if (sData == _T("curvedBracket")) {
                    fShowTupletBracket = true;
                }
                else if (sData == _T("numNone")) {
                    fShowNumber = false;
                }
                else if (sData == _T("numActual")) {
                    //TODO implement different options to display numbers
                    fShowNumber = true;
                }
                else if (sData == _T("numBoth")) {
                    fShowNumber = true;
                }
                else {
                    AnalysisError(pNode, _T("Element '%s': Found unknown data '%s'. Data ignored."),
                        sElmName.c_str(), sData.c_str());
                }
           }
        }

    }

    //All information parsed. Prepare return info
    if (fEndTuplet) {
        if (!fCloseAllowed) {
            // there isn't an open tuplet
            AnalysisError(pNode, _T("[%s] Requesting to end a tuplet but there is not an open tuplet or it is not possible to close it here. Tag '%s' ignored."),
                sParent.c_str(), sData.c_str());
            return true;
        }
        *pTuplet = (lmTupletBracket*) NULL;
    }
    else {
        if (!fOpenAllowed) {
            //there is already a tuplet open and not closed
            AnalysisError(pNode, _T("[%s] Requesting to start a tuplet but there is already a tuplet open. Tag '%s' ignored."),
                sParent.c_str(), sData.c_str());
            return true;
        }

        //save new options
        m_fShowTupletBracket = fShowTupletBracket;
        m_fShowNumber = fShowNumber;

        // create tuplet braket
        *pTuplet = new lmTupletBracket(fShowNumber, nActualNum, fShowTupletBracket,
                            nTupletAbove, nActualNum, nNormalNum);
        *pActual = nActualNum;
        *pNormal = nNormalNum;
    }

    return false;

}



////Devuelve las variables nMIDIChannel y nInstr actualizadas con los valores leidos. Si alguno
////de los valores no viene se ponen los valores por defecto
//void lmLDPParser::AnalizarInfoMIDI(lmLDPNode* pNode, ByRef nMIDIChannel As Integer, ByRef nInstr As Integer)
//
//    wxASSERT(pNode->GetName() = "INFOMIDI"
//    wxASSERT(pNode->GetNumParms() >= 1
//
//    Dim long i
//    Dim sTipo As String
//    Dim nValor As Integer
//    Dim wxString sData
//
//    for (i = 1 To pNode->GetNumParms()
//        sData = pNode->GetParameter(i).GetName();
//        sTipo = UCase$(Mid$(sData, 1, 1))
//        nValor = CInt(Mid$(sData, 2))
//        switch (sTipo
//            case "I"
//                if (nValor < 1 Or nValor > 128) {
//                    AnalysisError(wxString::Format(_T("Instrumento MIDI <" & sData & "> no está entre 1 y 128. " & _
//                        "Se ignora."
//                } else {
//                    nInstr = nValor - 1
//                }
//            case "C"
//                if (nValor < 1 Or nValor > 128) {
//                    AnalysisError(wxString::Format(_T("Canal MIDI <" & sData & "> no está entre 1 y 16. " & _
//                        "Se ignora."
//                } else {
//                    nMIDIChannel = nValor - 1
//                }
//            default:
//                AnalysisError(wxString::Format(_T("DatoMIDI <" & sData & "> desconocido. " & _
//                    "Se ignora."
//        }
//    }   // i
//
//}
//
bool lmLDPParser::AnalyzeBarline(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //returns true if error; in this case nothing is added to the lmVStaff

    // <Barline> = (barline <BarType> [<Visible>][<location>])
    // <BarType> = {"InicioRepeticion" | "FinRepeticion" | "Final" | "Doble" | "Simple" }

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("barline"));

    //check that bar type is specified
    if(pNode->GetNumParms() < 1) {
        //assume simple barline, visible
        pVStaff->AddBarline(lm_eBarlineSimple, true);
		m_nCurVoice = 1;
        return false;
    }

    lmEBarline nType = lm_eBarlineSimple;

    wxString sType = (pNode->GetParameter(1))->GetName();
    if (sType == _T("endRepetition")) {
        nType = lm_eBarlineEndRepetition;
    } else if (sType == _T("startRepetition")) {
        nType = lm_eBarlineStartRepetition;
    } else if (sType == _T("end")) {
        nType = lm_eBarlineEnd;
    } else if (sType == _T("double")) {
        nType = lm_eBarlineDouble;
    } else if (sType == _T("simple")) {
        nType = lm_eBarlineSimple;
    } else if (sType == _T("start")) {
        nType = lm_eBarlineStart;
    } else if (sType == _T("doubleRepetition")) {
        nType = lm_eBarlineDoubleRepetition;
    } else {
        AnalysisError(pNode, _T("Unknown barline type '%s'. 'simple' barline assumed."),
            sType.c_str() );
    }

    //analyze remaining optional parameters
    int iP = 2;
	lmLDPOptionalTags oOptTags(this);
	oOptTags.SetValid(lm_eTag_Visible, lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1
	lmLocation tPos = g_tDefaultPos;
    bool fVisible = true;
	oOptTags.AnalyzeCommonOptions(pNode, iP, pVStaff, &fVisible, NULL, &tPos);

	//create the tiem signature
    lmBarline* pBarline = pVStaff->AddBarline(nType, fVisible);
	m_nCurVoice = 1;
	pBarline->SetUserLocation(tPos);
    return false;

}

bool lmLDPParser::AnalyzeClef(lmVStaff* pVStaff, lmLDPNode* pNode)
{
    //returns true if error; in this case nothing is added to the lmVStaff
    //  <clef> = (clef {"G" | "F4" | "F3" | "C1" | "C2" | "C3" | "C4" | "percussion" }
    //                [<numStaff>] [<Visible>] )

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("clef"));

    //check that clef type is specified
    if(pNode->GetNumParms() < 1) {
        AnalysisError(
            pNode,
            _T("Element 'clef' has less parameters than the minimum required. Assumed '(clef G)'."));
        pVStaff->AddClef(lmE_Sol, 1, true);
        return false;
    }

    long iP = 1;
    wxString sName = (pNode->GetParameter(iP))->GetName();
    lmEClefType nClef = LDPNameToClef(sName);
    if (nClef == -1)
    {
        AnalysisError(pNode, _T("Unknown clef '%s'. Assumed 'G'."), sName.c_str());
        nClef = lmE_Sol;
    }
    iP++;

    //analyze optional parameters
	lmLDPOptionalTags oOptTags(this);
	oOptTags.SetValid(lm_eTag_Visible, lm_eTag_Location_x, lm_eTag_Location_y,
						lm_eTag_StaffNum, -1);		//finish list with -1

	lmLocation tPos = g_tDefaultPos;
    int nStaff = 1;
    bool fVisible = true;

	oOptTags.AnalyzeCommonOptions(pNode, iP, pVStaff, &fVisible, &nStaff, &tPos);

	//create the clef
    lmClef* pClef = pVStaff->AddClef(nClef, nStaff, fVisible);
	pClef->SetUserLocation(tPos);

    return false;

}

//returns true if error; in this case nothing is added to the lmVStaff
bool lmLDPParser::AnalyzeMetronome(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //  <metronome> = (metronome
    //                    { <NoteType> {<TicksPerMinute> | <NoteType>}  |
    //                      <TicksPerMinute> }
    //                    (parentheses) (<Visible>) )

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("metronome"));

    //check that at least one parameter is specified
    int nNumParms = pNode->GetNumParms();
    if(nNumParms < 1) {
        AnalysisError(
            pNode,
            _T("Element '%s' has less parameters than the minimum required. Ignored'."),
            sElmName.c_str() );
        return true;    //error
    }

    long iP = 1;
    wxString sName = (pNode->GetParameter(iP))->GetName();

    EMetronomeMarkType nMarkType;
    long nTicksPerMinute = 0;
    int nDots = 0;
    lmENoteType nLeftNoteType = eQuarter, nRightNoteType = eQuarter;
    int nLeftDots = 0, nRightDots = 0;

    //analize first parameter: value or left mark
    wxString sData = (pNode->GetParameter(iP++))->GetName();
    if (sData.IsNumber()) {
        //numeric value. Assume it is the ticks per minute rate
        sData.ToLong(&nTicksPerMinute);
        nMarkType = eMMT_MM_Value;
    }
    else {
        //string value. Assume it is mark type (note duration and dots)
        if (AnalyzeNoteType(sData, &nLeftNoteType, &nDots)) {
            AnalysisError(pNode, _T("Unknown note/rest duration '%s'. A quarter note assumed."),
                sData.c_str() );
        }
        nLeftDots = nDots;

        // Get right part
        if (iP > nNumParms) {
            AnalysisError(
                pNode,
                _T("Element '%s' has less parameters than the minimum required. Ignored'."),
                sElmName.c_str() );
            return true;    //error
        }
        sData = (pNode->GetParameter(iP++))->GetName();
        if (sData.IsNumber()) {
            //numeric value. Assume it is the ticks per minute rate
            sData.ToLong(&nTicksPerMinute);
            nMarkType = eMMT_Note_Value;
        }
        else {
            //string value. Assume it is mark type (note duration and dots)
            nMarkType = eMMT_Note_Note;
            if (AnalyzeNoteType(sData, &nRightNoteType, &nDots)) {
                AnalysisError(pNode, _T("Unknown note/rest duration '%s'. A quarter note assumed."),
                    sData.c_str() );
            }
            nRightDots = nDots;
        }
    }

    //Get optional 'parentheses' parameter
    bool fParentheses = false;
    lmLDPNode* pX = pNode->GetParameter( (wxString&)_T("parentheses") );
    if (pX) fParentheses = true;

    //Get common optional parameters
	lmLDPOptionalTags oOptTags(this);
	oOptTags.SetValid(lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1
	oOptTags.SetValid(lm_eTag_Visible, lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1

	lmLocation tPos = g_tDefaultPos;
    bool fVisible = true;

	oOptTags.AnalyzeCommonOptions(pNode, iP, pVStaff, &fVisible, NULL, &tPos);

    //bool fVisible = true;
    //for (; iP <= nNumParms; iP++) {
    //    pX = pNode->GetParameter(iP);
    //    if (pX->GetName() == _T("noVisible"))
    //        fVisible = false;
    //    else if (pX->GetName() == _T("parentheses"))
    //        fParentheses = true;
    //    else {
    //        AnalysisError(pX, _T("Unknown parameter '%s'. Ignored."), pX->GetName().c_str());
    //    }
    //}

    //create the metronome mark StaffObj
    lmMetronomeMark* pMM;
    switch (nMarkType)
    {
        case eMMT_MM_Value:
            pMM = pVStaff->AddMetronomeMark(nTicksPerMinute, fParentheses, fVisible);
            break;
        case eMMT_Note_Note:
            pMM = pVStaff->AddMetronomeMark(nLeftNoteType, nLeftDots, nRightNoteType, nRightDots,
                            fParentheses, fVisible);
            break;
        case eMMT_Note_Value:
            pMM = pVStaff->AddMetronomeMark(nLeftNoteType, nLeftDots, nTicksPerMinute,
                            fParentheses, fVisible);
            break;
        default:
            wxASSERT(false);
    }

    //set location
	pMM->SetUserLocation(tPos);

    return false;    //no error

}

bool lmLDPParser::GetYesNoValue(lmLDPNode* pNode, bool fDefault)
{
    wxString sValue = ((pNode->GetParameter(1))->GetName()).Lower();
    if (sValue == _T("true") || sValue == _T("yes"))
    {
        return true;
    }
    else if (sValue == _T("false") || sValue == _T("no"))
    {
        return false;
    }
    else
    {
        //get option name and value
        wxString sName = pNode->GetName();
        wxString sError = _T("a 'yes/no' or 'true/false' value");
        AnalysisError(pNode, _T("Error in data value for option '%s'.  It requires %s. Value '%s' ignored."),
            sName.c_str(), sError.c_str(), sValue.c_str());
    }
    return fDefault;
}

void lmLDPParser::AnalyzeOption(lmLDPNode* pNode, lmScoreObj* pObject)
{
    //  <opt> := <name><value>

    enum {
        lmBoolean = 0,
        lmNumberLong,
        lmNumberDouble,
        lmString
    };

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("opt"));

    //check that there are 2 parameters (name and value)
    if(pNode->GetNumParms() != 2) {
        AnalysisError(pNode, _T("Element 'opt' needs exactly 2 parameters. Tag ignored."));
        return;
    }

    //get option name and value
    wxString sName = (pNode->GetParameter(1))->GetName();
    wxString sValue = ((pNode->GetParameter(2))->GetName()).Lower();

    //verify option name and determine required data type
    int nDataType;
    if (sName == _T("StaffLines.StopAtFinalBarline"))
        nDataType = lmBoolean;
    else if (sName == _T("StaffLines.Hide"))
        nDataType = lmBoolean;
    else if (sName == _T("Staff.DrawLeftBarline"))
        nDataType = lmBoolean;
    else if (sName == _T("Staff.UpperLegerLines.Displacement"))
        nDataType = lmNumberLong;
    else if (sName == _T("Render.SpacingFactor"))
        nDataType = lmNumberDouble;
    else if (sName == _T("Render.SpacingMethod"))
        nDataType = lmString;
    else if (sName == _T("Render.SpacingValue"))
        nDataType = lmNumberLong;
    else
    {
        AnalysisError(pNode, _T("Option '%s' unknown. Ignored."), sName.c_str());
        return;
    }

    //get value
    long nNumberLong;
    double rNumberDouble;
    //bool fError = false;
	wxString sError;

    switch(nDataType) {
        case lmBoolean:
            if (sValue == _T("true") || sValue == _T("yes")) {
                pObject->SetOption(sName, true);
                return;
            }
            else if (sValue == _T("false") || sValue == _T("no")) {
                pObject->SetOption(sName, false);
                return;
            }
            else {
                wxString sError = _T("a 'yes/no' or 'true/false' value");
                AnalysisError(pNode, _T("Error in data value for option '%s'.  It requires %s. Value '%s' ignored."),
                    sName.c_str(), sError.c_str(), sValue.c_str());
            }
            return;

        case lmNumberLong:
            if (sValue.ToLong(&nNumberLong)) {
                pObject->SetOption(sName, nNumberLong);
                return;
            }
            else {
                sError = _T("an integer number");
                AnalysisError(pNode, _T("Error in data value for option '%s'.  It requires %s. Value '%s' ignored."),
                    sName.c_str(), sError.c_str(), sValue.c_str());
            }
            return;

        case lmNumberDouble:
			if (!StrToDouble(sValue, &rNumberDouble))
			{
                pObject->SetOption(sName, rNumberDouble);
                return;
            }
            sError = _T("a real number");
            AnalysisError(pNode, _T("Error in data value for option '%s'.  It requires %s. Value '%s' ignored."),
                sName.c_str(), sError.c_str(), sValue.c_str());
			return;

        case lmString:
            if (sName == _T("Render.SpacingMethod"))
            {
                if (sValue == _T("fixed"))
                    pObject->SetOption(sName, (long)esm_Fixed);
                else if (sValue == _T("propConstantFixed"))
                    pObject->SetOption(sName, (long)esm_PropConstantFixed);
                else
                    AnalysisError(pNode, _T("Error in data value for option '%s'.  Value '%s' ignored."),
                        sName.c_str(), sValue.c_str());
            }
            else
                pObject->SetOption(sName, sValue);

            return;
    }

}

//returns true if error; in this case nothing is added to the score
bool lmLDPParser::AnalyzeTitle(lmLDPNode* pNode, lmScore* pScore)
{
    //  (title <alignment> string [<font>][<location>])

    wxASSERT(pNode->GetName() == _T("title"));

    //check that at least two parameters (aligment and text string) are specified
    if(pNode->GetNumParms() < 2) {
        AnalysisError(
            pNode,
            _T("Element 'title' has less parameters than the minimum required. Element ignored."));
        return true;
    }

    wxString sTitle;
    wxString sStyle = _T("");
    bool fFont = false;
    lmEHAlign nAlign = m_nTitleAlignment;
    lmFontInfo tFont = {m_sTitleFontName, m_nTitleFontSize, m_nTitleStyle, m_nTitleWeight};
    lmLocation tPos;
    tPos.xUnits = lmTENTHS;
    tPos.yUnits = lmTENTHS;
    tPos.x = 0.0f;
    tPos.y = 0.0f;

    //get the aligment
    long iP = 1;
    wxString sName = (pNode->GetParameter(iP))->GetName();
    if (sName == _T("left"))
        nAlign = lmHALIGN_LEFT;
    else if (sName == _T("right"))
        nAlign = lmHALIGN_RIGHT;
    else if (sName == _T("center"))
        nAlign = lmHALIGN_CENTER;
    else {
        AnalysisError(pNode, _T("Invalid alignment value '%s'. Assumed 'center'."),
            sName.c_str() );
        nAlign = lmHALIGN_CENTER;
    }
    //save alignment as new default for titles
    m_nTitleAlignment = nAlign;
    iP++;

    //get the string
    sTitle = (pNode->GetParameter(iP))->GetName();
    iP++;

    //analyze remaining parameters (optional): font, style, location
    lmLDPNode* pX;
    for(; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);
        sName = pX->GetName();

        if (sName == _T("font"))
        {
            if (sStyle != _T(""))
                AnalysisError(pX, _T("[Conflict: 'Font' and 'Style' in the same definition. Font ingnored."));
            else
            {
                fFont = true;
                AnalyzeFont(pX, &tFont);
                //save font values as new default for titles
                m_sTitleFontName = tFont.sFontName;
                m_nTitleFontSize = tFont.nFontSize;
                m_nTitleStyle = tFont.nFontStyle;
                m_nTitleWeight = tFont.nFontWeight;
            }
        }
        else if (sName == _T("style"))
        {
            if (fFont)
                AnalysisError(pX, _T("[Conflict: 'Font' and 'Style' in the same definition. Font ingnored."));
            sStyle = (pX->GetParameter(1))->GetName();
        }
        else if (sName == _T("dx"))
        {
            AnalysisError(pX, _T("Obsolete: x location is not allowed in titles.") );
        }
        else if (sName == _T("dy"))
        {
            AnalyzeLocation(pX, &tPos);
        }
        else {
            AnalysisError(pX, _T("Unknown parameter '%s'. Ignored."), sName.c_str());
        }
    }

    //create the title
    lmTextStyle* pStyle = (lmTextStyle*)NULL;
    if (sStyle != _T(""))
    {
        pStyle = pScore->GetStyleInfo(sStyle);
        if (!pStyle)
            AnalysisError(pNode, _T("Style '%s' is not defined. Default style will be used."),
                           sStyle.c_str());
    }

    if (!pStyle)
        pStyle = pScore->GetStyleName(tFont);

    lmScoreTitle* pTitle = pScore->AddTitle(sTitle, nAlign, pStyle);
	pTitle->SetUserLocation(tPos);

    return false;

}

bool lmLDPParser::AnalyzeTextString(lmLDPNode* pNode, wxString* pText, wxString* pStyle,
                                    lmEHAlign* pAlign, lmLocation* pPos,
                                    lmFontInfo* pFont, bool* pHasWidth)
{
    //A certain number of LDP elements accepts a text-string with additional parameters,
    //such as location, font or alignment. This method parses these elements.
    //Default values for information not present must be initialized in return variables
    //before invoking this method.
    //Optional parameters not allowed in a particular context should be NULL pointers.
    //Returns true if error; in this case return variables are not changed.
    //If no error all variables but pNode are loaded with parsed information

    // <text-string> = (any-tag string [<location>] [{<font> | <style>}] [<alingment>])
    // <style> = (style <name>)

    //check that at least one parameter (text string) is specified
    if(pNode->GetNumParms() < 1) {
        AnalysisError(
            pNode,
            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
            pNode->GetName().c_str() );
        return true;
    }

    wxString sText;
    wxString sStyle = _T("");
    bool fFont = false;
    lmEHAlign nAlign;
    lmFontInfo tFont;
    lmLocation tPos;
    bool fHasWidth;

    //load default values
    if (pAlign)
        nAlign = *pAlign;
    if (pFont)
    {
        tFont.nFontSize = pFont->nFontSize;
        tFont.nFontStyle = pFont->nFontStyle;
        tFont.nFontWeight = pFont->nFontWeight;
        tFont.sFontName = pFont->sFontName;
    }
    if (pPos)
        tPos = *pPos;
    if (pHasWidth)
        fHasWidth = *pHasWidth;

    int iP = 1;

    //get the string
    sText = (pNode->GetParameter(iP))->GetName();
    iP++;

    //get remaining optional parameters: location, font, alignment
    lmLDPNode* pX;
    wxString sName;
    for(; iP <= pNode->GetNumParms(); iP++)
    {
        pX = pNode->GetParameter(iP);
        sName = pX->GetName();

        if (sName == _T("x") || sName == _T("dx") ||
            sName == _T("y") || sName == _T("dy") )
        {
            AnalyzeLocation(pX, &tPos);
        }
        else if (sName == _T("font"))
        {
            fFont = true;
            if (sStyle != _T(""))
                AnalysisError(pX, _T("[Conflict: 'Font' and 'Style' in the same definition. Font ingnored."));
            else
                AnalyzeFont(pX, &tFont);
        }
        else if (sName == _T("style"))
        {
            if (fFont)
                AnalysisError(pX, _T("[Conflict: 'Font' and 'Style' in the same definition. Font ingnored."));
            sStyle = (pX->GetParameter(1))->GetName();
        }
        else if (sName == _T("left")) {
            nAlign = lmHALIGN_LEFT;
        }
        else if (sName == _T("right")) {
            nAlign = lmHALIGN_RIGHT;
        }
        else if (sName == _T("center")) {
            nAlign = lmHALIGN_CENTER;
        }
        else if (sName == _T("hasWidth")) {
            fHasWidth = true;
        }
        else {
            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
                pNode->GetName().c_str(), sName.c_str() );
        }
    }

    //return parsed values
    *pText = sText;
    if (pAlign)
        *pAlign = nAlign;
    if (pPos)
        *pPos = tPos;
    if (pFont)
        *pFont = tFont;
    if (pHasWidth)
        *pHasWidth = fHasWidth;
    if (pStyle)
        *pStyle = sStyle;

    return false;
}

bool lmLDPParser::AnalyzeDefineStyle(lmLDPNode* pNode, lmScore* pScore)
{
    // <defineStyle> = (defineStyle <name><font><color>)

    //Analyzes a 'defineStyle' tag and, if successful, register the style in the
    //received score. Returns true if success.

    wxASSERT(pNode->GetName() == _T("defineStyle"));

    //check that three parameters are specified
    if(pNode->GetNumParms() != 3) {
        AnalysisError(
            pNode,
            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
            pNode->GetName().c_str() );
        return false;
    }

    //initialize values
    lmFontInfo tFont = {m_sTextFontName, m_nTextFontSize, m_nTextStyle, m_nTextWeight};
    wxColour color(0, 0, 0);        //default: black

    //get the style name
    int iP = 1;
    wxString sStyleName = (pNode->GetParameter(iP))->GetName();
    iP++;

    //get font and color, in no particular order
    lmLDPNode* pX;
    wxString sName;
    for(; iP <= pNode->GetNumParms(); iP++)
    {
        pX = pNode->GetParameter(iP);
        sName = pX->GetName();

        if (sName == _T("font"))
        {
            AnalyzeFont(pX, &tFont);
        }
        else if (sName == _T("color"))
        {
            color = AnalyzeColor(pX);
        }
        else
        {
            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
                pNode->GetName().c_str(), sName.c_str() );
        }
    }

    //register the style
    if (!sStyleName.IsEmpty())
        pScore->AddStyle(sStyleName, tFont, color);

    return true;
}

wxColour lmLDPParser::AnalyzeColor(lmLDPNode* pNode)
{
    // <color> = (color #rrggbb))

    //returns the result of the analysis. If error, returns black color.

    wxASSERT(pNode->GetName() == _T("color"));

    //check that one parameter is specified
    wxColor color;
    if(pNode->GetNumParms() != 1) {
        AnalysisError(
            pNode,
            _T("Element 'color' has less parameters than the minimum required. Color black will be used."));
        color.Set(0,0,0);
        return color;
    }

    wxString sColor;

    //get the color in HTML-like syntax (i.e. "#" followed by 6 hexadecimal digits
    //for red, green and blue components or 8 hexadecimal digits to include alpha channel
    sColor = (pNode->GetParameter(1))->GetName();

    //convert to color value
    if (!color.Set(sColor))
    {
        AnalysisError(pNode, _T("Invalid color value '%s'. Black will be used."),
                       sColor.c_str() );
        color.Set(0,0,0);
    }

    return color;
}

bool lmLDPParser::AnalyzePageLayout(lmLDPNode* pNode, lmScore* pScore)
{
	//  <pageLayout> := (pageLayout <pageSize><pageMargins><pageOrientation>)
	//  <pageSize> := (pageSize width height)
	//  <pageMargins> := (pageMargins left top right bottom binding)
	//  <pageOrientation> := [ "portrait" | "landscape" ]

    //Analyzes a 'pageLayout' tag and, if successful, pass layout data to the
    //received score. Returns true if success.

    wxASSERT(pNode->GetName() == _T("pageLayout"));

    //check that three parameters are specified
    if(pNode->GetNumParms() != 3) {
        AnalysisError(
            pNode,
            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
            pNode->GetName().c_str() );
        return false;
    }

    //get page size
    int iP = 1;
    lmLDPNode* pX = pNode->GetParameter(iP);
    wxString sName = pX->GetName();
    if (sName != _T("pageSize"))
    {
        AnalysisError(pX, _T("Expected 'pageSize' element but found '%s'. Ignored."),
            sName.c_str() );
		return false;
    }
    if(pX->GetNumParms() != 2)
    {
        AnalysisError(
            pNode,
            _T("Element '%s' has %d parameters, less than the minimum required. Element ignored."),
				_T("pageSize"), pX->GetNumParms() );
        return false;
    }
	lmLUnits uWidth, uHeight;
    wxString sValue = (pX->GetParameter(1))->GetName();
	if (GetFloatNumber(pNode, sValue, sName, &uWidth))
        return false;
    sValue = (pX->GetParameter(2))->GetName();
	if (GetFloatNumber(pNode, sValue, sName, &uHeight))
        return false;
    pScore->SetPageSize(uWidth, uHeight);
    iP++;

    //get page margins
    pX = pNode->GetParameter(iP);
    sName = pX->GetName();
    if (sName != _T("pageMargins"))
    {
        AnalysisError(pX, _T("Expected 'pageMargins' element but found '%s'. Ignored."),
            sName.c_str() );
		return false;
    }
    if(pX->GetNumParms() != 5) {
        AnalysisError(
            pX,
            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
				_T("pageMargins") );
        return false;
    }
	lmLUnits uLeft, uTop, uRight, uBottom, uBinding;
    sValue = (pX->GetParameter(1))->GetName();
	if (GetFloatNumber(pNode, sValue, sName, &uLeft))
        return false;
    sValue = (pX->GetParameter(2))->GetName();
	if (GetFloatNumber(pNode, sValue, sName, &uTop))
        return false;
    sValue = (pX->GetParameter(3))->GetName();
	if (GetFloatNumber(pNode, sValue, sName, &uRight))
        return false;
    sValue = (pX->GetParameter(4))->GetName();
	if (GetFloatNumber(pNode, sValue, sName, &uBottom))
        return false;
    sValue = (pX->GetParameter(5))->GetName();
	if (GetFloatNumber(pNode, sValue, sName, &uBinding))
        return false;
    pScore->SetPageSize(uWidth, uHeight);
    pScore->SetPageBindingMargin(uBinding);
	pScore->SetPageBottomMargin(uBottom);
	pScore->SetPageLeftMargin(uLeft);
	pScore->SetPageRightMargin(uRight);
	pScore->SetPageTopMargin(uTop);
    iP++;

    //get page orientation
    pX = pNode->GetParameter(iP);
    sName = (pNode->GetParameter(iP))->GetName();
    if (sName == _T("portrait"))
		pScore->SetPageOrientation(true);
    else if (sName == _T("landscape"))
		pScore->SetPageOrientation(false);
	else
    {
        AnalysisError(pNode, _T("Expected 'portrait' or 'landscape' but found '%s'. Ignored."),
            sName.c_str() );
		pScore->SetPageOrientation(true);
    }

	return true;
}

bool lmLDPParser::GetFloatNumber(lmLDPNode* pNode, wxString& sValue, wxString& nodeName,
                                 float* pValue)
{
	//if error, returns true, sets pValue to 0.0f and issues an error message

	double rNumberDouble;
	if (!StrToDouble(sValue, &rNumberDouble))
	{
        *pValue = (float)rNumberDouble;
		return false;
	}
    else
	{
        AnalysisError(pNode, _T("Element '%s': Invalid value '%s'. It must be a float number."),
            nodeName.c_str(), sValue.c_str() );
        *pValue = 0.0f;
        return true;
    }
}

bool lmLDPParser::GetValueFloatNumber(lmLDPNode* pNode, float* pValue)
{
	//if error, returns true, sets pValue to 0.0f and issues an error message

    wxString sValue = pNode->GetParameter(1)->GetName();
	double rNumberDouble;
	if (!StrToDouble(sValue, &rNumberDouble))
	{
        *pValue = (float)rNumberDouble;
		return false;
	}
    else
	{
        AnalysisError(pNode, _T("Element '%s': Invalid value '%s'. It must be a float number."),
            pNode->GetName().c_str(), sValue.c_str() );
        *pValue = 0.0f;
        return true;
    }
}

bool lmLDPParser::AnalyzeText(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //returns true if error; in this case nothing is added to the VStaff
    // <text> = (text string <location>[<font><alingment>])

    wxASSERT(pNode->GetName() == _T("text"));

    //check that at least two parameters (location and text string) are specified
    if(pNode->GetNumParms() < 2) {
        AnalysisError(
            pNode,
            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
            _T("text") );
        return true;
    }

    wxString sText;
    wxString sStyle;
    lmEHAlign nAlign = lmHALIGN_LEFT;     //TODO user options instead of fixed values
    bool fHasWidth = false;
    lmFontInfo tFont = {m_sTextFontName, m_nTextFontSize, m_nTextStyle, m_nTextWeight};
    lmLocation tPos;
    tPos.xUnits = lmTENTHS;
    tPos.yUnits = lmTENTHS;
    tPos.x = 0.0f;
    tPos.y = 0.0f;

    if (AnalyzeTextString(pNode, &sText, &sStyle, &nAlign, &tPos, &tFont, &fHasWidth))
        return true;

    //no error:
    //save font values as new default for texts
    m_sTextFontName = tFont.sFontName;
    m_nTextFontSize = tFont.nFontSize;
    m_nTextStyle = tFont.nFontStyle;
    m_nTextWeight = tFont.nFontWeight;

    //create the text
    lmTextStyle* pStyle = (lmTextStyle*)NULL;
    if (sStyle != _T(""))
    {
        pStyle = pVStaff->GetScore()->GetStyleInfo(sStyle);
        if (!pStyle)
            AnalysisError(pNode, _T("Style '%s' is not defined. Default style will be used."),
                           sStyle.c_str());
    }

    if (!pStyle)
        pStyle = pVStaff->GetScore()->GetStyleName(tFont);

    lmTextItem* pText = pVStaff->AddText(sText, nAlign, pStyle, fHasWidth);
	pText->SetUserLocation(tPos);

    return false;
}

bool lmLDPParser::AnalyzeKeySignature(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //returns true if error; in this case nothing is added to the lmVStaff

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("key"));

    //check that key value is specified
    if(pNode->GetNumParms() < 1) {
        AnalysisError(
            pNode,
            _T("Element '%s' has less parameters than the minimum required. Assumed '(%s %s)'."),
            sElmName.c_str(), sElmName.c_str(), _T("C") );
        pVStaff->AddKeySignature(earmDo);
        return false;
    }

    long iP = 1;
    wxString sKey = (pNode->GetParameter(iP))->GetName();
    lmEKeySignatures nKey = LDPNameToKey( sKey );
    if (nKey == -1) {
        //not found.
        AnalysisError(pNode, _T("Unknown key '%s'. Assumed 'C'."), sKey.c_str() );
        nKey = earmDo;
    }
    iP++;

    //analyze optional parameters
	lmLDPOptionalTags oOptTags(this);
	oOptTags.SetValid(lm_eTag_Visible, lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1
	lmLocation tPos = g_tDefaultPos;
    bool fVisible = true;
	oOptTags.AnalyzeCommonOptions(pNode, iP, pVStaff, &fVisible, NULL, &tPos);

	//create the tiem signature
    lmKeySignature* pKS = pVStaff->AddKeySignature(nKey, fVisible);
	pKS->SetUserLocation(tPos);
    return false;

}

bool lmLDPParser::AnalyzeTimeSignature(lmVStaff* pVStaff, lmLDPNode* pNode)
{
    //returns true if error and in this case nothing is added to the lmVStaff
    //  <timeSignature> ::= ("time" <num> <num> [<visible>])

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("time"));

    //check that the two numbers are specified
    if(pNode->GetNumParms() < 2) {
        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Assumed '(Metrica 4 4)'."),
            _T("time") );
        pVStaff->AddTimeSignature(emtr44);
        return false;
    }

    wxString sNum1 = (pNode->GetParameter(1))->GetName();
    wxString sNum2 = (pNode->GetParameter(2))->GetName();
    if (!sNum1.IsNumber() || !sNum2.IsNumber()) {
        AnalysisError(
            pNode,
            _T("Element 'time': Two numbers expected but found '%s' and '%s'. Assumed '(time 4 4)'."),
            sNum1.c_str(),
            sNum2.c_str() );
        pVStaff->AddTimeSignature(emtr44);
        return false;
    }

    long nBeats, nBeatType;
    sNum1.ToLong(&nBeats);
    sNum2.ToLong(&nBeatType);

    //analyze optional parameters
	lmLDPOptionalTags oOptTags(this);
	oOptTags.SetValid(lm_eTag_Visible, lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1
	lmLocation tPos = g_tDefaultPos;
    bool fVisible = true;
	oOptTags.AnalyzeCommonOptions(pNode, 3, pVStaff, &fVisible, NULL, &tPos);

	//create the tiem signature
    lmTimeSignature* pTS = pVStaff->AddTimeSignature((int)nBeats, (int)nBeatType, fVisible);
	pTS->SetUserLocation(tPos);
    return false;

}

void lmLDPParser::AnalyzeSpacer(lmLDPNode* pNode, lmVStaff* pVStaff)
{
//  <spacer> ::= ("spacer" <width>)     width in tenths

    wxString sElmName = pNode->GetName();

    //check that the width is specified
    if(pNode->GetNumParms() < 1)
    {
        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Ignored."),
            sElmName.c_str());
        return;
    }

    wxString sNum1 = (pNode->GetParameter(1))->GetName();
    if (!sNum1.IsNumber()) {
        AnalysisError(
            pNode,
            _T("Element '%s': Width expected but found '%s'. Ignored."),
            sElmName.c_str(), sNum1.c_str());
        return;
    }

    long nWidth;
    sNum1.ToLong(&nWidth);
    pVStaff->AddSpacer((lmTenths)nWidth);

}

void lmLDPParser::AnalyzeGraphicObj(lmLDPNode* pNode, lmVStaff* pVStaff)
{
//  <graphic> ::= ("graphic" <type> <params>*)

    wxString sElmName = pNode->GetName();
    int nNumParms = pNode->GetNumParms();

    //check that type is specified
    if(nNumParms < 2)
    {
        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Element ignored."),
            sElmName.c_str());
        return;
    }

    // analyze type and get its params.
    int iP = 1;
    wxString sType = (pNode->GetParameter(iP))->GetName();
    if (sType == _T("line"))
    {
        // line
        // Parms: xStart, yStart, xEnd, yEnd, nWidth, colour.
        // All coordinates in tenths, relative to current pos.
        // line width in tenths (optional parameter). Default: 1 tenth
        // colour (optional parameter). Default: black

        // get parameters
        if(nNumParms < 5)
        {
            AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Element ignored."),
                sElmName.c_str());
            return;
        }

        lmTenths rPos[4];
        long nPos;
        wxString sNum;
        for (iP=2; iP <= 5; iP++) {
            sNum = (pNode->GetParameter(iP))->GetName();
            if (!sNum.IsNumber()) {
                AnalysisError(
                    pNode,
                    _T("Element '%s': Coordinate expected but found '%s'. Ignored."),
                    sElmName.c_str(), sNum.c_str());
                return;
            }
            sNum.ToLong(&nPos);
            rPos[iP-2] = (lmTenths)nPos;
        }

        // get line width (optional parameter). Default: 1 tenth
        //TODO
        lmTenths rWidth = 1;

        // get colour (optional parameter). Default: black
        //TODO
        wxColour nColor = *wxBLACK;

        // create the AuxObj and attach it to the VStaff
        lmScoreLine* pLine = new lmScoreLine(rPos[0], rPos[1], rPos[2], rPos[3],
                                       rWidth, nColor);
        lmStaffObj* pAnchor = (lmStaffObj*) pVStaff->AddAnchorObj();
        pAnchor->AttachAuxObj(pLine);

    }
    else {
        AnalysisError(
            pNode,
            _T("Element '%s': Type of graphic (%s) unknown. Ignored."),
            sElmName.c_str(), sType.c_str());
    }

}

void lmLDPParser::AnalyzeTextbox(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //<textbox> ::= (textbox <location>[<size>][<color>][<border>]<text>[<anchorLine>])

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("textbox"));
    int nNumParms = pNode->GetNumParms();

    //parameters and their default values
        //box
	lmLocation tPos = g_tDefaultPos;
    lmTenths ntWidth = 160.0f;
    lmTenths ntHeight = 100.0f;
    wxColour nBgColor(255, 255, 255);
        //text
    lmFontInfo tFont = {m_sTextFontName, m_nTextFontSize, m_nTextStyle, m_nTextWeight};
    wxString sText = _("Error in text!");
    wxString sStyle = _T("");
        //border
    lmELineStyle nBorderStyle = lm_eLine_Solid;
    wxColour nBorderColor = *wxBLACK;
    lmTenths ntBorderWidth = 1.0f;
        //anchor line
	lmLocation tAnchorPoint = g_tDefaultPos;
    lmELineStyle nAnchorLineStyle = lm_eLine_Solid;
    wxColour nAnchorLineColor = *wxBLACK;
    lmTenths ntAnchorLineWidth = 1.0f;

    //loop to analyze parameters. Optional: color, border, line
    for(int iP=1; iP <= pNode->GetNumParms(); iP++)
    {
        lmLDPNode* pX = pNode->GetParameter(iP);
        wxString sName = pX->GetName();
        if (sName == _T("dx") || sName == _T("dy"))
        {
            AnalyzeLocation(pX, &tPos);
        }
        else if(sName == _T("size"))
        {
            AnalyzeSize(pX, &ntWidth, &ntHeight);
        }
        else if(sName == _T("color"))
        {
            nBgColor = AnalyzeColor(pX);
        }
        else if(sName == _T("border"))
        {
            AnalyzeBorder(pX, &ntBorderWidth, &nBorderStyle, &nBorderColor);
        }
        else if(sName == _T("text"))
        {
            //(text string [<location>] [{<font> | <style>}] [<alignment>])
            //mandatory: string. Optional: style. All others forbidden
            if (AnalyzeTextString(pX, &sText, &sStyle, (lmEHAlign*)NULL,
                                  (lmLocation*)NULL, (lmFontInfo*)NULL, (bool*)NULL))
            {
                //error in text element
                //TODO
            }
        }
        else if(sName == _T("anchorLine"))
        {
            //TODO
        }
        else
        {
            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
                          _T("textbox"), sName.c_str() );
        }
    }

    // create the AuxObj and attach it to the VStaff
    lmTPoint ntPos(tPos.x, tPos.y);
    lmScoreTextParagraph* pSTP =
        new lmScoreTextParagraph(ntWidth, ntHeight, ntPos);
    lmStaffObj* pAnchor = (lmStaffObj*) pVStaff->AddAnchorObj();
    pAnchor->AttachAuxObj(pSTP);

    //apply values to created lmScoreTextParagraph

    //background colour
    pSTP->SetBgColour(nBgColor);

    //border
    pSTP->SetBorderWidth(ntBorderWidth);
    pSTP->SetBorderColor(nBorderColor);
    pSTP->SetBorderStyle(nBorderStyle);

    //anchor line
    tAnchorPoint.x = 0.0f;
    tAnchorPoint.y = 0.0f;
    pSTP->AddAnchorLine(tAnchorPoint, ntAnchorLineWidth, nAnchorLineStyle,
                        nAnchorLineColor);

    //text
    lmTextStyle* pStyle = GetTextStyle(pNode, sStyle);
    lmBaseText* pBText = new lmBaseText(sText, pStyle);
    pSTP->InsertTextUnit(pBText);
}

bool lmLDPParser::AnalyzeBorder(lmLDPNode* pNode, lmTenths* ptWidth,
                                lmELineStyle* pLineStyle, wxColour* pColor)
{
    //returns true if error
    //<border> ::= (border <width><lineStyle><color>)

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("border"));
    int nNumParms = pNode->GetNumParms();

    //parameters and their default values
    wxColour nColor(0, 0, 0);           //default: black
    lmTenths ntWidth = 1.0f;            //default: 1 tenth
    lmELineStyle nLineStyle = lm_eLine_Solid;

    //load default values
    *ptWidth = ntWidth;
    *pLineStyle = nLineStyle;
    *pColor = nColor;

    //check that type is specified
    if(nNumParms < 3)
    {
        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Element ignored."),
                      _T("border") );
        return true;    //error
    }

    //loop to analyze parameters: width & height
    for(int iP=1; iP <= pNode->GetNumParms(); iP++)
    {
        lmLDPNode* pX = pNode->GetParameter(iP);
        wxString sName = pX->GetName();
        if (sName == _T("width"))
            GetValueFloatNumber(pX, ptWidth);
        else if (sName == _T("color"))
            nColor = AnalyzeColor(pX);
        else if (sName == _T("lineStyle"))
            GetValueLineStyle(pX, &nLineStyle);
        else
            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
                          _T("size"), sName.c_str() );
    }

    //return parsed values
    *ptWidth = ntWidth;
    *pLineStyle = nLineStyle;
    *pColor = nColor;

    return false;    //no error
}

bool lmLDPParser::GetValueLineStyle(lmLDPNode* pNode, lmELineStyle* pLineStyle)
{
	//if error, returns true, sets pLineStyle to lm_eLine_Solid and issues an error message
    //<lineStyle> = (lineStyle { none | solid | longDash | shortDash | dot | dotDash } )

    wxString sValue = pNode->GetParameter(1)->GetName();
    if (sValue == _T("none"))
        *pLineStyle = lm_eLine_None;
    else if (sValue == _T("solid"))
        *pLineStyle = lm_eLine_Solid;
    else if (sValue == _T("longDash"))
        *pLineStyle = lm_eLine_LongDash;
    else if (sValue == _T("shortDash"))
        *pLineStyle = lm_eLine_ShortDash;
    else if (sValue == _T("dot"))
        *pLineStyle = lm_eLine_Dot;
    else if (sValue == _T("dotDash"))
        *pLineStyle = lm_eLine_DotDash;
    else
	{
        AnalysisError(pNode, _T("Element 'lineStyle': Invalid value '%s'. Replaced by 'solid'"));
        *pLineStyle = lm_eLine_Solid;
        return true;
    }
    return false;       //no error
}

bool lmLDPParser::AnalyzeSize(lmLDPNode* pNode, lmTenths* ptWidth, lmTenths* ptHeight)
{
    //returns true if error
    //<size> ::= (size <width><height>)
    //<width> ::= (width num)
    //<height> ::= (height num)

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("size"));
    int nNumParms = pNode->GetNumParms();

    //check that it has two more parameters
    if(nNumParms != 2)
    {
        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Element ignored."),
                      _T("size"));
        return true;    //error
    }

    //loop to analyze parameters: width & height
    for(int iP=1; iP <= pNode->GetNumParms(); iP++)
    {
        lmLDPNode* pX = pNode->GetParameter(iP);
        wxString sName = pX->GetName();
        if (sName == _T("width"))
            GetValueFloatNumber(pX, ptWidth);
        else if (sName == _T("height"))
            GetValueFloatNumber(pX, ptHeight);
        else
            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
                          _T("size"), sName.c_str() );
    }
    return false;    //no error
}

void lmLDPParser::AnalyzeAnchorLine(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //<anchorLine> = (anchorLine <destination-point>[<width>][<lineStyle>][<color>]
    //                           [<lineEndStyle>])
    //<destination-point> = <location>
    //<lineEndStyle> = (lineEndStyle { none | arrow | dot | square | diamond })

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("rectangle"));
    int nNumParms = pNode->GetNumParms();

    //parameters and their default values
	lmLocation tPos = g_tDefaultPos;
    lmTenths ntWidth = 160.0f;
    lmTenths ntHeight = 100.0f;
    wxColour nBgColor(255, 255, 255);
    //border
    lmELineStyle nBorderStyle = lm_eLine_Solid;
    wxColour nBorderColor = *wxBLACK;
    lmTenths ntBorderWidth = 1.0f;

    //loop to analyze parameters. Optional: color, border
    for(int iP=1; iP <= pNode->GetNumParms(); iP++)
    {
        lmLDPNode* pX = pNode->GetParameter(iP);
        wxString sName = pX->GetName();
        if (sName == _T("dx") || sName == _T("dy"))
        {
            AnalyzeLocation(pX, &tPos);
        }
        else if(sName == _T("size"))
        {
            AnalyzeSize(pX, &ntWidth, &ntHeight);
        }
        else if(sName == _T("color"))
        {
            nBgColor = AnalyzeColor(pX);
        }
        else if(sName == _T("border"))
        {
            AnalyzeBorder(pX, &ntBorderWidth, &nBorderStyle, &nBorderColor);
        }
        else
        {
            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
                          _T("rectangle"), sName.c_str() );
        }
    }

    //// create the AuxObj and attach it to the VStaff
    //lmScoreTextParagraph* pSTP = new lmScoreTextParagraph();
    //lmStaffObj* pAnchor = (lmStaffObj*) pVStaff->AddAnchorObj();
    //pAnchor->AttachAuxObj(pSTP);

    ////apply values to created lmScoreTextParagraph

    ////rectangle
    //pSTP->SetWidth(ntWidth);
    //pSTP->SetHeight(ntHeight);
    //pSTP->SetBgColour(nBgColor);

    ////border
    //pSTP->SetBorderWidth(ntBorderWidth);
    //pSTP->SetBorderColor(nBorderColor);
    //pSTP->SetBorderStyle(nBorderStyle);
}

lmTextStyle* lmLDPParser::GetTextStyle(lmLDPNode* pNode, const wxString& sStyle)
{
    //Returns style for that style name or style for normal text if style name is not
    //defined in the score

    lmTextStyle* pStyle = (lmTextStyle*)NULL;
    if (sStyle != _T(""))
    {
        pStyle = m_pScore->GetStyleInfo(sStyle);
        if (!pStyle)
            AnalysisError(pNode, _T("Style '%s' is not defined. Default style will be used."),
                          sStyle.c_str());
    }
    if (!pStyle)
        pStyle = m_pScore->GetStyleInfo(_("Normal text"));

    return pStyle;
}

bool lmLDPParser::AnalyzeNewSystem(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //returns true if error; in this case nothing is added to the lmVStaff
    //<newSystem> ::= (newSystem}

    wxASSERT(pNode->GetName() == _T("newSystem"));

    //check if there are parameters
    if(pNode->GetNumParms() >= 1) {
        //for now, no parameters allowed
        wxASSERT(false);
        return true;
    }

    //add control object
    pVStaff->AddNewSystem();
    return false;

}

lmEStemType lmLDPParser::AnalyzeStem(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //<Stem> ::= (stem [up | down] <lenght> }

    wxASSERT(pNode->GetName() == _T("stem"));

    lmEStemType nStem = lmSTEM_DEFAULT;

    //check that there are parameters
    if(pNode->GetNumParms() < 1) {
        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Tag ignored. Assumed default stem."),
            _T("stem"));
        return nStem;
    }

    //get stem direction
    wxString sDir = (pNode->GetParameter(1))->GetName();
    if (sDir == _T("up"))
        nStem = lmSTEM_UP;
    else if (sDir == _T("down"))
        nStem = lmSTEM_DOWN;
    else {
        AnalysisError(pNode, _T("Invalid stem direction '%s'. Default direction taken."), sDir.c_str());
    }

    return nStem;

}

lmEPlacement lmLDPParser::AnalyzeFermata(lmLDPNode* pNode, lmVStaff* pVStaff, lmLocation* pPos)
{
    //<Fermata> ::= (fermata [above | below]}

    wxASSERT(pNode->GetName() == _T("fermata"));

    lmEPlacement nPlacement = ep_Default;

    //check that there are parameters
    if(pNode->GetNumParms() < 1) {
        AnalysisError(pNode,_T("Element '%s' has less parameters than the minimum required. Tag ignored. Assumed default stem."),
            _T("fermata") );
        return nPlacement;
    }

    //get fermata placement
    wxString sDir = (pNode->GetParameter(1))->GetName();
    if (sDir == _T("above"))
        nPlacement = ep_Above;
    else if (sDir == _T("below"))
        nPlacement = ep_Below;
    else {
        AnalysisError(pNode, _T("Invalid fermata placement '%s'. Default placement assumed."), sDir.c_str());
    }

    //analyze optional parameters
	lmLDPOptionalTags oOptTags(this);
	oOptTags.SetValid(lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1
	oOptTags.AnalyzeCommonOptions(pNode, 2, pVStaff, NULL, NULL, pPos);

    return nPlacement;

}

void lmLDPParser::AnalyzeFont(lmLDPNode* pNode, lmFontInfo* pFont)
{
    // <font> = (font <name> <size> <style>)

    //returns, in variables pointed by pFontName, pFontSize and pStyleDevuelve the
    //result of the analysis. No default values are returned, only the real values
    //found. Any defaults must be set before invoking this method

    wxASSERT(pNode->GetName() == _T("font"));

    //check that there are parameters
    if (!(pNode->GetNumParms() > 0)) {
        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Tag ignored."),
            pNode->GetName().c_str() );
    }

    //flags to control that the corresponding parameter has been processed
    bool fName = false;
    bool fSize = false;
    bool fStyle = false;

    //get parameters. The come in any order
    lmFontInfo tFont = *pFont;
    int iP;
    wxString sParm;

    bool fProcessed;
    for(iP=1; iP <= pNode->GetNumParms(); iP++)
    {
        sParm = (pNode->GetParameter(iP))->GetName();
        fProcessed = false;

        if (!fStyle) {
            //try style and weight
            fStyle = true;
            fProcessed = true;
            if (sParm == _T("bold"))
            {
                tFont.nFontStyle = wxFONTSTYLE_NORMAL;
                tFont.nFontWeight = wxFONTWEIGHT_BOLD;
            }
            else if (sParm == _T("normal"))
            {
                tFont.nFontStyle = wxFONTSTYLE_NORMAL;
                tFont.nFontWeight = wxFONTWEIGHT_NORMAL;
            }
            else if (sParm == _T("italic"))
            {
                tFont.nFontStyle = wxFONTSTYLE_ITALIC;
                tFont.nFontWeight = wxFONTWEIGHT_NORMAL;
            }
            else if (sParm == _T("bold-italic"))
            {
                tFont.nFontStyle = wxFONTSTYLE_ITALIC;
                tFont.nFontWeight = wxFONTWEIGHT_BOLD;
            }
            else {
                fStyle = false;
                fProcessed = false;
            }
        }

        if (!fSize && !fProcessed) {
            wxString sSize = sParm;
            if (sParm.length() > 2 && sParm.Right(2) == _T("pt")) {
                sSize = sParm.Left(sParm.length() - 2);
            }
            if (sSize.IsNumber()) {
                long nSize;
                sSize.ToLong(&nSize);
                tFont.nFontSize = (int)nSize;
                fSize = true;
                fProcessed = true;
            }
        }

        if (!fName && !fProcessed) {
            //assume it is the name
            fName = true;
            tFont.sFontName = (pNode->GetParameter(iP))->GetName();
            fProcessed = true;
        }

        if (!fProcessed) {
            AnalysisError(pNode, _T("Element '%s': invalid parameter '%s'. It is ignored."),
                _T("font"), sParm.c_str() );
        }
    }

    *pFont = tFont;

}

void lmLDPParser::AnalyzeLocation(lmLDPNode* pNode, float* pValue, lmEUnits* pUnits)
{
    // <location> = { (dx num) | (dy num) }
    // <num> = number [units]

    //returns, in variables pointed by pValue and pUnits the
    //result of the analysis.

    wxString sElement = pNode->GetName();

    //check that there are parameters
    if (pNode->GetNumParms()!= 1) {
        AnalysisError(pNode, _T("Element '%s' has less or more parameters than required. Tag ignored."),
            sElement.c_str() );
        return;
    }

    //get value
    wxString sParm = (pNode->GetParameter(1))->GetName();
    wxString sValue = sParm;
    wxString sUnits = sParm.Right(2);
	if (sUnits.at(0) != _T('.') && !sUnits.IsNumber() )
	{
        AnalysisError(pNode, _T("Element '%s' has units '%s'. Units no longer supported. Ignored"),
            sElement.c_str(), sUnits.c_str() );
    }

    GetFloatNumber(pNode, sValue, sElement, pValue);

}

void lmLDPParser::AnalyzeLocation(lmLDPNode* pNode, lmLocation* pPos)
{
    //analyze location
    wxString sName = pNode->GetName();

    wxASSERT(sName == _T("dx") || sName == _T("dy") );

    float rValue;
    lmEUnits nUnits = lmTENTHS;     //default value
    AnalyzeLocation(pNode, &rValue, &nUnits);
    if (sName == _T("dx"))
    {
        //dx
        pPos->x = rValue;
        pPos->xUnits = nUnits;
    }
    else {
        //dy
        pPos->y = rValue;
        pPos->yUnits = nUnits;
    }

}

////Devuelve true si hay error, es decir si no añade objeto al pentagrama
//Function AnalizarDirectivaRepeticion(lmVStaff* pVStaff, lmLDPNode* pNode) As Boolean
////<repeticion> = (repeticion <valor> <posicion> )
////<valor> =
////        "dacapo" |
////        "segno" | ("segno" num) |
////        "dalsegno" | ("dalsegno" num) |
////        "coda" | ("coda" num) |
////        "tocoda" | ("tocoda" num) |
////        "fine"
//
//    wxASSERT(pNode->GetName() = "REPETICION"
//    wxASSERT(pNode->GetNumParms() = 2
//
//    Dim lmLDPNode* pX, long iP
//    Dim nNum As Long, sDuration As String, nType As EDirectivasRepeticion
//    Dim sNum As String
//
//    //obtiene tipo de repeticion
//    Set pX = pNode->GetParameter(1)
//    sDuration = UCase$(pX->GetName())
//    if (pX->IsSimple()) {
//        nNum = 1
//    } else {
//        sNum = pX->GetParameter(1).GetName();
//        if (Not IsNumeric(sNum)) {
//            AnalysisError(pX, wxString::Format(_T("[AnalizarDirectivaRepeticion]: Valor <" & sNum & _
//                "> para la directiva de repetición <" & _
//                pX->GetName() & "> no es numérico. Se ignora este elemento."
//            AnalizarDirectivaRepeticion = true
//            Exit Function
//        }
//        nNum = CLng(sNum)
//    }
//
//    switch (sDuration
//        case "DACAPO"
//            nType = eDR_DaCapo
//        case "DC"
//            nType = eDR_DC
//        case "SEGNO"
//            nType = eDR_Segno
//        case "DALSEGNO"
//            nType = eDR_DalSegno
//        case "DS"
//            nType = eDR_DS
//        case "CODA"
//            nType = eDR_Coda
//        case "ALCODA"
//            nType = eDR_AlCoda
//        case "FINE"
//            nType = eDR_Fine
//        case "ALFINE"
//            nType = eDR_AlFine
//        case "REPETICION"
//            nType = eDR_Repeticion
//        default:
//            AnalysisError(pNode, wxString::Format(_T("Signo de repetición <" & sDuration & "> desconocido. " & _
//                "Se ignora elemento."
//            AnalizarDirectivaRepeticion = true
//            Exit Function
//    }
//
//    //obtiene posicion
//    iP = 2
//    Set pX = pNode->GetParameter(iP)
//    Dim nX As Long, nY As Long, fXAbs As Boolean, fYAbs As Boolean
//    AnalizarPosicion pX, nX, nY, fXAbs, fYAbs
//
//    //crea el pentobj
//    pVStaff.AddDirectivaRepeticion nType, nNum, nX, nY, fXAbs, fYAbs
//
//    AnalizarDirectivaRepeticion = false       //no hay error
//
//}

int lmLDPParser::AnalyzeNumStaff(const wxString& sNotation, lmLDPNode* pNode, long nNumStaves)
{
    //analyzes a notation Pxx.  xx must be lower or equal than nNumStaves

    if (sNotation.Left(1) != _T("p"))
    {
        AnalysisError(pNode, _T("Staff number expected but found '%s'. Replaced by 'p1'"),
            sNotation.c_str() );
        return 1;
    }

    wxString sData = sNotation.substr(1);         //remove char 'p'
    if (!sData.IsNumber()) {
        AnalysisError(pNode, _T("Staff number not followed by number (%s). Replaced by 'p1'"),
            sNotation.c_str() );
        return 1;
    }

    long nValue;
    sData.ToLong(&nValue);
    if (nValue > nNumStaves) {
        AnalysisError(pNode, _T("Notation '%s': number is greater than number of staves defined (%d). Replaced by 'p1'."),
            sNotation.c_str(), nNumStaves );
        return 1;
    }
    return (int)nValue;

}

int lmLDPParser::AnalyzeVoiceNumber(const wxString& sNotation, lmLDPNode* pNode)
{
    //analyzes a notation Vx.  x must be 1..lmMAX_VOICE

    if (sNotation.Left(1) != _T("v")) {
        AnalysisError(pNode, _T("Voice number expected but found '%s'. Replaced by 'v1'"),
            sNotation.c_str() );
        return 1;
    }

    wxString sData = sNotation.substr(1);         //remove char 'v'
    if (!sData.IsNumber()) {
        AnalysisError(pNode, _T("Voice number expected but found '%s'. Replaced by 'v1'"),
            sNotation.c_str() );
        return 1;
    }

    long nValue;
    sData.ToLong(&nValue);
    if (nValue >= lmMAX_VOICE) {
        AnalysisError(pNode, _T("Notation '%s': number is greater than supported voices (%d). Replaced by 'v1'."),
            sNotation.c_str(), lmMAX_VOICE );
        return 1;
    }
    return (int)nValue;

}

float lmLDPParser::GetDefaultDuration(lmENoteType nNoteType, int nDots, int nActualNotes,
                                    int nNormalNotes)
{
    //compute duration without modifiers
    float rDuration = NoteTypeToDuration(nNoteType, nDots);

    //alter by tuplet modifiers
    if (nActualNotes != 0) rDuration = (rDuration * (float)nNormalNotes) / (float)nActualNotes;

    return rDuration;
}

int lmLDPParser::GetBeamingLevel(lmENoteType nNoteType)
{
    switch(nNoteType) {
        case eEighth:
            return 0;
        case e16th:
            return 1;
        case e32th:
            return 2;
        case e64th:
            return 3;
        case e128th:
            return 4;
        case e256th:
            return 5;
        default:
            return -1; //Error: Requesting beaming a note longer than eight
    }
}

bool lmLDPParser::AnalyzeNoteType(wxString& sNoteType, lmENoteType* pnNoteType,
                                  int* pNumDots)
{
    // Receives a string (sNoteType) with the LDP letter for the type of note and, optionally,
    // dots "."
    // Set up variables nNoteType and pNumDots.
    //
    //  USA           UK                      ESP               LDP     NoteType
    //  -----------   --------------------    -------------     ---     ---------
    //  long          longa                   longa             l       eLonga = 0
    //  double whole  breve                   cuadrada, breve   d       eBreve = 1
    //  whole         semibreve               redonda           r       eWhole = 2
    //  half          minim                   blanca            b       eHalf = 3
    //  quarter       crochet                 negra             n       eQuarter = 4
    //  eighth        quaver                  corchea           c       eEighth = 5
    //  sixteenth     semiquaver              semicorchea       s       e16th = 6
    //  32nd          demisemiquaver          fusa              f       e32th = 7
    //  64th          hemidemisemiquaver      semifusa          m       e64th = 8
    //  128th         semihemidemisemiquaver  garrapatea        g       e128th = 9
    //  256th         ???                     semigarrapatea    p       e256th = 10
    //
    // Returns true if error in parsing


    sNoteType.Trim(false);      //remove spaces from left
    sNoteType.Trim(true);       //and from right

    //locate dots, if exist, and extract note type string
    wxString sType;
    wxString sDots;
    int iDot = sNoteType.Find(_T("."));
    if (iDot != -1) {
        sType = sNoteType.Left(iDot);
        sDots = sNoteType.substr(iDot);
    }
    else {
        sType = sNoteType;
        sDots = _T("");
    }

    //identify note type
    if (sType.Left(1) == _T("'")) {
        // numeric duration: '1, '2, '4, '8, '16, '32, ..., '256
        sType = sType.substr(1);
        if (!sType.IsNumber()) return true;     //error
        long nType;
        sType.ToLong(&nType);
        switch(nType) {
            case 1:     *pnNoteType = eWhole;       break;
            case 2:     *pnNoteType = eHalf;        break;
            case 4:     *pnNoteType = eQuarter;     break;
            case 8:     *pnNoteType = eEighth;      break;
            case 16:    *pnNoteType = e16th;        break;
            case 32:    *pnNoteType = e32th;        break;
            case 64:    *pnNoteType = e64th;        break;
            case 128:   *pnNoteType = e128th;       break;
            case 256:   *pnNoteType = e256th;       break;
            default:
                return true;    //error
        }
    }
    // duration as a letter
    else if (sType == _T("l"))
        *pnNoteType = eLonga;
    else if (sType == _T("d"))
        *pnNoteType = eBreve;
    else if (sType == _T("w"))
        *pnNoteType = eWhole;
    else if (sType == _T("h"))
        *pnNoteType = eHalf;
    else if (sType == _T("q"))
        *pnNoteType = eQuarter;
    else if (sType == _T("e"))
        *pnNoteType = eEighth;
    else if (sType == _T("s"))
        *pnNoteType = e16th;
    else if (sType == _T("t"))
        *pnNoteType = e32th;
    else if (sType == _T("i"))
        *pnNoteType = e64th;
    else if (sType == _T("o"))
        *pnNoteType = e128th;
    else if (sType == _T("f"))
        *pnNoteType = e256th;
    else
        return true;    //error

    //analyze dots
    *pNumDots = 0;
    if (sDots.length() > 0) {
        if (sDots.StartsWith( _T("....") ))
            *pNumDots = 4;
        else if (sDots.StartsWith( _T("...") ))
            *pNumDots = 3;
        else if (sDots.StartsWith( _T("..") ))
            *pNumDots = 2;
        else if (sDots.StartsWith( _T(".") ))
            *pNumDots = 1;
        else
            return true;    //error
    }

    return false;   //no error

}




//-----------------------------------------------------------------------------------------
// lmLDPOptionalTags implementation: Helper class to analyze optional elements
//-----------------------------------------------------------------------------------------

lmLDPOptionalTags::lmLDPOptionalTags(lmLDPParser* pParser)
{
	m_pParser = pParser;

	//no tag valid, for now
	m_ValidTags.reserve(lm_eTag_Max);
	m_ValidTags.assign(lm_eTag_Max, false);
}

lmLDPOptionalTags::~lmLDPOptionalTags()
{
}

void lmLDPOptionalTags::SetValid(lmETagLDP nTag, ...)
{
	//process optional tags. Finish list with -1

	//process first arg
	m_ValidTags[nTag] = true;

	//process additional args
	va_list pArgs;
	// va_start is a macro which accepts two arguments, a va_list and the name of the
	// variable that directly precedes the ellipsis (...).
	va_start(pArgs, nTag);     // initialize the list to point to first variable argument
	while(true)
	{
		// va_arg takes a va_list and a variable type, and returns the next argument
		// in the list in the form of whatever variable type it is told. It then moves
		// down the list to the next argument.
		lmETagLDP nNextTag = (lmETagLDP)va_arg(pArgs, int);
		if (nNextTag == -1) break;
		m_ValidTags[nNextTag] = true;
	}
	va_end(pArgs);		//clean up the list
}

bool lmLDPOptionalTags::VerifyAllowed(lmETagLDP nTag, wxString sName, lmLDPNode* pNode)
{
	if (m_ValidTags[nTag]) return true;

	//tag invalid. Log error message
    m_pParser->AnalysisError(
                pNode,
				_T("[AnalyzeCommonOptions]: Not allowed element '%s' found. Element ignored."),
                sName.c_str() );
	return false;

}

void lmLDPOptionalTags::AnalyzeCommonOptions(lmLDPNode* pNode, int iP, lmVStaff* pVStaff,
									   // variables to return optional values
									   bool* pfVisible,
									   int* pStaffNum,
									   lmLocation* pLocation
									   )
{
    //analyze optional parameters
	//if the optional tag is valid fills corresponding received variables
	//if tag is not allowed, ignore it and continue with the next option

	for(; iP <= pNode->GetNumParms(); iP++)
	{
        lmLDPNode* pX = pNode->GetParameter(iP);
        const wxString sName = pX->GetName();

		//number of staff on which the element is located
        if (sName.Left(1) == _T("p"))
        {
			if (VerifyAllowed(lm_eTag_StaffNum, sName, pNode)) {
				*pStaffNum = m_pParser->AnalyzeNumStaff(sName, pNode, pVStaff->GetNumStaves());
			}
            pX->SetProcessed(true);
        }

		//visible or not
        else if (sName == _T("noVisible"))
		{
			if (VerifyAllowed(lm_eTag_Visible, sName, pNode)) {
				*pfVisible = false;
			}
            pX->SetProcessed(true);
        }

		// X location
        else if (sName == _T("dx"))
        {
			if (VerifyAllowed(lm_eTag_Location_x, sName, pNode)) {
				m_pParser->AnalyzeLocation(pX, pLocation);
			}
            pX->SetProcessed(true);
		}

		// Y location
        else if (sName == _T("dy"))
        {
			if (VerifyAllowed(lm_eTag_Location_y, sName, pNode)) {
				m_pParser->AnalyzeLocation(pX, pLocation);
			}
            pX->SetProcessed(true);
		}

		// Octave shift
		else if (sName == _T("-8va") || sName == _T("+8va")
                    || sName == _T("+15ma") || sName == _T("-15ma") )
        {
            //TODO tessiture option in clef
            pX->SetProcessed(true);
        }

        //else
			// Unknown tag. Ignore it
    }

}

