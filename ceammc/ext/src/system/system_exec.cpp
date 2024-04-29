/*****************************************************************************
 * Copyright 2020 Serge Poltavsky. All rights reserved.
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
#include "system_exec.h"
#include "args/argcheck2.h"
#include "ceammc_factory.h"
#include "ceammc_format.h"
#include "ceammc_string.h"
#include "datatype_string.h"
#include "fmt/core.h"
#include "process.hpp"
#include "sys_process.h"

#include <cstdio>

#if defined(__linux__) || defined(__APPLE__)
#include <sys/types.h>
#include <unistd.h>
#endif

SystemExec::SystemExec(const PdArgs& args)
    : BaseObject(args)
    , clock_([this]() { checkProcess(); })
    , priority_(nullptr)
    , log_(this, LOG_DEBUG)
{
    createOutlet();
    createOutlet();

    createCbBoolProperty("@is_running", [this]() { return isRunning(); });
    createCbIntProperty("@state", [this]() { return state(); });

    priority_ = new SymbolEnumProperty("@priority", { "normal", "low" });
    addProperty(priority_);
    addProperty(new SymbolEnumAlias("@low", priority_, gensym("low")));
}

// for std::unique_ptr
SystemExec::~SystemExec() = default;

void SystemExec::onSymbol(t_symbol* s)
{
    onList(Atom(s));
}

static std::vector<std::string> prepareArgs(const AtomListView& lv)
{
    std::vector<std::string> res;
    for (auto& a : lv)
        res.push_back(to_string(a));

    return res;
}

void SystemExec::onList(const AtomListView& lv)
{
    if (lv.empty()) {
        OBJ_ERR << "empty args";
        return;
    }

    if (!process_) {
        process_.reset(new sys::Process());
    }

    if (process_->started()) {
        OBJ_ERR << "process is running";
        return;
    }

    process_->setLog(log_);

    if (!process_->run(prepareArgs(lv))) {
        OBJ_ERR << fmt::format("can't run '{}': {}", to_string(lv), process_->error());
        return;
    }

    sys::Process::Priority pri = sys::Process::PRIORITY_LOW;

#ifndef __WIN32__
    if (!process_->setPriority(pri))
        OBJ_ERR << fmt::format("can't set priority {}: {}", (int)pri, process_->error());
#endif

    should_close_stdin_ = false;
    checkProcess();
}

void SystemExec::m_stop(t_symbol* s, const AtomListView& l)
{
    if (!process_) {
        METHOD_ERR(s) << "no active process";
        return;
    }

    if (!process_->sendSignal(sys::Process::INTERRUPT))
        OBJ_ERR << process_->error();
}

void SystemExec::m_terminate(t_symbol* s, const AtomListView& l)
{
    static const args::ArgChecker chk("FORCE:b?");

    if (!chk.check(l, this))
        return chk.usage(this, s);

    if (!process_) {
        METHOD_ERR(s) << "no active process";
        return;
    }

    if (l.boolAt(0, false)) {
        process_->sendSignal(sys::Process::KILL);
    } else
        process_->sendSignal(sys::Process::TERMINATE);
}

void SystemExec::m_write(t_symbol* s, const AtomListView& l)
{
    if (!process_) {
        METHOD_ERR(s) << "no active process";
        return;
    }

    if (!isRunning()) {
        OBJ_ERR << "process is not running...";
        return;
    }

    process_->addLineToInBuffer(to_string(l, " "));
}

void SystemExec::m_eof(t_symbol* s, const AtomListView&)
{
    should_close_stdin_ = true;
}

void SystemExec::checkProcess()
{
    constexpr int DELAY_TIME = 15;

    if (!process_)
        return;

    log_.flush();

    if (!process_->checkState()) {
        OBJ_ERR << fmt::format("check state error: {}", process_->error());
        return;
    }

    // should never happens
    if (!process_->started())
        OBJ_ERR << process_->error();

    // write to stdin
    if (!process_->writeStdIn())
        OBJ_ERR << process_->error();

    // read stdout
    std::string out_str;
    if (process_->readStdOut(out_str) && !out_str.empty()) {
        std::vector<std::string> lines;
        string::split(lines, out_str, "\r\n");
        for (auto& l : lines)
            atomTo(1, StringAtom(l));
    }

    // read stderr
    std::string err_str;
    if (process_->readStdErr(err_str) && !err_str.empty())
        OBJ_ERR << "[stderr] " << err_str;

    if (process_->running()) {
        if (should_close_stdin_) {
            if (!process_->closeStdIn()) {
                OBJ_ERR << fmt::format("stdin close error: ", process_->error());
                should_close_stdin_ = false;
            }
        }

        clock_.delay(DELAY_TIME);
        return;
    }

    // remove if finished
    if (process_->finished()) {
        floatTo(0, process_->exitStatus());
        process_.reset(nullptr);
        log_.flush();
    }
}

bool SystemExec::isRunning() const
{
    return process_ && process_->running();
}

int SystemExec::state() const
{
    if (!process_)
        return -1;
    else
        return process_->state();
}

void setup_system_exec()
{
    ObjectFactory<SystemExec> obj("system.exec");
    obj.addMethod("stop", &SystemExec::m_stop);
    obj.addMethod("terminate", &SystemExec::m_terminate);
    obj.addMethod("write", &SystemExec::m_write);
    obj.addMethod("eof", &SystemExec::m_eof);

    obj.setXletsInfo({ "list: command name and args" }, { "int: process result code", "process output: flow of String datatypes" });
}
