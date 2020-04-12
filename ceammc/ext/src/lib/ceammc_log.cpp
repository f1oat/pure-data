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

#include "ceammc_log.h"
#include "ceammc_object.h"

#include <cstdio>

namespace ceammc {

#define PG_LOG_FMT_PREFIX "[%s]"
#define PD_LOG_FMT PG_LOG_FMT_PREFIX " %s"
constexpr size_t PD_MAXLOGSTRLENGTH = MAXPDSTRING - (sizeof(PD_LOG_FMT) + 16);
// see s_print.c for details, 16 seems to be enough for all cases

static const char* pd_object_name(const void* x)
{
    if (!x)
        return "ceammc";
    else
        return class_getname(pd_class(&static_cast<const t_gobj*>(x)->g_pd));
}

void pdPost(const char* name, const std::string& s)
{
    const size_t N = PD_MAXLOGSTRLENGTH - strlen(name);
    assert(N < MAXPDSTRING);

    if (s.size() < N) {
        post(PD_LOG_FMT, name, s.c_str());
    } else {
        char buf[MAXPDSTRING] = { 0 };
        for (size_t i = 0; i < s.size(); i += N) {
            const auto len = s.copy(buf, N, i);
            buf[len] = '\0';
            post(PD_LOG_FMT, name, buf);
        }
    }
}

void pdDebug(const void* pd_obj, const std::string& s)
{
    pdLog(pd_obj, LOG_DEBUG, s);
}

void pdLog(const void* pd_obj, LogLevel level, const std::string& s)
{
    const char* name = pd_object_name(pd_obj);
    const size_t N = PD_MAXLOGSTRLENGTH - strlen(name);
    assert(N < MAXPDSTRING);

    if (s.size() < N) {
        logpost(pd_obj, static_cast<int>(level), PD_LOG_FMT, name, s.c_str());
    } else {
        char buf[MAXPDSTRING] = { 0 };
        for (size_t i = 0; i < s.size(); i += N) {
            const auto len = s.copy(buf, N, i);
            buf[len] = '\0';

            logpost(pd_obj, static_cast<int>(level), PD_LOG_FMT, name, s.c_str());
        }
    }
}

void pdError(const void* pd_obj, const std::string& s)
{
    const char* name = pd_object_name(pd_obj);
    const size_t N = PD_MAXLOGSTRLENGTH - strlen(name);
    assert(N < MAXPDSTRING);

    if (s.size() < N) {
        pd_error(pd_obj, PD_LOG_FMT, name, s.c_str());
    } else {
        char buf[MAXPDSTRING] = { 0 };
        for (size_t i = 0; i < s.size(); i += N) {
            const auto len = s.copy(buf, N, i);
            buf[len] = '\0';

            pd_error(pd_obj, PD_LOG_FMT, name, buf);
        }
    }
}

Error::Error(const BaseObject* obj)
    : LogBaseObject(obj, LogLevel::LOG_ERROR)
{
}

Post::Post(const BaseObject* obj)
    : LogBaseObject(obj, LogLevel::LOG_POST)
{
}

Debug::Debug(const BaseObject* obj)
    : LogBaseObject(obj, LOG_DEBUG)
{
}

Log::Log(const BaseObject* obj)
    : LogBaseObject(obj, LogLevel::LOG_ALL)
{
}

LogPdObject::LogPdObject(const void* obj, LogLevel level, bool log_empty)
    : obj_(obj)
    , level_(level)
    , log_empty_(log_empty)
{
}

LogPdObject::~LogPdObject()
{
    flush();
}

void LogPdObject::error(const std::string& str) const
{
    pdError(obj_, str);
}

void LogPdObject::post(const std::string& str) const
{
    pdPost(pd_object_name(obj_), str);
}

void LogPdObject::debug(const std::string& str) const
{
    pdDebug(obj_, str);
}

void LogPdObject::flush()
{
    if (!log_empty_ && str().empty())
        return;

    if (!pd_objectmaker) {
        std::cerr << str() << "\n";
        return;
    }

    switch (level_) {
    case LogLevel::LOG_FATAL:
    case LogLevel::LOG_ERROR:
        error(str());
        break;
    case LOG_DEBUG:
        debug(str());
        break;
    case LogLevel::LOG_ALL:
        pdLog(obj_, level_, str());
        break;
    case LogLevel::LOG_POST:
        post(str());
        break;
    case LogLevel::LOG_NONE:
        break;
    }

    str("");
}

void LogPdObject::endl()
{
    flush();
}

std::string LogPdObject::prefix() const
{
    char buf[64];
    snprintf(buf, sizeof(buf), PG_LOG_FMT_PREFIX, pd_object_name(obj_));
    return buf;
}

void LogPdObject::setLogLevel(LogLevel level)
{
    level_ = level;
}

void LogPdObject::setLogEmpty(bool v)
{
    log_empty_ = v;
}

LogBaseObject::LogBaseObject(const BaseObject* obj, LogLevel level)
    : LogPdObject(obj ? static_cast<void*>(obj->owner()) : nullptr, level)
{
}

}

std::ostream& operator<<(std::ostream& os, t_symbol* s)
{
    if (s == nullptr)
        os << "NULLSYM";
    else
        os << '"' << s->s_name << '"';

    return os;
}
