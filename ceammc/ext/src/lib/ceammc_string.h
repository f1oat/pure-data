/*****************************************************************************
 * Copyright 2017 Serge Poltavsky. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#ifndef CEAMMC_STRING_H
#define CEAMMC_STRING_H

#include <cstddef>
#include <string>
#include <vector>

namespace ceammc {
namespace string {
    // utf-8 functions
    size_t utf8_strlen(const char* str) noexcept;
    std::string utf8_to_upper(const char* str);
    std::string utf8_to_lower(const char* str);
    std::string utf8_substr(const char* str, int from, size_t len);
    void utf8_split_by_char(std::vector<std::string>& vec, const char* str);

    bool starts_with(const char* str, const char* prefix);
    bool starts_with(const std::string& str, const std::string& prefix);

    bool ends_with(const char* str, const char* suffix);
    bool ends_with(const std::string& str, const std::string& suffix);

    bool contains(const char* haystack, const char* needle);
    bool contains(const std::string& haystack, const std::string& needle);

    /**
     * remove all the occurrences of the string from the input
     */
    std::string remove_all(const std::string& input, const std::string& search);

    /**
     * remove first occurrence of the string from the input
     */
    std::string remove_first(const std::string& input, const std::string& search);

    /**
     * remove last occurrence of the string from the input
     */
    std::string remove_last(const std::string& input, const std::string& search);

    /**
     * replace all the occurrences of the string from
     */
    std::string replace_all(const std::string& input, const std::string& from, const std::string& to);

    /**
     * replace first occurrence of the string from
     */
    std::string replace_first(const std::string& input, const std::string& from, const std::string& to);

    /**
     * replace last occurrence of the string from
     */
    std::string replace_last(const std::string& input, const std::string& from, const std::string& to);

    // pd-string is single quoted or double quoted string
    //   with ` as escape symbol.
    // - `" -> "
    // - `` -> `
    // - `/ -> \
    // - `. -> ,
    // - `: -> ;

    /**
     * Try to parse pd-string
     * @param str - pdstring
     * @param out - writted parsed and unescaped string
     * @return true if pd-string given, false on error
     */
    bool pd_string_parse(const std::string& str, std::string& out);
    bool pd_string_match(const std::string& str, std::string& matched);
    bool is_pd_string(const char* str);
    bool pd_string_end_quote(const char* str);
    std::string pd_string_unescape(const std::string& str);

    std::string escape_for_json(const std::string& str);
}
}

#endif // CEAMMC_STRING_H
