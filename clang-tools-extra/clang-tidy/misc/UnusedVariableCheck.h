//===--- UnusedVariableCheck.h - clang-tidy ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_UNUSEDVARIABLECHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_UNUSEDVARIABLECHECK_H

#include "../ClangTidyCheck.h"
#include "clang/AST/Decl.h"
#include "clang/Basic/SourceLocation.h"
#include <vector>

namespace clang {
namespace tidy {
namespace misc {

/// Finds unused variable and fixes them, so that `-Wunused-variable` can be
/// turned on.
/// Limitations:
///   - cannot reuse the -Wunused-variable deduction, so the computation of
///     unused could be slow and the deduction not as accurate.
class UnusedVariableCheck : public ClangTidyCheck {
public:
  UnusedVariableCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  void onEndOfTranslationUnit() override;

private:
  struct VarDeclContext {
    explicit VarDeclContext(const VarDecl *FoundVarDecl,
                            CharSourceRange VarDeclRange)
        : FoundVarDecl(FoundVarDecl), VarDeclRange(VarDeclRange) {}

    const VarDecl *FoundVarDecl;
    CharSourceRange VarDeclRange;
    bool IsUsed = false;
  };
  std::vector<VarDeclContext> Contexts;
};

} // namespace misc
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_UNUSEDVARIABLECHECK_H
