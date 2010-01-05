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

#ifndef AGDOC_PAIR_QUOTE_STRUCTURIZER_INCLUDED
#define AGDOC_PAIR_QUOTE_STRUCTURIZER_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{


    //------------------------------------------------------------------
    class pair_quote_structurizer
    {
        struct pair_quote_element
        {
            string_type name;
            const element* e;
        };

    public:
        pair_quote_structurizer(const config& cfg,
                                const element& e,
                                content_storage& result);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& e, const char_type* c, unsigned len);


    private:
        const string_type* is_pair_quote_replacement(const char_type* str, 
                                                    unsigned len, 
                                                    unsigned* ret_len) const;
        bool pair_quote_started(const element& e, const string_type& repl) const;
        void start_formatting(const element& e, const string_type& repl);
        void end_formatting(const element& e, 
                            const char_type* ptr, 
                            const string_type& repl);

        const config&                   m_cfg;
        const string_list&              m_pair_quotes;
        content_storage&                m_result;
        const element*                  m_skip_element;
        std::vector<pair_quote_element> m_stack;
        charset_type                    m_charset;
    };


}

#endif


