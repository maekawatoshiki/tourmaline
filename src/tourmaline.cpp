#include "tourmaline.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"

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
  // FOR DEBUGGING
  // Lexer lexer; lexer.set_filename(file_name);
  // while(!lexer.eot()) {
  //   auto t = lexer.get(); std::cout << t.line << ":" << t.kind << " " << t.val << std::endl;
  // }
  Parser parser;
  parser.set_filename(file_name);
  auto statements = parser.read_all();
  for(auto st : statements) st->show();
  Codegen codegen;
  codegen.gen(statements);
  return 0;
}

void Tourmaline::show_usage() {
  puts("Usage: tourmaline [file]");
}
void Tourmaline::show_version() {
  puts("tourmaline " TOURMALINE_VERSION_S " (build " __DATE__ " " __TIME__ ")");
}
