#pragma once

#include "common.hpp"

enum {
  TOK_IDENT,
  TOK_NUMBER,
  TOK_STRING,
  TOK_SYMBOL,
  TOK_NEWLINE,
};

struct token_t {
  token_t(int _kind, std::string _val, int _line = 0):
    kind(_kind), val(_val), line(_line) {};
  int kind;
  std::string val;
  int line;
};

class Lexer {
  private:
    std::string filename;
    std::ifstream ifs;

  public:
    void set_filename(std::string);

    token_t read_token();

    token_t read_number();
    token_t read_ident();
    token_t read_blank();
    token_t read_string();
    token_t read_newline();
    token_t read_symbol();
};
