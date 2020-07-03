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
#include "ceammc_canvas.h"
#include "ceammc_object.h"

#include "m_pd.h"

extern "C" {
#include "g_canvas.h"
#include "m_imp.h"
#include "s_stuff.h"
}

// Note! keep in sync with PD declaration
struct _canvasenvironment {
    t_symbol* ce_dir; /* directory patch lives in */
    int ce_argc; /* number of "$" arguments */
    t_atom* ce_argv; /* array of "$" arguments */
    int ce_dollarzero; /* value of "$0" */
    t_namelist* ce_path; /* search path */
};

// temp crash fix with newer version
// because _glits have other field order and size
struct pd048_glist {
    t_object gl_obj; /* header in case we're a glist */
    t_gobj* gl_list; /* the actual data */
    struct _gstub* gl_stub; /* safe pointer handler */
    int gl_valid; /* incremented when pointers might be stale */
    struct _glist* gl_owner; /* parent glist, supercanvas, or 0 if none */
    int gl_pixwidth; /* width in pixels (on parent, if a graph) */
    int gl_pixheight;
    t_float gl_x1; /* bounding rectangle in our own coordinates */
    t_float gl_y1;
    t_float gl_x2;
    t_float gl_y2;
    int gl_screenx1; /* screen coordinates when toplevel */
    int gl_screeny1;
    int gl_screenx2;
    int gl_screeny2;
    int gl_xmargin; /* origin for GOP rectangle */
    int gl_ymargin;
    t_tick gl_xtick; /* ticks marking X values */
    int gl_nxlabels; /* number of X coordinate labels */
    t_symbol** gl_xlabel; /* ... an array to hold them */
    t_float gl_xlabely; /* ... and their Y coordinates */
    t_tick gl_ytick; /* same as above for Y ticks and labels */
    int gl_nylabels;
    t_symbol** gl_ylabel;
    t_float gl_ylabelx;
    t_editor* gl_editor; /* editor structure when visible */
    t_symbol* gl_name; /* symbol bound here */
    int gl_font; /* nominal font size in points, e.g., 10 */
    struct _glist* gl_next; /* link in list of toplevels */
    t_canvasenvironment* gl_env; /* root canvases and abstractions only */
    unsigned int gl_havewindow : 1; /* true if we own a window */
    unsigned int gl_mapped : 1; /* true if, moreover, it's "mapped" */
    unsigned int gl_dirty : 1; /* (root canvas only:) patch has changed */
    unsigned int gl_loading : 1; /* am now loading from file */
    unsigned int gl_willvis : 1; /* make me visible after loading */
    unsigned int gl_edit : 1; /* edit mode */
    unsigned int gl_isdeleting : 1; /* we're inside glist_delete -- hack! */
    unsigned int gl_goprect : 1; /* draw rectangle for graph-on-parent */
    unsigned int gl_isgraph : 1; /* show as graph on parent */
    unsigned int gl_hidetext : 1; /* hide object-name + args when doing graph on parent */
    unsigned int gl_private : 1; /* private flag used in x_scalar.c */
    unsigned int gl_isclone : 1; /* exists as part of a clone object */
    int gl_zoom; /* zoom factor (integer zoom-in only) */
    void* gl_privatedata; /* private data */
};

using namespace ceammc;

static bool is_pd048()
{
    int major, minor, bugfix;
    sys_getversion(&major, &minor, &bugfix);
    static_assert(PD_MINOR_VERSION < 51, "update for minor version");
    if (major == PD_MAJOR_VERSION && minor == PD_MINOR_VERSION)
        return false;

    return true;
}

t_canvasenvironment* canvas_get_current_env(const t_canvas* c)
{
    static bool fix = is_pd048();

    if (!c)
        return nullptr;

    if (!fix) {
        return c->gl_env;
    } else {
        auto x = reinterpret_cast<const pd048_glist*>(c);
        return x->gl_env;
    }
}

