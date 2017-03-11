#pragma once 

#include "common.hpp"

class Tourmaline {
  private:
    int argc;
    char **argv;

  public:
    Tourmaline();

    int run(int, char **);
    void show_usage();

    int run_from_file(std::string);
};
