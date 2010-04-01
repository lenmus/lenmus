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

#ifndef __LM_SYNTAX_VALIDATOR_H__ 
#define __LM_SYNTAX_VALIDATOR_H__

#include "../elements/Elements.h"

using namespace std;

namespace lenmus
{

class SyntaxValidator
{
protected:
    ostream&            m_reporter;
    const SpLdpTree&    m_tree;

public:
    SyntaxValidator(const SpLdpTree& tree, ostream& reporter);
    bool validate(LdpTree::iterator itNode, ELdpElements type);

protected:
    //error reporting
    bool error_missing_element(SpLdpElement node, ELdpElements type);
    void report_msg(int numLine, const std::string& msg);
    void report_msg(int numLine, const std::stringstream& msg);

    //tree management
    void replace_content(LdpTree::iterator itNode, const std::string& newContent);
    bool replace_if_failure(bool success, LdpTree::iterator itNode,
                            const std::string& newContent);
    bool remove_invalid(LdpTree::iterator itNode);

    //tree validation
    bool has(LdpTree::iterator itNode, int* iP, ELdpElements type);
    bool optional(LdpTree::iterator itNode, int* iP, ELdpElements type);
    bool zero_or_more(LdpTree::iterator itNode, int* iP, ELdpElements type);
    bool one_or_more(LdpTree::iterator itNode, int* iP, ELdpElements type);
    bool has_number(LdpTree::iterator itNode, int* iP);
    bool has_label(LdpTree::iterator itNode, int* iP);
    bool has_string(LdpTree::iterator itNode, int* iP);
    bool no_more(LdpTree::iterator itNode, int* iP);

    template <class ActionsFunctor> 
    bool has_number(LdpTree::iterator itNode, int* iP, ActionsFunctor actions,
                   const std::string& replacement="")
    {
        if (has_number(itNode, iP))
            return apply_actions_or_replacements(itNode, actions, replacement);
        else
            return false;
    }

    template <class ActionsFunctor> 
    bool has_label(LdpTree::iterator itNode, int* iP, ActionsFunctor actions,
                  const std::string& replacement="")
    {
        if (has_label(itNode, iP))
            return apply_actions_or_replacements(itNode, actions, replacement);
        else
            return false;
    }

    template <class ActionsFunctor> 
    bool has_string(LdpTree::iterator itNode, int* iP, ActionsFunctor actions,
                   const std::string& replacement="")
    {
        if (has_string(itNode, iP))
            return apply_actions_or_replacements(itNode, actions, replacement);
        else
            return false;
    }

    template <class ActionsFunctor> 
    bool apply_actions_or_replacements(LdpTree::iterator itNode,
                  ActionsFunctor actions, const std::string& replacement)
    {
        SpLdpElement node = *itNode;
        if (!actions(m_tree, node, m_reporter))
        {
            if (replacement != "")
            {
                replace_content(itNode, replacement);
                return true;
            }
            else
                return false;
        }
        return true;
    }

};



}   //namespace lenmus

#endif      //__LM_SYNTAX_VALIDATOR_H__
