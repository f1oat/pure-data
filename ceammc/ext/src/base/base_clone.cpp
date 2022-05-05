/*****************************************************************************
 * Copyright 2022 Serge Poltavsky. All rights reserved.
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
#include "base_clone.h"
#include "ceammc_factory.h"
#include "ceammc_inlet.h"
#include "ceammc_outlet.h"
#include "fmt/format.h"

constexpr const char* CONTAINTER_NAME = "/CONTAINER/";
constexpr const char* PATTERN_NAME = "/PATTERN/";
constexpr const char* SYM_CANVAS_RESTORE = "#Z";
constexpr int PATTERN_WINDOW_W = 700;
constexpr int PATTERN_WINDOW_H = 500;

extern "C" {
#include "g_canvas.h"
#include "g_ceammc_draw.h"
#include "m_imp.h"

void canvas_dodsp(t_canvas* x, int toplevel, t_signal** sp);
t_signal* signal_newfromcontext(int borrowed);
void signal_makereusable(t_signal* sig);
}

namespace {

class DspSuspendGuard {
    const int dsp_state_;

public:
    DspSuspendGuard()
        : dsp_state_(canvas_suspend_dsp())
    {
    }

    ~DspSuspendGuard() { canvas_resume_dsp(dsp_state_); }
};

void clone_pop_canvas(t_canvas* x, bool show)
{
    if (show)
        canvas_vis(x, 1);

    pd_popsym(&x->gl_pd);
    canvas_resortinlets(x);
    canvas_resortoutlets(x);
    x->gl_loading = 0;
}

t_canvas* clone_create_container(t_canvas* owner)
{
    t_atom a[6];
    SETFLOAT(a, 0);
    SETFLOAT(a + 1, 22);
    SETFLOAT(a + 2, PATTERN_WINDOW_W);
    SETFLOAT(a + 3, PATTERN_WINDOW_H);
    SETSYMBOL(a + 4, gensym(CONTAINTER_NAME));
    SETFLOAT(a + 5, 1);

    auto* c = canvas_new(0, 0, 6, a);
    c->gl_owner = owner;
    clone_pop_canvas(c, false);
    return c;
}

t_canvas* clone_create_pattern(t_canvas* owner, bool show)
{
    t_atom a[6];

    SETFLOAT(a, 0);
    SETFLOAT(a + 1, 22);
    SETFLOAT(a + 2, PATTERN_WINDOW_W);
    SETFLOAT(a + 3, PATTERN_WINDOW_H);
    SETSYMBOL(a + 4, gensym(PATTERN_NAME));
    SETFLOAT(a + 5, 0);

    auto c = canvas_new(0, 0, 6, a);
    c->gl_owner = owner;
    clone_pop_canvas(c, show);
    glist_add(owner, &c->gl_obj.te_g);
    return c;
}

void clone_bind_restore(t_object* owner)
{
    auto z = gensym(SYM_CANVAS_RESTORE);
    z->s_thing = nullptr;
    pd_bind(&owner->te_g.g_pd, z);
}

/**
 * get canvas content as binbuf
 * @note caller should free result
 */
t_binbuf* clone_get_canvas_content(const t_canvas* z)
{
    auto x = (t_canvas*)z;
    auto b = binbuf_new();

    for (auto y = x->gl_list; y != nullptr; y = y->g_next)
        gobj_save(y, b);

    t_linetraverser t;
    linetraverser_start(&t, x);

    auto sym_X = gensym("#X");
    auto sym_connect = gensym("connect");

    while (linetraverser_next(&t)) {
        int srcno = canvas_getindex(x, &t.tr_ob->ob_g);
        int sinkno = canvas_getindex(x, &t.tr_ob2->ob_g);

        binbuf_addv(b, "ssiiii;", sym_X, sym_connect,
            srcno, t.tr_outno,
            sinkno, t.tr_inno);
    }

    return b;
}

