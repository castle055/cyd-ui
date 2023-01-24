//
// Created by castle on 8/21/22.
//

#include "layout.hpp"
#include "../graphics/events.hpp"

logging::logger log_lay = {
        .name = "LAYOUT", .on = true, .min_level = logging::INFO};

cydui::layout::Layout::Layout(cydui::components::Component* root): root(root) {
}

#define COMPUTE(DIM) \
if (!dimension_t::compute(DIM)) { return false; }

static bool compute_dimensions(cydui::components::Component* rt) {
    using namespace cydui::dimensions;
    component_dimensions_t* dim = &rt->state->dim;
    /// POSITION
    COMPUTE(dim->x)
    COMPUTE(dim->y)

    /// SIZE
    COMPUTE(dim->margin.top)
    COMPUTE(dim->margin.right)
    COMPUTE(dim->margin.bottom)
    COMPUTE(dim->margin.left)

    COMPUTE(dim->padding.top)
    COMPUTE(dim->padding.right)
    COMPUTE(dim->padding.bottom)
    COMPUTE(dim->padding.left)

    if (!dim->given_w) {
        dim->w.unknown = true;
    }
    if (!dim->given_h) {
        dim->h.unknown = true;
    }

    std::vector<cydui::components::Component*> pending;

    dimension_value_t total_w = 0;
    dimension_value_t total_h = 0;
    for (auto &child: rt->children) {
        //compute_dimensions(child);
        // if error (circular dependency), skip for now, and then recalculate
        if (compute_dimensions(child)) {
            component_dimensions_t* c_dim = &child->state->dim;
            dimension_value_t child_max_w = c_dim->x.val() + c_dim->w.val();
            dimension_value_t child_max_h = c_dim->y.val() + c_dim->h.val();
            total_w = std::max(total_w, child_max_w);
            total_h = std::max(total_h, child_max_h);
        } else {
            pending.push_back(child);
        }
    }

    if (dim->given_w) {
        COMPUTE(dim->w)
    } else {// If not given, or given has error (ie: circular dep)
        dim->w = total_w;
    }

    if (dim->given_h) {
        COMPUTE(dim->h)
    } else {// If not given, or given has error (ie: circular dep)
        dim->h = total_h;
    }

    for (auto &item: pending) {
        if (!compute_dimensions(item)) {
            return false;
        }
    }

    return true;
}

#undef COMPUTE

static void redraw_component(
        const cydui::window::CWindow* win, cydui::components::Component* target
) {
    log_lay.debug("REDRAW");
    // Clear render area of component instances
    for (auto child: target->children)
        delete child;
    target->children.clear();

    // Recreate those instances with redraw(), this set all size hints relationships
    target->redraw();

    if (!compute_dimensions(target)) {
        cydui::components::Component* c = target;
        while (c->parent && !compute_dimensions(c->parent)) {
            c = c->parent;
        }
        if (c->parent) {
            // TODO - Catch dimensional error
        }
    }

    // Clear screen area
    cydui::graphics::clr_rect(win->win_ref,
            target->state->dim.x,
            target->state->dim.y,
            target->state->dim.w,
            target->state->dim.h);

    // Render screen area & flush graphics
    target->render(win);

    //    if (render_if_dirty(root))
    cydui::graphics::flush(win->win_ref);
}

