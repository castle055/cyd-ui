/*! \file  profiling.cppm
 *! \brief
 *!
 */

module;
#include "cyd_ui/debug/profiling/macros.h"

export module cydui.debug.profiling;

import std;

template <std::size_t N>
consteval std::
  array<
    char,
    N>
  array_from_str(const std::string_view& a) {
  std::array<char, N> result{};
  std::copy_n(a.begin(), N, result.begin());
  return result;
}

template <
  std::size_t N1,
  std::size_t N2>
consteval std::array<
  char,
  N1 + N2>
concat(
  const std::array<
    char,
    N1>& a,
  const std::array<
    char,
    N2>& b
  // const char (&a)[N1],
  // const char (&b)[N2]
) {
  std::array<char, N1 + N2> result{}; // -1 to avoid double null terminator

  std::copy_n(a.begin(), N1, result.begin());        // copy without null
  std::copy_n(b.begin(), N2, result.begin() + (N1)); // copy including null

  return result;
}

template <std::uint32_t Color = 0xFCAE1E>
struct module_data_t {
  static constexpr std::uint32_t color = Color;
};

#define ROOT_MODULE(NAME, ...)                                                                     \
  namespace NAME {                                                                                 \
    module_data_t<__VA_ARGS__> data{};                                                             \
    bool                       enable_profiling = false;                                           \
    namespace this_module                       = NAME;                                            \
    constexpr bool             is_root_module   = true;                                            \
    constexpr bool             is_partition     = false;                                           \
    constexpr auto             name_arr         = array_from_str<sizeof(#NAME) - 1>(#NAME);        \
    constexpr auto             path_arr         = name_arr;                                        \
    constexpr std::string_view name = std::string_view{name_arr.data(), name_arr.size()};          \
    constexpr std::string_view path = std::string_view{path_arr.data(), path_arr.size()};          \
  }                                                                                                \
  namespace NAME

#define MODULE(NAME, ...)                                                                           \
  namespace NAME {                                                                                  \
    module_data_t<__VA_ARGS__> data{};                                                              \
    bool                       enable_profiling = false;                                            \
    namespace parent_module                     = this_module;                                      \
    namespace this_module                       = NAME;                                             \
    constexpr auto name_arr                     = array_from_str<sizeof(#NAME) - 1>(#NAME);         \
    constexpr auto path_arr =                                                                       \
      concat(concat(parent_module::path_arr, array_from_str<1>(".")), name_arr);                    \
    constexpr std::string_view name           = std::string_view{name_arr.data(), name_arr.size()}; \
    constexpr std::string_view path           = std::string_view{path_arr.data(), path_arr.size()}; \
    constexpr bool             is_root_module = false;                                              \
    constexpr bool             is_partition   = false;                                              \
  }                                                                                                 \
  namespace NAME

#define PARTITION(NAME, ...)                                                                        \
  namespace p_##NAME {                                                                              \
    module_data_t<__VA_ARGS__> data{};                                                              \
    bool                       enable_profiling = false;                                            \
    namespace parent_module                     = this_module;                                      \
    namespace this_module                       = p_##NAME;                                         \
    constexpr auto name_arr                     = array_from_str<sizeof(#NAME) - 1>(#NAME);         \
    constexpr auto path_arr =                                                                       \
      concat(concat(parent_module::path_arr, array_from_str<1>(":")), name_arr);                    \
    constexpr std::string_view name           = std::string_view{name_arr.data(), name_arr.size()}; \
    constexpr std::string_view path           = std::string_view{path_arr.data(), path_arr.size()}; \
    constexpr bool             is_root_module = false;                                              \
    constexpr bool             is_partition   = true;                                               \
  }


export namespace cydui::debug::modules {
  ROOT_MODULE(cydui) {
    MODULE(debug) {}
    MODULE(backends) {
      MODULE(sdl3) {
        PARTITION(window_events)
      }
    }
    MODULE(layout) {
      MODULE(focus_state){}
      MODULE(hover_state){}
      MODULE(ui_style){}
      MODULE(ui_tree){}
      MODULE(ui_renderer){}
      MODULE(ui_compositor){}
      MODULE(ui_frame){}
      MODULE(ui_updater){}
    }
  }
} // namespace cydui::debug::modules