void clone_set_canvas_content(t_canvas* x, const t_binbuf* b, int ninst, int inst)
{
    DspSuspendGuard dsp_guard;

    t_symbol* asym = gensym("#A");
    /* save and clear bindings to symbols #A, #N, #X; restore when done */
    t_pd *boundx = s__X.s_thing, *bounda = asym->s_thing,
         *boundn = s__N.s_thing;
    asym->s_thing = 0;
    s__X.s_thing = &x->gl_pd;
    s__N.s_thing = &pd_canvasmaker;

    auto n = binbuf_getnatom(b);
    auto v = binbuf_getvec(b);
    // store replaced atoms
    std::vector<int> nvec, ivec;
    // placeholder: number of instances
    auto nsym = gensym("$1");
    // placeholder: instance index
    auto isym = gensym("$2");

    // if ninst < 0 we are in a pattern
    if (ninst >= 0 && inst >= 0) {
        // replace placeholders
        for (int i = 0; i < n; i++) {
            auto a = &v[i];
            if (a->a_type == A_SYMBOL) {
                auto s = atom_getsymbol(a);
                if (s == nsym) {
                    nvec.push_back(i);
                    SETFLOAT(a, (t_float)ninst);
                } else if (s == isym) {
                    ivec.push_back(i);
                    SETFLOAT(a, (t_float)inst);
                }
            }
        }
    }

    binbuf_eval(b, 0, 0, 0);

    if (ninst >= 0 && inst >= 0) {
        // restore placeholders
        for (auto i : nvec) {
            SETSYMBOL(v + i, nsym);
        }

        for (auto i : ivec) {
            SETSYMBOL(v + i, isym);
        }
    }

    // restore bindings
    asym->s_thing = bounda;
    s__X.s_thing = boundx;
    s__N.s_thing = boundn;
}

class BaseCloneFactory : public SoundExternalFactory<BaseClone> {
public:
    BaseCloneFactory(const char* name, int flags)
        : SoundExternalFactory<BaseClone>(name, flags)
    {
        class_addmethod(this->classPointer(), (t_method)restore_fn, gensym("restore"), A_GIMME, 0);
        class_setsavefn(this->classPointer(), (t_savefn)save_fn);
    }

    static void save_fn(ObjectProxy* x, t_binbuf* b)
    {
        x->impl->onSave(b);
        x->impl->updateInstances();
    }

    static void restore_fn(ObjectProxy* x, t_symbol* /*sel*/, int argc, t_atom* argv)
    {
        x->impl->onRestore(AtomListView(argv, argc));
    }
};

using MouseFn = void (*)(t_canvas*, t_floatarg, t_floatarg, t_floatarg, t_floatarg);

enum : int {
    NOMOD = 0,
    SHIFTMOD = 1,
    CTRLMOD = 2,
    ALTMOD = 4,
    RIGHTCLICK = 8
};

MouseFn ceammc_old_canvas_mouse_fn = nullptr;

void canvas_new_mouse_fn(t_canvas* x, t_floatarg xpos, t_floatarg ypos,
    t_floatarg which, t_floatarg mod)
{
    if (!x)
        return;

    const bool runmode = (((int)mod & CTRLMOD) || (!x->gl_edit));
    const bool rightclick = ((int)mod & RIGHTCLICK);

    if (x->gl_editor && !runmode && !rightclick) {
        // find first selected BaseClone object
        auto sel = x->gl_editor->e_selection;
        while (sel) {
            auto clone_obj = sel->sel_what;
            sel = sel->sel_next;
            if (clone_obj->g_pd == BaseCloneFactory::classPointer()) {
                auto x = reinterpret_cast<PdObject<BaseClone>*>(clone_obj);
                // store the object content to restore it in a future
                x->impl->storeContent();
                break;
            }
        }
    }

    if (ceammc_old_canvas_mouse_fn)
        return ceammc_old_canvas_mouse_fn(x, xpos, ypos, which, mod);
}

}

CloneInstance::CloneInstance(uint16_t idx, t_canvas* owner)
    : idx_(idx)
    , canvas_(nullptr)
{
    t_atom a[6];
    SETFLOAT(a, 0);
    SETFLOAT(a + 1, 22);
    SETFLOAT(a + 2, PATTERN_WINDOW_W);
    SETFLOAT(a + 3, PATTERN_WINDOW_H);
    SETSYMBOL(a + 4, gensym(fmt::format("instance({})", idx).c_str()));
    SETFLOAT(a + 5, 0);

    canvas_ = canvas_new(nullptr, nullptr, 6, a);
    canvas_->gl_owner = owner;
    clone_pop_canvas(canvas_, false);
}

CloneInstance::CloneInstance(CloneInstance&& ci)
    : idx_(ci.idx_)
    , canvas_(std::move(ci.canvas_))
{
}

CloneInstance::~CloneInstance()
{
    if (canvas_) {
        canvas_free(canvas_);
        canvas_ = nullptr;
    }
}

void CloneInstance::fillWithPattern(const t_binbuf* pattern, int num)
{
    if (!canvas_)
        return;

    clear();
    clone_set_canvas_content(canvas_, pattern, num, (int)idx_);
}

