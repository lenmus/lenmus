//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 Cecilio Salmeron
//
//    This file is based/copied from Anti-Grain Documenter - Version 1.0 
//    project by Maxim Shemanarev (McSeem)
//
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

#ifndef AGDOC_BASICS_INCLUDED
#define AGDOC_BASICS_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef _MSC_VER
// Disable MSC++ "identifier was truncated blah-blah-blah-..."
// Don't they understand it becomes impossible to find any
// other warnings among this crap.
#pragma warning(disable : 4786 4503)
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <fstream>

extern void ltLogMessage(const char* msg);
extern void ltLogMessage(const char* msg, const char* arg1);
extern void ltLogMessage(const char* msg, const char* arg1, const char* arg2);
extern void ltLogDbg(wxString& msg);

//#define ltUSE_WXSTRING

namespace agdoc
{
    //==================================================================
    // 
    // Char and String type dependent part
    // 
    //==================================================================
#ifdef ltUSE_WXSTRING
    typedef wxChar char_type;
#else
    typedef char char_type;
#endif
    typedef std::basic_string<char_type> string_type;

    typedef std::vector<char_type>       string_buffer_type;

    inline bool operator < (const string_type& s1, const string_type& s2)
    {
        return s1.compare(s2) < 0;
    }

    typedef std::set<string_type>              strset_type;
    typedef std::map<string_type, string_type> strmap_type;
    typedef std::map<string_type, strset_type> strmapset_type;
    typedef std::set<char_type>                charset_type;

    enum
    {
        element_replacement_char = 1,
        ignore_char              = 2,
        backslash_restore        = 3,
        dollar_restore           = 4,
        open_brace_restore       = 5,
        close_brace_restore      = 6,
        open_bracket_restore     = 17,
        close_bracket_restore    = 18,
        reserved_char            = 19,
        colon                    = ':',
        semicolon                = ';',
        slash                    = '/',
        backslash                = '\\',
        cr                       = '\r',
        lf                       = '\n',
        tab                      = '\t',
        space                    = ' ',
        underscore               = '_',
        dot                      = '.',
        comma                    = ',',
        asterisk                 = '*',
        apostrophe               = '\'',
        quote                    = '"',
        open_brace               = '{',
        vbar                     = '|',
        close_brace              = '}',
        open_bracket             = '[',
        close_bracket            = ']',
        open_parenthesis         = '(',
        close_parenthesis        = ')',
        open_angle               = '<',
        close_angle              = '>',
        toc_char                 = 'h',
        dollar                   = '$',
        minus                    = '-',
        equal                    = '=',
        question_mark            = '?',
        caret                    = '^',

        // INPORTANT! Last character in the whole charset
        // Used to find sorted labels by the primary segment 
        // of the index key, that is, if you have a composite 
        // index key "LastName", "FirstName" and you need to 
        // select all the recors by only "LastName" you will need
        // to use something like:
        // ... where LastName == last_name_val &&
        //           FirstName >= "" &&
        //           FirstName <= max_str
        // Here max_str contains max_char as the first element of it.
        // So, use the maximal value that will guarantee that there 
        // will be no other values greater than that. Note that 
        // type "char" is usually signed, so, 128 and above will be 
        // treated as negative values. Also note that if you use some 
        // 8-bit international charset, there's OK to use max_char = 127
        // unless you want to use international characters in labels.
        max_char = 127
    };


    //-----------------------------------------------------------------
    // A simple structure to define all keywors and other sequences of 
    // char_type. It was decided to use a paradigm of statically defined 
    // "pointer + length" in order not to restrict the character
    // types with "char" ot "wchar_t". Thus, if you need to use int32
    // for "true" Unicode, you can redefine the keywords in the following way:
    // keyword keyword_sup = { {'s', 'u', 'p'}, 3 };
    // Besides, it's better and faster to operate with keywords with 
    // pre-calculated length. 
    struct keyword 
    { 
        const char_type* name; 
        unsigned len; 
    };

