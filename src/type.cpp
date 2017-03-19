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
    case TYPEKIND_NONE: return "none";
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
  llvm::Type *to_type(Type *ty) {
    switch(ty->get()) {
      case TYPEKIND_NONE:  return llvm::Type::getInt32Ty(llvm::getGlobalContext());
      case TYPEKIND_INT:   return llvm::Type::getInt32Ty(llvm::getGlobalContext());
      case TYPEKIND_FLOAT: return llvm::Type::getDoubleTy(llvm::getGlobalContext());
      case TYPEKIND_STRING:return llvm::Type::getInt8PtrTy(llvm::getGlobalContext());
    }
    return nullptr;
  }
  Type *to_type(std::string ty) {
    if(ty == "none") return new Type(TYPEKIND_NONE);
    if(ty == "int") return new Type(TYPEKIND_INT);
    if(ty == "float") return new Type(TYPEKIND_FLOAT);
    if(ty == "string") return new Type(TYPEKIND_STRING);
    return nullptr;
  }
};
