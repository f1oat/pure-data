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
#ifndef DATATYPE_SET_H
#define DATATYPE_SET_H

#include "ceammc_atomlist.h"
#include "ceammc_datapointer.h"

#include <boost/functional/hash.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_set.hpp>

using namespace ceammc;

class DataAtom {
    typedef boost::scoped_ptr<Data<BaseData> > DataPtr;
    DataPtr data_;
    Atom atom_;

public:
    DataAtom(const Atom& a);
    DataAtom(const DataAtom& d);
    void set(const Atom& a);
    Atom toAtom() const;
    bool isAtom() const;
    bool isData() const;
    bool isEqual(const Atom& a) const;

    bool operator==(const DataAtom& d) const;

    BaseData* data();
    Data<BaseData>* dataPtr();

private:
};

size_t hash_value(const DataAtom& d);

class DataTypeSet : public BaseData {
private:
    typedef boost::unordered_set<DataAtom> DataSet;
    DataSet data_;

public:
    DataTypeSet();
    ~DataTypeSet();
    void add(const Atom& a);
    void add(const AtomList& l);
    void remove(const Atom& a);
    void remove(const AtomList& l);
    void clear();
    size_t size() const;

    bool contains(const Atom& a) const;
    bool contains(const DataAtom& a) const;
    std::string toString() const;
    DataType type() const;
    bool isEqual(const BaseData* d) const;
    AtomList toList() const;

    BaseData* clone() const;

public:
    static const DataType dataType;

private:
    DataTypeSet(const DataTypeSet& ds);
};

#endif // DATATYPE_SET_H
