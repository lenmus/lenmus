//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
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
/*! @file LDPParser.cpp
    @brief Implementation file for class lmLDPParser
    @ingroup ldp_parser
*/
/*! @class lmLDPParser
    @ingroup ldp_parser
    @brief The parser for the LDP language

    - "Parse" functions: work on source text
    - "Analyze" functions: work on a tree of LMNodes

*/
#ifdef __GNUG__
// #pragma implementation
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/textfile.h"
#include "wx/log.h"
#include <math.h>        // for function pow()

#include "../score/Score.h"
#include "LDPParser.h"
#include "AuxString.h"
#include "../auxmusic/Conversion.h"
#include "LDPTags.h"


//the next three lines ar for getting the main frame for wxMessageBox
#include "../app/TheApp.h"
#include "../app/MainFrame.h"
extern lmMainFrame *GetMainFrame();

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

const wxString sEOF = _T("<<$$EOF$$>>");

lmLDPParser::lmLDPParser()
{
    m_pTokenizer = new lmLDPTokenBuilder(this);
    m_pCurNode = (lmLDPNode*) NULL;
    m_fDebugMode = false;
    m_pTupletBracket = (lmTupletBracket*)NULL;
    m_pTags = lmLdpTagsTable::GetInstance();
    m_pTags->LoadTags(_T("es"), _T("iso-8859-1"));      //default tags in Spanish

    // default values for font and aligment for <title> elements
    //! @todo user options instead of fixed values
    m_nTitleAlignment = lmALIGN_CENTER;
    m_sTitleFontName = _T("Times New Roman");
    m_nTitleFontSize = 14; 
    m_nTitleStyle = lmTEXT_BOLD;

    // default values for font and aligment for <text> elements
    //! @todo user options instead of fixed values
    m_sTextFontName = _T("Times New Roman");
    m_nTextFontSize = 10; 
    m_nTextStyle = lmTEXT_NORMAL;


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

    long i = m_stackNodes.GetCount();
    for(; i > 0; i--) {
        //if (m_fDebugMode) {
        //    lmLDPNode* pNode = (lmLDPNode*)m_stackNodes.Item(i-1);
        //    wxLogMessage(
        //        _T("**TRACE**         lmLDPParser destructor:  deleting stackNode %d, name=<%s>"),
        //        i, pNode->GetName() );
        //}
         delete m_stackNodes.Item(i-1);
        m_stackNodes.RemoveAt(i-1);
    }

    delete m_pCurNode;
    m_pCurNode = (lmLDPNode*) NULL;
}


lmScore* lmLDPParser::ParseFile(const wxString& filename) 
{
    wxFileInputStream inFile(filename);
    if (!inFile.Ok()) {
        FileParsingError(wxString::Format( _T("Error opening file <%s>"), filename));
        return (lmScore*) NULL;
    }

    wxTextInputStream inTextFile(inFile);

    m_pTextFile = &inTextFile;
    m_pFile = &inFile;
    m_fFromString = false;        //a file is going to be parsed, not a string
    m_nErrors = 0;
    m_nWarnings = 0;
    lmLDPNode* pRoot = LexicalAnalysis();
    lmScore* pScore = (lmScore*) NULL;
    if (pRoot) 
        pScore = AnalyzeScore(pRoot);

    // report errors
    bool fShowLog = true;
    if (fShowLog && m_nErrors != 0) {
        g_pLogger->ShowDataErrors(_("Warnings/errores while reading LenMus score."));
    }

    if (pScore) pScore->Dump(_T("lenmus_score_dump.txt"));      //dbg
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
    int i = sSource.Length();
    int nPar = 0;
    for(i=0; i < (int)sSource.Length(); i++) {
        if (sSource.GetChar(i) == _T('('))
            nPar++;
        else if (sSource.GetChar(i) == _T(')'))
            nPar--;
    }
    return (nPar == 0);

}


void lmLDPParser::FileParsingError(const wxString sMsg)
{
    wxMessageBox(sMsg, _T("Error"), wxOK, GetMainFrame() );
}

//void lmLDPParser::InformarIncidencias()
//
//    if (m_nWarnings > 0 Or m_nErrors > 0) {
//        Dim sMsje As String
//        if (m_nWarnings) {
//            sMsje = sMsje & "Hay " & m_nWarnings & " aviso"
//            if (m_nWarnings > 1) { sMsje = sMsje & "s"
//            if (m_nErrors > 0) { sMsje = sMsje & " y " & m_nErrors & " error"
//        } else {
//            sMsje = "Hay " & m_nErrors & " error"
//        }
//        if (m_nErrors > 1) { sMsje = sMsje & "es"
//        MsgBox sMsje, vbCritical, "Errores al analizar partitura"
//        //mostrar el archivo de avisos
//        if (m_nWarnings != 0 Or m_nErrors != 0) { MostrarErrores
//    }
//    
//}
//    

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
    
    if (m_fDebugMode)
        wxLogMessage( _T("**WARNING** %s"), sMsg );
    
}

void lmLDPParser::ParseError(EParsingStates nState, lmLDPToken* pTk)
{
    m_nErrors++;
    wxLogMessage(_T("** LDP ERROR **: Syntax error. State %d, TkType %s, tkValue <%s>"),
            nState, pTk->GetDescription(), pTk->GetValue() );
    
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

//void lmLDPParser::MsjeDebug(sEstado As String, m_pTk As lmLDPToken, sMsje As String)
//    Dim sLit As String
//    sLit = _
//        "Estado: " & sEstado & ", Tipo token:" & m_pTk->GetDescription & sCrLf & _
//        "Token: [" & m_pTk->GetValue() & "]" & sCrLf & _
//        sMsje & sCrLf & _
//        "Línea " & nNumLinea & sCrLf & m_sLastBuffer
//    GrabarTrace sLit
//    
//}
//
//
//
//
//
//


//Enum ETies
//    eL_NotTied = 0
//    eL_Tied
//End Enum
//
//Enum ECalderon
//    eC_SinCalderon = 0
//    eC_ConCalderon
//End Enum
//
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
    m_stackNodes.Clear();
    m_nErrors = 0;
    m_nWarnings = 0;

    m_nNumStaves = 1;
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
                    m_nState, m_pTk->GetDescription(), m_pTk->GetValue(), m_pCurNode->GetName() );
            } else {
                wxLogMessage(
                    _T("**TRACE** State %d, TkType %s, tkValue <%s>"),
                    m_nState, m_pTk->GetDescription(), m_pTk->GetValue() );
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
        //! @todo delete node objects. Clear() is not enough. Something else is needed.
        return (lmLDPNode*) NULL;
    }
    
    // at this point m_pCurNode is all the tree. Verify it.
    if (m_pCurNode->GetName() != _T("Root Node")) {
        AnalysisError( _("Element RAIZ expected but found element %s. Analysis stopped."), 
            m_pCurNode->GetName() );
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
                    m_nState, m_pTk->GetDescription(), m_pCurNode->GetName() );
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
    m_stackNodes.Add(m_pCurNode);    //Append(m_pCurNode);
    m_nLevel++;

    if (m_fDebugMode) {
        wxLogMessage( _T("**TRACE** PushNode - Stored values:  State %d, node <%s>. stack count=%d"),
            nPopState, m_pCurNode->GetName(), m_stackNodes.GetCount() );
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
        AnalysisError(_("Syntax error: more closing parenthesis than parenthesis opened. Analysis stopped."));
        return true;    //error
    }
    i--;    // the array is 0 based
    EParsingStates curState = m_nState;
    m_nState = (EParsingStates) m_stackStates.Item(i);
    m_stackStates.RemoveAt(i);
    m_pCurNode = (lmLDPNode*)m_stackNodes.Item(i);
    m_stackNodes.RemoveAt(i);

    // if debug mode print message
    if (m_fDebugMode) {
        wxLogMessage( _T("**TRACE** PopNode - State %d, New Values: State %d, node <%s>"),
            curState, m_nState, m_pCurNode->GetName() );
    }

    return false;       //no error
}