    extern keyword keyword_include;
    extern keyword keyword_cntn;
    extern keyword keyword_content;
    extern keyword keyword_p;
    extern keyword keyword_br;
    extern keyword keyword_b;
    extern keyword keyword_i;
    extern keyword keyword_u;
    extern keyword keyword_m;
    extern keyword keyword_sup;
    extern keyword keyword_sub;
    extern keyword keyword_href;
    extern keyword keyword_attr_suffix;
    extern keyword keyword_open_suffix;
    extern keyword keyword_close_suffix;
    extern keyword keyword_toc;
    extern keyword keyword_toc0;
    extern keyword keyword_toc_open_suffix;
    extern keyword keyword_toc_close_suffix;
    extern keyword keyword_page_open;
    extern keyword keyword_page_close;
    extern keyword keyword_suppress_output_elements;
    extern keyword keyword_this;
    extern keyword keyword_attr;
    extern keyword keyword_iref;
    extern keyword keyword_root_rel;
    extern keyword keyword_code;
    extern keyword keyword_as_is;
    extern keyword keyword_line_comments;
    extern keyword keyword_block_comments;
    extern keyword keyword_line_comments_suffix;
    extern keyword keyword_block_comments_suffix;
    extern keyword keyword_strings_suffix;
    extern keyword keyword_string_mask_suffix;
    extern keyword keyword_operators_suffix;
    extern keyword keyword_kw1;
    extern keyword keyword_kw2;
    extern keyword keyword_kw3;
    extern keyword keyword_kw4;
    extern keyword keyword_rem;
    extern keyword keyword_str;
    extern keyword keyword_num;
    extern keyword keyword_op;
    extern keyword keyword_kw1_suffix;
    extern keyword keyword_kw2_suffix;
    extern keyword keyword_kw3_suffix;
    extern keyword keyword_kw4_suffix;
    extern keyword keyword_paragraph_structure_elements;
    extern keyword keyword_paragraphless_elements;
    extern keyword keyword_paragraph_br_style_elements;
    extern keyword keyword_output_specific_replacements;
    extern keyword keyword_singletons;
    extern keyword keyword_pair_quotes;
    extern keyword keyword_formatless_elements;
    extern keyword keyword_linking_keywords;
    extern keyword keyword_imgl0;
    extern keyword keyword_imgr0;
    extern keyword keyword_imgl;
    extern keyword keyword_imgr;
    extern keyword keyword_imgc;
    extern keyword keyword_img;
    extern keyword keyword_ssv_table;
    extern keyword keyword_vbar_table;
    extern keyword keyword_eln_table;
    extern keyword keyword_table;
    extern keyword keyword_tr;
    extern keyword keyword_trh;
    extern keyword keyword_th;
    extern keyword keyword_td;
    extern keyword keyword_tdr;
    extern keyword keyword_tdc;
    extern keyword keyword_tdj;
    extern keyword keyword_al;
    extern keyword keyword_ul;
    extern keyword keyword_ol;
    extern keyword keyword_li;
    extern keyword keyword_replace;
    extern keyword keyword_label;
    extern keyword keyword_label0;
    extern keyword keyword_category;
    extern keyword keyword_anchor;
    extern keyword keyword_index;
    extern keyword keyword_figure;
    extern keyword keyword_listing;
    extern keyword keyword_title;
    extern keyword keyword_dot_dot;
    extern keyword keyword_dot_dot_slash;
    extern keyword keyword_ext;
    extern keyword keyword_outdir;
    extern keyword keyword_config;
    extern keyword keyword_process_dir;
    extern keyword keyword_process_files;
    extern keyword keyword_process_code;
    extern keyword keyword_copy_files;
    extern keyword keyword_src;
    extern keyword keyword_lang;
    extern keyword keyword_variables_only;
    extern keyword keyword_true;
    extern keyword keyword_false;
    extern keyword keyword_trim_lines;
    extern keyword keyword_remove_eof;
    extern keyword keyword_exclude;
    extern keyword keyword_dir;
    extern keyword keyword_file;
    extern keyword keyword_index_ext;
    extern keyword keyword_open_comment;
    extern keyword keyword_close_comment;
    extern keyword keyword_line_comment;

