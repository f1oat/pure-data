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
#include "ceammc_timeline.h"
#include "ceammc_log.h"

#include <algorithm>
#include <string>

namespace ceammc {
namespace tl {

    CueData::CueData(t_canvas* c, t_object* o)
        : c_(c)
        , obj_(o)
        , index_(-1)
        , xpos_(0)
        , str_("cue_x")
    {
    }

    void CueData::setXPos(int x)
    {
        xpos_ = x;
    }

    void CueData::setIndex(int idx)
    {
        if (idx == index_)
            return;

        index_ = idx;
        char buf[100];
        snprintf(buf, 100, "cue_%d", idx);
        str_ = buf;
    }

    bool CueData::operator==(const CueData& c) const
    {
        return obj_ == c.obj_;
    }

    // static member init
    CueStorage::CanvasCueMap CueStorage::cue_map_;

    void CueStorage::add(CueData* c)
    {
        if (c == 0) {
            LIB_ERR << "[CueStorage::add] null pointer";
            return;
        }

        if (exists(c)) {
            LIB_ERR << "already exists in storage";
            return;
        }

        t_canvas* cnv = const_cast<t_canvas*>(c->canvas());
        cue_map_[cnv].push_back(c);

        sort(cnv);
        enumerate(cnv);
    }

    void CueStorage::remove(CueData* c)
    {
        if (c == 0)
            return;

        t_canvas* cnv = const_cast<t_canvas*>(c->canvas());
        CanvasCueMap::iterator it = cue_map_.find(cnv);
        if (it == cue_map_.end())
            return;

        CueList* lst = &it->second;

        CueList::iterator cue_it = std::find(lst->begin(), lst->end(), c);
        if (cue_it != lst->end())
            lst->erase(cue_it);

        sort(cnv);
        enumerate(cnv);

        if (lst->empty())
            cue_map_.erase(it);
    }

    int CueStorage::index(CueData* c)
    {
        CueList* lst = cueList(c);
        if (lst == 0)
            return -1;

        CueList::iterator it = std::find(lst->begin(), lst->end(), c);
        return (it == lst->end()) ? -1 : static_cast<int>(std::distance(lst->begin(), it));
    }

    CueData* CueStorage::at(t_canvas* cnv, size_t pos)
    {
        CueList* lst = cueList(cnv);
        if (lst == 0)
            return 0;

        return (pos < lst->size()) ? lst->at(pos) : 0;
    }

    struct ObjPred {
        t_object* o_;
        ObjPred(t_object* o)
            : o_(o)
        {
        }

        bool operator()(CueData* c) const { return c->object() == o_; }
    };

    CueData* CueStorage::find(t_canvas* cnv, t_object* obj)
    {
        CueList* lst = cueList(cnv);
        if (lst == 0)
            return 0;

        ObjPred pred(obj);
        CueList::iterator it = std::find_if(lst->begin(), lst->end(), pred);
        return it == lst->end() ? 0 : (*it);
    }

    bool CueStorage::exists(CueData* c)
    {
        CueList* lst = cueList(c);
        if (lst == 0)
            return false;

        return std::find(lst->begin(), lst->end(), c) != lst->end();
    }

    bool CueStorage::exists(t_canvas* cnv)
    {
        return cue_map_.find(cnv) != cue_map_.end();
    }

    static bool sort_cue_ptr(CueData* a, CueData* b)
    {
        return a->xPos() < b->xPos();
    }

    void CueStorage::sort(t_canvas* cnv)
    {
        CueList* lst = cueList(cnv);
        if (lst == 0)
            return;

        std::sort(lst->begin(), lst->end(), sort_cue_ptr);
    }

    void CueStorage::enumerate(t_canvas* cnv)
    {
        CueList* lst = cueList(cnv);
        if (lst == 0)
            return;

        CueList::iterator it;
        int i = 0;
        for (it = lst->begin(); it != lst->end(); ++it) {
            (*it)->setIndex(i++);
        }
    }

    size_t CueStorage::cueCount(t_canvas* cnv)
    {
        CueList* lst = cueList(cnv);
        return lst == 0 ? 0 : lst->size();
    }

    CueList* CueStorage::cueList(t_canvas* c)
    {
        CanvasCueMap::iterator it = cue_map_.find(c);
        return it == cue_map_.end() ? 0 : &it->second;
    }

    CueList* CueStorage::cueList(CueData* c)
    {
        if (c == 0)
            return 0;

        return cueList(const_cast<t_canvas*>(c->canvas()));
    }
}
}
