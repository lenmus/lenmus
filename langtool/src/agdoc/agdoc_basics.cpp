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

#include "agdoc_basics.h"

#include <sys/stat.h>
#if defined(_WIN32)    // Use _findfirst/_findnext
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>    // Assume POSIX opendir/readdir/closedir
#endif


namespace agdoc
{

    //==================================================================
    //
    // Char and String type dependent part
    //
    //==================================================================


    //------------------------------------------------------------------
    // Keywors and other sequences of char_type. It was decided to use a
    // paradigm of statically defined "pointer + length" in order not to
    // restrict the character type with "char" ot "wchar_t". Thus, if you
    // need to use int32 for "true" Unicode, you can redefine the keywords
    // in the following way:
    // keyword keyword_sup = { {'s', 'u', 'p'}, 3 };
    // Besides, it's better and faster to operate with keywords with
    // pre-calculated length.
    //-----------------------------------------------------------------=
#ifdef ltUSE_WXSTRING

    keyword keyword_include                      = { _T("\\include"),        8  };
    keyword keyword_cntn                         = { _T("cntn"),             4  };
    keyword keyword_content                      = { _T("content"),          7  };
    keyword keyword_p                            = { _T("p"),                1  };
    keyword keyword_br                           = { _T("br"),               2  };
    keyword keyword_b                            = { _T("b"),                1  };
    keyword keyword_i                            = { _T("i"),                1  };
    keyword keyword_u                            = { _T("u"),                1  };
    keyword keyword_m                            = { _T("m"),                1  };
    keyword keyword_sup                          = { _T("sup"),              3  };
    keyword keyword_sub                          = { _T("sub"),              3  };
    keyword keyword_href                         = { _T("href"),             4  };
    keyword keyword_attr_suffix                  = { _T("_attr"),            5  };
    keyword keyword_open_suffix                  = { _T("_open"),            5  };
    keyword keyword_close_suffix                 = { _T("_close"),           6  };
    keyword keyword_toc                          = { _T("toc"),              3  };
    keyword keyword_toc0                         = { _T("toc0"),             4  };
    keyword keyword_toc_open_suffix              = { _T("_toc_open"),        9  };
    keyword keyword_toc_close_suffix             = { _T("_toc_close"),       10 };
    keyword keyword_page_open                    = { _T("page_open"),        9  };
    keyword keyword_page_close                   = { _T("page_close"),       10 };
    keyword keyword_suppress_output_elements     = { _T("suppress_output_elements"), 24 };
    keyword keyword_this                         = { _T("this"),             4  };
    keyword keyword_attr                         = { _T("attr"),             4  };
    keyword keyword_iref                         = { _T("iref"),             4  };
    keyword keyword_root_rel                     = { _T("root_rel"),         8  };
    keyword keyword_code                         = { _T("code"),             4  };
    keyword keyword_as_is                        = { _T("as_is"),            5  };
    keyword keyword_line_comments                = { _T("line_comments"),    13 };
    keyword keyword_block_comments               = { _T("block_comments"),   14 };
    keyword keyword_line_comments_suffix         = { _T("_line_comments"),   14 };
    keyword keyword_block_comments_suffix        = { _T("_block_comments"),  15 };
    keyword keyword_strings_suffix               = { _T("_strings"),         8  };
    keyword keyword_string_mask_suffix           = { _T("_string_mask"),     12 };
    keyword keyword_operators_suffix             = { _T("_operators"),       10 };
    keyword keyword_kw1                          = { _T("kw1"),              3  };
    keyword keyword_kw2                          = { _T("kw2"),              3  };
    keyword keyword_kw3                          = { _T("kw3"),              3  };
    keyword keyword_kw4                          = { _T("kw4"),              3  };
    keyword keyword_rem                          = { _T("rem"),              3  };
    keyword keyword_str                          = { _T("str"),              3  };
    keyword keyword_num                          = { _T("num"),              3  };
    keyword keyword_op                           = { _T("op"),               2  };
    keyword keyword_kw1_suffix                   = { _T("_kw1"),             4  };
    keyword keyword_kw2_suffix                   = { _T("_kw2"),             4  };
    keyword keyword_kw3_suffix                   = { _T("_kw3"),             4  };
    keyword keyword_kw4_suffix                   = { _T("_kw4"),             4  };
    keyword keyword_paragraph_structure_elements = { _T("paragraph_structure_elements"), 28 };
    keyword keyword_paragraphless_elements       = { _T("paragraphless_elements"),       22 };
    keyword keyword_paragraph_br_style_elements  = { _T("paragraph_br_style_elements"),  27 };
    keyword keyword_output_specific_replacements = { _T("output_specific_replacements"), 28 };
    keyword keyword_singletons                   = { _T("singletons"),                   10 };
    keyword keyword_pair_quotes                  = { _T("pair_quotes"),                  11 };
    keyword keyword_formatless_elements          = { _T("formatless_elements"),          19 };
    keyword keyword_linking_keywords             = { _T("linking_keywords"),             16 };
    keyword keyword_imgl0                        = { _T("imgl0"),            5  };
    keyword keyword_imgr0                        = { _T("imgr0"),            5  };
    keyword keyword_imgl                         = { _T("imgl"),             4  };
    keyword keyword_imgr                         = { _T("imgr"),             4  };
    keyword keyword_imgc                         = { _T("imgc"),             4  };
    keyword keyword_img                          = { _T("img"),              3  };
    keyword keyword_ssv_table                    = { _T("ssv_table"),        9  };
    keyword keyword_vbar_table                   = { _T("vbar_table"),       10 };
    keyword keyword_eln_table                    = { _T("eln_table"),        9  };
    keyword keyword_table                        = { _T("table"),            5  };
    keyword keyword_tr                           = { _T("tr"),               2  };
    keyword keyword_trh                          = { _T("trh"),              3  };
    keyword keyword_th                           = { _T("th"),               2  };
    keyword keyword_td                           = { _T("td"),               2  };
    keyword keyword_tdr                          = { _T("tdr"),              3  };
    keyword keyword_tdc                          = { _T("tdc"),              3  };
    keyword keyword_tdj                          = { _T("tdj"),              3  };
    keyword keyword_al                           = { _T("al"),               2  };
    keyword keyword_ul                           = { _T("ul"),               2  };
    keyword keyword_ol                           = { _T("ol"),               2  };
    keyword keyword_li                           = { _T("li"),               2  };
    keyword keyword_replace                      = { _T("replace"),          7  };
    keyword keyword_label                        = { _T("label"),            5  };
    keyword keyword_label0                       = { _T("label0"),           6  };
    keyword keyword_category                     = { _T("category"),         8  };
    keyword keyword_anchor                       = { _T("anchor"),           6  };
    keyword keyword_index                        = { _T("index"),            5  };
    keyword keyword_figure                       = { _T("figure"),           6  };
    keyword keyword_listing                      = { _T("listing"),          7  };
    keyword keyword_title                        = { _T("title"),            5  };
    keyword keyword_dot_dot                      = { _T(".."),               2  };
    keyword keyword_dot_dot_slash                = { _T("../"),              3  };
    keyword keyword_ext                          = { _T("ext"),              3  };
    keyword keyword_outdir                       = { _T("outdir"),           6  };
    keyword keyword_config                       = { _T("config"),           6  };
    keyword keyword_process_dir                  = { _T("process_dir"),      11 };
    keyword keyword_process_files                = { _T("process_files"),    13 };
    keyword keyword_process_code                 = { _T("process_code"),     12 };
    keyword keyword_copy_files                   = { _T("copy_files"),       10 };
    keyword keyword_src                          = { _T("src"),              3  };
    keyword keyword_lang                         = { _T("lang"),             4  };
    keyword keyword_variables_only               = { _T("variables_only"),   14 };
    keyword keyword_true                         = { _T("true"),             4  };
    keyword keyword_false                        = { _T("false"),            5  };
    keyword keyword_trim_lines                   = { _T("trim_lines"),       10 };
    keyword keyword_remove_eof                   = { _T("remove_eof"),       10 };
    keyword keyword_exclude                      = { _T("exclude"),          7  };
    keyword keyword_dir                          = { _T("dir"),              3  };
    keyword keyword_file                         = { _T("file"),             4  };
    keyword keyword_index_ext                    = { _T(".agidx"),           6  };
    keyword keyword_open_comment                 = { _T("%%*"),              3  };
    keyword keyword_close_comment                = { _T("*%%"),              3  };
    keyword keyword_line_comment                 = { _T("%%%"),              3  };

#else