    inline bool is_backslash(char_type c) 
    { 
        return c && c == backslash; 
    }

    inline bool is_cr(char_type c) 
    { 
        return c && c == cr; 
    }

    inline bool is_lf(char_type c) 
    { 
        return c && c == lf; 
    }

    inline bool is_quote(char_type c) 
    { 
        return c && c == quote; 
    }

    inline bool is_minus(char_type c) 
    { 
        return c && c == minus; 
    }

    inline bool is_element_name(char_type c) 
    { 
        return c && (isalnum(c) || c == underscore);
    }

    inline bool is_reference_char(char_type c) 
    { 
        return c && (isalnum(c) || 
                     c == underscore ||
                     c == dot ||
                     c == colon);
    }

    inline bool is_lower(char_type c) 
    { 
        return c && c >= 'a' && c <= 'z'; 
    }

    inline bool is_digit(char_type c) 
    { 
        return c && isdigit(c); 
    }

    inline bool is_alnum(char_type c) 
    { 
        return c && isalnum(c); 
    }

    inline bool is_alpha(char_type c) 
    { 
        return c && isalpha(c); 
    }

    inline bool is_asterisk(char_type c)
    {
        return c && c == asterisk;
    }

    inline bool is_open_brace(char_type c) 
    { 
        return c && c == open_brace; 
    }

    inline bool is_close_brace(char_type c) 
    { 
        return c && c == close_brace; 
    }

    inline bool is_open_bracket(char_type c) 
    { 
        return c && c == open_bracket; 
    }

    inline bool is_close_bracket(char_type c) 
    { 
        return c && c == close_bracket; 
    }

    inline bool is_dollar(char_type c) 
    { 
        return c && c == dollar; 
    }

    inline bool is_open_parenthesis(char_type c) 
    { 
        return c && c == open_parenthesis; 
    }

    inline bool is_close_parenthesis(char_type c) 
    { 
        return c && c == close_parenthesis; 
    }

    inline bool is_space(char_type c) 
    { 
        return c && (c == ignore_char ||
                     c == reserved_char ||
                     c == space || 
                     c == tab || 
                     c == lf || 
                     c == cr); 
    }

    inline bool is_line_space(char_type c) 
    { 
        return c && (c == ignore_char ||
                     c == reserved_char ||
                     c == space || 
                     c == tab); 
    }

    inline int is_toc_element(const char_type* s, unsigned len) 
    {
        if(len == 2 && *s == toc_char && isdigit(s[1])) return s[1] - '0';
        return 0;
    }

    inline char_type toc_num_to_char(int tocn) 
    {
        return tocn + '0';
    }


    bool is_empty_line(const char_type* str, unsigned len);
    bool is_empty_line(const char_type* str);
    bool is_row_break(const char_type* str);
    bool is_double_vbar(const char_type* str);
    unsigned skip_lf(const char_type* str, unsigned len, unsigned count);
    string_type from_ascii(const std::string& s);
    string_type from_ascii(const char* s);
    std::string to_ascii(const string_type& s);
    std::string to_ascii(const char_type* s);
    void clean_and_trim_string(string_type& s);

    wxString ToString(string_type& s, int len); 
    wxString ToString(const char_type* s, int len);




    
    //------------------------------------------------------------------
    inline int str_cmp(const char_type* s1, unsigned l1, 
                       const char_type* s2, unsigned l2)
    {
        while(l1 && l2)
        {
            int r = int(*s1) - int(*s2);
            if(r != 0) return r;
            ++s1; ++s2; --l1; --l2;
        }
        if(l1 && l2 == 0) return 1;
        if(l2 && l1 == 0) return -1;
        return 0;
    }

    //------------------------------------------------------------------
    inline unsigned str_len(const char_type* s)
    {
        unsigned len = 0;
        while(*s++) ++len;
        return len;
    }

    //------------------------------------------------------------------
    inline int str_cmp(const char_type* str, unsigned len, const keyword& kw)
    {
        return str_cmp(str, len, kw.name, kw.len);
    }

