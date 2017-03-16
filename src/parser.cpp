#include "parser.hpp"

void Parser::set_filename(std::string filename) { 
  lexer.set_filename(filename);
}
std::string Parser::get_filename() {
  return lexer.get_filename();
}

AST_vec Parser::read_all() {
  AST_vec statements;
  while(!lexer.eot()) {
    auto st = read_statement();
    if(st) statements.push_back(st);
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
  AST *lhs = read_assign();
  return lhs;
}

AST *Parser::read_assign() {
  AST *lhs = read_logand_logor();
  bool is_assign = lexer.next_token_is("="),
       is_aadd   = lexer.next_token_is("+="),
       is_asub   = lexer.next_token_is("-="),
       is_amul   = lexer.next_token_is("*="),
       is_adiv   = lexer.next_token_is("/=");
  while(is_assign || is_aadd || is_asub ||
        is_amul || is_adiv) {
    auto op = lexer.get().val;
    auto rhs = read_expr();
    lhs = new BinaryOpAST(op, lhs, rhs);

    is_assign = lexer.next_token_is("=");
    is_aadd   = lexer.next_token_is("+=");
    is_asub   = lexer.next_token_is("-=");
    is_amul   = lexer.next_token_is("*=");
    is_adiv   = lexer.next_token_is("/=");
  }
  return lhs;
}

AST *Parser::read_logand_logor() {
  AST *lhs = read_comparation();
  bool is_and = lexer.next_token_is("&&"),
       is_or  = lexer.next_token_is("||");
  while(is_or  || is_and) {
    auto op = lexer.get().val;
    auto rhs = read_expr();
    lhs = new BinaryOpAST(op, lhs, rhs);

    is_and = lexer.next_token_is("&&");
    is_or  = lexer.next_token_is("||");
  }
  return lhs;
}

AST *Parser::read_comparation() {
  AST *lhs = read_or_xor();
  bool is_eql = lexer.next_token_is("=="),
       is_ne  = lexer.next_token_is("!="),
       is_le  = lexer.next_token_is("<="),
       is_ge  = lexer.next_token_is(">="),
       is_lt  = lexer.next_token_is("<"),
       is_gt  = lexer.next_token_is(">");
  while(is_eql || is_ne || is_le || is_ge || is_lt || is_gt) {
    auto op = lexer.get().val;
    auto rhs = read_expr();
    lhs = new BinaryOpAST(op, lhs, rhs);

    is_eql = lexer.next_token_is("==");
    is_ne  = lexer.next_token_is("!=");
    is_le  = lexer.next_token_is("<=");
    is_ge  = lexer.next_token_is(">=");
    is_lt  = lexer.next_token_is("<");
    is_gt  = lexer.next_token_is(">");
  }
  return lhs;
}

AST *Parser::read_or_xor() {
  AST *lhs = read_and();
  bool is_or  = lexer.next_token_is("|"),
       is_xor = lexer.next_token_is("^");
  while(is_or  || is_xor) {
    auto op = lexer.get().val;
    auto rhs = read_expr();
    lhs = new BinaryOpAST(op, lhs, rhs);

    is_or  = lexer.next_token_is("|");
    is_xor = lexer.next_token_is("^");
  }
  return lhs;
}

AST *Parser::read_and() {
  AST *lhs = read_shift();
  bool is_and = lexer.next_token_is("&");
  while(is_and) {
    auto op = lexer.get().val;
    auto rhs = read_expr();
    lhs = new BinaryOpAST(op, lhs, rhs);

    is_and = lexer.next_token_is("&");
  }
  return lhs;
}

AST *Parser::read_shift() {
  AST *lhs = read_add_sub();
  bool is_lsh = lexer.next_token_is("<<"),
       is_rsh = lexer.next_token_is(">>");
  while(is_lsh || is_rsh) {
    auto op = lexer.get().val;
    auto rhs = read_expr();
    lhs = new BinaryOpAST(op, lhs, rhs);

    is_lsh = lexer.next_token_is("<<");
    is_rsh = lexer.next_token_is(">>");
  }
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
    if(!lexer.skip(",")) reporter.error(get_filename(), 
        lexer.get().line, "expected ','");
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
      if(t.val == "if") {
        return read_if();
      } else 
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

AST *Parser::read_if() {
  auto cond = read_expr();
  if(!lexer.skip("then")) 
     if(!lexer.skip(";")) { auto t = lexer.get();
      if(t.kind != TOK_NEWLINE) 
        reporter.error(get_filename(), t.line, "expected '\\n', ';' or 'then'");}

  // TODO: not beautiful, FIX
  auto then = new BlockAST([&]() -> AST_vec {
    AST_vec then_;
    while(!lexer.skip("end") && !lexer.next_token_is("else") && !lexer.next_token_is("elsif")) {
      auto t = read_statement();
      if(t) then_.push_back(t);
    }
    return then_;
  }());

  AST *else_ = nullptr;
  if(lexer.skip("else")) {
    else_ = new BlockAST([&]() -> AST_vec {
      AST_vec else_v;
      while(!lexer.skip("end")) {
        auto t = read_statement();
        if(t) else_v.push_back(t);
      }
      return else_v;
    }());
  } else if(lexer.skip("elsif")) {
    else_ = read_if();
  }
  return new IfAST(cond, then, else_);
}

