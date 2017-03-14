#pragma once

#include "common.hpp"

class Reporter {
  public:
    void error(std::string filename, int line, const char *err_content, ...);
    void error_file_failed(std::string filename);
};
