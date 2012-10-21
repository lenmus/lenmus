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

#include "agdoc_pair_quote_structurizer.h"

namespace agdoc
{


    //==================================================================
    // 
    // pair_quote_structurizer
    // 
    //==================================================================


    //------------------------------------------------------------------
    pair_quote_structurizer::pair_quote_structurizer(const config& cfg,
                                                     const element& e,
                                                     content_storage& result) :
        m_cfg(cfg),
        m_pair_quotes(cfg.strlist(keyword_pair_quotes)),
        m_result(result),
        m_skip_element(0)
    {
        m_result.reserve(e.total_len());
        add_to_charset(m_charset, m_pair_quotes);
        e.process(*this);
    }



    //------------------------------------------------------------------
    const string_type* pair_quote_structurizer::is_pair_quote_replacement(const char_type* str, unsigned len, unsigned* ret_len) const
    {
        if(len == 0) return 0;
        if(m_charset.find(*str) == m_charset.end()) return 0;
        return find_replacement(m_pair_quotes, str, len, ret_len);
    }


    //------------------------------------------------------------------
    void pair_quote_structurizer::start_element(const element& e)
    {
        if(m_skip_element) return;

        if(m_cfg.is_solid_element(e.name(), e.name_len()))
        {
            element_serializer s(m_result, e);
            m_skip_element = &e;
            return;
        }
        m_result.add_element_header(e);
    }


    //------------------------------------------------------------------
    void pair_quote_structurizer::end_element(const element& e)
    {
        if(m_skip_element)
        {
            if(m_skip_element == &e)
            {
                m_skip_element = 0;
            }
            return;
        }
        m_result.add_element_footer(e);
        if(e.name_len() == 0)
        {
            if(m_stack.size())
            {
                e.throw_exception(e.content(), "Formatting elements are not balanced");
            }
        }
    }


    //------------------------------------------------------------------
    bool pair_quote_structurizer::pair_quote_started(const element& WXUNUSED(e), const string_type& repl) const
    {
        int i;
        for(i = 0; i < (int)m_stack.size(); ++i)
        {
            if(m_stack[i].name.compare(repl) == 0) return true;
        }
        return false;
    }


    //------------------------------------------------------------------
    void pair_quote_structurizer::start_formatting(const element& e, const string_type& repl)
    {
        pair_quote_element fe;
        fe.name = repl;
        fe.e    = &e;
        m_stack.push_back(fe);
        m_result.add(backslash);
        m_result.add(repl);
        m_result.add(open_brace);
    }


    //------------------------------------------------------------------
    void pair_quote_structurizer::end_formatting(const element& e, const char_type* ptr, const string_type& repl)
    {
        if(m_stack.size() == 0)
        {
            e.throw_exception(ptr, "Formatting element was not started");
        }

        const pair_quote_element& fe = m_stack[m_stack.size() - 1];
        if(fe.name.compare(repl) != 0)
        {
            e.throw_exception(ptr, "Intersecting format elements detected");
        }

        if(fe.e != &e)
        {
            e.throw_exception(ptr, "Bad formatting structure (Start/end belong to different elements)");
        }
        m_result.add(close_brace);
        m_stack.pop_back();
    }


    //------------------------------------------------------------------
    void pair_quote_structurizer::content(const element& e, const char_type* str, unsigned len)
    {
        if(m_skip_element) return;

        while(len)
        {
            unsigned n = 1;
            const string_type* repl;

            if((repl = is_pair_quote_replacement(str, len, &n)) != 0)
            {
                if(pair_quote_started(e, *repl))
                {
                    end_formatting(e, str, *repl);
                }
                else
                {
                    start_formatting(e, *repl);
                }
            }
            else
            {
                m_result.add(*str);
            }
            if(n > len) n = len;
            str += n;
            len -= n;
        }
    }


}