    keyword keyword_include                      = { "\\include",        8  };
    keyword keyword_cntn                         = { "cntn",             4  };
    keyword keyword_content                      = { "content",          7  };
    keyword keyword_p                            = { "p",                1  };
    keyword keyword_br                           = { "br",               2  };
    keyword keyword_b                            = { "b",                1  };
    keyword keyword_i                            = { "i",                1  };
    keyword keyword_u                            = { "u",                1  };
    keyword keyword_m                            = { "m",                1  };
    keyword keyword_sup                          = { "sup",              3  };
    keyword keyword_sub                          = { "sub",              3  };
    keyword keyword_href                         = { "href",             4  };
    keyword keyword_attr_suffix                  = { "_attr",            5  };
    keyword keyword_open_suffix                  = { "_open",            5  };
    keyword keyword_close_suffix                 = { "_close",           6  };
    keyword keyword_toc                          = { "toc",              3  };
    keyword keyword_toc0                         = { "toc0",             4  };
    keyword keyword_toc_open_suffix              = { "_toc_open",        9  };
    keyword keyword_toc_close_suffix             = { "_toc_close",       10 };
    keyword keyword_page_open                    = { "page_open",        9  };
    keyword keyword_page_close                   = { "page_close",       10 };
    keyword keyword_suppress_output_elements     = { "suppress_output_elements", 24 };
    keyword keyword_this                         = { "this",             4  };
    keyword keyword_attr                         = { "attr",             4  };
    keyword keyword_iref                         = { "iref",             4  };
    keyword keyword_root_rel                     = { "root_rel",         8  };
    keyword keyword_code                         = { "code",             4  };
    keyword keyword_as_is                        = { "as_is",            5  };
    keyword keyword_line_comments                = { "line_comments",    13 };
    keyword keyword_block_comments               = { "block_comments",   14 };
    keyword keyword_line_comments_suffix         = { "_line_comments",   14 };
    keyword keyword_block_comments_suffix        = { "_block_comments",  15 };
    keyword keyword_strings_suffix               = { "_strings",         8  };
    keyword keyword_string_mask_suffix           = { "_string_mask",     12 };
    keyword keyword_operators_suffix             = { "_operators",       10 };
    keyword keyword_kw1                          = { "kw1",              3  };
    keyword keyword_kw2                          = { "kw2",              3  };
    keyword keyword_kw3                          = { "kw3",              3  };
    keyword keyword_kw4                          = { "kw4",              3  };
    keyword keyword_rem                          = { "rem",              3  };
    keyword keyword_str                          = { "str",              3  };
    keyword keyword_num                          = { "num",              3  };
    keyword keyword_op                           = { "op",               2  };
    keyword keyword_kw1_suffix                   = { "_kw1",             4  };
    keyword keyword_kw2_suffix                   = { "_kw2",             4  };
    keyword keyword_kw3_suffix                   = { "_kw3",             4  };
    keyword keyword_kw4_suffix                   = { "_kw4",             4  };
    keyword keyword_paragraph_structure_elements = { "paragraph_structure_elements", 28 };
    keyword keyword_paragraphless_elements       = { "paragraphless_elements",       22 };
    keyword keyword_paragraph_br_style_elements  = { "paragraph_br_style_elements",  27 };
    keyword keyword_output_specific_replacements = { "output_specific_replacements", 28 };
    keyword keyword_singletons                   = { "singletons",                   10 };
    keyword keyword_pair_quotes                  = { "pair_quotes",                  11 };
    keyword keyword_formatless_elements          = { "formatless_elements",          19 };
    keyword keyword_linking_keywords             = { "linking_keywords",             16 };
    keyword keyword_imgl0                        = { "imgl0",            5  };
    keyword keyword_imgr0                        = { "imgr0",            5  };
    keyword keyword_imgl                         = { "imgl",             4  };
    keyword keyword_imgr                         = { "imgr",             4  };
    keyword keyword_imgc                         = { "imgc",             4  };
    keyword keyword_img                          = { "img",              3  };
    keyword keyword_ssv_table                    = { "ssv_table",        9  };
    keyword keyword_vbar_table                   = { "vbar_table",       10 };
    keyword keyword_eln_table                    = { "eln_table",        9  };
    keyword keyword_table                        = { "table",            5  };
    keyword keyword_tr                           = { "tr",               2  };
    keyword keyword_trh                          = { "trh",              3  };
    keyword keyword_th                           = { "th",               2  };
    keyword keyword_td                           = { "td",               2  };
    keyword keyword_tdr                          = { "tdr",              3  };
    keyword keyword_tdc                          = { "tdc",              3  };
    keyword keyword_tdj                          = { "tdj",              3  };
    keyword keyword_al                           = { "al",               2  };
    keyword keyword_ul                           = { "ul",               2  };
    keyword keyword_ol                           = { "ol",               2  };
    keyword keyword_li                           = { "li",               2  };
    keyword keyword_replace                      = { "replace",          7  };
    keyword keyword_label                        = { "label",            5  };
    keyword keyword_label0                       = { "label0",           6  };
    keyword keyword_category                     = { "category",         8  };
    keyword keyword_anchor                       = { "anchor",           6  };
    keyword keyword_index                        = { "index",            5  };
    keyword keyword_figure                       = { "figure",           6  };
    keyword keyword_listing                      = { "listing",          7  };
    keyword keyword_title                        = { "title",            5  };
    keyword keyword_dot_dot                      = { "..",               2  };
    keyword keyword_dot_dot_slash                = { "../",              3  };
    keyword keyword_ext                          = { "ext",              3  };
    keyword keyword_outdir                       = { "outdir",           6  };
    keyword keyword_config                       = { "config",           6  };
    keyword keyword_process_dir                  = { "process_dir",      11 };
    keyword keyword_process_files                = { "process_files",    13 };
    keyword keyword_process_code                 = { "process_code",     12 };
    keyword keyword_copy_files                   = { "copy_files",       10 };
    keyword keyword_src                          = { "src",              3  };
    keyword keyword_lang                         = { "lang",             4  };
    keyword keyword_variables_only               = { "variables_only",   14 };
    keyword keyword_true                         = { "true",             4  };
    keyword keyword_false                        = { "false",            5  };
    keyword keyword_trim_lines                   = { "trim_lines",       10 };
    keyword keyword_remove_eof                   = { "remove_eof",       10 };
    keyword keyword_exclude                      = { "exclude",          7  };
    keyword keyword_dir                          = { "dir",              3  };
    keyword keyword_file                         = { "file",             4  };
    keyword keyword_index_ext                    = { ".agidx",           6  };
    keyword keyword_open_comment                 = { "%%*",              3  };
    keyword keyword_close_comment                = { "*%%",              3  };
    keyword keyword_line_comment                 = { "%%%",              3  };

#endif



    //------------------------------------------------------------------
    bool is_empty_line(const char_type* str, unsigned len)
    {
        unsigned lf_count = 0;
        while(len)
        {
            if(!is_space(*str)) break;
            if(is_lf(*str)) ++lf_count;
            if(lf_count > 1) return true;
            ++str;
            --len;
        }
        return false;
    }


    //------------------------------------------------------------------
    bool is_empty_line(const char_type* str)
    {
        unsigned lf_count = 0;
        while(*str)
        {
            if(!is_space(*str)) break;
            if(is_lf(*str)) ++lf_count;
            if(lf_count > 1) return true;
            ++str;
        }
        return false;
    }


    //------------------------------------------------------------------
    bool is_row_break(const char_type* str)
    {
        if(is_lf(*str))
        {
            if(is_minus(str[1]) && is_minus(str[2]) && is_minus(str[3]) && is_minus(str[4]))
            {
                return true;
            }

        }
        return false;
    }


    //------------------------------------------------------------------
    bool is_double_vbar(const char_type* str)
    {
        if(*str && *str == vbar)
        {
            ++str;
            if(*str && *str == vbar) return true;
        }
        return false;
    }

    //------------------------------------------------------------------
    unsigned skip_lf(const char_type* str, unsigned len, unsigned count)
    {
        unsigned lf_count = 0;
        unsigned ret = 0;
        while(len)
        {
            if(!is_space(*str)) break;
            if(is_lf(*str)) ++lf_count;
            if(lf_count >= count) break;
            ++ret;
            ++str;
            --len;
        }
        return ret;
    }

    //------------------------------------------------------------------
    string_type from_ascii(const std::string& s)
    {
        string_type ret;
        ret.reserve(s.length());
        unsigned i;
        for(i = 0; i < s.length(); ++i) ret += (char_type)s[i];
        return ret;
    }

    //------------------------------------------------------------------
    string_type from_ascii(const char* s)
    {
        string_type ret;
        while(*s) ret += (char_type)*s++;
        return ret;
    }

    //------------------------------------------------------------------
    std::string to_ascii(const string_type& s)
    {
        std::string ret;
        ret.reserve(s.length());
        unsigned i;
        for(i = 0; i < s.length(); ++i) ret += (char)s[i];
        return ret;
    }

    //------------------------------------------------------------------
    std::string to_ascii(const char_type* s)
    {
        std::string ret;
        while(*s) ret += (char)*s++;
        return ret;
    }


    //------------------------------------------------------------------
    bool str_is(const char_type* p, const string_type& s)
    {
        for(unsigned i = 0; i < s.length(); ++i)
        {
            if(*p == 0) return false;
            if(*p != s[i]) return false;
            ++p;
        }
        return true;
    }

    //------------------------------------------------------------------
    bool str_is(const char_type* p, unsigned len, const string_type& s)
    {
        for(unsigned i = 0; i < s.length(); ++i)
        {
            if(len == 0) return false;
            if(*p == 0) return false;
            if(*p != s[i]) return false;
            --len;
            ++p;
        }
        return true;
    }


    //------------------------------------------------------------------
    void clean_and_trim_string(string_type& s)
    {
        unsigned i;
        unsigned j = 0;
        bool spaces = true;
        for(i = 0; i < s.length(); i++)
        {
            bool skip = false;
            if(s[i] == ignore_char || s[i] == reserved_char)
            {
                skip = true;
            }
            else
            {
                if(is_space(s[i]))
                {
                    if(spaces)
                    {
                        skip = true;
                    }
                }
                else
                {
                    spaces = false;
                }
            }
            if(!skip)
            {
                s[j++] = s[i];
            }
        }
        while(j)
        {
            if(!is_space(s[--j]))
            {
                ++j;
                break;
            }
        }
        s.erase(j);
    }


    //------------------------------------------------------------------
    void trim_lines(string_type& s)
    {
        unsigned i;
        unsigned j = 0;
        bool start_line = true;
        for(i = 0; i < s.length();)
        {
            if(start_line)
            {
                for(; i < s.length(); ++i)
                {
                    if(!is_space(s[i]))
                    {
                        start_line = false;
                        break;
                    }
                }
                continue;
            }

            s[j++] = s[i++];

            if(!start_line && j > 0)
            {
                if(s[j - 1] == lf)
                {
                    --j;
                    while(j && is_space(s[j])) --j;
                    ++j;
                    s[j++] = lf;
                    start_line = true;
                }
            }
        }
        s.erase(j);
    }


    //------------------------------------------------------------------
    char_type replace_keysym(char_type c)
    {
        switch(c)
        {
        case backslash     : c = backslash_restore;     break;
        case dollar        : c = dollar_restore;        break;
        case open_brace    : c = open_brace_restore;    break;
        case close_brace   : c = close_brace_restore;   break;
        case open_bracket  : c = open_bracket_restore;  break;
        case close_bracket : c = close_bracket_restore; break;
        }
        return c;
    }


