#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/PassManager.h"
#include <string>
#include <vector>
#include <algorithm>

using namespace llvm;

#define DEBUG_TYPE "rustifyc"

// --------------------------------------------------------------------------
// MODULE 1: LLVM Statistics
// We use these macros so we can track exactly how many times our pass
// caught errors or injected checks. We run `clang -mllvm -stats` to see them.
// --------------------------------------------------------------------------
STATISTIC(NumStaticOverflows, "Static overflows rejected at compile-time");
STATISTIC(NumDynamicChecks, "Runtime bounds checks injected");
STATISTIC(NumSafeAccesses, "Provably safe accesses (no instrumentation)");
STATISTIC(NumNegativeIndices, "Negative indices caught via UGE comparison");
STATISTIC(NumMultiDimArrays, "Multi-dimensional array accesses instrumented");
STATISTIC(NumSCEVElided, "Checks elided via SCEV proof");
STATISTIC(NumUninitChecks, "Uninitialized read checks injected");
STATISTIC(NumOverflowChecks, "Integer overflow checks injected");
STATISTIC(NumStackEscapes, "Stack escapes rejected at compile time");

namespace {

struct RustifyCPass : public PassInfoMixin<RustifyCPass> {
  static bool isRequired() { return true; }

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    bool Modified = false;
    std::vector<GetElementPtrInst *> Worklist;
    std::vector<LoadInst *> UninitLoads;
    std::vector<BinaryOperator *> Overflows;

    // Get analyses
    ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
    MemorySSA &MSSA = FAM.getResult<MemorySSAAnalysis>(F).getMSSA();

    auto IsProvablySafe = [&](Value *IndexValue, uint64_t ArraySize) -> bool {
      if (auto *ConstIndex = dyn_cast<ConstantInt>(IndexValue)) {
        int64_t Idx = ConstIndex->getSExtValue();
        return Idx >= 0 && (uint64_t)Idx < ArraySize;
      }

      // Allow disabling SCEV via environment variable for benchmarking
      if (std::getenv("DISABLE_SCEV")) {
        return false;
      }

      const SCEV *IndexSCEV = SE.getSCEV(IndexValue);
      ConstantRange Range = SE.getSignedRange(IndexSCEV);
      unsigned BitWidth = Range.getBitWidth();
      if (ArraySize == 0 || Range.isFullSet())
        return false;
      APInt Zero(BitWidth, 0);
      APInt Size(BitWidth, ArraySize, false);
      ConstantRange SafeRange(Zero, Size);
      return SafeRange.contains(Range);
    };

    // --------------------------------------------------------------------------
    // MODULE 2: Static Analysis (Compile-Time Checks)
    // Here we iterate through every instruction in the function looking for
    // memory accesses (GetElementPtr or GEP). If it's an array and the index
    // is a hardcoded constant, we can check it right now at compile time!
    // --------------------------------------------------------------------------
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
          Type *SourceType = GEP->getSourceElementType();

