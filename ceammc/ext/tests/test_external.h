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
#ifndef TEST_EXTERNAL_H
#define TEST_EXTERNAL_H

#include "ceammc_message.h"
#include "ceammc_pd.h"

#include "test_base.h"
#include "test_external_log_output.h"

#include <type_traits>

using namespace ceammc;
using namespace ceammc::pd;

// from s_sched.h
extern "C" void sched_tick();
extern "C" int* get_sys_schedblocksize();
extern "C" t_float* get_sys_dacsr();

#define PD_TEST_CANVAS() static CanvasPtr cnv = PureData::instance().createTopCanvas("test_canvas")

#define PD_TEST_TYPEDEF(T)                \
    typedef TestPdExternal<T> TestExt##T; \
    typedef TestExternal<T> Test##T

#define PD_TEST_SND_TYPEDEF(T) \
    typedef TestSoundExternal<T> TestExt##T;

#define PD_TEST_SND_DSP(T)                                       \
    template <size_t IN, size_t OUT>                             \
    class T##DSP : public DSP<TestSignal<IN, OUT>, TestExt##T> { \
    public:                                                      \
        T##DSP(TestSignal<IN, OUT>& sig, TestExt##T& ext)        \
            : DSP<TestSignal<IN, OUT>, TestExt##T>(sig, ext)     \
        {                                                        \
        }                                                        \
    };

#define PD_TEST_CORE_INIT()         \
    static void pd_test_core_init() \
    {                               \
        pd_init();                  \
        LogExternalOutput::setup(); \
        ListenerExternal::setup();  \
        test::pdPrintToStdError();  \
    }

#define PD_TEST_MOD_INIT(mod, name)               \
    static void pd_test_mod_init_##mod##_##name() \
    {                                             \
        setup_##mod##_##name();                   \
    }

#define PD_TEST_FULL_INIT(mod, name)          \
    CEAMMC_NO_ASAN static void pd_test_init() \
    {                                         \
        static bool done = false;             \
        if (done)                             \
            return;                           \
        pd_test_core_init();                  \
        pd_test_mod_init_##mod##_##name();    \
        done = true;                          \
    }                                         \
    template <typename F>                     \
    static void pd_test_init(F fn)            \
    {                                         \
        static bool done = false;             \
        if (done)                             \
            return;                           \
        pd_test_core_init();                  \
        pd_test_mod_init_##mod##_##name();    \
        fn();                                 \
        done = true;                          \
    }

#define PD_COMPLETE_TEST_SETUP(T, mod, name) \
    PD_TEST_CANVAS();                        \
    PD_TEST_TYPEDEF(T);                      \
    PD_TEST_CORE_INIT();                     \
    PD_TEST_MOD_INIT(mod, name);             \
    PD_TEST_FULL_INIT(mod, name);

#define PD_COMPLETE_SND_TEST_SETUP(T, mod, name) \
    PD_TEST_CANVAS();                            \
    PD_TEST_SND_TYPEDEF(T);                      \
    PD_TEST_SND_DSP(T);                          \
    PD_TEST_CORE_INIT();                         \
    PD_TEST_MOD_INIT(mod, name);                 \
    PD_TEST_FULL_INIT(mod, name);

struct BANG__ {
    BANG__() {}
};

extern const BANG__ BANG;

class ListenerExternal : public External {
public:
    ListenerExternal(const char* s);
    ListenerExternal(t_symbol* s);
    Message msg();
    t_symbol* addr();
    void reset();

public:
    static void setup();
    static bool isBinded(const char* s);
};

class PropertySetter {
    mutable t_symbol* name_;
    AtomList value_;

public:
    PropertySetter(const char* s, const AtomList& v)
        : name_(gensym(s + 1))
        , value_(v)
    {
    }

    t_symbol* name() const
    {
        return name_;
    }

    const AtomList& value() const
    {
        return value_;
    }

    static PropertySetter create(const char* name, t_float f)
    {
        return PropertySetter(name, AtomList(Atom(f)));
    }

    static PropertySetter create(const char* name, t_symbol* s)
    {
        return PropertySetter(name, AtomList(Atom(s)));
    }

    static PropertySetter create(const char* name, const char* s)
    {
        return PropertySetter(name, AtomList(Atom(gensym(s))));
    }

    static PropertySetter create(const char* name, const AtomList& lst)
    {
        return PropertySetter(name, lst);
    }
};

template <class A>
PropertySetter $1(const char* n, A v) { return PropertySetter::create(n, v); }
template <class A, class B>
PropertySetter $2(const char* n, A a, B b) { return PropertySetter::create(n, LA(a, b)); }