lmScore* lmLDPParser::AnalyzeScore(lmLDPNode* pNode)
{
    lmScore* pScore = (lmScore*) NULL;
    int i;

    if (!(pNode->GetName() == _T("score") || pNode->GetName() == _T("Score")) ) {
        AnalysisError( _("Element 'score' expected but found element %s. Analysis stopped."),
            pNode->GetName() );
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
        AnalysisError( _("Element 'vers' expected but found element %s. Analysis stopped."),
            pX->GetName() );
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
            AnalysisError( _("Error analysing LDP score: LDP version (%d) not supported. Analysis stopped."),
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
        //! @todo no treatment yet. Ignore
        iP++;
    }
    
    // parse element <NumInstruments>
    long nInstruments = 0;
    pX = pNode->GetParameter(iP);
    if (pX->GetName() != _T("NumInstrumentos")) {
        AnalysisError( _("Element 'NumInstrumentos' expected but found element %s. Analysis stopped."),
            pX->GetName() );
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

    //parse element <titles>
    pX = pNode->GetParameter(iP);
    while(pX->GetName() == m_pTags->TagName(_T("title")) &&  iP <= nNumParms) {
        AnalyzeTitle(pX, pScore);
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
    //<Voice> = (voice <music>+ )
    
    lmLDPNode* pX;
    wxString sData;
    long iP;
    iP = 1;
    
    if (pNode->GetName() != m_pTags->TagName(_T("instrument")) ) {
        AnalysisError( _("Element '%s' expected but found element %s. Analysis stopped."),
            m_pTags->TagName(_T("instrument")), pNode->GetName() );
        return;
    }

    //default values
    int nMIDIChannel=0, nMIDIInstr=0;       //default MIDI values: channel 0, instr=Piano
    bool fMusicFound = false;               // <voice> tag found
    wxString sNumStaves = _T("1");          //one staff
    wxString sInstrName = _T("");           //no name for instrument
    wxString sInstrAbbrev = _T("");         //no abreviated name for instrument

    // parse optional elements until <voice> tag found
    for (; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);

        if (pX->GetName() == m_pTags->TagName(_T("voice")) ) {
            fMusicFound = true;
            break;      //start of voice. Exit this loop
        }
        else if (pX->GetName() == m_pTags->TagName(_T("instrName")) ) {
            lmLDPNode* pIN;
            pIN = pX->GetParameter(1);
            if (pIN->IsSimple()) {
                sInstrName = pIN->GetName();
            }
            else {
                AnalysisError( _("Expected name string for %s but found element '%s'. Ignored."),
                    m_pTags->TagName(_T("instrName")), pIN->GetName() );
            }
            //abbreviation (optional)
            if (pX->GetNumParms() > 1) {
                pIN = pX->GetParameter(2);
                if (pIN->IsSimple()) {
                    sInstrAbbrev = pIN->GetName();
                }
                else {
                    AnalysisError( _("Expected name string for %s but found element '%s'. Ignored."),
                        m_pTags->TagName(_T("instrName")), pIN->GetName() );
                }
            }
        }
        else if (pX->GetName() == m_pTags->TagName(_T("infoMIDI")) ) {
            //! @todo No treatment for now
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
                    AnalysisError( _("Number of staves expected but found '%s'. Element '%s' ignored."),
                        sNumStaves, m_pTags->TagName(_T("staves")) );
                    sNumStaves = _T("1");
                }
            }
            else {
                AnalysisError( _("Expected value for %s but found element '%s'. Ignored."),
                    m_pTags->TagName(_T("staves")), pX->GetName() );
           }
        }
        else {
            AnalysisError( _("[%s]: unknown element '%s' found. Element ignored."),
                m_pTags->TagName(_T("instrument")), pX->GetName() );
        }
    }

    //set number of staves
    sNumStaves.ToLong(&m_nNumStaves);

    //process firts voice
    if (!fMusicFound) {
        AnalysisError( _("Expected '%s' but found element %s. Analysis stopped."),
            m_pTags->TagName(_T("voice")), pX->GetName() );
        return;
    }

    // create the instrument with one empty VStaff
    lmInstrument* pInstr = pScore->AddInstrument(1, nMIDIChannel, nMIDIInstr,
                                        sInstrName, sInstrAbbrev);
    lmVStaff* pVStaff = pInstr->GetVStaff(1);      //get the VStaff created

    // analyce first voice
    AnalyzeVoice(pX, pVStaff);
    iP++;

    //analyze other voice elements
    for(; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);
        if (pX->GetName() = m_pTags->TagName(_T("voice")) ) {
            pVStaff = pInstr->AddVStaff(true);      //true -> overlayered
            AnalyzeVoice(pX, pVStaff);
        }
        else {
            AnalysisError( _("Expected '%s' but found element %s. Element ignored."),
                m_pTags->TagName(_T("voice")), pX->GetName() );
        }
    }

}

void lmLDPParser::AnalyzeVoice(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    // <voice> = (voice <music>+ )
    // <music> ::= {<Figure> | <Attribute> | <Indicacion> |
    //              <Barra> | <desplazamiento> | <opciones>}
    // <Atributo> ::= (<Clave> | <Tonalidad> | <Metrica>)
    // <Indicacion> ::= (<Metronomo>)

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("voice")));

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
        if (sName == _T("n")) {             // note
            AnalyzeNote(pX, pVStaff);
        } else if (sName == _T("s")) {      // rest
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
        } else {
            AnalysisError( _("[AnalyzeVoice]: Unknown or not allowed element '%s' found. Element ignored."),
                sName );
        }
    }

}

void lmLDPParser::AnalyzeSplit(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //! @todo   AnalyzeSplit code

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
            AnalysisError( _("[AnalyzeChord]: Expecting notes found element '%s'. Element ignored."),
                sName );
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
    long i;
    wxString sData;
    long iP;
    iP = 1;
    
    if (pNode->GetName() != _T("Instrumento") ) {
        AnalysisError( _("Element '%s' expected but found element %s. Analysis stopped."),
            _T("Instrumento"), pNode->GetName() );
        return;
    }

    // parse element [<NombreInstrumento> | num ]
    pX = pNode->GetParameter(iP);
    if (pX->GetName() = _T("NombreInstrumento")) {
        //! @todo de momento no hay tratamiento
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
        AnalysisError( _("Element %s expected but found element %s. Analysis stopped."),
            sLabelNumVStaves, pX->GetName() );
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
    int nMIDIChannel, nMIDIInstr;
//    nMIDIChannel = nMidiCanalVoz
//    nMIDIInstr = nMidiInstrVoz
//    
//    if (pX->GetName() = "INFOMIDI") {
//        AnalizarInfoMIDI pX, nMIDIChannel, nMIDIInstr
//        iP = iP + 1
//    }
    
    // create the instrument with empty VStaves
    nMIDIChannel=0, nMIDIInstr=0;        //dbg
    pScore->AddInstrument(nVStaves, nMIDIChannel, nMIDIInstr, _T(""));
    
    //Loop to analyze elements <Pentagrama>
    lmVStaff* pVStaff;
    for (i=1; i <= nVStaves; i++) {
        pVStaff = pScore->GetVStaff(nInstr, i);      //nInstr, nVstaff
        pX = pNode->GetParameter(iP);
        AnalyzeVStaff(pX, pVStaff);
        iP++;
    }

   
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
        AnalysisError( _("Expected node '%s' but found node '%s'. Analysis ended."),
            sLabel, pNode->GetName() );
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
        AnalysisError( _("Expected 'número de parte' but found '%s'. Analysis stopped."),
            pNode->GetName() );
        return;
    }
    iP++;
    
    //analyze second parameter (optional): number of staves of this lmVStaff
    wxString sNumStaves = _T("1");       //default value
    pX = pNode->GetParameter(iP);
    if (pX->IsSimple()) {
        sNumStaves = pX->GetName();
        if (!sNumStaves.IsNumber()) {
            AnalysisError( _("Number of staves expected but found '%s'. Analysis stopped."),
                pX->GetName() );
            return;
        } else {
            iP++;
        }
    }
    sNumStaves.ToLong(&m_nNumStaves);
    int i;
    //the VStaff already contains one staff. So we have to add nNumStaves - 1
    for(i=1; i < m_nNumStaves; i++) {
        pVStaff->AddStaff(5);    //five lines staff, standard size
    }

    //analyze remaining parameters: bars
    for (; iP <= pNode->GetNumParms(); iP++) {
        pX = pNode->GetParameter(iP);
        AnalyzeMeasure(pX, pVStaff);
    }
    
}

