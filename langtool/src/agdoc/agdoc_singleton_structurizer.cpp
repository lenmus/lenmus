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

#include "agdoc_singleton_structurizer.h"

namespace agdoc
{


    //==================================================================
    // 
    // singleton_structurizer
    // 
    //==================================================================


    //------------------------------------------------------------------
    singleton_structurizer::singleton_structurizer(const config& cfg,
                                                   const element& e,
                                                   content_storage& result) :
        m_cfg(cfg),
        m_output_specific_replacements(cfg.strlist(keyword_output_specific_replacements)),
        m_singletons(cfg.strlist(keyword_singletons)),
        m_result(result),
        m_skip_element(0)
    {
        m_result.reserve(e.total_len());
        add_to_charset(m_charset, m_output_specific_replacements);
        add_to_charset(m_charset, m_singletons);
        e.process(*this);
    }


    //------------------------------------------------------------------
    const string_type* singleton_structurizer::is_output_specific_replacement(const char_type* str, unsigned len, unsigned* ret_len) const
    {
        if(len == 0) return 0;
        if(m_charset.find(*str) == m_charset.end()) return 0;
        return find_replacement(m_output_specific_replacements, str, len, ret_len);
    }

    //------------------------------------------------------------------
    const string_type* singleton_structurizer::is_singleton(const char_type* str, unsigned len, unsigned* ret_len) const
    {
        if(len == 0) return 0;
        if(m_charset.find(*str) == m_charset.end()) return 0;
        return find_replacement(m_singletons, str, len, ret_len);
    }


    //------------------------------------------------------------------
    void singleton_structurizer::start_element(const element& e)
    {
        if(m_skip_element) return;

        if(m_cfg.is_solid_element(e.name(), e.name_len()))
        {
            content_storage storage;
            element_serializer s(storage, e);
            const char_type* str = storage.text();
            unsigned len = storage.length();
            while(len)
            {
                unsigned n = 1;
                const string_type* repl;

                if((repl = is_output_specific_replacement(str, len, &n)) != 0)
                {
                    m_result.add(backslash);
                    m_result.add(repl);
                    m_result.add(open_brace);
                    m_result.add(close_brace);
                }
                else
                {
                    m_result.add(*str);
                }
                if(n > len) n = len;
                str += n;
                len -= n;
            }
            m_skip_element = &e;
            return;
        }
        m_result.add_element_header(e);
    }

    //------------------------------------------------------------------
    void singleton_structurizer::end_element(const element& e)
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
    }


    //------------------------------------------------------------------
    void singleton_structurizer::content(const element& WXUNUSED(e), const char_type* str, unsigned len)
    {
        if(m_skip_element) return;

        while(len)
        {
            unsigned n = 1;
            const string_type* repl;

            if((repl = is_singleton(str, len, &n)) != 0 ||
               (repl = is_output_specific_replacement(str, len, &n)) != 0)
            {
                m_result.add(backslash);
                m_result.add(repl);
                m_result.add(open_brace);
                m_result.add(close_brace);
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

