//
// Created by castle on 8/21/22.
//

#include "../../include/logging.hpp"
#include <cstdarg>
#include <ctime>
#include <string>

void logging::logger::debug(const char* format, ...) const {
  if (!this->on || this->min_level > DEBUG)
    return;
  std::va_list va;
  time_t       now = time(nullptr);
  
  std::string msg = ctime(&now);
  msg = msg.substr(0, msg.size() - 2);
  msg.append(": [DEBUG]");
  msg.append("[");
  msg.append(name);
  msg.append("] ");
  msg.append(format);
  msg.append("\n\r");
  
  va_start(va, format);
  vfprintf(stdout, msg.c_str(), va);
  va_end(va);
  fflush(stdout);
}

void logging::logger::info(const char* format, ...) const {
  if (!this->on || this->min_level > INFO)
    return;
  va_list va;
  time_t  now = time(nullptr);
  
  std::string msg = ctime(&now);
  msg = msg.substr(0, msg.size() - 2);
  msg.append(": [INFO] ");
  msg.append("[");
  msg.append(name);
  msg.append("] ");
  msg.append(format);
  msg.append("\n\r");
  
  va_start(va, format);
  vfprintf(stdout, msg.c_str(), va);
  va_end(va);
  fflush(stdout);
}

void logging::logger::warn(const char* format, ...) const {
  if (!this->on || this->min_level > WARN)
    return;
  va_list va;
  time_t  now = time(nullptr);
  
  std::string msg = ctime(&now);
  msg = msg.substr(0, msg.size() - 2);
  msg.append(": [WARN] ");
  msg.append("[");
  msg.append(name);
  msg.append("] ");
  msg.append(format);
  msg.append("\n\r");
  
  va_start(va, format);
  vfprintf(stdout, msg.c_str(), va);
  va_end(va);
  fflush(stdout);
}

void logging::logger::error(const char* format, ...) const {
  if (!this->on || this->min_level > ERROR)
    return;
  va_list va;
  time_t  now = time(nullptr);
  
  std::string msg = ctime(&now);
  msg = msg.substr(0, msg.size() - 2);
  msg.append(": [ERROR] ");
  msg.append("[");
  msg.append(name);
  msg.append("] ");
  msg.append(format);
  msg.append("\n\r");
  
  va_start(va, format);
  vfprintf(stderr, msg.c_str(), va);
  va_end(va);
  fflush(stderr);
}
