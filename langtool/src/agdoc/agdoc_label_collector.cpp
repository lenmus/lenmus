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

#include "agdoc_label_collector.h"

namespace agdoc
{

    //==================================================================
    // 
    // label_collector
    // 
    //==================================================================


    //------------------------------------------------------------------
    label_collector::label_collector(const config& cfg,
                                     const element& e,
                                     content_storage& result,
                                     content_storage& lab,
                                     content_storage& toc) :
        m_cfg(cfg),
        m_result(result),
        m_lab(lab),
        m_toc(toc),
        m_toc_counter(0),
        m_toc_offset(0),
        m_skip_element(0),
        m_code_element(0)
    {
        m_result.reserve(e.total_len());
        m_toc.add(backslash);
        m_toc.add(keyword_toc.name, keyword_toc.len);
        m_toc.add(lf);
        m_toc.add(open_brace);
        e.process(*this);
        m_toc.add(lf);
        m_toc.add(close_brace);
        m_toc.add(lf);
    }


    //------------------------------------------------------------------
    void label_collector::add_label(const keyword& category, 
                                    const char_type* anchor, unsigned anchor_len, 
                                    const char_type* comment, unsigned comment_len)
    {
        m_lab.add(backslash);
        m_lab.add(keyword_label.name, keyword_label.len);
        m_lab.add(open_bracket);
        m_lab.add(keyword_category.name, keyword_category.len);
        m_lab.add(equal);
        m_lab.add(quote);
        m_lab.add(category.name, category.len);
        m_lab.add(quote);
        m_lab.add(semicolon);
        m_lab.add(space);
        m_lab.add(keyword_anchor.name, keyword_anchor.len);
        m_lab.add(equal);
        m_lab.add(quote);
        m_lab.add(anchor, anchor_len);
        m_lab.add(quote);
        m_lab.add(semicolon);
        m_lab.add(close_bracket);
        m_lab.add(open_brace);
        m_lab.add(comment, comment_len);
        m_lab.add(close_brace);
        m_lab.add(lf);
    }



    //------------------------------------------------------------------
    void label_collector::add_label(const keyword& category, 
                                    const char_type* attr, unsigned attr_len,
                                    const element* e)
    {
        if(attr_len)
        {
            string_type anchor;
            if(parse_and_find_attr_variable(attr, attr_len, 
                                            string_type(keyword_anchor.name, keyword_anchor.len), 
                                            anchor))
            {
                string_type comment;
                if(e)
                {
                    content_storage s;
                    element_serializer(s, *e, false, false);
                    comment = s.text();
                }
                else
                {
                    parse_and_find_attr_content(attr, attr_len, comment);
                }

                if(comment.length() == 0)
                {
                    comment = anchor;
                }
                add_label(category, 
                          anchor.data(), anchor.length(), 
                          comment.data(), comment.length());

                m_result.add(backslash);
                m_result.add(keyword_label.name, keyword_label.len);
                m_result.add(open_bracket);
                m_result.add(anchor);
                m_result.add(close_bracket);
                m_result.add(open_brace);
                m_result.add(close_brace);
            }
        }
    }




