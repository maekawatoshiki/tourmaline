#include "type.hpp"

Type::Type(TypeKind _kind):
  kind(_kind) {
}

TypeKind Type::get() {
  return kind;
}

void Type::set(Type *ty) {
  this->kind = ty->get();
}

std::string Type::to_string() {
  switch(kind) {
    case TYPEKIND_INT: return "int";
    case TYPEKIND_FLOAT: return "float";
    case TYPEKIND_STRING: return "string";
  }
  return "";
}

namespace TypeUtil {
  Type *to_type(llvm::Type *ty) {
    if(ty->isIntegerTy()) return new Type(TYPEKIND_INT);
    if(ty->isPointerTy() && 
        ty->getPointerElementType()->isIntegerTy(8)) 
      return new Type(TYPEKIND_STRING);
    if(ty->isDoubleTy()) return new Type(TYPEKIND_FLOAT);
    return nullptr;
  }
};