////Devuelve true si hay error, es decir si no añade objeto al pentagrama
//Function AnalizarMetronomo(lmVStaff* pVStaff, lmLDPNode* pNode) As Boolean
////v1.1  <Metronomo> ::= ("Metronomo" <Valor_nota> {<Tics_minuto> | (<Valor_nota> )
//
//    wxASSERT(pNode->GetName() = "METRONOMO"
//    
//    Dim nNota1 As EMetronomo, nNota2 As EMetronomo, nVelocidad As Long
//    Dim wxString sData
//    
//    //obtiene parámetros
//    Dim int nParms
//    nParms = pNode->GetNumParms()
//    
//    
//    //analiza primer parámetro: valor de nota inicial
//    sData = (pNode->GetParameter(1))->GetName();
//    switch (sData
//        case "N"
//            nNota1 = eMtr_Negra
//        case "N."
//            nNota1 = eMtr_NegraPuntillo
//        default:
//            AnalysisError(wxString::Format(_T("Tipo de nota inicio <" & sData & "> desconocido. " & _
//                "Se supone Negra."
//            nNota1 = eMtr_Negra
//    }
//    
//    //analiza segundo parámetro: valor de nota o num. tics por minuto
//    sData = (pNode->GetParameter(2))->GetName();
//    nVelocidad = 0
//    if (IsNumeric(sData)) {
//        nVelocidad = CLng(sData)
//    } else {
//        switch (sData
//            case "N"
//                nNota2 = eMtr_Negra
//            case "N."
//                nNota2 = eMtr_NegraPuntillo
//            default:
//                AnalysisError(wxString::Format(_T("Tipo de nota final <" & sData & "> desconocido. " & _
//                    "Se supone Negra."
//                nNota2 = eMtr_Negra
//        }
//    }
//
//    pVStaff.AddIndicacionMetronomo nNota1, nNota2, nVelocidad
//    AnalizarMetronomo = false
//    
//}

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
            //    AnalizarOpciones pVStaff, pX
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
            AnalysisError( _("[AnalyzeMeasure]: Expected node 'Figura', 'Grupo', 'Atributo' or 'Desplazamiento' but found node '%s'. Node ignored."),
                sName );
        }
    }

    if (fSomethingAdded && !fBarline) {
        pVStaff->AddBarline(etb_SimpleBarline);    //finish the bar
    }

}

////devuelve el desplazamiento temporal con signo.
////Si es un desplazamiento a inicio o fin devuelve unos números fuera de rango para que
////al truncarlos lleve al inicio o al fin
//Function AnalizarDesplazamiento(lmLDPNode* pNode) As Single
////<desplazamiento> = { ("retroceso" partes) | ("avance" partes) }
////
////donde partes es:
////a) una expresión algebráica, formada por sumas de notas, que indica lo que
////   se avanza o retrocede. Por ejemplo:  3n+s  indica que se avance/retroceda tres notas
////   negras y una semicorchea.
////b) el número de semigarrapateas que se avanza o retrocede (1 negra = 64 semigarrapateas)
////   (ES TAMBIEN UNA EXPRESION)
//
//    //analiza primer parámetro: desplazamiento
//    Dim wxString sData, rDesplz As Single
//    sData = UCase$((pNode->GetParameter(1))->GetName();)
//    if (IsNumeric(sData)) {
//        rDesplz = CSng(sData)
//    } else if { sData = "INICIO") {
//        if (pNode->GetName() != "RETROCESO") {
//            AnalysisError(wxString::Format(_T("<desplazamiento>: Se pide avance a //inicio//. Se ignora"
//            rDesplz = 0#
//        } else {
//            rDesplz = 9999999#
//        }
//    } else if { sData = "Fin") {
//        if (pNode->GetName() != "AVANCE") {
//            AnalysisError(wxString::Format(_T("<desplazamiento>: Se pide retroceso a //fin//. Se ignora"
//            rDesplz = 0#
//        } else {
//            rDesplz = 9999999#
//        }
//    } else {
//        //expresión algebraica. analizar
//        rDesplz = AnalizarExpresion(sData)
//    }
//    
//    //determina el signo
//    if (pNode->GetName() = "RETROCESO") { rDesplz = -rDesplz
//    
//    AnalizarDesplazamiento = rDesplz
//    
//}
//
////Recibe una expresión algebráica, formada por sumas de notas, que indica lo que
////se avanza o retrocede. Por ejemplo:  3n+s  indica que se avance/retroceda tres notas
////negras y una semicorchea.
////Devuelve el valor numérico equivalente a esa expresión
//Function AnalizarExpresion(wxString sData) As Single
//    
//    Dim nTotal As Long
//    Dim fSumar As Boolean
//    Dim long i               //posición sobre sData
//    Dim nNum As Long            //token numérico que está siendo leido
//    Dim sLetra As String        //token caracter
//    Dim nValor As Long          //valor de la letra
//    Dim fNumPendiente As Boolean    //hay un token numérico pendiente de procesar
//    
//    //inicializar
//    nTotal = 0
//    fSumar = true
//    nNum = 0
//    
//    //bucle. Se toma un token, se analiza y se opera
//    i = 1
//    fNumPendiente = false
//    Do While i <= Len(sData)
//        sLetra = Mid$(sData, i, 1)
//        
//        if (IsNumeric(sLetra)) {
//            //Numero: guardar en nNum
//            nNum = 10 * nNum + CLng(sLetra)
//            fNumPendiente = true
//            
//        } else if { sLetra = "+" Or sLetra = "-") {
//            //operador + o -. Guardar en fSigno
//            if (fNumPendiente) {
//                //el numero es un sumando. Añadirlo
//                if (fSumar) {
//                    nTotal = nTotal + nNum
//                } else {
//                    nTotal = nTotal - nNum
//                }
//                fNumPendiente = false
//            }
//            
//            //guardar el nuevo signo
//            fSumar = (sLetra = "+")
//            
//        } else {
//            //letra: calcular su valor. Multiplicar por nNum y sumar a total, según fSigno
//            nValor = GetDuracionFromLetra(sLetra)
//            if (nValor = 0) {
//                AnalysisError(wxString::Format(_T("<desplazamiento>: Letra (" & sLetra & ") desconocida. Se supone negra"
//                nValor = eQuarter
//            }
//            
//            if (Not fNumPendiente) { nNum = 1       //no había multiplicador
//            if (fSumar) {
//                nTotal = nTotal + (nValor * nNum)
//            } else {
//                nTotal = nTotal - (nValor * nNum)
//            }
//            fNumPendiente = false
//            
//            //limpiar datos usados
//            nNum = 0
//            fSumar = true
//            
//        }
//        i = i + 1
//    Loop
//    
//    if (fNumPendiente) {
//        //el numero es un sumando. Añadirlo
//        if (fSumar) {
//            nTotal = nTotal + nNum
//        } else {
//            nTotal = nTotal - nNum
//        }
//        fNumPendiente = false
//    }
//    
//    AnalizarExpresion = CSng(nTotal)
//    
//}
//
//void lmLDPParser::AnalizarGrupo(lmLDPNode* pNode, lmVStaff* pVStaff)
////  <Grupo> ::= ("G" <Nota> [<Figura>*] <Nota> [<Flags_grupo>])
//    
//    //obtiene parámetros
//    Dim int nParms, lmLDPNode* pX, long iP, iPMax As Long
//    nParms = pNode->GetNumParms()
//    
//    //analiza el último parámetro para ver si son flags de grupo
//    Dim nTupla As ETuplas
//    Set pX = pNode->GetParameter(nParms)
//    if (pX->GetName() = "T3") {
//        iPMax = nParms - 1
//        nTupla = eTP_Tresillo
//    } else {
//        nTupla = eTP_NoTupla
//        iPMax = nParms
//    }
//        
//    //analiza las notas/silencios que componen el grupo
//    Dim nBeamMode As ETipoAgrupacion
//    iP = 1
//    Do While iP <= iPMax
//        if (iP = 1) {
//            nBeamMode = etaInicioGrupo
//        } else if { iP = iPMax) {
//            nBeamMode = etaFinGrupo
//        } else {
//            nBeamMode = etaEnGrupo
//        }
//            
//        Set pX = pNode->GetParameter(iP)
//        switch (pX->GetName()
//            case "N", "NA"    //<nota>
//                AnalyzeNote pX, pVStaff, nTupla, nBeamMode
//            case "S"   //<silencio>
//                AnalizarSilencio pX, pVStaff, nTupla, nBeamMode
//            default:
//                AnalysisError(wxString::Format(_T("[AnalizarGrupo]: Se esperaba nodo NOTA o SILENCIO pero " & _
//                    "viene un nodo <" & pX->GetName() & ">. Se ignora este nodo."
//        }
//        iP = iP + 1
//    Loop
//
//}

