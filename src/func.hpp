#pragma once

#include "common.hpp"
#include "type.hpp"
#include "var.hpp"

struct func_t {
  func_t() {};
  func_t(std::string _name, Type_vec _args, Type *_ret_ty = nullptr):
    name(_name), args(_args), ret_ty(_ret_ty) {};
  std::string name;
  Type_vec args;
  Type *ret_ty;
  VariableMap varmap;
};

class FunctionMap {
  private:
    std::map<std::string, func_t *> funcmap;
  public:
    void add(std::string name, Type_vec args, Type * = nullptr);
    func_t *get(std::string name);
};
