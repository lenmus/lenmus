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


#include "agdoc_basics.h"
#include "agdoc_code_colorer.h"
#include "agdoc_content_processor.h"
#include "agdoc_cross_reference_processor.h"
#include "agdoc_index_storage.h"
#include "agdoc_label_collector.h"
#include "agdoc_link_structurizer.h"
#include "agdoc_list_structurizer.h"
#include "agdoc_pair_quote_structurizer.h"
#include "agdoc_paragraph_structurizer.h"
#include "agdoc_replacement_processor.h"
#include "agdoc_singleton_structurizer.h"
#include "agdoc_structure_processor.h"
#include "agdoc_table_structurizer.h"
#include "agdoc_toc_structurizer.h"
#include "agdoc_project_processor.h"



namespace agdoc
{

    //==================================================================
    // 
    // project_processor
    // 
    //==================================================================


    //------------------------------------------------------------------
    project_processor::~project_processor()
    {
        delete m_cfg;
    }


    //------------------------------------------------------------------
    project_processor::project_processor(const char_type* file_name, 
                                         log_file& log,
                                         const index_storage* index, 
                                         const element* index_src,
                                         bool quick_mode) :
        m_index(index),
        m_index_src(index_src),
        m_quick_mode(quick_mode),
        m_log(log),
        m_project_dir(path_prefix(file_name)),
        m_cfg(0),
        m_element_level(0)
    {
        ifile f(file_name);
        f.elements().process(*this);
    }


    //------------------------------------------------------------------
    string_type project_processor::make_input_file_name(const char_type* short_fname) const
    {
        string_type ret;
        ret.reserve(256);
        ret = m_project_dir;
        if(ret.length()) ret += slash;
        unsigned i;
        for(i = 0; i < m_input_path.size(); i++)
        {
            ret += m_input_path[i];
            if(m_input_path[i].length()) ret += slash;
        }
        ret += short_fname;
        trim_ending_slashes(ret);
        reduce_dot_dot(ret);
        return ret;
    }


    //------------------------------------------------------------------
    string_type project_processor::make_input_file_name(const string_type& short_fname) const
    {
        return make_input_file_name(short_fname.c_str());
    }

    //------------------------------------------------------------------
    string_type project_processor::make_input_file_name() const
    {
        const string_type empty_str;
        return make_input_file_name(empty_str);
    }

    //------------------------------------------------------------------
    std::string project_processor::make_output_file_name(const char_type* short_fname) const
    {
        string_type ret;
        ret.reserve(256);
        ret = m_project_dir;
        if(ret.length()) ret += slash;
        ret += m_output_dir;
        if(m_output_dir.length()) ret += slash;
        unsigned i;
        for(i = 0; i < m_output_path.size(); i++)
        {
            ret += m_output_path[i];
            if(m_output_path[i].length()) ret += slash;
        }
        ret += short_fname;
        trim_ending_slashes(ret);
        reduce_dot_dot(ret);
        return ret;
    }


    //------------------------------------------------------------------
    string_type project_processor::make_output_file_name(const string_type& short_fname) const
    {
        return make_output_file_name(short_fname.c_str());
    }

    //------------------------------------------------------------------
    string_type project_processor::make_output_file_name() const
    {
        const string_type empty_str;
        return make_output_file_name(empty_str);
    }

    //------------------------------------------------------------------
    void project_processor::verify_mandatory_elements(const element& e) const
    {
        if(m_cfg == 0)
        {
            e.throw_exception(e.name(), "project_processor: No confing file defined");
        }
        if(m_ext.size() == 0)
        {
            e.throw_exception(e.name(), "project_processor: No output extensions defined");
        }
        if(m_output_dir.empty())
        {
            e.throw_exception(e.name(), "project_processor: No output directory defined");
        }
    }


    //------------------------------------------------------------------
    void project_processor::create_output_dir(const element& e) const
    {
        if(m_index == 0 || m_index_src == 0) return;

        string_type name = make_output_file_name();
        if(name.empty())
        {
            e.throw_exception(e.name(), "project_processor: Directory name is empty");
        }

        stat_e stat = name_stat(name.c_str());
        if(stat == file_exists)
        {
            e.throw_exception(e.name(), 
                              "project_processor: Cannot create directory, a file exists: '%s'", 
                              name.c_str());
        }
        else
        if(stat == not_found)
        {
            if(!make_dir(name.c_str()))
            {
                e.throw_exception(e.name(), 
                                  "project_processor: Cannot create directory: '%s'",
                                  name.c_str());
            }
            m_log.write("Created directory: '%s'", name.c_str());
        }
    }


