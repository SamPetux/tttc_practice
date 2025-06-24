#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {
struct LoopWrapperPass : public PassInfoMixin<LoopWrapperPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    LoopInfo &LI = FAM.getResult<LoopAnalysis>(F);
    Module *M = F.getParent();
    LLVMContext &Ctx = M->getContext();
    
    // Создаем объявления функций
    FunctionType *VoidTy = FunctionType::get(Type::getVoidTy(Ctx), false);
    FunctionCallee LoopBegin = M->getOrInsertFunction("cycle_begin", VoidTy);
    FunctionCallee LoopEnd = M->getOrInsertFunction("cycle_finish", VoidTy);

    bool Changed = false;
    
    for (Loop *L : LI) {
      // Вставка cycle_begin в заголовок цикла
      BasicBlock *Header = L->getHeader();
      IRBuilder<> Builder(Header, Header->getFirstInsertionPt());
      Builder.CreateCall(LoopBegin);
      
      // Вставка cycle_finish во все точки выхода
      SmallVector<BasicBlock*, 4> ExitBlocks;
      L->getExitBlocks(ExitBlocks);
      
      for (BasicBlock *ExitBB : ExitBlocks) {
        IRBuilder<> ExitBuilder(ExitBB, ExitBB->getFirstInsertionPt());
        ExitBuilder.CreateCall(LoopEnd);
      }
      Changed = true;
    }
    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION,
    "LoopWrapper",
    "v1.0",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM, auto...) {
          if (Name == "wrap-loops") {
            FPM.addPass(LoopWrapperPass());
            return true;
          }
          return false;
        }
      );
    }
  };
}

