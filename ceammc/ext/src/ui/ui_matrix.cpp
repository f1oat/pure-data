#include "ui_matrix.h"
#include "ceammc_convert.h"
#include "ceammc_format.h"
#include "ceammc_preset.h"
#include "ceammc_ui.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <limits>
#include <random>

static t_symbol* SYM_CELL = gensym("cell");
static t_symbol* SYM_ROW = gensym("row");
static t_symbol* SYM_COL = gensym("col");
static t_symbol* SYM_LIST = gensym("list");
static t_symbol* SYM_OUTPUT_ALL_COLS = gensym("cols");
static t_symbol* SYM_OUTPUT_ALL_ROWS = gensym("rows");
static t_symbol* SYM_OUTPUT_ALL_CELLS = gensym("cells");

#define CELL_TAG_FMT "cell_#%x_%d_%d"
#define ALL_CELLS_TAG_FMT "cells_#%x"

static const int CELL_MARGIN = 1;

UIMatrix::UIMatrix()
    : matrix_()
    , update_all_cells_(false)
    , reset_all_cells_(false)
    , prop_color_active_(rgba_black)
    , prop_color_current_(rgba_black)
    , prop_rows_(4)
    , prop_cols_(8)
    , mouse_current_col_(-1)
    , mouse_current_row_(-1)
    , current_col_(-1)
    , current_row_(-1)
    , cells_are_created_(false)
{
    static_assert(std::numeric_limits<CellIdxT>::max() > UI_MAX_MATRIX_SIZE, "check cell index type");

    createOutlet();
}

void UIMatrix::init(t_symbol* name, const AtomList& args, bool usePresets)
{
    UIObject::init(name, args, usePresets);

    int row = args.intAt(0, -1);
    int col = args.intAt(1, -1);

    if (row > 0 && col > 0) {
        prop_cols_ = clip<int>(col, 1, UI_MAX_MATRIX_SIZE);
        prop_rows_ = clip<int>(row, 1, UI_MAX_MATRIX_SIZE);
        int cell_wd = std::max<int>(prop_cols_, prop_rows_) > 8 ? 7 : 10;

        asEBox()->b_rect.width = prop_cols_ * (cell_wd + 2) + 1 * zoom();
        asEBox()->b_rect.height = prop_rows_ * (cell_wd + 2) + 1 * zoom();
    }
}

bool UIMatrix::cell(size_t row, size_t col) const
{
    row %= UI_MAX_MATRIX_SIZE;
    col %= UI_MAX_MATRIX_SIZE;
    return matrix_[row * UI_MAX_MATRIX_SIZE + col];
}

void UIMatrix::setCell(const AtomList& lst)
{
    if (lst.size() != 3) {
        UI_ERR << "usage set cell ROW COL VALUE";
        return;
    }

    int row = lst[0].asInt(-1);
    int col = lst[1].asInt(-1);
    int v = lst[2].asInt(0);

    if (row < 0 || row >= prop_rows_ || col < 0 || col >= prop_cols_) {
        UI_ERR << "invalid indexes: " << lst;
        return;
    }

    setCell(row, col, v);
}

AtomList UIMatrix::column(size_t col) const
{
    AtomList res;
    res.reserve(prop_rows_);

    for (int i = 0; i < prop_rows_; i++)
        res.append(Atom(cell(i, col) ? 1 : 0));

    return res;
}

void UIMatrix::setColumn(size_t col, const AtomList& lst)
{
    size_t t = std::min<size_t>(lst.size(), prop_rows_);
    for (size_t row = 0; row < t; row++)
        setCell(row, col, lst[row].asFloat() != 0);
}

void UIMatrix::setColumn(const AtomList& lst)
{
    if (lst.size() < 2) {
        UI_ERR << "column index and values are expected: " << lst;
        return;
    }

    int idx = lst[0].asInt(-1);

    if (idx < 0 || idx >= prop_cols_) {
        UI_ERR << "invalid column index: " << idx;
        return;
    }

    setColumn(idx, lst.slice(1));
}

AtomList UIMatrix::row(size_t idx) const
{
    AtomList res;
    res.fill(Atom(0.f), prop_cols_);

    for (int i = 0; i < prop_cols_; i++) {
        if (cell(idx, i))
            res[i] = Atom(1);
    }

    return res;
}

void UIMatrix::setRow(size_t row, const AtomList& lst)
{
    size_t t = std::min<size_t>(lst.size(), prop_cols_);

    for (size_t col = 0; col < t; col++)
        setCell(row, col, lst[col].asFloat() != 0);
}