    //------------------------------------------------------------------
    char_type restore_keysym(char_type c)
    {
        switch(c)
        {
        case backslash_restore     : c = backslash;      break;
        case dollar_restore        : c = dollar;         break;
        case open_brace_restore    : c = open_brace;     break;
        case close_brace_restore   : c = close_brace;    break;
        case open_bracket_restore  : c = open_bracket;   break;
        case close_bracket_restore : c = close_bracket;  break;
        }
        return c;
    }



    //------------------------------------------------------------------
    bool parse_and_find_attr_variable(const char_type* attr,
                                      unsigned attr_len,
                                      const string_type& name,
                                      string_type& val)
    {
        const char_type* p = attr;
        const char_type* end = attr + attr_len;
        const char_type* start = 0;
        val.erase();

        if(*p == quote)
        {
            ++p;
            while(p < end)
            {
                if(*p == backslash)
                {
                    ++p;
                }
                else
                {
                    if(*p == quote)
                    {
                        ++p;
                        break;
                    }
                }
                ++p;
            }
        }


        while(p < end)
        {
            if(start == 0 && p < end && is_alpha(*p))
            {
                start = p;
            }
            if(start && p < end && !is_element_name(*p))
            {
                if(str_cmp(name, start, (unsigned(p - start))) == 0)
                {
                    while(p < end && is_space(*p)) ++p;

                    if(*p == equal)
                    {
                        ++p;
                        while(p < end && is_space(*p)) ++p;

                        if(p < end)
                        {
                            if(*p == semicolon)
                            {
                                return true;
                            }
                            start = p++;
                            bool qu = *start == quote;
                            if(qu) ++start;

                            while(p < end)
                            {
                                if(qu)
                                {
                                    if(*p == quote &&
                                        (*(p - 1) != backslash_restore &&
                                         *(p - 1) != reserved_char)) break;
                                }
                                else
                                {
                                    if(*p == semicolon) break;
                                }
                                ++p;
                            }
                            if(start < p && *start == quote) ++start;
                            if(p > start)
                            {
                                if(qu)
                                {
                                    if(*p == quote) --p;
                                }
                                else
                                {
                                    if(*p == semicolon) --p;
                                    while(p > start && is_space(*p)) --p;
                                }
                                if(p < end) ++p;
                            }
                            if(p > start)
                            {
                                val.reserve(unsigned(p - start + 1));
                                while(start < p)
                                {
                                    if(start[1] &&
                                        (*start == reserved_char ||
                                         *start == backslash_restore)) ++start;
                                    val += *start++;
                                }
                            }
                        }
                        return true;
                    }
                }
                else
                {
                    start = 0;
                }
            }
            ++p;
        }
        return false;
    }



    //------------------------------------------------------------------
    bool parse_and_find_attr_variable(const char_type* attr,
                                      unsigned attr_len,
                                      const keyword& name,
                                      string_type& val)
    {
        return parse_and_find_attr_variable(attr,
                                            attr_len,
                                            string_type(name.name, name.len),
                                            val);
    }



    //------------------------------------------------------------------
    void parse_and_find_attr_content(const char_type* attr,
                                     unsigned attr_len,
                                     string_type& content)
    {
        const char_type* start    = attr;
        const char_type* end      = attr;
        const char_type* attr_end = attr + attr_len;
        content.erase();

        if(*end == quote)
        {
            ++end;
            start = end;
            while(end < attr_end)
            {
                if(*end == backslash)
                {
                    ++end;
                }
                else
                {
                    if(*end == quote) break;
                }
                ++end;
            }
        }
        else
        {
            while(end < attr_end)
            {
                if(*end == backslash)
                {
                    ++end;
                    ++end;
                }
                else
                {
                    if(*end == equal) return;
                    if(*end == semicolon) break;
                    ++end;
                }
            }
        }

        if(start < end)
        {
            content.reserve(unsigned(end - start));
            while(start < end)
            {
                if(*start == backslash)
                {
                    ++start;
                    if(start < end) content += *start++;
                }
                else
                {
                    content += *start++;
                }
            }
        }
    }


    //------------------------------------------------------------------
    const string_type* find_replacement(const string_list& repl,
                                        const char_type* str,
                                        unsigned len,
                                        unsigned* ret_len)
    {
        unsigned i;
        unsigned n = repl.size();
        n &= ~1; // decrease n if it's an odd number
        for(i = 0; i < n; i += 2)
        {
            const string_type& from = repl[i];
            const string_type& to   = repl[i + 1];
            if(str_is(str, len, from))
            {
                *ret_len = from.length();
                return &to;
            }
        }
        return 0;
    }



    //------------------------------------------------------------------
    void add_to_charset(charset_type& ch, const string_list& repl)
    {
        unsigned i;
        unsigned n = repl.size();
        n &= ~1; // decrease n if it's an odd number
        for(i = 0; i < n; i += 2)
        {
            const string_type& str = repl[i];
            unsigned j;
            for(j = 0; j < str.length(); ++j)
            {
                ch.insert(str[j]);
            }
        }
    }






    //==================================================================
    //
    // parsing_exception
    //
    //==================================================================



    //------------------------------------------------------------------
    parsing_exception::parsing_exception(const char_type* msg) :
        m_line_num(-1)
    {
        copy_message(msg);
    }

    //------------------------------------------------------------------
    parsing_exception::parsing_exception(const char_type* msg,
                                         const char_type* arg1) :
        m_line_num(-1)
    {
        copy_message(msg, arg1);
    }

    //------------------------------------------------------------------
    parsing_exception::parsing_exception(const char_type* msg,
                                         const char_type* arg1,
                                         const char_type* arg2) :
        m_line_num(-1)
    {
        copy_message(msg, arg1, arg2);
    }

    //------------------------------------------------------------------
    parsing_exception::parsing_exception(const src_line& line, const char_type* msg) :
        m_line_num(line.num)
    {
        copy_message(msg);
        make_line_content(line);
    }


    //------------------------------------------------------------------
    parsing_exception::parsing_exception(const src_line& line,
                                         const char_type* msg,
                                         const char_type* arg1) :
        m_line_num(line.num)
    {
        copy_message(msg, arg1);
        make_line_content(line);
    }


    //------------------------------------------------------------------
    parsing_exception::parsing_exception(const src_line& line,
                                         const char_type* msg,
                                         const char_type* arg1,
                                         const char_type* arg2) :
        m_line_num(line.num)
    {
        copy_message(msg, arg1, arg2);
        make_line_content(line);
    }


    //------------------------------------------------------------------
    void parsing_exception::copy_message(const char_type* msg)
    {
        while(*msg) m_message += *(char_type*)msg++;
    }


    //------------------------------------------------------------------
    void parsing_exception::copy_message(const char_type* msg,
                                         const char_type* arg1)
    {
        char buf[4096];
        sprintf(buf, msg, to_ascii(arg1).c_str());
        copy_message(buf);
    }


    //------------------------------------------------------------------
    void parsing_exception::copy_message(const char_type* msg,
                                         const char_type* arg1,
                                         const char_type* arg2)
    {
        char buf[4096];
        sprintf(buf, msg, to_ascii(arg1).c_str(), to_ascii(arg2).c_str());
        copy_message(buf);
    }

    //------------------------------------------------------------------
    void parsing_exception::make_line_content(const src_line& line)
    {
        m_line_content.reserve(line.len + 1);
        if(line.ptr)
        {
            for(unsigned i = 0; i < line.len; ++i)
            {
                char_type c = line.ptr[i];
                if(c != ignore_char && c != lf)
                {
                    c = restore_keysym(c);
                    if(c == element_replacement_char) c = backslash;
                    m_line_content += c;
                }
            }
        }
    }




    //==================================================================
    //
    // string_list
    //
    //==================================================================


    //------------------------------------------------------------------
    void string_list::parse(const char_type* str, unsigned len, const char_type sep)
    {
        while(len)
        {
            while(len && (is_space(*str) || *str == sep))
            {
                ++str;
                --len;
            }
            const char_type* start = str;
            while(len && *str && (!is_space(*str) && *str != sep))
            {
                ++str;
                --len;
            }
            if(str > start)
            {
                m_strings.push_back(string_type(start, unsigned(str - start)));
            }
        }
    }


    //------------------------------------------------------------------
    bool string_list::exists(const char_type* str, unsigned len) const
    {
        unsigned i;
        for(i = 0; i < m_strings.size(); i++)
        {
            if(str_cmp(str, len, m_strings[i].c_str(), m_strings[i].length()) == 0)
            {
                return true;
            }
        }
        return false;
    }



    //------------------------------------------------------------------
    bool string_list::char_exists(const char_type c) const
    {
        unsigned i;
        for(i = 0; i < m_strings.size(); i++)
        {
            if(m_strings[i].find(c) != string_type::npos)
            {
                return true;
            }
        }
        return false;
    }





    //==================================================================
    //
    // string_set
    //
    //==================================================================


    //----------------------------------------------------@string_set::parse
    void string_set::parse(const char_type* str, unsigned len, const char_type sep)
    {
        string_list lst(str, len, sep);
        for(unsigned i = 0; i < lst.size(); i++)
        {
            m_set.insert(lst[i]);
        }
    }


    //----------------------------------------------------@string_set::add
    void string_set::add(const strset_type* strset)
    {
        if(strset)
        {
            strset_type::const_iterator it = strset->begin();
            while(it != strset->end())
            {
                add(*it);
                ++it;
            }
        }
    }










    //==================================================================
    //
    // element
    //
    //==================================================================


    //------------------------------------------------------------------
    element::element(char_type* src_text, const string_set* solid_elements) :
        m_src_text(src_text),
        m_solid_elements(solid_elements),
        m_name(0),
        m_name_len(0),
        m_attr(0),
        m_attr_len(0),
        m_content(0),
        m_content_len(0),
        m_total_len(0),
        m_brace_count(0),
        m_parent(0)
    {
    }


    //------------------------------------------------------------------
    src_line element::line(const char_type* ptr) const
    {
        src_line ln;
        ln.len = 0;
        ln.ptr = 0;
        ln.num = -1;

        if(m_src_text && ptr)
        {
            const char_type* p = m_src_text;
            int count = 1;
            while(p < ptr)
            {
                if(*p == 0) break;
                if(is_lf(*p)) ++count;
                ++p;
            }
            if(*p)
            {
                ln.num = count;

                // Look for the begining and the end of the line
                while(p > m_src_text)
                {
                    if(is_lf(*--p)) break;
                }
                ++p;
                while(p[ln.len])
                {
                    if(is_lf(p[ln.len++])) break;
                }
                ln.ptr = p;
            }
        }
        return ln;
    }