lmNote* lmLDPParser::AnalyzeNote(lmLDPNode* pNode, lmVStaff* pVStaff, bool fChord)
{
    /*
    Analyze the source of a Note and with its information builds a lmNote object and appends
    it to the lmVStaff received as parameter. Returns a pointer to the lmNote created.

    <Nota> ::= ("N" <Altura> <Valor> [<Flags_nota>*] )
    <Flags_nota> :: {L | C | AMR | G | P}

    */

    wxASSERT(pNode->GetName() == _T("n") || pNode->GetName() == _T("na") );
    
    EStemType nStem = eDefaultStem;
    bool fBeamed = false;
    bool fTie = false;
    lmTBeamInfo BeamInfo[6];
    for (int i=0; i < 6; i++) {
        BeamInfo[i].Repeat = false;
        BeamInfo[i].Type = eBeamNone;
    }
//    Dim cAnotaciones As Collection
//    Set cAnotaciones = new Collection
    
    
    //Tuplet brakets
    bool fEndTuplet = false;
    int nTupletNumber = 0;      // 0 = no tuplet

    //default values
    bool fDotted = false;
    bool fDoubleDotted = false;
    ENoteType nNoteType = eQuarter;
    float rDuration = GetDefaultDuration(nNoteType, fDotted, fDoubleDotted, nTupletNumber);
    wxString sStep = _T("c");
    wxString sOctave = _T("4");
    EAccidentals nAccidentals = eNoAccidentals;


    //get parameters
    bool fInChord = (pNode->GetName() == _T("na")) || fChord;
    long nParms = pNode->GetNumParms();
    if (nParms < 2) {
        AnalysisError( _("Missing parameters in node nota '%s'. Assumed (n c4 n)."),
            pNode->ToString() );
        return pVStaff->AddNote(false,    //relative pitch
                                _T("c"), _T("4"), _T("0"), nAccidentals,
                                nNoteType, rDuration, fDotted, fDoubleDotted, m_nCurStaff,
                                fBeamed, BeamInfo, fInChord, fTie, nStem);
    }
   
    //analyze firts parameter: pitch and accidentals
    wxString sAltura = (pNode->GetParameter(1))->GetName();
    if (sAltura.IsNumber()) {
        //if sAltura is a number it represents a MIDI pitch in C major key signature.
        lmConverter oConverter;
        long nMidi = 0;
        sAltura.ToLong(&nMidi);   
        sAltura = oConverter.MidiPitchToLDPName((lmPitch)nMidi);
    }
    if (LDPDataToPitch(sAltura, &nAccidentals, &sStep, &sOctave)) {
        AnalysisError( _("Unknown note pitch in '%s'. Assumed 'c4'."),
            pNode->ToString() );
    }

    //analyze second parameter: duration and dots
    wxString sDuration = (pNode->GetParameter(2))->GetName();
    if (NoteTypeToData(sDuration, &nNoteType, &fDotted, &fDoubleDotted)) {
        AnalysisError( _("Note type unknown in '%s'. A quarter note assumed."),
            pNode->ToString() );
    }
    
    //analyze remaining parameters: annotations
//    Dim nLigada As ETies, nCalderon As ECalderon, nValor As Long
//    Dim nTipoStem As EStemType
//    nLigada = eL_NotTied
//    nCalderon = eC_SinCalderon
//    nTipoStem = eDefaultStem
//    
    wxString sData;
    lmLDPNode* pX;
    int iLevel, nLevel;
    for (int iP = 3; iP <= nParms; iP++)
    {
        pX = pNode->GetParameter(iP);
        if (pX->IsSimple()) {
            //
            // Analysis of simple notations
            //
            sData = pX->GetName();
    //            case "AMR"       //Articulaciones y acentos: marca de respiración
    //                cAnotaciones.Add sData
    //                
    //            case "C"        //Calderón
    //                nCalderon = eC_ConCalderon
    //                
            if (sData == _T("l") ) {       //Tied to the next one
                fTie = true;
            }
            else if (sData == _T("g+")) {       //Start of beamed group. Simple parameter
                //compute beaming level dependig on note type
                nLevel = GetBeamingLevel(nNoteType);
                if (nLevel == -1) {
                    AnalysisError(
                        _("Requesting beaming a note longer than eight. Beaming ignored."));
                }
                else {
                    fBeamed = true;
                    for (iLevel=0; iLevel <= nLevel; iLevel++) {
                        BeamInfo[iLevel].Type = eBeamBegin;
                        //wxLogMessage(wxString::Format(
                        //    _T("[lmLDPParser::AnalyzeNote] BeamInfo[%d] = eBeamBegin"), iLevel));
                    }
                }
            }

            else if (sData == _T("t3")) {       //start of triplet. Simple parameter
                if (m_pTupletBracket) {
                    //there is already a tuplet open and not closed
                    AnalysisError(_("Requesting to start a tuplet but there is already a tuplet open. Tag 't3' ignored."));
                }
                else {
                    nTupletNumber = 3;

                    // tuplet braket information
                    bool fShowTupletBracket = true;
                    bool fShowNumber = true;
                    bool fTupletAbove = true;
                    m_pTupletBracket = new lmTupletBracket(fShowNumber, nTupletNumber, 
                                                        fShowTupletBracket, fTupletAbove);
                }
            }

            else if (sData == _T("t-")) {       //end of tuplet
                // if there is an open tuplet, close the tuplet bracket
                if (m_pTupletBracket) {
                    fEndTuplet = true;
                }
                else {
                    AnalysisError(_("Requesting to end a tuplet but there is not an open tuplet. Tag 't-' ignored."));
                }
            }

            else if (sData == _T("g-")) {       //End of beamed group
                //allow to close the beamed group
                bool fCloseBeam = true;

                //! @todo   Beaming information only allowed in base note of chords
                //!         This program should move this information to base note
                //!         as this restriction is un-coherent with forcing the t- flag
                //!         to be in the last note of the chord.
                if (fInChord) {
                    AnalysisError(
                        _("Requesting ending a beaming a group in a note that is note the first one of a chord. Beaming ignored."));
                    fCloseBeam = false;
                }

                //There must exist a previous note/rest
                if (!g_pLastNoteRest) {
                    AnalysisError(
                        _("Requesting ending a beaming a group but there is not a  previous note. Beaming ignored."));
                    fCloseBeam = false;
                }
                else {
                    // and the previous note must be beamed
                    if (!g_pLastNoteRest->IsBeamed() || 
                        g_pLastNoteRest->GetBeamType(0) == eBeamEnd) {
                        AnalysisError(
                            _("Requesting ending a beaming a group but previous note is not beamed. Beaming ignored."));
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
                            _T("[lmLDPParser::AnalyzeNote] BeamInfo[%d] = eBeamEnd"), iLevel);
                    }

                    // deal with differences between current note level and previous note level
                    if (nCurLevel > nPrevLevel) {
                        // current level is grater than previous one ==> 
                        // close common levels (done) and put backward in current deeper levels
                        for (; iLevel <= nCurLevel; iLevel++) {
                            BeamInfo[iLevel].Type = eBeamBackward;
                            g_pLogger->LogTrace(_T("LDPParser_beams"), 
                                _T("[lmLDPParser::AnalyzeNote] BeamInfo[%d] = eBeamBackward"), iLevel);
                        }
                    }
                    else if  (nCurLevel < nPrevLevel) {
                        // current level is lower than previous one:
                        // close common levels (done) and close deeper levels in previous note
                        for (; iLevel <= nPrevLevel; iLevel++) {
                            if (g_pLastNoteRest->GetBeamType(iLevel) == eBeamContinue) {
                                g_pLastNoteRest->SetBeamType(iLevel, eBeamEnd);
                                g_pLogger->LogTrace(_T("LDPParser_beams"), 
                                    _T("[lmLDPParser::AnalyzeNote] Changing previous BeamInfo[%d] = eBeamEnd"), iLevel);
                            }
                            else if (g_pLastNoteRest->GetBeamType(iLevel) == eBeamBegin) {
                                g_pLastNoteRest->SetBeamType(iLevel, eBeamForward);
                                g_pLogger->LogTrace(_T("LDPParser_beams"), 
                                    _T("[lmLDPParser::AnalyzeNote] Changing previous BeamInfo[%d] = eBeamForward"), iLevel);
                            }
                        }
                    }
                    else {
                        // current level is equal than previous one:
                        // close common levels (done)
                    }
                }
            }
            else if (sData.Left(1) == _T("p")) {       //staff number
                m_nCurStaff = AnayzeNumStaff(sData);
            }
            else {
                AnalysisError(_("Error: notation '%s' unknown. It will be ignored."), sData );
            }

       }

        else {
            //
            // Analysis of compound notations
            //
            sData = pX->GetName();
            if (sData == _T("g")) {       //Start of group element
                AnalysisError(_("Notation '%s' unknown or not implemented. Old (g + t3) syntax?"), sData);
            }
            else if (sData == m_pTags->TagName(_T("stem")) ) {       //stem attributes
                nStem = AnalyzeStem(pX, pVStaff);
            }
            else {
                AnalysisError(_("Notation '%s' unknown or not implemented."), sData);
            }

        }
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
                            _T("[lmLDPParser::AnalyzeNote] BeamInfo[%d] = eBeamContinue"), iLevel);
                    }

                    if (nCurLevel > nPrevLevel) {
                        // current level is grater than previous one, start new beams
                        for (; iLevel <= nCurLevel; iLevel++) {
                            BeamInfo[iLevel].Type = eBeamBegin;
                            g_pLogger->LogTrace(_T("LDPParser_beams"), 
                                _T("[lmLDPParser::AnalyzeNote] BeamInfo[%d] = eBeamBegin"), iLevel);
                        }
                    }
                    else if  (nCurLevel < nPrevLevel) {
                        // current level is lower than previous one
                        // close common levels (done) and close deeper levels in previous note
                        for (; iLevel <= nPrevLevel; iLevel++) {
                            if (g_pLastNoteRest->GetBeamType(iLevel) == eBeamContinue) {
                                g_pLastNoteRest->SetBeamType(iLevel, eBeamEnd);
                                g_pLogger->LogTrace(_T("LDPParser_beams"), 
                                    _T("[lmLDPParser::AnalyzeNote] Changing previous BeamInfo[%d] = eBeamEnd"), iLevel);
                            }
                            else if (g_pLastNoteRest->GetBeamType(iLevel) == eBeamBegin) {
                                g_pLastNoteRest->SetBeamType(iLevel, eBeamForward);
                                g_pLogger->LogTrace(_T("LDPParser_beams"), 
                                    _T("[lmLDPParser::AnalyzeNote] Changing previous BeamInfo[%d] = eBeamFordward"), iLevel);
                            }
                        }
                    }
                }
            }
        }
    }

    //if not first note of tuple, tuple information is not present and need to be taken from
    //previous note
    if (m_pTupletBracket) {
        // a tuplet is open
       nTupletNumber = m_pTupletBracket->GetTupletNumber();
    }

    // calculation of duration
    rDuration = GetDefaultDuration(nNoteType, fDotted, fDoubleDotted, nTupletNumber);

    lmNote* pNR = pVStaff->AddNote(false,    //relative pitch
                            sStep, sOctave, _T("0"), nAccidentals,
                            nNoteType, rDuration, fDotted, fDoubleDotted, m_nCurStaff, 
                            fBeamed, BeamInfo, fInChord, fTie, nStem);

    // Add notations
    if (m_pTupletBracket) {
        m_pTupletBracket->Include(pNR);             // add this note to the tuplet
        pNR->SetTupletBracket(m_pTupletBracket);    // inform the note

        if (fEndTuplet) {
            m_pTupletBracket = (lmTupletBracket*)NULL;
        }
    }

    return pNR;

}