static float operator"" _tick(unsigned long long x)
{
    auto p_sr = get_sys_dacsr();
    if (!p_sr || !*p_sr)
        return 0;

    return (1000 * x * *get_sys_schedblocksize()) / *get_sys_dacsr();
}

static float operator"" _ticks(unsigned long long x)
{
    return (1000 * x * *get_sys_schedblocksize()) / *get_sys_dacsr();
}

template <class T>
class TestPdExternal : public pd::External {
    typedef std::map<t_symbol*, ListenerExternal*> ListenerMap;

    std::vector<LogExternalOutput*> outs_;
    ListenerMap listeners_;

public:
    TestPdExternal(const char* name, const AtomList& args = L())
        : pd::External(name, args)
    {
        REQUIRE(object());

        for (size_t i = 0; i < numOutlets(); i++) {
            LogExternalOutput* e = new LogExternalOutput;
            connectTo(i, e->object(), 0);
            outs_.push_back(e);
        }
    }

    t_object* pdObject()
    {
        return object();
    }

    Property* property(const char* key)
    {
        return property(gensym(key));
    }

    Property* property(t_symbol* key)
    {
        bool is_ceammc_ctl = std::is_base_of<ceammc::BaseObject, T>::value;

        if (is_ceammc_ctl) {
            PdObject<T>* proxy = (PdObject<T>*)pdObject();
            ceammc::BaseObject* base = (ceammc::BaseObject*)proxy->impl;
            return base->property(key);
        }

        return nullptr;
    }

    virtual T* operator->()
    {
        PdObject<T>* obj = (PdObject<T>*)object();
        return obj->impl;
    }

    void call(const char* method, const AtomList& l = L())
    {
        clearAll();
        sendMessage(gensym(method), l);
    }

    void call(t_symbol* method, const AtomList& l = L())
    {
        clearAll();
        sendMessage(method, l);
    }

    template <typename... Args>
    void call(const char* method, Args... args)
    {
        call(method, AtomList({ test_atom_wrap(args)... }));
    }

    void bang()
    {
        clearAll();
        sendBang();
    }

    void call(const char* method, float f)
    {
        clearAll();
        sendMessage(gensym(method), AtomList(Atom(f)));
    }

    void send(t_float f)
    {
        clearAll();
        sendFloat(f);
    }

    void send(t_symbol* s)
    {
        clearAll();
        sendSymbol(s);
    }

    void send(const char* s)
    {
        send(gensym(s));
    }

    void send(const AtomList& lst)
    {
        clearAll();
        sendList(lst);
    }

    void send(const Atom& a)
    {
        clearAll();
        sendList({ a });
    }

    template <typename... Args>
    void send(Args... args)
    {
        clearAll();
        sendList(AtomList(args...));
    }

    void sendBangTo(size_t inlet)
    {
        clearAll();
        pd::External::sendBangTo(inlet);
    }

    void sendFloatTo(t_float v, size_t inlet)
    {
        clearAll();
        pd::External::sendFloatTo(v, inlet);
    }

    void sendSymbolTo(t_symbol* s, size_t inlet)
    {
        clearAll();
        pd::External::sendSymbolTo(s, inlet);
    }

    void sendSymbolTo(const char* s, size_t inlet)
    {
        sendSymbolTo(gensym(s), inlet);
    }

    void sendListTo(const AtomList& l, size_t inlet)
    {
        clearAll();
        pd::External::sendListTo(l, inlet);
    }

    ListenerExternal* addListener(const char* l)
    {
        return addListener(gensym(l));
    }

