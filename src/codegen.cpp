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
      builder.getVoidTy(), std::vector<llvm::Type *>{
        builder.getInt8PtrTy()
      }, true);
  funcmap.add("puts", Type::get_none_ty(), Type_vec{Type::get_string_ty()}, 
        llvm::Function::Create(puts_func_ty, 
          llvm::Function::ExternalLinkage, "puts_va", mod), true);
  // define 'print'
  auto print_func_ty = llvm::FunctionType::get(
      builder.getVoidTy(), std::vector<llvm::Type *>{
        builder.getInt8PtrTy()
      }, true);
  funcmap.add("print", Type::get_none_ty(), Type_vec{Type::get_string_ty()}, 
        llvm::Function::Create(print_func_ty, 
          llvm::Function::ExternalLinkage, "print_va", mod), true);
}

void Codegen::gen(AST_vec ast) {
  // define functions excepting main function
  std::list<AST *> main_body;
  std::list<AST *> funcs_list;
  for(auto st : ast) {
    if(st->get_kind() == AST_FUNC_DEF) {
      gen(st); // only register the func to funcmap. do not generate the body of func
      funcs_list.push_back(st);
    } else main_body.push_back(st);
  }
  for(auto func : funcs_list) gen(func); // create body

  // main func
  auto main_func_ty = llvm::FunctionType::get(
      builder.getInt32Ty(), std::vector<llvm::Type *>(), false);
  auto main_func = llvm::Function::Create(main_func_ty, 
      llvm::Function::ExternalLinkage, "main", mod);
  funcmap.add("main", Type::get_int_ty(), Type_vec{}, main_func);
  cur_func = funcmap.get("main");

  auto bb = llvm::BasicBlock::Create(context, "entry", main_func);
  builder.SetInsertPoint(bb);
  for(auto st : main_body) {
    gen(st);
  }
  builder.CreateRet(make_int(0));
  llvm::verifyModule(*mod);

  jit->addModule(mod);//std::unique_ptr<llvm::Module>(mod));
  auto main_symbol = jit->findSymbol("main");
  int (*FP)() = (int (*)())main_symbol.getAddress();
  puts("\n### run 'main' ###");
  FP();
}

llvm::Value *Codegen::gen(AST *ast) {
  if(!ast) return nullptr;
  switch(ast->get_kind()) {
    case AST_FUNC_DEF:  return gen((FuncDefAST *)ast);
    case AST_IF:        return gen((IfAST *)ast);
    case AST_BLOCK:     return gen((BlockAST *)ast);
    case AST_FUNC_CALL: return gen((FuncCallAST *)ast);
    case AST_BINARY_OP: return gen((BinaryOpAST *)ast);
    case AST_INDEX:     return gen((IndexAST *)ast);
    case AST_ARRAY:     return gen((ArrayAST *)ast);
    case AST_VARIABLE:  return gen((VariableAST *)ast);
    case AST_INUMBER:   return gen((INumberAST *)ast);
    case AST_STRING:    return gen((StringAST *)ast);
  }
  reporter.error("", 0, "unknown AST");
  return nullptr;
}