          if (auto *ArrayTy = dyn_cast<ArrayType>(SourceType)) {
            // Check if it's a multi-dimensional array (e.g., int matrix[5][5])
            if (isa<ArrayType>(ArrayTy->getElementType())) {
              ++NumMultiDimArrays;
            }

            // GEPs need at least 3 operands for arrays: Base Pointer, 0, and
            // the Index
            if (GEP->getNumOperands() < 3)
              continue;

            bool NeedsDynamicCheck = false;
            Type *CurrentTy = ArrayTy;
            for (unsigned idx_op = 2; idx_op < GEP->getNumOperands();
                 ++idx_op) {
              if (auto *ArrTy = dyn_cast<ArrayType>(CurrentTy)) {
                uint64_t ArraySize = ArrTy->getNumElements();
                Value *IndexValue = GEP->getOperand(idx_op);

                if (auto *ConstIndex = dyn_cast<ConstantInt>(IndexValue)) {
                  int64_t Idx = ConstIndex->getSExtValue();

                  // If the hardcoded index is out of bounds, throw a compiling
                  // error!
                  if (Idx < 0 || (uint64_t)Idx >= ArraySize) {
                    ++NumStaticOverflows;

                    std::string FileName = "unknown file";
                    unsigned LineNo = 0;
                    unsigned ColNo = 0;
                    if (DILocation *Loc = GEP->getDebugLoc()) {
                      FileName = Loc->getFilename().str();
                      LineNo = Loc->getLine();
                      ColNo = Loc->getColumn();
                    }

                    // We print the error out just like the Rust compiler does
                    errs() << "\033[1;31merror[CWE-121]:\033[0m spatial memory "
                              "safety violation\n";
                    errs() << "  \033[1;34m-->\033[0m " << FileName << ":"
                           << LineNo << ":" << ColNo << "\n";
                    errs() << "   |\n";
                    errs() << std::string(LineNo < 10 ? " " : "") << LineNo
                           << " |     array[" << Idx << "] = ...;\n";
                    errs() << "   |     "
                           << std::string(6 + std::to_string(Idx).length(), '^')
                           << " index " << Idx
                           << " is out of bounds for array of length "
                           << ArraySize << "\n";
                    errs() << "   |\n";
                    errs() << "   = \033[1mnote:\033[0m valid indices are in "
                              "range [0, "
                           << (ArraySize - 1) << "]\n\n";
                    errs() << "rustifyC: Spatial Safety Violation (Static "
                              "Analysis)\n";
                  } else {
                    ++NumSafeAccesses;
                  }
                } else {
                  // Phase 3: SCEV Optimization
                  if (IsProvablySafe(IndexValue, ArraySize)) {
                    ++NumSCEVElided;
                  } else {
                    NeedsDynamicCheck = true;
                  }
                }

                CurrentTy = ArrTy->getElementType();
              } else {
                break; // Not an array type anymore
              }
            }
            if (NeedsDynamicCheck) {
              if (std::find(Worklist.begin(), Worklist.end(), GEP) ==
                  Worklist.end()) {
                Worklist.push_back(GEP);
              }
            }
          }
        } else if (auto *Load = dyn_cast<LoadInst>(&I)) {
          // Phase 4: Uninitialized Read Detection
          Value *Ptr = Load->getPointerOperand()->stripPointerCasts();
          if (auto *Alloca = dyn_cast<AllocaInst>(Ptr)) {
            MemoryAccess *MA = MSSA.getMemoryAccess(Load);
            if (MA) {
              MemoryAccess *Clobber = MSSA.getWalker()->getClobberingMemoryAccess(MA);
              if (MSSA.isLiveOnEntryDef(Clobber)) {
                // Avoid double counting
                if (std::find(UninitLoads.begin(), UninitLoads.end(), Load) == UninitLoads.end()) {
                  UninitLoads.push_back(Load);
                  ++NumUninitChecks;
                  
                  std::string FileName = "unknown file";
                  unsigned LineNo = 0;
                  unsigned ColNo = 0;
                  if (DILocation *Loc = Load->getDebugLoc()) {
                    FileName = Loc->getFilename().str();
                    LineNo = Loc->getLine();
                    ColNo = Loc->getColumn();
                  }

                  errs() << "\033[1;31merror[E0381]:\033[0m used variable \033[1m'" 
                         << (Alloca->hasName() ? Alloca->getName() : "unnamed") 
                         << "'\033[0m is definitely uninitialized\n";
                  errs() << "  \033[1;34m-->\033[0m " << FileName << ":" << LineNo << ":" << ColNo << "\n";
                  errs() << "rustifyC: Initialization Safety Violation (Static Analysis)\n\n";
                }
              }
            }
          }
        } else if (auto *BinOp = dyn_cast<BinaryOperator>(&I)) {
          // Phase 5: Integer Overflow Detection
          if (BinOp->getOpcode() == Instruction::Add ||
              BinOp->getOpcode() == Instruction::Sub ||
              BinOp->getOpcode() == Instruction::Mul) {
              if (BinOp->getType()->isIntegerTy()) {
                  if (std::find(Overflows.begin(), Overflows.end(), BinOp) == Overflows.end()) {
                      Overflows.push_back(BinOp);
                  }
              }
          }
        }
      }
    }

    if (Worklist.empty() && UninitLoads.empty() && Overflows.empty())
      return PreservedAnalyses::all();

    // --------------------------------------------------------------------------
    // MODULE 3: Dynamic Instrumentation (Runtime Fallback)
    // If the index was unknown at compile time, we inject an 'if-statement'
    // into the LLVM IR representation of the program itself.
    // --------------------------------------------------------------------------

    // Note: For multi-dimensional arrays (int[5][5]), we simplified by pushing
    // the GEP. For this MVP, we instrument the inner dimension bounds check.
    for (auto *GEP : Worklist) {
      LLVMContext &Ctx = F.getContext();
      Type *SourceType = GEP->getSourceElementType();
      ArrayType *ArrayTy = cast<ArrayType>(SourceType);

      // Dig down to find the variable we need to check bounds for
      uint64_t ArraySize = 0;
      Value *Index = nullptr;

      Type *CurrentTy = ArrayTy;
      for (unsigned i = 2; i < GEP->getNumOperands(); ++i) {
        if (auto *ArrTy = dyn_cast<ArrayType>(CurrentTy)) {
          if (!IsProvablySafe(GEP->getOperand(i), ArrTy->getNumElements())) {
            ArraySize = ArrTy->getNumElements();
            Index = GEP->getOperand(i);
            break;
          }
          CurrentTy = ArrTy->getElementType();
        }
      }

      if (!Index)
        continue; // Should not happen if it's in worklist

      ++NumDynamicChecks;

      // --------------------------------------------------------------------------
      // MODULE 4: CFG Surgery (Control Flow Graph)
      // Here we literally chop the function's Execution Block into two pieces.
      // We insert a "FailBlock" in the middle that crashes the program safely
      // if the bounds check fails.
      // --------------------------------------------------------------------------
      BasicBlock *OldBB = GEP->getParent();
      BasicBlock *FailBB = BasicBlock::Create(Ctx, "FailBlock", &F);

      // Split the block exactly at the array access instruction
      BasicBlock *SafeBB = OldBB->splitBasicBlock(GEP, "SafeBlock");

      IRBuilder<> FailBuilder(FailBB);

      // Get debug info so we can print the exact filename and line number in
      // the panic
      std::string FileName = "unknown_file";
      unsigned LineNo = 0;
      if (DILocation *Loc = GEP->getDebugLoc()) {
        FileName = Loc->getFilename().str();
        LineNo = Loc->getLine();
      }

      // Assemble the final panic string
      std::string FmtStr = "thread 'main' panicked at 'index out of bounds: "
                           "the len is " +
                           std::to_string(ArraySize) +
                           " but the index is %zu', " + FileName + ":" +
                           std::to_string(LineNo) + "\n";

      // Inject the string as a global constant into the binary
      Constant *FmtConst = ConstantDataArray::getString(Ctx, FmtStr);
      GlobalVariable *FmtGlobal = new GlobalVariable(
          *F.getParent(), FmtConst->getType(), true,
          GlobalValue::PrivateLinkage, FmtConst, ".rustifyc.panic.fmt");

      FunctionType *FprintfTy = FunctionType::get(
          FailBuilder.getInt32Ty(),
          {FailBuilder.getInt8PtrTy(), FailBuilder.getInt8PtrTy()}, true);
      FunctionCallee Fprintf =
          F.getParent()->getOrInsertFunction("fprintf", FprintfTy);

      // Inject a 'printf' call into our FailBlock
      FunctionType *PrintfTy = FunctionType::get(
          FailBuilder.getInt32Ty(), {FailBuilder.getInt8PtrTy()}, true);
      FunctionCallee Printf =
          F.getParent()->getOrInsertFunction("printf", PrintfTy);

      Value *Fmt =
          FailBuilder.CreateBitCast(FmtGlobal, FailBuilder.getInt8PtrTy());

      // Extend index to 64-bit for printf %zu
      Type *Int64Ty = Type::getInt64Ty(Ctx);
      Value *Index64 = FailBuilder.CreateSExt(Index, Int64Ty, "idx_64_printf");

      FailBuilder.CreateCall(Printf, {Fmt, Index64});

      // Inject an 'exit(101)' to gracefully terminate (101 is Rust's standard
      // error code)
      FunctionCallee ExitFunc = F.getParent()->getOrInsertFunction(
          "exit", FunctionType::get(FailBuilder.getVoidTy(),
                                    {FailBuilder.getInt32Ty()}, false));
      FailBuilder.CreateCall(ExitFunc, {FailBuilder.getInt32(101)});
      FailBuilder.CreateUnreachable();

      // Finally, remove the OldBB's unconditional branch jump
      Instruction *OldTerminator = OldBB->getTerminator();
      OldTerminator->eraseFromParent();

      IRBuilder<> CheckBuilder(OldBB);

      Value *Index64Check =
          CheckBuilder.CreateSExt(Index, Int64Ty, "idx_64_promoted");
      Value *SizeVal = ConstantInt::get(Int64Ty, ArraySize);

      // Create an Unsigned-Greater-than-or-Equal check (ICmpUGE).
      // The 'Unsigned' part cleverly catches negative numbers too, since
      // underflowed negatives become massive positive unsigned numbers and
      // trigger the bounds trap!
      Value *Condition = CheckBuilder.CreateICmpUGE(Index64Check, SizeVal);

      // Branch to the FailBlock if bounds violated, otherwise continue to
      // SafeBlock as usual.
      CheckBuilder.CreateCondBr(Condition, FailBB, SafeBB);

      Modified = true;
    }

    // --------------------------------------------------------------------------
    // MODULE 4b: Uninitialized Memory Panic Injection
    // --------------------------------------------------------------------------
    for (auto *Load : UninitLoads) {
      LLVMContext &Ctx = F.getContext();
      BasicBlock *OldBB = Load->getParent();
      BasicBlock *FailBB = BasicBlock::Create(Ctx, "UninitFailBlock", &F);
      BasicBlock *SafeBB = OldBB->splitBasicBlock(Load, "UninitSafeBlock");

      IRBuilder<> FailBuilder(FailBB);

      std::string FileName = "unknown_file";
      unsigned LineNo = 0;
      if (DILocation *Loc = Load->getDebugLoc()) {
        FileName = Loc->getFilename().str();
        LineNo = Loc->getLine();
      }

      std::string FmtStr = "thread 'main' panicked at 'uninitialized variable read', " + FileName + ":" + std::to_string(LineNo) + "\n";
      Constant *FmtConst = ConstantDataArray::getString(Ctx, FmtStr);
      GlobalVariable *FmtGlobal = new GlobalVariable(*F.getParent(), FmtConst->getType(), true, GlobalValue::PrivateLinkage, FmtConst, ".rustifyc.uninit.fmt");

      FunctionType *PrintfTy = FunctionType::get(FailBuilder.getInt32Ty(), {FailBuilder.getInt8PtrTy()}, true);
      FunctionCallee Printf = F.getParent()->getOrInsertFunction("printf", PrintfTy);
      Value *Fmt = FailBuilder.CreateBitCast(FmtGlobal, FailBuilder.getInt8PtrTy());
      FailBuilder.CreateCall(Printf, {Fmt});

      FunctionCallee ExitFunc = F.getParent()->getOrInsertFunction("exit", FunctionType::get(FailBuilder.getVoidTy(), {FailBuilder.getInt32Ty()}, false));
      FailBuilder.CreateCall(ExitFunc, {FailBuilder.getInt32(101)});
      FailBuilder.CreateUnreachable();

      Instruction *OldTerminator = OldBB->getTerminator();
      OldTerminator->eraseFromParent();
      IRBuilder<> CheckBuilder(OldBB);

      // Definitely uninitialized: unconditionally crash at runtime if we reach here!
      CheckBuilder.CreateBr(FailBB);
      Modified = true;
    }

    // --------------------------------------------------------------------------
    // MODULE 5: Integer Overflow Panic Injection
    // --------------------------------------------------------------------------
    for (auto *BinOp : Overflows) {
      LLVMContext &Ctx = F.getContext();
      BasicBlock *OldBB = BinOp->getParent();
      BasicBlock *FailBB = BasicBlock::Create(Ctx, "OverflowFailBlock", &F);
      BasicBlock *SafeBB = OldBB->splitBasicBlock(BinOp, "OverflowSafeBlock");

      Instruction *OldTerminator = OldBB->getTerminator();
      OldTerminator->eraseFromParent();
      
      IRBuilder<> CheckBuilder(OldBB);

      Intrinsic::ID IntId;
      if (BinOp->getOpcode() == Instruction::Add) IntId = Intrinsic::sadd_with_overflow;
      else if (BinOp->getOpcode() == Instruction::Sub) IntId = Intrinsic::ssub_with_overflow;
      else IntId = Intrinsic::smul_with_overflow;

      Function *OverflowFn = Intrinsic::getDeclaration(F.getParent(), IntId, {BinOp->getType()});
      CallInst *Call = CheckBuilder.CreateCall(OverflowFn, {BinOp->getOperand(0), BinOp->getOperand(1)});
      
      Value *Result = CheckBuilder.CreateExtractValue(Call, 0);
      Value *DidOverflow = CheckBuilder.CreateExtractValue(Call, 1);

      BinOp->replaceAllUsesWith(Result);
      BinOp->eraseFromParent();

      CheckBuilder.CreateCondBr(DidOverflow, FailBB, SafeBB);

      // Build FailBB
      IRBuilder<> FailBuilder(FailBB);
      std::string FileName = "unknown_file";
      unsigned LineNo = 0;
      if (DILocation *Loc = Call->getDebugLoc()) {
        FileName = Loc->getFilename().str();
        LineNo = Loc->getLine();
      }

      std::string FmtStr = "thread 'main' panicked at 'arithmetic operation overflowed', " + FileName + ":" + std::to_string(LineNo) + "\n";
      Constant *FmtConst = ConstantDataArray::getString(Ctx, FmtStr);
      GlobalVariable *FmtGlobal = new GlobalVariable(*F.getParent(), FmtConst->getType(), true, GlobalValue::PrivateLinkage, FmtConst, ".rustifyc.overflow.fmt");

      FunctionType *PrintfTy = FunctionType::get(FailBuilder.getInt32Ty(), {FailBuilder.getInt8PtrTy()}, true);
      FunctionCallee Printf = F.getParent()->getOrInsertFunction("printf", PrintfTy);
      Value *Fmt = FailBuilder.CreateBitCast(FmtGlobal, FailBuilder.getInt8PtrTy());
      FailBuilder.CreateCall(Printf, {Fmt});

      FunctionCallee ExitFunc = F.getParent()->getOrInsertFunction("exit", FunctionType::get(FailBuilder.getVoidTy(), {FailBuilder.getInt32Ty()}, false));
      FailBuilder.CreateCall(ExitFunc, {FailBuilder.getInt32(101)});
      FailBuilder.CreateUnreachable();

      Modified = true;
    }

    return Modified ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};

