%%{
    machine string_common;

    normal_symbol = ('`' ascii) | [^`"];
    normal_symbol_nz = normal_symbol - 0;

    string_ends = normal_symbol* '"';

    str_escape    = '`';
    str_squote    = "'";
    str_dquote    = '"';
    str_space     = ' ';
    str_comma     = ',';
    str_semicolon = ';';
    str_backslash = '\\';

    str_envvar    = '%' [A-Z_0-9]{1,16} '%';

    esc_escape    = str_escape str_escape;
    esc_space     = str_escape str_space;
    esc_dquote    = str_escape str_dquote;
    esc_comma     = str_escape '.';
    esc_semicolon = str_escape ':';
    esc_slash     = str_escape '/';
    esc_at        = str_escape '@';

    all_escapes =
        str_escape |
        str_dquote |
        str_space |
        str_comma |
        str_semicolon |
        str_backslash;
}%%
