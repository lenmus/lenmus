//----------------------------------------------------------------------------
// Anti-Grain Documenter - Version 1.0 
// Copyright (C) 2002 Maxim Shemanarev (McSeem)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------

#include "agdoc_code_colorer.h"

namespace agdoc
{

    //==================================================================
    // 
    // code_colorer
    // 
    //==================================================================



    //------------------------------------------------------------------
    code_colorer::code_colorer(const config& cfg, 
                               const element& e,
                               content_storage& result) : 
        m_cfg(cfg),
        m_result(result),
        m_coloring_element(0)
    {
        m_result.reserve(e.total_len());
        e.process(*this);
    }




    //------------------------------------------------------------------
    void code_colorer::start_element(const element& e)
    {
        if(e.name_len())
        {
            if(m_coloring_element == 0)
            {
                if(m_cfg.keyword_exists(keyword_code, e.name(), e.name_len()) ||
                   str_cmp(e.name(), e.name_len(), keyword_code) == 0 ||
                   str_cmp(e.name(), e.name_len(), keyword_m) == 0)
                {
                    m_coloring_element = &e;
                    color_code(e);
                }
                else
                {
                    m_result.add_element_header(e);
                }
            }
        }
    }



    //------------------------------------------------------------------
    void code_colorer::end_element(const element& e)
    {
        if(e.name_len())
        {
            if(m_coloring_element == 0)
            {
                m_result.add(close_brace);
            }
            else
            {
                if(m_coloring_element == &e)
                {
                    m_coloring_element = 0;
                }
            }
        }
    }



    //------------------------------------------------------------------
    void code_colorer::content(const element&, const char_type* c, unsigned len)
    {
        if(m_coloring_element == 0)
        {
            m_result.add(c, len);
        }
    }



    //------------------------------------------------------------------
    void code_colorer::add_to_identifier_charset(const keyword& suffix)
    {
        const strset_type* k = m_cfg.find_keywords(m_language, suffix);
        if(k)
        {
            strset_type::const_iterator i = k->begin();
            for(; i != k->end(); ++i)
            {
                const string_type& str = *i;
                for(unsigned j = 0; j < str.length(); j++)
                {
                    char_type c = str[j];
                    if(!is_alnum(c))
                    {
                        m_identifier_charset.insert(replace_keysym(c));
                    }
                }
            }
        }
    }


    //------------------------------------------------------------------
    void code_colorer::replace_strlist_keysym(string_list& strlist)
    {
        unsigned i;
        for(i = 0; i < strlist.size(); ++i)
        {
            string_type& s = strlist[i];
            unsigned j;
            for(j = 0; j < s.length(); ++j)
            {
                s[j] = replace_keysym(s[j]);
            }
        }
    }


    //------------------------------------------------------------------
    void code_colorer::color_code(const element& e)
    {
        m_language.assign(e.name(), e.name_len());

        m_block_comments   = m_cfg.strlist(string_type(e.name(), e.name_len()), 
                                           keyword_block_comments_suffix);

        m_line_comments    = m_cfg.strlist(string_type(e.name(), e.name_len()), 
                                           keyword_line_comments_suffix);

        m_string_quotation = m_cfg.strlist(string_type(e.name(), e.name_len()), 
                                           keyword_strings_suffix);

        m_string_mask      = m_cfg.strlist(string_type(e.name(), e.name_len()), 
                                           keyword_string_mask_suffix);

        m_operators        = m_cfg.strlist(string_type(e.name(), e.name_len()), 
                                           keyword_operators_suffix);

        replace_strlist_keysym(m_block_comments);
        replace_strlist_keysym(m_line_comments);
        replace_strlist_keysym(m_string_quotation);
        replace_strlist_keysym(m_string_mask);
        replace_strlist_keysym(m_operators);
        
        m_identifier_charset.clear();
        add_to_identifier_charset(keyword_kw1_suffix);
        add_to_identifier_charset(keyword_kw2_suffix);
        add_to_identifier_charset(keyword_kw3_suffix);
        add_to_identifier_charset(keyword_kw4_suffix);

        m_operator_charset.clear();

        unsigned i;
        for(i = 0; i < m_operators.size(); ++i)
        {
            const string_type& str = m_operators[i];
            for(unsigned j = 0; j < str.length(); j++)
            {
                char_type c = str[j];
                if(!is_alnum(c))
                {
                    m_operator_charset.insert(replace_keysym(c));
                }
            }
        }

        content_storage code;
        element_serializer ser(code, e, false, false);
        code.replace_text_keysym();
        m_result.add(backslash);
        if(str_cmp(m_language, keyword_m) == 0)
        {
            m_result.add(keyword_m.name, keyword_m.len);
            m_result.add(open_brace);
            m_result.add(code.text());
            m_result.add(close_brace);
        }
        else
        if(str_cmp(m_language, keyword_code) == 0)
        {
            m_result.add(keyword_code.name, keyword_code.len);
            if(e.attr_len())
            {
                m_result.add(open_bracket);
                m_result.add(e.attr(), e.attr_len());
                m_result.add(close_bracket);
            }
            m_result.add(open_brace);
            m_result.add(code.text());
            m_result.add(close_brace);
        }
        else
        {
            m_result.add(keyword_code.name, keyword_code.len);
            if(e.attr_len())
            {
                m_result.add(open_bracket);
                m_result.add(e.attr(), e.attr_len());
                m_result.add(close_bracket);
            }
            m_result.add(open_brace);
            color_code(code.text());
            m_result.add(close_brace);
        }
    }



