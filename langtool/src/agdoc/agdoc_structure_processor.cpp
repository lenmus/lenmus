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

namespace agdoc
{

    //==================================================================
    // 
    // structure_processor
    // 
    //==================================================================


    //------------------------------------------------------------------
    structure_processor::structure_processor(const config& cfg, 
                                             const element& e,
                                             content_storage& result) :
        m_cfg(cfg),
        m_result(result)
    {
        m_result.reserve(e.total_len());
        e.process(*this);
    }


    //------------------------------------------------------------------
    void structure_processor::start_element(const element& e)
    {
        const string_type* p;
        if(e.name_len())
        {
            m_result.add_element_header(e);

            p = m_cfg.find(e.name(), e.name_len(), keyword_open_suffix);
            m_result.add(p);

            m_result.add(backslash);
            m_result.add(keyword_cntn.name, keyword_cntn.len);
            m_result.add(open_brace);
        }
        else
        {
            p = m_cfg.find(keyword_page_open.name);
            m_result.add(p);
        }
    }


    
    //------------------------------------------------------------------
    void structure_processor::end_element(const element& e)
    {
        const string_type* p;
        if(e.name_len())
        {
            m_result.add_element_footer(e);

            p = m_cfg.find(e.name(), e.name_len(), keyword_close_suffix);
            m_result.add(p);
            m_result.add(close_brace);
        }
        else
        {
            p = m_cfg.find(keyword_page_close.name);
            m_result.add(p);
        }
    }



    //------------------------------------------------------------------
    void structure_processor::content(const element& WXUNUSED(e), const char_type* c, unsigned len)
    {
        m_result.add(c, len);
    }

}