lmRest* lmLDPParser::AnalyzeRest(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    /*
    Analyze the source of a Rest and with its information builds a lmRest object and appends
    it to the lmVStaff received as parameter. Returns a pointer to the lmRest created.

    <Silencio> ::= ("S" <Valor> [<Flags>]*)
    <flags> ::= "C"
    */

    wxASSERT(pNode->GetName() == _T("s") );
    
    //Tuplet brakets
    bool fEndTuplet = false;
    int nTupletNumber = 0;      // 0 = no tuplet

    //! @todo rests in a tuplet
  
    //default values
    bool fDotted = false;
    bool fDoubleDotted = false;
    ENoteType nNoteType = eQuarter;
    float rDuration = GetDefaultDuration(nNoteType, fDotted, fDoubleDotted, nTupletNumber);


    //get parameters
    long nParms = pNode->GetNumParms();
    if (nParms < 1) {
        AnalysisError( _("Missing parameters in rest node '%s'. Node replaced by '(s n)'."),
            pNode->ToString() );
        return pVStaff->AddRest(nNoteType, rDuration, fDotted, fDoubleDotted,
                                m_nCurStaff);
    }
   
    //analyze first parameter: duration and dots
    wxString sDuration = (pNode->GetParameter(1))->GetName();
    if (NoteTypeToData(sDuration, &nNoteType, &fDotted, &fDoubleDotted)) {
        AnalysisError( _("Note type unknown in '%s'. A quarter note assumed."),
            pNode->ToString() );
    }

    //analyze remaining parameters: notations
    
//    Dim nCalderon As ECalderon
//    nCalderon = eC_SinCalderon
    wxString sData;
    lmLDPNode* pX;
    int iP;
    for (iP = 2; iP <= nParms; iP++)
    {
        pX = pNode->GetParameter(iP);
        sData = pX->GetName();
        if (sData.Left(1) == _T("p")) {       //staff number
            m_nCurStaff = AnayzeNumStaff(sData);
        }
//        } else {
//            switch (sData
//                case "AMR"       //Articulaciones y acentos: marca de respiración
//                    cAnotaciones.Add sData
//                
//                case "C"        //Calderón
//                    nCalderon = eC_ConCalderon
//                    
//                default:
//                    MsgBox "Error: Anotación <" & sData & "> desconocida. Se ignora"
//            }
//        }
    }


    //beaming information for rests inside a beamed group
    bool fBeamed = false;
    lmTBeamInfo BeamInfo[6];
    for (int i=0; i < 6; i++) {
        BeamInfo[i].Repeat = false;
        BeamInfo[i].Type = eBeamNone;
    }

    if (nNoteType > eQuarter) {
        if (g_pLastNoteRest) {
            if (g_pLastNoteRest->IsBeamed()) {
                int iLevel;
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
                        //wxLogMessage(wxString::Format(
                        //    _T("[lmLDPParser::AnalyzeNote] BeamInfo[%d] = eBeamContinue"), iLevel));
                    }

                    if (nCurLevel > nPrevLevel) {
                        // current level is grater than previous one, start new beams
                        for (; iLevel <= nCurLevel; iLevel++) {
                            BeamInfo[iLevel].Type = eBeamBegin;
                            //wxLogMessage(wxString::Format(
                            //    _T("[lmLDPParser::AnalyzeNote] BeamInfo[%d] = eBeamBegin"), iLevel));
                        }
                    }
                    else if  (nCurLevel < nPrevLevel) {
                        // current level is lower than previous one, close the remaining beams
                        for (; iLevel <= nPrevLevel; iLevel++) {
                            BeamInfo[iLevel].Type = eBeamEnd;
                            //wxLogMessage(wxString::Format(
                            //    _T("[lmLDPParser::AnalyzeNote] BeamInfo[%d] = eBeamEnd"), iLevel));
                        }
                    }
                }
            }
        }
    }
    
    // compute duration
    rDuration = GetDefaultDuration(nNoteType, fDotted, fDoubleDotted, nTupletNumber);

    return pVStaff->AddRest(nNoteType, rDuration, fDotted, fDoubleDotted,
                            m_nCurStaff, fBeamed, BeamInfo);

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

    //<Barra> ::= ("Barra" <Tipo_barra> [<Visible>])
    //<Tipo_barra> ::= {"InicioRepeticion" | "FinRepeticion" | "Final" | "Doble" | "Simple" }

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("barline")) );

    //check that bar type is specified
    if(pNode->GetNumParms() < 1) {
        //assume simple barline, visible
        pVStaff->AddBarline(etb_SimpleBarline, true);
        return false;
    }
    
    bool fVisible = true;
    EBarline nType = etb_SimpleBarline;
    
    wxString sType = (pNode->GetParameter(1))->GetName();
    if (sType == _T("FinRepeticion")) {
        nType = etb_EndRepetitionBarline;
    } else if (sType == _T("InicioRepeticion")) {
        nType = etb_StartRepetitionBarline;
    } else if (sType == _T("Final")) {
        nType = etb_EndBarline;
    } else if (sType == _T("Doble")) {
        nType = etb_DoubleBarline;
    } else if (sType == _T("Simple")) {
        nType = etb_SimpleBarline;
    } else if (sType == _T("Inicial")) {
        nType = etb_StartBarline;
    } else if (sType == _T("DobleRepeticion")) {
        nType = etb_DoubleRepetitionBarline;
    } else {
        AnalysisError( _("Unknown barline type '%s'. Assumed 'Simple' barline."),
            (pNode->GetParameter(1))->GetName() );
    }

    if (pNode->GetNumParms() == 2) {
        if ((pNode->GetParameter(2))->GetName() == _T("NoVisible")) {
            fVisible = false;
        }
    }
    
    pVStaff->AddBarline(nType, fVisible);
    return false;
    
}