void CloneInstance::loadbang()
{
    if (canvas_)
        canvas_loadbang(canvas_);
}

void CloneInstance::clear()
{
    if (!canvas_)
        return;

    t_gobj* y = nullptr;
    while ((y = canvas_->gl_list))
        glist_delete(canvas_, y);
}

void CloneInstance::open()
{
    if (canvas_)
        canvas_vis(canvas_, 1);
}

t_binbuf* BaseClone::old_content_ = nullptr;

BaseClone::BaseClone(const PdArgs& args)
    : BaseObject(args)
    , num_(nullptr)
    , wrapper_(nullptr)
    , pattern_(nullptr)
    , n_sig_in_(0)
    , n_sig_out_(0)
    , block_size_(sys_getblksize())
    , sample_rate_(sys_getsr())
    , renaming_(false)
{
    num_ = new IntProperty("@n", 0);
    num_->setInitOnly();
    num_->checkMinEq(0);
    num_->setArgIndex(0);
    addProperty(num_);

    // pattern container
    wrapper_ = clone_create_container(canvas());

    if (!isPatchLoading()) {
        renaming_ = (old_content_ != nullptr);
        pattern_ = clone_create_pattern(wrapper_, !renaming_);
        if (renaming_) {
            clone_set_canvas_content(pattern_, old_content_, -1, -1);
            binbuf_free(old_content_);
            old_content_ = nullptr;
        }
    } else {
        clone_bind_restore(owner());
    }
}

BaseClone::~BaseClone()
{
    if (wrapper_) {
        canvas_free(wrapper_);
        wrapper_ = nullptr;
        // clone_pattern_ should be freed automatically
    }
}

void BaseClone::onClick(t_floatarg /*xpos*/, t_floatarg /*ypos*/, t_floatarg /*shift*/, t_floatarg /*ctrl*/, t_floatarg /*alt*/)
{
    if (!wrapper_) {
        OBJ_ERR << "NULL canvas pattern wrapper";
        return;
    }

    if (!pattern_) {
        OBJ_ERR << "NULL canvas pattern";
        return;
    }

    canvas_vis(pattern_, 1);
}

void BaseClone::initDone()
{
    try {
        if (num_->value() < 1)
            throw std::invalid_argument("number of copies required");

        if (!initInstances())
            throw std::runtime_error("can't init instances");

        if (renaming_) {
            updateInstances();
            renaming_ = false;
        }

    } catch (std::exception& e) {
        if (!args().empty())
            OBJ_ERR << e.what();
        else
            OBJ_LOG << e.what(); // object without args - used in help
    }
}

bool BaseClone::initInstances()
{
    DspSuspendGuard dsp_guard;

    const uint16_t NINSTANCE = num_->value();
    instances_.reserve(NINSTANCE);

    for (uint16_t i = 0; i < NINSTANCE; i++) {
        if (!initInstance(i))
            return false;
    }

    return true;
}

bool BaseClone::initInstance(uint16_t idx)
{
    instances_.emplace_back(idx, wrapper_);
    return true;
}

void BaseClone::updateInstances()
{
    DspSuspendGuard dsp_guard;

    if (pattern_) {
        auto bb = clone_get_canvas_content(pattern_);

        for (auto& i : instances_) {
            i.fillWithPattern(bb, instances_.size());
            i.loadbang();
        }

        binbuf_free(bb);

        const bool visible = gobj_shouldvis(&owner()->te_g, canvas());

        if (visible)
            gobj_vis(&owner()->te_g, canvas(), 0);

        updateInlets();
        updateOutlets();

        if (visible) {
            gobj_vis(&owner()->te_g, canvas(), 1);
            canvas_fixlinesfor(canvas(), owner());
        }
    }
}