    //------------------------------------------------------------------
    bool code_colorer::is_block_comment(const char_type* p, unsigned* len) const
    {
        if(m_block_comments.size() > 1)
        {
            if(str_is(p, m_block_comments[0]))
            {
                const char_type* start = p;
                p += m_block_comments[0].length();
                int level = 1;
                while(p)
                {
                    if(str_is(p, m_block_comments[0]))
                    {
                        ++level;
                        p += m_block_comments[0].length();
                    }
                    else if(str_is(p, m_block_comments[1]))
                    {
                        --level;
                        p += m_block_comments[1].length();
                        if(level == 0)
                        {
                            *len = unsigned(p - start);
                            return true;
                        }
                    }
                    else
                    {
                        ++p;
                    }
                }
            }
        }
        return false;
    }


    
    //------------------------------------------------------------------
    bool code_colorer::is_line_comment(const char_type* p, unsigned* len) const
    {
        if(m_line_comments.size())
        {
            if(str_is(p, m_line_comments[0]))
            {
                const char_type* start = p;
                p += m_line_comments[0].length();
                while(p)
                {
                    if(is_lf(*p)) break;
                    ++p;
                }
                *len = unsigned(p - start);
                return true;
            }
        }

        return false;
    }



    //------------------------------------------------------------------
    char_type code_colorer::is_quotation(char_type c) const
    {
        if(c)
        {
            for(unsigned i = 0; i < m_string_quotation.size(); ++i)
            {
                if(m_string_quotation[i].length() &&
                   c == m_string_quotation[i][0]) return c;
            }
        }
        return 0;
    }


    //------------------------------------------------------------------
    bool code_colorer::is_string_literal(const char_type* p, unsigned* len) const
    {
        char_type q = is_quotation(*p);
        if(q)
        {
            char_type mask = 0; 
            if(m_string_mask.size() && m_string_mask[0].length())
            {
                mask = m_string_mask[0][0];
            }
            const char_type* start = p;
            ++p;
            while(*p)
            {
                if(*p == mask)
                {
                    ++p;
                }
                else
                {
                    if(*p == q)
                    {
                        ++p;
                        break;
                    }
                }
                ++p;
            }
            *len = unsigned(p - start);
            return true;
        }
        return false;
    }


    
    //------------------------------------------------------------------
    bool code_colorer::is_number(const char_type* p, unsigned* len) const
    {
        if(is_digit(*p))
        {
            const char_type* start = p;
            while(*p && is_alnum(*p)) ++p;
            *len = unsigned(p - start);
            return true;
        }
        return false;
    }


    
    //------------------------------------------------------------------
    bool code_colorer::is_operator(const char_type* p, unsigned* len) const
    {
        if(m_operator_charset.find(*p) != m_operator_charset.end())
        {
            unsigned i;
            for(i = 0; i < m_operators.size(); i++)
            {
                *len = m_operators[i].length();
                if(str_cmp(m_operators[i], p, *len) == 0) 
                {
                    return true;
                }
            }
        }
        return false;
    }


    
    //------------------------------------------------------------------
    bool code_colorer::is_identifier(const char_type* p, unsigned* len) const
    {
        if(*p && (is_alpha(*p) || m_identifier_charset.find(*p) != m_identifier_charset.end()))
        {
            const char_type* start = p;
            while(*p && (is_alnum(*p) || m_identifier_charset.find(*p) != m_identifier_charset.end())) ++p;
            *len = unsigned(p - start);
            return true;
        }
        return false;
    }



    //------------------------------------------------------------------
    bool code_colorer::is_keyword(const keyword& category, const char_type* p, unsigned len) const
    {
        return m_cfg.keyword_exists(m_language, category, p, len);
    }


    
    //------------------------------------------------------------------
    void code_colorer::color_code(const char_type* p)
    {
        while(*p)
        {
            unsigned len;
            if(is_block_comment(p, &len))
            {
                m_result.add(keyword_rem, p, len, true);
                p += len;
            }
            else if(is_line_comment(p, &len))
            {
                m_result.add(keyword_rem, p, len, true);
                p += len;
            }
            else if(is_string_literal(p, &len))
            {
                m_result.add(keyword_str, p, len, true);
                p += len;
            }
            else if(is_number(p, &len))
            {
                m_result.add(keyword_num, p, len, true);
                p += len;
            }
            else if(is_operator(p, &len))
            {
                m_result.add(keyword_op, p, len, true);
                p += len;
            }
            else if(is_identifier(p, &len))
            {
                const keyword* kw = 0;
                if     (is_keyword(keyword_kw1_suffix, p, len)) kw = &keyword_kw1;
                else if(is_keyword(keyword_kw2_suffix, p, len)) kw = &keyword_kw2;
                else if(is_keyword(keyword_kw3_suffix, p, len)) kw = &keyword_kw3;
                else if(is_keyword(keyword_kw4_suffix, p, len)) kw = &keyword_kw4;
                if(kw)
                {
                    m_result.add(backslash);
                    m_result.add(kw->name, kw->len);
                    m_result.add(open_brace);
                }
                m_result.add(p, len, true);
                if(kw)
                {
                    m_result.add(close_brace);
                }
                p += len;
            }
            else
            {
                m_result.add(*p++);
            }
        }
    }



}