void UIMatrix::setRow(const AtomList& lst)
{
    if (lst.size() < 2) {
        UI_ERR << "row index and values are expected: " << lst;
        return;
    }

    int idx = lst[0].asInt(-1);

    if (idx < 0 || idx >= prop_rows_) {
        UI_ERR << "invalid row index: " << idx;
        return;
    }

    setRow(idx, lst.slice(1));
}

void UIMatrix::setList(const AtomList& lst)
{
    if (lst.size() != prop_cols_ * prop_rows_)
        UI_DBG << "warning: list size is not equal number of matrix cells";

    const size_t total = std::min<size_t>(lst.size(), prop_cols_ * prop_rows_);
    for (size_t i = 0; i < total; i++) {
        size_t col = i % prop_cols_;
        size_t row = i / prop_cols_;

        setCell(row, col, lst[i].asFloat() != 0);
    }
}

void UIMatrix::flipCell(size_t row, size_t col)
{
    matrix_.flip(row * UI_MAX_MATRIX_SIZE + col);
    addToUpdateList(row, col);
}

void UIMatrix::flipColumn(size_t col)
{
    for (int row = 0; row < prop_rows_; row++) {
        matrix_.flip(row * UI_MAX_MATRIX_SIZE + col);
        addToUpdateList(row, col);
    }
}

void UIMatrix::flipRow(size_t row)
{
    for (int col = 0; col < prop_cols_; col++) {
        matrix_.flip(row * UI_MAX_MATRIX_SIZE + col);
        addToUpdateList(row, col);
    }
}

void UIMatrix::flipAll()
{
    matrix_.flip();
    update_all_cells_ = true;
}

AtomList UIMatrix::asList() const
{
    AtomList res;
    res.fill(Atom(0.f), prop_cols_ * prop_rows_);

    for (size_t r = 0; r < prop_rows_; r++) {
        for (size_t c = 0; c < prop_cols_; c++) {
            if (cell(r, c))
                res[r * prop_cols_ + c] = Atom(1);
        }
    }

    return res;
}

void UIMatrix::okSize(t_rect* newrect)
{
    if (!isPatchLoading()) {
        newrect->width = pd_clip_min(newrect->width, prop_cols_ * 6);
        newrect->height = pd_clip_min(newrect->height, prop_rows_ * 6);

        float cell_w = std::floor(newrect->width / prop_cols_);
        float cell_h = std::floor(newrect->height / prop_rows_);

        newrect->width = cell_w * prop_cols_ + 1;
        newrect->height = cell_h * prop_rows_ + 1;
    } else {
        newrect->width = pd_clip_min(newrect->width, 31);
        newrect->height = pd_clip_min(newrect->height, 31);
    }
}

void UIMatrix::drawActiveCells()
{
    if (!asEBox() || !asEBox()->b_drawing_id)
        return;

    int color_inactive = rgba_to_hex_int(prop_color_background);
    int color_active = rgba_to_hex_int(prop_color_active_);

    for (const CellList::value_type& c : cell_update_list_) {
        int col = c.second;
        int row = c.first;
        sys_vgui("%s itemconfigure " CELL_TAG_FMT " -fill #%6.6x\n",
            asEBox()->b_drawing_id->s_name,
            asEBox(),
            col, row,
            cell(row, col)
                ? color_active
                : color_inactive);
    }

    cell_update_list_.clear();

    // update all cells on flag set (on total flip)
    if (update_all_cells_) {
        for (size_t col = 0; col < prop_cols_; col++) {
            for (size_t row = 0; row < prop_rows_; row++) {
                sys_vgui("%s itemconfigure " CELL_TAG_FMT " -fill #%6.6x\n",
                    asEBox()->b_drawing_id->s_name,
                    asEBox(),
                    col, row,
                    cell(row, col)
                        ? color_active
                        : color_inactive);
            }
        }

        update_all_cells_ = false;
    }

    // reset only active cells
    if (reset_all_cells_) {
        for (size_t col = 0; col < prop_cols_; col++) {
            for (size_t row = 0; row < prop_rows_; row++) {
                if (!cell(row, col))
                    continue;

                sys_vgui("%s itemconfigure " CELL_TAG_FMT " -fill #%6.6x\n",
                    asEBox()->b_drawing_id->s_name,
                    asEBox(),
                    col, row,
                    color_inactive);
            }
        }

        reset_all_cells_ = false;
    }
}

