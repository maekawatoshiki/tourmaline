#pragma once

#include "common.hpp"
#include "ast.hpp"
#include "error.hpp"
#include "JIT.hpp"

extern llvm::IRBuilder<> builder;
extern llvm::Module *mod;
extern llvm::DataLayout *data_layout;
extern llvm::orc::TourmalineJIT *jit;

class Codegen {
  private:
    std::string filename;
    Reporter reporter;
  public:
    Codegen();
    void set_filename(std::string);

    void def_std_func();
    void gen(AST_vec);
    llvm::Value *gen(AST *);
  
    llvm::Value *gen(FuncCallAST *);
    llvm::Value *gen(IfAST *);
    llvm::Value *gen(BlockAST *);
    llvm::Value *gen(INumberAST *);
    llvm::Value *gen(StringAST *);

    llvm::Value *make_int(int, llvm::Type * = llvm::Type::getInt32Ty(llvm::getGlobalContext()));
    llvm::Value *type_cast(llvm::Value *, llvm::Type *);
};
