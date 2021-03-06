#pragma once

#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>

#include "ir.h"
#include "diagnostics.h"
#include "bfvm.h"

namespace Backend::LLVM {
  // Converts various printable llvm types to a std::string
  template<typename T>
  std::string printRaw(T &value) {
    std::string out;
    llvm::raw_string_ostream stream(out);
    stream << value;
    stream.flush();
    return out;
  }

  struct ModuleCompiler {
    const BFVM::Config &config;
    llvm::TargetMachine &machine;
    llvm::LLVMContext &context;
    llvm::IRBuilder<> builder;
    llvm::Module &module;
    llvm::legacy::PassManager passManager;
    llvm::legacy::FunctionPassManager functionPassManager;

    llvm::Type *intType;
    llvm::Type *voidType;
    llvm::Type *contextPtrType;
    llvm::Type *sizeType;
    llvm::Type *cellType;
    llvm::Type *cellPtrType;

    llvm::FunctionType *putcharType;
    llvm::Function *putcharFunction;

    llvm::FunctionType *getcharType;
    llvm::Function *getcharFunction;

    llvm::FunctionType *fragmentType;

    std::vector<IR::Inst*> pendingPhis;

    llvm::Value *regValues[IR::NUM_REGS];

    DIAG_DECL()

    explicit ModuleCompiler(
      const BFVM::Config &config,
      llvm::TargetMachine &machine,
      llvm::LLVMContext &context,
      llvm::Module &module
    );

    void optimize();

    // Gets the llvm type of an IR type
    llvm::Type *convertType(IR::TypeId typeId);

    // Compiles this IR graph into the current llvm module
    void compileGraph(IR::Graph &graph, const std::string &name);

    void compileBlock(IR::Block &block);
    llvm::Value *compileInst(IR::Inst *inst);
    llvm::Value *getValue(IR::Inst *inst, llvm::Type *type = nullptr);
  };
}