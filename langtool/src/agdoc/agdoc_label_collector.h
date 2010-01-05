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

#ifndef AGDOC_LABEL_COLLECTOR_INCLUDED
#define AGDOC_LABEL_COLLECTOR_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{

    //------------------------------------------------------------------
    class label_collector
    {
    public:
        label_collector(const config& cfg,
                        const element& e,
                        content_storage& result,
                        content_storage& labels,
                        content_storage& toc);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& e, const char_type* c, unsigned len);

    private:
        void add_label(const keyword& category, 
                       const char_type* anchor, unsigned anchor_len, 
                       const char_type* comment, unsigned comment_len);

        void add_label(const keyword& category, 
                       const char_type* attr, unsigned attr_len, 
                       const element* e = 0);


        const config&    m_cfg;
        content_storage& m_result;
        content_storage& m_lab;
        content_storage& m_toc;
        unsigned         m_toc_counter;
        unsigned         m_toc_offset;
        content_storage  m_buffer;
        const element*   m_skip_element;
        const element*   m_code_element;
    };

}

#endif
