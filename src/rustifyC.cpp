#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h" // <--- CRITICAL FOR INJECTING CODE

using namespace llvm;

namespace {

struct RustifyCPass : public PassInfoMixin<RustifyCPass> {
  
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    bool Modified = false; // Track if we changed anything
    
    // We collect GEPs first to avoid iterator invalidation issues
    std::vector<GetElementPtrInst*> Worklist;

    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
            Type *SourceType = GEP->getSourceElementType();
            if (auto *ArrayTy = dyn_cast<ArrayType>(SourceType)) {
                // Get the last operand (the index)
                Value *IndexValue = GEP->getOperand(GEP->getNumOperands() - 1);
                
                // Only instrument if it's NOT a constant number
                if (!isa<ConstantInt>(IndexValue)) {
                    Worklist.push_back(GEP);
                }
            }
        }
      }
    }

    if (Worklist.empty()) return PreservedAnalyses::all();

    errs() << "[rustifyC] 🛡️ Injecting Runtime Checks for " << Worklist.size() << " accesses in " << F.getName() << "...\n";

    for (auto *GEP : Worklist) {
        LLVMContext &Ctx = F.getContext();
        uint64_t ArraySize = cast<ArrayType>(GEP->getSourceElementType())->getNumElements();
        Value *Index = GEP->getOperand(GEP->getNumOperands() - 1);

        // 1. Setup the split
        BasicBlock *OldBB = GEP->getParent();
        Instruction *NextInst = GEP->getNextNode(); // Code after the GEP
        
        // 2. Create the Fail Block (The Trap)
        BasicBlock *FailBB = BasicBlock::Create(Ctx, "FailBlock", &F);
        IRBuilder<> FailBuilder(FailBB);
        
        // Declare abort() if not exists
        FunctionCallee AbortFunc = F.getParent()->getOrInsertFunction("abort", Type::getVoidTy(Ctx));
        FailBuilder.CreateCall(AbortFunc);
        FailBuilder.CreateUnreachable();

        // 3. Split the Old Block right before the GEP
        // OldBB (Start) -> check -> (FailBB OR SafeBB)
        BasicBlock *SafeBB = OldBB->splitBasicBlock(GEP, "SafeBlock");
        
        // 4. Fix the Terminator of OldBB (The split created an unconditional branch, we need a conditional one)
        Instruction *OldTerminator = OldBB->getTerminator();
        OldTerminator->eraseFromParent(); // Delete the auto-generated jump

        IRBuilder<> CheckBuilder(OldBB);
        
        // 5. Inject the Comparison: if (Index >= Size)
        Value *SizeVal = ConstantInt::get(Index->getType(), ArraySize);
        Value *Condition = CheckBuilder.CreateICmpUGE(Index, SizeVal);

        // 6. Branch
        CheckBuilder.CreateCondBr(Condition, FailBB, SafeBB);
        
        Modified = true;
    }

    return Modified ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};

} // end anonymous namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "rustifyC", LLVM_VERSION_STRING,
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