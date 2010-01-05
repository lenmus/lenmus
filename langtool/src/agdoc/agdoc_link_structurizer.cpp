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

#include "agdoc_link_structurizer.h"

namespace agdoc
{

    //==================================================================
    // 
    // link_structurizer
    // 
    //==================================================================



    //------------------------------------------------------------------
    link_structurizer::link_structurizer(const config& cfg,
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
    void link_structurizer::start_element(const element& e)
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
    void link_structurizer::end_element(const element& e)
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
    bool link_structurizer::is_linking_keyword(const char_type* str, unsigned len) const
    {
        return m_cfg.keyword_exists(keyword_linking_keywords, str, len);
    }


    //------------------------------------------------------------------
    bool link_structurizer::is_link(const char_type* start, 
                                    const char_type* str, 
                                    unsigned len, 
                                    unsigned* ret)
    {
        if(len)
        {
            if(is_lower(*str))
            {
                const char_type* beg = str;
                *ret = 1;
                char_type prev = 0;
                if(str > start)
                {
                    prev = *(str - 1);
                }

                ++str;
                while(len && is_alnum(*str))
                {
                    ++str; ++(*ret); --len;
                }

                if(len == 0 || *str != colon) return false;

                if(!is_linking_keyword(beg, *ret))
                {
                    ++str; ++(*ret); --len;

                    if(len == 0 || *str != slash) return false;
                    ++str; ++(*ret); --len;

                    if(len == 0 || *str != slash) return false;
                    ++str; ++(*ret); --len;
                }

                char_type closing_char = 0;
                switch(prev)
                {
                case 0: break;
                case apostrophe       : closing_char = apostrophe;        break;
                case quote            : closing_char = quote;             break;
                case open_brace       : closing_char = close_brace;       break;
                case open_bracket     : closing_char = close_bracket;     break;
                case open_parenthesis : closing_char = close_parenthesis; break;
                case open_angle       : closing_char = close_angle;       break;
                }

                while(len && 
                      !is_space(*str) && 
                      *str != closing_char && 
                      *str != close_brace &&
                      *str != backslash && 
                      *str != element_replacement_char &&
                      *str != backslash_restore &&
                      *str != dollar_restore &&
                      *str != open_brace_restore &&
                      *str != close_brace_restore &&
                      *str != open_bracket_restore &&
                      *str != close_bracket_restore &&
                      *str != reserved_char)
                {
                    ++str; ++(*ret); --len;
                }
                while(*ret && str > start && !is_alnum(*str))
                {
                    --str; --(*ret); ++len;
                }
                if(is_alnum(*str)) ++(*ret);
                return true;
            }
        }
        return false;
    }


    //------------------------------------------------------------------
    void link_structurizer::add_img(const char_type* str, unsigned len, const keyword& img)
    {
        while(len)
        {
            if(*str != colon && *str != slash) break;
            ++str;
            --len;
        }
        if(len)
        {
            m_result.add(backslash);
            m_result.add(img.name, img.len);
            m_result.add(open_bracket);
            m_result.add(quote);
            m_result.add(str, len);
            m_result.add(quote);
            m_result.add(close_bracket);
        }
    }



    //------------------------------------------------------------------
    void link_structurizer::add_link(const char_type* str, unsigned len)
    {
        if(len)
        {
            unsigned i = 0;
            for(i = 0; i < len; i++)
            {
                if(str[i] == colon)
                {
                    if(str_cmp(str, i, keyword_imgl0) == 0)
                    {
                        add_img(str+i, len-i, keyword_imgl0);
                        return;
                    }
                    else
                    if(str_cmp(str, i, keyword_imgr0) == 0)
                    {
                        add_img(str+i, len-i, keyword_imgr0);
                        return;
                    }
                    else
                    if(str_cmp(str, i, keyword_imgl) == 0)
                    {
                        add_img(str+i, len-i, keyword_imgl);
                        return;
                    }
                    else
                    if(str_cmp(str, i, keyword_imgr) == 0)
                    {
                        add_img(str+i, len-i, keyword_imgr);
                        return;
                    }
                    else
                    if(str_cmp(str, i, keyword_imgc) == 0)
                    {
                        add_img(str+i, len-i, keyword_imgc);
                        return;
                    }
                    else
                    if(str_cmp(str, i, keyword_img) == 0)
                    {
                        add_img(str+i, len-i, keyword_img);
                        return;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            m_result.add(backslash);
            m_result.add(keyword_href.name, keyword_href.len);
            m_result.add(open_bracket);
            m_result.add(quote);
            m_result.add(str, len);
            m_result.add(quote);
            m_result.add(close_bracket);
            m_result.add(open_brace);
            m_result.add(str, len);
            m_result.add(close_brace);
        }
    }


    //------------------------------------------------------------------
    void link_structurizer::content(const element& WXUNUSED(e), 
                                    const char_type* str, 
                                    unsigned len)
    {
        if(m_skip_element) return;

        const char_type* start = str;
        while(len)
        {
            unsigned n = 1;
            if(is_link(start, str, len, &n))
            {
                add_link(str, n);
            }
            else
            {
                m_result.add(str, n);
            }
            if(n > len) n = len;
            len -= n;
            str += n;
        }
    }


}