llvm::Value *Codegen::gen(FuncDefAST *ast) {
  func_t *func = nullptr;
  if(funcmap.get(ast->get_name())) { // second visit. 
    func = funcmap.get(ast->get_name());
  } else { // first visit
    auto llvm_func_ty = llvm::FunctionType::get(ast->get_ret_ty()->to_llvmty(), 
        [&]() -> std::vector<llvm::Type *> {
          std::vector<llvm::Type *> args;
          for(auto arg : ast->get_args()) {
            args.push_back(arg->type->to_llvmty());
          }
          return args;
        }(), false);
    auto llvm_func = llvm::Function::Create(llvm_func_ty, 
        llvm::GlobalValue::ExternalLinkage, ast->get_name(), mod);
    funcmap.add(ast->get_name(), ast->get_ret_ty(), [&]() -> Type_vec {
      Type_vec types;
      for(auto t : ast->get_args())
        types.push_back(t->type);
      return types;
    }(), llvm_func);

    return llvm_func;
  }

  { // create function body
    auto tmp_cur_func = cur_func;
    cur_func = func;

    llvm::BasicBlock *entry = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", func->func);
    builder.SetInsertPoint(entry);

    for(auto arg : ast->get_args()) {
      cur_func->varmap.add(arg->name, arg->type);
    }

    auto llvm_arg_it = func->func->arg_begin();
    for(size_t i = 0; i < ast->get_args().size(); i++) {
      std::string &name = ast->get_args()[i]->name;
      llvm_arg_it->setName(name);
      llvm::AllocaInst *ainst = create_entry_alloca(func->func, name, llvm_arg_it->getType());
      builder.CreateStore(&*llvm_arg_it, ainst);
      var_t *v = cur_func->varmap.get(name);
      if(v) v->val = ainst;
      llvm_arg_it++;
    }

    auto ret = gen(ast->get_body());
    builder.CreateRet(ret);

    cur_func = tmp_cur_func;
  }
  return func->func;
}

