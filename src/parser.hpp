#pragma once

#include "common.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "error.hpp"

class Parser {
  private:
    Lexer lexer;
    Reporter reporter;
  public:
    void set_filename(std::string);     

    AST_vec read_all();
      
    AST *read_statement();
    AST *read_func_def();
};
