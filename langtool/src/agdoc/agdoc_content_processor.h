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

#ifndef AGDOC_CONTENT_PROCESSOR_INCLUDED
#define AGDOC_CONTENT_PROCESSOR_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{

    //------------------------------------------------------------------
    class content_processor
    {
    public:
        content_processor(const config& cfg, 
                          const element& e,
                          content_storage& result);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element&, const char_type* c, unsigned len);

    private:
        bool is_suppress_element(const char_type* name, unsigned name_len) const;

        const config&      m_cfg;
        content_storage&   m_result;
        const strset_type* m_suppress_list;
        const element*     m_skip_element;
    };

}

#endif