    //------------------------------------------------------------------
    inline int str_cmp(const string_type& s1, const char_type* s2, unsigned l2)
    {
        return str_cmp(s1.c_str(), s1.length(), s2, l2);
    }

    //------------------------------------------------------------------
    inline int str_cmp(const string_type& s1, const string_type& s2)
    {
        return str_cmp(s1.c_str(), s1.length(), s2.c_str(), s2.length());
    }

    //------------------------------------------------------------------
    inline int str_cmp(const string_type& str, const keyword& kw)
    {
        return str_cmp(str.c_str(), str.length(), kw.name, kw.len);
    }

    //------------------------------------------------------------------
    inline int str_case_cmp(const char_type* s1, const char_type* s2)
    {
        while(*s1 && *s2)
        {
            int r = toupper(*s1) - toupper(*s2);
            if(r) return r;
            ++s1;
            ++s2;
        }
        return toupper(*s1) - toupper(*s2);
    }

    //------------------------------------------------------------------
    bool str_is(const char_type* p, const string_type& s);
    bool str_is(const char_type* p, unsigned len, const string_type& s);
    bool str_is(const char_type* p, unsigned len, const string_type& s);

    char_type replace_keysym(char_type c);
    char_type restore_keysym(char_type c);
    //==================================================================




    //------------------------------------------------------------------
    struct src_line
    {
        const char_type* ptr;
        unsigned         len;
        int              num;
    };



    //------------------------------------------------------------------
    // Path, dir, and file utility functions
    enum stat_e
    {
        not_found,
        file_exists,
        dir_exists
    };

    void calc_relative_name_to(const char_type* from, 
                               const char_type* to, 
                               string_type& rel_name);
    void reduce_dot_dot(string_type& fname);
    void extract_file_name(const char_type* path, string_type& fname);
    bool file_name_match(const char_type* wildcard, const char_type* target);
    bool read_dir(const char_type* dir_name, strset_type& names);
    stat_e name_stat(const char_type* name, time_t* mod_time = 0);
    bool make_dir(const char_type* dir_name);
    bool copy_file(const char_type* from, const char_type* to);
    string_type path_prefix(const char_type* fname);
    void trim_ending_slashes(string_type& s);


    //------------------------------------------------------------------
    class file_enumerator
    {
    public:
        typedef strset_type::const_iterator const_iterator;

        file_enumerator(const string_type& path, 
                        const string_type& names, 
                        const string_type& exclude);
        const_iterator begin() const { return m_names.begin(); }
        const_iterator end()   const { return m_names.end(); }

    private:
        strset_type m_names;
    };




    //------------------------------------------------------------------
    void parse_and_find_attr_content(const char_type* attr, 
                                     unsigned attr_len, 
                                     string_type& content);

    //------------------------------------------------------------------
    bool parse_and_find_attr_variable(const char_type* attr, 
                                      unsigned attr_len,
                                      const string_type& name, 
                                      string_type& val);

    bool parse_and_find_attr_variable(const char_type* attr, 
                                      unsigned attr_len,
                                      const keyword& name, 
                                      string_type& val);

    //------------------------------------------------------------------
    class parsing_exception
    {
    public:
        parsing_exception(const char_type* msg);

        parsing_exception(const char_type* msg,
                          const char_type* arg1);

        parsing_exception(const char_type* msg,
                          const char_type* arg1,
                          const char_type* arg2);

        parsing_exception(const src_line& line, 
                          const char_type* msg);

        parsing_exception(const src_line& line, 
                          const char_type* msg, 
                          const char_type* arg1);

        parsing_exception(const src_line& line, 
                          const char_type* msg, 
                          const char_type* arg1,
                          const char_type* arg2);

        int                line_num()     const { return m_line_num;     }
        const string_type& line_content() const { return m_line_content; }
        const string_type& message()      const { return m_message;      }

    private:
        void copy_message(const char_type* msg);

        void copy_message(const char_type* msg, 
                          const char_type* arg1);

