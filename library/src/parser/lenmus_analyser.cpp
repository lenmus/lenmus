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

#include <iostream>
#include <sstream>

#include <vector>
#include <algorithm>   // for find

#include "lenmus_factory.h"
#include "lenmus_tree.h"
#include "lenmus_analyser.h"
#include "lenmus_values.h"
#include "lenmus_parser.h"
#include "lenmus_internal_model.h"
#include "lenmus_im_note.h"

using namespace std;

namespace lenmus
{

//-------------------------------------------------------------------------------------
// The syntax analyser is based on the Interpreter pattern ()
//
// The basic idea is to have a class for each language symbol, terminal or nonterminal
// (classes derived from ElementAnalyser). The parse tree created by the Parser is an
// instance of the composite pattern and is traversed by the analysers to evaluate
// (interpret) the sentence.
//
// The result of this step (semantic analysis) is a decorated parse tree, that is, the
// parse tree with a ImObj added to certain nodes. The ImObj collects the information
// present on the subtree:
//      Input: parse tree.
//      Output: parse tree with ImObjs
//


//-------------------------------------------------------------------------------------
// Abstract class: any element analyser must derive from it

class ElementAnalyser
{
protected:
    Analyser* m_pAnalyser;
    ostream& m_reporter;
    LdpFactory* m_pLdpFactory;

public:
    ElementAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : m_reporter(reporter)
        , m_pAnalyser(pAnalyser)
        , m_pLdpFactory(pFactory) {}
    virtual ~ElementAnalyser() {}
    void analyse_node(LdpElement* pNode);

protected:

    //analysis
    virtual void do_analysis() = 0;

    //error reporting
    bool error_missing_element(ELdpElements type);
    void report_msg(int numLine, const std::string& msg);
    void report_msg(int numLine, const std::stringstream& msg);

    //helpers, to simplify writting grammar rules
    LdpElement* m_pAnalysedNode;
    LdpElement* m_pParamToAnalyse;
    LdpElement* m_pNextParam;
    LdpElement* m_pNextNextParam;

    bool get_mandatory(ELdpElements type);
    void analyse_mandatory(ELdpElements type);
    bool get_optional(ELdpElements type);
    bool analyse_optional(ELdpElements type);
    void analyse_one_or_more(ELdpElements* pValid, int nValid);
    void error_if_more_elements();

    //auxiliary
    bool contains(ELdpElements type, ELdpElements* pValid, int nValid);
    void error_and_remove_invalid();
    inline bool more_params_to_analyse() {
        return m_pNextParam != NULL;
    }
    inline LdpElement* get_param_to_analyse() {
        return m_pNextParam;
    }
    inline void move_to_next_param() {
        m_pNextParam = m_pNextNextParam;
        prepare_next_one();
    }
    inline void prepare_next_one() {
        if (m_pNextParam)
            m_pNextNextParam = m_pNextParam->get_next_sibling();
        else
            m_pNextNextParam = NULL;
    }
    inline void move_to_first_param() {
        m_pNextParam = m_pAnalysedNode->get_first_child();
        prepare_next_one();
    }
    void get_num_staff()
    {
        string staff = m_pParamToAnalyse->get_value().substr(1);
        int nStaff;
        //http://www.codeguru.com/forum/showthread.php?t=231054
        std::istringstream iss(staff);
        if ((iss >> std::dec >> nStaff).fail())
        {
            report_msg(m_pParamToAnalyse->get_line_number(),
                "Invalid staff 'p" + staff + "'. Replaced by 'p1'.");
            LdpElement* value = m_pLdpFactory->new_value(k_label, "p1");
            m_pAnalyser->replace_node(m_pParamToAnalyse, value);
            m_pAnalyser->change_staff(0);
        }
        else
            m_pAnalyser->change_staff(--nStaff);
    }
    int get_integer_number(int nDefault)
    {
        string number = m_pParamToAnalyse->get_value();
        int nNumber;
        std::istringstream iss(number);
        if ((iss >> std::dec >> nNumber).fail())
        {
            stringstream replacement;
            replacement << nDefault;
            report_msg(m_pParamToAnalyse->get_line_number(),
                "Invalid integer number '" + number + "'. Replaced by '"
                + replacement.str() + "'.");
            LdpElement* value = m_pLdpFactory->new_value(k_number, replacement.str());
            m_pAnalyser->replace_node(m_pParamToAnalyse, value);
            return nDefault;
        }
        else
            return nNumber;
    }
    float get_float_number(float rDefault=0.0f)
    {
        string number = m_pParamToAnalyse->get_value();
        float rNumber;
        std::istringstream iss(number);
        if ((iss >> std::dec >> rNumber).fail())
        {
            stringstream replacement;
            replacement << rDefault;
            report_msg(m_pParamToAnalyse->get_line_number(),
                "Invalid real number '" + number + "'. Replaced by '"
                + replacement.str() + "'.");
            LdpElement* value = m_pLdpFactory->new_value(k_number, replacement.str());
            m_pAnalyser->replace_node(m_pParamToAnalyse, value);
            return rDefault;
        }
        else
            return rNumber;
    }
    string get_string()
    {
        return m_pParamToAnalyse->get_value();
    }


};

//-------------------------------------------------------------------------------------
// default analyser to use when there is no defined analyser for an LDP element

class NullAnalyser : public ElementAnalyser
{
public:
    NullAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        string name = m_pLdpFactory->get_name( m_pAnalysedNode->get_type() );
        cout << "Missing analyser for element '" << name << "'. Node removed." << endl;
        m_pAnalyser->erase_node(m_pAnalysedNode);
    }
};

//-------------------------------------------------------------------------------------
// <barline> = (barline <type>[<visible>][<location>])
// <type> = label: { start | end | double | simple | startRepetition |
//                   endRepetition | doubleRepetition }

class BarlineAnalyser : public ElementAnalyser
{
public:
    BarlineAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImBarline* pBarline = new ImBarline();
        m_pAnalysedNode->set_imobj(pBarline);

        // <type> (label)
        if (get_optional(k_label))
            pBarline->set_type( get_barline_type() );

        //TODO: [<visible>][<location>]

        error_if_more_elements();
    }

protected:

    int get_barline_type()
    {
        string value = m_pParamToAnalyse->get_value();
        int type = ImBarline::kSimple;
        if (value == "simple")
            type = ImBarline::kSimple;
        else if (value == "double")
            type = ImBarline::kDouble;
        else if (value == "start")
            type = ImBarline::kStart;
        else if (value == "end")
            type = ImBarline::kEnd;
        else if (value == "endRepetition")
            type = ImBarline::kEndRepetition;
        else if (value == "startRepetition")
            type = ImBarline::kStartRepetition;
        else if (value == "doubleRepetition")
            type = ImBarline::kDoubleRepetition;
        else
        {
            report_msg(m_pParamToAnalyse->get_line_number(),
                    "Unknown barline type '" + value + "'. 'simple' barline assumed.");
            LdpElement* value = m_pLdpFactory->new_value(k_label, "simple");
            m_pAnalyser->replace_node(m_pParamToAnalyse, value);
        }

        return type;
    }

};

//-------------------------------------------------------------------------------------
// <clef> = (clef <type>[<numStaff>][<visible>][<location>] )
// <type> = label: { G | F4 | F3 | C1 | C2 | C3 | C4 | percussion |
//                   C3 | C5 | F5 | G1 | 8_G | G_8 | 8_F4 | F4_8 |
//                   15_G | G_15 | 15_F4 | F4_15 }

class ClefAnalyser : public ElementAnalyser
{
public:
    ClefAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImClef* pClef = new ImClef();
        m_pAnalysedNode->set_imobj(pClef);

        // <type> (label)
        if (get_optional(k_label))
            pClef->set_type( get_clef_type() );

        //<numStaff>
        if (get_optional(k_label))
        {
            char type = (m_pParamToAnalyse->get_value())[0];
            if (type == 'p')
                get_num_staff();
            else
                error_and_remove_invalid();
        }

        //TODO: [<visible>][<location>]

        error_if_more_elements();

        //set values that can be inherited
        m_pAnalyser->set_staff(pClef);
    }

    int get_clef_type()
    {
        string value = m_pParamToAnalyse->get_value();
        long type = ImClef::kG3;
        if (value == "G")
            type = ImClef::kG3;
        else if (value == "F4")
            type = ImClef::kF4;
        else if (value == "F3")
            type = ImClef::kF3;
        else if (value == "C1")
            type = ImClef::kC1;
        else if (value == "C2")
            type = ImClef::kC2;
        else if (value == "C3")
            type = ImClef::kC3;
        else if (value == "C4")
            type = ImClef::kC4;
        else if (value == "percussion")
            type = ImClef::kPercussion;
        else if (value == "C3")
            type = ImClef::kC3;
        else if (value == "C5")
            type = ImClef::kC5;
        else if (value == "F5")
            type = ImClef::kF5;
        else if (value == "G1")
            type = ImClef::kG1;
        else if (value == "8_G")
            type = ImClef::k8_G3;
        else if (value == "G_8")
            type = ImClef::kG3_8;
        else if (value == "8_F4")
            type = ImClef::k8_F4;
        else if (value == "F4_8")
            type = ImClef::kF4_8;
        else if (value == "15_G")
            type = ImClef::k15_G3;
        else if (value == "G_15")
            type = ImClef::kG3_15;
        else if (value == "15_F4")
            type = ImClef::k15_F4;
        else if (value == "F4_15")
            type = ImClef::kF4_15;
       else
        {
            report_msg(m_pParamToAnalyse->get_line_number(),
                    "Unknown clef type '" + value + "'. Assumed 'G'.");
            LdpElement* value = m_pLdpFactory->new_value(k_label, "G");
            m_pAnalyser->replace_node(m_pParamToAnalyse, value);
        }

        return type;
    }

};

//-------------------------------------------------------------------------------------
// <content> = (content [<score>|<text>]*)

class ContentAnalyser : public ElementAnalyser
{
public:
    ContentAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        while (more_params_to_analyse())
        {
            if (! (analyse_optional(k_score)
                 || analyse_optional(k_text) ))
            {
                error_and_remove_invalid();
                move_to_next_param();
            }
        }

        error_if_more_elements();
    }
};

//-------------------------------------------------------------------------------------
// <newSystem> = (newSystem}

class ControlAnalyser : public ElementAnalyser
{
public:
    ControlAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImControl* pCtrol = new ImControl();
        m_pAnalysedNode->set_imobj(pCtrol);
    }
};
////bool lmLDPParser::AnalyzeNewSystem(lmLDPNode* pNode, lmVStaff* pVStaff)
////{
////    //returns true if error; in this case nothing is added to the lmVStaff
////    //<newSystem> ::= (newSystem}
////
////    wxASSERT(GetNodeName(pNode) == _T("newSystem"));
////    long nId = GetNodeID(pNode);
////
////    //check if there are parameters
////    if(GetNodeNumParms(pNode) >= 1) {
////        //for now, no parameters allowed
////        wxASSERT(false);
////        return true;
////    }
////
////    //add control object
////    lmSOControl* pSO = pVStaff->AddNewSystem(nId);
////
////    //save cursor data
////    if (m_fCursorData && m_nCursorObjID == nId)
////        m_pCursorSO = pSO;
////
////    return false;
////
////}
////

//-------------------------------------------------------------------------------------
//  <figuredBass> = (figuredBass <figuredBassSymbols>[<parentheses>][<fbline>])
//  <parentheses> = (parentheses { yes | no })  default: no
//
//  <figuredBassSymbols> = an string.
//        It is formed by concatenation of individual strings for each interval.
//        Each interval string is separated by a blank space from the previous one.
//        And it can be enclosed in parenthesis.
//        Each interval string is a combination of prefix, number and suffix,
//        such as  "#3", "5/", "(3)", "2+" or "#".
//        Valid values for prefix and suffix are:
//            prefix = { + | - | # | b | = | x | bb | ## }
//            suffix = { + | - | # | b | = | x | bb | ## | / | \ }
//
//  examples:
//
//    b6              (figuredBass "b6 b")
//    b
//
//    6               (figuredBass "6 (3)")
//   (3)
//

class FiguredBassAnalyser : public ElementAnalyser
{
public:
    FiguredBassAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImFiguredBass* pImo = new ImFiguredBass();
        m_pAnalysedNode->set_imobj(pImo);

        // <figuredBassSymbols> (string)
        if (get_mandatory(k_string))
        {}
    }
};

////    //get figured bass string and split it into components
////    wxString sData = GetNodeName(pNode->GetParameter(1));
////    lmFiguredBassData oFBData(sData);
////    if (oFBData.GetError() != _T(""))
////    {
////        AnalysisError(pNode, oFBData.GetError());
////        return (lmFiguredBass*)NULL;    //error
////    }
////
////    //initialize options with default values
////    //AWARE: There can be two fblines, one starting in this FB and another
////    //one ending in it.
////    int nFBL=0;     //index to next fbline
////    lmFBLineInfo* pFBLineInfo[2];
////    pFBLineInfo[0] = (lmFBLineInfo*)NULL;
////    pFBLineInfo[1] = (lmFBLineInfo*)NULL;
////
////    //get options: <parenthesis> & <fbline>
////    int iP;
////    for(iP=2; iP <= nNumParms; ++iP)
////    {
////        lmLDPNode* pX = pNode->GetParameter(iP);
////        wxString sName = GetNodeName(pX);
////        if (sName == _T("parenthesis"))
////            ;   //TODO
////        else if (sName == _T("fbline"))     //start/end of figured bass line
////        {
////            if (nFBL > 1)
////                AnalysisError(pX, _T("[Element '%s'. More than two 'fbline'. Ignored."),
////                            sElmName.c_str() );
////            else
////                pFBLineInfo[nFBL++] = AnalyzeFBLine(pX, pVStaff);
////        }
////        else
////            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
////                          sElmName.c_str(), sName.c_str() );
////    }
////
////    //analyze remaining optional parameters: <location>, <cursorPoint>
////	lmLDPOptionalTags oOptTags(this);
////	oOptTags.SetValid(lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1
////	lmLocation tPos = g_tDefaultPos;
////	oOptTags.AnalyzeCommonOptions(pNode, iP, pVStaff, NULL, NULL, &tPos);
////
////	//create the Figured Bass object
////    lmFiguredBass* pFB = pVStaff->AddFiguredBass(&oFBData, nId);
////	pFB->SetUserLocation(tPos);
////
////    //save cursor data
////    if (m_fCursorData && m_nCursorObjID == nId)
////        m_pCursorSO = pFB;
////
////    //add FB line, if exists
////    for(int i=0; i < 2; i++)
////    {
////        if (pFBLineInfo[i])
////        {
////            if (pFBLineInfo[i]->fStart)
////            {
////                //start of FB line. Save the information
////                pFBLineInfo[i]->pFB = pFB;
////                m_PendingFBLines.push_back(pFBLineInfo[i]);
////            }
////            else
////            {
////                //end of FB line. Add it to the internal model
////                AddFBLine(pFB, pFBLineInfo[i]);
////            }
////        }
////    }
////
////    return pFB;       //no error
////}

//-------------------------------------------------------------------------------------
// <goBack> = (goBack <timeShift>)
// <goFwd> = (goFwd <timeShift>)
// <timeShift> = { start | end | <number> | <duration> }
//
// the time shift can be:
//   a) one of the tags 'start' and 'end': i.e. (goBack start) (goFwd end)
//   b) a number: the amount of 256th notes to go forward or backwards
//   c) a note/rest duration, i.e. 'e..'

class GoBackFwdAnalyser : public ElementAnalyser
{
public:
    GoBackFwdAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        bool fFwd = m_pAnalysedNode->is_type(k_goFwd);
        ImGoBackFwd* pImo = new ImGoBackFwd(fFwd);
        m_pAnalysedNode->set_imobj(pImo);

        // <duration> |start | end (label) or <number>
        if (get_optional(k_label))
        {
            string duration = m_pParamToAnalyse->get_value();
            if (duration == "start")
            {
                if (!fFwd)
                    pImo->set_to_start();
                else
                {
                    report_msg(m_pParamToAnalyse->get_line_number(),
                        "Element 'goFwd' has an incoherent value: go forward to start?. Element ignored.");
                    m_pAnalyser->erase_node(m_pAnalysedNode);
                    return;
                }
            }
            else if (duration == "end")
            {
                if (fFwd)
                    pImo->set_to_end();
                else
                {
                    report_msg(m_pParamToAnalyse->get_line_number(),
                        "Element 'goBack' has an incoherent value: go backwards to end?. Element ignored.");
                    m_pAnalyser->erase_node(m_pAnalysedNode);
                    return;
                }
            }
            else
            {
                int noteType, dots;
                if (ldp_duration_to_components(duration, &noteType, &dots))
                {
                    report_msg(m_pParamToAnalyse->get_line_number(),
                        "Unknown duration '" + duration + "'. Element ignored.");
                    m_pAnalyser->erase_node(m_pParamToAnalyse);
                    return;
                }
                else
                {
                    float rTime = to_duration(noteType, dots);
                    pImo->set_time_shift(rTime);
                }
            }
        }
        else if (get_optional(k_number))
        {
            float rTime = m_pParamToAnalyse->get_value_as_float();
            if (rTime < 0.0f)
            {
                report_msg(m_pParamToAnalyse->get_line_number(),
                    "Negative value for element 'goFwd/goBack'. Element ignored.");
                m_pAnalyser->erase_node(m_pAnalysedNode);
                return;
            }
            else
                pImo->set_time_shift(rTime);
        }
        else
        {
            report_msg(m_pParamToAnalyse->get_line_number(),
                "Unknown duration '" + m_pParamToAnalyse->get_name() + "'. Element ignored.");
            m_pAnalyser->erase_node(m_pParamToAnalyse);
            return;
        }

        error_if_more_elements();
    }
};

//-------------------------------------------------------------------------------------
// <instrument> = (instrument [<instrName>][<instrAbbrev>][<infoMIDI>]
//                            [<staves>] <musicData> )

class InstrumentAnalyser : public ElementAnalyser
{
public:
    InstrumentAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImInstrument* pInstrument = new ImInstrument();
        m_pAnalysedNode->set_imobj(pInstrument);

        // [<name>]
        analyse_optional(k_name);

        // [<abbrev>]
        analyse_optional(k_abbrev);

        // [<infoMIDI>]
        analyse_optional(k_infoMIDI);

        // [<staves>]
        if (get_optional(k_staves))
            set_staves(pInstrument);

        // <musicData>
        analyse_mandatory(k_musicData);

        error_if_more_elements();
    }


    void set_staves(ImInstrument* pInstrument)
    {
        // <staves> = (staves <num>)

        LdpElement* pValue = m_pParamToAnalyse->get_first_child();
        string staves = pValue->get_value();
        int nStaves;
        bool fError = !pValue->is_type(k_number);
        if (!fError)
        {
            std::istringstream iss(staves);
            fError = (iss >> std::dec >> nStaves).fail();
        }
        if (fError)
        {
            report_msg(m_pParamToAnalyse->get_line_number(),
                "Invalid value '" + staves + "' for staves. Replaced by 1.");
            LdpElement* value = m_pLdpFactory->new_value(k_number, "1");
            m_pAnalyser->replace_node(m_pParamToAnalyse, value);
            pInstrument->set_num_staves(1);
        }
        else
        {
            //TODO: check that maximum number of supported staves is not reached
            //sNumStaves.ToLong(&nNumStaves);
            //if (nNumStaves > lmMAX_STAFF)
            //{
            //    AnalysisError(pX, _T("Program limit reached: the number of staves per instrument must not be greater than %d. Please inform LenMus developers."),
            //                  lmMAX_STAFF);
            //    nNumStaves = lmMAX_STAFF;
            //}
            pInstrument->set_num_staves(nStaves);
        }
    }

};

//-------------------------------------------------------------------------------------
// <key> = (key <type>[<visible>][<location>] )
// <type> = label: { G | F4 | F3 | C1 | C2 | C3 | C4 | percussion |
//                   C3 | C5 | F5 | G1 | 8_G | G_8 | 8_F4 | F4_8 |
//                   15_G | G_15 | 15_F4 | F4_15 }

class KeySignatureAnalyser : public ElementAnalyser
{
public:
    KeySignatureAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImKeySignature* pKeySignature = new ImKeySignature();
        m_pAnalysedNode->set_imobj(pKeySignature);

        // <type> (label)
        if (get_optional(k_label))
            pKeySignature->set_type( get_key_type() );

        //TODO: [<visible>][<location>]

        error_if_more_elements();
    }

    int get_key_type()
    {
        string value = m_pParamToAnalyse->get_value();
        int type = ImKeySignature::C;
        if (value == "C")
            type = ImKeySignature::C;
        else if (value == "G")
            type = ImKeySignature::G;
        else if (value == "D")
            type = ImKeySignature::D;
        else if (value == "A")
            type = ImKeySignature::A;
        else if (value == "E")
            type = ImKeySignature::E;
        else if (value == "B")
            type = ImKeySignature::B;
        else if (value == "F+")
            type = ImKeySignature::Fs;
        else if (value == "C+")
            type = ImKeySignature::Cs;
        else if (value == "C-")
            type = ImKeySignature::Cf;
        else if (value == "G-")
            type = ImKeySignature::Gf;
        else if (value == "D-")
            type = ImKeySignature::Df;
        else if (value == "A-")
            type = ImKeySignature::Af;
        else if (value == "E-")
            type = ImKeySignature::Ef;
        else if (value == "B-")
            type = ImKeySignature::Bf;
        else if (value == "F")
            type = ImKeySignature::F;
        else if (value == "a")
            type = ImKeySignature::a;
        else if (value == "e")
            type = ImKeySignature::e;
        else if (value == "b")
            type = ImKeySignature::b;
        else if (value == "f+")
            type = ImKeySignature::fs;
        else if (value == "c+")
            type = ImKeySignature::cs;
        else if (value == "g+")
            type = ImKeySignature::gs;
        else if (value == "d+")
            type = ImKeySignature::ds;
        else if (value == "a+")
            type = ImKeySignature::as;
        else if (value == "a-")
            type = ImKeySignature::af;
        else if (value == "e-")
            type = ImKeySignature::ef;
        else if (value == "b-")
            type = ImKeySignature::bf;
        else if (value == "f")
            type = ImKeySignature::f;
        else if (value == "c")
            type = ImKeySignature::c;
        else if (value == "g")
            type = ImKeySignature::g;
        else if (value == "d")
            type = ImKeySignature::d;
       else
        {
            report_msg(m_pParamToAnalyse->get_line_number(),
                    "Unknown key '" + value + "'. Assumed 'C'.");
            LdpElement* value = m_pLdpFactory->new_value(k_label, "C");
            m_pAnalyser->replace_node(m_pParamToAnalyse, value);
        }

        return type;
    }

};

//-------------------------------------------------------------------------------------
// <language> = (language <languageCode> <charset>)
// obsolete since vers 1.6

class LanguageAnalyser : public ElementAnalyser
{
public:
    LanguageAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        m_pAnalyser->erase_node(m_pAnalysedNode);
    }
};

//-------------------------------------------------------------------------------------
// <lenmusdoc> = (lenmusdoc <vers> <content>)

class LenmusdocAnalyser : public ElementAnalyser
{
public:
    LenmusdocAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        // <vers>
        if (get_mandatory(k_vers))
        {
            ;   //pScore->set_version( get_version() );
        }

        // <content>
        analyse_mandatory(k_content);

        error_if_more_elements();
    }
};

//-------------------------------------------------------------------------------------
// <metronome> = (metronome { <NoteType>
//                            | { <TicksPerMinute> | <NoteType> }
//                            | <TicksPerMinute> }
//                          [parentheses][<Visible>] )

class MetronomeAnalyser : public ElementAnalyser
{
public:
    MetronomeAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImMetronomeMark* pMM = new ImMetronomeMark();
        m_pAnalysedNode->set_imobj(pMM);
    }
};

