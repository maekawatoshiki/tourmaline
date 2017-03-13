#include "type.hpp"

Type::Type(TypeKind _kind):
  kind(_kind) {
}

TypeKind Type::get() {
  return kind;
}

std::string Type::to_string() {
  switch(kind) {
    case TYPEKIND_INT: return "int";
    case TYPEKIND_FLOAT: return "float";
    case TYPEKIND_STRING: return "string";
  }
  return "";
}
