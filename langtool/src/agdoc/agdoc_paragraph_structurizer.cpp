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

#include "agdoc_paragraph_structurizer.h"

namespace agdoc
{


    //==================================================================
    // 
    // paragraph_structurizer
    // 
    //==================================================================


    //------------------------------------------------------------------
    paragraph_structurizer::paragraph_structurizer(const config& cfg,
                                                   const element& e,
                                                   content_storage& result) :
        m_status(waiting_for_paragraph),
        m_cfg(cfg),
        m_result(result),
        m_skip_element(0)
    {
        m_result.reserve(e.total_len());
        e.process(*this);
    }



    //------------------------------------------------------------------
    void paragraph_structurizer::start_element(const element& e)
    {
        if(m_skip_element) return;

        if(is_paragraphless_element(e))
        {
            end_paragraph();
            m_skip_element = &e;
            write_element(e);
            return;
        }

        if(is_paragraph_br_style_element(e))
        {
            end_paragraph();
            m_skip_element = &e;
            write_br_style_element(e);
            return;
        }

        if(is_paragraph_structure_element(e))
        {
            end_paragraph();
            m_result.add_element_header(e);
        }
        else
        {
            start_paragraph();
            m_skip_element = &e;
            write_element(e);
        }
    }



    //------------------------------------------------------------------
    void paragraph_structurizer::end_element(const element& e)
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
            end_paragraph();
            m_result.add_element_footer(e);
        }
    }


    //------------------------------------------------------------------
    void paragraph_structurizer::content(const element& WXUNUSED(e), const char_type* c, unsigned len)
    {
        if(m_skip_element) return;

        while(len)
        {
            switch(m_status)
            {
            case waiting_for_paragraph:
                if(!is_space(*c))
                {
                    start_paragraph();
                }
                m_result.add(*c);
                break;

            case paragraph_started:
                if(is_empty_line(c, len))
                {
                    end_paragraph();
                }
                m_result.add(*c);
                break;
            }
            --len;
            ++c;
        }
    }



    //------------------------------------------------------------------
    void paragraph_structurizer::start_paragraph()
    {
        if(m_status != paragraph_started)
        {
            m_result.add(backslash);
            m_result.add(keyword_p.name, keyword_p.len);
            m_result.add(open_brace);
            m_status = paragraph_started;
        }
    }


    //------------------------------------------------------------------
    void paragraph_structurizer::end_paragraph()
    {
        if(m_status == paragraph_started)
        {
            m_result.add(close_brace);
            m_status = waiting_for_paragraph;
        }
    }


    //------------------------------------------------------------------
    void paragraph_structurizer::write_element(const element& e)
    {
        element_serializer s(m_result, e);
    }


    //------------------------------------------------------------------
    void paragraph_structurizer::write_br_style_element(const element& e)
    {
        content_storage storage;
        element_serializer s(storage, e);
        const char_type* str = storage.text();
        unsigned len = storage.length();
        while(len)
        {
            if(is_empty_line(str, len))
            {
                m_result.add(backslash);
                m_result.add(keyword_br.name, keyword_br.len);
                unsigned l = skip_lf(str, len, 1);
                len -= l;
                str += l;
            }
            m_result.add(*str);
            ++str;
            --len;
        }
    }


    //------------------------------------------------------------------
    bool paragraph_structurizer::is_paragraphless_element(const element& e) const
    {
        return m_cfg.keyword_exists(keyword_paragraphless_elements, e.name(), e.name_len());
    }


    //------------------------------------------------------------------
    bool paragraph_structurizer::is_paragraph_structure_element(const element& e) const
    {
        if(e.name_len() == 0) return true;
        return m_cfg.keyword_exists(keyword_paragraph_structure_elements, e.name(), e.name_len());
    }


    //------------------------------------------------------------------
    bool paragraph_structurizer::is_paragraph_br_style_element(const element& e) const
    {
        return m_cfg.keyword_exists(keyword_paragraph_br_style_elements, e.name(), e.name_len());
    }


}