void UIMatrix::createCells()
{
    const int w = cellWidth();
    const int h = cellHeight();
    const int color_inactive = rgba_to_hex_int(prop_color_background);
    const int color_active = rgba_to_hex_int(prop_color_active_);

    if (!cells_are_created_ && asEBox() && asEBox()->b_drawing_id) {
        int color_cell_border = rgba_to_hex_int(prop_color_border);
        for (int inc_x = CELL_MARGIN, col = 0; col < prop_cols_; col++, inc_x += w) {
            for (int inc_y = CELL_MARGIN, row = 0; row < prop_rows_; row++, inc_y += h) {
                int x0 = inc_x + CELL_MARGIN;
                int y0 = inc_y + CELL_MARGIN;
                int x1 = inc_x + w - CELL_MARGIN;
                int y1 = inc_y + h - CELL_MARGIN;
                sys_vgui("%s create rectangle %d %d %d %d -outline #%6.6x -fill #%6.6x"
                         " -tags { " CELL_TAG_FMT " " ALL_CELLS_TAG_FMT " }\n",
                    asEBox()->b_drawing_id->s_name,
                    x0, y0, x1, y1,
                    color_cell_border,
                    cell(row, col)
                        ? color_active
                        : color_inactive,
                    asEBox(),
                    col, row,
                    asEBox());
            }
        }

        cells_are_created_ = true;
    }
}

void UIMatrix::eraseCells()
{
    if (cells_are_created_ && asEBox() && asEBox()->b_drawing_id) {
        sys_vgui("%s delete " ALL_CELLS_TAG_FMT "\n",
            asEBox()->b_drawing_id->s_name, asEBox());
    }

    cells_are_created_ = false;
}

std::pair<int, int> UIMatrix::cellAt(const t_pt& pt)
{
    std::pair<int, int> res;
    res.first = -1;
    res.second = -1;

    res.first = clip<int>(std::floor((pt.x - 2 * CELL_MARGIN) / cellWidth()), 0, prop_cols_ - 1);
    res.second = clip<int>(std::floor((pt.y - 2 * CELL_MARGIN) / cellHeight()), 0, prop_rows_ - 1);
    return res;
}

void UIMatrix::paint()
{
    assert(prop_cols_ > 0);
    assert(prop_rows_ > 0);

    const auto r = rect();
    UIPainter p = bg_layer_.painter(r);

    if (p) {
        const int cell_w = cellWidth();
        const int cell_h = cellHeight();

        const t_rgba cc = rgba_addContrast(prop_color_background, -0.45f);

        // column selection
        if (current_col_ >= 0 && current_col_ < prop_cols_) {
            p.setColor(cc);
            p.setLineWidth(3);
            p.drawRect(current_col_ * cell_w, 0, cell_w, r.height);
            p.stroke();
        }

        if (current_row_ >= 0 && current_row_ < prop_rows_) {
            p.setColor(cc);
            p.setLineWidth(1);
            p.drawRect(0, current_row_ * cell_h, r.width, cell_h);
            p.stroke();
        }

        createCells();
    }
}

void UIMatrix::outputCell(size_t row, size_t col)
{
    AtomList args;
    args.reserve(3);
    args.append(row);
    args.append(col);
    args.append(cell(row, col) ? 1 : 0);

    anyTo(0, SYM_CELL, args);
    send(SYM_CELL, args);
}

void UIMatrix::outputCell(const AtomList& args)
{
    if (args.size() != 2) {
        UI_ERR << "usage: get cell ROW COL";
        return;
    }

    int row = args[0].asInt(-1);
    int col = args[1].asInt(-1);

    if (row < 0 || row >= prop_rows_ || col < 0 || col >= prop_cols_) {
        UI_ERR << "indexes are out of range: " << args;
        return;
    }

    outputCell(row, col);
}

void UIMatrix::outputCol(size_t col)
{
    AtomList lst;
    lst.reserve(prop_rows_ + 1);
    lst.append(Atom(col));
    lst.append(column(col));
    anyTo(0, SYM_COL, lst);
    send(SYM_COL, lst);
}

void UIMatrix::outputCol(const AtomList& args)
{
    if (args.empty()) {
        UI_ERR << "column index expected";
        return;
    }

    int idx = args[0].asInt(-1);

    if (idx < 0 || idx >= prop_cols_) {
        UI_ERR << "invalid column index: " << idx;
        return;
    }

    outputCol(size_t(idx));
}

