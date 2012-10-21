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

#ifndef AGDOC_CROSS_REFERENCE_PROCESSOR_INCLUDED
#define AGDOC_CROSS_REFERENCE_PROCESSOR_INCLUDED

#include "agdoc_basics.h"
#include "agdoc_index_storage.h"

namespace agdoc
{

    //------------------------------------------------------------------
    class cross_reference_processor
    {
    public:
        cross_reference_processor(const config& cfg, 
                                  const element& e, 
                                  content_storage& result,
                                  const index_storage& idx,
                                  const element& local_toc,
                                  const element& global_toc,
                                  const string_type& self_name);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& e, const char_type* c, unsigned len);

    private:
        const element* find_toc(const string_type& name, string_type& title, string_type& file);
        void process_reference(const char_type* str, unsigned len, unsigned* ret_len);
        void add_reference(const index_storage::index_element& ie, bool path_as_a_comment = false);
        void add_index(const char_type* category, unsigned category_len);

    private:
        const config&        m_cfg;
        content_storage&     m_result;
        const index_storage& m_index;
        const element&       m_local_toc;
        const element&       m_global_toc;
        string_type          m_self_name;
        const element*       m_skip_element;
    };


}

#endif