    //------------------------------------------------------------------
    bool project_processor::needs_processing(const string_type& iname, 
                                             const string_type& oname)
    {
        if(m_quick_mode)
        {
            time_t itime;
            time_t otime;
            if(name_stat(iname.c_str(), &itime) == file_exists)
            {
                if(name_stat(oname.c_str(), &otime) == file_exists)
                {
                    if(otime > itime) return false;
                }
            }
        }
        return true;
    }




    //------------------------------------------------------------------
    void project_processor::process_file(const string_type& file_name, 
                                         const string_type& ext, 
                                         bool  variables_only, 
                                         bool  trim_lines, 
                                         bool  remove_eof, 
                                         const char_type* lang)
    {
        content_storage buf1;
        content_storage buf2;
        content_storage toc;
        content_storage labels;

        string_type iname = make_input_file_name(file_name);
        string_type oname = make_output_file_name(file_name);
        oname += ext;

        if(!needs_processing(iname, oname)) return;

        ifile srcf(iname.c_str(), 
                   m_cfg->solid_elements(), 
                   trim_lines, 
                   remove_eof);

        if(lang)
        {
            content_storage tbuf(m_cfg->solid_elements());
            tbuf.add(backslash);
            tbuf.add(lang);
            tbuf.add(open_brace);
            tbuf.add(srcf.text());
            tbuf.add(close_brace);
            element_serializer(buf1, tbuf.elements());
        }
        else
        {
            element_serializer(buf1, srcf.elements());
        }

        if(variables_only)
        {
            if(m_index && m_index_src)
            {
                content_processor(*m_cfg, buf1.elements(), buf2);
                write_content_storage(oname.c_str(), buf2);
                m_log.write("Processed file: '%s' to '%s'", 
                            make_input_file_name(file_name).c_str(), 
                            oname.c_str());
            }
            return;
        }

//buf1.elements().dump_all(stdout);
        toc_structurizer        (*m_cfg, buf1.elements(), buf2);
//buf2.elements().dump_all(stdout);
        code_colorer code       (*m_cfg, buf2.elements(), buf1);
//buf1.elements().dump_all(stdout);
        replacement_processor   (*m_cfg, buf1.elements(), buf2);
//buf2.elements().dump_all(stdout);
        table_structurizer      (*m_cfg, buf2.elements(), buf1);
//buf1.elements().dump_all(stdout);
        link_structurizer       (*m_cfg, buf1.elements(), buf2);
//buf2.elements().dump_all(stdout);
        pair_quote_structurizer (*m_cfg, buf2.elements(), buf1);
//buf1.elements().dump_all(stdout);
        singleton_structurizer  (*m_cfg, buf1.elements(), buf2);
//buf2.elements().dump_all(stdout);
        list_structurizer       (*m_cfg, buf2.elements(), buf1);
//buf1.elements().dump_all(stdout);
        paragraph_structurizer  (*m_cfg, buf1.elements(), buf2);
//buf2.elements().dump_all(stdout);
        label_collector         (*m_cfg, buf2.elements(), buf1, labels, toc);
//buf1.elements().dump_all(stdout);
        if(m_index && m_index_src)
        {
            string_type rel_oname;
            unsigned i;
            for(i = 0; i < m_output_path.size(); i++)
            {
                rel_oname += m_output_path[i];
                if(m_output_path[i].length()) rel_oname += slash;
            }
            rel_oname += file_name;
            rel_oname += ext;
            cross_reference_processor(*m_cfg, 
                                      buf1.elements(), 
                                      buf2,
                                      *m_index,
                                      toc.elements(),
                                      *m_index_src,
                                      rel_oname);
            buf1.assign(buf2);
//buf1.elements().dump_all(stdout);
        }
        structure_processor     (*m_cfg, buf1.elements(), buf2);
//buf2.elements().dump_all(stdout);
        if(m_index && m_index_src)
        {
            content_processor   (*m_cfg, buf2.elements(), buf1);
//buf1.elements().dump_names(stdout);

            write_content_storage(oname.c_str(), buf1);
            m_log.write("Wrote file: '%s' to '%s'", 
                        make_input_file_name(file_name).c_str(), 
                        oname.c_str());
        }
        else
        {
            m_index_content.add(backslash);
            m_index_content.add(keyword_file.name, keyword_file.len);
            m_index_content.add(open_bracket);
            m_index_content.add(file_name);
            m_index_content.add(semicolon);
            m_index_content.add(space);
            m_index_content.add(keyword_ext.name, keyword_ext.len);
            m_index_content.add(equal);
            m_index_content.add(ext);
            m_index_content.add(semicolon);
            m_index_content.add(close_bracket);
            m_index_content.add(open_brace);
            m_index_content.add(lf);
            toc.restore_text_keysym();
            m_index_content.add(toc.text());
            m_index_content.add(lf);
            labels.restore_text_keysym();
            m_index_content.add(labels.text());
            m_index_content.add(lf);
            m_index_content.add(close_brace);
            m_index_content.add(lf);

            m_log.write("Processed file: '%s'", 
                        make_input_file_name(file_name).c_str());

        }
    }



