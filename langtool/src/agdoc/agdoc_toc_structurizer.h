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

#ifndef AGDOC_TOC_STRUCTURIZER_INCLUDED
#define AGDOC_TOC_STRUCTURIZER_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{

    //------------------------------------------------------------------
    class toc_structurizer
    {
    public:
        toc_structurizer(const config& cfg,
                         const element& e, 
                         content_storage& result);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element&, const char_type* c, unsigned len);

    private:
        const config&    m_cfg;
        content_storage& m_result;
        const element*   m_root_element;
        const element*   m_toc_element;
        int              m_toc_level;
    };


}

#endif

