#include "ast.hpp"

FuncDefAST::FuncDefAST(std::string _name, func_args_t _args, AST_vec _body):
  name(_name), args(_args), body(_body) {
}
void FuncDefAST::show() {
  std::cout << "(def-func " << name << "(";
  for(auto arg : args) {
    std::cout << "(" << arg->type->to_string() << " " << arg->name << ") ";
  }
}
