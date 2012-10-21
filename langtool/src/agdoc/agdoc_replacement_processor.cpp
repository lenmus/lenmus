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

#include "agdoc_replacement_processor.h"

namespace agdoc
{


    //==================================================================
    // 
    // replacement_processor
    // 
    //==================================================================



    //------------------------------------------------------------------
    replacement_processor::replacement_processor(const config& cfg,
                                                 const element& e,
                                                 content_storage& result) :
        m_cfg(cfg),
        m_result(result),
        m_skip_element(0),
        m_replace_element(0),
        m_replaced(0)
    {
        m_result.reserve(e.total_len());
        e.process(*this);
    }



    //------------------------------------------------------------------
    void replacement_processor::start_element(const element& e)
    {
        if(m_replace_element != 0 || m_replaced != 0) return;

        if(m_skip_element) 
        {
            content_storage s;
            element_serializer(s, e);
            m_replaces[string_type(e.name(), e.name_len())] = s.text();
            m_replace_element = &e;
            return;
        }

        if(str_cmp(e.name(), e.name_len(), keyword_replace) == 0)
        {
            m_skip_element = &e;
            return;
        }

        strmap_type::const_iterator it = 
            m_replaces.find(string_type(e.name(), e.name_len()));
        if(it != m_replaces.end())
        {
            m_result.add(it->second);
            m_replaced = &e;
            return;
        }

        m_result.add_element_header(e);
    }



    //------------------------------------------------------------------
    void replacement_processor::end_element(const element& e)
    {
        if(m_skip_element != 0 || m_replace_element != 0 || m_replaced != 0)
        {
            if(m_replaced        && m_replaced == &e)        m_replaced = 0;
            if(m_replace_element && m_replace_element == &e) m_replace_element = 0;
            if(m_skip_element && m_skip_element == &e)       m_skip_element = 0;
            return;
        }
        m_result.add_element_footer(e);
    }



    //------------------------------------------------------------------
    void replacement_processor::content(const element& WXUNUSED(e), 
                                        const char_type* c, 
                                        unsigned len)
    {
        if(m_skip_element != 0 || m_replace_element != 0 || m_replaced != 0) return;
        m_result.add(c, len);
    }


}

