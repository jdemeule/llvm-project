//===--- UseRangesCheck.cpp - clang-tidy ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UseRangesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace modernize {

void UseRangesCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("::std::copy")))).bind("match"),
      this);
}

void UseRangesCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchExpr = Result.Nodes.getNodeAs<CallExpr>("match");
  diag(MatchExpr->getExprLoc(),
       "Consider to replace 'std::copy' by 'std::ranges::copy'");
}

} // namespace modernize
} // namespace tidy
} // namespace clang