    //------------------------------------------------------------------
    void label_collector::start_element(const element& e)
    {
        if(m_skip_element) return;

        int tocn;
        if(str_cmp(e.name(), e.name_len(), keyword_label) == 0)
        {
            if(e.content_len())  
            {
                // Visible label
                //------------------
                m_buffer.clear();
                element_serializer(m_buffer, e, false, false);

                if(e.attr_len())
                {
                    // Variant 1. \label[ANCHOR]{Comment}
                    add_label(keyword_index, 
                              e.attr(), e.attr_len(), 
                              m_buffer.text(), m_buffer.length());
                    m_result.add_element_header(e);
                    m_result.add(m_buffer.text(), m_buffer.length());
                    m_result.add_element_footer(e);
                }
                else
                {
                    // Variant 2. \label{Keyword}
                    add_label(keyword_index, 
                              m_buffer.text(), m_buffer.length(), 
                              m_buffer.text(), m_buffer.length());
                    m_result.add(backslash);
                    m_result.add(keyword_label.name, keyword_label.len);
                    m_result.add(open_bracket);
                    m_result.add(m_buffer.text(), m_buffer.length());
                    m_result.add(close_bracket);
                    m_result.add(open_brace);
                    m_result.add(m_buffer.text(), m_buffer.length());
                    m_result.add(close_brace);
                }
            }
            else                 
            {
                // Invisible label
                //------------------
                string_type anchor;
                string_type comment;
                if(parse_and_find_attr_variable(e.attr(), e.attr_len(), 
                                                string_type(keyword_anchor.name, keyword_anchor.len), 
                                                anchor))
                {
                    // Variant 1. \label[Comment; anchor=ANCHOR]
                    parse_and_find_attr_content(e.attr(), e.attr_len(), comment);
                    if(comment.length() == 0)
                    {
                        comment = anchor;
                    }
                    add_label(keyword_index, 
                              anchor.data(), anchor.length(), 
                              comment.data(), comment.length());
                }
                else
                {
                    // Variant 2. \label[Keyword]
                    anchor.assign(e.attr(), e.attr_len());
                    add_label(keyword_index, 
                              e.attr(), e.attr_len(), 
                              e.attr(), e.attr_len());
                }
                m_result.add(backslash);
                m_result.add(keyword_label.name, keyword_label.len);
                m_result.add(open_bracket);
                m_result.add(anchor.data(), anchor.length());
                m_result.add(close_bracket);
                m_result.add(open_brace);
                m_result.add(close_brace);
            }
            m_skip_element = &e;
            return;
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_label0) == 0)
        {
            // Invisible label - \label0
            //--------------------------
            m_buffer.clear();
            element_serializer(m_buffer, e, false, false);
            add_label(keyword_index, 
                      m_buffer.text(), m_buffer.length(), 
                      m_buffer.text(), m_buffer.length());
            m_result.add(backslash);
            m_result.add(keyword_label.name, keyword_label.len);
            m_result.add(open_bracket);
            m_result.add(m_buffer.text(), m_buffer.length());
            m_result.add(close_bracket);
            m_result.add(open_brace);
            m_result.add(close_brace);
            m_skip_element = &e;
            return;
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_table) == 0)  
        {
            // Tables
            //------------------
            add_label(keyword_table, e.attr(), e.attr_len());
        }
        else
        if(m_cfg.is_solid_element(e.name(), e.name_len()))
        {
            // Listings
            //------------------
            add_label(keyword_listing, e.attr(), e.attr_len());
            if(m_code_element == 0) m_code_element = &e;
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_imgc)   == 0 ||
           str_cmp(e.name(), e.name_len(), keyword_imgl)   == 0 ||
           str_cmp(e.name(), e.name_len(), keyword_imgr)   == 0 ||
           str_cmp(e.name(), e.name_len(), keyword_img)    == 0 ||
           str_cmp(e.name(), e.name_len(), keyword_imgl0)  == 0 ||
           str_cmp(e.name(), e.name_len(), keyword_imgr0)  == 0)
        {
            // Figures
            //------------------
            add_label(keyword_figure, e.attr(), e.attr_len(), &e);
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_title) == 0)
        {
            // Title
            //------------------
            m_buffer.clear();
            element_serializer(m_buffer, e, false, false);
            m_lab.add(backslash);
            m_lab.add(keyword_title.name, keyword_title.len);
            m_lab.add(open_brace);
            m_lab.add(m_buffer.text());
            m_lab.add(close_brace);
            m_lab.add(lf);
        }
        else
        if((tocn = is_toc_element(e.name(), e.name_len())) != 0)
        {
            // Generate Toc Anchor
            //----------------------
            string_type anchor;
            anchor.reserve(16);
            char buf[8];
            sprintf(buf, "%04d", ++m_toc_counter);
            const char* p = buf;
            while(*p) anchor.append(1, *p++);

            m_result.add(backslash);
            m_result.add(e.name(), e.name_len());
            m_result.add(open_bracket);
            m_result.add(e.attr(), e.attr_len());
            m_result.add(semicolon);
            m_result.add(space);
            m_result.add(keyword_anchor.name, keyword_anchor.len);
            m_result.add(equal);
            m_result.add(quote);
            m_result.add(keyword_toc.name, keyword_toc.len);
            m_result.add(anchor);
            m_result.add(quote);
            m_result.add(semicolon);
            m_result.add(close_bracket);
            m_result.add(open_brace);
        
            m_toc_offset += 4;
            m_toc.add(lf);
            unsigned i;
            for(i = 0; i < m_toc_offset; i++) m_toc.add(space);

            m_toc.add(backslash);
            m_toc.add(keyword_toc.name, keyword_toc.len);
            m_toc.add(toc_num_to_char(tocn));
            m_toc.add(open_bracket);
            m_toc.add(e.attr(), e.attr_len());
            m_toc.add(semicolon);
            m_toc.add(space);
            m_toc.add(keyword_anchor.name, keyword_anchor.len);
            m_toc.add(equal);
            m_toc.add(quote);
            m_toc.add(keyword_toc.name, keyword_toc.len);
            m_toc.add(anchor);
            m_toc.add(quote);
            m_toc.add(semicolon);
            m_toc.add(close_bracket);
            m_toc.add(open_brace);
            return;
        }
        m_result.add_element_header(e);
    }


    //------------------------------------------------------------------
    void label_collector::end_element(const element& e)
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

        if(is_toc_element(e.name(), e.name_len()))
        {
            m_toc.add(close_brace);
            m_toc_offset -= 4;
        }

        if(m_code_element && m_code_element == &e)
        {
            m_code_element = 0;
        }
    }


    //------------------------------------------------------------------
    void label_collector::content(const element& WXUNUSED(e), const char_type* c, unsigned len)
    {
        if(m_skip_element) return;
        if(m_code_element)
        {
            unsigned nminus = 0;
            unsigned nequal = 0;
            while(len)
            {
                if(*c == minus) 
                {
                    ++nminus;
                }
                else
                if(*c == equal) 
                {
                    ++nequal;
                }
                else
                {
                    if(is_alnum(*c) && (nminus > 4 || nequal > 4))
                    {
                        const char_type* end = c;
                        unsigned l2 = len;
                        while(l2 && is_reference_char(*end)) 
                        {
                            ++end;
                            --l2;
                        }
                        l2 = unsigned(end - c);
                        if(l2 > 1)
                        {
                            m_result.add(backslash);
                            m_result.add(keyword_label.name, keyword_label.len);
                            m_result.add(open_bracket);
                            m_result.add(c, l2);
                            m_result.add(close_bracket);
                            add_label(keyword_listing, c, l2, c, l2);
                        }
                    }
                    nminus = 0;
                    nequal = 0;
                }
                m_result.add(*c);
                ++c;
                --len;
            }
        }
        else
        {
            m_result.add(c, len);
        }
    }

}

