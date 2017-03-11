#include "tourmaline.hpp"
#include "lexer.hpp"

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
  Lexer lexer;
  lexer.set_filename(file_name);
  auto t = lexer.read_token();
  while(t.kind != TOK_END) {
    std::cout << t.line << ":" << t.kind << " " << t.val << std::endl;
    t = lexer.read_token();
  }
  return 0;
}

void Tourmaline::show_usage() {
  puts("./tourmaline <file>");
}
