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
  return read_expr();
}

AST *Parser::read_func_def() {
  return nullptr;
}

AST *Parser::read_expr() {
  AST *lhs = read_if();
  return lhs;
}

AST *Parser::read_if() {
  AST *lhs = read_add_sub();
  return lhs;
}

AST *Parser::read_add_sub() {
  AST *lhs = read_mul_div_mod();
  bool is_add = lexer.next_token_is("+"),
       is_sub = lexer.next_token_is("-");
  while(is_add || is_sub) {
    auto op = lexer.get().val;
    auto rhs = read_expr();
    lhs = new BinaryOpAST(op, lhs, rhs);

    is_add = lexer.next_token_is("+");
    is_sub = lexer.next_token_is("-");
  }
  return lhs;
}

AST *Parser::read_mul_div_mod() {
  AST *lhs = read_func_call();
  bool is_mul = lexer.next_token_is("*"),
       is_div = lexer.next_token_is("/"),
       is_mod = lexer.next_token_is("%");
  while(is_mul || is_div || is_mod) {
    auto op = lexer.get().val;
    auto rhs = read_expr();
    lhs = new BinaryOpAST(op, lhs, rhs);

    is_mul = lexer.next_token_is("*");
    is_div = lexer.next_token_is("/");
    is_mod = lexer.next_token_is("%");
  }
  return lhs;
}

AST *Parser::read_func_call() {
  AST *lhs = read_primary();
  if(!lexer.skip("(")) return lhs;
  AST_vec args;
  while(!lexer.skip(")")) {
    args.push_back( read_expr() );
    if(lexer.skip(")")) break;
    lexer.skip(",");
  }
  lhs = new FuncCallAST(lhs, args);
  return lhs;
}

AST *Parser::read_primary() {
  auto t = lexer.get();
  switch(t.kind) {
    case TOK_INUMBER: 
      return new INumberAST(atoi(t.val.c_str()));
    case TOK_FNUMBER:
      break;
    case TOK_STRING:
      return new StringAST(t.val);
    case TOK_IDENT:
      return new VariableAST(t.val);
    case TOK_SYMBOL:
      if(t.val == "(") {
        auto expr = read_expr();
        lexer.skip(")");
        return expr;
      }
      break;
  }
  return nullptr;
}
