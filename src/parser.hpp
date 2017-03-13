#pragma once

#include "common.hpp"
#include "lexer.hpp"
#include "ast.hpp"

class Parser {
  private:
    Lexer lexer;
  public:
    void set_filename(std::string);     

    AST_vec read_toplevel();
};
