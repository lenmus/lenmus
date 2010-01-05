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

#ifndef AGDOC_LINK_STRUCTURIZER_INCLUDED
#define AGDOC_LINK_STRUCTURIZER_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{


    //------------------------------------------------------------------
    class link_structurizer
    {
    public:
        link_structurizer(const config& cfg,
                          const element& e,
                          content_storage& result);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& e, const char_type* c, unsigned len);

    private:
        bool is_linking_keyword(const char_type* str, unsigned len) const;

        bool is_link(const char_type* start, 
                     const char_type* str, 
                     unsigned len, 
                     unsigned* ret_len);

        void add_img(const char_type* str, unsigned len, const keyword& img);
        void add_link(const char_type* str, unsigned len);

        const config&    m_cfg;
        content_storage& m_result;
        const element*   m_skip_element;
    };


}

#endif
