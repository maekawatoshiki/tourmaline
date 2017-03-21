#pragma once

#include "common.hpp"
#include "ast.hpp"
#include "error.hpp"
#include "JIT.hpp"
#include "func.hpp"

extern llvm::IRBuilder<> builder;
extern llvm::Module *mod;
extern llvm::DataLayout *data_layout;
extern llvm::orc::TourmalineJIT *jit;

class Codegen {
  private:
    std::string filename;
    Reporter reporter;
    FunctionMap funcmap;
    func_t *cur_func = nullptr;
  public:
    Codegen();
    void set_filename(std::string);

    void def_std_func();
    void gen(AST_vec);
    llvm::Value *gen(AST *);
  
    llvm::Value *gen(FuncDefAST *);
    llvm::Value *gen(FuncCallAST *);
    llvm::Value *gen(IfAST *);
    llvm::Value *gen(BlockAST *);
    llvm::Value *gen(BinaryOpAST *);
    llvm::Value *gen(VariableAST *);
    llvm::Value *gen(INumberAST *);
    llvm::Value *gen(StringAST *);

    llvm::Value *make_if(llvm::Value *cond, llvm::BasicBlock *, llvm::BasicBlock *);
    llvm::Value *make_assign(AST *dst, AST *src);
    llvm::Value *make_add   (llvm::Value *dst, llvm::Value *src);
    llvm::Value *make_sub   (llvm::Value *dst, llvm::Value *src);
    llvm::Value *make_mul   (llvm::Value *dst, llvm::Value *src);
    llvm::Value *make_div   (llvm::Value *dst, llvm::Value *src);
    llvm::Value *make_eql   (llvm::Value *dst, llvm::Value *src);
    llvm::Value *make_ne    (llvm::Value *dst, llvm::Value *src);
    llvm::Value *make_lt    (llvm::Value *dst, llvm::Value *src);
    llvm::Value *make_gt    (llvm::Value *dst, llvm::Value *src);
    llvm::Value *make_le    (llvm::Value *dst, llvm::Value *src);
    llvm::Value *make_ge    (llvm::Value *dst, llvm::Value *src);

    llvm::Value *get_var_val(VariableAST *);
    llvm::Value *make_int(int, llvm::Type * = llvm::Type::getInt32Ty(llvm::getGlobalContext()));
    llvm::Value *type_cast(llvm::Value *, llvm::Type *);
    llvm::AllocaInst *create_entry_alloca(llvm::Function *, std::string &, llvm::Type *);
};

// th(is|ese) func will move to 'src/std_func.hpp'

extern "C" {
  void puts_va(const char *, ...);
  // TODO: tentative implementation. must fix
};
