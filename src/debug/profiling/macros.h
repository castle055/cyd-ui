//
// Created by castle on 6/2/25.
//

#ifndef PROF_MACROS_H
#define PROF_MACROS_H

#include <tracy/Tracy.hpp>

#define PROF_DEFAULT_SCOPE_VAR_NAME ___profiled_scope

#define PROF_SCOPE_COLOR(R, G, B)                                                                  \
  ZoneColorV(PROF_DEFAULT_SCOPE_VAR_NAME, (R << 16U) | (G << 8U) | B)

#define PROF_SCOPE_TEXT(...)                                                                       \
  {                                                                                                \
    std::string t = std::format(__VA_ARGS__);                                                      \
    ZoneTextV(PROF_DEFAULT_SCOPE_VAR_NAME, t.c_str(), t.size());                                   \
  }

#define PROF_MESSAGE(...)                                                                          \
  {                                                                                                \
    std::string msg = std::format(__VA_ARGS__);                                                    \
    TracyMessage(msg.c_str(), msg.size());                                                         \
  }

#define PROF_SCOPE(...)                                                                            \
  ZoneNamedNC(                                                                                     \
    PROF_DEFAULT_SCOPE_VAR_NAME,                                                                   \
    #__VA_ARGS__,                                                                                  \
    cydui::debug::modules::PROF_CURRENT_MODULE::data.color,                                        \
    cydui::debug::modules::PROF_CURRENT_MODULE::enable_profiling                                   \
  );                                                                                               \
  PROF_SCOPE_TEXT(                                                                                 \
    "Module: {} ({})",                                                                             \
    cydui::debug::modules::PROF_CURRENT_MODULE::name,                                              \
    cydui::debug::modules::PROF_CURRENT_MODULE::path                                               \
  )


#define PROF_CONFIG(MODULE, ENABLED) cydui::debug::modules::MODULE::enable_profiling = ENABLED

#endif // PROF_MACROS_H