    //------------------------------------------------------------------
    const element* element::find_first_element(const string_type& name) const
    {
        unsigned i;
        for(i = 0; i < m_elements.size(); i++)
        {
            if(str_cmp(m_elements[i].name(), m_elements[i].name_len(),
                       name.c_str(), name.length()) == 0)
            {
                return &(m_elements[i]);
            }
        }
        return 0;
    }



    //------------------------------------------------------------------
    const element* element::recursive_search(const string_type& name) const
    {
        const element* e = find_first_element(name);
        if(e) return e;

        unsigned i;
        for(i = 0; i < m_elements.size(); i++)
        {
            e = m_elements[i].recursive_search(name);
            if(e) return e;
        }
        return 0;
    }



    //------------------------------------------------------------------
    const element* element::find_first_from_root(const string_type& name) const
    {
        const element* root = this;
        while(root->m_parent)
        {
            root = root->m_parent;
        }
        return root->recursive_search(name);
    }


    //------------------------------------------------------------------
    const element* element::find_first_to_root(const string_type& name) const
    {
        const element* e = this;
        while(e)
        {
            const element* ret = e->recursive_search(name);
            if(ret) return ret;
            e = e->m_parent;
        }
        return 0;
    }


    //------------------------------------------------------------------
    const string_type*
    element::find_variable_to_root(const strmap_type& cfg,
                                   const string_type& name) const
    {
        const element* e = this;
        string_type elname;
        do
        {
            if(e->name_len())
            {
                elname.assign(e->name(), e->name_len());
                elname.append(1, underscore);
                elname.append(name);
                strmap_type::const_iterator it = cfg.find(elname);
                if(it != cfg.end())
                {
                    return &(it->second);
                }
            }
            e = e->m_parent;
        }
        while(e);
        return 0;
    }


    //------------------------------------------------------------------
    void element::throw_exception(const char_type* ptr, const char_type* msg) const
    {
        throw parsing_exception(line(ptr), msg);
    }

    //------------------------------------------------------------------
    void element::throw_exception(const char_type* ptr,
                                  const char_type* msg,
                                  const char_type* arg1) const
    {
        throw parsing_exception(line(ptr), msg, arg1);
    }

    //------------------------------------------------------------------
    void element::throw_exception(const char_type* ptr,
                                  const char_type* msg,
                                  const char_type* arg1,
                                  const char_type* arg2) const
    {
        throw parsing_exception(line(ptr), msg, arg1, arg2);
    }


    //------------------------------------------------------------------
    char_type* element::parse_attr(char_type* text)
    {
        unsigned nb = 1;
        const char_type* start = text;
        while(*text)
        {
            if(is_backslash(*text))
            {
                const char_type* p = text;
                while(is_backslash(*p)) ++p;
                if(is_open_bracket(*p) || is_close_bracket(*p))
                {
                    *text++ = reserved_char;
                    while(text < p) *text++ = backslash_restore;
                    *text++ = (*p == open_bracket) ? open_bracket_restore :
                                                     close_bracket_restore;
                    continue;
                }
                *text++ = backslash_restore;
                continue;
            }

            if(is_open_bracket(*text))
            {
                *text = open_bracket_restore;
                ++nb;
            }
            else
            if(is_close_bracket(*text))
            {
                *text = close_bracket_restore;
                if(--nb == 0)
                {
                    //*text = reserved_char;
                    return text;
                }
            }
            else if(is_open_brace(*text))  *text = open_brace_restore;
            else if(is_close_brace(*text)) *text = close_brace_restore;
            ++text;
        }
        throw_exception(start, "Bad Attribute Syntax (brackets ain't balanced)");
        return 0;
    }






    //------------------------------------------------------------------
    char_type* element::parse_element(char_type* text)
    {
        if(is_alpha(text[1]))
        {
            char_type* start = text;
            *text++ = element_replacement_char;
            m_elements.push_back(element(m_src_text, m_solid_elements));
            element& e = m_elements[m_elements.size() - 1];
            e.m_name   = text;
            while(is_element_name(*text)) ++text;
            e.m_name_len = unsigned(text - e.m_name);

            char_type* end_tag = text;
            while(is_space(*text)) *text++;

            if(is_open_bracket(*text))
            {
                ++text;
                e.m_attr = text;
                text = parse_attr(text);
                e.m_attr_len = unsigned(text - e.m_attr);
                ++text;
                end_tag = text;
                while(is_space(*text)) *text++;
            }

            if(is_open_brace(*text))
            {
                ++text;
                e.m_total_len   = unsigned(text - start);
                e.m_brace_count = 1;
                if(m_solid_elements && m_solid_elements->exists(e.m_name, e.m_name_len))
                {
                    e.m_content = text;
                    while(*text)
                    {
                        if(is_backslash(*text))
                        {
                            *text = backslash_restore;
                        }
                        if(is_dollar(*text))
                        {
                            *text = dollar_restore;
                        }
                        if(is_open_brace(*text))
                        {
                            *text = open_brace_restore;
                            if(*(text - 1) != backslash_restore) ++e.m_brace_count;
                        }
                        if(is_close_brace(*text))
                        {
                            if(*(text - 1) != backslash_restore) --e.m_brace_count;
                            if(e.m_brace_count < 1)
                            {
                                break;
                            }
                            *text = close_brace_restore;
                        }
                        ++text;
                    }
                    e.m_content_len = unsigned(text - e.m_content);
                }
                else
                {
                    text = e.parse_nested(text);
                }
                if(is_close_brace(*text)) ++text;
            }
            else
            {
                text = end_tag;
                e.m_content     = 0;
                e.m_content_len = 0;
            }
            m_elements[m_elements.size() - 1].m_total_len = unsigned(text - start);
        }
        else
        {
            *text = reserved_char;
            ++text;
            if(*text == backslash)   *text = backslash_restore;
            if(*text == open_brace)  *text = open_brace_restore;
            if(*text == close_brace) *text = close_brace_restore;
        }

        return text;
    }





    //------------------------------------------------------------------
    char_type* element::parse_content(char_type* text)
    {
        while(*text)
        {
            if(is_backslash(*text))
            {
                text = parse_element(text);
            }
            else
            {
                if(m_brace_count)
                {
                    if(is_open_brace(*text))
                    {
                        ++m_brace_count;
                    }
                    if(is_close_brace(*text))
                    {
                        --m_brace_count;
                        if(m_brace_count < 1)
                        {
                            break;
                        }
                    }
                }
                if(is_cr(*text)) *text = ignore_char;
                ++text;
            }
        }
        return text;
    }




    //------------------------------------------------------------------
    char_type* element::parse_nested(char_type* text)
    {
        if(text == 0) return 0;
        m_content = text;
        while(*text)
        {
            if(is_backslash(*text))
            {
                text = parse_element(text);
            }
            else
            {
                text = parse_content(text);
                break;
            }
        }

        m_content_len = unsigned(text - m_content);
        return text;
    }



    //------------------------------------------------------------------
    void element::set_parent(element* parent)
    {
        m_parent = parent;
        for(unsigned i = 0; i < m_elements.size(); i++)
        {
            m_elements[i].set_parent(this);
        }
    }



    //------------------------------------------------------------------
    char_type* element::parse()
    {
        char_type* end = parse_nested(m_src_text);
        for(unsigned i = 0; i < m_elements.size(); i++)
        {
            m_elements[i].set_parent(this);
        }
        m_total_len = unsigned(end - m_src_text);
        return end;
    }









/*
    void element::dump_names(FILE* fd, unsigned offset) const
    {
        unsigned i;
        unsigned cmd_idx = 0;
        if(m_name)
        {
            for(i = 0; i < offset; i++) fputc(' ', fd);
            fputc('[', fd);
            for(i = 0; i < m_name_len; i++) fputc(m_name[i], fd);
            fprintf(fd, ".%d.%d", m_total_len, m_content_len);

            const element* e = this;
            while(e)
            {
                fprintf(fd, ".%x", e);
                e = e->m_parent;
            }

            fprintf(fd, "]\n");
        }
        for(i = 0; i < m_elements.size(); i++)
        {
            m_elements[i].dump_names(fd, offset + 1);
        }
    }
*/

    void element::DumpData(const char_type* pContent, unsigned content_len) const
    {
        wxString sLine = _T("---------------------------------------------------------\n");
        ltLogDbg(sLine);
        wxString sStr = wxString::Format(_T("Name=%s, Attr=%s"), GetName().c_str(), GetAttr().c_str());
        ltLogDbg(sStr);
        sStr = ToString(pContent, (int)content_len);
        ltLogDbg(sStr);
        ltLogDbg(sLine);
    }


    void element::dump_all(FILE* fd, unsigned offset) const
    {
        unsigned i, j;
        unsigned cmd_idx = 0;
        if(m_name_len)
        {
            fputc('\\', fd);
            for(j = 0; j < m_name_len; j++) { fputc(m_name[j], fd); }
            if(m_attr_len)
            {
                fputc('[', fd);
                for(j = 0; j < m_attr_len; j++) { fputc(m_attr[j], fd); }
                fputc(']', fd);
            }
            fputc('{', fd);
        }
        for(i = 0; i < m_content_len;)
        {
            if(m_content[i] == element_replacement_char)
            {
                if(cmd_idx < m_elements.size())
                {
                    const element& e = m_elements[cmd_idx];
                    e.dump_all(fd, offset + 1);
                    i += e.m_total_len;
                    cmd_idx++;
                }
                else
                {
                    fprintf(fd, "!!!!!!!ELEMENT COUNT ERROR\n");
                }
            }
            else
            {
                fputc(m_content[i], fd);
                ++i;
            }
        }
        if(m_name_len)
        {
            fputc('}',  fd);
        }
    }






    //==================================================================
    //
    // config
    //
    //==================================================================


