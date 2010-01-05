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

#ifndef AGDOC_PARAGRAPH_STRUCTURIZER_INCLUDED
#define AGDOC_PARAGRAPH_STRUCTURIZER_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{
    //------------------------------------------------------------------
    class paragraph_structurizer
    {
        enum status
        {
            waiting_for_paragraph,
            paragraph_started
        };

    public:
        paragraph_structurizer(const config& cfg,
                               const element& e,
                               content_storage& result);


        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& e, const char_type* c, unsigned len);

    private:
        void start_paragraph();
        void end_paragraph();
        void write_element(const element& e);
        void write_br_style_element(const element& e);

        bool is_paragraphless_element(const element& e) const;
        bool is_paragraph_structure_element(const element& e) const;
        bool is_paragraph_br_style_element(const element& e) const;

        status           m_status;
        const config&    m_cfg;
        content_storage& m_result;
        const element*   m_skip_element; 
    };

}

#endif
