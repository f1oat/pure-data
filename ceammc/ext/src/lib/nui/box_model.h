// this is autogenerated file, do not edit!
#ifndef BOX_H_
#define BOX_H_
// clang-format off

#include "nui/model.h"
#include "nui/style.h"
#include "m_pd.h"

#include <tuple>

namespace ceammc {
namespace ui {

class BoxData
    : public std::tuple<Size, HexColor, HexColor, bool, Xlets, Xlets, int> {
public:
    enum Fields {
        SIZE,
        BORDER_COLOR,
        FILL_COLOR,
        SELECTED,
        INLETS,
        OUTLETS,
        STYLE_IDX,
    };
public:
    BoxData()
        : std::tuple<Size, HexColor, HexColor, bool, Xlets, Xlets, int>(Size(10, 10), colors::st_border, colors::st_fill, false, Xlets(), Xlets(), 0) { }

    BoxData(int style)
        : BoxData() {
        setStyle(style);
        loadStyle(style);
    }

    // constants
    static HexColor selectColor() noexcept { return colors::blue; }

    // getters
    Size size() const noexcept { return std::get<SIZE>(*this); }
    HexColor borderColor() const noexcept { return std::get<BORDER_COLOR>(*this); }
    HexColor fillColor() const noexcept { return std::get<FILL_COLOR>(*this); }
    bool selected() const noexcept { return std::get<SELECTED>(*this); }
    Xlets inlets() const noexcept { return std::get<INLETS>(*this); }
    Xlets outlets() const noexcept { return std::get<OUTLETS>(*this); }
    int style() const noexcept { return std::get<STYLE_IDX>(*this); }

    // setters
    void setSize(Size v) { std::get<SIZE>(*this) = v; }
    void setBorderColor(HexColor v) { std::get<BORDER_COLOR>(*this) = v; }
    void setFillColor(HexColor v) { std::get<FILL_COLOR>(*this) = v; }
    void setSelected(bool v) { std::get<SELECTED>(*this) = v; }
    void setInlets(Xlets v) { std::get<INLETS>(*this) = v; }
    void setOutlets(Xlets v) { std::get<OUTLETS>(*this) = v; }
    void setStyle(int v) { std::get<STYLE_IDX>(*this) = v; }

    // refs
    Size& sizeRef() { return std::get<SIZE>(*this); }
    HexColor& borderColorRef() { return std::get<BORDER_COLOR>(*this); }
    HexColor& fillColorRef() { return std::get<FILL_COLOR>(*this); }
    bool& selectedRef() { return std::get<SELECTED>(*this); }
    Xlets& inletsRef() { return std::get<INLETS>(*this); }
    Xlets& outletsRef() { return std::get<OUTLETS>(*this); }
    int& styleRef() { return std::get<STYLE_IDX>(*this); }

    // style
    void loadStyle(int st) {
        using sc = StyleCollection;
        setBorderColor(sc::color(st, "box:border_color"_hash, colors::st_border));
        setFillColor(sc::color(st, "box:fill_color"_hash, colors::st_fill));
    }
};

using BoxModel = ModelBase<BoxData>;

}
}

// clang-format on
#endif
