//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#include "../score/MetronomeMark.h"
#include "../auxmusic/Conversion.h"
#include "LDPParser.h"
#include "AuxString.h"
#include "LDPTags.h"


//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"


const wxString sEOF = _T("<<$$EOF$$>>");

lmLDPParser::lmLDPParser()
{
    Create(_T("es"), _T("iso-8859-1"));     //default tags in Spanish
}

lmLDPParser::lmLDPParser(wxString sLanguage, wxString sCharset)
{
    Create(sLanguage, sCharset);
}

void lmLDPParser::Create(const wxString& sLanguage, const wxString& sCharset)
{
    m_pTokenizer = new lmLDPTokenBuilder(this);
    m_pCurNode = (lmLDPNode*) NULL;
    m_fDebugMode = g_pLogger->IsAllowedTraceMask(_T("lmLDPParser"));
    m_pTuplet = (lmTupletBracket*)NULL;
    m_pTags = lmLdpTagsTable::GetInstance();
    m_pTags->LoadTags(sLanguage, sCharset);
    // default values for font and aligment for <title> elements
    //TODO user options instead of fixed values
    m_nTitleAlignment = lmALIGN_CENTER;
    m_sTitleFontName = _T("Times New Roman");
    m_nTitleFontSize = 14;
    m_nTitleStyle = lmTEXT_BOLD;

    // default values for font and aligment for <text> elements
    //TODO user options instead of fixed values
    m_sTextFontName = _T("Times New Roman");
    m_nTextFontSize = 10;
    m_nTextStyle = lmTEXT_NORMAL;

    // default values for notes/rests octave and duration
    m_sLastOctave = _T("4");                                        // octave 4
    m_sLastDuration = m_pTags->TagName(_T("n"), _T("NoteType"));    // quarter note

    // default values for tuplet options
    m_fShowTupletBracket = true;
    m_fShowNumber = true;
    m_fTupletAbove = true;

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
    {
        delete *it;
    }
    m_StackNodes.clear();

    delete m_pCurNode;
    m_pCurNode = (lmLDPNode*) NULL;
}


lmScore* lmLDPParser::ParseFile(const wxString& filename, bool fErrorMsg)
{
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
}

//------------------------------------------------------------------------------------------
// File management functions
//------------------------------------------------------------------------------------------

const wxString& lmLDPParser::GetNewBuffer()
{
    if (m_fFromString) {
        //parsing a string
        if (m_fStartingTextAnalysis) {
            // m_sLastBuffer is loaded with string to analyse. So return this buffer
            //nNumLinea = 1
            m_fStartingTextAnalysis = false;
        } else {
            // the string is finished. End of analysis. Return EOF buffer
            m_sLastBuffer = sEOF;
        }
    } else {
        //parsing a file
        //m_sLastBuffer = _T("(n s)") + sEOF;
//        if (Not fRepetirLinea) {
            if (m_pFile->Eof()) {
                m_sLastBuffer = sEOF;
            } else {
                m_sLastBuffer = m_pTextFile->ReadLine();
                //nNumLinea++;
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

void lmLDPParser::AnalysisError(const wxChar* szFormat, ...)
{
    m_nErrors++;

    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg = _T("** LDP ERROR **: ") + wxString::FormatV(szFormat, argptr);
    wxLogMessage(sMsg);
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


    m_pCurNode = new lmLDPNode(_T("Root Node"));
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
        AnalysisError( _T("Element RAIZ expected but found element %s. Analysis stopped."),
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
            m_pCurNode = new lmLDPNode(m_pTk->GetValue());
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
        AnalysisError(_T("Syntax error: more closing parenthesis than parenthesis opened. Analysis stopped."));
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
    lmScore* pScore = (lmScore*) NULL;
    int i;

    if (!(pNode->GetName() == _T("score") || pNode->GetName() == _T("Score")) ) {
        AnalysisError( _T("Element 'score' expected but found element %s. Analysis stopped."),
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
        AnalysisError( _T("Element 'vers' expected but found element %s. Analysis stopped."),
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
        case 102:
        case 103:
            pScore = AnalyzeScoreV102(pNode);
            break;
        case 105:
            pScore = AnalyzeScoreV105(pNode);
            break;
        default:
            AnalysisError( _T("Error analysing LDP score: LDP version (%d) not supported. Analysis stopped."),
                m_nVersion );
            return pScore;
    }

     return pScore;

}

lmScore* lmLDPParser::AnalyzeScoreV102(lmLDPNode* pNode)
{
    //<score> = (score <vers> [<credits>] <instrument>*)

    lmLDPNode* pX;
    long i, iP;
    wxString sData;
    lmScore* pScore = (lmScore*) NULL;

    //for version 1.3 or lower all tags in Spanish
    m_pTags->LoadTags(_T("es"), _T("iso-8859-1"));

    //parse element <credits>
    wxString sTitle = _T("");
    iP = 2;      //first parameter is always the version and is already analyzed. So, skip it
    pX = pNode->GetParameter(iP);
    if (pX->GetName() == _T("Credits") ) {
        //TODO no treatment yet. Ignore
        iP++;
    }

    // parse element <NumInstruments>
    long nInstruments = 0;
    pX = pNode->GetParameter(iP);
    if (pX->GetName() != _T("NumInstrumentos")) {
        AnalysisError( _T("Element 'NumInstrumentos' expected but found element %s. Analysis stopped."),
            pX->GetName().c_str() );
        return pScore;
    } else {
        sData = pX->GetParameter(1)->GetName();
        if (sData.IsNumber()) {
            sData.ToLong(&nInstruments);
         }
        iP++;
    }

    // build the score
    pScore = new lmScore();

    //if (sTitulo != _T(""))pScore->Set.Title(sTitle);

    // loop to parse elements <instrument>
    for (i=1; iP <= pNode->GetNumParms(); i++, iP++) {
        pX = pNode->GetParameter(iP);
        AnalyzeInstrument(pX, pScore, i);
    }

    return pScore;

}

lmScore* lmLDPParser::AnalyzeScoreV105(lmLDPNode* pNode)
{
    //<score> = (score <vers> [<language>] [<titles>] <instrument>*)
    //<language> = (language LanguageCode Charset )

    lmLDPNode* pX;
    long i, iP;
    wxString sData;
    lmScore* pScore = (lmScore*) NULL;
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
            m_pTags->LoadTags(sLangCode, sCharset);
        }
        iP++;
    }

    // create the score
    pScore = new lmScore();

    //parse optional element <titles>
    pX = pNode->GetParameter(iP);
    while(pX->GetName() == m_pTags->TagName(_T("title")) &&  iP <= nNumParms) {
        AnalyzeTitle(pX, pScore);
        iP++;
        if (iP <= nNumParms) pX = pNode->GetParameter(iP);
    }

    //parse optional elements <opt>
    pX = pNode->GetParameter(iP);
    while(pX->GetName() == m_pTags->TagName(_T("opt")) &&  iP <= nNumParms) {
        AnalyzeOption(pX, pScore);
        iP++;
        if (iP <= nNumParms) pX = pNode->GetParameter(iP);
    }

    // loop to parse elements <instrument>
    i=0;
    while(pX->GetName() == m_pTags->TagName(_T("instrument")) &&  iP <= nNumParms) {
        AnalyzeInstrument105(pX, pScore, i++);
        iP++;
        if (iP <= nNumParms) pX = pNode->GetParameter(iP);
    }

    return pScore;

}

void lmLDPParser::AnalyzeInstrument105(lmLDPNode* pNode, lmScore* pScore, int nInstr)
{
    //<instrument> = (instrument [<InstrName>][<InfoMIDI>][<Staves>] <Voice>+ )

    //<InstrName> = (instrName name-string [abbreviation-string])
    //<InfoMIDI> = (infoMIDI num-instr [num-device])
    //<Staves> = (staves {num | overlayered} )
    //<Voice> = (MusicData <music>+ )

    lmLDPNode* pX;
    wxString sData;
    long iP;
    iP = 1;

    if (pNode->GetName() != m_pTags->TagName(_T("instrument")) ) {
        AnalysisError( _T("Element '%s' expected but found element %s. Analysis stopped."),
            m_pTags->TagName(_T("instrument")).c_str(), pNode->GetName().c_str() );
        return;
    }

    //default values
	int nMIDIChannel = g_pMidi->DefaultVoiceChannel();
	int nMIDIInstr = g_pMidi->DefaultVoiceInstr();
    bool fMusicFound = false;               // <MusicData> tag found
    wxString sNumStaves = _T("1");          //one staff

    //default values for name
    //TODO user options instead of fixed values
    wxString sInstrName = _T("");           //no name for instrument
    lmEAlignment nNameAlign = lmALIGN_LEFT;
    bool fNameHasWidth = false;
    lmFontInfo tNameFont = g_tInstrumentDefaultFont;
    lmLocation tNamePos = g_tDefaultPos;

    //default values for abbreviation
    //TODO user options instead of fixed values
    wxString sInstrAbbrev = _T("");         //no abreviated name for instrument
    lmEAlignment nAbbrevAlign = lmALIGN_LEFT;
    bool fAbbrevHasWidth = false;
    lmFontInfo tAbbrevFont = g_tInstrumentDefaultFont;
    lmLocation tAbbrevPos = g_tDefaultPos;

    // parse optional elements until <MusicData> tag found
    for (; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);

        if (pX->GetName() == m_pTags->TagName(_T("musicData")) ) {
            fMusicFound = true;
            break;      //start of MusicData. Exit this loop
        }
        else if (pX->GetName() == m_pTags->TagName(_T("name")) ) {
            AnalyzeTextString(pX, &sInstrName, &nNameAlign, &tNamePos,
                              &tNameFont, &fNameHasWidth);
        }
        else if (pX->GetName() == m_pTags->TagName(_T("abbrev")) ) {
            AnalyzeTextString(pX, &sInstrAbbrev, &nAbbrevAlign, &tAbbrevPos,
                              &tAbbrevFont, &fAbbrevHasWidth);
        }
        else if (pX->GetName() == m_pTags->TagName(_T("infoMIDI")) ) {
            //TODO No treatment for now
            //    nMIDIChannel = nMidiCanalVoz
            //    nMIDIInstr = nMidiInstrVoz
            //
            //    if (pX->GetName() = "INFOMIDI") {
            //        AnalizarInfoMIDI pX, nMIDIChannel, nMIDIInstr
            //        iP = iP + 1
            //    }
        }
        else if (pX->GetName() == m_pTags->TagName(_T("staves")) ) {
            pX = pX->GetParameter(1);
            if (pX->IsSimple()) {
                sNumStaves = pX->GetName();
                if (!sNumStaves.IsNumber()) {
                    AnalysisError( _T("Number of staves expected but found '%s'. Element '%s' ignored."),
                        sNumStaves.c_str(), m_pTags->TagName(_T("staves")).c_str() );
                    sNumStaves = _T("1");
                }
            }
            else {
                AnalysisError( _T("Expected value for %s but found element '%s'. Ignored."),
                    m_pTags->TagName(_T("staves")).c_str(), pX->GetName().c_str() );
           }
        }
        else {
            AnalysisError( _T("[%s]: unknown element '%s' found. Element ignored."),
                m_pTags->TagName(_T("instrument")).c_str(), pX->GetName().c_str() );
        }
    }

    //set number of staves (to be used by AnalyzeMusicData in order to add staves)
    sNumStaves.ToLong(&m_nNumStaves);

    //process firts voice
    if (!fMusicFound) {
        AnalysisError( _T("Expected '%s' but found element %s. Analysis stopped."),
            m_pTags->TagName(_T("musicData")).c_str(), pX->GetName().c_str() );
        return;
    }

    // create the instrument with one empty VStaff
    lmScoreText* pName = (lmScoreText*)NULL;
    lmScoreText* pAbbrev = (lmScoreText*)NULL;
    if (sInstrName != _T(""))
        pName = new lmScoreText(sInstrName, nNameAlign, tNamePos, tNameFont);
    if (sInstrAbbrev != _T(""))
        pAbbrev = new lmScoreText(sInstrAbbrev, nAbbrevAlign, tAbbrevPos, tAbbrevFont);

    lmInstrument* pInstr = pScore->AddInstrument(nMIDIChannel, nMIDIInstr,
                                        pName, pAbbrev);
    lmVStaff* pVStaff = pInstr->GetVStaff();

    // analyce first MusicData
    AnalyzeMusicData(pX, pVStaff);
    iP++;

	//CSG: 19/jan/2008. Support for more than one VStaff removed
    ////analyze other MusicData elements
    //for(; iP <= pNode->GetNumParms(); iP++) {
    //    pX = pNode->GetParameter(iP);
    //    if (pX->GetName() = m_pTags->TagName(_T("musicData")) ) {
    //        pVStaff = pInstr->AddVStaff(true);      //true -> overlayered
    //        AnalyzeMusicData(pX, pVStaff);
    //    }
    //    else {
    //        AnalysisError( _T("Expected '%s' but found element %s. Element ignored."),
    //            m_pTags->TagName(_T("musicData")).c_str(), pX->GetName().c_str() );
    //    }
    //}

}

void lmLDPParser::AnalyzeMusicData(lmLDPNode* pNode, lmVStaff* pVStaff)
{

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("musicData")));

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
        if (sName == m_pTags->TagName(_T("n"), _T("SingleChar")) ) {        // note
            AnalyzeNote(pX, pVStaff);
        } else if (sName == m_pTags->TagName(_T("r"), _T("SingleChar")) ) { // rest
            AnalyzeRest(pX, pVStaff);
        } else if (sName == m_pTags->TagName(_T("clef")) ) {
            AnalyzeClef(pVStaff, pX);
        } else if (sName == m_pTags->TagName(_T("time")) ) {
            AnalyzeTimeSignature(pVStaff, pX);
        } else if (sName == m_pTags->TagName(_T("key")) ) {
            AnalyzeKeySignature(pX, pVStaff);
        } else if (sName == m_pTags->TagName(_T("barline")) ) {
            AnalyzeBarline(pX, pVStaff);
        } else if (sName == m_pTags->TagName(_T("chord")) ) {
            AnalyzeChord(pX, pVStaff);
        } else if (sName == m_pTags->TagName(_T("text")) ) {
            AnalyzeText(pX, pVStaff);
        } else if (sName == m_pTags->TagName(_T("newSystem")) ) {
            AnalyzeNewSystem(pX, pVStaff);
        } else if (sName == m_pTags->TagName(_T("spacer")) ) {
            AnalyzeSpacer(pX, pVStaff);
        } else if (sName == m_pTags->TagName(_T("graphic")) ) {
            AnalyzeGraphicObj(pX, pVStaff);
        } else if (sName == m_pTags->TagName(_T("metronome")) ) {
            AnalyzeMetronome(pX, pVStaff);
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
        else if (sName == m_pTags->TagName(_T("goFwd"))
                 || sName == m_pTags->TagName(_T("goBack")) )
        {
            AnalyzeTimeShift(pX, pVStaff);
			if (sName == m_pTags->TagName(_T("goBack")) ) m_nCurVoice++;
		}
        //abbreviated syntax for notes and rests
        else if (sName.Left(1) == m_pTags->TagName(_T("n"), _T("SingleChar")) ) {
            AnalyzeNote(pX, pVStaff);
        } else if (sName.Left(1) == m_pTags->TagName(_T("r"), _T("SingleChar")) ) {
            AnalyzeRest(pX, pVStaff);
        }
        //error or non-supported elements
        else {
            AnalysisError( _T("[AnalyzeMusicData]: Unknown or not allowed element '%s' found. Element ignored."),
                sName.c_str() );
        }
    }
	//pVStaff->AddBarline(lm_eBarlineEOS, true);
	m_nCurVoice = 1;

}

void lmLDPParser::AnalyzeSplit(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //TODO   AnalyzeSplit code

}

void lmLDPParser::AnalyzeChord(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    // <chord> = (chord <Note>* )

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("chord")));

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
            AnalysisError( _T("[AnalyzeChord]: Expecting notes found element '%s'. Element ignored."),
                sName.c_str() );
        }
    }

}