    //------------------------------------------------------------------
    config::config(const char_type* file_name, bool trim_flag) :
        m_level(0),
        m_trim_flag(trim_flag)
    {
        ifile f(file_name);
        f.elements().process(*this);
        m_root += dot;
        process_keywords();
    }



    //------------------------------------------------------------------
    void config::add_keywords(const string_type& category)
    {
        const string_type* str = find(category);
        if(str)
        {
            string_set kw(str);
            if(kw.strings().size())
            {
                m_keywords[category] = kw.strings();
            }
        }
    }

    //------------------------------------------------------------------
    void config::add_keyword(const keyword& category,
                             const char_type* name, unsigned len)
    {
        m_keywords[string_type(category.name, category.len)].insert(string_type(name, len));
    }


    //------------------------------------------------------------------
    void config::add_keywords(const keyword& category)
    {
        add_keywords(string_type(category.name, category.len));
    }


    //------------------------------------------------------------------
    void config::add_keywords(const string_type& language, const keyword& suffix)
    {
        string_type category;
        category.reserve(language.length() + suffix.len);
        category.append(language);
        category.append(suffix.name, suffix.len);
        add_keywords(category);
    }


    //------------------------------------------------------------------
    void config::add_strlist(const string_type& category)
    {
        const string_type* str = find(category);
        if(str)
        {
            string_list kw(str);
            if(kw.size())
            {
                m_strlists[category] = kw;
            }
        }
    }


    //------------------------------------------------------------------
    void config::add_strlist(const keyword& category)
    {
        add_strlist(string_type(category.name, category.len));
    }


    //------------------------------------------------------------------
    void config::add_strlist(const string_type& language, const keyword& suffix)
    {
        string_type category;
        category.reserve(language.length() + suffix.len);
        category.append(language);
        category.append(suffix.name, suffix.len);
        add_strlist(category);
    }


    //------------------------------------------------------------------
    void config::restore_strlist_keysym(const keyword& category)
    {
        string_list& lst = strlist(category);
        unsigned i;
        for(i = 0; i < lst.size(); i++)
        {
            string_type& s = lst[i];
            std::transform(s.begin(), s.end(), s.begin(), restore_keysym);
        }
    }


    //------------------------------------------------------------------
    void config::process_keywords()
    {
        add_strlist(keyword_line_comments);
        add_strlist(keyword_block_comments);
        add_strlist(keyword_output_specific_replacements);
        add_strlist(keyword_singletons);
        add_strlist(keyword_pair_quotes);
        restore_strlist_keysym(keyword_singletons);
        restore_strlist_keysym(keyword_pair_quotes);

        add_keywords(keyword_suppress_output_elements);
        add_keywords(keyword_code);
        add_keywords(keyword_paragraph_structure_elements);
        add_keywords(keyword_paragraphless_elements);
        add_keywords(keyword_paragraph_br_style_elements);
        add_keywords(keyword_formatless_elements);
        add_keywords(keyword_linking_keywords);

        const strset_type* languages = find_keywords(keyword_code);
        if(languages)
        {
            strset_type::const_iterator it = languages->begin();
            for(; it != languages->end(); ++it)
            {
                add_strlist(*it, keyword_line_comments_suffix);
                add_strlist(*it, keyword_block_comments_suffix);
                add_strlist(*it, keyword_strings_suffix);
                add_strlist(*it, keyword_string_mask_suffix);
                add_strlist(*it, keyword_operators_suffix);

                add_keywords(*it, keyword_kw1_suffix);
                add_keywords(*it, keyword_kw2_suffix);
                add_keywords(*it, keyword_kw3_suffix);
                add_keywords(*it, keyword_kw4_suffix);
            }
        }
        m_solid_elements.add(string_type(keyword_as_is.name, keyword_as_is.len));
        m_solid_elements.add(find_keywords(keyword_formatless_elements));
        m_solid_elements.add(find_keywords(keyword_code));
    }



    //------------------------------------------------------------------
    const strset_type* config::find_keywords(const string_type& category) const
    {
        strmapset_type::const_iterator i = m_keywords.find(category);
        return (i == m_keywords.end()) ? 0 : &(i->second);
    }


    //------------------------------------------------------------------
    const strset_type* config::find_keywords(const keyword& category) const
    {
        return find_keywords(string_type(category.name, category.len));
    }


    //------------------------------------------------------------------
    const strset_type* config::find_keywords(const string_type& prefix, const keyword& suffix) const
    {
        string_type category = prefix;
        category.append(suffix.name, suffix.len);
        return find_keywords(category);
    }


    //------------------------------------------------------------------
    bool config::keyword_exists(const string_type& category, const char_type* name, unsigned name_len) const
    {
        if(name == 0 || name_len == 0) return false;
        strmapset_type::const_iterator i1 = m_keywords.find(category);
        if(i1 != m_keywords.end())
        {
            strset_type::const_iterator i2 = i1->second.find(string_type(name, name_len));
            return i2 != i1->second.end();
        }
        return false;
    }


    //------------------------------------------------------------------
    bool config::keyword_exists(const keyword& category, const char_type* name, unsigned name_len) const
    {
        return keyword_exists(string_type(category.name, category.len), name, name_len);
    }


    //------------------------------------------------------------------
    bool config::keyword_exists(const string_type& prefix, const keyword& suffix,
                                const char_type* name, unsigned name_len) const
    {
        string_type category = prefix;
        category.append(suffix.name, suffix.len);
        return keyword_exists(category, name, name_len);
    }


    //------------------------------------------------------------------
    const string_list& config::strlist(const string_type& category) const
    {
        strmaplist_type::const_iterator i = m_strlists.find(category);
        return (i == m_strlists.end()) ? m_empty_list : i->second;
    }


    //------------------------------------------------------------------
    const string_list& config::strlist(const keyword& category) const
    {
        return strlist(string_type(category.name, category.len));
    }


    //------------------------------------------------------------------
    const string_list& config::strlist(const string_type& prefix, const keyword& suffix) const
    {
        string_type category = prefix;
        category.append(suffix.name, suffix.len);
        return strlist(category);
    }


    //------------------------------------------------------------------
    string_list& config::strlist(const keyword& category)
    {
        strmaplist_type::iterator i = m_strlists.find(string_type(category.name, category.len));
        return (i == m_strlists.end()) ? m_empty_list : i->second;
    }


    //------------------------------------------------------------------
    void config::start_element(const element& e)
    {
        ++m_level;
        if(e.name_len())
        {
            if(m_level != 2)
            {
                throw config_exception(e.line(e.name()),
                                       "Nested elements are not allowed in the config file");
            }
            m_name.assign(e.name(), e.name_len());
            if(e.attr_len())
            {
                string_type s(e.attr(), e.attr_len());
                if(m_trim_flag) trim_lines(s);
                if(!s.empty())
                {
                    m_elements[m_name] = s;
                }
            }
        }
    }


    //------------------------------------------------------------------
    void config::end_element(const element&)
    {
        --m_level;
    }


    //------------------------------------------------------------------
    void config::content(const element& WXUNUSED(e),
                         const char_type* c, unsigned len)
    {
        if(m_level == 2 && !m_name.empty() && len)
        {
            string_type s(c, len);
            if(m_trim_flag) trim_lines(s);
            if(!s.empty())
            {
                m_elements[m_name] = s;
            }
        }
    }


    //------------------------------------------------------------------
    const string_type* config::find(const string_type& name) const
    {
        strmap_type::const_iterator i = m_elements.find(name);
        if(i == m_elements.end()) return 0;
        return &(i->second);
    }


    //------------------------------------------------------------------
    const string_type* config::find(const char_type* name, unsigned name_len) const
    {
        return find(string_type(name, name_len));
    }


    //------------------------------------------------------------------
    const string_type* config::find(const keyword& kw) const
    {
        return find(kw.name, kw.len);
    }


    //------------------------------------------------------------------
    const string_type* config::find(const char_type* name, unsigned name_len,
                                    const keyword& suffix) const
    {
        string_type n;
        n.reserve(name_len + suffix.len + 1);
        n.append(name, name_len);
        n.append(suffix.name, suffix.len);
        return find(n);
    }


    //------------------------------------------------------------------
    void config::push_subdir()
    {
        if(m_root_path.length()) m_root_path += slash;
        m_root_path += dot;
        m_root_path += dot;
    }


    //------------------------------------------------------------------
    void config::pop_subdir()
    {
        if(m_root_path.length() < 3) m_root_path.erase();
        else m_root_path.erase(m_root_path.length() - 3);
    }


    //------------------------------------------------------------------
    const string_type& config::root_rel() const
    {
        if(m_root_path.length()) return m_root_path;
        return m_root;
    }


    //------------------------------------------------------------------
    void config::dump(FILE* fd) const
    {
        strmap_type::const_iterator i = m_elements.begin();
        for(; i != m_elements.end(); ++i)
        {
            fprintf(fd, "%s : [%s]\n", i->first.c_str(), i->second.c_str());
        }
    }

    //------------------------------------------------------------------
    bool config::is_solid_element(const char_type* name, unsigned len) const
    {
        return m_solid_elements.exists(name, len);
    }










    //==================================================================
    //
    // content_storage
    //
    //==================================================================


    //------------------------------------------------------------------
    content_storage::~content_storage()
    {
        delete m_element;
    }

    //------------------------------------------------------------------
    content_storage::content_storage(const string_set* m_solid_elements) :
        m_solid_elements(m_solid_elements),
        m_element(0),
        m_restore_keysym_flag(false),
        m_recursion_level(0)
    {
    }

    //------------------------------------------------------------------
    void content_storage::clear()
    {
        m_buffer.clear();
        delete m_element;
        m_element = 0;
        m_restore_keysym_flag = false;
    }


    //------------------------------------------------------------------
    void content_storage::reserve(unsigned r)
    {
        clear();
        if(r) m_buffer.reserve(r);
    }


    //------------------------------------------------------------------
    void content_storage::assign(const content_storage& c)
    {
        m_buffer = c.m_buffer;
        delete m_element;
        m_element = 0;
        m_solid_elements = c.m_solid_elements;
        m_restore_keysym_flag = c.m_restore_keysym_flag;
    }