//returns true if error; in this case nothing is added to the lmVStaff
bool lmLDPParser::AnalyzeClef(lmVStaff* pVStaff, lmLDPNode* pNode)
{
//  <Clave> = ("Clave" {"Sol" | "Fa4" | "Fa3" | "Do1" | "Do2" | "Do3" | "Do4" | "SinClave" }
//                [<numStaff>] [<Visible>] )
    
    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("clef")) );

    //check that clef type is specified
    if(pNode->GetNumParms() < 1) {
        AnalysisError(
            _("Element '%s' has less parameters that the minimum required. Assumed '(%s Sol)'."),
            m_pTags->TagName(_T("clef")), m_pTags->TagName(_T("clef")) );
        pVStaff->AddClef(eclvSol, 1, true);
        return false;
    }
    
    long iP = 1;
    wxString sName = (pNode->GetParameter(iP))->GetName();
    EClefType nClef = LDPNameToClef(sName);
    if (nClef == (EClefType)-1) {
        AnalysisError( _("Unknown clef '%s'. Assumed 'Sol'."), sName );
        nClef = eclvSol;
    }
    iP++;
    
    //analyze second parameter (optional): number of staff on which this clef is located
    lmLDPNode* pX;
    long nStaff = 1;
    if (pNode->GetNumParms() >= iP) {
        pX = pNode->GetParameter(iP);
        wxString sStaffNum = pX->GetName();
        if (sStaffNum.Left(1) == _T("p")) {
            nStaff = AnayzeNumStaff(sStaffNum);
            iP++;
        }
    }
    
    //analyze third parameter (optional): visible or not
    bool fVisible = true;
    if (pNode->GetNumParms() >= iP) {
        pX = pNode->GetParameter(iP);
        if (pX->GetName() == _T("NoVisible")) fVisible = false;
    }
    
    pVStaff->AddClef(nClef, nStaff, fVisible);
    return false;
    
}

//void lmLDPParser::AnalizarOpciones(lmVStaff* pVStaff, lmLDPNode* pNode)
////(EspaciadoLineas { (pantalla  pixels) | (papel mm) }
////
////(EspacioNotas decimas)   <-- separación mínima entre notas
////(EspacioAntesParte decimas)
////(EspacioPentagramas decimas) <-- para instr. multipentagramas (piano)
////(EspacioTrasParte decimas)
////(EspacioSistemas decimas)
////(EspacioNegra decimas)  <-- factor conversión tiempo/espacio
//
//
//    Dim sData1 As String
//    
//    //obtiene parámetros
//    Dim lmLDPNode* pX, long iP
//    Dim sOpcion As String, rNum As Single, sNum As String
//    
//    //bucle de análisis de opciones
//    for (iP = 1 To pNode->GetNumParms()
//        Set pX = pNode->GetParameter(iP)
//        if (pX->IsSimple()) {
//            AnalysisError(wxString::Format(_T("[AnalizarOpciones]: Opción <" & sOpcion & _
//                "> desconocida o falta su valor. Se ignora este elemento."
//        } else {
//            //opción con parámetros. Obtiene nombre y valor
//            sOpcion = pX->GetName()
//            sNum = pX->GetParameter(1).GetName();
//            if (Not IsNumeric(sNum)) {
//                AnalysisError(wxString::Format(_T("[AnalizarOpciones]: Valor <" & sNum & "> para la opción <" & _
//                    sOpcion & "> no es numérica. Se ignora este elemento."
//            } else {
//                rNum = CSng(sNum)
//                switch (UCase$(sOpcion)
//                    case "ESPACIOANTESPARTE"
//                        pVStaff.EspaciadoAntesParte = rNum
//                    default:
//                        AnalysisError(wxString::Format(_T("[AnalizarOpciones]: Opción <" & sOpcion & _
//                            "> desconocida. Se ignora este elemento."
//                }
//            }
//        }
//    }   // iP
//
//}
//
////Devuelve true si hay error, es decir si no añade objeto al pentagrama
//Function AnalizarStem(lmVStaff* pVStaff, lmLDPNode* pNode) As Boolean
////<plica> = (plica [ "arriba" | "abajo" | "sin-plica" | "doble"])
//    
//    wxASSERT(pNode->GetName() = "PLICA"
//    wxASSERT(pNode->GetNumParms() = 1
//    
//    Dim fVisible As Boolean
//    Dim nTonalidad As EKeySignatures, sData1 As String
//    
//    sData1 = (pNode->GetParameter(1))->GetName();
//    switch (sData1
//        case "ARRIBA"
//            nTonalidad = earmDo
//        case "ABAJO"
//            nTonalidad = earmDom
//        case "SIN-PLICA"
//            nTonalidad = earmDosm
//        case "DOBLE"
//            nTonalidad = earmFa
//        default:
//            AnalysisError(wxString::Format(_T("Elemento plica: valor <" & sData1 & "> desconocido. " & _
//                "Se ignora elemento."
//            nTonalidad = earmDo
//    }
//    
//    pVStaff.AddArmadura nTonalidad, fVisible
//    
//    AnalizarStem = false       //no hay error
//    
//}