AtomList ceammc::canvas_info_paths(const t_canvas* c)
{
    AtomList res;

    if (!canvas_get_current_env(c))
        return res;

    t_canvasenvironment* e = canvas_getenv(const_cast<t_canvas*>(c));

    t_namelist* path_list = e->ce_path;
    while (path_list) {
        res.append(Atom(gensym(path_list->nl_string)));
        path_list = path_list->nl_next;
    }

    return res;
}

t_symbol* ceammc::canvas_info_name(const t_canvas* c)
{
    return c ? c->gl_name : &s_;
}

t_symbol* ceammc::canvas_info_dir(const t_canvas* c)
{
    auto cnv = canvas_root(c);
    if (!canvas_get_current_env(c))
        return &s_;

    return canvas_getdir(const_cast<t_canvas*>(cnv));
}

int ceammc::canvas_info_font(const t_canvas* c)
{
    return c ? c->gl_font : 0;
}

bool ceammc::canvas_info_is_root(const t_canvas* c)
{
    return c ? (c->gl_owner == 0) : false;
}

bool ceammc::canvas_info_is_abstraction(const t_canvas* c)
{
    return c ? canvas_isabstraction(const_cast<t_canvas*>(c)) : false;
}

t_rect ceammc::canvas_info_rect(const _glist* c)
{
    if (!c)
        return t_rect(0, 0, 0, 0);

    if (canvas_info_is_root(c)) {
        return t_rect(c->gl_screenx1,
            c->gl_screeny1,
            c->gl_screenx2 - c->gl_screenx1,
            c->gl_screeny2 - c->gl_screeny1);
    } else {
        return t_rect(c->gl_xmargin,
            c->gl_ymargin,
            c->gl_pixwidth,
            c->gl_pixheight);
    }
}

Canvas::Canvas(t_canvas* c)
    : canvas_(c)
{
}

Canvas::~Canvas()
{
}

ArrayPtr Canvas::createArray(const char* name, size_t n)
{
    ArrayPtr ptr;
    if (!canvas_)
        return ptr;

    t_symbol* sname = gensym(name);
    t_garray* a = graph_array(canvas_, sname, &s_float, n, 0);
    if (!a)
        return ptr;

    ptr.reset(new Array(name));
    return ptr;
}

bool Canvas::connect(t_object* src, size_t nout, t_object* dest, size_t ninl)
{
    if (!src || !dest)
        return false;

    t_outconnect* c = obj_connect(src, int(nout), dest, int(ninl));
    return c != 0;
}

bool Canvas::connect(const BaseObject& src, size_t nout, BaseObject& dest, size_t ninl)
{
    return connect(src.owner(), nout, dest.owner(), ninl);
}

std::vector<const t_object*> Canvas::objectList() const
{
    if (!canvas_)
        return {};

    std::vector<const t_object*> res;

    for (const t_gobj* y = canvas_->gl_list; y; y = y->g_next) {
        // skip non t_object's
        if (!y->g_pd->c_patchable)
            continue;

        res.push_back(reinterpret_cast<const t_object*>(y));
    }

    return res;
}

std::vector<const t_object*> Canvas::findObjectByClassName(t_symbol* name)
{
    std::vector<const t_object*> res;
    if (!canvas_)
        return res;

    for (const t_gobj* y = canvas_->gl_list; y; y = y->g_next) {
        if ((y->g_pd->c_name != name) || (!y->g_pd->c_patchable))
            continue;

        res.push_back(reinterpret_cast<const t_object*>(y));
    }

    return res;
}

t_gobj* Canvas::findIf(std::function<bool(t_gobj*)> pred)
{
    if (!canvas_)
        return nullptr;

    for (t_gobj* y = canvas_->gl_list; y; y = y->g_next) {
        if (pred(y))
            return y;
    }

    return nullptr;
}