    ListenerExternal* addListener(t_symbol* addr)
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it != listeners_.end())
            return it->second;

        ListenerExternal* l = new ListenerExternal(addr);
        listeners_[addr] = l;
        return l;
    }

    bool hasListener(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        return it != listeners_.end();
    }

    void listenerReset(t_symbol* addr)
    {
        if (!hasListener(addr))
            return;

        listeners_[addr]->reset();
    }

    bool bangWasSentTo(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it == listeners_.end())
            return false;

        return it->second->msg().isBang();
    }

    bool floatWasSentTo(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it == listeners_.end())
            return false;

        return it->second->msg().isFloat();
    }

    bool symbolWasSentTo(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it == listeners_.end())
            return false;

        return it->second->msg().isSymbol();
    }

    bool listWasSentTo(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it == listeners_.end())
            return false;

        return it->second->msg().isList();
    }

    bool anyWasSentTo(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it == listeners_.end())
            return false;

        return it->second->msg().isAny();
    }

    bool noneWasSentTo(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it == listeners_.end())
            return true;

        return it->second->msg().isNone();
    }

    bool dataWasSentTo(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it == listeners_.end())
            return false;

        return it->second->msg().isData();
    }

    t_float lastSentFloat(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it == listeners_.end())
            return std::numeric_limits<t_float>::min();

        return it->second->msg().atomValue().asFloat();
    }

    t_symbol* lastSentSymbol(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it == listeners_.end())
            return &s_;

        return it->second->msg().atomValue().asSymbol();
    }

    AtomList lastSentList(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it == listeners_.end())
            return L();

        return it->second->msg().listValue();
    }

    AtomList lastSentAny(t_symbol* addr) const
    {
        ListenerMap::const_iterator it = listeners_.find(addr);
        if (it == listeners_.end())
            return L();

        return it->second->msg().anyValue();
    }

    std::vector<Message> messagesAt(size_t n) const
    {
        return outs_.at(n)->msgList();
    }

    bool hasOutputAt(size_t n) const
    {
        return !outs_.at(n)->msg().isNone();
    }

    bool hasOutput() const
    {
        for (size_t i = 0; i < outs_.size(); i++) {
            if (!outs_.at(i)->msg().isNone())
                return true;
        }

        return false;
    }

    bool isOutputDataAt(size_t n) const
    {
        return outs_.at(n)->msg().isData();
    }

    bool isOutputListAt(size_t n) const
    {
        return outs_.at(n)->msg().isList();
    }

    bool isOutputAnyAt(size_t n) const
    {
        return outs_.at(n)->msg().isAny();
    }

    bool isOutputFloatAt(size_t n) const
    {
        return outs_.at(n)->msg().isFloat();
    }

    bool isOutputSymbolAt(size_t n) const
    {
        return outs_.at(n)->msg().isSymbol();
    }

    bool isOutputBangAt(size_t n) const
    {
        return outs_.at(n)->msg().isBang();
    }

    AtomList outputListAt(size_t n) const
    {
        return outs_.at(n)->msg().listValue();
    }

    AtomList outputAnyAt(size_t n) const
    {
        return outs_.at(n)->msg().anyValue();
    }

    t_float outputFloatAt(size_t n) const
    {
        return outs_.at(n)->msg().atomValue().asFloat();
    }

    Atom outputAtomAt(size_t n) const
    {
        return outs_.at(n)->msg().atomValue();
    }

    t_symbol* outputSymbolAt(size_t n) const
    {
        return outs_.at(n)->msg().atomValue().asSymbol();
    }

    t_float floatProperty(const char* name) const
    {
        TestPdExternal<T>* t = (TestPdExternal<T>*)this;
        const T* obj = (const T*)(t->object());
        t_float res = std::numeric_limits<float>::min();
        obj->getProperty(gensym(name), res);
        return res;
    }

    AtomList listProperty(const char* name) const
    {
        TestPdExternal<T>* t = (TestPdExternal<T>*)this;
        const T* obj = (const T*)(t->object());
        AtomList res;
        obj->getProperty(gensym(name), res);
        return res;
    }

    void clearAll()
    {
        for (size_t i = 0; i < outs_.size(); i++)
            outs_[i]->reset();
    }

    void clearAt(size_t n)
    {
        outs_.at(n)->reset();
    }

    TestPdExternal& operator<<(const BANG__&)
    {
        bang();
        return *this;
    }

    TestPdExternal& operator<<(const char* s)
    {
        send(gensym(s));
        return *this;
    }

    TestPdExternal& operator<<(t_float f)
    {
        send(f);
        return *this;
    }

    TestPdExternal& operator<<(t_symbol* s)
    {
        send(s);
        return *this;
    }

    TestPdExternal& operator<<(const AtomList& lst)
    {
        send(lst);
        return *this;
    }

    TestPdExternal& operator<<=(const AtomList& lst)
    {
        call(lst[0].asSymbol(), lst.slice(1));
        return *this;
    }

    AtomList operator>>(const char* name)
    {
        t_symbol* p = gensym(name + 1);
        AtomList res;
        operator->()->getProperty(p, res);
        return res;
    }

    ~TestPdExternal()
    {
        for (auto p : outs_)
            delete p;

        for (auto& kv : listeners_)
            delete kv.second;
    }

    TestPdExternal& operator<<(const PropertySetter& p)
    {
        (*this)->setProperty(p.name(), p.value());
        return *this;
    }

    void schedMs(t_float ms)
    {
        //        static const double UNIT = clock_getsystimeafter(1) - clock_getlogicaltime();
        //        return (x * *get_sys_schedblocksize()) / get_sys_dacsr();
        //        while (n-- > 0)
        //            sched_tick();
    }

    void schedTicks(size_t n)
    {
        while (n-- > 0)
            sched_tick();
    }
};

#endif // TEST_EXTERNAL_H
