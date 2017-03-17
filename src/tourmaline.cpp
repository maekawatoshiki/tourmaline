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

  std::string filename;
  bool opt_emit_ir = false,
       opt_version = false,
       opt_help    = false;
  for(int i = 1; i < argc; i++) {
    if(!strcmp(argv[i], "-emit-ir")) {
      opt_emit_ir = true;
    } else if(!strcmp(argv[i], "-v")) {
      opt_version = true;
    } else if(!strcmp(argv[i], "-h")) {
      opt_help = true;
    } else filename = argv[i];
  }
  if(opt_version) show_version();
  if(opt_help)    show_usage();

  if(!filename.empty()) run_from_file(filename);

  if(opt_emit_ir) mod->dump();
  return 0;
}

int Tourmaline::run_from_file(std::string filename) {
  // FOR DEBUGGING
  // Lexer lexer; lexer.set_filename(file_name);
  // while(!lexer.eot()) {
  //   auto t = lexer.get(); std::cout << t.line << ":" << t.kind << " " << t.val << std::endl;
  // }
  Parser parser;
  parser.set_filename(filename);
  auto statements = parser.read_all();
  for(auto st : statements) st->show();
  puts("");
  Codegen codegen;
  codegen.set_filename(filename);
  codegen.gen(statements);
  return 0;
}

void Tourmaline::show_usage() {
  puts("Usage: tourmaline [file]");
  puts("   -emit-ir   : emit llvm-ir to stderr");
  puts("   -v         : show version info.");
  puts("   -h         : show this help");
}
void Tourmaline::show_version() {
  puts("tourmaline " TOURMALINE_VERSION_S " (build " __DATE__ " " __TIME__ ")");
}
