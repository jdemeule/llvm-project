//===--- UnusedFunctionCheck.h - clang-tidy ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_UNUSEDFUNCTIONCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_UNUSEDFUNCTIONCHECK_H

#include "../ClangTidyCheck.h"
#include "clang/AST/Decl.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/SmallVector.h"

namespace clang {
namespace tidy {
namespace misc {

/// Finds unused function and fixes them, so that `-Wunused-function` can be
/// turned on.
/// Limitations:
///   - cannot reuse the -Wunused-function deduction, so the computation of
///     unused could be slow and the deduction not as accurate.
class UnusedFunctionCheck : public ClangTidyCheck {
public:
  UnusedFunctionCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  void onEndOfTranslationUnit() override;

private:
  struct FctDeclContext {
    explicit FctDeclContext(const FunctionDecl *FctDecl,
                            CharSourceRange SourceRange)
        : FctDecl(FctDecl), SourceRange(SourceRange) {}

    const FunctionDecl *FctDecl;
    CharSourceRange SourceRange;
  };
  llvm::DenseMap<const FunctionDecl *, llvm::SmallVector<FctDeclContext, 1>>
      FoundDecls;
};

} // namespace misc
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_UNUSEDFUNCTIONCHECK_H
