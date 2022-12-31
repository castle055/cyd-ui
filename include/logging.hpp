//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_LOGGING_HPP
#define CYD_UI_LOGGING_HPP


namespace logging {
  enum LogLevel {
      DEBUG,
      INFO,
      WARN,
      ERROR,
  };

  struct logger {
    const char* name = "log_task";
    bool on          = true;
    LogLevel min_level = DEBUG;

    void debug(const char* format, ...) const;

    void info(const char* format, ...) const;

    void warn(const char* format, ...) const;

    void error(const char* format, ...) const;
  };
}// namespace logging


#endif//CYD_UI_LOGGING_HPP
