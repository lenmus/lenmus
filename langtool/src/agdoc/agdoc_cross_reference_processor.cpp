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

#include "agdoc_cross_reference_processor.h"

namespace agdoc
{


    //------------------------------------------------------------------
    void tokenize_path(const string_type& name, std::vector<string_type>& path)
    {
        const char* pn = name.c_str();
        const char* pp = name.c_str();
        while(*pn)
        {
            if(*pn == slash) 
            {
                path.push_back(string_type(pp, unsigned(pn - pp)));
                while(*pn && *pn == slash) ++pn; // Skip all consecutive slashes
                pp = pn;
            }
            else
            {
                ++pn;
            }
        }
        if(pn > pp) path.push_back(string_type(pp, unsigned(pn - pp)));
    }


    //------------------------------------------------------------------
    const element* find_element(const element* e, const keyword& el_name)
    {
        unsigned i;
        for(i = 0; i < e->num_elements(); ++i)
        {
            const element& e2 = e->at(i);
            if(str_cmp(e2.name(), e2.name_len(), el_name) == 0)
            {
                return &e2;
            }
        }
        return 0;
    }



    //------------------------------------------------------------------
    const element* find_element(const element* e, const keyword& el_name, 
                               const string_type& attr_name)
    {
        unsigned i;
        for(i = 0; i < e->num_elements(); ++i)
        {
            const element& e2 = e->at(i);
            if(str_cmp(e2.name(), e2.name_len(), el_name) == 0 && e2.attr_len())
            {
                string_type attr;
                parse_and_find_attr_content(e2.attr(), e2.attr_len(), attr);
                if(str_cmp(attr, attr_name) == 0)
                {
                    return &e2;
                }
            }
        }
        return 0;
    }




    //==================================================================
    // 
    // cross_reference_processor
    // 
    //==================================================================


    //------------------------------------------------------------------
    cross_reference_processor::cross_reference_processor(const config& cfg, 
                                                         const element& e, 
                                                         content_storage& result,
                                                         const index_storage& idx,
                                                         const element& local_toc,
                                                         const element& global_toc,
                                                         const string_type& self_name) :
        m_cfg(cfg),
        m_result(result),
        m_index(idx),
        m_local_toc(local_toc),
        m_global_toc(global_toc),
        m_self_name(self_name),
        m_skip_element(0)
    {
        m_result.reserve(e.total_len());
        e.process(*this);
    }



    //------------------------------------------------------------------
    const element* cross_reference_processor::find_toc(const string_type& name, 
                                                       string_type& title, 
                                                       string_type& file)
    {
        std::vector<string_type> path;

        // Make path. First, add the path to the self-name
        // (removing the file name itsef), then add the path 
        // to the destination name, keeping the destination file name
        //----------------------
        tokenize_path(m_self_name, path);
        if(path.size()) path.pop_back(); // Remove file name;
        tokenize_path(name, path);

        if(path.size() == 0) return 0;

        unsigned i;
        const element* toc = &m_global_toc;

        file.erase();
        file.reserve(256);

        // Look for the directory
        //------------------------
        for(i = 0; i < path.size() - 1; i++)
        {
            if(str_cmp(path[i], keyword_dot_dot) == 0)
            {
                toc = toc->parent();
            }
            else
            {
                toc = find_element(toc, keyword_dir, path[i]);
            }
            if(toc == 0) return 0;
        }

        toc = find_element(toc, keyword_file, path[path.size() - 1]);

        if(toc == 0) return 0;

        file = name;
        
        string_type ext;
        if(parse_and_find_attr_variable(toc->attr(), toc->attr_len(), 
                                        keyword_ext, 
                                        ext))
        {
            file += ext;
        }

        const element* ttl = find_element(toc, keyword_title);
        if(ttl)
        {
            title.assign(ttl->content(), ttl->content_len());
        }
        else
        {
            title = file;
        }

        return find_element(toc, keyword_toc);
    }


