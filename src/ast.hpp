#pragma once 

#include "common.hpp"

enum {
  AST_NONE,
};

class AST {
  public:
    virtual ~AST() = default;
    virtual int get_type() const = 0;
    virtual void show() = 0;
};

typedef std::vector<AST *> AST_vec;