void UIMatrix::outputRow(size_t idx)
{
    AtomList lst;
    lst.reserve(prop_cols_ + 1);
    lst.append(Atom(idx));
    lst.append(row(idx));
    anyTo(0, SYM_ROW, lst);
    send(SYM_ROW, lst);
}

void UIMatrix::outputRow(const AtomList& args)
{
    if (args.empty()) {
        UI_ERR << "row index expected";
        return;
    }

    int idx = args[0].asInt(-1);

    if (idx < 0 || idx >= prop_rows_) {
        UI_ERR << "invalid row index: " << idx;
        return;
    }

    outputRow(size_t(idx));
}

void UIMatrix::outputAllCols()
{
    for (size_t col = 0; col < prop_cols_; col++)
        outputCol(col);
}

void UIMatrix::outputAllRows()
{
    for (size_t i = 0; i < prop_rows_; i++)
        outputRow(i);
}

void UIMatrix::outputAllCells()
{
    for (size_t row = 0; row < prop_rows_; row++) {
        for (size_t col = 0; col < prop_cols_; col++)
            outputCell(row, col);
    }
}

void UIMatrix::onBang()
{
    outputAllCells();
}

void UIMatrix::onList(const AtomList& lst)
{
    setList(lst);
    outputAllCells();
}

void UIMatrix::onMouseDown(t_object* view, const t_pt& pt, const t_pt& abs_pt, long modifiers)
{
    if (modifiers == EMOD_RIGHT) {
        UIPopupMenu menu(asEObj(), "menu", abs_pt);
        menu.addItem(_("reset"));
        menu.addItem(_("flip"));
        menu.addItem(_("random"));
        menu.addSeparator();
        menu.addItem(_("load"));
        menu.addItem(_("save"));
        return;
    }

    auto c = cellAt(pt);
    mouse_current_col_ = c.first;
    mouse_current_row_ = c.second;

    if (mouse_current_col_ >= 0 && mouse_current_row_ >= 0) {
        flipCell(mouse_current_row_, mouse_current_col_);
        outputCell(mouse_current_row_, mouse_current_col_);
        drawActiveCells();
    }
}

void UIMatrix::onMouseDrag(t_object* view, const t_pt& pt, long modifiers)
{
    auto c = cellAt(pt);
    int col = c.first;
    int row = c.second;

    //    if (abs(int(pt.x) - col * cell_w) < 3)
    //        return;

    //    if (abs(int(pt.y) - col * cell_h) < 3)
    //        return;

    if (col != mouse_current_col_ || row != mouse_current_row_) {
        mouse_current_col_ = col;
        mouse_current_row_ = row;

        if (row >= 0 && col >= 0) {
            flipCell(row, col);
            outputCell(row, col);
            drawActiveCells();
        }
    }
}

void UIMatrix::onMouseLeave(t_object*, const t_pt&, long)
{
    mouse_current_col_ = -1;
    mouse_current_row_ = -1;
}

void UIMatrix::read(const std::string& fname)
{
    std::ifstream ifs(fname.c_str());
    if (!ifs) {
        UI_ERR << "can't open file " << quote(fname);
        return;
    }

    int row = 0;
    for (std::string line; std::getline(ifs, line) && (row < prop_rows_); row++) {
        std::string::size_type from = 0;
        for (int col = 0; col < prop_cols_ && from < line.size(); col++) {
            auto to = line.find(',', from);

            try {
                if (to == std::string::npos) {
                    float v = std::stof(line.substr(from));
                    setCell(row, col, v != 0);
                    // last value in column - break inner loop
                    break;
                } else {
                    float v = std::stof(line.substr(from, (to - from)));
                    setCell(row, col, v != 0);
                    from = to + 1;
                }
            } catch (std::invalid_argument& e) {
                UI_ERR << "parse error: " << e.what();
                // last value in column - break inner loop
                break;
            }
        }
    }

    drawActiveCells();
}

void UIMatrix::write(const std::string& fname)
{
    std::ofstream ofs(fname.c_str());
    if (!ofs) {
        UI_ERR << "can't open file " << quote(fname) << " for writing";
        return;
    }

    for (size_t row = 0; row < prop_rows_; row++) {
        for (size_t col = 0; col < prop_cols_; col++) {
            if (col != 0)
                ofs.put(',');

            ofs.put(cell(row, col) ? '1' : '0');
        }

        ofs << "\n";
    }

    if (ofs.fail())
        UI_ERR << "error while writing to file: " << quote(fname);
}

