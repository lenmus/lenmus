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

#ifndef AGDOC_INDEX_STORAGE_INCLUDED
#define AGDOC_INDEX_STORAGE_INCLUDED

#include "agdoc_basics.h"

namespace agdoc
{

    class index_storage
    {
    public:
        struct index_element
        {
            string_type category;
            string_type anchor;
            string_type comment;
            string_type full_path;
        };

        typedef index_element* key_element;

        struct anchor_less
        {
            bool operator() (key_element e1, key_element e2) const
            {
                return e1->anchor.compare(e2->anchor) < 0;
            }
        };

        struct category_less
        {
            bool operator() (key_element e1, key_element e2) const
            {
                int cat = e1->category.compare(e2->category);
                if(cat) return cat < 0;
                if(str_is(keyword_file.name, keyword_file.len, e1->category))
                {
                    return str_case_cmp(e1->full_path.c_str(), e2->full_path.c_str()) < 0;
                }
                return str_case_cmp(e1->anchor.c_str(), e2->anchor.c_str()) < 0;
//                return str_case_cmp(e1->comment.c_str(), e2->comment.c_str()) < 0;
            }
        };

        typedef std::multiset<key_element, anchor_less> key_anchor_type;
        typedef key_anchor_type::const_iterator iterator_anchor;

        typedef std::multiset<key_element, category_less> key_category_type;
        typedef key_category_type::const_iterator iterator_category;

    public:
        index_storage(const element& e);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& WXUNUSED(e), const char_type* WXUNUSED(c), unsigned WXUNUSED(len)) {}

        //-------------------------- Anchor key
        iterator_anchor begin_anchor(const string_type& anchor) const;
        iterator_anchor end_anchor(const string_type& anchor) const;

        const index_element* find_anchor(const string_type& anchor, 
                                         const string_type& category) const;

        iterator_anchor begin_anchor() const { return m_key_anchor.begin(); }
        iterator_anchor end_anchor()   const { return m_key_anchor.end();   }

        //-------------------------- Category + anchor key
        iterator_category begin_category(const string_type& cat) const;
        iterator_category end_category(const string_type& cat) const;

        iterator_category begin_category() const { return m_key_category.begin(); }
        iterator_category end_category()   const { return m_key_category.end();   }


    private:
        void make_full_path(string_type& ret) const;
        void extract_element_content(const element& e, string_type& ret) const;

        std::vector<string_type>   m_path;
        std::vector<index_element> m_data;
        key_anchor_type            m_key_anchor;
        key_category_type          m_key_category;
        string_type                m_file_name;
        string_type                m_file_ext;
    };


}

#endif
