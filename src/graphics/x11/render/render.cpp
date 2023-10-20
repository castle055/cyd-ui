//
// Created by castle on 8/23/22.
//

#include "../state/state.hpp"
#include "render.hpp"
#include "cydstd/logging.hpp"

#include <X11/Xlib.h>

logging::logger xlog_ctrl = {.name = "X11::RENDER::CTRL", .on = false};
logging::logger xlog_task = {.name = "X11::RENDER::TASK", .on = true};

static std::mutex x_mtx {};

void render_sbr(cydui::graphics::window_t* win, XImage* image) {
  //window_render_req req;
  
  //win->render_mtx.lock();
  //bool dirty = win->dirty;
  //win->dirty = false;
  //win->render_mtx.unlock();
  //
  //if (!dirty)
  //  return;
  
  win->render_mtx.lock();
  {
    image->depth = DisplayPlanes(state::get_dpy(), state::get_screen());
    image->format = ZPixmap;
    image->xoffset = 0;
    image->data = (char*) win->render_target->data;
    image->width = win->render_target->width();
    image->height = win->render_target->height();
    image->bitmap_pad = 32;
    image->bitmap_unit = 32;
    image->bits_per_pixel = 32;
    image->bytes_per_line = (int) win->render_target->width() * 4;
    if (0 != XInitImage(image)
      && win->gc) {
      //if (x_mtx.try_lock()) {
      auto _pev = win->profiler->scope_event("render::render_sbr");
      //win->x_mtx.lock();
      XPutImage(state::get_dpy(),
        win->xwin,
        win->gc,
        image,
        0, 0,
        0, 0,
        win->render_target->width(), win->render_target->height()
      );
      //x_mtx.unlock();
      //}
      //X Flush(state::get_dpy());
      
      //XDestroyImage(image);
    }
  }
  win->render_mtx.unlock();
  //win->x_mtx.unlock();
}

using namespace std::chrono_literals;

void render_task(cydui::threading::thread_t* this_thread) {
  xlog_task.debug("Started render thread");
  auto* render_data = (render::RenderThreadData*) this_thread->data;
  auto t0 = std::chrono::system_clock::now();
  while (this_thread->running) {
    t0 = std::chrono::system_clock::now();
    render_sbr(render_data->win, render_data->image);
    //std::this_thread::sleep_until(t0 + 16666us); // 60 FPS
    std::this_thread::sleep_until(t0 + 2 * 16666us); // 60 FPS
  }
}

void render::start(cydui::graphics::window_t* win) {
  if (win->render_thd && win->render_thd->running)
    return;
  xlog_ctrl.debug("Starting render thread");
  
  delete (RenderThreadData*) win->render_data;
  win->render_data = new RenderThreadData {
    .win = win,
  };
  win->render_thd = cydui::threading::new_thread(render_task, win->render_data)
    ->set_name("RENDER_THD");
}

static std::unordered_map <u32, XColor> xcolor_cache;

struct xcolor_hot_cache_entry_t {
  u32 id;
  XColor xcolor;
};
static constexpr u32 xcolor_hot_cache_size = 8;
static xcolor_hot_cache_entry_t xcolor_hot_cache[xcolor_hot_cache_size];
static u32 xcolor_hot_cache_current_insert_index = 0U;

XColor color_to_xcolor(color::Color color) {
  u32 color_id = color.to_id();
  
  for (auto &entry: xcolor_hot_cache) {
    if (entry.id == color_id) {
      return entry.xcolor;
    }
  }
  
  if (xcolor_cache.contains(color_id)) {
    auto &c = xcolor_cache[color_id];
    xcolor_hot_cache[xcolor_hot_cache_current_insert_index++] = {
      .id = color_id,
      .xcolor = c,
    };
    if (xcolor_hot_cache_current_insert_index >= xcolor_hot_cache_size) {
      xcolor_hot_cache_current_insert_index = 0U;
    }
    return c;
  }
  
  auto dpy = state::get_dpy();
  auto screen = state::get_screen();
  Colormap map = DefaultColormap(dpy, screen);
  XColor c;
  XParseColor(dpy, map, color.to_string().c_str(), &c);
  XAllocColor(dpy, map, &c);
  
  xcolor_hot_cache[xcolor_hot_cache_current_insert_index++] = {
    .id = color_id,
    .xcolor = c,
  };
  if (xcolor_hot_cache_current_insert_index >= xcolor_hot_cache_size) {
    xcolor_hot_cache_current_insert_index = 0U;
  }
  
  xcolor_cache[color_id] = c;
  return c;
}

static std::unordered_map<u32, XftColor*> xftcolor_cache;

XftColor* color_to_xftcolor(color::Color color) {
  u32 color_id = color.to_id();
  if (xftcolor_cache.contains(color_id))
    return xftcolor_cache[color_id];
  
  auto* c = new XftColor;
  
  auto dpy = state::get_dpy();
  auto screen = state::get_screen();
  if (!XftColorAllocName(dpy,
    DefaultVisual(dpy, screen),
    DefaultColormap(dpy, screen),
    color.to_string().c_str(),
    c)) {
    xlog_ctrl.error("Cannot allocate color %s", color.to_string().c_str());
  }
  
  xftcolor_cache[color_id] = c;
  return c;
}

void render::resize(pixelmap_t* target, int w, int h) {
  //if (w != target->w || h != target->h) {
  //  Drawable new_drw = XCreatePixmap(state::get_dpy(),
  //    target->win->xwin,
  //    w,
  //    h,
  //    DefaultDepth(state::get_dpy(), state::get_screen()));
  //  XSetForeground(state::get_dpy(),
  //    target->gc,
  //    BlackPixel(state::get_dpy(), state::get_screen()));
  //  XFillRectangle(state::get_dpy(), new_drw, target->gc, 0, 0, w, h);
  //
  //  //target->win->x_mtx.lock();
  //  XCopyArea(state::get_dpy(),
  //    target->drawable,
  //    new_drw,
  //    target->gc,
  //    0,
  //    0,
  //    target->w,
  //    target->h,
  //    0,
  //    0);
  //
  //  XFreePixmap(state::get_dpy(), target->drawable);
  //  XFreeGC(state::get_dpy(), target->gc);
  //
  //  target->drawable = new_drw;
  //  target->gc = XCreateGC(state::get_dpy(), target->drawable, 0, NULL);
  //
  //  //target->win->x_mtx.unlock();
  //
  //  XFlush(state::get_dpy());
  //
  //  target->w = w;
  //  target->h = h;
  //  render::flush(target->win);
  //}
}

void render::flush(cydui::graphics::window_t* win) {
  std::lock_guard lk {win->render_mtx};
  auto _pev = win->profiler->scope_event("render::flush");
  
  auto* tmp = win->render_target;
  win->render_target = win->staging_target;
  win->staging_target = tmp;
  
  win->dirty = true;
}