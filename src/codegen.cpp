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
  // mod->dump();

  jit->addModule(std::unique_ptr<llvm::Module>(mod));
  auto main_symbol = jit->findSymbol("main");
  int (*FP)() = (int (*)())main_symbol.getAddress();
  puts("### run 'main' ###");
  FP();
}

llvm::Value *Codegen::gen(AST *ast) {
  switch(ast->get_kind()) {
    case AST_FUNC_DEF:  break;
    case AST_FUNC_CALL: return gen((FuncCallAST *)ast);
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
