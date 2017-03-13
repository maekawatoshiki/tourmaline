#pragma once 

#include "common.hpp"

#define TOURMALINE_VERSION_S "0.0.1"

class Tourmaline {
  private:
    int argc;
    char **argv;

  public:
    Tourmaline();

    int run(int, char **);
    void show_usage();
    void show_version();

    int run_from_file(std::string);
};
