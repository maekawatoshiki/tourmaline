#pragma once

#include "common.hpp"
#include "type.hpp"
#include "var.hpp"

struct func_t {
  func_t() {};
  func_t(std::string _name, Type *_ret_ty, Type_vec _args, llvm::Function *_func = nullptr):
    name(_name), ret_ty(_ret_ty), args(_args), func(_func) {};
  std::string name;
  Type *ret_ty;
  Type_vec args;
  llvm::Function *func;
  VariableMap varmap;
};

class FunctionMap {
  private:
    std::map<std::string, func_t *> funcmap;
  public:
    void add(std::string name, Type *retty, Type_vec args, llvm::Function * = nullptr);
    func_t *get(std::string name);
};
