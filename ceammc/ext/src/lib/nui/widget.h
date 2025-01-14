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
#ifndef WIDGET_H
#define WIDGET_H

#include <array>
#include <cstdint>
#include <type_traits>

#include "ceammc_object.h"
#include "nui/common.h"
#include "nui/property.h"
#include "nui/rect.h"

namespace ceammc {
namespace ui {

    namespace utils {
        int canvas_zoom(const t_glist* c);
        bool canvas_is_visible(const t_glist* c);
        bool canvas_is_edit(const t_glist* c);
        void canvas_delete_object_lines(const t_glist* c, t_text* x);
        void canvas_update_object_lines(const t_glist* c, t_text* x);
        void canvas_motion(t_glist* c, t_object* obj, int mode);
        void canvas_down(t_glist* c, t_object* obj, int mode);
        void canvas_up(t_glist* c, t_object* obj);
        void canvas_right(t_glist* c, t_object* obj);
        Point object_pos(t_text* x);
        Point object_abs_pos(t_text* x, const t_glist* parent);
        void object_move(t_text* x, int dx, int dy);
        void object_bind(t_text* x);
        void object_open_help(t_text* x);
        bool is_toplevel(t_glist* x);
        t_glist* object_get_draw_canvas(t_glist* c);
        void widget_bind_mouse(t_glist* c, t_object* obj, UIFactoryFlags flags);
        void widget_bind_drag_and_drop(t_glist* c, t_object* obj, UIFactoryFlags flags);
        void widget_create(t_glist* c, t_object* obj, const Point& pos, const Size& sz, int zoom);
        void widget_erase(t_glist* c, t_object* obj);
        void widget_focus(t_glist* c, t_object* obj);
        void widget_move(t_glist* c, t_object* obj, const Point& pos);
        void widget_resize(t_glist* c, t_object* obj, const Size& sz, int zoom);
        bool is_platform_control(uint32_t mod);
        void set_cursor(t_glist* c, t_object* x, CursorFlags cursor);
        int object_outlet_at_pos(const Point& pos, const Size& bbox, size_t nout, int zoom);
    }

    template <typename T>
    class Widget : public T {
        ui::SizeProperty* size_;
        std::vector<Property*> widget_props_;
        UIFactoryFlags ui_flags_ { UI_FACTORY_NONE };
        t_glist* draw_canvas_ { nullptr };
        SelectionType selection_ { SELECT_NONE };
        CursorFlags cursor_ = { CURSOR_LEFT_PTR };
        ResizeMode resize_mode_ = { RESIZE_NONE };
        bool mouse_down_ = { false };
        bool top_level_ { true };

    public:
        Widget(const PdArgs& args)
            : T(args)
            , size_(nullptr)
        {
            static_assert(std::is_base_of<BaseObject, T>::value, "");

            draw_canvas_ = T::canvas();

            size_ = new ui::SizeProperty("@size", { 10, 10 });
            size_->setSuccessFn([this](Property*) { resizeWidget(size_->value()); });
            T::addProperty(size_);
        }

        void initDone() override
        {
            T::initDone();
            buildUI();
        }

        void setWidgetFlags(UIFactoryFlags flags)
        {
            ui_flags_ = flags;
            subscribeMouseEvents();
        }

        Property* addWidgetProperty(Property* p)
        {
            T::addProperty(p);
            widget_props_.push_back(p);
            p->setSuccessFn([this](Property* p) { notifyPropChange(p); });
            return p;
        }

        std::vector<Property*>& widgetProperties() { return widget_props_; }
        const std::vector<Property*>& widgetProperties() const { return widget_props_; }

        Point absPos() const { return utils::object_abs_pos(T::owner(), T::canvas()); }
        const Size& size() const { return size_->value(); }
        void setSize(const Size& sz) { size_->setValue(fixNewSize(sz)); }
        Size viewSize() const { return size() * zoom(); }
        int zoom() const { return utils::canvas_zoom(T::canvas()); }
        bool isVisible() const { return utils::canvas_is_visible(T::canvas()); }
        bool isEdit() const { return utils::canvas_is_edit(drawCanvas()); }
        CursorFlags cursor() const { return cursor_; }
        void setCursor(CursorFlags cursor)
        {
            if (cursor_ != cursor) {
                cursor_ = cursor;
                utils::set_cursor(drawCanvas(), T::owner(), cursor_);
            }
        }

        ResizeMode resizeMode() const { return resize_mode_; }
        void setResizeMode(ResizeMode mode) { resize_mode_ = mode; }

        t_glist* drawCanvas() const { return draw_canvas_; }
        void syncDrawCanvas() { draw_canvas_ = utils::object_get_draw_canvas(T::canvas()); }

        Rect viewBBox(t_glist* owner) const { return Rect(absPos(), viewSize()); }

        virtual Size fixNewSize(const Size& sz) { return sz.clippedMin({ 10, 10 }); }