//////returns true if error; in this case nothing is added to the lmVStaff
////bool lmLDPParser::AnalyzeMetronome(lmLDPNode* pNode, lmVStaff* pVStaff)
////{
////
////    wxString sElmName = GetNodeName(pNode);
////    wxASSERT(sElmName == _T("metronome"));
////    long nId = GetNodeID(pNode);
////
////    //check that at least one parameter is specified
////    int nNumParms = GetNodeNumParms(pNode);
////    if(nNumParms < 1) {
////        AnalysisError(
////            pNode,
////            _T("Element '%s' has less parameters than the minimum required. Ignored'."),
////            sElmName.c_str() );
////        return true;    //error
////    }
////
////    long iP = 1;
////    wxString sName = GetNodeName(pNode->GetParameter(iP));
////
////    EMetronomeMarkType nMarkType;
////    long nTicksPerMinute = 0;
////    int nDots = 0;
////    lmENoteType nLeftNoteType = eQuarter, nRightNoteType = eQuarter;
////    int nLeftDots = 0, nRightDots = 0;
////
////    //analize first parameter: value or left mark
////    wxString sData = GetNodeName(pNode->GetParameter(iP));
////    iP++;
////    if (sData.IsNumber()) {
////        //numeric value. Assume it is the ticks per minute rate
////        sData.ToLong(&nTicksPerMinute);
////        nMarkType = eMMT_MM_Value;
////    }
////    else {
////        //string value. Assume it is mark type (note duration and dots)
////        if (AnalyzeNoteType(sData, &nLeftNoteType, &nDots)) {
////            AnalysisError(pNode, _T("Unknown note/rest duration '%s'. A quarter note assumed."),
////                sData.c_str() );
////        }
////        nLeftDots = nDots;
////
////        // Get right part
////        if (iP > nNumParms) {
////            AnalysisError(
////                pNode,
////                _T("Element '%s' has less parameters than the minimum required. Ignored'."),
////                sElmName.c_str() );
////            return true;    //error
////        }
////        sData = GetNodeName(pNode->GetParameter(iP));
////        iP++;
////        if (sData.IsNumber()) {
////            //numeric value. Assume it is the ticks per minute rate
////            sData.ToLong(&nTicksPerMinute);
////            nMarkType = eMMT_Note_Value;
////        }
////        else {
////            //string value. Assume it is mark type (note duration and dots)
////            nMarkType = eMMT_Note_Note;
////            if (AnalyzeNoteType(sData, &nRightNoteType, &nDots)) {
////                AnalysisError(pNode, _T("Unknown note/rest duration '%s'. A quarter note assumed."),
////                    sData.c_str() );
////            }
////            nRightDots = nDots;
////        }
////    }
////
////    //Get optional 'parentheses' parameter
////    bool fParentheses = false;
////#if lmUSE_LIBRARY
////    lmLDPNode* pX = pNode->GetParameterFromName( "parentheses" );
////#else
////    lmLDPNode* pX = pNode->GetParameterFromName( _T("parentheses") );
////#endif
////    if (pX) fParentheses = true;
////
////    //Get common optional parameters
////	lmLDPOptionalTags oOptTags(this);
////	oOptTags.SetValid(lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1
////	oOptTags.SetValid(lm_eTag_Visible, lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1
////
////	lmLocation tPos = g_tDefaultPos;
////    bool fVisible = true;
////
////	oOptTags.AnalyzeCommonOptions(pNode, iP, pVStaff, &fVisible, NULL, &tPos);
////
////    //bool fVisible = true;
////    //for (; iP <= nNumParms; iP++) {
////    //    pX = pNode->GetParameter(iP);
////    //    if (GetNodeName(pX) == _T("noVisible"))
////    //        fVisible = false;
////    //    else if (GetNodeName(pX) == _T("parentheses"))
////    //        fParentheses = true;
////    //    else {
////    //        AnalysisError(pX, _T("Unknown parameter '%s'. Ignored."), GetNodeName(pX).c_str());
////    //    }
////    //}
////
////    //create the metronome mark StaffObj
////    lmMetronomeMark* pMM;
////    switch (nMarkType)
////    {
////        case eMMT_MM_Value:
////            pMM = pVStaff->AddMetronomeMark(nTicksPerMinute, fParentheses, fVisible, nId);
////            break;
////        case eMMT_Note_Note:
////            pMM = pVStaff->AddMetronomeMark(nLeftNoteType, nLeftDots, nRightNoteType, nRightDots,
////                            fParentheses, fVisible, nId);
////            break;
////        case eMMT_Note_Value:
////            pMM = pVStaff->AddMetronomeMark(nLeftNoteType, nLeftDots, nTicksPerMinute,
////                            fParentheses, fVisible, nId);
////            break;
////        default:
////            wxASSERT(false);
////    }
////
////    //set location
////	pMM->SetUserLocation(tPos);
////
////    //save cursor data
////    if (m_fCursorData && m_nCursorObjID == nId)
////        m_pCursorSO = pMM;
////
////    return false;    //no error
////
////}

//-------------------------------------------------------------------------------------
// <musicData> = (musicData [{<note>|<rest>|<barline>|<chord>|<clef>|<figuredBass>|
//                            <graphic>|<key>|<metronome>|<newSystem>|<spacer>|
//                            <text>|<time>|<goFwd>|<goBack>}*] )

class MusicDataAnalyser : public ElementAnalyser
{
public:
    MusicDataAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        // [{<xxxx>|<yyyy>|<zzzz>}*]    alternatives: zero or more
        while (more_params_to_analyse())
        {
            if (! (analyse_optional(k_note)
                   || analyse_optional(k_rest)
                   || analyse_optional(k_barline)
                   || analyse_optional(k_chord)
                   || analyse_optional(k_clef)
                   || analyse_optional(k_figuredBass)
                   || analyse_optional(k_graphic)
                   || analyse_optional(k_key)
                   || analyse_optional(k_metronome)
                   || analyse_optional(k_newSystem)
                   || analyse_optional(k_spacer)
                   || analyse_optional(k_text)
                   || analyse_optional(k_time)
                   || analyse_optional(k_goFwd)
                   || analyse_optional(k_goBack) ))
            {
                error_and_remove_invalid();
                move_to_next_param();
            }
        }

        error_if_more_elements();
    }

};

//-------------------------------------------------------------------------------------
// <note> = (n <pitch><duration> [<tie>][<beam>][<tuplet>][<voice>][<numStaff>]
//             [<fermata>][<stem>][<attachments>*])
// <rest> = (r <duration> [<beam>][<tuplet>][<voice>][<numStaff>]
//             [<fermata>][<attachments>*])
// <pitch> = label
// <duration> = label

class NoteRestAnalyser : public ElementAnalyser
{
public:
    NoteRestAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        if (m_pAnalysedNode->is_type(k_note))
            analyse_note();
        else
            analyse_rest();
    }

protected:

    void analyse_note()
    {
        ImNote* pNote = new ImNote();
        m_pAnalysedNode->set_imobj(pNote);

        // <pitch> (label)
        if (get_mandatory(k_label))
            set_pitch(pNote);

        // <duration> (label)
        if (get_mandatory(k_label))
            set_duration(pNote);

        //after duration we can find old compatibility bad designed items: l, g, p, v
        //in any order
        while(get_optional(k_label))
        {
            char type = (m_pParamToAnalyse->get_value())[0];
            if (type == 'l')
                set_tie_l(pNote);
            else if (type == 'g')
                set_beam_g(pNote);
            else if (type == 'v')
                get_voice();
            else if (type == 'p')
                get_num_staff();
            else
                error_and_remove_invalid();
        }

        error_if_more_elements();

        //set values that can be inherited
        m_pAnalyser->set_staff(pNote);
        m_pAnalyser->set_voice(pNote);
    }

    void analyse_rest()
    {
        ImRest* pRest = new ImRest();
        m_pAnalysedNode->set_imobj(pRest);

        // <duration> (label)
        if (get_mandatory(k_label))
            set_duration(pRest);

        //after duration we can find old compatibility bad designed items: g, p, v
        //in any order
        while(get_optional(k_label))
        {
            char type = (m_pParamToAnalyse->get_value())[0];
            if (type == 'g')
                set_beam_g(pRest);
            else if (type == 'v')
                get_voice();
            else if (type == 'p')
                get_num_staff();
            else
                error_and_remove_invalid();
        }

        error_if_more_elements();

        //set values that can be inherited
        m_pAnalyser->set_staff(pRest);
        m_pAnalyser->set_voice(pRest);
    }

    void set_pitch(ImNote* pNote)
    {
        string pitch = m_pParamToAnalyse->get_value();
        int step, octave, accidentals;
        if (ldp_pitch_to_components(pitch, &step, &octave, &accidentals))
        {
            report_msg(m_pParamToAnalyse->get_line_number(),
                "Unknown note pitch '" + pitch + "'. Replaced by 'c4'.");
            LdpElement* value = m_pLdpFactory->new_value(k_pitch, "c4");
            m_pAnalyser->replace_node(m_pParamToAnalyse, value);
            pNote->set_pitch(ImNote::C, 4, ImNote::NoAccidentals);
        }
        else
            pNote->set_pitch(step, octave, accidentals);
    }

    void set_duration(ImNoteRest* pNR)
    {
        string duration = m_pParamToAnalyse->get_value();
        int noteType, dots;
        if (ldp_duration_to_components(duration, &noteType, &dots))
        {
            report_msg(m_pParamToAnalyse->get_line_number(),
                "Unknown note/rest duration '" + duration + "'. Replaced by 'q'.");
            LdpElement* value = m_pLdpFactory->new_value(k_duration, "q");
            m_pAnalyser->replace_node(m_pParamToAnalyse, value);
            pNR->set_duration(ImNoteRest::Quarter, 0);
        }
        else
            pNR->set_duration(noteType, dots);
    }

    void set_tie_l(ImNote* pNote)
    {
    }

    void set_beam_g(ImNoteRest* pNR)
    {
    }

    void get_voice()
    {
        string voice = m_pParamToAnalyse->get_value().substr(1);
        int nVoice;
        std::istringstream iss(voice);
        if ((iss >> std::dec >> nVoice).fail())
        {
            report_msg(m_pParamToAnalyse->get_line_number(),
                "Invalid voice 'v" + voice + "'. Replaced by 'v1'.");
            LdpElement* value = m_pLdpFactory->new_value(k_label, "v1");
            m_pAnalyser->replace_node(m_pParamToAnalyse, value);
            m_pAnalyser->change_voice(1);
        }
        else
            m_pAnalyser->change_voice(nVoice);
    }

};

//-------------------------------------------------------------------------------------
// <opt> = (opt <name><value>)
// <name> = label
// <value> = { number | label | string }

class OptAnalyser : public ElementAnalyser
{
public:
    OptAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        string name;
        string value;

        // <name> (label)
        if (get_mandatory(k_label))
            name = m_pParamToAnalyse->get_value();

        // <value> (label | number | string)
        if (get_optional(k_label) || get_optional(k_number) || get_optional(k_string))
            value = m_pParamToAnalyse->get_value();

        error_if_more_elements();

        set_option(name, value);
    }

    void set_option(string& name, string& value)
    {
        ImOption* pOpt = new ImOption(name, value);
        m_pAnalysedNode->set_imobj(pOpt);

     //   enum {k_boolean = 0, k_number_long, k_number_double, k_string };

     //   //verify option name and determine required data type
     //   int nDataType;
     //   if (   (name == "StaffLines.StopAtFinalBarline")
     //       || (name == "StaffLines.Hide")
     //       || (name == "Staff.DrawLeftBarline")
     //       || (name == "Score.FillPageWithEmptyStaves")
     //       || (name == "Score.JustifyFinalBarline")
     //   )
     //       nDataType = k_boolean;

     //   else if (   (name == "Staff.UpperLegerLines.Displacement")
     //            || (name == "Render.SpacingMethod")
     //            || (name == "Render.SpacingValue")
     //           )
     //       nDataType = k_number_long;

     //   else if (name == "Render.SpacingFactor")
     //       nDataType = k_number_double;

     //   else
     //   {
     //       AnalysisError(pNode, "Option '%s' unknown. Ignored."), name.c_str());
     //       return;
     //   }

     //   //conver value to appropriate data type and set ImOption object
     //   long nNumberLong;
     //   double rNumberDouble;
	    //string sError;

     //   switch(nDataType)
     //   {
     //       case k_boolean:
     //           if (value == "true") || value == "yes")
     //           {
     //               pOpt->set(name, true);
     //               return;
     //           }
     //           else if (value == "false") || value == "no")
     //           {
     //               pOpt->set(name, false);
     //               return;
     //           }
     //           else
     //           {
     //               string sError = "a 'yes/no' or 'true/false' value");
     //               AnalysisError(pNode, "Error in data value for option '%s'.  It requires %s. Value '%s' ignored."),
     //                   name.c_str(), sError.c_str(), value.c_str());
     //           }
     //           return;

     //       case k_number_long:
     //           if (value.ToLong(&nNumberLong))
     //           {
     //               pOpt->set(name, nNumberLong);
     //               return;
     //           }
     //           else
     //           {
     //               sError = "an integer number");
     //               AnalysisError(pNode, "Error in data value for option '%s'.  It requires %s. Value '%s' ignored."),
     //                   name.c_str(), sError.c_str(), value.c_str());
     //           }
     //           return;

     //       case k_number_double:
			  //  if (!StrToDouble(value, &rNumberDouble))
			  //  {
     //               pOpt->set(name, rNumberDouble);
     //               return;
     //           }
     //           sError = "a real number");
     //           AnalysisError(pNode, "Error in data value for option '%s'.  It requires %s. Value '%s' ignored."),
     //               name.c_str(), sError.c_str(), value.c_str());
			  //  return;

     //       case k_string:
     //           if (name == "Render.SpacingMethod")
     //           {
     //               if (value == "fixed")
     //                   pOpt->set(name, (long)esm_Fixed);
     //               else if (value == "propConstantFixed")
     //                   pOpt->set(name, (long)esm_PropConstantFixed);
     //               else
     //                   AnalysisError(pNode, "Error in data value for option '%s'.  Value '%s' ignored."),
     //                       name.c_str(), value.c_str());
     //           }
     //           else
     //               pOpt->set(name, value);

                return;
    }
};

//-------------------------------------------------------------------------------------
// <score> = (score <vers><language>[<undoData>][<creationMode>][<defineStyle>*]
//                  [<title>*][<pageLayout>*][<systemLayout>*][<cursor>][<opt>*]
//                  {<instrument> | <group>}* )

class ScoreAnalyser : public ElementAnalyser
{
public:
    ScoreAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImScore* pScore = new ImScore();
        m_pAnalysedNode->set_imobj(pScore);

        // <vers>
        if (get_mandatory(k_vers))
            pScore->set_version( get_version() );

        // [<language>]
        analyse_optional(k_language);

        // [<undoData>]
        analyse_optional(k_undoData);

        // [<creationMode>]
        analyse_optional(k_creationMode);

        // [<defineStyle>*]
        while (analyse_optional(k_defineStyle));

        // [<title>*]
        while (analyse_optional(k_title));

        // [<pageLayout>*]
        while (analyse_optional(k_pageLayout));

        // [<systemLayout>*]
        while (analyse_optional(k_systemLayout));

        // [<cursor>]
        analyse_optional(k_cursor);

        // [<opt>*]
        while (analyse_optional(k_opt));

        // {<instrument> | <group>}*
        if (!more_params_to_analyse())
            error_missing_element(k_instrument);
        else
        {
            while (more_params_to_analyse())
            {
                if (! (analyse_optional(k_instrument)
                    || analyse_optional(k_group) ))
                {
                    error_and_remove_invalid();
                    move_to_next_param();
                }
            }
        }

        error_if_more_elements();
    }

protected:

    string get_version()
    {
        return m_pParamToAnalyse->get_parameter(1)->get_value();
    }

};

//-------------------------------------------------------------------------------------
// <metronomeMark> = (metronomeMark ?)

class ScoreAnchorAnalyser : public ElementAnalyser
{
public:
    ScoreAnchorAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        //ImMetronomeMark* pMM = new ImText();
        //m_pAnalysedNode->set_imobj(pMM);
    }
};

//-------------------------------------------------------------------------------------
// <spacer> = (spacer <width>[<numStaff>])     width in tenths

class SpacerAnalyser : public ElementAnalyser
{
public:
    SpacerAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImSpacer* pSpacer = new ImSpacer();
        m_pAnalysedNode->set_imobj(pSpacer);
    }
};
////void lmLDPParser::AnalyzeSpacer(lmLDPNode* pNode, lmVStaff* pVStaff)
////{
////    // <spacer> = (spacer <width>[<numStaff>])     width in tenths
////
////    wxString sElmName = GetNodeName(pNode);
////    long nId = GetNodeID(pNode);
////
////    //check that the width is specified
////    if(GetNodeNumParms(pNode) < 1)
////    {
////        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Ignored."),
////            sElmName.c_str());
////        return;
////    }
////
////    //get spacer width
////    int iP = 1;
////    wxString sNum1 = GetNodeName(pNode->GetParameter(iP));
////    if (!sNum1.IsNumber())
////    {
////        AnalysisError(
////            pNode,
////            _T("Element '%s': Width expected but found '%s'. Ignored."),
////            sElmName.c_str(), sNum1.c_str());
////        return;
////    }
////    long nWidth;
////    sNum1.ToLong(&nWidth);
////    ++iP;
////
////    //analyze optional parameters: num staff
////	lmLDPOptionalTags oOptTags(this);
////	oOptTags.SetValid(lm_eTag_StaffNum, -1);		//finish list with -1
////    int nStaff = 1;
////	oOptTags.AnalyzeCommonOptions(pNode, iP, pVStaff, NULL, &nStaff, NULL);
////
////    //create the spacer
////    lmSpacer* pSpacer = pVStaff->AddSpacer((lmTenths)nWidth, nId, nStaff);
////
////    //save cursor data
////    if (m_fCursorData && m_nCursorObjID == nId)
////        m_pCursorSO = (lmStaffObj*)pSpacer;
////
////    //analyze possible attachments
////    if (iP <= GetNodeNumParms(pNode))
////    {
////        lmLDPNode* pX = pNode->StartIterator(iP);
////        AnalyzeAttachments(pNode, pVStaff, pX, (lmStaffObj*)pSpacer);
////    }
////}

//-------------------------------------------------------------------------------------
// <systemLayout> = (systemLayout {first | other} <systemMargins>)

class SystemLayoutAnalyser : public ElementAnalyser
{
public:
    SystemLayoutAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImSystemLayout* pSL = new ImSystemLayout();
        m_pAnalysedNode->set_imobj(pSL);

        // {first | other} <label>
        if (get_mandatory(k_label))
        {
            string type = m_pParamToAnalyse->get_value();
            if (type == "first")
                pSL->set_first(true);
            else if (type == "other")
                pSL->set_first(false);
            else
            {
                report_msg(m_pParamToAnalyse->get_line_number(),
                        "Expected 'first' or 'other' value but found '" + type 
                        + "'. 'first' assumed.");
                LdpElement* value = m_pLdpFactory->new_value(k_label, "first");
                m_pAnalyser->replace_node(m_pParamToAnalyse, value);
                pSL->set_first(true);
            }
        }

        // <systemMargins>
        analyse_mandatory(k_systemMargins);

        error_if_more_elements();
    }

};

//-------------------------------------------------------------------------------------
// <systemMargins> = (systemMargins <leftMargin><rightMargin><systemDistance>
//                                  <topSystemDistance>)
// <leftMargin>, <rightMargin>, <systemDistance>, <topSystemDistance> = number (tenths)

class SystemMarginsAnalyser : public ElementAnalyser
{
public:
    SystemMarginsAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImSystemMargins* pSM = new ImSystemMargins();
        m_pAnalysedNode->set_imobj(pSM);

        if (get_mandatory(k_number))
            pSM->set_left_margin(get_float_number());

        if (get_mandatory(k_number))
            pSM->set_right_margin(get_float_number());

        if (get_mandatory(k_number))
            pSM->set_system_distance(get_float_number());

        if (get_mandatory(k_number))
            pSM->set_top_system_distance(get_float_number());

        error_if_more_elements();
    }

};

//-------------------------------------------------------------------------------------
// <text> = (text string <location>[<font><alingment>])

class TextAnalyser : public ElementAnalyser
{
public:
    TextAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImText* pText = new ImText();
        m_pAnalysedNode->set_imobj(pText);

        if (get_mandatory(k_string))
            pText->set_text(get_string());

    }
};
////bool lmLDPParser::AnalyzeText(lmLDPNode* pNode, lmVStaff* pVStaff, lmStaffObj* pTarget)
////{
////    //returns true if error; in this case nothing is added to the VStaff
////    // <text> = (text string <location>[<font><alingment>])
////
////    wxASSERT(GetNodeName(pNode) == _T("text"));
////    long nId = GetNodeID(pNode);
////
////    //check that at least two parameters (location and text string) are specified
////    if(GetNodeNumParms(pNode) < 2) {
////        AnalysisError(
////            pNode,
////            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////            _T("text") );
////        return true;
////    }
////
////    wxString sText;
////    wxString sStyle;
////    lmEHAlign nAlign = lmHALIGN_LEFT;     //TODO user options instead of fixed values
////    lmFontInfo tFont = {m_sTextFontName, m_nTextFontSize, m_nTextStyle, m_nTextWeight};
////    lmLocation tPos;
////    tPos.xUnits = lmTENTHS;
////    tPos.yUnits = lmTENTHS;
////    tPos.x = 0.0f;
////    tPos.y = 0.0f;
////
////    if (AnalyzeTextString(pNode, &sText, &sStyle, &nAlign, &tPos, &tFont))
////        return true;
////
////    //no error:
////    //save font values as new default for texts
////    m_sTextFontName = tFont.sFontName;
////    m_nTextFontSize = tFont.nFontSize;
////    m_nTextStyle = tFont.nFontStyle;
////    m_nTextWeight = tFont.nFontWeight;
////
////    //create the text
////    lmTextStyle* pStyle = (lmTextStyle*)NULL;
////    if (sStyle != _T(""))
////    {
////        pStyle = pVStaff->GetScore()->GetStyleInfo(sStyle);
////        if (!pStyle)
////            AnalysisError(pNode, _T("Style '%s' is not defined. Default style will be used."),
////                           sStyle.c_str());
////    }
////
////    if (!pStyle)
////        pStyle = pVStaff->GetScore()->GetStyleName(tFont);
////
////    if (!pTarget)
////        pTarget = pVStaff->AddAnchorObj();  //AWARE: generating a text element without anchor
////                                            //object is no longer possible since v1.6. Therefore,
////                                            //for undo/redo all text elements will have anchor
////                                            //with ID correctly saved/restored
////
////    lmTextItem* pText = pVStaff->AddText(sText, nAlign, pStyle, pTarget, nId);
////	pText->SetUserLocation(tPos);
////
////    return false;
////}


//-------------------------------------------------------------------------------------
// <timeSignature> = (time <beats><beatType>[<visible>][<location>])
// <beats> = <num>
// <beatType> = <num>

class TimeSignatureAnalyser : public ElementAnalyser
{
public:
    TimeSignatureAnalyser(Analyser* pAnalyser, ostream& reporter, LdpFactory* pFactory)
        : ElementAnalyser(pAnalyser, reporter, pFactory) {}

    void do_analysis()
    {
        ImTimeSignature* pTimeSignature = new ImTimeSignature();
        m_pAnalysedNode->set_imobj(pTimeSignature);

        // <beats> (num)
        if (get_mandatory(k_number))
            pTimeSignature->set_beats( get_integer_number(2) );

        // <beatType> (num)
        if (get_mandatory(k_number))
            pTimeSignature->set_beat_type( get_integer_number(4) );

        //TODO: [<visible>][<location>]

        error_if_more_elements();
    }

};


