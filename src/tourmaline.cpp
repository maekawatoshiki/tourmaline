#include "tourmaline.hpp"

Tourmaline::Tourmaline() {
}


int Tourmaline::run(int _argc, char *_argv[]) {
  argc = _argc; argv = _argv;
  
  if(argc < 2) {
    show_usage();
    return 0;
  }

  return run_from_file(argv[1]);
}

int Tourmaline::run_from_file(std::string file_name) {
  return 0;
}

void Tourmaline::show_usage() {
  puts("./tourmaline <file>");
}
