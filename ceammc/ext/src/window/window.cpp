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
#include "window.h"
#include "ceammc_factory.h"
#include "ceammc_window.h"

#include <algorithm>
#include <map>

static t_symbol* WIN_TRIANGLE = gensym("tri");
static t_symbol* WIN_WELCH = gensym("welch");
static t_symbol* WIN_HANN = gensym("hann");
static t_symbol* WIN_RECT = gensym("rect");
static t_symbol* WIN_SINE = gensym("sine");
static t_symbol* WIN_HAMMING = gensym("hamming");
static t_symbol* WIN_BLACKMAN = gensym("blackman");
static t_symbol* WIN_NUTTALL = gensym("nuttall");
static t_symbol* WIN_BLACKMAN_HARRIS = gensym("blackman-harris");
static t_symbol* WIN_FLATTOP = gensym("flattop");
static t_symbol* WIN_GAUSS = gensym("gauss");
static t_symbol* WIN_DEFAULT = WIN_HANN;
static const size_t DEFAULT_SIZE = 512;

typedef std::map<t_symbol*, WindowFuncPtr> WFuncMap;
static WFuncMap win_func_map;

static bool initFuncMap()
{
    win_func_map[WIN_DEFAULT] = window::hann<float>;
    win_func_map[WIN_WELCH] = window::welch<float>;
    win_func_map[WIN_TRIANGLE] = window::triangle<float>;
    win_func_map[WIN_HANN] = window::hann<float>;
    win_func_map[WIN_RECT] = window::rect<float>;
    win_func_map[WIN_SINE] = window::sine<float>;
    win_func_map[WIN_HAMMING] = window::hamming<float>;
    win_func_map[WIN_BLACKMAN] = window::blackman<float>;
    win_func_map[WIN_NUTTALL] = window::nuttall<float>;
    win_func_map[WIN_BLACKMAN_HARRIS] = window::blackman_harris<float>;
    win_func_map[WIN_FLATTOP] = window::flattop<float>;
    win_func_map[WIN_GAUSS] = window::gauss<20>;
    return true;
}

namespace {
bool init = initFuncMap();
}

Window::Window(const PdArgs& a)
    : BaseObject(a)
    , size_(nullptr)
    , type_(WIN_DEFAULT)
    , fn_(window::hann<float>)
{
    createCbProperty("@type", &Window::pTypeGet, &Window::pTypeSet);
    property("@type")->info().setType(PropertyInfoType::SYMBOL);

    setWindowFunc(positionalSymbolArgument(0, WIN_DEFAULT));

    size_ = new IntPropertyMinEq("@size", positionalFloatArgument(1, DEFAULT_SIZE), 16);
    createProperty(size_);

    createOutlet();
}

void Window::onBang()
{
    AtomList res;
    const size_t N = size_->value();
    res.reserve(N);

    for (size_t i = 0; i < N; i++)
        res.append(fn_(i, N));

    listTo(0, res);
}

void Window::onFloat(float v)
{
    int idx = int(v);
    float res = 0;

    if (0 <= idx && size_t(idx) < size_->value())
        res = fn_(size_t(idx), size_->value());

    floatTo(0, res);
}

void Window::onList(const AtomList& l)
{
    if (!checkArgs(l, ARG_INT, ARG_INT))
        return;

    size_->set(l[1]);
    onFloat(l[0].asFloat());
}

void Window::pTypeSet(const AtomList& l)
{
    if (!checkArgs(l, ARG_SYMBOL))
        return;

    setWindowFunc(l[0].asSymbol());
}

AtomList Window::pTypeGet() const
{
    return AtomList(type_);
}

void Window::setWindowFunc(t_symbol* name)
{
    WFuncMap::iterator it = win_func_map.find(name);
    if (it == win_func_map.end()) {
        OBJ_ERR << "unknown window type: " << name->s_name << ". setting default: "
                << WIN_DEFAULT->s_name;
        return setWindowFunc(WIN_DEFAULT);
    }

    fn_ = it->second;
    type_ = it->first;
}

WindowFuncPtr Window::windowFunc()
{
    return fn_;
}

void Window::m_hann(t_symbol*, const AtomList&)
{
    setWindowFunc(WIN_HANN);
}

void Window::m_tri(t_symbol*, const AtomList&)
{
    setWindowFunc(WIN_TRIANGLE);
}

void Window::m_welch(t_symbol*, const AtomList&)
{
    setWindowFunc(WIN_WELCH);
}

void Window::m_rect(t_symbol*, const AtomList&)
{
    setWindowFunc(WIN_RECT);
}

void Window::m_sine(t_symbol*, const AtomList&)
{
    setWindowFunc(WIN_SINE);
}

void Window::m_hamming(t_symbol*, const AtomList&)
{
    setWindowFunc(WIN_HAMMING);
}

void Window::m_blackman(t_symbol*, const AtomList&)
{
    setWindowFunc(WIN_BLACKMAN);
}

void Window::m_nuttall(t_symbol*, const AtomList&)
{
    setWindowFunc(WIN_NUTTALL);
}

void Window::m_blackman_harris(t_symbol*, const AtomList&)
{
    setWindowFunc(WIN_BLACKMAN_HARRIS);
}

void Window::m_flattop(t_symbol*, const AtomList&)
{
    setWindowFunc(WIN_FLATTOP);
}

void Window::m_gauss(t_symbol*, const AtomList&)
{
    setWindowFunc(WIN_GAUSS);
}

void window_setup()
{
    ObjectFactory<Window> obj("window");
    obj.addAlias("win");
    obj.addMethod("tri", &Window::m_tri);
    obj.addMethod("welch", &Window::m_welch);
    obj.addMethod("hann", &Window::m_hann);
    obj.addMethod("rect", &Window::m_rect);
    obj.addMethod("sine", &Window::m_sine);
    obj.addMethod("hamming", &Window::m_hamming);
    obj.addMethod("blackman", &Window::m_blackman);
    obj.addMethod("nuttall", &Window::m_nuttall);
    obj.addMethod("blackman-harris", &Window::m_blackman_harris);
    obj.addMethod("flattop", &Window::m_flattop);
    obj.addMethod("gauss", &Window::m_gauss);
}
