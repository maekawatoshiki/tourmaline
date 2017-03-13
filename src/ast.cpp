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