void lmLDPParser::AnalyzeInstrument(lmLDPNode* pNode, lmScore* pScore, int nInstr)
{
    //<instrument> = (instrument [<instrName>][<infoMIDI>] <Parte>*)

    //<NombreInstrumento string [string]) - nombre, [abreviatura]
    //<NumPartes> = (NumPartes num ) - num vstaffs
    //<InfoMIDI num [num]> - Num.Instrumento MIDI, [Num.Dispositivo MIDI]
    //<Parte> = (Parte num [num pentagramas] <compas>* )

    // <Parte> represents an lmVStaff
    wxString sLabelNumVStaves;
    sLabelNumVStaves = (m_nVersion > 102 ? _T("NumPartes") : _T("NumPentagramas"));


    lmLDPNode* pX;
    wxString sData;
    long iP;
    iP = 1;

    if (pNode->GetName() != _T("Instrumento") ) {
        AnalysisError( _T("Element '%s' expected but found element %s. Analysis stopped."),
            _T("Instrumento"), pNode->GetName().c_str() );
        return;
    }

    // parse element [<NombreInstrumento> | num ]
    pX = pNode->GetParameter(iP);
    if (pX->GetName() = _T("NombreInstrumento")) {
        //TODO de momento no hay tratamiento
        iP++;
    } else {
        //if { IsNumeric(pX->GetName())) {
        //por ahora ignorar. No vale para nada
        iP++;
    }

    // parse element <NumPartes>
    long nVStaves=0;
    pX = pNode->GetParameter(iP);
    if (pX->GetName() != sLabelNumVStaves) {
        AnalysisError( _T("Element %s expected but found element %s. Analysis stopped."),
            sLabelNumVStaves.c_str(), pX->GetName().c_str() );
        return;
    } else {
        sData = (pX->GetParameter(1))->GetName();
        if (sData.IsNumber()) {
            sData.ToLong(&nVStaves);
         }
        iP++;
    }

//    // parse element <InfoMIDI>
//    Set pX = pNode->GetParameter(iP)
	int nMIDIChannel = g_pMidi->DefaultVoiceChannel();
	int nMIDIInstr = g_pMidi->DefaultVoiceInstr();
//
//    if (pX->GetName() = "INFOMIDI") {
//        AnalizarInfoMIDI pX, nMIDIChannel, nMIDIInstr
//        iP = iP + 1
//    }

    // create the instrument
	//CSG: 19/jan/2008. Support for more than one VStaff removed
    lmInstrument* pInstr = pScore->AddInstrument(nMIDIChannel, nMIDIInstr, _T(""));
    pX = pNode->GetParameter(iP);
    AnalyzeVStaff(pX, pInstr->GetVStaff());

}

