#include "canvas_top.h"
#include "ceammc_property_callback.h"
#include "ceammc_canvas.h"
#include "ceammc_factory.h"
#include "datatype_dict.h"

CanvasTop::CanvasTop(const PdArgs& a)
    : BaseObject(a)
{
    createOutlet();

    createCbProperty("@name", &CanvasTop::p_name);
    createCbProperty("@dir", &CanvasTop::p_dir);
    createCbProperty("@font", &CanvasTop::p_font);
    createCbProperty("@paths", &CanvasTop::p_paths);
    createCbProperty("@size", &CanvasTop::p_size);
    createCbProperty("@x", &CanvasTop::p_x);
    createCbProperty("@y", &CanvasTop::p_y);
    createCbProperty("@width", &CanvasTop::p_width);
    createCbProperty("@height", &CanvasTop::p_height);
}

void CanvasTop::onBang()
{
    DataTypeDict* dict = new DataTypeDict;

    dict->insert("name", p_name());
    dict->insert("dir", p_dir());
    dict->insert("font", p_font());
    dict->insert("paths", p_paths());
    dict->insert("size", p_size());
    dict->insert("x", p_x());
    dict->insert("y", p_y());
    dict->insert("width", p_width());
    dict->insert("height", p_height());

    dataTo(0, DataPtr(dict));
}

AtomList CanvasTop::p_name() const
{
    return Atom(canvas_info_name(rootCanvas()));
}

AtomList CanvasTop::p_dir() const
{
    return Atom(canvas_info_dir(rootCanvas()));
}

AtomList CanvasTop::p_font() const
{
    return Atom(canvas_info_font(rootCanvas()));
}

AtomList CanvasTop::p_paths() const
{
    return canvas_info_paths(rootCanvas());
}

AtomList CanvasTop::p_size() const
{
    t_rect bb = canvas_info_rect(rootCanvas());
    return AtomList(Atom(bb.w), Atom(bb.h));
}

AtomList CanvasTop::p_x() const
{
    return Atom(canvas_info_rect(rootCanvas()).x);
}

AtomList CanvasTop::p_y() const
{
    return Atom(canvas_info_rect(rootCanvas()).y);
}

AtomList CanvasTop::p_width() const
{
    return Atom(canvas_info_rect(rootCanvas()).w);
}

AtomList CanvasTop::p_height() const
{
    return Atom(canvas_info_rect(rootCanvas()).h);
}

void setup_canvas_top()
{
    ObjectFactory<CanvasTop> obj("canvas.top");
}
