//
// Created by castle on 1/25/23.
//

#ifndef CYD_UI_X11_IMAGES_H
#define CYD_UI_X11_IMAGES_H

#include <string>
#include <jpeglib.h>

namespace imgs {
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
    
    inline img_data read_jpg(std::string path) {
      FILE* infile;
      JSAMPARRAY buffer;
      jpeg_decompress_struct cinfo {};
      jpg_err_mgr jerr {};
      
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
      
      logging::logger logg {};
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

#endif //CYD_UI_X11_IMAGES_H
