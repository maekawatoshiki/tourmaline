#pragma once 

#include "common.hpp"
#include "type.hpp"

enum {
  AST_FUNC_DEF,
  AST_FUNC_CALL,
};

class AST {
  public:
    virtual ~AST() = default;
    virtual int get_kind() const = 0;
    virtual void show() = 0;
};

typedef std::vector<AST *> AST_vec;

struct func_arg_t {
  func_arg_t(std::string _name, Type *_type):
    name(_name), type(_type) {};
  std::string name;
  Type *type;
};
typedef std::vector<func_arg_t *> func_args_t;

class FuncDefAST : AST {
  private:
    std::string name;
    func_args_t args;
    AST_vec body;
  public:
    FuncDefAST(std::string name, func_args_t args, AST_vec body);
    virtual int get_kind() const { return AST_FUNC_DEF; };
    virtual void show();
};

class FuncCallAST : AST {
  private:
    AST *callee;
    AST_vec args;
  public:
    FuncCallAST(AST *callee, AST_vec args);
    virtual int get_kind() const { return AST_FUNC_CALL; };
    virtual void show();
};
