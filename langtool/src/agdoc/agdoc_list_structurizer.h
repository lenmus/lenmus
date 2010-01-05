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

#ifndef AGDOC_LIST_STRUCTURIZER_INCLUDED
#define AGDOC_LIST_STRUCTURIZER_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{


    //------------------------------------------------------------------
    class list_structurizer
    {
        enum list_type
        {
            bullet_list,
            ordered_list
        };

        struct list_info
        {
            list_type type;
            unsigned char offset;
        };

    public:
        list_structurizer(const config& cfg,
                          const element& e,
                          content_storage& result);


        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& e, const char_type* c, unsigned len);

    private:
        void complete_lists();
        void write_element(const element& e);

        unsigned detect_list_item(const char_type* str, unsigned len, 
                                  char_type* lt, unsigned* skip);
        void     start_list(char_type lt, unsigned offset);
        void     start_item();
        void     end_list();
        void     end_item();

        bool is_paragraphless_element(const element& e) const;
        bool is_paragraph_structure_element(const element& e) const;
        bool is_paragraph_br_style_element(const element& e) const;

        const config&          m_cfg;
        content_storage&       m_result;
        const element*         m_skip_element; 
        std::vector<list_info> m_list_info;
    };


}

#endif
