#include "parser.hpp"

void Parser::set_filename(std::string filename) { 
  lexer.set_filename(filename);
}

AST_vec Parser::read_toplevel() {
  AST_vec statements;
  while(!lexer.eot()) {
    auto t = lexer.get(); std::cout << t.line << ":" << t.kind << " " << t.val << std::endl; lexer.unget(t);
    statements.push_back([&]() -> AST * {
      if(lexer.skip("def")) return read_func_def();
    }());
  }
  return statements;
}

AST *Parser::read_func_def() {
  return nullptr;
}
