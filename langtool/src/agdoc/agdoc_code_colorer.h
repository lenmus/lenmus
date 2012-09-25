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

#ifndef AGDOC_CODE_COLORER_INCLUDED
#define AGDOC_CODE_COLORER_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{

    //------------------------------------------------------------------
    class code_colorer
    {
    public:
        code_colorer(const config& cfg, 
                     const element& e,
                     content_storage& result);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element&, const char_type* c, unsigned len);

    private:
        void color_code(const element& e);
        void color_code(const char_type* p);

        void add_to_identifier_charset(const keyword& suffix);
        void replace_strlist_keysym(string_list& strlist);

        bool is_block_comment(const char_type* p, unsigned* len) const;
        bool is_line_comment(const char_type* p, unsigned* len) const;
        bool is_string_literal(const char_type* p, unsigned* len) const;
        bool is_number(const char_type* p, unsigned* len) const;
        bool is_operator(const char_type* p, unsigned* len) const;
        bool is_identifier(const char_type* p, unsigned* len) const;
        bool is_keyword(const keyword& category, const char_type* p, unsigned len) const;
        char_type is_quotation(char_type c) const;

        const config&    m_cfg;
        content_storage& m_result;
        const element*   m_coloring_element;
        string_type      m_language;
        string_list      m_block_comments;
        string_list      m_line_comments;
        string_list      m_string_quotation;
        string_list      m_string_mask;
        string_list      m_operators;
        charset_type     m_identifier_charset;
        charset_type     m_operator_charset;
    };


}

#endif