t_object* Canvas::findIf(std::function<bool(t_object*)> pred)
{
    if (!canvas_)
        return nullptr;

    for (t_gobj* y = canvas_->gl_list; y; y = y->g_next) {
        auto x = reinterpret_cast<t_object*>(y);

        if (y->g_pd->c_patchable && pred(x))
            return x;
    }

    return nullptr;
}

void Canvas::addExternal(pd::External& ext)
{
    glist_add(canvas_, &ext.object()->te_g);
    ext.setParent(canvas_);
}

std::shared_ptr<pd::External> Canvas::createObject(const char* name, const AtomList& args)
{
    std::shared_ptr<pd::External> ptr;
    if (!canvas_)
        return ptr;

    ptr.reset(new pd::External(name, args));
    return ptr;
}

void Canvas::createPdObject(int x, int y, t_symbol* name, const AtomList& args)
{
    if (!canvas_)
        return;

    static t_symbol* SYM_OBJ = gensym("obj");

    AtomList xargs({ t_float(x), t_float(y) });
    xargs.reserve(args.size() + 3);
    xargs.append(Atom(name));
    xargs.append(args);

    pd_typedmess(&canvas_->gl_obj.te_g.g_pd, SYM_OBJ, static_cast<int>(xargs.size()), xargs.toPdData());
}

_glist* Canvas::createAbstraction(int x, int y, t_symbol* name, const AtomList& args)
{
    static t_symbol* SYM_CANVAS = gensym("canvas");

    if (!canvas_)
        return nullptr;

    createPdObject(x, y, name, args);

    t_gobj* z;
    for (z = canvas_->gl_list; z->g_next; z = z->g_next) {
        // find last created object
    }

    // load abstraction
    if (z && z->g_pd->c_name == SYM_CANVAS)
        return reinterpret_cast<t_canvas*>(z);
    else {
        LIB_ERR << "can't create abstraction: " << name << ' ' << args;
        return nullptr;
    }
}

_glist* Canvas::owner()
{
    return canvas_ ? canvas_->gl_owner : nullptr;
}

void Canvas::loadBang()
{
    if (canvas_)
        canvas_loadbang(canvas_);
}

void Canvas::show()
{
    if (canvas_)
        canvas_vis(canvas_, 1);
}

void Canvas::hide()
{
    if (canvas_)
        canvas_vis(canvas_, 0);
}

void Canvas::free()
{
    if (canvas_) {
        canvas_free(canvas_);
        canvas_ = nullptr;
    }
}

void Canvas::setupDsp()
{
    static t_symbol* SYM_DSP = gensym("dsp");

    if (canvas_) {
        mess0(&canvas_->gl_obj.te_g.g_pd, SYM_DSP);
    }
}

t_symbol* Canvas::name()
{
    return canvas_ ? canvas_->gl_name : &s_;
}

void Canvas::setName(const char* str)
{
    canvas_->gl_name = gensym(str);
}

std::string Canvas::parentName() const
{
    return canvas_->gl_owner ? canvas_->gl_owner->gl_name->s_name : "";
}

_glist* Canvas::current()
{
    return canvas_getcurrent();
}

void Canvas::setCurrent(t_canvas* c)
{
    if (c)
        canvas_setcurrent(c);
}

AtomList ceammc::canvas_info_args(const _glist* c)
{
    AtomList res;
    if (!c)
        return res;

    t_canvasenvironment* env = canvas_get_current_env(c);

    if (!env) {
        t_binbuf* b = c->gl_obj.te_binbuf;
        if (b) {
            int argc = binbuf_getnatom(b);
            t_atom* argv = binbuf_getvec(b);

            for (int i = 1; i < argc; i++)
                res.append(Atom(argv[i]));
        }

        return res;
    }

    return AtomList(env->ce_argc, env->ce_argv);
}

const _glist* ceammc::canvas_root(const _glist* c)
{
    if (!c)
        return nullptr;

    return canvas_getrootfor(const_cast<t_canvas*>(c));
}
