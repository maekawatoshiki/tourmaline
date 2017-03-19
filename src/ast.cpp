#include "ast.hpp"

FuncDefAST::FuncDefAST(std::string _name, Type *_ret_ty, func_args_t _args, AST *_body):
  name(_name), ret_ty(_ret_ty), args(_args), body(_body) {
}
void FuncDefAST::show() {
  std::cout << "(def-func " << name;
  for(auto arg : args) {
    std::cout << " (" << arg->type->to_string() << " " << arg->name << ")";
  }
  std::cout << "(";
  body->show();
  std::cout << ")";
  std::cout << ")";
}

IfAST::IfAST(AST *_cond, AST *_then_st, AST *_else_st):
  cond(_cond), then_st(_then_st), else_st(_else_st) {
}
void IfAST::show() {
  std::cout << "(if (";
  cond->show();
  std::cout << ") (";
  if(then_st) then_st->show();
  std::cout << ") (";
  if(else_st) else_st->show();
  std::cout << "))";
}

BlockAST::BlockAST(AST_vec _statements):
  statements(_statements) {
}
void BlockAST::show() {
  std::cout << "(";
  for(auto s : statements) {
    if(s) s->show();
  }
  std::cout << ")";
}

FuncCallAST::FuncCallAST(AST *_callee, AST_vec _args):
  callee(_callee), args(_args) {
}
void FuncCallAST::show() {
  std::cout << "(call-func ";
  this->callee->show();
  for(auto arg : args) {
    std::cout << " (";
    arg->show();
    std::cout << ")";
  }
  std::cout << ")";
}

BinaryOpAST::BinaryOpAST(std::string _op, AST *_lhs, AST*_rhs):
  op(_op), lhs(_lhs), rhs(_rhs) {
}
void BinaryOpAST::show() {
  std::cout << "(binaryop " << op << " (";
  lhs->show();
  std::cout << ") (";
  rhs->show();
  std::cout << "))";
}

VariableAST::VariableAST(std::string _name):
  name(_name) {
}
void VariableAST::show() {
  std::cout << "(var " << name << ")";
}

INumberAST::INumberAST(int _number):
  number(_number) {
}
void INumberAST::show() {
  std::cout << number;
}

StringAST::StringAST(std::string _str):
  str(_str) {
}
void StringAST::show() {
  std::cout << "\"" << str << "\"";
}
