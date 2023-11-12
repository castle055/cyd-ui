//
// Created by castle on 10/27/23.
//

#ifndef CYD_UI_ENV_H
#define CYD_UI_ENV_H

#include <cstdlib>
#include <string>

/**
 * \brief Process environmental variables interface
 *
 * This namespace provides an abstraction over environmental variables.
 */
namespace cydstd::env {
    /**
     * \brief Get the value of the environmental variable with name `ENVVAR`
     * \returns `std::string` containing the value of the variable
     */
    inline std::string get(const std::string &ENVVAR /**< [in] Variable name */) {
      return std::getenv(ENVVAR.c_str());
    }
    
    /**
     * \brief Set the value of the environmental variable with name `ENVVAR` to `VALUE`
     * \note This function **overwrites** the old value of the variable
     * \returns `std::string` containing the **old value** of the variable
     */
    inline std::string set(
      const std::string &ENVVAR, ///< [in] Variable name
      const std::string &VALUE   ///< [in] New value
    ) {
      std::string prev = get(ENVVAR);
      setenv(ENVVAR.c_str(), VALUE.c_str(), true);
      return prev;
    }
    
    struct var {
      const std::string &ENVVAR;
      
      operator std::string() {
        return env::get(ENVVAR);
      }
      
      var &operator=(const std::string &VALUE) {
        env::set(ENVVAR, VALUE);
        return *this;
      }
    };
}


#endif //CYD_UI_ENV_H
