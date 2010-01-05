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

#include "agdoc_index_storage.h"

namespace agdoc
{

    //==================================================================
    // 
    // index_storage
    // 
    //==================================================================

    //------------------------------------------------------------------
    index_storage::index_storage(const element& e)
    {
        e.process(*this);
        unsigned i;
        for(i = 0; i < m_data.size(); ++i)
        {
            m_key_anchor.insert(&m_data[i]);
            m_key_category.insert(&m_data[i]);
        }
    }



    //------------------------------------------------------------------
    index_storage::iterator_anchor 
    index_storage::begin_anchor(const string_type& anchor) const
    {
        index_element ie;
        ie.anchor = anchor;
        return m_key_anchor.lower_bound(&ie);
    }


    //------------------------------------------------------------------
    index_storage::iterator_anchor 
    index_storage::end_anchor(const string_type& anchor) const
    {
        index_element ie;
        ie.anchor = anchor;
        return m_key_anchor.upper_bound(&ie);
    }


    //------------------------------------------------------------------
    const index_storage::index_element* 
    index_storage::find_anchor(const string_type& anchor, 
                               const string_type& category) const
    {
        iterator_anchor beg = begin_anchor(anchor);
        if(beg == end_anchor()) return 0;
        iterator_anchor end = end_anchor(anchor);
        for(; beg != end; ++beg)
        {
            if((*beg)->category.compare(category) == 0) return *beg;
        }
        return 0;
    }


    //------------------------------------------------------------------
    index_storage::iterator_category 
    index_storage::begin_category(const string_type& cat) const
    {
        index_element ie;
        ie.category = cat;
        return m_key_category.lower_bound(&ie);
    }

    //------------------------------------------------------------------
    index_storage::iterator_category 
    index_storage::end_category(const string_type& cat) const
    {
        index_element ie;
        ie.category = cat;
        ie.anchor += max_char;
        ie.comment += max_char;
        ie.full_path += max_char;
        return m_key_category.upper_bound(&ie);
    }


    //------------------------------------------------------------------
    void index_storage::make_full_path(string_type& ret) const
    {
        ret.reserve(256);
        unsigned i;
        for(i = 0; i < m_path.size(); i++)
        {
            ret += m_path[i];
            if(m_path[i].length()) ret += slash;
        }
        ret += m_file_name;
        reduce_dot_dot(ret);
        ret += m_file_ext;
    }


    //------------------------------------------------------------------
    void index_storage::extract_element_content(const element& e, string_type& ret) const
    {
        ret.assign(e.content(), e.content_len());
        clean_and_trim_string(ret);
    }


    //------------------------------------------------------------------
    void index_storage::start_element(const element& e)
    {
        string_type ts;
        index_element ie;

        if(str_cmp(e.name(), e.name_len(), keyword_dir) == 0)
        {
            parse_and_find_attr_content(e.attr(), e.attr_len(), ts);
            clean_and_trim_string(ts);
            if(ts.empty())
            {
                e.throw_exception(e.name(), "index_storage: \\dir: No directory name");
            }
            trim_ending_slashes(ts);
            m_path.push_back(ts);
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_file) == 0)
        {
            parse_and_find_attr_content(e.attr(), e.attr_len(), ts);
            clean_and_trim_string(ts);
            if(ts.empty())
            {
                e.throw_exception(e.name(), "index_storage: \\file: No file name");
            }
            m_file_name = ts;
           
            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), keyword_ext, ts))
            {
                clean_and_trim_string(ts);
                m_file_ext = ts;
            }
            ie.category.assign(keyword_file.name, keyword_file.len);
            ie.anchor.assign(m_file_name);
            ie.comment.erase();
            ie.comment.assign(m_file_name);
            make_full_path(ie.full_path);
            m_data.push_back(ie);
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_title) == 0)
        {
            if(m_file_name.empty())
            {
                e.throw_exception(e.name(), "index_storage: \\title: Illegal outside of \\file");
            }
            extract_element_content(e, ts);
            ie.category.assign(keyword_title.name, keyword_title.len);
            ie.anchor.assign(m_file_name);
            ie.comment.assign(ts);
            make_full_path(ie.full_path);
            m_data.push_back(ie);
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_label) == 0)
        {
            if(m_file_name.empty())
            {
                e.throw_exception(e.name(), "index_storage: \\title: Illegal outside of \\file");
            }

            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), keyword_category, ts))
            {
                clean_and_trim_string(ts);
                if(ts.empty())
                {
                    e.throw_exception(e.name(), "index_storage: \\label: No 'category' attribute");
                }
                ie.category.assign(ts);
            }

            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), keyword_anchor, ts))
            {
                clean_and_trim_string(ts);
                if(ts.empty())
                {
                    e.throw_exception(e.name(), "index_storage: \\label: No 'anchor' attribute");
                }
                ie.anchor.assign(ts);
            }
            extract_element_content(e, ts);
            ie.comment.assign(ts);
            make_full_path(ie.full_path);
            m_data.push_back(ie);
        }
    }



    //------------------------------------------------------------------
    void index_storage::end_element(const element& e)
    {
        if(str_cmp(e.name(), e.name_len(), keyword_dir) == 0)
        {
            if(m_path.size()) m_path.pop_back();
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_file) == 0)
        {
            m_file_name.erase();
            m_file_ext.erase();
        }
    }

}