    //------------------------------------------------------------------
    void content_storage::add(char_type c, bool repl_keysym)
    {
        if(c != ignore_char)
        {
            if(m_restore_keysym_flag)
            {
                c = restore_keysym(c);
            }
            else
            {
                if(repl_keysym)
                {
                    c = replace_keysym(c);
                }
            }
            m_buffer.push_back(c);
        }
    }


    //------------------------------------------------------------------
    void content_storage::add(const string_type& str)
    {
        unsigned i;
        for(i = 0; i < str.length(); i++)
        {
            add(str[i]);
        }
    }


    //------------------------------------------------------------------
    void content_storage::add(const string_type* str)
    {
        if(str) add(*str);
    }


    //------------------------------------------------------------------
    void content_storage::add(const char_type* str, unsigned len, bool repl_keysym)
    {
        if(str)
        {
            unsigned i;
            for(i = 0; i < len; i++)
            {
                add(str[i], repl_keysym);
            }
        }
    }


    //------------------------------------------------------------------
    void content_storage::add(const char_type* str)
    {
        while(*str)
        {
            add(*str++);
        }
    }


    //------------------------------------------------------------------
    void content_storage::add(const keyword& el, const char_type* content, unsigned len, bool repl_keysym)
    {
        add(backslash);
        add(el.name, el.len);
        add(open_brace);
        add(content, len, repl_keysym);
        add(close_brace);
    }


    //------------------------------------------------------------------
    void content_storage::add_element_header(const element& e)
    {
        if(e.name_len())
        {
            add(backslash);
            add(e.name(), e.name_len());
            if(e.attr_len())
            {
                add(open_bracket);
                add(e.attr(), e.attr_len());
                add(close_bracket);
            }
            add(open_brace);
        }
    }


    //------------------------------------------------------------------
    void content_storage::add_element_footer(const element& e)
    {
        if(e.name_len())
        {
            add(close_brace);
        }
    }




    // Process variables

    //------------------------------------------------------------------
    void content_storage::add_whole_element(const element& e, const config& cfg)
    {
        content_storage tmp;
        content_serializer c(tmp);
        e.process(c);
        add_str_with_variables(e, tmp.text(), tmp.length(), cfg);
    }


    //------------------------------------------------------------------
    void content_storage::add_element_content(const element& e, const config& cfg)
    {
        const element* el =
            e.find_first_element(string_type(keyword_cntn.name,
                                             keyword_cntn.len));
        if(el)
        {
            add_whole_element(*el, cfg);
        }
    }


    //------------------------------------------------------------------
    void content_storage::add_attr_content(const element& e, const config& cfg)
    {
        if(e.attr_len())
        {
            string_type content;
            parse_and_find_attr_content(e.attr(), e.attr_len(), content);
            add_str_with_variables(e, content.c_str(), content.length(), cfg);
        }
    }


    //------------------------------------------------------------------
    void content_storage::add_attr_variable(const element& e,
                                            const string_type& name,
                                            const config& cfg)
    {
        if(e.attr_len())
        {
            string_type val;
            if(parse_and_find_attr_variable(e.attr(), e.attr_len(), name, val))
            {
                add_str_with_variables(e, val.c_str(), val.length(), cfg);
            }
        }
    }


    //------------------------------------------------------------------
    void content_storage::add_variable(const char_type* name,
                                       unsigned name_len,
                                       const element& e,
                                       const config& cfg)
    {
        string_list tokens(name, name_len, dot);
        if(tokens.size())
        {
            if(str_cmp(tokens[0], keyword_this) == 0)
            {
                if(tokens.size() > 1)
                {
                    if(str_cmp(tokens[1], keyword_attr) == 0)
                    {
                        if(tokens.size() > 2)
                        {
                            add_attr_variable(e, tokens[2], cfg);
                        }
                        else
                        {
                            add_attr_content(e, cfg);
                        }
                    }
                    else if(str_cmp(tokens[1], keyword_content) == 0)
                    {
                        add_element_content(e, cfg);
                    }
                }
                else
                {
                    throw parsing_exception(e.line(name), "Recursive 'this' detected");
                }
            }
            else if(str_cmp(tokens[0], keyword_root_rel) == 0)
            {
                add(cfg.root_rel());
            }
            else
            {
                const string_type* pval;
                if(e.name_len() &&
                    (pval = e.find_variable_to_root(cfg.all_elements(), tokens[0])) != 0)
                {
                    add_str_with_variables(e, pval->c_str(), pval->length(), cfg);
                }
                else
                if((pval = cfg.find(tokens[0])) != 0)
                {
                    add_str_with_variables(e, pval->c_str(), pval->length(), cfg);
                }
                else
                {
                    // Process element insertion
                    const element* el = e.find_first_to_root(tokens[0]);
                    if(el)
                    {
                        if(tokens.size() > 1)
                        {
                            if(str_cmp(tokens[1], keyword_attr) == 0)
                            {
                                if(tokens.size() > 2)
                                {
                                    add_attr_variable(*el, tokens[2], cfg);
                                }
                                else
                                {
                                    add_attr_content(*el, cfg);
                                }
                            }
                            else if(str_cmp(tokens[1], keyword_content) == 0)
                            {
                                add_element_content(*el, cfg);
                            }
                        }
                        else
                        {
                            add_whole_element(*el, cfg);
                        }
                    }
                }
            }
        }
    }




    //------------------------------------------------------------------
    const char_type* content_storage::process_variable(const element& e,
                                                       const char_type* str,
                                                       unsigned len,
                                                       const config& cfg)
    {
        if(len < 4)
        {
            e.throw_exception(str, "Invalid variable syntax");
        }
        str += 2;
        len -= 2;
        const char_type* name = str;
        while(len)
        {
            if(is_close_parenthesis(*str))
            {
                add_variable(name, unsigned(str - name), e, cfg);
                ++str;
                return str;
            }
            --len;
            ++str;
        }
        e.throw_exception(str, "Invalid variable syntax (parentheses ain't balanced)");
        return str;
    }




    //------------------------------------------------------------------
    void content_storage::add_str_with_variables(const element& e,
                                                 const char_type* s,
                                                 unsigned len,
                                                 const config& cfg)
    {
        ++m_recursion_level;
        if(m_recursion_level > 32)
        {
            e.throw_exception(e.name(), "The level of recursion exceeded 32");
        }
        while(len)
        {
            if(*s != ignore_char && *s != reserved_char)
            {
                if(is_dollar(*s) && len > 1 && is_open_parenthesis(s[1]))
                {
                    const char_type* end = process_variable(e, s, len, cfg);
                    len -= unsigned(end - s);
                    s = end;
                }
                else
                {
                    add(*s);
                    ++s;
                    --len;
                }
            }
            else
            {
                ++s;
                --len;
            }
        }
        --m_recursion_level;
    }




    //------------------------------------------------------------------
    unsigned content_storage::length() const
    {
        unsigned s = m_buffer.size();
        if(s && m_buffer[s - 1] == 0) --s;
        return s;
    }


    //------------------------------------------------------------------
    char_type* content_storage::text()
    {
        if(m_buffer.size() == 0) m_buffer.push_back(0);
        if(m_buffer[m_buffer.size() - 1] != 0) m_buffer.push_back(0);
        return &(m_buffer[0]);
    }


    //------------------------------------------------------------------
    const element& content_storage::elements()
    {
        if(m_element == 0)
        {
            m_element = new element(text(), m_solid_elements);
            m_element->parse();
        }
        return *m_element;
    }



    //------------------------------------------------------------------
    void content_storage::replace_text_keysym()
    {
        char_type* str = text();
        while(*str)
        {
            *str++ = replace_keysym(*str);
        }
    }


    //------------------------------------------------------------------
    void content_storage::restore_text_keysym()
    {
        char_type* str = text();
        while(*str)
        {
            char_type c = *str;
            switch(c)
            {
                case element_replacement_char:
                case backslash_restore:        c = backslash;     break;
                case dollar_restore:           c = dollar;        break;
                case open_brace_restore:       c = open_brace;    break;
                case close_brace_restore:      c = close_brace;   break;
                case open_bracket_restore:     c = open_bracket;  break;
                case close_bracket_restore:    c = close_bracket; break;
            }
            *str++ = c;
        }
        delete m_element;
        m_element = 0;
    }






    //==================================================================
    //
    // element_serializer
    //
    //==================================================================


    //------------------------------------------------------------------
    element_serializer::element_serializer(content_storage& storage,
                                           const element& e,
                                           bool root_element_decoration,
                                           bool empty_elements_decoration,
                                           const keyword* add_attr_name,
                                           const char_type* add_attr_value) :
        m_storage(storage),
        m_level(root_element_decoration ? 1 : 0),
        m_empty_elements_decoration(empty_elements_decoration),
        m_add_attr_name(add_attr_name),
        m_add_attr_value(add_attr_value)
    {
        e.process(*this);
    }


    //------------------------------------------------------------------
    void element_serializer::start_element(const element& e)
    {
        if(m_level && e.name_len())
        {
            m_storage.add(backslash);
            m_storage.add(e.name(), e.name_len());
            if(e.attr_len())
            {
                m_storage.add(open_bracket);
                m_storage.add(e.attr(), e.attr_len());
                if(m_add_attr_name)
                {
                    if(e.attr()[e.attr_len() - 1] != semicolon)
                    {
                        m_storage.add(semicolon);
                    }
                    m_storage.add(space);
                    m_storage.add(m_add_attr_name->name, m_add_attr_name->len);
                    m_storage.add(equal);
                    m_storage.add(m_add_attr_value);
                    m_storage.add(semicolon);
                }
                m_storage.add(close_bracket);
            }
            else
            {
                if(m_add_attr_name)
                {
                    m_storage.add(open_bracket);
                    m_storage.add(m_add_attr_name->name, m_add_attr_name->len);
                    m_storage.add(equal);
                    m_storage.add(m_add_attr_value);
                    m_storage.add(semicolon);
                    m_storage.add(close_bracket);
                }
            }

            if(m_empty_elements_decoration || e.content_len())
            {
                m_storage.add(open_brace);
            }
        }
        if(e.name_len()) ++m_level;
    }