llvm::Value *Codegen::gen(FuncCallAST *ast) {
  std::vector<llvm::Value *> args;
  {
    llvm::Function *func = nullptr;
    if(VariableAST *vast = static_cast<VariableAST *>(ast->get_callee())) {
      auto f = funcmap.get(vast->get_name());
      if(!f) reporter.error(filename, 0, "not found function '%s'", vast->get_name().c_str());
      func = f->func;
      if(f->varg) { // variable-length args
        size_t i = 0, minimum_args_size = f->args.size();
        for(i = 0; i < minimum_args_size; i++)
          args.push_back( type_cast(gen(ast->get_args()[i]), f->args[i]->to_llvmty()) );

        size_t args_size = ast->get_args().size();
        for(; i < args_size; i++) {
          auto val = gen(ast->get_args()[i]);
          args.push_back(llvm::ConstantInt::get(builder.getInt64Ty(), (uint64_t)retty_last_stmt));
          args.push_back(val);
        }
      } else {
        size_t args_size = ast->get_args().size();
        for(size_t i = 0; i < args_size; i++) 
          args.push_back( type_cast(gen(ast->get_args()[i]), f->args[i]->to_llvmty()) );
      }
    }
    return builder.CreateCall(func, args);
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
 
  if(then_val->getType()->isVoidTy() || else_val->getType()->isVoidTy()) {
    return make_int(0);
  } 

  llvm::PHINode *pnode = builder.CreatePHI(then_val->getType(), 2);
  pnode->addIncoming(then_val, bb_then);
  pnode->addIncoming(else_val, bb_else);

  retty_last_stmt = TypeUtil::to_type(pnode->getType());
  return pnode; 
}

llvm::Value *Codegen::gen(BlockAST *ast) {
  auto stmts = ast->get_statements();
  llvm::Value *ret = make_int(0);
  for(auto stmt : stmts) {
    ret = gen(stmt);
  }
  return ret;
}

llvm::Value *Codegen::gen(BinaryOpAST *ast) {
  auto op = ast->get_op();
  if(op == "&&" || op == "||" || op == "=") {
    if(op == "=") return make_assign(ast->get_lhs(), ast->get_rhs());
  } else {
    auto lhs = gen(ast->get_lhs()),
         rhs = gen(ast->get_rhs());
    if(op == "+") return make_add   (lhs, rhs);
    if(op == "-") return make_sub   (lhs, rhs);
    if(op == "*") return make_mul   (lhs, rhs);
    if(op == "/") return make_div   (lhs, rhs);
    if(op =="==") return make_eql   (lhs, rhs);
    if(op =="!=") return make_ne    (lhs, rhs);
    if(op == "<") return make_lt    (lhs, rhs);
    if(op == ">") return make_gt    (lhs, rhs);
    if(op =="<=") return make_le    (lhs, rhs);
    if(op ==">=") return make_ge    (lhs, rhs);
  }
  reporter.error("", 0, "unknown operator %s %d", __FILE__, __LINE__);
  return nullptr;
}

llvm::Value *Codegen::gen(IndexAST *ast) {
  auto dst = gen(ast->get_dst());
  assert(retty_last_stmt->get() == Type::Array);
  auto retty = retty_last_stmt->get_next();
  auto idx = gen(ast->get_idx());
  retty_last_stmt = retty;
  return builder.CreateLoad(get_element_val(dst, idx));
}

llvm::Value *Codegen::gen(ArrayAST *ast) {
  std::vector<llvm::Value *> ary_vals;
  Type *elem_ty = nullptr;
  for(auto elem : ast->get_elements()) {
    ary_vals.push_back( gen(elem) );
    if(!elem_ty) elem_ty = retty_last_stmt;
  }
  auto ary_ty = new Type(elem_ty);
  uint64_t *ary_ptr = (uint64_t *)calloc(ast->get_elements().size(), sizeof(int));
  llvm::Value *ary = llvm::ConstantExpr::getIntToPtr(
    llvm::ConstantInt::get(builder.getInt64Ty(), (uint64_t)ary_ptr), ary_ty->to_llvmty());
  for(size_t i = 0; i < ast->get_elements().size(); i++) {
    builder.CreateStore(ary_vals[i], get_element_val(ary, i));
  }
  retty_last_stmt = ary_ty;
  return ary;
}

llvm::Value *Codegen::gen(VariableAST *ast) {
  auto name = ast->get_name();
  auto v = cur_func->varmap.get(name);
  retty_last_stmt = v->type;
  if(!v) reporter.error(filename, 0, "undefined var '%s'", name.c_str());
  return builder.CreateLoad(v->val);
}

llvm::Value *Codegen::gen(INumberAST *ast) {
  retty_last_stmt = Type::get_int_ty();
  return make_int(ast->get_number());
}

llvm::Value *Codegen::gen(StringAST *ast) {
  retty_last_stmt = Type::get_string_ty();
  // auto s = llvm::ConstantDataArray::getString(context, ast->get_str());
  // llvm::GlobalVariable *gv = new llvm::GlobalVariable(*mod, s->getType(),
  //     true, llvm::GlobalValue::PrivateLinkage,
  //     s, "", nullptr,
  //     llvm::GlobalVariable::NotThreadLocal, 0);
  // return builder.CreateInBoundsGEP(gv->getValueType(), gv, std::vector<llvm::Value *> { make_int(0), make_int(0) });
  return  builder.CreateGlobalStringPtr(ast->get_str());
}

llvm::Value *Codegen::make_if(llvm::Value *cond, llvm::BasicBlock *bb_then, llvm::BasicBlock *bb_else) {
  return nullptr;
}

llvm::Value *Codegen::make_assign(AST *dst, AST *src) {
  auto src_val = gen(src);
  auto type = retty_last_stmt;
  llvm::Value *dst_val = nullptr;
  if(VariableAST *vast = static_cast<VariableAST *>(dst)) {
    auto name = vast->get_name();
    if(!cur_func->varmap.get(name)) {
      cur_func->varmap.add(name, type);
      auto v = cur_func->varmap.get(name);
      v->val = builder.CreateAlloca(src_val->getType(), nullptr, name);
    }
    dst_val = get_var_val(vast);
  } else {
    // TODO: tentative
    dst_val = gen(dst);
  }
  return builder.CreateStore( type_cast(src_val, dst_val->getType()->getPointerElementType()), dst_val);
}
#define IS_INT_EXPR (lhs->getType()->isIntegerTy())
llvm::Value *Codegen::make_add(llvm::Value *lhs, llvm::Value *rhs) {
  if(IS_INT_EXPR) {
    return builder.CreateAdd(lhs, rhs);
  }
  return nullptr;
}
llvm::Value *Codegen::make_sub(llvm::Value *lhs, llvm::Value *rhs) {
  if(IS_INT_EXPR) {
    return builder.CreateSub(lhs, rhs);
  }
  return nullptr;
}
llvm::Value *Codegen::make_mul(llvm::Value *lhs, llvm::Value *rhs) {
  if(IS_INT_EXPR) {
    return builder.CreateMul(lhs, rhs);
  }
  return nullptr;
}
llvm::Value *Codegen::make_div(llvm::Value *lhs, llvm::Value *rhs) {
  if(IS_INT_EXPR) {
    return builder.CreateSDiv(lhs, rhs);
  }
  return nullptr;
}
llvm::Value *Codegen::make_eql(llvm::Value *lhs, llvm::Value *rhs) {
  if(IS_INT_EXPR) {
    return builder.CreateICmpEQ(lhs, rhs);
  }
  return nullptr;
}
llvm::Value *Codegen::make_ne(llvm::Value *lhs, llvm::Value *rhs) {
  if(IS_INT_EXPR) {
    return builder.CreateICmpNE(lhs, rhs);
  }
  return nullptr;
}
llvm::Value *Codegen::make_lt(llvm::Value *lhs, llvm::Value *rhs) {
  if(IS_INT_EXPR) {
    return builder.CreateICmpSLT(lhs, rhs);
  }
  return nullptr;
}
llvm::Value *Codegen::make_gt(llvm::Value *lhs, llvm::Value *rhs) {
  if(IS_INT_EXPR) {
    return builder.CreateICmpSGT(lhs, rhs);
  }
  return nullptr;
}
llvm::Value *Codegen::make_le(llvm::Value *lhs, llvm::Value *rhs) {
  if(IS_INT_EXPR) {
    return builder.CreateICmpSLE(lhs, rhs);
  }
  return nullptr;
}
llvm::Value *Codegen::make_ge(llvm::Value *lhs, llvm::Value *rhs) {
  if(IS_INT_EXPR) {
    return builder.CreateICmpSGE(lhs, rhs);
  }
  return nullptr;
}
#undef IS_INT_EXPR

llvm::Value *Codegen::get_element_val(llvm::Value *ary, int idx) {
  return get_element_val(ary, make_int(idx));
}

llvm::Value *Codegen::get_element_val(llvm::Value *ary, llvm::Value *idx) {
  return llvm::GetElementPtrInst::CreateInBounds(              
      ary,                                                       
      llvm::ArrayRef<llvm::Value *>{idx}, "elem", builder.GetInsertBlock());  
}

llvm::Value *Codegen::get_var_val(VariableAST *ast) {
  auto name = ast->get_name();
  auto v = cur_func->varmap.get(name);
  if(!v) return nullptr;
  return v->val;
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

llvm::AllocaInst *Codegen::create_entry_alloca(llvm::Function *TheFunction, std::string &VarName, llvm::Type *type) {
  llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
      TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(type == nullptr ? llvm::Type::getInt32Ty(context) : type, 0, VarName.c_str());
}


// standard func
extern "C" {
  void puts_va(char *str, ...) {
    va_list args;
    va_start(args, str);
    {
      for(char *p = str; *p; p++) {

      if(*p == '{' && *(++p) == '}') {
        Type *ty = va_arg(args, Type *);
        switch(ty->get()) {
          case Type::None: break;
          case Type::Int:    printf("%d", va_arg(args, int)); break;
          case Type::Float:  printf("%.15g", va_arg(args, double)); break;
          case Type::String: printf("%s", va_arg(args, char *)); break;
          case Type::Array:  printf("Array<0x%lx>", va_arg(args, uint64_t)); break;
        }
      } else putchar(*p);

      }
      puts("");
    }
    va_end(args);
  }
  void print_va(char *str, ...) {
    va_list args;
    va_start(args, str);
    {
      for(char *p = str; *p; p++) {

      if(*p == '{' && *(++p) == '}') {
        Type *ty = va_arg(args, Type *);
        switch(ty->get()) {
          case Type::None: break;
          case Type::Int:    printf("%d", va_arg(args, int)); break;
          case Type::Float:  printf("%.15g", va_arg(args, double)); break;
          case Type::String: printf("%s", va_arg(args, char *)); break;
          case Type::Array:  printf("Array<0x%lx>", va_arg(args, uint64_t)); break;
        }
      } else putchar(*p);

      }
    }
    va_end(args);
  }
};