void cydui::layout::Layout::bind_window(cydui::window::CWindow* _win) {
    this->win = _win;

    listen(RedrawEvent, {
        if (it.data->win != 0 && it.data->win != win->win_ref->xwin)
            return;
        cydui::components::Component* target = root;
        if (it.data->component) {
            cydui::components::ComponentState* target_state =
                    ((cydui::components::ComponentState*) it.data->component);
            cydui::components::Component* specified_target =
                    target_state->component_instance;
            if (specified_target)
                target = specified_target;
        }

        redraw_component(this->win, target);
    });
    listen(KeyEvent, {
        if (it.data->win != win->win_ref->xwin)
            return;
    });
    listen(ButtonEvent, {
        if (it.data->win != win->win_ref->xwin)
            return;
        if (it.data->pressed) {
            cydui::components::Component* target = root;
            cydui::components::Component* specified_target =
                    find_by_coords(root, it.data->x, it.data->y);
            if (specified_target)
                target = specified_target;

            int rel_x = it.data->x - target->state->geom.border_x().compute();
            int rel_y = it.data->y - target->state->geom.border_y().compute();

            target->on_mouse_click(rel_x, rel_y, it.data->button);
            if (render_if_dirty(root))
                graphics::flush(win->win_ref);
        }
    });
    listen(ScrollEvent, {
        if (it.data->win != win->win_ref->xwin)
            return;
        cydui::components::Component* target = root;
        cydui::components::Component* specified_target =
                find_by_coords(root, it.data->x, it.data->y);
        if (specified_target)
            target = specified_target;

        target->on_scroll(it.data->d);
        if (render_if_dirty(root))
            graphics::flush(win->win_ref);
    });
    listen(MotionEvent, {
        if (it.data->win != win->win_ref->xwin)
            return;

        if (it.data->x == -1 && it.data->y == -1) {
            if (focused && focused->component_instance) {
                int exit_rel_x = 0;
                int exit_rel_y = 0;
                focused->component_instance->on_mouse_exit(exit_rel_x, exit_rel_y);
            }
        } else {
            cydui::components::Component* target = root;
            cydui::components::Component* specified_target =
                    find_by_coords(root, it.data->x, it.data->y);
            if (specified_target)
                target = specified_target;

            if (focused != target->state) {
                if (focused && focused->component_instance) {
                    int exit_rel_x = it.data->x - focused->geom.border_x().compute();
                    int exit_rel_y = it.data->y - focused->geom.border_y().compute();
                    focused->component_instance->on_mouse_exit(exit_rel_x, exit_rel_y);
                }
                focused = target->state;
            }

            int rel_x = it.data->x - target->state->geom.border_x().compute();
            int rel_y = it.data->y - target->state->geom.border_y().compute();
            target->on_mouse_enter(rel_x, rel_y);
        }

        if (render_if_dirty(root))
            graphics::flush(win->win_ref);
    });
    listen(ResizeEvent, {
        if (it.data->win != win->win_ref->xwin)
            return;
        log_lay.debug("RESIZE w=%d, h=%d", it.data->w, it.data->h);

        root->state->geom.w = win->win_ref->w;
        root->state->geom.h = win->win_ref->h;

        if (render_if_dirty(root))
            graphics::flush(win->win_ref);
    });
    listen(UpdatePropEvent, {
        if (it.data->win != win->win_ref->xwin)
            return;
        ((Property*) it.data->target_property)
                ->set_raw_value((void*) (it.data->new_value));
        if (render_if_dirty(root))
            graphics::flush(win->win_ref);
    });
}

bool cydui::layout::Layout::render_if_dirty(cydui::components::Component* c) {
    if (c->state->_dirty) {
        redraw_component(this->win, c);
        return true;
    } else {
        bool any = false;
        for (auto &item: c->children)
            any = render_if_dirty(item) || any;// F**K, order here matters
        return any;
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
#pragma ide diagnostic ignored "modernize-loop-convert"

cydui::components::Component* cydui::layout::Layout::find_by_coords(
        components::Component* c, int x, int y
) {
    components::Component* target = nullptr;
    for (auto i = c->children.rbegin(); i != c->children.rend(); ++i) {
        auto* item = *i;
        if (x >= item->state->geom.border_x().compute()
                && x < (item->state->geom.border_x().compute()
                + item->state->geom.border_w().compute())
                && y >= item->state->geom.border_y().compute()
                && y < (item->state->geom.border_y().compute()
                + item->state->geom.border_h().compute())) {
            target = find_by_coords(item, x, y);
            if (target)
                break;
        }
    }
    if (target)
        return target;

    if (!c->state->stateless_comp && x >= c->state->geom.border_x().compute()
            && x < (c->state->geom.border_x().compute()
            + c->state->geom.border_w().compute())
            && y >= c->state->geom.border_y().compute()
            && y < (c->state->geom.border_y().compute()
            + c->state->geom.border_h().compute())) {
        target = c;
    }
    return target;
}

#pragma clang diagnostic pop