//returns true if error; in this case nothing is added to the score
bool lmLDPParser::AnalyzeTitle(lmLDPNode* pNode, lmScore* pScore)
{
    //  (title <alignment> string [<font>][<location>])

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("title")) );

    //check that at least two parameters (aligment and text string) are specified
    if(pNode->GetNumParms() < 2) {
        AnalysisError(
            _("Element '%s' has less parameters that the minimum required. Element ignored."),
            m_pTags->TagName(_T("title")) );
        return true;
    }
    
    wxString sTitle;
    lmEAlignment nAlign = m_nTitleAlignment;
    wxString sFontName = m_sTitleFontName;
    int nFontSize = m_nTitleFontSize; 
    lmETextStyle nStyle = m_nTitleStyle;

    lmLocation tPos;
    tPos.xType = lmLOCATION_DEFAULT;
    tPos.xUnits = lmTENTHS;
    tPos.yType = lmLOCATION_DEFAULT;
    tPos.yUnits = lmTENTHS;

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
        AnalysisError( _("Invalid alignment value '%s'. Assumed '%s'."),
            sName, m_pTags->TagName(_T("center")) );
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
            AnalyzeFont(pX, &sFontName, &nFontSize, &nStyle);
            //save font values as new default for titles
            m_sTitleFontName = sFontName;
            m_nTitleFontSize = nFontSize; 
            m_nTitleStyle = nStyle;
        }
        else if (sName == m_pTags->TagName(_T("x")) || sName == m_pTags->TagName(_T("dx")) ||
                 sName == m_pTags->TagName(_T("y")) || sName == m_pTags->TagName(_T("dy")) )
        {
            AnalyzeLocation(pX, &tPos);
        }
        else {
            AnalysisError( _("Unknown parameter '%s'. Ignored."), sName);
        }
    }

    //create the title
    pScore->AddTitle(sTitle, nAlign, tPos, sFontName, nFontSize, nStyle);
    
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
            _("Element '%s' has less parameters that the minimum required. Element ignored."),
            m_pTags->TagName(_T("text")) );
        return true;
    }
    
    wxString sText;
    lmEAlignment nAlign = lmALIGN_LEFT;     //! @todo user options instead of fixed values
    wxString sFontName = m_sTextFontName;
    int nFontSize = m_nTextFontSize; 
    lmETextStyle nStyle = m_nTextStyle;

    bool fHasWidth = false;

    lmLocation tPos;
    tPos.xType = lmLOCATION_DEFAULT;
    tPos.xUnits = lmTENTHS;
    tPos.yType = lmLOCATION_DEFAULT;
    tPos.yUnits = lmTENTHS;

    int iP = 1;

    //get the string
    sText = (pNode->GetParameter(iP))->GetName();
    iP++;

    //get remaining parameters: location, font, alignment
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
            AnalyzeFont(pX, &sFontName, &nFontSize, &nStyle);
            //save font values as new default for titles
            m_sTextFontName = sFontName;
            m_nTextFontSize = nFontSize; 
            m_nTextStyle = nStyle;
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
            AnalysisError( _("[Element '%s'. Invalid parameter '%s'. Ignored."),
                m_pTags->TagName(_T("text")), sName );
        }
    }

    //create the text
    lmFontInfo tFont = { sFontName, nFontSize, nStyle };
    pVStaff->AddWordsDirection(sText, nAlign, &tPos, tFont, fHasWidth);
    
    return false;
    
}

//returns true if error; in this case nothing is added to the lmVStaff
bool lmLDPParser::AnalyzeKeySignature(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //  <KeySignature> ::= (key {"Do" | "Sol" | "Re" | "La" | "Mi" | "Si" | "Fa+" |
    //                        | "Sol-" | "Re-" | "La-" | "Mi-" | "Si-" | "Fa" } [<Visible>])

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("key")) );

    //check that key value is specified
    if(pNode->GetNumParms() < 1) {
        AnalysisError(
            _("Element '%s' has less parameters that the minimum required. Assumed '(%s Do)'."),
            m_pTags->TagName(_T("key")), m_pTags->TagName(_T("key")) );
        pVStaff->AddKeySignature(earmDo);
        return false;
    }
    
    long iP = 1;
    wxString sName = (pNode->GetParameter(iP))->GetName();
    EKeySignatures nKey = LDPNameToKey(sName);
    if (nKey == (EKeySignatures)-1) {
        AnalysisError( _("Unknown key '%s'. Assumed 'Do'."), sName );
        nKey = earmDo;
    }
    iP++;
    
    //analyze second parameter (optional): visible or not
    lmLDPNode* pX;
    bool fVisible = true;
    if (pNode->GetNumParms() >= iP) {
        pX = pNode->GetParameter(iP);
        if (pX->GetName() == _T("NoVisible")) fVisible = false;
    }
    
    pVStaff->AddKeySignature(nKey, fVisible);
    return false;
    
}

//returns true if error and in this case nothing is added to the lmVStaff
bool lmLDPParser::AnalyzeTimeSignature(lmVStaff* pVStaff, lmLDPNode* pNode)
{
//  <Métrica> ::= ("Metrica" <num> <num> [<Visible>])
    
    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("time")) );

    //check that the two numbers are specified
    if(pNode->GetNumParms() < 2) {
        AnalysisError( _("Element '%s' has less parameters that the minimum required. Assumed '(Metrica 4 4)'."),
            m_pTags->TagName(_T("time")));
        pVStaff->AddTimeSignature(emtr44);
        return false;
    }
    
    wxString sNum1 = (pNode->GetParameter(1))->GetName();
    wxString sNum2 = (pNode->GetParameter(2))->GetName();
    if (!sNum1.IsNumber() || !sNum2.IsNumber()) {
        AnalysisError(
            _("Element '%s': Two numbers expected but found '%s' and '%s'. Assumed '(%s 4 4)'."),
            m_pTags->TagName(_T("time")), sNum1, sNum2, m_pTags->TagName(_T("time")) );
        pVStaff->AddTimeSignature(emtr44);
        return false;
    }

    long nBeats, nBeatType;
    sNum1.ToLong(&nBeats);
    sNum2.ToLong(&nBeatType);

    //analyze third parameter (optional): visible or not
    bool fVisible = true;
    if (pNode->GetNumParms() > 2) {
        lmLDPNode* pX = pNode->GetParameter(3);
        if (pX->GetName() == _T("NoVisible")) fVisible = false;
    }
    
    pVStaff->AddTimeSignature((int)nBeats, (int)nBeatType, fVisible);
    return false;
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

EStemType lmLDPParser::AnalyzeStem(lmLDPNode* pNode, lmVStaff* pVStaff)
{
    //<Stem> ::= (stem [up | down] <lenght> }

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("stem")) );

    EStemType nStem = eDefaultStem;

    //check that there are parameters
    if(pNode->GetNumParms() < 1) {
        AnalysisError( _("Element '%s' has less parameters that the minimum required. Tag ignored. Assumed default stem."),
            m_pTags->TagName(_T("stem")));
        return nStem;
    }
    
    //get stem direction
    wxString sDir = (pNode->GetParameter(1))->GetName();
    if (sDir == m_pTags->TagName(_T("up")) )
        nStem = eStemUp;
    else if (sDir == m_pTags->TagName(_T("down")) )
        nStem = eStemDown;
    else {
        AnalysisError( _("Invalid stem direction '%s'. Default direction taken."), sDir);
    }

    return nStem;
    
}

void lmLDPParser::AnalyzeFont(lmLDPNode* pNode, wxString* pFontName, int* pFontSize,
                              lmETextStyle* pStyle)
{        
    // <font> = (font <name> <size> <style>)

    //returns, in variables pointed by pFontName, pFontSize and pStyleDevuelve the
    //result of the analysis. No default values are returned, only the real values
    //found. Any defaults must be set before invoking this method

    wxASSERT(pNode->GetName() == m_pTags->TagName(_T("font")) );

    //check that there are parameters
    if (!(pNode->GetNumParms()== 2 || pNode->GetNumParms() == 3)) {
        AnalysisError( _("Element '%s' has less parameters than the minimum required. Tag ignored."),
            m_pTags->TagName(_T("stem")));
    }

    //flags to control that the corresponding parameter has been processed
    bool fName = false;
    bool fSize = false;
    bool fStyle = false;
    
    //get parameters. The come in any order
    int iP;
    wxString sParm;

    for(iP=1; iP <= pNode->GetNumParms(); iP++) {
        sParm = (pNode->GetParameter(iP))->GetName();

        if (!fStyle) {
            //try style
            fStyle = true;
            if (sParm == m_pTags->TagName(_T("bold")) )
                *pStyle = lmTEXT_BOLD;
            else if (sParm == m_pTags->TagName(_T("normal")) )
                *pStyle = lmTEXT_NORMAL;
            else if (sParm == m_pTags->TagName(_T("italic")) )
                *pStyle = lmTEXT_ITALIC;
            else if (sParm == m_pTags->TagName(_T("bold-italic")) )
                *pStyle = lmTEXT_ITALIC_BOLD;
            else {
                fStyle = false;
            }
        }

        else if (!fSize) {
            wxString sSize = sParm;
            if (sParm.Length() > 2 && sParm.Right(2) == _T("pt")) {
                sSize = sParm.Left(sParm.Length() - 2);
            }
            if (sSize.IsNumber()) {
                long nSize;
                sSize.ToLong(&nSize);
                *pFontSize = (int)nSize;
                fSize = true;
            }
        }

        else if (!fName) {
            //assume it is the name
            fName = true;
            *pFontName = (pNode->GetParameter(iP))->GetName();
        }

        else {
            AnalysisError( _("Element '%s': invalid parameter '%s'. It is ignored."),
                m_pTags->TagName(_T("font")), sParm );
        }
    }
    
}

