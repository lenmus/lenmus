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

#include "lenmus_factory.h"
#include "lenmus_tree.h"
#include "lenmus_syntax_validator.h"
#include "lenmus_parser.h"
#include "lenmus_values.h"

using namespace std;

namespace lenmus
{

SyntaxValidator::SyntaxValidator(LdpTree* tree, ostream& reporter)
    : m_reporter(reporter)
    , m_pTree(tree)
{
}

bool SyntaxValidator::has(LdpTree::iterator itNode, int* iP, ELdpElements type)
{
    LdpElement* node = *itNode;
    if (*iP > node->get_num_parameters())
        return error_missing_element(node, type);

    LdpElement* child = node->get_parameter(*iP);
    if (child->get_type() != type)
        return error_missing_element(node, type);

    (*iP)++;
    if (child->is_simple())
        return true;
    else
    {
        LdpTree::iterator it(child);
        return validate(it);
    }
}

bool SyntaxValidator::optional(LdpTree::iterator itNode, int* iP, ELdpElements type)
{
    LdpElement* node = *itNode;
    if (!more_parameters_to_process(node, *iP))
        return true;
    //if (*iP > node->get_num_parameters())
    //    return true;

    LdpElement* child = node->get_parameter(*iP);
    if (child->get_type() != type)
        return true;

    (*iP)++;
    LdpTree::iterator itChild(child);
    return validate(itChild);
}

bool SyntaxValidator::zero_or_more(LdpTree::iterator itNode, int* iP, ELdpElements type)
{
    LdpElement* node = *itNode;
    if (*iP > node->get_num_parameters())
        return true;

    LdpElement* child = node->get_parameter(*iP);
    if (child->get_type() != type)
        return true;

    //at least one occurrence
    (*iP)++;
    LdpTree::iterator itChild(child);
    if (validate(itChild))
        return zero_or_more(itNode, iP, type);
    else
        return false;
}

bool SyntaxValidator::one_or_more(LdpTree::iterator itNode, int* iP, ELdpElements type)
{
    //one occurrence is mandatory
    if (!has(itNode, iP, type))
        return false;

    //now additional optional occurrences
    return zero_or_more(itNode, iP, type);
}

bool SyntaxValidator::no_more(LdpTree::iterator itNode, int* iP)
{
    LdpElement* node = *itNode;
    if (*iP > node->get_num_parameters())
        return true;
    report_msg(node->get_line_number(),
               node->get_name() + ": too many parameters. Extra parameters removed.");
    while (*iP <= node->get_num_parameters())
    {
        LdpElement* child = node->get_parameter(*iP);
        (*iP)++;
        LdpTree::iterator it(child);
        m_pTree->erase(it);
    }
    return true;
}

bool SyntaxValidator::has_number(LdpTree::iterator itNode, int* iP)
{
    if (has(itNode, iP, k_number))
        return true;    //return predicate();
    else
        return false;
}

bool SyntaxValidator::has_label(LdpTree::iterator itNode, int* iP)
{
    if (has(itNode, iP, k_label))
        return true;    //return predicate();
    else
        return false;
}

bool SyntaxValidator::has_string(LdpTree::iterator itNode, int* iP)
{
    if (has(itNode, iP, k_string))
        return true;    //return predicate();
    else
        return false;
}

bool SyntaxValidator::error_missing_element(LdpElement* node, ELdpElements type)
{
    const string& parentName = Factory::instance()->get_name(node->get_type());
    const string& name = Factory::instance()->get_name(type);
    report_msg(node->get_line_number(),
               parentName + ": missing mandatory element '" + name + "'.");
    return false;
}

void SyntaxValidator::replace_content(LdpTree::iterator itNode,
                                      const std::string& newContent)
{
    LdpParser parser(m_reporter);
    LdpTree* treeChild = parser.parse_text(newContent);
    LdpElement* oldNode = *itNode;
    m_pTree->replace_node(itNode, treeChild->get_root());
    delete treeChild;
    delete oldNode;
}

bool SyntaxValidator::replace_if_failure(bool success, LdpTree::iterator itNode,
                                         const std::string& newContent)
{
    if (!success)
        replace_content(itNode, newContent);

    return success;
}

bool SyntaxValidator::remove_invalid(LdpTree::iterator itNode)
{
    report_msg((*itNode)->get_line_number(), "Removed unknown element.");
    LdpElement* oldNode = *itNode;
    m_pTree->erase(itNode);
    delete oldNode;
    return true;
}

void SyntaxValidator::report_msg(int numLine, const std::stringstream& msg)
{
    report_msg(numLine, msg.str());
}

void SyntaxValidator::report_msg(int numLine, const std::string& msg)
{
    m_reporter << "Line " << numLine << ". ";
    m_reporter << msg << endl;
}

//-----------------------------------------------------------------------------------
// LDP action part for the grammar rules
//
//  Each LDP grammar rule has the posibility of executing actions. Fot this, the
//  actions to execute must be enclosed in a functor class to be passed as parameter
//  to the grammar rule.
//  The action functors should return 'true' to indicate that the test has been 
//  passed , either because it ther are no errors or beacuse any error has been
//  fixed. If it returns 'false' it will mean that the node did not pass the check,
//  and the caller is then allowed to do any necessary action, such as replacing
//  the node or remove it.
//-----------------------------------------------------------------------------------

class CheckDuration
{
public:
    bool operator()(LdpTree* tree, LdpElement* node, ostream& reporter)
    {
        LdpElement* child = node->get_parameter(2);
        if (!LdpValues::CheckDuration( child->get_ldp_value() ))
            return true;
        else
        {
            reporter << "Line " << node->get_line_number()
                     << ". Bad note/rest duration '" 
                     << child->get_ldp_value()
                     << "'. Replaced by 'q'." << endl;
            LdpTree::iterator it(child);
            LdpElement* value = new_value(k_duration, "q");
            tree->replace_node(it, value);
            delete child;
            return true;
        }
    }
};

class CheckPitch
{
public:
    bool operator()(LdpTree* tree, LdpElement* node, ostream& reporter)
    {
        LdpElement* child = node->get_parameter(1);
        if (!LdpValues::CheckPitch( child->get_ldp_value() ))
            return true;
        else
        {
            reporter << "Line " << node->get_line_number()
                     << ". Note. Bad pitch '" 
                     << child->get_ldp_value()
                     << "'. Replaced by 'c4'." << endl;
            LdpTree::iterator it(child);
            LdpElement* value = new_value(k_pitch, "c4");
            tree->replace_node(it, value);
            delete child;
            return true;
        }
    }
};

class CheckLabelValue
{
public:
    CheckLabelValue() {};
    bool operator()(LdpTree* tree, LdpElement* node, ostream& reporter)
    {
        LdpElement* child = node->get_parameter(1);
        size_t found = m_validValues.find( child->get_ldp_value() );
        if (found != string::npos)
            return true;
        else
        {
            reporter << "Line " << node->get_line_number()
                     << ". Bad value '" 
                     << child->get_ldp_value()
                     << "'. Replaced by '" << m_replacement << "'." << endl;
            LdpTree::iterator it(child);
            LdpElement* value = new_value(k_label, m_replacement);
            tree->replace_node(it, value);
            delete child;
            return true;
        }
    }
    void set_valid_values(const std::string& values) { m_validValues = values; }
    void set_replacement(const std::string& replacement) { m_replacement = replacement; }

protected:
    std::string  m_validValues;
    std::string  m_replacement;
};

class CheckVersion
{
public:
    bool operator()(LdpTree* tree, LdpElement* node, ostream& reporter)
    {
        if (node->get_parameter(1)->get_ldp_value() == "1.6")
            return true;
        else
        {
            reporter << "Line " << node->get_line_number()
                     << ". Score. Bad version number: " 
                     << node->get_parameter(1)->get_ldp_value() << endl;
            return true;
        }
    }
};

//-----------------------------------------------------------------------------------
// LDP language rules (alphabetical, by tag name)
//
//  To simplify maintenance I've decided to have a big switch statement
//  instead of creating a factory scheme with an abstract class and a derived
//  class for each element grammar rule. This second approach would require
//  much more maintenance each time a new LDP element is added and it doesn't
//  make the code neither cleaner, nor faster, nor easier to use, but more
//  oscure.
//
//  Each case statement represents a LDP grammar rule.
//-----------------------------------------------------------------------------------

bool SyntaxValidator::validate(LdpTree::iterator itNode)
{
    //itNode points to the node to validate 

    //functors for specific checks / actions
    static CheckPitch check_pitch;
    static CheckDuration check_duration;
    static CheckLabelValue check_label_value;
    static CheckVersion check_version;

    //the rules
	int iP=1;
    ELdpElements type = (*itNode)->get_type();
    const string& parentName = Factory::instance()->get_name(type);
    switch (type)
    {
        case k_clef:
        {
            check_label_value.set_valid_values("GF4F3C1C2C3C4C5");
            check_label_value.set_replacement("G");
            return (has_label(itNode, &iP, check_label_value)      //clef name
                   );
        }

        case k_instrument:
        {
            return (optional(itNode, &iP, k_infoMIDI)
                    && optional(itNode, &iP, k_name)
                    && optional(itNode, &iP, k_abbrev)
                    && optional(itNode, &iP, k_staves)
                    && optional(itNode, &iP, k_staff)
                    && has(itNode, &iP, k_musicData)
                   );
        }

        case k_language:
        {
            return (has_label(itNode, &iP)      //langcode
                    && has_label(itNode, &iP)      //file encoding
                   );
        }

        case k_note:
        {
            return (has_label(itNode, &iP, check_pitch)
                    && has_label(itNode, &iP, check_duration)
                    && optional(itNode, &iP, k_l)
                    && optional(itNode, &iP, k_g)
                    && optional(itNode, &iP, k_p)
                    && no_more(itNode, &iP)
                   );
        }

        case k_score:
        {
            return //replace_if_failure(
                    (has(itNode, &iP, k_vers)
                    && has(itNode, &iP, k_language)
                    && optional(itNode, &iP, k_undoData)
                    && optional(itNode, &iP, k_creationMode)
                    && zero_or_more(itNode, &iP, k_style)
                    && zero_or_more(itNode, &iP, k_title)
                    && optional(itNode, &iP, k_pageLayout)
                    && optional(itNode, &iP, k_systemLayout)
                    && optional(itNode, &iP, k_cursor)
                    && zero_or_more(itNode, &iP, k_opt)
                    && one_or_more(itNode, &iP, k_instrument)  );
                   //), 
                   //itNode, 
                   //"(score (vers 1.6)(language en utf-8)(instrument (musicData)))");
        }

        case k_undefined:
        {
            return remove_invalid(itNode);
        }

        case k_vers:
        {
            return has_number(itNode, &iP, check_version);
        }

        case k_abbrev:
        case k_above:
        case k_barline:
        case k_below:
        case k_bold:
        case k_bold_italic:
        case k_brace:
        case k_bracket:
        case k_center:
        case k_chord:
        case k_color:   
        case k_creationMode:
        case k_cursor:
        case k_defineStyle:
        case k_down:
        case k_duration:
        case k_dx:
        case k_dy:
        case k_end:
        case k_fermata:
        case k_font:
        case k_g:   //beamed group 
        case k_goBack:
        case k_goFwd:
        case k_graphic:
        case k_group:
        case k_hasWidth:
        case k_infoMIDI:
        case k_instrName:
        case k_italic:
        case k_joinBarlines:
        case k_key:
        case k_l:   //tie 
        case k_landscape:
        case k_left:
        case k_line:
        case k_metronome:
        case k_musicData:
        case k_name:
        case k_newSystem:
        case k_no:
        case k_normal:
        case k_noVisible:
        case k_opt:
        case k_p:   //staff number  
        case k_pageLayout:
        case k_pageMargins:
        case k_pageSize:
        case k_parentheses:
        case k_pitch:
        case k_portrait:
        case k_rest:   //rest 
        case k_right:
        case k_spacer:
        case k_split:
        case k_staff:
        case k_start:
        case k_staves:
        case k_stem:
        case k_style:
        case k_symbol:
        case k_systemLayout:
        case k_systemMargins:
        case k_t:   //tuplet 
        case k_text:
        case k_time:
        case k_title:
        case k_v:   //voice
        case k_undoData:
        case k_up:
        case k_yes:
            m_reporter << "SyntaxValidator: missig rule for " << parentName << endl;
            return true;

        default:
            m_reporter << "SyntaxValidator: unknown element " << parentName << endl;
            return true;
    }
}


}   //namespace lenmus