void BaseClone::updateInlets()
{
    const size_t NUM_PATTERN_INLETS = pattern_ ? obj_ninlets(&pattern_->gl_obj) : 0;
    const size_t NUM_ISTANCES = num_->value();
    const auto new_count = NUM_PATTERN_INLETS * NUM_ISTANCES;
    const auto prev_count = inlets().size();

    // add/remove inlets if needed
    if (prev_count < new_count) {
        // add new
        for (size_t i = prev_count; i < new_count; i++) {
            auto in = inlet_new(owner(), nullptr, nullptr, nullptr);
            appendInlet(in);
        }
    } else if (prev_count > new_count) {
        // remove extra
        while (inlets().size() > new_count) {
            if (!popInlet())
                break;
        }
    }

    // update inlet destinations
    for (size_t n = 0; n < NUM_ISTANCES; n++) {
        // instance canvas
        auto ci = instances_[n].canvas();
        auto x = &ci->gl_obj;

        // inlet index
        size_t idx = n * NUM_PATTERN_INLETS;

        // iterate instance inlets
        InletIterator it(x);
        while (it) {
            auto inl = inlets()[idx];
            util::inlet_set_dest(inl, it.asObject());
            util::inlet_set_signal(inl, it.isSignal());
            it.next();
            idx++;
        }
    }

    n_sig_in_ = obj_nsiginlets(&pattern_->gl_obj) * NUM_ISTANCES;
}

void BaseClone::updateOutlets()
{
    const size_t NUM_PATTERN_OUTLETS = pattern_ ? obj_noutlets(&pattern_->gl_obj) : 0;
    const size_t NUM_ISTANCES = num_->value();
    const auto new_count = NUM_PATTERN_OUTLETS * NUM_ISTANCES;
    const auto prev_count = outlets().size();

    // add/remove outlets if needed
    if (prev_count < new_count) {
        // add new
        for (size_t i = prev_count; i < new_count; i++) {
            auto out = outlet_new(owner(), &s_list);
            appendOutlet(out);
        }
    } else if (prev_count > new_count) {
        // remove extra
        while (outlets().size() > new_count) {
            if (!popOutlet())
                break;
        }
    }

    proxy_.clear();
    proxy_.reserve(outlets().size());
    n_sig_out_ = 0;

    // divizion by zero check: in next loop
    if (outlets().empty() || NUM_PATTERN_OUTLETS == 0)
        return;

    for (size_t i = 0; i < outlets().size(); i++) {
        const auto inst_idx = i / NUM_PATTERN_OUTLETS;
        const auto src_idx = i % NUM_PATTERN_OUTLETS;

        proxy_.emplace_back(this, (int)i);

        auto ci = instances_[inst_idx].canvas();
        auto x = &ci->gl_obj;
        obj_connect(x, src_idx, proxy_.back().object(), 0);

        // count signal outlets
        const bool is_sig = obj_issignaloutlet(x, src_idx);
        if (is_sig) {
            util::outlet_set_signal(outletAt(i), true);
            n_sig_out_++;
        }
    }
}

void BaseClone::signalInit(t_signal** sp)
{
    if (n_sig_in_ != 0 || n_sig_out_ != 0) {
        block_size_ = size_t(sp[0]->s_n);
        sample_rate_ = size_t(sp[0]->s_sr);
    } else {
        block_size_ = 64;
        sample_rate_ = sys_getsr();
    }

    auto sig_in = sp;
    auto sig_out = sp + n_sig_in_;

    const size_t NINST = instances_.size();
    const size_t INST_IN = n_sig_in_ / NINST;
    const size_t INST_OUT = n_sig_out_ / NINST;

    auto inst_sig_in = (t_signal**)alloca((INST_IN + INST_OUT) * sizeof(t_signal*));
    auto inst_sig_out = inst_sig_in + INST_IN;

    for (size_t i = 0; i < NINST; i++) {
        auto& inst = instances_[i];

        for (size_t k = 0; k < INST_IN; k++)
            inst_sig_in[k] = sig_in[i * INST_IN + k];

        for (size_t k = 0; k < INST_OUT; k++)
            inst_sig_out[k] = signal_newfromcontext(1);

        canvas_dodsp(inst.canvas(), 0, inst_sig_in);

        for (size_t k = 0; k < INST_OUT; k++) {
            auto out = sig_out[i * INST_OUT + k];
            dsp_add_copy(inst_sig_out[k]->s_vec, out->s_vec, out->s_n);
            //            signal_makereusable(inst_sig_out[k]);
        }
    }
}

void BaseClone::processBlock()
{
}

void BaseClone::setupDSP(t_signal** sp)
{
    signalInit(sp);
    dsp_add(dspPerform, 1, static_cast<void*>(this));

    OBJ_DBG << fmt::format("{}:\n"
                           " - block size: {}\n"
                           " - samplerate: {}\n"
                           " - inputs:     {}\n"
                           " - outputs:    {}",
        __FUNCTION__, block_size_, sample_rate_, n_sig_in_, n_sig_out_);
}

