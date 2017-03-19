#pragma once 

#include "common.hpp"
#include "type.hpp"

enum {
  AST_FUNC_DEF,
  AST_FUNC_CALL,
  AST_IF,
  AST_BLOCK,
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
    std::string name;
    Type *ret_ty;
    func_args_t args;
    AST *body;
  public:
    FuncDefAST(std::string name, Type *ret_ty, func_args_t args, AST *body);
    virtual int get_kind() const { return AST_FUNC_DEF; };
    virtual void show();

    std::string get_name() const { return name; };
    func_args_t get_args() const { return args; };
    Type       *get_ret_ty() const { return ret_ty; };
    AST*        get_body() const { return body; };
};

class IfAST : public AST {
  private:
    AST *cond, *then_st, *else_st;
  public:
    IfAST(AST *, AST *, AST * = nullptr);
    virtual int get_kind() const { return AST_IF; };
    virtual void show();

    AST *get_cond() const { return cond; };
    AST *get_then_st() const { return then_st; };
    AST *get_else_st() const { return else_st; };
};

class BlockAST : public AST {
  private: 
    AST_vec statements;
  public:
    BlockAST(AST_vec);
    virtual int get_kind() const { return AST_BLOCK; };
    virtual void show();

    AST_vec get_statements() const { return statements; };
};

class FuncCallAST : public AST {
  private:
    AST *callee;
    AST_vec args;
  public:
    FuncCallAST(AST *callee, AST_vec args);
    virtual int get_kind() const { return AST_FUNC_CALL; };
    virtual void show();
    AST    *get_callee() const { return callee; };
    AST_vec get_args()   const { return args; };
};

class BinaryOpAST : public AST {
  private:
    AST *lhs, *rhs;
    std::string op;
  public:
    BinaryOpAST(std::string op, AST *lhs, AST *rhs);
    virtual int get_kind() const { return AST_BINARY_OP; };
    virtual void show();

    AST         *get_lhs() const { return lhs; };
    AST         *get_rhs() const { return rhs; };
    std::string  get_op()  const { return op; };
};

class VariableAST : public AST {
  private:
    std::string name;
  public:
    VariableAST(std::string);
    virtual int get_kind() const { return AST_VARIABLE; };
    virtual void show();

    std::string get_name() const { return name; };
};

class INumberAST : public AST {
  private:
    int number;
  public:
    INumberAST(int);
    virtual int get_kind() const { return AST_INUMBER; };
    virtual void show();

    int get_number() const { return number; };
};

class StringAST : public AST {
  private:
    std::string str;
  public:
    StringAST(std::string);
    virtual int get_kind() const { return AST_STRING; };
    virtual void show();

    std::string get_str() const { return str; };
};