        void copy_message(const char_type* msg, 
                          const char_type* arg1, 
                          const char_type* arg2);

        void make_line_content(const src_line& line);

        int         m_line_num;
        string_type m_line_content;
        string_type m_message;
    };



    //------------------------------------------------------------------
    struct config_exception : public parsing_exception
    {
        config_exception(const src_line& line, const char_type* msg) : 
            parsing_exception(line, msg) {}
    };


    //------------------------------------------------------------------
    struct file_not_found : public parsing_exception
    {
        file_not_found(const char_type* msg) : parsing_exception(msg) {}
    };





    //------------------------------------------------------------------
    class string_list
    {
    public:
        string_list() {}

        string_list(const char_type* str, unsigned len, const char_type sep = space)
        {
            parse(str, len, sep);
        }

        string_list(const string_type& str, const char_type sep = space)
        {
            if(!str.empty()) parse(str.c_str(), str.length(), sep);
        }

        string_list(const string_type* str, const char_type sep = space)
        {
            if(str) parse(str->c_str(), str->length(), sep);
        }

        bool exists(const char_type* str, unsigned len) const;
        bool char_exists(const char_type c) const;

        unsigned size() const { return m_strings.size(); }
        const string_type& operator [] (unsigned idx) const
        {
            return m_strings[idx];
        }

        string_type& operator [] (unsigned idx)
        {
            return m_strings[idx];
        }

        void parse(const char_type* str, unsigned len, const char_type sep = space);

    private:
        std::vector<string_type> m_strings;
    };


    //------------------------------------------------------------------
    typedef std::map<string_type, string_list> strmaplist_type;


    //------------------------------------------------------------------
    class string_set
    {
    public:
        string_set() {}

        void add(const string_type& str)
        {
            m_set.insert(str);
        }

        void add(const strset_type* strset);

        string_set(const char_type* str, unsigned len, const char_type sep = space)
        {
            parse(str, len, sep);
        }

        string_set(const string_type& str, const char_type sep = space)
        {
            parse(str.c_str(), str.length(), sep);
        }

        string_set(const string_type* str, const char_type sep = space)
        {
            if(str) parse(str->c_str(), str->length(), sep);
        }

        bool exists(const char_type* str, unsigned len) const
        {
            if(str == 0 || len == 0) return false;
            return m_set.find(string_type(str, len)) != m_set.end();
        }

        const strset_type& strings() const
        {
            return m_set;
        }

    private:
        void parse(const char_type* str, unsigned len, const char_type sep);

    private:
        strset_type m_set;
    };

    //------------------------------------------------------------------
    const string_type* find_replacement(const string_list& repl, 
                                        const char_type* str, 
                                        unsigned len, 
                                        unsigned* ret_len);

    void add_to_charset(charset_type& ch, const string_list& repl);


    //------------------------------------------------------------------
    class element
    {
    public:
        element(char_type* src_text, const string_set* solid_elements = 0);

        src_line line(const char_type* ptr) const;

        char_type* parse();

        void clear(); 

        bool is_empty() const
        {
            return m_total_len == 0 && m_elements.size() == 0;
        }

        const char_type* name()        const { return m_name;        }
        unsigned         name_len()    const { return m_name_len;    }
        const char_type* attr()        const { return m_attr;        }
        unsigned         attr_len()    const { return m_attr_len;    }
        const char_type* content()     const { return m_content;     }
        unsigned         content_len() const { return m_content_len; }
        unsigned         total_len()   const { return m_total_len;   }
        const element*   parent()      const { return m_parent;      }
        unsigned         num_elements()const { return m_elements.size(); }
        const element& at(unsigned i)  const { return m_elements[i]; }

        wxString GetName() const { return ToString(m_name, (int)m_name_len); }
        wxString GetAttr() const { return ToString(m_attr, (int)m_attr_len); }
        wxString GetContent() const { return ToString(m_content, (int)m_content_len); }
        void DumpData(const char_type* pContent, unsigned content_len) const;