void lmLDPParser::AnalyzeLocation(lmLDPNode* pNode, int* pValue, lmEUnits* pUnits)
{        
    // <location> = (x num) | (y num) | (dx num) | (dy num)
    // <num> = number [units]

    //returns, in variables pointed by pValue and pUnits the
    //result of the analysis. 

    //check that there are parameters
    if (pNode->GetNumParms()!= 1) {
        AnalysisError( _("Element '%s' has less or more parameters than required. Tag ignored."),
            pNode->GetName() );
    }

    //get value
    wxString sParm = (pNode->GetParameter(1))->GetName();
    long nValue;
    wxString sValue = sParm;
    if (sParm.Length() > 2) {
        wxString sUnits = sParm.Right(2);
        if (sUnits == _T("mm")) {
            sValue = sParm.Left(sParm.Length() - 2);
            *pUnits = lmMILLIMETERS;
        }
        else if (sUnits == _T("cm")) {
            sValue = sParm.Left(sParm.Length() - 2);
            *pUnits = lmCENTIMETERS;
        }
        else if (sUnits == _T("in")) {
            sValue = sParm.Left(sParm.Length() - 2);
            *pUnits = lmINCHES;
        }
        else {
            AnalysisError( _("Element '%s': Invalid units '%s'. Ignored"),
                pNode->GetName(), sUnits );
        }
    }
    if (sValue.IsNumber()) {
        sValue.ToLong(&nValue);
        *pValue = (int)nValue;
    }
    else {
        AnalysisError( _("Element '%s': Invalid value '%s'. It must be a number with optional units. Zero assumed."),
            pNode->GetName(), sParm );
        *pValue = 0;
    }

}

void lmLDPParser::AnalyzeLocation(lmLDPNode* pNode, lmLocation* pPos)
{
    //analyze location
    wxString sName = pNode->GetName();

    wxASSERT(sName == m_pTags->TagName(_T("x")) || sName == m_pTags->TagName(_T("dx")) ||
        sName == m_pTags->TagName(_T("y")) || sName == m_pTags->TagName(_T("dy")) );

    int nValue;
    lmEUnits nUnits;
    AnalyzeLocation(pNode, &nValue, &nUnits);
    if (sName == m_pTags->TagName(_T("x")) ) {
        //x
        pPos->x = nValue;
        pPos->xType = lmLOCATION_ABSOLUTE;
        pPos->xUnits = nUnits;
    }
    else if (sName == m_pTags->TagName(_T("dx")) ) {
        //dx
        pPos->x = nValue;
        pPos->xType = lmLOCATION_RELATIVE;
        pPos->xUnits = nUnits;
    }
    else if (sName == m_pTags->TagName(_T("y")) ) {
        //y
        pPos->y = nValue;
        pPos->yType = lmLOCATION_ABSOLUTE;
        pPos->yUnits = nUnits;
    }
    else {
        //dy
        pPos->y = nValue;
        pPos->yType = lmLOCATION_RELATIVE;
        pPos->yUnits = nUnits;
    }

}


////Devuelve, en las variables nX, nY, fXabs y fYabs, los valores obtenidos tras el análisis
//void lmLDPParser::AnalizarPosicion(lmLDPNode* pNode, _
//        ByRef nX As Long, ByRef nY As Long, _
//        ByRef fXAbs As Boolean, ByRef fYAbs As Boolean)
//        
////<posicion> = (xy ["x<modo><pos>"] ["y<modo><pos>"])
////
////<modo> = { r | a }
////<pos> = num entero con signo opcional
////
////R = relativa a origen del compás, A = absoluta, referida a origen del papel.
////Ejemplos:
////(xy xa54) // coordenada x: 54 décimas de línea desde el origen del lienzo
////(xy xr-54) // coordenada x: 54 décimas de línea menos que la x de origen de la barra de inicio del compas
//    
//    wxASSERT(pNode->GetName() = "XY"
//    wxASSERT(pNode->GetNumParms() = 1 Or pNode->GetNumParms() = 2
//    
//    Dim sNum As String, nNum As Long, wxString sData
//    Dim fAbsoluta As Boolean                //modo: true=absoluta, false=relativa
//    Dim long iP
//    
//    //inicializa valores a devolver
//    nX = 0
//    fXAbs = false
//    nY = 0
//    fYAbs = false
//    
//    //bucle de análisis de parámetros
//    for (iP = 1 To pNode->GetNumParms()
//        //comprueba que el parámetro consta de al menos tres caracteres
//        sData = (pNode->GetParameter(iP))->GetName();;
//        if (Len(sData) < 3) {
//            AnalysisError(wxString::Format(_T("Parámetro de coordenada erróneo <" & sData & _
//                ">. Se sustituye por xr0."
//            sData = "xr0"
//        }
//        
//        //obtiene el valor numérico de la coordenada
//        sNum = Mid$(sData, 3)
//        if (Not IsNumeric(sNum)) {
//            AnalysisError(wxString::Format(_T("Se esperaba un número con signo opcional (valor de la coordenada) " & _
//                "pero viene <" & sNum & ">. se supone 0."
//            sNum = "0"
//        }
//        nNum = CLng(sNum)
//        
//        //analiza la segunda letra para determinar el modo
//        switch (UCase$(Mid$(sData, 2, 1))
//            case "R"
//                fAbsoluta = false
//            case "A"
//                fAbsoluta = true
//            default:
//                AnalysisError(wxString::Format(_T("Segunda letra de coordena <" & sData & _
//                    "> no es r ni a. Se supone r: relativa"
//                fAbsoluta = false
//        }
//        
//        //analiza la primera letra y guarda el valor de la coordenada
//        switch (UCase$(Left$(sData, 1))
//            case "X"
//                nX = nNum
//                fXAbs = fAbsoluta
//            case "Y"
//                nY = nNum
//                fYAbs = fAbsoluta
//            default:
//                AnalysisError(wxString::Format(_T("Primera letra de coordena <" & sData & _
//                    "> no es x ni y. Se ignora esta coordenada."
//        }
//        
//    }   // iP
//    
//}
//
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

int lmLDPParser::AnayzeNumStaff(wxString sNotation)
{
    //analyzes a notation Pxx.  xx must be lower or equal than m_nNumStaves

    if (sNotation.Left(1) != _T("p")) {
        AnalysisError( _("Notation P expected but found '%s'. Replaced by 'p1'"),
            sNotation );
        return 1;
    }
    
    wxString sData = sNotation.Mid(1);         //remove char 'P'
    if (!sData.IsNumber()) {
        AnalysisError( _("Notation P not followed by number (%s). Replaced by 'p1'"),
            sNotation );
        return 1;
    }
    
    long nValue;
    sData.ToLong(&nValue);
    if (nValue > m_nNumStaves) {
        AnalysisError( _("Notation '%s': number is greater than number of staves defined (%d). Replaced by 'P1'."),
            sNotation, m_nNumStaves );
        return 1;
    }
    return (int)nValue;

}

float lmLDPParser::GetDefaultDuration(ENoteType nNoteType, bool fDotted, bool fDoubleDotted,
                      int nTupletNumber)
{
    //compute duration without modifiers
    float rDuration = NoteTypeToDuration(nNoteType, fDotted, fDoubleDotted);
    
    //alter by modifiers (i.e. tupla)
    switch (nTupletNumber) {
        case 0:     //no tuplet
            break;
        case 3:     // triplet
            rDuration = (rDuration * 2) / 3;
            break;
        default:
            wxLogMessage(_T("[lmLDPParser::GetDefaultDuration] TODO: other tuplets different from triplet"));
            //! @todo other tuplets different from triplet
            ;
    }
    
    return rDuration;
}

int lmLDPParser::GetBeamingLevel(ENoteType nNoteType)
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