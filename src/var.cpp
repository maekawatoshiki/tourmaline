#include "var.hpp"

void VariableMap::add(std::string name, Type *type) {
  varmap[name] = new var_t(name, type);
}

var_t *VariableMap::get(std::string name) {
  if(!varmap.count(name)) return nullptr;
  return varmap[name];
}
