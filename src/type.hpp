#pragma once

#include "common.hpp"

// types (will increase from now on)
class Type;
extern Type none_ty, int_ty, float_ty, string_ty;

class Type {
  public:
    enum Kind {
      None,
      Int,
      Float,
      String,
      Array,
    };
    Type(Kind);
    Type(Type *);

    Kind get();
    Type *get_next();
    void set(Type *);

    std::string to_string();
    llvm::Type *to_llvmty();

    static Type *get_none_ty()   { return &none_ty; }
    static Type *get_int_ty()    { return &int_ty; }
    static Type *get_float_ty()  { return &float_ty; }
    static Type *get_string_ty() { return &string_ty; }
  private:
    Kind kind;
    Type *next;
};

typedef std::vector<Type *> Type_vec;

namespace TypeUtil {
  Type *to_type(llvm::Type *);
  Type *to_type(std::string);
};
