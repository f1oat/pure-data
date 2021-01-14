/*****************************************************************************
 * Copyright 2021 Serge Poltavsky. All rights reserved.
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
#include "seq_life.h"
#include "ceammc_args.h"
#include "ceammc_factory.h"
#include "fmt/format.h"

static ArgChecker m_cell_check("i i b");
static ArgChecker m_pos_check("i i");

SeqLife::SeqLife(const PdArgs& args)
    : BaseObject(args)
    , rows_(nullptr)
    , cols_(nullptr)
{
    rows_ = new IntProperty("@rows", 16);
    rows_->checkClosedRange(1, MAX_MTX_SIZE);
    rows_->setArgIndex(0);
    rows_->setSuccessFn([this](Property*) { out_buffer_.resizePad(numCells(), Atom(0.)); });
    addProperty(rows_);

    cols_ = new IntProperty("@cols", 16);
    cols_->checkClosedRange(1, MAX_MTX_SIZE);
    cols_->setArgIndex(1);
    cols_->setSuccessFn([this](Property*) { out_buffer_.resizePad(numCells(), Atom(0.)); });
    addProperty(cols_);

    createCbIntProperty("@size", [this]() { return numCells(); });

    createOutlet();
}

void SeqLife::initDone()
{
    life_.set(rows_->value(), cols_->value());
    out_buffer_.resizePad(life_.numCells(), Atom(0.));
}

void SeqLife::onBang()
{
    output();
}

void SeqLife::dump() const
{
    BaseObject::dump();

    Error err(this);
    err << '\n'
        << life_;
}

void SeqLife::m_rand(t_symbol* s, const AtomListView& lv)
{
    static ArgChecker argcheck("f0..1?");

    if (!argcheck.check(lv)) {
        METHOD_ERR(s) << "usage: DENSITY(0..1)";
        return;
    }

    const auto dens = lv.floatAt(0, 0.6);
    if (!life_.random(dens))
        METHOD_ERR(s) << "can't set density: " << dens;
}

void SeqLife::m_next(t_symbol* s, const AtomListView& lv)
{
    life_.next();
}

void SeqLife::m_cell(t_symbol* s, const AtomListView& lv)
{
    if (!m_cell_check.check(lv)) {
        METHOD_ERR(s) << "usage: ROW COL STATE";
        return;
    }

    const auto row = lv[0].asInt();
    const auto col = lv[1].asInt();
    if (!checkPos(s, row, col))
        return;

    life_.setAt(row, col, lv[2].asBool());
}

void SeqLife::m_block(t_symbol* s, const AtomListView& lv)
{
    if (!m_pos_check.check(lv)) {
        METHOD_ERR(s) << "usage: ROW COL";
        return;
    }

    const auto row = lv[0].asInt();
    const auto col = lv[1].asInt();
    if (!checkPos(s, row, col))
        return;

    life_.addBlock(row, col);
}

size_t SeqLife::numCells() const
{
    return rows_->value() * cols_->value();
}

bool SeqLife::checkPos(t_symbol* method, int row, int col)
{
    if (row < 0 || row >= rows_->value()) {
        METHOD_ERR(method) << fmt::format("row value in 0..{} range expected, got {}", rows_->value() - 1, row);
        return false;
    }

    if (col < 0 || col >= cols_->value()) {
        METHOD_ERR(method) << fmt::format("column value in 0..{} range expected, got {}", cols_->value() - 1, col);
        return false;
    }

    return true;
}

void SeqLife::output()
{
    for (size_t r = 0; r < life_.rows(); r++) {
        for (size_t c = 0; c < life_.cols(); c++) {
            const size_t i = r * life_.rows() + c;
            if (i > out_buffer_.size())
                break;

            out_buffer_[i] = life_.at(r, c);
        }
    }

    listTo(0, out_buffer_);
}

void setup_seq_life()
{
    ObjectFactory<SeqLife> obj("seq.life");
    obj.addMethod("next", &SeqLife::m_next);
    obj.addMethod("cell", &SeqLife::m_cell);
    obj.addMethod("rand", &SeqLife::m_rand);
    obj.addMethod("block", &SeqLife::m_block);
}
