#pragma once

#include <string>
#include <memory>

#include "tape_memory.h"

namespace BFVM {
  struct Config {
    Memory::Config memory;
    std::string dump;
    int cellWidth = 8;
    uint32_t eofValue = 0;
#ifndef NDIAG
    int profile = 0;
    bool quiet = false;
#endif
  };

  struct Handle {
    virtual char* operator()(void*, char*) = 0;
    virtual ~Handle() = default;
  };

  struct Interpreter {
    static std::unique_ptr<Interpreter> initialize(const Config &config);
    virtual std::unique_ptr<BFVM::Handle> compile(const std::string &code, const std::string &name) = 0;
    virtual void run(BFVM::Handle &handle) = 0;
  protected:
    explicit Interpreter();
  };

  void run(const std::string &code, const Config &config = {});
}