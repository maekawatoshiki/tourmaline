#pragma once

#include "common.hpp"

// types (will increase from now on)
enum TypeKind {
  TYPEKIND_INT,
  TYPEKIND_FLOAT,
  TYPEKIND_STRING,
};

class Type {
  private:
    TypeKind kind;
  public:
    Type(TypeKind); 
};
