//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_LOGGING_H
#define CYD_UI_LOGGING_H


namespace logging {
  struct logger {
    const char* name = "log_task";
    bool on = true;
    
    void debug(const char *format, ...) const;
  
    void info(const char *format, ...) const;
  
    void warn(const char *format, ...) const;
  
    void error(const char *format, ...) const;
  };
}


#endif //CYD_UI_LOGGING_H
