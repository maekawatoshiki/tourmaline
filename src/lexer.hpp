#pragma once

#include "common.hpp"
#include "error.hpp"


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
    Reporter reporter;

    char get_char();
    bool next_char_is(char);

    token_t read_token();
    token_t read_number();
    token_t read_ident();
    token_t read_blank();
    token_t read_string();
    token_t read_newline();
    token_t read_symbol();

    char replace_escape();
  public:
    enum {
      Ident,
      INumber,
      FNumber,
      String,
      Symbol,
      Newline,
      End,
    };
    void set_filename(std::string);
    std::string get_filename();

    token_t get();
    void    unget(token_t);
    bool    next_token_is(std::string);
    bool    skip(std::string);
    bool    expect_skip(std::string);
    bool    eot(); // END OF TOKEN
};