void BaseClone::m_open(t_symbol* s, const AtomListView& lv)
{
    if (!checkArgs(lv, ARG_NATURAL, s))
        return;

    int n = lv.intAt(0, 0);
    if (n < 0 || n > instances_.size()) {
        OBJ_ERR << fmt::format("invalid index: {}", n);
        return;
    }

    instances_[n].open();
}

void BaseClone::m_menu_open(t_symbol*, const AtomListView& /*lv*/)
{
    canvas_vis(pattern_, 1);
}

void BaseClone::storeContent() const
{
    if (!old_content_)
        old_content_ = binbuf_new();
    else
        binbuf_clear(old_content_);

    if (pattern_)
        old_content_ = clone_get_canvas_content(pattern_);
}

void BaseClone::onRestore(const AtomListView& lv)
{
    auto cnv = gensym("#X");
    if (cnv->s_thing == nullptr) {
        OBJ_ERR << "null canvas";
        return;
    }

    // current canvas binding
    auto z = cnv->s_thing;
    if (!z || *z != canvas_class) {
        OBJ_ERR << "not a canvas";
        return;
    }

    // current canvas pointer
    auto x = (t_canvas*)z;

    clone_pop_canvas(x, false);
    x->gl_owner = wrapper_;

    auto t = &x->gl_obj;

    t->te_width = 0; /* don't know it yet. */
    t->te_type = T_OBJECT;
    t->te_binbuf = binbuf_new();
    t->te_xpix = atom_getfloatarg(0, lv.size(), lv.toPdData());
    t->te_ypix = atom_getfloatarg(1, lv.size(), lv.toPdData());

    if (lv.size() > 2)
        binbuf_restore(t->te_binbuf, lv.size() - 2, lv.toPdData() + 2);

    glist_add(wrapper_, &t->te_g);

    if (pattern_) {
        canvas_free(pattern_);
        pattern_ = nullptr;
    }

    pattern_ = x;

    updateInstances();
}

void BaseClone::onSave(t_binbuf* b) const
{
    auto obj = owner();

    // save object
    binbuf_addv(b, "ssii", gensym("#X"), gensym("obj"),
        (int)obj->te_xpix, (int)obj->te_ypix);
    binbuf_addbinbuf(b, obj->te_binbuf);
    if (obj->te_width)
        binbuf_addv(b, ",si", gensym("f"), (int)obj->te_width);
    binbuf_addv(b, ";");

    if (wrapper_ && pattern_) {
        auto x = pattern_;

        /* have to go to original binbuf to find out how we were named. */
        binbuf_addv(b, "ssiiiisi;", gensym("#N"), gensym("canvas"),
            (int)(x->gl_screenx1),
            (int)(x->gl_screeny1),
            (int)(x->gl_screenx2 - x->gl_screenx1),
            (int)(x->gl_screeny2 - x->gl_screeny1),
            gensym(PATTERN_NAME),
            0);

        // save objects
        for (auto y = x->gl_list; y != nullptr; y = y->g_next)
            gobj_save(y, b);

        // save connections
        t_linetraverser t;
        linetraverser_start(&t, x);
        while (linetraverser_next(&t)) {
            int srcno = canvas_getindex(x, &t.tr_ob->ob_g);
            int sinkno = canvas_getindex(x, &t.tr_ob2->ob_g);
            binbuf_addv(b, "ssiiii;", gensym("#X"), gensym("connect"),
                srcno, t.tr_outno, sinkno, t.tr_inno);
        }

        binbuf_addv(b, "ssiis", gensym("#Z"), gensym("restore"),
            (int)obj->te_xpix, (int)obj->te_ypix, gensym(PATTERN_NAME));
        binbuf_addv(b, ";");
    }

    //    if (obj->te_width)
    //        binbuf_addv(b, "ssi;",
    //            gensym("#X"), gensym("f"), (int)obj->te_width);
}

void setup_base_clone()
{
    BaseCloneFactory obj("clone:", OBJECT_FACTORY_DEFAULT);
    obj.useClick();
    obj.addMethod("open", &BaseClone::m_open);

    obj.addMethod("menu-open", &BaseClone::m_menu_open);

    // HACK to rename the object without loosing it pattern
    auto mouse_fn = (MouseFn)zgetfn(&canvas_class, gensym("mouse"));
    if (mouse_fn != canvas_new_mouse_fn) {
        ceammc_old_canvas_mouse_fn = mouse_fn;
        class_addmethod(canvas_class, (t_method)canvas_new_mouse_fn, gensym("mouse"), A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_NULL);
    }
}