        template<class Consumer> void process(Consumer& c) const
        {
            unsigned i;
            unsigned cmd_idx = 0;

            c.start_element(*this);
            const char_type* content_ptr = m_content;
            unsigned content_len = 0;

            for(i = 0; i < m_content_len; )
            {
                if(m_content[i] == element_replacement_char)
                {
                    c.content(*this, content_ptr, content_len);
                    if(cmd_idx < m_elements.size())
                    {
                        const element& e = m_elements[cmd_idx];
                        e.process(c);
                        i += e.m_total_len;
                        cmd_idx++;
                        content_ptr = m_content + i;
                        content_len = 0;
                    }
                    else
                    {
                        throw_exception(m_content + i, "ELEMENT COUNT ERROR");
                    }
                }
                else
                {
                    ++content_len;
                    ++i;
                }
            }
            c.content(*this, content_ptr, content_len);
            c.end_element(*this);
        }

        void dump_all(FILE* fd, unsigned offset = 0) const;
        void dump_names(FILE* fd, unsigned offset = 0) const;

        const element* find_first_from_root(const string_type& name) const;
        const element* find_first_to_root(const string_type& name) const;
        const element* find_first_element(const string_type& name) const;
        const element* recursive_search(const string_type& name) const;
        const string_type* find_variable_to_root(const strmap_type& cfg, 
                                                 const string_type& name) const;

        void throw_exception(const char_type* ptr, 
                             const char_type* msg) const;

        void throw_exception(const char_type* ptr, 
                             const char_type* msg, 
                             const char_type* arg1) const;

        void throw_exception(const char_type* ptr, 
                             const char_type* msg, 
                             const char_type* arg1, 
                             const char_type* arg2) const;

    private:
        char_type* parse_nested(char_type* start);
        char_type* parse_element(char_type* start);
        char_type* parse_content(char_type* start);
        char_type* parse_attr(char_type* start);
        void set_parent(element* parent);
    
    private:
        char_type*           m_src_text;
        const string_set*    m_solid_elements;
        const char_type*     m_name;
        unsigned             m_name_len;
        const char_type*     m_attr;
        unsigned             m_attr_len;
        char_type*           m_content;
        unsigned             m_content_len;
        unsigned             m_total_len;
        int                  m_brace_count;
        std::vector<element> m_elements;
        const element*       m_parent;               
    };








    //------------------------------------------------------------------
    class config
    {
    public:
        config(const char_type* file_name, bool trim_flag);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element& e, const char_type*, unsigned len);

        void push_subdir();
        void pop_subdir();
        const string_type& root_rel() const;

        void dump(FILE* fd) const;

        const string_type* find(const string_type& name) const;
        const string_type* find(const char_type* name, unsigned name_len) const;
        const string_type* find(const keyword& kw) const;
        const string_type* find(const char_type* name, unsigned name_len, const keyword& suffix) const;

        const strset_type* find_keywords(const string_type& category) const;
        const strset_type* find_keywords(const keyword& category) const;
        const strset_type* find_keywords(const string_type& prefix, const keyword& suffix) const;

        const strmap_type& all_elements() const { return m_elements; }

        const string_set* solid_elements() const   { return &m_solid_elements; }
        bool is_solid_element(const char_type* name, unsigned len) const;

        bool keyword_exists(const string_type& category, const char_type* name, unsigned name_len) const;
        bool keyword_exists(const keyword& category, const char_type* name, unsigned name_len) const;
        bool keyword_exists(const string_type& prefix, const keyword& suffix, const char_type* name, unsigned name_len) const;

        const string_list& strlist(const string_type& category) const;
        const string_list& strlist(const keyword& category) const;
        const string_list& strlist(const string_type& prefix, const keyword& suffix) const;


    private:
        void process_keywords();
        void add_keywords(const string_type& category);
        void add_keywords(const keyword& category);
        void add_keywords(const string_type& language, const keyword& suffix);
        void add_keyword(const keyword& category, 
                         const char_type* name, unsigned len);

        void add_strlist(const string_type& category);
        void add_strlist(const keyword& category);
        void add_strlist(const string_type& language, const keyword& suffix);