    //------------------------------------------------------------------
    void project_processor::copy_file_as_is(const element& e, const string_type& file_name)
    {
        string_type iname = make_input_file_name(file_name);
        string_type oname = make_output_file_name(file_name);
        if(!needs_processing(iname, oname)) return;

        if(!copy_file(iname.c_str(), oname.c_str()))
        {
            e.throw_exception(e.name(), 
                              "project_processor: Cannot copy file: '%s' to '%s'", 
                              iname.c_str(),
                              iname.c_str());
        }
        m_log.write("Copied file: '%s' to '%s'", 
                     iname.c_str(),
                     oname.c_str());
    }



    //------------------------------------------------------------------
    static bool check_boolean(const element& e, const string_type& str)
    {
        if(str_cmp(str, keyword_true) == 0) 
        {
            return true;
        }
        else if(str_cmp(str, keyword_false) == 0) 
        {
            return false;
        }
        else
        {
            e.throw_exception(e.attr(), "The attribute value must be 'true' or 'false'");
        }
        return false;
    }


    //------------------------------------------------------------------
    void project_processor::start_element(const element& e)
    {
        string_type name;
        string_type exclude;
        string_type ts;
        if(str_cmp(e.name(), e.name_len(), keyword_ext) == 0)
        {
            if(m_element_level != 0)
            {
                e.throw_exception(e.name(), "project_processor: \\ext element cannot be nested");
            }
            m_ext.parse(e.attr(), e.attr_len());
            if(m_ext.size() == 0)
            {
                e.throw_exception(e.name(), "project_processor: \\ext element cannot be empty");
            }
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_outdir) == 0)
        {
            if(m_element_level != 0)
            {
                e.throw_exception(e.name(), "project_processor: \\outdir element cannot be nested");
            }
            name.assign(e.attr(), e.attr_len());
            clean_and_trim_string(name);
            m_output_dir = name;
            if(m_output_dir.empty())
            {
                e.throw_exception(e.name(), "project_processor: \\outdir element cannot be empty");
            }
            trim_ending_slashes(m_output_dir);
            create_output_dir(e);
            m_index_content.add(backslash);
            m_index_content.add(keyword_outdir.name, keyword_outdir.len);
            m_index_content.add(open_bracket);
            m_index_content.add(make_output_file_name());
            m_index_content.add(close_bracket);
            m_index_content.add(lf);
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_config) == 0)
        {
            if(m_element_level != 0)
            {
                e.throw_exception(e.name(), "project_processor: \\config element cannot be nested");

            }


            parse_and_find_attr_content(e.attr(), e.attr_len(), name);
            bool trim_flag = false;
            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), keyword_trim_lines, ts))
            {
                trim_flag = check_boolean(e, ts);
            }
            clean_and_trim_string(name);
            delete m_cfg;
            m_cfg = new config(make_input_file_name(name).c_str(), trim_flag);
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_process_dir) == 0)
        {
            verify_mandatory_elements(e);
            m_cfg->push_subdir();
            parse_and_find_attr_content(e.attr(), e.attr_len(), name);
            clean_and_trim_string(name);
            if(name.empty())
            {
                e.throw_exception(e.name(), "project_processor: \\process_dir: No directory name");
            }
            trim_ending_slashes(name);
            m_input_path.push_back(name);
            m_output_path.push_back(name);
            create_output_dir(e);

            m_index_content.add(backslash);
            m_index_content.add(keyword_dir.name, keyword_dir.len);
            m_index_content.add(open_bracket);
            m_index_content.add(name);
            m_index_content.add(close_bracket);
            m_index_content.add(open_brace);
            m_index_content.add(lf);

            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), keyword_src, name))
            {
                m_input_path.pop_back();
                clean_and_trim_string(name);
                if(name.empty())
                {
                    e.throw_exception(e.name(), "project_processor: \\process_dir: 'src=' is empty");
                }
                trim_ending_slashes(name);
                m_input_path.push_back(name);
            }

            name = make_input_file_name();
            if(name.empty())
            {
                e.throw_exception(e.name(), "project_processor: Directory name is empty");
            }

            stat_e stat = name_stat(name.c_str());
            if(stat == file_exists)
            {
                e.throw_exception(e.name(), "project_processor: Input directory is a file: '%s'", name.c_str());
            }
            if(stat == not_found)
            {
                e.throw_exception(e.name(), "project_processor: Input directory does not exist: '%s'", name.c_str());
            }
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_process_files) == 0)
        {
            verify_mandatory_elements(e);
            parse_and_find_attr_content(e.attr(), e.attr_len(), name);
            clean_and_trim_string(name);
            if(name.empty())
            {
                e.throw_exception(e.name(), "project_processor: \\process_files: No files");
            }

            string_type ext = m_ext[0];
            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), keyword_ext, ts))
            {
                int en = atoi(to_ascii(ts).c_str());
                if(en)
                {
                    if(en >= (int)m_ext.size())
                    {
                        e.throw_exception(e.name(), "project_processor: \\process_files: File extension index is out of range");
                    }
                }
                else
                {
                    ext = ts;
                }
            }

            bool variables_only = false;
            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), keyword_variables_only, ts))
            {
                variables_only = check_boolean(e, ts);
            }

            bool trim_lines = false;
            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), keyword_trim_lines, ts))
            {
                trim_lines = check_boolean(e, ts);
            }

            bool remove_eof = false;
            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), keyword_remove_eof, ts))
            {
                remove_eof = check_boolean(e, ts);
            }

            parse_and_find_attr_variable(e.attr(), e.attr_len(), 
                                         keyword_exclude, 
                                         exclude);

            file_enumerator fe(make_input_file_name(), name, exclude);
            file_enumerator::const_iterator it = fe.begin();
            for(; it != fe.end(); ++it)
            {
                process_file(*it, ext, variables_only, trim_lines, remove_eof, 0);
            }
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_process_code) == 0)
        {
            verify_mandatory_elements(e);
            parse_and_find_attr_content(e.attr(), e.attr_len(), name);
            clean_and_trim_string(name);
            if(name.empty())
            {
                e.throw_exception(e.name(), "project_processor: \\process_code: No files");
            }

            string_type ext = m_ext[0];
            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), keyword_ext, ts))
            {
                int en = atoi(to_ascii(ts).c_str());
                if(en)
                {
                    if(en >= (int)m_ext.size())
                    {
                        e.throw_exception(e.name(), "project_processor: \\process_files: File extension index is out of range");
                    }
                }
                else
                {
                    ext = ts;
                }
            }

            string_type lang(keyword_code.name, keyword_code.len);
            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), keyword_lang, ts))
            {
                lang = ts;
                if(!m_cfg->keyword_exists(keyword_code, ts.c_str(), ts.length()))
                {
                    e.throw_exception(e.name(), "project_processor: \\process_code: Unknown code processor language");
                }
            }

            parse_and_find_attr_variable(e.attr(), e.attr_len(), 
                                                   keyword_exclude, 
                                                   exclude);

            file_enumerator fe(make_input_file_name(), name, exclude);
            file_enumerator::const_iterator it = fe.begin();
            for(; it != fe.end(); ++it)
            {
                process_file(*it, ext, false, false, false, lang.c_str());
            }
        }
        else
        if(str_cmp(e.name(), e.name_len(), keyword_copy_files) == 0)
        {
            if(m_index && m_index_src)
            {
                parse_and_find_attr_variable(e.attr(), e.attr_len(), 
                                             keyword_exclude, 
                                             exclude);
                file_enumerator fe(make_input_file_name(), 
                                   string_type(e.attr(), e.attr_len()),
                                   exclude);
                file_enumerator::const_iterator it = fe.begin();
                for(; it != fe.end(); ++it)
                {
                    copy_file_as_is(e, *it);
                }
            }
        }
        if(e.name_len()) ++m_element_level;
    }



    //------------------------------------------------------------------
    void project_processor::end_element(const element& e)
    {
        if(str_cmp(e.name(), e.name_len(), keyword_process_dir) == 0)
        {
            if(m_input_path.size())  m_input_path.pop_back();
            if(m_output_path.size()) m_output_path.pop_back();
            m_cfg->pop_subdir();
            m_index_content.add(close_brace);
            m_index_content.add(lf);
        }
        if(e.name_len()) --m_element_level;
    }



}
      
