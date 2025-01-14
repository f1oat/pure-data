#include "editor_data.h"
#include "ceammc_string.h"
#include "datatype_dict.h"
#include "datatype_mlist.h"

namespace ceammc {

namespace {
    void editorAppend(EditorLineList& res, const DataTypeMList& ml, int indentLevel);
    void editorAppend(EditorLineList& res, const DataTypeDict& dict, int indentLevel);

    bool isSimpleList(const AtomListView& lv)
    {
        return lv.allOf([](const Atom& a) { return !a.isData(); });
    }

    void appendIndent(EditorStringPtr& str, int level)
    {
        str->append(std::string(level * 4, ' '));
    }

    void appendIndent(EditorLineList& res, const char* txt, int level)
    {
        auto str = EditorStringPool::pool().allocate();
        appendIndent(str, level);
        str->append(txt);
        res.push_back(str);
    }

    void appendIndent(EditorLineList& res, char ch, int level)
    {
        auto str = EditorStringPool::pool().allocate();
        appendIndent(str, level);
        str->append(ch);
        res.push_back(str);
    }

    template <typename L>
    void appendMultiLine(EditorLineList& res, const L& lst, int indentLevel)
    {
        // each value on separate line
        for (auto& a : lst) {
            if (a.template isA<DataTypeMList>()) {
                editorAppend(res, *a.template asD<DataTypeMList>(), indentLevel + 1);
            } else if (a.template isA<DataTypeDict>()) {
                editorAppend(res, *a.template asD<DataTypeDict>(), indentLevel + 1);
            } else {
                auto str = EditorStringPool::pool().allocate();
                appendIndent(str, indentLevel + 1);

                string::SmallString buf;
                if (string::parsed_atom_to_string(a, buf))
                    str->append(buf.data(), buf.size());

                res.push_back(str);
            }
        }
    }

    void editorAppend(EditorLineList& res, const DataTypeMList& ml, int indentLevel)
    {
        const bool simple_content = isSimpleList(ml.data().view());
        const bool single_line = simple_content && ml.size() < 7;

        if (single_line) {
            auto str = EditorStringPool::pool().allocate();
            appendIndent(str, indentLevel);

            if (ml.empty())
                str->append("( )");
            else
                str->append(ml.toString());

            res.push_back(str);
        } else {
            appendIndent(res, '(', indentLevel);
            appendMultiLine(res, ml, indentLevel);
            appendIndent(res, ')', indentLevel);
        }
    }

    void editorAppend(EditorLineList& res, const DataTypeDict& dict, int indentLevel)
    {
        const bool single_line = dict.size() < 2;

        if (single_line) {
            auto str = EditorStringPool::pool().allocate();

            appendIndent(str, indentLevel);
            if (dict.size() == 0)
                str->append("[ ]");
            else
                str->append(dict.toString());

            res.push_back(str);
        } else {

            appendIndent(res, '[', indentLevel);

            for (auto& kv : dict) {
                auto str = EditorStringPool::pool().allocate();
                appendIndent(str, indentLevel + 1);

                str->append(kv.first->s_name);
                str->append(':');
                str->append(' ');

                auto& l = kv.second;

                // simple list is one-liner
                if (isSimpleList(l.view())) {
                    for (auto& a : l) {
                        str->appendQuoted(a);
                        str->append(' ');
                    }

                    if (!l.empty())
                        str->trim();

                    res.push_back(str);
                } else {
                    res.push_back(str);
                    appendMultiLine(res, l, indentLevel + 1);
                }
            }

            appendIndent(res, ']', indentLevel);
        }
    }
}

void editorAppend(EditorLineList& res, const AbstractData* d, int indentLevel)
{
    if (!d)
        return;

    if (d->type() == DataTypeMList::staticType())
        return editorAppend(res, static_cast<const DataTypeMList&>(*d), indentLevel);
    else if (d->type() == DataTypeDict::staticType())
        return editorAppend(res, static_cast<const DataTypeDict&>(*d), indentLevel);
    else {
        auto str = EditorStringPool::pool().allocate();
        appendIndent(str, indentLevel);

        if (d->canInitWithList()) {
            str->append(d->toListString());
            res.push_back(str);
        } else if (d->canInitWithDict()) {
            auto dict_str = d->toDictString();
            for (size_t i = 0; i < dict_str.length(); i++) {
                const auto c = dict_str[i];
                if (c == '\n') {
                    res.push_back(str);
                    str = EditorStringPool::pool().allocate();
                    appendIndent(str, indentLevel);
                } else {
                    str->append(c);
                }
            }

            if (str->length() > 0)
                res.push_back(str);
        }
    }
}

void editorAppend(EditorLineList& res, const AtomListView& lv, int indentLevel)
{
    for (auto& a : lv) {
        auto str = EditorStringPool::pool().allocate();
        appendIndent(str, indentLevel);

        if (a.isData())
            editorAppend(res, a.asData(), indentLevel + 1);
        else {
            str->appendQuoted(a);
            res.push_back(str);
        }
    }
}

}