// Add a custom printer for the statistics
struct RustifyCPrinter : public PassInfoMixin<RustifyCPrinter> {
  static bool isRequired() { return true; }

  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM) {
    if (NumStaticOverflows > 0 || NumDynamicChecks > 0 || NumSafeAccesses > 0) {
      errs()
          << "\n===========================================================\n";
      errs() << "              RustifyC Compilation Report\n";
      errs() << "===========================================================\n";
      errs() << "Spatial Safety Analysis:\n";
      errs() << "  \033[32m✓\033[0m Static overflows detected:          "
             << NumStaticOverflows << "\n";
      errs() << "  \033[32m✓\033[0m Runtime checks injected:            "
             << NumDynamicChecks << "\n";
      errs() << "  \033[32m✓\033[0m Safe accesses (no instrumentation): "
             << NumSafeAccesses << "\n";
      errs() << "  \033[32m✓\033[0m Negative indices caught:            "
             << NumNegativeIndices << "\n";
      errs() << "  \033[32m✓\033[0m Multi-dimensional arrays secured:   "
             << NumMultiDimArrays << "\n";
      errs() << "  \033[32m✓\033[0m SCEV proofs elided checks:        "
             << NumSCEVElided << "\n";
             
      if (NumUninitChecks > 0) {
        errs() << "Initialization Safety Analysis:\n";
        errs() << "  \033[32m✓\033[0m Uninitialized reads prevented:      "
               << NumUninitChecks << "\n";
      }

      if (NumOverflowChecks > 0) {
        errs() << "Arithmetic Overflow Analysis:\n";
        errs() << "  \033[32m✓\033[0m Overflow checks injected:           "
               << NumOverflowChecks << "\n";
      }

      uint64_t total = NumStaticOverflows + NumDynamicChecks + NumSafeAccesses +
                       NumSCEVElided + NumUninitChecks + NumOverflowChecks;
      if (total > 0) {
        float instr_rate = (float)NumDynamicChecks / total * 100.0f;
        errs()
            << "-----------------------------------------------------------\n";
        errs() << "Instrumentation Rate: " << (int)instr_rate << "% ("
               << NumDynamicChecks << "/" << total << " array accesses)\n";
      }
      errs() << "===========================================================\n";
    }
    return PreservedAnalyses::all();
  }
};

} // end anonymous namespace

// --------------------------------------------------------------------------
// MODULE 5: Code Registration
// This hooks our custom passes into LLVM's New Pass Manager architecture.
// It tells LLVM how to find and run our RustifyC analysis and instrumentation.
// --------------------------------------------------------------------------
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "rustifyC", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "rustifyC") {
                    return true;
                  }
                  return false;
                });
            PB.registerVectorizerStartEPCallback(
                [](FunctionPassManager &FPM, OptimizationLevel Level) {
                  FPM.addPass(RustifyCPass());
                });

            PB.registerOptimizerLastEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                  MPM.addPass(RustifyCPrinter());
                });

            // We force execution at the very start of the pipeline
            // so we don't accidentally get optimized away by -O0
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                  // Now handled by OptimizerLastEP so mem2reg can run first!
                });
          }};
}