//
// Created by castle on 8/21/22.
//

#ifndef CYD_LOG_LOGGING_HPP
#define CYD_LOG_LOGGING_HPP


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

    void debug(const char* format, ...) const __attribute__ ((format (printf, 2, 3)));

    void info(const char* format, ...) const __attribute__ ((format (printf, 2, 3)));

    void warn(const char* format, ...) const __attribute__ ((format (printf, 2, 3)));

    void error(const char* format, ...) const __attribute__ ((format (printf, 2, 3)));
  };
}// namespace logging


#endif//CYD_LOG_LOGGING_HPP
