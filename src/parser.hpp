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
    // for error handler
    void set_filename(std::string);     
    std::string get_filename();

    AST_vec read_all();
      
    AST *read_statement();
    AST *read_func_def();

    AST *read_expr();
    AST *read_assign();
    AST *read_logand_logor();
    AST *read_comparation();
    AST *read_or_xor();
    AST *read_and();
    AST *read_shift();
    AST *read_add_sub();
    AST *read_mul_div_mod();
    AST *read_index();
    AST *read_func_call();
    AST *read_primary();
    AST *read_array();
    AST *read_if();
};
