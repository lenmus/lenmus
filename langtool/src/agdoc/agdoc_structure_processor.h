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

#ifndef AGDOC_STRUCTURE_PROCESSOR_INCLUDED
#define AGDOC_STRUCTURE_PROCESSOR_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{
    //------------------------------------------------------------------
    class structure_processor
    {
    public:
        structure_processor(const config& cfg, 
                            const element& e,
                            content_storage& result);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& e, const char_type*, unsigned len);

    private:
        const config&    m_cfg;
        content_storage& m_result;
    };

}

#endif
