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

#include "agdoc_structure_processor.h"
#include "agdoc_content_processor.h"

namespace agdoc
{


    //==================================================================
    // 
    // content_processor
    // 
    //==================================================================


    //------------------------------------------------------------------
    content_processor::content_processor(const config& cfg, 
                                         const element& e,
                                         content_storage& result) :
        m_cfg(cfg),
        m_result(result),
        m_suppress_list(cfg.find_keywords(keyword_suppress_output_elements)),
        m_skip_element(0)
    {
        m_result.reserve(e.total_len());
        m_result.restore_keysym_flag(true);
        e.process(*this);
    }


    //------------------------------------------------------------------
    bool content_processor::is_suppress_element(const char_type* name, 
                                                unsigned name_len) const
    {
        if(m_suppress_list)
        {
            strset_type::const_iterator i = 
                m_suppress_list->find(string_type(name, name_len));

            if(i != m_suppress_list->end())
            {
                return true;
            }
        }
        return false;
    }


    //------------------------------------------------------------------
    void content_processor::start_element(const element& e)
    {
        if(m_skip_element) return;

        if(is_suppress_element(e.name(), e.name_len()))
        {
            m_skip_element = &e;
        }
    }



    //------------------------------------------------------------------
    void content_processor::end_element(const element& e)
    {
        if(m_skip_element)
        {
            if(m_skip_element == &e)
            {
                m_skip_element = 0;
            }
        }
    }



    //------------------------------------------------------------------
    void content_processor::content(const element& e, const char_type* s, unsigned len)
    {
        if(m_skip_element) return;
        m_result.add_str_with_variables(e, s, len, m_cfg);
    }

}

