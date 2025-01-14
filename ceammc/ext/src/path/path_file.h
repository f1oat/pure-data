#ifndef CEAMMC_PATH_FILE_H
#define CEAMMC_PATH_FILE_H

#include "ceammc_object.h"
using namespace ceammc;

#include <fstream>

class PathFile : public BaseObject {
    SymbolProperty* fname_;
    std::fstream fs_;

public:
    PathFile(const PdArgs& args);

    void m_open(t_symbol* s, const AtomListView& lv);
    void m_close(t_symbol* s, const AtomListView& lv);
    void m_remove(t_symbol* s, const AtomListView& lv);
    void m_rename(t_symbol* s, const AtomListView& lv);

    void m_write_string(t_symbol* s, const AtomListView& lv);
    void m_write_line(t_symbol* s, const AtomListView& lv);
    void m_write_bytes(t_symbol* s, const AtomListView& lv);

    void m_read_line(t_symbol* s, const AtomListView& lv);
    void m_read_bytes(t_symbol* s, const AtomListView& lv);

    void m_seek_read(t_symbol* s, const AtomListView& lv);
    void m_seek_write(t_symbol* s, const AtomListView& lv);

private:
    bool updateFullPath(const AtomListView& args);
    bool checkExists() const;
    bool checkOpen(std::ios::openmode mode);
    bool checkOpenMode(const char* mode, std::ios::openmode& res) const;
    t_symbol* expandPath(t_symbol* path) const;
};

void setup_path_file();

#endif // CEAMMC_PATH_FILE_H
