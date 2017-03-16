#pragma once

#include "common.hpp"
#include "type.hpp"

struct var_t {
  var_t() {};
  var_t(std::string _name, Type *_type, llvm::Value *_val = nullptr):
    name(_name), type(_type), val(_val) {};
  std::string name;
  Type *type;
  llvm::Value *val;
};

class VariableMap {
  private:
    std::map<std::string, var_t *> varmap;
  public:
    void add(std::string, Type *);
    var_t *get(std::string);
};
