#include "lexer.hpp"

char Lexer::get_char() {
  return ifs.get();
}

bool Lexer::next_char_is(char c) {
  char cc = ifs.get();
  auto r = c == cc;
  ifs.unget();
  return r;
}

token_t Lexer::read_token() {
  if(!buffer.empty()) {
    auto f = buffer.front();
    buffer.erase(buffer.begin());
    return f;
  }
  char c = ifs.get();
  if(ifs.eof()) return token_t(TOK_END, "");
  ifs.unget();
       if(isdigit(c))             return read_number();
  else if(isalpha(c) || c == '_') return read_ident();
  else if(isblank(c))             return read_blank();
  else if(c == '\"')              return read_string();
  else if(c == '\n')              return read_newline();
  else                            return read_symbol();
}

token_t Lexer::read_number() {
  std::string number;
  char last = 0;
  bool is_float_tok = false;
  for(;;) {
    char c = ifs.get();
    bool is_float = strchr("eEpP", last) && strchr("+-", c);
    if(last == '.' && c == '.') {
      ifs.unget();
      number.pop_back();
      break;
    }
    if(!isdigit(c) && !isalpha(c) && c != '.' && !is_float) break;
    if(!is_float_tok && (is_float || c == '.')) is_float_tok = true;
    number += c;
    last = c;
  }
  ifs.unget();
  return token_t(is_float_tok ? TOK_FNUMBER : TOK_INUMBER, number, cur_line);
}

token_t Lexer::read_ident() {
  std::string ident;
  char c = ifs.get();
  for(; isalpha(c) || isdigit(c) ||
      c == '_'; c = ifs.get())
    ident += c;
  ifs.unget();
  return token_t(TOK_IDENT, ident, cur_line);
}

token_t Lexer::read_blank() {
  ifs.get();
  return read_token();
}

token_t Lexer::read_string() {
  std::string content;
  ifs.get(); // "
  for(char c = ifs.get(); c != '\"'; c = ifs.get())
    content += (c == '\\') ? replace_escape() : c;
  return token_t(TOK_STRING, content, cur_line);
}

token_t Lexer::read_newline() {
  cur_line++;
  get_char();
  return token_t(TOK_NEWLINE, "", cur_line);
}

token_t Lexer::read_symbol() {
  char c = ifs.get();
  std::string op; op = c;
  switch(c) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '^':
      if(next_char_is('=')) op += get_char();
      break;
    case '&':
      if(next_char_is('&')) op += get_char();
      if(next_char_is('=')) op += get_char();
      break;
    case '|':
      if(next_char_is('|')) op += get_char();
      if(next_char_is('=')) op += get_char();
      break;
    case '<':
      if(next_char_is('=')) op += get_char();
      else if(next_char_is('<')) { 
        op += get_char();
        if(next_char_is('=')) op += get_char();
      }
      break;
    case '>':
      if(next_char_is('=')) op += get_char();
      else if(next_char_is('>')) { 
        op += get_char();
        if(next_char_is('=')) op += get_char();
      }
      break;
    case '.':
      if(next_char_is('.')) op += get_char();
      if(next_char_is('.')) op += get_char();
      break;
  }
  return token_t(TOK_SYMBOL, op, cur_line);
}

char Lexer::replace_escape() {
  char c = ifs.get();
  switch(c) {
    case '\'': case '"': case '?': case '\\':
      return c;
    case 'a': return '\a';
    case 'b': return '\b';
    case 'f': return '\f';
    case 'n': return '\n';
    case 'r': return '\r';
    case 't': return '\t';
    case 'v': return '\v';
  }
  return c;
}

void Lexer::set_filename(std::string _filename) {
  filename = _filename;
  ifs.open(filename);
  if(ifs.fail()) exit(0);
}

token_t Lexer::get() {
  return read_token();
}

void Lexer::unget(token_t t) {
  buffer.push_back(t);
}

bool Lexer::next_token_is(std::string s) {
  auto t = get();
  auto is_next_token_expected = (t.kind == TOK_IDENT && t.kind == TOK_SYMBOL && t.val == s);
  unget(t);
  return is_next_token_expected;
}

bool Lexer::skip(std::string s) { 
  auto t = get();
  auto is_next_token_expected = (t.kind == TOK_IDENT && t.kind == TOK_SYMBOL && t.val == s);
  if(!is_next_token_expected) unget(t);
  return is_next_token_expected;
}

bool Lexer::eot() {
  auto t = get();
  if(t.kind == TOK_END) return true;
  unget(t);
  return false;
}