//class InstrNameAnalyser : public ElementAnalyser
//{
//    //<instrName> = (name string)
//};
//
//class InstrAbbrevAnalyser : public ElementAnalyser
//{
//    //<instrAbbrev> = (abbrev string)
//};
//
//class InfoMidiAnalyser : public ElementAnalyser
//{
//    //<infoMIDI> = (infoMIDI num-instr [num-channel])
//}
//


//--------------------------------------------------------------------------------
// ElementAnalyser implementation
//--------------------------------------------------------------------------------

void ElementAnalyser::analyse_node(LdpElement* pNode)
{
    m_pAnalysedNode = pNode;
    move_to_first_param();
    do_analysis();
}

bool ElementAnalyser::error_missing_element(ELdpElements type)
{
    const string& parentName =
        m_pLdpFactory->get_name( m_pAnalysedNode->get_type() );
    const string& name = m_pLdpFactory->get_name(type);
    report_msg(m_pAnalysedNode->get_line_number(),
               parentName + ": missing mandatory element '" + name + "'.");
    return false;
}

void ElementAnalyser::report_msg(int numLine, const std::stringstream& msg)
{
    report_msg(numLine, msg.str());
}

void ElementAnalyser::report_msg(int numLine, const std::string& msg)
{
    m_reporter << "Line " << numLine << ". " << msg << endl;
}

bool ElementAnalyser::get_mandatory(ELdpElements type)
{
    if (!more_params_to_analyse())
    {
        error_missing_element(type);
        return NULL;
    }

    m_pParamToAnalyse = get_param_to_analyse();
    if (m_pParamToAnalyse->get_type() != type)
    {
        error_missing_element(type);
        return false;
    }

    move_to_next_param();
    return true;
}

void ElementAnalyser::analyse_mandatory(ELdpElements type)
{
    if (get_mandatory(type))
        m_pAnalyser->analyse_node(m_pParamToAnalyse);
}

bool ElementAnalyser::get_optional(ELdpElements type)
{
    if (more_params_to_analyse())
    {
        m_pParamToAnalyse = get_param_to_analyse();
        if (m_pParamToAnalyse->get_type() == type)
        {
            move_to_next_param();
            return true;
        }
    }
    return false;
}

bool ElementAnalyser::analyse_optional(ELdpElements type)
{
    if (get_optional(type))
    {
        m_pAnalyser->analyse_node(m_pParamToAnalyse);
        return true;
    }
    return false;
}

void ElementAnalyser::analyse_one_or_more(ELdpElements* pValid, int nValid)
{
    while(more_params_to_analyse())
    {
        m_pParamToAnalyse = get_param_to_analyse();

        ELdpElements type = m_pParamToAnalyse->get_type();
        if (contains(type, pValid, nValid))
        {
            move_to_next_param();
            m_pAnalyser->analyse_node(m_pParamToAnalyse);
        }
        else
        {
            string name = m_pLdpFactory->get_name(type);
            report_msg(m_pParamToAnalyse->get_line_number(),
                "Element '" + name + "' is not possible here. Removed.");
            m_pAnalyser->erase_node(m_pParamToAnalyse);
        }
        move_to_next_param();
    }
}

bool ElementAnalyser::contains(ELdpElements type, ELdpElements* pValid, int nValid)
{
    for (int i=0; i < nValid; i++, pValid++)
        if (*pValid == type) return true;
    return false;
}

void ElementAnalyser::error_and_remove_invalid()
{
    ELdpElements type = m_pParamToAnalyse->get_type();
    string name = m_pLdpFactory->get_name(type);
    if (name == "label")
        name += ":" + m_pParamToAnalyse->get_value();
    report_msg(m_pParamToAnalyse->get_line_number(),
        "Element '" + name + "' is not possible here. Removed.");
    m_pAnalyser->erase_node(m_pParamToAnalyse);
}

void ElementAnalyser::error_if_more_elements()
{
    if (more_params_to_analyse())
    {
        report_msg(m_pAnalysedNode->get_line_number(),
                "Element '" + m_pAnalysedNode->get_name()
                + "': too many parameters. Extra parameters after '"
                + m_pParamToAnalyse->get_name()
                + "' have been removed.");
        while (more_params_to_analyse())
        {
            m_pParamToAnalyse = get_param_to_analyse();
            move_to_next_param();
            m_pAnalyser->erase_node(m_pParamToAnalyse);
        }
    }
}


//bool Analyser::has_number(LdpTree::iterator itNode, int* iP)
//{
//    if (has(itNode, iP, k_number))
//        return true;    //return predicate();
//    else
//        return false;
//}
//
//bool Analyser::has_label(LdpTree::iterator itNode, int* iP)
//{
//    if (has(itNode, iP, k_label))
//        return true;    //return predicate();
//    else
//        return false;
//}
//
//bool Analyser::has_string(LdpTree::iterator itNode, int* iP)
//{
//    if (has(itNode, iP, k_string))
//        return true;    //return predicate();
//    else
//        return false;
//}
//bool Analyser::replace_if_failure(bool success, LdpTree::iterator itNode,
//                                         const std::string& newContent)
//{
//    if (!success)
//        replace_content(itNode, newContent);
//
//    return success;
//}


//class LabelValueChecker
//{
//public:
//    LabelValueChecker() {};
//    bool operator()(LdpTree* tree, LdpElement* node, ostream& reporter)
//    {
//        LdpElement* child = node->get_parameter(1);
//        size_t found = m_validValues.find( child->get_ldp_value() );
//        if (found != string::npos)
//            return true;
//        else
//        {
//            reporter << "Line " << node->get_line_number()
//                     << ". Bad value '"
//                     << child->get_ldp_value()
//                     << "'. Replaced by '" << m_replacement << "'." << endl;
//            LdpTree::iterator it(child);
//            LdpElement* value = m_pLdpFactory->new_value(k_label, m_replacement);
//            tree->replace_node(it, value);
//            delete child;
//            return true;
//        }
//    }
//    void set_valid_values(const std::string& values) { m_validValues = values; }
//    void set_replacement(const std::string& replacement) { m_replacement = replacement; }
//
//protected:
//    std::string  m_validValues;
//    std::string  m_replacement;
//};
//


//--------------------------------------------------------------------------------
// Analyser implementation
//--------------------------------------------------------------------------------

Analyser::Analyser(ostream& reporter, LdpFactory* pFactory)
    : m_reporter(reporter)
    , m_pLdpFactory(pFactory)
    , m_pTree(NULL)
{
}

void Analyser::analyse_tree(LdpTree* tree)
{
    m_pTree = tree;
    m_curStaff = 0;
    m_curVoice = 1;
    analyse_node(tree->get_root());
}

void Analyser::analyse_node(LdpTree::iterator itNode)
{
    analyse_node(*itNode);
}

void Analyser::analyse_node(LdpElement* pNode)
{
    ElementAnalyser* a = new_analyser( pNode->get_type() );
    a->analyse_node(pNode);
    delete a;
}

void Analyser::erase_node(LdpElement* pNode)
{
    LdpTree::iterator it(pNode);
    m_pTree->erase(it);
    delete pNode;
}

//void Analyser::replace_node(LdpElement* pNode, const std::string& newContent)
//{
//    LdpParser parser(m_reporter);
//    LdpTree* treeChild = parser.parse_text(newContent);
//    LdpTree::iterator it(pNode);
//    m_pTree->replace_node(it, treeChild->get_root());
//    delete treeChild;
//    delete pNode;
//}

void Analyser::replace_node(LdpElement* pOldNode, LdpElement* pNewNode)
{
    LdpTree::iterator it(pOldNode);
    m_pTree->replace_node(it, pNewNode);
    delete pOldNode;
}

void Analyser::set_staff(ImStaffObj* pSO)
{
    pSO->set_staff(m_curStaff);
}

void Analyser::set_voice(ImNoteRest* pNR)
{
    pNR->set_voice(m_curVoice);
}

ElementAnalyser* Analyser::new_analyser(ELdpElements type)
{
    //Factory method to create analysers

    switch (type)
    {
//        case k_abbrev:
//        case k_above:
        case k_barline:         return new BarlineAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_below:
//        case k_bold:
//        case k_bold_italic:
//        case k_brace:
//        case k_bracket:
//        case k_center:
//        case k_chord:
        case k_clef:            return new ClefAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_color:
        case k_content:         return new ContentAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_creationMode:
//        case k_cursor:
//        case k_defineStyle:
//        case k_down:
//        case k_dx:
//        case k_dy:
//        case k_end:
//        case k_fermata:
        case k_figuredBass:     return new FiguredBassAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_font:
        case k_goBack:          return new GoBackFwdAnalyser(this, m_reporter, m_pLdpFactory);
        case k_goFwd:           return new GoBackFwdAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_graphic:
//        case k_group:
//        case k_hasWidth:
//        case k_infoMIDI:
//        case k_instrName:
        case k_instrument:      return new InstrumentAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_italic:
//        case k_joinBarlines:
        case k_key:             return new KeySignatureAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_landscape:
        case k_language:        return new LanguageAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_left:
        case k_lenmusdoc:       return new LenmusdocAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_line:
        case k_metronome:       return new MetronomeAnalyser(this, m_reporter, m_pLdpFactory);
        case k_musicData:       return new MusicDataAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_name:
        case k_newSystem:       return new ControlAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_no:
//        case k_normal:
        case k_note:            return new NoteRestAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_noVisible:
        case k_opt:             return new OptAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_pageLayout:
//        case k_pageMargins:
//        case k_pageSize:
//        case k_parentheses:
//        case k_portrait:
        case k_rest:            return new NoteRestAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_right:
        case k_score:           return new ScoreAnalyser(this, m_reporter, m_pLdpFactory);
        case k_spacer:          return new SpacerAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_split:
//        case k_staff:
//        case k_start:
//        case k_stem:
//        case k_style:
//        case k_symbol:
        case k_systemLayout:    return new SystemLayoutAnalyser(this, m_reporter, m_pLdpFactory);
        case k_systemMargins:   return new SystemMarginsAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_t:   //tuplet  <<<--------------- label
        case k_text:            return new TextAnalyser(this, m_reporter, m_pLdpFactory);
        case k_time:            return new TimeSignatureAnalyser(this, m_reporter, m_pLdpFactory);
//        case k_title:
//        case k_undoData:
//        case k_up:
//        case k_yes:
        default:
            return new NullAnalyser(this, m_reporter, m_pLdpFactory);
    }
}


}   //namespace lenmus


