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

#include "agdoc_toc_structurizer.h"

namespace agdoc
{

    //==================================================================
    // 
    // toc_structurizer
    // 
    //==================================================================



    //------------------------------------------------------------------
    toc_structurizer::toc_structurizer(const config& cfg,
                                       const element& e, 
                                       content_storage& result) :
        m_cfg(cfg),
        m_result(result),
        m_root_element(0),
        m_toc_element(0),
        m_toc_level(0)
    {
        m_result.reserve(e.total_len());
        e.process(*this);
    }



    //------------------------------------------------------------------
    void toc_structurizer::start_element(const element& e)
    {
        int level = is_toc_element(e.name(), e.name_len());
        if(level)
        {
            if(m_root_element == 0)
            {
                m_root_element = e.parent();
            }
            else
            {
                if(m_root_element != e.parent())
                {
                    e.throw_exception(e.name(), "All TOC elements must belong to the same root element");
                }
            }

            if(level > m_toc_level)
            {
                if(level != m_toc_level + 1)
                {
                    e.throw_exception(e.name(), "TOC structure violation (h1/h2/h3...)");
                }
            }
            else
            {
                m_result.add(close_brace);
                while(m_toc_level > level)
                {
                    m_result.add(close_brace);
                    --m_toc_level;
                }
            }

            content_storage cap;
            element_serializer ser(cap, e, false);
            m_result.add(backslash);
            m_result.add(e.name(), e.name_len());
            m_result.add(open_bracket);
            m_result.add(cap.text());
            m_result.add(close_bracket);
            m_result.add(open_brace);
            m_toc_level = level;
            m_toc_element = &e;
        }
        else
        {
            m_result.add_element_header(e);
        }
    }



    //------------------------------------------------------------------
    void toc_structurizer::end_element(const element& e)
    {
        if(m_root_element && m_root_element == &e)
        {
            while(m_toc_level)
            {
                m_result.add(close_brace);
                --m_toc_level;
            }
        }
        if(m_toc_element == 0)
        {
            if(e.name_len()) m_result.add(close_brace);
        }
        else
        {
            if(m_toc_element == &e)
            {
                m_toc_element = 0;
            }
        }
    }



    //------------------------------------------------------------------
    void toc_structurizer::content(const element&, const char_type* c, unsigned len)
    {
        if(m_toc_element == 0)
        {
            m_result.add(c, len);
        }
    }


}

