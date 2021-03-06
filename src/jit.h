#pragma once

#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/Legacy.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/IR/Mangler.h>

#include "ir.h"
#include "backend_llvm.h"
#include "diagnostics.h"

namespace JIT {
  void init();

  typedef char* (*EntryFn)(void*, char*);

  struct Linker {
    const BFVM::Config &config;
    llvm::TargetMachine &machine;
    const llvm::DataLayout dataLayout;
    llvm::LLVMContext &context;
    llvm::orc::ExecutionSession session;
    std::shared_ptr<llvm::orc::SymbolResolver> resolver;
    llvm::orc::LegacyRTDyldObjectLinkingLayer objectLayer;
    llvm::orc::LegacyIRCompileLayer<decltype(objectLayer), llvm::orc::SimpleCompiler> compileLayer;
    std::unordered_map<std::string, llvm::JITTargetAddress> symbols;

    DIAG_DECL()

    Linker(
      const BFVM::Config &config,
      llvm::TargetMachine &machine,
      llvm::LLVMContext &context
    );

    std::string mangle(const std::string &name);

    llvm::orc::VModuleKey addModule(std::unique_ptr<llvm::Module> module);
    void removeModule(llvm::orc::VModuleKey key);
    EntryFn findEntry(const std::string& name);
  };

  struct Pipeline;
  struct Handle : public BFVM::Handle {
    llvm::orc::VModuleKey key;
    Pipeline &pipeline;
    EntryFn entry;

    Handle(
      llvm::orc::VModuleKey key,
      Pipeline &pipeline,
      EntryFn entry
    );

    char *operator()(void *context, char *memory) override;

    ~Handle() override;
  };

  struct Pipeline {
    const BFVM::Config &config;
    std::unique_ptr<llvm::TargetMachine> machine;
    llvm::LLVMContext context;
    Linker linker;

    DIAG_DECL()

    explicit Pipeline(const BFVM::Config &config);

    std::unique_ptr<BFVM::Handle> compile(IR::Graph &graph, const std::string &name);

    template<typename T> void addSymbol(const std::string& name, T *pointer) {
      linker.symbols[name] = llvm::pointerToJITTargetAddress(pointer);
    }
  };
}