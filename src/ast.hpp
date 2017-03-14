#pragma once 

#include "common.hpp"
#include "type.hpp"

enum {
  AST_FUNC_DEF,
  AST_FUNC_CALL,
  AST_BINARY_OP,
  AST_VARIABLE,
  AST_INUMBER,
  AST_STRING,
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

class FuncDefAST : public AST {
  private:
  public:
    std::string name;
    func_args_t args;
    AST_vec body;
    FuncDefAST(std::string name, func_args_t args, AST_vec body);
    virtual int get_kind() const { return AST_FUNC_DEF; };
    virtual void show();
};

class FuncCallAST : public AST {
  private:
  public:
    AST *callee;
    AST_vec args;
    FuncCallAST(AST *callee, AST_vec args);
    virtual int get_kind() const { return AST_FUNC_CALL; };
    virtual void show();
};

class BinaryOpAST : public AST {
  private:
  public:
    AST *lhs, *rhs;
    std::string op;
    BinaryOpAST(std::string op, AST *lhs, AST *rhs);
    virtual int get_kind() const { return AST_BINARY_OP; };
    virtual void show();
};

class VariableAST : public AST {
  private:
  public:
    std::string name;
    VariableAST(std::string);
    virtual int get_kind() const { return AST_VARIABLE; };
    virtual void show();
};

class INumberAST : public AST {
  private:
  public:
    int number;
    INumberAST(int);
    virtual int get_kind() const { return AST_INUMBER; };
    virtual void show();
};

class StringAST : public AST {
  private:
  public:
    std::string str;
    StringAST(std::string);
    virtual int get_kind() const { return AST_STRING; };
    virtual void show();
};
