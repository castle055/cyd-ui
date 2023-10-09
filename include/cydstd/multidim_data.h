//
// Created by castle on 10/6/23.
//

#ifndef CYD_UI_STD_MULTIDIM_DATA_H
#define CYD_UI_STD_MULTIDIM_DATA_H

#include <cstddef>
#include <cstdlib>
#include <cmath>

#include <ImageMagick-7/Magick++.h>

using size_t = std::size_t;
using u8 = unsigned char;

template<typename E, size_t D>
struct md_buffer_t {
  size_t size[D];
  E* data = nullptr;
  
  explicit md_buffer_t(const size_t size[D]): size(size) {
    size_t total_size = compute_total_size(this->size);
    data = (E*)malloc(sizeof(E) * total_size);
  }
  explicit md_buffer_t(std::initializer_list<size_t> size) {
    size_t i = 0;
    for (const auto &s: size) {
      this->size[i++] = s;
    }
    
    size_t total_size = compute_total_size(this->size);
    data = (E*)malloc(sizeof(E) * total_size);
  }
  ~md_buffer_t() {
    free(data);
  }
  
  void resize(const size_t new_size[D]) {
    size_t total_size = compute_total_size(this->size);
    size_t total_new_size = compute_total_size(new_size);
    // TODO
  }
  
  E* get(std::initializer_list<size_t> coords) {
    size_t index = compute_index(coords);
    return &data[index];
  }
  
  void set(std::initializer_list<size_t> coords, const E& value) {
    size_t index = compute_index(coords);
    if (index < compute_total_size(size)) {
      data[index] = value;
    }
  }
  
private:
  size_t compute_total_size(const size_t s[D]) const {
    size_t total_size = 1;
    for (size_t i = 0; i < D; ++i) {
      total_size *= s[i];
    }
    return total_size;
  }
  
  size_t compute_index(std::initializer_list<size_t> coords) const {
    // i = x + X*y + X*Y*z + ...
    auto iter = coords.begin();
    size_t index = *iter;
    size_t multiplier = size[0];
    iter = std::next(iter);
    for (size_t i = 1; i < D; ++i) {
      index += multiplier * *iter;
      multiplier *= size[i];
      iter = std::next(iter);
    }
    return index;
  }
  
  void copy_from(const E* source, const size_t src_coords[D], const size_t dst_coords[D], const size_t _size[D]) {
    size_t stride = _size[0];
    
  }
};

#endif //CYD_UI_STD_MULTIDIM_DATA_H
