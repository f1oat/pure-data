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
#ifndef CEAMMC_EXT_LIST_H
#define CEAMMC_EXT_LIST_H

#include "ceammc_atomlist.h"
#include "ceammc_atomlist_view.h"

#include <utility>
#include <vector>

namespace ceammc {
class AtomListView;
class DataTypeDict;

namespace list {

    size_t shortestListSize(const std::vector<AtomList>& l);
    size_t longestListSize(const std::vector<AtomList>& l);
    std::pair<size_t, size_t> minmaxListSize(const std::vector<AtomList>& l);

    typedef const Atom* (AtomList::*constAtomlistAt)(int) const;

    AtomList interleaveMinLength(const std::vector<AtomList>& l);
    AtomList interleaveMaxLength(const std::vector<AtomList>& l, constAtomlistAt fn);

    AtomList interleavePadWith(const std::vector<AtomList>& l, const Atom& pad);
    AtomList interleaveClip(const std::vector<AtomList>& l);
    AtomList interleaveWrap(const std::vector<AtomList>& l);
    AtomList interleaveFold(const std::vector<AtomList>& l);

    void deinterleaveMinLength(const AtomListView& in, std::vector<AtomList>& out);
    void deinterleavePadWith(const AtomListView& in, const Atom& pad, std::vector<AtomList>& out);

    MaybeFloat average(const AtomListView& lv);

    AtomList countRepeats(const AtomListView& lv, bool normalizeBySum = false);

    bool containsAllOff(const AtomListView& input, const AtomListView& needles);
    bool containsAnyOff(const AtomListView& input, const AtomListView& needles);

    AtomList sliceWrap(const AtomListView& lv, int pos, size_t len);
    AtomList sliceClip(const AtomListView& lv, int pos, size_t len);
    AtomList sliceFold(const AtomListView& lv, int pos, size_t len);

    bool calcClipIndex(int pos, size_t len, size_t* idx);
    bool calcWrapIndex(int pos, size_t len, size_t* idx);
    bool calcFoldIndex(int pos, size_t len, size_t* idx);

    AtomList rotate(const AtomListView& lv, int steps);
    AtomList repeat(const AtomListView& lv, size_t n);

    AtomList histogram(const AtomListView& lv, size_t bins);

    /**
     * Normalize floats by total sum, that goes to 1, and each element is equal value/sum
     * @param src - source data (should contains only t_floats, otherwise result in undefined)
     * @param dest - write result
     * @return true on success, false on error (zero range, empty etc.)
     */
    bool normalizeBySum(const AtomListView& src, AtomList& dest);

    /**
     * Normalize floats to [0, 1] range: min element goes to 0, max element goes to 1
     * @param src - source data (should contains only t_floats, otherwise result in undefined)
     * @param dest - write result
     * @return true on success, false on error (zero range, empty etc.)
     */
    bool normalizeByRange(const AtomListView& src, AtomList& dest);

    /**
     * Resample list
     * @param src - source data
     * @param dest - output data
     * @param ratio - resample ratio (src/desc)
     * @return treu on success, false on error
     */
    bool resample(const AtomListView& src, AtomList& dest, t_float ratio);

    /**
     * Return vector of types containing in lst, elements of other types are discarded
     */
    template <typename T>
    std::vector<T> extractByType(const AtomListView& lv)
    {
        std::vector<T> res;
        res.reserve(lv.size());

        for (const Atom& a : lv) {
            if (a.isA<T>())
                res.push_back(a.asT<T>());
        }

        return res;
    }

    enum enumerateMode {
        PREPEND,
        APPEND
    };

    /**
     * Generate euclidean rhythm pattern using bresenham line algorithm
     * @see https://en.wikipedia.org/wiki/Euclidean_rhythm
     * @see https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
     * @param onsets- number of onsets, should be <= pulses
     * @param pulses - pattern length, should be >0
     * @note caller should check params
     * @return list of 1 and 0
     */
    AtomList bresenham(size_t onsets, size_t pulses);

    /**
     * Generate rhythm pattern based on hex value bits
     * @param str - hex string with options 0[Xx] prefix
     * @return list of 1 and 0
     */
    AtomList hexbeat_bin(const char* str);

    /**
     * Generate rhythm pattern based on hex value bits
     * @param str - hex string with options 0[Xx] prefix
     * @return list of beat durations
     */
    bool hexbeat_dur(const char* str, AtomList& res, int& shift);

    AtomList lsystem(const AtomListView& state, const DataTypeDict& rules);

    AtomList enumerate(const AtomListView& lv, int from = 0, enumerateMode mode = PREPEND);

    // preserves element order but not suitable for data atoms
    AtomList uniqueStable(const AtomListView& lv);
    // suitable for data atoms, but returns sorted output
    AtomList uniqueSorted(const AtomListView& lv);
    AtomList shift(const AtomListView& lv, t_float off);
    AtomList stretch(const AtomListView& lv, size_t sz);

    std::vector<std::pair<Atom, size_t>> rleEncode(const AtomListView& lv);
    AtomList rleDecode(const std::vector<std::pair<Atom, size_t>>& rle);

    template <typename T>
    static bool canConvertToType(const AtomListView&) { return false; }

    template <>
    bool canConvertToType<bool>(const AtomListView& lv)
    {
        if (lv.size() != 1)
            return false;

        if (lv[0].isFloat())
            return true;

        if (lv[0].isSymbol()
            && (lv[0].asSymbol() == gensym("true") || lv[0].asSymbol() == gensym("false")))
            return true;

        return false;
    }

    template <>
    bool canConvertToType<float>(const AtomListView& lv)
    {
        return lv.isFloat();
    }

    template <>
    bool canConvertToType<double>(const AtomListView& lv)
    {
        return lv.isFloat();
    }

    template <>
    bool canConvertToType<int>(const AtomListView& lv)
    {
        return lv.isFloat();
    }

    template <>
    bool canConvertToType<size_t>(const AtomListView& lv)
    {
        if (!lv.isFloat())
            return false;

        if (lv[0].asFloat() < 0)
            return false;

        return true;
    }

    template <>
    bool canConvertToType<t_symbol*>(const AtomListView& lv)
    {
        return lv.isSymbol();
    }

    template <>
    bool canConvertToType<Atom>(const AtomListView& lv)
    {
        return lv.size() == 1;
    }

    template <>
    bool canConvertToType<AtomList>(const AtomListView&)
    {
        return true;
    }

    using ViewSlice = std::pair<AtomListView, AtomListView>;

    /**
     * Search property in given list
     * @param lv - searched list
     * @return view of first found property and remaining props view
     * @note no dynamic memory allocation
     */
    ViewSlice findProperty(const AtomListView& lv);

    /**
     * Iterate all properties in list
     * @param lv - source list
     * @param fn - function to apply to each found property
     * @note no dynamic memory allocation
     */
    int foreachProperty(const AtomListView& lv, std::function<void(const AtomListView&)> fn);

    /**
     * Iterate all properties in list
     * @param lv - source list
     * @param fn - function to apply to each found property
     * @note no dynamic memory allocation
     */
    int foreachProperty(const AtomListView& lv, std::function<void(t_symbol* prop, const AtomListView&)> fn);

    /**
     * Linear list interpoltation
     * @param l0 - first list
     * @param l1 - second list
     * @param k - interpolation factor [0-1]
     * @return interpolated list of size min(l0.size(), l1.size())
     */
    AtomList interpolate_lin(const AtomListView& l0, const AtomListView& l1, t_float k, AtomList::NonEqualLengthBehaivor mode = AtomList::MINSIZE);
}
}

#endif // CEAMMC_EXT_LIST_H
