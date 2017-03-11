#pragma once

#include "common.hpp"

enum {
  TOK_IDENT,
  TOK_INUMBER,
  TOK_FNUMBER,
  TOK_STRING,
  TOK_SYMBOL,
  TOK_NEWLINE,
  TOK_END,
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
    int cur_line = 1;
    std::vector<token_t> buffer;

    char get_char();
    bool next_char_is(char);

    token_t read_number();
    token_t read_ident();
    token_t read_blank();
    token_t read_string();
    token_t read_newline();
    token_t read_symbol();

    char replace_escape();
  public:
    void set_filename(std::string);

    token_t read_token();
};
