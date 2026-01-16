#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

namespace {

struct RustifyCPass : public PassInfoMixin<RustifyCPass> {
  
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    // DEBUG POINT 2: The pass is actually running
    errs() << "[rustifyC] 🟢 RUNNING ON: " << F.getName() << "\n";

    for (auto &BB : F) {
      for (auto &I : BB) {
        if (I.getOpcodeName() == StringRef("getelementptr")) {
             errs() << "[rustifyC]   🔥 FOUND GEP INSTRUCTION!\n";
        }
      }
    }
    return PreservedAnalyses::all();
  }
};

} // end anonymous namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "rustifyC", LLVM_VERSION_STRING,
    [](PassBuilder &PB) {
      // DEBUG POINT 1: The plugin is being registered
      errs() << "[rustifyC] 🔌 PLUGIN REGISTERED SUCCESSFULLY!\n";
      
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "rustifyC") {
             FPM.addPass(RustifyCPass());
             return true;
          }
          return false;
        });
    }};
}