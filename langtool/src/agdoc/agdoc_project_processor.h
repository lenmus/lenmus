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

#ifndef AGDOC_PROJECT_PROCESSOR_INCLUDED
#define AGDOC_PROJECT_PROCESSOR_INCLUDED

#include "agdoc_basics.h"


namespace agdoc
{
    class index_storage;

    //------------------------------------------------------------------
    class project_processor
    {
    public:
        ~project_processor();
        project_processor(const char_type* file_name, 
                          log_file& log,
                          const index_storage* index = 0, 
                          const element* index_src = 0,
                          bool quick_mode = false);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element&, const char_type*, unsigned) {}

        content_storage& index_content() { return m_index_content; }
            
    private:
        void verify_mandatory_elements(const element& e) const;
        void create_output_dir(const element& e) const;

        string_type make_input_file_name(const char_type*    short_fname) const;
        string_type make_input_file_name(const string_type&  short_fname) const;
        string_type make_input_file_name()                                const;
        string_type make_output_file_name(const char_type*   short_fname) const;
        string_type make_output_file_name(const string_type& short_fname) const;
        string_type make_output_file_name()                               const;

        bool needs_processing(const string_type& iname, 
                              const string_type& oname);

        void copy_file_as_is(const element& e, const string_type& file_name);

        void process_file(const string_type& file_name, 
                          const string_type& ext, 
                          bool  variables_only,
                          bool  trim_lines, 
                          bool  remove_eof, 
                          const char_type* lang);

    private:
        const index_storage*     m_index;
        const element*           m_index_src;
        bool                     m_quick_mode;
        log_file&                m_log;
        string_type              m_project_dir;
        string_type              m_output_dir;
        std::vector<string_type> m_input_path;
        std::vector<string_type> m_output_path;
        config*                  m_cfg;
        string_list              m_ext;
        content_storage          m_index_content;
        unsigned                 m_element_level;
    };


}

#endif