        virtual void onWidgetActivation(bool state)
        { /*LIB_ERR << __FUNCTION__;*/
        }
        virtual void onWidgetMove(int dx, int dy)
        { /*LIB_ERR << __FUNCTION__;*/
        }
        virtual void onWidgetDelete()
        { /*LIB_ERR << __FUNCTION__;*/
        }
        virtual void onWidgetSelect(bool state)
        { /*LIB_ERR << __FUNCTION__;*/
        }
        virtual void onWidgetShow()
        { /*LIB_ERR << __FUNCTION__;*/
        }

        /**
         * called right after widget resize
         * @param sz - new widget size in model coordinates (without zoom)
         */
        virtual void onWidgetResize(const Size& sz)
        { /*LIB_ERR << __FUNCTION__ << ' ' << sz;*/
        }
        virtual void onWidgetHide()
        { /*LIB_ERR << __FUNCTION__;*/
        }

        virtual void activateWidget(t_glist* owner, bool state)
        {
            onWidgetActivation(state);
        }

        virtual void displaceWidget(t_glist* owner, int dx, int dy)
        {
            utils::object_move(T::owner(), dx, dy);

            if (isVisible()) {
                const Point norm_pos = absPos() / zoom();
                utils::widget_move(drawCanvas(), T::owner(), absPos());
                utils::canvas_update_object_lines(drawCanvas(), T::owner());
                onWidgetMove(dx, dy);
            }
        }

        virtual void deleteWidget(t_glist* owner)
        {
            syncDrawCanvas();
            utils::canvas_delete_object_lines(drawCanvas(), T::owner());
            onWidgetDelete();
        }

        virtual void selectWidget(t_glist* owner, bool state)
        {
            onWidgetSelect(state);
        }

        virtual void showWidget(t_glist* owner)
        {
            top_level_ = utils::is_toplevel(owner);
            syncDrawCanvas();
            utils::widget_create(drawCanvas(), T::owner(), absPos(), size(), zoom());
            utils::widget_bind_mouse(drawCanvas(), T::owner(), ui_flags_);
            utils::widget_bind_drag_and_drop(drawCanvas(), T::owner(), ui_flags_);
            onWidgetShow();
        }

        virtual void resizeWidget(const Size& sz)
        {
            if (resize_mode_ == RESIZE_NONE)
                return;

            setSize(sz / zoom());
            utils::widget_resize(drawCanvas(), T::owner(), size(), zoom());
            onWidgetResize(size());
        }

        virtual void hideWidget(t_glist* owner)
        {
            syncDrawCanvas();
            utils::widget_erase(drawCanvas(), T::owner());
            onWidgetHide();
        }

        virtual void buildUI()
        {
            // view controller
        }

        virtual void notifyPropChange(Property* p)
        {
        }

        virtual void onMouseEnter()
        { /*LIB_ERR << __FUNCTION__;*/
        }
        virtual void onMouseLeave()
        { /*LIB_ERR << __FUNCTION__;*/
        }
        virtual void onMouseMove()
        { /*LIB_ERR << __FUNCTION__;*/
        }
        virtual void onMouseDrag(const Point& pt, uint32_t mod)
        { /*LIB_ERR << __FUNCTION__;*/
        }
        virtual void onMouseDown(const Point& pt, const Point& abspt, uint32_t mod)
        { /*LIB_ERR << __FUNCTION__;*/
        }
        virtual void onMouseUp(const Point& pt, uint32_t mod)
        { /*LIB_ERR << __FUNCTION__;*/
        }

        virtual void onRightClick(const Point& pt, const Point& abspt, uint32_t mod)
        {
            // LIB_ERR << __FUNCTION__;
            utils::canvas_right(drawCanvas(), T::owner());
        }

        virtual void onDropFiles(const AtomListView& lv)
        { /*LIB_ERR << __FUNCTION__;*/
        }

        virtual void onDropText(const AtomListView& lv)
        { /*LIB_ERR << __FUNCTION__;*/
        }

        void mouseEnter()
        {
            syncDrawCanvas();

            if (!isEdit() && !mouse_down_) {
                utils::widget_focus(drawCanvas(), T::owner());
                onMouseEnter();
            }
        }

        void mouseLeave()
        {
            syncDrawCanvas();

            if (!mouse_down_) {
                if (isEdit()) {
                    setCursor(CURSOR_HAND);
                } else {
                    onMouseLeave();
                    setCursor(CURSOR_LEFT_PTR);
                }
            }
        }

        void mouseDown(const Point& pt, const Point& abspt, uint32_t mod)
        {
            if (editModeAccept(mod)) {
                onMouseDown(pt / zoom(), abspt, mod);
            } else {
                if (selection_ == SELECT_NONE) {
                    if (mod & KEY_MOD_SHIFT) {
                        utils::canvas_down(drawCanvas(), T::owner(), 1);
                    } else if (mod & KEY_MOD_RIGHT) {
                        utils::canvas_right(drawCanvas(), T::owner());
                    } else {
                        utils::canvas_down(drawCanvas(), T::owner(), 0);
                    }
                }
            }

            mouse_down_ = true;
        }

