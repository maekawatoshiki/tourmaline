#include "parser.hpp"

void Parser::set_filename(std::string filename) { 
  lexer.set_filename(filename);
}

AST_vec Parser::read_all() {
  AST_vec statements;
  while(!lexer.eot()) {
    statements.push_back( read_statement() );
  }
  return statements;
}

AST *Parser::read_statement() {
  if(lexer.skip("def")) return read_func_def();
  return nullptr;
}

AST *Parser::read_func_def() {
  return nullptr;
}
