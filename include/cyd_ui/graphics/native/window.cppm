/*! \file  window.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <png.h>
#include <cyd_fabric/async/async_bus.h>
#include <cyd_fabric/profiling/profiling.h>
#include <jpeglib.h>
#include <cyd_fabric/type_aliases.h>

export module cydui.graphics.native:window;

import std;

export import :state;


using namespace x11;
using namespace cyd::fabric::type_aliases;


export struct window_font {
  XftFont* xfont;
  FcPattern* pattern;
};
export typedef std::unordered_map<std::string, window_font> loaded_font_map_t;

export struct window_image {
  XImage* ximg;
};
export typedef std::unordered_map<std::string, window_image> loaded_images_map_t;

export namespace cyd::ui::graphics {
  struct window_input_method_t {
    Window xwin;
    XIM    xim;
    XIC    xic;
    // Status st;
    // KeySym ksym;
    char input_buffer[64];

    explicit window_input_method_t(Window xwin);

    ~window_input_method_t();
  };

  window_input_method_t::window_input_method_t(Window xwin) {
    this->xwin = xwin;
    this->xim  = XOpenIM(state::get_dpy(), NULL, NULL, NULL);
    this->xic  = XCreateIC(this->xim,
                          XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                          XNClientWindow, xwin, NULL
    );
  }

  window_input_method_t::~window_input_method_t() {
    XDestroyIC(xic);
    XCloseIM(xim);
  }

  struct window_t {
    cyd::fabric::async::async_bus_t* bus;
    prof::context_t*                 profiler;
    Window                           xwin;
    GC                               gc;
    window_input_method_t            input_method;
    pixelmap_t*                      staging_target = nullptr;
    pixelmap_t*                      render_target  = nullptr;
    //Drawable drawable;
    //Drawable staging_drawable;
    //int staging_w;
    //int staging_h;
    //GC gc;
    //int w;
    //int h;
    bool                dirty = true;
    std::mutex          render_mtx;
    loaded_font_map_t   loaded_fonts;
    loaded_images_map_t loaded_images;

    window_t(
      cyd::fabric::async::async_bus_t* async_bus,
      prof::context_t*                 profiler,
      Window                           xwin,
      unsigned long                    w,
      unsigned long                    h
    );

    cyd::ui::threading::thread_t* render_thd  = nullptr;
    void*                         render_data = nullptr;
  };

  window_t::window_t(
    cyd::fabric::async::async_bus_t* async_bus,
    prof::context_t*                 profiler,
    Window                           xwin,
    unsigned long                    w,
    unsigned long                    h
  ): input_method(xwin) {
    this->bus            = async_bus;
    this->profiler       = profiler;
    this->xwin           = xwin;
    this->staging_target = new pixelmap_t {w, h};
    this->render_target  = new pixelmap_t {w, h};
  }

}


namespace x11::imgs {
  struct jpg_err_mgr {
    jpeg_error_mgr pub;
  };

  METHODDEF(void)
  jpg_error_exit(j_common_ptr cinfo) {
    (*cinfo->err->output_message)(cinfo);
  }

  struct img_data {
    ulong width = 0;
    ulong height = 0;
    int components = 0;
    char* data = nullptr;
  };

  inline img_data read_jpg(str path) {
    FILE* infile;
    JSAMPARRAY buffer;
    jpeg_decompress_struct cinfo{};
    jpg_err_mgr jerr{};

    if ((infile = fopen(path.c_str(), "rb")) == nullptr) {
      return {};
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = jpg_error_exit;

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);

    jpeg_start_decompress(&cinfo);
    JDIMENSION row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)(
      (j_common_ptr) &cinfo,
      JPOOL_IMAGE,
      row_stride,
      1
    );

    auto* data = (unsigned char*) malloc(cinfo.output_height * cinfo.output_width * 4);

    while (cinfo.output_scanline < cinfo.output_height) {
      jpeg_read_scanlines(&cinfo, buffer, 1);
      for (uint i = 0; i < row_stride; i += cinfo.output_components) {
        unsigned char c1 = buffer[0][i];
        unsigned char c2 = buffer[0][i + 1];
        unsigned char c3 = buffer[0][i + 2];
        uint r = cinfo.output_width * 4 * (cinfo.output_scanline - 1);
        data[r + (4 * i / 3) + 0] = c3;
        data[r + (4 * i / 3) + 1] = c2;
        data[r + (4 * i / 3) + 2] = c1;
        data[r + (4 * i / 3) + 3] = 5;
      }
    }


    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return {
      cinfo.output_width,
      cinfo.output_height,
      4,
      (char*) data,
    };
  }
}

