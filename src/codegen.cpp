#include "codegen.hpp"
#include "JIT.hpp"

llvm::IRBuilder<> builder(llvm::getGlobalContext());
llvm::Module *mod;
llvm::DataLayout *data_layout;
llvm::LLVMContext context;
llvm::orc::TourmalineJIT *jit;

void Codegen::set_filename(std::string name) {
  filename = name;
}

Codegen::Codegen() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  jit = new llvm::orc::TourmalineJIT;
  mod = new llvm::Module("Tourmaline", context);
  mod->setDataLayout(jit->getTargetMachine().createDataLayout());
  data_layout = new llvm::DataLayout(mod);
  def_std_func();
}

void Codegen::def_std_func() {
  // define 'puts'
  auto puts_func_ty = llvm::FunctionType::get(
      builder.getInt32Ty(), std::vector<llvm::Type *>{
        builder.getInt8PtrTy()
      }, false);
  llvm::Function::Create(puts_func_ty, 
      llvm::Function::ExternalLinkage, "puts", mod);
}

void Codegen::gen(AST_vec ast) {
  // main func
  auto main_func_ty = llvm::FunctionType::get(
      builder.getInt32Ty(), std::vector<llvm::Type *>(), false);
  auto main_func = llvm::Function::Create(main_func_ty, 
      llvm::Function::ExternalLinkage, "main", mod);
  auto bb = llvm::BasicBlock::Create(context, "entry", main_func);
  builder.SetInsertPoint(bb);
  for(auto st : ast) {
    gen(st);
  }
  builder.CreateRet(make_int(0));
  mod->dump();

  jit->addModule(std::unique_ptr<llvm::Module>(mod));
  auto main_symbol = jit->findSymbol("main");
  int (*FP)() = (int (*)())main_symbol.getAddress();
  puts("\n### run 'main' ###");
  FP();
}

llvm::Value *Codegen::gen(AST *ast) {
  if(!ast) return nullptr;
  switch(ast->get_kind()) {
    case AST_FUNC_DEF:  break;
    case AST_IF:        return gen((IfAST *)ast);
    case AST_BLOCK:     return gen((BlockAST *)ast);
    case AST_FUNC_CALL: return gen((FuncCallAST *)ast);
    case AST_INUMBER:   return gen((INumberAST *)ast);
    case AST_STRING:    return gen((StringAST *)ast);
  }
  return nullptr;
}

llvm::Value *Codegen::gen(FuncCallAST *ast) {
  // TODO: tentative 'puts'. must fix
  if(static_cast<VariableAST *>(ast->get_callee())->get_name() == "puts") {
    std::vector<llvm::Value *> args;
    for(auto a : ast->get_args())
      args.push_back(gen(a));
    auto f = mod->getFunction("puts");
    if(!f) reporter.error("", 0, "%s %d f is null", __FILE__, __LINE__);
    return builder.CreateCall(f, args);
  }
  return nullptr;
}

llvm::Value *Codegen::gen(IfAST *ast) {
  auto cond = gen(ast->get_cond());
  cond = builder.CreateICmpNE(
      cond,
      cond->getType()->isPointerTy() ?
      llvm::ConstantPointerNull::getNullValue(cond->getType()) :
      make_int(0, cond->getType()));
  auto func = builder.GetInsertBlock()->getParent();

  llvm::BasicBlock *bb_then = llvm::BasicBlock::Create(context, "then", func);
  llvm::BasicBlock *bb_else = llvm::BasicBlock::Create(context, "else", func);
  llvm::BasicBlock *bb_merge= llvm::BasicBlock::Create(context, "merge",func);

  builder.CreateCondBr(cond, bb_then, bb_else);
  builder.SetInsertPoint(bb_then);

  // then_val and else_val are tentatibly 'i32 0' by default. 
  auto then_val = ast->get_then_st() ? gen(ast->get_then_st()) : make_int(0);
  builder.CreateBr(bb_merge);
  bb_then = builder.GetInsertBlock();
  
  builder.SetInsertPoint(bb_else);
  
  auto else_val = ast->get_else_st() ? gen(ast->get_else_st()) : make_int(0);
  builder.CreateBr(bb_merge);
  bb_else = builder.GetInsertBlock();

  builder.SetInsertPoint(bb_merge);
  
  llvm::PHINode *pnode = builder.CreatePHI(then_val->getType(), 2);
  pnode->addIncoming(then_val, bb_then);
  pnode->addIncoming(else_val, bb_else);
  return pnode; 
}

llvm::Value *Codegen::make_if(llvm::Value *cond, llvm::BasicBlock *bb_then, llvm::BasicBlock *bb_else) {
  return nullptr;
}

llvm::Value *Codegen::gen(BlockAST *ast) {
  auto stmts = ast->get_statements();
  llvm::Value *ret = make_int(0);
  for(auto stmt : stmts) {
    ret = gen(stmt);
  }
  return ret;
}

llvm::Value *Codegen::gen(INumberAST *ast) {
  return make_int(ast->get_number());
}

llvm::Value *Codegen::gen(StringAST *ast) {
  auto s = llvm::ConstantDataArray::getString(context, ast->get_str());
  llvm::GlobalVariable *gv = new llvm::GlobalVariable(*mod, s->getType(),
      true, llvm::GlobalValue::PrivateLinkage,
      s, "", nullptr,
      llvm::GlobalVariable::NotThreadLocal, 0);
  return builder.CreateInBoundsGEP(gv->getValueType(), gv, std::vector<llvm::Value *> { make_int(0), make_int(0) });
}

llvm::Value *Codegen::make_int(int n, llvm::Type *ty) {
  if(ty->isPointerTy())
    return llvm::ConstantPointerNull::get(static_cast<llvm::PointerType *>(ty));
  return llvm::ConstantInt::get(ty, n);
}

llvm::Value *Codegen::type_cast(llvm::Value *val, llvm::Type *to) {                
  if(val->getType()->isIntegerTy() && to->isIntegerTy()) {                         
    llvm::IntegerType *ival = (llvm::IntegerType *)val->getType();                 
    llvm::IntegerType *ito  = (llvm::IntegerType *)to;                             
    if(ival->getBitWidth() < ito->getBitWidth())                                   
      return builder.CreateZExtOrBitCast(val, to);                                 
  } else if(val->getType()->isIntegerTy() && to->isDoubleTy()) {                   
    return builder.CreateSIToFP(val, to);                                          
  } else if(val->getType()->isDoubleTy() && to->isIntegerTy()) {                   
    return builder.CreateFPToSI(val, to);                                          
  } else if(to->isVoidTy()) return val;                                            
  return builder.CreateTruncOrBitCast(val, to);                                    
}                                                                                  