    //------------------------------------------------------------------
    void cross_reference_processor::start_element(const element& e)
    {
        m_result.add_element_header(e);
        if(m_skip_element == 0 &&
          (str_cmp(e.name(), e.name_len(), keyword_label) == 0 ||
           str_cmp(e.name(), e.name_len(), keyword_as_is) == 0))
        {
            m_skip_element = &e;
        }

        if(str_cmp(e.name(), e.name_len(), keyword_toc) == 0)
        {
            if(e.attr_len() == 0)
            {
                element_serializer(m_result, m_local_toc, false);
            }
            else
            {
                string_type title;
                string_type file;
                const element* toc = find_toc(string_type(e.attr(), e.attr_len()), 
                                              title, 
                                              file);
                if(toc)
                {
                    m_result.add(lf);
                    m_result.add(backslash);
                    m_result.add(keyword_toc0.name, keyword_toc0.len);
                    m_result.add(open_bracket);
                    m_result.add(title);
                    m_result.add(semicolon);
                    m_result.add(space);
                    m_result.add(keyword_file.name, keyword_file.len);
                    m_result.add(equal);
                    m_result.add(file);
                    m_result.add(semicolon);
                    m_result.add(close_bracket);
                    m_result.add(open_brace);
                    m_result.add(lf);
                    element_serializer(m_result, 
                                       *toc, 
                                       false, 
                                       true, 
                                       &keyword_file,
                                       file.c_str());
                    m_result.add(lf);
                    m_result.add(close_brace);
                }
                else
                {
                    e.throw_exception(e.attr(), 
                                      "Table of Content not Found: ", 
                                      string_type(e.attr(), e.attr_len()).c_str());
                }
            }
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_index) == 0)
        {
            if(e.attr_len())
            {
                add_index(e.attr(), e.attr_len());
            }
        }
    }


    //------------------------------------------------------------------
    void cross_reference_processor::end_element(const element& e)
    {
        m_result.add_element_footer(e);
        if(m_skip_element && m_skip_element == &e)
        {
            m_skip_element = 0;
        }
    }



    //------------------------------------------------------------------
    void cross_reference_processor::add_reference(const index_storage::index_element& ie, 
                                                  bool path_as_a_comment)
    {
        string_type path;
        calc_relative_name_to(m_self_name.c_str(), ie.full_path.c_str(), path);

        if(path.empty())
        {
            extract_file_name(ie.full_path.c_str(), path);
        }

        m_result.add(backslash);
        m_result.add(keyword_iref.name, keyword_iref.len);
        m_result.add(underscore);
        m_result.add(ie.category);
        m_result.add(open_bracket);
        m_result.add(keyword_file.name, keyword_file.len);
        m_result.add(equal);
        m_result.add(quote);
        m_result.add(path);
        m_result.add(quote);
        m_result.add(semicolon);
        m_result.add(space);
        m_result.add(keyword_anchor.name, keyword_anchor.len);
        m_result.add(equal);
        m_result.add(quote);
        m_result.add(ie.anchor);
        m_result.add(quote);
        m_result.add(semicolon);
        m_result.add(close_bracket);
        m_result.add(open_brace);
        m_result.add(path_as_a_comment ? ie.full_path : ie.comment);
        m_result.add(close_brace);
    }




    //------------------------------------------------------------------
    void cross_reference_processor::add_index(const char_type* category,
                                              unsigned category_len)
    {
        string_type cat(category, category_len);
        index_storage::iterator_category beg = m_index.begin_category(cat);
        bool path_as_a_comment = str_cmp(category, category_len, keyword_file) == 0;
        if(beg != m_index.end_category())
        {
            index_storage::iterator_category end = m_index.end_category(cat);
            while(beg != end)
            {
                add_reference(**beg, path_as_a_comment);
                m_result.add(backslash);
                m_result.add(keyword_br.name, keyword_br.len);
                m_result.add(lf);
                ++beg;
            }
        }
    }





    //------------------------------------------------------------------
    void cross_reference_processor::process_reference(const char_type* str, 
                                                      unsigned len, 
                                                      unsigned* ret_len)
    {
        const char_type* start = str;
        bool found = false;
        while(len && is_reference_char(*str)) 
        {
            --len;
            ++str;
        }
        if(str > start + 1)
        {
            while(!is_element_name(*(str - 1))) // Trim all ending punctuations
            {
                ++len;
                --str;
            }

            bool code_preference = false;
            if(len >= 2)
            {
                if(str[0] == open_brace && str[1] == close_brace)
                {
                    code_preference = true;
                }
            }
            string_type ref_name(start, unsigned(str - start));

            index_storage::iterator_anchor beg = m_index.begin_anchor(ref_name);
            if(beg != m_index.end_anchor())
            {
                index_storage::iterator_anchor end = m_index.end_anchor(ref_name);
                const index_storage::index_element* first = 0;
                const index_storage::index_element* prefer = 0;
                for(; beg != end; ++beg)
                {
                    if(first == 0) first = *beg;
                    if(prefer == 0)
                    {
                        if((str_cmp((*beg)->category, keyword_listing) == 0) == code_preference)
                        {
                            prefer = *beg;
                        }
                    }
                }
                const index_storage::index_element* ie = prefer;
                if(ie == 0) ie = first;
                if(ie)
                {
                    add_reference(*ie);
                    found = true;
                    *ret_len = unsigned(str - start);
                    if(code_preference) *ret_len += 2;
                }
            }
        }
        if(!found)
        {
            const char_type* start2 = start;
            while(start < str)
            {
                if(!is_element_name(*start)) break;
                m_result.add(*start);
                ++start;
            }
            *ret_len = unsigned(start - start2);
        }
    }



    //------------------------------------------------------------------
    void cross_reference_processor::content(const element&, 
                                            const char_type* str, 
                                            unsigned len)
    {
        if(m_skip_element)
        {
            m_result.add(str, len);
            return;
        }

        while(len)
        {
            unsigned n = 1;

            if(is_alpha(*str))
            {
                process_reference(str, len, &n);
            }
            else
            {
                m_result.add(*str);
            }
            str += n;
            len -= n;
        }
    }

}




