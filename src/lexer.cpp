#include "lexer.hpp"

void Lexer::set_filename(std::string _filename) {
  filename = _filename;
  ifs.open(filename);
}

token_t Lexer::read_token() {
  char c = ifs.get();

         if(isdigit(c)) {
  } else if(isalpha(c) || c == '_') {
  } else if(isblank(c)) {
  } else if(c == '\"') {
  } else if(c == '\n') {
  } else ;
}
