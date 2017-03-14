#include "ast.hpp"

FuncDefAST::FuncDefAST(std::string _name, func_args_t _args, AST_vec _body):
  name(_name), args(_args), body(_body) {
}
void FuncDefAST::show() {
  std::cout << "(def-func " << name;
  for(auto arg : args) {
    std::cout << " (" << arg->type->to_string() << " " << arg->name << ")";
  }
  std::cout << ")" << std::endl;
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
  std::cout << ")" << std::endl;
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
