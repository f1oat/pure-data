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
#ifndef CONWAY_LIFE_H
#define CONWAY_LIFE_H

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iosfwd>
#include <utility>

class ConwayLife {
public:
    static const size_t MAXDIM = 64;
    static const size_t MAXSIZE = MAXDIM * MAXDIM;
    using CellType = uint8_t;

private:
    CellType states_[MAXSIZE], counts_[MAXSIZE];
    uint16_t rows_ { 16 }, cols_ { 16 }, alive_ { 0 };

public:
    ConwayLife();

    inline size_t cols() const { return cols_; }
    inline size_t rows() const { return rows_; }
    inline size_t numCells() const { return rows_ * cols_; }
    inline size_t numAlive() const { return alive_; }
    inline size_t idx(uint16_t row, uint16_t col) const { return row * cols_ + col; }

    inline bool at(uint16_t row, uint16_t col) const { return states_[idx(row, col)]; }
    CellType countAt(uint16_t row, uint16_t col) const { return counts_[idx(row, col)]; }

    bool random(float density = 0.5);
    bool set(uint16_t rows, uint16_t cols);
    bool set(std::initializer_list<bool> l);
    bool setAt(uint16_t row, uint16_t col, bool v);
    bool flipAt(uint16_t row, uint16_t col);

    void clear();
    void next();

    void addBlock(uint16_t row, uint16_t col);
    void addHive(uint16_t row, uint16_t col);
    void addVHive(uint16_t row, uint16_t col);
    void addGlider(uint16_t row, uint16_t col);
    void addRPentamino(uint16_t row, uint16_t col);
    void addBlinker(uint16_t row, uint16_t col);
    void addOctagon2(uint16_t row, uint16_t col);
    bool addFigure(uint16_t row, uint16_t col, std::pair<uint16_t, uint16_t> size, std::initializer_list<bool> l);

private:
    inline CellType& refStateAt(uint16_t row, uint16_t col) { return states_[idx(row, col)]; }

    inline void addNeighbour(uint16_t row, uint16_t col)
    {
        if (row >= rows_ || col >= cols_)
            return;

        counts_[idx(row, col)]++;
    }

    void clearCounts();
    void countNeighbours();
};

std::ostream& operator<<(std::ostream& os, const ConwayLife& life);

#endif // CONWAY_LIFE_H
