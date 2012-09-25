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

#ifndef AGDOC_SINGLETON_STRUCTURIZER_INCLUDED
#define AGDOC_SINGLETON_STRUCTURIZER_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{

    //------------------------------------------------------------------
    class singleton_structurizer
    {
    public:
        singleton_structurizer(const config& cfg,
                               const element& e,
                               content_storage& result);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& e, const char_type* c, unsigned len);


    private:
        const string_type* is_output_specific_replacement(const char_type* str, 
                                                          unsigned len, 
                                                          unsigned* ret_len) const;

        const string_type* is_singleton(const char_type* str, 
                                        unsigned len, 
                                        unsigned* ret_len) const;

        const config&      m_cfg;
        const string_list& m_output_specific_replacements;
        const string_list& m_singletons;
        content_storage&   m_result;
        const element*     m_skip_element;
        charset_type       m_charset;
    };


}

#endif