    //------------------------------------------------------------------
    void element_serializer::end_element(const element& e)
    {
        if(e.name_len()) --m_level;
        if(m_level && e.name_len())
        {
            if(m_empty_elements_decoration || e.content_len())
            {
                m_storage.add(close_brace);
            }
        }
    }


    //------------------------------------------------------------------
    void element_serializer::content(const element&, const char_type* c, unsigned len)
    {
        m_storage.add(c, len);
    }








    //==================================================================
    //
    // ifile
    //
    //==================================================================


    //------------------------------------------------------------------
    ifile::ifile(const char_type* name,
                 const string_set* solid_elements,
                 bool  trim_lines,
                 bool  remove_eof) :
        m_content(solid_elements),
        m_trim_lines(trim_lines),
        m_remove_eof(remove_eof)
    {
        std::ifstream s(to_ascii(name).c_str());
        if(!s.is_open())
        {
            throw file_not_found(name);
        }

        const char_type* pn = name;
        const char_type* pp = name;
        while(*pn)
        {
            if(*pn == slash) pp = pn + 1;
            ++pn;
        }
        string_type fname_prefix(name, unsigned(pp - name));

        string_type buf;
        buf.reserve(256);
        unsigned line_count = 0;
        src_line line;
        unsigned comment_level = 0;
        unsigned li;
        while(read_line(s, buf))
        {
            ++line_count;
            line.len = buf.length();
            line.ptr = buf.c_str();
            line.num = line_count;

            li = 0;
            if(comment_level == 0)
            {
                while(li < buf.length() && buf[li] != lf)
                {
                    if(str_cmp(buf.c_str() + li, keyword_line_comment.len, keyword_line_comment) == 0)
                    {
                        while(li < buf.length() && buf[li] != lf) buf[li++] = reserved_char;
                        break;
                    }
                    if(str_cmp(buf.c_str() + li, keyword_open_comment.len, keyword_open_comment) == 0)
                    {
                        unsigned l = keyword_open_comment.len;
                        do buf[li++] = reserved_char; while(--l);
                        ++comment_level;
                        break;
                    }
                    ++li;
                }
            }


            if(comment_level)
            {
                while(li < buf.length() && buf[li] != lf)
                {
                    if(str_cmp(buf.c_str() + li, keyword_open_comment.len, keyword_open_comment) == 0)
                    {
                        ++comment_level;
                    }
                    if(str_cmp(buf.c_str() + li, keyword_close_comment.len, keyword_close_comment) == 0)
                    {
                        --comment_level;
                        if(comment_level == 0)
                        {
                            for(unsigned i = 0; i < keyword_close_comment.len; i++)
                            {
                                buf[li++] = reserved_char;
                            }
                            break;
                        }
                    }
                    buf[li++] = reserved_char;
                }
            }


            if(str_cmp(buf.c_str(), keyword_include.len, keyword_include) == 0)
            {
                const char_type* p = buf.c_str() + keyword_include.len;
                while(*p && (is_space(*p) ||
                             *p == open_bracket ||
                             *p == open_brace))
                {
                    ++p;
                }
                if(*p == 0)
                {
                    throw parsing_exception(line, "Invalid \\include syntax");
                }
                string_type fname;
                fname.reserve(256);
                while(*p && (!is_space(*p) &&
                             *p != close_bracket &&
                             *p != close_brace))
                {
                    fname += *p++;
                }
                if(fname.length() == 0)
                {
                    throw parsing_exception(line, "Invalid \\include syntax");
                }
                try
                {
                    if(fname[0] != slash && fname_prefix.length())
                    {
                        fname.insert(0, fname_prefix);
                    }
                    reduce_dot_dot(fname);
                    ifile f(fname.c_str(), solid_elements);
                    m_content.add(f.text());
                    continue;
                }
                catch(file_not_found&)
                {
                    throw parsing_exception(line, "Include file not found: '%s'", fname.c_str());
                }
            }
            m_content.add(buf);
        }
    }




    //------------------------------------------------------------------
    bool ifile::read_line(std::ifstream& s, string_type& buf)
    {
        buf.erase();
        while(s.good())
        {
            int ch = s.get();
            if(s.eof()) break;
            if(is_cr(ch)) continue;
            buf += (char_type)ch;
            if(is_lf(ch)) break;
        }

        if(buf.length() == 0) return false;

        if(m_trim_lines)
        {
            trim_lines(buf);
        }
        if(m_remove_eof && buf.length())
        {
            buf.erase(buf.length() - 1);
        }
        return true;
    }





    //------------------------------------------------------------------
    void write_content_storage(const char_type* name, content_storage& content)
    {
        FILE* fd = fopen(to_ascii(name).c_str(), "wb");
        if(fd == 0)
        {
            throw parsing_exception("Cannot write file: '%s'", name);
        }
        fwrite(content.text(), sizeof(char_type), content.length(), fd);
        fclose(fd);
    }





    //------------------------------------------------------------------
    void calc_relative_name_to(const char_type* from,
                               const char_type* to,
                               string_type& rel_name)
    {
        rel_name.erase();

        // Find the common prefix
        const char_type* pdiff_from = from;
        const char_type* pdiff_to   = to;
        for(;;)
        {
            if(*from != *to) break;
            if(*from == 0)
            {
                pdiff_from = from;
                pdiff_to   = to;
                break;
            }
            if(*from == slash)
            {
                pdiff_from = from + 1;
                pdiff_to   = to   + 1;
            }
            ++from;
            ++to;
        }

        // Add the dot-dot-slashes and the rest
        while(*pdiff_from)
        {
            if(*pdiff_from == slash)
            {
                rel_name.append(keyword_dot_dot_slash.name,
                                keyword_dot_dot_slash.len);
            }
            ++pdiff_from;
        }
        rel_name.append(pdiff_to);
    }




    //------------------------------------------------------------------
    void reduce_dot_dot(string_type& fname)
    {
        bool reduced;
        do
        {
            const char_type* prev_dir = 0;
            const char_type* p = fname.c_str();
            const char_type* start = fname.c_str();
            reduced = false;
            while(*p)
            {
                if(p == start && *p != dot && *p != slash) prev_dir = p;
                if(*p == slash)
                {
                    if(p[1] && p[1] != dot) prev_dir = p + 1;
                    if(prev_dir && p > prev_dir && p[1] == dot && p[2] == dot)
                    {
                        fname.erase(unsigned(prev_dir - start), unsigned(p - prev_dir) + 4);
                        reduced = true;
                        break;
                    }
                }
                ++p;
            }
        }
        while(reduced);
    }



    //------------------------------------------------------------------
    void extract_file_name(const char_type* path, string_type& fname)
    {
        const char_type* start = path;
        while(*path)
        {
            if(*path == slash) start = path + 1;
            ++path;
        }
        fname = start;
    }



    //------------------------------------------------------------------
    string_type path_prefix(const char_type* fname)
    {
        const char_type* pn = fname;
        const char_type* pp = fname;
        while(*pn)
        {
            if(*pn == slash) pp = pn;
            ++pn;
        }
        return string_type(fname, unsigned(pp - fname));
    }



    //------------------------------------------------------------------
    void trim_ending_slashes(string_type& s)
    {
        while(s.length() && s[s.length() - 1] == slash)
        {
            s.erase(s.length() - 1);
        }
    }




    //------------------------------------------------------------------
    // Definition of wildcard syntax:
    //
    // - * matches any sequence of characters, including zero.
    // - ? matches exactly one character which can be anything.
    // - [abc] matches exactly one character which is a, b or c.
    // - [a-f] matches anything from a through f.
    // - [^a-f] matches anything _except_ a through f.
    // - [-_] matches - or _; [^-_] matches anything else. (The - is
    //   non-special if it occurs immediately after the opening
    //   bracket or ^.)
    // - [a^] matches an a or a ^. (The ^ is non-special if it does
    //   _not_ occur immediately after the opening bracket.)
    // - \*, \?, \[, \], \\ match the single characters *, ?, [, ], \.
    // - All other characters are non-special and match themselves.
    //------------------------------------------------------------------
    //
    // This is the routine that tests a target string to see if an
    // initial substring of it matches a fragment. If successful, it
    // returns 1, and advances both `fragment' and `target' past the
    // fragment and matching substring respectively. If unsuccessful it
    // returns zero. If the wildcard fragment suffers a syntax error,
    // it returns <0 and the precise value indexes into wc_error.
    //------------------------------------------------------------------
    static int match_fragment(const char_type** fragment,
                              const char_type** target)
    {
        const char_type* f;
        const char_type* t;

        f = *fragment;
        t = *target;

        // The fragment terminates at either the end of the string, or
        // the first (unescaped) *.
        //------------
        while (*f && *f != asterisk && *t)
        {
            // Extract one character from t, and one character's worth
            // of pattern from f, and step along both. Return 0 if they
            // fail to match.
            //-------------------
            if(*f == backslash)
            {
                // Backslash, which means f[1] is to be treated as a
                // literal character no matter what it is. It may not
                // be the end of the string.
                //-------------------
                if(f[1] == 0)  return -1;  // error
                if(f[1] != *t) return 0;   // failed to match
                f += 2;
            }
            else
            if(*f == question_mark)
            {
                // Question mark matches anything.
                //-----------
                ++f;
            }
            else
            if(*f == open_bracket)
            {
                int invert = 0;
                int matched = 0;

                // Open bracket introduces a character class.
                //------------
                ++f;
                if(*f == caret)
                {
                    invert = 1;
                    ++f;
                }

                while(*f != close_bracket)
                {
                    if(*f == backslash) ++f;  // backslashes still work
                    if(*f == 0) return -1;    // error again

                    if(f[1] == minus)
                    {
                        unsigned lower, upper, ourchr;
                        lower = *(unsigned char*)f++;
                        ++f;        // eat the minus
                        if(*f == close_bracket)
                        {
                            return -1;  // different error!
                        }
                        if(*f == backslash)
                        {
                            ++f;        // backslashes _still_ work
                        }

                        if(*f == 0)
                        {
                            return -1;  // error again
                        }
                        upper = *(unsigned char*)f++;
                        ourchr = *(unsigned char*)t;

                        if(lower > upper)
                        {
                            unsigned t = lower;
                            lower = upper;
                            upper = t;
                        }
                        if(ourchr >= lower && ourchr <= upper)
                        {
                            matched = 1;
                        }
                    }
                    else
                    {
                        matched |= (*t == *f++);
                    }
                }
                if(invert == matched)
                {
                    return 0;  // failed to match character class
                }
                ++f;           // eat the ]
            }
            else
            {
                // Non-special character matches itself.
                //-----------------
                if(*f != *t)
                {
                    return 0;
                }
                ++f;
            }
            // Now we've done that, increment t past the character we
            // matched.
            //------------------
            ++t;
        }
        if(*f == 0 || *f == asterisk)
        {
            // We have reached the end of f without finding a mismatch;
            // so we're done. Update the caller pointers and return 1.
            //------------------
            *fragment = f;
            *target = t;
            return 1;
        }
        // Otherwise, we must have reached the end of t before we
        // reached the end of f; so we've failed. Return 0.
        //-----------------
        return 0;
    }




