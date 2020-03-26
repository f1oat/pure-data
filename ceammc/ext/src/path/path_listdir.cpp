#include "path_listdir.h"
#include "ceammc_factory.h"
#include "ceammc_format.h"
#include "ceammc_platform.h"
#include "config.h"
#include "datatype_string.h"

#include <cerrno>
#include <cstring>

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#else
#error <dirent.h> is missing...
#endif

PathListDir::PathListDir(const PdArgs& a)
    : BaseObject(a)
    , match_(&s_)
{
    createOutlet();

    addProperty(new PointerProperty<t_symbol*>("@match", &match_, PropValueAccess::READWRITE));

    path_ = to_string(positionalArguments());
}

void PathListDir::onBang()
{
    readDirList();
    listTo(0, ls_.toList());
}

void PathListDir::onSymbol(t_symbol* path)
{
    path_ = path->s_name;
    onBang();
}

void PathListDir::onDataT(const DataTPtr<DataTypeString>& dptr)
{
    path_ = dptr->str();
    onBang();
}

void PathListDir::readDirList()
{
    if (path_.empty())
        return;

    ls_.clear();

    std::string path = platform::expand_tilde_path(path_);

    if (!sys_isabsolutepath(path.c_str())) {
        if (canvas()) {
            t_symbol* canvas_dir = canvas_getdir(canvas());
            if (canvas_dir)
                path = std::string(canvas_dir->s_name) + "/" + path;
        }
    }

    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        OBJ_ERR << "can't read directory: '" << path << "'. Error: " << strerror(errno);
        return;
    }

    struct dirent* p_dirent;
    while ((p_dirent = readdir(dir)) != NULL) {
        // skip hidden files on UNIX
        if (p_dirent->d_name[0] == '.')
            continue;

        if (match_->s_name[0] != 0) {
            if (platform::fnmatch(match_->s_name, p_dirent->d_name)) {
                DataPtr d(new DataTypeString(p_dirent->d_name));
                ls_.append(d.asAtom());
            }
        } else {
            DataPtr d(new DataTypeString(p_dirent->d_name));
            ls_.append(d.asAtom());
        }
    }

    closedir(dir);
}

extern "C" void setup_path0x2elsdir()
{
    ObjectFactory<PathListDir> obj("path.lsdir");
    obj.addAlias("path.ls");
    obj.processData<DataTypeString>();
}
