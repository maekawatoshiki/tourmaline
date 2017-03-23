#include "func.hpp"

void FunctionMap::add(std::string name, Type *ret_ty, Type_vec args, llvm::Function *func, bool varg) {
  funcmap[name] = new func_t(name, ret_ty, args, func, varg);
}

func_t *FunctionMap::get(std::string name) {
  if(!funcmap.count(name)) return nullptr;
  return funcmap[name];
}