//////========================================================================================
//////helper class to keep info about a tie
//////========================================================================================
////class lmTieInfo
////{
////public:
////    lmTieInfo() {}
////    ~lmTieInfo() {}
////
////    bool        fStart;
////    long        nTieNum;
////    long        nTieID;
////    lmNote*     pNote;
////    lmTPoint    tBezier[4];
////};
////
//////========================================================================================
//////helper class to keep info about a FB line
//////========================================================================================
////class lmFBLineInfo
////{
////public:
////    lmFBLineInfo() {}
////    ~lmFBLineInfo() {}
////
////    bool            fStart;
////    long            nFBLineNum;
////    long            nFBLineID;
////    lmFiguredBass*  pFB;
////	lmLocation      tStartPos;
////	lmLocation      tEndPos;
////    lmTenths        ntWidth;
////    wxColour        nColor;
////};
////
////int lmLDPParser::AnalyzeGroup(lmLDPNode* pNode, lmScore* pScore, int nInstr)
////{
////    //Returns the number of instruments added to the score
////
////    //<Group> = (group [<GrpName>] <GrpSymbol> [<JoinBarlines>] Instrument+ )
////    //
////    //<GrpName> = (name name-string [(abbrev abbreviation-string)])
////    //<GrpSymbol> = (symbol {none | brace | bracket} )
////    //<JoinBarlines> = (joinBarlines {yes | no} )
////
////    lmLDPNode* pX;
////    wxString sData;
////    long iP;
////    iP = 1;
////
////    wxASSERT( GetNodeName(pNode) == _T("group") );
////
////    //default values for name
////    //TODO user options instead of fixed values
////    wxString sGrpName = _T("");           //no name for group
////    wxString sNameStyle = _T("");
////    lmEHAlign nNameAlign = lmHALIGN_LEFT;
////    lmFontInfo tNameFont = g_tInstrumentDefaultFont;
////    lmLocation tNamePos = g_tDefaultPos;
////    long nNameID = lmNEW_ID;
////
////    //default values for abbreviation
////    //TODO user options instead of fixed values
////    wxString sGrpAbbrev = _T("");         //no abreviated name for group
////    wxString sAbbrevStyle = _T("");
////    lmEHAlign nAbbrevAlign = lmHALIGN_LEFT;
////    lmFontInfo tAbbrevFont = g_tInstrumentDefaultFont;
////    lmLocation tAbbrevPos = g_tDefaultPos;
////    long nAbbrevID = lmNEW_ID;
////
////    //default values for other parameters
////    bool fJoinBarlines = true;
////    lmEBracketSymbol nGrpSymbol = lm_eBrace;
////
////    //parse elements until <Instrument> tag found
////    bool fInstrFound = false;
////    for (; iP <= GetNodeNumParms(pNode); iP++) {
////        pX = pNode->GetParameter(iP);
////
////        if (GetNodeName(pX) == _T("instrument") )
////        {
////            fInstrFound = true;
////            break;      //start of Instrument. Exit this loop
////        }
////        else if (GetNodeName(pX) == _T("name") )
////        {
////            nNameID = GetNodeID(pX);
////            AnalyzeTextString(pX, &sGrpName, &sNameStyle, &nNameAlign, &tNamePos,
////                                &tNameFont);
////        }
////        else if (GetNodeName(pX) == _T("abbrev") )
////        {
////            nAbbrevID = GetNodeID(pX);
////            AnalyzeTextString(pX, &sGrpAbbrev, &sAbbrevStyle, &nAbbrevAlign,
////                                &tAbbrevPos, &tAbbrevFont);
////        }
////        else if (GetNodeName(pX) == _T("symbol") )
////        {
////            wxString sSymbol = GetNodeName(pX->GetParameter(1));
////            if (sSymbol == _T("brace") )
////                nGrpSymbol = lm_eBrace;
////            else if (sSymbol == _T("bracket") )
////                nGrpSymbol = lm_eBracket;
////            else
////            {
////                AnalysisError(pX, _T("Invalid group symbol '%s'. Brace assumed."), sSymbol.c_str());
////            }
////        }
////        else if (GetNodeName(pX) == _T("joinBarlines") )
////        {
////            fJoinBarlines = GetValueYesNo(pX, fJoinBarlines);
////        }
////        else
////        {
////            AnalysisError(pX, _T("[%s]: unknown element '%s' found. Element ignored."),
////                _T("group"), GetNodeName(pX).c_str() );
////        }
////    }
////
////    //create the group relationship
////    lmInstrGroup* pGroup = new lmInstrGroup(nGrpSymbol, fJoinBarlines);
//////#if lmUSE_LIBRARY
//////    pGroup->SetLdpElement(pNode);
//////#endif
////
////    // loop to parse elements <instrument>
////    while(iP <= GetNodeNumParms(pNode))
////    {
////        pX = pNode->GetParameter(iP);
////        if ( GetNodeName(pX) == _T("instrument") )
////        {
////            AnalyzeInstrument105(pX, pScore, nInstr++, pGroup);
////        }
////        else
////        {
////            AnalysisError(pNode, _T("Elements <instrument> expected but found element %s. Analysis stopped."),
////                GetNodeName(pNode).c_str() );
////            break;
////        }
////        iP++;
////        if (iP <= GetNodeNumParms(pNode))
////            pX = pNode->GetParameter(iP);
////    }
////
////    return nInstr;
////}
////
////void lmLDPParser::AnalyzeInstrument105(lmLDPNode* pNode, lmScore* pScore, int nInstr,
////                                       lmInstrGroup* pGroup)
////{
////    //<instrument> = (instrument [<InstrName>][<InfoMIDI>][<Staves>] <Voice>+ )
////
////    //<InstrName> = (instrName name-string [abbreviation-string])
////    //<InfoMIDI> = (infoMIDI num-instr [num-channel])
////    //<Staves> = (staves {num | overlayered} )
////    //<Voice> = (MusicData <music>+ )
////
////    wxASSERT( GetNodeName(pNode) == _T("instrument") );
////    long nId = GetNodeID(pNode);
////
////    //default values
////	int nMIDIChannel = g_pMidi->DefaultVoiceChannel();
////	int nMIDIInstr = g_pMidi->DefaultVoiceInstr();
////    bool fMusicFound = false;               // <MusicData> tag found
////    long nVStaffID = (nId == lmNEW_ID ? lmNEW_ID : nId+1);
////    long nStaffID = (nVStaffID == lmNEW_ID ? lmNEW_ID : nVStaffID+1);
////
////    //staves
////    wxString sNumStaves = _T("1");          //one staff
////    long nNumStaves = 1L;
////    long nAddedStaves = 0L;
////
////    //default values for name
////    wxString sInstrName = _T("");           //no name for instrument
////    wxString sInstrNameStyle = _T("");
////    lmEHAlign nNameAlign = lmHALIGN_LEFT;
////    lmFontInfo tNameFont = g_tInstrumentDefaultFont;
////    lmLocation tNamePos = g_tDefaultPos;
////    long nNameID = lmNEW_ID;
////    lmInstrNameAbbrev* pName = (lmInstrNameAbbrev*)NULL;
////
////    //default values for abbreviation
////    wxString sInstrAbbrev = _T("");         //no abreviated name for instrument
////    wxString sInstrAbbrevStyle = _T("");
////    lmEHAlign nAbbrevAlign = lmHALIGN_LEFT;
////    lmFontInfo tAbbrevFont = g_tInstrumentDefaultFont;
////    lmLocation tAbbrevPos = g_tDefaultPos;
////    long nAbbrevID = lmNEW_ID;
////    lmInstrNameAbbrev* pAbbrev = (lmInstrNameAbbrev*)NULL;
////
////    //create an instrument initialized with default values. Only one staff
////    lmInstrument* pInstr = pScore->AddInstrument(nMIDIChannel, nMIDIInstr, pName, pAbbrev,
////                                                 nId, nVStaffID, nStaffID, pGroup);
////    lmVStaff* pVStaff = pInstr->GetVStaff();
////    nAddedStaves++;
////#if lmUSE_LIBRARY
////    pInstr->SetLdpElement(pNode);
////#endif
////
////
////    // parse elements until <musicData> tag found
////    lmLDPNode* pX;
////    for (int iP=1; iP <= GetNodeNumParms(pNode); iP++)
////    {
////        pX = pNode->GetParameter(iP);
////
////        if (GetNodeName(pX) == _T("musicData") )
////        {
////            fMusicFound = true;
////            if (nVStaffID != pX->get_id())
////            {
////                nVStaffID = GetNodeID(pX);
////                AnalysisError(pX, _T("Program error: incoherent ID for VStaff. nId=%d, nVStaffID=%d."),
////                              nId, nVStaffID );
////            }
////            break;      //start of MusicData. Exit this loop
////        }
////        else if (GetNodeName(pX) == _T("name") )
////        {
////            nNameID = GetNodeID(pX);
////            AnalyzeTextString(pX, &sInstrName, &sInstrNameStyle, &nNameAlign,
////                                &tNamePos, &tNameFont);
////
////            if (sInstrName != _T(""))
////            {
////                lmTextStyle* pTS;
////                if (sInstrNameStyle != _T(""))
////                    pTS = pScore->GetStyleInfo(sInstrNameStyle);
////                else
////                    pTS = pScore->GetStyleName(tNameFont);
////                wxASSERT(pTS);
////                pName = pInstr->AddName(sInstrName, nNameID, pTS);
////                //convert position to LUnits. As the text is not yet owned we must use the score
////	            if (tNamePos.xUnits == lmTENTHS)
////                {
////		            tNamePos.x = pScore->TenthsToLogical(tNamePos.x);
////                    tNamePos.xUnits = lmLUNITS;
////                }
////	            if (tNamePos.yUnits == lmTENTHS)
////                {
////		            tNamePos.y = pScore->TenthsToLogical(tNamePos.y);
////                    tNamePos.yUnits = lmLUNITS;
////                }
////                pName->SetUserLocation(tNamePos);
////            }
////        }
////        else if (GetNodeName(pX) == _T("abbrev") )
////		{
////            nAbbrevID = GetNodeID(pX);
////            AnalyzeTextString(pX, &sInstrAbbrev, &sInstrAbbrevStyle, &nAbbrevAlign,
////                                &tAbbrevPos, &tAbbrevFont);
////
////            if (sInstrAbbrev != _T(""))
////            {
////                lmTextStyle* pTS;
////                if (sInstrAbbrevStyle != _T(""))
////                    pTS = pScore->GetStyleInfo(sInstrAbbrevStyle);
////                else
////                    pTS = pScore->GetStyleName(tAbbrevFont);
////                wxASSERT(pTS);
////                pAbbrev = pInstr->AddAbbreviation(sInstrAbbrev, nAbbrevID, pTS);
////                //convert position to LUnits. As the text is not yet owned we must use the score
////	            if (tAbbrevPos.xUnits == lmTENTHS)
////                {
////		            tAbbrevPos.x = pScore->TenthsToLogical(tAbbrevPos.x);
////                    tAbbrevPos.xUnits = lmLUNITS;
////                }
////	            if (tAbbrevPos.yUnits == lmTENTHS)
////                {
////		            tAbbrevPos.y = pScore->TenthsToLogical(tAbbrevPos.y);
////                    tAbbrevPos.yUnits = lmLUNITS;
////                }
////                pAbbrev->SetUserLocation(tAbbrevPos);
////            }
////        }
////        else if (GetNodeName(pX) == _T("infoMIDI") )
////		{
////			AnalyzeInfoMIDI(pX, &nMIDIChannel, &nMIDIInstr);
////            pInstr->SetMIDIChannel(nMIDIChannel);
////            pInstr->SetMIDIInstrument(nMIDIInstr);
////		}
////        else if (GetNodeName(pX) == _T("staves") )
////		{
////            pX = pX->GetParameter(1);
////            if (pX->IsSimple()) {
////                sNumStaves = GetNodeName(pX);
////                if (!sNumStaves.IsNumber()) {
////                    AnalysisError(pX, _T("Number of staves expected but found '%s'. Element '%s' ignored."),
////                        sNumStaves.c_str(), _T("staves") );
////                    sNumStaves = _T("1");
////                }
////            }
////            else {
////                AnalysisError(pX, _T("Expected value for %s but found element '%s'. Ignored."),
////                    _T("staves"), GetNodeName(pX).c_str() );
////            }
////
////            //check that maximum number of supported staves is not reached
////            sNumStaves.ToLong(&nNumStaves);
////            if (nNumStaves > lmMAX_STAFF)
////            {
////                AnalysisError(pX, _T("Program limit reached: the number of staves per instrument must not be greater than %d. Please inform LenMus developers."),
////                              lmMAX_STAFF);
////                nNumStaves = lmMAX_STAFF;
////            }
////        }
////        else if (GetNodeName(pX) == _T("staff") )
////		{
////			AnalyzeStaff(pX, pVStaff);
////            nAddedStaves++;
////		}
////        else
////        {
////            AnalysisError(pX, _T("[%s]: unknown element '%s' found. Element ignored."),
////                _T("instrument"), GetNodeName(pX).c_str() );
////        }
////    }
////
////
////    //the default instrument only contains one staff. So if more than one staff
////    //requested and no <staff> elements, we have to add nNumStaves - 1
////    if (nAddedStaves < nNumStaves)
////    {
////        if (nAddedStaves != 1)
////            AnalysisError(pNode, _T("[instrument]: less <staff> elements (%d) than staves (%d). Default sataves added."),
////                          nAddedStaves, nNumStaves );
////        while (nAddedStaves < nNumStaves)
////        {
////            pVStaff->AddStaff(5);    //five lines staff, standard size
////            nAddedStaves++;
////        }
////    }
////
////    // analyze musicData
////    if (fMusicFound)
////        AnalyzeMusicData(pX, pVStaff);
////}
////
////bool lmLDPParser::AnalyzeInfoMIDI(lmLDPNode* pNode, int* pChannel, int* pNumInstr)
////{
////	//analizes a <infoMIDI> element and updates variables pChannel and pNumInstr
////	//returns true if error.
////	//
////    //		<InfoMIDI> = (infoMIDI num-instr num-channel)
////
////    wxString sElmName = GetNodeName(pNode);
////    wxASSERT(sElmName == _T("infoMIDI") );
////
////    //check that two numbers are specified
////    if(GetNodeNumParms(pNode) < 2) {
////        AnalysisError(pNode, _T("Element 'infoMIDI' has less parameters than the minimum required. Ignored."));
////        return true;
////    }
////
////    wxString sNum1 = GetNodeName(pNode->GetParameter(1));
////    wxString sNum2 = GetNodeName(pNode->GetParameter(2));
////    if (!sNum1.IsNumber() || !sNum2.IsNumber()) {
////        AnalysisError(
////            pNode,
////            _T("Element 'infoMIDI': Two numbers expected but found '%s' and '%s'. Ignored."),
////            sNum1.c_str(), sNum2.c_str() );
////        return true;
////    }
////
////    long nAux;
////    sNum1.ToLong(&nAux);
////	*pNumInstr = int(nAux);
////    sNum2.ToLong(&nAux);
////	*pChannel = int(nAux);
////
////    return false;
////}
////
////void lmLDPParser::AnalyzeUndoData(lmLDPNode* pNode)
////{
////    // <undoData> = (undoData (idCounter  num))
////
////    //TODO
////
////    wxASSERT(GetNodeName(pNode) == _T("undoData"));
////
////    m_pScore->SetUndoMode();
////}
////
////void lmLDPParser::AnalyzeChord(lmLDPNode* pNode, lmVStaff* pVStaff)
////{
////    // <chord> = (chord <Note>* )
////
////    wxASSERT(GetNodeName(pNode) == _T("chord"));
////
////    //loop to analyze remaining elements: notes
////    long iP;
////    wxString sName;
////    lmLDPNode* pX;
////
////    for(iP=1; iP <= GetNodeNumParms(pNode); iP++) {
////        pX = pNode->GetParameter(iP);
////        sName = GetNodeName(pX);
////        if (sName == _T("n")) {
////            AnalyzeNote(pX, pVStaff, (iP != 1));     //first note is base of chord
////        }
////        else {
////            AnalysisError(pX, _T("[AnalyzeChord]: Expecting notes found element '%s'. Element ignored."),
////                sName.c_str() );
////        }
////    }
////
////}
////
////lmTieInfo* lmLDPParser::AnalyzeTie(lmLDPNode* pNode, lmVStaff* pVStaff)
////{
////    // <tie> = (tie num [start | stop] [<bezier>])
////
////    //returns a ptr. to a new lmTieInfo struct or NULL if any important error.
////
////    wxASSERT(GetNodeName(pNode) == _T("tie"));
////
////    //check that there are parameters
////    if (GetNodeNumParms(pNode) < 2 || GetNodeNumParms(pNode) > 3)
////    {
////        AnalysisError(pNode, _T("Element '%s' has less or more parameters than required. Tag ignored."),
////            GetNodeName(pNode).c_str() );
////        return (lmTieInfo*)NULL;    //error
////    }
////
////    //create the TieInfo struct to save Tie data
////    lmTieInfo* pTieInfo = new lmTieInfo;
////
////    //initialize points
////    for (int i=0; i < 4; i++)
////        pTieInfo->tBezier[i] = lmTPoint(0.0f, 0.0f);
////
////    //get tie ID
////    pTieInfo->nTieID = GetNodeID(pNode);
////
////    //get tie number
////    wxString sNum = GetNodeName(pNode->GetParameter(1));
////    if (!sNum.IsNumber())
////    {
////        AnalysisError(pNode,
////            _T("Element 'tie': Number expected but found '%s'. Tie ignored."), sNum.c_str() );
////        delete pTieInfo;
////        return (lmTieInfo*)NULL;    //error;
////    }
////    sNum.ToLong(&(pTieInfo->nTieNum));
////
////    //get tie type: start / end
////    wxString sType = GetNodeName(pNode->GetParameter(2));
////    if (!(sType == _T("start") || sType == _T("stop")) )
////    {
////        AnalysisError(pNode,
////            _T("Element 'tie': Type must be 'start' or 'stop' but found '%s'. Tie ignored."), sType.c_str() );
////        delete pTieInfo;
////        return (lmTieInfo*)NULL;    //error;
////    }
////    pTieInfo->fStart = (sType == _T("start"));
////
////    //get points values
////    if (GetNodeNumParms(pNode) == 3)
////    {
////        if (GetNodeName(pNode->GetParameter(3)) != _T("bezier"))
////            AnalysisError(pNode,
////                _T("Element 'tie': Expected 'bezier' element but found '%s'. Parameter ignored."),
////                GetNodeName(pNode->GetParameter(3)).c_str() );
////        else
////            AnalyzeBezier(pNode->GetParameter(3), &(pTieInfo->tBezier[0]));
////    }
////
////    //end of analysis
////    return pTieInfo;
////}
////
////lmFBLineInfo* lmLDPParser::AnalyzeFBLine(lmLDPNode* pNode, lmVStaff* pVStaff)
////{
////    // <fbline> = (fbline num [start | stop]
////    //              [<startPoint>][<endPoint>][<width>][<color>]
////
////    //returns a ptr. to a new lmFBLineInfo struct or NULL if any important error.
////
////    wxASSERT(GetNodeName(pNode) == _T("fbline"));
////
////    //check that there are parameters
////    if (GetNodeNumParms(pNode) < 2 || GetNodeNumParms(pNode) > 3)
////    {
////        AnalysisError(pNode, _T("Element '%s' has less or more parameters than required. Tag ignored."),
////            GetNodeName(pNode).c_str() );
////        return (lmFBLineInfo*)NULL;    //error
////    }
////
////    //create the lmFBLineInfo struct to save FB line data
////    lmFBLineInfo* pFBLineInfo = new lmFBLineInfo;
////
////    //initialize points and default values
////	pFBLineInfo->tStartPos = g_tDefaultPos;
////	pFBLineInfo->tEndPos = g_tDefaultPos;
////    pFBLineInfo->ntWidth = 1.0f;
////    pFBLineInfo->nColor = *wxBLACK;
////
////    //get FB line ID
////    pFBLineInfo->nFBLineID = GetNodeID(pNode);
////
////    //get FB line number
////    int iP = 1;
////    wxString sNum = GetNodeName(pNode->GetParameter(iP));
////    if (!sNum.IsNumber())
////    {
////        AnalysisError(pNode,
////            _T("Element 'FB line': Number expected but found '%s'. FB line ignored."), sNum.c_str() );
////        delete pFBLineInfo;
////        return (lmFBLineInfo*)NULL;    //error;
////    }
////    sNum.ToLong(&(pFBLineInfo->nFBLineNum));
////
////    //get FB line type: start / end
////    iP++;
////    wxString sType = GetNodeName(pNode->GetParameter(iP));
////    if (!(sType == _T("start") || sType == _T("stop")) )
////    {
////        AnalysisError(pNode,
////            _T("Element 'FB line': Type must be 'start' or 'stop' but found '%s'. FB line ignored."), sType.c_str() );
////        delete pFBLineInfo;
////        return (lmFBLineInfo*)NULL;    //error;
////    }
////    pFBLineInfo->fStart = (sType == _T("start"));
////
////    //loop to analyze line parameters
////    iP++;
////    for(; iP <= GetNodeNumParms(pNode); iP++)
////    {
////        lmLDPNode* pX = pNode->GetParameter(iP);
////        wxString sName = GetNodeName(pX);
////        if (sName == _T("startPoint"))
////            AnalyzeLocationPoint(pX, &(pFBLineInfo->tStartPos));
////        else if (sName == _T("endPoint"))
////            AnalyzeLocationPoint(pX, &(pFBLineInfo->tEndPos));
////        else if(sName == _T("width"))
////            GetValueFloatNumber(pX, &(pFBLineInfo->ntWidth));
////        else if(sName == _T("color"))
////            pFBLineInfo->nColor = AnalyzeColor(pX);
////        else
////        {
////            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
////                          _T("fbline"), sName.c_str() );
////        }
////    }
////
////    //end of analysis
////    return pFBLineInfo;
////}
////
////lmBeamInfo* lmLDPParser::AnalyzeBeam(lmLDPNode* pNode, lmVStaff* pVStaff)
////{
////    // <beam> = (beam num <beamtype>+)
////    // <beamtype> = { start | continue | end | forward | backward }
////
////    //returns a ptr. to a new lmBeamInfo struct or NULL if any important error.
////
////    wxASSERT(GetNodeName(pNode) == _T("beam"));
////
////    //check that there are parameters
////    if (GetNodeNumParms(pNode) < 2)
////    {
////        AnalysisError(pNode, _T("Element '%s' has less or more parameters than required. Tag ignored."),
////            GetNodeName(pNode).c_str() );
////        return (lmBeamInfo*)NULL;    //error
////    }
////
////    //create the lmBeamInfo object to save beam data
////    lmBeamInfo* pBeamInfo = new lmBeamInfo;
////
////    //get beam ID
////    pBeamInfo->nBeamID = GetNodeID(pNode);
////
////    //get beam number
////    wxString sNum = GetNodeName(pNode->GetParameter(1));
////    if (!sNum.IsNumber())
////    {
////        AnalysisError(pNode,
////            _T("Element 'beam': Number expected but found '%s'. Beam ignored."), sNum.c_str() );
////        delete pBeamInfo;
////        return (lmBeamInfo*)NULL;    //error;
////    }
////    sNum.ToLong(&(pBeamInfo->nBeamNum));
////
////    //get beam type: start / continue / end / forward / backward
////    int iP = 2;
////    int iB = 0;
////    bool fEndOfBeam = true;     //assute it all beam types are 'end'
////    for(; iP <= GetNodeNumParms(pNode); iP++, iB++)
////    {
////        lmLDPNode* pX = pNode->GetParameter(iP);
////        wxString sType = GetNodeName(pX);
////
////        if (sType == _T("begin"))
////            pBeamInfo->nBeamType[iB] = eBeamBegin;
////        else if(sType == _T("continue"))
////            pBeamInfo->nBeamType[iB] = eBeamContinue;
////        else if (sType == _T("end"))
////            pBeamInfo->nBeamType[iB] = eBeamEnd;
////        else if (sType == _T("forward"))
////            pBeamInfo->nBeamType[iB] = eBeamForward;
////        else if (sType == _T("backward"))
////            pBeamInfo->nBeamType[iB] = eBeamBackward;
////        else
////        {
////            AnalysisError(pNode,
////                _T("Element 'beam': Invalid beam type '%s' found. Beam ignored."), sType.c_str() );
////            delete pBeamInfo;
////            return (lmBeamInfo*)NULL;    //error;
////        }
////        fEndOfBeam &= (pBeamInfo->nBeamType[iB] == eBeamEnd
////                       || pBeamInfo->nBeamType[iB] == eBeamBackward);
////    }
////    pBeamInfo->fEndOfBeam = fEndOfBeam;
////
////    //end of analysis
////    return pBeamInfo;
////}
////
////bool lmLDPParser::AnalyzeBezierLocation(lmLDPNode* pNode, lmTPoint* pPoints)
////{
////    // <bezier-location> = { (start-x num) | (start-y num) | (end-x num) | (end-y num) |
////    //                       (ctrol1-x num) | (ctrol1-y num) | (ctrol2-x num) | (ctrol2-y num) }
////    // <num> = real number, in tenths
////
////    //Returns true if error.  As result of the analysis, the corresponding value of array of
////    //points pPoints is updated.
////
////    //check that there is one parameter and only one
////    if (GetNodeNumParms(pNode)!= 1)
////    {
////        AnalysisError(pNode, _T("Element '%s' has less or more parameters than required. Tag ignored."),
////            GetNodeName(pNode).c_str() );
////        return true;    //error
////    }
////
////    //get point name
////    lmTenths* pTarget;
////    wxString sName = GetNodeName(pNode);
////    if (sName == _T("start-x"))
////        pTarget = &((*(pPoints+lmBEZIER_START)).x);
////    else if (sName == _T("end-x"))
////        pTarget = &((*(pPoints+lmBEZIER_END)).x);
////    else if (sName == _T("ctrol1-x"))
////        pTarget = &((*(pPoints+lmBEZIER_CTROL1)).x);
////    else if (sName == _T("ctrol2-x"))
////        pTarget = &((*(pPoints+lmBEZIER_CTROL2)).x);
////    else if (sName == _T("start-y"))
////        pTarget = &((*(pPoints+lmBEZIER_START)).y);
////    else if (sName == _T("end-y"))
////        pTarget = &((*(pPoints+lmBEZIER_END)).y);
////    else if (sName == _T("ctrol1-y"))
////        pTarget = &((*(pPoints+lmBEZIER_CTROL1)).y);
////    else if (sName == _T("ctrol2-y"))
////        pTarget = &((*(pPoints+lmBEZIER_CTROL2)).y);
////    else
////    {
////        AnalysisError(pNode, _T("Element '%s' unknown. Ignored."), sName.c_str() );
////        return true;    //error
////    }
////
////    //get point value
////    wxString sValue = GetNodeName(pNode->GetParameter(1));
////	double rNumberDouble;
////	if (!StrToDouble(sValue, &rNumberDouble))
////	{
////        *pTarget = (float)rNumberDouble;
////    }
////    else
////    {
////        AnalysisError(pNode, _T("Element '%s': Invalid value '%s'. It must be a number with optional units. Zero assumed."),
////            sName.c_str(), sValue.c_str() );
////        return true;    //error
////    }
////    return false;       //no error
////}
////
////void lmLDPParser::AnalyzeBezier(lmLDPNode* pNode, lmTPoint* pPoints)
////{
////    // <bezier> = (bezier [bezier-location]* )
////
////    //returns, in variable pPoints[4] the result of the analysis. If a point is not specified
////    //value (0, 0) is assigned.
////
////    //check that there are parameters
////    if (GetNodeNumParms(pNode) < 1 || GetNodeNumParms(pNode) > 8)
////    {
////        AnalysisError(pNode, _T("Element '%s' has less or more parameters than required. Tag ignored."),
////            GetNodeName(pNode).c_str() );
////        return;
////    }
////
////    //get points values
////    for (int iP = 1; iP <= GetNodeNumParms(pNode); iP++)
////    {
////        AnalyzeBezierLocation(pNode->GetParameter(iP), pPoints);
////    }
////}
////
////void lmLDPParser::AddTie(lmNote* pNote, lmTieInfo* pTieInfo)
////{
////    //Receives a note with a tie of type 'stop' and the lmTieInfo for the stop tie element.
////    //This method must look for the matching start element and, if found, build the tie
////
////    //look for the matching start element
////    std::list<lmTieInfo*>::iterator itT;
////    for(itT=m_PendingTies.begin(); itT != m_PendingTies.end(); ++itT)
////    {
////         if ((*itT)->nTieNum == pTieInfo->nTieNum)
////             break;     //found
////    }
////    if (itT == m_PendingTies.end())
////    {
////        //Ignore errors if the parsed score is for undo.
////       if (m_pIgnoreSet == NULL)
////            AnalysisError((lmLDPNode*)NULL, _T("No 'start' element for tie num. %d. Tie ignored."),
////                        pTieInfo->nTieNum );
////
////        delete pTieInfo;
////        return;
////    }
////
////    //element found. verify that it is of type 'start'
////    if (!(*itT)->fStart)
////    {
////        AnalysisError((lmLDPNode*)NULL, _T("Duplicated 'stop' element for tie num. %d. Tie ignored."),
////                      pTieInfo->nTieNum );
////        delete pTieInfo;
////        delete *itT;
////        m_PendingTies.erase(itT);
////        return;
////    }
////
////    //verify that both notes have the same pitch
////    if((*itT)->pNote->GetFPitch() != pNote->GetFPitch())
////    {
////        AnalysisError((lmLDPNode*)NULL, _T("Requesting to tie notes of different pitch. Tie %d ignored."),
////                      pTieInfo->nTieNum );
////        delete pTieInfo;
////        delete *itT;
////        m_PendingTies.erase(itT);
////        return;
////    }
////
////    //create the tie
////    (*itT)->pNote->CreateTie(pNote, pTieInfo->nTieID, (*itT)->tBezier,
////                                pTieInfo->tBezier);
////
////    //remove and delete consumed lmTieInfo elements
////    delete pTieInfo;
////    delete *itT;
////    m_PendingTies.erase(itT);
////}
////
////void lmLDPParser::AddFBLine(lmFiguredBass* pFB, lmFBLineInfo* pFBLineInfo)
////{
////    //Receives a FB with a FB line of type 'stop' and the lmFBLineInfo for the stop
////    //FB line element. This method must look for the matching start element and,
////    //if found, build the FB line
////
////    //look for the matching start element
////    std::list<lmFBLineInfo*>::iterator itFBL;
////    for(itFBL=m_PendingFBLines.begin(); itFBL != m_PendingFBLines.end(); ++itFBL)
////    {
////         if ((*itFBL)->nFBLineNum == pFBLineInfo->nFBLineNum)
////             break;     //found
////    }
////    if (itFBL == m_PendingFBLines.end())
////    {
////        //Ignore errors if the parsed score is for undo.
////       if (m_pIgnoreSet == NULL)
////            AnalysisError((lmLDPNode*)NULL, _T("No 'start' element for FB line num. %d. FBLine ignored."),
////                          pFBLineInfo->nFBLineNum );
////
////        delete pFBLineInfo;
////        return;
////    }
////
////    //element found. verify that it is of type 'start'
////    if (!(*itFBL)->fStart)
////    {
////        AnalysisError((lmLDPNode*)NULL, _T("Duplicated 'stop' element for FB line num. %d. FBLine ignored."),
////                      pFBLineInfo->nFBLineNum );
////        delete pFBLineInfo;
////        delete *itFBL;
////        m_PendingFBLines.erase(itFBL);
////        return;
////    }
////
////    //create the FB line
////    (*itFBL)->pFB->CreateFBLine(pFB, pFBLineInfo->nFBLineID, (*itFBL)->tStartPos,
////                                pFBLineInfo->tEndPos, pFBLineInfo->ntWidth,
////                                pFBLineInfo->nColor);
////
////    //remove and delete consumed lmFBLineInfo elements
////    delete pFBLineInfo;
////    delete *itFBL;
////    m_PendingFBLines.erase(itFBL);
////}
////
////void lmLDPParser::AddBeam(lmNoteRest* pNR, lmBeamInfo* pBeamInfo)
////{
////    //Received the last note of a beam and its lmBeamInfo data
////    //This method must look for the matching lmBeamInfo elements and build the beam
////
////    //look for the matching start/continue elements
////    std::vector<lmBeamInfo*> cBeamInfo;
////    std::list<lmBeamInfo*>::iterator itB;
////    bool fInPendingBeams = false;       //pBeamInfo is in m_PendingBeams
////    for(itB=m_PendingBeams.begin(); itB != m_PendingBeams.end(); ++itB)
////    {
////         if ((*itB)->nBeamNum == pBeamInfo->nBeamNum)
////         {
////             if (pBeamInfo != *itB)
////                cBeamInfo.push_back(*itB);
////             else
////                fInPendingBeams |= true;
////         }
////    }
////    if (cBeamInfo.empty())
////    {
////        //Ignore errors if the parsed score is for undo.
////        if (m_pIgnoreSet == NULL)
////            AnalysisError((lmLDPNode*)NULL, _T("No 'start' element for beam num. %d. Beam ignored."),
////                        pBeamInfo->nBeamNum );
////
////        delete pBeamInfo;
////        return;
////    }
////
////    //create the beam
////    cBeamInfo.push_back(pBeamInfo);
////    lmVStaff* pVStaff = pNR->GetVStaff();
////    pVStaff->CreateBeam(cBeamInfo);
////
////    //remove and delete consumed lmBeamInfo elements
////    for(itB=m_PendingBeams.begin(); itB != m_PendingBeams.end(); )
////    {
////        if ((*itB)->nBeamNum == pBeamInfo->nBeamNum)
////        {
////            delete *itB;
////            itB = m_PendingBeams.erase(itB);
////        }
////        else
////            ++itB;
////    }
////    if (!fInPendingBeams)
////        delete pBeamInfo;
////}
////
////lmNoteRest* lmLDPParser::AnalyzeNoteRest(lmLDPNode* pNode, lmVStaff* pVStaff, bool fChord)
////{
////    //Notes and rests have a very similar structure so they share a lot of analysis code.
////    //This method analyses the source of Notes and Rests and with its information builds
////    //the corresponding lmNote or lmRest object and appends
////    //it to the lmVStaff received as parameter.
////    //Returns a pointer to the lmNoteRest created.
////
////    // <Note> = (n <Pitch> <NoteType> [<NoteFlags>*] )
////    // <NoteFlags> = {L | <RestFlags> }
////    //
////    // <Rest> = (s <NoteType> [<RestFlags>]*)
////    // <RestFlags> = {C | AMR | G | P}
////
////    wxString sElmName = GetNodeName(pNode);       //for error messages
////    long nId = GetNodeID(pNode);
////    wxASSERT(sElmName.Left(1) == _T("n") ||
////             sElmName.Left(1) == _T("r") ||
////             sElmName == _T("na") );
////
////    bool fIsRest = (sElmName.Left(1) == _T("r"));   //analysing a rest
////
////    lmEStemType nStem = lmSTEM_DEFAULT;
////    bool fVisible = true;
////
////    //beam
////    lmBeamInfo* pBeamInfo = (lmBeamInfo*)NULL;
////    bool fBeamed = false;
////    lmTBeamInfo BeamInfo[6];
////    for (int i=0; i < 6; i++)
////    {
////        BeamInfo[i].Repeat = false;
////        BeamInfo[i].Type = eBeamNone;
////    }
////
////    //tie
////    lmTieInfo* pTieInfo = (lmTieInfo*)NULL;
////    bool fTie = false;
////
////    //Tuplet brakets
////    bool fEndTuplet = false;
////    //int nTupletNumber = 0;      // 0 = no tuplet
////    int nActualNotes = 0;       // 0 = no tuplet
////    int nNormalNotes = 0;
////
////    //default values
////    int nDots = 0;
////    lmENoteType nNoteType = eQuarter;
////    float rDuration = GetDefaultDuration(nNoteType, nDots, nActualNotes, nNormalNotes);
////    wxString sStep = _T("c");
////    wxString sOctave = _T("4");
////    lmEAccidentals nAccidentals = lm_eNoAccidentals;
////    lmEPitchType nPitchType = lm_ePitchRelative;
////
////    bool fInChord = !fIsRest && ((sElmName == _T("na")) || fChord );
////    long nParms = GetNodeNumParms(pNode);
////
////    //get parameters for pitch and duration
////    int iP = 1;
////    wxString sPitch = _T("");
////    wxString sDuration = _T("");
////    //if (sElmName != _T("na") && sElmName.length() > 1)
////    //{
////    //    //abbreviated notation. Split node name
////    //    bool fPitchFound = false;
////    //    bool fOctaveFound = false;
////    //    int i;
////    //    wxChar sChar;
////    //    for (i=1; i < (int)sElmName.length(); i++)
////    //    {
////    //        sChar = sElmName.GetChar(i);
////    //        if (sChar == _T('-') ||
////    //            sChar == _T('+') ||
////    //            sChar == _T('=') ||
////    //            sChar == _T('x') )
////    //        {
////    //            //accidental
////    //            sPitch += sChar;
////    //        }
////    //        else if ( (sElmName.Mid(i, 1)).IsNumber()) {
////    //            //octave
////    //            fOctaveFound = true;
////    //            sPitch += sChar;
////    //            i++;
////    //            break;
////    //        }
////    //        else {
////    //            if (fPitchFound) {
////    //                //octave not present. This is Duration
////    //                break;
////    //            }
////    //            else {
////    //                //note step name
////    //                sPitch += sChar;
////    //                fPitchFound = true;
////    //            }
////    //        }
////    //    }
////
////    //    //remaining string is Duration
////    //    if (i >= (int)sElmName.length()) {
////    //        //Duration not included. Inherit it
////    //        sDuration = m_sLastDuration;
////    //    }
////    //    else
////    //        sDuration = sElmName.substr(i);
////
////    //    if (fIsRest)
////    //    {
////    //       // for rests, first parameter is duration
////    //        sDuration = sPitch;
////    //    }
////
////    //    // inherit octave if not found
////    //    if (!fOctaveFound) sPitch += m_sLastOctave;
////
////    //    iP = 1;
////    //}
////
////    //else    //full notation. Get parameters
////   {
////        if (fIsRest) {
////            if (nParms < 1) {
////                AnalysisError(pNode, _T("Missing parameters in rest '%s'. Replaced by '(r n)'."),
////                    NodeToString(pNode).c_str() );
////				m_pLastNoteRest = pVStaff->AddRest(nId, nNoteType, rDuration, nDots,
////										m_nCurStaff, m_nCurVoice, fVisible);
////#if lmUSE_LIBRARY
////    m_pLastNoteRest->SetLdpElement(pNode);
////#endif
////                return m_pLastNoteRest;
////            }
////        }
////        else
////		{
////            if (nParms < 2)
////			{
////                AnalysisError(pNode, _T("Missing parameters in note '%s'. Assumed (n c4 n)."),
////                    NodeToString(pNode).c_str() );
////                lmNote* pNt = pVStaff->AddNote(nId, lm_ePitchRelative, 0, 4, 0,
////                                               nAccidentals,
////											   nNoteType, rDuration, nDots, m_nCurStaff,
////											   m_nCurVoice, fVisible, fBeamed, BeamInfo,
////											   (lmNote*)NULL, fTie, nStem);
////#if lmUSE_LIBRARY
////    m_pLastNoteRest->SetLdpElement(pNode);
////#endif
////				m_pLastNoteRest = pNt;
////                return pNt;
////            }
////        }
////
////        //get pitch and duration parameters
////        if (fIsRest) {
////           // for rests, first parameter is duration
////            sDuration = GetNodeName(pNode->GetParameter(1));
////            sPitch = wxEmptyString;
////            iP = 2;
////        }
////        else {
////             //for notes: first one is pitch and accidentals, second one duration
////            sPitch = GetNodeName(pNode->GetParameter(1));
////            sDuration = GetNodeName(pNode->GetParameter(2));
////            iP = 3;
////        }
////    }
////
////    //for notes: analyse_node pitch and accidentals
////    if (!fIsRest)
////    {
////        if (sPitch.IsNumber()) {
////            //if sPitch is a number it represents a MIDI pitch in C major key signature.
////            long nMidi = 0;
////            sPitch.ToLong(&nMidi);
////            sPitch = MPitch_ToLDPName((lmMPitch)nMidi);
////        }
////        if (sPitch == _T("*")) {
////            nPitchType = lm_ePitchNotDefined;
////        }
////        else {
////            if (LDPDataToPitch(sPitch, &nAccidentals, &sStep, &sOctave)) {
////                AnalysisError(pNode, _T("Unknown note pitch '%s'. Assumed 'c4'."),
////                    sPitch.c_str() );
////            }
////        }
////    }
////
////    //analyze duration and dots
////    if (AnalyzeNoteType(sDuration, &nNoteType, &nDots)) {
////        AnalysisError(pNode, _T("Unknown note/rest duration '%s'. A quarter note assumed."),
////            sDuration.c_str() );
////    }
////    m_sLastDuration = sDuration;
////
////    //analyze optional parameters
////	lmLDPOptionalTags oOptTags(this);
////	oOptTags.SetValid(lm_eTag_Visible, lm_eTag_Location_x, lm_eTag_Location_y,
////						lm_eTag_StaffNum, -1);		//finish list with -1
////
////	lmLocation tPos = g_tDefaultPos;
////    int nStaff = m_nCurStaff;
////
////	oOptTags.AnalyzeCommonOptions(pNode, iP, pVStaff, &fVisible, &nStaff, &tPos);
////	m_nCurStaff = nStaff;
////
////    //analyze remaining parameters: annotations and attachments
////    bool fFermata = false;
////    bool fThereAreAttachments = false;
////    lmEPlacement nFermataPlacement = ep_Default;
////	lmLocation tFermataPos = g_tDefaultPos;
////    long nFermataID = lmNEW_ID;
////
////    wxString sData;
////    int iLevel, nLevel;
////    lmLDPNode* pX = pNode->StartIterator(iP);
////    while (pX)
////    {
////        if (pX->IsProcessed())
////            ;   //ignore it
////        else if (pX->IsSimple())
////        {
////            // Analysis of simple notations
////
////            sData = GetNodeName(pX);
////            if (sData == _T("l") && !fIsRest)       //Tied to the next one
////            //AWARE: This notation is needed for exercise patterns. Can not be removed!
////            {
////                fTie = true;
////            }
////            else if (sData.Left(1) == _T("g"))      //beamed group
////            //AWARE: This notation is needed for exercise patterns. Can not be removed!
////            {
////                if (sData.substr(1,1) == _T("+")) {       //Start of beamed group. Simple parameter
////                    //compute beaming level dependig on note type
////                    nLevel = GetBeamingLevel(nNoteType);
////                    if (nLevel == -1) {
////                        AnalysisError(pNode,
////                            _T("Requesting beaming a note longer than eight. Beaming ignored."));
////                    }
////                    else {
////                        // and the previous note must be beamed
////                        if (m_pLastNoteRest && m_pLastNoteRest->IsBeamed() &&
////                            m_pLastNoteRest->GetBeamType(0) != eBeamEnd) {
////                            AnalysisError(pNode,
////                                _T("Requesting to start a beamed group but there is already an open group. Beaming ignored."));
////                        }
////                        fBeamed = true;
////                        for (iLevel=0; iLevel <= nLevel; iLevel++) {
////                            BeamInfo[iLevel].Type = eBeamBegin;
////                            //wxLogMessage(wxString::Format(
////                            //    _T("[lmLDPParser::AnalyzeNote] BeamInfo[%d] = eBeamBegin"), iLevel));
////                        }
////                    }
////                }
////
////                else if (sData.substr(1,1) == _T("-")) {       //End of beamed group
////                    //allow to close the beamed group
////                    bool fCloseBeam = true;
////
////                    //TODO   Beaming information only allowed in base note of chords
////                    //!         This program should move this information to base note
////                    //!         as this restriction is un-coherent with forcing the t- flag
////                    //!         to be in the last note of the chord.
////                    if (fInChord) {
////                        AnalysisError(pNode,
////                            _T("Request to end beaming a group in a note that is note the first one of a chord. Beaming ignored."));
////                        fCloseBeam = false;
////                    }
////
////                    //There must exist a previous note/rest
////                    if (!m_pLastNoteRest) {
////                        AnalysisError(pNode,
////                            _T("Request to end beaming a group but there is not a  previous note. Beaming ignored."));
////                        fCloseBeam = false;
////                    }
////                    else {
////                        // and the previous note must be beamed
////                        if (!m_pLastNoteRest->IsBeamed() ||
////                            m_pLastNoteRest->GetBeamType(0) == eBeamEnd) {
////                            AnalysisError(pNode,
////                                _T("Request to end beaming a group but previous note is not beamed. Beaming ignored."));
////                            fCloseBeam = false;
////                        }
////                    }
////
////                    //proceed to close all previous open levels
////                    if (fCloseBeam) {
////                        fBeamed = true;
////                        int nCurLevel = GetBeamingLevel(nNoteType);
////                        int nPrevLevel = GetBeamingLevel(m_pLastNoteRest->GetNoteType());
////
////                        // close commom levels (as this must be done in each if/else branch it has
////                        // been moved here to optimize. A commnet has been included there instead to
////                        // facilitate the understanding of the algorithm)
////                        for (iLevel=0; iLevel <= wxMin(nCurLevel, nPrevLevel); iLevel++) {
////                            BeamInfo[iLevel].Type = eBeamEnd;
////                            g_pLogger->LogTrace(_T("LDPParser_beams"),
////                                _T("[lmLDPParser::AnalyzeNoteRest] BeamInfo[%d] = eBeamEnd"), iLevel);
////                        }
////
////                        // deal with differences between current note level and previous note level
////                        if (nCurLevel > nPrevLevel) {
////                            // current level is grater than previous one ==>
////                            // close common levels (done) and put backward in current deeper levels
////                            for (; iLevel <= nCurLevel; iLevel++) {
////                                BeamInfo[iLevel].Type = eBeamBackward;
////                                g_pLogger->LogTrace(_T("LDPParser_beams"),
////                                    _T("[lmLDPParser::AnalyzeNoteRest] BeamInfo[%d] = eBeamBackward"), iLevel);
////                            }
////                        }
////                        else if  (nCurLevel < nPrevLevel) {
////                            // current level is lower than previous one:
////                            // close common levels (done) and close deeper levels in previous note
////                            for (; iLevel <= nPrevLevel; iLevel++) {
////                                if (m_pLastNoteRest->GetBeamType(iLevel) == eBeamContinue) {
////                                    m_pLastNoteRest->SetBeamType(iLevel, eBeamEnd);
////                                    g_pLogger->LogTrace(_T("LDPParser_beams"),
////                                        _T("[lmLDPParser::AnalyzeNoteRest] Changing previous BeamInfo[%d] = eBeamEnd"), iLevel);
////                                }
////                                else if (m_pLastNoteRest->GetBeamType(iLevel) == eBeamBegin) {
////                                    m_pLastNoteRest->SetBeamType(iLevel, eBeamForward);
////                                    g_pLogger->LogTrace(_T("LDPParser_beams"),
////                                        _T("[lmLDPParser::AnalyzeNoteRest] Changing previous BeamInfo[%d] = eBeamForward"), iLevel);
////                                }
////                            }
////                        }
////                        else {
////                            // current level is equal than previous one:
////                            // close common levels (done)
////                        }
////                    }
////                }
////
////                else {
////                    AnalysisError(pNode, _T("Error: notation '%s' unknown. It will be ignored."), sData.c_str() );
////                }
////
////            }
////
////            else if (sData.Left(1) == _T("t"))
////            {
////                //start/end of tuplet. Simple parameter (tn / t-)
////
////                lmTupletBracket* pTuplet;
////                bool fOpenTuplet = (m_pTuplet ? false : true);
////                if (!AnalyzeTuplet(pX, sElmName, fOpenTuplet, !fOpenTuplet,
////                     &pTuplet, &nActualNotes, &nNormalNotes))
////                {
////                    if (pTuplet) {
////                        // start of tuplet
////                        m_pTuplet = pTuplet;
////                    }
////                    else {
////                        //end of tuplet
////                        fEndTuplet = true;
////                    }
////                }
////            }
////
////            else if (sData.Left(1) == _T("v"))      //voice
////            {
////				m_nCurVoice = AnalyzeVoiceNumber(sData, pNode);
////			}
////            else if (sData == _T("fermata"))        //fermata
////			{
////                fFermata = true;
////            }
////            else {
////                AnalysisError(pNode, _T("Error: notation '%s' unknown. It will be ignored."), sData.c_str() );
////            }
////
////       }
////
////       else     // Analysis of compound notations
////       {
////            sData = GetNodeName(pX);
////            if (sData == _T("g"))       //Start of group element
////            {
////                AnalysisError(pX, _T("Notation '%s' unknown or not implemented. Old (g + t3) syntax?"), sData.c_str());
////            }
////            else if (sData == _T("stem"))       //stem attributes
////            {
////                nStem = AnalyzeStem(pX, pVStaff);
////            }
////            else if (sData == _T("fermata"))        //fermata attributes
////            {
////                nFermataID = GetNodeID(pX);
////                fFermata = true;
////                nFermataPlacement = AnalyzeFermata(pX, pVStaff, &tFermataPos);
////            }
////            else if (sData == _T("t")) {       //start/end of tuplet. Simple parameter (tn / t-)
////                lmTupletBracket* pTuplet;
////                bool fOpenTuplet = (m_pTuplet ? false : true);
////                if (!AnalyzeTuplet(pX, sElmName, fOpenTuplet, !fOpenTuplet,
////                     &pTuplet, &nActualNotes, &nNormalNotes))
////                {
////                    if (pTuplet) {   // start of tuplet
////                        m_pTuplet = pTuplet;
////                    }
////                    else {          //end of tuplet
////                        fEndTuplet = true;
////                    }
////                }
////            }
////            else if (sData == _T("tie"))     //start/end of tie
////            {
////                if (fIsRest)
////                    AnalysisError(pX, _T("Rests can not be tied. Tie ignored."), sData.c_str());
////                else
////                    pTieInfo = AnalyzeTie(pX, pVStaff);
////            }
////            else if (sData == _T("beam"))     //start/continue/end of beam
////            {
////                pBeamInfo = AnalyzeBeam(pX, pVStaff);
////            }
////            else
////            {
////                //All other parameters will be considered as attachments. Therefore, I will
////                //proceed to create the note/rest and after, I will continue the analysis and
////                //add each attachment to the note/rest
////                fThereAreAttachments = true;
////                break;
////            }
////        }
////        pX = pNode->GetNextParameter();
////
////    }
////
////    //force beaming for notes between eBeamBegin and eBeamEnd (only for single notes
////    //and chord base notes, not for secondary notes of a chord)
////    if (!fBeamed && !fInChord && nNoteType > eQuarter)
////    {
////        if (m_pLastNoteRest)
////        {
////           if (m_pLastNoteRest->IsBeamed())
////            {
////                //it can be the end of a group. Let's verify that at least a beam is open
////                for (iLevel=0; iLevel < 6; iLevel++)
////                {
////                   if ((m_pLastNoteRest->GetBeamType(iLevel) == eBeamBegin)
////                         || (m_pLastNoteRest->GetBeamType(iLevel) == eBeamContinue))
////                    {
////                            fBeamed = true;
////                            break;
////                    }
////                }
////
////                if (fBeamed)
////                {
////                    int nCurLevel = GetBeamingLevel(nNoteType);
////                    int nPrevLevel = GetBeamingLevel(m_pLastNoteRest->GetNoteType());
////
////                    // continue common levels
////                    for (iLevel=0; iLevel <= wxMin(nCurLevel, nPrevLevel); iLevel++)
////                    {
////                        BeamInfo[iLevel].Type = eBeamContinue;
////                        g_pLogger->LogTrace(_T("LDPParser_beams"),
////                            _T("[lmLDPParser::AnalyzeNoteRest] BeamInfo[%d] = eBeamContinue"), iLevel);
////                    }
////
////                    if (nCurLevel > nPrevLevel)
////                    {
////                        // current level is grater than previous one, start new beams
////                        for (; iLevel <= nCurLevel; iLevel++) {
////                            BeamInfo[iLevel].Type = eBeamBegin;
////                            g_pLogger->LogTrace(_T("LDPParser_beams"),
////                                _T("[lmLDPParser::AnalyzeNoteRest] BeamInfo[%d] = eBeamBegin"), iLevel);
////                        }
////                    }
////                    else if  (nCurLevel < nPrevLevel)
////                    {
////                        // current level is lower than previous one
////                        // close common levels (done) and close deeper levels in previous note
////                        for (; iLevel <= nPrevLevel; iLevel++)
////                        {
////                            if (m_pLastNoteRest->GetBeamType(iLevel) == eBeamContinue)
////                            {
////                                m_pLastNoteRest->SetBeamType(iLevel, eBeamEnd);
////                                g_pLogger->LogTrace(_T("LDPParser_beams"),
////                                    _T("[lmLDPParser::AnalyzeNoteRest] Changing previous BeamInfo[%d] = eBeamEnd"), iLevel);
////                            }
////                            else if (m_pLastNoteRest->GetBeamType(iLevel) == eBeamBegin)
////                            {
////                                m_pLastNoteRest->SetBeamType(iLevel, eBeamForward);
////                                g_pLogger->LogTrace(_T("LDPParser_beams"),
////                                    _T("[lmLDPParser::AnalyzeNoteRest] Changing previous BeamInfo[%d] = eBeamFordward"), iLevel);
////                            }
////                        }
////                    }
////                }
////            }
////        }
////    }
////
////    //if not first note of tuple, tuple information is not present and need to be taken from
////    //previous note
////    if (m_pTuplet)
////    {
////        // a tuplet is open
////        nActualNotes = m_pTuplet->GetActualNotes();
////        nNormalNotes = m_pTuplet->GetNormalNotes();
////    }
////
////    // calculation of duration
////    rDuration = GetDefaultDuration(nNoteType, nDots, nActualNotes, nNormalNotes);
////
////    //Verify if note in chord has the same duration than base note
////    if (fInChord && m_pLastNoteRest && m_pLastNoteRest->IsNote()
////        && !IsEqualTime(m_pLastNoteRest->GetDuration(), rDuration) )
////    {
////        AnalysisError(pNode, _T("Error: note in chord has different duration than base note. Duration changed."));
////		rDuration = m_pLastNoteRest->GetDuration();
////        nNoteType = m_pLastNoteRest->GetNoteType();
////        nDots = m_pLastNoteRest->GetNumDots();
////    }
////
////    //create the note/rest
////    lmNoteRest* pNR;
////    if (fIsRest)
////	{
////        pNR = pVStaff->AddRest(nId, nNoteType, rDuration, nDots,
////                               m_nCurStaff, m_nCurVoice, fVisible, fBeamed, BeamInfo);
////		m_pLastNoteRest = pNR;
////    }
////    else
////	{
////        //TODO: Convert earlier to int
////        int nStep = LetterToStep(sStep);
////        long nAux;
////        sOctave.ToLong(&nAux);
////        int nOctave = (int)nAux;
////
////        lmNote* pNt = pVStaff->AddNote(nId, nPitchType,
////                               nStep, nOctave, 0, nAccidentals,
////                               nNoteType, rDuration, nDots, m_nCurStaff,
////                               m_nCurVoice, fVisible, fBeamed, BeamInfo,
////							   (fInChord ? (lmNote*)m_pLastNoteRest : (lmNote*)NULL),
////							   fTie, nStem);
////		if (!fInChord || pNt->IsBaseOfChord())
////			m_pLastNoteRest = pNt;
////
////		pNR = pNt;
////        m_sLastOctave = sOctave;
////
////        //add tie, if exists
////        if (pTieInfo)
////        {
////            if (pTieInfo->fStart)
////            {
////                //start of tie. Save the information
////                pTieInfo->pNote = pNt;
////                m_PendingTies.push_back(pTieInfo);
////            }
////            else
////            {
////                //end of tie. Add it to the internal model
////                AddTie(pNt, pTieInfo);
////            }
////        }
////    }
////	pNR->SetUserLocation(tPos);
////#if lmUSE_LIBRARY
////    pNR->SetLdpElement(pNode);
////#endif
////
////    //save beaming information
////    if (pBeamInfo)
////    {
////        pBeamInfo->pNR = pNR;
////        if (pBeamInfo->fEndOfBeam)
////        {
////            //end of beam. Add it to the internal model
////            AddBeam(pNR, pBeamInfo);
////        }
////        else
////        {
////            //start or continuation of beam. Save the information
////            m_PendingBeams.push_back(pBeamInfo);
////        }
////    }
////
////    //save cursor data
////    if (m_fCursorData && m_nCursorObjID == nId)
////        m_pCursorSO = pNR;
////
////    // Add notations
////    if (m_pTuplet) {
////        m_pTuplet->Include(pNR);             // add this note/rest to the tuplet
////
////        if (fEndTuplet) {
////            m_pTuplet = (lmTupletBracket*)NULL;
////        }
////    }
////
////	if (fFermata) {
////		lmFermata* pFermata = pNR->AddFermata(nFermataPlacement, nFermataID);
////		pFermata->SetUserLocation(tFermataPos);
////	}
////
////    //note/rest is created. Let's continue analyzing StaffObj options and
////    //attachments for the created note/rest
////    if (fThereAreAttachments)
////        AnalyzeAttachments(pNode, pVStaff, pX, pNR);
////
////    return pNR;
////}
////
////void lmLDPParser::AnalyzeAttachments(lmLDPNode* pNode, lmVStaff* pVStaff,
////                                     lmLDPNode* pX, lmStaffObj* pAnchor)
////{
////    //pX is a parameter of main node pNode
////
////    while (pX)
////    {
////        wxString sName = GetNodeName(pX);
////        //options for StaffObj
////        if (sName == _T("color"))
////            pAnchor->SetColour( AnalyzeColor(pX) );
////
////        //attachments
////        else if (sName == _T("line"))
////            AnalyzeLine(pX, pVStaff, pAnchor);
////        else if (sName == _T("textbox"))
////            AnalyzeTextbox(pX, pVStaff, pAnchor);
////        else if (sName == _T("text"))
////            AnalyzeText(pX, pVStaff, pAnchor);
////        else
////            AnalysisError(pX, _T("Notation '%s' unknown or not implemented."), sName.c_str());
////
////        pX = pNode->GetNextParameter();
////    }
////}
////
////bool lmLDPParser::AnalyzeTuplet(lmLDPNode* pNode, const wxString& sParent,
////                                bool fOpenAllowed, bool fCloseAllowed,
////                                lmTupletBracket** pTuplet, int* pActual, int* pNormal)
////{
////    // sParent: name of parent element
////    // Returns true if errors. The elemenent is ignored.
////    // If no errors, updates pTuplet, pActual and pNormal with the result of parsing:
////    //      - start of tuplet: pTuple points to new lmTupletBracket
////    //      - end of tuplet: pTuplet is NULL
////
////
////    // <Tuplet> = (t {- | + ActualNotes [NormalNotes][options] } )
////    // Abbreviations:
////    //      (t -)     --> t-
////    //      (t + n)   --> tn
////    //      (t + n m) --> tn/m
////
////    bool fEndTuplet = false;
////    int nActualNum, nNormalNum;
////
////    bool fShowTupletBracket = m_fShowTupletBracket;
////    bool fShowNumber = m_fShowNumber;
////	lmEPlacement nTupletAbove = ep_Default;
////
////    wxString sData = GetNodeName(pNode);
////
////    if (pNode->IsSimple()) {
////        //start/end of tuplet. Simple parameter (t- | tn | tn/m )
////        wxASSERT(sData.Left(1) == _T("t"));
////        if (sData == _T("t-")) {
////            //end of tuplet
////            fEndTuplet = true;
////        }
////        else {
////            //start of tuplet
////            wxString sNumTuplet = sData.substr(1);
////            int nSlash = sNumTuplet.Find(_T("/"));
////            if (nSlash == 0) {
////                //error: invalid element 't/num'
////                AnalysisError(pNode, _T("[%s] Found unknown tag '%s'. Ignored."),
////                    sParent.c_str(), sData.c_str());
////                return true;
////            }
////            else if (nSlash == -1) {
////                //abbreviated tuplet: 'tn'
////                if (!sNumTuplet.IsNumber()) {
////                    AnalysisError(pNode, _T("[%s] Found unknown tag '%s'. Ignored."),
////                        sParent.c_str(), sData.c_str());
////                    return true;
////                }
////                else {
////                    long nNum;
////                    sNumTuplet.ToLong(&nNum);
////                    nActualNum = (int)nNum;
////                    //implicit value for denominator
////                    if (nActualNum == 2)
////                        nNormalNum = 3;   //duplet
////                    else if (nActualNum == 3)
////                        nNormalNum = 2;   //triplet
////                    else if (nActualNum == 4)
////                        nNormalNum = 6;
////                    else if (nActualNum == 5)
////                        nNormalNum = 6;
////                    else {
////                        AnalysisError(pNode, _T("[%s] Found tag '%s' but no default value exists for NormalNotes. Ignored."),
////                            sParent.c_str(), sData.c_str());
////                        return true;
////                    }
////                }
////            }
////            else {
////                //abbreviated tuplet: 'tn:m'. Split the two numbers
////                wxString sActualNum = sNumTuplet.Left(nSlash);
////                wxString sNormalNum = sNumTuplet.substr(nSlash+1);
////
////                if (!sActualNum.IsNumber() || !sNormalNum.IsNumber() ) {
////                    AnalysisError(pNode, _T("[%s] Found unknown tag '%s'. Ignored."),
////                        sParent.c_str(), sData.c_str());
////                    return true;
////                }
////                else {
////                    long nNum;
////                    sActualNum.ToLong(&nNum);
////                    nActualNum = (int)nNum;
////                    sNormalNum.ToLong(&nNum);
////                    nNormalNum = (int)nNum;
////                    if (nNormalNum < 1 || nActualNum < 1) {
////                        AnalysisError(pNode, _T("[%s] Tag '%s'. Numbers must be greater than 0. Tag ignored."),
////                            sParent.c_str(), sData.c_str());
////                        return true;
////                    }
////                }
////            }
////        }
////    }
////
////    else {
////        //compound element
////
////        wxString sElmName = GetNodeName(pNode);
////
////        //check that at least one parameters (+, - sign) is specified
////        if(GetNodeNumParms(pNode) < 2) {
////            AnalysisError(
////                pNode,
////                _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////                sElmName.c_str() );
////            return true;
////        }
////
////        // get type: + or -
////        wxString sType = GetNodeName(pNode->GetParameter(1));
////        if (sType ==_T("+") ) {             //start of tuplet
////            fEndTuplet = false;
////        } else if (sType ==_T("-") ) {      //end of tuplet
////            fEndTuplet = true;
////        } else {
////            AnalysisError(pNode, _T("Element '%s': invalid type '%s'. It is neither '+' nor '-'. Tuplet ignored."),
////                sElmName.c_str(), sType.c_str() );
////            return true;    //error
////        }
////
////        // get actual notes number
////        wxString sNumTuplet = GetNodeName(pNode->GetParameter(2));
////        if (!sNumTuplet.IsNumber()) {
////            AnalysisError(pNode, _T("Element '%s': Expected number but found '%s'. Tuplet ignored."),
////                sElmName.c_str(), sData.c_str());
////            return true;
////        }
////        else {
////            long nNum;
////            sNumTuplet.ToLong(&nNum);
////            nActualNum = (int)nNum;
////            //implicit value for denominator
////            if (nActualNum == 2)
////                nNormalNum = 3;   //duplet
////            else if (nActualNum == 3)
////                nNormalNum = 2;   //triplet
////            else if (nActualNum == 4)
////                nNormalNum = 6;
////            else if (nActualNum == 5)
////                nNormalNum = 6;
////            else
////                nNormalNum = 0;  //required
////        }
////
////        // loop to parse remaining parameters: NormalNum and Options
////        long iP = 3;
////        wxString sData;
////        for(; iP <= GetNodeNumParms(pNode); iP++) {
////            sData = GetNodeName(pNode->GetParameter(iP));
////            if (fEndTuplet) {
////                AnalysisError(pNode, _T("Element '%s': Found unknown data '%s'. Data ignored."),
////                    sElmName.c_str(), sData.c_str());
////            }
////            else {
////                //check if Normal notes number
////                if (sData.IsNumber()) {
////                    long nNum;
////                    sData.ToLong(&nNum);
////                    nNormalNum = (int)nNum;
////                    if (nNormalNum < 1) {
////                        AnalysisError(pNode, _T("Element '%s': Number for 'normal notes' must be greater than 0. Number ignored."),
////                            sElmName.c_str(), sData.c_str());
////                        return true;
////                    }
////                }
////                else if (sData == _T("noBracket")) {
////                    fShowTupletBracket = false;
////                }
////                else if (sData == _T("squaredBracket")) {
////                    //TODO implement different kinds of brackets
////                    fShowTupletBracket = true;
////                }
////                else if (sData == _T("curvedBracket")) {
////                    fShowTupletBracket = true;
////                }
////                else if (sData == _T("numNone")) {
////                    fShowNumber = false;
////                }
////                else if (sData == _T("numActual")) {
////                    //TODO implement different options to display numbers
////                    fShowNumber = true;
////                }
////                else if (sData == _T("numBoth")) {
////                    fShowNumber = true;
////                }
////                else {
////                    AnalysisError(pNode, _T("Element '%s': Found unknown data '%s'. Data ignored."),
////                        sElmName.c_str(), sData.c_str());
////                }
////           }
////        }
////
////    }
////
////    //All information parsed. Prepare return info
////    if (fEndTuplet) {
////        if (!fCloseAllowed) {
////            // there isn't an open tuplet
////            AnalysisError(pNode, _T("[%s] Requesting to end a tuplet but there is not an open tuplet or it is not possible to close it here. Tag '%s' ignored."),
////                sParent.c_str(), sData.c_str());
////            return true;
////        }
////        *pTuplet = (lmTupletBracket*) NULL;
////    }
////    else {
////        if (!fOpenAllowed) {
////            //there is already a tuplet open and not closed
////            AnalysisError(pNode, _T("[%s] Requesting to start a tuplet but there is already a tuplet open. Tag '%s' ignored."),
////                sParent.c_str(), sData.c_str());
////            return true;
////        }
////
////        //save new options
////        m_fShowTupletBracket = fShowTupletBracket;
////        m_fShowNumber = fShowNumber;
////
////        // create tuplet braket
////        *pTuplet = new lmTupletBracket(fShowNumber, nActualNum, fShowTupletBracket,
////                            nTupletAbove, nActualNum, nNormalNum);
////        *pActual = nActualNum;
////        *pNormal = nNormalNum;
////        //#if lmUSE_LIBRARY
////        //    (*pTuplet)->SetLdpElement(pNode);
////        //#endif
////    }
////
////    return false;
////
////}
////
////lmBarline* lmLDPParser::AnalyzeBarline(lmLDPNode* pNode, lmVStaff* pVStaff)
////{
////    //returns the created barline, or NULL if error; in this case nothing is added
////    //to the lmVStaff
////
////    // <Barline> = (barline <BarType> [<Visible>][<location>])
////    // <BarType> = {"start" | "end" | "double" | "simple" |
////    //              "startRepetition" | "endRepetition" | "doubleRepetition" }
////
////    wxString sElmName = GetNodeName(pNode);
////    long nId = GetNodeID(pNode);
////    wxASSERT(sElmName == _T("barline"));
////
////    //check that bar type is specified
////    if(GetNodeNumParms(pNode) < 1) {
////        //assume simple barline, visible
////        lmBarline* pBL = pVStaff->AddBarline(lm_eBarlineSimple, true, nId);
////		m_nCurVoice = 1;
////        return pBL;
////    }
////
////    lmEBarline nType = lm_eBarlineSimple;
////
////    wxString sType = GetNodeName(pNode->GetParameter(1));
////    if (sType == _T("simple"))
////        nType = lm_eBarlineSimple;
////    else if (sType == _T("double"))
////        nType = lm_eBarlineDouble;
////    else if (sType == _T("start"))
////        nType = lm_eBarlineStart;
////    else if (sType == _T("end"))
////        nType = lm_eBarlineEnd;
////    else if (sType == _T("endRepetition"))
////        nType = lm_eBarlineEndRepetition;
////    else if (sType == _T("startRepetition"))
////        nType = lm_eBarlineStartRepetition;
////    else if (sType == _T("doubleRepetition"))
////        nType = lm_eBarlineDoubleRepetition;
////    else
////        AnalysisError(pNode, _T("Unknown barline type '%s'. 'simple' barline assumed."),
////            sType.c_str() );
////
////    //analyze remaining optional parameters
////    int iP = 2;
////	lmLDPOptionalTags oOptTags(this);
////	oOptTags.SetValid(lm_eTag_Visible, lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1
////	lmLocation tPos = g_tDefaultPos;
////    bool fVisible = true;
////	oOptTags.AnalyzeCommonOptions(pNode, iP, pVStaff, &fVisible, NULL, &tPos);
////
////	//create the barline
////    lmBarline* pBarline = pVStaff->AddBarline(nType, fVisible, nId);
////	m_nCurVoice = 1;
////	pBarline->SetUserLocation(tPos);
////
////    //save cursor data
////    if (m_fCursorData && m_nCursorObjID == nId)
////        m_pCursorSO = pBarline;
////
////    return pBarline;
////}
////
////
////bool lmLDPParser::GetValueYesNo(lmLDPNode* pNode, bool fDefault)
////{
////    wxString sValue = GetNodeName(pNode->GetParameter(1)).Lower();
////    if (sValue == _T("true") || sValue == _T("yes"))
////    {
////        return true;
////    }
////    else if (sValue == _T("false") || sValue == _T("no"))
////    {
////        return false;
////    }
////    else
////    {
////        //get option name and value
////        wxString sName = GetNodeName(pNode);
////        wxString sError = _T("a 'yes/no' or 'true/false' value");
////        AnalysisError(pNode, _T("Error in data value for option '%s'.  It requires %s. Value '%s' ignored."),
////            sName.c_str(), sError.c_str(), sValue.c_str());
////    }
////    return fDefault;
////}
////
////
////}
////
////bool lmLDPParser::AnalyzeTitle(lmLDPNode* pNode, lmScore* pScore)
////{
////    //returns true if error; in this case nothing is added to the score
////    //
////    //  (title <alignment> string [<font>][<location>])
////
////    wxASSERT(GetNodeName(pNode) == _T("title"));
////    long nId = GetNodeID(pNode);
////
////    //check that at least two parameters (aligment and text string) are specified
////    if(GetNodeNumParms(pNode) < 2) {
////        AnalysisError(
////            pNode,
////            _T("Element 'title' has less parameters than the minimum required. Element ignored."));
////        return true;
////    }
////
////    wxString sTitle;
////    wxString sStyle = _T("");
////    bool fFont = false;
////    lmEHAlign nAlign = m_nTitleAlignment;
////    lmFontInfo tFont = {m_sTitleFontName, m_nTitleFontSize, m_nTitleStyle, m_nTitleWeight};
////    lmLocation tPos;
////    tPos.xUnits = lmTENTHS;
////    tPos.yUnits = lmTENTHS;
////    tPos.x = 0.0f;
////    tPos.y = 0.0f;
////
////    //get the aligment
////    long iP = 1;
////    wxString sName = GetNodeName(pNode->GetParameter(iP));
////    if (sName == _T("left"))
////        nAlign = lmHALIGN_LEFT;
////    else if (sName == _T("right"))
////        nAlign = lmHALIGN_RIGHT;
////    else if (sName == _T("center"))
////        nAlign = lmHALIGN_CENTER;
////    else {
////        AnalysisError(pNode, _T("Invalid alignment value '%s'. Assumed 'center'."),
////            sName.c_str() );
////        nAlign = lmHALIGN_CENTER;
////    }
////    //save alignment as new default for titles
////    m_nTitleAlignment = nAlign;
////    iP++;
////
////    //get the string
////    sTitle = GetNodeName(pNode->GetParameter(iP));
////    iP++;
////
////    //analyze remaining parameters (optional): font, style, location
////    lmLDPNode* pX;
////    for(; iP <= GetNodeNumParms(pNode); iP++) {
////        pX = pNode->GetParameter(iP);
////        sName = GetNodeName(pX);
////
////        if (sName == _T("font"))
////        {
////            if (sStyle != _T(""))
////                AnalysisError(pX, _T("[Conflict: 'Font' and 'Style' in the same definition. Font ingnored."));
////            else
////            {
////                fFont = true;
////                AnalyzeFont(pX, &tFont);
////                //save font values as new default for titles
////                m_sTitleFontName = tFont.sFontName;
////                m_nTitleFontSize = tFont.nFontSize;
////                m_nTitleStyle = tFont.nFontStyle;
////                m_nTitleWeight = tFont.nFontWeight;
////            }
////        }
////        else if (sName == _T("style"))
////        {
////            if (fFont)
////                AnalysisError(pX, _T("[Conflict: 'Font' and 'Style' in the same definition. Font ingnored."));
////            sStyle = GetNodeName(pX->GetParameter(1));
////        }
////        else if (sName == _T("dx"))
////        {
////            AnalysisError(pX, _T("Obsolete: x location is not allowed in titles.") );
////        }
////        else if (sName == _T("dy"))
////        {
////            AnalyzeLocation(pX, &tPos);
////        }
////        else {
////            AnalysisError(pX, _T("Unknown parameter '%s'. Ignored."), sName.c_str());
////        }
////    }
////
////    //create the title
////    lmTextStyle* pStyle = (lmTextStyle*)NULL;
////    if (sStyle != _T(""))
////    {
////        pStyle = pScore->GetStyleInfo(sStyle);
////        if (!pStyle)
////            AnalysisError(pNode, _T("Style '%s' is not defined. Default style will be used."),
////                           sStyle.c_str());
////    }
////
////    if (!pStyle)
////        pStyle = pScore->GetStyleName(tFont);
////
////    lmScoreTitle* pTitle = pScore->AddTitle(sTitle, nAlign, pStyle, nId);
////	pTitle->SetUserLocation(tPos);
////
////    return false;
////
////}
////
////bool lmLDPParser::AnalyzeTextString(lmLDPNode* pNode, wxString* pText, wxString* pStyle,
////                                    lmEHAlign* pAlign, lmLocation* pPos,
////                                    lmFontInfo* pFont)
////{
////    //A certain number of LDP elements accepts a text-string with additional parameters,
////    //such as location, font or alignment. This method parses these elements.
////    //Default values for information not present must be initialized in return variables
////    //before invoking this method.
////    //Optional parameters not allowed in a particular context should be NULL pointers.
////    //Returns true if error; in this case return variables are not changed.
////    //If no error all variables but pNode are loaded with parsed information
////
////    // <text-string> = (validTextTag string [<location>][{<font> | <style>}][<alingment>])
////    // <style> = (style <name>)
////    // <validTextTag> = { name | abbrev | text }
////
////    wxASSERT(GetNodeName(pNode) == _T("name")
////             || GetNodeName(pNode) == _T("abbrev")
////             || GetNodeName(pNode) == _T("text") );
////
////    //check that at least one parameter (text string) is specified
////    if(GetNodeNumParms(pNode) < 1) {
////        AnalysisError(
////            pNode,
////            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////            GetNodeName(pNode).c_str() );
////        return true;
////    }
////
////    wxString sText;
////    wxString sStyle = _T("");
////    bool fFont = false;
////    lmEHAlign nAlign;
////    lmFontInfo tFont;
////    lmLocation tPos;
////
////    //load default values
////    if (pAlign)
////        nAlign = *pAlign;
////    if (pFont)
////    {
////        tFont.nFontSize = pFont->nFontSize;
////        tFont.nFontStyle = pFont->nFontStyle;
////        tFont.nFontWeight = pFont->nFontWeight;
////        tFont.sFontName = pFont->sFontName;
////    }
////    if (pPos)
////        tPos = *pPos;
////
////    int iP = 1;
////
////    //get the string
////    sText = GetNodeName(pNode->GetParameter(iP));
////    iP++;
////
////    //get remaining optional parameters: location, font, alignment
////    lmLDPNode* pX;
////    wxString sName;
////    for(; iP <= GetNodeNumParms(pNode); iP++)
////    {
////        pX = pNode->GetParameter(iP);
////        sName = GetNodeName(pX);
////
////        if (sName == _T("x") || sName == _T("dx") ||
////            sName == _T("y") || sName == _T("dy") )
////        {
////            AnalyzeLocation(pX, &tPos);
////        }
////        else if (sName == _T("font"))
////        {
////            fFont = true;
////            if (sStyle != _T(""))
////                AnalysisError(pX, _T("[Conflict: 'Font' and 'Style' in the same definition. Font ingnored."));
////            else
////                AnalyzeFont(pX, &tFont);
////        }
////        else if (sName == _T("style"))
////        {
////            if (fFont)
////                AnalysisError(pX, _T("[Conflict: 'Font' and 'Style' in the same definition. Font ingnored."));
////            sStyle = GetNodeName(pX->GetParameter(1));
////        }
////        else if (sName == _T("left")) {
////            nAlign = lmHALIGN_LEFT;
////        }
////        else if (sName == _T("right")) {
////            nAlign = lmHALIGN_RIGHT;
////        }
////        else if (sName == _T("center")) {
////            nAlign = lmHALIGN_CENTER;
////        }
////        else if (sName == _T("hasWidth")) {
////            AnalysisError(pX, _T("[Element '%s'. Obsolete parameter '%s'. Ignored."),
////                GetNodeName(pNode).c_str(), sName.c_str() );
////        }
////        else {
////            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
////                GetNodeName(pNode).c_str(), sName.c_str() );
////        }
////    }
////
////    //return parsed values
////    *pText = sText;
////    if (pAlign)
////        *pAlign = nAlign;
////    if (pPos)
////        *pPos = tPos;
////    if (pFont)
////        *pFont = tFont;
////    if (pStyle)
////        *pStyle = sStyle;
////
////    return false;
////}
////
////bool lmLDPParser::AnalyzeDefineStyle(lmLDPNode* pNode, lmScore* pScore)
////{
////    // <defineStyle> = (defineStyle <name><font><color>)
////
////    //Analyzes a 'defineStyle' tag and, if successful, register the style in the
////    //received score. Returns true if success.
////
////    wxASSERT(GetNodeName(pNode) == _T("defineStyle"));
////
////    //check that three parameters are specified
////    if(GetNodeNumParms(pNode) != 3) {
////        AnalysisError(
////            pNode,
////            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////            GetNodeName(pNode).c_str() );
////        return false;
////    }
////
////    //initialize values
////    lmFontInfo tFont = {m_sTextFontName, m_nTextFontSize, m_nTextStyle, m_nTextWeight};
////    wxColour color(0, 0, 0);        //default: black
////
////    //get the style name
////    int iP = 1;
////    wxString sStyleName = GetNodeName(pNode->GetParameter(iP));
////    iP++;
////
////    //get font and color, in no particular order
////    lmLDPNode* pX;
////    wxString sName;
////    for(; iP <= GetNodeNumParms(pNode); iP++)
////    {
////        pX = pNode->GetParameter(iP);
////        sName = GetNodeName(pX);
////
////        if (sName == _T("font"))
////        {
////            AnalyzeFont(pX, &tFont);
////        }
////        else if (sName == _T("color"))
////        {
////            color = AnalyzeColor(pX);
////        }
////        else
////        {
////            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
////                GetNodeName(pNode).c_str(), sName.c_str() );
////        }
////    }
////
////    //register the style
////    if (!sStyleName.IsEmpty())
////        pScore->AddStyle(sStyleName, tFont, color);
////
////    return true;
////}
////
////bool lmLDPParser::AnalyzeCreationMode(lmLDPNode* pNode, lmScore* pScore)
////{
////    // <creationMode> = (creationMode <modeName><modeVersion>)
////
////    //Returns true if success.
////
////    wxASSERT(GetNodeName(pNode) == _T("creationMode"));
////
////    //check that two parameters are specified
////    if(GetNodeNumParms(pNode) != 2) {
////        AnalysisError(
////            pNode,
////            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////            GetNodeName(pNode).c_str() );
////        return false;
////    }
////
////    //get the mode info
////    wxString sModeName = GetNodeName(pNode->GetParameter(1));
////    wxString sModeVers = GetNodeName(pNode->GetParameter(2));
////
////    //transfer to the score
////    pScore->SetCreationMode(sModeName, sModeVers);
////
////    return true;
////}
////
////bool lmLDPParser::AnalyzeCursor(lmLDPNode* pNode, lmScore* pScore)
////{
////    // <cursor> = (cursor <instrNumber><staffNumber><timePos><objID>)
////
////    //Returns true if success.
////
////    wxASSERT(pNode && GetNodeName(pNode) == _T("cursor"));
////
////    //check that four parameters are specified
////    if(GetNodeNumParms(pNode) != 4) {
////        AnalysisError(
////            pNode,
////            _T("Element '%s' has %d parameters, less than the minimum required. Element ignored."),
////            GetNodeName(pNode).c_str(), GetNodeNumParms(pNode) );
////        return false;
////    }
////
////    //get the cursor info
////    GetValueIntNumber(pNode, &m_nCursorInstr, 1);
////    GetValueIntNumber(pNode, &m_nCursorStaff, 2);
////    GetValueFloatNumber(pNode, &m_rCursorTime, 3);
////    GetValueLongNumber(pNode, &m_nCursorObjID, 4);
////
////    //save data
////    m_fCursorData = true;
////
////    return true;
////}
////
////wxColour lmLDPParser::AnalyzeColor(lmLDPNode* pNode)
////{
////    // <color> = (color #rrggbb))
////
////    //returns the result of the analysis. If error, returns black color.
////
////    wxASSERT(GetNodeName(pNode) == _T("color"));
////
////    //check that one parameter is specified
////    wxColor color;
////    if(GetNodeNumParms(pNode) != 1) {
////        AnalysisError(
////            pNode,
////            _T("Element 'color' has less parameters than the minimum required. Color black will be used."));
////        color.Set(0,0,0);
////        return color;
////    }
////
////    wxString sColor;
////
////    //get the color in HTML-like syntax (i.e. "#" followed by 6 hexadecimal digits
////    //for red, green and blue components or 8 hexadecimal digits to include alpha channel
////    sColor = GetNodeName(pNode->GetParameter(1));
////
////    //convert to color value
////    if (!color.Set(sColor))
////    {
////        AnalysisError(pNode, _T("Invalid color value '%s'. Black will be used."),
////                       sColor.c_str() );
////        color.Set(0,0,0);
////    }
////
////    return color;
////}
////
////bool lmLDPParser::AnalyzePageLayout(lmLDPNode* pNode, lmScore* pScore)
////{
////	//  <pageLayout> := (pageLayout <pageSize><pageMargins><pageOrientation>)
////	//  <pageSize> := (pageSize width height)
////	//  <pageMargins> := (pageMargins left top right bottom binding)
////	//  <pageOrientation> := [ "portrait" | "landscape" ]
////
////    //Analyzes a 'pageLayout' tag and, if successful, pass layout data to the
////    //received score. Returns true if success.
////
////    wxASSERT(GetNodeName(pNode) == _T("pageLayout"));
////
////    //check that three parameters are specified
////    if(GetNodeNumParms(pNode) != 3) {
////        AnalysisError(
////            pNode,
////            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////            GetNodeName(pNode).c_str() );
////        return false;
////    }
////
////    //get page size
////    int iP = 1;
////    lmLDPNode* pX = pNode->GetParameter(iP);
////    wxString sName = GetNodeName(pX);
////    if (sName != _T("pageSize"))
////    {
////        AnalysisError(pX, _T("Expected 'pageSize' element but found '%s'. Ignored."),
////            sName.c_str() );
////		return false;
////    }
////    if(GetNodeNumParms(pX) != 2)
////    {
////        AnalysisError(
////            pNode,
////            _T("Element '%s' has %d parameters, less than the minimum required. Element ignored."),
////				_T("pageSize"), GetNodeNumParms(pX) );
////        return false;
////    }
////	lmLUnits uWidth, uHeight;
////    wxString sValue = GetNodeName(pX->GetParameter(1));
////	if (GetFloatNumber(pNode, sValue, sName, &uWidth))
////        return false;
////    sValue = GetNodeName(pX->GetParameter(2));
////	if (GetFloatNumber(pNode, sValue, sName, &uHeight))
////        return false;
////    pScore->SetPageSize(uWidth, uHeight);
////    iP++;
////
////    //get page margins
////    pX = pNode->GetParameter(iP);
////    sName = GetNodeName(pX);
////    if (sName != _T("pageMargins"))
////    {
////        AnalysisError(pX, _T("Expected 'pageMargins' element but found '%s'. Ignored."),
////            sName.c_str() );
////		return false;
////    }
////    if(GetNodeNumParms(pX) != 5) {
////        AnalysisError(
////            pX,
////            _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////				_T("pageMargins") );
////        return false;
////    }
////	lmLUnits uLeft, uTop, uRight, uBottom, uBinding;
////    sValue = GetNodeName(pX->GetParameter(1));
////	if (GetFloatNumber(pNode, sValue, sName, &uLeft))
////        return false;
////    sValue = GetNodeName(pX->GetParameter(2));
////	if (GetFloatNumber(pNode, sValue, sName, &uTop))
////        return false;
////    sValue = GetNodeName(pX->GetParameter(3));
////	if (GetFloatNumber(pNode, sValue, sName, &uRight))
////        return false;
////    sValue = GetNodeName(pX->GetParameter(4));
////	if (GetFloatNumber(pNode, sValue, sName, &uBottom))
////        return false;
////    sValue = GetNodeName(pX->GetParameter(5));
////	if (GetFloatNumber(pNode, sValue, sName, &uBinding))
////        return false;
////    pScore->SetPageSize(uWidth, uHeight);
////    pScore->SetPageBindingMargin(uBinding);
////	pScore->SetPageBottomMargin(uBottom);
////	pScore->SetPageLeftMargin(uLeft);
////	pScore->SetPageRightMargin(uRight);
////	pScore->SetPageTopMargin(uTop);
////    iP++;
////
////    //get page orientation
////    pX = pNode->GetParameter(iP);
////    sName = GetNodeName(pNode->GetParameter(iP));
////    if (sName == _T("portrait"))
////		pScore->SetPageOrientation(true);
////    else if (sName == _T("landscape"))
////		pScore->SetPageOrientation(false);
////	else
////    {
////        AnalysisError(pNode, _T("Expected 'portrait' or 'landscape' but found '%s'. Ignored."),
////            sName.c_str() );
////		pScore->SetPageOrientation(true);
////    }
////
////	return true;
////}
////
////
////bool lmLDPParser::GetFloatNumber(lmLDPNode* pNode, wxString& sValue, wxString& nodeName,
////                                 float* pValue)
////{
////	//if error, returns true, sets pValue to 0.0f and issues an error message
////
////	double rNumberDouble;
////	if (!StrToDouble(sValue, &rNumberDouble))
////	{
////        *pValue = (float)rNumberDouble;
////		return false;
////	}
////    else
////	{
////        AnalysisError(pNode, _T("Element '%s': Invalid value '%s'. It must be a float number."),
////            nodeName.c_str(), sValue.c_str() );
////        *pValue = 0.0f;
////        return true;
////    }
////}
////
////bool lmLDPParser::GetValueFloatNumber(lmLDPNode* pNode, float* pValue, int iP, float rDefault)
////{
////	//if error, returns true, sets pValue to rDefault and issues an error message
////
////    wxString sValue = GetNodeName(pNode->GetParameter(iP));
////	double rNumberDouble;
////	if (!StrToDouble(sValue, &rNumberDouble))
////	{
////        *pValue = (float)rNumberDouble;
////		return false;
////	}
////    else
////	{
////        AnalysisError(pNode, _T("Element '%s': Invalid value '%s'. It must be a float number."),
////            GetNodeName(pNode).c_str(), sValue.c_str() );
////        *pValue = rDefault;
////        return true;
////    }
////}
////
////bool lmLDPParser::GetValueLongNumber(lmLDPNode* pNode, long* pValue, int iP, long nDefault)
////{
////	//if error, returns true, sets pValue to nDefault and issues an error message
////
////    wxString sValue = GetNodeName(pNode->GetParameter(iP));
////	long nNumberLong;
////	if (sValue.ToLong(&nNumberLong))
////	{
////        *pValue = nNumberLong;
////		return false;
////	}
////    else
////	{
////        AnalysisError(pNode, _T("Element '%s': Invalid value '%s'. It must be an integer number."),
////            GetNodeName(pNode).c_str(), sValue.c_str() );
////        *pValue = nDefault;
////        return true;
////    }
////}
////
////bool lmLDPParser::GetValueIntNumber(lmLDPNode* pNode, int* pValue, int iP, int nDefault)
////{
////	//if error, returns true, sets pValue to nDefault and issues an error message
////
////    wxString sValue = GetNodeName(pNode->GetParameter(iP));
////	long nNumberLong;
////	if (sValue.ToLong(&nNumberLong))
////	{
////        *pValue = (int)nNumberLong;
////		return false;
////	}
////    else
////	{
////        AnalysisError(pNode, _T("Element '%s': Invalid value '%s'. It must be an integer number."),
////            GetNodeName(pNode).c_str(), sValue.c_str() );
////        *pValue = nDefault;
////        return true;
////    }
////}
////
////
////
////void lmLDPParser::AnalyzeStaff(lmLDPNode* pNode, lmVStaff* pVStaff)
////{
////    //Modifies default staff values with those in the <staff> element
////    //
////    //    <staff> = (staff <num> [<staffType>][<staffLines>][<staffSpacing>]
////    //                     [<staffDistance>][<lineThickness>] )
////    //
////    //    <staffType> = { ossia | cue | editorial | regular | alternate }
////    //    <staffLines> = <num>
////    //    <staffSize> = <num>
////    //    <staffDistance> = <num>
////    //    <lineThickness> = <num>
////
////
////
////
////    wxString sElmName = GetNodeName(pNode);
////    wxASSERT(sElmName == _T("staff"));
////    long nId = GetNodeID(pNode);
////
////    //check that the staff number is specified
////    if(GetNodeNumParms(pNode) < 1)
////    {
////        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Ignored."),
////            sElmName.c_str());
////        return;
////    }
////
////    //get staff number
////    int iP = 1;
////    wxString sNum = GetNodeName(pNode->GetParameter(iP));
////    if (!sNum.IsNumber())
////    {
////        AnalysisError(pNode, _T("Element 'staff': staff number expected but found '%s'. Ignored."),
////                      sNum.c_str());
////        return;
////    }
////    long nStaffNum;
////    sNum.ToLong(&nStaffNum);
////    int nStaff = (int)nStaffNum;
////
////    //default values
////    wxString nStaffType = _T("regular");
////    int nStaffLines = 5;
////    lmLUnits uStaffSpacing = 0.0f;      //value 0 sets default spacing in constructor
////    lmLUnits uStaffDistance = 0.0f;     //value 0 sets default spacing in constructor
////    lmLUnits uLineThickness = 0.0f;     //value 0 sets default spacing in constructor
////
////    //get remaining optional parameters
////    ++iP;
////    lmLDPNode* pX;
////    for (; iP <= GetNodeNumParms(pNode); iP++)
////    {
////        pX = pNode->GetParameter(iP);
////
////        if (GetNodeName(pX) == _T("staffType") )
////        {
////            //TODO
////       }
////        else if (GetNodeName(pX) == _T("staffLines") )
////        {
////            GetValueIntNumber(pX, &nStaffLines);
////            if (nStaff == 1)
////                pVStaff->SetStaffNumLines(nStaff, nStaffLines);
////        }
////        else if (GetNodeName(pX) == _T("staffSpacing") )
////        {
////            GetValueFloatNumber(pX, &uStaffSpacing);
////            if (nStaff == 1)
////                pVStaff->SetStaffLineSpacing(nStaff, uStaffSpacing);
////        }
////        else if (GetNodeName(pX) == _T("staffDistance") )
////        {
////            GetValueFloatNumber(pX, &uStaffDistance);
////            if (nStaff == 1)
////                pVStaff->SetStaffDistance(nStaff, uStaffDistance);
////        }
////        else if (GetNodeName(pX) == _T("lineThickness") )
////        {
////            GetValueFloatNumber(pX, &uLineThickness);
////            if (nStaff == 1)
////                pVStaff->SetStaffLineThickness(nStaff, uLineThickness);
////        }
////        else
////        {
////            AnalysisError(pX, _T("[%s]: unknown element '%s' found. Element ignored."),
////                _T("staff"), GetNodeName(pX).c_str() );
////        }
////    }
////
////    //proceed to create the staff if not staff #1
////    if (nStaff > 1)
////        pVStaff->AddStaff(nStaffLines, nId, uStaffSpacing, uStaffDistance, uLineThickness);
////
////    return;
////}
////
////
////void lmLDPParser::AnalyzeGraphicObj(lmLDPNode* pNode, lmVStaff* pVStaff)
////{
////    //  <graphic> ::= ("graphic" <type> <params>*)
////    //
////    //AWARE: elemnet <graphic> is obsolete. No longer in use since v1.6
////    //Generating a <graphic> element no longer possible since v1.6. Therefore,
////    //for undo/redo these objects no longer exists.
////
////    wxString sElmName = GetNodeName(pNode);
////    int nNumParms = GetNodeNumParms(pNode);
////    long nId = GetNodeID(pNode);
////
////    //check that type is specified
////    if(nNumParms < 2)
////    {
////        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////            sElmName.c_str());
////        return;
////    }
////
////    // analyze type and get its params.
////    int iP = 1;
////    wxString sType = GetNodeName(pNode->GetParameter(iP));
////    if (sType == _T("line"))
////    {
////        // line
////        // Parms: xStart, yStart, xEnd, yEnd, nWidth, colour.
////        // All coordinates in tenths, relative to current pos.
////        // line width in tenths (optional parameter). Default: 1 tenth
////        // colour (optional parameter). Default: black
////
////        // get parameters
////        if(nNumParms < 5)
////        {
////            AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////                sElmName.c_str());
////            return;
////        }
////
////        lmTenths rPos[4];
////        long nPos;
////        wxString sNum;
////        for (iP=2; iP <= 5; iP++) {
////            sNum = GetNodeName(pNode->GetParameter(iP));
////            if (!sNum.IsNumber()) {
////                AnalysisError(
////                    pNode,
////                    _T("Element '%s': Coordinate expected but found '%s'. Ignored."),
////                    sElmName.c_str(), sNum.c_str());
////                return;
////            }
////            sNum.ToLong(&nPos);
////            rPos[iP-2] = (lmTenths)nPos;
////        }
////
////        // get line width (optional parameter). Default: 1 tenth
////        //TODO
////        lmTenths rWidth = 1;
////
////        // get colour (optional parameter). Default: black
////        //TODO
////        wxColour nColor = *wxBLACK;
////
////        // create the AuxObj and attach it to the VStaff
////        lmStaffObj* pAnchor = (lmStaffObj*) pVStaff->AddAnchorObj();
////        lmScoreLine* pLine
////            = new lmScoreLine(pAnchor, nId, rPos[0], rPos[1], rPos[2], rPos[3], rWidth,
////                              lm_eLineCap_Arrowhead, lm_eLineCap_None, lm_eLine_Solid, nColor);
////        pAnchor->AttachAuxObj(pLine);
////#if lmUSE_LIBRARY
////    pLine->SetLdpElement(pNode);
////#endif
////
////    }
////    else {
////        AnalysisError(
////            pNode,
////            _T("Element '%s': Type of graphic (%s) unknown. Ignored."),
////            sElmName.c_str(), sType.c_str());
////    }
////}
////
////void lmLDPParser::AnalyzeLine(lmLDPNode* pNode, lmVStaff* pVStaff, lmStaffObj* pTarget)
////{
////    //<line> = (line <startPoint><endPoint>[<width>][<startCap>][<endCap>][<lineStyle>][<color>])
////    //<startPoint> = (startPoint <location>)
////    //<endPoint> = (endPoint <location>)
////    //<startCap> = (lineCapStart value)
////    //<endCap> = (lineCapEnd value)
////
////    wxString sElmName = GetNodeName(pNode);
////    int nNumParms = GetNodeNumParms(pNode);
////    long nId = GetNodeID(pNode);
////
////    //check number of params.
////    if(nNumParms < 4)
////    {
////        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////            sElmName.c_str());
////        return;
////    }
////
////    //parameters and their default values
////	lmLocation tStartPos = g_tDefaultPos;
////	lmLocation tEndPos = g_tDefaultPos;
////    lmTenths ntWidth = 1.0f;
////    lmELineStyle nLineStyle = lm_eLine_Solid;
////    wxColour nColor = *wxBLACK;
////    lmELineCap nStartCap = lm_eLineCap_None;
////    lmELineCap nEndCap = lm_eLineCap_None;
////
////    //loop to analyze parameters
////    for(int iP=1; iP <= nNumParms; iP++)
////    {
////        lmLDPNode* pX = pNode->GetParameter(iP);
////        wxString sName = GetNodeName(pX);
////        if (sName == _T("startPoint"))
////            AnalyzeLocationPoint(pX, &tStartPos);
////        else if (sName == _T("endPoint"))
////            AnalyzeLocationPoint(pX, &tEndPos);
////        else if(sName == _T("width"))
////            GetValueFloatNumber(pX, &ntWidth);
////        else if(sName == _T("color"))
////            nColor = AnalyzeColor(pX);
////        else if(sName == _T("lineStyle"))
////            GetValueLineStyle(pX, &nLineStyle);
////        else if(sName == _T("lineCapStart"))
////            GetValueLineCap(pX, &nStartCap);
////        else if(sName == _T("lineCapEnd"))
////            GetValueLineCap(pX, &nEndCap);
////        else
////        {
////            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
////                          _T("line"), sName.c_str() );
////        }
////    }
////
////    // create the line and attach it to the anchor StaffObj
////    lmScoreLine* pLine = new lmScoreLine(pTarget, nId, tStartPos.x, tStartPos.y, tEndPos.x, tEndPos.y,
////                                         ntWidth, nStartCap, nEndCap, nLineStyle, nColor);
////#if lmUSE_LIBRARY
////    pLine->SetLdpElement(pNode);
////#endif
////    pTarget->AttachAuxObj(pLine);
////}
////
////void lmLDPParser::AnalyzeTextbox(lmLDPNode* pNode, lmVStaff* pVStaff,
////                                 lmStaffObj* pTarget)
////{
////    //<textbox> ::= (textbox <location>[<size>][<color>][<border>]<text>[<anchorLine>])
////
////    wxString sElmName = GetNodeName(pNode);
////    wxASSERT(sElmName == _T("textbox"));
////    int nNumParms = GetNodeNumParms(pNode);
////    long nId = GetNodeID(pNode);
////
////    //parameters and their default values
////        //box
////	lmLocation tPos = g_tDefaultPos;
////    lmTenths ntWidth = 160.0f;
////    lmTenths ntHeight = 100.0f;
////    wxColour nBgColor(255, 255, 255);
////        //text
////    lmFontInfo tFont = {m_sTextFontName, m_nTextFontSize, m_nTextStyle, m_nTextWeight};
////    wxString sText = _("Error in text!");
////    wxString sStyle = _T("");
////        //border
////    lmELineStyle nBorderStyle = lm_eLine_Solid;
////    wxColour nBorderColor = *wxBLACK;
////    lmTenths ntBorderWidth = 1.0f;
////        //anchor line
////    bool fAnchorLine = false;       //assume no anchor line
////	lmLocation tAnchorPoint = g_tDefaultPos;
////    lmELineStyle nAnchorLineStyle = lm_eLine_Solid;
////    lmELineCap nAnchorLineEndStyle = lm_eLineCap_None;
////    wxColour nAnchorLineColor = *wxBLACK;
////    lmTenths ntAnchorLineWidth = 1.0f;
////
////    //loop to analyze parameters. Optional: color, border, line
////    for(int iP=1; iP <= GetNodeNumParms(pNode); iP++)
////    {
////        lmLDPNode* pX = pNode->GetParameter(iP);
////        wxString sName = GetNodeName(pX);
////        if (sName == _T("dx") || sName == _T("dy"))
////        {
////            AnalyzeLocation(pX, &tPos);
////        }
////        else if(sName == _T("size"))
////        {
////            AnalyzeSize(pX, &ntWidth, &ntHeight);
////        }
////        else if(sName == _T("color"))
////        {
////            nBgColor = AnalyzeColor(pX);
////        }
////        else if(sName == _T("border"))
////        {
////            AnalyzeBorder(pX, &ntBorderWidth, &nBorderStyle, &nBorderColor);
////        }
////        else if(sName == _T("text"))
////        {
////            //(text string [<location>] [{<font> | <style>}] [<alignment>])
////            //mandatory: string. Optional: style. All others forbidden
////            if (AnalyzeTextString(pX, &sText, &sStyle, (lmEHAlign*)NULL,
////                                  (lmLocation*)NULL, (lmFontInfo*)NULL))
////            {
////                //error in text element
////                //TODO
////            }
////        }
////        else if(sName == _T("anchorLine"))
////        {
////            AnalyzeAnchorLine(pX, &tAnchorPoint, &ntAnchorLineWidth, &nAnchorLineStyle,
////                              &nAnchorLineEndStyle, &nAnchorLineColor);
////            fAnchorLine = true;
////        }
////        else
////        {
////            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
////                          _T("textbox"), sName.c_str() );
////        }
////    }
////
////    // create the AuxObj and attach it to the anchor StaffObj
////    lmTPoint ntPos(tPos.x, tPos.y);
////    lmScoreTextBox* pSTP =
////        new lmScoreTextBox(pTarget, nId, ntWidth, ntHeight, ntPos);
////    pTarget->AttachAuxObj(pSTP);
////#if lmUSE_LIBRARY
////    pSTP->SetLdpElement(pNode);
////#endif
////
////    //apply values to created lmScoreTextBox
////
////    //background colour
////    pSTP->SetBgColour(nBgColor);
////
////    //border
////    pSTP->SetBorderWidth(ntBorderWidth);
////    pSTP->SetBorderColor(nBorderColor);
////    pSTP->SetBorderStyle(nBorderStyle);
////
////    //anchor line
////    if (fAnchorLine)
////        pSTP->AddAnchorLine(tAnchorPoint, ntAnchorLineWidth, nAnchorLineStyle,
////                            nAnchorLineEndStyle, nAnchorLineColor);
////
////    //text
////    lmTextStyle* pStyle = GetTextStyle(pNode, sStyle);
////    lmBaseText* pBText = new lmBaseText(sText, pStyle);
////    pSTP->InsertTextUnit(pBText);
////}
////
////bool lmLDPParser::AnalyzeBorder(lmLDPNode* pNode, lmTenths* ptWidth,
////                                lmELineStyle* pLineStyle, wxColour* pColor)
////{
////    //returns true if error
////    //<border> ::= (border <width><lineStyle><color>)
////
////    wxString sElmName = GetNodeName(pNode);
////    wxASSERT(sElmName == _T("border"));
////    int nNumParms = GetNodeNumParms(pNode);
////
////    //parameters and their default values
////    wxColour nColor(0, 0, 0);           //default: black
////    lmTenths ntWidth = 1.0f;            //default: 1 tenth
////    lmELineStyle nLineStyle = lm_eLine_Solid;
////
////    //load default values
////    *ptWidth = ntWidth;
////    *pLineStyle = nLineStyle;
////    *pColor = nColor;
////
////    //check that type is specified
////    if(nNumParms < 3)
////    {
////        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////                      _T("border") );
////        return true;    //error
////    }
////
////    //loop to analyze parameters: width & height
////    for(int iP=1; iP <= GetNodeNumParms(pNode); iP++)
////    {
////        lmLDPNode* pX = pNode->GetParameter(iP);
////        wxString sName = GetNodeName(pX);
////        if (sName == _T("width"))
////            GetValueFloatNumber(pX, ptWidth);
////        else if (sName == _T("color"))
////            nColor = AnalyzeColor(pX);
////        else if (sName == _T("lineStyle"))
////            GetValueLineStyle(pX, &nLineStyle);
////        else
////            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
////                          _T("size"), sName.c_str() );
////    }
////
////    //return parsed values
////    *ptWidth = ntWidth;
////    *pLineStyle = nLineStyle;
////    *pColor = nColor;
////
////    return false;    //no error
////}
////
////bool lmLDPParser::GetValueLineStyle(lmLDPNode* pNode, lmELineStyle* pLineStyle)
////{
////	//if error, returns true, sets pLineStyle to lm_eLine_Solid and issues an error message
////    //<lineStyle> = (lineStyle { none | solid | longDash | shortDash | dot | dotDash } )
////
////    wxString sValue = GetNodeName(pNode->GetParameter(1));
////    if (sValue == _T("none"))
////        *pLineStyle = lm_eLine_None;
////    else if (sValue == _T("solid"))
////        *pLineStyle = lm_eLine_Solid;
////    else if (sValue == _T("longDash"))
////        *pLineStyle = lm_eLine_LongDash;
////    else if (sValue == _T("shortDash"))
////        *pLineStyle = lm_eLine_ShortDash;
////    else if (sValue == _T("dot"))
////        *pLineStyle = lm_eLine_Dot;
////    else if (sValue == _T("dotDash"))
////        *pLineStyle = lm_eLine_DotDash;
////    else
////	{
////        AnalysisError(pNode, _T("Element 'lineStyle': Invalid value '%s'. Replaced by 'solid'"));
////        *pLineStyle = lm_eLine_Solid;
////        return true;
////    }
////    return false;       //no error
////}
////
////bool lmLDPParser::GetValueLineCap(lmLDPNode* pNode, lmELineCap* pEndCap)
////{
////	//if error, returns true, sets pEndCap to lm_eLineCap_None and issues an error message
////    //{ none | arrowhead | arrowtail | circle | square | diamond }
////
////    wxString sValue = GetNodeName(pNode->GetParameter(1));
////    if (sValue == _T("none"))
////        *pEndCap = lm_eLineCap_None;
////    else if (sValue == _T("arrowhead"))
////        *pEndCap = lm_eLineCap_Arrowhead;
////    else if (sValue == _T("arrowtail"))
////        *pEndCap = lm_eLineCap_Arrowtail;
////    else if (sValue == _T("circle"))
////        *pEndCap = lm_eLineCap_Circle;
////    else if (sValue == _T("square"))
////        *pEndCap = lm_eLineCap_Square;
////    else if (sValue == _T("diamond"))
////        *pEndCap = lm_eLineCap_Diamond;
////    else
////	{
////        AnalysisError(pNode, _T("Element 'lineCap': Invalid value '%s'. Replaced by 'none'"),
////            sValue.c_str() );
////        *pEndCap = lm_eLineCap_None;
////        return true;
////    }
////    return false;       //no error
////}
////
////bool lmLDPParser::AnalyzeSize(lmLDPNode* pNode, lmTenths* ptWidth, lmTenths* ptHeight)
////{
////    //returns true if error
////    //<size> ::= (size <width><height>)
////    //<width> ::= (width num)
////    //<height> ::= (height num)
////
////    wxString sElmName = GetNodeName(pNode);
////    wxASSERT(sElmName == _T("size"));
////    int nNumParms = GetNodeNumParms(pNode);
////
////    //check that it has two more parameters
////    if(nNumParms != 2)
////    {
////        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Element ignored."),
////                      _T("size"));
////        return true;    //error
////    }
////
////    //loop to analyze parameters: width & height
////    for(int iP=1; iP <= GetNodeNumParms(pNode); iP++)
////    {
////        lmLDPNode* pX = pNode->GetParameter(iP);
////        wxString sName = GetNodeName(pX);
////        if (sName == _T("width"))
////            GetValueFloatNumber(pX, ptWidth);
////        else if (sName == _T("height"))
////            GetValueFloatNumber(pX, ptHeight);
////        else
////            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
////                          _T("size"), sName.c_str() );
////    }
////    return false;    //no error
////}
////
////void lmLDPParser::AnalyzeAnchorLine(lmLDPNode* pNode, lmLocation* ptPos, lmTenths* ptWidth,
////                                    lmELineStyle* pLineStyle, lmELineCap* pEndCap,
////                                    wxColour* pColor)
////{
////    //Received parameters must be initialized with default values
////    //<anchorLine> = (anchorLine <destination-point>[<width>][<lineStyle>][<color>]
////    //                           [<lineCapEnd>])
////    //<destination-point> = <location>
////
////    wxString sElmName = GetNodeName(pNode);
////    wxASSERT(sElmName == _T("anchorLine"));
////    int nNumParms = GetNodeNumParms(pNode);
////
////    //loop to analyze parameters
////    for(int iP=1; iP <= GetNodeNumParms(pNode); iP++)
////    {
////        lmLDPNode* pX = pNode->GetParameter(iP);
////        wxString sName = GetNodeName(pX);
////        if (sName == _T("dx") || sName == _T("dy"))
////            AnalyzeLocation(pX, ptPos);
////        else if(sName == _T("width"))
////            GetValueFloatNumber(pX, ptWidth);
////        else if(sName == _T("color"))
////            *pColor = AnalyzeColor(pX);
////        else if(sName == _T("lineStyle"))
////            GetValueLineStyle(pX, pLineStyle);
////        else if(sName == _T("lineCapEnd"))
////            GetValueLineCap(pX, pEndCap);
////        else
////        {
////            AnalysisError(pX, _T("[Element '%s'. Invalid parameter '%s'. Ignored."),
////                          _T("anchorLine"), sName.c_str() );
////        }
////    }
////}
////
////lmTextStyle* lmLDPParser::GetTextStyle(lmLDPNode* pNode, const wxString& sStyle)
////{
////    //Returns style for that style name or style for normal text if style name is not
////    //defined in the score
////
////    lmTextStyle* pStyle = (lmTextStyle*)NULL;
////    if (sStyle != _T(""))
////    {
////        pStyle = m_pScore->GetStyleInfo(sStyle);
////        if (!pStyle)
////            AnalysisError(pNode, _T("Style '%s' is not defined. Default style will be used."),
////                          sStyle.c_str());
////    }
////    if (!pStyle)
////        pStyle = m_pScore->GetStyleInfo(_("Normal text"));
////
////    return pStyle;
////}
////
////lmEStemType lmLDPParser::AnalyzeStem(lmLDPNode* pNode, lmVStaff* pVStaff)
////{
////    //<Stem> ::= (stem [up | down] <lenght> }
////
////    wxASSERT(GetNodeName(pNode) == _T("stem"));
////
////    lmEStemType nStem = lmSTEM_DEFAULT;
////
////    //check that there are parameters
////    if(GetNodeNumParms(pNode) < 1) {
////        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Tag ignored. Assumed default stem."),
////            _T("stem"));
////        return nStem;
////    }
////
////    //get stem direction
////    wxString sDir = GetNodeName(pNode->GetParameter(1));
////    if (sDir == _T("up"))
////        nStem = lmSTEM_UP;
////    else if (sDir == _T("down"))
////        nStem = lmSTEM_DOWN;
////    else {
////        AnalysisError(pNode, _T("Invalid stem direction '%s'. Default direction taken."), sDir.c_str());
////    }
////
////    return nStem;
////
////}
////
////lmEPlacement lmLDPParser::AnalyzeFermata(lmLDPNode* pNode, lmVStaff* pVStaff,
////                                         lmLocation* pPos)
////{
////    //<Fermata> ::= (fermata [above | below]}
////
////    wxASSERT(GetNodeName(pNode) == _T("fermata"));
////
////    lmEPlacement nPlacement = ep_Default;
////
////    //check that there are parameters
////    if(GetNodeNumParms(pNode) < 1) {
////        AnalysisError(pNode,_T("Element '%s' has less parameters than the minimum required. Tag ignored. Assumed default stem."),
////            _T("fermata") );
////        return nPlacement;
////    }
////
////    //get fermata placement
////    wxString sDir = GetNodeName(pNode->GetParameter(1));
////    if (sDir == _T("above"))
////        nPlacement = ep_Above;
////    else if (sDir == _T("below"))
////        nPlacement = ep_Below;
////    else {
////        AnalysisError(pNode, _T("Invalid fermata placement '%s'. Default placement assumed."), sDir.c_str());
////    }
////
////    //analyze optional parameters
////	lmLDPOptionalTags oOptTags(this);
////	oOptTags.SetValid(lm_eTag_Location_x, lm_eTag_Location_y, -1);		//finish list with -1
////	oOptTags.AnalyzeCommonOptions(pNode, 2, pVStaff, NULL, NULL, pPos);
////
////    return nPlacement;
////}
////
////void lmLDPParser::AnalyzeFont(lmLDPNode* pNode, lmFontInfo* pFont)
////{
////    // <font> = (font <name> <size> <style>)
////
////    //returns, in variables pointed by pFontName, pFontSize and pStyleDevuelve the
////    //result of the analysis. No default values are returned, only the real values
////    //found. Any defaults must be set before invoking this method
////
////    wxASSERT(GetNodeName(pNode) == _T("font"));
////
////    //check that there are parameters
////    if (!(GetNodeNumParms(pNode) > 0)) {
////        AnalysisError(pNode, _T("Element '%s' has less parameters than the minimum required. Tag ignored."),
////            GetNodeName(pNode).c_str() );
////    }
////
////    //flags to control that the corresponding parameter has been processed
////    bool fName = false;
////    bool fSize = false;
////    bool fStyle = false;
////
////    //get parameters. The come in any order
////    lmFontInfo tFont = *pFont;
////    int iP;
////    wxString sParm;
////
////    bool fProcessed;
////    for(iP=1; iP <= GetNodeNumParms(pNode); iP++)
////    {
////        sParm = GetNodeName(pNode->GetParameter(iP));
////        fProcessed = false;
////
////        if (!fStyle) {
////            //try style and weight
////            fStyle = true;
////            fProcessed = true;
////            if (sParm == _T("bold"))
////            {
////                tFont.nFontStyle = wxFONTSTYLE_NORMAL;
////                tFont.nFontWeight = wxFONTWEIGHT_BOLD;
////            }
////            else if (sParm == _T("normal"))
////            {
////                tFont.nFontStyle = wxFONTSTYLE_NORMAL;
////                tFont.nFontWeight = wxFONTWEIGHT_NORMAL;
////            }
////            else if (sParm == _T("italic"))
////            {
////                tFont.nFontStyle = wxFONTSTYLE_ITALIC;
////                tFont.nFontWeight = wxFONTWEIGHT_NORMAL;
////            }
////            else if (sParm == _T("bold-italic"))
////            {
////                tFont.nFontStyle = wxFONTSTYLE_ITALIC;
////                tFont.nFontWeight = wxFONTWEIGHT_BOLD;
////            }
////            else {
////                fStyle = false;
////                fProcessed = false;
////            }
////        }
////
////        if (!fSize && !fProcessed) {
////            wxString sSize = sParm;
////            if (sParm.length() > 2 && sParm.Right(2) == _T("pt")) {
////                sSize = sParm.Left(sParm.length() - 2);
////            }
////            if (sSize.IsNumber()) {
////                long nSize;
////                sSize.ToLong(&nSize);
////                tFont.nFontSize = (int)nSize;
////                fSize = true;
////                fProcessed = true;
////            }
////        }
////
////        if (!fName && !fProcessed) {
////            //assume it is the name
////            fName = true;
////            tFont.sFontName = GetNodeName(pNode->GetParameter(iP));
////            fProcessed = true;
////        }
////
////        if (!fProcessed) {
////            AnalysisError(pNode, _T("Element '%s': invalid parameter '%s'. It is ignored."),
////                _T("font"), sParm.c_str() );
////        }
////    }
////
////    *pFont = tFont;
////
////}
////
////void lmLDPParser::AnalyzeLocation(lmLDPNode* pNode, float* pValue, lmEUnits* pUnits)
////{
////    // <location> = { (dx num) | (dy num) }
////    // <num> = number [units]
////
////    //returns, in variables pointed by pValue and pUnits the
////    //result of the analysis.
////
////    wxString sElement = GetNodeName(pNode);
////
////    //check that there are parameters
////    if (GetNodeNumParms(pNode)!= 1) {
////        AnalysisError(pNode, _T("Element '%s' has less or more parameters than required. Tag ignored."),
////            sElement.c_str() );
////        return;
////    }
////
////    //get value
////    wxString sParm = GetNodeName(pNode->GetParameter(1));
////    wxString sValue = sParm;
////    wxString sUnits = sParm.Right(2);
////	if (sUnits.at(0) != _T('.') && !sUnits.IsNumber() )
////	{
////        AnalysisError(pNode, _T("Element '%s' has units '%s'. Units no longer supported. Ignored"),
////            sElement.c_str(), sUnits.c_str() );
////    }
////
////    GetFloatNumber(pNode, sValue, sElement, pValue);
////
////}
////
////void lmLDPParser::AnalyzeLocation(lmLDPNode* pNode, lmLocation* pPos)
////{
////    //analyze location
////    wxString sName = GetNodeName(pNode);
////
////    wxASSERT(sName == _T("dx") || sName == _T("dy") );
////
////    float rValue;
////    lmEUnits nUnits = lmTENTHS;     //default value
////    AnalyzeLocation(pNode, &rValue, &nUnits);
////    if (sName == _T("dx"))
////    {
////        //dx
////        pPos->x = rValue;
////        pPos->xUnits = nUnits;
////    }
////    else {
////        //dy
////        pPos->y = rValue;
////        pPos->yUnits = nUnits;
////    }
////
////}
////
////void lmLDPParser::AnalyzeLocationPoint(lmLDPNode* pNode, lmLocation* pPos)
////{
////    //analyze location point
////    //i.e.: (startPoint <location>)
////    //      (endPoint <location>)
////
////    wxString sElmName = GetNodeName(pNode);       //i.e.: startPoint, endPoint, etc.
////    int nNumParms = GetNodeNumParms(pNode);
////
////    //loop to analyze parameters
////    for(int iP=1; iP <= nNumParms; iP++)
////    {
////        float rValue;
////        lmEUnits nUnits = lmTENTHS;     //default value
////        lmLDPNode* pX = pNode->GetParameter(iP);
////        wxString sName = GetNodeName(pX);
////        AnalyzeLocation(pX, &rValue, &nUnits);
////        if (sName == _T("dx"))
////        {
////            //dx
////            pPos->x = rValue;
////            pPos->xUnits = nUnits;
////        }
////        else {
////            //dy
////            pPos->y = rValue;
////            pPos->yUnits = nUnits;
////        }
////    }
////}
////
////////Devuelve true si hay error, es decir si no añade objeto al pentagrama
//////Function AnalizarDirectivaRepeticion(lmVStaff* pVStaff, lmLDPNode* pNode) As Boolean
////////<repeticion> = (repeticion <valor> <posicion> )
////////<valor> =
////////        "dacapo" |
////////        "segno" | ("segno" num) |
////////        "dalsegno" | ("dalsegno" num) |
////////        "coda" | ("coda" num) |
////////        "tocoda" | ("tocoda" num) |
////////        "fine"
//////
//////    wxASSERT(GetNodeName(pNode) = "REPETICION"
//////    wxASSERT(GetNodeNumParms(pNode) = 2
//////
//////    Dim lmLDPNode* pX, long iP
//////    Dim nNum As Long, sDuration As String, nType As EDirectivasRepeticion
//////    Dim sNum As String
//////
//////    //obtiene tipo de repeticion
//////    Set pX = pNode->GetParameter(1)
//////    sDuration = UCase$(GetNodeName(pX))
//////    if (pX->IsSimple()) {
//////        nNum = 1
//////    } else {
//////        sNum = pX->GetParameter(1).GetName();
//////        if (Not IsNumeric(sNum)) {
//////            AnalysisError(pX, wxString::Format(_T("[AnalizarDirectivaRepeticion]: Valor <" & sNum & _
//////                "> para la directiva de repetición <" & _
//////                GetNodeName(pX) & "> no es numérico. Se ignora este elemento."
//////            AnalizarDirectivaRepeticion = true
//////            Exit Function
//////        }
//////        nNum = CLng(sNum)
//////    }
//////
//////    switch (sDuration
//////        case "DACAPO"
//////            nType = eDR_DaCapo
//////        case "DC"
//////            nType = eDR_DC
//////        case "SEGNO"
//////            nType = eDR_Segno
//////        case "DALSEGNO"
//////            nType = eDR_DalSegno
//////        case "DS"
//////            nType = eDR_DS
//////        case "CODA"
//////            nType = eDR_Coda
//////        case "ALCODA"
//////            nType = eDR_AlCoda
//////        case "FINE"
//////            nType = eDR_Fine
//////        case "ALFINE"
//////            nType = eDR_AlFine
//////        case "REPETICION"
//////            nType = eDR_Repeticion
//////        default:
//////            AnalysisError(pNode, wxString::Format(_T("Signo de repetición <" & sDuration & "> desconocido. " & _
//////                "Se ignora elemento."
//////            AnalizarDirectivaRepeticion = true
//////            Exit Function
//////    }
//////
//////    //obtiene posicion
//////    iP = 2
//////    Set pX = pNode->GetParameter(iP)
//////    Dim nX As Long, nY As Long, fXAbs As Boolean, fYAbs As Boolean
//////    AnalizarPosicion pX, nX, nY, fXAbs, fYAbs
//////
//////    //crea el pentobj
//////    pVStaff.AddDirectivaRepeticion nType, nNum, nX, nY, fXAbs, fYAbs
//////
//////    AnalizarDirectivaRepeticion = false       //no hay error
//////
//////}
////
////int lmLDPParser::AnalyzeNumStaff(const wxString& sNotation, lmLDPNode* pNode, long nNumStaves)
////{
////    //analyzes a notation Pxx.  xx must be lower or equal than nNumStaves
////
////    if (sNotation.Left(1) != _T("p"))
////    {
////        AnalysisError(pNode, _T("Staff number expected but found '%s'. Replaced by 'p1'"),
////            sNotation.c_str() );
////        return 1;
////    }
////
////    wxString sData = sNotation.substr(1);         //remove char 'p'
////    if (!sData.IsNumber()) {
////        AnalysisError(pNode, _T("Staff number not followed by number (%s). Replaced by 'p1'"),
////            sNotation.c_str() );
////        return 1;
////    }
////
////    long nValue;
////    sData.ToLong(&nValue);
////    if (nValue > nNumStaves) {
////        AnalysisError(pNode, _T("Notation '%s': number is greater than number of staves defined (%d). Replaced by 'p1'."),
////            sNotation.c_str(), nNumStaves );
////        return 1;
////    }
////    return (int)nValue;
////
////}
////
////int lmLDPParser::AnalyzeVoiceNumber(const wxString& sNotation, lmLDPNode* pNode)
////{
////    //analyzes a notation Vx.  x must be 1..lmMAX_VOICE
////
////    if (sNotation.Left(1) != _T("v")) {
////        AnalysisError(pNode, _T("Voice number expected but found '%s'. Replaced by 'v1'"),
////            sNotation.c_str() );
////        return 1;
////    }
////
////    wxString sData = sNotation.substr(1);         //remove char 'v'
////    if (!sData.IsNumber()) {
////        AnalysisError(pNode, _T("Voice number expected but found '%s'. Replaced by 'v1'"),
////            sNotation.c_str() );
////        return 1;
////    }
////
////    long nValue;
////    sData.ToLong(&nValue);
////    if (nValue >= lmMAX_VOICE) {
////        AnalysisError(pNode, _T("Notation '%s': number is greater than supported voices (%d). Replaced by 'v1'."),
////            sNotation.c_str(), lmMAX_VOICE );
////        return 1;
////    }
////    return (int)nValue;
////
////}
////
////float lmLDPParser::GetDefaultDuration(lmENoteType nNoteType, int nDots, int nActualNotes,
////                                    int nNormalNotes)
////{
////    //compute duration without modifiers
////    float rDuration = NoteTypeToDuration(nNoteType, nDots);
////
////    //alter by tuplet modifiers
////    if (nActualNotes != 0) rDuration = (rDuration * (float)nNormalNotes) / (float)nActualNotes;
////
////    return rDuration;
////}
////
////int lmLDPParser::GetBeamingLevel(lmENoteType nNoteType)
////{
////    switch(nNoteType) {
////        case eEighth:
////            return 0;
////        case e16th:
////            return 1;
////        case e32th:
////            return 2;
////        case e64th:
////            return 3;
////        case e128th:
////            return 4;
////        case e256th:
////            return 5;
////        default:
////            return -1; //Error: Requesting beaming a note longer than eight
////    }
////}
////
////bool lmLDPParser::AnalyzeNoteType(wxString& sNoteType, lmENoteType* pnNoteType,
////                                  int* pNumDots)
////{
////    // Receives a string (sNoteType) with the LDP letter for the type of note and, optionally,
////    // dots "."
////    // Set up variables nNoteType and pNumDots.
////    //
////    //  USA           UK                      ESP               LDP     NoteType
////    //  -----------   --------------------    -------------     ---     ---------
////    //  long          longa                   longa             l       eLonga = 0
////    //  double whole  breve                   cuadrada, breve   d       eBreve = 1
////    //  whole         semibreve               redonda           r       eWhole = 2
////    //  half          minim                   blanca            b       eHalf = 3
////    //  quarter       crochet                 negra             n       eQuarter = 4
////    //  eighth        quaver                  corchea           c       eEighth = 5
////    //  sixteenth     semiquaver              semicorchea       s       e16th = 6
////    //  32nd          demisemiquaver          fusa              f       e32th = 7
////    //  64th          hemidemisemiquaver      semifusa          m       e64th = 8
////    //  128th         semihemidemisemiquaver  garrapatea        g       e128th = 9
////    //  256th         ???                     semigarrapatea    p       e256th = 10
////    //
////    // Returns true if error in parsing
////
////
////    sNoteType.Trim(false);      //remove spaces from left
////    sNoteType.Trim(true);       //and from right
////
////    //locate dots, if exist, and extract note type string
////    wxString sType;
////    wxString sDots;
////    int iDot = sNoteType.Find(_T("."));
////    if (iDot != -1) {
////        sType = sNoteType.Left(iDot);
////        sDots = sNoteType.substr(iDot);
////    }
////    else {
////        sType = sNoteType;
////        sDots = _T("");
////    }
////
////    //identify note type
////    if (sType.Left(1) == _T("'")) {
////        // numeric duration: '1, '2, '4, '8, '16, '32, ..., '256
////        sType = sType.substr(1);
////        if (!sType.IsNumber()) return true;     //error
////        long nType;
////        sType.ToLong(&nType);
////        switch(nType) {
////            case 1:     *pnNoteType = eWhole;       break;
////            case 2:     *pnNoteType = eHalf;        break;
////            case 4:     *pnNoteType = eQuarter;     break;
////            case 8:     *pnNoteType = eEighth;      break;
////            case 16:    *pnNoteType = e16th;        break;
////            case 32:    *pnNoteType = e32th;        break;
////            case 64:    *pnNoteType = e64th;        break;
////            case 128:   *pnNoteType = e128th;       break;
////            case 256:   *pnNoteType = e256th;       break;
////            default:
////                return true;    //error
////        }
////    }
////    // duration as a letter
////    else if (sType == _T("l"))
////        *pnNoteType = eLonga;
////    else if (sType == _T("d"))
////        *pnNoteType = eBreve;
////    else if (sType == _T("w"))
////        *pnNoteType = eWhole;
////    else if (sType == _T("h"))
////        *pnNoteType = eHalf;
////    else if (sType == _T("q"))
////        *pnNoteType = eQuarter;
////    else if (sType == _T("e"))
////        *pnNoteType = eEighth;
////    else if (sType == _T("s"))
////        *pnNoteType = e16th;
////    else if (sType == _T("t"))
////        *pnNoteType = e32th;
////    else if (sType == _T("i"))
////        *pnNoteType = e64th;
////    else if (sType == _T("o"))
////        *pnNoteType = e128th;
////    else if (sType == _T("f"))
////        *pnNoteType = e256th;
////    else
////        return true;    //error
////
////    //analyze dots
////    *pNumDots = 0;
////    if (sDots.length() > 0) {
////        if (sDots.StartsWith( _T("....") ))
////            *pNumDots = 4;
////        else if (sDots.StartsWith( _T("...") ))
////            *pNumDots = 3;
////        else if (sDots.StartsWith( _T("..") ))
////            *pNumDots = 2;
////        else if (sDots.StartsWith( _T(".") ))
////            *pNumDots = 1;
////        else
////            return true;    //error
////    }
////
////    return false;   //no error
////
////}
////
////
////
//////-----------------------------------------------------------------------------------------
////// lmLDPOptionalTags implementation: Helper class to analyze optional elements
//////-----------------------------------------------------------------------------------------
////
////lmLDPOptionalTags::lmLDPOptionalTags(lmLDPParser* pParser)
////{
////	m_pParser = pParser;
////
////	//no tag valid, for now
////	m_ValidTags.reserve(lm_eTag_Max);
////	m_ValidTags.assign(lm_eTag_Max, false);
////}
////
////lmLDPOptionalTags::~lmLDPOptionalTags()
////{
////}
////
////void lmLDPOptionalTags::SetValid(lmETagLDP nTag, ...)
////{
////	//process optional tags. Finish list with -1
////
////	//process first arg
////	m_ValidTags[nTag] = true;
////
////	//process additional args
////	va_list pArgs;
////	// va_start is a macro which accepts two arguments, a va_list and the name of the
////	// variable that directly precedes the ellipsis (...).
////	va_start(pArgs, nTag);     // initialize the list to point to first variable argument
////	while(true)
////	{
////		// va_arg takes a va_list and a variable type, and returns the next argument
////		// in the list in the form of whatever variable type it is told. It then moves
////		// down the list to the next argument.
////		int nNextTag = va_arg(pArgs, int);
////		if (nNextTag == -1) break;
////		m_ValidTags[nNextTag] = true;
////	}
////	va_end(pArgs);		//clean up the list
////}
////
////bool lmLDPOptionalTags::VerifyAllowed(lmETagLDP nTag, wxString sName, lmLDPNode* pNode)
////{
////	if (m_ValidTags[nTag]) return true;
////
////	//tag invalid. Log error message
////    m_pParser->AnalysisError(
////                pNode,
////				_T("[AnalyzeCommonOptions]: Not allowed element '%s' found. Element ignored."),
////                sName.c_str() );
////	return false;
////
////}
////
////void lmLDPOptionalTags::AnalyzeCommonOptions(lmLDPNode* pNode, int iP, lmVStaff* pVStaff,
////									   // variables to return optional values
////									   bool* pfVisible,
////									   int* pStaffNum,
////									   lmLocation* pLocation
////									   )
////{
////    //analyze optional parameters
////	//if the optional tag is valid fills corresponding received variables
////	//if tag is not allowed, ignore it and continue with the next option
////
////	for(; iP <= GetNodeNumParms(pNode); iP++)
////	{
////        lmLDPNode* pX = pNode->GetParameter(iP);
////        const wxString sName = GetNodeName(pX);
////
////		//number of staff on which the element is located
////        if (pX->IsSimple() && sName.Left(1) == _T("p"))
////        {
////			if (VerifyAllowed(lm_eTag_StaffNum, sName, pNode)) {
////				*pStaffNum = m_pParser->AnalyzeNumStaff(sName, pNode, pVStaff->GetNumStaves());
////			}
////            pX->SetProcessed(true);
////        }
////
////		//visible or not
////        else if (sName == _T("noVisible"))
////		{
////			if (VerifyAllowed(lm_eTag_Visible, sName, pNode)) {
////				*pfVisible = false;
////			}
////            pX->SetProcessed(true);
////        }
////
////		// X location
////        else if (sName == _T("dx"))
////        {
////			if (VerifyAllowed(lm_eTag_Location_x, sName, pNode)) {
////				m_pParser->AnalyzeLocation(pX, pLocation);
////			}
////            pX->SetProcessed(true);
////		}
////
////		// Y location
////        else if (sName == _T("dy"))
////        {
////			if (VerifyAllowed(lm_eTag_Location_y, sName, pNode)) {
////				m_pParser->AnalyzeLocation(pX, pLocation);
////			}
////            pX->SetProcessed(true);
////		}
////
////		// Octave shift
////		else if (sName == _T("-8va") || sName == _T("+8va")
////                    || sName == _T("+15ma") || sName == _T("-15ma") )
////        {
////            //TODO tessiture option in clef
////            pX->SetProcessed(true);
////        }
////
////        //else
////			// Unknown tag. Ignore it
////    }
////
////}