void UIMatrix::m_flip(const AtomList& lst)
{
    if (lst.empty()) {
        flipAll();
    } else if (lst.size() == 2) {
        if (lst[0].isSymbol()) {
            t_symbol* s = lst[0].asSymbol();

            if (s == SYM_COL) {
                int col = lst[1].asInt(-1);

                if (col < 0 || col >= prop_cols_) {
                    UI_ERR << "invalid column index: " << lst[1];
                    return;
                }

                flipColumn(col);

            } else if (s == SYM_ROW) {
                int row = lst[1].asInt(-1);

                if (row < 0 || row >= prop_rows_) {
                    UI_ERR << "invalid row index: " << lst[1];
                    return;
                }

                flipRow(row);
            } else {
                UI_ERR << "unknown flip target: " << s->s_name;
                return;
            }
        }

        if (lst[0].isFloat()) {
            int row = lst[0].asInt(-1);
            int col = lst[1].asInt(-1);

            if (row < 0 || row >= prop_rows_ || col < 0 || col >= prop_cols_) {
                UI_ERR << "invalid indexes: " << lst;
                return;
            }

            flipCell(row, col);
        }
    } else {
        UI_ERR << "usage: flip [col IDX] | [row IDX] | [row col]";
        return;
    }

    drawActiveCells();
}

void UIMatrix::m_reset()
{
    // redraw should be done before actual matrix reset
    reset_all_cells_ = true;
    drawActiveCells();

    // do after redraw with reset_all_cells_ flag
    matrix_.reset();
}

void UIMatrix::m_random()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 1);

    for (size_t i = 0; i < matrix_.size(); i++)
        matrix_.set(i, dist(mt));

    // better to redraw all then allocate memory to update list
    update_all_cells_ = true;
    drawActiveCells();
}

void UIMatrix::m_get(const AtomList& lst)
{
    if (lst.empty() || !lst[0].isSymbol()) {
        UI_ERR << "missing arguments: ";
        UI_ERR << "    usage: get col|row|cell|list|cols|rows|cells [ARGS]";
        return;
    }

    t_symbol* sel = lst[0].asSymbol();
    const AtomList args = lst.slice(1);

    if (sel == SYM_CELL) {
        outputCell(args);
    } else if (sel == SYM_COL) {
        outputCol(args);
    } else if (sel == SYM_ROW) {
        outputRow(args);
    } else if (sel == SYM_LIST) {
        listTo(0, asList());
    } else if (sel == SYM_OUTPUT_ALL_COLS) {
        outputAllCols();
    } else if (sel == SYM_OUTPUT_ALL_ROWS) {
        outputAllRows();
    } else if (sel == SYM_OUTPUT_ALL_CELLS) {
        outputAllCells();
    } else {
        UI_ERR << "unknown method: " << sel->s_name;
        UI_ERR << "    usage: get col|row|cell|list|cols|rows|cells [ARGS]";
        return;
    }
}

void UIMatrix::m_set(const AtomList& lst)
{
    if (lst.empty() || !lst[0].isSymbol()) {
        UI_ERR << "missing arguments: ";
        UI_ERR << "    usage: set col|row|cell|list [ARGS]";
        return;
    }

    t_symbol* sel = lst[0].asSymbol();
    const AtomList args = lst.slice(1);

    if (sel == SYM_CELL) {
        setCell(args);
    } else if (sel == SYM_COL) {
        setColumn(args);
    } else if (sel == SYM_ROW) {
        setRow(args);
    } else if (sel == SYM_LIST) {
        setList(args);
    } else {
        UI_ERR << "unknown method: " << sel->s_name;
        UI_ERR << "    usage: set col|row|cell|list [ARGS]";
        return;
    }

    drawActiveCells();
}

void UIMatrix::loadPreset(size_t idx)
{
    onList(PresetStorage::instance().listValueAt(presetId(), idx));
}

void UIMatrix::storePreset(size_t idx)
{
    PresetStorage::instance().setListValueAt(presetId(), idx, asList());
}

void UIMatrix::updateCellsCoords()
{
    const int w = cellWidth();
    const int h = cellHeight();

    if (cells_are_created_ && asEBox() && asEBox()->b_drawing_id) {
        for (int inc_x = CELL_MARGIN, col = 0; col < prop_cols_; col++, inc_x += w) {
            for (int inc_y = CELL_MARGIN, row = 0; row < prop_rows_; row++, inc_y += h) {

                int x0 = inc_x + CELL_MARGIN;
                int y0 = inc_y + CELL_MARGIN;
                int x1 = inc_x + w - CELL_MARGIN;
                int y1 = inc_y + h - CELL_MARGIN;

                sys_vgui("%s coords " CELL_TAG_FMT " %d %d %d %d\n",
                    asEBox()->b_drawing_id->s_name, asEBox(),
                    col, row, x0, y0, x1, y1);
            }
        }
    }
}

