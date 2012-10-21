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

#ifndef AGDOC_TABLE_STRUCTURIZER_INCLUDED
#define AGDOC_TABLE_STRUCTURIZER_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{


    //------------------------------------------------------------------
    class table_content_extractor
    {
    public:
        table_content_extractor(const element& e,
                                content_storage& header,
                                content_storage& table,
                                string_buffer_type& align);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& e, const char_type* c, unsigned len);

    private:
        content_storage&    m_header;
        content_storage&    m_table;
        string_buffer_type& m_align;
        const element*      m_skip_element;
        unsigned            m_level;
    };






    //------------------------------------------------------------------
    class table_structurizer
    {
    public:
        table_structurizer(const config& cfg,
                           const element& e,
                           content_storage& result);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& e, const char_type* c, unsigned len);

    private:
        void process_ssv_table(const element& e);
        void process_vbar_table(const element& e);
        void process_eln_table(const element& e);
        void process_table_header(const element& e);
        void add_th(const char_type* str, unsigned len, const keyword* kw, unsigned ncol);
        void parse_double_vbar(const char_type* str, unsigned len, const keyword* kw);


        const config&      m_cfg;
        content_storage&   m_result;
        const element*     m_skip_element;
        content_storage    m_header;
        content_storage    m_table;
        string_buffer_type m_align;
    };


}

#endif

