#pragma once

#include "common.hpp"

class Reporter {
  private:
  public:
    template<typename ... Args>
    void error(std::string filename, int line, char *err_content, Args ... );
    void error_file_failed(std::string filename);
};
