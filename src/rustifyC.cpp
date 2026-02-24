#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct RustifyCPass : public PassInfoMixin<RustifyCPass> {

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    bool Modified = false;
    std::vector<GetElementPtrInst *> Worklist;

    // Get Scalar Evolution for the current function
    ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);

    // --- PHASE 1 & 2: ANALYSIS ---
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
          Type *SourceType = GEP->getSourceElementType();

          if (auto *ArrayTy = dyn_cast<ArrayType>(SourceType)) {
            // Week 6 Constraint: Must be a strict 1D Alloca Array Access
            if (GEP->getNumOperands() != 3) {
              continue;
            }

            uint64_t ArraySize = ArrayTy->getNumElements();
            Value *IndexValue = GEP->getOperand(2);

            std::string LocInfo = "unknown location";
            if (DILocation *Loc = GEP->getDebugLoc()) {
              LocInfo = "line " + std::to_string(Loc->getLine());
            }

            // --- WEEK 5 DELIVERABLE: STATIC OVERFLOW DETECTION ---
            if (auto *ConstIndex = dyn_cast<ConstantInt>(IndexValue)) {
              int64_t Idx = ConstIndex->getSExtValue();

              if (Idx < 0 || (uint64_t)Idx >= ArraySize) {
                errs() << "\n[rustifyC] ❌ CRITICAL ERROR: Static Buffer "
                          "Overflow Detected!\n";
                errs() << "    File: " << F.getParent()->getSourceFileName()
                       << ":" << LocInfo << "\n";
                errs() << "    Array Size: " << ArraySize
                       << " | Accessed Index: " << Idx << "\n";
                errs() << "    Action: Aborting compilation to prevent unsafe "
                          "binary.\n";
                report_fatal_error(
                    "rustifyC: Spatial Safety Violation (Static Analysis)");
              }
            }
            // --- WEEK 6-7 DELIVERABLE: DYNAMIC INSTRUMENTATION ---
            else {
              Worklist.push_back(GEP);
            }
          }
        }
      }
    }

    if (Worklist.empty())
      return PreservedAnalyses::all();

    // --- EXECUTION OF WEEKS 6-7 (Runtime Fallback) ---
    for (auto *GEP : Worklist) {
      LLVMContext &Ctx = F.getContext();
      uint64_t ArraySize =
          cast<ArrayType>(GEP->getSourceElementType())->getNumElements();
      Value *Index = GEP->getOperand(2);

      BasicBlock *OldBB = GEP->getParent();
      BasicBlock *FailBB = BasicBlock::Create(Ctx, "FailBlock", &F);

      // Split the block right AT the GEP instruction.
      // SafeBB now contains the GEP, the Store, and everything else.
      // OldBB now ends right before the GEP.
      BasicBlock *SafeBB = OldBB->splitBasicBlock(GEP, "SafeBlock");

      IRBuilder<> FailBuilder(FailBB);
      FunctionCallee AbortFunc =
          F.getParent()->getOrInsertFunction("abort", Type::getVoidTy(Ctx));
      CallInst *FailCall = FailBuilder.CreateCall(AbortFunc);
      FailCall->setDoesNotReturn();
      FailBuilder.CreateUnreachable();

      // The splitBasicBlock() automatically adds an unconditional 'br label
      // %SafeBB' to OldBB. We must remove this unconditional branch to replace
      // it with our conditional bounds check.
      Instruction *OldTerminator = OldBB->getTerminator();
      OldTerminator->eraseFromParent();

      IRBuilder<> CheckBuilder(OldBB);

      Type *Int64Ty = Type::getInt64Ty(Ctx);
      // Ensure the index is sign-extended to i64 to handle negative array
      // indices correctly
      Value *Index64 =
          CheckBuilder.CreateSExt(Index, Int64Ty, "idx_64_promoted");
      Value *SizeVal = ConstantInt::get(Int64Ty, ArraySize);

      // Check: if (Index >= ArraySize) goto FailBlock
      Value *Condition = CheckBuilder.CreateICmpUGE(Index64, SizeVal);

      // Branch to FailBlock if overflow, otherwise go to SafeBlock to execute
      // the GEP
      CheckBuilder.CreateCondBr(Condition, FailBB, SafeBB);

      errs() << "[rustifyC] 🛡️ Injected Runtime Check (Size: " << ArraySize
             << ")\n";
      Modified = true;
    }

    return Modified ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};

} // end anonymous namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "rustifyC", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
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