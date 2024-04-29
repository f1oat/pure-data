#ifndef PARSER_STRINGS_H
#define PARSER_STRINGS_H

#include "ceammc_atomlist.h"
#include "ceammc_atomlist_view.h"
#include "ceammc_string_types.h"

#include <string>

namespace ceammc {
namespace string {

    /**
     * Checks if given string is matched to quoted string end
     * This is usefull if we have list atom Pd atoms and want to do a quick check
     * if they are data string representation
     */
    bool quoted_string_end(const char* str);
    bool quoted_string_end(t_symbol* s);
    bool quoted_string_end(const std::string& str);
    bool quoted_string_end(const Atom& a);

    bool is_quoted_string(const char* str);
    bool is_quoted_string(t_symbol* s);
    bool is_quoted_string(const std::string& str);
    bool is_quoted_string(const AtomListView& lv);

    bool string_need_quotes(const char* str);

    /**
     * Escapes special symbol in string and adds outer double quotes
     * @param str - source string (with spaces, commas, semicolons etc.)
     * @param out - string buffer to append data
     * @return number of escaped charactes or -1 on error
     */
    int escape_and_quote(const char* str, StaticString& out);
    int escape_and_quote(const char* str, SmallString& out);
    int escape_and_quote(const char* str, MediumString& out);

    template <typename T>
    bool escape_and_quote(const Atom& a, T& out)
    {
        if (a.isSymbol()) {
            return escape_and_quote(a.asT<t_symbol*>()->s_name, out) > 0 || a.asT<t_symbol*>()->s_name[0] == '\0';
        } else if (a.isSemicolon()) {
            return escape_and_quote(";", out) > 0;
        } else if (a.isComma()) {
            return escape_and_quote(",", out) > 0;
        } else
            return false;
    }

    /**
     * Escape and quote symbol atoms, commas and semicolons
     * @param a - atom to escape
     */
    void escape_and_quote(Atom& a);

    /**
     * Escape and quote symbol atoms, commas and semicolons
     * @param a - atom to escape
     * @return new escaped atom
     */
    Atom escape_and_quote(const Atom& a);

    /**
     * Escape and quote symbol atoms, commas and semicolons in given list
     * @param lst - list to escape
     */
    void escape_and_quote(AtomList& lst);

    /**
     * Escape and quote symbol atoms, commas and semicolons in given list
     * @param lst - list to escape
     * @return new escaped list
     */
    AtomList escape_and_quote(const AtomListView& lv);

    /**
     * Appends to buffer unquoted string (in "..." or S"..." form), substitute escaped chars
     * and expand environment %VARS%.
     * If env %VAR% was not found, output as row string: '%VAR%'.
     * @note if input string was not quoted, output is not specified and should be used
     *
     * '``' => '`'
     * '`"' => '"'
     * '`(' => '{'
     * '`)' => '}'
     * @param str - input string
     * @param out - output static buffer
     * @return -1 on error, 0 - if no unquoting was done, 1 - string was unquoted
     */
    int unquote_and_unescape(const char* str, StaticString& out);
    int unquote_and_unescape(const char* str, SmallString& out);
    int unquote_and_unescape(const char* str, MediumString& out);

    template <typename T>
    bool unquote_and_unescape(const Atom& a, T& out)
    {
        if (!a.isSymbol())
            return false;
        else
            return unquote_and_unescape(a.asT<t_symbol*>()->s_name, out);
    }

    /**
     * Parse quoted ceammc string: unquote and unescape if needed, otherwise return unchanged
     * @param s - symbol to string
     * @return - parsed or untouched symbol
     */
    t_symbol* parse_ceammc_quoted_string(t_symbol* s);

    /**
     * Parse quoted ceammc string in atom
     * if atom is not a ceammc string return original atom
     * @param a - atom
     * @return - parsed or untouched atom
     */
    Atom parse_ceammc_quoted_string(const Atom& a);

    AtomList parse_ceammc_quoted_string(const AtomListView& lv);

    /**
     * Checks is atomlist contains atoms with starting or ending double quote
     * @return true if contains start/end/both quoted atoms
     * @note this is a quick check, to decide should we do the quote-string parsing or not
     */
    bool maybe_ceammc_quoted_string(const AtomListView& lv);

    /**
     * extract instance form full mdns service name
     * @param service - service fullname
     * @return domain name or empty string on error
     * @example test._http_._tcp -> 'test'
     * @example test._http_._tcp.local -> 'test'
     */
    std::string mdns_instance_name_from_service(const char* service) noexcept;
}
}

#endif // PARSER_STRINGS_H