        void mouseUp(const Point& pt, uint32_t mod)
        {
            if (editModeAccept(mod)) {
                onMouseUp(pt / zoom(), mod);
            } else {
                utils::canvas_up(drawCanvas(), T::owner());
            }

            mouse_down_ = false;
        }

        void mouseMove(const Point& pt, uint32_t mod)
        {
            const int CURSOR_AREA = 3 * zoom();

            if (mouse_down_) { // mouse drag
                if (editModeAccept(mod)) {
                    onMouseDrag(pt / zoom(), mod);
                } else if (top_level_) {
                    if (selection_ == SELECT_NONE) {
                        return utils::canvas_motion(drawCanvas(), T::owner(), 0);
                    } else if (resize_mode_ == RESIZE_BOTH) {
                        switch (selection_) {
                        case SELECT_BOTTOM:
                            resizeWidget(Size(size_->width(), pt.y()));
                            break;
                        case SELECT_RIGHT:
                            resizeWidget(Size(pt.x(), size_->height()));
                            break;
                        case SELECT_CORNER:
                            resizeWidget(Size(pt.x(), pt.y()));
                            break;
                        default:
                            break;
                        }
                    } else if (resize_mode_ == RESIZE_HEIGHT) {
                        switch (selection_) {
                        case SELECT_CORNER:
                        case SELECT_BOTTOM:
                            resizeWidget(Size(size_->width(), pt.y()));
                            break;
                        default:
                            break;
                        }
                    } else if (resize_mode_ == RESIZE_WIDTH) {
                        switch (selection_) {
                        case SELECT_CORNER:
                        case SELECT_RIGHT:
                            resizeWidget(Size(pt.x(), size_->height()));
                            break;
                        default:
                            break;
                        }
                    }
                } else {
                    return utils::canvas_motion(drawCanvas(), T::owner(), 1);
                }
            } else { // mouse move
                if (editModeAccept(mod)) {
                    onMouseMove();
                } else if (top_level_) {
                    selection_ = SELECT_NONE;

                    utils::canvas_motion(drawCanvas(), T::owner(), 0);

                    const auto vsz = viewSize();
                    if (vsz.nearRightBottom(pt, CURSOR_AREA) && canResizeBoth()) {
                        selection_ = SELECT_CORNER;
                        setCursor(CURSOR_RIGHT_CORNER);
                        return;
                    } else if (vsz.nearBottomSide(pt, CURSOR_AREA) && canResizeHeight()) {
                        const auto N = T::numOutlets();
                        auto i = utils::object_outlet_at_pos(pt, vsz, N, zoom());
                        if (i >= 0) {
                            setCursor(CURSOR_CIRCLE);
                            return;
                        } else {
                            selection_ = SELECT_BOTTOM;
                            setCursor(CURSOR_BOTTOM);
                        }

                    } else if (vsz.nearRightSide(pt, CURSOR_AREA) && canResizeWidth()) {
                        selection_ = SELECT_RIGHT;
                        setCursor(CURSOR_RIGHT_SIDE);
                        return;
                    } else {
                        setCursor(CURSOR_HAND);
                    }
                } else {
                    utils::canvas_motion(drawCanvas(), T::owner(), 0);
                }
            }
        }

        void mouseRight(const Point& pt, const Point& abspt, uint32_t mod)
        {
            // show help menu in all modes (edit/performance) with pressed Shift
            if (mod & KEY_MOD_SHIFT)
                return openHelp();

            // show object properties in all modes (edit/performance) with pressed Alt
            if (mod & KEY_MOD_ALT)
                return openProperties();

            if (isEdit()) {
                // in edit mode show standart Pd popup
                utils::canvas_right(drawCanvas(), T::owner());
            } else {
                onRightClick(pt, abspt, mod);
            }
        }

        void dragAndDropFiles(const AtomListView& lv)
        {
            onDropFiles(lv);
        }

        void dragAndDropText(const AtomListView& lv)
        {
            onDropText(lv);
        }

        void openHelp()
        {
            utils::object_open_help(T::owner());
        }

        void openProperties()
        {
            // LIB_ERR << __FUNCTION__;
            // return ebox_properties(x, nullptr);
        }

    private:
        void subscribeMouseEvents()
        {
            utils::object_bind(T::owner());
        }

        bool editModeAccept(uint32_t mod) const
        {
            return !isEdit() || utils::is_platform_control(mod);
        }

        bool canResizeWidth() const
        {
            return resize_mode_ == RESIZE_BOTH
                || resize_mode_ == RESIZE_LINKED
                || resize_mode_ == RESIZE_WIDTH;
        }

        bool canResizeHeight() const
        {
            return resize_mode_ == RESIZE_BOTH
                || resize_mode_ == RESIZE_LINKED
                || resize_mode_ == RESIZE_HEIGHT;
        }

        bool canResizeBoth() const
        {
            return resize_mode_ == RESIZE_BOTH
                || resize_mode_ == RESIZE_LINKED;
        }
    };
}
}

#endif // WIDGET_H
