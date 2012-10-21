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

#include "agdoc_table_structurizer.h"

namespace agdoc
{


    //==================================================================
    // 
    // table_content_extractor
    // 
    //==================================================================


    //------------------------------------------------------------------
    table_content_extractor::table_content_extractor(const element& e,
                                                     content_storage& header,
                                                     content_storage& table,
                                                     string_buffer_type& align) :
        m_header(header),
        m_table(table),
        m_align(align),
        m_skip_element(0),
        m_level(0)
    {
        m_header.clear();
        m_table.clear();
        m_align.clear();
        e.process(*this);
    }

        
    //------------------------------------------------------------------
    void table_content_extractor::start_element(const element& e)
    {
        if(m_skip_element) return;
    
        if(str_cmp(e.name(), e.name_len(), keyword_th) == 0)
        {
            element_serializer(m_header, e, false);
            m_skip_element = &e;
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_al) == 0)
        {
            content_storage st;
            element_serializer(st, e, false);
            const char_type* str = st.text();
            while(*str)
            {
                char_type c = *str++;
                if(c == open_angle ||
                   c == close_angle ||
                   c == vbar ||
                   c == minus)
                {
                    m_align.push_back(c);
                }
            }
            m_skip_element = &e;
        }
        else
        {
            if(m_level)
            {
                m_table.add_element_header(e);
            }
            ++m_level;
        }
    }


    //------------------------------------------------------------------
    void table_content_extractor::end_element(const element& e)
    {
        if(m_skip_element)
        {
            if(m_skip_element == &e)
            {
                m_skip_element = 0;
            }
            return;
        }

        --m_level;
        if(m_level)
        {
            m_table.add_element_footer(e);
        }
    }



    //------------------------------------------------------------------
    void table_content_extractor::content(const element& WXUNUSED(e), const char_type* c, unsigned len)
    {
        if(m_skip_element == 0)
        {
            m_table.add(c, len);
        }
    }















    //==================================================================
    // 
    // table_content_extractor
    // 
    //==================================================================



    //------------------------------------------------------------------
    table_structurizer::table_structurizer(const config& cfg,
                                           const element& e,
                                           content_storage& result) :
        m_cfg(cfg),
        m_result(result),
        m_skip_element(0)
    {
        m_result.reserve(e.total_len());
        e.process(*this);
    }



    //------------------------------------------------------------------
    void table_structurizer::start_element(const element& e)
    {
        if(m_skip_element) return;

//        if(m_cfg.keyword_exists(keyword_formatless_elements, e.name(), e.name_len()))
        if(m_cfg.is_solid_element(e.name(), e.name_len()))
        {
            element_serializer s(m_result, e);
            m_skip_element = &e;
            return;
        }

        if(str_cmp(e.name(), e.name_len(), keyword_ssv_table) == 0)
        {
            process_ssv_table(e);
            m_skip_element = &e;
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_vbar_table) == 0)
        {
            process_vbar_table(e);
            m_skip_element = &e;
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_eln_table) == 0)
        {
            process_eln_table(e);
            m_skip_element = &e;
        }
        else
        {
            m_result.add_element_header(e);
        }
    }




    //------------------------------------------------------------------
    void table_structurizer::end_element(const element& e)
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
    void table_structurizer::content(const element& WXUNUSED(e), const char_type* str, unsigned len)
    {
        if(m_skip_element == 0)
        {
            m_result.add(str, len);
        }
    }


    //------------------------------------------------------------------
    void table_structurizer::add_th(const char_type* str, unsigned len, const keyword* kw, unsigned ncol)
    {
        if(len)
        {
            if(str_cmp(kw->name, kw->len, keyword_th.name, keyword_th.len) != 0)
            {
                if(ncol < m_align.size())
                {
                    switch(m_align[ncol])
                    {
                    case open_angle:   kw = &keyword_td;  break;
                    case close_angle:  kw = &keyword_tdr; break;
                    case vbar:         kw = &keyword_tdc; break;
                    case minus:        kw = &keyword_tdj; break;
                    }
                }

                if(len >= 2 && str[len-1] == close_angle && str[len-2] == vbar)
                {
                    kw = &keyword_tdr;
                    len -= 2;
                    if(len >= 2 && str[0] == open_angle && str[1] == vbar)
                    {
                        str += 2;
                        len -= 2;
                        kw = &keyword_tdc;
                    }
                }

                if(len >= 4 && 
                   str[0]     == open_angle  && str[1]     == minus &&
                   str[len-1] == close_angle && str[len-2] == minus)
                {
                    str += 2;
                    len -= 4;
                    kw = &keyword_tdj;
                }
            }
        }
        m_result.add(backslash);
        m_result.add(kw->name, kw->len);
        m_result.add(open_brace);
        if(len) 
        {
            m_result.add(str, len);
        }
        m_result.add(close_brace);
    }




    //------------------------------------------------------------------
    void table_structurizer::parse_double_vbar(const char_type* str, unsigned len, const keyword* kw)
    {
        unsigned ncol = 0;
        while(len)
        {
            while(len && is_space(*str)) 
            {
                ++str;
                --len;
            }

            if(len >= 2 && is_double_vbar(str)) 
            {
                str += 2;
                len -= 2;
            }

            while(len && is_space(*str)) 
            {
                ++str;
                --len;
            }

            if(len)
            {
                const char_type* start = str;
                while(len && !is_double_vbar(str)) 
                {
                    ++str;
                    --len;
                }
                const char_type* end = str - 1;
                while(end > start && is_space(*end)) --end;
                add_th(start, unsigned(end - start) + 1, kw, ncol);
                ++ncol;
            }
        }
    }



    //------------------------------------------------------------------
    void table_structurizer::process_table_header(const element& e)
    {
        m_result.add(backslash);
        m_result.add(keyword_table.name, keyword_table.len);
        if(e.attr_len())
        {
            m_result.add(open_bracket);
            m_result.add(e.attr(), e.attr_len());
            m_result.add(close_bracket);
        }
        m_result.add(open_brace);
        m_result.add(lf);
        if(m_header.length())
        {
            m_result.add(backslash);
            m_result.add(keyword_trh.name, keyword_trh.len);
            m_result.add(open_brace);
            parse_double_vbar(m_header.text(), m_header.length(), &keyword_th);
            m_result.add(close_brace);
        }
    }



    //------------------------------------------------------------------
    void table_structurizer::process_ssv_table(const element& e)
    {
        table_content_extractor(e, m_header, m_table, m_align);
        process_table_header(e);

        if(m_table.length())
        {
            m_result.add(lf);
            const char_type* str = m_table.text();
            while(*str)
            {
                while(is_space(*str)) ++str;
                while(is_lf(*str))    ++str;
                while(is_space(*str)) ++str;
                if(*str)
                {
                    const char_type* start = str;
                    while(*str && !is_lf(*str)) ++str;
                    const char_type* end = str - 1;
                    while(end > start && is_space(*end)) --end;
                    m_result.add(backslash);
                    m_result.add(keyword_tr.name, keyword_tr.len);
                    m_result.add(open_brace);
                    unsigned ncol = 0;
                    while(start <= end)
                    {
                        while(is_space(*start)) ++start;
                        const char_type* start0 = start;
                        while(start <= end && *start && !is_space(*start)) ++start;
                        if(start > start0)
                        {
                            add_th(start0, unsigned(start - start0), &keyword_td, ncol);
                            ++ncol;
                        }
                    }
                    m_result.add(close_brace);
                    m_result.add(lf);
                }
            }
        }
        m_result.add(close_brace);
    }


    //------------------------------------------------------------------
    void table_structurizer::process_vbar_table(const element& e)
    {
        table_content_extractor(e, m_header, m_table, m_align);
        process_table_header(e);
        if(m_table.length())
        {
            m_result.add(lf);
            const char_type* str = m_table.text();
            while(*str)
            {
                while(is_space(*str)) ++str;
                while(is_lf(*str))    ++str;
                while(is_space(*str)) ++str;
                if(*str)
                {
                    const char_type* start = str;
                    while(*str && !is_lf(*str)) ++str;
                    const char_type* end = str - 1;
                    while(end > start && is_space(*end)) --end;
                    m_result.add(backslash);
                    m_result.add(keyword_tr.name, keyword_tr.len);
                    m_result.add(open_brace);
                    if(start <= end)
                    {
                        parse_double_vbar(start, unsigned(end - start) + 1, &keyword_td);
                    }
                    m_result.add(close_brace);
                    m_result.add(lf);
                }
            }
        }
        m_result.add(close_brace);
    }


    //------------------------------------------------------------------
    void table_structurizer::process_eln_table(const element& e)
    {
        table_content_extractor(e, m_header, m_table, m_align);
        process_table_header(e);

        if(m_table.length())
        {
            m_result.add(lf);
            const char_type* str = m_table.text();
            while(*str)
            {
                while(is_row_break(str)) ++str;
                while(is_space(*str) || is_minus(*str)) ++str;
                if(*str)
                {
                    const char_type* start = str;
                    while(*str && !is_row_break(str)) ++str;
                    const char_type* end = str - 1;
                    while(end > start && is_space(*end)) --end;
                    m_result.add(backslash);
                    m_result.add(keyword_tr.name, keyword_tr.len);
                    m_result.add(open_brace);
                    unsigned ncol = 0;
                    while(start <= end)
                    {
                        while(start <= end && (is_space(*start) || is_lf(*start))) ++start;
                        const char_type* start0 = start;
                        while(start <= end && !is_empty_line(start)) ++start;
                        if(start > start0)
                        {
                            add_th(start0, unsigned(start - start0), &keyword_td, ncol);
                            ++ncol;
                        }

                    }
                    m_result.add(close_brace);
                    m_result.add(lf);
                }

            }
        }
        m_result.add(close_brace);
    }


}