void lmLDPParser::AnalyzeVStaff(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //v1.1  <Linea> = ("LINEA" <num> <Compas>* )
    //v1.2  <Pentagrama> = ("PENTAGRAMA" <num> <Compas>* )
    //v1.3  <Parte> = ("PARTE" <num> [<num_pentagramas>] <Compas>* )

    wxString sLabel;
    if (m_nVersion <= 101) {
        sLabel = _T("Linea");
    } else if (m_nVersion == 102) {
        sLabel = _T("Pentagrama");
    } else {
        sLabel = _T("Parte");
    }

    if (pNode->GetName() != sLabel) {
        AnalysisError( _T("Expected node '%s' but found node '%s'. Analysis ended."),
            sLabel.c_str(), pNode->GetName().c_str() );
        return;
    }

    AnalyzeVStaff_V103(pNode, pVStaff);     //version 1.3

}

void lmLDPParser::AnalyzeVStaff_V103(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //v1.3  <Parte> = ("PARTE" <num> [<num_pentagramas>] <Compas>* )

    lmLDPNode* pX;
    long iP = 1;

    //analyze first parameter: num of this lmVStaff
    wxString sData1 = (pNode->GetParameter(iP))->GetName();
    if (! sData1.IsNumber()) {
        AnalysisError( _T("Expected 'numero de parte' but found '%s'. Analysis stopped."),
            pNode->GetName().c_str() );
        return;
    }
    iP++;

    //analyze second parameter (optional): number of staves of this lmVStaff
    wxString sNumStaves = _T("1");       //default value
    pX = pNode->GetParameter(iP);
    if (pX->IsSimple()) {
        sNumStaves = pX->GetName();
        if (!sNumStaves.IsNumber()) {
            AnalysisError( _T("Number of staves expected but found '%s'. Analysis stopped."),
                pX->GetName().c_str() );
            return;
        } else {
            iP++;
        }
    }
    long nNumStaves;
    sNumStaves.ToLong(&nNumStaves);
    int i;
    //the VStaff already contains one staff. So we have to add nNumStaves - 1
    for(i=1; i < nNumStaves; i++) {
        pVStaff->AddStaff(5);    //five lines staff, standard size
    }

    //analyze remaining parameters: bars
    for (; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);
        AnalyzeMeasure(pX, pVStaff);
    }

    //pVStaff->AddBarline(lm_eBarlineEOS, true);

}

void lmLDPParser::AnalyzeMeasure(lmLDPNode* pNode, lmVStaff* pVStaff)
{
//v1.1  <Compas> ::= ("C" [<Num>] {<Figura> | <Grupo> | <Atributo> | <Indicacion> |
//                     <Barra> | <desplazamiento> | <opciones>}* )
//v1.1  <Atributo> ::= (<Clave> | <Tonalidad> | <Metrica>)
//v1.1  <Indicacion> ::= (<Metronomo>)
//v1.1  <Metronomo> ::= ("Metronomo" <Valor_nota> {<Tics_minuto> | (<Valor_nota> )
//v1.1  <Barra> ::= ("Barra" <Tipo_barra> [<posición>])
//      <desplazamiento> ::= { ("retroceso" partes) | ("avance" partes) }

    //analyze first parameter: measure number (optional)
    long iP = 1;
    wxString sNumBar = (pNode->GetParameter(iP))->GetName();
    if (sNumBar.IsNumber()) {
        iP++;
    } else {
    }

    //analyze remaining parameters: {<Figura> | <Grupo> | <Atributo>}*
    bool fSomethingAdded = false, fBarline = false;
    //double rDesplz;
    wxString sName;
    lmLDPNode* pX;
    for(; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);
        sName = pX->GetName();
        if (sName == _T("n") || sName == _T("na")) {    //nota simple, nota en acorde
                AnalyzeNote(pX, pVStaff);
                fSomethingAdded = true;
        } else if (sName == _T("s")) {        // rest
                AnalyzeRest(pX, pVStaff);
                fSomethingAdded = true;
            //case "G"        //<grupo>
            //    AnalizarGrupo pX, pVStaff
            //    fSomethingAdded = true
        } else if (sName == _T("Clave")) {
            fSomethingAdded = ! AnalyzeClef(pVStaff, pX);
        } else if (sName == _T("Metrica")) {
            fSomethingAdded = ! AnalyzeTimeSignature(pVStaff, pX);
        } else if (sName == _T("Tonalidad")) {
            fSomethingAdded = ! AnalyzeKeySignature(pX, pVStaff);
            //case "METRONOMO"
            //    fSomethingAdded = Not AnalizarMetronomo(pVStaff, pX)
        } else if (sName == _T("Barra")) {
                fBarline = !AnalyzeBarline(pX, pVStaff);
            //case "OPCIONES"
            //    AnalyzeOpt pVStaff, pX
            ////avances y retrocesos ----------------------------------------
            //case "AVANCE"
            //case "RETROCESO"
            //    rDesplz = AnalizarDesplazamiento(pX)
            //    pVStaff.ShiftTime rDesplz
            ////directivas ---------------------------------------------------
            //case "REPETICION"
            //    AnalizarDirectivaRepeticion pVStaff, pX
            //case "TEXTO"
            //    AnalizarDirectivaTexto pVStaff, pX
        } else {
            AnalysisError( _T("[AnalyzeMeasure]: Expected node 'Figura', 'Grupo', 'Atributo' or 'Desplazamiento' but found node '%s'. Node ignored."),
                sName.c_str() );
        }
    }

    if (fSomethingAdded && !fBarline) {
        pVStaff->AddBarline(lm_eBarlineSimple);    //finish the bar
		m_nCurVoice = 1;
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
    wxASSERT(sElmName == m_pTags->TagName(_T("goFwd"))
             || sElmName == m_pTags->TagName(_T("goBack")) );

    bool fForward = (sElmName == m_pTags->TagName(_T("goFwd")));

    //check that there are parameters
    if(pNode->GetNumParms() < 1) {
        AnalysisError( _T("Element '%s' has less parameters than the minimum required. Element ignored."),
            sElmName.c_str());
        return;
    }

    //get first parameter: time shift amount
    float rShift;
    wxString sValue = (pNode->GetParameter(1))->GetName();
    if (sValue == m_pTags->TagName(_T("start")) ) {
        if (!fForward)
            rShift = lmTIME_SHIFT_START_END;
        else {
            AnalysisError( _T("Element '%s' has an incoherent value: go forward to start?. Element ignored"),
                sElmName.c_str());
            return;
        }
    }
    else if (sValue == m_pTags->TagName(_T("end")) ) {
        if (fForward)
            rShift = lmTIME_SHIFT_START_END;
        else {
            AnalysisError( _T("Element '%s' has an incoherent value: go backwards to end?. Element ignored"),
                sElmName.c_str());
            return;
        }
    }
    else {
        //algebraic expression. Analyze it
        if (AnalyzeTimeExpression(sValue, &rShift)) return;      //error
    }

    //change sign for backwad movement
    if (!fForward) rShift = - rShift;

    //procced to do the time shift
    if (pVStaff->ShiftTime(rShift))
    {
        AnalysisError( pVStaff->GetErrorMessage() );
    }

}

