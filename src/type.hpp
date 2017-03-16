#pragma once

#include "common.hpp"

// types (will increase from now on)
enum TypeKind {
  TYPEKIND_NONE,
  TYPEKIND_INT,
  TYPEKIND_FLOAT,
  TYPEKIND_STRING,
};

class Type {
  private:
    TypeKind kind;
  public:
    Type(TypeKind);

    TypeKind get();
    void set(Type *);

    std::string to_string();
};

typedef std::vector<Type *> Type_vec;

namespace TypeUtil {
  Type *to_type(llvm::Type *);
};
