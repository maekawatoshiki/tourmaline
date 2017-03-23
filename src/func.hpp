#pragma once

#include "common.hpp"
#include "type.hpp"
#include "var.hpp"

struct func_t {
  func_t() {};
  func_t(std::string _name, Type *_ret_ty, Type_vec _args, llvm::Function *_func = nullptr, bool _varg = false):
    name(_name), ret_ty(_ret_ty), args(_args), func(_func), varg(_varg) {};
  std::string name;
  Type *ret_ty;
  Type_vec args;
  llvm::Function *func;
  bool varg;
  VariableMap varmap;
};

class FunctionMap {
  private:
    std::map<std::string, func_t *> funcmap;
  public:
    void add(std::string name, Type *retty, Type_vec args, llvm::Function * = nullptr, bool varg = false);
    func_t *get(std::string name);
};
