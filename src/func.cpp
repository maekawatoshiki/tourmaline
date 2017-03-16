#include "func.hpp"

void FunctionMap::add(std::string name, Type_vec args, Type *ret_ty) {
  funcmap[name] = new func_t(name, args, ret_ty);
}

func_t *FunctionMap::get(std::string name) {
  if(!funcmap.count(name)) return nullptr;
  return funcmap[name];
}

