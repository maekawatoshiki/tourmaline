#include "type.hpp"

Type none_ty(Type::None), int_ty(Type::Int), float_ty(Type::Float), string_ty(Type::String);

Type::Type(Type::Kind _kind):
  kind(_kind) {
}

Type::Type(Type *_next):
  kind(Type::Array), next(_next) {
}

Type::Kind Type::get() {
  return kind;
}

Type *Type::get_next() {
  return next;
}

void Type::set(Type *ty) {
  this->kind = ty->get();
  this->next = ty->get_next();
}

std::string Type::to_string() {
  switch(kind) {
    case Type::None: return "none";
    case Type::Int: return "int";
    case Type::Float: return "float";
    case Type::String: return "string";
    case Type::Array: return next->to_string() + "[]";
  }
  return "";
}

llvm::Type *Type::to_llvmty() {
  switch(this->get()) {
    case Type::None:  return llvm::Type::getInt32Ty(llvm::getGlobalContext());
    case Type::Int:   return llvm::Type::getInt32Ty(llvm::getGlobalContext());
    case Type::Float: return llvm::Type::getDoubleTy(llvm::getGlobalContext());
    case Type::String:return llvm::Type::getInt8PtrTy(llvm::getGlobalContext());
    case Type::Array: return next->to_llvmty()->getPointerTo();
  }
  return nullptr;
}

namespace TypeUtil {
  Type *to_type(llvm::Type *ty) {
    if(ty->isIntegerTy()) return new Type(Type::Int);
    if(ty->isPointerTy() && 
        ty->getPointerElementType()->isIntegerTy(8)) 
      return new Type(Type::String);
    if(ty->isDoubleTy()) return new Type(Type::Float);
    if(ty->isPointerTy()) return new Type(to_type(ty->getPointerElementType()));
    return nullptr;
  }
  Type *to_type(std::string ty) {
    if(ty == "none") return new Type(Type::None);
    if(ty == "int") return new Type(Type::Int);
    if(ty == "float") return new Type(Type::Float);
    if(ty == "string") return new Type(Type::String);
    return nullptr;
  }
};