void UIMatrix::onPropChange(t_symbol* prop_name)
{
    if (prop_name == s_size)
        updateCellsCoords();
    else
        UIObject::onPropChange(prop_name);
}

void UIMatrix::onZoom(t_float z)
{
    bg_layer_.invalidate();
    cells_are_created_ = false;
}

void UIMatrix::onPopup(t_symbol* menu_name, long item_idx)
{
    if (menu_name == gensym("menu")) {
        switch (item_idx) {
        case 0:
            m_reset();
            break;
        case 1:
            m_flip(AtomList());
            break;
        case 2:
            m_random();
            break;
        case 3:
            eobj_read(asEObj(), gensym("read"), 0, nullptr);
            break;
        case 4:
            eobj_write(asEObj(), gensym("write"), 0, nullptr);
            break;
        }
    }
}

float UIMatrix::p_rows() const
{
    return prop_rows_;
}

float UIMatrix::p_cols() const
{
    return prop_cols_;
}

void UIMatrix::p_setRows(float n)
{
    int num = clip<int>(n, 1, UI_MAX_MATRIX_SIZE);
    if (num != prop_rows_) {
        prop_rows_ = num;
        matrix_.reset();
        updateSize();
        eraseCells();
        createCells();
    }
}

void UIMatrix::p_setCols(float n)
{
    int num = clip<int>(n, 1, UI_MAX_MATRIX_SIZE);
    if (num != prop_cols_) {
        prop_cols_ = num;
        matrix_.reset();
        updateSize();
        eraseCells();
        createCells();
    }
}

void UIMatrix::setup()
{
    UIObjectFactory<UIMatrix> obj("ui.matrix");
    obj.setDefaultSize(105, 53);

    obj.usePresets();
    obj.useList();
    obj.useBang();
    obj.useMouseEvents(UI_MOUSE_DOWN | UI_MOUSE_DRAG | UI_MOUSE_LEAVE);
    obj.readWrite();
    obj.hideLabelInner();

    obj.addProperty("rows", _("Rows"), 4, &UIMatrix::prop_rows_);
    obj.setPropertyRange("rows", 1, UI_MAX_MATRIX_SIZE);
    obj.setPropertyAccessor("rows", &UIMatrix::p_rows, &UIMatrix::p_setRows);
    obj.addProperty("cols", _("Columns"), 8, &UIMatrix::prop_cols_);
    obj.setPropertyRange("cols", 1, UI_MAX_MATRIX_SIZE);
    obj.setPropertyAccessor("cols", &UIMatrix::p_cols, &UIMatrix::p_setCols);
    obj.addProperty("active_color", _("Active Color"), DEFAULT_ACTIVE_COLOR, &UIMatrix::prop_color_active_);
    obj.addProperty("current_color", _("Current Color"), "1. 0.75 0. 1.", &UIMatrix::prop_color_current_);

    obj.addProperty("current_row", "", -1, &UIMatrix::current_row_);
    obj.hideProperty("current_row");
    obj.setPropertySave("current_row", false);
    obj.addProperty("current_col", "", -1, &UIMatrix::current_col_);
    obj.hideProperty("current_col");
    obj.setPropertySave("current_col", false);

    obj.addMethod("flip", &UIMatrix::m_flip);
    obj.addMethod("reset", &UIMatrix::m_reset);
    obj.addMethod("random", &UIMatrix::m_random);
    obj.addMethod("get", &UIMatrix::m_get);
    obj.addMethod("set", &UIMatrix::m_set);
}

void UIMatrix::addToUpdateList(int row, int col)
{
    cell_update_list_.push_back(CellList::value_type(row, col));
}

void UIMatrix::setCell(int row, int col, bool v)
{
    auto idx = row * UI_MAX_MATRIX_SIZE + col;
    if (matrix_[idx] != v) {
        matrix_.set(idx, v);
        addToUpdateList(row, col);
    }
}

int UIMatrix::cellWidth() const
{
    return std::floor(width() / prop_cols_);
}

int UIMatrix::cellHeight() const
{
    return std::floor(height() / prop_rows_);
}

void setup_ui_matrix()
{
    UIMatrix::setup();
}
