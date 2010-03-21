//--------------------------------------------------------------------------------------
//  LenMus Library
//  Copyright (c) 2010 LenMus project
//
//  This program is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along
//  with this library; if not, see <http://www.gnu.org/licenses/> or write to the
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
//  MA  02111-1307,  USA.
//
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include "LdpParser.h"
#include "LdpReader.h"
#include "../elements/Factory.h"
#include "../elements/Elements.h"
//#include "AuxString.h"

using namespace std;

namespace lenmus
{


//========================================================================================
// LdpParser implementation
//========================================================================================

LdpParser::LdpParser(tostream& reporter)
    : m_reporter(reporter)
    //, m_fDebugMode(g_pLogger->IsAllowedTraceMask(_T("LdpParser")))
    //, m_pIgnoreSet((std::set<long>*)NULL)
    , m_pTokenizer(NULL)
{
}

LdpParser::~LdpParser()
{
    if (m_pTokenizer)
        delete m_pTokenizer;
    m_stack.empty();
}

void LdpParser::initialize()
{
    m_pTk = NULL;
    m_stack.empty();
    m_numErrors = 0;
    m_curNode = NULL;
    m_stack.empty();
}

SpLdpElement LdpParser::parse_text(const string_type& sourceText)
{
    LdpTextReader reader(sourceText);
    return do_syntax_analysis(reader);
}

SpLdpElement LdpParser::parse_file(const string_type& filename, bool fErrorMsg)
{
    LdpFileReader reader(filename);
    return do_syntax_analysis(reader);
}

SpLdpElement LdpParser::do_syntax_analysis(LdpReader& reader)
{
    //This function analyzes source code. The result of the analysis is a tree
    //of nodes, each one representing an element. The root node is the parsed
    //elemnent, usually the whole score. Nevertheless, the parser can be used
    //to parse any sub-element, such as a note, or a measure.
    //
    //This method performs a the lexical analysis and syntactical analysis and,
    //as result, builds a tree of syntactically correct nodes: the source code
    //has the structure of an element with nested elements (data between parenthesis).
    //
    //The analyzer is implemented with a main loop to deal with current
    //automata state and as many functions as automata states, to perform the
    //tasks asociated to each state.

    initialize();

    m_pTokenizer = new LdpTokenizer(reader, m_reporter);
    m_state = A0_WaitingForStartOfElement;
    PushNode(A0_WaitingForStartOfElement);      //start the tree with the root node
    bool fExitLoop = false;
    while(!fExitLoop)
    {
        m_pTk = m_pTokenizer->read_token();        //m_pTk->read_token();

        //// if debug mode write trace
        //if (m_fDebugMode) {
        //    if (m_pTk->get_type() == tkEndOfElement) {
        //        wxLogMessage(
        //            _T("**TRACE** State %d, TkType %s, tkValue <%s>, node <%s>"),
        //            m_state, m_pTk->get_description().c_str(),
        //            m_pTk->get_value().c_str(), m_curNode->GetName().c_str() );
        //    } else {
        //        wxLogMessage(
        //            _T("**TRACE** State %d, TkType %s, tkValue <%s>"),
        //            m_state, m_pTk->get_description().c_str(),
        //            m_pTk->get_value().c_str() );
        //    }
        //}

        switch (m_state) {
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
                report_error(m_state, m_pTk);
                fExitLoop = true;
        }
        if (m_pTk->get_type() == tkEndOfFile)
            fExitLoop = true;
    }

    // exit if error
    if (m_state == A5_ExitError)
        return NULL;

    // at this point m_curNode is all the tree. Verify it.
    if (m_curNode->get_name() != _T("score"))
    {
        report_error(m_curNode, _T("'score' element expected but found element '%s'. Analysis stopped."),
            m_curNode->get_name().c_str() );
        //m_curNode->DumpNode();
        return (SpLdpElement) NULL;
    }
    return m_curNode;
}

void LdpParser::Do_WaitingForStartOfElement()
{
    switch (m_pTk->get_type())
    {
        case tkStartOfElement:
            m_state = A1_WaitingForName;
            break;
        case tkEndOfFile:
            m_state = A4_Exit;
            break;
        default:
            report_error(m_state, m_pTk);
            m_state = A0_WaitingForStartOfElement;
    }
}

void LdpParser::Do_WaitingForName()
{
    switch (m_pTk->get_type())
    {
        case tkLabel:
            //m_curNode = new LdpNode(m_pTk->get_value(), m_numLine, false);      //false -> it is not a parameter node
            m_curNode = Factory::instance().create(m_pTk->get_value()); //, m_numLine);
            if (!m_curNode)
                m_curNode = Factory::instance().create(k_undefined);  //, m_numLine);
            //if (m_fDebugMode) {
            //    wxLogMessage(
            //        _T("**TRACE** State %d, TkType %s : creando nuevo nodo <%s>"),
            //        m_state, m_pTk->get_description().c_str(),
            //        m_curNode->GetName().c_str() );
            //}
            m_state = A2_WaitingForParameter;
            break;
        default:
            report_error(m_state, m_pTk);
            if (m_pTk->get_type() == tkEndOfFile)
                m_state = A4_Exit;
            else
                m_state = A1_WaitingForName;
    }

}

void LdpParser::Do_WaitingForParameter()
{
    switch (m_pTk->get_type())
    {
        case tkStartOfElement:
            PushNode(A3_ProcessingParameter);    // add current node (name of element or parameter) to the tree
            m_pTokenizer->repeat_token();
            m_state = A0_WaitingForStartOfElement;
            break;
        case tkLabel:
            m_curNode->push( new_label(m_pTk->get_value()) );
            m_state = A3_ProcessingParameter;
            break;
        case tkIntegerNumber:
        case tkRealNumber:
            m_curNode->push( new_number(m_pTk->get_value()) );
            m_state = A3_ProcessingParameter;
            break;
        case tkString:
            m_curNode->push( new_string(m_pTk->get_value()) );
            m_state = A3_ProcessingParameter;
            break;
        default:
            report_error(m_state, m_pTk);
            if (m_pTk->get_type() == tkEndOfFile)
                m_state = A4_Exit;
            else
                m_state = A2_WaitingForParameter;
    }
}

void LdpParser::Do_ProcessingParameter()
{
    switch (m_pTk->get_type())
    {
        case tkLabel:
            m_curNode->push( new_label(m_pTk->get_value()) );
            m_state = A3_ProcessingParameter;
            break;
        case tkIntegerNumber:
        case tkRealNumber:
            m_curNode->push( new_number(m_pTk->get_value()) );
            m_state = A3_ProcessingParameter;
            break;
        case tkString:
            m_curNode->push( new_string(m_pTk->get_value()) );
            m_state = A3_ProcessingParameter;
            break;
        case tkStartOfElement:
            PushNode(A3_ProcessingParameter);    // add current node (name of element or parameter) to the tree
            m_pTokenizer->repeat_token();
            m_state = A0_WaitingForStartOfElement;
            break;
        case tkEndOfElement:
            {
            SpLdpElement pParm = m_curNode;        //save ptr to node just created
            if (PopNode()) {                      //restore previous node (the owner of this parameter)
                //error
                m_state = A5_ExitError;
            }
            else
            {
                if (m_curNode)
                    m_curNode->push(pParm);
                else
                    m_curNode = pParm;

                ////Filter out this element if its ID is in the ignore list
                //long nID = GetNodeID(pParm);
                //if (!(m_pIgnoreSet
                //      && nID != lmNEW_ID
                //      && m_pIgnoreSet->find(nID) != m_pIgnoreSet->end() ))
                //    m_curNode->push(pParm);
                //else
                //    delete pParm;   //discard this node
            }
            break;
            }
        default:
            report_error(m_state, m_pTk);
            if (m_pTk->get_type() == tkEndOfFile)
                m_state = A4_Exit;
            else
                m_state = A3_ProcessingParameter;
    }
}

void LdpParser::PushNode(EParsingState state)
{
    std::pair<EParsingState, SpLdpElement> data(state, m_curNode);
    m_stack.push(data);
}

//! returns true if error
bool LdpParser::PopNode()
{
    if (m_stack.size() == 0)
    {
        //more closing parenthesis than parenthesis opened
        report_error(NULL, _T("Syntax error: more closing parenthesis than parenthesis opened. Analysis stopped."));
        return true;    //error
    }
    else
    {
        std::pair<EParsingState, SpLdpElement> data = m_stack.top();
        m_state = data.first;
        m_curNode = data.second;
        m_stack.pop();
        return false;   //no error
    }
}

void LdpParser::report_error(EParsingState nState, LdpToken* pTk)
{
    //m_numErrors++;
    //wxLogMessage(_T("** LDP ERROR **: Syntax error. State %d, TkType %s, tkValue <%s>"),
    //        nState, pTk->get_description().c_str(), pTk->get_value().c_str() );
}

void LdpParser::report_error(SpLdpElement pNode, const char_type* szFormat, ...)
{
    //m_numErrors++;

    //va_list argptr;
    //va_start(argptr, szFormat);
    //string_type sMsg;
    //if (pNode)
    //    sMsg << _T("** LDP ERROR ** (line ") << pNode->GetNumLine() << _T("): "); 
    //else
    //    sMsg << _T("** LDP ERROR **: ");

    //sMsg += string_type::FormatV(szFormat, argptr);
    //wxLogMessage(sMsg);
    //if (m_fFromString)
    //    wxLogMessage(m_sLastBuffer);
    //g_pLogger->LogDataError(sMsg);
    //va_end(argptr);
}


//========================================================================================
//========================================================================================
//========================================================================================
#if 0
long LdpParser::GetNodeID(SpLdpElement pNode)
{
    long nID = pNode->GetID();
    m_nMaxID = wxMax(m_nMaxID, nID);
    return nID;
}

bool LdpParser::ParenthesisMatch(const string_type& sSource)
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


float LdpParser::GetDefaultDuration(lmENoteType nNoteType, int nDots, int nActualNotes,
                                    int nNormalNotes)
{
    //compute duration without modifiers
    float rDuration = NoteTypeToDuration(nNoteType, nDots);

    //alter by tuplet modifiers
    if (nActualNotes != 0) rDuration = (rDuration * (float)nNormalNotes) / (float)nActualNotes;

    return rDuration;
}

int LdpParser::GetBeamingLevel(lmENoteType nNoteType)
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

bool LdpParser::AnalyzeNoteType(string_type& sNoteType, lmENoteType* pnNoteType,
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
    string_type sType;
    string_type sDots;
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
#endif

} //namespace lenmus