bool lmLDPParser::AnalyzeTimeExpression(const wxString& sData, float* pValue)
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
            bool fDotted, fDoubleDotted;
            lmENoteType nNoteType;
            if (AnalyzeNoteType(sChar, &nNoteType, &fDotted, &fDoubleDotted)) {
                AnalysisError(_T("Time shift: Letter %s is not a valid note duration. Replaced by a quarter note"), sChar.c_str());
                rValue = (float)eQuarter;
            }
            else {
                rValue = NoteTypeToDuration(nNoteType, fDotted, fDoubleDotted);
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
    wxASSERT(sElmName.Left(1) == m_pTags->TagName(_T("n"), _T("SingleChar")) ||
             sElmName.Left(1) == m_pTags->TagName(_T("r"), _T("SingleChar")) ||
             sElmName == _T("na") );

    bool fIsRest = (sElmName.Left(1) == m_pTags->TagName(_T("r"), _T("SingleChar")) );   //analysing a rest

    lmEStemType nStem = lmSTEM_DEFAULT;
    bool fBeamed = false;
    bool fTie = false;
    bool fVisible = true;
    lmTBeamInfo BeamInfo[6];
    for (int i=0; i < 6; i++) {
        BeamInfo[i].Repeat = false;
        BeamInfo[i].Type = eBeamNone;
    }

    //Tuplet brakets
    bool fEndTuplet = false;
    //int nTupletNumber = 0;      // 0 = no tuplet
    int nActualNotes = 0;       // 0 = no tuplet
    int nNormalNotes = 0;

    //default values
    bool fDotted = false;
    bool fDoubleDotted = false;
    lmENoteType nNoteType = eQuarter;
    float rDuration = GetDefaultDuration(nNoteType, fDotted, fDoubleDotted, nActualNotes, nNormalNotes);
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
                AnalysisError( _T("Missing parameters in rest '%s'. Replaced by '(%s %s)'."),
                    pNode->ToString().c_str(), sElmName.c_str(), m_pTags->TagName(_T("n"), _T("NoteType")).c_str() );
                return pVStaff->AddRest(nNoteType, rDuration, fDotted, fDoubleDotted,
                                        m_nCurStaff, m_nCurVoice, fVisible);
            }
        }
        else {
            if (nParms < 2) {
                AnalysisError( _T("Missing parameters in note '%s'. Assumed (%s c4 %s)."),
                    pNode->ToString().c_str(), sElmName.c_str(), m_pTags->TagName(_T("n"), _T("NoteType")).c_str() );
                return pVStaff->AddNote(lm_ePitchRelative,
                                        _T("c"), _T("4"), _T("0"), nAccidentals,
                                        nNoteType, rDuration, fDotted, fDoubleDotted, m_nCurStaff,
                                        m_nCurVoice, fVisible, fBeamed, BeamInfo, fInChord, fTie, nStem);
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
                AnalysisError( _T("Unknown note pitch '%s'. Assumed 'c4'."),
                    sPitch.c_str() );
            }
        }
    }

    //analyze duration and dots
    if (AnalyzeNoteType(sDuration, &nNoteType, &fDotted, &fDoubleDotted)) {
        AnalysisError( _T("Unknown note/rest duration '%s'. A quarter note assumed."),
            sDuration.c_str() );
    }
    m_sLastDuration = sDuration;

    //analyze optional parameters
	lmLDPOptionalTags oOptTags(this, m_pTags);
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
        else if (pX->IsSimple()) {
            //
            // Analysis of simple notations
            //
            sData = pX->GetName();
    //            case "AMR"       //Articulaciones y acentos: marca de respiración
    //                cAnotaciones.Add sData
    //
            if (sData == m_pTags->TagName(_T("l"), _T("SingleChar")) && !fIsRest) {       //Tied to the next one
                fTie = true;
            }
            else if (sData.Left(1) == m_pTags->TagName(_T("g"), _T("SingleChar")))
            {
                if (sData.substr(1,1) == _T("+")) {       //Start of beamed group. Simple parameter
                    //compute beaming level dependig on note type
                    nLevel = GetBeamingLevel(nNoteType);
                    if (nLevel == -1) {
                        AnalysisError(
                            _T("Requesting beaming a note longer than eight. Beaming ignored."));
                    }
                    else {
                        // and the previous note must be beamed
                        if (g_pLastNoteRest && g_pLastNoteRest->IsBeamed() &&
                            g_pLastNoteRest->GetBeamType(0) != eBeamEnd) {
                            AnalysisError(
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
                        AnalysisError(
                            _T("Request to end beaming a group in a note that is note the first one of a chord. Beaming ignored."));
                        fCloseBeam = false;
                    }

                    //There must exist a previous note/rest
                    if (!g_pLastNoteRest) {
                        AnalysisError(
                            _T("Request to end beaming a group but there is not a  previous note. Beaming ignored."));
                        fCloseBeam = false;
                    }
                    else {
                        // and the previous note must be beamed
                        if (!g_pLastNoteRest->IsBeamed() ||
                            g_pLastNoteRest->GetBeamType(0) == eBeamEnd) {
                            AnalysisError(
                                _T("Request to end beaming a group but previous note is not beamed. Beaming ignored."));
                            fCloseBeam = false;
                        }
                    }

                    //proceed to close all previous open levels
                    if (fCloseBeam) {
                        fBeamed = true;
                        int nCurLevel = GetBeamingLevel(nNoteType);
                        int nPrevLevel = GetBeamingLevel(g_pLastNoteRest->GetNoteType());

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
                                if (g_pLastNoteRest->GetBeamType(iLevel) == eBeamContinue) {
                                    g_pLastNoteRest->SetBeamType(iLevel, eBeamEnd);
                                    g_pLogger->LogTrace(_T("LDPParser_beams"),
                                        _T("[lmLDPParser::AnalyzeNoteRest] Changing previous BeamInfo[%d] = eBeamEnd"), iLevel);
                                }
                                else if (g_pLastNoteRest->GetBeamType(iLevel) == eBeamBegin) {
                                    g_pLastNoteRest->SetBeamType(iLevel, eBeamForward);
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
                    AnalysisError(_T("Error: notation '%s' unknown. It will be ignored."), sData.c_str() );
                }

            }

            else if (sData.Left(1) == m_pTags->TagName(_T("t"), _T("SingleChar"))) {       //start/end of tuplet. Simple parameter (tn / t-)
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

            else if (sData.Left(1) == m_pTags->TagName(_T("v"), _T("SingleChar")))
            {	//voice
				m_nCurVoice = AnalyzeVoiceNumber(sData);
			}
            else if (sData == m_pTags->TagName(_T("fermata")))
			{	//fermata
                fFermata = true;
            }
            else {
                AnalysisError(_T("Error: notation '%s' unknown. It will be ignored."), sData.c_str() );
            }

       }

       else     // Analysis of compound notations
       {
            sData = pX->GetName();
            if (sData == m_pTags->TagName(_T("g"), _T("SingleChar")) ) {       //Start of group element
                AnalysisError(_T("Notation '%s' unknown or not implemented. Old (g + t3) syntax?"), sData.c_str());
            }
            else if (sData == m_pTags->TagName(_T("stem")) ) {       //stem attributes
                nStem = AnalyzeStem(pX, pVStaff);
            }
            else if (sData == m_pTags->TagName(_T("fermata")) ) {   //fermata attributes
                fFermata = true;
                nFermataPlacement = AnalyzeFermata(pX, pVStaff, &tFermataPos);
            }
            else if (sData == m_pTags->TagName(_T("t"), _T("SingleChar"))) {       //start/end of tuplet. Simple parameter (tn / t-)
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
            else {
                AnalysisError(_T("Notation '%s' unknown or not implemented."), sData.c_str());
            }

        }
        pX = pNode->GetNextParameter();
    }

    //force beaming for notes between eBeamBegin and eBeamEnd (only for single notes
    //and chord base notes, not for secondary notes of a chord)
    if (!fBeamed && !fInChord && nNoteType > eQuarter) {
        if (g_pLastNoteRest) {
            if (g_pLastNoteRest->IsBeamed()) {
                //it can be the end of a group. Let's verify that at least a beam is open
                for (iLevel=0; iLevel <= 6; iLevel++) {
                    if ((g_pLastNoteRest->GetBeamType(iLevel) == eBeamBegin) ||
                        (g_pLastNoteRest->GetBeamType(iLevel) == eBeamContinue)) {
                            fBeamed = true;
                            break;
                    }
                }

                if (fBeamed) {
                    int nCurLevel = GetBeamingLevel(nNoteType);
                    int nPrevLevel = GetBeamingLevel(g_pLastNoteRest->GetNoteType());

                    // continue common levels
                    for (iLevel=0; iLevel <= wxMin(nCurLevel, nPrevLevel); iLevel++) {
                        BeamInfo[iLevel].Type = eBeamContinue;
                        g_pLogger->LogTrace(_T("LDPParser_beams"),
                            _T("[lmLDPParser::AnalyzeNoteRest] BeamInfo[%d] = eBeamContinue"), iLevel);
                    }

                    if (nCurLevel > nPrevLevel) {
                        // current level is grater than previous one, start new beams
                        for (; iLevel <= nCurLevel; iLevel++) {
                            BeamInfo[iLevel].Type = eBeamBegin;
                            g_pLogger->LogTrace(_T("LDPParser_beams"),
                                _T("[lmLDPParser::AnalyzeNoteRest] BeamInfo[%d] = eBeamBegin"), iLevel);
                        }
                    }
                    else if  (nCurLevel < nPrevLevel) {
                        // current level is lower than previous one
                        // close common levels (done) and close deeper levels in previous note
                        for (; iLevel <= nPrevLevel; iLevel++) {
                            if (g_pLastNoteRest->GetBeamType(iLevel) == eBeamContinue) {
                                g_pLastNoteRest->SetBeamType(iLevel, eBeamEnd);
                                g_pLogger->LogTrace(_T("LDPParser_beams"),
                                    _T("[lmLDPParser::AnalyzeNoteRest] Changing previous BeamInfo[%d] = eBeamEnd"), iLevel);
                            }
                            else if (g_pLastNoteRest->GetBeamType(iLevel) == eBeamBegin) {
                                g_pLastNoteRest->SetBeamType(iLevel, eBeamForward);
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
    if (m_pTuplet) {
        // a tuplet is open
       nActualNotes = m_pTuplet->GetActualNotes();
       nNormalNotes = m_pTuplet->GetNormalNotes();
    }

    // calculation of duration
    rDuration = GetDefaultDuration(nNoteType, fDotted, fDoubleDotted, nActualNotes, nNormalNotes);

    //create the nore/rest
    lmNoteRest* pNR;
    if (fIsRest) {
        pNR = pVStaff->AddRest(nNoteType, rDuration, fDotted, fDoubleDotted,
                               m_nCurStaff, m_nCurVoice, fVisible, fBeamed, BeamInfo);
    }
    else {
        pNR = pVStaff->AddNote(nPitchType,
                               sStep, sOctave, _T("0"), nAccidentals,
                               nNoteType, rDuration, fDotted, fDoubleDotted, m_nCurStaff,
                               m_nCurVoice, fVisible, fBeamed, BeamInfo, fInChord, fTie,
							   nStem);
        m_sLastOctave = sOctave;
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
        wxASSERT(sData.Left(1) == m_pTags->TagName(_T("t"), _T("SingleChar")) );
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
                AnalysisError(_T("[%s] Found unknown tag '%s'. Ignored."),
                    sParent.c_str(), sData.c_str());
                return true;
            }
            else if (nSlash == -1) {
                //abbreviated tuplet: 'tn'
                if (!sNumTuplet.IsNumber()) {
                    AnalysisError(_T("[%s] Found unknown tag '%s'. Ignored."),
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
                        AnalysisError(_T("[%s] Found tag '%s' but no default value exists for NormalNotes. Ignored."),
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
                    AnalysisError(_T("[%s] Found unknown tag '%s'. Ignored."),
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
                        AnalysisError(_T("[%s] Tag '%s'. Numbers must be greater than 0. Tag ignored."),
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
            AnalysisError(_T("Element '%s': invalid type '%s'. It is neither '+' nor '-'. Tuplet ignored."),
                sElmName.c_str(), sType.c_str() );
            return true;    //error
        }

        // get actual notes number
        wxString sNumTuplet = (pNode->GetParameter(2))->GetName();
        if (!sNumTuplet.IsNumber()) {
            AnalysisError(_T("Element '%s': Expected number but found '%s'. Tuplet ignored."),
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
                AnalysisError(_T("Element '%s': Found unknown data '%s'. Data ignored."),
                    sElmName.c_str(), sData.c_str());
            }
            else {
                //check if Normal notes number
                if (sData.IsNumber()) {
                    long nNum;
                    sData.ToLong(&nNum);
                    nNormalNum = (int)nNum;
                    if (nNormalNum < 1) {
                        AnalysisError(_T("Element '%s': Number for 'normal notes' must be greater than 0. Number ignored."),
                            sElmName.c_str(), sData.c_str());
                        return true;
                    }
                }
                else if (sData == m_pTags->TagName(_T("noBracket"), _T("Tuplets")) ) {
                    fShowTupletBracket = false;
                }
                else if (sData == m_pTags->TagName(_T("squaredBracket"), _T("Tuplets")) ) {
                    //TODO implement different kinds of brackets
                    fShowTupletBracket = true;
                }
                else if (sData == m_pTags->TagName(_T("curvedBracket"), _T("Tuplets")) ) {
                    fShowTupletBracket = true;
                }
                else if (sData == m_pTags->TagName(_T("numNone"), _T("Tuplets")) ) {
                    fShowNumber = false;
                }
                else if (sData == m_pTags->TagName(_T("numActual"), _T("Tuplets")) ) {
                    //TODO implement different options to display numbers
                    fShowNumber = true;
                }
                else if (sData == m_pTags->TagName(_T("numBoth"), _T("Tuplets")) ) {
                    fShowNumber = true;
                }
                else {
                    AnalysisError(_T("Element '%s': Found unknown data '%s'. Data ignored."),
                        sElmName.c_str(), sData.c_str());
                }
           }
        }

    }

    //All information parsed. Prepare return info
    if (fEndTuplet) {
        if (!fCloseAllowed) {
            // there isn't an open tuplet
            AnalysisError(_T("[%s] Requesting to end a tuplet but there is not an open tuplet or it is not possible to close it here. Tag '%s' ignored."),
                sParent.c_str(), sData.c_str());
            return true;
        }
        *pTuplet = (lmTupletBracket*) NULL;
    }
    else {
        if (!fOpenAllowed) {
            //there is already a tuplet open and not closed
            AnalysisError(_T("[%s] Requesting to start a tuplet but there is already a tuplet open. Tag '%s' ignored."),
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
    wxASSERT(sElmName == _T("Barra") || sElmName == m_pTags->TagName(_T("barline")) );
    //       ----------------------- compatibility eBooks v1.3

    //check that bar type is specified
    if(pNode->GetNumParms() < 1) {
        //assume simple barline, visible
        pVStaff->AddBarline(lm_eBarlineSimple, true);
		m_nCurVoice = 1;
        return false;
    }

    lmEBarline nType = lm_eBarlineSimple;

    wxString sType = (pNode->GetParameter(1))->GetName();
    if (sType == m_pTags->TagName(_T("endRepetition"), _T("Barlines")) ) {
        nType = lm_eBarlineEndRepetition;
    } else if (sType == m_pTags->TagName(_T("startRepetition"), _T("Barlines")) ) {
        nType = lm_eBarlineStartRepetition;
    } else if (sType == m_pTags->TagName(_T("end"), _T("Barlines")) ) {
        nType = lm_eBarlineEnd;
    } else if (sType == m_pTags->TagName(_T("double"), _T("Barlines")) ) {
        nType = lm_eBarlineDouble;
    } else if (sType == m_pTags->TagName(_T("simple"), _T("Barlines")) ) {
        nType = lm_eBarlineSimple;
    } else if (sType == m_pTags->TagName(_T("start"), _T("Barlines")) ) {
        nType = lm_eBarlineStart;
    } else if (sType == m_pTags->TagName(_T("doubleRepetition"), _T("Barlines")) ) {
        nType = lm_eBarlineDoubleRepetition;
    } else {
        AnalysisError( _T("Unknown barline type '%s'. '%s' barline assumed."),
            sType.c_str(), m_pTags->TagName(_T("simple"), _T("Barlines")).c_str() );
    }

    //analyze remaining optional parameters
    int iP = 2;
	lmLDPOptionalTags oOptTags(this, m_pTags);
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
    //  <clef> = ("clef" {"Sol" | "Fa4" | "Fa3" | "Do1" | "Do2" | "Do3" | "Do4" | "SinClave" }
    //                [<numStaff>] [<Visible>] )

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("Clave") || sElmName == m_pTags->TagName(_T("clef")) );
    //       ----------------------- compatibility eBooks v1.3

    //check that clef type is specified
    if(pNode->GetNumParms() < 1) {
        AnalysisError(
            _T("Element '%s' has less parameters than the minimum required. Assumed '(%s Sol)'."),
            m_pTags->TagName(_T("clef")).c_str(), m_pTags->TagName(_T("clef")).c_str() );
        pVStaff->AddClef(lmE_Sol, 1, true);
        return false;
    }

    long iP = 1;
    wxString sName = (pNode->GetParameter(iP))->GetName();
    lmEClefType nClef;
    if (sName == m_pTags->TagName(_T("treble"), _T("Clefs")) ||
        sName == m_pTags->TagName(_T("G"), _T("Clefs")) )
    {
        nClef = lmE_Sol;
    }
    else if (sName == m_pTags->TagName(_T("bass"), _T("Clefs")) ||
        sName == m_pTags->TagName(_T("F"), _T("Clefs")) )
    {
        nClef = lmE_Fa4;
    }
    else if (sName == m_pTags->TagName(_T("baritone"), _T("Clefs")) ||
        sName == m_pTags->TagName(_T("F3"), _T("Clefs")) )
    {
        nClef = lmE_Fa3;
    }
    else if (sName == m_pTags->TagName(_T("soprano"), _T("Clefs")) ||
        sName == m_pTags->TagName(_T("C1"), _T("Clefs")) )
    {
        nClef = lmE_Do1;
    }
    else if (sName == m_pTags->TagName(_T("mezzosoprano"), _T("Clefs")) ||
        sName == m_pTags->TagName(_T("C2"), _T("Clefs")) )
    {
        nClef = lmE_Do2;
    }
    else if (sName == m_pTags->TagName(_T("alto"), _T("Clefs")) ||
        sName == m_pTags->TagName(_T("C3"), _T("Clefs")) )
    {
        nClef = lmE_Do3;
    }
    else if (sName == m_pTags->TagName(_T("tenor"), _T("Clefs")) ||
        sName == m_pTags->TagName(_T("C4"), _T("Clefs")) )
    {
        nClef = lmE_Do4;
    }
    else if (sName == m_pTags->TagName(_T("percussion"), _T("Clefs")) )
    {
        nClef = lmE_Percussion;
    }
    //else if (sName == m_pTags->TagName(_T("baritoneC"), _T("Clefs")) ||
    //    sName == m_pTags->TagName(_T("C5"), _T("Clefs")) )
    //{
    //    nClef = lmE_Do5;
    //}
    //else if (sName == m_pTags->TagName(_T("subbass"), _T("Clefs")) ||
    //    sName == m_pTags->TagName(_T("F5"), _T("Clefs")) )
    //{
    //    nClef = lmE_Fa5;
    //}
    //else if (sName == m_pTags->TagName(_T("french"), _T("Clefs")) ||
    //    sName == m_pTags->TagName(_T("G1"), _T("Clefs")) )
    //{
    //    nClef = lmE_Sol1;
    //}
    else {
        AnalysisError( _T("Unknown clef '%s'. Assumed '%s'."),
            sName.c_str(), m_pTags->TagName(_T("G")).c_str() );
        nClef = lmE_Sol;
    }
    iP++;

    //analyze optional parameters
	lmLDPOptionalTags oOptTags(this, m_pTags);
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
    wxASSERT(sElmName == m_pTags->TagName(_T("metronome")) );

    //check that at least one parameter is specified
    int nNumParms = pNode->GetNumParms();
    if(nNumParms < 1) {
        AnalysisError(
            _T("Element '%s' has less parameters than the minimum required. Ignored'."),
            sElmName.c_str() );
        return true;    //error
    }

    long iP = 1;
    wxString sName = (pNode->GetParameter(iP))->GetName();

    EMetronomeMarkType nMarkType;
    long nTicksPerMinute = 0;
    bool fDotted = false;
    bool fDoubleDotted = false;
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
        if (AnalyzeNoteType(sData, &nLeftNoteType, &fDotted, &fDoubleDotted)) {
            AnalysisError( _T("Unknown note/rest duration '%s'. A quarter note assumed."),
                sData.c_str() );
        }
        if (fDotted) nLeftDots++;
        if (fDoubleDotted) nLeftDots++;

        // Get right part
        if (iP > nNumParms) {
            AnalysisError(
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
            if (AnalyzeNoteType(sData, &nRightNoteType, &fDotted, &fDoubleDotted)) {
                AnalysisError( _T("Unknown note/rest duration '%s'. A quarter note assumed."),
                    sData.c_str() );
            }
            if (fDotted) nRightDots++;
            if (fDoubleDotted) nRightDots++;
        }
    }

    //Get optional 'parentheses' parameter
    bool fParentheses = false;
    lmLDPNode* pX = pNode->GetParameter( (wxString&)m_pTags->TagName(_T("parentheses")) );
    if (pX) fParentheses = true;

    //Get common optional parameters
	lmLDPOptionalTags oOptTags(this, m_pTags);
	oOptTags.SetValid(lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1
	oOptTags.SetValid(lm_eTag_Visible, lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1

	lmLocation tPos = g_tDefaultPos;
    bool fVisible = true;

	oOptTags.AnalyzeCommonOptions(pNode, iP, pVStaff, &fVisible, NULL, &tPos);

    //bool fVisible = true;
    //for (; iP <= nNumParms; iP++) {
    //    pX = pNode->GetParameter(iP);
    //    if (pX->GetName() == m_pTags->TagName(_T("noVisible")) )
    //        fVisible = false;
    //    else if (pX->GetName() == m_pTags->TagName(_T("parentheses")) )
    //        fParentheses = true;
    //    else {
    //        AnalysisError( _T("Unknown parameter '%s'. Ignored."), pX->GetName().c_str());
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
    wxASSERT(sElmName == m_pTags->TagName(_T("opt")) );

    //check that there are 2 parameters (name and value)
    if(pNode->GetNumParms() != 2) {
        AnalysisError( _T("Element '%s' needs exactly %d parameters. Tag ignored."),
            m_pTags->TagName(_T("opt")).c_str(), 2);
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
        AnalysisError( _T("Option '%s' unknown. Ignored."), sName.c_str());
        return;
    }

    //get value
    long nNumberLong;
    double rNumberDouble;
    //bool fError = false;
	wxString sError;

    switch(nDataType) {
        case lmBoolean:
            if (sValue == _T("true") || sValue == m_pTags->TagName(_T("yes")) ) {
                pObject->SetOption(sName, true);
                return;
            }
            else if (sValue == _T("false") || sValue == m_pTags->TagName(_T("no")) ) {
                pObject->SetOption(sName, false);
                return;
            }
            else {
                wxString sError = _T("a 'yes/no' or 'true/false' value");
                AnalysisError( _T("Error in data value for option '%s'.  It requires %s. Value '%s' ignored."),
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
                AnalysisError( _T("Error in data value for option '%s'.  It requires %s. Value '%s' ignored."),
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
            AnalysisError( _T("Error in data value for option '%s'.  It requires %s. Value '%s' ignored."),
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
                    AnalysisError( _T("Error in data value for option '%s'.  Value '%s' ignored."),
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

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("title")) );

    //check that at least two parameters (aligment and text string) are specified
    if(pNode->GetNumParms() < 2) {
        AnalysisError(
            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
            m_pTags->TagName(_T("title")).c_str() );
        return true;
    }

    wxString sTitle;
    lmEAlignment nAlign = m_nTitleAlignment;
    lmFontInfo tFont = {m_sTitleFontName, m_nTitleFontSize, m_nTitleStyle};
    lmLocation tPos;
    tPos.xType = lmLOCATION_DEFAULT;
    tPos.xUnits = lmTENTHS;
    tPos.yType = lmLOCATION_DEFAULT;
    tPos.yUnits = lmTENTHS;
    tPos.x = 0.0f;
    tPos.y = 0.0f;

    //get the aligment
    long iP = 1;
    wxString sName = (pNode->GetParameter(iP))->GetName();
    if (sName == m_pTags->TagName(_T("left")) )
        nAlign = lmALIGN_LEFT;
    else if (sName == m_pTags->TagName(_T("right")) )
        nAlign = lmALIGN_RIGHT;
    else if (sName == m_pTags->TagName(_T("center")) )
        nAlign = lmALIGN_CENTER;
    else {
        AnalysisError( _T("Invalid alignment value '%s'. Assumed '%s'."),
            sName.c_str(), m_pTags->TagName(_T("center")).c_str() );
        nAlign = lmALIGN_CENTER;
    }
    //save alignment as new default for titles
    m_nTitleAlignment = nAlign;
    iP++;

    //get the string
    sTitle = (pNode->GetParameter(iP))->GetName();
    iP++;

    //analyze remaining parameters (optional): font, location
    lmLDPNode* pX;
    for(; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);
        sName = pX->GetName();

        if (sName == m_pTags->TagName(_T("font")) ) {
            AnalyzeFont(pX, &tFont);
            //save font values as new default for titles
            m_sTitleFontName = tFont.sFontName;
            m_nTitleFontSize = tFont.nFontSize;
            m_nTitleStyle = tFont.nStyle;
        }
        else if (sName == m_pTags->TagName(_T("x")) || sName == m_pTags->TagName(_T("dx")) ||
                 sName == m_pTags->TagName(_T("y")) || sName == m_pTags->TagName(_T("dy")) )
        {
            AnalyzeLocation(pX, &tPos);
        }
        else {
            AnalysisError( _T("Unknown parameter '%s'. Ignored."), sName.c_str());
        }
    }

    //create the title
    lmScoreText* pTitle = pScore->AddTitle(sTitle, nAlign, tPos, tFont.sFontName,
                                           tFont.nFontSize, tFont.nStyle);
	pTitle->SetUserLocation(tPos);

    return false;

}

bool lmLDPParser::AnalyzeTextString(lmLDPNode* pNode, wxString* pText,
                                    lmEAlignment* pAlign, lmLocation* pPos,
                                    lmFontInfo* pFont, bool* pHasWidth)
{
    //A certain number of LDP elements accepts a text-string with additional parameters,
    //such as location font or alignment. This method parses these elements.
    //Default values for information not present must be initialized in return variables
    //before invoking this method.
    //Returns true if error; in this case return variables are not changed.
    //If no error all variables but pNode are loaded with parsed information

    // <text-string> = (any-tag string [<location>][<font>][<alingment>])

    //check that at least one parameter (text string) is specified
    if(pNode->GetNumParms() < 1) {
        AnalysisError(
            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
            pNode->GetName().c_str() );
        return true;
    }

    wxString sText;
    lmEAlignment nAlign = *pAlign;
    lmFontInfo tFont = {pFont->sFontName, pFont->nFontSize, pFont->nStyle};
    lmLocation tPos = *pPos;
    bool fHasWidth = *pHasWidth;

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

        if (sName == m_pTags->TagName(_T("x")) || sName == m_pTags->TagName(_T("dx")) ||
                 sName == m_pTags->TagName(_T("y")) || sName == m_pTags->TagName(_T("dy")) )
        {
            AnalyzeLocation(pX, &tPos);
        }
        else if (sName == m_pTags->TagName(_T("font")) ) {
            AnalyzeFont(pX, &tFont);
        }
        else if (sName == m_pTags->TagName(_T("left")) ) {
            nAlign = lmALIGN_LEFT;
        }
        else if (sName == m_pTags->TagName(_T("right")) ) {
            nAlign = lmALIGN_RIGHT;
        }
        else if (sName == m_pTags->TagName(_T("center")) ) {
            nAlign = lmALIGN_CENTER;
        }
        else if (sName == m_pTags->TagName(_T("hasWidth")) ) {
            fHasWidth = true;
        }
        else {
            AnalysisError( _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
                pNode->GetName().c_str(), sName.c_str() );
        }
    }

    //return parsed values
    *pText = sText;
    *pAlign = nAlign;
    *pPos = tPos;
    *pFont = tFont;
    *pHasWidth = fHasWidth;
    return false;

}

//returns true if error; in this case nothing is added to the VStaff
bool lmLDPParser::AnalyzeText(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    // <text> = (text string <location>[<font><alingment>])

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("text")) );

    //check that at least two parameters (location and text string) are specified
    if(pNode->GetNumParms() < 2) {
        AnalysisError(
            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
            m_pTags->TagName(_T("text")).c_str() );
        return true;
    }

    wxString sText;
    lmEAlignment nAlign = lmALIGN_LEFT;     //TODO user options instead of fixed values
    bool fHasWidth = false;
    lmFontInfo tFont = {m_sTextFontName, m_nTextFontSize, m_nTextStyle};
    lmLocation tPos;
    tPos.xType = lmLOCATION_DEFAULT;
    tPos.xUnits = lmTENTHS;
    tPos.yType = lmLOCATION_DEFAULT;
    tPos.yUnits = lmTENTHS;
    tPos.x = 0.0f;
    tPos.y = 0.0f;

    if (AnalyzeTextString(pNode, &sText, &nAlign, &tPos, &tFont, &fHasWidth)) return true;

    //no error:
    //save font values as new default for titles
    m_sTextFontName = tFont.sFontName;
    m_nTextFontSize = tFont.nFontSize;
    m_nTextStyle = tFont.nStyle;

    //create the text
    lmScoreText* pText = pVStaff->AddText(sText, nAlign, &tPos, tFont, fHasWidth);
    pText->SetUserLocation(tPos);

    return false;

}

bool lmLDPParser::AnalyzeKeySignature(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //returns true if error; in this case nothing is added to the lmVStaff
    //  <KeySignature> ::= (key {"Do" | "Sol" | "Re" | "La" | "Mi" | "Si" | "Fa+" |
    //                        | "Sol-" | "Re-" | "La-" | "Mi-" | "Si-" | "Fa" } [<Visible>])

    wxString sElmName = pNode->GetName();
    wxASSERT(sElmName == _T("Tonalidad") || sElmName == m_pTags->TagName(_T("key")) );
    //       --------------------------- compatibility eBooks v1.3

    //check that key value is specified
    if(pNode->GetNumParms() < 1) {
        AnalysisError(
            _T("Element '%s' has less parameters than the minimum required. Assumed '(%s %s)'."),
            sElmName.c_str(), sElmName.c_str(), m_pTags->TagName(_T("Do"), _T("Keys")).c_str() );
        pVStaff->AddKeySignature(earmDo);
        return false;
    }

    long iP = 1;
    wxString sName = (pNode->GetParameter(iP))->GetName();
    lmEKeySignatures nKey;
    wxString sKey = m_pTags->GetInternalTag(sName, _T("Keys"));
    if (sKey == _T("")) {
        //not found.
        AnalysisError( _T("Unknown key '%s'. Assumed '%s'."),
            sName.c_str(), m_pTags->TagName(_T("Do"), _T("Keys")).c_str() );
        nKey = earmDo;
    }
    else {
        //found.
        nKey = LDPInternalNameToKey( sKey );
    }
    iP++;

    //analyze optional parameters
	lmLDPOptionalTags oOptTags(this, m_pTags);
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
    wxASSERT(sElmName == _T("Metrica") || sElmName == m_pTags->TagName(_T("time")) );
    //       ------------------------- compatibility eBooks v1.3

    //check that the two numbers are specified
    if(pNode->GetNumParms() < 2) {
        AnalysisError( _T("Element '%s' has less parameters than the minimum required. Assumed '(Metrica 4 4)'."),
            m_pTags->TagName(_T("time")).c_str() );
        pVStaff->AddTimeSignature(emtr44);
        return false;
    }

    wxString sNum1 = (pNode->GetParameter(1))->GetName();
    wxString sNum2 = (pNode->GetParameter(2))->GetName();
    if (!sNum1.IsNumber() || !sNum2.IsNumber()) {
        AnalysisError(
            _T("Element '%s': Two numbers expected but found '%s' and '%s'. Assumed '(%s 4 4)'."),
            m_pTags->TagName(_T("time")).c_str(), sNum1.c_str(),
            sNum2.c_str(), m_pTags->TagName(_T("time")).c_str() );
        pVStaff->AddTimeSignature(emtr44);
        return false;
    }

    long nBeats, nBeatType;
    sNum1.ToLong(&nBeats);
    sNum2.ToLong(&nBeatType);

    //analyze optional parameters
	lmLDPOptionalTags oOptTags(this, m_pTags);
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
    if(pNode->GetNumParms() < 1) {
        AnalysisError( _T("Element '%s' has less parameters than the minimum required. Ignored."),
            sElmName.c_str());
        return;
    }

    wxString sNum1 = (pNode->GetParameter(1))->GetName();
    if (!sNum1.IsNumber()) {
        AnalysisError(
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
    if(nNumParms < 2) {
        AnalysisError( _T("Element '%s' has less parameters than the minimum required. Element ignored."),
            sElmName.c_str());
        return;
    }

    // analyze type and get its params.
    int iP = 1;
    wxString sType = (pNode->GetParameter(iP))->GetName();
    if (sType == m_pTags->TagName(_T("line"), _T("GraphObjs")))
    {
        // line
        // Parms: xStart, yStart, xEnd, yEnd, nWidth, colour.
        // All coordinates in tenths, relative to current pos.
        // line width in tenths (optional parameter). Default: 1 tenth
        // colour (optional parameter). Default: black

        // get parameters
        if(nNumParms < 5) {
            AnalysisError( _T("Element '%s' has less parameters than the minimum required. Element ignored."),
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
                    _T("Element '%s': Coordinate expected but found '%s'. Ignored."),
                    sElmName.c_str(), sNum.c_str());
                return;
            }
            sNum.ToLong(&nPos);
            rPos[iP-2] = (lmTenths)nPos;
        }

        // get line width (optional parameter). Default: 1 tenth
        //@todo
        lmTenths rWidth = 1;

        // get colour (optional parameter). Default: black
        //@todo
        wxColour nColor = *wxBLACK;

        // create the AuxObj and attach it to the VStaff
        lmScoreLine* pLine = new lmScoreLine(rPos[0], rPos[1], rPos[2], rPos[3],
                                       rWidth, nColor);
        lmStaffObj* pAnchor = (lmStaffObj*) pVStaff->AddAnchorObj();
        pAnchor->AttachAuxObj(pLine);

    }
    else {
        AnalysisError(
            _T("Element '%s': Type of graphic (%s) unknown. Ignored."),
            sElmName.c_str(), sType.c_str());
    }

}

//returns true if error; in this case nothing is added to the lmVStaff
bool lmLDPParser::AnalyzeNewSystem(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //<newSystem> ::= (newSystem}

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("newSystem")) );

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

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("stem")) );

    lmEStemType nStem = lmSTEM_DEFAULT;

    //check that there are parameters
    if(pNode->GetNumParms() < 1) {
        AnalysisError( _T("Element '%s' has less parameters than the minimum required. Tag ignored. Assumed default stem."),
            m_pTags->TagName(_T("stem")).c_str());
        return nStem;
    }

    //get stem direction
    wxString sDir = (pNode->GetParameter(1))->GetName();
    if (sDir == m_pTags->TagName(_T("up")) )
        nStem = lmSTEM_UP;
    else if (sDir == m_pTags->TagName(_T("down")) )
        nStem = lmSTEM_DOWN;
    else {
        AnalysisError( _T("Invalid stem direction '%s'. Default direction taken."), sDir.c_str());
    }

    return nStem;

}

lmEPlacement lmLDPParser::AnalyzeFermata(lmLDPNode* pNode, lmVStaff* pVStaff, lmLocation* pPos)
{
    //<Fermata> ::= (fermata [above | below]}

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("fermata")) );

    lmEPlacement nPlacement = ep_Default;

    //check that there are parameters
    if(pNode->GetNumParms() < 1) {
        AnalysisError( _T("Element '%s' has less parameters than the minimum required. Tag ignored. Assumed default stem."),
            m_pTags->TagName(_T("fermata")).c_str() );
        return nPlacement;
    }

    //get fermata placement
    wxString sDir = (pNode->GetParameter(1))->GetName();
    if (sDir == m_pTags->TagName(_T("above")) )
        nPlacement = ep_Above;
    else if (sDir == m_pTags->TagName(_T("below")) )
        nPlacement = ep_Below;
    else {
        AnalysisError( _T("Invalid fermata placement '%s'. Default placement assumed."), sDir.c_str());
    }

    //analyze optional parameters
	lmLDPOptionalTags oOptTags(this, m_pTags);
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

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("font")) );

    //check that there are parameters
    if (!(pNode->GetNumParms() > 0)) {
        AnalysisError( _T("Element '%s' has less parameters than the minimum required. Tag ignored."),
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
            //try style
            fStyle = true;
            fProcessed = true;
            if (sParm == m_pTags->TagName(_T("bold")) )
                tFont.nStyle = lmTEXT_BOLD;
            else if (sParm == m_pTags->TagName(_T("normal")) )
                tFont.nStyle = lmTEXT_NORMAL;
            else if (sParm == m_pTags->TagName(_T("italic")) )
                tFont.nStyle = lmTEXT_ITALIC;
            else if (sParm == m_pTags->TagName(_T("bold-italic")) )
                tFont.nStyle = lmTEXT_ITALIC_BOLD;
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
            AnalysisError( _T("Element '%s': invalid parameter '%s'. It is ignored."),
                m_pTags->TagName(_T("font")).c_str(), sParm.c_str() );
        }
    }

    *pFont = tFont;

}

void lmLDPParser::AnalyzeLocation(lmLDPNode* pNode, float* pValue, lmEUnits* pUnits)
{
    // <location> = (x num) | (y num) | (dx num) | (dy num)
    // <num> = number [units]

    //returns, in variables pointed by pValue and pUnits the
    //result of the analysis.

    //check that there are parameters
    if (pNode->GetNumParms()!= 1) {
        AnalysisError( _T("Element '%s' has less or more parameters than required. Tag ignored."),
            pNode->GetName().c_str() );
        return;
    }

    //get value
    wxString sParm = (pNode->GetParameter(1))->GetName();
    wxString sValue = sParm;
    wxString sUnits = sParm.Right(2);
	if (sUnits.at(0) != _T('.') && !sUnits.IsNumber() )
	{
		sValue = sParm.Left(sParm.length() - 2);
        if (sUnits == _T("mm")) {
            *pUnits = lmMILLIMETERS;
        }
        else if (sUnits == _T("cm")) {
            *pUnits = lmCENTIMETERS;
        }
        else if (sUnits == _T("in")) {
            *pUnits = lmINCHES;
        }
        else {
            AnalysisError( _T("Element '%s': Invalid units '%s'. Ignored"),
                pNode->GetName().c_str(), sUnits.c_str() );
            return;
        }
    }

	double rNumberDouble;
	if (!StrToDouble(sValue, &rNumberDouble))
	{
        *pValue = (float)rNumberDouble;
    }
    //if (sValue.IsNumber()) {
    //    sValue.ToLong(&nValue);
    //    *pValue = (int)nValue;
    //}
    else {
        AnalysisError( _T("Element '%s': Invalid value '%s'. It must be a number with optional units. Zero assumed."),
            pNode->GetName().c_str(), sParm.c_str() );
        *pValue = 0.0;
    }

}

void lmLDPParser::AnalyzeLocation(lmLDPNode* pNode, lmLocation* pPos)
{
    //analyze location
    wxString sName = pNode->GetName();

    wxASSERT(sName == m_pTags->TagName(_T("x")) || sName == m_pTags->TagName(_T("dx")) ||
        sName == m_pTags->TagName(_T("y")) || sName == m_pTags->TagName(_T("dy")) );

    float rValue;
    lmEUnits nUnits = lmTENTHS;     //default value
    AnalyzeLocation(pNode, &rValue, &nUnits);
    if (sName == m_pTags->TagName(_T("x")) ) {
        //x
        pPos->x = rValue;
        pPos->xType = lmLOCATION_USER_ABSOLUTE;
        pPos->xUnits = nUnits;
    }
    else if (sName == m_pTags->TagName(_T("dx")) ) {
        //dx
        pPos->x = rValue;
        pPos->xType = lmLOCATION_USER_RELATIVE;
        pPos->xUnits = nUnits;
    }
    else if (sName == m_pTags->TagName(_T("y")) ) {
        //y
        pPos->y = rValue;
        pPos->yType = lmLOCATION_USER_ABSOLUTE;
        pPos->yUnits = nUnits;
    }
    else {
        //dy
        pPos->y = rValue;
        pPos->yType = lmLOCATION_USER_RELATIVE;
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
//            AnalysisError(wxString::Format(_T("[AnalizarDirectivaRepeticion]: Valor <" & sNum & _
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
//            AnalysisError(wxString::Format(_T("Signo de repetición <" & sDuration & "> desconocido. " & _
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

int lmLDPParser::AnalyzeNumStaff(const wxString& sNotation, long nNumStaves)
{
    //analyzes a notation Pxx.  xx must be lower or equal than nNumStaves

    if (sNotation.Left(1) != m_pTags->TagName(_T("p"), _T("SingleChar")) ) {
        AnalysisError( _T("Staff number expected but found '%s'. Replaced by '%s1'"),
            sNotation.c_str(), m_pTags->TagName(_T("p"), _T("SingleChar")).c_str() );
        return 1;
    }

    wxString sData = sNotation.substr(1);         //remove char 'p'
    if (!sData.IsNumber()) {
        AnalysisError( _T("Staff number not followed by number (%s). Replaced by '%s1'"),
            sNotation.c_str(), m_pTags->TagName(_T("p"), _T("SingleChar")).c_str() );
        return 1;
    }

    long nValue;
    sData.ToLong(&nValue);
    if (nValue > nNumStaves) {
        AnalysisError( _T("Notation '%s': number is greater than number of staves defined (%d). Replaced by '%s1'."),
            sNotation.c_str(), nNumStaves, m_pTags->TagName(_T("p"), _T("SingleChar")).c_str() );
        return 1;
    }
    return (int)nValue;

}

int lmLDPParser::AnalyzeVoiceNumber(const wxString& sNotation)
{
    //analyzes a notation Vx.  x must be 1..lmMAX_VOICE

    if (sNotation.Left(1) != m_pTags->TagName(_T("v"), _T("SingleChar")) ) {
        AnalysisError( _T("Voice number expected but found '%s'. Replaced by '%s1'"),
            sNotation.c_str(), m_pTags->TagName(_T("v"), _T("SingleChar")).c_str() );
        return 1;
    }

    wxString sData = sNotation.substr(1);         //remove char 'v'
    if (!sData.IsNumber()) {
        AnalysisError( _T("Voice number expected but found '%s'. Replaced by '%s1'"),
            sNotation.c_str(), m_pTags->TagName(_T("v"), _T("SingleChar")).c_str() );
        return 1;
    }

    long nValue;
    sData.ToLong(&nValue);
    if (nValue > lmMAX_VOICE) {
        AnalysisError( _T("Notation '%s': number is greater than supported voices (%d). Replaced by '%s1'."),
            sNotation.c_str(), lmMAX_VOICE, m_pTags->TagName(_T("v"), _T("SingleChar")).c_str() );
        return 1;
    }
    return (int)nValue;

}

float lmLDPParser::GetDefaultDuration(lmENoteType nNoteType, bool fDotted, bool fDoubleDotted,
                      int nActualNotes, int nNormalNotes)
{
    //compute duration without modifiers
    float rDuration = NoteTypeToDuration(nNoteType, fDotted, fDoubleDotted);

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
                                  bool* pfDotted, bool* pfDoubleDotted)
{
    // Receives a string (sNoteType) with the LDP letter for the type of note and, optionally,
    // dots "."
    // Set up variables nNoteType and flags fDotted and fDoubleDotted.
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
    else if (sType == m_pTags->TagName(_T("l"), _T("NoteType")))
        *pnNoteType = eLonga;
    else if (sType == m_pTags->TagName(_T("d"), _T("NoteType")))
        *pnNoteType = eBreve;
    else if (sType == m_pTags->TagName(_T("r"), _T("NoteType")))
        *pnNoteType = eWhole;
    else if (sType == m_pTags->TagName(_T("b"), _T("NoteType")))
        *pnNoteType = eHalf;
    else if (sType == m_pTags->TagName(_T("n"), _T("NoteType")))
        *pnNoteType = eQuarter;
    else if (sType == m_pTags->TagName(_T("c"), _T("NoteType")))
        *pnNoteType = eEighth;
    else if (sType == m_pTags->TagName(_T("s"), _T("NoteType")))
        *pnNoteType = e16th;
    else if (sType == m_pTags->TagName(_T("f"), _T("NoteType")))
        *pnNoteType = e32th;
    else if (sType == m_pTags->TagName(_T("m"), _T("NoteType")))
        *pnNoteType = e64th;
    else if (sType == m_pTags->TagName(_T("g"), _T("NoteType")))
        *pnNoteType = e128th;
    else if (sType == m_pTags->TagName(_T("p"), _T("NoteType")))
        *pnNoteType = e256th;
    else
        return true;    //error

    //analyze dots
    *pfDotted = false;
    *pfDoubleDotted = false;
    if (sDots.length() > 0) {
        if (sDots.StartsWith( _T("..") )) {
            *pfDoubleDotted = true;
        } else if (sDots.StartsWith( _T(".") )) {
            *pfDotted = true;
        } else {
            return true;    //error
        }
    }

    return false;   //no error

}




//-----------------------------------------------------------------------------------------
// lmLDPOptionalTags implementation: Helper class to analyze optional elements
//-----------------------------------------------------------------------------------------

lmLDPOptionalTags::lmLDPOptionalTags(lmLDPParser* pParser, lmLdpTagsTable* pTags)
{
	m_pParser = pParser;
	m_pTags = pTags;

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

bool lmLDPOptionalTags::VerifyAllowed(lmETagLDP nTag, wxString sName)
{
	if (m_ValidTags[nTag]) return true;

	//tag invalid. Log error message
    m_pParser->AnalysisError(
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
        if (sName.Left(1) == m_pTags->TagName(_T("p"), _T("SingleChar")))
        {
			if (VerifyAllowed(lm_eTag_StaffNum, sName)) {
				*pStaffNum = m_pParser->AnalyzeNumStaff(sName, pVStaff->GetNumStaves());
			}
            pX->SetProcessed(true);
        }

		//visible or not
        else if (sName == m_pTags->TagName(_T("noVisible")))
		{
			if (VerifyAllowed(lm_eTag_Visible, sName)) {
				*pfVisible = false;
			}
            pX->SetProcessed(true);
        }

		// X location
        else if (sName == m_pTags->TagName(_T("x")) || sName == m_pTags->TagName(_T("dx")) )
        {
			if (VerifyAllowed(lm_eTag_Location_x, sName)) {
				m_pParser->AnalyzeLocation(pX, pLocation);
			}
            pX->SetProcessed(true);
		}

		// Y location
        else if (sName == m_pTags->TagName(_T("y")) || sName == m_pTags->TagName(_T("dy")) )
        {
			if (VerifyAllowed(lm_eTag_Location_y, sName)) {
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

