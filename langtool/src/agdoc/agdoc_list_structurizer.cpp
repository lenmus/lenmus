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

#include "agdoc_list_structurizer.h"

namespace agdoc
{

    //==================================================================
    // 
    // list_structurizer
    // 
    //==================================================================



    //------------------------------------------------------------------
    list_structurizer::list_structurizer(const config& cfg,
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
    void list_structurizer::start_element(const element& e)
    {
        if(m_skip_element) return;

        if(is_paragraphless_element(e))
        {
            complete_lists();
            m_skip_element = &e;
            write_element(e);
            return;
        }

        if(is_paragraph_structure_element(e) || is_paragraph_br_style_element(e))
        {
            complete_lists();
            m_result.add_element_header(e);
        }
        else
        {
            m_skip_element = &e;
            write_element(e);
        }
    }



    //------------------------------------------------------------------
    void list_structurizer::end_element(const element& e)
    {
        if(m_skip_element)
        {
            if(m_skip_element == &e)
            {
                m_skip_element = 0;
            }
        }
        else
        {
            complete_lists();
            m_result.add_element_footer(e);
        }
    }



    //------------------------------------------------------------------
    unsigned list_structurizer::detect_list_item(const char_type* str, unsigned len, 
                                                 char_type* lt, unsigned* skip)
    {
        if(len < 3) return 0;

        if(is_lf(*str))
        {
            const char_type* start = str;
            ++str;
            --len;
            if(is_space(*str))
            {
                unsigned offset = 0;
                while(len && is_space(*str))
                {
                    --len;
                    ++str;
                    ++offset;
                }
                if(len == 0) return 0;
                if(*str == asterisk)
                {
                    ++str;
                    --len;
                    *lt = *str;
                    while(len && is_space(*str))
                    {
                        --len;
                        ++str;
                    }
                    *lt = asterisk;
                    *skip = unsigned(str - start);
                    return offset;
                }
                else
                if(is_digit(*str))
                {
                    *lt = *str;
                    while(len && is_digit(*str))
                    {
                        --len;
                        ++str;
                    }
                    if(len == 0) return 0;
                    if(*str == dot)
                    {
                        ++str;
                        --len;
                        while(len && is_space(*str))
                        {
                            --len;
                            ++str;
                        }
                        *skip = unsigned(str - start);
                        return offset;
                    }
                }
            }
        }
        return 0;
    }


    
    //------------------------------------------------------------------
    void list_structurizer::start_list(char_type lt, unsigned offset)
    {
        list_info info = { ordered_list, 0 };
        info.offset = offset;
        if(lt == asterisk)
        {
            info.type = bullet_list;
        }
        m_list_info.push_back(info);
        m_result.add(lf);
        m_result.add(backslash);
        if(info.type == bullet_list)
        {
            m_result.add(keyword_ul.name, keyword_ul.len);
        }
        else
        {
            m_result.add(keyword_ol.name, keyword_ol.len);
        }
        m_result.add(open_brace);
    }


    //------------------------------------------------------------------
    void list_structurizer::start_item()
    {
        if(m_list_info.size())
        {
            m_result.add(lf);
            m_result.add(backslash);
            m_result.add(keyword_li.name, keyword_li.len);
            m_result.add(open_brace);
        }
    }


    //------------------------------------------------------------------
    void list_structurizer::end_list()
    {
        if(m_list_info.size())
        {
            m_result.add(close_brace);
            m_list_info.pop_back();
        }
    }


    //------------------------------------------------------------------
    void list_structurizer::end_item()
    {
        if(m_list_info.size())
        {
            m_result.add(close_brace);
        }
    }


    //------------------------------------------------------------------
    void list_structurizer::content(const element& WXUNUSED(e), const char_type* str, unsigned len)
    {
        if(m_skip_element) return;

        while(len)
        {
            unsigned n = 1;
            if(is_empty_line(str, len))
            {
                complete_lists();
                m_result.add(*str);
            }
            else
            {
                unsigned offset = 0;
                if(m_list_info.size()) offset = m_list_info[m_list_info.size() - 1].offset;
                char_type lt;
                unsigned new_offset = detect_list_item(str, len, &lt, &n);
                if(new_offset)
                {
                    if(new_offset > offset)
                    {
                        end_item();
                        start_list(lt, new_offset);
                        start_item();
                    }
                    else
                    {
                        while(m_list_info.size() > 1 && new_offset < offset)
                        {
                            end_list();
                            offset = m_list_info[m_list_info.size() - 1].offset;
                        }
                        end_item();
                        start_item();
                    }
                }
                else
                {
                    m_result.add(*str);
                }
            }
            len -= n;
            str += n;
        }
    }



    //------------------------------------------------------------------
    void list_structurizer::complete_lists()
    {
        end_item();
        while(m_list_info.size())
        {
            end_list();
        }
    }


    //------------------------------------------------------------------
    void list_structurizer::write_element(const element& e)
    {
        element_serializer s(m_result, e);
    }


    //------------------------------------------------------------------
    bool list_structurizer::is_paragraphless_element(const element& e) const
    {
        return m_cfg.keyword_exists(keyword_paragraphless_elements, e.name(), e.name_len());
    }


    //------------------------------------------------------------------
    bool list_structurizer::is_paragraph_structure_element(const element& e) const
    {
        if(e.name_len() == 0) return true;
        return m_cfg.keyword_exists(keyword_paragraph_structure_elements, e.name(), e.name_len());
    }


    //------------------------------------------------------------------
    bool list_structurizer::is_paragraph_br_style_element(const element& e) const
    {
        return m_cfg.keyword_exists(keyword_paragraph_br_style_elements, e.name(), e.name_len());
    }


}

