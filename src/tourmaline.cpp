#include "tourmaline.hpp"
#include "lexer.hpp"
#include "parser.hpp"

Tourmaline::Tourmaline() {
}


int Tourmaline::run(int _argc, char *_argv[]) {
  argc = _argc; argv = _argv;
  
  if(argc < 2) {
    show_version();
    show_usage();
    return 0;
  }

  return run_from_file(argv[1]);
}

int Tourmaline::run_from_file(std::string file_name) {
  Parser parser;
  parser.set_filename(file_name);
  parser.read_toplevel();
  return 0;
}

void Tourmaline::show_usage() {
  puts("Usage: tourmaline [file]");
}
void Tourmaline::show_version() {
  puts("tourmaline " TOURMALINE_VERSION_S " (build " __DATE__ " " __TIME__ ")");
}