        void restore_strlist_keysym(const keyword& category);
        string_list& strlist(const keyword& category);

    private:
        int             m_level;
        bool            m_trim_flag;
        string_type     m_name;
        strmap_type     m_elements;
        string_type     m_root_path;
        string_type     m_root;
        strmapset_type  m_keywords;
        strmaplist_type m_strlists;
        string_list     m_empty_list;
        string_set      m_solid_elements;
    };





    //------------------------------------------------------------------
    class content_storage
    {
    public:
        ~content_storage();
        content_storage(const string_set* solid_elements = 0);

        void clear();
        void reserve(unsigned r);
        void assign(const content_storage& c);
        void restore_keysym_flag(bool r) { m_restore_keysym_flag = r; }

        void add(char_type c, bool repl_keysym = false);
        void add(const string_type& str);
        void add(const string_type* str);
        void add(const char_type* str, unsigned len, bool repl_keysym = false);
        void add(const char_type* str);
        void add(const keyword& el, const char_type* content, unsigned len, bool repl_keysym = false);

        void add_element_header(const element& e);
        void add_element_footer(const element& e);

        void replace_text_keysym();
        void restore_text_keysym();

        unsigned length() const;

        char_type* text();
        const element& elements();

        void add_str_with_variables(const element& e, 
                                    const char_type* s, 
                                    unsigned len,
                                    const config& cfg);

    private:
        void add_variable(const char_type* name, 
                          unsigned name_len, 
                          const element& e, 
                          const config& cfg);
        void add_element_content(const element& e, const config& cfg);
        void add_whole_element(const element& e, const config& cfg);
        void add_attr_content(const element& e, const config& cfg);

        void add_attr_variable(const element& e, 
                               const string_type& str, 
                               const config& cfg);

        const char_type* process_variable(const element& e, 
                                          const char_type* str, 
                                          unsigned len,
                                          const config& cfg);

        content_storage(const content_storage&);
        const content_storage& operator = (const content_storage&);

        string_buffer_type m_buffer;
        const string_set*  m_solid_elements;
        element*           m_element;
        bool               m_restore_keysym_flag;
        int                m_recursion_level;
    };





    //------------------------------------------------------------------
    class ifile
    {
    public:
        ifile(const char_type* name, 
              const string_set* solid_elements = 0,
              bool  trim_lines = false,
              bool  remove_eof = false);

        unsigned length() const   { return m_content.length();   }
        char_type* text()         { return m_content.text();     }
        const element& elements() { return m_content.elements(); }

    private:
        bool read_line(std::ifstream& s, string_type& buf);
        content_storage m_content;
        bool m_trim_lines;
        bool m_remove_eof;
    };


    //------------------------------------------------------------------
    void write_content_storage(const char_type* name, content_storage& content);


    //------------------------------------------------------------------
    class content_serializer
    {
    public:
        content_serializer(content_storage& buf) : m_buf(buf) {}

        void start_element(const element&) {}
        void end_element(const element&) {}
        void content(const element&, const char_type* c, unsigned len)
        {
            m_buf.add(c, len);
        }

    private:
        content_storage& m_buf;
    };




    //------------------------------------------------------------------
    class element_serializer
    {
    public:
        element_serializer(content_storage& storage, 
                           const element& e,
                           bool  root_element_decoration=true,
                           bool  empty_elements_decoration=true,
                           const keyword* add_attr_name = 0,
                           const char_type* add_attr_value = 0);

        void start_element(const element& e);
        void end_element(const element& e);
        void content(const element&, const char_type* c, unsigned len);

    private:
        content_storage& m_storage;
        int              m_level;
        bool             m_empty_elements_decoration;
        const keyword*   m_add_attr_name;
        const char_type* m_add_attr_value;
    };



    //------------------------------------------------------------------
    class log_file
    {
    public:
        void write(const char_type* message);
        void write(const char_type* message, const char_type* arg1);
        void write(const char_type* message, const char_type* arg1, 
                   const char_type* arg2);
    };



}


#endif