    //------------------------------------------------------------------
    bool file_name_match(const char_type* wildcard, const char_type* target)
    {
        int ret;

        // Every time we see a '*' _followed_ by a fragment, we just
        // search along the string for a location at which the fragment
        // matches. The only special case is when we see a fragment
        // right at the start, in which case we just call the matching
        // routine once and give up if it fails.
        //---------------
        if(*wildcard != asterisk)
        {
            if(match_fragment(&wildcard, &target) <= 0)
            {
                return false;   // pass back failure or error alike
            }
        }

        while(*wildcard)
        {
            while(*wildcard == asterisk) ++wildcard;

            // It's possible we've just hit the end of the wildcard
            // after seeing a *, in which case there's no need to
            // bother searching any more because we've won.
            // ------------
            if(*wildcard == 0)
            {
                return true;
            }

            // Now `wildcard' points at the next fragment. So we
            // attempt to match it against `target', and if that fails
            // we increment `target' and try again, and so on. When we
            // find we're about to try matching against the empty
            // string, we give up and return 0.
            //-------------
            ret = 0;
            while(*target)
            {
                const char_type* save_w = wildcard;
                const char_type* save_t = target;

                ret = match_fragment(&wildcard, &target);

                if(ret < 0) return false; // syntax error

                if(ret > 0 && *wildcard == 0 && *target)
                {
                    // Final special case - literally.
                    //
                    // This situation arises when we are matching a
                    // _terminal_ fragment of the wildcard (that is,
                    // there is nothing after it, e.g. "*a"), and it
                    // has matched _too early_. For example, matching
                    // "*a" against "parka" will match the "a" fragment
                    // against the _first_ a, and then (if it weren't
                    // for this special case) matching would fail
                    // because we're at the end of the wildcard but not
                    // at the end of the target string.
                    //
                    // In this case what we must do is measure the
                    // length of the fragment in the target (which is
                    // why we saved `target'), jump straight to that
                    // distance from the end of the string using
                    // strlen, and match the same fragment again there
                    // (which is why we saved `wildcard'). Then we
                    // return whatever that operation returns.
                    //------------
                    target = save_t + str_len(save_t) - (target - save_t);
                    wildcard = save_w;
                    return match_fragment(&wildcard, &target) > 0;
                }
                if(ret > 0) break;
                ++target;
            }
            if(ret > 0) continue;

            return false;
        }

        // If we reach here, it must be because we successfully matched
        // a fragment and then found ourselves right at the end of the
        // wildcard. Hence, we return 1 if and only if we are also
        // right at the end of the target.
        //------------
        return *target == 0;
    }






    //------------------------------------------------------------------
    bool read_dir(const char_type* dir_name, strset_type& names)
    {
        std::string dir;
        string_type fname;

        dir.reserve(256);
        fname.reserve(256);

        const char_type* p = dir_name;
        while(*p) dir += (char)*p++;
        if(dir.length() && dir[dir.length() - 1] != slash)  dir += slash;

#if defined(_WIN32)    // Use _findfirst/_findnext

        struct _finddata_t fi;
        long fh;

        dir += "*.*";
        fh = _findfirst(dir.c_str(), &fi);
        if(fh == -1L) return false;
        do
        {
            if((fi.attrib & _A_SUBDIR) == 0)
            {
                fname.erase();
                const char* p = fi.name;
                while(*p) fname += (char_type)*p++;
                names.insert(fname);
            }
        }
        while(_findnext(fh, &fi) == 0);
        _findclose(fh);

#else                  // Assume POSIX opendir/readdir/closedir

        std::string full_fname;
        full_fname.reserve(256);

        DIR* dirp;
        struct dirent* direntp;

        dirp = opendir(dir.c_str());
        if(dirp == 0) return false;
        for(;;)
        {
            direntp = readdir(dirp);
            if(direntp == 0) break;
            struct stat st;
            full_fname = dir;
            full_fname += direntp->d_name;
            if(stat(full_fname.c_str(), &st) == 0)
            {
                if(S_ISREG(st.st_mode))
                {
                    fname.erase();
                    const char* p = direntp->d_name;
                    while(*p) fname += (char_type)*p++;
                    names.insert(fname);
                }
            }
        }
        closedir(dirp);
#endif
        return true;
    }






    //------------------------------------------------------------------
    stat_e name_stat(const char_type* name, time_t* mod_time)
    {
        std::string nm;
        nm.reserve(256);
        const char_type* p = name;
        while(*p) nm += (char)*p++;
        if(nm.length() && nm[nm.length() - 1] == slash)
        {
            nm.erase(nm.length() - 1);
        }
#if defined(_WIN32)
        struct _stat st;
        if(_stat(nm.c_str(), &st) == 0)
        {
            if(mod_time) *mod_time = st.st_mtime;
            if(st.st_mode & _S_IFREG) return file_exists;
            if(st.st_mode & _S_IFDIR) return dir_exists;
        }
#else                  // Assume POSIX
        struct stat st;
        if(stat(nm.c_str(), &st) == 0)
        {
            if(mod_time) *mod_time = st.st_mtime;
            if(S_ISREG(st.st_mode)) return file_exists;
            if(S_ISDIR(st.st_mode)) return dir_exists;
        }
#endif
        return not_found;
    }





    //------------------------------------------------------------------
    bool make_dir(const char_type* dir_name)
    {
        std::string dir;
        dir.reserve(256);
        const char_type* p = dir_name;
        while(*p) dir += (char)*p++;
        if(dir.length() && dir[dir.length() - 1] == slash)
        {
            dir.erase(dir.length() - 1);
        }
#if defined(_WIN32)    // Use _findfirst/_findnext
        return _mkdir(dir.c_str()) == 0;
#else                  // Assume POSIX opendir/readdir/closedir
        return mkdir(dir.c_str(), 0700) == 0;
#endif
    }




    //------------------------------------------------------------------
    bool copy_file(const char_type* from, const char_type* to)
    {
        std::string sfrom;
        std::string sto;
        sfrom.reserve(256);
        sto.reserve(256);
        const char_type* p;

        p = from;
        while(*p) sfrom += (char)*p++;

        p = to;
        while(*p) sto += (char)*p++;

        FILE* ffrom = fopen(sfrom.c_str(), "rb");
        if(ffrom == 0) return false;

        FILE* fto = fopen(sto.c_str(), "wb");
        if(fto == 0)
        {
            fclose(ffrom);
            return false;
        }

        char* buf = new char[65536];

        unsigned len;
        while((len = fread(buf, 1, 65536, ffrom)) > 0)
        {
            fwrite(buf, 1, len, fto);
        }

        delete [] buf;
        fclose(fto);
        fclose(ffrom);
        return true;
    }



    //==================================================================
    //
    // log_file
    //
    //==================================================================


    //------------------------------------------------------------------
    void log_file::write(const char_type* message)
    {
        //puts(message);
        //puts("");
        ltLogMessage(message);
    }

    //------------------------------------------------------------------
    void log_file::write(const char_type* message, const char* arg1)
    {
        //printf(message, to_ascii(arg1).c_str());
        //puts("");
        ltLogMessage(message, arg1);
    }

    //------------------------------------------------------------------
    void log_file::write(const char_type* message, const char* arg1,
                                              const char* arg2)
    {
        //printf(message, to_ascii(arg1).c_str(), to_ascii(arg2).c_str());
        //puts("");
        ltLogMessage(message, arg1, arg2);
    }




    //==================================================================
    //
    // file_enumerator
    //
    //==================================================================


    //------------------------------------------------------------------
    file_enumerator::file_enumerator(const string_type& path,
                                     const string_type& names,
                                     const string_type& exclude)
    {
        unsigned i, j;
        string_list incl(names);
        string_list excl(exclude);
        if(incl.char_exists(asterisk) || incl.char_exists(question_mark))
        {
            strset_type dir;
            read_dir(path.c_str(), dir);
            for(i = 0; i < incl.size(); ++i)
            {
                strset_type::const_iterator it = dir.begin();
                for(; it != dir.end(); ++it)
                {
                    if(file_name_match(incl[i].c_str(), it->c_str()))
                    {
                        bool excl_flag = false;
                        for(j = 0; j < excl.size(); ++j)
                        {
                            if(file_name_match(excl[j].c_str(), it->c_str()))
                            {
                                excl_flag = true;
                                break;
                            }
                        }
                        if(!excl_flag)
                        {
                            m_names.insert(*it);
                        }
                    }
                }
            }
        }
        else
        {
            for(i = 0; i < incl.size(); ++i)
            {
                m_names.insert(incl[i]);
            }
        }
    }




    //==================================================================
    //
    // my functions, to deal with wxString
    //
    //==================================================================

#ifdef ltUSE_WXSTRING
    wxString ToString(string_type& s, int len)
    {
        return wxString(s.c_str(), len);
    }

    wxString ToString(const char_type* s, int len)
    {
        return wxString(s, len);
    }
#else
    wxString ToString(string_type& s, int len)
    {
        return ToString(s.c_str(), len);
    }

    wxString ToString(const char_type* s, int len)
    {
        string_type data;
        data.reserve(len);
        const char_type* pData = s;
        for(int i = 0; i < len; ++i, ++pData)
            data.push_back(*pData);

        return wxString::From8BitData( data.c_str() );
    }
#endif



}

